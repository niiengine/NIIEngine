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

#include "gsgsEvent.h"
#include "gsgsLogManager.h"
#include <wx/app.h>
#include <wx/stc/stc.h>
#include <wx/combobox.h>

namespace gsgs
{
    //-----------------------------------------------------------------------------------
    Event::Event(wxEventType type, int winid)
        : wxCommandEvent(type, winid)
        , m_answer(false)
        , m_allowed(true)
        , m_lineNumber(0)
        , m_selected(false)
    {
    }
    //-----------------------------------------------------------------------------------
    Event::Event(const Event& event)
        : wxCommandEvent(event)
        , m_answer(false)
        , m_allowed(true)
    {
        *this = event;
    }
    //-----------------------------------------------------------------------------------
    Event& Event::operator=(const Event& src)
    {
        m_strings.clear();
        m_ptr = src.m_ptr;
        for(size_t i = 0; i < src.m_strings.size(); ++i) {
            m_strings.Add(src.m_strings.Item(i).c_str());
        }
        m_fileName = src.m_fileName;
        m_answer = src.m_answer;
        m_allowed = src.m_allowed;
        m_oldName = src.m_oldName;
        m_lineNumber = src.m_lineNumber;
        m_selected = src.m_selected;

        // Copy wxCommandEvent members here
        m_eventType = src.m_eventType;
        m_id = src.m_id;
        m_cmdString = src.m_cmdString;
        m_commandInt = src.m_commandInt;
        m_extraLong = src.m_extraLong;
        return *this;
    }
    //-----------------------------------------------------------------------------------
    Event::~Event() { m_ptr.reset(); }
    //-----------------------------------------------------------------------------------
    wxEvent* Event::Clone() const
    {
        Event* new_event = new Event(*this);
        return new_event;
    }
    //-----------------------------------------------------------------------------------
    void Event::SetClientObject(wxClientData* clientObject) { m_ptr = clientObject; }
    //-----------------------------------------------------------------------------------
    wxClientData* Event::GetClientObject() const { return m_ptr.get(); }
    //-----------------------------------------------------------------------------------
    // ---------------------------------
    // Code Completion event
    // ---------------------------------
    wxEvent* CodeCompleteEvent::Clone() const
    {
        CodeCompleteEvent* new_event = new CodeCompleteEvent(*this);
        return new_event;
    }
    //-----------------------------------------------------------------------------------
    CodeCompleteEvent::CodeCompleteEvent(const CodeCompleteEvent& event)
        : Event(event)
        , m_editor(NULL)
        , m_insideCommentOrString(false)
    {
        *this = event;
        m_position = wxNOT_FOUND;
        m_entry.reset(NULL);
    }
    //-----------------------------------------------------------------------------------
    CodeCompleteEvent::CodeCompleteEvent(wxEventType commandType, int winid)
        : Event(commandType, winid)
        , m_editor(NULL)
        , m_insideCommentOrString(false)
    {
        m_position = wxNOT_FOUND;
        m_entry.reset(NULL);
    }
    //-----------------------------------------------------------------------------------
    CodeCompleteEvent::~CodeCompleteEvent() {}
    //-----------------------------------------------------------------------------------
    CodeCompleteEvent& CodeCompleteEvent::operator=(const CodeCompleteEvent& src)
    {
        // Call parent operator =
        Event::operator=(src);
        // Implement our copy c tor
        m_editor = src.m_editor;
        m_word = src.m_word;
        m_position = src.m_position;
        m_tooltip = src.m_tooltip;
        m_insideCommentOrString = src.m_insideCommentOrString;
        m_entry = src.m_entry;
        m_definitions = src.m_definitions;
        m_entries = src.m_entries;
        m_triggerKind = src.m_triggerKind;
        return *this;
    }

    // ------------------------------------------------
    // ColourEvent
    // ------------------------------------------------

