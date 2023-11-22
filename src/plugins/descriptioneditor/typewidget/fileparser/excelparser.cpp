#include "excelparser.h"
#include "fileparserbase.h"
#include <coreplugin/messagemanager.h>

ExcelParser::ExcelParser()
{

}

ExcelParser::~ExcelParser()
{
    if(m_xlsx)
        delete m_xlsx;
}

bool ExcelParser::setData(QString excelName, QDomDocument &doc, QString mode)
{
    m_doc = doc;
    m_mode = mode;
    RETURN_IF_FALSE(excelRead(excelName));
    return true;
}

bool ExcelParser::excelRead(QString excelName)
{
    m_xlsx = new QXlsx::Document(excelName);
    if(m_mode == "Cover")
    {
        removeAllNodes(m_doc);
    }
    bool datatypeFlag = datatypeParser();
    bool enumFlag = enumParser();
    bool macroFlag = macroParser();
    bool structFlag = structParser();
    bool typedefFlag = typedefParser();
    RETURN_IF_FALSE(datatypeFlag && structFlag && enumFlag && macroFlag && typedefFlag);
    initHash(m_doc, m_typeHash);
    calculateSize(m_doc, m_typeHash);
    return true;
}

bool ExcelParser::datatypeParser()
{
    if(!m_xlsx->selectSheet("DataTypes"))
    {
        QString errLog = "FormatError: Sheet \"DataTypes\" doesn't exist, please check your excel is right!";
        Core::MessageManager::instance()->writeWithTime(errLog, Utils::WarningMessageFormat);
        return false;
    }
    QDomElement datatypesNode = getNode(m_doc, "datatypes");
    QDomElement typeElement;

    QDomNodeList childList = datatypesNode.elementsByTagName("datatype");
    if(m_mode == "Cover")
    {
        for(int i = 0; i < childList.size();)
        {
            datatypesNode.removeChild(childList.at(i));
        }
    }
    QXlsx::CellRange usedRange = m_xlsx->dimension();
    QList<int> usefulCols = {0, 0};
    for(int col = 1; col <= 2; col++)
    {
        QString value = m_xlsx->read(1, col).toString();
        if(value == "name")
        {
            usefulCols[0] = col;
        }
        else if(value == "size")
        {
            usefulCols[1] = col;
        }
    }
    int rows = usedRange.rowCount();
    for (int i = 2; i <= rows; i++)
    {
        QString name = m_xlsx->read(i, usefulCols[0]).toString();
        QString size =  m_xlsx->read(i, usefulCols[1]).toString();
        if(name == "" || size == "")
        {
            QString warningLog = "Warning: The information in sheet \"DataTypes\": line " + QString::number(i) + " is incomplete, and this line has been skipped for you!";
            Core::MessageManager::instance()->writeWithTime(warningLog, Utils::WarningMessageFormat);
            continue;
        }
        QString description = "predefined  " + name + " datatype";
        typeElement = m_doc.createElement("datatype");
        typeElement.setAttribute("name", name);
        typeElement.setAttribute("size", size);
        typeElement.setAttribute("description", description);
        datatypesNode.appendChild(typeElement);
    }
    mergeNode(datatypesNode, "datatype");
    QString generateLog = "Datatypes import success!";
    Core::MessageManager::instance()->writeWithTime(generateLog, Utils::NormalMessageFormat);
    return true;
}

