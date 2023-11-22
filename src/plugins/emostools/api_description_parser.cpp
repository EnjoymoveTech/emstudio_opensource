#include "api_description_parser.h"
#include <QDomDocument>
#include <QFile>
#include <explorer/project.h>
#include <explorer/projectnodes.h>
#include <explorer/projecttree.h>

namespace EmosTools
{
ApiParser ApiParser::_instance_api_parse;

ApiParser::ApiParser()
{

}

ApiParser::~ApiParser()
{

}

ApiParser* ApiParser::getInstance()
{
    return &_instance_api_parse;
}

bool matchNodesApi(const ProjectExplorer::Node* n)
{
    if(!n)
        return false;

    if(n->filePath().exists() && !n->filePath().isDir() && n->filePath().fileName() == "api.description")
        return true;

    return false;
}

bool ApiParser::reloadXml()
{
    return reloadXml(filePath());
}

bool ApiParser::reloadXml(const QString &path)
{
    QMutexLocker lock(&m_mutex);

    m_services.clear();
    QFile xmlFile(path);
    if(!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QDomDocument doc;
    doc.setContent(&xmlFile);

    m_services = ApiParser::loadFromDocument(&doc);
    xmlFile.close();
    return true;
}

QList<tSrvService> ApiParser::getServices()
{
    QMutexLocker lock(&m_mutex);

    return m_services;
}

tSrvService ApiParser::getService(const QString &serviceName)
{
    tSrvService service = tSrvService();
    for(const auto& i : m_services)
    {
        if(i.name == serviceName)
        {
            service = i;
            break;
        }
    }

    return service;
}

QStringList ApiParser::getServiceNames()
{
    QStringList nameList;
    for(int i = 0; i < m_services.size(); i++)
    {
        nameList.append(m_services.at(i).name);
    }
    return nameList;
}

QList<tSrvService> ApiParser::loadFromDocument(QDomDocument *doc)
{
    QList<tSrvService> services;
    QDomNodeList servicesNode = doc->elementsByTagName("service");
    for(int i = 0; i < servicesNode.size(); ++i)
    {
        QDomNode serviceNode = servicesNode.at(i);
        tSrvService service;
        QList<tSrvFunction> functions;
        QList<tSrvEvent> events;
        QList<tSrvField> fields;
        QList<tSrvPin> pins;
        service.name = serviceNode.toElement().attribute("name");
        service.id = serviceNode.toElement().attribute("id").toUInt();
        service.version = serviceNode.toElement().attribute("version").toUInt();
        service.interfaceType = serviceNode.toElement().attribute("interface_type");
        service.inProcess = serviceNode.toElement().attribute("inprocess").toInt();

        QDomNode now = serviceNode.firstChild();
        while(!now.isNull())
        {
            QString type = now.toElement().attribute("type");
            if (now.toElement().tagName() != "element")
            {
                now = now.nextSibling();
                continue;
            }
            if(now.toElement().attribute("type") == "Method")
            {
                tSrvFunction function;
                QList<tParam> params;
                function.name = now.toElement().attribute("name");
                function.id = now.toElement().attribute("id").toUInt();
                function.returntype = now.toElement().attribute("ret");

                QDomNode para = now.firstChild();
                while(!para.isNull())
                {
                    if (para.toElement().tagName() != "para")
                    {
                        para = para.nextSibling();
                        continue;
                    }
                    tParam param;
                    param.name = para.toElement().attribute("name");
                    param.type = para.toElement().attribute("type");
                    param.version = para.toElement().attribute("version").toUInt();
                    param.direction = tParamDirection(para.toElement().attribute("direction").toInt());
                    param.size = para.toElement().attribute("size").toUInt();

                    params.push_back(param);

                    para = para.nextSibling();
                }
                function.params = params;
                functions.push_back(function);
            }
            else if(now.toElement().attribute("type") == "Event")
            {
                tSrvEvent event;
                event.name = now.toElement().attribute("name");
                event.data = now.toElement().attribute("data");
                event.id = now.toElement().attribute("id").toUInt();
                event.groupid = now.toElement().attribute("groupid").toUInt();
                events.push_back(event);
            }
            else if(now.toElement().attribute("type") == "Field")
            {
                tSrvField field;
                field.name = now.toElement().attribute("name");
                field.id = now.toElement().attribute("id").toUInt();
                field.data = now.toElement().attribute("data");
                field.type = now.toElement().attribute("selecttype").toUInt();
                fields.push_back(field);
            }
            else if(now.toElement().attribute("type") == "Pin")
            {
                tSrvPin pin;
                pin.name = now.toElement().attribute("name");
                pin.id = now.toElement().attribute("id").toUInt();
                pin.data = now.toElement().attribute("data");
                pins.push_back(pin);
            }
            now = now.nextSibling();
        }
        service.events = events;
        service.functions = functions;
        service.fields = fields;
        service.pins = pins;
        services.push_back(service);
    }

    return services;
}

QList<tSrvService> ApiParser::loadFromFile(const QString &fileName)
{
    QDomDocument *doc = new QDomDocument();
    QFile docFile(fileName);
    docFile.open(QIODevice::ReadOnly);
    doc->setContent(&docFile);
    docFile.close();
    return loadFromDocument(doc);
}

QString ApiParser::filePath()
{
    const ProjectExplorer::Project *pro = ProjectExplorer::ProjectTree::currentProject();
    if(!pro)
        return "";

    Utils::FilePaths files = pro->files(matchNodesApi);
    if(files.size() == 0)
        return "";

    return files.at(0).toString();
}

}
