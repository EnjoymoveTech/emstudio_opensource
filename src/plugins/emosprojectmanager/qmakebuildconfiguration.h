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
#include <explorer/buildconfiguration.h>


namespace ProjectExplorer { class FileNode; }

namespace QmakeProjectManager {

class QmakeBuildSystem;
class QmakeProFileNode;

class EMOSPROJECTMANAGER_EXPORT QmakeBuildConfiguration : public ProjectExplorer::BuildConfiguration
{
    Q_OBJECT

public:
    QmakeBuildConfiguration(ProjectExplorer::Target *target, Utils::Id id);
    ~QmakeBuildConfiguration() override;

    ProjectExplorer::BuildSystem *buildSystem() const final;

    void setSubNodeBuild(QmakeProFileNode *node);
    QmakeProFileNode *subNodeBuild() const;

    ProjectExplorer::FileNode *fileNodeBuild() const;
    void setFileNodeBuild(ProjectExplorer::FileNode *node);

    QmakeBuildSystem *qmakeBuildSystem() const;

protected:

private:
    void kitChanged();

    QmakeProFileNode *m_subNodeBuild = nullptr;
    ProjectExplorer::FileNode *m_fileNodeBuild = nullptr;
    QmakeBuildSystem *m_buildSystem = nullptr;
};

class QmakeBuildConfigurationFactory : public ProjectExplorer::BuildConfigurationFactory
{
public:
    QmakeBuildConfigurationFactory();
};

} // namespace QmakeProjectManager
