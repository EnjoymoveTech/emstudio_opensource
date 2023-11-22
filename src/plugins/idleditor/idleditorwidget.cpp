#include <QTabWidget>
#include <coreplugin/minisplitter.h>
#include "propertywidget.h"
#include "threadwidget.h"
#include "servicewidget.h"
#include "idleditor.h"
#include "idleditorwidget.h"

namespace Idl {
namespace Internal{

IdlEditorWidget::IdlEditorWidget(IdlEditor* editor,QWidget *parent) :
    Utils::FancyMainWindow(parent),
    m_editor(editor)
{
    m_tabwidget   = new QTabWidget(this);
    m_propertyWidget        = new PropertyWidget(editor);
    m_threadWidget          = new ThreadWidget(editor);
    m_tabwidget->addTab(m_threadWidget, "Runnable");
    m_tabwidget->addTab(m_propertyWidget, "Property");

    m_serviceWidget         = new ServiceWidget(editor);
    Core::MiniSplitter *w   = new Core::MiniSplitter;
    w->addWidget(m_serviceWidget);
    w->addWidget(m_tabwidget);
    this->setCentralWidget(w);
    connect(m_serviceWidget,&ServiceWidget::CurrentWidgetChange,m_threadWidget,&ThreadWidget::onWidgetChange);
}

IdlEditorWidget::~IdlEditorWidget()
{
    delete m_serviceWidget;
    delete m_threadWidget;
    delete m_propertyWidget;
    m_editor = nullptr;
}

void IdlEditorWidget::loadFromDoc()
{
    m_propertyWidget->loadFromDoc();
    //m_threadWidget->loadFromDoc();
    //m_serviceWidget->loadFromDoc();
}

PropertyWidget *IdlEditorWidget::getPropertyWidget()
{
    return m_propertyWidget;
}

ThreadWidget *IdlEditorWidget::getThreadWidget()
{
    return m_threadWidget;
}

ServiceWidget *IdlEditorWidget::getServiceWidget()
{
    return m_serviceWidget;
}

void IdlEditorWidget::onappendEmptyRow()
{
    if(m_tabwidget->currentWidget() == m_threadWidget){
        m_threadWidget->appendRow();
    } else {
        m_propertyWidget->appendRow();
    }
}

void IdlEditorWidget::onremoveOneRow()
{
    if(m_tabwidget->currentWidget() == m_threadWidget){
        m_threadWidget->removeOneRow();
    } else {
        m_propertyWidget->removeOneRow();
    }
}

void IdlEditorWidget::exportToDoc()
{
    m_serviceWidget->exportToDoc();
    m_propertyWidget->exportToDoc();
    m_threadWidget->exportToDoc();
}

}
}
