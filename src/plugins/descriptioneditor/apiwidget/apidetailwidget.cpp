#include "apidetailwidget.h"

#include "apimethodwidget.h"
#include "apieventwidget.h"
#include "apifieldwidget.h"
#include "apipinwidget.h"

#include <emostools/api_description_parser.h>

#include <QDomDocument>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <utils/styledbar.h>
#include <utils/utilsicons.h>
#include <coreplugin/minisplitter.h>
#include <apiinfowidget.h>
#include <apipropertywidget.h>
#include <QTreeView>

namespace DescriptionEditor {
namespace Internal {

ApiDetailWidget::ApiDetailWidget(QDomDocument* doc, QWidget *parent) :
    QWidget(parent),
    m_doc(doc)
{
    // api main widget
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);

    detailSplitter = new Core::MiniSplitter();
    layout->addWidget(detailSplitter);

    m_tabWidget = new QTabWidget(this);
    m_methodWidget = new ApiMethodWidget(m_doc, this);
    m_eventWidget = new ApiEventWidget(m_doc, this);
    m_fieldWidget = new ApiFieldWidget(m_doc, this);
    m_pinWidget = new ApiPinWidget(m_doc, this);

    m_tabWidget->addTab(m_methodWidget, "Method");
    m_tabWidget->addTab(m_eventWidget, "Event");
    m_tabWidget->addTab(m_fieldWidget, "Field");
    m_tabWidget->addTab(m_pinWidget, "Pin");

