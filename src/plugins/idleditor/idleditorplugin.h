#ifndef IDLEDITORPLUGIN_H
#define IDLEDITORPLUGIN_H
#include <pluginmanager/iplugin.h>
enum IDLSubWindows
{
    WidgetBoxSubWindow,
    ObjectInspectorSubWindow,
    PropertyEditorSubWindow,
    IdlSubWindowCount
};
namespace Idl {
namespace Internal {

class IdlEditorPluginPrivate;
class IdlEditorPlugin final : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.EmStudioPlugin" FILE "idleditor.json")

public:
    IdlEditorPlugin();
    ~IdlEditorPlugin() override;
    bool initialize(const QStringList &arguments, QString *errorMessage = nullptr) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;
private:
    class IdlEditorPluginPrivate *d = nullptr;
};

} // namespace Internal
} // namespace Idl

#endif
