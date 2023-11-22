#include "advancemethod.h"

#include <utils/emoswidgets/flattable.h>
#include <utils/emoswidgets/autocomobox.h>
#include <utils/stringutils.h>
#include <utils/styledbar.h>
#include <utils/utilsicons.h>

#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/icontext.h>

#include "descriptioneditorconstants.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QAction>
#include <QToolButton>
#include <QTableWidgetItem>

namespace DescriptionEditor {

AdvanceMethod::AdvanceMethod(QDialog *parent)
    : QDialog{parent}
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* layoutToolBar = new QHBoxLayout();
    layoutToolBar->setContentsMargins(0, 0, 0, 0);
    layoutToolBar->setSpacing(5);
    layout->addLayout(layoutToolBar);

    Core::Context context(Constants::DESCRIPTION_EDITOR_ID);
    QAction* m_addaction = new QAction(Utils::Icons::PLUS_TOOLBAR.icon(), tr("add"), this);
    Core::Command *cmd = Core::ActionManager::command(Constants::ACTION_METHOD_PARAM_ADD_ID);
    if(!cmd)
        cmd = Core::ActionManager::registerAction(m_addaction, Constants::ACTION_METHOD_PARAM_ADD_ID, context);
    connect(m_addaction, &QAction::triggered, this, &AdvanceMethod::slotAddRow);

    QAction* m_removeaction = new QAction(Utils::Icons::NOTLOADED.icon(), tr("remove"), this);
    cmd = Core::ActionManager::command(Constants::ACTION_METHOD_PARAM_REMOVE_ID);
    if(!cmd)
        cmd = Core::ActionManager::registerAction(m_removeaction, Constants::ACTION_METHOD_PARAM_REMOVE_ID, context);
    connect(m_removeaction, &QAction::triggered, this, &AdvanceMethod::slotRemoveRow);

    layoutToolBar->addWidget(Core::Command::toolButtonWithAppendedShortcut(m_addaction, cmd));
    layoutToolBar->addWidget(Core::Command::toolButtonWithAppendedShortcut(m_removeaction, cmd));
    layoutToolBar->addStretch(1);

    m_table = new EmosWidgets::FlatTable(this);
    m_table->setColumnCount(4);
    m_table->setHeaderSize({1,1,1,1});
    m_table->setHorizontalHeaderLabels(QStringList()<< "Name"<< "Type"<< "Direction"<< "Size");

    layout->addLayout(layoutToolBar);
    layout->addWidget(m_table);
}

void AdvanceMethod::setTypes(const QStringList &types)
{
    m_types = types;
    for(auto &type : types)
    {
        if(type.contains("*"))
        {
            m_typesPointer.append(type);
        }
    }
}

void AdvanceMethod::setParams(const QList<EmosTools::tParam> &params)
{
    m_params = params;
}

void AdvanceMethod::showDialog()
{
    for(const auto& param : m_params)
    {
        m_table->appendRowItems({param.name, param.type, direction2String(param.direction) , QString::number(param.size)});

        EmosWidgets::AutoComoBox* comboBox = new EmosWidgets::AutoComoBox(m_table);
        comboBox->setEditable(true);
        comboBox->setCompleteList(m_types);
        comboBox->setCurrentText(param.type);
        m_table->setCellWidget(m_table->rowCount() - 1, 1, comboBox);
        connect(comboBox, &EmosWidgets::AutoComoBox::currentTextChanged, this, &AdvanceMethod::slotDirCombo);

        if(param.type.contains("*"))
        {
            appendComboBox({"in", "out", "inout"}, m_table->rowCount() - 1, 2, direction2String(param.direction), true);
        }
        else
        {
            appendComboBox({"in"}, m_table->rowCount() - 1, 2, direction2String(param.direction), true);
        }
    }
}

QList<EmosTools::tParam> AdvanceMethod::getParams() const
{
    return m_params;
}

