#ifndef SCXML_PARSER_H
#define SCXML_PARSER_H

#include "emostools_global.h"

#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QStringList>

namespace EmosTools
{

typedef struct tAppInfo
{
    QString name;
    QString path;
    int ecuIndex;
}tAppInfo;

typedef struct tState
{
    QString stateName;
    QStringList events;
}tState;

typedef struct tFuncGroup
{
    QString name;
    int groupId;
    int eventId;
    QList<tState> states;
}tFuncGroup;

class EMOSTOOLS_EXPORT ScxmlParser
{
public:
    static ScxmlParser* getInstance();

    bool reloadXml();
    QList<tFuncGroup> getGroups();
    QList<tAppInfo> getAppInfos();

    tFuncGroup getGroupByName(QString groupName);
    tAppInfo getAppInfoByName(QString appName);

    QString getScxmlFilePath();
private:
    ScxmlParser(const ScxmlParser&)  = delete;
    ScxmlParser(ScxmlParser&&)  = delete;
    ScxmlParser& operator=(const ScxmlParser&) = delete;
    ScxmlParser& operator=(ScxmlParser&&) = delete;
private:
    ScxmlParser();
    ~ScxmlParser();
    static ScxmlParser _instance_scxml_parse;

    QList<tFuncGroup> m_funcGroupList;
    QList<tAppInfo> m_appInfoList;
    QMutex m_mutex;
};
}

#endif // SCXML_PARSER_H
