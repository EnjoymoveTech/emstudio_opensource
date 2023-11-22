#include "globalsconfigbuild.h"
#include "emosconfigbuild.h"

#include <emostools/srv_parser.h>
#include <buildoutpane/buildoutlogmanager.h>

namespace EmosCodeBuild {

GlobalsConfigBuild::GlobalsConfigBuild(EmosConfigBuild* configBuild) : m_configBuild(configBuild)
{

}

bool GlobalsConfigBuild::build(const QString &buildPath, const QString &filePath, const CfvStruct& cfvStruct, XmlBuildType type)
{
    QDomDocument doc;
    QDomElement rootnode =  doc.createElement("emos:configuration");
    rootnode.setAttribute("xmlns:emos","emos");
    doc.appendChild(rootnode);

    QDomElement pluginsnode =  doc.createElement("plugins");
    rootnode.appendChild(pluginsnode);

    QSet<QString> scrPivot;

    for(const auto& device : cfvStruct.deviceList)
    {
        for(const auto& process : device.processList)
        {
            //基础服务
            QMapIterator<QString, EmosTools::tSrvFixedModule> itmodule(EmosTools::SrvParser::getInstance()->getModules());
            while (itmodule.hasNext()) {
                itmodule.next();

                if(!itmodule.value().show)
                    continue;

                if(process.baseSrvList.contains(itmodule.value().moduleName) &&
                        !itmodule.value().comp.isEmpty())
                {
                    if(scrPivot.contains(itmodule.value().oid))
                        continue;

                    scrPivot.insert(itmodule.value().oid);

                    QDomElement baseSrvNode = doc.createElement("plugin");
                    baseSrvNode.setAttribute("optional","true");
                    baseSrvNode.setAttribute("url", itmodule.value().comp);
                    pluginsnode.appendChild(baseSrvNode);
                }
            }

            for(const auto& comp : process.compList)
            {
                if(!comp.idl.isValid)
                {
                    EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime(QString("idl:%1(%2) not exist").arg(comp.name).arg(comp.oid), Utils::ErrorMessageFormat);
                    return false;
                }

                if(scrPivot.contains(comp.oid))
                    continue;

                scrPivot.insert(comp.oid);

                QDomElement compnode = doc.createElement("plugin");
                compnode.setAttribute("optional","true");
                compnode.setAttribute("url",comp.idl.name.toLower()+".comp");
                pluginsnode.appendChild(compnode);

                for(const auto& c : comp.connectList)
                {
                    ComponentStruct CompA = CfvParser::getCompFromUid(c.a_uid, cfvStruct.compList);

                    if(CfvParser::getProcess(CompA, cfvStruct.processList).name !=
                       CfvParser::getProcess(comp, cfvStruct.processList).name)
                    {
                        if(scrPivot.contains(CompA.idl.name + "proxy"))
                            continue;

                        QDomElement compnodeProxy = doc.createElement("plugin");
                        compnodeProxy.setAttribute("optional","true");
                        compnodeProxy.setAttribute("url",CompA.idl.name.toLower()+"proxy.comp");
                        pluginsnode.appendChild(compnodeProxy);

                        scrPivot.insert(CompA.idl.name + "proxy");
                    }
                }
            }
        }
    }

    RETURN_IFFALSE(EmosConfigBuild::writeFile(buildPath + "/globals.xml", buildPath + "/../../build_out/neo/globals.xml", &doc));

    for(const auto& device : cfvStruct.deviceList)
    {
        for(const auto& process : device.processList)
        {
            QDomDocument globals_process = CfvParser::SplitGlobalsForProcess(doc, process, cfvStruct);
            RETURN_IFFALSE(EmosConfigBuild::writeFile(buildPath + "/globals_" + process.name + ".xml", buildPath + "/../../build_out/neo/globals_" +  process.name  + ".xml", &globals_process));
        }
        QDomDocument globals_device = CfvParser::SplitGlobalsForDevice(doc, device, cfvStruct);
        RETURN_IFFALSE(EmosConfigBuild::writeFile(buildPath + "/globals_device_" + device.name + ".xml", buildPath + "/../../build_out/neo/globals_device_" +  device.name  + ".xml", &globals_device));
    }
    return true;
}

}
