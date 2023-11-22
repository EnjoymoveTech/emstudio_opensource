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

#include "qmakeproject.h"

#include "qmakebuildconfiguration.h"
#include "qmakenodes.h"
#include "qmakenodetreebuilder.h"
#include "qmakeprojectmanagerconstants.h"
#include "profilereader.h"
#include "utils/stringutils.h"

#include <coreplugin/documentmanager.h>
#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>
#include <coreplugin/progressmanager/progressmanager.h>


#include <explorer/buildinfo.h>
#include <explorer/buildmanager.h>
#include <explorer/buildtargetinfo.h>
#include <explorer/deploymentdata.h>
#include <explorer/headerpath.h>
#include <explorer/explorer.h>
#include <explorer/projectexplorerconstants.h>
#include <explorer/runconfiguration.h>
#include <explorer/target.h>
#include <explorer/taskhub.h>
#include <explorer/toolchain.h>
#include <explorer/kitmanager.h>
#include <explorer/projecttree.h>

#include <proparser/qmakevfs.h>
#include <proparser/qmakeglobals.h>

#include <utils/algorithm.h>
#include <utils/runextensions.h>

#include <emoscodebuild/emoscodebuildexport.h>
#include <emoscodebuild/buildoutpane/buildoutlogmanager.h>

#include <QDebug>
#include <QDir>
#include <QFileSystemWatcher>
#include <QFuture>
#include <QLoggingCategory>
#include <QTimer>
#include <QElapsedTimer>
#include <QtConcurrent/QtConcurrent>
#include <QEventLoop>

using namespace QmakeProjectManager::Internal;
using namespace ProjectExplorer;
using namespace Utils;

namespace QmakeProjectManager {
namespace Internal {

const int UPDATE_INTERVAL = 100;

static Q_LOGGING_CATEGORY(qmakeBuildSystemLog, "qtc.qmake.buildsystem", QtWarningMsg);

#define TRACE(msg)                                                   \
    if (qmakeBuildSystemLog().isDebugEnabled()) {                    \
        qCDebug(qmakeBuildSystemLog)                                 \
            << qPrintable(buildConfiguration()->displayName())       \
            << ", guards project: " << int(m_guard.guardsProject())  \
            << ", isParsing: " << int(isParsing())                   \
            << ", hasParsingData: " << int(hasParsingData())         \
            << ", " << __FUNCTION__                                  \
            << msg;                                                  \
    }

class QmakePriFileDocument : public Core::IDocument
{
public:
    QmakePriFileDocument(QmakePriFile *qmakePriFile, const Utils::FilePath &filePath) :
        IDocument(nullptr), m_priFile(qmakePriFile)
    {
        setId("Qmake.PriFile");
        setMimeType(QLatin1String(QmakeProjectManager::Constants::PROFILE_MIMETYPE));
        setFilePath(filePath);
        Core::DocumentManager::addDocument(this);
    }

    ReloadBehavior reloadBehavior(ChangeTrigger state, ChangeType type) const override
    {
        Q_UNUSED(state)
        Q_UNUSED(type)
        return BehaviorSilent;
    }
    bool reload(QString *errorString, ReloadFlag flag, ChangeType type) override
    {
        Q_UNUSED(errorString)
        Q_UNUSED(flag)
        if (type == TypePermissions)
            return true;
        m_priFile->scheduleUpdate();
        return true;
    }

    void setPriFile(QmakePriFile *priFile) { m_priFile = priFile; }

private:
    QmakePriFile *m_priFile;
};

/// Watches folders for QmakePriFile nodes
/// use one file system watcher to watch all folders
/// such minimizing system ressouce usage

class CentralizedFolderWatcher : public QObject
{
    Q_OBJECT
public:
    CentralizedFolderWatcher(QmakeBuildSystem *BuildSystem);

    void watchFolders(const QList<QString> &folders, QmakePriFile *file);
    void unwatchFolders(const QList<QString> &folders, QmakePriFile *file);

private:
    void folderChanged(const QString &folder);
    void onTimer();
    void delayedFolderChanged(const QString &folder);

    QmakeBuildSystem *m_buildSystem;
    QSet<QString> recursiveDirs(const QString &folder);
    QFileSystemWatcher m_watcher;
    QMultiMap<QString, QmakePriFile *> m_map;

