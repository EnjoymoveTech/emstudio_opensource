#include "descriptioneditorexport.h"
#include "descriptioneditorconstants.h"

#include <QDir>
#include <QTextStream>

#include <explorer/projecttree.h>
#include <explorer/target.h>
#include <explorer/buildconfiguration.h>

namespace DescriptionEditor {

//DescriptionEditorExport DescriptionEditorExport::_instance_description_editor;

DescriptionEditorExport *DescriptionEditorExport::getInstance()
{
    static DescriptionEditorExport _instance_description_editor;
    return &_instance_description_editor;
}

class DescriptionEditorExportPrivate
{
public:
    DescriptionEditorExportPrivate(DescriptionEditorExport *q):q_ptr(q)
    {
        init();
    }

    void init();
    void fillServiceList();
    void fillServiceXml(serviceData&);
    void writeXml();

    QDomElement writeNode(serviceData&);
    serviceData writeData(QDomElement&);

    QList<serviceData> m_serviceList;
    QDomDocument apiDoc;
    QString apiPath;

    DescriptionEditorExport* q_ptr;
};

DescriptionEditorExport::DescriptionEditorExport():d_ptr(new DescriptionEditorExportPrivate(this))
{

}

DescriptionEditorExport::~DescriptionEditorExport()
{
    delete d_ptr;
    d_ptr = nullptr;
}

QList<serviceData> DescriptionEditorExport::servicesData()
{
    return d_ptr->m_serviceList;
}

QDomElement DescriptionEditorExport::servicesNode()
{
    return d_ptr->apiDoc.firstChildElement("emos:ddl").firstChildElement("services");
}

serviceData DescriptionEditorExport::getServiceData(QString name)
{
    for(int i = 0; i < d_ptr->m_serviceList.size(); i++)
    {
        if(d_ptr->m_serviceList.at(i).name == name)
        {
            return d_ptr->m_serviceList.at(i);
        }
    }
    return serviceData();
}

QDomElement DescriptionEditorExport::getServiceNode(QString name)
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

void DescriptionEditorExport::addService(serviceData& service)
{
    d_ptr->m_serviceList.append(service);
    d_ptr->fillServiceXml(service);
    d_ptr->writeXml();
}

void DescriptionEditorExport::addService(QDomElement& serviceNode)
{
    d_ptr->apiDoc.firstChildElement("emos:ddl").firstChildElement("services").appendChild(serviceNode);
    d_ptr->fillServiceList();
    d_ptr->writeXml();
}

void DescriptionEditorExport::clear()
{
    d_ptr->m_serviceList.clear();
    QDomElement servicesNode = d_ptr->apiDoc.firstChildElement("emos:ddl").firstChildElement("services");
    d_ptr->apiDoc.firstChildElement("emos:ddl").removeChild(servicesNode);
    servicesNode = d_ptr->apiDoc.createElement("services");
    d_ptr->apiDoc.firstChildElement("emos:ddl").appendChild(servicesNode);
    d_ptr->writeXml();
}

void DescriptionEditorExport::removeService(QString name)
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
    d_ptr->writeXml();
    for(int i = 0; i < d_ptr->m_serviceList.size(); i++)
    {
        if(d_ptr->m_serviceList.at(i).name == name)
        {
            d_ptr->m_serviceList.removeAt(i);
            break;
        }
    }
}

void DescriptionEditorExport::replaceService(serviceData& service, QString name)
{
    removeService(name);
    addService(service);
}

QDomElement DescriptionEditorExport::toDomElement(serviceData& service)
{
    return d_ptr->writeNode(service);
}

serviceData DescriptionEditorExport::toServiceData(QDomElement& serviceNode)
{
    return d_ptr->writeData(serviceNode);
}

void DescriptionEditorExport::writeToXml()
{
    d_ptr->writeXml();
}

void DescriptionEditorExport::reloadXml()
{
    d_ptr->init();
}

void DescriptionEditorExportPrivate::init()
{
    QString Path = ProjectExplorer::ProjectTree::currentFilePath().toString();
    int lastIdx = Path.lastIndexOf("/");
    lastIdx = Path.leftRef(lastIdx).lastIndexOf("/");
    apiPath = Path.left(lastIdx) + "/1_interface_design/api.description";
    QFile apifile(apiPath);
    apiDoc.clear();
    if(apifile.open(QFile::ReadOnly))
    {
        apiDoc.setContent(&apifile);
        apifile.close();
    }
    fillServiceList();
}