    ColourEvent::ColourEvent(const ColourEvent & event) { *this = event; }
    //-----------------------------------------------------------------------------------
    ColourEvent::ColourEvent(wxEventType commandType, int winid)
        : Event(commandType, winid)
    #if wxUSE_GUI
        , m_page(NULL)
    #endif
        , m_isActiveTab(false)
    {
    }
    //-----------------------------------------------------------------------------------
    ColourEvent::~ColourEvent() {}
    //-----------------------------------------------------------------------------------
    ColourEvent& ColourEvent::operator=(const ColourEvent& src)
    {
        // Call parent operator =
        Event::operator=(src);
    #if wxUSE_GUI
        m_bgColour = src.m_bgColour;
        m_fgColour = src.m_fgColour;
        m_page = src.m_page;
        m_borderColour = src.m_borderColour;
    #endif
        m_isActiveTab = src.m_isActiveTab;
        return *this;
    }
    // ------------------------------------------------
    // BuildEvent
    // ------------------------------------------------
    BuildEvent::BuildEvent(const BuildEvent& event) { *this = event; }
    // ------------------------------------------------
    BuildEvent::BuildEvent(wxEventType commandType, int winid)
        : Event(commandType, winid)
        , m_projectOnly(false)
        , m_warningCount(0)
        , m_errorCount(0)
        , m_isRunning(false)
    {
    }
    // ------------------------------------------------
    BuildEvent::~BuildEvent() {}
    // ------------------------------------------------
    BuildEvent& BuildEvent::operator=(const BuildEvent& src)
    {
        // Call parent operator =
        Event::operator=(src);
        m_projectName = src.m_projectName;
        m_configurationName = src.m_configurationName;
        m_command = src.m_command;
        m_projectOnly = src.m_projectOnly;
        m_errorCount = src.m_errorCount;
        m_warningCount = src.m_warningCount;
        m_kind = src.m_kind;
        m_isRunning = src.m_isRunning;
        return *this;
    }
    // ------------------------------------------------------------------
    // DebugEvent
    // ------------------------------------------------------------------
    DebugEvent::DebugEvent(wxEventType commandType, int winid)
        : Event(commandType, winid)
        , m_features(kAllFeatures)
        , m_memoryBlockSize(32)
    {
    }
    //-----------------------------------------------------------------------------------
    DebugEvent::~DebugEvent() {}
    //-----------------------------------------------------------------------------------
    DebugEvent::DebugEvent(const DebugEvent& event) { *this = event; }
    //-----------------------------------------------------------------------------------
    DebugEvent& DebugEvent::operator=(const DebugEvent& other)
    {
        Event::operator=(other);
        m_features = other.m_features;
        m_debuggerName = other.m_debuggerName;
        m_projectName = other.m_projectName;
        m_configurationName = other.m_configurationName;
        m_executableName =
            other.m_executableName;    // This will be set for wxEVT_DBG_UI_QUICK_DEBUG and wxEVT_DBG_UI_CORE_FILE
        m_coreFile = other.m_coreFile; // wxEVT_DBG_UI_CORE_FILE
        m_workingDirectory = other.m_workingDirectory; // wxEVT_DBG_UI_CORE_FILE, wxEVT_DBG_UI_QUICK_DEBUG
        m_arguments = other.m_arguments;               // wxEVT_DBG_UI_QUICK_DEBUG
        m_startupCommands = other.m_startupCommands;   // wxEVT_DBG_UI_QUICK_DEBUG
        m_memoryBlockSize = other.m_memoryBlockSize;
        m_memoryAddress = other.m_memoryAddress;
        m_memoryBlockValue = other.m_memoryBlockValue;
        return *this;
    }

    //-------------------------------------------------------------------------
    // NewProjectEvent
    //-------------------------------------------------------------------------

    NewProjectEvent::NewProjectEvent(wxEventType commandType, int winid)
        : Event(commandType, winid)
    {
    }
    //-----------------------------------------------------------------------------------
    NewProjectEvent::NewProjectEvent(const NewProjectEvent& event) { *this = event; }
    //-----------------------------------------------------------------------------------
    NewProjectEvent::~NewProjectEvent() {}
    //-----------------------------------------------------------------------------------
    NewProjectEvent& NewProjectEvent::operator=(const NewProjectEvent& other)
    {
        Event::operator=(other);
        m_templates = other.m_templates;
        m_toolchain = other.m_toolchain;
        m_debugger = other.m_debugger;
        m_projectName = other.m_projectName;
        m_projectFolder = other.m_projectFolder;
        m_templateName = other.m_templateName;
        return *this;
    }

    // --------------------------------------------------------------
    // CompilerEvent
    // --------------------------------------------------------------
    CompilerEvent::~CompilerEvent() {}
    //-----------------------------------------------------------------------------------
    CompilerEvent::CompilerEvent(const CompilerEvent& event) { *this = event; }
    //-----------------------------------------------------------------------------------
    CompilerEvent::CompilerEvent(wxEventType commandType, int winid)
        : Event(commandType, winid)
    {
    }
    //-----------------------------------------------------------------------------------
    CompilerEvent& CompilerEvent::operator=(const CompilerEvent& src)
    {
        Event::operator=(src);
        return *this;
    }

    // --------------------------------------------------------------
    // Source formatting event
    // --------------------------------------------------------------

    SourceFormatEvent::SourceFormatEvent(const SourceFormatEvent& event) { *this = event; }
    //-----------------------------------------------------------------------------------
    SourceFormatEvent::SourceFormatEvent(wxEventType commandType, int winid)
        : Event(commandType, winid)
    {
    }
    //-----------------------------------------------------------------------------------
    SourceFormatEvent::~SourceFormatEvent() {}
    //-----------------------------------------------------------------------------------
    SourceFormatEvent& SourceFormatEvent::operator=(const SourceFormatEvent& src)
    {
        Event::operator=(src);
        m_formattedString = src.m_formattedString;
        m_inputString = src.m_inputString;
        return *this;
    }

    //-------------------------------------------------------------------------
    // ContextMenuEvent
    //-------------------------------------------------------------------------

    ContextMenuEvent::ContextMenuEvent(wxEventType commandType, int winid)
        : Event(commandType, winid)
    #if wxUSE_GUI
        , m_menu(NULL)
    #endif
        , m_editor(NULL)
    {
    }
    //-----------------------------------------------------------------------------------
    ContextMenuEvent::ContextMenuEvent(const ContextMenuEvent& event) { *this = event; }
    //-----------------------------------------------------------------------------------
    ContextMenuEvent& ContextMenuEvent::operator=(const ContextMenuEvent& src)
    {
        Event::operator=(src);
        m_editor = src.m_editor;
    #if wxUSE_GUI
        m_menu = src.m_menu;
    #endif
        m_path = src.m_path;
        return *this;
    }
    //-----------------------------------------------------------------------------------
    ContextMenuEvent::~ContextMenuEvent() {}

    //-------------------------------------------------------------------------
    // ExecuteEvent
    //-------------------------------------------------------------------------

