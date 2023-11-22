#include "injamanager.h"
#include "injacallback.h"
#include "injadatabuild.h"
#include "injadebugdialog.h"
#include "jsonparser.h"
#include <inja/inja.hpp>
#include <buildoutpane/buildoutlogmanager.h>
using Json = inja::json;

namespace EmosCodeBuild {
namespace Internal {

class InjaManagerPrivate
{
public:

    inja::Environment env;
    QScopedPointer<injaCallBack> m_callBack;
    QScopedPointer<injaDataBuild> m_dataBuild;
    //lingfei.yu 智能指针便于释放
    QScopedPointer<JsonParser> m_jsonParser;
};

}

InjaManager::InjaManager(): TempEngineManager(),
    d(new Internal::InjaManagerPrivate)
{
    d->env.set_input_path(QString("%1/someip/").arg(m_staticDir).toStdString());
    d->m_callBack.reset(new injaCallBack(this));
    d->m_dataBuild.reset(new injaDataBuild());
    d->m_jsonParser.reset(new JsonParser());
}

InjaManager::~InjaManager()
{
}

void InjaManager::showDebugDialog()
{
    InjaDebugDialog dlg;
    dlg.exec();
}

void InjaManager::setTemplateType(TemplateType type)
{
    TempEngineManager::setTemplateType(type);

    switch(type)
    {
    case Template_Somip:
        d->env.set_input_path(QString("%1someip/").arg(m_staticDir).toStdString());
        break;
    default:
        d->env.set_input_path(QString("%1someip/").arg(m_staticDir).toStdString());
        break;
    }
}

TemplateType InjaManager::getTemplateType() const
{
    return TempEngineManager::getTemplateType();
}

void InjaManager::setCodeRootDir(const QString &dir)
{
    switch(m_type)
    {
    case Template_Somip:
        d->env.set_input_path(QString("%1/data/emoscodebuild/someip/").arg(dir).toStdString());
        break;
    default:
        d->env.set_input_path(QString("%1/data/emoscodebuild/someip/").arg(dir).toStdString());
        break;
    }
}

QString InjaManager::getTemplateDir()
{
    return QString::fromStdString(d->env.get_input_path());
}

void InjaManager::setTypeData()
{
    d->m_dataBuild->setTypeData();
}

void InjaManager::setApiData()
{
    d->m_dataBuild->setApiData();
}

void InjaManager::setIdlData(const EmosTools::IDLStruct& idlStruct)
{
    d->m_dataBuild->setIdlData(idlStruct);
}

void InjaManager::setIdlOldJson(const QString &JsonPath)
{
    QFile File(JsonPath);
    if (!File.open(QFile::ReadOnly)) {
        return ;
    }
    QByteArray array = File.readAll();
    Json dataold = Json::parse(array.toStdString());
    Json data = Json::parse(d->m_dataBuild->getidldata().toStdString());
    d->m_jsonParser->setJsonData(dataold, data);
}

QString InjaManager::getPropertyJson(QString name)
{
    return d->m_jsonParser->getSingleJsonData(PROPERTYLIST, name);
}

QString InjaManager::getThreadJson(QString name)
{
    return d->m_jsonParser->getSingleJsonData(THREADLIST, name);
}

QString InjaManager::getMethodJson(QString name)
{
    return d->m_jsonParser->getSingleJsonData(METHODLIST, name);
}

QString InjaManager::getQuetoServiceJson(QString name)
{
    return d->m_jsonParser->getSingleJsonData(QUETOLIST, name);
}

QStringList InjaManager::getoldPropertyList()
{
    return d->m_jsonParser->getDataList(PROPERTYLIST, false);
}

QStringList InjaManager::getoldThreadList()
{
    return d->m_jsonParser->getDataList(THREADLIST, false);
}

QStringList InjaManager::getoldMethodList()
{
    return d->m_jsonParser->getDataList(METHODLIST, false);
}

QStringList InjaManager::getoldQuetoList()
{
    return d->m_jsonParser->getDataList(QUETOLIST,false);
}

void InjaManager::setSingleData(const QString & key, const QVariant &value)
{
    d->m_dataBuild->setSingleData(key, value);
}

QString InjaManager::getdata()
{
    return d->m_dataBuild->getdata();
}

QString InjaManager::getidldata()
{
    return d->m_dataBuild->getidldata();
}

QString InjaManager::renderFile(const QString &filename, const QString& data)
{
    std::string renderstr;

    QString fileText;
    QFile file(QString("%1%2").arg(QString::fromStdString(d->env.get_input_path())).arg(filename));
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream fileStream(&file);
        fileText = fileStream.readAll();
        file.close();
    }
    else
    {
        BuildoutLogManager::instance()->writeWithTime("File Open Error: " + getTemplateDir() + filename, Utils::ErrorMessageFormat);
        return "";
    }