    QSet<QString> m_recursiveWatchedFolders;
    QTimer m_compressTimer;
    QSet<QString> m_changedFolders;
};

} // namespace Internal

/*!
  \class QmakeProject

  QmakeProject manages information about an individual qmake project file (.pro).
  */

QmakeProject::QmakeProject(const FilePath &fileName) :
    Project(QmakeProjectManager::Constants::PROFILE_MIMETYPE, fileName)
{
    setId(Constants::QMAKEPROJECT_ID);
    setProjectLanguages(Core::Context(ProjectExplorer::Constants::CXX_LANGUAGE_ID));
    setDisplayName(fileName.toFileInfo().completeBaseName());
    setCanBuildProducts();
    setHasMakeInstallEquivalent(true);
}

QmakeProject::~QmakeProject()
{
    // Make sure root node (and associated readers) are shut hown before proceeding
    setRootProjectNode(nullptr);
}

Project::RestoreResult QmakeProject::fromMap(const QVariantMap &map, QString *errorMessage)
{
    RestoreResult result = Project::fromMap(map, errorMessage);
    if (result != RestoreResult::Ok)
        return result;

    // Prune targets without buildconfigurations:
    // This can happen esp. when updating from a old version of Qt Creator
    QList<Target *>ts = targets();
    foreach (Target *t, ts) {
        if (t->buildConfigurations().isEmpty()) {
            qWarning() << "Removing" << t->id().name() << "since it has no buildconfigurations!";
            removeTarget(t);
        }
    }

    return RestoreResult::Ok;
}

DeploymentKnowledge QmakeProject::deploymentKnowledge() const
{
    return DeploymentKnowledge::Approximative; // E.g. EMSTUDIOBUG-21855
}

//
// QmakeBuildSystem
//

QmakeBuildSystem::QmakeBuildSystem(QmakeBuildConfiguration *bc)
    : BuildSystem(bc)
    , m_qmakeVfs(new QMakeVfs)
    //, m_cppCodeModelUpdater(new CppTools::CppProjectUpdater)
{
    setParseDelay(0);

    m_rootProFile = std::make_unique<QmakeProFile>(this, projectFilePath());

    connect(BuildManager::instance(), &BuildManager::buildQueueFinished,
            this, &QmakeBuildSystem::buildFinished);

    connect(bc->target(),
            &Target::activeBuildConfigurationChanged,
            this,
            [this](BuildConfiguration *bc) {
                if (bc == buildConfiguration())
                    scheduleUpdateAllNowOrLater();
                // FIXME: This is too eager in the presence of not handling updates
                // when the build configuration is not active, see startAsyncTimer
                // below.
                //        else
                //            m_cancelEvaluate = true;
            });

    connect(bc->project(), &Project::activeTargetChanged,
            this, &QmakeBuildSystem::activeTargetWasChanged);

    connect(bc->project(), &Project::projectFileIsDirty,
            this, &QmakeBuildSystem::scheduleUpdateAllLater);

    connect(bc, &BuildConfiguration::buildDirectoryChanged,
            this, &QmakeBuildSystem::scheduleUpdateAllNowOrLater);
    connect(bc, &BuildConfiguration::environmentChanged,
            this, &QmakeBuildSystem::scheduleUpdateAllNowOrLater);
}

QmakeBuildSystem::~QmakeBuildSystem()
{
    m_guard = {};

    m_asyncUpdateState = ShuttingDown;

    // Make sure root node (and associated readers) are shut hown before proceeding
    m_rootProFile.reset();
    if (m_qmakeGlobalsRefCnt > 0) {
        m_qmakeGlobalsRefCnt = 0;
        deregisterFromCacheManager();
    }

    m_cancelEvaluate = true;
    QTC_CHECK(m_qmakeGlobalsRefCnt == 0);
    delete m_qmakeVfs;
    m_qmakeVfs = nullptr;
}

void QmakeBuildSystem::updateCodeModels()
{
    if (!buildConfiguration()->isActive())
        return;
}

void QmakeBuildSystem::updateDocuments()
{
    QSet<FilePath> projectDocuments;
    project()->rootProjectNode()->forEachProjectNode([&projectDocuments](const ProjectNode *n) {
        projectDocuments.insert(n->filePath());

    });
    const auto priFileForPath = [p = project()](const FilePath &fp) -> QmakePriFile * {
        const Node * const n = p->nodeForFilePath(fp, [](const Node *n) {
            return dynamic_cast<const QmakePriFileNode *>(n); });
        QTC_ASSERT(n, return nullptr);
        return static_cast<const QmakePriFileNode *>(n)->priFile();
    };
    const auto docGenerator = [&](const FilePath &fp)
            -> std::unique_ptr<Core::IDocument> {
        QmakePriFile * const priFile = priFileForPath(fp);
        QTC_ASSERT(priFile, return std::make_unique<Core::IDocument>());
        return std::make_unique<QmakePriFileDocument>(priFile, fp);
    };
    const auto docUpdater = [&](Core::IDocument *doc) {
        QmakePriFile * const priFile = priFileForPath(doc->filePath());
        QTC_ASSERT(priFile, return);
        static_cast<QmakePriFileDocument *>(doc)->setPriFile(priFile);
    };
    project()->setExtraProjectFiles(projectDocuments, docGenerator, docUpdater);
}

void QmakeBuildSystem::scheduleAsyncUpdateFile(QmakeProFile *file, QmakeProFile::AsyncUpdateDelay delay)
{
    if (m_asyncUpdateState == ShuttingDown)
        return;

    if (m_cancelEvaluate) {
        // A cancel is in progress
        // That implies that a full update is going to happen afterwards
        // So we don't need to do anything
        return;
    }

    file->setParseInProgressRecursive(true);

    if (m_asyncUpdateState == AsyncFullUpdatePending) {
        // Just postpone
        startAsyncTimer(delay);
    } else if (m_asyncUpdateState == AsyncPartialUpdatePending
               || m_asyncUpdateState == Base) {
        // Add the node
        m_asyncUpdateState = AsyncPartialUpdatePending;

        bool add = true;
        auto it = m_partialEvaluate.begin();
        while (it != m_partialEvaluate.end()) {
            if (*it == file) {
                add = false;
                break;
            } else if (file->isParent(*it)) { // We already have the parent in the list, nothing to do
                it = m_partialEvaluate.erase(it);
            } else if ((*it)->isParent(file)) { // The node is the parent of a child already in the list
                add = false;
                break;
            } else {
                ++it;
            }
        }

        if (add)
            m_partialEvaluate.append(file);

        // Cancel running code model update
        //m_cppCodeModelUpdater->cancel();

        startAsyncTimer(delay);
    } else if (m_asyncUpdateState == AsyncUpdateInProgress) {
        // A update is in progress
        // And this slot only gets called if a file changed on disc
        // So we'll play it safe and schedule a complete evaluate
        // This might trigger if due to version control a few files
        // change a partial update gets in progress and then another
        // batch of changes come in, which triggers a full update
        // even if that's not really needed
        scheduleUpdateAll(delay);
    }
}

void QmakeBuildSystem::scheduleUpdateAllNowOrLater()
{
    if (m_firstParseNeeded)
        scheduleUpdateAll(QmakeProFile::ParseNow);
    else
        scheduleUpdateAll(QmakeProFile::ParseLater);
}

bool matchNodesNeedBuild(const ProjectExplorer::Node* n)
{
    if(n)
    {
        if(!n->filePath().exists() && n->filePath().isDir())
            return false;
        if(n->filePath().endsWith(".cfv") || n->filePath().endsWith(".sch"))
            return true;
    }
    return false;
}

void QmakeBuildSystem::buildEmosCode(Action action, bool isFileBuild, QmakePriFileNode *prifile,
                                     ProjectExplorer::FileNode *buildableFile)
{
    int buildRet = 0;
    QElapsedTimer elapsed;
    QString buildPath = project()->activeTarget()->activeBuildConfiguration()->buildDirectory().toString();

    EmosCodeBuild::EmosCodeBuildExport* emos = EmosCodeBuild::EmosCodeBuildExport::instance();
    if(!emos)
    {
        EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime(tr("Build Failed"), Utils::ErrorMessageFormat);
        return;
    }

    auto buildSingle = [&](Action a, const QString& filePath, const QString& buildPath) -> int {
        int ret = 0;
        switch(a)
        {
        case Action::BUILD:
            ret = emos->build(filePath, buildPath);
            break;
        case Action::SELECTBUILD:
            ret = emos->selectBuild(filePath, buildPath);
            break;
        case Action::BUILDXML:
            ret = emos->buildXml(XmlBuildType(Build_System|Build_Global|Build_OPT), filePath, buildPath);
            break;
        case Action::CLEAN:
            ret = emos->clear(buildPath);
            break;
        default:
            ret = -1;
        }
        return ret;
    };

    elapsed.start();

    if(action == Action::PROJECTBUILD)
    {
        ProjectExplorer::Project* project = ProjectTree::currentProject();
        EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime(
                    tr("Start Build Project ") + project->displayName() + "...", Utils::LogMessageFormat);
        Utils::FilePaths files = project->files(matchNodesNeedBuild);

        for(auto file : files)
        {
            buildRet += buildSingle(Action::BUILD, file.toString(), buildPath);
        }
    }
    else if(action == Action::CLEAN)
    {
        ProjectExplorer::Project* project = ProjectTree::currentProject();
        EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime(
                    tr("Start Clean Project ") + project->displayName() + "...", Utils::LogMessageFormat);
        buildRet += buildSingle(action, "", buildPath);
    }
    else
    {
        if(isFileBuild)
        {
            EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime(
                        tr("Start Build ") + buildableFile->displayName() + "...", Utils::LogMessageFormat);
            buildRet = buildSingle(action, buildableFile->filePath().toString(),buildPath);
        }
        else
        {
            EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime(
                        tr("Start Build Dir ") + prifile->displayName() + "...", Utils::LogMessageFormat);
            for(auto filenode : prifile->fileNodes())
            {
                buildRet += buildSingle(action, filenode->filePath().toString(),buildPath);
            }
        }
    }

    if(!buildRet)
        EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime(
                    tr("Build Successful !!"), Utils::LogMessageFormat);
    else
        EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime(
                    tr("Build Failed !!"), Utils::ErrorMessageFormat);

    const QString elapsedTime = Utils::formatElapsedTime(elapsed.elapsed());
    EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime(
                elapsedTime, Utils::LogMessageFormat);
    EmosCodeBuild::BuildoutLogManager::instance()->writeWithTime(
                "-------------------------------------------------------------------", Utils::LogMessageFormat);
}

QmakeBuildConfiguration *QmakeBuildSystem::qmakeBuildConfiguration() const
{
    return static_cast<QmakeBuildConfiguration *>(BuildSystem::buildConfiguration());
}

void QmakeBuildSystem::scheduleUpdateAll(QmakeProFile::AsyncUpdateDelay delay)
{
    if (m_asyncUpdateState == ShuttingDown) {
        TRACE("suppressed: we are shutting down");
        return;
    }

    if (m_cancelEvaluate) { // we are in progress of canceling
                            // and will start the evaluation after that
        TRACE("suppressed: was previously canceled");
        return;
    }

    //emos todo
#if 0
    if (!buildConfiguration()->isActive()) {
        TRACE("firstParseNeeded: " << int(m_firstParseNeeded)
              << ", suppressed: buildconfig not active");
        return;
    }
#endif

    TRACE("firstParseNeeded: " << int(m_firstParseNeeded) << ", delay: " << delay);

    rootProFile()->setParseInProgressRecursive(true);

    if (m_asyncUpdateState == AsyncUpdateInProgress) {
        m_cancelEvaluate = true;
        m_asyncUpdateState = AsyncFullUpdatePending;
        return;
    }

    m_partialEvaluate.clear();
    m_asyncUpdateState = AsyncFullUpdatePending;

    // Cancel running code model update
    //m_cppCodeModelUpdater->cancel();
    startAsyncTimer(delay);
}

void QmakeBuildSystem::startAsyncTimer(QmakeProFile::AsyncUpdateDelay delay)
{
    if (!buildConfiguration()->isActive()) {
        TRACE("skipped, not active")
        return;
    }

    const int interval = qMin(parseDelay(),
                              delay == QmakeProFile::ParseLater ? UPDATE_INTERVAL : 0);
    TRACE("interval: " << interval);
    requestParseWithCustomDelay(interval);
}

void QmakeBuildSystem::incrementPendingEvaluateFutures()
{
    if (m_pendingEvaluateFuturesCount == 0) {
        // The guard actually might already guard the project if this
        // here is the re-start of a previously aborted parse due to e.g.
        // changing build directories while parsing.
        if (!m_guard.guardsProject())
            m_guard = guardParsingRun();
    }
    ++m_pendingEvaluateFuturesCount;
}

void QmakeBuildSystem::decrementPendingEvaluateFutures()
{
    --m_pendingEvaluateFuturesCount;
    TRACE("pending dec to: " << m_pendingEvaluateFuturesCount);

    if (!rootProFile()) {
        TRACE("closing project");
        return; // We are closing the project!
    }

    if (m_pendingEvaluateFuturesCount == 0) {
        // We are done!
        setRootProjectNode(QmakeNodeTreeBuilder::buildTree(this));

        m_cancelEvaluate = false;

        // TODO clear the profile cache ?
        if (m_asyncUpdateState == AsyncFullUpdatePending || m_asyncUpdateState == AsyncPartialUpdatePending) {
            // Already parsing!
            rootProFile()->setParseInProgressRecursive(true);
            startAsyncTimer(QmakeProFile::ParseLater);
        } else  if (m_asyncUpdateState != ShuttingDown){
            // After being done, we need to call:

            m_asyncUpdateState = Base;
            updateBuildSystemData();
            updateCodeModels();
            updateDocuments();
            target()->updateDefaultDeployConfigurations();
            m_guard.markAsSuccess(); // Qmake always returns (some) data, even when it failed:-)
            TRACE("success" << int(m_guard.isSuccess()));
            m_guard = {}; // This triggers emitParsingFinished by destroying the previous guard.

            m_firstParseNeeded = false;
            TRACE("first parse succeeded");

            emitBuildSystemUpdated();
        }
    }
}

bool QmakeBuildSystem::wasEvaluateCanceled()
{
    return m_cancelEvaluate;
}

void QmakeBuildSystem::asyncUpdate()
{
    TaskHub::clearTasks(ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM);
    setParseDelay(UPDATE_INTERVAL);
    TRACE("");

    if (m_invalidateQmakeVfsContents) {
        m_invalidateQmakeVfsContents = false;
        m_qmakeVfs->invalidateContents();
    } else {
        m_qmakeVfs->invalidateCache();
    }

    if (m_asyncUpdateState == AsyncFullUpdatePending) {
        rootProFile()->asyncUpdate();
    } else {
        foreach (QmakeProFile *file, m_partialEvaluate)
            file->asyncUpdate();
    }

    m_partialEvaluate.clear();
    m_asyncUpdateState = AsyncUpdateInProgress;
}

void QmakeBuildSystem::buildFinished(bool success)
{
    if (success)
        m_invalidateQmakeVfsContents = true;
}

// Find the folder that contains a file with a certain name (recurse down)
static FolderNode *folderOf(FolderNode *in, const FilePath &fileName)
{
    foreach (FileNode *fn, in->fileNodes())
        if (fn->filePath() == fileName)
            return in;
    foreach (FolderNode *folder, in->folderNodes())
        if (FolderNode *pn = folderOf(folder, fileName))
            return pn;
    return nullptr;
}

// Find the QmakeProFileNode that contains a certain file.
// First recurse down to folder, then find the pro-file.
static FileNode *fileNodeOf(FolderNode *in, const FilePath &fileName)
{
    for (FolderNode *folder = folderOf(in, fileName); folder; folder = folder->parentFolderNode()) {
        if (auto *proFile = dynamic_cast<QmakeProFileNode *>(folder)) {
            foreach (FileNode *fileNode, proFile->fileNodes()) {
                if (fileNode->filePath() == fileName)
                    return fileNode;
            }
        }
    }
    return nullptr;
}

FilePath QmakeBuildSystem::buildDir(const FilePath &proFilePath) const
{
    const QDir srcDirRoot = QDir(projectDirectory().toString());
    const QString relativeDir = srcDirRoot.relativeFilePath(proFilePath.parentDir().toString());
    const QString buildConfigBuildDir = buildConfiguration()->buildDirectory().toString();
    const QString buildDir = buildConfigBuildDir.isEmpty()
                                 ? projectDirectory().toString()
                                 : buildConfigBuildDir;
    return FilePath::fromString(QDir::cleanPath(QDir(buildDir).absoluteFilePath(relativeDir)));
}

void QmakeBuildSystem::proFileParseError(const QString &errorMessage, const FilePath &filePath)
{
    TaskHub::addTask(BuildSystemTask(Task::Error, errorMessage, filePath));
}

ProFileReader *QmakeBuildSystem::createProFileReader(const QmakeProFile *qmakeProFile)
{
    if (!m_qmakeGlobals) {
        m_qmakeGlobals = std::make_unique<QMakeGlobals>();
        m_qmakeGlobalsRefCnt = 0;

        Kit *k = kit();
        QmakeBuildConfiguration *bc = qmakeBuildConfiguration();

        Environment env = bc->environment();

        m_qmakeGlobals->setDirectories(rootProFile()->sourceDir().toString(),
                                       buildDir(rootProFile()->filePath()).toString());

        Environment::const_iterator eit = env.constBegin(), eend = env.constEnd();
        for (; eit != eend; ++eit)
            m_qmakeGlobals->environment.insert(env.key(eit), env.expandedValueForKey(env.key(eit)));

        ProFileCacheManager::instance()->incRefCount();
    }
    ++m_qmakeGlobalsRefCnt;

    auto reader = new ProFileReader(m_qmakeGlobals.get(), m_qmakeVfs);

    reader->setOutputDir(buildDir(qmakeProFile->filePath()).toString());

    return reader;
}

QMakeGlobals *QmakeBuildSystem::qmakeGlobals()
{
    return m_qmakeGlobals.get();
}

QMakeVfs *QmakeBuildSystem::qmakeVfs()
{
    return m_qmakeVfs;
}

QString QmakeBuildSystem::qmakeSysroot()
{
    return m_qmakeSysroot;
}

void QmakeBuildSystem::destroyProFileReader(ProFileReader *reader)
{
    // The ProFileReader destructor is super expensive (but thread-safe).
    const auto deleteFuture = runAsync(ProjectExplorerPlugin::sharedThreadPool(), QThread::LowestPriority,
                    [reader] { delete reader; });
    onFinished(deleteFuture, this, [this](const QFuture<void> &) {
        if (!--m_qmakeGlobalsRefCnt) {
            deregisterFromCacheManager();
            m_qmakeGlobals.reset();
        }
    });
}

void QmakeBuildSystem::deregisterFromCacheManager()
{
    QString dir = projectFilePath().toString();
    if (!dir.endsWith(QLatin1Char('/')))
        dir += QLatin1Char('/');
    ProFileCacheManager::instance()->discardFiles(dir, qmakeVfs());
    ProFileCacheManager::instance()->decRefCount();
}

void QmakeBuildSystem::activeTargetWasChanged(Target *t)
{
    // We are only interested in our own target.
    if (t != target())
        return;

    m_invalidateQmakeVfsContents = true;
    scheduleUpdateAll(QmakeProFile::ParseLater);
}

static void notifyChangedHelper(const FilePath &fileName, QmakeProFile *file)
{
    if (file->filePath() == fileName) {
        ProFileCacheManager::instance()->discardFile(
                    fileName.toString(), file->buildSystem()->qmakeVfs());
        file->scheduleUpdate(QmakeProFile::ParseNow);
    }

    for (QmakePriFile *fn : file->children()) {
        if (auto pro = dynamic_cast<QmakeProFile *>(fn))
            notifyChangedHelper(fileName, pro);
    }
}

void QmakeBuildSystem::notifyChanged(const FilePath &name)
{
    FilePaths files = project()->files([&name](const Node *n) {
        return Project::SourceFiles(n) && n->filePath() == name;
    });

    if (files.isEmpty())
        return;

    notifyChangedHelper(name, m_rootProFile.get());
}

void QmakeBuildSystem::watchFolders(const QStringList &l, QmakePriFile *file)
{
    if (l.isEmpty())
        return;
    if (!m_centralizedFolderWatcher)
        m_centralizedFolderWatcher = new Internal::CentralizedFolderWatcher(this);
    m_centralizedFolderWatcher->watchFolders(l, file);
}

void QmakeBuildSystem::unwatchFolders(const QStringList &l, QmakePriFile *file)
{
    if (m_centralizedFolderWatcher && !l.isEmpty())
        m_centralizedFolderWatcher->unwatchFolders(l, file);
}

/////////////
/// Centralized Folder Watcher
////////////

// All the folder have a trailing slash!
CentralizedFolderWatcher::CentralizedFolderWatcher(QmakeBuildSystem *parent)
    : QObject(parent), m_buildSystem(parent)
{
    m_compressTimer.setSingleShot(true);
    m_compressTimer.setInterval(200);
    connect(&m_compressTimer, &QTimer::timeout, this, &CentralizedFolderWatcher::onTimer);
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged,
            this, &CentralizedFolderWatcher::folderChanged);
}

