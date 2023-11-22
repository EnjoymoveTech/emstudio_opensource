#include "systemconfigbuild.h"
#include "emosconfigbuild.h"
#include <emostools/srv_parser.h>
#include "emostools/api_description_parser.h"
#include <utils/algorithm.h>
#include <buildoutpane/buildoutlogmanager.h>

namespace EmosCodeBuild {

SystemConfigBuild::SystemConfigBuild(EmosConfigBuild* configBuild) : m_configBuild(configBuild)
{

}

bool SystemConfigBuild::build(const QString &buildPath, const QString &filePath, const CfvStruct& cfvStruct, XmlBuildType type)
{
    doc.clear();
    m_ServiceSettings.clear();
    m_ServiceInstance.clear();

    QDomElement rootnode =  doc.createElement("emos:project");
    rootnode.setAttribute("xmlns:emos","emos");
    doc.appendChild(rootnode);

    QDomElement configurations = doc.createElement("configurations");
    rootnode.appendChild(configurations);

    int onlyid = 0;
    int device_onlyid = 0;
    int process_onlyid = 0;
    for(const auto& device : cfvStruct.deviceList)
    {
        device_onlyid++;
        SrvParser::getInstance()->setInput("deviceOnlyid",QString::number(device_onlyid));
        SrvParser::getInstance()->setInput("ip",device.ip);
        SrvParser::getInstance()->setInput("ecuid",QString::number(device.ecuID));

        for(const auto& process : device.processList)
        {
            QSet<QString> connectUid; //同一进程内多个客户端引用同一个服务端,只写一段proxy
            QDomElement configuration = doc.createElement("configuration");
            configuration.setAttribute("description", "The master for the message bus.");
            configuration.setAttribute("url", "udp://localhost:10001");
            configuration.setAttribute("name", process.name);
            configurations.appendChild(configuration);

            QDomElement graph = doc.createElement("graph");
            configuration.appendChild(graph);

            process_onlyid++;
            SrvParser::getInstance()->setInput("processOnlyid",QString::number(process_onlyid));
            SrvParser::getInstance()->setInput("processName",process.name);

            addBaseSrvProcessNodeToSys(graph,process);

            QList<ComponentStruct> compList = sortComp(process.compList);//按优先级排序
            for(const auto& comp : compList)
            {
                onlyid++;
                SrvParser::getInstance()->setInput("onlyid",QString::number(onlyid));

                if(comp.oid == "Studio")//studio 添加prox comp
                {
                    for(const auto& con : comp.connectList)
                    {
                        ComponentStruct compA = CfvParser::getCompFromUid(con.a_uid, cfvStruct.compList);
                        if(connectUid.contains(compA.uid))
                            continue;

                        addCompNodeToSys(cfvStruct, process, compA, device, graph, false, true);
                        connectUid.insert(compA.uid);
                    }
                }

                if(!comp.idl.isValid)
                {
                    EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime(QString("idl:%1(%2) not exist").arg(comp.name).arg(comp.oid), Utils::ErrorMessageFormat);
                    return false;
                }

                addCompNodeToSys(cfvStruct, process, comp, device, graph, comp.idl.isService, false);

                if(comp.idl.isClient) //添加prox comp
                {
                    for(const auto& con : comp.connectList)
                    {
                        ComponentStruct compA = CfvParser::getCompFromUid(con.a_uid, cfvStruct.compList);
                        if(connectUid.contains(compA.uid))
                            continue;

                        //如果连线是在同一进程，不需要proxy
                        if(process.name == CfvParser::getProcess(compA, cfvStruct.processList).name)
                            continue;

                        //服务comp所在设备
                        addCompNodeToSys(cfvStruct, process, compA, CfvParser::getDevice(compA, cfvStruct.deviceList), graph, false, true);
                        //addLocalInfo(compA, cfvStruct, graph);
                        connectUid.insert(compA.uid);
                    }
                }
            }


        }
    }

    RETURN_IFFALSE(EmosConfigBuild::writeFile(buildPath + "/system.xml", buildPath + "/../../build_out/neo/system.xml", &doc));

    return true;
}

QList<ComponentStruct> SystemConfigBuild::sortComp(const QList<ComponentStruct> &compList)
{
    QList<ComponentStruct> sortCompList = compList;

    Utils::sort(sortCompList, [](const ComponentStruct& comp1, const ComponentStruct& comp2) {
        return comp1.idl.priority > comp2.idl.priority;
    });

    return sortCompList;
}

void SystemConfigBuild::addCompNodeToSys(const CfvStruct& cfvStruct, const ProcessStruct& process, const ComponentStruct& comp, const DeviceStruct& device, QDomElement fatherelement, bool isService, bool isProxy)
{
    QDomElement compnode = doc.createElement("comp");
    fatherelement.appendChild(compnode);

    /*    if(isService)
    {
        compnode.setAttribute("alias", comp.name + "skel");//使用实例名
        compnode.setAttribute("idref", "oid." + comp.oid.toLower() + "skel");
    }
    else */if(isProxy)
    {
        compnode.setAttribute("alias", comp.name + "prox");//使用实例名
        compnode.setAttribute("idref", "oid." + comp.oid.toLower() + "prox");
    }
    else
    {
        compnode.setAttribute("alias", comp.name.toLower());
        compnode.setAttribute("idref", "oid." + comp.oid.toLower());
    }
    compnode.setAttribute("priority","2");

    //记录Service setting, proxy直接 copy
    if(m_ServiceSettings.contains(comp.idl.name + comp.name))
    {
        QDomElement copyElement = m_ServiceSettings.value(comp.idl.name + comp.name).cloneNode().toElement();
        compnode.appendChild(copyElement);

        //检测property
        QDomNodeList propList = copyElement.elementsByTagName("property");
        for(int index = 0; index < propList.size(); index++)
        {
            for(const auto& s : comp.service)
            {
                if(propList.at(index).toElement().attribute("name") == s.name + "alias")
                {
                    if(isService)
                        propList.at(index).toElement().setAttribute("value", comp.name);
                    else
                        propList.at(index).toElement().setAttribute("value", comp.name + "prox");
                }
            }
        }

        addLocalInfo(comp, cfvStruct, copyElement, process);
    }
    else
    {
        QDomElement settings = doc.createElement("settings");
        compnode.appendChild(settings);

        QDomElement prop;
        QMapIterator<QString, int> s(comp.idl.service);
        while(s.hasNext())
        {
            s.next();

            prop = doc.createElement("property");
            prop.setAttribute("type","Int");
            prop.setAttribute("name","n"+ s.key() + "InstanceID");

            //服务实例不能相同
            if(m_ServiceInstance.contains(s.key()))
                m_ServiceInstance.insert(s.key(), m_ServiceInstance.value(s.key()) + 1);
            else
                m_ServiceInstance.insert(s.key(), 1);

            prop.setAttribute("value",m_ServiceInstance.value(s.key()));

            settings.appendChild(prop);
        }

        if(isService || isProxy)
        {
            bool onceSetAddress = false;
            foreach (const QString& pinName, comp.pinList.keys())
            {
                prop = doc.createElement("property");
                prop.setAttribute("type","Int");
                prop.setAttribute("name","n" + comp.pinList.value(pinName).serviceName + comp.pinList.value(pinName).name + "Port");
                prop.setAttribute("value", comp.pinList.value(pinName).port);
                settings.appendChild(prop);
            }
            for(const auto& s : comp.service)
            {
                //记录引用服务
                prop = doc.createElement("property");
                prop.setAttribute("type", "String");
                prop.setAttribute("name", s.name + "alias");
                if(isService)
                    prop.setAttribute("value", comp.name);
                else
                    prop.setAttribute("value", comp.name + "prox");
                settings.appendChild(prop);

                if(!onceSetAddress)
                {
                    prop = doc.createElement("property");
                    prop.setAttribute("type", "String");
                    prop.setAttribute("name", "strAddress");
                    prop.setAttribute("value", device.ip == "" ? "127.0.0.1" : device.ip);//使用process配置
                    settings.appendChild(prop);
                    onceSetAddress = true;
                }

                prop = doc.createElement("property");
                prop.setAttribute("type","Int");
                prop.setAttribute("name","n" + s.name + "Port");
                prop.setAttribute("value", s.port);
                settings.appendChild(prop);

                prop = doc.createElement("property");
                prop.setAttribute("type","Int");
                prop.setAttribute("name","n" + s.name + "Protocol");
                prop.setAttribute("value", s.protocol);
                settings.appendChild(prop);

                prop = doc.createElement("property");
                prop.setAttribute("type","Int");
                prop.setAttribute("name","n" + s.name + "Type");
                prop.setAttribute("value", s.type);
                settings.appendChild(prop);

                //IPC : 需要设置唯一标识  nServiceIpcKey : component实例名
                if(s.protocol == 4 && s.type == 4)
                {
                    prop = doc.createElement("property");
                    prop.setAttribute("type","String");
                    prop.setAttribute("name","str" + s.name + "IpcKey");
                    prop.setAttribute("value", s.name);
                    settings.appendChild(prop);
                }

                addProxy(s, cfvStruct, settings);
                addLocalInfo(comp, cfvStruct, settings, process);
            }

            addCustomProperty(comp, settings);

            m_ServiceSettings.insert(comp.idl.name + comp.name, settings);
        }
        else
        {
            addCustomProperty(comp, settings);
        }

        //当前进程是否有使用sd
        prop = doc.createElement("property");
        prop.setAttribute("type","Bool");
        prop.setAttribute("name","bHasSD");
        if(CfvParser::getProcess(comp,cfvStruct.processList).hasService("ServiceDiscovery"))
            prop.setAttribute("value","true");
        else
            prop.setAttribute("value","false");
        settings.appendChild(prop);

        //检测连线
        QSet<QString> connectUid;
        for(const auto& con : comp.connectList)
        {
            ComponentStruct compA = CfvParser::getCompFromUid(con.a_uid, cfvStruct.compList);

            addConnectNodeToSys(cfvStruct, con, comp, compA, settings, connectUid, CfvParser::getProcess(comp,cfvStruct.processList).name == CfvParser::getProcess(compA,cfvStruct.processList).name ? true:false);
            connectUid.insert(compA.uid);
        }
    }
}

void SystemConfigBuild::addConnectNodeToSys(const CfvStruct& cfvStruct, const ConnectStruct& con, const ComponentStruct& compB, const ComponentStruct& compA, QDomElement settings, QSet<QString> connectUid, bool sameProcess)
{
    EmosTools::ApiParser* api = EmosTools::ApiParser::getInstance();
    if(con.type == EVENT)
    {
        for(const auto& event : con.event)
        {
            //event id
            QDomElement propService = doc.createElement("property");
            propService.setAttribute("type", "Int");
            propService.setAttribute("name", event.name + "_QuoteService");

            QDomElement propEvent = doc.createElement("property");
            propEvent.setAttribute("type","Int");
            propEvent.setAttribute("name", event.name + "_QuoteEvent");

            QString srvServiceName = event.service;
            QString srvEventName = event.info;

            auto comps = EmosTools::CfvParser::getCompFromOid(con.a_oid, cfvStruct.compList);
            if(comps.size() < 1)
                continue;

            bool findEvent = false;
            QMapIterator<QString, int> it(comps.first().idl.service);
            while (it.hasNext()) {
                it.next();

                if(it.key() == srvServiceName)
                {
                    EmosTools::tSrvService srv = api->getService(srvServiceName);
                    if (srv.id == -1)
                    {
                        continue;
                    }
                    propService.setAttribute("value", srv.id);
                    foreach (const EmosTools::tSrvEvent& event,srv.events)
                    {
                        if(event.name == srvEventName)
                        {
                            propEvent.setAttribute("value", event.id);
                            findEvent = true;
                        }
                    }
                    break;
                }
            }
            if(findEvent)//只有找到了对应服务里的event才写入xml
            {
                settings.appendChild(propService);
                settings.appendChild(propEvent);
            }
        }
    }
    else if(con.type == FIELD)
    {
        for(const auto& field : con.field)
        {
            //event id
            QDomElement propService = doc.createElement("property");
            propService.setAttribute("type", "Int");
            propService.setAttribute("name", field.name + "_QuoteService");

            QDomElement propField = doc.createElement("property");
            propField.setAttribute("type","Int");
            propField.setAttribute("name", field.name + "_QuoteField");

            QString srvServiceName = field.service;
            QString srvFieldName = field.info;

            auto comps = EmosTools::CfvParser::getCompFromOid(con.a_oid, cfvStruct.compList);
            if(comps.size() < 1)
                continue;

            bool findField = false;
            QMapIterator<QString, int> it(comps.first().idl.service);
            while (it.hasNext()) {
                it.next();

                if(it.key() == srvServiceName)
                {
                    EmosTools::tSrvService srv = api->getService(srvServiceName);
                    if (srv.id == -1)
                    {
                        continue;
                    }
                    propService.setAttribute("value", srv.id);
                    foreach (const EmosTools::tSrvField& field,srv.fields)
                    {
                        if(field.name == srvFieldName)
                        {
                            propField.setAttribute("value", field.id);
                            findField = true;
                        }
                    }
                    break;
                }
            }
            if(findField)//只有找到了对应服务里的field才写入xml
            {
                settings.appendChild(propService);
                settings.appendChild(propField);
            }
        }
    }
    else if(con.type == PIN)
    {
        for(const auto& pin : con.pin)
        {
            //event id
            QDomElement propService = doc.createElement("property");
            propService.setAttribute("type", "Int");
            propService.setAttribute("name", pin.name + "_QuoteService");

            QDomElement propField = doc.createElement("property");
            propField.setAttribute("type","Int");
            propField.setAttribute("name", pin.name + "_QuotePin");

            QString srvServiceName = pin.service;
            QString srvPinName = pin.info;

            auto comps = EmosTools::CfvParser::getCompFromOid(con.a_oid, cfvStruct.compList);
            if(comps.size() < 1)
                continue;

            bool findPin = false;
            QMapIterator<QString, int> it(comps.first().idl.service);
            while (it.hasNext()) {
                it.next();

                if(it.key() == srvServiceName)
                {
                    EmosTools::tSrvService srv = api->getService(srvServiceName);
                    if (srv.id == -1)
                    {
                        continue;
                    }
                    propService.setAttribute("value", srv.id);
                    foreach (const EmosTools::tSrvPin& pin, srv.pins)
                    {
                        if(pin.name == srvPinName)
                        {
                            propField.setAttribute("value", pin.id);
                            findPin = true;
                        }
                    }
                    break;
                }
            }
            if(findPin)//只有找到了对应服务里的pin才写入xml
            {
                settings.appendChild(propService);
                settings.appendChild(propField);
            }
        }
    }

    //记录引用服务
    if(connectUid.contains(compA.uid))//只引用一次
        return;

    QMapIterator<QString, int> compASrvIt(compA.idl.service);
    while(compASrvIt.hasNext())
    {
        compASrvIt.next();

        QDomElement propSkel = doc.createElement("property");
        QDomElement propProxy = doc.createElement("property");

        propSkel.setAttribute("type", "String");
        propProxy.setAttribute("type", "String");

        if(sameProcess)
        {
            propSkel.setAttribute("name", compASrvIt.key() + "alias");
            propSkel.setAttribute("value", compA.name);
            settings.appendChild(propSkel);
        }
        else
        {
            propProxy.setAttribute("name", compASrvIt.key() + "alias");
            propProxy.setAttribute("value", compA.name + "prox");
            settings.appendChild(propProxy);
        }
    }
}

void SystemConfigBuild::addCustomProperty(const ComponentStruct& comp, QDomElement settings)
{
    QMapIterator<QString, QString> i(comp.idl.property);
    while (i.hasNext())
    {
        i.next();

        QDomElement propCustom = doc.createElement("property");
        bool hasSet = false;
        for(const auto& pro : comp.property)
        {
            if(pro.key == i.key())
            {
                propCustom.setAttribute("type", pro.type);
                propCustom.setAttribute("name", pro.key);
                propCustom.setAttribute("value", pro.value);
                settings.appendChild(propCustom);
                hasSet = true;
                break;
            }
        }
        if(!hasSet)
        {
            propCustom.setAttribute("type",i.value());
            propCustom.setAttribute("name",i.key());
            propCustom.setAttribute("value", "");
            settings.appendChild(propCustom);
        }
    }
}

void SystemConfigBuild::addBaseSrvProcessNodeToSys(QDomElement fatherelement, const ProcessStruct &comp)
{
    QMapIterator<QString, EmosTools::tSrvFixedModule> itmodule(EmosTools::SrvParser::getInstance()->getModules());
    while (itmodule.hasNext()) {
        itmodule.next();

        if(!itmodule.value().show)
            continue;

        if(comp.baseSrvList.contains(itmodule.value().moduleName))
        {
            BaseSrvStruct baseSrv = comp.baseSrvList.value(itmodule.value().moduleName);

            QDomElement compnode = doc.createElement("comp");
            fatherelement.appendChild(compnode);

            compnode.setAttribute("alias", itmodule.value().alias);
            compnode.setAttribute("idref", itmodule.value().oid);

            compnode.setAttribute("priority", "2");

            QDomElement settings = doc.createElement("settings");
            compnode.appendChild(settings);

            QDomElement prop;
            QMapIterator<QString, EmosTools::tSrvProperty> property(itmodule.value().propertyMap);
            while (property.hasNext()) {
                property.next();

                prop = doc.createElement("property");
                prop.setAttribute("type", property.value().type);
                prop.setAttribute("name", property.value().propertyName);

                if(property.value().isFixed)
                {
                    prop.setAttribute("value", property.value().generateValue(property.value().value));
                }
                else
                {
                    for(const auto& pro : baseSrv.property)
                    {
                        if(pro.key == property.value().propertyName)
                        {
                            prop.setAttribute("value", pro.value);
                            break;
                        }
                    }
                }
                settings.appendChild(prop);
            }
        }
    }
}

void SystemConfigBuild::addProxy(const SeriveInfoStruct &s, const CfvStruct &cfvStruct, QDomElement &settings)
{
    int proxNum = 0;
    QList<tSrvService> serviceList = ApiParser::getInstance()->getServices();
    QDomElement prop;
    for(auto& device : cfvStruct.deviceList)
    {
        QString ip = device.ip;
        for(auto & process : device.processList)
        {
            StaticInfoStruct staticInfo = process.staticInfo;
            if(!staticInfo.selected)
                continue;
            for(auto & proxy : staticInfo.staticInfoList)
            {
                if(proxy.serviceName != s.name)
                    continue;
                prop = doc.createElement("property");
                settings.appendChild(prop);
                prop.setAttribute("name", "strStaticProx" + s.name + QString::number(proxNum) + "Address");
                prop.setAttribute("type", "String");
                prop.setAttribute("value",ip);

                prop = doc.createElement("property");
                settings.appendChild(prop);
                prop.setAttribute("name", "nStaticProx" + s.name + QString::number(proxNum) + "Port");
                prop.setAttribute("type", "Int");
                prop.setAttribute("value", proxy.localPort);

                for(auto & service : serviceList)
                {
                    QList<int> groupNum;
                    if(service.name != s.name)
                        continue;
                    int groupIDIndex = 0;
                    for(auto & eventName : proxy.eventNameList)
                    {
                        for(auto & event : service.events)
                        {
                            if(event.name != eventName)
                                continue;
                            if(groupNum.contains(event.groupid))
                                continue;
                            prop = doc.createElement("property");
                            settings.appendChild(prop);
                            prop.setAttribute("name", "nStaticProx" + s.name + QString::number(proxNum)+ "ProxGroupID" + QString::number(groupIDIndex++));
                            prop.setAttribute("type", "Int");
                            prop.setAttribute("value", event.groupid);
                            groupNum.append(event.groupid);
                        }
                        for(auto & pin : service.pins)
                        {
                            if(pin.name != eventName)
                                continue;
                            if(groupNum.contains(pin.id))
                                continue;
                            prop = doc.createElement("property");
                            settings.appendChild(prop);
                            prop.setAttribute("name", "nStaticProx" + s.name + QString::number(proxNum)+ "ProxGroupID" + QString::number(groupIDIndex++));
                            prop.setAttribute("type", "Int");
                            prop.setAttribute("value", pin.id);
                            groupNum.append(pin.id);
                        }
                    }
                    prop = doc.createElement("property");
                    settings.appendChild(prop);
                    prop.setAttribute("name", "nStaticProx" + s.name + QString::number(proxNum)+"ProxGroupNum");
                    prop.setAttribute("type", "Int");
                    prop.setAttribute("value", QString::number(groupNum.size()));
                    proxNum++;
                }
            }
        }
    }
    prop = doc.createElement("property");
    settings.appendChild(prop);
    prop.setAttribute("name", "n" + s.name + "ProxNum");
    prop.setAttribute("type", "Int");
    prop.setAttribute("value", QString::number(proxNum));
}

void SystemConfigBuild::addLocalInfo(const ComponentStruct& comp, const CfvStruct& cfvStruct, QDomElement settings, const ProcessStruct& process)
{
    QDomElement prop;
    QString name = settings.tagName();
    for(auto & dev:cfvStruct.deviceList)
    {
        for(auto & pro : dev.processList)
        {
            if(pro.name != process.name)
                continue;
            for(auto & proxyInfo : process.staticInfo.staticInfoList)
            {
                for(auto & service : comp.service)
                {
                    if(service.name != proxyInfo.serviceName)
                        continue;
                    QDomNodeList propList = settings.elementsByTagName("property");
                    bool findFlag = false;
                    for(int index = 0; index < propList.size(); index++)
                    {
                        if(propList.at(index).toElement().attribute("name") == "strLocalAddress")
                        {
                            findFlag = true;
                            propList.at(index).toElement().setAttribute("value", dev.ip);
                        }
                        if(propList.at(index).toElement().attribute("name") == "nStaticProxLocalPort")
                            propList.at(index).toElement().setAttribute("value", proxyInfo.localPort);
                        if(propList.at(index).toElement().attribute("name") == "bIsStatic")
                            propList.at(index).toElement().setAttribute("value", process.staticInfo.selected?QString("true"):QString("false"));
                    }

                    if(!findFlag)
                    {
                        prop = doc.createElement("property");
                        settings.appendChild(prop);
                        prop.setAttribute("name", "strLocalAddress");
                        prop.setAttribute("type", "String");
                        prop.setAttribute("value", dev.ip);

                        prop = doc.createElement("property");
                        settings.appendChild(prop);
                        prop.setAttribute("name", "nStaticProxLocalPort");
                        prop.setAttribute("type", "Int");
                        prop.setAttribute("value", proxyInfo.localPort);

                        prop = doc.createElement("property");
                        settings.appendChild(prop);
                        prop.setAttribute("name", "bIsStatic");
                        prop.setAttribute("type", "Bool");
                        prop.setAttribute("value", process.staticInfo.selected?QString("true"):QString("false"));
                    }
                    break;
                }
            }
        }
    }
}
}
