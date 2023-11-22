#include "api_writer.h"

#include <QDir>
#include <QTextStream>
#include <explorer/project.h>
#include <explorer/projecttree.h>

ApiWriter *ApiWriter::getInstance()
{
    static ApiWriter _instance_api_writer;
    return &_instance_api_writer;
}

class ApiWriterPrivate
{
public:
    ApiWriterPrivate(ApiWriter *q):q_ptr(q)
    {
        QString Path = ProjectExplorer::ProjectTree::currentProject()->projectDirectory().toString();
        apiPath = Path + "/1_interface_design/api.description";
        init();
    }

    void init();
    void fillServiceList();
    void fillServiceXml(tSrvService&);
    void writeXml();

    QDomElement writeNode(tSrvService&);
    tSrvService writeData(QDomElement&);

    QList<tSrvService> m_serviceList;
    QDomDocument apiDoc;
    QString apiPath;

    ApiWriter* q_ptr;
};

ApiWriter::ApiWriter():d_ptr(new ApiWriterPrivate(this))
{

}

ApiWriter::~ApiWriter()
{
    delete d_ptr;
    d_ptr = nullptr;
}

QList<tSrvService> ApiWriter::servicesData()
{
    return d_ptr->m_serviceList;
}

void ApiWriter::setPath(const QString& path)
{
    d_ptr->apiPath = path;
}

QDomElement ApiWriter::servicesNode()
{
    return d_ptr->apiDoc.firstChildElement("emos:ddl").firstChildElement("services");
}

tSrvService ApiWriter::getServiceData(QString name)
{
    for(int i = 0; i < d_ptr->m_serviceList.size(); i++)
    {
        if(d_ptr->m_serviceList.at(i).name == name)
        {
            return d_ptr->m_serviceList.at(i);
        }
    }
    return tSrvService();
}

QDomElement ApiWriter::getServiceNode(QString name)
{
    QDomNodeList serviceNodeList = d_ptr->apiDoc.elementsByTagName("service");
    for(int i = 0; i < serviceNodeList.size(); i++)
    {
        if(serviceNodeList.at(i).toElement().attribute("name") == name)
        {
            return serviceNodeList.at(i).toElement();
        }
    }
    return QDomElement();
}

QStringList ApiWriter::getServicesName()
{
    QStringList res;
    for(auto& data : d_ptr->m_serviceList)
    {
        res << data.name;
    }
    return res;
}

void ApiWriter::addService(tSrvService& service)
{
    d_ptr->m_serviceList.append(service);
    d_ptr->fillServiceXml(service);
    //d_ptr->writeXml();
}

void ApiWriter::addService(QDomElement& serviceNode)
{
    d_ptr->apiDoc.firstChildElement("emos:ddl").firstChildElement("services").appendChild(serviceNode);
    d_ptr->fillServiceList();
    //d_ptr->writeXml();
}

void ApiWriter::clear()
{
    d_ptr->m_serviceList.clear();
    QDomElement servicesNode = d_ptr->apiDoc.firstChildElement("emos:ddl").firstChildElement("services");
    d_ptr->apiDoc.firstChildElement("emos:ddl").removeChild(servicesNode);
    servicesNode = d_ptr->apiDoc.createElement("services");
    d_ptr->apiDoc.firstChildElement("emos:ddl").appendChild(servicesNode);
    //d_ptr->writeXml();
}

void ApiWriter::removeService(QString name)
{
    QDomNodeList serviceNodeList = d_ptr->apiDoc.elementsByTagName("service");
    for(int i = 0; i < serviceNodeList.size(); i++)
    {
        if(serviceNodeList.at(i).toElement().attribute("name") == name)
        {
            d_ptr->apiDoc.firstChildElement("emos:ddl").firstChildElement("services").removeChild(serviceNodeList.at(i));
            break;
        }
    }
    //d_ptr->writeXml();
    for(int i = 0; i < d_ptr->m_serviceList.size(); i++)
    {
        if(d_ptr->m_serviceList.at(i).name == name)
        {
            d_ptr->m_serviceList.removeAt(i);
            break;
        }
    }
}

void ApiWriter::replaceService(tSrvService& service, QString name)
{
    removeService(name);
    addService(service);
}

QDomElement ApiWriter::toDomElement(tSrvService& service)
{
    return d_ptr->writeNode(service);
}

tSrvService ApiWriter::toServiceData(QDomElement& serviceNode)
{
    return d_ptr->writeData(serviceNode);
}

void ApiWriter::writeToXml()
{
    d_ptr->writeXml();
}

void ApiWriter::reloadXml()
{
    d_ptr->init();
}

void ApiWriterPrivate::init()
{
    QFile apifile(apiPath);
    apiDoc.clear();
    if(apifile.open(QFile::ReadOnly))
    {
        apiDoc.setContent(&apifile);
        apifile.close();
    }
    fillServiceList();
}

void ApiWriterPrivate::fillServiceList()
{
    m_serviceList.clear();
    QDomNodeList serviceList = apiDoc.elementsByTagName("service");
    for(int i = 0; i < serviceList.size(); i++)
    {
        QDomElement serviceNode = serviceList.at(i).toElement();
        tSrvService serviceItem = writeData(serviceNode);
        m_serviceList.append(serviceItem);
    }
}

