/****************************************************************************
**
** Copyright (C) 2016 Jochen Becher
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "modeleditor.h"

#include "actionhandler.h"
#include "diagramsviewmanager.h"
#include "editordiagramview.h"
#include "elementtasks.h"
#include "extdocumentcontroller.h"
#include "extpropertiesmview.h"
#include "modeldocument.h"
#include "modeleditor_constants.h"
#include "modeleditor_plugin.h"
#include "modelsmanager.h"
#include "openelementvisitor.h"
#include "uicontroller.h"

#include "qmt/controller/undocontroller.h"
#include "qmt/diagram/dpackage.h"
#include "qmt/diagram_controller/diagramcontroller.h"
#include "qmt/diagram_controller/dcontainer.h"
#include "qmt/diagram_controller/dreferences.h"
#include "qmt/diagram_controller/dselection.h"
#include "qmt/diagram_scene/diagramscenemodel.h"
#include "qmt/diagram_ui/diagram_mime_types.h"
#include "qmt/diagram_ui/diagramsmanager.h"
#include "qmt/model/mpackage.h"
#include "qmt/model/mclass.h"
#include "qmt/model/mcomponent.h"
#include "qmt/model/mcanvasdiagram.h"
#include "qmt/model_controller/modelcontroller.h"
#include "qmt/model_controller/mcontainer.h"
#include "qmt/model_controller/mreferences.h"
#include "qmt/model_controller/mselection.h"
#include "qmt/model_ui/treemodel.h"
#include "qmt/model_ui/treemodelmanager.h"
#include "qmt/model_widgets_ui/modeltreeview.h"
#include "qmt/model_widgets_ui/propertiesview.h"
#include "qmt/stereotype/shapepaintvisitor.h"
#include "qmt/stereotype/stereotypecontroller.h"
#include "qmt/stereotype/stereotypeicon.h"
#include "qmt/stereotype/toolbar.h"
#include "qmt/style/style.h"
#include "qmt/style/stylecontroller.h"
#include "qmt/tasks/diagramscenecontroller.h"
#include "qmt/tasks/finddiagramvisitor.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/icore.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/minisplitter.h>
#include <coreplugin/actionmanager/commandbutton.h>
#include <utils/fadingindicator.h>
#include <utils/styledbar.h>
#include <utils/qtcassert.h>
#include <utils/utilsicons.h>

#include <emostools/cfvparser.h>
#include <emostools/srv_parser.h>
#include <emostools/api_description_parser.h>
#include <emostools/type_description_parser.h>
#include <utils/emoswidgets/flowtoolbox.h>
#include <utils/emoswidgets/dragtool.h>

#include <QtXml/QDomDocument>
#include <QAction>
#include <QActionGroup>
#include <QComboBox>
#include <QDir>
#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QImageWriter>
#include <QLabel>
#include <QMap>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QScrollArea>
#include <QStackedWidget>
#include <QStyleFactory>
#include <QTimer>
#include <QToolBox>
#include <QUndoStack>
#include <QVBoxLayout>
#include <QMenu>
#include <QScrollBar>
#include <QMap>
#include <QList>
#include <QButtonGroup>
#include <QCheckBox>
#include <QListWidget>
#include <QListWidgetItem>

#include <algorithm>

using namespace EmosTools;

namespace ModelEditor {
namespace Internal {

static const char PROPERTYNAME_TOOLBARID[] = "ToolbarId";
static const double ZOOM_FACTOR = 1.05;

class ModelEditor::ModelEditorPrivate
{
public:
    UiController *uiController = nullptr;
    ActionHandler *actionHandler = nullptr;
    ModelDocument *document = nullptr;
    qmt::PropertiesView *propertiesView = nullptr;
    Core::MiniSplitter *rightSplitter = nullptr;
    Core::MiniSplitter *leftGroup = nullptr;
    EmosWidgets::FlowToolBox *leftToolBox = nullptr;
    QStackedWidget *diagramStack = nullptr;
    EditorDiagramView *diagramView = nullptr;
    QLabel *noDiagramLabel = nullptr;
    DiagramsViewManager *diagramsViewManager = nullptr;
    Core::MiniSplitter *rightHorizSplitter = nullptr;
    qmt::ModelTreeView *modelTreeView = nullptr;
    qmt::TreeModelManager *modelTreeViewServant = nullptr;
    QScrollArea *propertiesScrollArea = nullptr;
    QWidget *propertiesGroupWidget = nullptr;
    QWidget *toolbar = nullptr;
    QComboBox *diagramSelector = nullptr;
    SelectedArea selectedArea = SelectedArea::Nothing;
    QString lastExportDirPath;
    QList<EmosWidgets::DragTool*> toolList;
};

ModelEditor::ModelEditor(UiController *uiController, ActionHandler *actionHandler)
    : d(new ModelEditorPrivate)
{
    setContext(Core::Context(Constants::MODEL_EDITOR_ID));
    d->uiController = uiController;
    d->actionHandler = actionHandler;
    d->document = new ModelDocument(this);
    connect(d->document, &ModelDocument::contentSet, this, &ModelEditor::onContentSet);
    init();
}

ModelEditor::~ModelEditor()
{
    closeCurrentDiagram(false);
    delete d->toolbar;
    delete d;
}

Core::IDocument *ModelEditor::document() const
{
    return d->document;
}

QWidget *ModelEditor::toolBar()
{
    return d->toolbar;
}

QByteArray ModelEditor::saveState() const
{
    return saveState(currentDiagram());
}

void ModelEditor::restoreState(const QByteArray &state)
{
    QDataStream stream(state);
    int version = 0;
    stream >> version;
    if (version >= 1) {
        qmt::Uid uid;
        stream >> uid;
        if (version >= 2) {
        }
        if (uid.isValid()) {
            qmt::MDiagram *diagram = d->document->documentController()->modelController()->findObject<qmt::MDiagram>(uid);
            if (diagram) {
                openDiagram(diagram, false);
            }
        }
    }
}

void ModelEditor::init()
{
    // create and configure properties view
    d->propertiesView = new qmt::PropertiesView(this);

    // create and configure editor ui
    d->rightSplitter = new Core::MiniSplitter;
    connect(d->rightSplitter, &QSplitter::splitterMoved,
            this, &ModelEditor::onRightSplitterMoved);
    connect(d->uiController, &UiController::rightSplitterChanged,
            this, &ModelEditor::onRightSplitterChanged);

    d->leftGroup = new Core::MiniSplitter(d->rightSplitter);
    connect(d->leftGroup, &QSplitter::splitterMoved,
            this, &ModelEditor::onLeftGroupSplitterMoved);
    connect(d->uiController, &UiController::rightSplitterChanged,
            this, &ModelEditor::onLeftGroupSplitterChanged);

    d->leftToolBox = new EmosWidgets::FlowToolBox(d->rightSplitter);
    // Windows style does not truncate the tab label to a very small width (GTK+ does)
    static QStyle *windowsStyle = QStyleFactory().create("Windows");
    if (windowsStyle)
        d->leftToolBox->setStyle(windowsStyle);
    // TODO improve this (and the diagram colors) for use with dark theme

    d->diagramStack = new QStackedWidget(d->rightSplitter);
    d->diagramView = new EditorDiagramView(d->diagramStack);
    d->diagramStack->addWidget(d->diagramView);

    d->noDiagramLabel = new QLabel(d->diagramStack);
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
    d->diagramStack->addWidget(d->noDiagramLabel);
    d->diagramStack->setCurrentWidget(d->noDiagramLabel);

//    d->leftGroup->insertWidget(0, d->leftToolBox);
//    d->leftGroup->insertWidget(1, d->diagramStack);
//    d->leftGroup->setStretchFactor(0, 0);
//    d->leftGroup->setStretchFactor(1, 1);

    d->rightHorizSplitter = new Core::MiniSplitter(d->rightSplitter);
    d->rightHorizSplitter->setOrientation(Qt::Vertical);
    connect(d->rightHorizSplitter, &QSplitter::splitterMoved,
            this, &ModelEditor::onRightHorizSplitterMoved);
    connect(d->uiController, &UiController::rightHorizSplitterChanged,
            this, &ModelEditor::onRightHorizSplitterChanged);

    d->modelTreeView = new qmt::ModelTreeView(d->rightHorizSplitter);
    d->modelTreeView->setFrameShape(QFrame::NoFrame);

    d->modelTreeViewServant = new qmt::TreeModelManager(this);
    d->modelTreeViewServant->setModelTreeView(d->modelTreeView);

    d->propertiesScrollArea = new QScrollArea(d->rightHorizSplitter);
    d->propertiesScrollArea->setFrameShape(QFrame::NoFrame);
    d->propertiesScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    d->propertiesScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    d->propertiesScrollArea->setWidgetResizable(true);

    d->rightHorizSplitter->insertWidget(0, d->modelTreeView);
    d->rightHorizSplitter->insertWidget(1, d->propertiesScrollArea);
    d->rightHorizSplitter->setStretchFactor(0, 2); // magic stretch factors for equal sizing
    d->rightHorizSplitter->setStretchFactor(1, 3);

    //d->rightSplitter->insertWidget(0, d->leftGroup);
    d->rightSplitter->insertWidget(0, d->leftToolBox);
    d->rightSplitter->insertWidget(1, d->diagramStack);
    d->rightSplitter->insertWidget(2, d->rightHorizSplitter);
    d->rightSplitter->setStretchFactor(0, 0);
    d->rightSplitter->setStretchFactor(1, 1);
    d->rightSplitter->setStretchFactor(2, 0);

    setWidget(d->rightSplitter);

    // restore splitter sizes after any stretch factor has been set as fallback
    if (d->uiController->hasLeftGroupSplitterState())
        d->leftGroup->restoreState(d->uiController->leftGroupSplitterState());
    if (d->uiController->hasRightSplitterState())
        d->rightSplitter->restoreState(d->uiController->rightSplitterState());
    if (d->uiController->hasRightHorizSplitterState())
        d->rightHorizSplitter->restoreState(d->uiController->rightHorizSplitterState());

    // create and configure toolbar
    d->toolbar = new QWidget();
    auto toolbarLayout = new QHBoxLayout(d->toolbar);
    toolbarLayout->setContentsMargins(0, 0, 0, 0);
    toolbarLayout->setSpacing(0);

    auto openParentButton = new Core::CommandButton(Constants::OPEN_PARENT_DIAGRAM, d->toolbar);
    openParentButton->setDefaultAction(d->actionHandler->openParentDiagramAction());
    //toolbarLayout->addWidget(openParentButton);
    openParentButton->setVisible(false);

    d->diagramSelector = new QComboBox(d->toolbar);
    connect(d->diagramSelector, QOverload<int>::of(&QComboBox::activated),
            this, &ModelEditor::onDiagramSelectorSelected);
    //toolbarLayout->addWidget(d->diagramSelector, 1); 不显示
    d->diagramSelector->setVisible(false);

    toolbarLayout->addWidget(createToolbarCommandButton(Core::Constants::ZOOM_IN,
                                                        [this]() { zoomIn(); },
                                                        d->toolbar));
    toolbarLayout->addWidget(createToolbarCommandButton(Core::Constants::ZOOM_OUT,
                                                        [this]() { zoomOut(); },
                                                        d->toolbar));

    //视图显示筛选
    QToolButton * viewButton = new QToolButton(d->toolbar);
    viewButton->setDefaultAction(new QAction(Utils::Icons::EYE_OPEN_TOOLBAR.icon(), tr("View Show"), d->toolbar));

    //抓手模式
    QToolButton * panButton = new QToolButton(d->toolbar);
    panButton->setDefaultAction(d->actionHandler->panAction());
    connect(panButton, &QToolButton::clicked, this, &ModelEditor::pan);
    toolbarLayout->addWidget(panButton);

    QToolButton * exportDiagramButton = new QToolButton(d->toolbar);
    exportDiagramButton->setDefaultAction(d->actionHandler->exportDiagramAction());
    toolbarLayout->addWidget(exportDiagramButton);

//    QToolButton * viewButton = createToolbarCommandButton(Core::Constants::ZOOM_RESET,
//                                                          [this]() { },//resetZoom();
//                                                          d->toolbar);
    viewButton->setPopupMode(QToolButton::InstantPopup);
    toolbarLayout->addWidget(viewButton);

    auto filterMenu = new QMenu(viewButton);
    filterMenu->setTearOffEnabled(true);
    filterMenu->addAction(d->actionHandler->viewComponentAction());
    filterMenu->addAction(d->actionHandler->viewMethodAction());
    filterMenu->addAction(d->actionHandler->viewEventAction());
    filterMenu->addAction(d->actionHandler->viewFieldAction());
    filterMenu->addAction(d->actionHandler->viewPinAction());
    d->actionHandler->viewComponentAction()->setChecked(false);
    d->actionHandler->viewMethodAction()->setChecked(true);
    d->actionHandler->viewEventAction()->setChecked(true);
    d->actionHandler->viewFieldAction()->setChecked(true);
    d->actionHandler->viewPinAction()->setChecked(true);
    connect(d->actionHandler->viewComponentAction(), &QAction::toggled, this, &ModelEditor::viewFilter);
    connect(d->actionHandler->viewMethodAction(), &QAction::toggled, this, &ModelEditor::viewFilter);
    connect(d->actionHandler->viewEventAction(), &QAction::toggled, this, &ModelEditor::viewFilter);
    connect(d->actionHandler->viewFieldAction(), &QAction::toggled, this, &ModelEditor::viewFilter);
    connect(d->actionHandler->viewPinAction(), &QAction::toggled, this, &ModelEditor::viewFilter);
    viewButton->setMenu(filterMenu);

    //自动连线
    QToolButton * autoConnectButton = new QToolButton(d->toolbar);
    autoConnectButton->setDefaultAction(d->actionHandler->autoConnectAction());
    connect(autoConnectButton, &QToolButton::clicked, this, &ModelEditor::autoConnect);
    toolbarLayout->addWidget(autoConnectButton);

    //全部断开连线
    QToolButton *disconnectButton = new QToolButton(d->toolbar);
    disconnectButton->setDefaultAction(d->actionHandler->disconnectAction());
    connect(disconnectButton, &QToolButton::clicked, this, &ModelEditor::disconnect);
    toolbarLayout->addWidget(disconnectButton);

    //优化连线
    QToolButton *beautifulConnectButton = new QToolButton(d->toolbar);
    beautifulConnectButton->setDefaultAction(d->actionHandler->beautifulConnectAction());
    connect(beautifulConnectButton, &QToolButton::clicked, this, &ModelEditor::beautifulConnect);
    toolbarLayout->addWidget(beautifulConnectButton);

    //haojie.fang close tools
//    toolbarLayout->addWidget(createToolbarCommandButton(Constants::ACTION_ADD_PACKAGE,
//                                                        [this]() { onAddPackage(); },
//                                                        d->toolbar));
//    toolbarLayout->addWidget(createToolbarCommandButton(Constants::ACTION_ADD_COMPONENT,
//                                                        [this]() { onAddComponent(); },
//                                                        d->toolbar));
//    toolbarLayout->addWidget(createToolbarCommandButton(Constants::ACTION_ADD_CLASS,
//                                                        [this]() { onAddClass(); },
//                                                        d->toolbar));
//    toolbarLayout->addWidget(createToolbarCommandButton(Constants::ACTION_ADD_CANVAS_DIAGRAM,
//                                                        [this]() { onAddCanvasDiagram(); },
//                                                        d->toolbar));
    toolbarLayout->addStretch(1);
    toolbarLayout->addSpacing(20);
}

void ModelEditor::initDocument()
{
    if (d->diagramsViewManager)
        return;

    ExtDocumentController *documentController = d->document->documentController();

    d->diagramView->setPxNodeController(documentController->pxNodeController());

    d->diagramsViewManager = new DiagramsViewManager(this);
    connect(d->diagramsViewManager, &DiagramsViewManager::openNewDiagram,
            this, &ModelEditor::showDiagram);
    connect(d->diagramsViewManager, &DiagramsViewManager::closeOpenDiagram,
            this, &ModelEditor::closeDiagram);
    connect(d->diagramsViewManager, &DiagramsViewManager::closeAllOpenDiagrams,
            this, &ModelEditor::closeAllDiagrams);
    documentController->diagramsManager()->setDiagramsView(d->diagramsViewManager);

    d->propertiesView->setDiagramController(documentController->diagramController());
    d->propertiesView->setModelController(documentController->modelController());
    d->propertiesView->setStereotypeController(documentController->stereotypeController());
    d->propertiesView->setStyleController(documentController->styleController());
    d->propertiesView->setMViewFactory([=](qmt::PropertiesView *view) {
        auto extView = new ExtPropertiesMView(view);
        extView->setProjectController(documentController->projectController());
        return extView;
    });

    d->modelTreeView->setTreeModel(documentController->sortedTreeModel());
    d->modelTreeView->setElementTasks(documentController->elementTasks());

    d->modelTreeViewServant->setTreeModel(documentController->treeModel());

    connect(ModelEditorPlugin::modelsManager(), &ModelsManager::diagramClipboardChanged,
            this, &ModelEditor::onDiagramClipboardChanged, Qt::QueuedConnection);
    connect(documentController->undoController()->undoStack(), &QUndoStack::canUndoChanged,
            this, &ModelEditor::onCanUndoChanged, Qt::QueuedConnection);
    connect(documentController->undoController()->undoStack(), &QUndoStack::canRedoChanged,
            this, &ModelEditor::onCanRedoChanged, Qt::QueuedConnection);
    connect(documentController->treeModel(), &qmt::TreeModel::modelReset,
            this, &ModelEditor::onTreeModelReset, Qt::QueuedConnection);
    connect(documentController->diagramController(), &qmt::DiagramController::modified,
            this, &ModelEditor::onDiagramModified, Qt::QueuedConnection);
    connect(documentController->diagramController(), &qmt::DiagramController::endInsertElement,
            this, &ModelEditor::onComponentAdd, Qt::QueuedConnection);
    connect(documentController->diagramController(), &qmt::DiagramController::beginRemoveElement,
            this, &ModelEditor::onComponentRemove, Qt::DirectConnection);
    connect(documentController->diagramsManager(), &qmt::DiagramsManager::diagramSelectionChanged,
            this, &ModelEditor::onDiagramSelectionChanged, Qt::QueuedConnection);
    connect(documentController->diagramsManager(), &qmt::DiagramsManager::diagramActivated,
            this, &ModelEditor::onDiagramActivated, Qt::QueuedConnection);
    connect(documentController->diagramSceneController(), &qmt::DiagramSceneController::newElementCreated,
            this, &ModelEditor::onNewElementCreated, Qt::QueuedConnection);

    connect(Core::EditorManager::instance(), &Core::EditorManager::currentEditorChanged,
            this, &ModelEditor::onCurrentEditorChanged, Qt::QueuedConnection);

    connect(d->diagramView, &EditorDiagramView::zoomIn,
            this, &ModelEditor::zoomInAtPos);
    connect(d->diagramView, &EditorDiagramView::zoomOut,
            this, &ModelEditor::zoomOutAtPos);

    connect(d->modelTreeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &ModelEditor::onTreeViewSelectionChanged, Qt::QueuedConnection);
    connect(d->modelTreeView, &qmt::ModelTreeView::treeViewActivated,
            this, &ModelEditor::onTreeViewActivated, Qt::QueuedConnection);
    connect(d->modelTreeView, &QAbstractItemView::doubleClicked,
            this, &ModelEditor::onTreeViewDoubleClicked, Qt::QueuedConnection);

    connect(documentController->modelController(), &qmt::ModelController::endMoveObject,
            this, &ModelEditor::updateDiagramSelector);
    connect(documentController->modelController(), &qmt::ModelController::endRemoveObject,
            this, &ModelEditor::updateDiagramSelector);
    connect(documentController->modelController(), &qmt::ModelController::endResetModel,
            this, &ModelEditor::updateDiagramSelector);
    connect(documentController->modelController(), &qmt::ModelController::endUpdateObject,
            this, &ModelEditor::updateDiagramSelector);

    updateSelectedArea(SelectedArea::Nothing);
}

qmt::MDiagram *ModelEditor::currentDiagram() const
{
    if (!d->diagramView->diagramSceneModel())
        return nullptr;
    return d->diagramView->diagramSceneModel()->diagram();
}

void ModelEditor::showDiagram(qmt::MDiagram *diagram)
{
    openDiagram(diagram, true);
}

void ModelEditor::undo()
{
    d->document->documentController()->undoController()->undoStack()->undo();
}

void ModelEditor::redo()
{
    d->document->documentController()->undoController()->undoStack()->redo();
}

void ModelEditor::cut()
{
    return;//暂不支持
    ExtDocumentController *documentController = d->document->documentController();
    ModelsManager *modelsManager = ModelEditorPlugin::modelsManager();

    switch (d->selectedArea) {
    case SelectedArea::Nothing:
        break;
    case SelectedArea::Diagram:
        setDiagramClipboard(documentController->cutFromDiagram(currentDiagram()));
        break;
    case SelectedArea::TreeView:
        modelsManager->setModelClipboard(documentController, documentController->cutFromModel(d->modelTreeViewServant->selectedObjects()));
        break;
    }
}

void ModelEditor::copy()
{
    return;//暂不支持
    ExtDocumentController *documentController = d->document->documentController();
    ModelsManager *modelsManager = ModelEditorPlugin::modelsManager();

    switch (d->selectedArea) {
    case SelectedArea::Nothing:
        break;
    case SelectedArea::Diagram:
        if (documentController->hasDiagramSelection(currentDiagram()))
            setDiagramClipboard(documentController->copyFromDiagram(currentDiagram()));
        else
            documentController->copyDiagram(currentDiagram());
        break;
    case SelectedArea::TreeView:
        modelsManager->setModelClipboard(documentController, documentController->copyFromModel(d->modelTreeViewServant->selectedObjects()));
        break;
    }
}

void ModelEditor::paste()
{
    return;//暂不支持
    ExtDocumentController *documentController = d->document->documentController();
    ModelsManager *modelsManager = ModelEditorPlugin::modelsManager();

    switch (d->selectedArea) {
    case SelectedArea::Nothing:
        break;
    case SelectedArea::Diagram:
        // on cut/copy diagram and model elements were copied.
        documentController->pasteIntoModel(currentDiagram(), modelsManager->modelClipboard(), qmt::ModelController::PasteOnlyNewElements);
        documentController->pasteIntoDiagram(currentDiagram(), modelsManager->diagramClipboard());
        break;
    case SelectedArea::TreeView:
        documentController->pasteIntoModel(d->modelTreeViewServant->selectedObject(), modelsManager->modelClipboard(),
                                           documentController == modelsManager->modelClipboardDocumentController()
                                           ? qmt::ModelController::PasteAlwaysWithNewKeys : qmt::ModelController::PasteAlwaysAndKeepKeys);
        break;
    }
}

void ModelEditor::removeSelectedElements()
{
    switch (d->selectedArea) {
    case SelectedArea::Nothing:
        break;
    case SelectedArea::Diagram:
        d->document->documentController()->removeFromDiagram(currentDiagram());
        break;
    case SelectedArea::TreeView:
        break;
    }
}

void ModelEditor::deleteSelectedElements()
{
    ExtDocumentController *documentController = d->document->documentController();

    switch (d->selectedArea) {
    case SelectedArea::Nothing:
        break;
    case SelectedArea::Diagram:
        documentController->deleteFromDiagram(currentDiagram());
        break;
    case SelectedArea::TreeView:
        documentController->deleteFromModel(d->modelTreeViewServant->selectedObjects());
        break;
    }
}

void ModelEditor::selectAll()
{
    d->document->documentController()->selectAllOnDiagram(currentDiagram());
}

void ModelEditor::openParentDiagram()
{
    qmt::MDiagram *diagram = currentDiagram();
    if (diagram)
        d->document->documentController()->elementTasks()->openParentDiagram(diagram);
}

void ModelEditor::editProperties()
{
    d->propertiesView->editSelectedElement();
}

void ModelEditor::editSelectedItem()
{
    onEditSelectedElement();
}

void ModelEditor::exportDiagram()
{
    exportToImage(/*selectedElements=*/false);
}

