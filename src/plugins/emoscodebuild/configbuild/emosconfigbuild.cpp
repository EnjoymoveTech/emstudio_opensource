#include "emosconfigbuild.h"
#include <emostools/cfvparser.h>
#include <emostools/srv_parser.h>

#include "globalsconfigbuild.h"
#include "optconfigbuild.h"
#include "systemconfigbuild.h"

using namespace EmosTools;

namespace EmosCodeBuild {

EmosConfigBuild::EmosConfigBuild():
    m_globalsBuild(new GlobalsConfigBuild(this)),
    m_optBuild(new OptConfigBuild(this)),
    m_systemBuild(new SystemConfigBuild(this))
{

}

EmosConfigBuild::~EmosConfigBuild()
{
    delete m_globalsBuild;m_globalsBuild = nullptr;
    delete m_optBuild;m_optBuild = nullptr;
    delete m_systemBuild;m_systemBuild = nullptr;
}

bool EmosConfigBuild::build(XmlBuildType type, const QString &buildPath, const EmosTools::CfvStruct& cfv)
{
    SrvParser::getInstance()->reloadXml();
    SrvParser::getInstance()->setInput("buildPath",buildPath);
    SrvParser::getInstance()->setInput("onlyid","1");
    SrvParser::getInstance()->setInput("processOnlyid","1");
    SrvParser::getInstance()->setInput("processName","Master");
    SrvParser::getInstance()->setInput("deviceOnlyid","1");
    SrvParser::getInstance()->setInput("ip","127.0.0.1");
    SrvParser::getInstance()->setInput("ecuid","1");

    m_buildPath = buildPath;

    QDir buildPathDir(m_buildPath);
    if(!buildPathDir.exists())
    {
        buildPathDir.mkdir(m_buildPath);
    }

    if(type & Build_OPT)
         RETURN_IFFALSE(m_optBuild->build(buildPath, cfv.path, cfv, type));

    if(type & Build_Global)
         RETURN_IFFALSE(m_globalsBuild->build(buildPath, cfv.path, cfv, type));

    if(type & Build_System)
         RETURN_IFFALSE(m_systemBuild->build(buildPath, cfv.path, cfv, type));

    return true;
}

bool EmosConfigBuild::writeFile(const QString& filePath, const QString& copyFilePath, const QString& context)
{
    QFile file(filePath);
    if (file.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream outFile(&file);
        outFile.setCodec("UTF-8");
        outFile << context;

        if(!copyFilePath.isEmpty())
        {
            QFile copyFile(copyFilePath);
            if(copyFile.exists())
                copyFile.remove();

            file.copy(copyFilePath);
        }

        file.close();
        return true;
    }

    return false;
}

bool EmosConfigBuild::writeFile(const QString& filePath, const QString& copyFilePath, QDomDocument* doc)
{
    Utils::XmlFileSaver file(filePath);
    file.write(doc);
    if(!file.finalize())
        return false;

    Utils::XmlFileSaver file2(copyFilePath);
    file2.write(doc);
    if(!file2.finalize())
        return false;

    return true;
}

bool EmosConfigBuild::writeFile(const QString& filePath,QDomDocument* doc)
{
    Utils::XmlFileSaver file(filePath);
    file.write(doc);
    return file.finalize();
}
}