QSet<QString> CentralizedFolderWatcher::recursiveDirs(const QString &folder)
{
    QSet<QString> result;
    QDir dir(folder);
    QStringList list = dir.entryList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    foreach (const QString &f, list) {
        const QString a = folder + f + QLatin1Char('/');
        result.insert(a);
        result += recursiveDirs(a);
    }
    return result;
}

void CentralizedFolderWatcher::watchFolders(const QList<QString> &folders, QmakePriFile *file)
{
    m_watcher.addPaths(folders);

    const QChar slash = QLatin1Char('/');
    foreach (const QString &f, folders) {
        QString folder = f;
        if (!folder.endsWith(slash))
            folder.append(slash);
        m_map.insert(folder, file);

        // Support for recursive watching
        // we add the recursive directories we find
        QSet<QString> tmp = recursiveDirs(folder);
        if (!tmp.isEmpty())
            m_watcher.addPaths(Utils::toList(tmp));
        m_recursiveWatchedFolders += tmp;
    }
}

void CentralizedFolderWatcher::unwatchFolders(const QList<QString> &folders, QmakePriFile *file)
{
    const QChar slash = QLatin1Char('/');
    foreach (const QString &f, folders) {
        QString folder = f;
        if (!folder.endsWith(slash))
            folder.append(slash);
        m_map.remove(folder, file);
        if (!m_map.contains(folder))
            m_watcher.removePath(folder);

        // Figure out which recursive directories we can remove
        // this might not scale. I'm pretty sure it doesn't
        // A scaling implementation would need to save more information
        // where a given directory watcher actual comes from...

        QStringList toRemove;
        foreach (const QString &rwf, m_recursiveWatchedFolders) {
            if (rwf.startsWith(folder)) {
                // So the rwf is a subdirectory of a folder we aren't watching
                // but maybe someone else wants us to watch
                bool needToWatch = false;
                auto end = m_map.constEnd();
                for (auto it = m_map.constBegin(); it != end; ++it) {
                    if (rwf.startsWith(it.key())) {
                        needToWatch = true;
                        break;
                    }
                }
                if (!needToWatch) {
                    m_watcher.removePath(rwf);
                    toRemove << rwf;
                }
            }
        }

        foreach (const QString &tr, toRemove)
            m_recursiveWatchedFolders.remove(tr);
    }
}

