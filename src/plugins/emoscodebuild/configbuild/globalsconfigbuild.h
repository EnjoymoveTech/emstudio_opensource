#ifndef GLOBALSGBUILD_H
#define GLOBALSGBUILD_H

#include <emostools/cfvparser.h>
using namespace EmosTools;

#include "emosconfigbuild.h"

namespace EmosCodeBuild {

class EmosConfigBuild;
class GlobalsConfigBuild
{
public:
    GlobalsConfigBuild(EmosConfigBuild* configBuild);

    bool build(const QString &buildPath, const QString &filePath, const CfvStruct& cfvStruct, XmlBuildType type);
private:
    EmosConfigBuild * m_configBuild = nullptr;
};
}
#endif // OPTCONFIGBUILD_H