void ModelEditor::exportSelectedElements()
{
    exportToImage(/*selectedElements=*/true);
}

void ModelEditor::exportToImage(bool selectedElements)
{
    qmt::MDiagram *diagram = currentDiagram();
    if (diagram) {
        if (d->lastExportDirPath.isEmpty())
            d->lastExportDirPath = d->document->filePath().toFileInfo().canonicalPath();
        QString filter = tr("Images (*.png *.jpeg *.jpg *.tif *.tiff);;PDF (*.pdf)");
#ifndef QT_NO_SVG
        filter += tr(";;SVG (*.svg)");
#endif // QT_NO_SVG
        QString fileName = QFileDialog::getSaveFileName(
                    Core::ICore::dialogParent(),
                    selectedElements ? tr("Export Selected Elements") : tr("Export Diagram"),
                    d->lastExportDirPath, filter);
        if (!fileName.isEmpty()) {
            qmt::DocumentController *documentController = d->document->documentController();
            qmt::DiagramSceneModel *sceneModel = documentController->diagramsManager()->diagramSceneModel(diagram);
            bool success = false;
            QString suffix = QFileInfo(fileName).suffix().toLower();
            // TODO use QFileDialog::selectedNameFilter() as fallback if no suffix is given
            if (suffix.isEmpty()) {
                suffix = "png";
                fileName += ".png";
            }
            if (suffix == "pdf")
                success = sceneModel->exportPdf(fileName, selectedElements);
#ifndef QT_NO_SVG
            else if (suffix == "svg")
                success = sceneModel->exportSvg(fileName, selectedElements);
#endif // QT_NO_SVG
            else
                success = sceneModel->exportImage(fileName, selectedElements);
            if (success)
                d->lastExportDirPath = QFileInfo(fileName).canonicalPath();
            else if (selectedElements)
                QMessageBox::critical(Core::ICore::dialogParent(), tr("Exporting Selected Elements Failed"),
                                      tr("Exporting the selected elements of the current diagram into file<br>\"%1\"<br>failed.").arg(fileName));
            else
                QMessageBox::critical(Core::ICore::dialogParent(), tr("Exporting Diagram Failed"),
                                      tr("Exporting the diagram into file<br>\"%1\"<br>failed.").arg(fileName));
        }
    }
}

