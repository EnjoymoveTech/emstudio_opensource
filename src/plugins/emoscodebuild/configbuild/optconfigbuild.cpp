#include "optconfigbuild.h"
#include "emosconfigbuild.h"
#include <buildoutpane/buildoutlogmanager.h>

namespace EmosCodeBuild {

OptConfigBuild::OptConfigBuild(EmosConfigBuild* configBuild) : m_configBuild(configBuild)
{

}

bool OptConfigBuild::build(const QString &buildPath, const QString &filePath, const CfvStruct& cfvStruct, XmlBuildType type)
{
    QString optBuildPath = buildPath;//buildPath.left(buildPath.lastIndexOf("/"));
    QStringList ContantTotal;
    QSet<QString> scrTotalPivot;
    QString optName;

    if(type == Build_OPT) //单个idl生成opt
    {
        ContantTotal << generateSingleIdl(filePath);
        optName = "Build101.opt";
        EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime("use b 101 to build", Utils::NormalMessageFormat);
    }
    else
    {
        bool isDeveloper = qEnvironmentVariable("Developer", "false") == "true" ? true : false;
        optName = "Build100.opt";
        for(const auto& device : cfvStruct.deviceList)
        {
            QSet<QString> scrDevicePivot;
            QStringList ContantTDevice;
            for(const auto& process : device.processList)
            {
                QSet<QString> scrProcessPivot;
                QStringList ContantTProcess;

                if(generateProcessCode(optBuildPath, process.name))
                {
                    ContantTProcess << urlText + branchText + srcText + "application_" + process.name + "\n\n";
                    ContantTDevice << urlText + branchText + srcText + "application_" + process.name + "\n\n";
                    ContantTotal << urlText + branchText + srcText + "application_" + process.name + "\n\n";
                }

                for(const auto& comp : process.compList)
                {
                    if(!comp.idl.isValid)
                    {
                        EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime(QString("idl:%1(%2) not exist").arg(comp.name).arg(comp.oid), Utils::ErrorMessageFormat);
                        return false;
                    }

                    if(!scrProcessPivot.contains(comp.idl.name))
                    {
                        if(isDeveloper)
                            ContantTProcess << urlText + branchText + srcText + "../em_swc/" + comp.idl.name.toLower() + "\n\n";
                        else
                            ContantTProcess << urlText + branchText + srcText + comp.idl.name.toLower() + "\n\n";//"../em_swc/"
                        scrProcessPivot.insert(comp.idl.name);
                    }
                    if(!scrDevicePivot.contains(comp.idl.name))
                    {
                        if(isDeveloper)
                            ContantTDevice << urlText + branchText + srcText + "../em_swc/" + comp.idl.name.toLower() + "\n\n";
                        else
                            ContantTDevice << urlText + branchText + srcText + comp.idl.name.toLower() + "\n\n";
                        scrDevicePivot.insert(comp.idl.name);
                    }
                    if(!scrTotalPivot.contains(comp.idl.name))
                    {
                        if(isDeveloper)
                            ContantTotal << urlText + branchText + srcText + "../em_swc/" + comp.idl.name.toLower() + "\n\n";
                        else
                            ContantTotal << urlText + branchText + srcText + comp.idl.name.toLower() + "\n\n";
                        scrTotalPivot.insert(comp.idl.name);
                    }

                    for(const auto& c : comp.connectList)
                    {
                        ComponentStruct CompA = CfvParser::getCompFromUid(c.a_uid, cfvStruct.compList);

                        if(CfvParser::getProcess(CompA, cfvStruct.processList).name !=
                           CfvParser::getProcess(comp, cfvStruct.processList).name)
                        {
                            if(!scrProcessPivot.contains(CompA.idl.name + "proxy"))
                            {
                                ContantTProcess << urlText + branchText + srcText + "../em_swc/" + CompA.idl.name.toLower() + "proxy" + "\n\n";
                                scrProcessPivot.insert(CompA.idl.name + "proxy");
                            }
                            if(!scrDevicePivot.contains(CompA.idl.name + "proxy"))
                            {
                                ContantTDevice << urlText + branchText + srcText + "../em_swc/" + CompA.idl.name.toLower() + "proxy" + "\n\n";
                                scrDevicePivot.insert(CompA.idl.name + "proxy");
                            }
                            if(!scrTotalPivot.contains(CompA.idl.name + "proxy"))
                            {
                                ContantTotal << urlText + branchText + srcText + "../em_swc/" + CompA.idl.name.toLower() + "proxy" + "\n\n";
                                scrTotalPivot.insert(CompA.idl.name + "proxy");
                            }
                        }
                    }
                }
                EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime(QString("use b 100_%1 to build process %1").arg(process.name), Utils::NormalMessageFormat);
                RETURN_IFFALSE(EmosConfigBuild::writeFile(optBuildPath + "/Build100_" + process.name + ".opt", optBuildPath + "/../../script/config/Build100_" + process.name + ".opt", ContantTProcess.join("")));
            }
            EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime(QString("use b 100_device_%1 to build device %1").arg(device.name), Utils::NormalMessageFormat);
            RETURN_IFFALSE(EmosConfigBuild::writeFile(optBuildPath + "/Build100_device_" + device.name + ".opt", optBuildPath + "/../../script/config/Build100_device_" + device.name + ".opt", ContantTDevice.join("")));
        }
        EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime("use b 100 to all build", Utils::NormalMessageFormat);
    }

    RETURN_IFFALSE(EmosConfigBuild::writeFile(optBuildPath + "/" + optName, optBuildPath + "/../../script/config/" + optName, ContantTotal.join("")));

    return true;
}

QString OptConfigBuild::generateSingleIdl(const QString &filePath)
{
    QString ContantT;
    QSet<QString> scrPivot;
    IDLStruct idlStruct = IdlParser::getIdlStructByFile(filePath);

    if(!idlStruct.isValid)
    {
        EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime(QString("idl:%1 not exist").arg(filePath), Utils::ErrorMessageFormat);
        return ContantT;
    }

    if(idlStruct.isService && !scrPivot.contains(idlStruct.name + "proxy"))
    {
        if(!scrPivot.contains(idlStruct.name))
        {
            ContantT += urlText + branchText + srcText + idlStruct.name.toLower() + "\n\n";
            scrPivot.insert(idlStruct.name);
        }

        ContantT += urlText + branchText + srcText + "../em_swc/" + idlStruct.name.toLower() + "proxy" + "\n\n";//"../em_swc/"
        scrPivot.insert(idlStruct.name + "proxy");
    }
    else
    {
        if(!scrPivot.contains(idlStruct.name))
        {
            ContantT += urlText + branchText + srcText + idlStruct.name.toLower() +  + "\n\n";//"../em_swc/" 
            scrPivot.insert(idlStruct.name);
        }
    }

    return ContantT;
}

bool OptConfigBuild::generateProcessCode(const QString& buildPath, const QString &processName)
{
    QString srcPath = buildPath + "/../../src/";
    QDir src(srcPath);
    if(!src.exists()) {
        EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime(QString("CodeRoot src dir not exist"), Utils::ErrorMessageFormat);
        return false;
    }

    QString processPath = srcPath + "application_" + processName;
    QDir process(processPath);
    if(process.exists()) {
        process.removeRecursively();
    }
    process.mkdir(processPath);
    process.mkdir(processPath + "/emc");
    process.mkdir(processPath + "/emc/src");
    generateProcessCmake(processPath,processName);
    QString emc_Path = srcPath + "application/emc/src/main.c";
    if (!QFile::copy(emc_Path, processPath + "/emc/src/main.c")){
        return false;
    }
    return true;
}

void OptConfigBuild::generateProcessCmake(const QString& path, const QString& name)
{
    QString process_CmakePath = path + "/CMakeLists.txt";
    QString process_Cont = "project(emc_core)\n message(\"\tLIBRARIES   ${LIBRARIES}\")\n add_subdirectory(./emc)\n";
    QString emc_CmakePath = path + "/emc/CMakeLists.txt";
    QString emc_Cont = "project(application_" + name + " C CXX)\n" +
            "file(GLOB_RECURSE SOURCES \n" +
            "       \"src/*.*\")\n" +
            "add_definitions( -D_TARGET_APP_=\"" + name + "\")\n" +
            "add_definitions (-DEMC_USE_WINSOCK)\n" +
            "add_definitions (-DEMC_HAVE_WINDOWS)\n" +
            "add_definitions (-DEMC_STATIC_LIB)\n" +
            "add_definitions( -DNEO_DIR=\"${FRAMEWORK_DIR}\" )\n" +
            "add_definitions( -D_PRJ_ID_=0)\n" +
            "begin_addon_lib()\n" +
            "set( SDK_EMCPP 				ON)\n" +
            "end_addon_lib()\n" +
            "add_executable(${PROJECT_NAME} ${SOURCES})\n" +
            "target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})\n" +
            "set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER \"neo\")\n" +
            "set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${FRAMEWORK_DIR})\n" +
            "set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${FRAMEWORK_DIR})\n" +
            "set_property(TARGET ${PROJECT_NAME} PROPERTY ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${FRAMEWORK_DIR})\n" +
            "set_property(TARGET ${PROJECT_NAME} PROPERTY ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${FRAMEWORK_DIR})\n" +
            "set_property(TARGET ${PROJECT_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY_DEBUG ${FRAMEWORK_DIR})\n" +
            "set_property(TARGET ${PROJECT_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY_RELEASE ${FRAMEWORK_DIR})\n" +
            "framework_install(${PROJECT_NAME})\n" +
            "sdk_library_install(${PROJECT_NAME})\n";

    QFile file_process(process_CmakePath);
    if(file_process.open(QIODevice::WriteOnly))
    {
        QTextStream outFile(&file_process);
        outFile.setCodec("UTF-8");
        outFile << process_Cont;
        file_process.close();
    }
    QFile file(emc_CmakePath);
    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream outFile(&file);
        outFile.setCodec("UTF-8");
        outFile << emc_Cont;
        file.close();
    }
}
}