void CentralizedFolderWatcher::folderChanged(const QString &folder)
{
    m_changedFolders.insert(folder);
    m_compressTimer.start();
}

void CentralizedFolderWatcher::onTimer()
{
    foreach (const QString &folder, m_changedFolders)
        delayedFolderChanged(folder);
    m_changedFolders.clear();
}

void CentralizedFolderWatcher::delayedFolderChanged(const QString &folder)
{
    // Figure out whom to inform
    QString dir = folder;
    const QChar slash = QLatin1Char('/');
    bool newOrRemovedFiles = false;
    while (true) {
        if (!dir.endsWith(slash))
            dir.append(slash);
        QList<QmakePriFile *> files = m_map.values(dir);
        if (!files.isEmpty()) {
            // Collect all the files
            QSet<FilePath> newFiles;
            newFiles += QmakePriFile::recursiveEnumerate(folder);
            foreach (QmakePriFile *file, files)
                newOrRemovedFiles = newOrRemovedFiles || file->folderChanged(folder, newFiles);
        }

        // Chop off last part, and break if there's nothing to chop off
        //
        if (dir.length() < 2)
            break;

        // We start before the last slash
        const int index = dir.lastIndexOf(slash, dir.length() - 2);
        if (index == -1)
            break;
        dir.truncate(index + 1);
    }

    QString folderWithSlash = folder;
    if (!folder.endsWith(slash))
        folderWithSlash.append(slash);

    // If a subdirectory was added, watch it too
    QSet<QString> tmp = recursiveDirs(folderWithSlash);
    if (!tmp.isEmpty()) {
        QSet<QString> alreadyAdded = Utils::toSet(m_watcher.directories());
        tmp.subtract(alreadyAdded);
        if (!tmp.isEmpty())
            m_watcher.addPaths(Utils::toList(tmp));
        m_recursiveWatchedFolders += tmp;
    }

    if (newOrRemovedFiles)
        m_buildSystem->updateCodeModels();
}

