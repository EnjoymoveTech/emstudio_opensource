#include "apiwidget.h"
#include "descriptioneditorconstants.h"
#include "apilistwidget.h"
#include "apidetailwidget.h"
#include "descriptioneditor.h"

#include <emostools/api_description_parser.h>
#include <utils/styledbar.h>
#include <utils/utilsicons.h>
#include <coreplugin/actionmanager/command.h>

#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QDockWidget>
#include <QDate>
#include <QDomText>
#include <QToolButton>
#include <QFileDialog>
#include <explorer/projecttree.h>
#include <explorer/project.h>
#include <apiwidget/apiexcelparser.h>
#include <coreplugin/messagemanager.h>

using namespace EmosTools;

namespace DescriptionEditor {
namespace Internal {

ApiWidget::ApiWidget(DescriptionEditor* editor, QWidget *parent) :
    DescriptionEditorWidget(editor, parent)
{
    init();
}

ApiWidget::~ApiWidget()
{
    qDeleteAll(m_apiDetailListWidget);
}

void ApiWidget::init()
{
    QHBoxLayout* layout = new QHBoxLayout(mainWidget);

    m_stackedWidget = new QStackedWidget(this);
    m_apiListWidget = new ApiListWidget(this);

    connect(m_apiListWidget, &ApiListWidget::sigSelectChanged, this, &ApiWidget::textChanged);
    connect(m_apiListWidget, &ApiListWidget::sigAddService, this, &ApiWidget::slotAddService);
    connect(m_apiListWidget, &ApiListWidget::sigRemoveService, this, &ApiWidget::slotRemoveService);
    connect(m_apiListWidget, &ApiListWidget::sigModifyService, this, &ApiWidget::slotModifyService);

    //api.description mainWidget.
    mainSplitter = new Core::MiniSplitter(Qt::Horizontal);
    mainSplitter->addWidget(m_apiListWidget);
    mainSplitter->addWidget(m_stackedWidget);
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 3);
    layout->addWidget(mainSplitter);

    projectDirectory = ProjectExplorer::ProjectTree::currentProject()->projectDirectory().toString();
}

QDomDocument ApiWidget::saveWidget()
{
    QDomDocument doc;

    QDomProcessingInstruction ins;
    ins = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"");
    doc.appendChild(ins);

    QDomElement rootElement = doc.createElement("emos:ddl");
    rootElement.setAttribute("xmlns:emos", "emos");
    doc.appendChild(rootElement);

    QDomElement headerElement = doc.createElement("header");
    QDomElement versionElement = doc.createElement("version");versionElement.appendChild(doc.createTextNode("1.0"));headerElement.appendChild(versionElement);
    QDomElement authorElement = doc.createElement("author");authorElement.appendChild(doc.createTextNode("EMOS"));headerElement.appendChild(authorElement);
    QDomElement date_creationElement = doc.createElement("date_creation");date_creationElement.appendChild(doc.createTextNode(QDate::currentDate().toString("yyyy-MM-dd")));headerElement.appendChild(date_creationElement);
    QDomElement date_changeElement = doc.createElement("date_change");date_changeElement.appendChild(doc.createTextNode(QDate::currentDate().toString("yyyy-MM-dd")));headerElement.appendChild(date_changeElement);
    QDomElement descriptionElement = doc.createElement("description");descriptionElement.appendChild(doc.createTextNode("NULL"));headerElement.appendChild(descriptionElement);
    QDomElement typenameElement = doc.createElement("typename");typenameElement.appendChild(doc.createTextNode("type"));headerElement.appendChild(typenameElement);
    rootElement.appendChild(headerElement);

    QDomElement servicesElement = doc.createElement("services");
    rootElement.appendChild(servicesElement);

    for(const auto& srv : m_apiDetailListWidget)
    {
        QDomElement serviceElement = doc.createElement("service");
        serviceElement.setAttribute("name", srv->serviceName());
        serviceElement.setAttribute("id", srv->serviceId());
        serviceElement.setAttribute("version", srv->serviceVersion());

        srv->widgetData(&doc, &serviceElement);

        servicesElement.appendChild(serviceElement);

    }

    return doc;
}

