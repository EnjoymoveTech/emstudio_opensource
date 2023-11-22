#ifndef SRVPARSER_H
#define SRVPARSER_H

#include "emostools_global.h"

#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QMap>

class QDomDocument;
namespace EmosTools
{

typedef struct tSrvProperty
{
    QString propertyName;
    QString moduleName;
    QString type;
    QString value;
    QString dvalue;
    QString category;
    bool isFixed;

    QString (*generateValue)(const QString&);
}tSrvProperty;

typedef struct tSrvFixedModule
{
    QString moduleName;
    QString comp;
    QString oid;
    QString alias;
    QString type;
    bool checked;
    bool show;
    int priority;
    QMap<QString,tSrvProperty> propertyMap;

    bool isEmpty()
    {
        return moduleName.isEmpty();
    }

}tSrvFixedModule;


class EMOSTOOLS_EXPORT SrvParser
{
public:
    static SrvParser* getInstance();
    //重新载入：
    bool reloadXml();

    //调试接口：打印当前的文件结构：
    void PrintModuleInfo();

    void setInput(const QString& key, const QString& value);

    //获取名为moduleName的模块：
    tSrvFixedModule getModuleByName(const QString& moduleName);
    //获取oid为oid的模块：
    tSrvFixedModule getModuleByOid(const QString& oid);
    //获取别名为alias的模块：
    tSrvFixedModule getModuleByAlias(const QString& alias);
    //获取该项目定义的所有的模块：
    QMap<QString,tSrvFixedModule> getModules();
    //获取oid为oidValue的模块：
    //tSrvFixedModule getModule(const QString& oidValue);



    //获取moduleName模块的所有属性：
    QMap<QString,tSrvProperty> getAllProperties(const QString& moduleName);
    //获取moduleName模块的所有固定属性（用户不可修改）：
    QMap<QString,tSrvProperty> getFixedProperties(const QString& moduleName);
    //获取moduleName模块的所有自定义属性（用户可修改）：
    QMap<QString,tSrvProperty> getCustomProperties(const QString& moduleName);
    //获取moduleName模块中，包含nameList中名字的所有属性：
    QMap<QString,tSrvProperty> getSelectedProperties(const QString& moduleName,QStringList nameList);

    //获取moduleName模块中，名为propertyName的属性：
    tSrvProperty getSrvProperty(const QString& moduleName,const QString& propertyName);

    //获取module数量：
    int moduleCount();
    //获取module中property数量：
    int propertiesCount(const QString& moduleName);

    static QMap<QString,tSrvFixedModule> loadFromDocument(QDomDocument* doc);

private:
    SrvParser();
    ~SrvParser();

    static SrvParser _instance_srv_parse;
    QMap<QString,tSrvFixedModule> m_moduleMap;
    QMutex m_mutex;
};

}
#endif // SRVPARSER_H
