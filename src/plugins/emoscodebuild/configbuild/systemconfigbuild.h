#ifndef SYSTEMCONFIGBUILD_H
#define SYSTEMCONFIGBUILD_H

#include <emostools/cfvparser.h>
using namespace EmosTools;
#include "emosconfigbuild.h"

namespace EmosCodeBuild {

class EmosConfigBuild;
class SystemConfigBuild
{
public:
    SystemConfigBuild(EmosConfigBuild* configBuild);

    bool build(const QString &buildPath, const QString &filePath, const CfvStruct& cfvStruct, XmlBuildType type);

private:
    QList<ComponentStruct> sortComp(const QList<ComponentStruct>& compList);
    void addCompNodeToSys(const CfvStruct& cfvStruct, const ProcessStruct& process, const ComponentStruct& comp, const DeviceStruct& device, QDomElement fatherelement, bool isService, bool isProxy);
    void addConnectNodeToSys(const CfvStruct& cfvStruct, const ConnectStruct& con, const ComponentStruct& compB, const ComponentStruct& compA, QDomElement settings, QSet<QString> connectUid, bool sameProcess);
    void addCustomProperty(const ComponentStruct& comp, QDomElement settings);

    void addBaseSrvProcessNodeToSys(QDomElement fatherelement, const ProcessStruct& comp);
    void addProxy(const SeriveInfoStruct &s, const CfvStruct& cfvStruct, QDomElement& settings);
    void addLocalInfo(const ComponentStruct& comp, const CfvStruct& cfvStruct, QDomElement settings, const ProcessStruct& process);

    QDomDocument doc;
    QMap<QString,QDomElement> m_ServiceSettings;
    QMap<QString,int> m_ServiceInstance;

    EmosConfigBuild * m_configBuild = nullptr;
};
}
#endif // OPTCONFIGBUILD_H
