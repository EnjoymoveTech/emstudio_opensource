#include "servicedetailwidget.h"
#include "servicetabwidget.h"
namespace Idl {
namespace Internal {
ServiceTabWidget::ServiceTabWidget(IdlEditor *editor,QWidget *parent) :
    QTabWidget(parent),
    m_editor(editor)
{
    m_methodWidget = new ServiceDetailWidget(m_editor, MethodType, this);
    m_eventWidget = new ServiceDetailWidget(m_editor, EventType, this);
    m_fieldWidget = new ServiceDetailWidget(m_editor, FieldType, this);
    m_pinWidget = new ServiceDetailWidget(m_editor, PinType, this);

    this->addTab(m_methodWidget, "Method");
    this->addTab(m_eventWidget, "Event");
    this->addTab(m_fieldWidget, "Field");
    this->addTab(m_pinWidget, "Pin");
}

ServiceTabWidget::~ServiceTabWidget()
{
    delete m_pinWidget;
    delete m_fieldWidget;
    delete m_eventWidget;
    delete m_methodWidget;
}

void ServiceTabWidget::loadFromDoc()
{
    m_methodWidget->loadFromApi();
    m_eventWidget->loadFromApi();
    m_fieldWidget->loadFromApi();
    m_pinWidget->loadFromApi();
}

void ServiceTabWidget::onWidgetChange()
{
    loadFromDoc();
}

}
}