void AdvanceMethod::slotAddRow()
{
    m_table->appendRowItems(QStringList() << "i32Para" + QString::number(m_table->rowCount())  << "" << "" << "0");

    EmosWidgets::AutoComoBox* comboBox = new EmosWidgets::AutoComoBox(m_table);
    comboBox->setEditable(true);
    comboBox->setCompleteList(m_types);
    comboBox->setCurrentText("tInt32");
    m_table->setCellWidget(m_table->rowCount() - 1, 1, comboBox);
    connect(comboBox, &EmosWidgets::AutoComoBox::currentTextChanged, this, &AdvanceMethod::slotDirCombo);

    appendComboBox({"in"}, m_table->rowCount() - 1, 2, "in", true);

    if(m_table->rowCount() > 1)
        m_table->selectRow(m_table->rowCount() - 1);
}

void AdvanceMethod::slotRemoveRow()
{
    m_table->removeRow(m_table->currentRow());
}

void AdvanceMethod::slotTypeCombo(QString directionText)
{
    EmosWidgets::AutoComoBox* typeCombo = qobject_cast<EmosWidgets::AutoComoBox*>(m_table->cellWidget(m_table->currentRow(),1));
    disconnect(typeCombo, &EmosWidgets::AutoComoBox::currentTextChanged, this, &AdvanceMethod::slotDirCombo);
    QString currentText = typeCombo->currentText();
    if(directionText.contains("out"))
    {
        //只显示指针变量
        typeCombo->setCompleteList(m_typesPointer);
        typeCombo->setCurrentText(currentText);
    }
    else
    {
        typeCombo->setCompleteList(m_types);
        typeCombo->setCurrentText(currentText);
    }
    connect(typeCombo, &EmosWidgets::AutoComoBox::currentTextChanged, this, &AdvanceMethod::slotDirCombo);
}

void AdvanceMethod::slotDirCombo(QString typeText)
{
    QComboBox* direCombo = qobject_cast<QComboBox*>(m_table->cellWidget(m_table->currentRow(),2));
    disconnect(direCombo, &QComboBox::currentTextChanged, this, &AdvanceMethod::slotTypeCombo);
    if(!typeText.contains("*"))
    {
        while(direCombo->itemText(1) != "")
        {
            direCombo->removeItem(1);
        }
    }
    else
    {
        if(direCombo->itemText(1) == "")
        {
            direCombo->addItems({"out", "inout"});
        }
    }
    connect(direCombo, &QComboBox::currentTextChanged, this, &AdvanceMethod::slotTypeCombo);
}

void AdvanceMethod::closeEvent(QCloseEvent *event)
{
    m_params.clear();
    for(int modeIdx = 0; modeIdx < m_table->rowCount(); modeIdx++)
    {
        EmosTools::tParam param;
        if(m_table->item(modeIdx,0) == nullptr)
        {
            return;
        }

        param.name = m_table->item(modeIdx,0)->text();
        if(param.name.contains("Para", Qt::CaseSensitive))
        {
            param.name = "";
        }
        param.type = qobject_cast<EmosWidgets::AutoComoBox*>(m_table->cellWidget(modeIdx,1))->currentText();
        param.direction = string2Direction(qobject_cast<QComboBox*>(m_table->cellWidget(modeIdx,2))->currentText());
        param.size = m_table->item(modeIdx,3)->text().toInt();
        param.version = 1;
        m_params << param;
    }
}

void AdvanceMethod::appendComboBox(const QStringList &items, int row, int col, QString currentText, bool editorEnable)
{
    QComboBox* comboBox = new QComboBox(m_table);
    comboBox->setEditable(editorEnable);
    comboBox->addItems(items);
    comboBox->setCurrentText(currentText);
    m_table->setCellWidget(row, col, comboBox);
    connect(comboBox, &QComboBox::currentTextChanged, this, &AdvanceMethod::slotTypeCombo);
}

QString AdvanceMethod::direction2String(EmosTools::tParamDirection direction)
{
    switch(direction)
    {
    case EmosTools::Param_In:
            return "in";
    case EmosTools::Param_Out:
            return "out";
    case EmosTools::Param_InOut:
            return "inout";
    }
    return "in";
}

EmosTools::tParamDirection AdvanceMethod::string2Direction(const QString &str)
{
    if(str == "in")
        return EmosTools::Param_In;
    else if(str == "out")
        return EmosTools::Param_Out;
    else if(str == "inout")
        return EmosTools::Param_InOut;
    else
        return EmosTools::Param_In;
    return EmosTools::Param_In;
}

QSize AdvanceMethod::minimumSizeHint() const
{
    return QSize(600,400);
}
}
