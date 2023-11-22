#ifndef EXCELGENERATOR_H
#define EXCELGENERATOR_H

#include <QDomDocument>
#include <xlsx/xlsxdocument.h>
#include <emostools/type_description_parser.h>

class ExcelGenerator
{
public:
    ExcelGenerator();
    ~ExcelGenerator();
    void setData(QString excelName, QDomDocument* doc);

private:
    void writeExcel(QString excelName);
    void datatypeWriter();
    void structWriter();
    void enumWriter();
    void macroWriter();
    void typedefWriter();
    QDomDocument* m_doc;
    QXlsx::Document* m_xlsx;

    QList<EmosTools::tTypedefData> m_typedefList;
    QList<EmosTools::tDatatypeData> m_typeList;
    QList<EmosTools::tStructData> m_structList;
    QList<EmosTools::tEnumData> m_enumList;
    QList<EmosTools::tMacroData> m_macroList;
    QHash<QString, int> enumList;
};

#endif // EXCELGENERATOR_H
