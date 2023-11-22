#include "emoscodebuildexport.h"
#include "emoscodebuildconstants.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <explorer/project.h>
#include <explorer/projectnodes.h>
#include <explorer/projecttree.h>
#include <buildoutpane/buildoutlogmanager.h>
#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>
#include "utils/mimetypes/mimedatabase.h"
#include <emostools/cfvparser.h>
#include <emostools/idlparser.h>
#include "idlselectdialog.h"
#include "emoscfvchecker.h"
#include "someipbuild.h"
#include "buildoutpane/buildoutlogmanager.h"

using namespace ProjectExplorer;
namespace EmosCodeBuild {
enum Documenttype{
    IDL_DOCUMENT,
    CFV_DOCUMENT
};

class EmosCodeBuildExportPrivate
{
public:
    EmosCodeBuildExportPrivate(EmosCodeBuildExport *q):q_ptr(q){}
    int buildType(const QString& filePath, const QString& buildPath);
    int buildApi(const QString& filePath, const QString& buildPath);
    int buildIDL(const QString& filePath, const QString& buildPath);
    int buildCFV(const QStringList& noSelect, const QString& filePath, const QString& buildPath);
    bool CodeRootCheck(QString buildPath);

    EmosConfigBuild m_EmosConfigBuild;
    EmosBuildBase* m_codeBuild;
    EmosCfvChecker m_cfvChecker;
    EmosCodeBuildExport* q_ptr;
};

EmosCodeBuildExport::EmosCodeBuildExport():d_ptr(new EmosCodeBuildExportPrivate(this))
{  
    d_ptr->m_codeBuild = new SomeipBuild();
}

EmosCodeBuildExport::~EmosCodeBuildExport()
{
    delete d_ptr->m_codeBuild;d_ptr->m_codeBuild = nullptr;
    delete d_ptr;d_ptr = nullptr;
}

EmosCodeBuildExport *EmosCodeBuildExport::instance()
{
    static EmosCodeBuildExport _instance;
    return &_instance;
}

int EmosCodeBuildExport::buildXml(XmlBuildType type, const QString &filePath, const QString &buildPath)
{
    EmosTools::CfvStruct cfv = EmosTools::CfvParser::getCfvStruct(filePath);
    return buildXml(type, cfv, buildPath);
}

int EmosCodeBuildExport::buildXml(XmlBuildType type, const EmosTools::CfvStruct& cfv, const QString &buildPath)
{
    if(!d_ptr->CodeRootCheck(buildPath))
    {
        return -1;
    }

    if(!d_ptr->m_cfvChecker.setData(cfv))
    {
        return -1;
    }

    QString emswcPath = buildPath + "/em_swc";
    QString configPath = emswcPath + "/" + QFileInfo(cfv.path).baseName().toLower() + "_cfv";

    //创建文件夹
    QDir emswcDir(emswcPath);
    if(!emswcDir.exists())
    {
        emswcDir.mkdir(emswcPath);
    }
    QDir buildOutDir(buildPath + "/build_out");
    if(!buildOutDir.exists())
    {
        buildOutDir.mkdir(buildPath + "/build_out");
    }
    QDir neoDir(buildPath + "/build_out/neo");
    if(!neoDir.exists())
    {
        neoDir.mkdir(buildPath + "/build_out/neo");
    }

    BuildoutLogManager::instance()->writeWithTime("generate xml to " + Utils::FilePath::fromString(configPath).absolutePath().toString(), Utils::NormalMessageFormat);

    if(!d_ptr->m_EmosConfigBuild.build(type, configPath, cfv))
    {
        BuildoutLogManager::instance()->writeWithTime(tr("Xml Build error"), Utils::ErrorMessageFormat);
        QMessageBox::critical(Core::ICore::mainWindow(),
                                 tr("Build error"),
                                 tr("Xml build fail with") + "\nfilePath:" +
                                 cfv.path);
        return -1;
    }
    BuildoutLogManager::instance()->writeWithTime(QString("Build XML OK"), Utils::LogMessageFormat);

    return 0;
}

int EmosCodeBuildExport::selectBuild(const QString &filePath, const QString &buildPath)
{
    if(!d_ptr->CodeRootCheck(buildPath))
    {
        return -1;
    }
    IdlSelectDialog dlg(filePath);
    if(dlg.exec() == QDialog::Accepted)
    {
        return d_ptr->buildCFV(dlg.getNoSelectList(), filePath, buildPath);
    }
    else
    {
        BuildoutLogManager::instance()->writeWithTime(tr("Not Build"), Utils::ErrorMessageFormat);
        return -1;
    }

    return 0;
}

int EmosCodeBuildExport::build(const QString &filePath, const QString &buildPath)
{
    if(!d_ptr->CodeRootCheck(buildPath))
    {
        return -1;
    }
    BuildoutLogManager::instance()->writeWithTime("build file:" + filePath, Utils::NormalMessageFormat);
    BuildoutLogManager::instance()->writeWithTime("generate path:" + Utils::FilePath::fromString(buildPath).absolutePath().toString(), Utils::NormalMessageFormat);

    Utils::MimeType m = Utils::mimeTypeForFile(filePath);
    if(m.name() == "application/emos.idl.xml")
    {
        return d_ptr->buildIDL(filePath, buildPath);
    }
    else if(m.name() == "application/emos.cfv.model")
    {
        return d_ptr->buildCFV(QStringList(), filePath, buildPath);
    }
    else if(m.name() == "application/emos.description.xml")
    {
        if(filePath.indexOf("type") >= 0)
            return d_ptr->buildType(filePath, buildPath);
        else if(filePath.indexOf("api") >= 0)
            return d_ptr->buildApi(filePath, buildPath);
    }
    else if(m.name() == "application/emos.sch.xml")
    {
        return emit schBuild(filePath, buildPath);
    }
    else
    {
        BuildoutLogManager::instance()->writeWithTime("error build:" + buildPath, Utils::ErrorMessageFormat);
    }

    return 0;
}

int EmosCodeBuildExport::clear(const QString &buildPath)
{
    if(!d_ptr->CodeRootCheck(buildPath))
    {
        return -1;
    }

    QDir buildDir(buildPath + "/em_swc");
    if(buildDir.exists())
    {
        buildDir.removeRecursively();
    }
    buildDir.mkdir(buildPath + "/em_swc");

    return 0;
}

int EmosCodeBuildExportPrivate::buildType(const QString& filePath, const QString& buildPath)
{
    if(0 != m_codeBuild->buildType(buildPath))
        return -1;

    return 0;
}

int EmosCodeBuildExportPrivate::buildApi(const QString& filePath, const QString& buildPath)
{
    if(0 != m_codeBuild->buildType(buildPath))
        return -1;

    if(0 != m_codeBuild->buildApi(buildPath))
        return -1;

    return 0;
}

int EmosCodeBuildExportPrivate::buildIDL(const QString &filePath, const QString &buildPath)
{
    if(0 != m_codeBuild->buildType(buildPath))
        return -1;

    if(0 != m_codeBuild->buildApi(buildPath))
        return -1;

    if(0 != m_codeBuild->buildIdl(EmosTools::IdlParser::getIdlStructByFile(filePath), buildPath))
        return -1;

    if(0 != q_ptr->buildXml(XmlBuildType(Build_OPT), filePath, buildPath))
        return -1;

    return 0;
}

int EmosCodeBuildExportPrivate::buildCFV(const QStringList& noSelect, const QString &filePath, const QString &buildPath)
{
    EmosTools::CfvStruct cfv = EmosTools::CfvParser::getCfvStruct(filePath);

    if(0 != m_codeBuild->buildType(buildPath))
        return -1;

    if(0 != m_codeBuild->buildApi(buildPath))
        return -1;

    for(int i = 0; i < cfv.compList.size(); i++)
    {
        if(!cfv.compList[i].idl.isValid)
            continue;

        QString oid = cfv.compList[i].oid;
        if(noSelect.contains(oid))
            continue;

        BuildoutLogManager::instance()->writeWithTime("build file:" + cfv.compList[i].idl.path, Utils::NormalMessageFormat);
        RETURN_IFNOTZERO(m_codeBuild->buildIdl(cfv.compList[i].idl, buildPath));
    }

    if(0 != q_ptr->buildXml(XmlBuildType(Build_System|Build_Global|Build_OPT), cfv, buildPath))
        return -1;

    return 0;
}

bool EmosCodeBuildExportPrivate::CodeRootCheck(QString buildPath)
{
    QDir dir(QString("%1/data/emoscodebuild").arg(buildPath));
    if(!dir.exists())
    {
        QString errLog = "Build Directory Error: please check whether your build directory is CodeRoot!";
        BuildoutLogManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        BuildoutLogManager::instance()->writeWithTime(buildPath, Utils::ErrorMessageFormat);
        return false;
    }

    m_codeBuild->setCodeRootDir(buildPath);

    return true;
}
}