void ModelEditor::zoomIn()
{
    QTransform transform = d->diagramView->transform();
    transform.scale(ZOOM_FACTOR, ZOOM_FACTOR);
    d->diagramView->setTransform(transform);
    showZoomIndicator();
}

void ModelEditor::zoomOut()
{
    QTransform transform = d->diagramView->transform();
    transform.scale(1.0 / ZOOM_FACTOR, 1.0 / ZOOM_FACTOR);
    d->diagramView->setTransform(transform);
    showZoomIndicator();
}

void ModelEditor::zoomInAtPos(const QPoint &pos)
{
    zoomAtPos(pos, ZOOM_FACTOR);
}

void ModelEditor::zoomOutAtPos(const QPoint &pos)
{
    zoomAtPos(pos, 1.0 / ZOOM_FACTOR);
}

void ModelEditor::zoomAtPos(const QPoint &pos, double scale)
{
    QPointF scenePos = d->diagramView->mapToScene(pos);
    QTransform transform = d->diagramView->transform();
    transform.scale(scale, scale);
    d->diagramView->setTransform(transform);
    QPoint scaledPos = d->diagramView->mapFromScene(scenePos);
    QPoint delta = pos - scaledPos;
    QScrollBar *hBar = d->diagramView->horizontalScrollBar();
    if (hBar)
        hBar->setValue(hBar->value() - delta.x());
    QScrollBar *vBar = d->diagramView->verticalScrollBar();
    if (vBar)
        vBar->setValue(vBar->value() - delta.y());
    showZoomIndicator();
}

