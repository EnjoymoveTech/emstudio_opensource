#include "srv_parser.h"
#include <QDomDocument>
#include <QFile>
#include <explorer/project.h>
#include <explorer/projectnodes.h>
#include <explorer/projecttree.h>
#include <coreplugin/icore.h>

#include <inja/inja.hpp>
using Json = inja::json;

namespace EmosTools
{

inja::Environment env_srv_parser;
Json json_srv_parser;

QString valueGenerate(const QString& text)
{
    return QString::fromStdString(env_srv_parser.render(text.toStdString(), json_srv_parser).c_str());
}

SrvParser SrvParser::_instance_srv_parse;

SrvParser::SrvParser()
{
}

SrvParser::~SrvParser()
{

}

SrvParser* SrvParser::getInstance()
{
    return &_instance_srv_parse;
}

bool SrvParser::reloadXml()
{
    QMutexLocker lock(&m_mutex);

    QString srvPath = Core::ICore::resourcePath() + QLatin1String("/modeleditor/definite.srv");

    QFile xmlFile(srvPath);
    if(!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QDomDocument doc;
    doc.setContent(&xmlFile);
    xmlFile.close();
    m_moduleMap = SrvParser::loadFromDocument(&doc);
    return true;
}

void SrvParser::PrintModuleInfo()
{

}

void SrvParser::setInput(const QString& key, const QString& value)
{
    json_srv_parser[key.toStdString().c_str()] = value.toStdString().c_str();
}

QMap<QString,tSrvFixedModule> SrvParser::getModules()
{
    return m_moduleMap;
}

tSrvFixedModule SrvParser::getModuleByName(const QString& moduleName)
{
    if (!m_moduleMap.contains(moduleName))
    {
        qDebug()<<"No such module named:"<<moduleName;
        return tSrvFixedModule();
    }
    qDebug()<<"get Module named:"<<moduleName;
    return m_moduleMap[moduleName];
}

tSrvFixedModule SrvParser::getModuleByOid(const QString& oid)
{
    foreach (const QString& moduleName,m_moduleMap.keys())
    {
        if (m_moduleMap[moduleName].oid == oid)
        {
            qDebug()<<"get Module oid:"<<oid;
            return m_moduleMap[moduleName];
        }
    }
    qDebug()<<"No such module by oid:"<<oid;
    return tSrvFixedModule();
}

tSrvFixedModule SrvParser::getModuleByAlias(const QString& alias)
{
    foreach (const QString& moduleName,m_moduleMap.keys())
    {
        if (m_moduleMap[moduleName].alias == alias)
        {
            qDebug()<<"get Module alias:"<<alias;
            return m_moduleMap[moduleName];
        }
    }
    qDebug()<<"No such module by alias:"<<alias;
    return tSrvFixedModule();
}

QMap<QString,tSrvProperty> SrvParser::getAllProperties(const QString& moduleName)
{
    return m_moduleMap[moduleName].propertyMap;
}

QMap<QString,tSrvProperty> SrvParser::getFixedProperties(const QString& moduleName)
{
    QMap<QString,tSrvProperty> properties = getAllProperties(moduleName);
    QMap<QString,tSrvProperty> fixedproperties;
    foreach (const QString& propertyName,properties.keys())
    {
        tSrvProperty curproperty = properties[propertyName];
        if (curproperty.isFixed)
        {
            fixedproperties.insert(propertyName,properties[propertyName]);
        }
    }
    return fixedproperties;
}

QMap<QString,tSrvProperty> SrvParser::getCustomProperties(const QString& moduleName)
{
    QMap<QString,tSrvProperty> properties = getAllProperties(moduleName);
    QMap<QString,tSrvProperty> customproperties;
    foreach (const QString& propertyName,properties.keys())
    {
        tSrvProperty curproperty = properties[propertyName];
        if (!curproperty.isFixed)
        {
            customproperties.insert(propertyName,properties[propertyName]);
        }
    }
    return customproperties;
}

QMap<QString,tSrvProperty> SrvParser::getSelectedProperties(const QString& moduleName,QStringList nameList)
{
    QMap<QString,tSrvProperty> properties = getAllProperties(moduleName);
    QMap<QString,tSrvProperty> selectproperties;
    foreach (const QString& propertyName,nameList)
    {
        tSrvProperty curproperty = properties[propertyName];
        if (!curproperty.isFixed)
        {
            selectproperties.insert(propertyName,properties[propertyName]);
        }
    }
    return selectproperties;
}

tSrvProperty SrvParser::getSrvProperty(const QString& moduleName,const QString& propertyName)
{
    return m_moduleMap[moduleName].propertyMap[propertyName];
}

int SrvParser::moduleCount()
{
    return m_moduleMap.count();
}

int SrvParser::propertiesCount(const QString &moduleName)
{
    return m_moduleMap[moduleName].propertyMap.count();
}

QMap<QString,tSrvFixedModule> SrvParser::loadFromDocument(QDomDocument* doc)
{
    QMap<QString,tSrvFixedModule> moduleMap;
    QDomNodeList moduleList = doc->elementsByTagName("Module");
    for (int i = 0;i < moduleList.size();i++)
    {
        QDomNode moduleNode = moduleList.at(i);
        tSrvFixedModule module;
        module.moduleName = moduleNode.toElement().attribute("name");
        module.comp = moduleNode.toElement().attribute("comp");
        module.oid = moduleNode.toElement().attribute("oid");
        module.alias = moduleNode.toElement().attribute("alias");
        module.type = moduleNode.toElement().attribute("type");
        module.checked = moduleNode.toElement().attribute("checked") == "true" ? true : false;
        module.show = moduleNode.toElement().attribute("show") == "true" ? true : false;
        module.priority = moduleNode.toElement().attribute("priority").toInt();

        QDomNode curNode = moduleNode.firstChild();
        while (!curNode.isNull())
        {
            QDomElement curElement = curNode.toElement();
            if (curElement.tagName() != "Property")
            {
                curNode = curNode.nextSibling();
                continue;
            }
            tSrvProperty property;
            property.propertyName = curElement.attribute("name");
            property.moduleName = module.moduleName;
            property.type = curElement.attribute("type");
            property.value = curElement.attribute("value");
            property.dvalue = curElement.attribute("dvalue");
            property.category = curElement.attribute("category");
            property.isFixed = false;
            QString isFixedStr = curElement.attribute("isFixed");
            if (isFixedStr == "1" || isFixedStr == "true")
            {
                property.isFixed = true;
            }
            else if (isFixedStr == "0" || isFixedStr == "false")
            {
                property.isFixed = false;
            }
            property.generateValue = nullptr;
            if (property.isFixed)
            {
               property.generateValue = valueGenerate;
            }
            module.propertyMap.insert(property.propertyName,property);
            curNode = curNode.nextSibling();
        }
        moduleMap.insert(module.moduleName,module);
    }
    return moduleMap;
}

}
