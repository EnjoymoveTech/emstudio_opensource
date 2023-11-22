#include "apipinwidget.h"

#include <utils/emoswidgets/flattable.h>
#include <utils/emoswidgets/messagetips.h>
#include <emostools/api_description_parser.h>
#include <QComboBox>

#include <QDomDocument>
#include <QTableWidgetItem>
using namespace EmosWidgets;
using namespace EmosTools;

ApiPinWidget::ApiPinWidget(QDomDocument* doc, QWidget *parent) :
    ApiWidgetBase(doc, parent)
{
    m_table->setColumnCount(3);
    m_table->setHorizontalHeaderLabels(QStringList()<< "Name"<<"ID"<< "Data");
    m_table->setHeaderSize({1,1,1});
    m_table->setEditTriggers(QTableView::AllEditTriggers);
    m_tagName = "Pin";
}

ApiPinWidget::~ApiPinWidget()
{
}

void ApiPinWidget::widgetData(QDomDocument* doc, QDomElement* element)
{
    for(int modeIdx = 0; modeIdx < m_table->rowCount(); modeIdx++)
    {
        if(m_table->item(modeIdx,0) == nullptr)
        {
            return;
        }

        QDomElement fieldElement = doc->createElement("element");

        fieldElement.setAttribute("name", m_table->item(modeIdx,0)->text());
        fieldElement.setAttribute("type", "Pin");
        fieldElement.setAttribute("id", m_table->item(modeIdx,1)->text());
        fieldElement.setAttribute("data", qobject_cast<QComboBox*>(m_table->cellWidget(modeIdx,2))->currentText());

        element->appendChild(fieldElement);
    }
}

void ApiPinWidget::slotAddRow()
{
    QList<int> idList;
    for(int modeIdx = 0; modeIdx < m_table->rowCount(); modeIdx++)
    {
        idList << m_table->item(modeIdx,1)->text().toInt();
    }

    m_table->appendRowItems(QStringList() << getDefaultName("Pin") << QString::number(makeUniquelyId(idList, 1, 0x2000))  << "" << "");
    QTableWidgetItem* item = m_table->item(m_table->rowCount() - 1, 1);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    //Combox
    appendComboBox(m_typeWithoutPointer, m_table->rowCount() - 1, 2, m_typeWithoutPointer.at(0));

    selectLastRow();
    emit modify();
}

void ApiPinWidget::loadWidget(EmosTools::tSrvParsingService srv)
{
    disconnect(m_table, &QTableWidget::itemChanged, this, &ApiWidgetBase::slotItemChanged);
    getTypeInfo();
    QList<tSrvPin> pins = srv.pins;

    for(int i = 0; i < m_table->rowCount();)
    {
        m_table->removeRow(i);
    }

    for(int i = 0; i < pins.size(); i++)
    {
        QString strID = QString::number(pins[i].id);
        QString strName = pins[i].name;
        QString strData = pins[i].data;
        appendRow(QStringList() << strName << strID << strData);

        QTableWidgetItem* item = m_table->item(m_table->rowCount() - 1, 1);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }

    selectFirstRow();
    connect(m_table, &QTableWidget::itemChanged, this, &ApiWidgetBase::slotItemChanged);
}