void ModelEditor::resetZoom()
{
    d->diagramView->setTransform(QTransform());
    showZoomIndicator();
}

void ModelEditor::pan(bool checked)
{
    if(checked)
        d->diagramView->setDragMode(QGraphicsView::ScrollHandDrag);
    else
        d->diagramView->setDragMode(QGraphicsView::RubberBandDrag);
}

void ModelEditor::viewFilter()
{
    int flag = 0;
    if(d->actionHandler->viewMethodAction()->isChecked())
        flag |= 1;
    if(d->actionHandler->viewEventAction()->isChecked())
        flag |= 2;
    if(d->actionHandler->viewFieldAction()->isChecked())
        flag |= 4;
    if(d->actionHandler->viewPinAction()->isChecked())
        flag |= 8;
    if(d->actionHandler->viewComponentAction()->isChecked())
        flag |= 16;
    d->diagramView->showItem(flag);
}

void ModelEditor::beautifulConnect()
{
    d->diagramView->beautifulConnect();
}

void ModelEditor::autoConnect()
{
    d->diagramView->autoConnect();
}

void ModelEditor::disconnect()
{
    d->diagramView->disconnect();
}

qmt::MPackage *ModelEditor::guessSelectedPackage() const
{
    qmt::MPackage *package = nullptr;
    switch (d->selectedArea) {
    case SelectedArea::Nothing:
        package = d->modelTreeViewServant->selectedPackage();
        break;
    case SelectedArea::Diagram:
    {
        qmt::DocumentController *documentController = d->document->documentController();
        qmt::DiagramsManager *diagramsManager = documentController->diagramsManager();
        qmt::MDiagram *diagram = currentDiagram();
        qmt::DSelection selection = diagramsManager->diagramSceneModel(diagram)->selectedElements();
        if (selection.indices().size() == 1) {
            qmt::DPackage *diagramElement = documentController->diagramController()->findElement<qmt::DPackage>(selection.indices().at(0).elementKey(), diagram);
            if (diagramElement)
                package = documentController->modelController()->findObject<qmt::MPackage>(diagramElement->modelUid());
        }
        break;
    }
    case SelectedArea::TreeView:
        package = d->modelTreeViewServant->selectedPackage();
        break;
    }
    return package;
}