    if(fileText.isEmpty())
    {
        BuildoutLogManager::instance()->writeWithTime("template file is empty!", Utils::ErrorMessageFormat);
        return "";
    }

    try
    {
        renderstr = d->env.render(fileText.toStdString(), Json::parse(data.toStdString()));
#ifdef QT_DEBUG
        //BuildoutLogManager::instance()->writeWithTime("Json data:\n" + data, Utils::StdOutFormat);
#endif
    }
    catch (inja::ParserError e)
    {
        QString errLog = QString("%1%2; ParserError; %3; line: %4, col: %5").arg(QString::fromStdString(d->env.get_input_path()), filename, QString::fromStdString(e.message)).arg(e.location.line).arg(e.location.column);
        BuildoutLogManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        return "";
    }
    catch (inja::RenderError e)
    {
        QString errLog = QString("%1%2; RenderError; %3; line: %4, col: %5").arg(QString::fromStdString(d->env.get_input_path()), filename, QString::fromStdString(e.message)).arg(e.location.line).arg(e.location.column);
        BuildoutLogManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        BuildoutLogManager::instance()->writeWithTime("Json data:\n" + data, Utils::StdOutFormat);
        return "";
    }
    catch (inja::FileError e)
    {
        QString errLog = QString("FileError: %1;").arg(QString::fromStdString(e.message));
        BuildoutLogManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        return "";
    }
    catch (inja::DataError e)
    {
        QString errLog = QString("%1%2; DataError; %3; line: %4, col: %5").arg(QString::fromStdString(d->env.get_input_path()), filename, QString::fromStdString(e.message)).arg(e.location.line).arg(e.location.column);
        BuildoutLogManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        BuildoutLogManager::instance()->writeWithTime("Json data:\n" + data, Utils::LogMessageFormat);
        return "";
    }
    catch (Json::type_error e)
    {
        QString errLog = QString("%1%2; JsonError; %3; msg: %4").arg(QString::fromStdString(d->env.get_input_path()), filename).arg(e.id).arg(QString::fromStdString(e.what()));
        BuildoutLogManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        return "";
    }
    catch (Json::exception e)
    {
        QString errLog = QString("%1%2; JsonError; %3; msg: %4").arg(QString::fromStdString(d->env.get_input_path()), filename).arg(e.id).arg(QString::fromStdString(e.what()));
        BuildoutLogManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        return "";
    }
    catch (Json::parse_error e)
    {
        QString errLog = QString("%1%2; JsonError; %3; msg: %4").arg(QString::fromStdString(d->env.get_input_path()), filename).arg(e.id).arg(QString::fromStdString(e.what()));
        BuildoutLogManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        return "";
    }
    catch (Json::invalid_iterator e)
    {
        QString errLog = QString("%1%2; JsonError; %3; msg: %4").arg(QString::fromStdString(d->env.get_input_path()), filename).arg(e.id).arg(QString::fromStdString(e.what()));
        BuildoutLogManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        return "";
    }
    catch (Json::out_of_range e)
    {
        QString errLog = QString("%1%2; JsonError; %3; msg: %4").arg(QString::fromStdString(d->env.get_input_path()), filename).arg(e.id).arg(QString::fromStdString(e.what()));
        BuildoutLogManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        return "";
    }
    catch (Json::other_error e)
    {
        QString errLog = QString("%1%2; JsonError; %3; msg: %4").arg(QString::fromStdString(d->env.get_input_path()), filename).arg(e.id).arg(QString::fromStdString(e.what()));
        BuildoutLogManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        return "";
    }

    return QString::fromStdString(renderstr);
}

void* InjaManager::getEnv()
{
    return static_cast<void*>(&d->env);
}

}
