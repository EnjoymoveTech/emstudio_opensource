#include <generaleditor/generaleditor.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/documentmanager.h>
#include <emostools/idlparser.h>
#include <explorer/project.h>
#include <coreplugin/messagemanager.h>
#include <QFile>
#include <QDomDocument>
#include "idleditorconstants.h"
#include "idleditorfactory.h"
#include "idleditorplugin.h"
using namespace Core;
using namespace Utils;

namespace Idl {
namespace Internal {

class IdlEditorPluginPrivate : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(IdlEditor::Internal::IdlEditorPlugin)

public:
    explicit IdlEditorPluginPrivate(IdlEditorPlugin *q);
    Context m_contexts;
    IdlEditorFactory m_editorFactory;
public slots:
    void onIdlNameChange(const QString& from, const QString& to);
    void onAutoCreateNewFile(const Utils::MimeType& type, const void* data, const Core::EditorManager::DescriptionType& fileType, const ProjectExplorer::Project* project);
private:
    void buildXml(const EmosTools::IDLStruct& data, const ProjectExplorer::Project* project);
};

static IdlEditorPluginPrivate *s_d = nullptr;

IdlEditorPluginPrivate::IdlEditorPluginPrivate(IdlEditorPlugin *q)
{
    s_d = this;
    m_contexts.add(Core::Constants::C_EDITORMANAGER);
    m_contexts.add(Constants::C_IDLEDITOR);
    connect(Core::DocumentManager::instance(),&Core::DocumentManager::allDocumentsRenamed,
            s_d, &IdlEditorPluginPrivate::onIdlNameChange);
    connect(Core::EditorManager::instance(), &Core::EditorManager::autoCreateNewFile, this, &IdlEditorPluginPrivate::onAutoCreateNewFile);
}

void IdlEditorPluginPrivate::onIdlNameChange(const QString &from, const QString &to)
{
    QFile file(to);
    QDomDocument* doc = new QDomDocument;
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return;
    }
    if(!doc->setContent(&file)){
        return;
    }
    file.close();

    QDomElement deRootNode = doc->documentElement();
    QStringList cutpath = to.split("/");
    QString name = cutpath.at(cutpath.size() - 1);
    name = name.mid(0, name.indexOf("."));
    QString oid = deRootNode.attribute("oid");
    oid = oid.mid(0,oid.lastIndexOf(".") + 1);
    oid += name.toLower();
    deRootNode.setAttribute("oid", oid);
    deRootNode.setAttribute("project:name", name);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        return;
    }
    QTextStream out(&file);
    out << Utils::XmlFileSaver::serializeStr(doc->toString());
    out.flush();
    file.close();
    delete doc;
}

IdlEditorPlugin::IdlEditorPlugin()
{
}

IdlEditorPlugin::~IdlEditorPlugin()
{
}

bool IdlEditorPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    s_d = new IdlEditorPluginPrivate(this);

    return true;
}

void IdlEditorPlugin::extensionsInitialized()
{
    //DesignMode::setDesignModeIsRequired();
}

ExtensionSystem::IPlugin::ShutdownFlag IdlEditorPlugin::aboutToShutdown()
{
    return SynchronousShutdown;
}

void IdlEditorPluginPrivate::onAutoCreateNewFile(const MimeType &type, const void *data, const Core::EditorManager::DescriptionType &fileType, const ProjectExplorer::Project *project)
{
    if(!type.name().contains("emos.idl.xml"))
        return;
    EmosTools::IDLStruct idlData = *(EmosTools::IDLStruct*)data;
    buildXml(idlData, project);
}

void IdlEditorPluginPrivate::buildXml(const EmosTools::IDLStruct &data, const ProjectExplorer::Project *project)
{
    QDomDocument* idlDoc = new QDomDocument();
    QDomElement rootNode = idlDoc->createElement("emos:idl");
    idlDoc->appendChild(rootNode);
    rootNode.setAttribute("project:name", data.name);
    rootNode.setAttribute("oid", data.oid);
    rootNode.setAttribute("priority", QString::number(data.priority));

    QMapIterator<QString, int> serviceIt(data.service);
    while (serviceIt.hasNext())
    {
        serviceIt.next();
        QDomElement serviceNode = idlDoc->createElement("Service");
        rootNode.appendChild(serviceNode);
        serviceNode.setAttribute("name", serviceIt.key());
    }
    QMapIterator<QString, int> quoteServiceIt(data.quoteService);
    while (quoteServiceIt.hasNext())
    {
        quoteServiceIt.next();
        QDomElement quoteNode = idlDoc->createElement("QuoteService");
        rootNode.appendChild(quoteNode);
        quoteNode.setAttribute("name", quoteServiceIt.key());
    }
    QMapIterator<QString, QString> propertyIt(data.property);
    while (propertyIt.hasNext())
    {
        propertyIt.next();
        QDomElement propertyNode = idlDoc->createElement("Property");
        rootNode.appendChild(propertyNode);
        propertyNode.setAttribute("name", propertyIt.key());
        propertyNode.setAttribute("type", propertyIt.value());
    }
    for(auto &thread : data.thread)
    {
        QDomElement threadNode = idlDoc->createElement("Thread");
        rootNode.appendChild(threadNode);
        threadNode.setAttribute("name", thread.name);
        threadNode.setAttribute("type", thread.type);
        threadNode.setAttribute("data", thread.data);
    }

    QString fileName = project->projectDirectory().toString() + QString("/2_component_design/%1.idl").arg(data.name);
    if(QFile::exists(fileName))
    {
        QFile::remove(fileName);
    }
    QFile xml(fileName);
    if(!xml.open(QIODevice::ReadWrite))
    {
        QString errLog = "FileError: Can't open file " + fileName;
        Core::MessageManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        return;
    }
    QTextStream stream(&xml);
    idlDoc->save(stream, 4, QDomNode::EncodingFromTextStream);
    xml.close();
}


} // namespace Internal
} // namespace Idl
