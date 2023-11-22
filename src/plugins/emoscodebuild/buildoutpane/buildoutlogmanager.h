#ifndef BUILDOUTLOGMANAGER_H
#define BUILDOUTLOGMANAGER_H

#include "coreplugin/ioutputpane.h"
#include "coreplugin/outputwindow.h"
#include <emoscodebuild/emoscodebuild_global.h>

#include <QMetaType>
#include <QObject>

QT_BEGIN_NAMESPACE
class QFont;
QT_END_NAMESPACE

namespace EmosCodeBuild {

namespace Internal { class EmosCodeBuildPlugin; }

class EMOSCODEBUILD_EXPORT BuildoutLogManager : public QObject
{
    Q_OBJECT

public:
    static BuildoutLogManager *instance();

    enum PrintToOutputPaneFlag {
        NoModeSwitch   = Core::IOutputPane::NoModeSwitch,
        ModeSwitch     = Core::IOutputPane::ModeSwitch,
        WithFocus      = Core::IOutputPane::WithFocus,
        EnsureSizeHint = Core::IOutputPane::EnsureSizeHint,
        Silent         = 256,
        Flash          = 512
    };

    Q_DECLARE_FLAGS(PrintToOutputPaneFlags, PrintToOutputPaneFlag)

    static void showOutputPane(PrintToOutputPaneFlags flags = NoModeSwitch);
    static void hideOutputPane();

    static void setFont(const QFont &font);
    static void setWheelZoomEnabled(bool enabled);

    static void writeMessages(const QStringList &messages, Utils::OutputFormat foramt = Utils::DebugFormat,
                              PrintToOutputPaneFlags flags = NoModeSwitch);
    static void write(const QString &text, Utils::OutputFormat foramt = Utils::DebugFormat, PrintToOutputPaneFlags flags = NoModeSwitch);
    static void writeWithTime(const QString &text, Utils::OutputFormat foramt = Utils::DebugFormat, PrintToOutputPaneFlags flags = NoModeSwitch);

private:
    BuildoutLogManager();
    ~BuildoutLogManager() override;

    static void doWrite(const QString &text, Utils::OutputFormat foramt, PrintToOutputPaneFlags flags);

    static void init();
    friend class EmosCodeBuild::Internal::EmosCodeBuildPlugin;
};

} // namespace EmosCodeBuild

Q_DECLARE_METATYPE(EmosCodeBuild::BuildoutLogManager::PrintToOutputPaneFlags)


#endif // BUILDOUTMANAGER_H
