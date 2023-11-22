#include "typewidgetbase.h"

#include <utils/emoswidgets/flattable.h>

#include <QDomDocument>
#include <QVBoxLayout>
#include <QAbstractItemModel>
using namespace EmosWidgets;

TypeWidgetBase::TypeWidgetBase(QDomDocument* doc, QWidget *parent) :
    QWidget(parent),
    m_doc(doc)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    m_table = new FlatTable(this);
    layout->addWidget(m_table);

    connect(m_table->model(),&QAbstractItemModel::dataChanged,this,&TypeWidgetBase::modify);
}

QString TypeWidgetBase::serviceName() const
{
    return m_ServiceName;
}

void TypeWidgetBase::setServiceName(const QString &name)
{
    m_ServiceName = name;
    loadWidget();
}

void TypeWidgetBase::appendRow(QStringList items)
{
    m_table->appendRowItems(items);
    if(m_table->rowCount() > 0)
    {
        m_table->selectRow(m_table->rowCount() - 1);
    }
}

void TypeWidgetBase::removeCurrentRow()
{
    m_table->removeRow(m_table->currentRow());
}

void TypeWidgetBase::selectFirstRow()
{
    if(m_table->rowCount() > 0)
        m_table->selectRow(0);
}

void TypeWidgetBase::selectLastRow()
{
    if(m_table->rowCount() > 1)
        m_table->selectRow(m_table->rowCount() - 1);
}

void TypeWidgetBase::slotAddRow()
{
    selectLastRow();
    emit modify();
}

void TypeWidgetBase::slotRemoveRow()
{
    removeCurrentRow();
    selectLastRow();
    emit modify();
}

TypeWidgetBase::~TypeWidgetBase()
{

}
