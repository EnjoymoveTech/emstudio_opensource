#include "emoscfvchecker.h"
#include <buildoutpane/buildoutlogmanager.h>

using namespace ProjectExplorer;
using namespace EmosTools;

namespace EmosCodeBuild {

EmosCfvChecker::EmosCfvChecker()
{

}

EmosCfvChecker::~EmosCfvChecker()
{

}

bool EmosCfvChecker::setData(const CfvStruct &cfv)
{
    RETURN_IFFALSE(nameRepeatCheck(cfv));
    RETURN_IFFALSE(propertyCheck(cfv));
    RETURN_IFFALSE(deviceCheck(cfv));
    RETURN_IFFALSE(processCheck(cfv));
    RETURN_IFFALSE(componentCheck(cfv));
    RETURN_IFFALSE(connectCheck(cfv));
    RETURN_IFFALSE(portRepeat(cfv));
    RETURN_IFFALSE(multiCheck(cfv));

    return true;
}

bool EmosCfvChecker::componentCheck(const EmosTools::CfvStruct &cfv)
{
    int componentSize = 0;
    for(const auto& process : cfv.processList)
    {
        for(const auto& comp : process.compList)
        {
            componentSize ++;
        }
    }

    if(cfv.compList.size() != componentSize)
    {
        BuildoutLogManager::instance()->writeWithTime(QString("Component Error: has Component not config Process"), Utils::ErrorMessageFormat);
        return false;
    }

    return true;
}

bool EmosCfvChecker::connectCheck(const EmosTools::CfvStruct &cfv)
{
    //检查quote的service是否有连线上
    for(const auto& process : cfv.processList)
    {
        for(const auto& comp : process.compList)
        {
            QMapIterator<QString, int> i(comp.idl.quoteService);
            while (i.hasNext()) {
                i.next();

                bool hasConnet = false;
                for(const auto& con : comp.connectList)
                {
                    auto comps = EmosTools::CfvParser::getCompFromOid(con.a_oid, cfv.compList);
                    if(comps.size() < 1)
                        continue;

                    IDLStruct idl = comps.first().idl;
                    QMapIterator<QString, int> s(idl.service);
                    while (s.hasNext()) {
                        s.next();

                        if(s.key() == i.key())
                        {
                            hasConnet = true;
                            break;
                        }
                    }
                }

                if(!hasConnet)
                {
                    BuildoutLogManager::instance()->writeWithTime(QString("Connect Warning: idl(%1) quote service(%2), but do not connect service")
                                                               .arg(comp.name)
                                                               .arg(i.key()),
                                                               Utils::WarningMessageFormat);
                }
            }
        }
    }

    return true;
}

bool EmosCfvChecker::processCheck(const EmosTools::CfvStruct &cfv)
{
    int processSize = 0;
    for(const auto& device : cfv.deviceList)
    {
        for(const auto& process : device.processList)
        {
            if(process.baseSrvList.size() == 0)
            {
                BuildoutLogManager::instance()->writeWithTime(QString("Process Warning: %1 base service not config").arg(process.name), Utils::WarningMessageFormat);
            }
            processSize ++;
        }
    }

    if(cfv.processList.size() != processSize)
    {
        BuildoutLogManager::instance()->writeWithTime(QString("Process Error: has Process not config Device"), Utils::ErrorMessageFormat);
        return false;
    }

    return true;
}

bool EmosCfvChecker::deviceCheck(const EmosTools::CfvStruct &cfv)
{
    for(const auto& device : cfv.deviceList)
    {
        if(device.ip == "")
        {
            BuildoutLogManager::instance()->writeWithTime("Device Warning: device ip not configuration", Utils::WarningMessageFormat);
        }
        else if(device.ip == "127.0.0.1")
        {
            BuildoutLogManager::instance()->writeWithTime(QString("Device Warning: device %1 ip can not config 127.0.0.1").arg(device.name), Utils::WarningMessageFormat);
        }
    }

    return true;
}

bool EmosCfvChecker::nameRepeatCheck(const EmosTools::CfvStruct &cfv)
{
    QStringList processList;
    QStringList deviceList;
    //device
    for(int i = 0; i < cfv.deviceList.size(); i++)
    {
        QString deviceName = cfv.deviceList.at(i).name;
        if(deviceList.contains(deviceName))
        {
            QString errLog = "Device name should be unique, there are too many devices name " + deviceName;
            BuildoutLogManager::instance()->writeWithTime("Device Error: " + errLog, Utils::ErrorMessageFormat);
            /*QMessageBox::warning(Core::ICore::mainWindow(),
                                     EmosCodeBuildExport::tr("Device error"),
                                     errLog
                                     );*/
            return false;
        }
        deviceList.push_back(deviceName);
    }
    //process
    for(int i = 0; i < cfv.processList.size(); i++)
    {
        QString processName = cfv.processList.at(i).name;
        if(processList.contains(processName))
        {
            QString errLog = "Process name should be unique, there are too many processes name " + processName;
            BuildoutLogManager::instance()->writeWithTime("Process Error: " + errLog, Utils::ErrorMessageFormat);
            return false;
        }
        processList.push_back(processName);
    }
    //alias
    QStringList aliasList;
    for(int i = 0; i < cfv.processList.size(); i++)
    {
        for(int j = 0; j < cfv.processList[i].compList.size(); j++)
        {
            QString aliasName = cfv.processList[i].compList.at(j).name;
            if(aliasList.contains(aliasName))
            {
                QString errLog = "component's name should be unique, there are too many component name " + aliasName + " in " + cfv.processList[i].name;
                BuildoutLogManager::instance()->writeWithTime("Names Error: " + errLog, Utils::ErrorMessageFormat);
                return false;
            }
            aliasList.push_back(aliasName);
        }
    }
    return true;
}

bool EmosCfvChecker::propertyCheck(const EmosTools::CfvStruct &cfv)
{
    for(int i = 0; i < cfv.compList.size(); i++)
    {
        QStringList propertyNameList;
        QList<CustomPropertyStruct> propertyList = cfv.compList.at(i).property;
        for(int j = 0; j < propertyList.size(); j++)
        {
            //check name repeat
            QString propertyName = propertyList.at(j).key;
            if(propertyNameList.contains(propertyName))
            {
                QString errLog = "Property's name should be unique, there are too many properties name " + propertyName;
                BuildoutLogManager::instance()->writeWithTime("Property name Error: " + errLog, Utils::ErrorMessageFormat);
                return false;
            }
            else
            {
                propertyNameList.push_back(propertyName);
            }

            //check value is empty
            QString propertyType = propertyList.at(j).type;
            QString propertyValue = propertyList.at(j).value;
            if(propertyType == "String")
            {
                   ;
            }
            else
            {
                if(propertyValue == "")
                {
                    QString errLog = "Property " + propertyName + " contents cannot be empty";
                    BuildoutLogManager::instance()->writeWithTime("Property value Error: " + errLog, Utils::ErrorMessageFormat);
                    return false;
                }
                //check type-value match
                if(propertyType == "Int")
                {
                    for(auto p: propertyValue)
                    {
                        if(p < '0' || p > '9')
                        {
                            QString errLog = "Property " + propertyName + " contents only allow numbers, but there appear other characters";
                            BuildoutLogManager::instance()->writeWithTime("Property value Error: " + errLog, Utils::ErrorMessageFormat);
                            return false;
                        }
                    }
                }
                else if(propertyType == "Bool")
                {
                    if(propertyValue == "true" || propertyValue == "false")
                    {
                        ;
                    }
                    else
                    {
                        QString errLog = "Property " + propertyName + " contents only allow true or false, but there appear other value";
                        BuildoutLogManager::instance()->writeWithTime("Property value Error: " + errLog, Utils::ErrorMessageFormat);
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

bool EmosCfvChecker::portRepeat(const EmosTools::CfvStruct &cfv)
{
    QSet<QString> ipList;
    QSet<int> ecuID;
    for(const auto& device : cfv.deviceList)
    {
        QHash<int, QString> portHash;

        if(ipList.contains(device.ip))
        {
            QString warnLog = "Ip Warning: " + device.name + "'s ipAddress is same";
            BuildoutLogManager::instance()->writeWithTime(warnLog, Utils::WarningMessageFormat);
        }
        ipList.insert(device.ip);

        if(ecuID.contains(device.ecuID))
        {
            QString warnLog = "EcuID Error: " + device.name + "'s ecuID is same";
            BuildoutLogManager::instance()->writeWithTime(warnLog, Utils::ErrorMessageFormat);
            return false;
        }
        ecuID.insert(device.ecuID);

        bool hasSd = false;
        for(int k = 0; k < device.processList.count(); k++)
        {
            ProcessStruct process = device.processList.at(k);

            QMapIterator<QString, BaseSrvStruct> baseSrv(process.baseSrvList);
            while (baseSrv.hasNext()) {
                baseSrv.next();

                if(baseSrv.key() == "ServiceDiscovery")
                    hasSd = true;

                for(const auto& property : baseSrv.value().property)
                {
                    if(property.key.indexOf("Port") >= 0)
                    {
                        if(portHash.contains(property.value.toInt()))
                        {
                            QString warnLog = "Port Error: " + device.processList.at(k).name + " has the same nSdPort as " + portHash.value(property.value.toInt()) + "! 1.Click " + device.processList.at(k).name + "; 2.Check the nSdPort's value in the window at the lower right corner.";
                            BuildoutLogManager::instance()->writeWithTime(warnLog, Utils::ErrorMessageFormat);
                            return false;
                        }
                        portHash.insert(property.value.toInt(), device.processList.at(k).name);
                        break;
                    }
                }
            }

            if(!hasSd)//没有配置sd的进行不需要检测端口
                continue;

            for(int i = 0; i < process.compList.size(); i++)
            {
                ComponentStruct comp = process.compList.at(i);
                for(int j = 0; j < comp.service.size(); j++)
                {
                    SeriveInfoStruct service = comp.service.at(j);
                    if(portHash.contains(service.port))
                    {
                        QString warnLog = "Port Error: " + service.name + "(service) has the same port as " + portHash.value(service.port);
                        BuildoutLogManager::instance()->writeWithTime(warnLog, Utils::ErrorMessageFormat);
                        return false;
                    }
                    portHash.insert(service.port, service.name);
                }
                QMap<QString, PinDataStruct>::iterator iter = comp.pinList.begin();
                while(iter != comp.pinList.end())
                {
                    if(portHash.contains(iter->port.toInt()))
                    {
                        QString warnLog = "Port Error: " + iter->name + "(pin) has the same port as " + portHash.value(iter->port.toInt());
                        BuildoutLogManager::instance()->writeWithTime(warnLog, Utils::ErrorMessageFormat);
                        return false;
                    }
                    portHash.insert(iter->port.toInt(), iter->name);
                    iter++;
                }
            }
        }
    }

    return true;
}

bool EmosCfvChecker::multiCheck(const EmosTools::CfvStruct &cfv)
{
    QString multiIp = "";
    QString defaultName;
    for(int i = 0; i < cfv.processList.size(); i++)
    {
        QString multiIp_temp = cfv.processList.at(i).multiIp;
        if(multiIp == "")
        {
            multiIp = multiIp_temp;
            defaultName = cfv.processList.at(i).name;
        }
        else
        {
            if(multiIp_temp != multiIp)
            {
                QString warnLog = "MultiIP Warning: " + cfv.processList.at(i).name + "'s multiIp is different from " + defaultName;
                BuildoutLogManager::instance()->writeWithTime(warnLog, Utils::WarningMessageFormat);
            }
        }
    }
    return true;
}

}
