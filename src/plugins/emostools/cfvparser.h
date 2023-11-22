#ifndef CFVPARSER_H
#define CFVPARSER_H

#include "emostools_global.h"

#include <utils/fileutils.h>

#include "idlparser.h"

#include <QString>
#include <QList>
#include <QtXml/QDomDocument>

namespace EmosTools
{

enum ConnectType
{
    METHOD,
    EVENT,
    FIELD,
    PIN
};

struct SeriveInfoStruct
{
    QString name;
    QString address;
    int port;
    int protocol;
    int type;
};

struct ConnectInfoStruct
{
    QString name;//runnable name
    QString service;//连接的服务
    QString info;//服务对应的event/field名
};

struct CustomPropertyStruct
{
    QString key;
    QString type;
    QString value;
};

struct ConnectStruct
{
    QString a_uid;  // service
    QString a_oid;
    QString a_name;
    QString b_uid;  // client
    QString b_oid;
    QString b_name;
    ConnectType type;
    QList<ConnectInfoStruct> event;
    QList<ConnectInfoStruct> field;
    QList<ConnectInfoStruct> pin;
};

struct PinDataStruct
{
    QString name;
    QString port;
    QString serviceName;
};

struct ComponentStruct
{
    IDLStruct idl;
    QString name;//实例名
    QString oid;
    QString uid;
    QRect rect;

    QList<SeriveInfoStruct> service;//cfv界面显示的服务
    QList<CustomPropertyStruct> property;//cfv界面显示的属性
    QList<ConnectStruct> connectList;//cfv界面显示的连接
    QMap<QString, PinDataStruct> pinList;//pinMap name:port信息
};

struct BaseSrvStruct
{
    QString name;
    QList<CustomPropertyStruct> property;
};

struct ProxyInfoStruct
{
    QString serviceName;
    QString localPort;
    QStringList eventNameList;
};

struct StaticInfoStruct
{
    bool selected;
    QList<ProxyInfoStruct> staticInfoList;
};

struct ProcessStruct
{
    QString name;
    QRect rect;
    QString multiIp;
    StaticInfoStruct staticInfo;
    QMap<QString, BaseSrvStruct> baseSrvList;
    QList<ComponentStruct> compList;

    bool hasService(const QString& srvName)
    {
        QMapIterator<QString, BaseSrvStruct> srv(baseSrvList);
        while (srv.hasNext()) {
            srv.next();
            if(srv.key() == srvName)
                return true;
        }
        return false;
    }
};

struct DeviceStruct
{
    QString name;
    QString ip;
    int ecuID;
    QRect rect;
    QList<ProcessStruct> processList;//name: value
};

struct CfvStruct
{
    QList<ComponentStruct> compList;
    QList<ProcessStruct> processList;
    QList<DeviceStruct> deviceList;
    QList<ConnectStruct> connectList;
    QString path;//cfv文件路径
};

class EMOSTOOLS_EXPORT CfvParser
{
public:
    CfvParser();

    static Utils::FilePaths currentProCfvList();
    static QList<QDomNode> parseCfvToNode(const QString& file);
    static QList<QString> getOidListFromCfv(const QString& file);
    static CfvStruct getCfvStruct(const QString& filePath);
    static ComponentStruct getCompFromUid(const QString& uid, QList<ComponentStruct> compList);
    static QList<ComponentStruct> getCompFromOid(const QString& oid, QList<ComponentStruct> compList);
    static QDomDocument SplitGlobalsForProcess(QDomDocument globals, const ProcessStruct &process, const CfvStruct& cfvStruct);
    static QDomDocument SplitGlobalsForDevice(QDomDocument globals, const DeviceStruct &device, const CfvStruct& cfvStruct);
    static ProcessStruct getProcess(const ComponentStruct& comp, const QList<ProcessStruct>& processList);
    static DeviceStruct getDevice(const ComponentStruct& comp, const QList<DeviceStruct>& deviceList);
private:
    static QList<ConnectInfoStruct> getEventInfo(QDomElement com);
    static QList<ConnectInfoStruct> getFieldInfo(QDomElement com);
    static QList<ConnectInfoStruct> getPinInfo(QDomElement com);
    static QList<SeriveInfoStruct> getServiceInfo(QDomElement com);
    static ComponentStruct parseComponent(QDomElement now);
    static ProcessStruct parseProcess(QDomElement now);
    static DeviceStruct parseDevice(QDomElement now);
    static ConnectStruct parseConnect(QDomElement now);
    static QList<ComponentStruct> compMergeToComp(QList<ComponentStruct> compList, QList<ConnectStruct> connectList);
    static QList<ProcessStruct> compMergeToProcess(QList<ProcessStruct> processList, QList<ComponentStruct> compList, QList<ConnectStruct> connectList);
    static QList<DeviceStruct> compMergeToDevice(QList<DeviceStruct> deviceList, QList<ProcessStruct> processList);
};
}
#endif // CFVPARSER_H
