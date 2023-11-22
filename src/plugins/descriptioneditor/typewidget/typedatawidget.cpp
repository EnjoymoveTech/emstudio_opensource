#include "typedatawidget.h"

#include <utils/emoswidgets/flattable.h>
#include <utils/emoswidgets/messagetips.h>
#include <emostools/type_description_parser.h>

#include <QDomDocument>
using namespace EmosWidgets;
using namespace EmosTools;

TypeDataWidget::TypeDataWidget(QDomDocument* doc, QWidget *parent) :
    TypeWidgetBase(doc, parent)
{
    m_table->setColumnCount(2);
    m_table->setHorizontalHeaderLabels(QStringList()<< "type"<<"size");
    m_table->setHeaderSize({5,4});
    m_doc = doc;
    loadWidget();
}

TypeDataWidget::~TypeDataWidget()
{
}

void TypeDataWidget::widgetData(QDomDocument* doc, QDomElement* element)
{
    element->removeChild(element->firstChildElement("datatypes"));
    QDomElement datarootElement = doc->createElement("datatypes");
    element->appendChild(datarootElement);
    for(int modeIdx = 0; modeIdx < m_table->rowCount(); modeIdx++)
    {
        if(m_table->item(modeIdx,0) == nullptr)
        {
            return;
        }
        QDomElement dataElement = datarootElement.toDocument().createElement("datatype");

        dataElement.setAttribute("description", "predefined  " + m_table->item(modeIdx,0)->text() + " datatype");
        dataElement.setAttribute("name", m_table->item(modeIdx,0)->text());
        dataElement.setAttribute("size", m_table->item(modeIdx,1)->text());

        datarootElement.appendChild(dataElement);
    }

}

void TypeDataWidget::slotAddRow()
{
    m_table->appendRowItems(QStringList() << "" << "");
    selectLastRow();
    emit modify();
}

void TypeDataWidget::loadWidget()
{
    QDomNodeList datatypes = m_doc->firstChildElement("emos:ddl").firstChildElement("datatypes").childNodes();
    for(int i = 0; i < datatypes.size(); i++)
    {
        QString type = datatypes.at(i).toElement().attribute("name");
        QString size = datatypes.at(i).toElement().attribute("size");
        appendRow(QStringList() << type << size);
    }

    selectFirstRow();
}
