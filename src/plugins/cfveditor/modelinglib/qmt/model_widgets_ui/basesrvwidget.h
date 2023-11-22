/****************************************************************************
**
** Copyright (C) 2016 Jochen Becher
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

#include "qmt/infrastructure/qmt_global.h"
#include "qmt/diagram/dboundary.h"

#include <QWidget>

namespace EmosWidgets {class FlatTable;}
namespace EmosTools {struct tSrvProperty;}

class QCheckBox;
class QToolBox;
class QTableWidgetItem;
namespace qmt {

class BaseSrvWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BaseSrvWidget(QWidget *parent = nullptr);
    ~BaseSrvWidget();

    void load(const DBoundary* dboundary);
signals:
    void dataChanged(const QHash<QString, QString>& propertyInfo);
    void selectChanged(const QList<QString>& selectList);

public slots:
    void onBaseSrvPropertyTableChanged(QTableWidgetItem *item);
    void onBaseSrvSelectChanged(int checked);

private:
    EmosWidgets::FlatTable* m_baseSrvPropertyTable[20];
    QCheckBox* m_baseSrvCheck[20];
    QToolBox *m_toolBox = nullptr;
    int m_basSrvCount;
};

} // namespace qmt
