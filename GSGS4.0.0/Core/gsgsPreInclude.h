/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/

#ifndef gsgs_PreInclude_H
#define gsgs_PreInclude_H

#include <wx/version.h>
#include <wx/log.h>
#include <wx/filename.h>
#include <wx/ffile.h>
#include <wx/sharedptr.h>
#include <wx/timer.h>
#include <wx/event.h>
#include <wx/colour.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include <wx/utils.h>
#include <wx/tokenzr.h>
#include <wx/strconv.h>
#include <wx/vector.h>
#include <wx/window.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/wupdlock.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <stdio.h>
#include <set>
#include <map>
#include <list>
#include <queue>
#include <deque>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <atomic>

#include <wx/hashmap.h>
#include <wx_ordered_map.h>
#include "gsgsSharedPtr.h"

typedef std::unordered_map<wxString, wxString>  gsgsStringMap;
typedef std::unordered_set<wxString>            gsgsStringSet;

#ifdef __WXMSW__
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifdef _gsgsAPI
#undef _gsgsAPI
#endif

#endif

#ifdef WXMAKINGDLL_CL
    #define _gsgsAPI __declspec(dllexport)
#elif defined(WXUSINGDLL_CL)
    #define _gsgsAPI __declspec(dllimport)
#else
    #define _gsgsAPI
#endif

#include "gsgsPathManager.h"
#include "gsgsStringUtil.h"

namespace gsgs
{
    class Thread;
    class Builder;
    class BuildConfig;
    class BuildConfigCommon;
    class Compiler;
    class EditorConfig;
    class JobQueue;
    class ObjectData;

    class TreeCtrlModel;
    class TreeCtrl;
    class ToolBar;
    class DefaultTreeCtrl;
    class ThemedListCtrl;
    class QuickFindBar;
    class RowEntry;
    class ItemPanel;
    class EditorBar;
    class ScrollBar;
    class ScrolledWindow;
    class ToolBarButton;
    class Config;
    class BuildSettingsConfig;
    class EditorScrolledPanel;
    class CmdPrc;
    class TagsManager;
    class TagEntry;
    class SearchThread;
    class OptionsConfig;

    class BuildManager;
    class NavMgr;
    class CxxWS;
    class Project;
    class Plugin;
    class IEditor;
    class ConfigTool;
    class IManager;
    class IDebuggerObserver;
    class IDebugger;
    class Serializer;

    typedef SharedPtr<Builder> BuilderPtr;
    typedef SharedPtr<BuildConfig> BuildConfigPtr;
    typedef SharedPtr<BuildConfigCommon> BuildConfigCommonPtr;
    typedef SharedPtr<Compiler> CompilerPtr;
    typedef std::vector<CompilerPtr> CompilerPtrVec_t;
    typedef std::vector<wxWindow*> WindowList;
    typedef std::vector<wxBitmap> BitmapList;
}
_gsgsAPI extern const wxBitmap gsgsEmptyBitmap;
_gsgsAPI extern const wxString gsgsEmptyString;

//-------------------------------------------------------
// wxWidgets Connect macros
//-------------------------------------------------------

#define ConnectChoice(ctrl, fn)             ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(fn), NULL, this);
#define UnConnectChoice(ctrl, fn)           ctrl->Disconnect(ctrl->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(fn), NULL, this);

#define ConnectButton(ctrl, fn)             ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(fn), NULL, this);
#define UnConnectButton(ctrl, fn)           ctrl->Disconnect(ctrl->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(fn), NULL, this);

#define ConnectClick(ctrl, fn)              ctrl->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(fn), NULL, this);
#define UnConnectClick(ctrl, fn)            ctrl->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(fn), NULL, this);

#define ConnectListBoxDClick(ctrl, fn)      ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(fn), NULL, this);
#define UnConnectListBoxDClick(ctrl, fn)    ctrl->Disconnect(ctrl->GetId(), wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(fn), NULL, this);

#define ConnectListDClick(ctrl, fn)         ctrl->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(fn), NULL, this);
#define UnConnectListDClick(ctrl, fn)       ctrl->Disconnect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(fn), NULL, this);

#define ConnectCheckBox(ctrl, fn)           ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(fn), NULL, this);
#define UnConnectCheckBox(ctrl, fn)         ctrl->Disconnect(ctrl->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(fn), NULL, this);

#define ConnectListCtrlItemSelected(ctrl, fn)       ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(fn), NULL, this);
#define UnConnectListCtrlItemSelected(ctrl, fn)     ctrl->Disconnect(ctrl->GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(fn), NULL, this);

