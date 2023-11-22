#ifndef EMOSTOOLSPLUGIN_H
#define EMOSTOOLSPLUGIN_H
#include <QDomDocument>
#include "emostools_global.h"

#include <pluginmanager/iplugin.h>

namespace EmosTools {
namespace Internal {

class EmosToolsPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.EmStudioPlugin" FILE "EmosTools.json")

public:
    EmosToolsPlugin();
    ~EmosToolsPlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    void triggerAction();
};

} // namespace Internal
} // namespace EmosCodeBuild

#endif // EMOSCODEBUILDPLUGIN_H
