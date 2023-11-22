#ifndef OPTCONFIGBUILD_H
#define OPTCONFIGBUILD_H

#include <emostools/cfvparser.h>
using namespace EmosTools;

#include "emosconfigbuild.h"

namespace EmosCodeBuild {

class EmosConfigBuild;
class OptConfigBuild
{
public:
    OptConfigBuild(EmosConfigBuild* configBuild);

    bool build(const QString &buildPath, const QString &filePath, const CfvStruct& cfvStruct, XmlBuildType type);

private:
    bool generateProcessCode(const QString &buildPath, const QString &processName);
    void generateProcessCmake(const QString& path, const QString& name);
    QString generateSingleIdl(const QString &filePath);

    const QString urlText = "url:     http://admin@10.10.7.12:8080/r/application.git\n";
    const QString branchText = "branch:  master\n";
    const QString srcText = "src:     ";

    EmosConfigBuild * m_configBuild = nullptr;
};
}
#endif // OPTCONFIGBUILD_H
