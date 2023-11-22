#include "descriptioneditor.h"
#include "descriptioneditorwidget.h"
#include "descriptioneditorconstants.h"
#include "apiwidget.h"
#include "typewidget.h"
#include "descriptiondocument.h"

#include <emostools/type_description_parser.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/actionmanager/command.h>

#include <utils/styledbar.h>
#include <utils/utilsicons.h>

#include <QToolButton>
#include <QStackedWidget>
#include <QLabel>
#include <QFrame>
#include <QDomDocument>
#include <QHBoxLayout>

using namespace EmosTools;

namespace DescriptionEditor {
namespace Internal {

class DescriptionEditorPrivate
{
public:
    DescriptionEditorWidget* currentWidget = nullptr;
    ApiWidget* apiWidget = nullptr;
    TypeWidget* typeWidget = nullptr;
    DescriptionDocument *document = nullptr;
    QStackedWidget *widgetStack = nullptr;
    QLabel* noDiagramLabel = nullptr;
    Core::MiniSplitter *rightSplitter = nullptr;
    QWidget* toolbar = nullptr;
    QWidget* toolbarRight = nullptr;
    QWidget* toolbarApi = nullptr;
    QWidget* toolbarType = nullptr;
    QDomDocument doc;
    Utils::Id m_lastSwitchID;
};

DescriptionEditor::DescriptionEditor()
: d(new DescriptionEditorPrivate)
{
    d->document = new DescriptionDocument(this);
    init();

    connect(Core::EditorManager::instance(), &Core::EditorManager::currentEditorChanged, this, &DescriptionEditor::editorChanged);
}

DescriptionEditor::~DescriptionEditor()
{
    disconnect();
    delete d->toolbarRight;
    delete d->toolbarApi;
    delete d->toolbarType;
    delete d->toolbar;
    delete d;d=nullptr;
}

IDocument *DescriptionEditor::document() const
{
    return d->document;
}

QWidget *DescriptionEditor::toolBar()
{
    return d->toolbar;
}

QString DescriptionEditor::saveWidget()
{
    if(d->currentWidget)
        return d->currentWidget->saveWidget().toString();
    return QString();
}

void DescriptionEditor::loadWidget(const QString &text)
{
    d->doc.setContent(text);

    DescriptionType type = descriptionType(&d->doc);
    if(type == DescriptionType::API)
    {
        d->widgetStack->setCurrentWidget(d->apiWidget);
        d->currentWidget = d->apiWidget;
        d->toolbarRight->setVisible(true);
        d->toolbarApi->setVisible(true);
        d->toolbarType->setVisible(false);
    }
    else if(type == DescriptionType::TYPE)
    {
        d->widgetStack->setCurrentWidget(d->typeWidget);
        d->currentWidget = d->typeWidget;
        d->toolbarRight->setVisible(false);
        d->toolbarApi->setVisible(false);
        d->toolbarType->setVisible(true);
        //d->typeWidget->slotText(true);
    }
    else
    {
        d->widgetStack->setCurrentWidget(d->noDiagramLabel);
        d->currentWidget = nullptr;
    }

    if(d->currentWidget)
        return d->currentWidget->loadWidget(&d->doc);
}

void DescriptionEditor::updateCombo(const QString &text)
{
    d->doc.setContent(text);

    DescriptionType type = descriptionType(&d->doc);
    if(type == DescriptionType::API)
    {
        d->apiWidget->updateCombo();
    }
    else
    {
        ;
    }
}

QWidget *DescriptionEditor::toolBarRight()
{
    return d->toolbarRight;
}

DescriptionType DescriptionEditor::descriptionType(Utils::FilePath files)
{
    QFile xmlfile(files.toString());
    if(!xmlfile.open(QIODevice::ReadOnly | QIODevice::Text))
        return DescriptionType::None;

    QDomDocument doc;
    doc.setContent(&xmlfile);

    return descriptionType(&doc);
}

DescriptionType DescriptionEditor::descriptionType(QDomDocument *doc)
{
    QDomNodeList nodes = doc->elementsByTagName("services");
    if(nodes.size() > 0)
        return DescriptionType::API;

    nodes = doc->elementsByTagName("structs");
    if(nodes.size() > 0)
        return DescriptionType::TYPE;

    return DescriptionType::None;
}

void DescriptionEditor::editorChanged(IEditor *editor)
{
    if (!editor)
        return;

    if (editor->document()->id() == Constants::DESCRIPTION_EDITOR_ID &&
            d->m_lastSwitchID.isValid())
    {
        TypeParser::getInstance()->reloadXml();
        qobject_cast<DescriptionEditor*>(editor)->updateCombo(qobject_cast<DescriptionDocument*>(editor->document())->plainText());
    }
    if (d->m_lastSwitchID == Constants::DESCRIPTION_EDITOR_ID)
    {
//        if(document()->isModified())
//        {
//            QString errorString;
//            document()->save(&errorString, document()->filePath().toString(), false);
//        }
    }
    d->m_lastSwitchID = editor->document()->id();
}

void DescriptionEditor::init()
{
    d->toolbar = new QWidget();
    d->toolbarRight = new  QWidget();
    d->toolbarApi = new  QWidget();
    d->toolbarType = new  QWidget();

    d->widgetStack = new QStackedWidget();

    d->noDiagramLabel = new QLabel(d->widgetStack);
    const QString placeholderText =
                tr("<html><body style=\"color:#909090; font-size:14px\">"
                   "<div align='center'>"
                   "<div style=\"font-size:20px\">Open a diagram</div>"
                   "<table><tr><td>"
                   "<hr/>"
                   "<div style=\"margin-top: 5px\">&bull; Double-click on diagram in model tree</div>"
                   "<div style=\"margin-top: 5px\">&bull; Select \"Open Diagram\" from package's context menu in model tree</div>"
                   "</td></tr></table>"
                   "</div>"
                   "</body></html>");
    d->noDiagramLabel->setText(placeholderText);

    d->apiWidget = new ApiWidget(this, d->rightSplitter);
    d->typeWidget = new TypeWidget(this, d->rightSplitter);

    addToorBar();

    d->widgetStack->addWidget(d->apiWidget);
    d->widgetStack->addWidget(d->typeWidget);
    d->widgetStack->addWidget(d->noDiagramLabel);
    d->widgetStack->setCurrentWidget(d->noDiagramLabel);

    setWidget(d->widgetStack);
}

void DescriptionEditor::addToorBar()
{
    auto toolBarLayout = new QHBoxLayout(d->toolbar);
    toolBarLayout->setContentsMargins(0, 0, 0, 0);
    toolBarLayout->setSpacing(5);

    Core::Context context(Constants::DESCRIPTION_EDITOR_ID);
    QAction* m_textaction = new QAction(Utils::Icons::BOOKMARK_TEXTEDITOR.icon(), tr("show text"), this);
    m_textaction->setCheckable(true);
    Core::Command *cmd = Core::ActionManager::command(Constants::ACTION_TEXT_ID);
    if(!cmd)
        cmd = Core::ActionManager::registerAction(m_textaction, Constants::ACTION_TEXT_ID, context);
    connect(m_textaction, &QAction::triggered, d->apiWidget, &ApiWidget::slotText);
    connect(m_textaction, &QAction::triggered, d->typeWidget, &TypeWidget::slotText);

    QAction* m_addAction = new QAction(Utils::Icons::PLUS_TOOLBAR.icon(), tr("Add"), this);
    cmd = Core::ActionManager::command(Constants::ACTION_ADD_ID);
    if(!cmd)
        cmd = Core::ActionManager::registerAction(m_addAction, Constants::ACTION_ADD_ID, context);
    connect(m_addAction, &QAction::triggered, d->apiWidget, &ApiWidget::slotAddRow);

    QAction* m_removeAction = new QAction(Utils::Icons::NOTLOADED.icon(), tr("Remove"), this);
    cmd = Core::ActionManager::command(Constants::ACTION_REMOVE_ID);
    if(!cmd)
        cmd = Core::ActionManager::registerAction(m_removeAction, Constants::ACTION_REMOVE_ID, context);
    connect(m_removeAction, &QAction::triggered, d->apiWidget, &ApiWidget::slotRemoveRow);

    QAction* m_importExcelAction = new QAction(Utils::Icons::IMPORTC.icon(), tr("Import"), this);
    cmd = Core::ActionManager::command(Constants::ACTION_IMPORTEXCEL_ID);
    if(!cmd)
        cmd = Core::ActionManager::registerAction(m_importExcelAction, Constants::ACTION_IMPORTEXCEL_ID, context);
    connect(m_importExcelAction, &QAction::triggered, d->apiWidget, &ApiWidget::slotImportExcel);

    QAction* m_importMergeAction = new QAction(Utils::Icons::IMPORTA.icon(), tr("Merge"), this);
    cmd = Core::ActionManager::command(Constants::ACTION_IMPORTA_ID);
    if(!cmd)
        cmd = Core::ActionManager::registerAction(m_importMergeAction, Constants::ACTION_IMPORTA_ID, context);
    connect(m_importMergeAction, &QAction::triggered, d->typeWidget, &TypeWidget::slotImportMerge);

    QAction* m_importCoverAction = new QAction(Utils::Icons::IMPORTC.icon(), tr("Cover"), this);
    cmd = Core::ActionManager::command(Constants::ACTION_IMPORTC_ID);
    if(!cmd)
        cmd = Core::ActionManager::registerAction(m_importCoverAction, Constants::ACTION_IMPORTC_ID, context);
    connect(m_importCoverAction, &QAction::triggered, d->typeWidget, &TypeWidget::slotImportCover);

    QAction* m_exportAction = new QAction(Utils::Icons::EXPORTFILE_TOOLBAR.icon(), tr("Export"), this);
    cmd = Core::ActionManager::command(Constants::ACTION_EXPORT_ID);
    if(!cmd)
        cmd = Core::ActionManager::registerAction(m_exportAction, Constants::ACTION_EXPORT_ID, context);
    connect(m_exportAction, &QAction::triggered, d->typeWidget, &TypeWidget::slotExport);

    QHBoxLayout* layoutToolBarApi = new QHBoxLayout(d->toolbarApi);
    layoutToolBarApi->setContentsMargins(0, 0, 0, 0);
    layoutToolBarApi->setSpacing(5);
    QHBoxLayout* layoutToolBarType = new QHBoxLayout(d->toolbarType);
    layoutToolBarType->setContentsMargins(0, 0, 0, 0);
    layoutToolBarType->setSpacing(5);

    toolBarLayout->addWidget(Core::Command::toolButtonWithAppendedShortcut(m_textaction, cmd));

    layoutToolBarApi->addWidget(Core::Command::toolButtonWithAppendedShortcut(m_addAction, cmd));
    layoutToolBarApi->addWidget(Core::Command::toolButtonWithAppendedShortcut(m_removeAction, cmd));
    layoutToolBarApi->addWidget(Core::Command::toolButtonWithAppendedShortcut(m_importExcelAction, cmd));
    layoutToolBarApi->addStretch(1);

    layoutToolBarType->addWidget(Core::Command::toolButtonWithAppendedShortcut(m_importCoverAction, cmd));
    layoutToolBarType->addWidget(Core::Command::toolButtonWithAppendedShortcut(m_exportAction, cmd));
    layoutToolBarType->addWidget(Core::Command::toolButtonWithAppendedShortcut(m_importMergeAction, cmd));
    layoutToolBarType->addStretch(1);

    toolBarLayout->addWidget(d->toolbarApi);
    toolBarLayout->addWidget(d->toolbarType);
    toolBarLayout->addStretch(1);

    toolBarLayout->addWidget(d->toolbarRight);
}

}}
