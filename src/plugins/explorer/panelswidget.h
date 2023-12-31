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

#include <QWidget>

QT_BEGIN_NAMESPACE
class QGridLayout;
class QIcon;
QT_END_NAMESPACE

namespace Core { class MiniSplitter; }

namespace ProjectExplorer {

class PROJECTEXPLORER_EXPORT PanelsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PanelsWidget(QWidget *parent = nullptr, bool bar = true);
    PanelsWidget(const QString &displayName, const QIcon &icon,
                 QWidget *widget, bool bar = true);
    ~PanelsWidget() override;

    void addPropertiesPanel(const QString &displayName, const QIcon &icon,
                            QWidget *widget);

    QByteArray saveSplitterState() const;
    void loadSplitterState(const QByteArray &state);

private:
    QGridLayout *m_layout;
    Core::MiniSplitter * const m_splitter;
    QWidget *m_root;
};

} // namespace ProjectExplorer
