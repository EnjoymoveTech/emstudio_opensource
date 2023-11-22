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

#include "qmakebuildconfiguration.h"

#include "qmakeproject.h"
#include "qmakeprojectmanagerconstants.h"
#include "qmakenodes.h"
#include "qmakebuildconfiguration.h"

#include <coreplugin/documentmanager.h>
#include <coreplugin/icore.h>

#include <explorer/buildaspects.h>
#include <explorer/buildinfo.h>
#include <explorer/buildmanager.h>
#include <explorer/buildpropertiessettings.h>
#include <explorer/buildsteplist.h>
#include <explorer/kit.h>
#include <explorer/explorer.h>
#include <explorer/projectexplorerconstants.h>
#include <explorer/projectmacroexpander.h>
#include <explorer/target.h>
#include <explorer/toolchain.h>

#include <utils/mimetypes/mimedatabase.h>
#include <utils/qtcassert.h>
#include <utils/qtcprocess.h>

#include <QDebug>
#include <QInputDialog>
#include <QLoggingCategory>

#include <limits>

using namespace ProjectExplorer;
using namespace Utils;
using namespace QmakeProjectManager::Internal;

namespace QmakeProjectManager {

QmakeBuildConfiguration::QmakeBuildConfiguration(Target *target, Utils::Id id)
    : BuildConfiguration(target, id)
{
    setConfigWidgetDisplayName(tr("General"));
    setConfigWidgetHasFrame(true);

    m_buildSystem = new QmakeBuildSystem(this);
    connect(target, &Target::kitChanged,
            this, &QmakeBuildConfiguration::kitChanged);
}

QmakeBuildConfiguration::~QmakeBuildConfiguration()
{
    delete m_buildSystem;
}

void QmakeBuildConfiguration::kitChanged()
{
    m_buildSystem->scheduleUpdateAllNowOrLater();
}

BuildSystem *QmakeBuildConfiguration::buildSystem() const
{
    return m_buildSystem;
}

QmakeProFileNode *QmakeBuildConfiguration::subNodeBuild() const
{
    return m_subNodeBuild;
}

void QmakeBuildConfiguration::setSubNodeBuild(QmakeProFileNode *node)
{
    m_subNodeBuild = node;
}

FileNode *QmakeBuildConfiguration::fileNodeBuild() const
{
    return m_fileNodeBuild;
}

void QmakeBuildConfiguration::setFileNodeBuild(FileNode *node)
{
    m_fileNodeBuild = node;
}

QmakeBuildSystem *QmakeBuildConfiguration::qmakeBuildSystem() const
{
    return m_buildSystem;
}

// Returns true if both are equal.

/*!
  \class QmakeBuildConfigurationFactory
*/

QmakeBuildConfigurationFactory::QmakeBuildConfigurationFactory()
{
    registerBuildConfiguration<QmakeBuildConfiguration>(Constants::QMAKE_BC_ID);
    setSupportedProjectType(Constants::QMAKEPROJECT_ID);
    setSupportedProjectMimeTypeName(Constants::PROFILE_MIMETYPE);

    setBuildGenerator([](const Kit *k, const FilePath &projectPath, bool forSetup) {
        QList<BuildInfo> result;
        BuildInfo info;
        result << info;
        return result;
    });
}

} // namespace QmakeProjectManager

//#include "qmakebuildconfiguration.moc"
