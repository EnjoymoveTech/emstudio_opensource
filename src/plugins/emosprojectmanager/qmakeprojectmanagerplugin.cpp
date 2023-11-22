/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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

#include "qmakeprojectmanagerplugin.h"

#include "profileeditor.h"
#include "qmakenodes.h"
#include "qmakebuildconfiguration.h"
#include "qmakeprojectmanagerconstants.h"
#include "qmakeproject.h"
#include "profilehighlighter.h"
#include "projectmerge.h"
#include "profilereader.h"
#include "projectimport/excelprojectimport.h"

#include <coreplugin/icore.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/modemanager.h>

#include <explorer/buildmanager.h>
#include <explorer/projectnodes.h>
#include <explorer/projectmanager.h>
#include <explorer/projecttree.h>
#include <explorer/runcontrol.h>
#include <explorer/session.h>
#include <explorer/target.h>
#include <explorer/explorer.h>

#include <generaleditor/generaleditor.h>
#include <generaleditor/texteditoractionhandler.h>
#include <generaleditor/texteditorconstants.h>

#include <utils/hostosinfo.h>
#include <utils/parameteraction.h>

#ifdef WITH_TESTS
#    include <QTest>
#endif

using namespace Core;
using namespace ProjectExplorer;
using namespace TextEditor;

namespace QmakeProjectManager {
namespace Internal {

const int  P_ACTION_CODEBUILD       = 30;

class QmakeProjectManagerPluginPrivate : public QObject
{
public:
    ~QmakeProjectManagerPluginPrivate() override;

    void projectChanged();
    void activeTargetChanged();
    void updateActions();
    void updateContextActions();
    void buildStateChanged(Project *pro);
    void updateBuildFileAction();
    void disableBuildFileMenus();
    void enableBuildFileMenus(const Utils::FilePath &file);

    Core::Context projectContext;

    QmakeBuildConfigurationFactory buildConfigFactory;

    ProFileEditorFactory profileEditorFactory;

    QmakeProject *m_previousStartupProject = nullptr;
    Target *m_previousTarget = nullptr;
    ProjectMerge* m_projectMerge = nullptr;

    Utils::ParameterAction *m_buildSubProjectContextMenu = nullptr;

    QAction *m_buildFileContextMenu = nullptr;
    QAction *m_selectBuildFileContextMenu = nullptr;
    QAction *m_buildXmlContextMenu = nullptr;

    QAction *m_buildProjectContextMenu = nullptr;
    QAction *m_cleanProjectContextMenu = nullptr;
    Utils::ParameterAction *m_buildProjectAction = nullptr;
    Utils::ParameterAction *m_buildProjectActionDev = nullptr;
    Utils::ParameterAction *m_cleanProjectAction = nullptr;
    QAction *m_MergeProjectAaction = nullptr;
    QAction *m_ImportExcelProjectAaction = nullptr;
    QAction *m_codeBuildAction = nullptr;

    void mergeProject();
    void importExcelProject();
    void runQMake();
    void runQMakeContextMenu();

    void buildProjectContextMenuDev() { handleSubDirContextMenuDev(QmakeBuildSystem::PROJECTBUILD, false); }
    void buildProjectContextMenu() { handleSubDirContextMenu(QmakeBuildSystem::PROJECTBUILD, false); }
    void clearProjectContextMenu() { handleSubDirContextMenu(QmakeBuildSystem::CLEAN, false); }
    void buildSubDirContextMenu() { handleSubDirContextMenu(QmakeBuildSystem::BUILD, false); }
    void rebuildSubDirContextMenu() { handleSubDirContextMenu(QmakeBuildSystem::REBUILD, false); }
    void cleanSubDirContextMenu() { handleSubDirContextMenu(QmakeBuildSystem::CLEAN, false); }
    void buildFileContextMenu() { handleSubDirContextMenu(QmakeBuildSystem::BUILD, true); }
    void selectBuildFileContextMenu() { handleSubDirContextMenu(QmakeBuildSystem::SELECTBUILD, true); }
    void buildXmlContextMenu() { handleSubDirContextMenu(QmakeBuildSystem::BUILDXML, true); }