void ModelEditor::updateSelectedArea(SelectedArea selectedArea)
{
    d->selectedArea = selectedArea;

    qmt::DocumentController *documentController = d->document->documentController();
    ModelsManager *modelsManager = ModelEditorPlugin::modelsManager();
    bool canCutCopyDelete = false;
    bool canRemove = false;
    bool canPaste = false;
    bool canSelectAll = false;
    bool canCopyDiagram = false;
    bool canOpenParentDiagram = false;
    bool canExportDiagram = false;
    bool canExportSelectedElements = false;
    QList<qmt::MElement *> propertiesModelElements;
    QList<qmt::DElement *> propertiesDiagramElements;
    qmt::MDiagram *propertiesDiagram = nullptr;

    qmt::MDiagram *activeDiagram = currentDiagram();
    switch (d->selectedArea) {
    case SelectedArea::Nothing:
        canSelectAll = activeDiagram && !activeDiagram->diagramElements().isEmpty();
        canExportDiagram = activeDiagram != nullptr;
        break;
    case SelectedArea::Diagram:
    {
        if (activeDiagram) {
            bool hasSelection = documentController->diagramsManager()->diagramSceneModel(activeDiagram)->hasSelection();
            canCutCopyDelete = hasSelection;
            canRemove = hasSelection;
            canPaste = !modelsManager->isDiagramClipboardEmpty();
            canSelectAll = !activeDiagram->diagramElements().isEmpty();
            canCopyDiagram = !hasSelection;
            canExportDiagram = true;
            canExportSelectedElements = hasSelection;
            if (hasSelection) {
                qmt::DSelection selection = documentController->diagramsManager()->diagramSceneModel(activeDiagram)->selectedElements();
                if (!selection.isEmpty()) {
                    foreach (qmt::DSelection::Index index, selection.indices()) {
                        qmt::DElement *diagramElement = documentController->diagramController()->findElement(index.elementKey(), activeDiagram);
                        if (diagramElement)
                            propertiesDiagramElements.append(diagramElement);
                    }
                    if (!propertiesDiagramElements.isEmpty())
                        propertiesDiagram = activeDiagram;
                }
            }
        }
        break;
    }
    case SelectedArea::TreeView:
    {
        bool hasSelection = !d->modelTreeViewServant->selectedObjects().isEmpty();
        bool hasSingleSelection = d->modelTreeViewServant->selectedObjects().indices().size() == 1;
        canCutCopyDelete = hasSelection && !d->modelTreeViewServant->isRootPackageSelected();
        canPaste =  hasSingleSelection && !modelsManager->isModelClipboardEmpty();
        canSelectAll = activeDiagram && !activeDiagram->diagramElements().isEmpty();
        canExportDiagram = activeDiagram != nullptr;
        QModelIndexList indexes = d->modelTreeView->selectedSourceModelIndexes();
        if (!indexes.isEmpty()) {
            foreach (const QModelIndex &propertiesIndex, indexes) {
                if (propertiesIndex.isValid()) {
                    qmt::MElement *modelElement = documentController->treeModel()->element(propertiesIndex);
                    if (modelElement)
                        propertiesModelElements.append(modelElement);
                }
            }
        }
        break;
    }
    }
    canOpenParentDiagram = d->document->documentController()->elementTasks()->hasParentDiagram(currentDiagram());

    d->actionHandler->cutAction()->setEnabled(canCutCopyDelete);
    d->actionHandler->copyAction()->setEnabled(canCutCopyDelete || canCopyDiagram);
    d->actionHandler->pasteAction()->setEnabled(canPaste);
    d->actionHandler->removeAction()->setEnabled(canRemove);
    d->actionHandler->deleteAction()->setEnabled(canCutCopyDelete);
    d->actionHandler->selectAllAction()->setEnabled(canSelectAll);
    d->actionHandler->openParentDiagramAction()->setEnabled(canOpenParentDiagram);
    d->actionHandler->exportDiagramAction()->setEnabled(canExportDiagram);
    d->actionHandler->exportSelectedElementsAction()->setEnabled(canExportSelectedElements);

    if (!propertiesModelElements.isEmpty())
        showProperties(propertiesModelElements);
    else if (!propertiesDiagramElements.isEmpty())
        showProperties(propertiesDiagram, propertiesDiagramElements);
    else
        clearProperties();
}

void ModelEditor::showProperties(const QList<qmt::MElement *> &modelElements)
{
    if (modelElements != d->propertiesView->selectedModelElements()) {
        clearProperties();
        if (modelElements.size() > 0) {
            d->propertiesView->setSelectedModelElements(modelElements);
            d->propertiesGroupWidget = d->propertiesView->widget();
            d->propertiesScrollArea->setWidget(d->propertiesGroupWidget);
        }
    }
}

void ModelEditor::showProperties(qmt::MDiagram *diagram,
                                 const QList<qmt::DElement *> &diagramElements)
{
    if (diagram != d->propertiesView->selectedDiagram()
            || diagramElements != d->propertiesView->selectedDiagramElements())
    {
        clearProperties();
        if (diagram && diagramElements.size() > 0) {
            d->propertiesView->setSelectedDiagramElements(diagramElements, diagram);
            d->propertiesGroupWidget = d->propertiesView->widget();
            d->propertiesScrollArea->setWidget(d->propertiesGroupWidget);
        }
    }
}

void ModelEditor::clearProperties()
{
    d->propertiesView->clearSelection();
    if (d->propertiesGroupWidget) {
        QWidget *scrollWidget = d->propertiesScrollArea->takeWidget();
        Q_UNUSED(scrollWidget) // avoid warning in release mode
        QMT_CHECK(scrollWidget == d->propertiesGroupWidget);
        d->propertiesGroupWidget->deleteLater();
        d->propertiesGroupWidget = nullptr;
    }
}

