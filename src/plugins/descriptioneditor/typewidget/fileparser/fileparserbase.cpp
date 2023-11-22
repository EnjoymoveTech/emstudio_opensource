#include "fileparserbase.h"
#include <coreplugin/messagemanager.h>

FileParserBase::FileParserBase()
{

}

FileParserBase::~FileParserBase()
{

}

bool FileParserBase::mergeNode(QDomElement &Node, QString tagName)
{
    QDomNodeList childList = Node.elementsByTagName(tagName);
    for(int i = 0; i < childList.size(); i++)
    {
        QString name = childList.at(i).toElement().attribute("name");
        for(int j = childList.size() - 1; j > i; j--)
        {
            if(childList.at(j).toElement().attribute("name") == name)
            {
//                if(m_mode == "Cover")
//                {
//                    return false;
//                }
                Node.replaceChild(childList.at(j), childList.at(i));
                for(int k = i + 1; k < childList.size(); k++)
                {
                    if(childList.at(k).toElement().attribute("name") == name)
                    {
                        //return false;
                        Node.removeChild(childList.at(k));
                        k--;
                    }
                }
                break;
            }
        }
    }
    return true;
}

void FileParserBase::removeAllNodes(QDomDocument &doc)
{
    QDomElement Node = doc.firstChildElement(_DESCXML_ROOTNAME).toElement().firstChildElement("structs");
    QDomNodeList childList = Node.elementsByTagName("struct");
    for(int i = 0; i < childList.size();)
    {
        Node.removeChild(childList.at(i));
    }

    Node = doc.firstChildElement(_DESCXML_ROOTNAME).toElement().firstChildElement("enums");
    childList = Node.elementsByTagName("enum");
    for(int i = 0; i < childList.size();)
    {
        Node.removeChild(childList.at(i));
    }

    Node = doc.firstChildElement(_DESCXML_ROOTNAME).toElement().firstChildElement("macros");
    childList = Node.elementsByTagName("macro");
    for(int i = 0; i < childList.size();)
    {
        Node.removeChild(childList.at(i));
    }

    Node = doc.firstChildElement(_DESCXML_ROOTNAME).toElement().firstChildElement("typedefs");
    childList = Node.elementsByTagName("typedef");
    for(int i = 0; i < childList.size();)
    {
        Node.removeChild(childList.at(i));
    }
}

void FileParserBase::initHash(QDomDocument& doc, QHash<QString, int> &typeHash)
{
    QDomNodeList typesList = doc.elementsByTagName("datatype");
    for(int i = 0; i < typesList.size(); i++)
    {
        QString name = typesList.at(i).toElement().attribute("name");
        int size = typesList.at(i).toElement().attribute("size").toInt();
        typeHash.insert(name, size / 8);
        typeHash.insert(name + "*", _POINTER_SIZE);
        typeHash.insert("const " + name, size / 8);
    }

    typesList = doc.elementsByTagName("struct");
    for(int i = 0; i < typesList.size(); i++)
    {
        QString name = typesList.at(i).toElement().attribute("name");
        int size = typesList.at(i).toElement().attribute("size").toInt();
        typeHash.insert(name, size);
        typeHash.insert(name + "*", _POINTER_SIZE);
        typeHash.insert("const " + name, size);
    }

    typesList = doc.elementsByTagName("enum");
    for(int i = 0; i < typesList.size(); i++)
    {
        QString name = typesList.at(i).toElement().attribute("name");
        typeHash.insert(name, _ENUM_DEFAULT_SIZE);
        typeHash.insert(name + "*", _POINTER_SIZE);
        typeHash.insert("const " + name, _ENUM_DEFAULT_SIZE);
    }
}

QDomElement FileParserBase::getNode(QDomDocument& doc, QString nodeTag)
{
    QDomElement node = doc.firstChildElement(_DESCXML_ROOTNAME).toElement().firstChildElement(nodeTag);
    if(node.isNull())
    {
        node = doc.createElement(nodeTag);
        doc.firstChildElement(_DESCXML_ROOTNAME).appendChild(node);
    }
    return node;
}

void FileParserBase::calculateSize(QDomDocument &doc, QHash<QString, int> &typeHash)
{
    QDomElement typedefsNode = doc.firstChildElement(_DESCXML_ROOTNAME).toElement().firstChildElement("typedefs");
    QDomElement structsNode = doc.firstChildElement(_DESCXML_ROOTNAME).toElement().firstChildElement("structs");
    typeCalculator(typedefsNode, typeHash);
    structCalculator(structsNode, typeHash);
}

void FileParserBase::typeCalculator(QDomElement& typesNode, QHash<QString, int> &typeHash)
{
    QDomElement typeElement;
    QDomNodeList typeList = typesNode.elementsByTagName("typedef");
    for(int i = 0; i < typeList.size(); i++)
    {
        typeElement = typeList.at(i).toElement();
        int size = 0;
        QStringList values = typeElement.attribute("value").split(" ");
        if(values.contains("long"))
        {
            size = 4 * values.removeAll("long");
        }
        else if(values.size() == 1 && values[0].contains("signed"))
        {
            size = typeHash.value("signed");
        }
        else
        {
            size = typeHash.value(values.last());
        }

        if(size)
        {
            typeHash.insert(typeElement.attribute("name"), size);
            typeHash.insert(typeElement.attribute("name") + "*", _POINTER_SIZE);
            typeHash.insert("const " + typeElement.attribute("name"), size);
        }
    }
}

void FileParserBase::structCalculator(QDomElement& structsNode, QHash<QString, int> &typeHash)
{
    QDomElement structElement;
    QDomNodeList structList = structsNode.elementsByTagName("struct");
    for(int i = 0; i < structList.size(); i++)
    {
        structElement = structList.at(i).toElement();
        int bytepos = 0;
        int remaining = 0;
        int typeSize = 4;
        int align = structElement.attribute("alignment").toInt();
        if(!align)
            continue;

        QDomNodeList memberList = structElement.elementsByTagName("element");
        for(int j = 0; j < memberList.size(); j++)
        {
            QDomElement memberElement = memberList.at(j).toElement();
            QString type = memberElement.attribute("type");
            type.remove("unsigned ");
            type.remove("signed ");
            QStringList arraysizeList = memberElement.attribute("arraysize").split(",");
            int arraysize = 1;
            for(auto arr : arraysizeList)
            {
                arraysize *= (!arr.toInt())?1:arr.toInt();
            }

            if(typeHash.contains(type))
            {
                typeSize = typeHash.value(type);
            }
            else
            {
                structElement.removeChild(memberElement);
                j--;
                QString errLog = QString("TypeError: %1 is not defined! In file: %2.h, struct: %3").arg(type, structElement.attribute("filename"), structElement.attribute("name"));
                Core::MessageManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
                continue;
            }

            memberElement.setAttribute("bytepos", bytepos);

            int bitNum = arraysize * typeSize;
            bytepos += bitNum;
            if(typeSize % align)
            {
                remaining -= bitNum % align;
                remaining = (remaining + align) % align;
            }
            else
            {
                bytepos += remaining;
                remaining = 0;
            }
        }
        structElement.setAttribute("size", bytepos + remaining);
        typeHash.insert(structElement.attribute("name"), structElement.attribute("size").toInt());
        typeHash.insert(structElement.attribute("name") + "*", _POINTER_SIZE);
    }
}
