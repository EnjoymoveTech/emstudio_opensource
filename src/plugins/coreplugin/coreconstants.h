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

namespace Core {
namespace Constants {

// Modes
const char MODE_WELCOME[]          = "Welcome";
const char MODE_EDIT[]             = "Edit";
const char MODE_DESIGN[]           = "Design";
const int  P_MODE_WELCOME          = 100;
const int  P_MODE_EDIT             = 90;
const int  P_MODE_DESIGN           = 89;

// TouchBar
const char TOUCH_BAR[]             = "EmStudio.TouchBar";

// Menubar
const char MENU_BAR[]              = "EmStudio.MenuBar";

// Menus
const char M_FILE[]                = "EmStudio.Menu.File";
const char M_FILE_RECENTFILES[]    = "EmStudio.Menu.File.RecentFiles";
const char M_EDIT[]                = "EmStudio.Menu.Edit";
const char M_EDIT_ADVANCED[]       = "EmStudio.Menu.Edit.Advanced";
const char M_VIEW[]                = "EmStudio.Menu.View";
const char M_VIEW_MODESTYLES[]     = "EmStudio.Menu.View.ModeStyles";
const char M_VIEW_VIEWS[]          = "EmStudio.Menu.View.Views";
const char M_VIEW_PANES[]          = "EmStudio.Menu.View.Panes";
const char M_TOOLS[]               = "EmStudio.Menu.Tools";
const char M_TOOLS_EXTERNAL[]      = "EmStudio.Menu.Tools.External";
const char M_WINDOW[]              = "EmStudio.Menu.Window";
const char M_HELP[]                = "EmStudio.Menu.Help";

// Contexts
const char C_GLOBAL[]              = "Global Context";
const char C_WELCOME_MODE[]        = "Core.WelcomeMode";
const char C_EDIT_MODE[]           = "Core.EditMode";
const char C_DESIGN_MODE[]         = "Core.DesignMode";
const char C_EDITORMANAGER[]       = "Core.EditorManager";
const char C_NAVIGATION_PANE[]     = "Core.NavigationPane";
const char C_PROBLEM_PANE[]        = "Core.ProblemPane";
const char C_GENERAL_OUTPUT_PANE[] = "Core.GeneralOutputPane";

// Default editor kind
const char K_DEFAULT_TEXT_EDITOR_DISPLAY_NAME[] = QT_TRANSLATE_NOOP("OpenWith::Editors", "Plain Text Editor");
const char K_DEFAULT_TEXT_EDITOR_ID[] = "Core.PlainTextEditor";
const char K_DEFAULT_BINARY_EDITOR_ID[] = "Core.BinaryEditor";

//actions
const char UNDO[]                  = "EmStudio.Undo";
const char REDO[]                  = "EmStudio.Redo";
const char COPY[]                  = "EmStudio.Copy";
const char PASTE[]                 = "EmStudio.Paste";
const char CUT[]                   = "EmStudio.Cut";
const char SELECTALL[]             = "EmStudio.SelectAll";

const char GOTO[]                  = "EmStudio.Goto";
const char ZOOM_IN[]               = "EmStudio.ZoomIn";
const char ZOOM_OUT[]              = "EmStudio.ZoomOut";
const char ZOOM_RESET[]            = "EmStudio.ZoomReset";

const char NEW[]                   = "EmStudio.New";
const char OPEN[]                  = "EmStudio.Open";
const char OPEN_WITH[]             = "EmStudio.OpenWith";
const char REVERTTOSAVED[]         = "EmStudio.RevertToSaved";
const char SAVE[]                  = "EmStudio.Save";
const char SAVEAS[]                = "EmStudio.SaveAs";
const char SAVEALL[]               = "EmStudio.SaveAll";
const char PRINT[]                 = "EmStudio.Print";
const char EXIT[]                  = "EmStudio.Exit";

const char OPTIONS[]               = "EmStudio.Options";
const char TOGGLE_LEFT_SIDEBAR[]   = "EmStudio.ToggleLeftSidebar";
const char TOGGLE_RIGHT_SIDEBAR[]  = "EmStudio.ToggleRightSidebar";
const char CYCLE_MODE_SELECTOR_STYLE[] =
                                     "EmStudio.CycleModeSelectorStyle";
const char TOGGLE_FULLSCREEN[]     = "EmStudio.ToggleFullScreen";
const char THEMEOPTIONS[]          = "EmStudio.ThemeOptions";

const char TR_SHOW_LEFT_SIDEBAR[]  = QT_TRANSLATE_NOOP("Core", "Show Left Sidebar");
const char TR_HIDE_LEFT_SIDEBAR[]  = QT_TRANSLATE_NOOP("Core", "Hide Left Sidebar");

const char TR_SHOW_RIGHT_SIDEBAR[] = QT_TRANSLATE_NOOP("Core", "Show Right Sidebar");
const char TR_HIDE_RIGHT_SIDEBAR[] = QT_TRANSLATE_NOOP("Core", "Hide Right Sidebar");

const char MINIMIZE_WINDOW[]       = "EmStudio.MinimizeWindow";
const char ZOOM_WINDOW[]           = "EmStudio.ZoomWindow";
const char CLOSE_WINDOW[]           = "EmStudio.CloseWindow";

const char SPLIT[]                 = "EmStudio.Split";
const char SPLIT_SIDE_BY_SIDE[]    = "EmStudio.SplitSideBySide";
const char SPLIT_NEW_WINDOW[]      = "EmStudio.SplitNewWindow";
const char REMOVE_CURRENT_SPLIT[]  = "EmStudio.RemoveCurrentSplit";
const char REMOVE_ALL_SPLITS[]     = "EmStudio.RemoveAllSplits";
const char GOTO_PREV_SPLIT[]       = "EmStudio.GoToPreviousSplit";
const char GOTO_NEXT_SPLIT[]       = "EmStudio.GoToNextSplit";
const char CLOSE[]                 = "EmStudio.Close";
const char CLOSE_ALTERNATIVE[]     = "EmStudio.Close_Alternative"; // temporary, see EMSTUDIOBUG-72
const char CLOSEALL[]              = "EmStudio.CloseAll";
const char CLOSEOTHERS[]           = "EmStudio.CloseOthers";
const char CLOSEALLEXCEPTVISIBLE[] = "EmStudio.CloseAllExceptVisible";
const char GOTONEXTINHISTORY[]     = "EmStudio.GotoNextInHistory";
const char GOTOPREVINHISTORY[]     = "EmStudio.GotoPreviousInHistory";
const char GO_BACK[]               = "EmStudio.GoBack";
const char GO_FORWARD[]            = "EmStudio.GoForward";
const char GOTOLASTEDIT[]          = "EmStudio.GotoLastEdit";
const char ABOUT_EMSTUDIO[]       = "EmStudio.AboutEmStudio";
const char ABOUT_PLUGINS[]         = "EmStudio.AboutPlugins";
const char S_RETURNTOEDITOR[]      = "EmStudio.ReturnToEditor";

// Default groups
const char G_DEFAULT_ONE[]         = "EmStudio.Group.Default.One";
const char G_DEFAULT_TWO[]         = "EmStudio.Group.Default.Two";
const char G_DEFAULT_THREE[]       = "EmStudio.Group.Default.Three";

// Main menu bar groups
const char G_FILE[]                = "EmStudio.Group.File";
const char G_EDIT[]                = "EmStudio.Group.Edit";
const char G_VIEW[]                = "EmStudio.Group.View";
const char G_TOOLS[]               = "EmStudio.Group.Tools";
const char G_WINDOW[]              = "EmStudio.Group.Window";
const char G_HELP[]                = "EmStudio.Group.Help";

// File menu groups
const char G_FILE_NEW[]            = "EmStudio.Group.File.New";
const char G_FILE_OPEN[]           = "EmStudio.Group.File.Open";
const char G_FILE_PROJECT[]        = "EmStudio.Group.File.Project";
const char G_FILE_SAVE[]           = "EmStudio.Group.File.Save";
const char G_FILE_EXPORT[]         = "EmStudio.Group.File.Export";
const char G_FILE_CLOSE[]          = "EmStudio.Group.File.Close";
const char G_FILE_PRINT[]          = "EmStudio.Group.File.Print";
const char G_FILE_OTHER[]          = "EmStudio.Group.File.Other";

// Edit menu groups
const char G_EDIT_UNDOREDO[]       = "EmStudio.Group.Edit.UndoRedo";
const char G_EDIT_COPYPASTE[]      = "EmStudio.Group.Edit.CopyPaste";
const char G_EDIT_SELECTALL[]      = "EmStudio.Group.Edit.SelectAll";
const char G_EDIT_ADVANCED[]       = "EmStudio.Group.Edit.Advanced";

const char G_EDIT_FIND[]           = "EmStudio.Group.Edit.Find";
const char G_EDIT_OTHER[]          = "EmStudio.Group.Edit.Other";

// Advanced edit menu groups
const char G_EDIT_FORMAT[]         = "EmStudio.Group.Edit.Format";
const char G_EDIT_COLLAPSING[]     = "EmStudio.Group.Edit.Collapsing";
const char G_EDIT_TEXT[]           = "EmStudio.Group.Edit.Text";
const char G_EDIT_BLOCKS[]         = "EmStudio.Group.Edit.Blocks";
const char G_EDIT_FONT[]           = "EmStudio.Group.Edit.Font";
const char G_EDIT_EDITOR[]         = "EmStudio.Group.Edit.Editor";

// View menu groups
const char G_VIEW_VIEWS[]          = "EmStudio.Group.View.Views";
const char G_VIEW_PANES[]          = "EmStudio.Group.View.Panes";

// Tools menu groups
const char G_TOOLS_OPTIONS[]       = "EmStudio.Group.Tools.Options";

// Window menu groups
const char G_WINDOW_SIZE[]         = "EmStudio.Group.Window.Size";
const char G_WINDOW_SPLIT[]        = "EmStudio.Group.Window.Split";
const char G_WINDOW_NAVIGATE[]     = "EmStudio.Group.Window.Navigate";
const char G_WINDOW_LIST[]         = "EmStudio.Group.Window.List";
const char G_WINDOW_OTHER[]        = "EmStudio.Group.Window.Other";

// Help groups (global)
const char G_HELP_HELP[]           = "EmStudio.Group.Help.Help";
const char G_HELP_SUPPORT[]        = "EmStudio.Group.Help.Supprt";
const char G_HELP_ABOUT[]          = "EmStudio.Group.Help.About";
const char G_HELP_UPDATES[]        = "EmStudio.Group.Help.Updates";

// Touchbar groups
const char G_TOUCHBAR_HELP[]       = "EmStudio.Group.TouchBar.Help";
const char G_TOUCHBAR_EDITOR[]     = "EmStudio.Group.TouchBar.Editor";
const char G_TOUCHBAR_NAVIGATION[] = "EmStudio.Group.TouchBar.Navigation";
const char G_TOUCHBAR_OTHER[]      = "EmStudio.Group.TouchBar.Other";

const char WIZARD_CATEGORY_QT[] = "R.Qt";
const char WIZARD_TR_CATEGORY_QT[] = QT_TRANSLATE_NOOP("Core", "Qt");
const char WIZARD_KIND_UNKNOWN[] = "unknown";
const char WIZARD_KIND_PROJECT[] = "project";
const char WIZARD_KIND_FILE[] = "file";

const char SETTINGS_CATEGORY_CORE[] = "B.Core";
const char SETTINGS_ID_INTERFACE[] = "A.Interface";
const char SETTINGS_ID_SYSTEM[] = "B.Core.System";
const char SETTINGS_ID_SHORTCUTS[] = "C.Keyboard";
const char SETTINGS_ID_TOOLS[] = "D.ExternalTools";
const char SETTINGS_ID_MIMETYPES[] = "E.MimeTypes";

const char SETTINGS_DEFAULTTEXTENCODING[] = "General/DefaultFileEncoding";
const char SETTINGS_DEFAULT_LINE_TERMINATOR[] = "General/DefaultLineTerminator";

const char SETTINGS_THEME[] = "Core/CreatorTheme";
const char DEFAULT_THEME[] = "flat-light";

const char TR_CLEAR_MENU[]         = QT_TRANSLATE_NOOP("Core", "Clear Menu");

const int MODEBAR_ICON_SIZE = 34;
const int MODEBAR_ICONSONLY_BUTTON_SIZE = MODEBAR_ICON_SIZE + 4;
const int DEFAULT_MAX_CHAR_COUNT = 10000000;

} // namespace Constants
} // namespace Core
