#include "someipbuild.h"
#include <coreplugin/icore.h>
#include <emostools/cfvparser.h>
#include <emostools/type_description_parser.h>
#include <emostools/api_description_parser.h>
#include <explorer/project.h>
#include <explorer/projecttree.h>
#include "injamanager.h"
#include "codemergetemplate.h"
#include <buildoutpane/buildoutlogmanager.h>

#include <coreplugin/messagemanager.h>

using namespace ProjectExplorer;
using namespace EmosTools;

namespace EmosCodeBuild {


SomeipBuild::SomeipBuild()
{
    m_engineManager->setTemplateType(Template_Somip);
    //m_CodeMergeTemplate = new CodeMergeTemplate(m_engineManager);
}

SomeipBuild::~SomeipBuild()
{
    //delete m_CodeMergeTemplate;
    //m_CodeMergeTemplate = nullptr;
}

int SomeipBuild::buildType(const QString &buildPath)
{
    EmosTools::TypeParser::getInstance()->reloadXml();

    m_buildpath = buildPath + "/em_swc";

    QString intfPath = m_buildpath + "/comintf/";
    QDir projectfilecomintf(intfPath);
    projectfilecomintf.mkdir(intfPath);

    m_structInfo = TypeParser::getInstance()->getStructs();
    m_enumInfo = TypeParser::getInstance()->getEnums();

    m_engineManager->setTypeData();

#ifdef QT_DEBUG
    //BuildoutLogManager::instance()->writeWithTime("Type Json data:\n" + m_engineManager->getdata(), Utils::StdOutFormat);
#endif

    RETURN_IFFALSE(MacroBuild(intfPath));
    RETURN_IFFALSE(EnumBuild(intfPath));
    RETURN_IFFALSE(StructBuild(intfPath));

    BuildoutLogManager::instance()->writeWithTime("Build Type OK", Utils::LogMessageFormat);

    return 0;
}

int SomeipBuild::buildApi(const QString &buildPath)
{
    EmosTools::ApiParser::getInstance()->reloadXml();

    m_buildpath = buildPath + "/em_swc";

    QString intfPath = m_buildpath + "/comintf/";
    QDir projectfilecomintf(intfPath);
    projectfilecomintf.mkdir(intfPath);

    m_engineManager->setApiData();

#ifdef QT_DEBUG
    //BuildoutLogManager::instance()->writeWithTime("Api Json data:\n" + m_engineManager->getdata(), Utils::StdOutFormat);
#endif

    //API build
    RETURN_IFFALSE(IntfBuild(intfPath, Proxy));

    BuildoutLogManager::instance()->writeWithTime("Build Api OK", Utils::LogMessageFormat);
    
    return 0;
}

int SomeipBuild::buildIdl(const EmosTools::IDLStruct &idlStruct, const QString &buildPath)
{
    m_buildpath = buildPath + "/em_swc";
    m_srcBuildPath = buildPath + "/src";
    m_idlData = idlStruct;
    m_name = m_idlData.name;

    m_engineManager->setIdlData(m_idlData);
    //BuildoutLogManager::instance()->writeWithTime("TemplateDir: " + m_engineManager->getTemplateDir(), Utils::LogMessageFormat);
    /*
    m_CodeMergeTemplate->setIdlData(filePath);
    QString historyPath = ProjectExplorer::ProjectTree::currentProject()->projectDirectory().toString() + "/.history/";
    QDir historyDir(historyPath);
    if(!historyDir.exists())
    {
        codeMergeFlag = false;
        historyDir.mkdir(historyPath);
    }
    QString jsonPath = ProjectExplorer::ProjectTree::currentProject()->projectDirectory().toString() + "/.history/" + m_name.toLower() + ".json";
    m_CodeMergeTemplate->setoldIdlJson(jsonPath);
    */
#ifdef QT_DEBUG
    //BuildoutLogManager::instance()->writeWithTime("Idl Json data:\n" + m_engineManager->getdata(), Utils::StdOutFormat);
#endif

    if(m_idlData.isService)
    {
        bool inProcess = true;
        QMapIterator<QString, int> i(m_idlData.service);
        while (i.hasNext()) {
            i.next();
            inProcess &= EmosTools::ApiParser::getInstance()->getService(i.key()).inProcess;
        }
        //包含的服务全是进程内则不生成proxy
        if(!inProcess)
        {
            if(!proxyBuild())
             return -1;
        }
    }
    if(!skelBuild())
     return -1;

    /*
    if(codeMergeFlag == false){
        m_sendMessage(QString("IDL(%1) not old record for CodeMerge").arg(m_name), BuildStep::OutputFormat::WarningMessage);
    }
    m_sendMessage(QString("Build IDL(%1) OK").arg(m_name), BuildStep::OutputFormat::NormalMessage);

    codeWriter(jsonPath,m_CodeMergeTemplate->getIdlJson());
    */

    BuildoutLogManager::instance()->writeWithTime(QString("Build IDL(%1) OK").arg(m_name), Utils::LogMessageFormat);

    return 0;
}

//generate _template.h
bool SomeipBuild::TemplateHBuild(QString path,int type)
{
    QString ContentT;
    if(type == Service)
    {
        ContentT = m_engineManager->renderFile("service/template/sample_template.h", m_engineManager->getdata());
    }
    QString Path = path + m_name.toLower() +"_template.h";
    RETURN_IFFALSE(emptyLogPrint(ContentT, Path));
    RETURN_IFFALSE(codeWriter(Path, ContentT));
    return true;
}
//generate _template.cpp
bool SomeipBuild::TemplateCBuild(QString path,int type)
{
    QString ContentT;
    if(type == Service)
    {
        ContentT = m_engineManager->renderFile("service/template/sample_template.cpp", m_engineManager->getdata());
    }
    QString Path = path + m_name.toLower() + "_template.cpp";
    RETURN_IFFALSE(emptyLogPrint(ContentT, Path));
    RETURN_IFFALSE(codeWriter(Path, ContentT));
    return true;
}
//generate _comp.cpp
bool SomeipBuild::ComCBuild(QString path,int type)
{
    QString ContentT;
    QString suffix = "";
    if (type == Proxy)
    {
        suffix = "_prox";
        ContentT = m_engineManager->renderFile("proxy/source/sample_prox_comp.cpp", m_engineManager->getdata());
    }
    else if (type == Service)
    {
        suffix = "_skel";
        ContentT = m_engineManager->renderFile("service/source/sample_skel_comp.cpp", m_engineManager->getdata());
    }
    QString Path = path + m_name.toLower() + suffix + "_comp.cpp";
    RETURN_IFFALSE(emptyLogPrint(ContentT, Path));
    RETURN_IFFALSE(codeWriter(Path, ContentT));
    return true;
}
//generate _comp.h
bool SomeipBuild::ComHBuild(QString path,int type)
{
    QString ContentT;
    QString suffix = "";
    if (type == Proxy)
    {
        suffix = "_prox";
        ContentT = m_engineManager->renderFile("proxy/source/sample_prox_comp.h", m_engineManager->getdata());
    }
    else if (type == Service)
    {
        suffix = "_skel";
        ContentT = m_engineManager->renderFile("service/source/sample_skel_comp.h", m_engineManager->getdata());
    }
    QString Path = path + m_name.toLower() + suffix + "_comp.h";
    RETURN_IFFALSE(emptyLogPrint(ContentT, Path));
    RETURN_IFFALSE(codeWriter(Path, ContentT));
    return true;
}
//generate intf
bool SomeipBuild::IntfBuild(QString path,int type)
{
    QList<tSrvService> services = EmosTools::ApiParser::getInstance()->getServices();
    for(int i = 0;i < services.size();i++)
    {
        tSrvService serviceNode = services.at(i);
        QString nowservice = serviceNode.name;
        m_engineManager->setSingleData("nowservice",nowservice);
        QString ContentIT, ContentCT, ContentHT;
        QString Path;
        ContentIT = m_engineManager->renderFile("comintf/i_sample_intf.h", m_engineManager->getdata());
        ContentCT = m_engineManager->renderFile("comintf/sample_intf.c", m_engineManager->getdata());
        ContentHT = m_engineManager->renderFile("comintf/sample_intf.h", m_engineManager->getdata());
        Path = path + nowservice + "/";
        QDir projectfileintf(Path);
        if(projectfileintf.exists())
        {
            projectfileintf.removeRecursively();
        }
        projectfileintf.mkdir(Path);

        QString IPath = Path + "i_" + nowservice.toLower() + "_intf.h";
        QString CPath = Path + nowservice.toLower() + "_intf.c";
        if(serviceNode.interfaceType == "cpp")
            CPath = Path + nowservice.toLower() + "_intf.cpp";
        QString HPath = Path + nowservice.toLower() + "_intf.h";
        RETURN_IFFALSE(emptyLogPrint(ContentIT, IPath));
        RETURN_IFFALSE(emptyLogPrint(ContentCT, CPath));
        RETURN_IFFALSE(emptyLogPrint(ContentHT, HPath));
        RETURN_IFFALSE(codeWriter(IPath, ContentIT));
        RETURN_IFFALSE(codeWriter(CPath, ContentCT));
        RETURN_IFFALSE(codeWriter(HPath, ContentHT));
    }
    return true;
}
//generate cmakelist
bool SomeipBuild::CmakeBuild(QString path, int type)
{
    QString ContentT;
    if(type == Proxy)
    {
        ContentT = m_engineManager->renderFile("proxy/CMakeLists.txt", m_engineManager->getdata());
    }
    else
    {
        ContentT = m_engineManager->renderFile("service/CMakeLists.txt", m_engineManager->getdata());
    }
    QString Path = path + "CMakeLists.txt";
    RETURN_IFFALSE(emptyLogPrint(ContentT, Path));
    RETURN_IFFALSE(codeWriter(Path, ContentT));
    return true;
}

bool SomeipBuild::LittleCmakeBuild(QString path, int type)
{
    QString ContentT;
    QString Path = path + m_name.toLower();
    if(type == Proxy)
    {
        ContentT = m_engineManager->renderFile("proxy/sampleproxy.cmake", m_engineManager->getdata());
        Path += "proxy";
    }
    else
    {
        ContentT = m_engineManager->renderFile("service/sample.cmake", m_engineManager->getdata());
    }
    Path += ".cmake";
    RETURN_IFFALSE(emptyLogPrint(ContentT, Path));
    RETURN_IFFALSE(codeWriter(Path, ContentT));
    return true;
}
//generate method
bool SomeipBuild::MethodBuild(QString path, int type)
{
    QString ContentCT;
    QString ContentHT;
    QString suffix = "";

    QMapIterator<QString, int> i(m_idlData.service);
    while (i.hasNext())
    {
        i.next();
        tSrvService serviceNode = EmosTools::ApiParser::getInstance()->getService(i.key());

        if(serviceNode.inProcess)
        {
            continue;
        }

        m_engineManager->setSingleData("nowservice",serviceNode.name);
        QList<tSrvFunction> methodList = serviceNode.functions;
        for(int j = 0; j < methodList.size(); ++j)
        {
            QString nowmethod = methodList.at(j).name;
            m_engineManager->setSingleData("nowmethod",nowmethod);
            if(type == Proxy)
            {
                suffix = "_prox";
                ContentCT = m_engineManager->renderFile("proxy/source/method_sample_prox.cpp", m_engineManager->getdata());
                ContentHT = m_engineManager->renderFile("proxy/source/method_sample_prox.h", m_engineManager->getdata());
            }
            else
            {
                suffix = "_skel";
                ContentCT = m_engineManager->renderFile("service/source/method_sample_skel.cpp", m_engineManager->getdata());
                ContentHT = m_engineManager->renderFile("service/source/method_sample_skel.h", m_engineManager->getdata());
            }
            QString CPath = path + "method_" + nowmethod.toLower() + suffix + ".cpp";
            QString HPath = path + "method_" + nowmethod.toLower() + suffix + ".h";

            if(!emptyLogPrint(ContentCT, CPath))
            {
                BuildoutLogManager::instance()->writeWithTime("nowservice : " + serviceNode.name, Utils::ErrorMessageFormat);
                BuildoutLogManager::instance()->writeWithTime(m_engineManager->getdata(), Utils::ErrorMessageFormat);
            }
            
            RETURN_IFFALSE(emptyLogPrint(ContentCT, CPath));
            RETURN_IFFALSE(emptyLogPrint(ContentHT, HPath));
            RETURN_IFFALSE(codeWriter(CPath, ContentCT));
            RETURN_IFFALSE(codeWriter(HPath, ContentHT));
        }
    }
    return true;
}
//generate event
bool SomeipBuild::EventBuild(QString path, int type)
{
    QString ContentCT;
    QString ContentHT;
    QString suffix = "";

    QMapIterator<QString, int> i(m_idlData.service);
    while (i.hasNext())
    {
        i.next();
        tSrvService serviceNode = EmosTools::ApiParser::getInstance()->getService(i.key());

        if(serviceNode.inProcess)
        {
            continue;
        }

        m_engineManager->setSingleData("nowservice",serviceNode.name);
        QList<tSrvEvent> eventList = serviceNode.events;
        for(int j = 0; j < eventList.size(); ++j)
        {
            QString nowevent = eventList.at(j).name;
            m_engineManager->setSingleData("nowevent",nowevent);
            if(type == Proxy)
            {
                suffix = "_prox";
                ContentCT = m_engineManager->renderFile("proxy/source/event_sample_prox.cpp", m_engineManager->getdata());
                ContentHT = m_engineManager->renderFile("proxy/source/event_sample_prox.h", m_engineManager->getdata());
            }
            else
            {
                suffix = "_skel";
                ContentCT = m_engineManager->renderFile("service/source/event_sample_skel.cpp", m_engineManager->getdata());
                ContentHT = m_engineManager->renderFile("service/source/event_sample_skel.h", m_engineManager->getdata());
            }
            QString CPath = path + "event_" + nowevent.toLower() + suffix + ".cpp";
            QString HPath = path + "event_" + nowevent.toLower() + suffix + ".h";

            if(!emptyLogPrint(ContentCT, CPath))
            {
                BuildoutLogManager::instance()->writeWithTime("nowservice : " + serviceNode.name, Utils::ErrorMessageFormat);
                BuildoutLogManager::instance()->writeWithTime(m_engineManager->getdata(), Utils::ErrorMessageFormat);
            }

            RETURN_IFFALSE(emptyLogPrint(ContentCT, CPath));
            RETURN_IFFALSE(emptyLogPrint(ContentHT, HPath));
            RETURN_IFFALSE(codeWriter(CPath, ContentCT));
            RETURN_IFFALSE(codeWriter(HPath, ContentHT));
        }
    }
    return true;
}
//generate field
bool SomeipBuild::FieldBuild(QString path, int type)
{
    QString ContentCT;
    QString ContentHT;

    QMapIterator<QString, int> i(m_idlData.service);
    while (i.hasNext())
    {
        i.next();
        tSrvService serviceNode = EmosTools::ApiParser::getInstance()->getService(i.key());

        if(serviceNode.inProcess)
        {
            continue;
        }

        m_engineManager->setSingleData("nowservice",serviceNode.name);
        QList<tSrvField> fieldList = serviceNode.fields;
        for(int i = 0; i < fieldList.size(); ++i)
        {
            QString nowfield = fieldList.at(i).name;
            m_engineManager->setSingleData("nowfield", nowfield);
            QString suffix = "";
            if(type == Proxy)
            {
                suffix = "_prox";
                ContentCT = m_engineManager->renderFile("proxy/source/field_sample_prox.cpp", m_engineManager->getdata());
                ContentHT = m_engineManager->renderFile("proxy/source/field_sample_prox.h", m_engineManager->getdata());
            }
            else
            {
                suffix = "_skel";
                ContentCT = m_engineManager->renderFile("service/source/field_sample_skel.cpp", m_engineManager->getdata());
                ContentHT = m_engineManager->renderFile("service/source/field_sample_skel.h", m_engineManager->getdata());
            }
            QString CPath = path + "field_" + nowfield.toLower() + suffix + ".cpp";
            QString HPath = path + "field_" + nowfield.toLower() + suffix + ".h";
            RETURN_IFFALSE(emptyLogPrint(ContentCT, CPath));
            RETURN_IFFALSE(emptyLogPrint(ContentHT, HPath));
            RETURN_IFFALSE(codeWriter(CPath, ContentCT));
            RETURN_IFFALSE(codeWriter(HPath, ContentHT));
        }
    }
    return true;
}

bool SomeipBuild::PinBuild(QString path, int type)
{
    QString ContentCT;
    QString ContentHT;

    QMapIterator<QString, int> i(m_idlData.service);
    while (i.hasNext())
    {
        i.next();
        tSrvService serviceNode = EmosTools::ApiParser::getInstance()->getService(i.key());

        m_engineManager->setSingleData("nowservice",serviceNode.name);
        QList<tSrvPin> pinList = serviceNode.pins;
        for(int i = 0; i < pinList.size(); ++i)
        {
            QString nowpin = pinList.at(i).name;
            m_engineManager->setSingleData("nowpin", nowpin);
            QString suffix = "";
            if(type == Proxy)
            {
                suffix = "_prox";
                ContentCT = m_engineManager->renderFile("proxy/source/pin_sample_prox.cpp", m_engineManager->getdata());
                ContentHT = m_engineManager->renderFile("proxy/source/pin_sample_prox.h", m_engineManager->getdata());
            }
            else
            {
                suffix = "_skel";
                ContentCT = m_engineManager->renderFile("service/source/pin_sample_skel.cpp", m_engineManager->getdata());
                ContentHT = m_engineManager->renderFile("service/source/pin_sample_skel.h", m_engineManager->getdata());
            }
            QString CPath = path + "pin_" + nowpin.toLower() + suffix + ".cpp";
            QString HPath = path + "pin_" + nowpin.toLower() + suffix + ".h";
            RETURN_IFFALSE(emptyLogPrint(ContentCT, CPath));
            RETURN_IFFALSE(emptyLogPrint(ContentHT, HPath));
            RETURN_IFFALSE(codeWriter(CPath, ContentCT));
            RETURN_IFFALSE(codeWriter(HPath, ContentHT));
        }
    }
    return true;
}
//generate stub
bool SomeipBuild::StubBuild(QString path)
{
    QString ContentCT;
    QString ContentHT;
    ContentCT = m_engineManager->renderFile("service/stub/sample_stub.cpp", m_engineManager->getdata());
    ContentHT = m_engineManager->renderFile("service/stub/sample_stub.h", m_engineManager->getdata());

    QString CPath = path + m_name.toLower() + "_stub.cpp";
    QString HPath = path + m_name.toLower() + "_stub.h";
    RETURN_IFFALSE(emptyLogPrint(ContentCT, CPath));
    RETURN_IFFALSE(emptyLogPrint(ContentHT, HPath));
    RETURN_IFFALSE(codeWriter(CPath, ContentCT));
    RETURN_IFFALSE(codeWriter(HPath, ContentHT));
    return true;
}
//generate struct
bool SomeipBuild::StructBuild(QString path)
{
    QSet<QString> filenameSet;
    for(int i = 0; i < m_structInfo.size(); i++)
    {
        filenameSet.insert(m_structInfo.at(i).filename);
    }
    filenameSet.insert("common");
    QSet<QString>::iterator iter = filenameSet.begin();
    while (iter != filenameSet.end())
    {
        m_engineManager->setSingleData("nowfilename", *iter);
        QString ContentT = m_engineManager->renderFile("comintf/struct_common.h", m_engineManager->getdata());
        QString Path = path + "struct_" + *iter + ".h";
        RETURN_IFFALSE(emptyLogPrint(ContentT, Path));
        RETURN_IFFALSE(codeWriter(Path, ContentT));
        iter++;
    }
    return true;
}
//generate enum
bool SomeipBuild::EnumBuild(QString path)
{
    QSet<QString> filenameSet;
    for(int i = 0; i < m_enumInfo.size(); i++)
    {
        filenameSet.insert(m_enumInfo.at(i).filename);
    }
    filenameSet.insert("common");

    QSet<QString>::iterator iter = filenameSet.begin();
    while(iter != filenameSet.end())
    {
        m_engineManager->setSingleData("nowfilename", *iter);
        QString ContentT = m_engineManager->renderFile("comintf/enum_common.h", m_engineManager->getdata());
        QString Path = path + "enum_" + *iter + ".h";
        RETURN_IFFALSE(emptyLogPrint(ContentT, Path));
        RETURN_IFFALSE(codeWriter(Path, ContentT));
        iter++;
    }
    return true;
}

bool SomeipBuild::MacroBuild(QString path)
{
    QString ContentT = m_engineManager->renderFile("comintf/macro_common.h", m_engineManager->getdata());
    QString Path = path + "macro_common.h";
    RETURN_IFFALSE(emptyLogPrint(ContentT, Path));
    RETURN_IFFALSE(codeWriter(Path, ContentT));
    return true;
}

bool SomeipBuild::TemplateABCBuild(QString path)
{
    QString ContentT = m_engineManager->renderFile("service/stub/sample_base.h", m_engineManager->getdata());
    QString Path = path + m_name.toLower() +"_base.h";
    RETURN_IFFALSE(emptyLogPrint(ContentT, Path));
    RETURN_IFFALSE(codeWriter(Path, ContentT));
    return true;
}

bool SomeipBuild::TemplateHeaderBuild(QString path)
{
    QString ContentT = m_engineManager->renderFile("service/stub/sample_header.h", m_engineManager->getdata());
    QString Path = path + m_name.toLower() +"_header.h";
    RETURN_IFFALSE(emptyLogPrint(ContentT, Path));
    RETURN_IFFALSE(codeWriter(Path, ContentT));
    return true;
}

bool SomeipBuild::TemplateUserBuild(QString path)
{
    QString ContentT = m_engineManager->renderFile("service/user/sample_template.cpp", m_engineManager->getdata());
    QString Path = path + "user_" + m_name.toLower() +"_template.cpp";
    RETURN_IFFALSE(emptyLogPrint(ContentT, Path));
    RETURN_IFFALSE(codeWriter(Path, ContentT));
    return true;
}
/*
void SomeipBuild::CodeMerge(QString templatepath)
{
    //调用解析模块单独生成template的代码
    m_CodeMergeTemplate->templateHBuild(templatepath + m_name.toLower() +"_template.h");
    m_CodeMergeTemplate->templateCBuild(templatepath + m_name.toLower() + "_template.cpp");
    //CmakeList不变
}
*/
bool SomeipBuild::proxyBuild()
{
    QString builddir;
    builddir = m_buildpath + "/" + m_name.toLower() + "proxy/";
    QDir projectfileproxy(builddir);
    if(projectfileproxy.exists())
    {
        projectfileproxy.removeRecursively();
    }
    projectfileproxy.mkdir(builddir);
    QString intfPath = m_buildpath + "/comintf/";
    QDir projectfilecomintf(intfPath);
    projectfilecomintf.mkdir(intfPath);
    QString sourcePath = builddir + "source/";
    projectfileproxy.mkdir(sourcePath);

    RETURN_IFFALSE(ComCBuild(sourcePath, Proxy));
    RETURN_IFFALSE(ComHBuild(sourcePath, Proxy));
    RETURN_IFFALSE(MethodBuild(sourcePath, Proxy));
    RETURN_IFFALSE(EventBuild(sourcePath, Proxy));
    RETURN_IFFALSE(FieldBuild(sourcePath, Proxy));
    RETURN_IFFALSE(PinBuild(sourcePath, Proxy));
    RETURN_IFFALSE(CmakeBuild(builddir, Proxy));
    RETURN_IFFALSE(LittleCmakeBuild(builddir,Proxy));
    return true;
}

bool SomeipBuild::skelBuild()
{
    QString em_swc_builddir;
    em_swc_builddir = m_buildpath + "/" + m_name.toLower() + "/";
    //QString src_builddir = m_srcBuildPath + "/" + m_name.toLower() + "/";
    QDir projectfileservice(em_swc_builddir);
    if(projectfileservice.exists())
    {
        projectfileservice.removeRecursively();
    }
    projectfileservice.mkdir(em_swc_builddir);

    QString sourcePath = em_swc_builddir + "source/";
    QString intfPath = m_buildpath + "/comintf/";
    QString em_swc_templatePath = em_swc_builddir + "template/";
    //QString src_templatePath = src_builddir + "template/";
    QString stubPath = em_swc_builddir + "stub/";
    QString userPath = em_swc_builddir + "user/";
    projectfileservice.mkdir(sourcePath);
    projectfileservice.mkdir(em_swc_templatePath);
    projectfileservice.mkdir(stubPath);
    //projectfileservice.mkdir(userPath);
    //em_swc
    RETURN_IFFALSE(ComCBuild(sourcePath, Service));
    RETURN_IFFALSE(ComHBuild(sourcePath, Service));
    RETURN_IFFALSE(MethodBuild(sourcePath, Service));
    RETURN_IFFALSE(EventBuild(sourcePath, Service));
    RETURN_IFFALSE(FieldBuild(sourcePath, Service));
    RETURN_IFFALSE(PinBuild(sourcePath, Service));
    RETURN_IFFALSE(StubBuild(stubPath));
    RETURN_IFFALSE(TemplateHBuild(em_swc_templatePath, Service));
    RETURN_IFFALSE(TemplateCBuild(em_swc_templatePath, Service));
    RETURN_IFFALSE(CmakeBuild(em_swc_builddir, Service));
    RETURN_IFFALSE(LittleCmakeBuild(em_swc_builddir,Service));
    RETURN_IFFALSE(TemplateABCBuild(stubPath));
    RETURN_IFFALSE(TemplateHeaderBuild(stubPath));
    //RETURN_IFFALSE(TemplateUserBuild(userPath));
    /*
    QDir src_projectfileservice(src_builddir);
    if(src_projectfileservice.exists() && codeMergeFlag == true)
    {
        CodeMerge(src_templatePath);
        return true;
    } else {
        src_projectfileservice.mkdir(src_builddir);
        src_projectfileservice.mkdir(src_templatePath);
    }
    //src
    RETURN_IFFALSE(TemplateHBuild(src_templatePath, Service));
    RETURN_IFFALSE(TemplateCBuild(src_templatePath, Service));
    RETURN_IFFALSE(CmakeBuild(src_builddir, Service));
    */
    return true;
}
}