void QmakeProject::configureAsExampleProject(Kit *kit)
{
    QList<BuildInfo> infoList;
    QList<Kit *> kits;
    if (kit)
        kits.append(kit);
    else
        kits = KitManager::kits();
    for (Kit *k : kits) {
        if (auto factory = BuildConfigurationFactory::find(k, projectFilePath()))
            infoList << factory->allAvailableSetups(k, projectFilePath());
    }
    setup(infoList);
}

void QmakeBuildSystem::updateBuildSystemData()
{
    const QmakeProFile *const file = rootProFile();
    if (!file || file->parseInProgress())
        return;

    DeploymentData deploymentData;
    collectData(file, deploymentData);
    setDeploymentData(deploymentData);

    QList<BuildTargetInfo> appTargetList;

    project()->rootProjectNode()->forEachProjectNode([this, &appTargetList](const ProjectNode *pn) {
        auto node = dynamic_cast<const QmakeProFileNode *>(pn);
        if (!node || !node->includedInExactParse())
            return;

        if (node->projectType() != ProjectType::ApplicationTemplate
                && node->projectType() != ProjectType::ScriptTemplate)
            return;

        TargetInformation ti = node->targetInformation();
        if (!ti.valid)
            return;

        const QStringList &config = node->variableValue(Variable::Config);

        QString destDir = ti.destDir.toString();
        QString workingDir;
        if (!destDir.isEmpty()) {
            bool workingDirIsBaseDir = false;
            if (destDir == ti.buildTarget)
                workingDirIsBaseDir = true;
            if (QDir::isRelativePath(destDir))
                destDir = QDir::cleanPath(ti.buildDir.toString() + '/' + destDir);

            if (workingDirIsBaseDir)
                workingDir = ti.buildDir.toString();
            else
                workingDir = destDir;
        } else {
            workingDir = ti.buildDir.toString();
        }

        if (HostOsInfo::isMacHost() && config.contains("app_bundle"))
            workingDir += '/' + ti.target + ".app/Contents/MacOS";

        BuildTargetInfo bti;
        bti.targetFilePath = FilePath::fromString(executableFor(node->proFile()));
        bti.projectFilePath = node->filePath();
        bti.workingDirectory = FilePath::fromString(workingDir);
        bti.displayName = bti.projectFilePath.toFileInfo().completeBaseName();
        const FilePath relativePathInProject
                = bti.projectFilePath.relativeChildPath(projectDirectory());
        if (!relativePathInProject.isEmpty()) {
            bti.displayNameUniquifier = QString::fromLatin1(" (%1)")
                    .arg(relativePathInProject.toUserOutput());
        }
        bti.buildKey = bti.projectFilePath.toString();
        bti.isQtcRunnable = config.contains("qtc_runnable");

        if (config.contains("console") && !config.contains("testcase")) {
            const QStringList qt = node->variableValue(Variable::Qt);
            bti.usesTerminal = !qt.contains("testlib") && !qt.contains("qmltest");
        }

        QStringList libraryPaths;

        // The user could be linking to a library found via a -L/some/dir switch
        // to find those libraries while actually running we explicitly prepend those
        // dirs to the library search path
        const QStringList libDirectories = node->variableValue(Variable::LibDirectories);
        if (!libDirectories.isEmpty()) {
            QmakeProFile *proFile = node->proFile();
            QTC_ASSERT(proFile, return);
            const QString proDirectory = buildDir(proFile->filePath()).toString();
            foreach (QString dir, libDirectories) {
                // Fix up relative entries like "LIBS+=-L.."
                const QFileInfo fi(dir);
                if (!fi.isAbsolute())
                    dir = QDir::cleanPath(proDirectory + '/' + dir);
                libraryPaths.append(dir);
            }
        }

        bti.runEnvModifierHash = qHash(libraryPaths);
        bti.runEnvModifier = [libraryPaths](Environment &env, bool useLibrarySearchPath) {
            if (useLibrarySearchPath)
                env.prependOrSetLibrarySearchPaths(libraryPaths);
        };

        appTargetList.append(bti);
    });

    setApplicationTargets(appTargetList);
}

