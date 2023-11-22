#include "idlparser.h"
#include "api_description_parser.h"

#include <explorer/project.h>
#include <explorer/projectnodes.h>
#include <explorer/projecttree.h>

namespace EmosTools
{
bool matchNodesIdl(const ProjectExplorer::Node* n)
{
    if(n)
    {
        if(n->filePath().exists() && !n->filePath().isDir() && n->filePath().endsWith(".idl"))
            return true;
    }
    return false;
}

IdlParser::IdlParser()
{

}

QString IdlParser::readIdlOid(const QString& filePath)
{
    QDomDocument m_doc;
    QFile xmlFile(filePath);
    if (!xmlFile.open(QFile::ReadOnly)) {
        return "";
    }
    m_doc.setContent(&xmlFile);

    QDomNode node = m_doc.documentElement();
    if(!node.isElement())
    {
        xmlFile.close();
        return "";
    }

    if(node.toElement().tagName() != "emos:idl")
    {
        xmlFile.close();
        return "";
    }
    xmlFile.close();
    return node.toElement().attribute("oid");
}

QString IdlParser::readIdlName(const QString &filePath)
{
    QDomDocument m_doc;
    QFile xmlFile(filePath);
    if (!xmlFile.open(QFile::ReadOnly)) {
        return "";
    }
    m_doc.setContent(&xmlFile);

    QDomNode node = m_doc.documentElement();
    if(!node.isElement())
    {
        xmlFile.close();
        return "";
    }

    if(node.toElement().tagName() != "emos:idl")
    {
        xmlFile.close();
        return "";
    }
    xmlFile.close();
    return node.toElement().attribute("project:name");
}

IDLStruct IdlParser::getIdlStruct(const QString &oid)
{
    return getIdlStructByFile(IdlParser::getIdlFullPath(oid));
}

IDLStruct IdlParser::getIdlStructByFile(const QString &filePath)
{
    IDLStruct info;
    info.isService = false;
    info.isClient = false;
    info.isValid = false;
    info.path = filePath;
    ApiParser* api = ApiParser::getInstance();
    api->reloadXml();

    QDomDocument doc;
    QFile xmlFile(filePath);
    if (!xmlFile.open(QFile::ReadOnly)) {
        return info;
    }
    doc.setContent(&xmlFile);

    QDomNode node = doc.documentElement();
    if(!node.isElement())
    {
        xmlFile.close();
        return info;
    }

    if(node.toElement().tagName() != "emos:idl")
    {
        xmlFile.close();
        return info;
    }

    info.isValid = true;
    info.oid = node.toElement().attribute("oid");
    info.name = node.toElement().attribute("project:name");
    info.priority = node.toElement().attribute("priority").toInt();

    if((!node.firstChildElement("QuoteService").isNull()))
    {
        QDomNode quoteNode = node.firstChildElement("QuoteService");
        while(!quoteNode.isNull())
        {
            tSrvService quotesrv = api->getService(quoteNode.toElement().attribute("name"));
            if (quotesrv.id == -1)
            {
                quoteNode = quoteNode.nextSiblingElement("QuoteService");
                continue;
            }
            info.isClient = true;
            info.quoteService.insert(quoteNode.toElement().attribute("name"), quotesrv.id);
            quoteNode = quoteNode.nextSiblingElement("QuoteService");
        }
    }
    if((!node.firstChildElement("Service").isNull()))
    {
        QDomNode serviceNode = node.firstChildElement("Service");
        while(!serviceNode.isNull())
        {
            tSrvService srv = api->getService(serviceNode.toElement().attribute("name"));
            if (srv.id == -1)
            {
                serviceNode = serviceNode.nextSiblingElement("Service");
                continue;
            }
            info.isService = true;
            info.service.insert(serviceNode.toElement().attribute("name"), srv.id);
            serviceNode = serviceNode.nextSiblingElement("Service");
        }
    }
    if((!node.firstChildElement("Property").isNull()))
    {
        QDomNode propertyNode = node.firstChildElement("Property");
        while(!propertyNode.isNull())
        {
            info.property.insert(propertyNode.toElement().attribute("name"), propertyNode.toElement().attribute("type"));
            propertyNode = propertyNode.nextSiblingElement("Property");
        }
    }

    if((!node.firstChildElement("Thread").isNull()))
    {
        QDomNode threadNode = node.firstChildElement("Thread");
        while(!threadNode.isNull())
        {
            ThreadDataStruct thread;
            thread.name = threadNode.toElement().attribute("name");
            thread.type = threadNode.toElement().attribute("type");
            thread.data = threadNode.toElement().attribute("data");
            info.thread << thread;
            threadNode = threadNode.nextSiblingElement("Thread");
        }
    }

    xmlFile.close();
    return info;
}

Utils::FilePaths IdlParser::currentProIdlList()
{
    const ProjectExplorer::Project *pro = ProjectExplorer::ProjectTree::currentProject();
    if(pro)
    {
        Utils::FilePaths files = pro->files(matchNodesIdl);
        return files;
    }

    return Utils::FilePaths();
}

QString IdlParser::getIdlFullPath(const QString &oid)
{
    Utils::FilePaths files = IdlParser::currentProIdlList();
    for(const auto& file : files)
    {
        if(oid == IdlParser::readIdlOid(file.toString()))
            return file.toString();
    }
    return "";
}

QString IdlParser::getIdlFullPathByName(const QString &name)
{
    Utils::FilePaths files = IdlParser::currentProIdlList();
    for(const auto& file : files)
    {
        if(name == IdlParser::readIdlName(file.toString()))
            return file.toString();
    }
    return "";
}

QDomDocument IdlParser::getIdlDocument(const QString &oid)
{
    QString path = getIdlFullPath(oid);
    if(!path.isEmpty())
    {
        QDomDocument m_doc;
        QFile xmlFile(path);
        if (!xmlFile.open(QFile::ReadOnly)) {
            return QDomDocument();
        }
        m_doc.setContent(&xmlFile);
        xmlFile.close();
        return m_doc;
    }

    return QDomDocument();
}

QDomDocument IdlParser::getIdlDocumentByName(const QString &name)
{
    QString path = getIdlFullPathByName(name);
    if(!path.isEmpty())
    {
        QDomDocument m_doc;
        QFile xmlFile(path);
        if (!xmlFile.open(QFile::ReadOnly)) {
            return QDomDocument();
        }
        m_doc.setContent(&xmlFile);
        xmlFile.close();
        return m_doc;
    }

    return QDomDocument();
}

}