    void handleSubDirContextMenuDev(QmakeBuildSystem::Action action, bool isFileBuild);
    void handleSubDirContextMenu(QmakeBuildSystem::Action action, bool isFileBuild);
    void runQMakeImpl(Project *p, ProjectExplorer::Node *node);
};

QmakeProjectManagerPlugin::~QmakeProjectManagerPlugin()
{
    if(d->m_projectMerge) delete d->m_projectMerge;
    if(d) delete d;
}

bool QmakeProjectManagerPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)
    const Context projectContext(QmakeProjectManager::Constants::QMAKEPROJECT_ID);
    Context projectTreeContext(ProjectExplorer::Constants::C_PROJECT_TREE);

    d = new QmakeProjectManagerPluginPrivate;
    d->m_projectMerge = new ProjectMerge;
    new ProFileCacheManager(this);

    //create and register objects
    ProjectManager::registerProjectType<QmakeProject>(QmakeProjectManager::Constants::PROFILE_MIMETYPE);

    //menus
    ActionContainer *mbuild =
            ActionManager::actionContainer(ProjectExplorer::Constants::M_BUILDPROJECT);
    ActionContainer *mproject =
            ActionManager::actionContainer(ProjectExplorer::Constants::M_PROJECTCONTEXT);
    ActionContainer *msubproject =
            ActionManager::actionContainer(ProjectExplorer::Constants::M_SUBPROJECTCONTEXT);
    ActionContainer *mfile =
            ActionManager::actionContainer(ProjectExplorer::Constants::M_FILECONTEXT);

    //register actions
    Command *command = nullptr;

    //build sub dir
    d->m_buildSubProjectContextMenu = new Utils::ParameterAction(tr("Build"), tr("Build \"%1\""),
                                                              Utils::ParameterAction::AlwaysEnabled/*handled manually*/,
                                                              this);
    command = ActionManager::registerAction(d->m_buildSubProjectContextMenu, Constants::BUILDSUBDIRCONTEXTMENU, projectContext);
    command->setAttribute(Command::CA_Hide);
    command->setAttribute(Command::CA_UpdateText);
    command->setDescription(d->m_buildSubProjectContextMenu->text());
    msubproject->addAction(command, ProjectExplorer::Constants::G_PROJECT_BUILD);
    connect(d->m_buildSubProjectContextMenu, &QAction::triggered,
            d, &QmakeProjectManagerPluginPrivate::buildSubDirContextMenu);

    //build file
    d->m_buildFileContextMenu = new QAction(tr("Build"), this);
    command = ActionManager::registerAction(d->m_buildFileContextMenu, Constants::BUILDFILECONTEXTMENU, projectContext);
    command->setAttribute(Command::CA_Hide);
    mfile->addAction(command, ProjectExplorer::Constants::G_FILE_BUILD);
    connect(d->m_buildFileContextMenu, &QAction::triggered,
            d, &QmakeProjectManagerPluginPrivate::buildFileContextMenu);

    d->m_buildXmlContextMenu = new QAction(tr("Build Xml"), this);
    command = ActionManager::registerAction(d->m_buildXmlContextMenu, Constants::BUILDXMLCONTEXTMENU, projectContext);
    command->setAttribute(Command::CA_Hide);
    mfile->addAction(command, ProjectExplorer::Constants::G_FILE_BUILD);
    connect(d->m_buildXmlContextMenu, &QAction::triggered,
            d, &QmakeProjectManagerPluginPrivate::buildXmlContextMenu);

    d->m_selectBuildFileContextMenu = new QAction(tr("Select Build"), this);
    command = ActionManager::registerAction(d->m_selectBuildFileContextMenu, Constants::SELECTBUILDFILECONTEXTMENU, projectContext);
    command->setAttribute(Command::CA_Hide);
//    mfile->addAction(command, ProjectExplorer::Constants::G_FILE_BUILD);
//    connect(d->m_selectBuildFileContextMenu, &QAction::triggered,
//            d, &QmakeProjectManagerPluginPrivate::selectBuildFileContextMenu);

