#include "apilistwidget.h"
#include "descriptioneditorconstants.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QAction>
#include <QToolButton>

#include <emostools/api_description_parser.h>

#include <utils/emoswidgets/messagetips.h>

#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/icontext.h>
#include <utils/stringutils.h>
#include <utils/styledbar.h>
#include <utils/utilsicons.h>

#include <emostools/idlparser.h>
#include <coreplugin/editormanager/editormanager.h>
#include <utils/mimetypes/mimetype.h>
#include <utils/mimetypes/mimedatabase.h>
#include <explorer/project.h>
#include <explorer/projecttree.h>

using namespace EmosTools;

namespace DescriptionEditor {
namespace Internal {

ApiListWidget::ApiListWidget(QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle("Service");
    QVBoxLayout* layout = new QVBoxLayout(this);
    //layout->setContentsMargins(1,1,1,1);
    //layout->setSpacing(0);

    m_listWidget = new QListWidget(this);
    m_listWidget->setEditTriggers(QAbstractItemView::DoubleClicked);

    connect(m_listWidget, &QListWidget::currentTextChanged, this, &ApiListWidget::sigSelectChanged);
    connect(m_listWidget, &QListWidget::itemDoubleClicked, [&](QListWidgetItem *item){
        m_curEditorName = item->text();
    });
    connect(m_listWidget, &QListWidget::itemChanged, this, &ApiListWidget::itemChanged);

    addToorBar(layout);
    layout->addWidget(m_listWidget);
}

ApiListWidget::~ApiListWidget()
{

}

void ApiListWidget::addToorBar(QBoxLayout* layout)
{
    QHBoxLayout* layoutToolBar = new QHBoxLayout();
    layoutToolBar->setContentsMargins(0, 0, 0, 0);
    //layoutToolBar->setSpacing(5);
    layout->addLayout(layoutToolBar);

    Core::Context context(Constants::DESCRIPTION_EDITOR_ID);
    QAction* m_addaction = new QAction(Utils::Icons::PLUS_TOOLBAR.icon(), tr("add service"), this);
    Core::Command *cmd = Core::ActionManager::command(Constants::ACTION_SERVICE_ADD_ID);
    if(!cmd)
        cmd = Core::ActionManager::registerAction(m_addaction, Constants::ACTION_SERVICE_ADD_ID, context);
    connect(m_addaction, &QAction::triggered, this, &ApiListWidget::slotAddRow);

    QAction* m_removeaction = new QAction(Utils::Icons::NOTLOADED.icon(), tr("remove service"), this);
    cmd = Core::ActionManager::command(Constants::ACTION_SERVICE_REMOVE_ID);
    if(!cmd)
        cmd = Core::ActionManager::registerAction(m_removeaction, Constants::ACTION_SERVICE_REMOVE_ID, context);
    connect(m_removeaction, &QAction::triggered, this, &ApiListWidget::slotRemoveRow);

    layoutToolBar->addWidget(new QLabel("Service Switch", this));
    layoutToolBar->addWidget(Core::Command::toolButtonWithAppendedShortcut(m_addaction, cmd));
    layoutToolBar->addWidget(Core::Command::toolButtonWithAppendedShortcut(m_removeaction, cmd));
    layoutToolBar->addStretch(1);
}

void ApiListWidget::serviceChangeName(const QString &oldName, const QString &newName)
{
    QDir dir(ProjectExplorer::ProjectTree::currentProject()->projectDirectory().toString() + "/2_component_design");
    QFileInfoList infoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    QListIterator<QFileInfo> iter(infoList);
    while(iter.hasNext())
    {
        QFileInfo info = iter.next();
        if(!info.isFile())
            continue;
        if(!info.fileName().endsWith(".idl"))
            continue;
        IDLStruct data = IdlParser::getIdlStructByFile(info.filePath());
        if(data.name == oldName)
        {
            data.name = newName;
            data.oid.replace(oldName.toLower(), newName.toLower());
            QFile::remove(info.filePath());
        }
        QMapIterator<QString, int> serviceIt(data.service);
        while(serviceIt.hasNext())
        {
            serviceIt.next();
            if(serviceIt.key() != oldName)
                continue;
            data.service.insert(newName, serviceIt.value());
            data.service.remove(oldName);
        }
        QMapIterator<QString, int> quoteIt(data.quoteService);
        while(quoteIt.hasNext())
        {
            quoteIt.next();
            if(quoteIt.key() != oldName)
                continue;
            data.quoteService.insert(newName, quoteIt.value());
            data.quoteService.remove(oldName);
        }
        Utils::MimeType type = Utils::mimeTypeForName("application/emos.idl.xml");
        const void* idlData = &data;
        emit Core::EditorManager::instance()->autoCreateNewFile(type, idlData, Core::EditorManager::DescriptionType::Other, ProjectExplorer::ProjectTree::currentProject());
    }
}

QList<tSrvService> ApiListWidget::loadService(QDomDocument* doc)
{
    disconnect(m_listWidget, &QListWidget::itemChanged, this, &ApiListWidget::itemChanged);

    QList<tSrvService> services = ApiParser::getInstance()->loadFromDocument(doc);
    m_listWidget->clear();

    for(auto service : services)
    {
        QListWidgetItem *item = new QListWidgetItem(m_listWidget);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setText(service.name);
        m_listWidget->addItem(item);
    }
    m_listWidget->setCurrentRow(0);

    connect(m_listWidget, &QListWidget::itemChanged, this, &ApiListWidget::itemChanged);

    return services;
}

void ApiListWidget::slotAddRow()
{ 
    disconnect(m_listWidget, &QListWidget::itemChanged, this, &ApiListWidget::itemChanged);
    QListWidgetItem* item = new QListWidgetItem(Utils::makeUniquelyNumberedInsensitive(QString("newservice"), getListValue(), QString("")), m_listWidget);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    m_listWidget->addItem(item);
    m_listWidget->setCurrentItem(item);
    emit sigAddService(item->text());

    connect(m_listWidget, &QListWidget::itemChanged, this, &ApiListWidget::itemChanged);
    m_curEditorName = item->text();
    m_listWidget->editItem(item);
    addIdl(item->text());
}

void ApiListWidget::slotRemoveRow()
{
    disconnect(m_listWidget, &QListWidget::itemChanged, this, &ApiListWidget::itemChanged);

    QListWidgetItem* item = m_listWidget->takeItem(m_listWidget->currentRow());
    emit sigRemoveService(item->text(), m_listWidget->count() > 0 ? m_listWidget->currentItem()->text() : "");
    delete item;

    connect(m_listWidget, &QListWidget::itemChanged, this, &ApiListWidget::itemChanged);
}

void ApiListWidget::itemChanged(QListWidgetItem *item)
{
    if(item->text() == "")
    {
        EmosWidgets::MessageTips::showText(this->parentWidget()->parentWidget(), "name can't be empty");
        item->setText(m_curEditorName);
    }

    int count = 0;
    for(const auto& i : getListValue())
    {
        if(i.toLower() == item->text().toLower())
            count ++;
    }

    if(count > 1)
    {
        EmosWidgets::MessageTips::showText(this->parentWidget()->parentWidget(), "name can't same");
        item->setText(m_curEditorName);
    }
    else
    {
        emit sigModifyService(m_curEditorName, item->text());
        serviceChangeName(m_curEditorName, item->text());
    }
}

void ApiListWidget::addIdl(const QString &serviceName)
{
    IDLStruct idl;
    idl.name = serviceName;
    idl.oid = "emos.sdk." + serviceName.toLower();
    idl.isService = true;
    idl.priority = 10;
    idl.service.insert(serviceName, ApiParser::getInstance()->getService(serviceName).id);
    Utils::MimeType type = Utils::mimeTypeForName("application/emos.idl.xml");
    const void* data = &idl;
    emit Core::EditorManager::instance()->autoCreateNewFile(type, data, Core::EditorManager::DescriptionType::Other, ProjectExplorer::ProjectTree::currentProject());
}

QStringList ApiListWidget::getListValue()
{
    QStringList serviceList;
    for(int i = 0; i < m_listWidget->count(); i++)
    {
        serviceList << m_listWidget->item(i)->text();
    }
    return serviceList;
}

}}