    ExecuteEvent::ExecuteEvent(const ExecuteEvent& event) { *this = event; }
    //-----------------------------------------------------------------------------------
    ExecuteEvent::~ExecuteEvent() {}
    //-----------------------------------------------------------------------------------
    ExecuteEvent& ExecuteEvent::operator=(const ExecuteEvent& src)
    {
        Event::operator=(src);
        m_targetName = src.m_targetName;
        return *this;
    }
    //-----------------------------------------------------------------------------------
    ExecuteEvent::ExecuteEvent(wxEventType commandType, int winid)
        : Event(commandType, winid)
    {
    }

    //------------------------------------------------------------------------
    // ProjectSettingsEvent
    //------------------------------------------------------------------------
    ProjectSettingsEvent& ProjectSettingsEvent::operator=(const ProjectSettingsEvent& src)
    {
        Event::operator=(src);
        m_configName = src.m_configName;
        m_projectName = src.m_projectName;
        return *this;
    }
    //-----------------------------------------------------------------------------------
    ProjectSettingsEvent::ProjectSettingsEvent(const ProjectSettingsEvent& event) { *this = event; }
    //-----------------------------------------------------------------------------------
    ProjectSettingsEvent::ProjectSettingsEvent(wxEventType commandType, int winid)
        : Event(commandType, winid)
    {
    }
    //-----------------------------------------------------------------------------------
    ProjectSettingsEvent::~ProjectSettingsEvent() {}
    //-----------------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // FindEvent
    //------------------------------------------------------------------------
    FindEvent& FindEvent::operator=(const FindEvent& src)
    {
        Event::operator=(src);
        m_ctrl = src.m_ctrl;
        return *this;
    }
    //-----------------------------------------------------------------------------------
    FindEvent::FindEvent(const FindEvent& event) { *this = event; }
    //-----------------------------------------------------------------------------------
    FindEvent::FindEvent(wxEventType commandType, int winid)
        : Event(commandType, winid)
        , m_ctrl(NULL)
    {
    }
    //-----------------------------------------------------------------------------------
    FindEvent::~FindEvent() {}
    //-----------------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // FindInFilesEvent
    //------------------------------------------------------------------------
    FindInFilesEvent& FindInFilesEvent::operator=(const FindInFilesEvent& src)
    {
        Event::operator=(src);
        m_paths = src.m_paths;
        m_fileMask = src.m_fileMask;
        m_options = src.m_options;
        m_transientPaths = src.m_transientPaths;
        return *this;
    }
    //-----------------------------------------------------------------------------------
    FindInFilesEvent::FindInFilesEvent(const FindInFilesEvent& event) { *this = event; }
    //-----------------------------------------------------------------------------------
    FindInFilesEvent::FindInFilesEvent(wxEventType commandType, int winid)
        : Event(commandType, winid)
    {
    }
    //-----------------------------------------------------------------------------------
    FindInFilesEvent::~FindInFilesEvent() {}

    //------------------------------------------------------------------------
    // ParseEvent
    //------------------------------------------------------------------------

    ParseEvent::ParseEvent(const ParseEvent& event) { *this = event; }
    //-----------------------------------------------------------------------------------
    ParseEvent::ParseEvent(wxEventType commandType, int winid)
        : Event(commandType, winid)
        , m_curfileIndex(0)
        , m_totalFiles(0)
    {
    }
    //-----------------------------------------------------------------------------------
    ParseEvent::~ParseEvent() {}
    //-----------------------------------------------------------------------------------
    ParseEvent& ParseEvent::operator=(const ParseEvent& src)
    {
        Event::operator=(src);
        m_curfileIndex = src.m_curfileIndex;
        m_totalFiles = src.m_totalFiles;
        return *this;
    }

    //-------------------------------------------------------------------
    // ProcessEvent
    //-------------------------------------------------------------------

    ProcessEvent::ProcessEvent(const ProcessEvent& event) { *this = event; }
    //-----------------------------------------------------------------------------------
    ProcessEvent::ProcessEvent(wxEventType commandType, int winid)
        : Event(commandType, winid)
        , m_process(NULL)
    {
    }
    //-----------------------------------------------------------------------------------
    ProcessEvent::~ProcessEvent() {}
    //-----------------------------------------------------------------------------------
    ProcessEvent& ProcessEvent::operator=(const ProcessEvent& src)
    {
        Event::operator=(src);
        m_process = src.m_process;
        m_output = src.m_output;
        return *this;
    }
    // --------------------------------------------------------------
    EditEventsHandler::EditEventsHandler(wxStyledTextCtrl* wnd)
        : m_stc(wnd)
        , m_textCtrl(nullptr)
        , m_combo(nullptr)
        , m_noUnbind(false)
    {
        DoInitialize();
    }
    //-----------------------------------------------------------------------------------
    EditEventsHandler::EditEventsHandler(wxTextCtrl* wnd)
        : m_stc(nullptr)
        , m_textCtrl(wnd)
        , m_combo(nullptr)
        , m_noUnbind(false)
    {
        DoInitialize();
    }
    //-----------------------------------------------------------------------------------
    EditEventsHandler::EditEventsHandler(wxComboBox* wnd)
        : m_stc(nullptr)
        , m_textCtrl(nullptr)
        , m_combo(wnd)
        , m_noUnbind(false)
    {
        DoInitialize();
    }
    //-----------------------------------------------------------------------------------
    EditEventsHandler::~EditEventsHandler()
    {
        if(!m_noUnbind && (m_stc || m_textCtrl || m_combo)) {
            gsgs_Event().TopFrame()->Unbind(wxEVT_MENU, &EditEventsHandler::OnCopy, this, wxID_COPY);
            gsgs_Event().TopFrame()->Unbind(wxEVT_MENU, &EditEventsHandler::OnPaste, this, wxID_PASTE);
            gsgs_Event().TopFrame()->Unbind(wxEVT_MENU, &EditEventsHandler::OnCut, this, wxID_CUT);
            gsgs_Event().TopFrame()->Unbind(wxEVT_MENU, &EditEventsHandler::OnSelectAll, this, wxID_SELECTALL);
            gsgs_Event().TopFrame()->Unbind(wxEVT_MENU, &EditEventsHandler::OnUndo, this, wxID_UNDO);
            gsgs_Event().TopFrame()->Unbind(wxEVT_MENU, &EditEventsHandler::OnRedo, this, wxID_REDO);
        }
    }
    //-----------------------------------------------------------------------------------
    #define CHECK_FOCUS_WINDOW()                                                     \
        wxWindow* focus = wxWindow::FindFocus();                                     \
        if(!focus) {                                                                 \
            event.Skip();                                                            \
            return;                                                                  \
        } else if((focus != m_stc) && (focus != m_textCtrl) && (focus != m_combo)) { \
            event.Skip();                                                            \
            return;                                                                  \
        }