void ApiWriterPrivate::fillServiceXml(tSrvService &service)
{
    writeNode(service);
}

void ApiWriterPrivate::writeXml()
{
    if(QFile::exists(apiPath)) //如果文件已存在，进行删除
    {
        QFile::remove(apiPath);
    }
    QFile file(apiPath);
    if(!file.open(QIODevice::ReadWrite))
    {
        return;
    }
    QTextStream stream(&file);
    apiDoc.save(stream, 4, QDomNode::EncodingFromTextStream);
}

QDomElement ApiWriterPrivate::writeNode(tSrvService& service)
{
    QDomElement serviceNode = q_ptr->getServiceNode(service.name);
    if(!serviceNode.hasAttribute("name"))
    {
        serviceNode = apiDoc.createElement("service");
        apiDoc.firstChildElement("emos:ddl").firstChildElement("services").appendChild(serviceNode);
    }
    else
    {
        QDomNodeList child = serviceNode.elementsByTagName("element");
        for(int i = 0; i < child.size();i++)
        {
            serviceNode.removeChild(child.at(i));
        }
    }
    serviceNode.setAttribute("name", service.name);
    serviceNode.setAttribute("id", service.id);
    serviceNode.setAttribute("interface_type", service.interfaceType);
    serviceNode.setAttribute("version", service.version);
    for(const auto &method : qAsConst(service.functions))
    {
        QDomElement methodNode = apiDoc.createElement("element");
        serviceNode.appendChild(methodNode);
        methodNode.setAttribute("name", method.name);
        methodNode.setAttribute("id", method.id);
        methodNode.setAttribute("ret", method.returntype);
        methodNode.setAttribute("type", "Method");
        for(const auto &para : qAsConst(method.params))
        {
            QDomElement paraNode = apiDoc.createElement("para");
            methodNode.appendChild(paraNode);
            paraNode.setAttribute("name", para.name);
            paraNode.setAttribute("type", para.type);
            paraNode.setAttribute("version", para.version);
        }
    }
    for(const auto &event : qAsConst(service.events))
    {
        QDomElement eventNode = apiDoc.createElement("element");
        serviceNode.appendChild(eventNode);
        eventNode.setAttribute("name", event.name);
        eventNode.setAttribute("id", event.id);
        eventNode.setAttribute("data", event.data);
        eventNode.setAttribute("groupid", event.groupid);
        eventNode.setAttribute("type", "Event");
    }
    for(const auto &field : qAsConst(service.fields))
    {
        QDomElement fieldNode = apiDoc.createElement("element");
        serviceNode.appendChild(fieldNode);
        fieldNode.setAttribute("name", field.name);
        fieldNode.setAttribute("id", field.id);
        fieldNode.setAttribute("data", field.data);
        fieldNode.setAttribute("selecttype", field.type);
        fieldNode.setAttribute("type", "Field");
    }
    //writeXml();
    return serviceNode;
}

tSrvService ApiWriterPrivate::writeData(QDomElement& serviceNode)
{
    tSrvService serviceItem = tSrvService();
    serviceItem.name = serviceNode.attribute("name");
    serviceItem.id = serviceNode.attribute("id").toInt();
    serviceItem.interfaceType = serviceNode.attribute("interface_type");
    serviceItem.version = serviceNode.attribute("version").toInt();
    QDomNodeList elementList = serviceNode.elementsByTagName("element");
    for(int j = 0; j < elementList.size(); j++)
    {
        QDomElement elementNode = elementList.at(j).toElement();
        QString type = elementNode.attribute("type");
        if(type == "Method")
        {
            tSrvFunction methodItem = tSrvFunction();
            methodItem.name = elementNode.attribute("name");
            methodItem.id = elementNode.attribute("id").toInt();
            methodItem.returntype = elementNode.attribute("ret");
            QDomNodeList paraList = elementNode.elementsByTagName("para");
            for(int k = 0; k < paraList.size(); k++)
            {
                QDomElement paraNode = paraList.at(k).toElement();
                tParam paraItem = tParam();
                paraItem.name = paraNode.attribute("name");
                paraItem.type = paraNode.attribute("type");
                paraItem.version = paraNode.attribute("version").toInt();
                methodItem.params.append(paraItem);
            }
            serviceItem.functions.append(methodItem);
        }
        else if(type == "Event")
        {
            tSrvEvent eventItem = tSrvEvent();
            eventItem.name = elementNode.attribute("name");
            eventItem.data = elementNode.attribute("data");
            eventItem.id = elementNode.attribute("id").toInt();
            eventItem.groupid = elementNode.attribute("groupid").toInt();
            serviceItem.events.append(eventItem);
        }
        else if(type == "Field")
        {
            tSrvField fieldItem = tSrvField();
            fieldItem.name = elementNode.attribute("name");
            fieldItem.id = elementNode.attribute("id").toInt();
            fieldItem.type = elementNode.attribute("selecttype").toInt();
            fieldItem.data = elementNode.attribute("data");
            serviceItem.fields.append(fieldItem);
        }
    }
    return serviceItem;
}
