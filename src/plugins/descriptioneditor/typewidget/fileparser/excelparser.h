#ifndef EXCELPARSER_H
#define EXCELPARSER_H

#include <QDomDocument>
#include <xlsx/xlsxdocument.h>
#include "fileparserbase.h"

#define RETURN_IF_FALSE(result) if(!result) return false;

class ExcelParser : FileParserBase
{
public:
    ExcelParser();
    ~ExcelParser();
    bool setData(QString excelName, QDomDocument &doc, QString mode);

private:
    bool excelRead(QString excelName);
    bool datatypeParser();
    bool structParser();
    bool enumParser();
    bool macroParser();
    bool typedefParser();
    QHash<QString, int> m_typeHash;
    QString cellRead(int row, int col, QString name);
    QDomDocument m_doc;
    QXlsx::Document* m_xlsx;
};

#endif // EXCELPARSER_H
