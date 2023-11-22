#include "headerparser.h"
#include "fileparserbase.h"
#include <coreplugin/messagemanager.h>

HeaderParser::HeaderParser()
{

}

HeaderParser::~HeaderParser()
{

}

bool HeaderParser::syntaxCheck(QString headerText)
{
    RETURN_IF_FALSE(braceCheck(headerText));
    return true;
}

bool HeaderParser::braceCheck(QString &text)
{
    QList<int> leftIdxList = allIndexOf(text, "{");
    QList<int> rightIdxList = allIndexOf(text, "}");
    if(leftIdxList.size() != rightIdxList.size())
    {
        return false;
    }
    for(int i = 0; i < leftIdxList.size(); i++)
    {
        if(leftIdxList[i] > rightIdxList[i])
        {
            return false;
        }
        if(i < leftIdxList.size() - 1 && leftIdxList[i + 1] < rightIdxList[i])
        {
            return false;
        }
    }
    return true;
}

QList<int> HeaderParser::allIndexOf(QString &text, QString bid)
{
    QList<int> res;
    int idx = text.indexOf(bid);
    while(idx != -1)
    {
        res.append(idx);
        idx = text.indexOf(bid, idx + 1);
    }
    return res;
}

bool HeaderParser::getInfoFromHeader(QDomDocument &doc, QString mode)
{
    m_mode = mode;
    if(m_mode == "Cover")
    {
        removeAllNodes(doc);
    }
    initHash(doc, m_typeHash);
    for(int i = 0; i < fileNameList.size(); i++)
    {
        QString fileName = fileNameList[i];
        int lidx = fileName.lastIndexOf("/");
        int ridx = fileName.indexOf(".h");
        if(ridx == -1)
        {
            continue;  // skip the file which is not .h
        }
        fileName = fileName.mid(lidx + 1, ridx - lidx - 1);
        QFile header(fileNameList[i]);
        if(!header.open(QIODevice::ReadOnly))
        {
            QString errLog = "LoadError: Load file " + fileName + ".h failed!";
            Core::MessageManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
            continue;
        }
        QString text = QString::fromStdString(header.readAll().toStdString());
        text.replace("\t", " ");
        QStringList fileTextList = text.split(QRegExp("\n|\r\n"));
        loadInfoFromHeader(doc, fileTextList, fileName);
    }
    calculateSize(doc, m_typeHash);
    return true;
}