#define ConnectListCtrlItemActivated(ctrl, fn)      ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(fn), NULL, this);
#define UnConnectListCtrlItemActivated(ctrl, fn)    ctrl->Disconnect(ctrl->GetId(), wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(fn), NULL, this);

#define ConnectKeyDown(ctrl, fn)            ctrl->Connect(ctrl->GetId(), wxEVT_KEY_DOWN, wxKeyEventHandler(fn), NULL, this);
#define UnConnectKeyDown(ctrl, fn)          ctrl->Disconnect(ctrl->GetId(), wxEVT_KEY_DOWN, wxKeyEventHandler(fn), NULL, this);

#define ConnectCharEvent(ctrl, fn)          ctrl->Connect(ctrl->GetId(), wxEVT_CHAR, wxKeyEventHandler(fn), NULL, this);
#define UnConnectCharEvent(ctrl, fn)        ctrl->Disconnect(ctrl->GetId(), wxEVT_CHAR, wxKeyEventHandler(fn), NULL, this);

#define ConnectCmdTextEntered(ctrl, fn)     ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(fn), NULL, this);
#define UnConnectCmdTextEntered(ctrl, fn)   ctrl->Disconnect(ctrl->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(fn), NULL, this);

#define ConnectCmdTextUpdated(ctrl, fn)     ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(fn), NULL, this);
#define UnConnectCmdTextUpdated(ctrl, fn)   ctrl->Disconnect(ctrl->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(fn), NULL, this);

#define ConnectCombo(ctrl, fn)              ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(fn), NULL, this);
#define UnConnectCombo(ctrl, fn)            ctrl->Disconnect(ctrl->GetId(), wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(fn), NULL, this);

#define ConnectCheckList(ctrl, fn)          ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler(fn), NULL, this);
#define UnConnectCheckList(ctrl, fn)        ctrl->Disconnect(ctrl->GetId(), wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler(fn), NULL, this);

#define TrimString(str) {  str = str.Trim(); str = str.Trim(false); }

#define IsSourceFile(ext) (ext == wxT("cpp") || ext == wxT("cxx") || ext == wxT("c") || ext == wxT("c++") || ext == wxT("cc"))

#define BoolToString(b) b ? wxT("yes") : wxT("no")

//-----------------------------------------------------
// Constants
//-----------------------------------------------------

#define clCMD_NEW  "<New...>"
#define clCMD_EDIT "<Edit...>"

// constant message
#define BUILD_START_MSG                 "----------Build Started--------\n"
#define BUILD_END_MSG                   "----------Build Ended----------\n"
#define BUILD_PROJECT_PREFIX            "----------Building project:[ "
#define CLEAN_PROJECT_PREFIX            "----------Cleaning project:[ "

// Find in files options
#define SEARCH_IN_WORKSPACE             "<Entire Workspace>"
#define SEARCH_IN_PROJECT               "<Active Project>"
#define SEARCH_IN_CURR_FILE_PROJECT     "<Current File's Project>"
#define SEARCH_IN_CURRENT_FILE          "<Current File>"
#define SEARCH_IN_OPEN_FILES            "<Open Files>"
#define SEARCH_IN_WORKSPACE_FOLDER      "<Workspace Folder>"
#define USE_WORKSPACE_ENV_VAR_SET       _("<Use Defaults>")
#define USE_GLOBAL_SETTINGS             _("<Use Defaults>")

#define WORKSPACE_VIEW          "Workspace View"
#define DEBUGGER_VIEW           "Debugger"
#define OUTPUT_VIEW             "Output View"
#define Perspective_VIEW        "Perspective View"
#define Main_VIEW               "Main View"
#define Edit_VIEW               "Edit View"
#define Scene_VIEW              "Scene View"
// terminal macro
#ifdef __WXGTK__
#define TERMINAL_CMD                                               \
    wxString::Format(wxT("%s/gsgs_xterm '$(TITLE)' '$(CMD)'"), \
                     wxFileName(PathManager::Get().GetExecutablePath()).GetPath().c_str())
#elif defined(__WXMAC__)
#define TERMINAL_CMD wxString::Format(wxT("%s/OpenTerm '$(CMD)'"), PathManager::Get().GetDataDir().c_str())
#else
#define TERMINAL_CMD ""
#endif

#define gsgsPathSep wxFileName::GetPathSeparator()

#if defined(__WXMSW__)
#define PATH_SEP wxT("\\")
#else
#define PATH_SEP wxT("/")
#endif

#define ID_MENU_CUSTOM_TARGET_FIRST         15000
#define ID_MENU_CUSTOM_TARGET_MAX           15100

#define viewAsMenuItemID                    23500
#define viewAsMenuItemMaxID                 23500 + 100