void ModelEditor::expandModelTreeToDepth(int depth)
{
    d->modelTreeView->expandToDepth(depth);
}

QToolButton *ModelEditor::createToolbarCommandButton(const Utils::Id &id,
                                                     const std::function<void()> &slot,
                                                     QWidget *parent)
{
    Core::Command *command = Core::ActionManager::command(id);
    QTC_CHECK(command);
    const QString text = command ? command->description() : QString();
    auto action = new QAction(text, this);
    action->setIcon(command ? command->action()->icon() : QIcon());
    auto button = Core::Command::toolButtonWithAppendedShortcut(action, command);
    button->setParent(parent);
    connect(button, &QToolButton::clicked, this, slot);
    return button;
}

/*!
    Tries to change the \a button icon to the icon specified by \a name
    from the current theme. Returns \c true if icon is updated, \c false
    otherwise.
*/

bool ModelEditor::updateButtonIconByTheme(QAbstractButton *button, const QString &name)
{
    QMT_ASSERT(button, return false);
    QMT_ASSERT(!name.isEmpty(), return false);

    if (QIcon::hasThemeIcon(name)) {
        button->setIcon(QIcon::fromTheme(name));
        return true;
    }

    return false;
}

void ModelEditor::showZoomIndicator()
{
    int scale = int(d->diagramView->transform().map(QPointF(100, 100)).x() + 0.5);
    Utils::FadingIndicator::showText(d->diagramStack,
                                     QCoreApplication::translate("ModelEditor",
                                                                 "Zoom: %1%").arg(scale),
                                     Utils::FadingIndicator::SmallText);
}

void ModelEditor::onAddPackage()
{
    ExtDocumentController *documentController = d->document->documentController();

    qmt::MPackage *package = documentController->createNewPackage(d->modelTreeViewServant->selectedPackage());
    d->modelTreeView->selectFromSourceModelIndex(documentController->treeModel()->indexOf(package));
    QTimer::singleShot(0, this, [this]() { onEditSelectedElement(); });
}

void ModelEditor::onAddComponent()
{
    ExtDocumentController *documentController = d->document->documentController();

    qmt::MComponent *component = documentController->createNewComponent(d->modelTreeViewServant->selectedPackage());
    d->modelTreeView->selectFromSourceModelIndex(documentController->treeModel()->indexOf(component));
    QTimer::singleShot(0, this, [this]() { onEditSelectedElement(); });
}

void ModelEditor::onAddClass()
{
    ExtDocumentController *documentController = d->document->documentController();

    qmt::MClass *klass = documentController->createNewClass(d->modelTreeViewServant->selectedPackage());
    d->modelTreeView->selectFromSourceModelIndex(documentController->treeModel()->indexOf(klass));
    QTimer::singleShot(0, this, [this]() { onEditSelectedElement(); });
}

void ModelEditor::onAddCanvasDiagram()
{
    ExtDocumentController *documentController = d->document->documentController();

    qmt::MDiagram *diagram = documentController->createNewCanvasDiagram(d->modelTreeViewServant->selectedPackage());
    d->modelTreeView->selectFromSourceModelIndex(documentController->treeModel()->indexOf(diagram));
    QTimer::singleShot(0, this, [this]() { onEditSelectedElement(); });
}

void ModelEditor::onCurrentEditorChanged(Core::IEditor *editor)
{
    if (this == editor) {
        EmosTools::SrvParser::getInstance()->reloadXml();
        EmosTools::ApiParser::getInstance()->reloadXml();
        EmosTools::TypeParser::getInstance()->reloadXml();
        initToolbars();

        QUndoStack *undoStack = d->document->documentController()->undoController()->undoStack();
        d->actionHandler->undoAction()->setDisabled(!undoStack->canUndo());
        d->actionHandler->redoAction()->setDisabled(!undoStack->canRedo());

        updateSelectedArea(SelectedArea::Nothing);
    }
}

void ModelEditor::onCanUndoChanged(bool canUndo)
{
    if (this == Core::EditorManager::currentEditor())
        d->actionHandler->undoAction()->setEnabled(canUndo);
}

void ModelEditor::onCanRedoChanged(bool canRedo)
{
    if (this == Core::EditorManager::currentEditor())
        d->actionHandler->redoAction()->setEnabled(canRedo);
}

void ModelEditor::onTreeModelReset()
{
    updateSelectedArea(SelectedArea::Nothing);
}

void ModelEditor::onTreeViewSelectionChanged(const QItemSelection &selected,
                                             const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    updateSelectedArea(SelectedArea::TreeView);
}

void ModelEditor::onTreeViewActivated()
{
    updateSelectedArea(SelectedArea::TreeView);
}

void ModelEditor::onTreeViewDoubleClicked(const QModelIndex &index)
{
    ExtDocumentController *documentController = d->document->documentController();

    QModelIndex treeModelIndex = d->modelTreeView->mapToSourceModelIndex(index);
    qmt::MElement *melement = documentController->treeModel()->element(treeModelIndex);
    // double click on package is already used for toggeling tree
    if (melement && !dynamic_cast<qmt::MPackage *>(melement))
        documentController->elementTasks()->openElement(melement);
}

void ModelEditor::onCurrentDiagramChanged(const qmt::MDiagram *diagram)
{
    if (diagram == currentDiagram()) {
        updateSelectedArea(SelectedArea::Diagram);
    } else {
        updateSelectedArea(SelectedArea::Nothing);
    }
}

void ModelEditor::onDiagramActivated(const qmt::MDiagram *diagram)
{
    Q_UNUSED(diagram)

    updateSelectedArea(SelectedArea::Diagram);
}

void ModelEditor::onDiagramClipboardChanged(bool isEmpty)
{
    Q_UNUSED(isEmpty)

    if (this == Core::EditorManager::currentEditor())
        updateSelectedArea(d->selectedArea);
}

void ModelEditor::onNewElementCreated(qmt::DElement *element, qmt::MDiagram *diagram)
{
    if (diagram == currentDiagram()) {
        ExtDocumentController *documentController = d->document->documentController();

        documentController->diagramsManager()->diagramSceneModel(diagram)->selectElement(element);
        qmt::MElement *melement = documentController->modelController()->findElement(element->modelUid());
        if (!(melement && melement->flags().testFlag(qmt::MElement::ReverseEngineered)))
            QTimer::singleShot(0, this, [this]() { onEditSelectedElement(); });
    }
}

void ModelEditor::onDiagramSelectionChanged(const qmt::MDiagram *diagram)
{
    if (diagram == currentDiagram()) {
        updateSelectedArea(SelectedArea::Diagram);
    }
}

void ModelEditor::onDiagramModified(const qmt::MDiagram *diagram)
{
    Q_UNUSED(diagram)

    updateSelectedArea(d->selectedArea);
}

void ModelEditor::onLeftGroupSplitterMoved(int pos, int index)
{
    Q_UNUSED(pos)
    Q_UNUSED(index)

    d->uiController->onLeftGroupSplitterChanged(d->leftGroup->saveState());
}

void ModelEditor::onLeftGroupSplitterChanged(const QByteArray &state)
{
    d->leftGroup->restoreState(state);
}