void QmakeBuildSystem::collectData(const QmakeProFile *file, DeploymentData &deploymentData)
{
    if (!file->isSubProjectDeployable(file->filePath()))
        return;

    const InstallsList &installsList = file->installsList();
    for (const InstallsItem &item : installsList.items) {
        if (!item.active)
            continue;
        for (const auto &localFile : item.files) {
            deploymentData.addFile(localFile.fileName, item.path, item.executable
                                   ? DeployableFile::TypeExecutable : DeployableFile::TypeNormal);
        }
    }

    switch (file->projectType()) {
    case ProjectType::ApplicationTemplate:
        if (!installsList.targetPath.isEmpty())
            collectApplicationData(file, deploymentData);
        break;
    case ProjectType::SharedLibraryTemplate:
    case ProjectType::StaticLibraryTemplate:
        break;
    case ProjectType::SubDirsTemplate:
        for (const QmakePriFile *const subPriFile : file->subPriFilesExact()) {
            auto subProFile = dynamic_cast<const QmakeProFile *>(subPriFile);
            if (subProFile)
                collectData(subProFile, deploymentData);
        }
        break;
    default:
        break;
    }
}

void QmakeBuildSystem::collectApplicationData(const QmakeProFile *file, DeploymentData &deploymentData)
{
    QString executable = executableFor(file);
    if (!executable.isEmpty())
        deploymentData.addFile(executable, file->installsList().targetPath,
                               DeployableFile::TypeExecutable);
}