    connect(m_methodWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
    connect(m_eventWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
    connect(m_fieldWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
    connect(m_pinWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);

    m_tabWidget->setCurrentWidget(m_methodWidget);

    // treeWidget and propertyWidget
    rightSplitter = new Core::MiniSplitter(Qt::Vertical);

    detailSplitter->addWidget(m_tabWidget);
    detailSplitter->addWidget(rightSplitter);
    detailSplitter->setStretchFactor(0, 3);
    detailSplitter->setStretchFactor(1, 1);

    // treeWidget
    treeWidget = new QTreeView(this);
    apiInfo = new InspectorModel(getTableHash(), this);
    treeWidget->setModel((QAbstractItemModel*)apiInfo);

    connect(this, &ApiDetailWidget::modify, this, &ApiDetailWidget::slotTreeReload);

    // propertyWidget
    apiProperty = new ApiPropertyWidget(this);
    connect(apiProperty, &ApiPropertyWidget::modify, this, &ApiDetailWidget::modify);

    rightSplitter->addWidget(treeWidget);
    rightSplitter->addWidget(apiProperty);
    rightSplitter->setStretchFactor(0, 3);
    rightSplitter->setStretchFactor(1, 2);
}

ApiDetailWidget::~ApiDetailWidget()
{

}

QString ApiDetailWidget::serviceName() const
{
    return m_ServiceName;
}

void ApiDetailWidget::setServiceName(const QString& name)
{
    m_ServiceName = name;
    m_methodWidget->setServiceName(name);
    m_eventWidget->setServiceName(name);
    m_fieldWidget->setServiceName(name);
    m_pinWidget->setServiceName(name);
}

QString ApiDetailWidget::serviceId() const
{
    return m_ServiceId;
}

void ApiDetailWidget::setServiceId(const QString& id)
{
    m_ServiceId = id;
}

QString ApiDetailWidget::serviceVersion() const
{
    return m_ServiceVersion;
}

void ApiDetailWidget::setServiceVersion(const QString& ver)
{
    m_ServiceVersion = ver;
}

void ApiDetailWidget::widgetData(QDomDocument *doc, QDomElement* element)
{
    apiProperty->widgetData(doc, element);
    m_methodWidget->widgetData(doc, element);
    m_eventWidget->widgetData(doc, element);
    m_fieldWidget->widgetData(doc, element);
    m_pinWidget->widgetData(doc, element);
}

void ApiDetailWidget::loadWidget(EmosTools::tSrvParsingService srv)
{
    disconnect(m_methodWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
    disconnect(m_eventWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
    disconnect(m_fieldWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
    disconnect(m_pinWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);

    m_methodWidget->loadWidget(srv);
    m_eventWidget->loadWidget(srv);
    m_fieldWidget->loadWidget(srv);
    m_pinWidget->loadWidget(srv);

    apiInfo->LoadWidget(srv.name);
    treeWidget->expandAll();
    treeWidget->resizeColumnToContents(0);

    apiProperty->loadWidget(srv);

    connect(m_methodWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
    connect(m_eventWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
    connect(m_fieldWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
    connect(m_pinWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
}

tabSelect ApiDetailWidget::currentTab()
{
    if(m_tabWidget->currentWidget() == m_methodWidget)
        return Method;
    else if(m_tabWidget->currentWidget() == m_eventWidget)
        return Event;
    else if(m_tabWidget->currentWidget() == m_fieldWidget)
        return Field;
    else if(m_tabWidget->currentWidget() == m_pinWidget)
        return Pin;
}

void ApiDetailWidget::setTab(tabSelect selection)
{
    switch(selection)
    {
    case Method:
        m_tabWidget->setCurrentWidget(m_methodWidget);
        break;
    case Event:
        m_tabWidget->setCurrentWidget(m_eventWidget);
        break;
    case Field:
        m_tabWidget->setCurrentWidget(m_fieldWidget);
        break;
    case Pin:
        m_tabWidget->setCurrentWidget(m_pinWidget);
        break;
    }
}

QHash<QString, EmosWidgets::FlatTable*> ApiDetailWidget::getTableHash()
{
    QHash<QString, EmosWidgets::FlatTable*> tableHash;
    tableHash.insert("Method", m_methodWidget->getTable());
    tableHash.insert("Event", m_eventWidget->getTable());
    tableHash.insert("Field", m_fieldWidget->getTable());
    tableHash.insert("Pin", m_pinWidget->getTable());
    return tableHash;
}

void ApiDetailWidget::updateCombo()
{
    m_methodWidget->updateCombo();
    m_eventWidget->updateCombo();
    m_fieldWidget->updateCombo();
    m_pinWidget->updateCombo();
}

void ApiDetailWidget::slotAddRow()
{
    disconnect(m_methodWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
    disconnect(m_eventWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
    disconnect(m_fieldWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
    disconnect(m_pinWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
    if(m_tabWidget->currentWidget() == m_methodWidget)
        m_methodWidget->slotAddRow();
    else if(m_tabWidget->currentWidget() == m_eventWidget)
        m_eventWidget->slotAddRow();
    else if(m_tabWidget->currentWidget() == m_fieldWidget)
        m_fieldWidget->slotAddRow();
    else if(m_tabWidget->currentWidget() == m_pinWidget)
        m_pinWidget->slotAddRow();
    connect(m_methodWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
    connect(m_eventWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
    connect(m_fieldWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
    connect(m_pinWidget,&ApiWidgetBase::modify,this,&ApiDetailWidget::modify);
    emit modify();
}

void ApiDetailWidget::slotRemoveRow()
{
    if(m_tabWidget->currentWidget() == m_methodWidget)
        m_methodWidget->slotRemoveRow();
    else if(m_tabWidget->currentWidget() == m_eventWidget)
        m_eventWidget->slotRemoveRow();
    else if(m_tabWidget->currentWidget() == m_fieldWidget)
        m_fieldWidget->slotRemoveRow();
    else if(m_tabWidget->currentWidget() == m_pinWidget)
        m_pinWidget->slotRemoveRow();
}

void ApiDetailWidget::slotTreeReload()
{
    apiInfo->LoadWidget(m_ServiceName);
    treeWidget->expandAll();
    treeWidget->resizeColumnToContents(0);
}

}}
