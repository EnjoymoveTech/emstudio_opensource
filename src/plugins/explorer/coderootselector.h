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

#pragma once

#include "explorer_export.h"

#include <QDateTime>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QToolButton;
QT_END_NAMESPACE

namespace ProjectExplorer {
class Project;
namespace Internal {

class PROJECTEXPLORER_EXPORT CodeRootSelector : public QWidget
{
    Q_OBJECT

public:
    explicit CodeRootSelector(QAction *projectAction, QWidget *parent);
    static QString CodeRootPath();

    void setVisible(bool visible) override;
    void keyPressEvent(QKeyEvent *ke) override;
    void keyReleaseEvent(QKeyEvent *ke) override;
    bool event(QEvent *event) override;
    void toggleVisible();

public slots:
    void slotSetPath();

private:
    void delayedHide();
    void updateActionAndSummary(ProjectExplorer::Project *project);
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void doLayout(bool keepSize);

    QAction *m_projectAction;
    QToolButton *m_summaryLabel;
    bool m_hideOnRelease = false;
    QDateTime m_earliestHidetime;
    ProjectExplorer::Project * m_project;
};

} // namespace Internal
} // namespace ProjectExplorer