bool ExcelParser::structParser()
{
    if(!m_xlsx->selectSheet("Structs"))
    {
        QString errLog = "FormatError: Sheet \"Structs\" doesn't exist, please check your excel is right!";
        Core::MessageManager::instance()->writeWithTime(errLog, Utils::WarningMessageFormat);
        return false;
    }
    QDomElement structsNode = getNode(m_doc, "structs");
    QDomElement elementstruct;

    QXlsx::CellRange usedRange = m_xlsx->dimension();
    int cols = usedRange.columnCount();
    QList<int> usefulCols = {0, 0, 0, 0, 0, 0, 0};
    for(int col = 1; col <= cols; col++)
    {
        QString value = m_xlsx->read(1, col).toString();
        if(value == "DataType")
        {
            usefulCols[0] = col;
        }
        else if(value == "alignment")
        {
            usefulCols[1] = col;
        }
        else if(value == "FileName")
        {
            usefulCols[2] = col;
        }
        else if(value == "RecordElement")
        {
            usefulCols[3] = col;
        }
        else if(value == "RecordElementType")
        {
            usefulCols[4] = col;
        }
        else if(value == "Arraysize")
        {
            usefulCols[5] = col;
        }
        else if(value == "Description")
        {
            usefulCols[6] = col;
        }
    }
    if(usefulCols[6] == 0)
    {
        usefulCols[6] = 7;
    }
    if(usefulCols.contains(0))
    {
        QString errLog = "FormatError: Sheet \"Structs\" must have these seven columns at least: DataType、alignment、FileName、RecordElement、RecordElementType、Arraysize and Description, please check your excel!";
        Core::MessageManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        return false;
    }
    int rows = usedRange.rowCount();
    for (int row = 2; row <= rows; row++)
    {
        QString alignment;
        QString txt = m_xlsx->read(row, usefulCols[0]).toString();
        if(txt != "")
        {
            alignment = m_xlsx->read(row, usefulCols[1]).toString();
            QString filename = m_xlsx->read(row, usefulCols[2]).toString();
            if(filename == "")
            {
                filename = _DEFAULT_FILENAME;
            }
            else
            {
                filename = filename.split(".").at(0);
            }
            elementstruct = m_doc.createElement("struct");
            elementstruct.setAttribute("alignment", alignment);
            elementstruct.setAttribute("name", txt);
            elementstruct.setAttribute("filename", filename);
            elementstruct.setAttribute("version", "1");
            structsNode.appendChild(elementstruct);
        }
        QString name = cellRead(row, usefulCols[3], "RecordElement");
        QString type = cellRead(row, usefulCols[4], "RecordElementType");
        if(type.contains("Hyperlink", Qt::CaseInsensitive))
        {
            QStringList typeList = type.split("\"");
            typeList.removeAll("");
            type = typeList[3];
        }
        QString arraysize = m_xlsx->read(row, usefulCols[5]).toString();
        if(arraysize == "")
        {
            arraysize = _ARRAYSIZE_DEFAULT_VALUE;
        }
        if(name == "" || type == "")
        {
            QString warningLog = "Warning: The information in sheet \"Structs\": line " + QString::number(row) + " is incomplete, and this line has been skipped for you!";
            Core::MessageManager::instance()->writeWithTime(warningLog, Utils::WarningMessageFormat);
            continue;
        }
        QString description = m_xlsx->read(row, usefulCols[6]).toString();
        QDomElement elementelement = m_doc.createElement("element");
        elementelement.setAttribute("alignment", alignment);
        elementelement.setAttribute("arraysize", arraysize);
        elementelement.setAttribute("byteorder", "LE");
        elementelement.setAttribute("name", name);
        elementelement.setAttribute("type", type);
        elementelement.setAttribute("description", description);
        elementstruct.appendChild(elementelement);
    }
    mergeNode(structsNode, "struct");
    QString generateLog = "Structs import success!";
    Core::MessageManager::instance()->writeWithTime(generateLog, Utils::NormalMessageFormat);
    return true;
}

bool ExcelParser::enumParser()
{
    if(!m_xlsx->selectSheet("Enums"))
    {
        QString errLog = "FormatError: Sheet \"Enums\" doesn't exist, please check your excel is right!";
        Core::MessageManager::instance()->writeWithTime(errLog, Utils::WarningMessageFormat);
        return false;
    }
    QDomElement enumsNode = getNode(m_doc, "enums");
    QDomElement elementenum;

    QXlsx::CellRange usedRange = m_xlsx->dimension();
    int cols = usedRange.columnCount();
    QList<int> usefulCols = {0, 0, 0, 0, 0, 0};
    for(int col = 1; col <= cols; col++)
    {
        QString value = m_xlsx->read(1, col).toString();
        if(value == "EnumName")
        {
            usefulCols[0] = col;
        }
        else if(value == "EnumType")
        {
            usefulCols[1] = col;
        }
        else if(value == "FileName")
        {
            usefulCols[2] = col;
        }
        else if(value == "ElementName")
        {
            usefulCols[3] = col;
        }
        else if(value == "ElementValue")
        {
            usefulCols[4] = col;
        }
        else if(value == "Description")
        {
            usefulCols[5] = col;
        }
    }
    if(usefulCols[5] == 0)
    {
        usefulCols[5] = 6;
    }
    if(usefulCols.contains(0))
    {
        QString errLog = "FormatError: Sheet \"Enums\" must have these six columns at least: EnumName、EnumType、FileName、ElementName、ElementValue and Description, please check your excel!";
        Core::MessageManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        return false;
    }
    int rows = usedRange.rowCount();
    for (int row = 2; row <= rows; row++)
    {
        QString enumName = m_xlsx->read(row, usefulCols[0]).toString();
        if(enumName != "")
        {
            //QString enumType = m_xlsx->read(row, usefulCols[1]).toString();
            //if(enumType == "")
            //{
            //    enumType = _ENUM_DEFAULT_TYPE;
            //}
            QString filename = m_xlsx->read(row, usefulCols[2]).toString();
            if(filename == "")
            {
                filename = _DEFAULT_FILENAME;
            }
            else
            {
                filename = filename.split(".").at(0);
            }
            elementenum = m_doc.createElement("enum");
            //elementenum.setAttribute("type", enumType);
            elementenum.setAttribute("name", enumName);
            elementenum.setAttribute("filename", filename);
            enumsNode.appendChild(elementenum);
        }
        QString name = cellRead(row, usefulCols[3], "ElementName");
        QString value = m_xlsx->read(row, usefulCols[4]).toString();
        QString description = m_xlsx->read(row, usefulCols[5]).toString();
        if(name == "")
        {
            QString warningLog = "Warning: The information in sheet \"Enums\": line " + QString::number(row) + " is incomplete, and this line has been skipped for you!";
            Core::MessageManager::instance()->writeWithTime(warningLog, Utils::WarningMessageFormat);
            continue;
        }
        QDomElement elementelement = m_doc.createElement("element");
        elementelement.setAttribute("name", name);
        elementelement.setAttribute("value", value);
        elementelement.setAttribute("description", description);
        elementenum.appendChild(elementelement);
    }
    mergeNode(enumsNode, "enum");
    QString generateLog = "Enums import success!";
    Core::MessageManager::instance()->writeWithTime(generateLog, Utils::NormalMessageFormat);
    return true;
}