static FilePath destDirFor(const TargetInformation &ti)
{
    if (ti.destDir.isEmpty())
        return ti.buildDir;
    if (QDir::isRelativePath(ti.destDir.toString()))
        return FilePath::fromString(QDir::cleanPath(ti.buildDir.toString() + '/' + ti.destDir.toString()));
    return ti.destDir;
}

QString QmakeBuildSystem::executableFor(const QmakeProFile *file)
{
    TargetInformation ti = file->targetInformation();
    QString target;

    QTC_ASSERT(file, return QString());

    const QString extension = file->singleVariableValue(Variable::TargetExt);
    target = ti.target + extension;
    return QDir(destDirFor(ti).toString()).absoluteFilePath(target);
}

QmakeBuildSystem::AsyncUpdateState QmakeBuildSystem::asyncUpdateState() const
{
    return m_asyncUpdateState;
}

QmakeProFile *QmakeBuildSystem::rootProFile() const
{
    return m_rootProFile.get();
}

void QmakeBuildSystem::triggerParsing()
{
    asyncUpdate();
}

QStringList QmakeBuildSystem::filesGeneratedFrom(const QString &input) const
{
    if (!project()->rootProjectNode())
        return {};

    if (const FileNode *file = fileNodeOf(project()->rootProjectNode(), FilePath::fromString(input))) {
        const QmakeProFileNode *pro = dynamic_cast<QmakeProFileNode *>(file->parentFolderNode());
        QTC_ASSERT(pro, return {});
        if (const QmakeProFile *proFile = pro->proFile())
            return Utils::transform(proFile->generatedFiles(buildDir(pro->filePath()),
                                                            file->filePath(), file->fileType()),
                                    &FilePath::toString);
    }
    return {};
}