    #define CALL_FUNC(func)     \
        if(m_stc) {             \
            m_stc->func();      \
        } else if(m_combo) {    \
            m_combo->func();    \
        } else {                \
            m_textCtrl->func(); \
        }
    //-----------------------------------------------------------------------------------
    void EditEventsHandler::OnCopy(wxCommandEvent& event)
    {
        CHECK_FOCUS_WINDOW();
        CALL_FUNC(Copy);
    }
    //-----------------------------------------------------------------------------------
    void EditEventsHandler::OnPaste(wxCommandEvent& event)
    {
        CHECK_FOCUS_WINDOW();
        CALL_FUNC(Paste);
    }
    //-----------------------------------------------------------------------------------
    void EditEventsHandler::OnCut(wxCommandEvent& event)
    {
        CHECK_FOCUS_WINDOW();
        CALL_FUNC(Cut);
    }
    //-----------------------------------------------------------------------------------
    void EditEventsHandler::OnSelectAll(wxCommandEvent& event)
    {
        CHECK_FOCUS_WINDOW();
        CALL_FUNC(SelectAll);
    }
    //-----------------------------------------------------------------------------------
    void EditEventsHandler::OnUndo(wxCommandEvent& event)
    {
        CHECK_FOCUS_WINDOW();
        CALL_FUNC(Undo);
    }
    //-----------------------------------------------------------------------------------
    void EditEventsHandler::OnRedo(wxCommandEvent& event)
    {
        CHECK_FOCUS_WINDOW();
        CALL_FUNC(Redo);
    }
    //-----------------------------------------------------------------------------------
    void EditEventsHandler::DoInitialize()
    {
        if(m_textCtrl || m_stc || m_combo) {
            gsgs_Event().TopFrame()->Bind(wxEVT_MENU, &EditEventsHandler::OnCopy, this, wxID_COPY);
            gsgs_Event().TopFrame()->Bind(wxEVT_MENU, &EditEventsHandler::OnPaste, this, wxID_PASTE);
            gsgs_Event().TopFrame()->Bind(wxEVT_MENU, &EditEventsHandler::OnCut, this, wxID_CUT);
            gsgs_Event().TopFrame()->Bind(wxEVT_MENU, &EditEventsHandler::OnSelectAll, this, wxID_SELECTALL);
            gsgs_Event().TopFrame()->Bind(wxEVT_MENU, &EditEventsHandler::OnUndo, this, wxID_UNDO);
            gsgs_Event().TopFrame()->Bind(wxEVT_MENU, &EditEventsHandler::OnRedo, this, wxID_REDO);
        }
    }
    // --------------------------------------------------------------
    wxDEFINE_EVENT(wxEVT_FILE_DELETED,                      FileEvent);
    wxDEFINE_EVENT(wxEVT_FOLDER_DELETED,                    FileEvent);
    wxDEFINE_EVENT(wxEVT_INIT_DONE,                         wxCommandEvent);
    wxDEFINE_EVENT(GSGS_ProgramMinimize,                    wxCommandEvent);
    wxDEFINE_EVENT(GSGS_ProgramMaximize,                    wxCommandEvent);
    wxDEFINE_EVENT(GSGS_ProgramClose,                       wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_EDITOR_CONFIG_CHANGED,             wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_WORKSPACE_LOADED,                  wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_WORKSPACE_CONFIG_CHANGED,          wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_WORKSPACE_CLOSED,                  wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_WORKSPACE_CLOSING,                 wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_WORKSPACE_Recently,                wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_FILE_VIEW_INIT_DONE,               wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_FILE_VIEW_REFRESHED,               wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_FILE_EXP_INIT_DONE,                wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_FILE_EXP_REFRESHED,                wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CMD_FILE_EXP_ITEM_EXPANDING,       wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_TREE_ITEM_FILE_ACTIVATED,          Event);
    wxDEFINE_EVENT(wxEVT_PROJ_FILE_ADDED,                   Event);
    wxDEFINE_EVENT(wxEVT_PROJ_FILE_REMOVED,                 Event);
    wxDEFINE_EVENT(wxEVT_PROJ_REMOVED,                      Event);
    wxDEFINE_EVENT(wxEVT_PROJ_ADDED,                        Event);
    wxDEFINE_EVENT(wxEVT_FILE_SAVE_BY_BUILD_START,          wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_FILE_SAVE_BY_BUILD_END,            wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_FILE_SAVED,                        Event);
    wxDEFINE_EVENT(wxEVT_FILE_RENAMED,                      FileEvent);
    wxDEFINE_EVENT(wxEVT_FILE_SAVEAS,                       FileEvent);
    wxDEFINE_EVENT(wxEVT_FILE_CREATED,                      FileEvent);
    wxDEFINE_EVENT(wxEVT_FOLDER_CREATED,                    FileEvent);
    wxDEFINE_EVENT(wxEVT_FILE_RETAGGED,                     wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_ACTIVE_EDITOR_CHANGED,             wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_EDITOR_CLOSING,                    wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_ALL_EDITORS_CLOSING,               wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_ALL_EDITORS_CLOSED,                wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_EDITOR_CLICKED,                    wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_EDITOR_SETTINGS_CHANGED,           wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED,    wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT, wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CMD_PROJ_SETTINGS_SAVED,           ProjectSettingsEvent);
    wxDEFINE_EVENT(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT,        ExecuteEvent);
    wxDEFINE_EVENT(wxEVT_CMD_IS_PROGRAM_RUNNING,            ExecuteEvent);
    wxDEFINE_EVENT(wxEVT_CMD_STOP_EXECUTED_PROGRAM,         ExecuteEvent);
    wxDEFINE_EVENT(wxEVT_BUILD_STARTED,                     BuildEvent);
    wxDEFINE_EVENT(wxEVT_BUILD_ENDED,                       BuildEvent);
    wxDEFINE_EVENT(wxEVT_BUILD_STARTING,                    BuildEvent);
    wxDEFINE_EVENT(wxEVT_STOP_BUILD,                        BuildEvent);
    wxDEFINE_EVENT(wxEVT_GET_PROJECT_CLEAN_CMD,             BuildEvent);
    wxDEFINE_EVENT(wxEVT_GET_PROJECT_BUILD_CMD,             BuildEvent);
    wxDEFINE_EVENT(wxEVT_GET_IS_PLUGIN_MAKEFILE,            BuildEvent);
    wxDEFINE_EVENT(wxEVT_GET_IS_PLUGIN_BUILD,               BuildEvent);
    wxDEFINE_EVENT(wxEVT_GET_ADDITIONAL_COMPILEFLAGS,       BuildEvent);
    wxDEFINE_EVENT(wxEVT_GET_ADDITIONAL_LINKFLAGS,          BuildEvent);
    wxDEFINE_EVENT(wxEVT_PLUGIN_EXPORT_MAKEFILE,            BuildEvent);
    wxDEFINE_EVENT(wxEVT_GET_IS_BUILD_IN_PROGRESS,          BuildEvent);
    wxDEFINE_EVENT(wxEVT_DEBUG_STARTING,                    DebugEvent);
    wxDEFINE_EVENT(wxEVT_DEBUG_STARTED,                     DebugEvent);
    wxDEFINE_EVENT(wxEVT_DEBUG_ENDING,                      DebugEvent);
    wxDEFINE_EVENT(wxEVT_DEBUG_ENDED,                       DebugEvent);
    wxDEFINE_EVENT(wxEVT_DEBUGGER_REFRESH_PANE,             DebugEvent);
    wxDEFINE_EVENT(wxEVT_DEBUGGER_SET_MEMORY,               DebugEvent);
    wxDEFINE_EVENT(wxEVT_DEBUG_EDITOR_LOST_CONTROL,         wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_DEBUG_EDITOR_GOT_CONTROL,          wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CC_CODE_COMPLETE,                  CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CC_WORD_COMPLETE,                  CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CCBOX_SELECTION_MADE,              CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CC_CODE_COMPLETE_BOX_DISMISSED,    CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CCBOX_SHOWING,                     CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CC_SHOW_QUICK_OUTLINE,             CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CC_TYPEINFO_TIP,                   CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CC_CODE_COMPLETE_LANG_KEYWORD,     CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CC_JUMP_HYPER_LINK,                CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CC_FIND_SYMBOL,                    CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CC_FIND_SYMBOL_DECLARATION,        CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CC_FIND_SYMBOL_DEFINITION,         CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CC_GENERATE_DOXY_BLOCK,            CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CC_UPDATE_NAVBAR,                  CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CC_BLOCK_COMMENT_CODE_COMPLETE,    CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CC_BLOCK_COMMENT_WORD_COMPLETE,    CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CMD_CREATE_NEW_WORKSPACE,          Event);
    wxDEFINE_EVENT(wxEVT_CMD_OPEN_WORKSPACE,                Event);
    wxDEFINE_EVENT(wxEVT_CMD_CLOSE_WORKSPACE,               Event);
    wxDEFINE_EVENT(wxEVT_CMD_IS_WORKSPACE_OPEN,             Event);
    wxDEFINE_EVENT(wxEVT_CMD_RETAG_WORKSPACE,               wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CMD_RETAG_WORKSPACE_FULL,          wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CMD_GET_WORKSPACE_FILES,           wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CMD_GET_ACTIVE_PROJECT_FILES,      wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CMD_GET_CURRENT_FILE_PROJECT_FILES, wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CMD_OPEN_RESOURCE,                 wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CMD_EDITOR_CONTEXT_MENU,           wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CMD_RETAG_COMPLETED,               wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_NOTIFY_PAGE_CLOSING,               wxNotifyEvent);
    wxDEFINE_EVENT(wxEVT_CMD_PAGE_CHANGED,                  wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_REBUILD_WORKSPACE_TREE,            wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_ACTIVE_PROJECT_CHANGED,            ProjectSettingsEvent);
    wxDEFINE_EVENT(wxEVT_FINDBAR_ABOUT_TO_SHOW,             FindEvent);
    wxDEFINE_EVENT(wxEVT_FINDBAR_RELEASE_EDITOR,            FindEvent);
    wxDEFINE_EVENT(wxEVT_FINDINFILES_DLG_SHOWING,           FindInFilesEvent);
    wxDEFINE_EVENT(wxEVT_FINDINFILES_DLG_DISMISSED,         FindInFilesEvent);
    wxDEFINE_EVENT(wxEVT_CMD_BUILD_PROJECT_ONLY,            wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CMD_CLEAN_PROJECT_ONLY,            wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_GSGS_THEME_CHANGED,                wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_LOAD_SESSION,                      wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CODEFORMATTER_INDENT_COMPLETED,    wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CODEFORMATTER_INDENT_STARTING,     wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_GSGS_MAINFRAME_GOT_FOCUS,      wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_PROJECT_TREEITEM_CLICKED,          wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_GSGS_ALL_BREAKPOINTS_DELETED,  wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CMD_NEW_DOCKPANE,                  wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CMD_DELETE_DOCKPANE,               wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CC_SHOW_QUICK_NAV_MENU,            CodeCompleteEvent);
    wxDEFINE_EVENT(wxEVT_CMD_RELOAD_WORKSPACE,              Event);
    wxDEFINE_EVENT(wxEVT_COLOUR_TAB,                        ColourEvent);
    wxDEFINE_EVENT(wxEVT_WORKSPACE_VIEW_BUILD_STARTING,     Event);
    wxDEFINE_EVENT(wxEVT_WORKSPACE_VIEW_CUSTOMIZE_PROJECT,  ColourEvent);
    wxDEFINE_EVENT(wxEVT_GET_TAB_BORDER_COLOUR,             ColourEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_START,                      DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_CONTINUE,                   DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_STOP,                       DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_STEP_IN,                    DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_STEP_I,                     DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_STEP_OUT,                   DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_NEXT,                       DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_NEXT_INST,                  DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_INTERRUPT,                  DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_SHOW_CURSOR,                DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_RESTART,                    DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_IS_RUNNING,                    DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_TOGGLE_BREAKPOINT,          DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_CAN_INTERACT,                  DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_EXPR_TOOLTIP,                  DebugEvent);

