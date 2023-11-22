#ifndef DESCRIPTIONEDITORPLUGIN_H
#define DESCRIPTIONEDITORPLUGIN_H
#include <QDomDocument>
#include "descriptioneditor_global.h"

#include <pluginmanager/iplugin.h>

namespace DescriptionEditor {
namespace Internal {

class DescriptionEditorPluginPrivate;
class DescriptionEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.EmStudioPlugin" FILE "DescriptionEditor.json")

public:
    DescriptionEditorPlugin();
    ~DescriptionEditorPlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    void triggerAction();

    DescriptionEditorPluginPrivate* d = nullptr;
};

} // namespace Internal
} // namespace DescriptionEditor

#endif // DESCRIPTIONEDITORPLUGIN_H