void ModelEditor::onRightSplitterMoved(int pos, int index)
{
    Q_UNUSED(pos)
    Q_UNUSED(index)

    d->uiController->onRightSplitterChanged(d->rightSplitter->saveState());
}

void ModelEditor::onRightSplitterChanged(const QByteArray &state)
{
    d->rightSplitter->restoreState(state);
}

void ModelEditor::onRightHorizSplitterMoved(int pos, int index)
{
    Q_UNUSED(pos)
    Q_UNUSED(index)

    d->uiController->onRightHorizSplitterChanged(d->rightHorizSplitter->saveState());
}

void ModelEditor::onRightHorizSplitterChanged(const QByteArray &state)
{
    d->rightHorizSplitter->restoreState(state);
}

void ModelEditor::onComponentAdd(int row, const qmt::MDiagram *diagram)
{
    const QList<qmt::DElement *> diagramElements = diagram->diagramElements();
    if(row >= diagramElements.size())
        return;

    for(auto i : d->toolList)
    {
        if(diagramElements[row]->oid() == i->getDropValues()[3].toString())
            i->setColor(QColor(255, 140, 0 , 30));
    }
}

void ModelEditor::onComponentRemove(int row, const qmt::MDiagram *diagram)
{
    const QList<qmt::DElement *> diagramElements = diagram->diagramElements();
    if(row >= diagramElements.size())
        return;

    for(auto i : d->toolList)
    {
        if(diagramElements[row]->oid() == i->getDropValues()[3].toString())
            i->setColor(QColor());
    }
}

void ModelEditor::initToolbars()
{
    d->leftToolBox->clearGroup();

    //haojie.fang General
    addGeneralToolbars();

    //从project列表加载,选择component作为基础修改
    addIdlToolbars();

    //更新comp列表对应的item
    QStringList oidList;
    QStringList pushedOidList;

    const QList<qmt::DElement *> diagramElements = currentDiagram()->diagramElements();
    for(auto i : currentDiagram()->diagramElements())
    {
        pushedOidList << i->oid();
    }

    for(auto i : d->toolList)
    {
        oidList << i->getDropValues()[3].toString();
        if(pushedOidList.contains(oidList.last()))
            i->setColor(QColor(255, 140, 0 , 30));
        else
            i->setColor(QColor());
    }
    d->diagramView->updateAll(oidList);
}

void ModelEditor::addGeneralToolbars()
{
    QString generalId = "General";
    auto layout = new QVBoxLayout();
    layout->setContentsMargins(1,1,1,1);
    layout->setSpacing(2);
    auto toolBar = new EmosWidgets::FlowGroupWidget(generalId, EmosWidgets::FlowGroupWidget::dropWidgetFunction(), layout);
    toolBar->setProperty(PROPERTYNAME_TOOLBARID, generalId);
    toolBar->setStyleSheet(
                QLatin1String("QLabel {"
                              "    font: bold;"
                              "    color: black;"
                              "}"));
    d->leftToolBox->appendGroup(toolBar);

    auto dragTool = new EmosWidgets::DragTool(tr("Text"), toolBar);
    dragTool->setIcon(QIcon(":/modelinglib/48x48/annotation.png"));
    dragTool->addDropValue(QLatin1String(qmt::ELEMENT_TYPE_ANNOTATION));
    dragTool->addDropValue(QString());
    dragTool->addDropValue(QString());
    dragTool->addDropValue(QString());
    toolBar->appendItem(dragTool);

    dragTool = new EmosWidgets::DragTool(tr("Device"), toolBar);
    dragTool->setIcon(QIcon(":/modelinglib/48x48/device.png"));
    dragTool->addDropValue(QLatin1String(qmt::ELEMENT_TYPE_DEVICE));
    dragTool->addDropValue(QString());
    dragTool->addDropValue(QString());
    dragTool->addDropValue(QString());
    toolBar->appendItem(dragTool);

    dragTool = new EmosWidgets::DragTool(tr("Process"), toolBar);
    dragTool->setIcon(QIcon(":/modelinglib/48x48/boundary.png"));
    dragTool->addDropValue(QLatin1String(qmt::ELEMENT_TYPE_BOUNDARY));
    dragTool->addDropValue(QString());
    dragTool->addDropValue(QString());
    dragTool->addDropValue(QString());
    toolBar->appendItem(dragTool);

    dragTool = new EmosWidgets::DragTool(tr("Line"), toolBar);
    dragTool->setIcon(QIcon(":/modelinglib/48x48/swimlane.png"));
    dragTool->addDropValue(QLatin1String(qmt::ELEMENT_TYPE_SWIMLANE));
    dragTool->addDropValue(QString());
    dragTool->addDropValue(QString());
    dragTool->addDropValue(QString());
    toolBar->appendItem(dragTool);

//    toolBar->appendItem(
//                new DragTool(QIcon(":/modelinglib/48x48/studio.png"),
//                             tr("Studio"), QString("Studio"), QLatin1String(qmt::ELEMENT_TYPE_COMPONENT),
//                             QString(), QString("Studio"), toolBar));
}

void ModelEditor::addIdlToolbars()
{
    Utils::FilePaths files = IdlParser::currentProIdlList();
    d->toolList.clear();

    for(const auto& file : files)
    {
        QString itemString = IdlParser::readIdlOid(file.toString());
        if(itemString == "" || itemString.split(".").size() < 2)
            continue;
        QString tag = itemString.split(".").at(0) + "." + itemString.split(".").at(1);

        bool find = false;
        for(QWidget* w : d->leftToolBox->groups())
        {
            EmosWidgets::FlowGroupWidget* flowGroup = qobject_cast<EmosWidgets::FlowGroupWidget*>(w);
            if(flowGroup->title() == tag)//child
            {
                auto dragTool = new EmosWidgets::DragTool(itemString.split(".").at(2), flowGroup);
                dragTool->setIcon(QIcon(":/modelinglib/48x48/component.png"));
                dragTool->addDropValue(QLatin1String(qmt::ELEMENT_TYPE_COMPONENT));
                dragTool->addDropValue(itemString.split(".").at(2));
                dragTool->addDropValue(QString());
                dragTool->addDropValue(itemString);
                flowGroup->appendItem(dragTool);
                d->toolList << dragTool;

                find = true;
                break;
            }
        }
        if(!find)
        {
            auto layout = new QVBoxLayout();
            layout->setContentsMargins(1,1,1,1);
            layout->setSpacing(2);
            auto toolBar = new EmosWidgets::FlowGroupWidget(tag, EmosWidgets::FlowGroupWidget::dropWidgetFunction(), layout);
            toolBar->setProperty(PROPERTYNAME_TOOLBARID, tag);
            toolBar->setStyleSheet(
                        QLatin1String("QLabel {"
                                      "    font: bold;"
                                      "    color: black;"
                                      "}"));
            d->leftToolBox->appendGroup(toolBar);

            auto dragTool = new EmosWidgets::DragTool(itemString.split(".").at(2), toolBar);
            dragTool->setIcon(QIcon(":/modelinglib/48x48/component.png"));
            dragTool->addDropValue(QLatin1String(qmt::ELEMENT_TYPE_COMPONENT));
            dragTool->addDropValue(itemString.split(".").at(2));
            dragTool->addDropValue(QString());
            dragTool->addDropValue(itemString);
            toolBar->appendItem(dragTool);
            d->toolList << dragTool;
        }
    }
}