void QmakeBuildSystem::buildHelper(Action action, bool isFileBuild, QmakePriFileNode *prifile,
                                   FileNode *buildableFile)
{
    if(m_isBuilding)
        return;
    m_isBuilding = true;
    auto bc = qmakeBuildConfiguration();

    if (!buildableFile)
        isFileBuild = false;

    if (prifile) {
        if (prifile != project()->rootProjectNode())
            bc->setSubNodeBuild(prifile->proFileNode());
    }

    if (isFileBuild)
        bc->setFileNodeBuild(buildableFile);
    if (ProjectExplorerPlugin::saveModifiedFiles()) {
#if 1
        //modify by haojie.fang
        QEventLoop eventloop;
        QFutureWatcher<void> fwatcher;
        connect(&fwatcher, &QFutureWatcher<void>::finished, [&](){
            bc->setSubNodeBuild(nullptr);
            bc->setFileNodeBuild(nullptr);
            eventloop.exit();
        });

        QFuture<void> f = QtConcurrent::run(this, &QmakeBuildSystem::buildEmosCode, action, isFileBuild, prifile, buildableFile);
        fwatcher.setFuture(f);

        eventloop.exec();
#else
        if (action == BUILD)
            BuildManager::buildList(bc->buildSteps());
        else if (action == CLEAN)
            BuildManager::buildList(bc->cleanSteps());
        else if (action == REBUILD)
            BuildManager::buildLists({bc->cleanSteps(), bc->buildSteps()});
#endif
    }

//    bc->setSubNodeBuild(nullptr);
//    bc->setFileNodeBuild(nullptr);

    m_isBuilding = false;
}

} // QmakeProjectManager

#include "qmakeproject.moc"
