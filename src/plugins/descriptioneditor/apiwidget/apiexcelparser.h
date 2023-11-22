#ifndef APIEXCELPARSER_H
#define APIEXCELPARSER_H

#include <xlsx/xlsxdocument.h>
#include <QDomDocument>

struct portInfomation
{
    QString name;
    QString category;
    QString datatype;
    QString dataname;
    bool operator==(const portInfomation& other) const
    {
        if(name != other.name ||
           category != other.category)
            return false;
        return true;
    }
};

struct serviceInfo
{
    QString name;
    QList<portInfomation> portList;
};

class ApiExcelParser
{
public:
    ApiExcelParser();
    ~ApiExcelParser();
    void setData(QString excelName, QDomDocument* doc);

private:
    bool excelRead();
    void buildStruct(QList<serviceInfo>& serviceList);
    QString paraName(const QString &type, int index);
    QString cellRead(int row, int col, QString name);
    QXlsx::Document* m_xlsx;
    QDomDocument* m_doc;
};

#endif // APIEXCELPARSER_H
