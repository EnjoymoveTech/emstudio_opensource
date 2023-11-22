#include "emoscodebuild_global.h"
#include "buildoutlogmanager.h"
#include "buildoutpane.h"

#include <pluginmanager/pluginmanager.h>
#include <utils/qtcassert.h>

#include <QFont>
#include <QThread>
#include <QTime>
#include <QTimer>

namespace EmosCodeBuild {

static BuildoutLogManager *m_instance = nullptr;
static Internal::BuildoutPane *m_buildoutPane = nullptr;

BuildoutLogManager *BuildoutLogManager::instance()
{
    return m_instance;
}

void BuildoutLogManager::showOutputPane(BuildoutLogManager::PrintToOutputPaneFlags flags)
{
    QTC_ASSERT(m_buildoutPane, return);

    if (flags & Flash) {
        m_buildoutPane->flash();
    } else if (flags & Silent) {
        // Do nothing
    } else {
        m_buildoutPane->popup(Core::IOutputPane::Flag(int(flags)));
    }
}

void BuildoutLogManager::hideOutputPane()
{
    m_buildoutPane->hide();
}

BuildoutLogManager::BuildoutLogManager()
{
    m_instance = this;
    m_buildoutPane = nullptr;
    qRegisterMetaType<BuildoutLogManager::PrintToOutputPaneFlags>();
}

BuildoutLogManager::~BuildoutLogManager()
{
    if (m_buildoutPane) {
        ExtensionSystem::PluginManager::removeObject(m_buildoutPane);
        delete m_buildoutPane;
    }
    m_instance = nullptr;
}

void BuildoutLogManager::init()
{
    m_buildoutPane = new Internal::BuildoutPane;
    ExtensionSystem::PluginManager::addObject(m_buildoutPane);
}

void BuildoutLogManager::setFont(const QFont &font)
{
    QTC_ASSERT(m_buildoutPane, return);

    m_buildoutPane->setFont(font);
}

void BuildoutLogManager::setWheelZoomEnabled(bool enabled)
{
    QTC_ASSERT(m_buildoutPane, return);

    m_buildoutPane->setWheelZoomEnabled(enabled);
}

void BuildoutLogManager::writeMessages(const QStringList &messages, Utils::OutputFormat foramt, PrintToOutputPaneFlags flags)
{
    write(messages.join('\n'), foramt, flags);
}

void BuildoutLogManager::write(const QString &text, Utils::OutputFormat foramt, PrintToOutputPaneFlags flags)
{
    if (QThread::currentThread() == instance()->thread())
        doWrite(text, foramt, flags);
    else
        QTimer::singleShot(0, instance(), [text, foramt, flags] { doWrite(text, foramt, flags); });
}

void BuildoutLogManager::writeWithTime(const QString &text, Utils::OutputFormat foramt, PrintToOutputPaneFlags flags)
{
    const QString timeStamp = QTime::currentTime().toString("HH:mm:ss:zzz ");
    write(timeStamp + text, foramt, flags);
}

void BuildoutLogManager::doWrite(const QString &text, Utils::OutputFormat foramt, PrintToOutputPaneFlags flags)
{
    QTC_ASSERT(m_buildoutPane, return);

    showOutputPane(flags);
    m_buildoutPane->append(text + '\n', foramt);
}
}