    mfile->addSeparator(ProjectExplorer::Constants::G_FILE_BUILD);

    //build project
    d->m_buildProjectAction = new Utils::ParameterAction(tr("Build Project"), tr("Build Project"),
                                                         Utils::ParameterAction::AlwaysEnabled, this);
    command = ActionManager::registerAction(d->m_buildProjectAction, Constants::BUILDSUBDIR, projectContext);
    command->setAttribute(Command::CA_Hide);
    command->setAttribute(Command::CA_UpdateText);
    command->setDescription(d->m_buildProjectAction->text());
    command->setDefaultKeySequence(QKeySequence(tr("Ctrl+B")));
    mbuild->addAction(command, ProjectExplorer::Constants::G_BUILD_BUILD);
    connect(d->m_buildProjectAction, &QAction::triggered,
            d, &QmakeProjectManagerPluginPrivate::buildProjectContextMenu);

    d->m_buildProjectActionDev = new Utils::ParameterAction(tr("Build Project Dev"), tr("Build Project Dev"),
                                                            Utils::ParameterAction::AlwaysEnabled, this);
    command = ActionManager::registerAction(d->m_buildProjectActionDev, Constants::BUILDSUBDIRDEV, projectContext);
    command->setAttribute(Command::CA_Hide);
    command->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+B")));
    mbuild->addAction(command, "None");
    connect(d->m_buildProjectActionDev, &QAction::triggered,
            d, &QmakeProjectManagerPluginPrivate::buildProjectContextMenuDev);
    d->m_buildProjectActionDev->setVisible(false);

    d->m_cleanProjectAction = new Utils::ParameterAction(tr("Clean Project"), tr("Clean Project"),
                                                         Utils::ParameterAction::AlwaysEnabled, this);
    command = ActionManager::registerAction(d->m_cleanProjectAction, Constants::CLEANSUBDIR, projectContext);
    command->setAttribute(Command::CA_Hide);
    command->setAttribute(Command::CA_UpdateText);
    command->setDescription(d->m_cleanProjectAction->text());
    mbuild->addAction(command, ProjectExplorer::Constants::G_BUILD_BUILD);
    connect(d->m_cleanProjectAction, &QAction::triggered,
            d, &QmakeProjectManagerPluginPrivate::clearProjectContextMenu);

    d->m_buildProjectContextMenu = new QAction(tr("Build"), this);
    command = ActionManager::registerAction(d->m_buildProjectContextMenu, Constants::BUILDPROJECTCONTEXTMENU, projectContext);
    command->setAttribute(Command::CA_Hide);
    mproject->addAction(command, ProjectExplorer::Constants::G_PROJECT_BUILD);
    connect(d->m_buildProjectContextMenu, &QAction::triggered,
            d, &QmakeProjectManagerPluginPrivate::buildProjectContextMenu);

    d->m_cleanProjectContextMenu = new QAction(tr("Clear"), this);
    command = ActionManager::registerAction(d->m_cleanProjectContextMenu, Constants::CLEARPROJECTCONTEXTMENU, projectContext);
    command->setAttribute(Command::CA_Hide);
    mproject->addAction(command, ProjectExplorer::Constants::G_PROJECT_BUILD);
    connect(d->m_cleanProjectContextMenu, &QAction::triggered,
            d, &QmakeProjectManagerPluginPrivate::clearProjectContextMenu);

    connect(BuildManager::instance(), &BuildManager::buildStateChanged,
            d, &QmakeProjectManagerPluginPrivate::buildStateChanged);
    connect(SessionManager::instance(), &SessionManager::startupProjectChanged,
            d, &QmakeProjectManagerPluginPrivate::projectChanged);
    connect(ProjectTree::instance(), &ProjectTree::currentProjectChanged,
            d, &QmakeProjectManagerPluginPrivate::projectChanged);

