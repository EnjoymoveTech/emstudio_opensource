#include "descriptioneditorplugin.h"
#include "descriptioneditorfactory.h"
#include "descriptioneditorconstants.h"
#include "descriptioneditorwidget.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <explorer/project.h>
#include <explorer/explorer.h>
#include <explorer/projecttree.h>
#include <explorer/projectexplorerconstants.h>
#include <explorer/projectnodes.h>

#include <emostools/api_description_parser.h>
#include <emostools/type_description_parser.h>
#include <coreplugin/messagemanager.h>
#include <QDate>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>

using namespace ProjectExplorer;
using namespace Core;
using namespace Utils;

namespace DescriptionEditor {
namespace Internal {

static const char settingsGroupC[] = "Description";

class DescriptionEditorPluginPrivate : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(DescriptionEditor::Internal::DescriptionEditorPlugin)

public:
    explicit DescriptionEditorPluginPrivate(DescriptionEditorPlugin *q);

    void fullInit();

    Context m_contexts;
    DescriptionEditorfactory m_editorFactory;
public slots:
    void slotCreateNewFile(const Utils::MimeType& type, const void* data, const Core::EditorManager::DescriptionType& fileType, const ProjectExplorer::Project* project);
private:
    void buildApiXml(const QList<EmosTools::tSrvParsingService>& data, const ProjectExplorer::Project* project);
    void buildTypeXml(const EmosTools::tTypeData &data, const ProjectExplorer::Project *project);
    void buildData(const QList<EmosTools::tSrvParsingService>& data, QDomElement& node);
    void buildDataType(const QList<EmosTools::tDatatypeData> &data, QDomElement &node);
    void buildStream(QDomElement &node);
    void buildEnum(const QList<EmosTools::tEnumData> &data, QDomElement &node);
    void buildMacro(const QList<EmosTools::tMacroData> &data, QDomElement &node);
    void buildStruct(const QList<EmosTools::tStructData> &data, QDomElement &node);
    void buildInclude(const QString &data, QDomElement &node);
    void buildTypedef(const QList<EmosTools::tTypedefData> &data, QDomElement &node);
    QDomDocument m_doc;
};

static DescriptionEditorPluginPrivate *s_d = nullptr;

DescriptionEditorPluginPrivate::DescriptionEditorPluginPrivate(DescriptionEditorPlugin *q)
{
    s_d = this;


//    QObject::connect(ProjectTree::instance(), &ProjectTree::currentProjectChanged, [this](ProjectExplorer::Project *project)
//    {

//    });
//    QObject::connect(EditorManager::instance(), &EditorManager::editorsClosed, [this](QList<Core::IEditor *> editors)
//    {

//    });

    m_contexts.add(Core::Constants::C_EDITORMANAGER);
    m_contexts.add(Constants::DESCRIPTION_EDITOR_ID);

    fullInit();
}

void DescriptionEditorPluginPrivate::fullInit()
{
    QObject::connect(EditorManager::instance(), &EditorManager::editorsClosed,
                         [this] (const QList<IEditor *> editors) {
            for (IEditor *editor : editors)
            {
                //m_editorWidget->removeWindowEditor(editor);
            }

    });

    QSettings *settings = ICore::settings();
    settings->beginGroup(settingsGroupC);
    settings->endGroup();
    connect(Core::EditorManager::instance(), &Core::EditorManager::autoCreateNewFile, this, &DescriptionEditorPluginPrivate::slotCreateNewFile);
}

void DescriptionEditorPluginPrivate::slotCreateNewFile(const MimeType &type, const void *data, const EditorManager::DescriptionType &fileType, const Project *project)
{
    if(!type.name().contains(".description"))
        return;
    if(fileType == EditorManager::DescriptionType::Api)
    {
        QList<EmosTools::tSrvService> srvData = *(QList<EmosTools::tSrvService>*)data;
        buildApiXml(srvData, project);
    }
    else if(fileType == EditorManager::DescriptionType::Type)
    {
        EmosTools::tTypeData typeData = *(EmosTools::tTypeData*)data;
        buildTypeXml(typeData, project);
    }
}

void DescriptionEditorPluginPrivate::buildApiXml(const QList<EmosTools::tSrvService> &data, const ProjectExplorer::Project *project)
{
    QDomDocument* apiDoc = new QDomDocument();
    QDomElement rootNode = apiDoc->createElement("emos:ddl");
    apiDoc->appendChild(rootNode);
    rootNode.setAttribute("xmlns:emos", "emos");

    QDomElement headerElement = apiDoc->createElement("header");rootNode.appendChild(headerElement);
    QDomElement versionElement = apiDoc->createElement("version");versionElement.appendChild(apiDoc->createTextNode("1.0"));headerElement.appendChild(versionElement);
    QDomElement authorElement = apiDoc->createElement("author");authorElement.appendChild(apiDoc->createTextNode("EMOS"));headerElement.appendChild(authorElement);
    QDomElement date_creationElement = apiDoc->createElement("date_creation");date_creationElement.appendChild(apiDoc->createTextNode(QDate::currentDate().toString("yyyy-MM-dd")));headerElement.appendChild(date_creationElement);
    QDomElement date_changeElement = apiDoc->createElement("date_change");date_changeElement.appendChild(apiDoc->createTextNode(QDate::currentDate().toString("yyyy-MM-dd")));headerElement.appendChild(date_changeElement);
    QDomElement descriptionElement = apiDoc->createElement("description");descriptionElement.appendChild(apiDoc->createTextNode("NULL"));headerElement.appendChild(descriptionElement);
    QDomElement typenameElement = apiDoc->createElement("typename");typenameElement.appendChild(apiDoc->createTextNode("type"));headerElement.appendChild(typenameElement);

    QDomElement servicesNode = apiDoc->createElement("services");rootNode.appendChild(servicesNode);
    buildData(data, servicesNode);

    QString fileName = project->projectDirectory().toString() + "/1_interface_design/api.description";
    QFile xml(fileName);
    if(!xml.open(QIODevice::WriteOnly))
    {
        QString errLog = "FileError: Can't open file " + fileName;
        Core::MessageManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        return;
    }
    QTextStream stream(&xml);
    apiDoc->save(stream, 4, QDomNode::EncodingFromTextStream);
    xml.close();
}

void DescriptionEditorPluginPrivate::buildTypeXml(const EmosTools::tTypeData &data, const ProjectExplorer::Project* project)
{
    QDomDocument* typeDoc = new QDomDocument();
    QDomElement rootNode = typeDoc->createElement("emos:ddl");
    typeDoc->appendChild(rootNode);
    rootNode.setAttribute("xmlns:emos", "emos");

    QDomElement headerElement = typeDoc->createElement("header");rootNode.appendChild(headerElement);
    QDomElement versionElement = typeDoc->createElement("version");versionElement.appendChild(typeDoc->createTextNode("1.0"));headerElement.appendChild(versionElement);
    QDomElement authorElement = typeDoc->createElement("author");authorElement.appendChild(typeDoc->createTextNode("EMOS"));headerElement.appendChild(authorElement);
    QDomElement date_creationElement = typeDoc->createElement("date_creation");date_creationElement.appendChild(typeDoc->createTextNode(QDate::currentDate().toString("yyyy-MM-dd")));headerElement.appendChild(date_creationElement);
    QDomElement date_changeElement = typeDoc->createElement("date_change");date_changeElement.appendChild(typeDoc->createTextNode(QDate::currentDate().toString("yyyy-MM-dd")));headerElement.appendChild(date_changeElement);
    QDomElement descriptionElement = typeDoc->createElement("description");descriptionElement.appendChild(typeDoc->createTextNode("NULL"));headerElement.appendChild(descriptionElement);

    QDomElement datatypesNode = typeDoc->createElement("datatypes");rootNode.appendChild(datatypesNode);
    buildDataType(data.datatypes, datatypesNode);
    QDomElement streamsNode = typeDoc->createElement("streams");rootNode.appendChild(streamsNode);
    buildStream(streamsNode);
    QDomElement enumsNode = typeDoc->createElement("enums");rootNode.appendChild(enumsNode);
    buildEnum(data.enums, enumsNode);
    QDomElement macrosNode = typeDoc->createElement("macros");rootNode.appendChild(macrosNode);
    buildMacro(data.macros, macrosNode);
    QDomElement structsNode = typeDoc->createElement("structs");rootNode.appendChild(structsNode);
    buildStruct(data.structs, structsNode);
    QDomElement includesNode = typeDoc->createElement("includes");rootNode.appendChild(includesNode);
    buildInclude(data.includes, includesNode);
    QDomElement typedefsNode = typeDoc->createElement("typedefs");rootNode.appendChild(typedefsNode);
    buildTypedef(data.typedefs, typedefsNode);

    QString fileName = project->projectDirectory().toString() + "/1_interface_design/type.description";
    QFile xml(fileName);
    if(!xml.open(QIODevice::WriteOnly))
    {
        QString errLog = "FileError: Can't open file " + fileName;
        Core::MessageManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        return;
    }
    QTextStream stream(&xml);
    typeDoc->save(stream, 4, QDomNode::EncodingFromTextStream);
    xml.close();
}

void DescriptionEditorPluginPrivate::buildDataType(const QList<EmosTools::tDatatypeData> &data, QDomElement &node)
{
    QDomElement datatypeNode;
    for(int i = 0; i < data.size(); i++)
    {
        datatypeNode = m_doc.createElement("datatype");
        datatypeNode.setAttribute("name", data.at(i).name);
        datatypeNode.setAttribute("size", data.at(i).size);
        datatypeNode.setAttribute("description", "predefined  " + data.at(i).name + " datatype");
        node.appendChild(datatypeNode);
    }
}

void DescriptionEditorPluginPrivate::buildStream(QDomElement &node)
{
    QDomElement streamNode = m_doc.createElement("stream");
    streamNode.setAttribute("name", "Image");
    streamNode.setAttribute("type", "tTestData");
    node.appendChild(streamNode);
}

void DescriptionEditorPluginPrivate::buildEnum(const QList<EmosTools::tEnumData> &data, QDomElement &node)
{
    QDomElement enumNode;
    for(int i = 0; i < data.size(); i++)
    {
        enumNode = m_doc.createElement("enum");
        enumNode.setAttribute("name", data.at(i).name);
        enumNode.setAttribute("filename", data.at(i).filename);
        enumNode.setAttribute("type", data.at(i).type);
        QDomElement memberNode;
        for(int j = 0; j < data.at(i).enuminfo.size(); j++)
        {
            memberNode = m_doc.createElement("element");
            EmosTools::tEnumInfo member = data.at(i).enuminfo.at(j);
            memberNode.setAttribute("name", member.name);
            memberNode.setAttribute("value", member.value);
            memberNode.setAttribute("description", "");
            enumNode.appendChild(memberNode);
        }
        node.appendChild(enumNode);
    }
}

void DescriptionEditorPluginPrivate::buildData(const QList<EmosTools::tSrvParsingService> &data, QDomElement &node)
{
    auto direction2String = [](EmosTools::tParamDirection direction)
    {
        switch(direction)
        {
        case EmosTools::Param_In:
                return QString("in");
        case EmosTools::Param_Out:
                return QString("out");
        case EmosTools::Param_InOut:
                return QString("inout");
        }
        return QString("in");
    };

    for(auto& srv : data)
    {
        QDomElement serviceNode = m_doc.createElement("service");
        node.appendChild(serviceNode);
        serviceNode.setAttribute("name", srv.name);
        serviceNode.setAttribute("id", QString::number(srv.id));
        serviceNode.setAttribute("inprocess", (srv.inProcess)?QString("1"):QString("0"));
        serviceNode.setAttribute("interface_type", srv.interfaceType);
        serviceNode.setAttribute("version", srv.version);
        for(auto& method : srv.functions)
        {
            QDomElement elementNode = m_doc.createElement("element");
            serviceNode.appendChild(elementNode);
            elementNode.setAttribute("name", method.name);
            elementNode.setAttribute("id", QString::number(method.id));
            elementNode.setAttribute("ret", method.returntype);
            elementNode.setAttribute("type", "Method");
            for(auto& para : method.params)
            {
                QDomElement paraNode = m_doc.createElement("para");
                elementNode.appendChild(paraNode);
                paraNode.setAttribute("name", para.name);
                paraNode.setAttribute("version", QString::number(para.version));
                paraNode.setAttribute("size", QString::number(para.size));
                paraNode.setAttribute("type", para.type);
                paraNode.setAttribute("direction", direction2String(para.direction));
            }
        }
        for(auto& event : srv.events)
        {
            QDomElement elementNode = m_doc.createElement("element");
            serviceNode.appendChild(elementNode);
            elementNode.setAttribute("name", event.name);
            elementNode.setAttribute("id", QString::number(event.id));
            elementNode.setAttribute("groupid", QString::number(event.groupid));
            elementNode.setAttribute("data", event.data);
            elementNode.setAttribute("type", "Event");
        }
        for(auto& field : srv.fields)
        {
            QDomElement elementNode = m_doc.createElement("element");
            serviceNode.appendChild(elementNode);
            elementNode.setAttribute("name", field.name);
            elementNode.setAttribute("id", QString::number(field.id));
            elementNode.setAttribute("selecttype", QString::number(field.type));
            elementNode.setAttribute("data", field.data);
            elementNode.setAttribute("type", "Field");
        }
        for(auto& pin : srv.pins)
        {
            QDomElement elementNode = m_doc.createElement("element");
            serviceNode.appendChild(elementNode);
            elementNode.setAttribute("name", pin.name);
            elementNode.setAttribute("id", QString::number(pin.id));
            elementNode.setAttribute("data", pin.data);
            elementNode.setAttribute("type", "Pin");
        }
    }
}

void DescriptionEditorPluginPrivate::buildMacro(const QList<EmosTools::tMacroData> &data, QDomElement &node)
{
    QDomElement macroNode;
    for(int i = 0; i < data.size(); i++)
    {
        macroNode = m_doc.createElement("macro");
        macroNode.setAttribute("name", data.at(i).name);
        macroNode.setAttribute("value", data.at(i).value);
        macroNode.setAttribute("description", "");
        node.appendChild(macroNode);
    }
}

void DescriptionEditorPluginPrivate::buildStruct(const QList<EmosTools::tStructData> &data, QDomElement &node)
{
    QDomElement structNode;
    for(int i = 0; i < data.size(); i++)
    {
        structNode = m_doc.createElement("struct");
        structNode.setAttribute("name", data.at(i).name);
        structNode.setAttribute("size", data.at(i).size);
        structNode.setAttribute("version", data.at(i).version);
        structNode.setAttribute("filename", data.at(i).filename);
        structNode.setAttribute("alignment", data.at(i).alignment);
        QDomElement memberNode;
        for(int j = 0; j < data.at(i).structInfo.size(); j++)
        {
            memberNode = m_doc.createElement("element");
            EmosTools::tStructInfo member = data.at(i).structInfo.at(j);
            memberNode.setAttribute("name", member.name);
            memberNode.setAttribute("type", member.type);
            memberNode.setAttribute("alignment", data.at(i).alignment);
            memberNode.setAttribute("byteorder", "LE");
            memberNode.setAttribute("arraysize", member.arraysize);
            memberNode.setAttribute("bytepos", 0);
            memberNode.setAttribute("description", "");
            structNode.appendChild(memberNode);
        }
        node.appendChild(structNode);
    }
}

void DescriptionEditorPluginPrivate::buildInclude(const QString &data, QDomElement &node)
{
    node.setAttribute("text", data);
}

void DescriptionEditorPluginPrivate::buildTypedef(const QList<EmosTools::tTypedefData> &data, QDomElement &node)
{
    QDomElement typedefNode;
    for(int i = 0; i < data.size(); i++)
    {
        typedefNode = m_doc.createElement("typedef");
        typedefNode.setAttribute("name", data.at(i).name);
        typedefNode.setAttribute("value", data.at(i).value);
        node.appendChild(typedefNode);
    }
}

DescriptionEditorPlugin::DescriptionEditorPlugin()
{
    // Create your members
}

DescriptionEditorPlugin::~DescriptionEditorPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool DescriptionEditorPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    s_d = new DescriptionEditorPluginPrivate(this);

//    auto action = new QAction(tr("EmosCodeBuild Action"), this);
//    Core::Command *cmd = Core::ActionManager::registerAction(action, Constants::ACTION_ID,
//                                                             Core::Context(Core::Constants::C_GLOBAL));
//    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Meta+A")));
//    connect(action, &QAction::triggered, this, &DescriptionEditorPlugin::triggerAction);

//    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
//    menu->menu()->setTitle(tr("EmosCodeBuild"));
//    menu->addAction(cmd);
//    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

    return true;
}

void DescriptionEditorPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag DescriptionEditorPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void DescriptionEditorPlugin::triggerAction()
{
    QMessageBox::information(Core::ICore::mainWindow(),
                             tr("Action Triggered"),
                             tr("This is an action from EmosCodeBuild."));
}

} // namespace Internal
} // namespace DescriptionEditor
