#include "excelgenerator.h"
#include <coreplugin/messagemanager.h>
#include <emostools/type_description_parser.h>

ExcelGenerator::ExcelGenerator()
{

}

ExcelGenerator::~ExcelGenerator()
{
    if(m_xlsx)
        delete m_xlsx;
}

void ExcelGenerator::setData(QString excelName, QDomDocument* doc)
{
    m_doc = doc;
    m_xlsx = new QXlsx::Document(excelName);
    if(QFile::exists(excelName))
    {
        if(!QFile::remove(excelName))
        {
            //qDebug() << excelName + " is open, please close it first!";
        }
    }
    EmosTools::TypeParser* m_typeparser = EmosTools::TypeParser::getInstance();
    m_typeparser->reloadXml();
    m_typedefList = m_typeparser->getTypedefs();
    m_typeList = m_typeparser->getDatatypes();
    m_enumList = m_typeparser->getEnums();
    m_structList = m_typeparser->getStructs();
    m_macroList = m_typeparser->getMacros();
    writeExcel(excelName);
    QString exportLog = "Export succeeded, export file in this directory: " + excelName;
    Core::MessageManager::instance()->writeWithTime(exportLog, Utils::NormalMessageFormat);
}

void ExcelGenerator::writeExcel(QString excelName)
{
    datatypeWriter();
    enumWriter();
    structWriter();
    macroWriter();
    typedefWriter();
    m_xlsx->saveAs(excelName);
}

void ExcelGenerator::datatypeWriter()
{
    if(!m_xlsx->selectSheet("DataTypes"))
    {
        m_xlsx->addSheet("DataTypes");
    }
    QXlsx::Format format;
    format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    format.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    format.setFontBold(true);
    format.setFontSize(14);
    m_xlsx->setColumnWidth(1, 30);
    m_xlsx->setColumnWidth(2, 30);
    m_xlsx->write("A1", "name", format);
    m_xlsx->write("B1", "size", format);
    format.setFontBold(false);
    format.setFontSize(12);
    int row = 2;
    for(int i = 0; i < m_typeList.size(); i++)
    {
        if(m_typeList.at(i).name.contains("*") || m_typeList.at(i).name.contains("const"))
        {
            continue;
        }
        m_xlsx->write(row, 1, m_typeList.at(i).name, format);
        m_xlsx->write(row, 2, m_typeList.at(i).size, format);
        ++row;
    }
}

void ExcelGenerator::structWriter()
{
    if(!m_xlsx->selectSheet("Structs"))
    {
        m_xlsx->addSheet("Structs");
    }
    QXlsx::Format format;
    format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    format.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    format.setFontBold(true);
    format.setFontSize(14);
    m_xlsx->setColumnWidth(1, 35);
    m_xlsx->setColumnWidth(2, 35);
    m_xlsx->setColumnWidth(3, 35);
    m_xlsx->setColumnWidth(4, 25);
    m_xlsx->setColumnWidth(5, 25);
    m_xlsx->setColumnWidth(6, 40);
    m_xlsx->setColumnWidth(7, 20);
    //m_xlsx->setColumnWidth(8, 20);

    m_xlsx->write("A1", "DataType", format);
    m_xlsx->write("B1", "alignment", format);
    m_xlsx->write("C1", "FileName", format);
    m_xlsx->write("D1", "RecordElement", format);
    m_xlsx->write("E1", "RecordElementType", format);
    m_xlsx->write("F1", "Description", format);
    m_xlsx->write("G1", "Arraysize", format);
    //m_xlsx->write("H1", "Link", format);
    format.setFontBold(false);
    format.setFontSize(12);

    QStringList typeList;
    QHash<QString, int> structList;
    for(int i = 0; i < m_typeList.size(); i++)
    {
        typeList.append(m_typeList.at(i).name);
    }

    int row = 2;
    for(int i = 0; i < m_structList.size(); i++)
    {
        EmosTools::tStructData structItem = m_structList.at(i);
        int start = row;
        structList.insert(structItem.name, start);
        m_xlsx->write(row, 1, structItem.name, format);
        m_xlsx->write(row, 2, structItem.alignment, format);
        m_xlsx->write(row, 3, QString(structItem.filename + ".h"), format);
        for(int j = 0; j < structItem.structInfo.size(); j++)
        {
            EmosTools::tStructInfo element = structItem.structInfo.at(j);
            m_xlsx->write(row, 4, element.name, format);
            //m_xlsx->write(row, 5, element.type, format);
            QString linktext = element.type;
            if(typeList.contains(element.type))
            {
                 linktext = "= Hyperlink(\"#DataTypes!A" +(QString::number((typeList.indexOf(element.type) + 2) / 3 + 2)) + "\", \"" + element.type + "\")";
            }
            else if(structList.keys().contains(element.type))
            {
                linktext = "= Hyperlink(\"#A" +(QString::number(structList[element.type])) + "\", \"" + element.type + "\")";
            }
            else if(enumList.keys().contains(element.type))
            {
                linktext = "= Hyperlink(\"#Enums!A" +(QString::number(enumList[element.type])) + "\", \"" + element.type + "\")";
            }
            m_xlsx->write(row, 5, linktext, format);
            m_xlsx->write(row, 6, element.description, format);
            m_xlsx->write(row, 7, element.arraysize, format);
            ++row;
        }
        m_xlsx->mergeCells(QXlsx::CellRange(start, 1, row - 1, 1));
        m_xlsx->mergeCells(QXlsx::CellRange(start, 2, row - 1, 2));
        m_xlsx->mergeCells(QXlsx::CellRange(start, 3, row - 1, 3));
    }
}