    connect(ProjectTree::instance(), &ProjectTree::currentNodeChanged,
            d, &QmakeProjectManagerPluginPrivate::updateContextActions);

    ActionContainer *contextMenu = ActionManager::createMenu(QmakeProjectManager::Constants::M_CONTEXT);

    Context proFileEditorContext = Context(QmakeProjectManager::Constants::PROFILE_EDITOR_ID);

    command = ActionManager::command(TextEditor::Constants::JUMP_TO_FILE_UNDER_CURSOR);
    contextMenu->addAction(command);

    //haojie.fang 项目合并
    d->m_MergeProjectAaction = new QAction(tr("Merge Project..."), this);
    command = ActionManager::registerAction(d->m_MergeProjectAaction,
        Constants::MERGEPROJECT, projectTreeContext);
    connect(d->m_MergeProjectAaction, &QAction::triggered,
            d, &QmakeProjectManagerPluginPrivate::mergeProject);
    mproject->addAction(command, ProjectExplorer::Constants::G_PROJECT_FILES);

    d->m_ImportExcelProjectAaction = new QAction(tr("Import Excel Project..."), this);
    command = ActionManager::registerAction(d->m_ImportExcelProjectAaction,
        Constants::IMPORTEXCELPROJECT, projectTreeContext);
    connect(d->m_ImportExcelProjectAaction, &QAction::triggered,
            d, &QmakeProjectManagerPluginPrivate::importExcelProject);
    mproject->addAction(command, ProjectExplorer::Constants::G_PROJECT_FILES);

    contextMenu->addSeparator(proFileEditorContext);

    command = ActionManager::command(TextEditor::Constants::UN_COMMENT_SELECTION);
    contextMenu->addAction(command);

    connect(EditorManager::instance(), &EditorManager::currentEditorChanged,
            d, &QmakeProjectManagerPluginPrivate::updateBuildFileAction);

    d->m_codeBuildAction = new QAction(QIcon(":/qmakeprojectmanager/images/codebuild.png"), "Code Build", this);
    d->m_codeBuildAction->setObjectName("Code Build");
    connect(d->m_codeBuildAction, &QAction::triggered,
            d, &QmakeProjectManagerPluginPrivate::buildProjectContextMenu);
    ModeManager::addAction(d->m_codeBuildAction, P_ACTION_CODEBUILD);
    d->updateActions();

    return true;
}

QmakeProjectManagerPluginPrivate::~QmakeProjectManagerPluginPrivate()
{

}

void QmakeProjectManagerPluginPrivate::projectChanged()
{
    if (m_previousStartupProject)
        disconnect(m_previousStartupProject, &Project::activeTargetChanged,
                   this, &QmakeProjectManagerPluginPrivate::activeTargetChanged);

    if (ProjectTree::currentProject())
        m_previousStartupProject = qobject_cast<QmakeProject *>(ProjectTree::currentProject());
    else
        m_previousStartupProject = qobject_cast<QmakeProject *>(SessionManager::startupProject());

    if (m_previousStartupProject) {
        connect(m_previousStartupProject, &Project::activeTargetChanged,
                this, &QmakeProjectManagerPluginPrivate::activeTargetChanged);
    }

    activeTargetChanged();

    QTimer::singleShot(200, [](){
        if(ProjectTree::instance())
            ProjectTree::instance()->expandAllWithFirst();//haojie.fang 默认全展开
    });
}

static QmakePriFileNode *buildableDirNode(Node *node)
{
    if (node) {
        auto subPriFileNode = dynamic_cast<QmakePriFileNode *>(node);
        if (subPriFileNode)
            return subPriFileNode;
    }
    return nullptr;
}