#define viewAsSubMenuID                     23499
#define RecentFilesSubMenuID                23600
#define RecentWorkspaceSubMenuID            23650

// Useful macros
#define CHECK_PTR_RET(p)                    if(!p) return
#define CHECK_PTR_RET_FALSE(p)              if(!p) return false
#define CHECK_PTR_RET_NULL(p)               if(!p) return NULL
#define CHECK_PTR_RET_EMPTY_STRING(p)       if(!p) return wxEmptyString

#define CHECK_COND_RET(p)                   if(!(p)) return
#define CHECK_COND_RET_FALSE(p)             if(!(p)) return false
#define CHECK_COND_RET_NULL(p)              if(!(p)) return NULL
#define CHECK_COND_RET_EMPTY_STRING(p)      if(!(p)) return wxEmptyString

#define CHECK_ITEM_RET(item)                if(!item.IsOk()) return
#define CHECK_ITEM_RET_FALSE(item)          if(!item.IsOk()) return false
#define CHECK_ITEM_RET_NULL(item)           if(!item.IsOk()) return NULL
#define CHECK_ITEM_RET_EMPTY_STRING(item)   if(!item.IsOk()) return wxEmptyString

#ifdef __WXOSX__
#define SB_WIDTH 10
#define SB_RADIUS 2.0
#elif defined(__WXGTK__)
#define SB_WIDTH 10
#define SB_RADIUS 2.0
#else
#define SB_WIDTH 10
#define SB_RADIUS 2.0
#endif

// PATH environment variable separator
#ifdef __WXMSW__
#define clPATH_SEPARATOR ";"
#else
#define clPATH_SEPARATOR ":"
#endif

#define USE_AUI_NOTEBOOK 0

// Defaults
#ifdef __WXGTK__
#ifndef PLUGINS_DIR
#define PLUGINS_DIR "/usr/lib/gsgs"
#endif
#ifndef INSTALL_DIR
#define INSTALL_DIR "/usr/share/gsgs"
#endif
#define CL_USE_NATIVEBOOK 0
#else
#ifdef USE_POSIX_LAYOUT
#ifndef PLUGINS_DIR
#define PLUGINS_DIR "\\lib\\gsgs"
#endif
#ifndef INSTALL_DIR
#define INSTALL_DIR "\\share\\gsgs"
#endif
#ifndef CFGDIR
#define CFGDIR "\\share\\gsgs\\config\\cfg"
#endif
#endif
// MSW / OSX and the rest of the world
#define CL_USE_NATIVEBOOK 0
#endif

// Allow override the default CL_USE_NATIVEBOOK by cmake variable (GTK only)
#ifdef __WXGTK__
#if GTK_USE_NATIVEBOOK
#ifdef CL_USE_NATIVEBOOK
#undef CL_USE_NATIVEBOOK
#endif
#define CL_USE_NATIVEBOOK 1
#else
#ifdef CL_USE_NATIVEBOOK
#undef CL_USE_NATIVEBOOK
#endif
#define CL_USE_NATIVEBOOK 0
#endif
#endif

#if wxVERSION_NUMBER < 2904
#define CL_USE_NEW_BUILD_TAB 0
#else
#define CL_USE_NEW_BUILD_TAB 1
#endif

#define CL_N0_OF_BOOKMARK_TYPES 5

#define gsgsDlgSize             wxSize(1200, 900)

#define gsgsMaxDlgSize          wxSize(1920, 1080)

#define gsgsMinDlgSize          wxSize(800, 600)

#define gsgsMMinDlgSize         wxSize(400, 200)

#define gsgsOptionDlgSize       wxSize(600, 400)

#define gsgsNookbookStyle kNotebook_MouseMiddleClickFireEvent | kNotebook_EnableNavigationEvent |\
                          kNotebook_UnderlineActiveTab |  kNotebook_DynamicColours | kNotebook_MouseScrollSwitchTabs

#define gsgsDlgStyle            wxSIMPLE_BORDER
#define CHEVRON_SIZE            10
#define CHEVRONPadding_SIZE     2

#define DEBUGGER_INDICATOR              11
#define MATCH_INDICATOR                 10
#define MARKER_WORD_HIGHLIGHT           2
#define USER_INDICATOR                  3
#define HYPERLINK_INDICATOR             4
#define MARKER_FIND_BAR_WORD_HIGHLIGHT  5
#define MARKER_CONTEXT_WORD_HIGHLIGHT   6
#define CUR_LINE_NUMBER_STYLE           (wxSTC_STYLE_MAX - 1)

#endif // MACROS_H