    wxDEFINE_EVENT(wxEVT_DBG_IS_PLUGIN_DEBUGGER,            DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_QUICK_DEBUG,                DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_CORE_FILE,                  DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_ATTACH_TO_PROCESS,          DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_DELETE_ALL_BREAKPOINTS,     DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_ENABLE_ALL_BREAKPOINTS,     DebugEvent);
    wxDEFINE_EVENT(wxEVT_DBG_UI_DISABLE_ALL_BREAKPOINTS,    DebugEvent);
    wxDEFINE_EVENT(wxEVT_CMD_OPEN_PROJ_SETTINGS,            Event);
    wxDEFINE_EVENT(wxEVT_WORKSPACE_RELOAD_STARTED,          Event);
    wxDEFINE_EVENT(wxEVT_WORKSPACE_RELOAD_ENDED,            Event);
    wxDEFINE_EVENT(wxEVT_NEW_PROJECT_WIZARD_SHOWING,        NewProjectEvent);
    wxDEFINE_EVENT(wxEVT_NEW_PROJECT_WIZARD_FINISHED,       NewProjectEvent);
    wxDEFINE_EVENT(wxEVT_COMPILER_LIST_UPDATED,             CompilerEvent);
    wxDEFINE_EVENT(wxEVT_SAVE_ALL_EDITORS,                  Event);
    wxDEFINE_EVENT(wxEVT_FORMAT_STRING,                     SourceFormatEvent);
    wxDEFINE_EVENT(wxEVT_FORMAT_FILE,                       SourceFormatEvent);
    wxDEFINE_EVENT(wxEVT_CONTEXT_MENU_EDITOR,               ContextMenuEvent);
    wxDEFINE_EVENT(wxEVT_CONTEXT_MENU_EDITOR_MARGIN,        ContextMenuEvent);
    wxDEFINE_EVENT(wxEVT_CONTEXT_MENU_FOLDER,               ContextMenuEvent);
    wxDEFINE_EVENT(wxEVT_CONTEXT_MENU_VIRTUAL_FOLDER,       ContextMenuEvent);
    wxDEFINE_EVENT(wxEVT_CONTEXT_MENU_FILE,                 ContextMenuEvent);
    wxDEFINE_EVENT(wxEVT_CONTEXT_MENU_PROJECT,              ContextMenuEvent);
    wxDEFINE_EVENT(wxEVT_CONTEXT_MENU_WORKSPACE,            ContextMenuEvent);
    wxDEFINE_EVENT(wxEVT_CONTEXT_MENU_TAB_LABEL,            ContextMenuEvent);
    wxDEFINE_EVENT(wxEVT_CMD_COLOURS_FONTS_UPDATED,         Event);
    wxDEFINE_EVENT(wxEVT_FILE_LOADED,                       Event);
    wxDEFINE_EVENT(wxEVT_FILE_CLOSED,                       Event);
    wxDEFINE_EVENT(wxEVT_CL_FRAME_TITLE,                    Event);
    wxDEFINE_EVENT(wxEVT_BEFORE_EDITOR_SAVE,                Event);
    wxDEFINE_EVENT(wxEVT_EDITOR_MODIFIED,                   Event);
    wxDEFINE_EVENT(wxEVT_CLANG_CODE_COMPLETE_MESSAGE,       Event);
    wxDEFINE_EVENT(wxEVT_GOING_DOWN,                        Event);
    wxDEFINE_EVENT(wxEVT_PROJ_RENAMED,                      Event);
    wxDEFINE_EVENT(wxEVT_EDITOR_INITIALIZING,               Event);
    wxDEFINE_EVENT(wxEVT_FILE_SYSTEM_UPDATED,               FileEvent);
    wxDEFINE_EVENT(wxEVT_FILES_MODIFIED_REPLACE_IN_FILES,   FileEvent);
    wxDEFINE_EVENT(wxEVT_SAVE_SESSION_NEEDED,               Event);
    wxDEFINE_EVENT(wxEVT_ENVIRONMENT_VARIABLES_MODIFIED,    Event);
    wxDEFINE_EVENT(wxEVT_DND_FOLDER_DROPPED,                Event);
    wxDEFINE_EVENT(wxEVT_DND_FILE_DROPPED,                  Event);
    wxDEFINE_EVENT(wxEVT_RESTART_GSGS,                      Event);
    wxDEFINE_EVENT(wxEVT_SHOW_WORKSPACE_TAB,                Event);
    wxDEFINE_EVENT(wxEVT_SHOW_OUTPUT_TAB,                   Event);
    wxDEFINE_EVENT(wxEVT_SHOW_Perspective_TAB,              Event);
    wxDEFINE_EVENT(wxEVT_PAGE_MODIFIED_UPDATE_UI,           Event);
    wxDEFINE_EVENT(wxEVT_PHP_SETTINGS_CHANGED,              Event);
    wxDEFINE_EVENT(wxEVT_WORKSPACE_BUILD_CONFIG_CHANGED,    Event);
    wxDEFINE_EVENT(wxEVT_NAVBAR_SCOPE_MENU_SHOWING,         ContextMenuEvent);
    wxDEFINE_EVENT(wxEVT_NAVBAR_SCOPE_MENU_SELECTION_MADE,  Event);
    wxDEFINE_EVENT(wxEVT_MARKER_CHANGED,                    Event);
    wxDEFINE_EVENT(wxEVT_INFO_BAR_BUTTON,                   Event);

