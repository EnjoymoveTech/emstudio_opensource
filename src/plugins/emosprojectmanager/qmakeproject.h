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

#pragma once

#include "qmakeprojectmanager_global.h"

#include "qmakenodes.h"
#include "qmakeparsernodes.h"

#include <explorer/deploymentdata.h>
#include <explorer/project.h>
#include <explorer/toolchain.h>

#include <QStringList>
#include <QFutureInterface>

QT_BEGIN_NAMESPACE
class QMakeGlobals;
QT_END_NAMESPACE

//namespace CppTools { class CppProjectUpdater; }
namespace ProjectExplorer { class DeploymentData; }

namespace QmakeProjectManager {
class ProFileReader;
class QmakeBuildConfiguration;

namespace Internal { class CentralizedFolderWatcher; }

class  EMOSPROJECTMANAGER_EXPORT QmakeProject final : public ProjectExplorer::Project
{
    Q_OBJECT

public:
    explicit QmakeProject(const Utils::FilePath &proFile);
    ~QmakeProject() final;

    void configureAsExampleProject(ProjectExplorer::Kit *kit) final;
protected:
    RestoreResult fromMap(const QVariantMap &map, QString *errorMessage) final;

private:
    ProjectExplorer::DeploymentKnowledge deploymentKnowledge() const override;

};

// FIXME: This export here is only there to appease the current version
// of the appman plugin. This _will_ go away, one way or the other.
class EMOSPROJECTMANAGER_EXPORT QmakeBuildSystem : public ProjectExplorer::BuildSystem
{
    Q_OBJECT

public:
    explicit QmakeBuildSystem(QmakeBuildConfiguration *bc);
    ~QmakeBuildSystem();

    bool supportsAction(ProjectExplorer::Node *context,
                        ProjectExplorer::ProjectAction action,
                        const ProjectExplorer::Node *node) const override;

    bool addFiles(ProjectExplorer::Node *context,
                  const QStringList &filePaths,
                  QStringList *notAdded = nullptr) override;
    ProjectExplorer::RemovedFilesFromProject removeFiles(ProjectExplorer::Node *context,
                                                         const QStringList &filePaths,
                                                         QStringList *notRemoved = nullptr) override;
    bool deleteFiles(ProjectExplorer::Node *context,
                     const QStringList &filePaths) override;
    bool canRenameFile(ProjectExplorer::Node *context,
                       const QString &filePath, const QString &newFilePath) override;
    bool renameFile(ProjectExplorer::Node *context,
                    const QString &filePath, const QString &newFilePath) override;
    void triggerParsing() final;

    QStringList filesGeneratedFrom(const QString &file) const final;

    void asyncUpdate();
    void buildFinished(bool success);
    void activeTargetWasChanged(ProjectExplorer::Target *);

    QString executableFor(const QmakeProFile *file);

    static bool equalFileList(const QStringList &a, const QStringList &b);

    void updateBuildSystemData();
    void collectData(const QmakeProFile *file, ProjectExplorer::DeploymentData &deploymentData);
    void collectApplicationData(const QmakeProFile *file,
                                ProjectExplorer::DeploymentData &deploymentData);
    void startAsyncTimer(QmakeProFile::AsyncUpdateDelay delay);

    /// \internal
    ProFileReader *createProFileReader(const QmakeProFile *qmakeProFile);
    /// \internal
    QMakeGlobals *qmakeGlobals();
    /// \internal
    QMakeVfs *qmakeVfs();
    /// \internal
    QString qmakeSysroot();
    /// \internal
    void destroyProFileReader(ProFileReader *reader);
    void deregisterFromCacheManager();

    /// \internal
    void scheduleAsyncUpdateFile(QmakeProFile *file,
                                 QmakeProFile::AsyncUpdateDelay delay = QmakeProFile::ParseLater);
    /// \internal
    void incrementPendingEvaluateFutures();
    /// \internal
    void decrementPendingEvaluateFutures();
    /// \internal
    bool wasEvaluateCanceled();

    void updateCodeModels();
    void updateDocuments();

    void watchFolders(const QStringList &l, QmakePriFile *file);
    void unwatchFolders(const QStringList &l, QmakePriFile *file);

    static void proFileParseError(const QString &errorMessage, const Utils::FilePath &filePath);

    enum AsyncUpdateState { Base, AsyncFullUpdatePending, AsyncPartialUpdatePending, AsyncUpdateInProgress, ShuttingDown };
    AsyncUpdateState asyncUpdateState() const;

    QmakeProFile *rootProFile() const;

    void notifyChanged(const Utils::FilePath &name);

    enum Action { PROJECTBUILD, BUILD, SELECTBUILD, BUILDXML, REBUILD, CLEAN };
    void buildHelper(Action action, bool isFileBuild,
                     QmakePriFileNode *prifile,
                     ProjectExplorer::FileNode *buildableFile);

    Utils::FilePath buildDir(const Utils::FilePath &proFilePath) const;
    QmakeBuildConfiguration *qmakeBuildConfiguration() const;

    void scheduleUpdateAllNowOrLater();

private:
    //add by haojie.fang
    void buildEmosCode(Action action, bool isFileBuild, QmakePriFileNode *prifile,
                       ProjectExplorer::FileNode *buildableFile);

    void scheduleUpdateAll(QmakeProFile::AsyncUpdateDelay delay);

    void scheduleUpdateAllLater() { scheduleUpdateAll(QmakeProFile::ParseLater); }
    mutable QSet<const QPair<Utils::FilePath, Utils::FilePath>> m_toolChainWarnings;

    // Current configuration
    QString m_oldQtIncludePath;
    QString m_oldQtLibsPath;

    std::unique_ptr<QmakeProFile> m_rootProFile;

    QMakeVfs *m_qmakeVfs = nullptr;

    // cached data during project rescan
    std::unique_ptr<QMakeGlobals> m_qmakeGlobals;
    int m_qmakeGlobalsRefCnt = 0;
    bool m_invalidateQmakeVfsContents = false;

    QString m_qmakeSysroot;

    int m_pendingEvaluateFuturesCount = 0;
    AsyncUpdateState m_asyncUpdateState = Base;
    bool m_cancelEvaluate = false;
    QList<QmakeProFile *> m_partialEvaluate;

    //CppTools::CppProjectUpdater *m_cppCodeModelUpdater = nullptr;

    Internal::CentralizedFolderWatcher *m_centralizedFolderWatcher = nullptr;

    ProjectExplorer::BuildSystem::ParseGuard m_guard;
    bool m_firstParseNeeded = true;

    bool m_isBuilding = false;
};

} // namespace QmakeProjectManager
