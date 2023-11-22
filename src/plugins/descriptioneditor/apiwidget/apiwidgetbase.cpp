#include "apiwidgetbase.h"

#include <utils/emoswidgets/flattable.h>
#include <emostools/type_description_parser.h>
#include <utils/emoswidgets/autocomobox.h>
#include <utils/stringutils.h>
#include <utils/emoswidgets/messagetips.h>

#include <QDomDocument>
#include <QVBoxLayout>
#include <QAbstractItemModel>
#include <QComboBox>
#include <QSpinBox>

using namespace EmosWidgets;
using namespace EmosTools;

ApiWidgetBase::ApiWidgetBase(QDomDocument* doc, QWidget *parent) :
    QWidget(parent),
    m_doc(doc)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    m_table = new FlatTable(this);
    layout->addWidget(m_table);

    connect(m_table->model(),&QAbstractItemModel::dataChanged,this,&ApiWidgetBase::modify);
    getTypeInfo();
}

QString ApiWidgetBase::serviceName() const
{
    return m_ServiceName;
}

void ApiWidgetBase::setServiceName(const QString &name)
{
    m_ServiceName = name;
}

void ApiWidgetBase::getTypeInfo()
{
    TypeParser::getInstance()->reloadXml();
    m_typeList = TypeParser::getInstance()->getFullDatatypes();
    m_typeWithoutPointer.clear();
    for(auto &type : m_typeList)
    {
        if(!type.contains("*"))
        {
            m_typeWithoutPointer.append(type);
        }
    }
}

void ApiWidgetBase::appendRow(QStringList items)
{
    m_table->appendRowItems(items);

    if (m_typeList.isEmpty())
    {
        return;
    }
    appendComboBox(m_typeWithoutPointer, m_table->rowCount() - 1, 2, items.at(2));

    if(m_table->rowCount() > 0)
    {
        m_table->selectRow(m_table->rowCount() - 1);
    }
}

void ApiWidgetBase::appendComboBox(QStringList &items, int row, int col, QString currentText, bool editorEnable)
{
    AutoComoBox* comboBox = new AutoComoBox(m_table);
    comboBox->setEditable(editorEnable);
    comboBox->setCompleteList(items);
    comboBox->setCurrentText(currentText);
    connect(comboBox,&QComboBox::currentTextChanged,this,&ApiWidgetBase::modify);
    m_table->setCellWidget(row, col, comboBox);
}

void ApiWidgetBase::appendSpinBox(int min, int max, int row, int col, int currentValue)
{
    QSpinBox* spinBox = new QSpinBox(m_table);
    spinBox->setFrame(false);
    //QLineEdit* lineEditor = spinBox->lineEdit();
    spinBox->setMinimum(min);
    spinBox->setMaximum(max);
    spinBox->setValue(currentValue);
    connect(spinBox, static_cast<void (QSpinBox::*)(const QString&)>(&QSpinBox::valueChanged), this, &ApiWidgetBase::modify);
    m_table->setCellWidget(row, col, spinBox);
}

void ApiWidgetBase::removeCurrentRow()
{
    m_table->removeRow(m_table->currentRow());
}

void ApiWidgetBase::selectFirstRow()
{
    if(m_table->rowCount() > 0)
        m_table->selectRow(0);
}

void ApiWidgetBase::selectLastRow()
{
    if(m_table->rowCount() > 1)
        m_table->selectRow(m_table->rowCount() - 1);
}

QString ApiWidgetBase::getDefaultName(QString tagName)
{
    QStringList nameList;
    for(int row = 0; row < m_table->rowCount(); row++)
    {
        nameList.append(m_table->item(row, 0)->text());
    }
    return Utils::makeUniquelyNumberedInsensitive(tagName, nameList, QString("1"));
}

EmosWidgets::FlatTable* ApiWidgetBase::getTable()
{
    return m_table;
}

void ApiWidgetBase::updateCombo()
{
    getTypeInfo();
    int col = 2;
    for(int row = 0; row < m_table->rowCount(); row++)
    {
        AutoComoBox* combo = qobject_cast<AutoComoBox*>(m_table->cellWidget(row, col));
        QString curText = combo->currentText();
        combo->setCompleteList(m_typeWithoutPointer);
        combo->setCurrentText(curText);
    }
}

void ApiWidgetBase::slotAddRow()
{
    selectLastRow();
    emit modify();
}

void ApiWidgetBase::slotRemoveRow()
{
    removeCurrentRow();
    selectLastRow();
    emit modify();
}

void ApiWidgetBase::slotItemChanged(QTableWidgetItem *item)
{
    int Count = 0;
    for(int row = 0; row < m_table->rowCount(); row++)
    {
        if(m_table->item(row, 0)->text().toLower() == item->text().toLower())
        {
            Count++;
        }
    }

    if(Count > 1)
    {
        EmosWidgets::MessageTips::showText(this->parentWidget(), "name can't same");
        //item->setSelected(true); // 有效
        //m_table->editItem(m_table->item(0, 0)); // 无效
        item->setText(getDefaultName(m_tagName));
    }
}

int ApiWidgetBase::makeUniquelyId(QList<int> idList, int step, int start)
{
    int id = start;

    if (!idList.contains(id))
        return id;

    id += step;
    while (idList.contains(id))
        id += step;
    return id;
}

ApiWidgetBase::~ApiWidgetBase()
{

}
