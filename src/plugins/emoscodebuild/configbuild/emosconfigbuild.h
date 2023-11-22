#ifndef EMOSCONFIGBUILD_H
#define EMOSCONFIGBUILD_H

#define RETURN_IFFALSE(result) if(result == false) return false;

#include <QtXml/QDomDocument>
#include <explorer/buildstep.h>

enum XmlBuildType{
    Build_System = 1,
    Build_Global = 2,
    Build_OPT = 4,
};

namespace EmosTools{ struct CfvStruct; }


namespace EmosCodeBuild {
class GlobalsConfigBuild;
class OptConfigBuild;
class SystemConfigBuild;
class EmosConfigBuild
{
public:
    EmosConfigBuild();
    ~EmosConfigBuild();
    bool build(XmlBuildType type, const QString &buildPath, const EmosTools::CfvStruct& cfv);

    static bool writeFile(const QString& filePath, const QString& copyFilePath, const QString& context);
    static bool writeFile(const QString& filePath, const QString& copyFilePath, QDomDocument* doc);
    static bool writeFile(const QString& filePath, QDomDocument* doc);
private:
    QString m_buildPath;

    GlobalsConfigBuild* m_globalsBuild;
    OptConfigBuild* m_optBuild;
    SystemConfigBuild* m_systemBuild;
};
}
#endif // EMOSXMLBUILD_H