void HeaderParser::loadInfoFromHeader(QDomDocument &doc, QStringList &fileTextList, QString fileName)
{
    QDomElement typedefElement, structElement, enumElement, macroElement;
    QDomElement typedefsNode = getNode(doc, "typedefs");
    QDomElement structsNode = getNode(doc, "structs");
    QDomElement enumsNode = getNode(doc, "enums");
    QDomElement macrosNode = getNode(doc, "macros");

    bool inStruct = false;
    bool inEnum = false;
    bool nameFlag = false;
    int alignment = _DEFAULT_ALIGNMENT;

    for(int i = 0; i < fileTextList.size(); i++)
    {
        QString rowText = fileTextList.at(i);
        rowText.remove(QRegExp("^ +\\s*"));  // remove in front space
        rowText.remove(QRegExp("\\s* +$"));  // remove behind space
        if(rowText.startsWith("// file"))
        {
            QStringList Info = rowText.split(" ");
            fileName = Info.last().split(".").at(0);
        }
        else if(rowText.startsWith("//"))
        {
            continue;
        }
        else if(rowText.startsWith("typedef") && rowText.contains(";"))  // typedefs
        {
            rowText = rowText.left(rowText.indexOf(";"));
            QStringList typeList = rowText.split(" ");
            typeList.removeAll("");
            typedefElement = doc.createElement("typedef");
            QString name = typeList.last();
            rowText.remove("typedef");
            rowText.remove(name);
            QString value = rowText.trimmed();

            typedefElement.setAttribute("name", name);
            typedefElement.setAttribute("value", value);
            typedefsNode.appendChild(typedefElement);
        }
        else if(rowText.startsWith("#define"))  // macros
        {
            QString description = "";
            if(rowText.contains("//"))
            {
                 description = rowText.split("//").at(1).simplified();
                 if(description.startsWith("/<"))
                 {
                     description = description.mid(2).trimmed();
                 }
                 rowText = rowText.left(rowText.indexOf("//"));
            }
            else if(rowText.contains("/*"))
            {
                int leftIdx = rowText.indexOf("/*");
                int rightIdx = rowText.indexOf("*/");
                QString descText = rowText.mid(leftIdx + 2, rightIdx - leftIdx - 2);
                if(descText[0] == "*")
                {
                    descText[0] = QChar(' ');
                }
                description = descText.trimmed();
                rowText = rowText.left(rowText.indexOf("/*"));
            }
            rowText.remove("#define");
            rowText.remove(QRegExp("^ +\\s*"));  // remove in front space
            rowText.remove(QRegExp("\\s* +$"));  // remove behind space
            int spaceIdx = rowText.indexOf(" ");
            if(spaceIdx != -1)
            {
                macroElement = doc.createElement("macro");
                QString name = rowText.left(spaceIdx);
                QString value = rowText.right(rowText.size() - spaceIdx - 1);
                value.remove(QRegExp("^ +\\s*"));  // remove in front space
                value.remove(QRegExp("\\s* +$"));  // remove behind space
                macroElement.setAttribute("name", name);
                macroElement.setAttribute("value", value);
                macroElement.setAttribute("description", description);
                macrosNode.appendChild(macroElement);
            }
        }
        else if(rowText.startsWith("typedef enum"))  // enums
        {
            enumElement = doc.createElement("enum");
            enumElement.setAttribute("filename", fileName);
            nameFlag = true;
            inEnum = true;
        }
        else if(rowText.startsWith("enum class"))
        {
            QString name = "class " + rowText.trimmed().split(QRegExp(" |\\{")).at(2);
            inEnum = true;
            enumElement = doc.createElement("enum");
            enumElement.setAttribute("name", name);
            enumElement.setAttribute("filename", fileName);
            enumsNode.appendChild(enumElement);
            m_typeHash.insert(name, _ENUM_DEFAULT_SIZE);
            m_typeHash.insert(name + "*", _POINTER_SIZE);
        }
        else if(rowText.startsWith("enum"))
        {
            QString name = rowText.split(QRegExp(" |\\{")).at(1);
            inEnum = true;
            enumElement = doc.createElement("enum");
            enumElement.setAttribute("name", name);
            enumElement.setAttribute("filename", fileName);
            enumsNode.appendChild(enumElement);
            m_typeHash.insert(name, _ENUM_DEFAULT_SIZE);
            m_typeHash.insert(name + "*", _POINTER_SIZE);
        }
        else if(rowText.startsWith("#pragma"))
        {
            QStringList alignmentList = rowText.split(QRegExp("\\(|\\)"));
            if(alignmentList.size() < 2)
            {
                continue;
            }
            QString alignNum = alignmentList.at(1);
            if(alignNum == "" || alignNum == "pop")
            {
                alignment = _DEFAULT_ALIGNMENT;
            }
            else if(alignNum.contains("push"))
            {
                alignment = alignNum.remove(QRegExp("[push|,| ]")).toInt();
            }
            else
            {
                alignment = alignNum.toInt();
            }
        }
        else if(rowText.startsWith("typedef struct"))  // structs
        {
            structElement = doc.createElement("struct");
            structElement.setAttribute("filename", fileName);
            nameFlag = true;
            inStruct = true;
        }
        else if(rowText.startsWith("struct"))
        {
            QString name = rowText.split(QRegExp(" |\\{")).at(1);
            inStruct = true;
            structElement = doc.createElement("struct");
            structElement.setAttribute("name", name);
            structElement.setAttribute("filename", fileName);
            structsNode.appendChild(structElement);
        }
        else if(rowText.contains("}"))
        {
            if(inEnum)
            {
                QString lastText;
                while(i > 1)
                {
                    lastText = fileTextList.at(i - 1);
                    if(lastText != "")
                    {
                        break;
                    }
                    i--;
                }
                if(!lastText.contains(","))
                {
                    QString description = "";
                    if(lastText.contains("//"))
                    {
                         description = lastText.split("//").at(1).simplified();
                         if(description.startsWith("/<"))
                         {
                             description = description.mid(2).trimmed();
                         }
                    }
                    else if(lastText.contains("/*"))
                    {
                        int leftIdx = lastText.indexOf("/*");
                        int rightIdx = lastText.indexOf("*/");
                        QString descText = lastText.mid(leftIdx + 2, rightIdx - leftIdx - 2);
                        if(descText[0] == "*")
                        {
                            descText[0] = QChar(' ');
                        }
                        description = descText.trimmed();
                    }
                    QStringList Info = lastText.split(QRegExp(" |="));
                    Info.removeAll("");
                    QString name = Info.at(0);
                    if(name[0] == '/')
                    {
                        continue;
                    }
                    QString valueText = "";
                    int equalIdx = lastText.indexOf("=");
                    if(equalIdx != -1)
                    {
                        valueText = lastText.mid(equalIdx + 1).trimmed();
                    }
                    QDomElement element = doc.createElement("element");
                    element.setAttribute("name", name);
                    element.setAttribute("value", valueText);
                    element.setAttribute("description", description);
                    enumElement.appendChild(element);
                }
                if(nameFlag)
                {
                    QString name = rowText.split(QRegExp("\\}|;")).at(1);
                    name.remove(" ");
                    QStringList nameList = name.split(",");
                    for(int j = 0; j < nameList.size(); j++)
                    {
                        name = nameList.at(j);
                        QDomElement renameNode = enumElement.cloneNode().toElement();
                        renameNode.setAttribute("name", name);
                        enumsNode.appendChild(renameNode);
                        m_typeHash.insert(name, _ENUM_DEFAULT_SIZE);
                        m_typeHash.insert(name + "*", _POINTER_SIZE);
                    }
                    nameFlag = false;
                }
                enumElement.setAttribute("filename", fileName);
                enumElement.setAttribute("type", _ENUM_DEFAULT_TYPE);
                inEnum = false;
            }
            else if(inStruct)
            {
                if(nameFlag)
                {
                    QString name = rowText.split(QRegExp("\\}|;")).at(1);
                    name.remove(" ");
                    QStringList nameList = name.split(",");

                    structElement.setAttribute("alignment", alignment);
                    structElement.setAttribute("version", "1");
                    for(int j = 0; j < nameList.size(); j++)
                    {
                        name = nameList.at(j);
                        QDomElement renameNode = structElement.cloneNode().toElement();
                        renameNode.setAttribute("name", name);
                        structsNode.appendChild(renameNode);
                    }
                }
                else
                {
                    structElement.setAttribute("alignment", alignment);
                    structElement.setAttribute("version", "1");
                }
                inStruct = false;
                nameFlag = false;
            }
        }
        else if(rowText.contains(";"))
        {
            if(inStruct)
            {
                QString description = "";
                if(rowText.contains("//"))
                {
                     description = rowText.split("//").at(1).simplified();
                     if(description.startsWith("/<"))
                     {
                         description = description.mid(2).trimmed();
                     }
                }
                else if(rowText.contains("/*"))
                {
                    int leftIdx = rowText.indexOf("/*");
                    int rightIdx = rowText.indexOf("*/");
                    QString descText = rowText.mid(leftIdx + 2, rightIdx - leftIdx - 2);
                    if(descText[0] == "*")
                    {
                        descText[0] = QChar(' ');
                    }
                    description = descText.trimmed();
                }
                rowText = rowText.left(rowText.indexOf(";"));
                QStringList Info = rowText.split(QRegExp(" "));
                Info.removeAll("");
                QString type;
                QString name;
                QString arraysizeText = "";
                while(Info.size() > 0 && (Info.at(0) == "const" || Info.at(0) == "long" || Info.at(0).endsWith("signed")))
                {
                    type += Info.at(0) + " ";
                    Info.removeAt(0);
                }
                if(Info.at(1).startsWith("*"))
                {
                    type = Info.at(0) + "*";
                    Info[1].remove(0, 1);
                    Info.removeAll("");
                    name = Info.at(1);
                }
                else
                {
                    type += Info.at(0);
                    name = Info.at(1);
                }
                QStringList nameList = name.split(QRegExp("\\[|\\]"));
                nameList.removeAll("");
                if(nameList.size() == 1)
                {
                    arraysizeText = _ARRAYSIZE_DEFAULT_VALUE;
                }
                else
                {
                    for(int nameIdx = 1; nameIdx < nameList.size(); nameIdx++)
                    {
                        arraysizeText += nameList.at(nameIdx) + ",";
                    }
                    arraysizeText.chop(1);
                    name = nameList.at(0);
                }

                QDomElement element = doc.createElement("element");
                element.setAttribute("alignment", alignment);
                element.setAttribute("arraysize", arraysizeText);
                element.setAttribute("byteorder", "LE");
                element.setAttribute("name", name);
                element.setAttribute("type", type);
                element.setAttribute("description", description);
                structElement.appendChild(element);
            }
        }
        else if(rowText.contains(","))
        {
            if(inEnum)
            {
                QString description = "";
                if(rowText.contains("//"))
                {
                     description = rowText.split("//").at(1).simplified();
                     if(description.startsWith("/<"))
                     {
                         description = description.mid(2).trimmed();
                     }
                }
                else if(rowText.contains("/*"))
                {
                    int leftIdx = rowText.indexOf("/*");
                    int rightIdx = rowText.indexOf("*/");
                    QString descText = rowText.mid(leftIdx + 2, rightIdx - leftIdx - 2);
                    if(descText[0] == "*")
                    {
                        descText[0] = QChar(' ');
                    }
                    description = descText.trimmed();
                }
                int splitIdx = rowText.indexOf(",");
                rowText = rowText.left(splitIdx);
                QStringList Info = rowText.split(QRegExp(" |="));
                Info.removeAll("");
                QString name = Info.at(0);
                if(name[0] == '/')
                {
                    continue;
                }
                QString valueText = "";
                int equalIdx = rowText.indexOf("=");
                if(equalIdx != -1)
                {
                    valueText = rowText.mid(equalIdx + 1).trimmed();
                }
                QDomElement element = doc.createElement("element");
                element.setAttribute("name", name);
                element.setAttribute("value", valueText);
                element.setAttribute("description", description);
                enumElement.appendChild(element);
            }
        }
    }

    mergeNode(typedefsNode, "typedef");
    mergeNode(structsNode, "struct");
    mergeNode(enumsNode, "enum");
    mergeNode(macrosNode, "macro");
}
