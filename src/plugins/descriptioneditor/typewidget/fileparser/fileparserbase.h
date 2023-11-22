#ifndef FILEPARSERBASE_H
#define FILEPARSERBASE_H

#include <QDomDocument>

#define _POINTER_SIZE 8
#define _ENUM_DEFAULT_TYPE "tUInt8"
#define _ENUM_DEFAULT_SIZE 4
#define _ARRAYSIZE_DEFAULT_VALUE "0"
#define _DEFAULT_FILENAME "common"
#define _DEFAULT_DATATYPES_NUM 20
#define _DEFAULT_ALIGNMENT 4
#define _DESCXML_ROOTNAME "emos:ddl"

class FileParserBase
{
public:
    FileParserBase();
    ~FileParserBase();
    bool mergeNode(QDomElement &, QString);
    void removeAllNodes(QDomDocument &);
    void initHash(QDomDocument&, QHash<QString, int>&);
    QDomElement getNode(QDomDocument& doc, QString nodeTag);
    void calculateSize(QDomDocument&, QHash<QString, int>&);
    void typeCalculator(QDomElement&, QHash<QString, int>&);
    void structCalculator(QDomElement&, QHash<QString, int>&);
protected:
    QString m_mode;
};

#endif // FILEPARSERBASE_H
