#ifndef HEADERPARSER_H
#define HEADERPARSER_H

#include <QDomDocument>
#include <xlsx/xlsxdocument.h>
#include "fileparserbase.h"

#define RETURN_IF_FALSE(result) if(!result) return false;

class HeaderParser : FileParserBase
{
public:
    HeaderParser();
    ~HeaderParser();
    bool syntaxCheck(QString headerText);
    bool getInfoFromHeader(QDomDocument &doc, QString mode);
    void getFileNameList(QStringList FileName){fileNameList = FileName;}

private:
    bool braceCheck(QString&);
    QList<int> allIndexOf(QString &text, QString bid);
    void loadInfoFromHeader(QDomDocument &doc, QStringList &fileTextList, QString fileName);
    QHash<QString, int> m_typeHash;
    QStringList fileNameList;
};

#endif // HEADERPARSER_H
