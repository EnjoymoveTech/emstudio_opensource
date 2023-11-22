#include <QGridLayout>
#include "idleditor.h"
#include "idldocument.h"
#include "datahandle.h"
#include <emostools/api_description_parser.h>
#include <coreplugin/editormanager/editormanager.h>
#include "servicewidget.h"
using namespace EmosTools;
namespace Idl{
namespace Internal{

ServiceWidget::ServiceWidget(IdlEditor* editor, QWidget *parent) :
    QWidget(parent),
    m_editor(editor)
{
    QGridLayout *pLayout = new QGridLayout(this);
    EmosWidgets::DropFlowToolBox* serviceListWidget = new EmosWidgets::DropFlowToolBox(this);
    serviceListWidget->setFrameShape(EmosWidgets::FlowToolBox::Box);
    QPalette serviceListpal(serviceListWidget->palette());
    serviceListpal.setColor(QPalette::Background, Qt::white);
    serviceListWidget->setPalette(serviceListpal);
    EmosWidgets::DropFlowToolBox* serviceWidget = new EmosWidgets::DropFlowToolBox(this);
    serviceWidget->setFrameShape(EmosWidgets::FlowToolBox::Box);
    QPalette servicepal(serviceWidget->palette());
    servicepal.setColor(QPalette::Background, Qt::white);
    serviceWidget->setPalette(servicepal);
    EmosWidgets::DropFlowToolBox* quetoServiceWidget = new EmosWidgets::DropFlowToolBox(this);
    quetoServiceWidget->setFrameShape(EmosWidgets::FlowToolBox::Box);
    QPalette quetoServicepal(quetoServiceWidget->palette());
    quetoServicepal.setColor(QPalette::Background, Qt::white);
    quetoServiceWidget->setPalette(quetoServicepal);

    m_serviceListGroup = new EmosWidgets::DropFlowGroupWidget("ServiceList", false);
    m_serviceListGroup->setIcon(QIcon(":/utils/images/extension.png"));
    m_serviceListGroup->setEnableCollapsed(false);

    m_serviceGroup = new EmosWidgets::DropFlowGroupWidget("Provide Service");
    m_serviceGroup->setIcon(QIcon(":/utils/images/fileexport.png"));
    m_serviceGroup->setEnableCollapsed(false);
    m_serviceGroup->setDeleteSupport();
    m_serviceGroup->setDragDeleteSupport();

    m_quetoServiceGroup = new EmosWidgets::DropFlowGroupWidget("Use Service");
    m_quetoServiceGroup->setIcon(QIcon(":/utils/images/download.png"));
    m_quetoServiceGroup->setEnableCollapsed(false);
    m_quetoServiceGroup->setDeleteSupport();
    m_quetoServiceGroup->setDragDeleteSupport();

    serviceListWidget->appendGroup(m_serviceListGroup);
    serviceWidget->appendGroup(m_serviceGroup);
    quetoServiceWidget->appendGroup(m_quetoServiceGroup);
    //0,0开始,占12行5列
    pLayout->addWidget(serviceListWidget,0,0,12,5);
    //0,6开始,占5行5列
    pLayout->addWidget(serviceWidget,0,6,6,5);
    //6,6开始,占5行5列
    pLayout->addWidget(quetoServiceWidget,6,6,6,5);
    //设置间隔为10
    //pLayout->setHorizontalSpacing(10);
    //pLayout->setVerticalSpacing(10);
    //pLayout->setContentsMargins(10, 10, 10, 10);
    connect(Core::EditorManager::instance(), &Core::EditorManager::currentEditorChanged,
            this, &ServiceWidget::onCurrenWidgetChange);
    connect(m_serviceGroup, &EmosWidgets::DropFlowGroupWidget::itemDroped,
            this , &ServiceWidget::onModify);
    connect(m_serviceGroup, &EmosWidgets::DropFlowGroupWidget::itemRemoved,
            this , &ServiceWidget::onModify);
    connect(m_quetoServiceGroup, &EmosWidgets::DropFlowGroupWidget::itemDroped,
            this , &ServiceWidget::onModify);
    connect(m_quetoServiceGroup, &EmosWidgets::DropFlowGroupWidget::itemRemoved,
            this , &ServiceWidget::onModify);
}

ServiceWidget::~ServiceWidget()
{
    delete m_quetoServiceGroup;
    delete m_serviceGroup;
    delete m_serviceListGroup;
}

void ServiceWidget::loadFromDoc()
{
    updateDataForSelectService(SERVICELIST_GROUP);
    updateDataForSelectService(SERVICE_GROUP);
    updateDataForSelectService(QUOTESERVICE_GROUP);
}

void ServiceWidget::exportToDoc()
{
    QDomDocument* doc = qobject_cast<IdlDocument*>(m_editor->document())->getRootDoc();
    QDomElement deRootNode = doc->documentElement();
    Datahandle::removeChild(&deRootNode,"Service");
    Datahandle::removeChild(&deRootNode,"QuoteService");

    exportDocForSelectService(SERVICE_GROUP);
    exportDocForSelectService(QUOTESERVICE_GROUP);
}

QStringList ServiceWidget::getServices()
{
    return m_serviceGroup->itemNames();
}

QStringList ServiceWidget::getQuetoServices()
{
    return m_quetoServiceGroup->itemNames();
}

void ServiceWidget::onCurrenWidgetChange(Core::IEditor *editor)
{
    if(editor == m_editor){
        EmosTools::ApiParser::getInstance()->reloadXml();
        loadFromDoc();
        emit CurrentWidgetChange(this);
    }
}

void ServiceWidget::onModify()
{
    //需要更新QuoteService和Service节点后再发送通知
    exportToDoc();
    emit CurrentWidgetChange(this);
    qobject_cast<TextEditor::TextDocument*>(m_editor->document())->document()->setModified(true);
}

void ServiceWidget::onServiceChange()
{
    exportToDoc();
}

//检查是否实现/引用服务与当前服务一致
void ServiceWidget::updateDataForSelectService(SERVICETYPE type)
{
    QString serviceTypeName;
    EmosTools::ApiParser::getInstance()->reloadXml();
    QDomDocument* doc = qobject_cast<IdlDocument*>(m_editor->document())->getRootDoc();
    QList<tSrvService> services = ApiParser::getInstance()->getServices();
    switch(type){
    case SERVICELIST_GROUP:
        m_serviceListGroup->clearItem();
        for(int i = 0; i < services.size(); i++){
            if(!services[i].hasApi())
                continue;
            additemforGroup(services[i].name, SERVICELIST_GROUP);
        }
        return;
    case SERVICE_GROUP:
        m_serviceGroup->clearItem();
        serviceTypeName = "Service";
        break;
    case QUOTESERVICE_GROUP:
        m_quetoServiceGroup->clearItem();
        serviceTypeName = "QuoteService";
        break;
    default:
        return;
    }
    QDomNodeList serviceNodeList = doc->documentElement().elementsByTagName(serviceTypeName);
    for(int i = 0; i < serviceNodeList.size(); i++){
        bool flag = false;
        for(int j = 0; j < services.size(); j++){
            if(services[j].name == (serviceNodeList.at(i).toElement().attribute("name"))){
                additemforGroup(services[j].name, type);
                flag = true;
            }
        }
        if(flag == false){
            onServiceChange();
        }
    }
}

//新增item到对应的group里面
void ServiceWidget::additemforGroup(QString name, SERVICETYPE type)
{
    switch(type){
    case SERVICELIST_GROUP:
        m_serviceListGroup->appendItem(name);
        break;
    case SERVICE_GROUP:
        m_serviceGroup->appendItem(name);
        break;
    case QUOTESERVICE_GROUP:
        m_quetoServiceGroup->appendItem(name);
        break;
    default:
        return;
    }
}

//保存选中/引用服务到doc中
void ServiceWidget::exportDocForSelectService(SERVICETYPE type)
{
    QDomDocument* doc = qobject_cast<IdlDocument*>(m_editor->document())->getRootDoc();
    QDomElement deRootNode = doc->documentElement();
    QStringList itemsName;
    QString selectNameType;
    switch(type){
    case SERVICE_GROUP:
        selectNameType = "Service";
        itemsName = m_serviceGroup->itemNames();
        break;
    case QUOTESERVICE_GROUP:
        selectNameType = "QuoteService";
        itemsName = m_quetoServiceGroup->itemNames();
        break;
    default:
        return;
    }
    Datahandle::removeChild(&deRootNode, selectNameType);
    for(int i = 0; i < itemsName.size(); i++){
        QDomElement serviceElement = doc->createElement(selectNameType);
        deRootNode.appendChild(serviceElement);
        serviceElement.setAttribute("name",itemsName[i]);
    }
}

}}
