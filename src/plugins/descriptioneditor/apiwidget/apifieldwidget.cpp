#include "apifieldwidget.h"

#include <utils/emoswidgets/flattable.h>
#include <utils/emoswidgets/messagetips.h>
#include <emostools/api_description_parser.h>
#include <QComboBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QDomDocument>
#include <QTableWidgetItem>

using namespace EmosWidgets;
using namespace EmosTools;

ApiFieldWidget::ApiFieldWidget(QDomDocument* doc, QWidget *parent) :
    ApiWidgetBase(doc, parent)
{
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels(QStringList()<< "Name"<<"ID"<< "Data"<<"Type");
    m_table->setHeaderSize({1,1,1,1});
    m_table->setEditTriggers(QTableView::AllEditTriggers);
    m_tagName = "Field";
}

ApiFieldWidget::~ApiFieldWidget()
{
}

void ApiFieldWidget::widgetData(QDomDocument* doc, QDomElement* element)
{
    for(int modeIdx = 0; modeIdx < m_table->rowCount(); modeIdx++)
    {
        if(m_table->item(modeIdx,0) == nullptr)
        {
            return;
        }

        QDomElement fieldElement = doc->createElement("element");

        fieldElement.setAttribute("name", m_table->item(modeIdx,0)->text());
        fieldElement.setAttribute("type", "Field");
        fieldElement.setAttribute("id", m_table->item(modeIdx,1)->text());
        fieldElement.setAttribute("data", qobject_cast<QComboBox*>(m_table->cellWidget(modeIdx,2))->currentText());
        fieldElement.setAttribute("selecttype", getSelectType(modeIdx));

        element->appendChild(fieldElement);
    }
}

void ApiFieldWidget::slotAddRow()
{
    QList<int> idList;
    for(int modeIdx = 0; modeIdx < m_table->rowCount(); modeIdx++)
    {
        idList << m_table->item(modeIdx,1)->text().toInt();
    }

    m_table->appendRowItems(QStringList() << getDefaultName("Field") << QString::number(makeUniquelyId(idList, 2, 0x4002))  << "" << "");
    QTableWidgetItem* item = m_table->item(m_table->rowCount() - 1, 1);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    //Combox
    appendComboBox(m_typeWithoutPointer, m_table->rowCount() - 1, 2, m_typeWithoutPointer.at(0));
    setGroupBox(m_table->rowCount() - 1, 3, 7);

    selectLastRow();
    emit modify();
}

void ApiFieldWidget::loadWidget(EmosTools::tSrvParsingService srv)
{
    disconnect(m_table, &QTableWidget::itemChanged, this, &ApiWidgetBase::slotItemChanged);
    getTypeInfo();
    QList<tSrvField> fields = srv.fields;

    for(int i = 0; i < m_table->rowCount();)
    {
        m_table->removeRow(i);
    }

    for(int i = 0; i < fields.size(); i++)
    {
        QString strID = QString::number(fields[i].id);
        QString strName = fields[i].name;
        QString strData = fields[i].data;
        appendRow(QStringList() << strName << strID << strData);
        setGroupBox(m_table->rowCount() - 1, 3, fields[i].type);

        QTableWidgetItem* item = m_table->item(m_table->rowCount() - 1, 1);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }

    selectFirstRow();
    connect(m_table, &QTableWidget::itemChanged, this, &ApiWidgetBase::slotItemChanged);
}

void ApiFieldWidget::setGroupBox(int row, int col, int type)
{
    QWidget* groupProcess= new QWidget(this);
    QHBoxLayout* processLayout = new QHBoxLayout(groupProcess);

    QCheckBox* GET = new QCheckBox("GET", this);
    QCheckBox* SET = new QCheckBox("SET", this);
    QCheckBox* NOTIFY = new QCheckBox("NOTIFY", this);

    m_GETHash.insert(row, GET);
    m_SETHash.insert(row, SET);
    m_NOTIFYHash.insert(row, NOTIFY);

    if(type > 3)
    {
        GET->setChecked(true);
        type -= 4;
    }
    if(type > 1)
    {
        SET->setChecked(true);
        type -= 2;
    }
    if(type > 0)
    {
        NOTIFY->setChecked(true);
    }

    connect(GET, &QCheckBox::clicked, this, &ApiFieldWidget::modify);
    connect(SET, &QCheckBox::clicked, this, &ApiFieldWidget::modify);
    connect(NOTIFY, &QCheckBox::clicked, this, &ApiFieldWidget::modify);

    processLayout->addWidget(GET);
    processLayout->addWidget(SET);
    processLayout->addWidget(NOTIFY);
    processLayout->addStretch();

    m_table->setCellWidget(row, col, groupProcess);
    m_table->resizeRowsToContents();
}

QString ApiFieldWidget::getSelectType(int row)
{
    int select = 4 * m_GETHash.value(row)->isChecked() + 2 * m_SETHash.value(row)->isChecked() + m_NOTIFYHash.value(row)->isChecked();
    return QString::number(select);
}
