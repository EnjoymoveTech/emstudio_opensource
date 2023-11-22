#include "welcomewidget.h"

#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <coreplugin/imode.h>
#include <coreplugin/iwelcomepage.h>
#include <coreplugin/iwizardfactory.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/editormanager/editormanager.h>

#include <utils/algorithm.h>
#include <utils/fileutils.h>
#include <utils/hostosinfo.h>
#include <utils/icon.h>
#include <utils/porting.h>
#include <utils/qtcassert.h>
#include <utils/styledbar.h>
#include <utils/theme/theme.h>
#include <utils/treemodel.h>
#include <utils/emoswidgets/iconlinktext.h>
#include <coreplugin/minisplitter.h>

#include <utils/emoswidgets/flatbutton.h>
#include <utils/emoswidgets/widgetsstyle.h>
#include <utils/emoswidgets/widgetstheme.h>

#include <QDesktopServices>
#include <QHeaderView>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollArea>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QPushButton>

#include "docwidget.h"

using namespace Core;
using namespace Utils;

namespace Helpme {
namespace Internal {

const char currentPageSettingsKeyC[] = "Welcome2Tab";

static QColor themeColor(Theme::Color role)
{
    return Utils::creatorTheme()->color(role);
}

class SideBar : public QWidget
{
    Q_OBJECT
public:
    SideBar(QWidget *parent)
        : QWidget(parent)
    {
        setAutoFillBackground(true);
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        setPalette(themeColor(Theme::Welcome_BackgroundColor));

        auto vbox = new QVBoxLayout(this);
        vbox->setSpacing(0);
        vbox->setContentsMargins(0, 0, 0, 0);

        IWelcomePage *page = IWelcomePage::allWelcomePages().first();
        if(page)
        {
            QWidget *stackPage = page->createWidget();
            stackPage->setAutoFillBackground(true);
            vbox->addWidget(stackPage);
        }
        QAction *bugReportAction = new QAction(tr("Bug Report..."), this);
        bugReportAction->setMenuRole(QAction::ApplicationSpecificRole);
        Core::Command *bugReportCommand = Core::ActionManager::registerAction(bugReportAction, "Report.BugReport");
        connect(bugReportAction, &QAction::triggered, this, &SideBar::slotShowBugReportWidget);
        ActionContainer *const helpContainer = ActionManager::actionContainer(Core::Constants::M_HELP);
        helpContainer->addAction(bugReportCommand, Constants::G_HELP_UPDATES);
    }
public slots:
    void slotShowBugReportWidget()
    {
        if (!m_bugreportwidget)
        {
            m_bugreportwidget = new BugReportWidget(this);
            m_bugreportwidget->setWindowFlags(Qt::Window);
        }
        m_bugreportwidget->show();
    }
private:
    BugReportWidget* m_bugreportwidget = nullptr;
};

WelcomeWidget::WelcomeWidget(QWidget *parent)
    : QWidget{parent}
{
    QPalette palette = creatorTheme()->palette();
    palette.setColor(QPalette::Window, themeColor(Theme::Welcome_BackgroundColor));
    setPalette(palette);

    Core::MiniSplitter* splitter = new Core::MiniSplitter(this);

    m_sideBar = new SideBar(this);
    auto scrollableSideBar = new QScrollArea(this);
    scrollableSideBar->setWidget(m_sideBar);
    scrollableSideBar->setWidgetResizable(true);
    scrollableSideBar->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollableSideBar->setFrameShape(QFrame::NoFrame);

    m_docWidget = new DocWidget(this);

    splitter->addWidget(scrollableSideBar);
    splitter->addWidget(m_docWidget);
    splitter->setSizes(QList<int>() << 300 << 200);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(splitter);
}

WelcomeWidget::~WelcomeWidget()
{
    QSettings *settings = ICore::settings();
}

void WelcomeWidget::inits()
{
    QSettings *settings = ICore::settings();
    Core::EditorManager::setEmptyWidget(this);
}
}}

#include "welcomewidget.moc"