void QmakeProjectManagerPluginPrivate::mergeProject()
{
    QString projectPath;
    QString projectFile;

    Node *node = ProjectTree::currentNode();
    if (ContainerNode *cn = node->asContainerNode())
    {
        projectPath = cn->project()->projectDirectory().toString();
        projectFile = cn->project()->projectFilePath().toString();
    }
    else if (dynamic_cast<QmakeProFileNode *>(node))
        projectFile = node->filePath().toString();
    m_projectMerge->slotMerge(projectPath);
}

void QmakeProjectManagerPluginPrivate::importExcelProject()
{
    QString projectPath;
    QString projectFile;

    Node *node = ProjectTree::currentNode();
    if (ContainerNode *cn = node->asContainerNode())
    {
        projectPath = cn->project()->projectDirectory().toString();
        projectFile = cn->project()->projectFilePath().toString();

        ExcelProjectImport* import = new ExcelProjectImport(cn->project(), this);
        import->import();
        delete import;import = nullptr;
    }
}

void QmakeProjectManagerPluginPrivate::runQMake()
{
    runQMakeImpl(SessionManager::startupProject(), nullptr);
}

void QmakeProjectManagerPluginPrivate::runQMakeContextMenu()
{
    runQMakeImpl(ProjectTree::currentProject(), ProjectTree::currentNode());
}

void QmakeProjectManagerPluginPrivate::runQMakeImpl(Project *p, Node *node)
{
}

void QmakeProjectManagerPluginPrivate::handleSubDirContextMenuDev(QmakeBuildSystem::Action action, bool isFileBuild)
{
    qputenv("Developer", "true");
    handleSubDirContextMenu(action, isFileBuild);
    qunsetenv("Developer");
}

void QmakeProjectManagerPluginPrivate::handleSubDirContextMenu(QmakeBuildSystem::Action action, bool isFileBuild)
{
    //haojie.fang
    Node *node = ProjectTree::currentNode();

    QmakePriFileNode *subProjectNode = buildableDirNode(node);
    FileNode *fileNode = node ? node->asFileNode() : nullptr;
    bool buildFilePossible = fileNode && fileNode->fileType() == FileType::Source;
    FileNode *buildableFileNode = buildFilePossible ? fileNode : nullptr;

    if (auto bs = qobject_cast<QmakeBuildSystem *>(ProjectTree::currentBuildSystem()))
        bs->buildHelper(action, isFileBuild, subProjectNode, buildableFileNode);
}

void QmakeProjectManagerPluginPrivate::activeTargetChanged()
{
    m_previousTarget = m_previousStartupProject ? m_previousStartupProject->activeTarget() : nullptr;

    if (m_previousTarget) {
        connect(m_previousTarget, &Target::parsingFinished,
                this, &QmakeProjectManagerPluginPrivate::updateActions);
    }

}

void QmakeProjectManagerPluginPrivate::updateActions()
{
    const bool hasProjects = SessionManager::hasProjects();
    m_codeBuildAction->setEnabled(hasProjects);

    updateContextActions();
}

