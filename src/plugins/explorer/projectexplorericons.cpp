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

#include "projectexplorericons.h"

using namespace Utils;

namespace ProjectExplorer {
namespace Icons {

const Icon BUILD(":/explorer/images/build.png");
const Icon BUILD_FLAT({
        {":/explorer/images/build_hammerhandle_mask.png", Theme::IconsBuildHammerHandleColor},
        {":/explorer/images/build_hammerhead_mask.png", Theme::IconsBuildHammerHeadColor}});
const Icon BUILD_SMALL({
        {":/explorer/images/buildhammerhandle.png", Theme::IconsBuildHammerHandleColor},
        {":/explorer/images/buildhammerhead.png", Theme::IconsBuildHammerHeadColor}}, Icon::Tint);
const Icon CANCELBUILD_FLAT({
        {":/explorer/images/build_hammerhandle_mask.png", Theme::IconsDisabledColor},
        {":/explorer/images/build_hammerhead_mask.png", Theme::IconsDisabledColor},
        {":/explorer/images/cancelbuild_overlay.png", Theme::IconsStopToolBarColor}},
        Icon::Tint | Icon::PunchEdges);
const Icon REBUILD({
        {":/explorer/images/rebuildhammerhandles.png", Theme::IconsBuildHammerHandleColor},
        {":/explorer/images/buildhammerhandle.png", Theme::IconsBuildHammerHandleColor},
        {":/explorer/images/rebuildhammerheads.png", Theme::IconsBuildHammerHeadColor},
        {":/explorer/images/buildhammerhead.png", Theme::IconsBuildHammerHeadColor}}, Icon::Tint);
const Icon RUN(":/explorer/images/run.png");
const Icon RUN_FLAT({
        {":/explorer/images/run_mask.png", Theme::IconsRunToolBarColor}});
const Icon WINDOW(":/explorer/images/window.png");
const Icon DEBUG_START(":/explorer/images/debugger_start.png");
const Icon DEVICE_READY_INDICATOR({
        {":/utils/images/filledcircle.png", Theme::IconsRunColor}}, Icon::Tint);
const Icon DEVICE_READY_INDICATOR_OVERLAY({
        {":/explorer/images/devicestatusindicator.png", Theme::IconsRunToolBarColor}});
const Icon DEVICE_CONNECTED_INDICATOR({
        {":/utils/images/filledcircle.png", Theme::IconsWarningColor}}, Icon::Tint);
const Icon DEVICE_CONNECTED_INDICATOR_OVERLAY({
        {":/explorer/images/devicestatusindicator.png", Theme::IconsWarningToolBarColor}});
const Icon DEVICE_DISCONNECTED_INDICATOR({
        {":/utils/images/filledcircle.png", Theme::IconsStopColor}}, Icon::Tint);
const Icon DEVICE_DISCONNECTED_INDICATOR_OVERLAY({
        {":/explorer/images/devicestatusindicator.png", Theme::IconsStopToolBarColor}});
const Icon WIZARD_IMPORT_AS_PROJECT(
        ":/explorer/images/importasproject.png");

const Icon DEBUG_START_FLAT({
        {":/explorer/images/run_mask.png", Theme::IconsRunToolBarColor},
        {":/explorer/images/debugger_beetle_mask.png", Theme::IconsDebugColor}});
const Icon DEBUG_START_SMALL({
        {":/utils/images/run_small.png", Theme::IconsRunColor},
        {":/explorer/images/debugger_overlay_small.png", Theme::PanelTextColorMid}}, Icon::MenuTintedStyle);
const Icon DEBUG_START_SMALL_TOOLBAR({
        {":/utils/images/run_small.png", Theme::IconsRunToolBarColor},
        {":/explorer/images/debugger_overlay_small.png", Theme::IconsDebugColor}});
const Icon ANALYZER_START_SMALL({
        {":/utils/images/run_small.png", Theme::IconsRunColor},
        {":/explorer/images/analyzer_overlay_small.png", Theme::PanelTextColorMid}}, Icon::MenuTintedStyle);
const Icon ANALYZER_START_SMALL_TOOLBAR({
        {":/utils/images/run_small.png", Theme::IconsRunToolBarColor},
        {":/explorer/images/analyzer_overlay_small.png", Theme::IconsBaseColor}});

const Icon BUILDSTEP_MOVEUP({
        {":/explorer/images/buildstepmoveup.png", Theme::PanelTextColorDark}}, Icon::Tint);
const Icon BUILDSTEP_MOVEDOWN({
        {":/explorer/images/buildstepmovedown.png", Theme::PanelTextColorDark}}, Icon::Tint);
const Icon BUILDSTEP_DISABLE({
        {":/explorer/images/buildstepdisable.png", Theme::PanelTextColorDark}}, Icon::Tint);
const Icon BUILDSTEP_REMOVE({
        {":/explorer/images/buildstepremove.png", Theme::PanelTextColorDark}}, Icon::Tint);

const Icon DESKTOP_DEVICE({
        {":/explorer/images/desktopdevice.png", Theme::IconsBaseColor}});
const Icon DESKTOP_DEVICE_SMALL({
        {":/utils/images/desktopdevicesmall.png", Theme::PanelTextColorDark}}, Icon::Tint);

const Icon MODE_PROJECT_CLASSIC(":/explorer/images/mode_project.png");
const Icon MODE_PROJECT_FLAT({
        {":/explorer/images/mode_project_mask.png", Theme::IconsBaseColor}});
const Icon MODE_PROJECT_FLAT_ACTIVE({
        {":/explorer/images/mode_project_mask.png", Theme::IconsModeProjectActiveColor}});

} // namespace Icons
} // namespace ProjectExplorer
