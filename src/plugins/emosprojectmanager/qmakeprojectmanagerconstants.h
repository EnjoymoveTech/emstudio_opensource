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

#include <QtGlobal>

namespace QmakeProjectManager {
namespace Constants {

const char InterfaceDesign[] = "1_interface_design";
const char ComponentDesign[] =  "2_component_design";
const char ArchitectureDesign[] =  "3_architecture_design";
const char Config[] =  "config";
const char ScheduleConfiguration[] =  "schedule_configuration";

// Menus
const char M_CONTEXT[] = "ProFileEditor.ContextMenu";

// Kinds
const char PROFILE_EDITOR_ID[] = "EMOS.proFileEditor";
const char PROFILE_EDITOR_DISPLAY_NAME[] = QT_TRANSLATE_NOOP("OpenWith::Editors", ".emos File Editor");
const char PROFILE_MIMETYPE[] = "application/emos.profile";
const char PROINCLUDEFILE_MIMETYPE [] = "application/emos.includefile";

// Actions
const char BUILDPROJECTCONTEXTMENU[] = "EMOSBuilder.BuildProjectContextMenu";
const char CLEARPROJECTCONTEXTMENU[] = "EMOSBuilder.ClearProjectContextMenu";
const char BUILDSUBDIR[] = "EMOSBuilder.BuildSubDir";
const char BUILDSUBDIRDEV[] = "EMOSBuilder.BuildSubDirDev";
const char CLEANSUBDIR[] = "EMOSBuilder.ClearSubDir";
const char BUILDFILE[] = "EMOSBuilder.BuildFile";
const char BUILDSUBDIRCONTEXTMENU[] = "EMOSBuilder.BuildSubDirContextMenu";
const char REBUILDSUBDIRCONTEXTMENU[] = "EMOSBuilder.RebuildSubDirContextMenu";
const char BUILDFILECONTEXTMENU[] = "EMOSBuilder.BuildFileContextMenu";
const char SELECTBUILDFILECONTEXTMENU[] = "EMOSBuilder.SelectBuildFileContextMenu";
const char BUILDXMLCONTEXTMENU[] = "EMOSBuilder.BuildXmlContextMenu";
const char MERGEPROJECT[] = "EMOSBuilder.MergeProject";
const char IMPORTEXCELPROJECT[] = "EMOSBuilder.ImportExcelProject";

// Tasks
const char PROFILE_EVALUATE[] = "EMOSProjectManager.ProFileEvaluate";
const char QMAKE_BC_ID[] = "EMOSProjectManager.Qt4BuildConfiguration";
// Project
const char QMAKEPROJECT_ID[] = "EMOSProjectManager.Project";

} // namespace Constants
} // namespace QmakeProjectManager
