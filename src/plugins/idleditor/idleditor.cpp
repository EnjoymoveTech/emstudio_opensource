#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/minisplitter.h>

#include <utils/fileutils.h>
#include <utils/qtcassert.h>
#include <utils/styledbar.h>
#include <utils/utilsicons.h>
#include <QDomDocument>
#include <QLayout>
#include <QToolButton>
#include <QAction>
#include "objectInspectorwindows.h"
#include "propertywidget.h"
#include "propertyeditorwidget.h"
#include "threadwidget.h"
#include "servicewidget.h"
#include "inspectormodel.h"
#include "servicetabwidget.h"
#include "idleditorwidget.h"
#include "idleditorconstants.h"
#include "idldocument.h"
#include "idleditor.h"
namespace Idl {
namespace Internal{

class IdlEditorPrivate
{
public:
    //编辑idl属性的右下角
    PropertyEditorWidget* m_propertyEditorWidget = nullptr;
    //显示idl所有信息的窗口
    InspectorModel* m_modelInspector = nullptr;
    QTreeView* m_iDLDataShowWidget = nullptr;
    //下方显示service的信息
    ServiceTabWidget* m_serviceTabWidget = nullptr;
    //主窗口
    IdlEditorWidget* m_idlEditorWidget = nullptr;
    //文件类
    IdlDocument* m_document = nullptr;
    //操作thread或者property的增加和减少的工具栏
    QWidget* m_toolbar = nullptr;
};

IdlEditor::IdlEditor()
    : d(new IdlEditorPrivate)
{
    widgetInit();
}

void IdlEditor::widgetInit()
{
    Core::MiniSplitter *w =     new Core::MiniSplitter;
    Core::MiniSplitter *rightHorizSplitter = new Core::MiniSplitter(w);
    d->m_propertyEditorWidget = new PropertyEditorWidget(this);
    d->m_iDLDataShowWidget = new Internal::InspectorTreeView;
    d->m_modelInspector =       new InspectorModel(d->m_iDLDataShowWidget);
    d->m_iDLDataShowWidget->setModel(d->m_modelInspector);
    d->m_serviceTabWidget =     new ServiceTabWidget(this);
    d->m_idlEditorWidget =      new IdlEditorWidget(this);
    d->m_document =             new IdlDocument(this);
    d->m_toolbar =              new QWidget();
    Core::MiniSplitter* mainWidget = new Core::MiniSplitter;
    mainWidget->insertWidget(0,d->m_idlEditorWidget);
    mainWidget->insertWidget(1,d->m_serviceTabWidget);
    mainWidget->setStretchFactor(0, 4);
    mainWidget->setStretchFactor(1, 2);
    mainWidget->setOrientation(Qt::Vertical);

    rightHorizSplitter->setOrientation(Qt::Vertical);
    rightHorizSplitter->insertWidget(0, d->m_iDLDataShowWidget);
    rightHorizSplitter->insertWidget(1,d->m_propertyEditorWidget);
    rightHorizSplitter->setStretchFactor(0, 5);
    rightHorizSplitter->setStretchFactor(1, 1);

    w->insertWidget(0, mainWidget);
    w->insertWidget(1, rightHorizSplitter);
    w->setStretchFactor(0, 1);
    w->setStretchFactor(1, 0);

    initToolBar();
    setWidget(w);

    //连接thread的更改消息同步到右上角的显示窗口
    ThreadWidget * threadwidget = d->m_idlEditorWidget->getThreadWidget();
    connect(threadwidget, &ThreadWidget::threadChange, this, &IdlEditor::updateData);
    //连接property的更改消息同步到右上角的显示窗口
    PropertyWidget* propertywidget = d->m_idlEditorWidget->getPropertyWidget();
    connect(propertywidget, &PropertyWidget::propertyChange, this, &IdlEditor::updateData);

    ServiceWidget* servicewidget = d->m_idlEditorWidget->getServiceWidget();
    connect(servicewidget, &ServiceWidget::CurrentWidgetChange, d->m_serviceTabWidget, &ServiceTabWidget::onWidgetChange);
    connect(servicewidget, &ServiceWidget::CurrentWidgetChange, this, &IdlEditor::updateData);

}

IdlEditor::~IdlEditor()
{
    delete d->m_toolbar;
    delete d->m_document;
    delete d->m_idlEditorWidget;
    delete d->m_serviceTabWidget;
    delete d->m_modelInspector;
    delete d->m_iDLDataShowWidget;
    delete d->m_propertyEditorWidget;
    delete d;
    d = nullptr;
}

Core::IDocument *IdlEditor::document() const
{
    return d->m_document;
}

QWidget *IdlEditor::toolBar()
{
    return d->m_toolbar;
}

QString IdlEditor::saveWidget()
{
    QDomDocument *doc = d->m_document->getRootDoc();
    d->m_propertyEditorWidget->exportToDoc();
    d->m_idlEditorWidget->exportToDoc();
    return doc->toString();
}

void IdlEditor::loadWidget()
{
    d->m_propertyEditorWidget->loadFromDoc();
    //d->m_modelInspector->LoadFromDoc(doc);
    //d->m_iDLDataShowWidget->expandAll();
    //d->m_serviceTabWidget->loadFromDoc();
    d->m_idlEditorWidget->loadFromDoc();
}

void IdlEditor::updateData()
{
     QDomDocument *doc = d->m_document->getRootDoc();
     d->m_modelInspector->LoadFromDoc(doc);
    d->m_modelInspector->updateData();
    d->m_iDLDataShowWidget->expandAll();
}

void IdlEditor::initToolBar()
{
    QHBoxLayout* toolBarLayout = new QHBoxLayout(d->m_toolbar);
    toolBarLayout->setContentsMargins(0, 0, 0, 0);
    toolBarLayout->setSpacing(3);
    Core::Context context(Constants::IDLEDITOR_ID);
    Core::Command *cmd;

    QAction* m_addAction = new QAction(Utils::Icons::PLUS_TOOLBAR.icon(), tr("Add"), this);
    cmd = Core::ActionManager::registerAction(m_addAction, Constants::IDL_ADD, context);
    connect(m_addAction, &QAction::triggered, d->m_idlEditorWidget, &IdlEditorWidget::onappendEmptyRow);
    toolBarLayout->addWidget(Core::Command::toolButtonWithAppendedShortcut(m_addAction, cmd));

    QAction* m_removeAction = new QAction(Utils::Icons::NOTLOADED.icon(), tr("Remove"), this);
    cmd = Core::ActionManager::registerAction(m_removeAction, Constants::IDL_REMOVE, context);
    connect(m_removeAction, &QAction::triggered, d->m_idlEditorWidget, &IdlEditorWidget::onremoveOneRow);
    toolBarLayout->addWidget(Core::Command::toolButtonWithAppendedShortcut(m_removeAction, cmd));

    toolBarLayout->addStretch(1);
}

}
} // namespace Idl

