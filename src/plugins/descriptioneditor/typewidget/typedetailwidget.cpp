#include "typedetailwidget.h"

#include "typedatawidget.h"

namespace DescriptionEditor {
namespace Internal {

TypeDetailWidget::TypeDetailWidget(QDomDocument* doc, QWidget *parent) :
    QTabWidget(parent),
    m_doc(doc)
{
    m_dataWidget = new TypeDataWidget(m_doc, this);
    connect(m_dataWidget,&TypeWidgetBase::modify,this,&TypeDetailWidget::modify);
    addTab(m_dataWidget, "DataType");

    setCurrentWidget(m_dataWidget);
}

TypeDetailWidget::~TypeDetailWidget()
{

}

void TypeDetailWidget::widgetData(QDomDocument *doc, QDomElement* element)
{
    m_dataWidget->widgetData(doc, element);
}

void TypeDetailWidget::slotAddRow()
{
    if(currentWidget() == m_dataWidget)
        m_dataWidget->slotAddRow();
}

void TypeDetailWidget::slotRemoveRow()
{
    if(currentWidget() == m_dataWidget)
        m_dataWidget->slotRemoveRow();
}
}}