    static EventManager * eventNotifier = NULL;
    //-----------------------------------------------------------------------------------
    EventManager& EventManager::singleton()
    {
        if (eventNotifier == NULL)
            eventNotifier = new EventManager();
        return *eventNotifier;
    }
    //-----------------------------------------------------------------------------------
    void EventManager::Release()
    {
        if (eventNotifier)
            delete eventNotifier;
        eventNotifier = NULL;
    }
    //-----------------------------------------------------------------------------------
    EventManager::EventManager() :
        mDisable(false)
    {
    }
    //-----------------------------------------------------------------------------------
    EventManager::~EventManager() {}
    //-----------------------------------------------------------------------------------
    bool EventManager::SendCommandEvent(int eventId, void * clientData)
    {
        if (mDisable)
            return false;
        wxCommandEvent evt(eventId);
        evt.SetClientData(clientData);
        return ProcessEvent(evt);
    }
    //-----------------------------------------------------------------------------------
    bool EventManager::SendCommandEvent(int eventId, void * clientData, const wxString & s)
    {
        if (mDisable)
            return false;
        wxCommandEvent evt(eventId);
        evt.SetClientData(clientData);
        evt.SetString(s);
        return ProcessEvent(evt);
    }
    //-----------------------------------------------------------------------------------
    void EventManager::PostCommandEvent(int eventId, void* clientData)
    {
        if (mDisable)
            return;
        wxCommandEvent evt(eventId);
        evt.SetClientData(clientData);
        AddPendingEvent(evt);
    }
    //-----------------------------------------------------------------------------------
#if wxUSE_GUI
    wxFrame* EventManager::TopFrame() 
    { 
        return static_cast<wxFrame*>(wxTheApp->GetTopWindow()); 
    }
#endif
    //-----------------------------------------------------------------------------------
    void EventManager::PostFileSavedEvent(const wxString& filename)
    {
        if (mDisable)
            return;
        Event event(wxEVT_FILE_SAVED);
        event.SetString(filename);
        event.SetFileName(filename);
        AddPendingEvent(event);
    }
    //-----------------------------------------------------------------------------------
    void EventManager::PostReloadExternallyModifiedEvent(bool prompt)
    {
        if (mDisable)
            return;
        wxCommandEvent event(prompt ? wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED : wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT);
        AddPendingEvent(event);
    }
    //-----------------------------------------------------------------------------------
    void EventManager::PostFileRemovedEvent(const wxArrayString& files)
    {
        if (mDisable)
            return;
        Event filesRemovedEvent(wxEVT_PROJ_FILE_REMOVED);
        filesRemovedEvent.SetStrings(files);
        AddPendingEvent(filesRemovedEvent);
    }
    //-----------------------------------------------------------------------------------
    void EventManager::NotifyWorkspaceReloadEndEvent(const wxString& workspaceFile)
    {
        if (mDisable)
            return;
        Event event(wxEVT_WORKSPACE_RELOAD_ENDED);
        event.SetFileName(workspaceFile);
        ProcessEvent(event);
    }
    //-----------------------------------------------------------------------------------
    void EventManager::NotifyWorkspaceReloadStartEvet(const wxString& workspaceFile)
    {
        if (mDisable)
            return;
        Event event(wxEVT_WORKSPACE_RELOAD_STARTED);
        event.SetFileName(workspaceFile);
        ProcessEvent(event);
    }
    //-----------------------------------------------------------------------------------
    void EventManager::AddPendingEvent(const wxEvent& event)
    {
        if (mDisable)
            return;
        wxEvtHandler::AddPendingEvent(event);
    }
    //-----------------------------------------------------------------------------------
    bool EventManager::ProcessEvent(wxEvent& event)
    {
        if (mDisable)
            return false;
        return wxEvtHandler::ProcessEvent(event);
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // ServiceProvider
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    ServiceProvider::ServiceProvider(const wxString& name, eServiceType type)
        : m_name(name)
        , m_type(type)
    {
        ServiceManager::Get().Register(this);
    }
    //-----------------------------------------------------------------------------------
    ServiceProvider::~ServiceProvider() 
    { 
        ServiceManager::Get().Unregister(this); 
    }
    //-----------------------------------------------------------------------------------
    void ServiceProvider::SetPriority(int priority)
    {
        m_priority = priority;
        ServiceManager::Get().Sort(GetType());
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // ServiceManager
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    ServiceManager::ServiceManager() 
    {
    }
    //-----------------------------------------------------------------------------------
    ServiceManager::~ServiceManager() 
    { 
        m_providers.clear(); 
    }
    //-----------------------------------------------------------------------------------
    void ServiceManager::Register(ServiceProvider* provider)
    {
        // No duplicates
        Unregister(provider);
        // Now register the service
        if(this->m_providers.count(provider->GetType()) == 0) 
        {
            m_providers.insert({ provider->GetType(), ServiceProvider::Vec_t() });
        }
        ServiceProvider::Vec_t & V = m_providers[provider->GetType()];
        V.push_back(provider);

        gsgsDEBUG() << "Handler:" << provider->GetName() << "registerd. Priority:" << provider->GetPriority()
                  << ". Type:" << (int)provider->GetType();

        // Sort the providers by priority (descending order)
        std::sort(V.begin(), V.end(),
                  [](ServiceProvider* a, ServiceProvider* b) { return a->GetPriority() > b->GetPriority(); });

        wxString order;
        for(ServiceProvider* p : V) 
        {
            order << p->GetName() << "(" << p->GetPriority() << ") ";
        }
        gsgsDEBUG() << "Service providers:" << order;
    }
    //-----------------------------------------------------------------------------------
    void ServiceManager::Unregister(ServiceProvider * provider)
    {
        if(this->m_providers.count(provider->GetType()) == 0) 
        { 
            return; 
        }
        ServiceProvider::Vec_t& V = m_providers[provider->GetType()];

        // Find our provider and remove it
        while(true) 
        {
            // Do this in a loop, incase someone registered this provider twice...
            auto where =
                std::find_if(V.begin(), V.end(), [&](ServiceProvider* p) { return p->GetName() == provider->GetName(); });
            if(where == V.end()) { break; }
            V.erase(where); // remove it
            gsgsDEBUG() << "Handler:" << provider->GetName() << "Uregisterd. Priority:" << provider->GetPriority()
                      << ". Type:" << (int)provider->GetType();
        }
    }
    //-----------------------------------------------------------------------------------
    ServiceManager & ServiceManager::Get()
    {
        static ServiceManager instance;
        return instance;
    }
    //-----------------------------------------------------------------------------------
    bool ServiceManager::ProcessEvent(wxEvent & event)
    {
        eServiceType type = GetServiceFromEvent(event);
        if(type == eServiceType::kUnknown || m_providers.count(type) == 0) 
        {
            // Let the default event processing take place by using gsgsEventManager
            return gsgs_Event().ProcessEvent(event);
        } 
        else 
        {
            // Call our chain
            auto & V = m_providers[type];
            for(ServiceProvider* p : V) 
            {
                if(p->ProcessEvent(event)) 
                { 
                    return true; 
                }
            }
            return false;
        }
    }
    //-----------------------------------------------------------------------------------
    eServiceType ServiceManager::GetServiceFromEvent(wxEvent & event)
    {
        wxEventType type = event.GetEventType();
        // Code Completion events
        if(type == wxEVT_CC_CODE_COMPLETE || type == wxEVT_CC_FIND_SYMBOL || type == wxEVT_CC_FIND_SYMBOL_DECLARATION ||
           type == wxEVT_CC_FIND_SYMBOL_DEFINITION || type == wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP ||
           type == wxEVT_CC_TYPEINFO_TIP || type == wxEVT_CC_WORD_COMPLETE) 
        {
            return eServiceType::kCodeCompletion;
        }
        return eServiceType::kUnknown;
    }
    //-----------------------------------------------------------------------------------
    void ServiceManager::UnregisterAll() 
    { 
        m_providers.clear(); 
    }
    //-----------------------------------------------------------------------------------
    void ServiceManager::Sort(eServiceType type)
    {
        if(m_providers.count(type) == 0) 
        { 
            return; 
        }
        gsgsDEBUG() << "sorting providers for type:" << (int)type;
        ServiceProvider::Vec_t& V = m_providers[type];
        std::sort(V.begin(), V.end(),
                  [](ServiceProvider* a, ServiceProvider* b) { return a->GetPriority() > b->GetPriority(); });
        wxString order;
        for(ServiceProvider* p : V) {
            order << p->GetName() << "(" << p->GetPriority() << ") ";
        }
        gsgsDEBUG() << "Service providers:" << order;
    }
    //-----------------------------------------------------------------------------------
}