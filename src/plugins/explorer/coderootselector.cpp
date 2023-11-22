/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "buildconfiguration.h"
#include "buildmanager.h"
#include "deployconfiguration.h"
#include "kit.h"
#include "kitmanager.h"
#include "coderootselector.h"
#include "explorer.h"
#include "projectexplorericons.h"
#include "project.h"
#include "projectmodels.h"
#include "runconfiguration.h"
#include "session.h"
#include "target.h"

#include <utils/algorithm.h>
#include <utils/itemviews.h>
#include <utils/stringutils.h>
#include <utils/styledbar.h>
#include <utils/stylehelper.h>
#include <utils/theme/theme.h>
#include <utils/utilsicons.h>

#include <coreplugin/icore.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/modemanager.h>

#include <QGuiApplication>
#include <QTimer>
#include <QLayout>
#include <QLabel>
#include <QList>
#include <QListWidget>
#include <QStatusBar>
#include <QKeyEvent>
#include <QPainter>
#include <QPixmap>
#include <QStyleFactory>
#include <QAction>
#include <QItemDelegate>
#include <QToolButton>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QDesktopServices>
#include <QUrl>
#include <QProxyStyle>

using namespace Utils;

namespace ProjectExplorer {
namespace Internal {

static QString codeRootPath;

class QtButtonStyleProxy : public QProxyStyle
{
public:
    /**
    Default constructor.
    */
    QtButtonStyleProxy()
     : QProxyStyle()
    {
    }

    virtual void drawItemText(QPainter *painter, const QRect &rect,
     int flags, const QPalette &pal, bool enabled,
     const QString &text, QPalette::ColorRole textRole) const
    {
     flags |= Qt::TextWordWrap;
     QProxyStyle::drawItemText(painter, rect, flags, pal, enabled, text, textRole);
    }

private:
    Q_DISABLE_COPY(QtButtonStyleProxy)
};

CodeRootSelector::CodeRootSelector(QAction *targetSelectorAction, QWidget *parent) :
    QWidget(parent),
    m_projectAction(targetSelectorAction)
{
    setProperty("panelwidget", true);
    setContentsMargins(QMargins(0, 1, 1, 8));
    setWindowFlags(Qt::Popup);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);

    targetSelectorAction->setIcon(QIcon(":/explorer/images/mode_project.png"));
    targetSelectorAction->setProperty("titledAction", true);
    m_projectAction->setProperty("heading", "CodeRoot");
    m_projectAction->setProperty("subtitle", tr("Unconfig"));

    m_summaryLabel = new QToolButton(this);
    m_summaryLabel->setStyle(new QtButtonStyleProxy());
    m_summaryLabel->setFixedWidth(250);
    m_summaryLabel->setContentsMargins(3, 3, 3, 3);
    QPalette pal = m_summaryLabel->palette();
    pal.setColor(QPalette::Window, Utils::StyleHelper().baseColor());
    m_summaryLabel->setPalette(pal);
    m_summaryLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    connect(m_summaryLabel, &QToolButton::clicked, this, &CodeRootSelector::slotSetPath);

    QToolButton* m_openDir = new QToolButton(this);
    m_openDir->setFixedWidth(m_summaryLabel->width());
    m_openDir->setText("Open Directory");
    connect(m_openDir, &QToolButton::clicked, [&](){
        QDesktopServices::openUrl(QUrl(m_summaryLabel->text()));
    });

    layout->addWidget(m_summaryLabel);
    layout->addWidget(m_openDir);

    // for icon changes:

    SessionManager *sessionManager = SessionManager::instance();
    connect(sessionManager, &SessionManager::startupProjectChanged,
            this, &CodeRootSelector::updateActionAndSummary);
}

QString CodeRootSelector::CodeRootPath()
{
    return codeRootPath;
}

bool CodeRootSelector::event(QEvent *event)
{
    if (event->type() == QEvent::LayoutRequest) {
        doLayout(true);
        return true;
    }else if (event->type() == QEvent::Show) {
        setFixedSize(m_summaryLabel->width() + 1, 100);
    }else if (event->type() == QEvent::ShortcutOverride) {
        if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Escape) {
            event->accept();
            return true;
        }
    }
    return QWidget::event(event);
}

void CodeRootSelector::doLayout(bool keepSize)
{
    // An unconfigured project shows empty build/deploy/run sections
    // if there's a configured project in the seesion
    // that could be improved
    static QStatusBar *statusBar = Core::ICore::statusBar();
    static auto *actionBar = Core::ICore::mainWindow()->findChild<QWidget*>(QLatin1String("actionbar"));
    Q_ASSERT(actionBar);

    setFixedSize(m_summaryLabel->width() + 1, 100);

    QPoint moveTo = statusBar->mapToGlobal(QPoint(0, 0));
    moveTo -= QPoint(0, height());
    move(moveTo);
}

