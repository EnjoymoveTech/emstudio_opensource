#include "emoscodebuild.h"
#include "emoscodebuildconstants.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <pluginmanager/pluginmanager.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>

#include "injamanager.h"
#include "buildoutpane/buildoutlogmanager.h"

namespace EmosCodeBuild {
namespace Internal {


class EmosCodeBuildPluginPrivate : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(EmosCodeBuild::Internal::EmosCodeBuildPlugin)

public:
    explicit EmosCodeBuildPluginPrivate();

    EmosCodeBuild::BuildoutLogManager* m_manager = nullptr;
};


static EmosCodeBuildPluginPrivate *s_d = nullptr;

EmosCodeBuildPluginPrivate::EmosCodeBuildPluginPrivate()
{
    s_d = this;
}


EmosCodeBuildPlugin::EmosCodeBuildPlugin()
{

}

EmosCodeBuildPlugin::~EmosCodeBuildPlugin()
{
    delete s_d->m_manager;
    delete s_d;
    s_d = NULL;
}

bool EmosCodeBuildPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    s_d = new EmosCodeBuildPluginPrivate;
    auto action = new QAction(tr("Inja Debug"), this);
    Core::Command *cmd = Core::ActionManager::registerAction(action, Constants::ACTION_CODEDEBUG_ID,
                                                             Core::Context(Core::Constants::C_GLOBAL));
    connect(action, &QAction::triggered, this, &EmosCodeBuildPlugin::triggerAction);

    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("CodeBuild"));
    menu->addAction(cmd);
    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

    s_d->m_manager = new BuildoutLogManager;
    s_d->m_manager->init();
    QFont font;
    font.setPixelSize(10);
    font.setFamily("Courier");
    s_d->m_manager->setFont(font);
    return true;
}

void EmosCodeBuildPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag EmosCodeBuildPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void EmosCodeBuildPlugin::triggerAction()
{
    InjaManager::showDebugDialog();
}

} // namespace Internal
} // namespace EmosCodeBuild
