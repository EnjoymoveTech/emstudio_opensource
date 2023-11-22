#ifndef IDLPARSER_H
#define IDLPARSER_H

#include "emostools_global.h"

#include <utils/fileutils.h>

#include <QString>
#include <QList>
#include <QtXml/QDomDocument>

namespace EmosTools
{

struct ThreadDataStruct
{
    QString name;
    QString type;
    QString data;
    bool operator == (const ThreadDataStruct& other)
    {
        if(name != other.name ||
            type != other.type ||
            data != other.data)
        {
            return false;
        }
        return true;
    }
};

struct IDLStruct
{
    bool isValid;//是否有效
    bool isService;//是否实现了服务
    bool isClient;//是否引用了服务
    QString oid;//oid
    QString name;//project name
    int priority;//优先级
    QString path;//idl文件路径
    QMap<QString, int> quoteService;//name:id
    QMap<QString, int> service;//name:id
    QMap<QString, QString> property;//name:type
    QList<ThreadDataStruct> thread;
    bool operator == (const IDLStruct& other)
    {
        if(isValid != other.isValid ||
            isService != other.isService ||
            isClient != other.isClient ||
            oid != other.oid ||
            name != other.name ||
            //priority != other.priority ||
            quoteService != other.quoteService ||
            service != other.service ||
            property != other.property ||
            thread != other.thread)
        {
            return false;
        }
        return true;
    }
    bool isEmpty()
    {
        if(oid == "" &&
           name == "" &&
           quoteService.isEmpty() &&
           service.isEmpty() &&
           property.isEmpty() &&
           thread.isEmpty())
        {
            return true;
        }
        return false;
    }
};

class EMOSTOOLS_EXPORT IdlParser
{
public:
    IdlParser();

    static Utils::FilePaths currentProIdlList();
    static QString getIdlFullPath(const QString& oid);
    static QString getIdlFullPathByName(const QString& name);
    static QDomDocument getIdlDocument(const QString& oid);
    static QDomDocument getIdlDocumentByName(const QString& name);
    static QString readIdlOid(const QString& filePath);
    static QString readIdlName(const QString& filePath);
    static IDLStruct getIdlStruct(const QString& oid);
    static IDLStruct getIdlStructByFile(const QString& filePath);
};
}
#endif // CFVPARSER_H