void ModelEditor::openDiagram(qmt::MDiagram *diagram, bool addToHistory)
{
    closeCurrentDiagram(addToHistory);
    if (diagram) {
        qmt::DiagramSceneModel *diagramSceneModel = d->document->documentController()->diagramsManager()->bindDiagramSceneModel(diagram);
        d->diagramView->setDiagramSceneModel(diagramSceneModel);
        d->diagramStack->setCurrentWidget(d->diagramView);
        updateSelectedArea(SelectedArea::Nothing);
        addDiagramToSelector(diagram);
    }
}

void ModelEditor::closeCurrentDiagram(bool addToHistory)
{
    ExtDocumentController *documentController = d->document->documentController();
    qmt::DiagramsManager *diagramsManager = documentController->diagramsManager();
    qmt::DiagramSceneModel *sceneModel = d->diagramView->diagramSceneModel();
    if (sceneModel) {
        qmt::MDiagram *diagram = sceneModel->diagram();
        if (diagram) {
            if (addToHistory)
                addToNavigationHistory(diagram);
            d->diagramStack->setCurrentWidget(d->noDiagramLabel);
            d->diagramView->setDiagramSceneModel(nullptr);
            diagramsManager->unbindDiagramSceneModel(diagram);
        }
    }
}

void ModelEditor::closeDiagram(const qmt::MDiagram *diagram)
{
    ExtDocumentController *documentController = d->document->documentController();
    qmt::DiagramsManager *diagramsManager = documentController->diagramsManager();
    qmt::DiagramSceneModel *sceneModel = d->diagramView->diagramSceneModel();
    if (sceneModel && diagram == sceneModel->diagram()) {
        addToNavigationHistory(diagram);
        d->diagramStack->setCurrentWidget(d->noDiagramLabel);
        d->diagramView->setDiagramSceneModel(nullptr);
        diagramsManager->unbindDiagramSceneModel(diagram);
    }
}

void ModelEditor::closeAllDiagrams()
{
    closeCurrentDiagram(true);
}

void ModelEditor::onContentSet()
{
    initDocument();

    // open diagram
    ExtDocumentController *documentController = d->document->documentController();
    qmt::MDiagram *rootDiagram = documentController->findOrCreateRootDiagram();
    showDiagram(rootDiagram);
    // select diagram in model tree view
    QModelIndex modelIndex = documentController->treeModel()->indexOf(rootDiagram);
    if (modelIndex.isValid())
        d->modelTreeView->selectFromSourceModelIndex(modelIndex);

    expandModelTreeToDepth(0);
}

void ModelEditor::setDiagramClipboard(const qmt::DContainer &dcontainer)
{
    ExtDocumentController *documentController = d->document->documentController();
    qmt::ModelController *modelController = documentController->modelController();
    qmt::MSelection modelSelection;
    for (const auto &delement : dcontainer.elements()) {
        qmt::Uid melementUid = delement->modelUid();
        qmt::Uid mownerUid = modelController->ownerKey(melementUid);
        modelSelection.append(melementUid, mownerUid);
    }
    qmt::MContainer mcontainer = modelController->copyElements(modelSelection);
    ModelEditorPlugin::modelsManager()->setDiagramClipboard(documentController, dcontainer, mcontainer);
}

void ModelEditor::addDiagramToSelector(const qmt::MDiagram *diagram)
{
    QString diagramLabel = buildDiagramLabel(diagram);
    QVariant diagramUid = QVariant::fromValue(diagram->uid());
    int i = d->diagramSelector->findData(diagramUid);
    if (i >= 0)
        d->diagramSelector->removeItem(i);
    d->diagramSelector->insertItem(0, QIcon(":/modelinglib/48x48/canvas-diagram.png"), diagramLabel, diagramUid);
    d->diagramSelector->setCurrentIndex(0);
    while (d->diagramSelector->count() > 20)
        d->diagramSelector->removeItem(d->diagramSelector->count() - 1);
}

void ModelEditor::updateDiagramSelector()
{
    int i = 0;
    while (i < d->diagramSelector->count()) {
        qmt::Uid diagramUid = d->diagramSelector->itemData(i).value<qmt::Uid>();
        if (diagramUid.isValid()) {
            qmt::MDiagram *diagram = d->document->documentController()->modelController()->findObject<qmt::MDiagram>(diagramUid);
            if (diagram) {
                QString diagramLabel = buildDiagramLabel(diagram);
                if (diagramLabel != d->diagramSelector->itemText(i))
                    d->diagramSelector->setItemText(i, diagramLabel);
                ++i;
                continue;
            }
        }
        d->diagramSelector->removeItem(i);
    }
}

void ModelEditor::onDiagramSelectorSelected(int index)
{
    qmt::Uid diagramUid = d->diagramSelector->itemData(index).value<qmt::Uid>();
    if (diagramUid.isValid()) {
        qmt::MDiagram *diagram = d->document->documentController()->modelController()->findObject<qmt::MDiagram>(diagramUid);
        if (diagram) {
            showDiagram(diagram);
            return;
        }
    }
    d->diagramSelector->setCurrentIndex(0);
}

QString ModelEditor::buildDiagramLabel(const qmt::MDiagram *diagram)
{
    QString label = diagram->name();
    qmt::MObject *owner = diagram->owner();
    QStringList path;
    while (owner) {
        path.append(owner->name());
        owner = owner->owner();
    }
    if (!path.isEmpty()) {
        label += " [";
        label += path.last();
        for (int i = path.count() - 2; i >= 0; --i) {
            label += QLatin1Char('.');
            label += path.at(i);
        }
        label += QLatin1Char(']');
    }
    return label;
}

void ModelEditor::addToNavigationHistory(const qmt::MDiagram *diagram)
{
    if (Core::EditorManager::currentEditor() == this) {
        Core::EditorManager::cutForwardNavigationHistory();
        Core::EditorManager::addCurrentPositionToNavigationHistory(saveState(diagram));
    }
}

QByteArray ModelEditor::saveState(const qmt::MDiagram *diagram) const
{
    QByteArray state;
    QDataStream stream(&state, QIODevice::WriteOnly);
    stream << 2; // version number
    if (diagram)
        stream << diagram->uid();
    else
        stream << qmt::Uid::invalidUid();
    return state;
}

void ModelEditor::onEditSelectedElement()
{
    // TODO introduce similar method for selected elements in model tree
    // currently this method is called on adding new elements in model tree
    // but the method is a no-op in that case.
    qmt::MDiagram *diagram = d->propertiesView->selectedDiagram();
    QList<qmt::DElement *> elements = d->propertiesView->selectedDiagramElements();
    if (diagram && !elements.isEmpty()) {
        qmt::DElement *element = elements.at(0);
        if (element) {
            qmt::DiagramSceneModel *diagramSceneModel = d->document->documentController()->diagramsManager()->diagramSceneModel(diagram);
            if (diagramSceneModel->isElementEditable(element)) {
                d->diagramStack->currentWidget()->setFocus();
                //diagramSceneModel->editElement(element); haojie.fang 不设焦点编辑
                return;
            }
        }
        d->propertiesView->editSelectedElement();
    }
}

} // namespace Internal
} // namespace ModelEditor