void DescriptionEditorExportPrivate::fillServiceList()
{
    m_serviceList.clear();
    QDomNodeList serviceList = apiDoc.elementsByTagName("service");
    for(int i = 0; i < serviceList.size(); i++)
    {
        QDomElement serviceNode = serviceList.at(i).toElement();
        serviceData serviceItem = writeData(serviceNode);
        m_serviceList.append(serviceItem);
    }
}

void DescriptionEditorExportPrivate::fillServiceXml(serviceData &service)
{
    writeNode(service);
}

void DescriptionEditorExportPrivate::writeXml()
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

QDomElement DescriptionEditorExportPrivate::writeNode(serviceData& service)
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
        for(int i = 0; i < child.size();)
        {
            serviceNode.removeChild(child.at(i));
        }
    }
    serviceNode.setAttribute("name", service.name);
    serviceNode.setAttribute("id", service.id);
    serviceNode.setAttribute("interface_type", service.interfaceType);
    serviceNode.setAttribute("version", service.version);
    for(const auto &method : qAsConst(service.methodList))
    {
        QDomElement methodNode = apiDoc.createElement("element");
        serviceNode.appendChild(methodNode);
        methodNode.setAttribute("name", method.name);
        methodNode.setAttribute("id", method.id);
        methodNode.setAttribute("ret", method.ret);
        methodNode.setAttribute("type", "Method");
        for(const auto &para : qAsConst(method.paraList))
        {
            QDomElement paraNode = apiDoc.createElement("para");
            methodNode.appendChild(paraNode);
            paraNode.setAttribute("name", para.name);
            paraNode.setAttribute("type", para.type);
            paraNode.setAttribute("version", para.version);
        }
    }
    for(const auto &event : qAsConst(service.eventList))
    {
        QDomElement eventNode = apiDoc.createElement("element");
        serviceNode.appendChild(eventNode);
        eventNode.setAttribute("name", event.name);
        eventNode.setAttribute("id", event.id);
        eventNode.setAttribute("data", event.data);
        eventNode.setAttribute("groupid", event.groupId);
        eventNode.setAttribute("type", "Event");
    }
    for(const auto &field : qAsConst(service.fieldList))
    {
        QDomElement fieldNode = apiDoc.createElement("element");
        serviceNode.appendChild(fieldNode);
        fieldNode.setAttribute("name", field.name);
        fieldNode.setAttribute("id", field.id);
        fieldNode.setAttribute("data", field.data);
        fieldNode.setAttribute("selecttype", field.selecttype);
        fieldNode.setAttribute("type", "Field");
    }
    writeXml();
    return serviceNode;
}

serviceData DescriptionEditorExportPrivate::writeData(QDomElement& serviceNode)
{
    serviceData serviceItem = serviceData();
    serviceItem.name = serviceNode.attribute("name");
    serviceItem.id = serviceNode.attribute("id");
    serviceItem.interfaceType = serviceNode.attribute("interface_type");
    serviceItem.version = serviceNode.attribute("version");
    QDomNodeList elementList = serviceNode.elementsByTagName("element");
    for(int j = 0; j < elementList.size(); j++)
    {
        QDomElement elementNode = elementList.at(j).toElement();
        QString type = elementNode.attribute("type");
        if(type == "Method")
        {
            methodData methodItem = methodData();
            methodItem.name = elementNode.attribute("name");
            methodItem.id = elementNode.attribute("id");
            methodItem.ret = elementNode.attribute("ret");
            QDomNodeList paraList = serviceNode.elementsByTagName("para");
            for(int k = 0; k < paraList.size(); k++)
            {
                QDomElement paraNode = paraList.at(k).toElement();
                paraData paraItem = paraData();
                paraItem.name = paraNode.attribute("name");
                paraItem.type = paraNode.attribute("type");
                paraItem.version = paraNode.attribute("version");
                methodItem.paraList.append(paraItem);
            }
            serviceItem.methodList.append(methodItem);
        }
        else if(type == "Event")
        {
            eventData eventItem = eventData();
            eventItem.name = elementNode.attribute("name");
            eventItem.data = elementNode.attribute("data");
            eventItem.id = elementNode.attribute("id");
            eventItem.groupId = elementNode.attribute("groupid");
            serviceItem.eventList.append(eventItem);
        }
        else if(type == "Field")
        {
            fieldData fieldItem = fieldData();
            fieldItem.name = elementNode.attribute("name");
            fieldItem.id = elementNode.attribute("id");
            fieldItem.selecttype = elementNode.attribute("selecttype");
            fieldItem.data = elementNode.attribute("data");
            serviceItem.fieldList.append(fieldItem);
        }
    }
    return serviceItem;
}

}
