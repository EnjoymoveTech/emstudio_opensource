#ifndef APIPARSER_H
#define APIPARSER_H

#include "emostools_global.h"

#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QList>

class QDomDocument;
namespace EmosTools
{

typedef enum tParamDirection
{
    Param_In,
    Param_Out,
    Param_InOut
}tParamDirection;

typedef struct tParam
{
    QString type;
    QString name;
    quint32 version;
    quint64 size;
    tParamDirection direction;

    bool operator == (const tParam& other) const
    {
        if(name != other.name
                || type != other.type)
        {
            return false;
        }
        return true;
    }
}tParam;

typedef struct tSrvParsingFunction
{
    QString returntype;
    QString name;
    quint64 id;
    QList<tParam> params;//QString name , QString type

    bool operator ==(const tSrvParsingFunction& other) const
    {
        if(name != other.name || id != other.id
                || returntype != other.returntype)
        {
            return false;
        }
        return true;
    }
}tSrvFunction;

typedef struct tSrvParsingEvent
{
    QString name;
    QString data;
    quint64 groupid;
    quint64 id;

    bool operator ==(const tSrvParsingEvent& other) const
    {
        if(name != other.name || groupid != other.groupid || id != other.id || data != other.data)
        {
            return false;
        }
        return true;
    }
}tSrvEvent;

typedef struct tSrvParsingField
{
    QString name;
    quint64 id;
    QString data;
    quint64 type;
    bool operator ==(const tSrvParsingField& other) const
    {
        if(name != other.name ||  id != other.id  || data != other.data || type != other.type)
        {
            return false;
        }
        return true;
    }
}tSrvField;

typedef struct tSrvParsingPin
{
    QString name;
    quint64 id;
    QString data;
    bool operator ==(const tSrvParsingPin& other) const
    {
        if(name != other.name ||  id != other.id  || data != other.data)
        {
            return false;
        }
        return true;
    }
}tSrvPin;

typedef struct tSrvParsingService
{
    QString name;
    quint64 id = -1;
    quint32 version = 1;
    QString interfaceType = "c"; //c接口还是 cpp接口
    bool inProcess = false;//是否只是进程内使用

    QList<tSrvFunction> functions;
    QList<tSrvEvent> events;
    QList<tSrvField> fields;
    QList<tSrvPin> pins;
    bool operator ==(const tSrvParsingService& other) const
    {
        if(name != other.name ||
           version != other.version ||
           interfaceType != other.interfaceType ||
           inProcess != other.inProcess ||
           !(events == other.events) ||
           !(functions == other.functions) ||
           !(fields == other.fields) ||
           !(pins == other.pins))
        {
            return false;
        }
        return true;
    }
    bool isValid() const
    {
        return id > 0;
    }
    bool hasApi() const
    {
        return (functions.size() | events.size() | fields.size() | pins.size());
    }
}tSrvService;

class EMOSTOOLS_EXPORT ApiParser
{
public:
    static ApiParser* getInstance();

    bool reloadXml();
    bool reloadXml(const QString&);

    QList<tSrvService> getServices();
    tSrvService getService(const QString& serviceName);
    QStringList getServiceNames();

    static QList<tSrvService> loadFromDocument(QDomDocument* doc);
    static QList<tSrvService> loadFromFile(const QString& fileName);
private:
    ApiParser(const ApiParser&)  = delete;
    ApiParser(ApiParser&&)  = delete;
    ApiParser& operator=(const ApiParser&) = delete;
    ApiParser& operator=(ApiParser&&) = delete;
private:
    ApiParser();
    ~ApiParser();
    QString filePath();
    static ApiParser _instance_api_parse;

    QList<tSrvService> m_services;
    QMutex m_mutex;
};
}

Q_DECLARE_METATYPE(EmosTools::tParamDirection)
Q_DECLARE_METATYPE(EmosTools::tParam)
Q_DECLARE_METATYPE(EmosTools::tSrvParsingFunction)
Q_DECLARE_METATYPE(EmosTools::tSrvParsingEvent)
Q_DECLARE_METATYPE(EmosTools::tSrvParsingField)
Q_DECLARE_METATYPE(EmosTools::tSrvParsingService)

#endif // APIPARSER_H
