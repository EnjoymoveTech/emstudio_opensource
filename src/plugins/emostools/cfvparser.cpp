#include "cfvparser.h"
#include "srv_parser.h"
#include "api_description_parser.h"

#include <explorer/project.h>
#include <explorer/projectnodes.h>
#include <explorer/projecttree.h>

#include <utils/algorithm.h>

namespace EmosTools
{

bool matchNodesCfv(const ProjectExplorer::Node* n)
{
    if(n)
    {
        if(n->filePath().exists() && !n->filePath().isDir() && n->filePath().endsWith(".cfv"))
            return true;
    }
    return false;
}

CfvParser::CfvParser()
{

}

QList<QDomNode> CfvParser::parseCfvToNode(const QString &file)
{
    QList<QDomNode> nodeList;

    QDomDocument m_doc;
    QFile xmlFile(file);
    if (!xmlFile.open(QFile::ReadOnly)) {
        return nodeList;
    }
    m_doc.setContent(&xmlFile);

    QDomNode node = m_doc.documentElement();
    if(!node.isElement())
    {
        xmlFile.close();
        return nodeList;
    }

    QDomNodeList instanceList = m_doc.elementsByTagName("instance");
    for(int i = 0; i < instanceList.size(); ++i)
    {
        if(instanceList.at(i).toElement().attribute("type") == "DComponent" ||
           instanceList.at(i).toElement().attribute("type") == "DAssociation" ||
           instanceList.at(i).toElement().attribute("type") == "DBoundary" ||
           instanceList.at(i).toElement().attribute("type") == "DDevice")
        {
            nodeList.push_back(instanceList.at(i));
        }
    }
    xmlFile.close();
    return nodeList;
}

QList<QString> CfvParser::getOidListFromCfv(const QString &file)
{
    QList<QString> oidList;

    QList<QDomNode> nodeList = CfvParser::parseCfvToNode(file);

    for(int i = 0; i < nodeList.size(); ++i)
    {
        QDomElement e = nodeList.at(i).firstChildElement("DComponent").firstChildElement("base-DObject").firstChildElement("DObject").
                firstChildElement("base-DElement").firstChildElement("DElement").firstChildElement("oid");
        if(!e.isNull())
        {
            oidList.push_back(e.text());
        }
    }

    return oidList;
}

QList<SeriveInfoStruct> CfvParser::getServiceInfo(QDomElement com)
{
    QList<SeriveInfoStruct> serviceList;

    QDomNodeList itemList = com.firstChildElement("serviceInfo").firstChildElement("qhash").childNodes();
    for(int i = 0; i < itemList.size(); i++)
    {
        SeriveInfoStruct service;
        service.name = itemList.at(i).firstChildElement("pair").firstChildElement("key").text();
        QString value = itemList.at(i).firstChildElement("pair").firstChildElement("value").text();
        service.address = value.split(";")[0];
        service.port = value.split(";")[1].toInt();
        service.protocol = value.split(";")[2].toInt();
        service.type = value.split(";")[3].toInt();
        serviceList.push_back(service);
    }

    return serviceList;
}

ComponentStruct CfvParser::parseComponent(QDomElement now)
{
    ComponentStruct comp;
    QDomElement dComponent = now.firstChildElement("DComponent");
    QDomElement dObject = dComponent.firstChildElement("base-DObject").firstChildElement("DObject");
    QDomElement dElement = dObject.firstChildElement("base-DElement").firstChildElement("DElement");
    comp.oid = dElement.firstChildElement("oid").text();
    comp.uid = dElement.firstChildElement("uid").text();
    comp.name = dObject.firstChildElement("name").text();

    int center_x = dObject.firstChildElement("pos").text().split(";").first().split(":").last().toInt();
    int center_y = dObject.firstChildElement("pos").text().split(";").last().split(":").last().toInt();
    int center_w = dObject.firstChildElement("rect").text().split(";")[2].split(":").last().toInt();
    int center_h = dObject.firstChildElement("rect").text().split(";")[3].split(":").last().toInt();
    comp.rect.setRect(center_x - center_w / 2, center_y - center_h / 2, center_w, center_h);
    comp.service = CfvParser::getServiceInfo(dComponent);

    //property
    QDomNodeList itemList = dComponent.firstChildElement("propertyInfo").firstChildElement("qhash").elementsByTagName("item");
    for(int j = 0; j < itemList.size(); j++)
    {
        CustomPropertyStruct property;
        property.key = itemList.at(j).firstChildElement("pair").firstChildElement("key").text();
        property.type = itemList.at(j).firstChildElement("pair").firstChildElement("value").text().split(";").first();
        property.value = itemList.at(j).firstChildElement("pair").firstChildElement("value").text().split(";").last();
        comp.property.push_back(property);
    }

    QDomNodeList pinList = dComponent.firstChildElement("pinInfo").firstChildElement("qhash").elementsByTagName("item");
    for(int j = 0; j < pinList.size(); j++)
    {
        PinDataStruct pin;
        QStringList strlist = pinList.at(j).firstChildElement("pair").firstChildElement("key").text().split(":");
        if(strlist.size() != 2)
            continue;
        pin.name = strlist.at(1).trimmed();
        pin.port = pinList.at(j).firstChildElement("pair").firstChildElement("value").text();
        pin.serviceName = strlist.at(0).trimmed();
        comp.pinList.insert(pinList.at(j).firstChildElement("pair").firstChildElement("key").text(), pin);
    }

    //IDL
    IDLStruct idlStruct = IdlParser::getIdlStruct(comp.oid);
    comp.idl = idlStruct;
    return comp;
}

ProcessStruct CfvParser::parseProcess(QDomElement now)
{
    ProcessStruct process;
    QDomElement dBoundary = now.firstChildElement("DBoundary");
    process.name = dBoundary.firstChildElement("text").text();
    process.name = process.name.isEmpty() ? "Master" : process.name;
    int center_x = dBoundary.firstChildElement("pos").text().split(";").first().split(":").last().toInt();
    int center_y = dBoundary.firstChildElement("pos").text().split(";").last().split(":").last().toInt();
    int center_w = dBoundary.firstChildElement("rect").text().split(";")[2].split(":").last().toInt();
    int center_h = dBoundary.firstChildElement("rect").text().split(";")[3].split(":").last().toInt();
    process.rect.setRect(center_x - center_w / 2, center_y - center_h / 2, center_w, center_h);
    QDomNodeList items = dBoundary.firstChildElement("baseSrvInfo").firstChildElement("qhash").elementsByTagName("item");
    for(int i = 0; i < items.size(); i++)
    {
        if(items.at(i).firstChildElement("pair").firstChildElement("key").text().contains("strSdMultiAddress"))
        {
            process.multiIp = items.at(i).firstChildElement("pair").firstChildElement("value").text();
            break;
        }
    }
    //自定义服务
    QStringList selectSrv;
    QMap<QString, BaseSrvStruct> baseSrvList;
    QDomNodeList selectItem = dBoundary.firstChildElement("baseSrvSelect").firstChildElement("qlist").elementsByTagName("item");
    for(int i = 0; i < selectItem.size(); i++)
    {
        selectSrv << selectItem.at(i).toElement().text();
    }

    QDomNodeList itemList = dBoundary.firstChildElement("baseSrvInfo").firstChildElement("qhash").elementsByTagName("item");
    for(int j = 0; j < itemList.size(); j++)
    {
        QString moduleName = itemList.at(j).firstChildElement("pair").firstChildElement("key").text().split(";").first();
        QString propertyName = itemList.at(j).firstChildElement("pair").firstChildElement("key").text().split(";").last();

        if(selectSrv.contains(moduleName))
        {
            BaseSrvStruct baseSrv;
            if(baseSrvList.contains(moduleName))
                baseSrv = baseSrvList.value(moduleName);
            else
                baseSrv.name = moduleName;

            CustomPropertyStruct customProperty;
            customProperty.key = propertyName;
            customProperty.value = itemList.at(j).firstChildElement("pair").firstChildElement("value").text();
            baseSrv.property << customProperty;

            baseSrvList.insert(moduleName, baseSrv);
        }
    }
    process.baseSrvList = baseSrvList;

    // 静态配置
    StaticInfoStruct staticInfo;
    staticInfo.selected = (dBoundary.firstChildElement("staticSelected").text() == "true");
    itemList = dBoundary.firstChildElement("staticInfo").firstChildElement("qhash").elementsByTagName("item");
    for(int j = 0; j < itemList.size(); j++)
    {
        ProxyInfoStruct proxyInfo;
        proxyInfo.serviceName = itemList.at(j).firstChildElement("pair").firstChildElement("key").text();
        proxyInfo.eventNameList = itemList.at(j).firstChildElement("pair").firstChildElement("value").text().split(";");
        proxyInfo.localPort = proxyInfo.eventNameList.at(0);
        proxyInfo.eventNameList.removeAt(0);
        staticInfo.staticInfoList.append(proxyInfo);
    }
    process.staticInfo = staticInfo;

    return process;
}

DeviceStruct CfvParser::parseDevice(QDomElement now)
{
    DeviceStruct device;
    QDomElement dDevice = now.firstChildElement("DDevice");
    device.name = dDevice.firstChildElement("text").text();
    device.name = device.name.isEmpty() ? "Local" : device.name;
    int center_x = dDevice.firstChildElement("pos").text().split(";").first().split(":").last().toInt();
    int center_y = dDevice.firstChildElement("pos").text().split(";").last().split(":").last().toInt();
    int center_w = dDevice.firstChildElement("rect").text().split(";")[2].split(":").last().toInt();
    int center_h = dDevice.firstChildElement("rect").text().split(";")[3].split(":").last().toInt();
    device.rect.setRect(center_x - center_w / 2, center_y - center_h / 2, center_w, center_h);
    device.ip = dDevice.firstChildElement("ip").text();
    device.ecuID = dDevice.firstChildElement("ecuID").text().toInt();

    return device;
}

ConnectStruct CfvParser::parseConnect(QDomElement now)
{
    ConnectStruct connect;
    QDomElement dAssociation = now.firstChildElement("DAssociation");
    QDomElement dRelation = dAssociation.firstChildElement("base-DRelation").firstChildElement("DRelation");
    connect.a_name = dRelation.firstChildElement("a_name").text();
    connect.a_oid = dRelation.firstChildElement("a_oid").text();
    connect.a_uid = dRelation.firstChildElement("a").text();
    connect.b_name = dRelation.firstChildElement("b_name").text();
    connect.b_oid = dRelation.firstChildElement("b_oid").text();
    connect.b_uid = dRelation.firstChildElement("b").text();

    QString type = dAssociation.firstChildElement("a").firstChildElement("DAssociationEnd").firstChildElement("kindName").text();
    if(type == "Method")
        connect.type = ConnectType::METHOD;
    if(type == "Event")
        connect.type = ConnectType::EVENT;
    if(type == "Field")
        connect.type = ConnectType::FIELD;
    if(type == "Pin")
        connect.type = ConnectType::PIN;

    connect.event = CfvParser::getEventInfo(dAssociation);
    connect.field = CfvParser::getFieldInfo(dAssociation);
    connect.pin = CfvParser::getPinInfo(dAssociation);

    return connect;
}

QList<ComponentStruct> CfvParser::compMergeToComp(QList<ComponentStruct> compList, QList<ConnectStruct> connectList)
{
    QList<ComponentStruct> outCompList;

    for(const auto& comp : compList)
    {
        ComponentStruct compCopy = comp;

        //添加connect
        for(const auto& i : connectList)
        {
            if(i.b_uid == compCopy.uid)
                compCopy.connectList.push_back(i);
        }

        outCompList.push_back(compCopy);
    }

    return outCompList;
}

QList<ProcessStruct> CfvParser::compMergeToProcess(QList<ProcessStruct> processList, QList<ComponentStruct> compList, QList<ConnectStruct> connectList)
{
    QList<ProcessStruct> outProcessList;

    auto getComponentProcessName = [](const QRect& rect, QList<ProcessStruct> processList) -> QString
    {
        for(const auto& processIt : processList)
        {
            if(processIt.rect.contains(rect))//属于此进程
            {
                return processIt.name;
            }
        }
        return "";
    };

    for(const auto& processIt : processList)
    {
        ProcessStruct process;
        process.name = processIt.name;
        process.rect = processIt.rect;
        process.multiIp = processIt.multiIp;
        process.baseSrvList = processIt.baseSrvList;
        process.staticInfo = processIt.staticInfo;

        for(const auto& comp : compList)
        {
            if(getComponentProcessName(comp.rect, processList) == processIt.name &&
                    processIt.name != "")
            {
                process.compList.push_back(comp);
            }
        }

        outProcessList.push_back(process);
    }

    return outProcessList;
}

QList<DeviceStruct> CfvParser::compMergeToDevice(QList<DeviceStruct> deviceList, QList<ProcessStruct> processList)
{
    QList<DeviceStruct> outDeviceList;

    auto getProcessDeviceName = [](const QRect& rect, QList<DeviceStruct> deviceList) -> QString
    {
        for(const auto& deviceIt : deviceList)
        {
            if(deviceIt.rect.contains(rect))//属于此进程
            {
                return deviceIt.name;
            }
        }
        return "";
    };

    for(const auto& deviceIt : deviceList)
    {
        DeviceStruct device;
        device.name = deviceIt.name;
        device.rect = deviceIt.rect;
        device.ip = deviceIt.ip;
        device.ecuID = deviceIt.ecuID;

        for(const auto& process : processList)
        {
            if(getProcessDeviceName(process.rect, deviceList) == deviceIt.name &&
               deviceIt.name != "")
            {
                ProcessStruct processCopy = process;
                device.processList.push_back(processCopy);
            }
        }

        outDeviceList.push_back(device);
    }
    return outDeviceList;
}

CfvStruct CfvParser::getCfvStruct(const QString &filePath)
{
    CfvStruct cfv;
    cfv.path = filePath;
    QList<ComponentStruct> compList;
    QList<ConnectStruct> connectList;
    QList<ProcessStruct> processList;
    QList<DeviceStruct> deviceList;

    QList<QDomNode> listNode = CfvParser::parseCfvToNode(filePath);

    for(int i = 0;i < listNode.size();i++)
    {
        QDomElement now = listNode.at(i).toElement();
        if(now.tagName() == "instance" && now.attribute("type") == "DComponent")//解析Component
        {
            ComponentStruct com = CfvParser::parseComponent(now);
            compList.push_back(com);
        }
        else if(now.tagName() == "instance" && now.attribute("type") == "DBoundary")//解析Process
        {
            ProcessStruct process = CfvParser::parseProcess(now);
            processList.push_back(process);
        }
        else if(now.tagName() == "instance" && now.attribute("type") == "DDevice")//解析Device
        {
            DeviceStruct device = CfvParser::parseDevice(now);
            deviceList.push_back(device);
        }
        else if(now.tagName() == "instance" && now.attribute("type") == "DAssociation")//解析Connect
        {
            connectList.push_back(CfvParser::parseConnect(now));
        }
    }

    cfv.connectList = connectList;
    cfv.compList = CfvParser::compMergeToComp(compList,connectList);//连线信息加入到comp
    cfv.processList = CfvParser::compMergeToProcess(processList, cfv.compList, connectList);//comp信息加入到process
    cfv.deviceList = CfvParser::compMergeToDevice(deviceList, cfv.processList);//comp信息加入到device

    return cfv;
}

ComponentStruct CfvParser::getCompFromUid(const QString &uid, QList<ComponentStruct> compList)
{
    return Utils::findOrDefault(compList, [uid](const ComponentStruct& comp){
        return comp.uid == uid;
    });
}

QList<ComponentStruct> CfvParser::getCompFromOid(const QString &oid, QList<ComponentStruct> compList)
{
    return Utils::filtered(compList, [oid](const ComponentStruct& comp){
        return comp.oid == oid;
    });
}

QDomDocument CfvParser::SplitGlobalsForProcess(QDomDocument globals, const ProcessStruct &process, const CfvStruct& cfvStruct)
{
    EmosTools::SrvParser::getInstance()->reloadXml();
    QDomDocument outGlobals;
    outGlobals.setContent(globals.toString());

    QSet<QString> plugins;

    //基础服务
    QMapIterator<QString, EmosTools::BaseSrvStruct> baseSrvList(process.baseSrvList);
    while(baseSrvList.hasNext())
    {
        baseSrvList.next();
        QString compName = EmosTools::SrvParser::getInstance()->getModuleByName(baseSrvList.key()).comp;
        if(compName != "")
            plugins.insert(compName);
    }

    for(const auto& comp : process.compList)
    {
        plugins.insert(comp.oid.split(".").last() + ".comp");
        plugins.insert(comp.oid.split(".").last() + "service.comp");

        for(const auto& c : comp.connectList)
        {
            ComponentStruct CompA = CfvParser::getCompFromUid(c.a_uid, cfvStruct.compList);

            if(CfvParser::getProcess(CompA, cfvStruct.processList).name !=
               CfvParser::getProcess(comp, cfvStruct.processList).name)
            {
                plugins.insert(c.a_oid.split(".").last() + "proxy.comp");
            }
        }
    }

    QDomElement pluginsElement = outGlobals.documentElement().firstChildElement("plugins");
    QDomElement itemPlugin = pluginsElement.firstChildElement("plugin");
    while(!itemPlugin.isNull())
    {
        if(!plugins.contains(itemPlugin.attribute("url")))
        {
            QDomElement removeItem = itemPlugin;
            itemPlugin = itemPlugin.nextSiblingElement("plugin");
            pluginsElement.removeChild(removeItem);
        }
        else
            itemPlugin = itemPlugin.nextSiblingElement("plugin");
    }

    return outGlobals;
}

QDomDocument CfvParser::SplitGlobalsForDevice(QDomDocument globals, const DeviceStruct &device, const CfvStruct& cfvStruct)
{
    EmosTools::SrvParser::getInstance()->reloadXml();
    QDomDocument outGlobals;
    outGlobals.setContent(globals.toString());

    QSet<QString> plugins;

    for(const auto& processIt : device.processList)
    {
        //基础服务
        QMapIterator<QString, EmosTools::BaseSrvStruct> baseSrvList(processIt.baseSrvList);
        while(baseSrvList.hasNext())
        {
            baseSrvList.next();
            QString compName = EmosTools::SrvParser::getInstance()->getModuleByName(baseSrvList.key()).comp;
            if(compName != "")
                plugins.insert(compName);
        }

        for(const auto& comp : processIt.compList)
        {
            plugins.insert(comp.oid.split(".").last() + ".comp");
            plugins.insert(comp.oid.split(".").last() + "service.comp");

            for(const auto& c : comp.connectList)
            {
                ComponentStruct CompA = CfvParser::getCompFromUid(c.a_uid, cfvStruct.compList);

                if(CfvParser::getProcess(CompA, cfvStruct.processList).name !=
                   CfvParser::getProcess(comp, cfvStruct.processList).name)
                {
                    plugins.insert(c.a_oid.split(".").last() + "proxy.comp");
                }
            }
        }
    }

    QDomElement pluginsElement = outGlobals.documentElement().firstChildElement("plugins");
    QDomElement itemPlugin = pluginsElement.firstChildElement("plugin");
    while(!itemPlugin.isNull())
    {
        if(!plugins.contains(itemPlugin.attribute("url")))
        {
            QDomElement removeItem = itemPlugin;
            itemPlugin = itemPlugin.nextSiblingElement("plugin");
            pluginsElement.removeChild(removeItem);
        }
        else
            itemPlugin = itemPlugin.nextSiblingElement("plugin");
    }

    return outGlobals;
}

ProcessStruct CfvParser::getProcess(const ComponentStruct& comp, const QList<ProcessStruct>& processList)
{
    return Utils::findOrDefault(processList, [comp](const ProcessStruct& process){
        return Utils::anyOf(process.compList, [comp](const ComponentStruct& c){
            return comp.uid == c.uid;
        });
    });
}

DeviceStruct CfvParser::getDevice(const ComponentStruct &comp, const QList<DeviceStruct> &deviceList)
{
    return Utils::findOrDefault(deviceList, [comp](const DeviceStruct& device){
        return Utils::anyOf(device.processList, [comp](const ProcessStruct& p){
            return Utils::anyOf(p.compList, [comp](const ComponentStruct& c){
                return comp.uid == c.uid;
            });
        });
    });
}

QList<ConnectInfoStruct> CfvParser::getEventInfo(QDomElement com)
{
    QList<ConnectInfoStruct> eventList;

    QDomNodeList itemList = com.firstChildElement("eventMap").firstChildElement("qhash").childNodes();
    for(int i = 0; i < itemList.size(); i++)
    {
        ConnectInfoStruct event;
        event.name = itemList.at(i).firstChildElement("pair").firstChildElement("key").text();
        QString value = itemList.at(i).firstChildElement("pair").firstChildElement("value").text();
        if(value.split(":").size() < 2)
            continue;
        event.service = value.split(":")[0].trimmed();
        event.info = value.split(":")[1].trimmed();
        eventList.push_back(event);
    }

    return eventList;
}

QList<ConnectInfoStruct> CfvParser::getFieldInfo(QDomElement com)
{
    QList<ConnectInfoStruct> fieldList;

    QDomNodeList itemList = com.firstChildElement("fieldMap").firstChildElement("qhash").childNodes();
    for(int i = 0; i < itemList.size(); i++)
    {
        ConnectInfoStruct field;
        field.name = itemList.at(i).firstChildElement("pair").firstChildElement("key").text();
        QString value = itemList.at(i).firstChildElement("pair").firstChildElement("value").text();
        if(value.split(":").size() < 2)
            continue;
        field.service = value.split(":")[0].trimmed();
        field.info = value.split(":")[1].trimmed();
        fieldList.push_back(field);
    }

    return fieldList;
}

QList<ConnectInfoStruct> CfvParser::getPinInfo(QDomElement com)
{
    QList<ConnectInfoStruct> pinList;

    QDomNodeList itemList = com.firstChildElement("pinMap").firstChildElement("qhash").childNodes();
    for(int i = 0; i < itemList.size(); i++)
    {
        ConnectInfoStruct pin;
        pin.name = itemList.at(i).firstChildElement("pair").firstChildElement("key").text();
        QString value = itemList.at(i).firstChildElement("pair").firstChildElement("value").text();
        if(value.split(":").size() < 2)
            continue;
        pin.service = value.split(":")[0].trimmed();
        pin.info = value.split(":")[1].trimmed();
        pinList.push_back(pin);
    }

    return pinList;
}

Utils::FilePaths CfvParser::currentProCfvList()
{
    const ProjectExplorer::Project *pro = ProjectExplorer::ProjectTree::currentProject();
    if(pro)
    {
        Utils::FilePaths files = pro->files(matchNodesCfv);
        return files;
    }

    return Utils::FilePaths();
}

}