void CodeRootSelector::setVisible(bool visible)
{
    doLayout(false);
    QWidget::setVisible(visible);
    m_projectAction->setChecked(visible);
}

void CodeRootSelector::toggleVisible()
{
    setVisible(!isVisible());
}

void CodeRootSelector::slotSetPath()
{
    Utils::FilePath rootPath = Utils::FilePath::fromString(
    QFileDialog::getExistingDirectory(Core::ICore::dialogParent(),
                                      tr("Select the CodeRoot Directory"),
                                      m_project->activeTarget()->activeBuildConfiguration()->buildDirectory().toString(),
                                      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
    if(rootPath.isDir())
    {
        m_project->activeTarget()->activeBuildConfiguration()->setBuildDirectory(rootPath);
        updateActionAndSummary(m_project);
    }
}

void CodeRootSelector::keyPressEvent(QKeyEvent *ke)
{
    if (ke->key() == Qt::Key_Return
            || ke->key() == Qt::Key_Enter
            || ke->key() == Qt::Key_Space
            || ke->key() == Qt::Key_Escape) {
        hide();
    } else {
        QWidget::keyPressEvent(ke);
    }
}

void CodeRootSelector::keyReleaseEvent(QKeyEvent *ke)
{
    if (m_hideOnRelease) {
        if (ke->modifiers() == 0
                /*HACK this is to overcome some event inconsistencies between platforms*/
                || (ke->modifiers() == Qt::AltModifier
                    && (ke->key() == Qt::Key_Alt || ke->key() == -1))) {
            delayedHide();
            m_hideOnRelease = false;
        }
    }
    if (ke->key() == Qt::Key_Return
            || ke->key() == Qt::Key_Enter
            || ke->key() == Qt::Key_Space
            || ke->key() == Qt::Key_Escape)
        return;
    QWidget::keyReleaseEvent(ke);
}

void CodeRootSelector::delayedHide()
{
    QDateTime current = QDateTime::currentDateTime();
    if (m_earliestHidetime > current) {
        // schedule for later
        QTimer::singleShot(current.msecsTo(m_earliestHidetime) + 50, this, &CodeRootSelector::delayedHide);
    } else {
        hide();
    }
}

// This is a workaround for the problem that Windows
// will let the mouse events through when you click
// outside a popup to close it. This causes the popup
// to open on mouse release if you hit the button, which
//
//
// A similar case can be found in QComboBox
void CodeRootSelector::mousePressEvent(QMouseEvent *e)
{
    setAttribute(Qt::WA_NoMouseReplay);
    QWidget::mousePressEvent(e);
}

void CodeRootSelector::updateActionAndSummary(ProjectExplorer::Project *project)
{
    m_project = project;
    m_projectAction->setProperty("subtitle", tr("Unconfig"));
    m_summaryLabel->setText("select coderoot");
    if(!project || !project->activeTarget() || !project->activeTarget()->activeBuildConfiguration())
        return;

    FilePath buidlDir = project->activeTarget()->activeBuildConfiguration()->buildDirectory();
    if(!buidlDir.isDir())
        return;

    codeRootPath = buidlDir.toString();

    QStringList lines;
    lines << tr("<b>CodeRoot:</b> %1").arg(buidlDir.toString());
    QString toolTip = QString("<html><nobr>%1</html>")
            .arg(lines.join(QLatin1String("<br/>")));
    m_projectAction->setToolTip(toolTip);
    m_summaryLabel->setText(buidlDir.toString());
    m_projectAction->setProperty("subtitle", tr("Config"));
}

void CodeRootSelector::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), Utils::StyleHelper().baseColor());
    painter.setPen(creatorTheme()->color(Theme::MiniProjectTargetSelectorBorderColor));
    // draw border on top and right
    QRectF borderRect = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    painter.drawLine(borderRect.topLeft(), borderRect.topRight());
    painter.drawLine(borderRect.topRight(), borderRect.bottomRight());
    if (creatorTheme()->flag(Theme::DrawTargetSelectorBottom)) {
        // draw thicker border on the bottom
        QRect bottomRect(0, rect().height() - 8, rect().width(), 8);
        static const QImage image(":/explorer/images/targetpanel_bottom.png");
        StyleHelper::drawCornerImage(image, &painter, bottomRect, 1, 1, 1, 1);
    }
}

} // namespace Internal
} // namespace ProjectExplorer