void ExcelGenerator::enumWriter()
{
    if(!m_xlsx->selectSheet("Enums"))
    {
        m_xlsx->addSheet("Enums");
    }
    QXlsx::Format format;
    format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    format.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    format.setFontBold(true);
    format.setFontSize(14);
    m_xlsx->setColumnWidth(1, 35);
    m_xlsx->setColumnWidth(2, 30);
    m_xlsx->setColumnWidth(3, 30);
    m_xlsx->setColumnWidth(4, 40);
    m_xlsx->setColumnWidth(5, 35);
    m_xlsx->setColumnWidth(6, 40);
    m_xlsx->write("A1", "EnumName", format);
    m_xlsx->write("B1", "EnumType", format);
    m_xlsx->write("C1", "FileName", format);
    m_xlsx->write("D1", "ElementName", format);
    m_xlsx->write("E1", "ElementValue", format);
    m_xlsx->write("F1", "Description", format);
    format.setFontBold(false);
    format.setFontSize(12);
    int row = 2;
    for(int i = 0; i < m_enumList.size(); i++)
    {
        int start = row;
        enumList.insert(m_enumList.at(i).name, row);
        m_xlsx->write(row, 1, m_enumList.at(i).name, format);
        m_xlsx->write(row, 2, m_enumList.at(i).type, format);
        m_xlsx->write(row, 3, QString(m_enumList.at(i).filename + ".h"), format);
        QList<EmosTools::tEnumInfo> elementList = m_enumList.at(i).enuminfo;
        for(int j = 0; j < elementList.size(); j++)
        {
            m_xlsx->write(row, 4, elementList.at(j).name, format);
            m_xlsx->write(row, 5, elementList.at(j).value, format);
            m_xlsx->write(row, 6, elementList.at(j).description, format);
            ++row;
        }
        m_xlsx->mergeCells(QXlsx::CellRange(start, 1, row - 1, 1));
        m_xlsx->mergeCells(QXlsx::CellRange(start, 2, row - 1, 2));
        m_xlsx->mergeCells(QXlsx::CellRange(start, 3, row - 1, 3));
    }
}

void ExcelGenerator::macroWriter()
{
    if(!m_xlsx->selectSheet("Macros"))
    {
        m_xlsx->addSheet("Macros");
    }
    QXlsx::Format format;
    format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    format.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    format.setFontBold(true);
    format.setFontSize(14);
    m_xlsx->setColumnWidth(1, 35);
    m_xlsx->setColumnWidth(2, 30);
    m_xlsx->setColumnWidth(3, 40);
    m_xlsx->write("A1", "MacroName", format);
    m_xlsx->write("B1", "MacroValue", format);
    m_xlsx->write("C1", "Description", format);
    format.setFontBold(false);
    format.setFontSize(12);
    int row = 2;
    for(int i = 0; i < m_macroList.size(); i++)
    {
        m_xlsx->write(row, 1, m_macroList.at(i).name, format);
        m_xlsx->write(row, 2, m_macroList.at(i).value, format);
        m_xlsx->write(row, 3, m_macroList.at(i).description, format);
        ++row;
    }
}

void ExcelGenerator::typedefWriter()
{
    if(!m_xlsx->selectSheet("Typedefs"))
    {
        m_xlsx->addSheet("Typedefs");
    }
    QXlsx::Format format;
    format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    format.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    format.setFontBold(true);
    format.setFontSize(14);
    m_xlsx->setColumnWidth(1, 35);
    m_xlsx->setColumnWidth(2, 35);
    m_xlsx->write("A1", "TypeName", format);
    m_xlsx->write("B1", "OriginalType", format);
    format.setFontBold(false);
    format.setFontSize(12);
    int row = 2;
    for(int i = 0; i < m_typedefList.size(); i++)
    {
        m_xlsx->write(row, 1, m_typedefList.at(i).name, format);
        m_xlsx->write(row, 2, m_typedefList.at(i).value, format);
        ++row;
    }
}