void ApiWidget::loadWidget(QDomDocument* doc)
{
    m_doc = doc;
    QList<tSrvService> services = m_apiListWidget->loadService(doc);
    if(services.size() == 0)
        return;

    if(m_apiDetailListWidget.size() == 0)
    {
        for(const auto& srv : services)
        {
            ApiDetailWidget* detail = new ApiDetailWidget(doc, this);
            detail->setServiceName(srv.name);
            detail->setServiceId(QString::number(srv.id));
            detail->setServiceVersion(QString::number(srv.version));
            detail->loadWidget(srv);
            connect(detail,&ApiDetailWidget::modify,this,&ApiWidget::slotModify);

            m_stackedWidget->addWidget(detail);
            m_apiDetailListWidget.push_back(detail);
        }
    }

    else
    {
        for(int i = 0; i < services.size(); i++)
        {
            if(i < m_apiDetailListWidget.size())
            {
                ApiDetailWidget* detail = m_apiDetailListWidget.at(i);
                detail->setServiceName(services[i].name);
                detail->setServiceId(QString::number(services[i].id));
                detail->setServiceVersion(QString::number(services[i].version));
                detail->loadWidget(services[i]);
            }
            else
            {
                ApiDetailWidget* detail = new ApiDetailWidget(doc, this);
                detail->setServiceName(services[i].name);
                detail->setServiceId(QString::number(services[i].id));
                detail->setServiceVersion(QString::number(services[i].version));
                detail->loadWidget(services[i]);
                connect(detail,&ApiDetailWidget::modify,this,&ApiWidget::slotModify);

                m_stackedWidget->addWidget(detail);
                m_apiDetailListWidget.push_back(detail);
            }
        }
    }

    if(m_apiDetailListWidget.size() > 0)
        m_stackedWidget->setCurrentIndex(0);
}

void ApiWidget::updateCombo()
{
    for(auto& detail : m_apiDetailListWidget)
    {
        detail->updateCombo();
    }
}

void ApiWidget::textChanged(const QString &currentText)
{
    ApiDetailWidget* detail = qobject_cast<ApiDetailWidget*>(m_stackedWidget->currentWidget());
    tabSelect selection;
    if(detail)
        selection = detail->currentTab();

    for(const auto& srv : m_apiDetailListWidget)
    {
        if(srv->serviceName() == currentText)
        {
            if(detail)
                srv->setTab(selection);
            m_stackedWidget->setCurrentWidget(srv);
            break;
        }
    }
}

void ApiWidget::slotAddRow()
{
    for(const auto& srv : m_apiDetailListWidget)
    {
        if(srv == m_stackedWidget->currentWidget())
        {
            srv->slotAddRow();
            break;
        }
    }
}

void ApiWidget::slotRemoveRow()
{
    for(const auto& srv : m_apiDetailListWidget)
    {
        if(srv == m_stackedWidget->currentWidget())
        {
            srv->slotRemoveRow();
            break;
        }
    }
}

void ApiWidget::slotImportExcel()
{
    QString defaultPath = projectDirectory + "/config";
    QString fileName = QFileDialog::getOpenFileName(mainWidget, "Choose excel to import:", defaultPath, "Excel(*xlsx)");
    ApiExcelParser* excelParser = new ApiExcelParser();
    excelParser->setData(fileName, m_doc);
    loadWidget(m_doc);
    slotModify();
}

void ApiWidget::slotAddService(const QString &service)
{
    ApiDetailWidget* detail = new ApiDetailWidget(m_doc, this);
    detail->setServiceName(service);
    detail->setServiceId(QString::number(makeUniquelyServiceId()));
    detail->setServiceVersion(QString::number(1));
    connect(detail,&ApiDetailWidget::modify,this,&ApiWidget::slotModify);

    m_stackedWidget->addWidget(detail);
    m_stackedWidget->setCurrentWidget(detail);
    m_apiDetailListWidget.push_back(detail);

    detail->slotTreeReload();
    slotModify();
}

void ApiWidget::slotRemoveService(const QString &service, const QString &selectService)
{
    // mainWidget
    for(ApiDetailWidget* srv : m_apiDetailListWidget)
    {
        if(srv->serviceName() == service)
        {
            m_apiDetailListWidget.removeOne(srv);
            srv->disconnect();
            srv->setParent(nullptr);
            srv->deleteLater();
            break;
        }
    }
    for(ApiDetailWidget* srv : m_apiDetailListWidget)
    {
        if(srv->serviceName() == selectService)
        {
            m_stackedWidget->setCurrentWidget(srv);
            break;
        }
    }

    slotModify();
}

void ApiWidget::slotModifyService(const QString &service, const QString &newName)
{
    // mainWidget
    for(ApiDetailWidget* srv : m_apiDetailListWidget)
    {
        if(srv->serviceName() == service)
        {
            srv->setServiceName(newName);
            srv->slotTreeReload();
            break;
        }
    }

    slotModify();
}

int ApiWidget::makeUniquelyServiceId()
{
    QList<int> serviceIdList;
    for(const auto& srv : m_apiDetailListWidget)
    {
        serviceIdList << srv->serviceId().toInt();
    }
    int id = 1;

    if (!serviceIdList.contains(id))
        return id;

    id ++;
    while (serviceIdList.contains(id))
        id++;
    return id;
}

}}