void QmakeProjectManagerPluginPrivate::updateContextActions()
{
    Node *node = ProjectTree::currentNode();
    Project *project = ProjectTree::currentProject();

    const ContainerNode *containerNode = node ? node->asContainerNode() : nullptr;
    const auto *proFileNode = dynamic_cast<const QmakeProFileNode *>(containerNode ? containerNode->rootProjectNode() : node);

    auto *qmakeProject = qobject_cast<QmakeProject *>(project);
    QmakePriFileNode *subDir = nullptr;
    disableBuildFileMenus();
    if (node) {
        subDir = dynamic_cast<QmakePriFileNode *>(node);

        if (const FileNode *fileNode = node->asFileNode())
            enableBuildFileMenus(fileNode->filePath());
    }

    bool subProjectActionsVisible = false;
    if (qmakeProject && subDir) {
        if (ProjectNode *rootNode = qmakeProject->rootProjectNode())
            subProjectActionsVisible = subDir != rootNode;
        if (subDir->displayName() != Constants::InterfaceDesign &&
            subDir->displayName() != Constants::ComponentDesign &&
            subDir->displayName() != Constants::ArchitectureDesign &&
            subDir->displayName() != Constants::ScheduleConfiguration)
            subProjectActionsVisible = false;
    }

    QString subProjectName;
    if (subProjectActionsVisible)
        subProjectName = subDir->displayName();

    bool projectActionsVisible = false;
    QString projectName;
    if(project)
    {
        projectActionsVisible = true;
        projectName = project->displayName();
    }

    bool projectActionsContextMenuVisible = true;
    if(project && proFileNode)
        projectActionsContextMenuVisible = true;

    //m_buildProjectAction->setParameter(projectName);
    //m_cleanProjectAction->setParameter(projectName);
    //m_buildSubProjectContextMenu->setParameter(proFileNode ? proFileNode->displayName() : QString());

    bool isBuilding = BuildManager::isBuilding(project);
    bool enabled = subProjectActionsVisible && !isBuilding;

    m_buildProjectActionDev->setVisible(projectActionsVisible);
    m_buildProjectAction->setVisible(projectActionsVisible);
    m_cleanProjectAction->setVisible(projectActionsVisible);
    m_buildProjectContextMenu->setVisible(projectActionsContextMenuVisible);
    m_cleanProjectContextMenu->setVisible(projectActionsContextMenuVisible);
    m_buildSubProjectContextMenu->setVisible(subProjectActionsVisible);

    m_buildProjectActionDev->setEnabled(projectActionsVisible && !isBuilding);
    m_buildProjectAction->setEnabled(projectActionsVisible && !isBuilding);
    m_cleanProjectAction->setEnabled(projectActionsVisible && !isBuilding);
    m_buildProjectContextMenu->setEnabled(projectActionsContextMenuVisible && !isBuilding);
    m_cleanProjectContextMenu->setEnabled(projectActionsContextMenuVisible && !isBuilding);
    m_buildSubProjectContextMenu->setEnabled(enabled);
}

void QmakeProjectManagerPluginPrivate::buildStateChanged(Project *pro)
{
    if (pro == ProjectTree::currentProject()) {
        updateContextActions();
        updateBuildFileAction();
    }
}

void QmakeProjectManagerPluginPrivate::updateBuildFileAction()
{
    disableBuildFileMenus();
    if (IDocument *currentDocument = EditorManager::currentDocument())
        enableBuildFileMenus(currentDocument->filePath());
}

void QmakeProjectManagerPluginPrivate::disableBuildFileMenus()
{
    m_buildFileContextMenu->setEnabled(false);
    m_buildXmlContextMenu->setEnabled(false);
    m_selectBuildFileContextMenu->setEnabled(false);
}

void QmakeProjectManagerPluginPrivate::enableBuildFileMenus(const Utils::FilePath &file)
{
    bool visible = false;
    bool enabled = false;
    bool isCfv = false;

    if (Node *node = ProjectTree::nodeForFile(file)) {
        if (Project *project = SessionManager::projectForFile(file)) {
            if (const FileNode *fileNode = node->asFileNode()) {
                const FileType type = fileNode->fileType();
                visible = qobject_cast<QmakeProject *>(project)
                        && dynamic_cast<QmakePriFileNode *>(node->parentProjectNode())
                        && (type == FileType::Source || type == FileType::Header);

                enabled = !BuildManager::isBuilding(project);

                if(file.endsWith(".cfv") || file.endsWith(".CFV"))
                    isCfv = true;
            }
        }
    }
    m_buildFileContextMenu->setEnabled(visible && enabled);

    //haojie.fang
    m_buildXmlContextMenu->setVisible(visible && enabled && isCfv);
    m_buildXmlContextMenu->setEnabled(visible && enabled && isCfv);
    m_selectBuildFileContextMenu->setVisible(visible && enabled && isCfv);
    m_selectBuildFileContextMenu->setEnabled(visible && enabled && isCfv);
}

} // Internal
} // QmakeProjectManager
