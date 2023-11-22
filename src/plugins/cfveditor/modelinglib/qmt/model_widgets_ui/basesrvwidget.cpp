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

#include "basesrvwidget.h"

#include <QCheckBox>
#include <QToolBox>
#include <QHeaderView>
#include <QTableWidgetItem>
#include "qmt/infrastructure/contextmenuaction.h"
#include "qmt/model/melement.h"

#include <utils/emoswidgets/flattable.h>
#include <utils/utilsicons.h>
#include <emostools/srv_parser.h>

#include <QStyleFactory>
#include <QVBoxLayout>

namespace qmt {

BaseSrvWidget::BaseSrvWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    m_toolBox = new QToolBox(this);
    static QStyle *windowsStyle = QStyleFactory().create("Windows");
    if (windowsStyle)
        m_toolBox->setStyle(windowsStyle);
    m_toolBox->setStyleSheet(
                QLatin1String("QToolBox::tab {"
                              "    margin-left: 0px;"
                              "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                              "                           stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,"
                              "                           stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);"
                              "    color: #404040;"
                              "}"
                              ""
                              "QToolBox::tab:selected {"
                              "    font: bold 12px;"
                              "    color: black;"
                              "}"));
    layout->addWidget(m_toolBox);
}

BaseSrvWidget::~BaseSrvWidget()
{
}

void BaseSrvWidget::load(const DBoundary* dboundary)
{
    QMap<QString,EmosTools::tSrvFixedModule> modules = EmosTools::SrvParser::getInstance()->getModules();
    QMapIterator<QString, EmosTools::tSrvFixedModule> itmodule(modules);
    QHash<QString, QString> propertyInfo = dboundary->baseSrvInfo();
    QList<QString> baseSrvSelect = dboundary->baseSrvSelect();


    //优先级排序
    QList<EmosTools::tSrvFixedModule> listModule;
    while (itmodule.hasNext()) {
        itmodule.next();

        if(!itmodule.value().show)
            continue;

        int index = 0;
        for(; index < listModule.count(); index++)
        {
            if(itmodule.value().priority > listModule[index].priority)
            break;
        }
        listModule.insert(index, itmodule.value());
    }

    m_basSrvCount = 0;
    bool needSave = false;
    for(const auto& itmodule : listModule)
    {
        QMapIterator<QString, EmosTools::tSrvProperty> property(itmodule.propertyMap);

        m_baseSrvPropertyTable[m_basSrvCount] = new EmosWidgets::FlatTable(this);
        m_baseSrvPropertyTable[m_basSrvCount]->setColumnCount(3);
        m_baseSrvPropertyTable[m_basSrvCount]->setHorizontalHeaderLabels(QStringList()<<"Name"<<"Type"<<"Value");
        m_baseSrvPropertyTable[m_basSrvCount]->setHeaderSize(QVector<int>()<< 1 << 1 << 1);
        QFont font = qobject_cast<QWidget*>(parent())->font();
        m_baseSrvPropertyTable[m_basSrvCount]->setFont(font);
        m_baseSrvPropertyTable[m_basSrvCount]->horizontalHeader()->setFont(font);
        m_baseSrvCheck[m_basSrvCount] = new QCheckBox(this);
        m_baseSrvCheck[m_basSrvCount]->setText(itmodule.moduleName);
        if(baseSrvSelect.contains(itmodule.moduleName))
            m_baseSrvCheck[m_basSrvCount]->setChecked(true);

        while (property.hasNext()) {
            property.next();

            if(property.value().isFixed)
                continue;

            QString value = "";
            if(propertyInfo.contains(property.value().moduleName + ";" + property.value().propertyName))
            {
                value = propertyInfo.value(property.value().moduleName + ";" + property.value().propertyName).split(";").last();
            }
            else
            {
                value = property.value().dvalue;
                needSave = true;
            }
            m_baseSrvPropertyTable[m_basSrvCount]->appendRowItems(QStringList()
                                            << property.value().propertyName
                                            << property.value().type
                                            << value);
            for(int i = 0; i < m_baseSrvPropertyTable[m_basSrvCount]->rowCount(); i++)
            {
                m_baseSrvPropertyTable[m_basSrvCount]->item(i, 0)->setFlags(m_baseSrvPropertyTable[m_basSrvCount]->item(i,0)->flags() & ~Qt::ItemIsEditable);
                m_baseSrvPropertyTable[m_basSrvCount]->item(i, 1)->setFlags(m_baseSrvPropertyTable[m_basSrvCount]->item(i,1)->flags() & ~Qt::ItemIsEditable);
            }
        }

        auto toolBar = new QWidget(m_toolBox);
        auto toolBarLayout = new QVBoxLayout(toolBar);
        toolBarLayout->setContentsMargins(0, 0, 0, 0);
        toolBarLayout->setSpacing(6);
        if(baseSrvSelect.contains(itmodule.moduleName))
            m_toolBox->addItem(toolBar, Utils::Icons::OK.icon(), property.value().moduleName);
        else
            m_toolBox->addItem(toolBar, QIcon(), property.value().moduleName);
        toolBarLayout->addWidget(m_baseSrvCheck[m_basSrvCount]);
        toolBarLayout->addWidget(m_baseSrvPropertyTable[m_basSrvCount]);

        connect(m_baseSrvPropertyTable[m_basSrvCount], &EmosWidgets::FlatTable::itemChanged,
                this, &BaseSrvWidget::onBaseSrvPropertyTableChanged);
        connect(m_baseSrvCheck[m_basSrvCount], &QCheckBox::stateChanged,
                this, &BaseSrvWidget::onBaseSrvSelectChanged);

        m_basSrvCount++;
    }
    if(needSave)
        onBaseSrvPropertyTableChanged(NULL);
}

void BaseSrvWidget::onBaseSrvPropertyTableChanged(QTableWidgetItem *item)
{
    QHash<QString, QString> propertyInfo;

    for(int i = 0; i < m_basSrvCount; i++)
    {
        for(int row = 0; row < m_baseSrvPropertyTable[i]->rowCount(); row++)
        {
            propertyInfo.insert(m_baseSrvCheck[i]->text() + ";" + m_baseSrvPropertyTable[i]->item(row, 0)->text(),
                                m_baseSrvPropertyTable[i]->item(row, 2)->text());
        }
    }

    emit dataChanged(propertyInfo);
}

void BaseSrvWidget::onBaseSrvSelectChanged(int checked)
{
    QList<QString> selectList;

    for(int i = 0; i < m_basSrvCount; i++)
    {
        if(m_baseSrvCheck[i]->isChecked())
        {
            selectList << m_baseSrvCheck[i]->text();
            m_toolBox->setItemIcon(i, Utils::Icons::OK.icon());
        }
        else
        {
            m_toolBox->setItemIcon(i, QIcon());
        }
    }

    emit selectChanged(selectList);
}

} // namespace qmt
