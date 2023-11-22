#include "scxml_parser.h"

#include <QDomDocument>
#include <QFile>
#include <explorer/project.h>
#include <explorer/projectnodes.h>
#include <explorer/projecttree.h>

namespace EmosTools
{
ScxmlParser ScxmlParser::_instance_scxml_parse;

ScxmlParser::ScxmlParser()
{

}

ScxmlParser::~ScxmlParser()
{

}

ScxmlParser* ScxmlParser::getInstance()
{
    return &_instance_scxml_parse;
}

bool matchNodesScxml(const ProjectExplorer::Node* n)
{
    if(!n)
        return false;

    if(n->filePath().exists() && !n->filePath().isDir() && n->filePath().endsWith(".scxml"))
        return true;

    return false;
}

bool ScxmlParser::reloadXml()
{
    QMutexLocker lock(&m_mutex);

    m_funcGroupList.clear();
    m_appInfoList.clear();

    const ProjectExplorer::Project *pro = ProjectExplorer::ProjectTree::currentProject();
    if(!pro)
        return false;

    Utils::FilePaths files = pro->files(matchNodesScxml);
    if(files.size() == 0)
        return false;

    QFile xmlFile(files.at(0).toString());
    if(!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QDomDocument doc;
    doc.setContent(&xmlFile);

    //QDomElement funcGroup = doc.documentElement().firstChildElement("scxml");
    //if(funcGroup.isNull())
        //return false;

    QDomElement fgNode = doc.documentElement().firstChildElement("group");
    while(!fgNode.isNull())
    {
        tFuncGroup fg;
        fg.name = fgNode.toElement().attribute("id");
        fg.groupId = fgNode.toElement().attribute("GroupID").toInt();
        fg.eventId = fgNode.toElement().attribute("EventID").toInt();

        QDomNode now = fgNode.firstChild();
        while(!now.isNull())
        {
            if (now.toElement().tagName() != "state" && now.toElement().tagName() != "final")
            {
                now = now.nextSibling();
                continue;
            }
            tState state = {};
            if (now.toElement().tagName() == "state")
            {
                state.stateName = now.toElement().attribute("id");
            }
            else if (now.toElement().tagName() == "final")
            {
                state.stateName = "Shutdown";
            }
            QDomNode statenode = now.firstChild();
            while (!statenode.isNull())
            {
                if (statenode.toElement().tagName() != "transition" || statenode.toElement().attribute("type") != "internal")
                {
                    statenode = statenode.nextSibling();
                    continue;
                }
                state.events.push_back(statenode.toElement().attribute("event"));
                statenode = statenode.nextSibling();
            }
            fg.states.push_back(state);
            now = now.nextSibling();
        }
        m_funcGroupList.push_back(fg);

        fgNode = fgNode.nextSiblingElement("group");
    }
    fgNode = doc.documentElement().firstChildElement("AppList");
    while (!fgNode.isNull())
    {
        QDomElement appNode = fgNode.firstChildElement("appInfo");
        while (!appNode.isNull())
        {
            tAppInfo appinfo;
            appinfo.name = appNode.attribute("name");
            appinfo.path = appNode.attribute("path");
            appinfo.ecuIndex = appNode.attribute("ecuIndex").toInt();
            m_appInfoList.push_back(appinfo);
            appNode = appNode.nextSiblingElement();
        }
        fgNode = fgNode.nextSiblingElement("appInfo");
    }

    xmlFile.close();
    return true;
}

QList<tFuncGroup> ScxmlParser::getGroups()
{
    QMutexLocker lock(&m_mutex);

    return m_funcGroupList;
}

QList<tAppInfo> ScxmlParser::getAppInfos()
{
    QMutexLocker lock(&m_mutex);

    return m_appInfoList;
}

tFuncGroup ScxmlParser::getGroupByName(QString groupName)
{
    foreach (const tFuncGroup& func,m_funcGroupList)
    {
        if (func.name == groupName)
        {
            return func;
        }
    }
    return tFuncGroup();
}

tAppInfo ScxmlParser::getAppInfoByName(QString appName)
{
    foreach (const tAppInfo& appinfo,m_appInfoList)
    {
        if (appinfo.name == appName)
        {
            return appinfo;
        }
    }
    return tAppInfo();
}

QString ScxmlParser::getScxmlFilePath()
{
    const ProjectExplorer::Project *pro = ProjectExplorer::ProjectTree::currentProject();
    if(!pro)
        return QString();

    Utils::FilePaths files = pro->files(matchNodesScxml);
    if(files.size() == 0)
        return QString();

    return files.at(0).toString();
}
}
