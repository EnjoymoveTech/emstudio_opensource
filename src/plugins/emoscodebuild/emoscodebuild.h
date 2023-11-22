#ifndef EMOSCODEBUILDPLUGIN_H
#define EMOSCODEBUILDPLUGIN_H
#include <QDomDocument>
#include "emoscodebuild_global.h"

#include <pluginmanager/iplugin.h>


namespace EmosCodeBuild {
class BuildoutLogManager;
namespace Internal {

class EmosCodeBuildPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.EmStudioPlugin" FILE "EmosCodeBuild.json")

public:
    EmosCodeBuildPlugin();
    ~EmosCodeBuildPlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    void triggerAction();

    class EmosCodeBuildPluginPrivate *d = nullptr;
};

} // namespace Internal
} // namespace EmosCodeBuild

#endif // EMOSCODEBUILDPLUGIN_H