bool ExcelParser::macroParser()
{
    if(!m_xlsx->selectSheet("Macros"))
    {
        QString errLog = "FormatWarnnig: Sheet \"Macros\" doesn't exist, please check your excel is right!";
        Core::MessageManager::instance()->writeWithTime(errLog, Utils::WarningMessageFormat);
        return true;
    }
    QDomElement macrosNode = getNode(m_doc, "macros");
    QDomElement elementmacro;

    QXlsx::CellRange usedRange = m_xlsx->dimension();
    QList<int> usefulCols = {0, 0, 0};
    for(int col = 1; col <= usedRange.columnCount(); col++)
    {
        QString value = m_xlsx->read(1, col).toString();
        if(value == "MacroName")
        {
            usefulCols[0] = col;
        }
        else if(value == "MacroValue")
        {
            usefulCols[1] = col;
        }
        else if(value == "Description")
        {
            usefulCols[2] = col;
        }
    }
    if(usefulCols.contains(0))
    {
        QString errLog = "FormatError: Sheet \"Macros\" must have these three columns at least: MacroName、MacroValue、Description, please check your excel!";
        Core::MessageManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        return false;
    }
    for (int row = 2; row <= usedRange.rowCount(); row++)
    {
        QString macroName = m_xlsx->read(row, usefulCols[0]).toString();
        QString macroValue = m_xlsx->read(row, usefulCols[1]).toString();
        if(macroName == "" || macroValue == "")
        {
            QString warningLog = "Warning: The information in sheet \"Macros\": line " + QString::number(row) + " is incomplete, and this line has been skipped for you!";
            Core::MessageManager::instance()->writeWithTime(warningLog, Utils::WarningMessageFormat);
            continue;
        }
        QString description = m_xlsx->read(row, usefulCols[2]).toString();
        elementmacro = m_doc.createElement("macro");
        elementmacro.setAttribute("value", macroValue);
        elementmacro.setAttribute("name", macroName);
        elementmacro.setAttribute("description", description);
        macrosNode.appendChild(elementmacro);
    }
    mergeNode(macrosNode, "macro");
    QString generateLog = "Macros import success!";
    Core::MessageManager::instance()->writeWithTime(generateLog, Utils::NormalMessageFormat);
    return true;
}

bool ExcelParser::typedefParser()
{
    if(!m_xlsx->selectSheet("Typedefs"))
    {
        QString errLog = "FormatError: Sheet \"Typedefs\" doesn't exist, please check your excel is right!";
        Core::MessageManager::instance()->writeWithTime(errLog, Utils::WarningMessageFormat);
        return false;
    }
    QDomElement typedefsNode = getNode(m_doc, "typedefs");
    QDomElement typedefElement;

    QXlsx::CellRange usedRange = m_xlsx->dimension();
    QList<int> usefulCols = {0, 0};
    for(int col = 1; col <= 2; col++)
    {
        QString value = m_xlsx->read(1, col).toString();
        if(value == "TypeName")
        {
            usefulCols[0] = col;
        }
        else if(value == "OriginalType")
        {
            usefulCols[1] = col;
        }
    }
    int rows = usedRange.rowCount();
    for (int i = 2; i <= rows; i++)
    {
        QString name = m_xlsx->read(i, usefulCols[0]).toString();
        QString value =  m_xlsx->read(i, usefulCols[1]).toString();
        if(name == "" || value == "")
        {
            QString warningLog = "Warning: The information in sheet \"Typedefs\": line " + QString::number(i) + " is incomplete, and this line has been skipped for you!";
            Core::MessageManager::instance()->writeWithTime(warningLog, Utils::WarningMessageFormat);
            continue;
        }
        typedefElement = m_doc.createElement("typedef");
        typedefElement.setAttribute("name", name);
        typedefElement.setAttribute("value", value);
        typedefsNode.appendChild(typedefElement);
    }
    mergeNode(typedefsNode, "typedef");
    QString generateLog = "Typedefs import success!";
    Core::MessageManager::instance()->writeWithTime(generateLog, Utils::NormalMessageFormat);
    return true;
}

QString ExcelParser::cellRead(int row, int col, QString name)
{
    QString res = m_xlsx->read(row, col).toString();
    if(res == "")
    {
        QString errLog = "Warning: The " + name + " is empty! In row " + QString::number(row) + ", col " + QString::number(col);
        Core::MessageManager::instance()->writeWithTime(errLog, Utils::WarningMessageFormat);
    }
    return res;
}
