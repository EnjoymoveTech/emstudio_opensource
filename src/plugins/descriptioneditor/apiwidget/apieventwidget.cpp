#include "apieventwidget.h"

#include <utils/emoswidgets/flattable.h>
#include <utils/emoswidgets/messagetips.h>
#include <emostools/api_description_parser.h>
#include <QComboBox>

#include <QDomDocument>
#include <QTableWidgetItem>
#include <QSpinBox>

using namespace EmosWidgets;
using namespace EmosTools;

ApiEventWidget::ApiEventWidget(QDomDocument* doc, QWidget *parent) :
    ApiWidgetBase(doc, parent)
{
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels(QStringList()<< "Name"<<"ID"<< "Data"<<"GroupId");
    m_table->setHeaderSize({1,1,1,1});
    m_table->setEditTriggers(QTableView::AllEditTriggers);
    m_tagName = "Event";
}

ApiEventWidget::~ApiEventWidget()
{
}

void ApiEventWidget::widgetData(QDomDocument* doc, QDomElement* element)
{
    for(int modeIdx = 0; modeIdx < m_table->rowCount(); modeIdx++)
    {
        if(m_table->item(modeIdx,0) == nullptr)
        {
            return;
        }

        QDomElement eventElement = doc->createElement("element");

        eventElement.setAttribute("name", m_table->item(modeIdx,0)->text());
        eventElement.setAttribute("type", "Event");
        eventElement.setAttribute("id", m_table->item(modeIdx,1)->text());
        eventElement.setAttribute("data", qobject_cast<QComboBox*>(m_table->cellWidget(modeIdx,2))->currentText());
        eventElement.setAttribute("groupid", qobject_cast<QSpinBox*>(m_table->cellWidget(modeIdx,3))->value());

        element->appendChild(eventElement);
    }
}

void ApiEventWidget::slotAddRow()
{
    QList<int> idList;
    for(int modeIdx = 0; modeIdx < m_table->rowCount(); modeIdx++)
    {
        idList << m_table->item(modeIdx,1)->text().toInt();
    }

    m_table->appendRowItems(QStringList() << getDefaultName("Event") << QString::number(makeUniquelyId(idList))  << "" << "1");

    QTableWidgetItem* item = m_table->item(m_table->rowCount() - 1, 1);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    //Combox
    appendComboBox(m_typeWithoutPointer, m_table->rowCount() - 1, 2, m_typeWithoutPointer.at(0));

    //SpinBox
    appendSpinBox(1, INT_MAX, m_table->rowCount() - 1, 3, 1);

    selectLastRow();
    emit modify();
}

void ApiEventWidget::loadWidget(EmosTools::tSrvParsingService srv)
{
    disconnect(m_table, &QTableWidget::itemChanged, this, &ApiWidgetBase::slotItemChanged);
    getTypeInfo();
    QList<tSrvEvent> events = srv.events;

    for(int i = 0; i < m_table->rowCount();)
    {
        m_table->removeRow(i);
    }

    for(int i = 0; i < events.size(); i++)
    {
        QString strID = QString::number(events[i].id);
        QString strName = events[i].name;
        QString strData = events[i].data;
        QString strGroupId = QString::number(events[i].groupid);
        appendRow(QStringList() << strName << strID << strData << strGroupId);
        appendSpinBox(1, INT_MAX, i, 3, strGroupId.toInt());
        QTableWidgetItem* item = m_table->item(m_table->rowCount() - 1, 1);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }

    selectFirstRow();
    connect(m_table, &QTableWidget::itemChanged, this, &ApiWidgetBase::slotItemChanged);
}
