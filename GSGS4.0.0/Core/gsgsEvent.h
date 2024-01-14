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
#ifndef _gsgs_Event_h_
#define _gsgs_Event_h_

#include "gsgsPreInclude.h"
#include "wxCodeCompletionBoxEntry.h"
#include "LSP/CompletionItem.h"

#if wxUSE_GUI
#include <wx/frame.h>
#endif

class IProcess;
class wxComboBox;
class wxStyledTextCtrl;
class wxTextCtrl;

// Set of flags that can be passed within the 'S{G}etInt' function of Event
enum {
    kEventImportingFolder = 0x00000001,
};

enum class eServiceType {
    kUnknown = -1,
    kCodeCompletion,
};

namespace std
{
    template <>
    struct hash<eServiceType>
    {
        std::size_t operator()(const eServiceType& t) const { return static_cast<std::size_t>(t); }
    };
}

#if !wxUSE_GUI
// To allow building libgsgs without UI we need these 2 event types (wxCommandEvent and wxNotifyEvent)
class _gsgsAPI wxCommandEvent : public wxEvent, public wxEventBasicPayloadMixin
{
public:
    wxCommandEvent(wxEventType commandType = wxEVT_NULL, int winid = 0)
        : wxEvent(winid, commandType)
    {
        m_clientData = NULL;
        m_clientObject = NULL;
        m_isCommandEvent = true;

        // the command events are propagated upwards by default
        m_propagationLevel = wxEVENT_PROPAGATE_MAX;
    }

    wxCommandEvent(const wxCommandEvent& event)
        : wxEvent(event)
        , wxEventBasicPayloadMixin(event)
        , m_clientData(event.m_clientData)
        , m_clientObject(event.m_clientObject)
    {
        // Because GetString() can retrieve the string text only on demand, we
        // need to copy it explicitly.
        if(m_cmdString.empty()) m_cmdString = event.GetString();
    }

    virtual ~wxCommandEvent() {}

    // Set/Get client data from controls
    inline void SetClientData(void* clientData)             { m_clientData = clientData; }
    inline void* GetClientData() const                      { return m_clientData; }

    // Set/Get client object from controls
    inline void SetClientObject(wxClientData* clientObject) { m_clientObject = clientObject; }
    inline wxClientData* GetClientObject() const            { return m_clientObject; }

    // Note: this shadows wxEventBasicPayloadMixin::GetString(), because it does some
    // GUI-specific hacks
    inline wxString GetString() const                       { return m_cmdString; }

    // Get listbox selection if single-choice
    inline int GetSelection() const                         { return m_commandInt; }

    // Get checkbox value
    inline bool IsChecked() const                           { return m_commandInt != 0; }

    // true if the listbox event was a selection.
    inline bool IsSelection() const                         { return (m_extraLong != 0); }

    virtual wxEvent* Clone() const wxOVERRIDE               { return new wxCommandEvent(*this); }
    virtual wxEventCategory GetEventCategory() const wxOVERRIDE { return wxEVT_CATEGORY_USER_INPUT; }

protected:
    void* m_clientData;           // Arbitrary client data
    wxClientData* m_clientObject; // Arbitrary client object
};

// this class adds a possibility to react (from the user) code to a control
// notification: allow or veto the operation being reported.
class _gsgsAPI wxNotifyEvent : public wxCommandEvent
{
public:
    wxNotifyEvent(wxEventType commandType = wxEVT_NULL, int winid = 0)
        : wxCommandEvent(commandType, winid)
    {
        m_bAllow = true;
    }

    wxNotifyEvent(const wxNotifyEvent& event)
        : wxCommandEvent(event)
    {
        m_bAllow = event.m_bAllow;
    }

    // veto the operation (usually it's allowed by default)
    inline void Veto()                                      { m_bAllow = false; }

    // allow the operation if it was disabled by default
    inline void Allow()                                     { m_bAllow = true; }

    // for implementation code only: is the operation allowed?
    inline bool IsAllowed() const                           { return m_bAllow; }

    virtual wxEvent* Clone() const wxOVERRIDE               { return new wxNotifyEvent(*this); }

private:
    bool m_bAllow;
};

#endif // !wxUSE_GUI

namespace gsgs
{
    /// a wxCommandEvent that takes ownership of the clientData
    class _gsgsAPI Event : public wxCommandEvent
    {
    public:
        Event(wxEventType type = wxEVT_NULL, int winid = 0);
        Event(const Event & event);
        Event & operator=(const Event & src);
        virtual ~Event();

        inline Event & SetSelected(bool selected)
        {
            this->m_selected = selected;
            return *this;
        }
        inline bool IsSelected() const                          { return m_selected; }

        // Veto management
        inline void Veto()                                      { this->m_allowed = false; }
        inline void Allow()                                     { this->m_allowed = true; }

        // Hides wxCommandEvent::Set{Get}ClientObject
        void SetClientObject(wxClientData* clientObject);

        wxClientData * GetClientObject() const;
        virtual wxEvent * Clone() const;

        inline Event & SetLineNumber(int lineNumber)
        {
            this->m_lineNumber = lineNumber;
            return *this;
        }
        inline int GetLineNumber() const                        { return m_lineNumber; }
        inline Event& SetAllowed(bool allowed)
        {
            this->m_allowed = allowed;
            return *this;
        }
        inline Event& SetAnswer(bool answer)
        {
            this->m_answer = answer;
            return *this;
        }
        inline Event& SetFileName(const wxString& fileName)
        {
            this->m_fileName = fileName;
            return *this;
        }
        inline Event& SetOldName(const wxString& oldName)
        {
            this->m_oldName = oldName;
            return *this;
        }
        inline Event& SetPtr(const wxSharedPtr<wxClientData>& ptr)
        {
            this->m_ptr = ptr;
            return *this;
        }
        inline Event& SetStrings(const wxArrayString& strings)
        {
            this->m_strings = strings;
            return *this;
        }
        inline bool IsAllowed() const                           { return m_allowed; }
        inline bool IsAnswer() const                            { return m_answer; }
        inline const wxString& GetFileName() const              { return m_fileName; }
        inline const wxString& GetOldName() const               { return m_oldName; }
        inline const wxSharedPtr<wxClientData>& GetPtr() const  { return m_ptr; }
        inline const wxArrayString& GetStrings() const          { return m_strings; }
        inline wxArrayString& GetStrings()                      { return m_strings; }

    protected:
        wxSharedPtr<wxClientData> m_ptr;
        wxArrayString m_strings;
        wxString m_fileName;
        wxString m_oldName;
        int m_lineNumber;
        bool m_answer;
        bool m_allowed;
        bool m_selected;
    };

    typedef void (wxEvtHandler::*gsgsEventFunction)(Event&);
    #define gsgsEventHandler(func) wxEVENT_HANDLER_CAST(gsgsEventFunction, func)

    class _gsgsAPI FileEvent : public Event
    {
    public:
        FileEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        FileEvent(const FileEvent& event);
        FileEvent& operator=(const FileEvent& src);

        virtual ~FileEvent();
        virtual wxEvent* Clone() const { return new FileEvent(*this); }
        inline void SetPath(const wxString& path) { this->m_path = path; }
        inline const wxString& GetPath() const { return m_path; }
        inline void SetNewpath(const wxString& newpath) { this->m_newpath = newpath; }
        inline const wxString& GetNewpath() const { return m_newpath; }
        inline void SetPaths(const wxArrayString& paths) { this->m_paths = paths; }
        inline const wxArrayString& GetPaths() const { return m_paths; }
        inline wxArrayString& GetPaths() { return m_paths; }
    protected:
        wxString m_path;
        wxString m_newpath;
        wxArrayString m_paths;
    };

    typedef void (wxEvtHandler::*gsgsFileEventFunction)(FileEvent&);
    #define gsgsFileEventHandler(func) wxEVENT_HANDLER_CAST(gsgsFileEventFunction, func)
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_MODIFIED, FileEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_NOT_FOUND, FileEvent);

    class _gsgsAPI CodeCompleteEvent : public Event
    {
    public:
        CodeCompleteEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        CodeCompleteEvent(const CodeCompleteEvent& event);
        CodeCompleteEvent& operator=(const CodeCompleteEvent& src);
        virtual ~CodeCompleteEvent();
        virtual wxEvent* Clone() const;

        inline void SetDefinitions(const wxArrayString& definitions) { this->m_definitions = definitions; }
        inline const wxArrayString& GetDefinitions() const { return m_definitions; }
        inline void SetEntry(wxCodeCompletionBoxEntry::Ptr_t entry) { this->m_entry = entry; }
        inline wxCodeCompletionBoxEntry::Ptr_t GetEntry() { return m_entry; }

        inline void SetInsideCommentOrString(bool insideCommentOrString) { this->m_insideCommentOrString = insideCommentOrString; }

        inline bool IsInsideCommentOrString() const { return m_insideCommentOrString; }
        inline void SetEditor(wxObject* editor) { this->m_editor = editor; }
        inline void SetEntries(const wxCodeCompletionBoxEntry::Vec_t& entries) { this->m_entries = entries; }
        inline const wxCodeCompletionBoxEntry::Vec_t& GetEntries() const { return m_entries; }
        inline wxCodeCompletionBoxEntry::Vec_t& GetEntries() { return m_entries; }

        inline const LSP::CompletionItem::eTriggerKind& GetTriggerKind() const { return m_triggerKind; }
        inline void SetTriggerKind(const LSP::CompletionItem::eTriggerKind& triggerKind) { this->m_triggerKind = triggerKind; }

        /**
         * @brief return the Editor object
         */
        inline wxObject* GetEditor() { return m_editor; }
        inline void SetWord(const wxString& word) { this->m_word = word; }

        /**
         * @brief return the user typed word up to the caret position
         */
        inline const wxString& GetWord() const { return m_word; }
        inline void SetPosition(int position) { this->m_position = position; }
        /**
         * @brief return the editor position
         */
        inline int GetPosition() const { return m_position; }
        inline void SetTooltip(const wxString& tooltip) { this->m_tooltip = tooltip; }
        inline const wxString& GetTooltip() const { return m_tooltip; }

    private:
        wxObject* m_editor;
        wxString m_word;
        wxString m_tooltip;
        wxCodeCompletionBoxEntry::Ptr_t m_entry;
        wxArrayString m_definitions;
        wxCodeCompletionBoxEntry::Vec_t m_entries;
        LSP::CompletionItem::eTriggerKind m_triggerKind = LSP::CompletionItem::kTriggerUnknown;
        int m_position;
        bool m_insideCommentOrString;
    };

    typedef void (wxEvtHandler::*gsgsCodeCompleteEventFunction)(CodeCompleteEvent&);
    #define clCodeCompletionEventHandler(func) wxEVENT_HANDLER_CAST(gsgsCodeCompleteEventFunction, func)

    class _gsgsAPI ColourEvent : public Event
    {
    public:
        ColourEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        ColourEvent(const ColourEvent& event);
        ColourEvent& operator=(const ColourEvent& src);
        virtual ~ColourEvent();
        virtual wxEvent* Clone() const { return new ColourEvent(*this); };

    #if wxUSE_GUI
        inline void SetBorderColour(const wxColour& borderColour) { this->m_borderColour = borderColour; }
        inline const wxColour& GetBorderColour() const { return m_borderColour; }
        inline void SetPage(wxWindow* page) { this->m_page = page; }
        inline wxWindow* GetPage() { return m_page; }
        inline void SetBgColour(const wxColour& bgColour) { this->m_bgColour = bgColour; }
        inline void SetFgColour(const wxColour& fgColour) { this->m_fgColour = fgColour; }
        inline const wxColour& GetBgColour() const { return m_bgColour; }
        inline const wxColour& GetFgColour() const { return m_fgColour; }
    #endif
        inline void SetIsActiveTab(bool isActiveTab) { this->m_isActiveTab = isActiveTab; }
        inline bool IsActiveTab() const { return m_isActiveTab; }

    private:
    #if wxUSE_GUI
        wxColour m_bgColour;
        wxColour m_fgColour;
        wxColour m_borderColour;
        wxWindow* m_page;
    #endif
        bool m_isActiveTab;
    };

    typedef void (wxEvtHandler::*gsgsColourEventFunction)(ColourEvent&);
    #define gsgsColourEventHandler(func) wxEVENT_HANDLER_CAST(gsgsColourEventFunction, func)

    class _gsgsAPI BuildEvent : public Event
    {
    public:
        BuildEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        BuildEvent(const BuildEvent& event);
        BuildEvent& operator=(const BuildEvent& src);
        virtual ~BuildEvent();
        virtual wxEvent* Clone() const { return new BuildEvent(*this); };

        inline void SetIsRunning(bool isRunning) { this->m_isRunning = isRunning; }
        inline bool IsRunning() const { return m_isRunning; }

        inline void SetKind(const wxString& kind) { this->m_kind = kind; }
        inline const wxString& GetKind() const { return this->m_kind; }

        inline void SetProjectOnly(bool projectOnly) { this->m_projectOnly = projectOnly; }
        inline bool IsProjectOnly() const { return m_projectOnly; }
        inline void SetCommand(const wxString& command) { this->m_command = command; }
        inline const wxString& GetCommand() const { return m_command; }
        inline void SetConfigurationName(const wxString& configurationName) { this->m_configurationName = configurationName; }
        inline void SetProjectName(const wxString& projectName) { this->m_projectName = projectName; }
        inline const wxString& GetConfigurationName() const { return m_configurationName; }
        inline const wxString& GetProjectName() const { return m_projectName; }

        inline void SetErrorCount(size_t errorCount) { this->m_errorCount = errorCount; }
        inline void SetWarningCount(size_t warningCount) { this->m_warningCount = warningCount; }
        inline size_t GetErrorCount() const { return m_errorCount; }
        inline size_t GetWarningCount() const { return m_warningCount; }

    private:
        wxString m_projectName;
        wxString m_configurationName;
        wxString m_command;
        wxString m_kind;
        size_t m_warningCount;
        size_t m_errorCount;
        bool m_isRunning;
        bool m_projectOnly;
    };

    typedef void (wxEvtHandler::*gsgsBuildEventFunction)(BuildEvent&);
    #define gsgsBuildEventHandler(func) wxEVENT_HANDLER_CAST(gsgsBuildEventFunction, func)

    class _gsgsAPI DebugEvent : public Event
    {
    public:
        // Special features not available by all the debuggers
        enum eFeatures {
            kStepInst = (1 << 0),         // single step instruction
            kInterrupt = (1 << 1),        // interrup the running process
            kShowCursor = (1 << 2),       // show the current active line
            kJumpToCursor = (1 << 3),     // Jump to the caret line, without executing the code in between
            kRunToCursor = (1 << 4),      // execute all the code until reaching the caret
            kReverseDebugging = (1 << 5), // execute all the code until reaching the caret
            kAllFeatures = kStepInst | kInterrupt | kShowCursor | kJumpToCursor | kRunToCursor | kReverseDebugging,
        };

    public:
        DebugEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        DebugEvent(const DebugEvent& event);
        DebugEvent& operator=(const DebugEvent& other);

        virtual ~DebugEvent();
        virtual wxEvent* Clone() const { return new DebugEvent(*this); };

        inline void SetFeatures(size_t features) { m_features = features; }
        inline size_t GetFeatures() const { return m_features; }
        inline void SetDebuggerName(const wxString& debuggerName) { this->m_debuggerName = debuggerName; }
        inline const wxString& GetDebuggerName() const { return m_debuggerName; }
        inline void SetArguments(const wxString& arguments) { this->m_arguments = arguments; }
        inline void SetCoreFile(const wxString& coreFile) { this->m_coreFile = coreFile; }
        inline void SetExecutableName(const wxString& executableName) { this->m_executableName = executableName; }
        inline void SetStartupCommands(const wxString& startupCommands) { this->m_startupCommands = startupCommands; }
        inline void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }
        inline const wxString& GetArguments() const { return m_arguments; }
        inline const wxString& GetCoreFile() const { return m_coreFile; }
        inline const wxString& GetExecutableName() const { return m_executableName; }
        inline const wxString& GetStartupCommands() const { return m_startupCommands; }
        inline const wxString& GetWorkingDirectory() const { return m_workingDirectory; }
        inline void SetConfigurationName(const wxString& configurationName) { this->m_configurationName = configurationName; }
        inline void SetProjectName(const wxString& projectName) { this->m_projectName = projectName; }
        inline const wxString& GetConfigurationName() const { return m_configurationName; }
        inline const wxString& GetProjectName() const { return m_projectName; }
        inline void SetMemoryAddress(const wxString& memoryAddress) { this->m_memoryAddress = memoryAddress; }
        inline void SetMemoryBlockSize(size_t memoryBlockSize) { this->m_memoryBlockSize = memoryBlockSize; }
        inline void SetMemoryBlockValue(const wxString& memoryBlockValue) { this->m_memoryBlockValue = memoryBlockValue; }
        inline const wxString& GetMemoryAddress() const { return m_memoryAddress; }
        inline size_t GetMemoryBlockSize() const { return m_memoryBlockSize; }
        inline const wxString& GetMemoryBlockValue() const { return m_memoryBlockValue; }

    private:
        wxString m_projectName;       // wxEVT_DBG_UI_START_OR_CONT
        wxString m_configurationName; // wxEVT_DBG_UI_START_OR_CONT
        wxString m_debuggerName; // holds the selected debugger name. wxEVT_DBG_UI_START_OR_CONT, wxEVT_DBG_UI_QUICK_DEBUG,
                                 // wxEVT_DBG_UI_CORE_FILE
        wxString m_executableName;   // This will be set for wxEVT_DBG_UI_QUICK_DEBUG and wxEVT_DBG_UI_CORE_FILE
        wxString m_coreFile;         // wxEVT_DBG_UI_CORE_FILE
        wxString m_workingDirectory; // wxEVT_DBG_UI_CORE_FILE, wxEVT_DBG_UI_QUICK_DEBUG
        wxString m_arguments;        // wxEVT_DBG_UI_QUICK_DEBUG
        wxString m_startupCommands;  // wxEVT_DBG_UI_QUICK_DEBUG
        wxString m_memoryAddress;    // wxEVT_DEBUGGER_SET_MEMORY
        wxString m_memoryBlockValue; // wxEVT_DEBUGGER_SET_MEMORY
        size_t m_memoryBlockSize;    // wxEVT_DEBUGGER_SET_MEMORY
        size_t m_features;
    };

    typedef void (wxEvtHandler::*gsgsDebugEventFunction)(DebugEvent&);
    #define gsgsDebugEventHandler(func) wxEVENT_HANDLER_CAST(gsgsDebugEventFunction, func)

    class _gsgsAPI NewProjectEvent : public Event
    {
    public:
        struct Template {
            wxString m_category;
            wxString m_categoryPng;
            wxString m_template;
            wxString m_templatePng;
            wxString m_toolchain;
            wxString m_debugger;
            bool m_allowSeparateFolder;
            Template()
                : m_allowSeparateFolder(true)
            {
            }
            typedef std::vector<NewProjectEvent::Template> Vec_t;
        };
    public:
        NewProjectEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        NewProjectEvent(const NewProjectEvent& event);
        NewProjectEvent& operator=(const NewProjectEvent& src);
        virtual ~NewProjectEvent();
        virtual wxEvent* Clone() const { return new NewProjectEvent(*this); };

        inline void SetTemplates(const NewProjectEvent::Template::Vec_t& templates) { this->m_templates = templates; }
        inline const NewProjectEvent::Template::Vec_t& GetTemplates() const { return m_templates; }
        inline NewProjectEvent::Template::Vec_t& GetTemplates() { return m_templates; }

        inline void SetDebugger(const wxString& debugger) { this->m_debugger = debugger; }
        inline void SetProjectFolder(const wxString& projectFolder) { this->m_projectFolder = projectFolder; }
        inline void SetProjectName(const wxString& projectName) { this->m_projectName = projectName; }
        inline void SetToolchain(const wxString& toolchain) { this->m_toolchain = toolchain; }
        inline const wxString& GetDebugger() const { return m_debugger; }
        inline const wxString& GetProjectFolder() const { return m_projectFolder; }
        inline const wxString& GetProjectName() const { return m_projectName; }
        inline const wxString& GetToolchain() const { return m_toolchain; }
        inline void SetTemplateName(const wxString& templateName) { this->m_templateName = templateName; }
        inline const wxString& GetTemplateName() const { return m_templateName; }

    protected:
        NewProjectEvent::Template::Vec_t m_templates;
        wxString m_toolchain;
        wxString m_debugger;
        wxString m_projectName;
        wxString m_projectFolder;
        wxString m_templateName;
    };

    typedef void (wxEvtHandler::*gsgsNewProjectEventFunction)(NewProjectEvent&);
    #define gsgsNewProjectEventHandler(func) wxEVENT_HANDLER_CAST(gsgsNewProjectEventFunction, func)

    class _gsgsAPI CompilerEvent : public Event
    {
    public:
        CompilerEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        CompilerEvent(const CompilerEvent& event);
        CompilerEvent& operator=(const CompilerEvent& src);
        virtual ~CompilerEvent();
        virtual wxEvent* Clone() const { return new CompilerEvent(*this); }
    };

    typedef void (wxEvtHandler::*gsgsCompilerEventFunction)(CompilerEvent&);
    #define gsgsCompilerEventHandler(func) wxEVENT_HANDLER_CAST(gsgsCompilerEventFunction, func)

    class _gsgsAPI ProcessEvent : public Event
    {
    public:
        ProcessEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        ProcessEvent(const ProcessEvent& event);
        ProcessEvent& operator=(const ProcessEvent& src);
        virtual ~ProcessEvent();
        virtual wxEvent* Clone() const { return new ProcessEvent(*this); }

        inline void SetOutput(const wxString& output) { this->m_output = output; }
        inline void SetProcess(IProcess* process) { this->m_process = process; }
        inline const wxString& GetOutput() const { return m_output; }
        inline IProcess* GetProcess() { return m_process; }
    private:
        wxString m_output;
        IProcess* m_process;
    };

    typedef void (wxEvtHandler::*gsgsProcessEventFunction)(ProcessEvent&);
    #define gsgsProcessEventHandler(func) wxEVENT_HANDLER_CAST(gsgsProcessEventFunction, func)

    class _gsgsAPI SourceFormatEvent : public Event
    {
    public:
        SourceFormatEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        SourceFormatEvent(const SourceFormatEvent& event);
        SourceFormatEvent& operator=(const SourceFormatEvent& src);
        virtual ~SourceFormatEvent();
        virtual wxEvent* Clone() const { return new SourceFormatEvent(*this); }
        inline void SetFormattedString(const wxString& formattedString) { this->m_formattedString = formattedString; }
        inline void SetInputString(const wxString& inputString) { this->m_inputString = inputString; }
        inline const wxString& GetFormattedString() const { return m_formattedString; }
        inline const wxString& GetInputString() const { return m_inputString; }

    private:
        wxString m_inputString;
        wxString m_formattedString;
    };

    typedef void (wxEvtHandler::*gsgsSourceFormatEventFunction)(SourceFormatEvent&);
    #define gsgsSourceFormatEventHandler(func) wxEVENT_HANDLER_CAST(gsgsSourceFormatEventFunction, func)

    class _gsgsAPI ContextMenuEvent : public Event
    {
    public:
        ContextMenuEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        ContextMenuEvent(const ContextMenuEvent& event);
        ContextMenuEvent& operator=(const ContextMenuEvent& src);
        virtual ~ContextMenuEvent();
        virtual wxEvent* Clone() const { return new ContextMenuEvent(*this); }
    #if wxUSE_GUI
        inline void SetMenu(wxMenu* menu) { this->m_menu = menu; }
        inline wxMenu* GetMenu() { return m_menu; }
    #endif
        inline void SetEditor(wxObject* editor) { this->m_editor = editor; }
        inline wxObject* GetEditor() { return m_editor; }
        inline const wxString& GetPath() const { return m_path; }
        inline void SetPath(const wxString& path) { m_path = path; }

    private:
    #if wxUSE_GUI
            wxMenu* m_menu;
    #endif
        wxObject* m_editor;
        wxString m_path;
    };

    typedef void (wxEvtHandler::*gsgsContextMenuEventFunction)(ContextMenuEvent&);
    #define gsgsContextMenuEventHandler(func) wxEVENT_HANDLER_CAST(gsgsContextMenuEventFunction, func)

    class _gsgsAPI ExecuteEvent : public Event
    {
    public:
        ExecuteEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        ExecuteEvent(const ExecuteEvent& event);
        ExecuteEvent& operator=(const ExecuteEvent& src);
        virtual ~ExecuteEvent();
        virtual wxEvent* Clone() const { return new ExecuteEvent(*this); }

        inline void SetTargetName(const wxString& targetName) { this->m_targetName = targetName; }
        inline const wxString& GetTargetName() const { return m_targetName; }

    private:
        wxString m_targetName;
    };

    typedef void (wxEvtHandler::*gsgsExecuteEventFunction)(ExecuteEvent&);
    #define gsgsExecuteEventHandler(func) wxEVENT_HANDLER_CAST(gsgsExecuteEventFunction, func)

    class _gsgsAPI ProjectSettingsEvent : public Event
    {
    public:
        ProjectSettingsEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        ProjectSettingsEvent(const ProjectSettingsEvent& event);
        ProjectSettingsEvent& operator=(const ProjectSettingsEvent& src);
        virtual ~ProjectSettingsEvent();
        virtual wxEvent* Clone() const { return new ProjectSettingsEvent(*this); }
        inline void SetConfigName(const wxString& configName) { this->m_configName = configName; }
        inline void SetProjectName(const wxString& projectName) { this->m_projectName = projectName; }
        inline const wxString& GetConfigName() const { return m_configName; }
        inline const wxString& GetProjectName() const { return m_projectName; }

    private:
        wxString m_configName;
        wxString m_projectName;
    };

    typedef void (wxEvtHandler::*clProjectSettingsEventFunction)(ProjectSettingsEvent&);
    #define clProjectSettingsEventHandler(func) wxEVENT_HANDLER_CAST(clProjectSettingsEventFunction, func)


    class _gsgsAPI FindEvent : public Event
    {
    public:
        FindEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        FindEvent(const FindEvent& event);
        FindEvent& operator=(const FindEvent& src);
        virtual ~FindEvent();
        virtual wxEvent* Clone() const { return new FindEvent(*this); }

        inline void SetCtrl(wxStyledTextCtrl* ctrl) { this->m_ctrl = ctrl; }
        inline wxStyledTextCtrl* GetCtrl() { return m_ctrl; }

    private:
        wxStyledTextCtrl* m_ctrl;
    };

    typedef void (wxEvtHandler::*clFindEventFunction)(FindEvent&);
    #define clFindEventHandler(func) wxEVENT_HANDLER_CAST(clFindEventFunction, func)


    class _gsgsAPI FindInFilesEvent : public Event
    {
        wxString m_paths;
        wxString m_fileMask;
        size_t m_options = 0;
        wxString m_transientPaths;

    public:
        FindInFilesEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        FindInFilesEvent(const FindInFilesEvent& event);
        FindInFilesEvent& operator=(const FindInFilesEvent& src);
        virtual ~FindInFilesEvent();
        virtual wxEvent* Clone() const { return new FindInFilesEvent(*this); }
        inline void SetFileMask(const wxString& fileMask) { this->m_fileMask = fileMask; }
        inline void SetOptions(size_t options) { this->m_options = options; }
        inline const wxString& GetFileMask() const { return m_fileMask; }
        inline size_t GetOptions() const { return m_options; }
        inline void SetPaths(const wxString& paths) { this->m_paths = paths; }
        inline const wxString& GetPaths() const { return m_paths; }
        inline void SetTransientPaths(const wxString& transientPaths) { this->m_transientPaths = transientPaths; }
        inline const wxString& GetTransientPaths() const { return m_transientPaths; }
    };

    typedef void (wxEvtHandler::*clFindInFilesEventFunction)(FindInFilesEvent&);
    #define clFindInFilesEventHandler(func) wxEVENT_HANDLER_CAST(clFindInFilesEventFunction, func)

    // --------------------------------------------------------------
    // Parsing event
    // --------------------------------------------------------------
    class _gsgsAPI ParseEvent : public Event
    {
    public:
        ParseEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        ParseEvent(const ParseEvent& event);
        ParseEvent& operator=(const ParseEvent& src);
        virtual ~ParseEvent();
        virtual wxEvent* Clone() const { return new ParseEvent(*this); }
        inline void SetCurfileIndex(size_t curfileIndex) { this->m_curfileIndex = curfileIndex; }
        inline void SetTotalFiles(size_t totalFiles) { this->m_totalFiles = totalFiles; }
        inline size_t GetCurfileIndex() const { return m_curfileIndex; }
        inline size_t GetTotalFiles() const { return m_totalFiles; }
    private:
        size_t m_curfileIndex;
        size_t m_totalFiles;
    };

    typedef void (wxEvtHandler::*clParseEventFunction)(ParseEvent&);
    #define clParseEventHandler(func) wxEVENT_HANDLER_CAST(clParseEventFunction, func)

    class _gsgsAPI EditEventsHandler : public wxEvtHandler
    {
        wxStyledTextCtrl* m_stc;
        wxTextCtrl* m_textCtrl;
        wxComboBox* m_combo;
        bool m_noUnbind;

    private:
        void DoInitialize();

    protected:
        void OnCopy(wxCommandEvent& event);
        void OnPaste(wxCommandEvent& event);
        void OnCut(wxCommandEvent& event);
        void OnSelectAll(wxCommandEvent& event);
        void OnUndo(wxCommandEvent& event);
        void OnRedo(wxCommandEvent& event);

    public:
        EditEventsHandler(wxTextCtrl* wnd);
        EditEventsHandler(wxStyledTextCtrl* wnd);
        EditEventsHandler(wxComboBox* wnd);
        virtual ~EditEventsHandler();
        void NoUnbind() { m_noUnbind = true; }
        typedef SharedPtr<EditEventsHandler> Ptr_t;
    };

    // clientData is NULL
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_INIT_DONE,                     wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, GSGS_ProgramMinimize,                wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, GSGS_ProgramMaximize,                wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, GSGS_ProgramClose,                   wxCommandEvent);

    // wxCommandEvent::GetString() will return the node name modified
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_EDITOR_CONFIG_CHANGED,         wxCommandEvent);

    // wxCommandEvent::GetString() will return the workspace fullpath
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WORKSPACE_LOADED,              wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WORKSPACE_CLOSED,              wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WORKSPACE_CLOSING,             wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WORKSPACE_Recently,            wxCommandEvent);

    // The build configuration was changed
    // use event.GetString() to get the selected configuration name
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WORKSPACE_CONFIG_CHANGED,      wxCommandEvent);

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_VIEW_INIT_DONE,           wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_VIEW_REFRESHED,           wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_EXP_INIT_DONE,            wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_EXP_REFRESHED,            wxCommandEvent);

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_EXP_REFRESHED,            wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_EXP_REFRESHED,            wxCommandEvent);

    // clientData is the wxTreeItemId*
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_FILE_EXP_ITEM_EXPANDING,   wxCommandEvent);

    // an attempt to open a file using double click / ENTER was made
    // Use Event.GetFileName() to get the file name
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_TREE_ITEM_FILE_ACTIVATED,      Event);

    // File(s) were added to the project
    // Event type: Event
    // Use: Event::GetStrings() to get the list of files added
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PROJ_FILE_ADDED,               Event);

    // File(s) were removed from the project
    // Event type: Event
    // Use: Event::GetStrings() to get the list of files removed
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PROJ_FILE_REMOVED,             Event);

    // Project has been removed from the workspace
    // Event type: Event
    // Use: Event::GetStrings() to get the name of the removed project
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PROJ_REMOVED,                  Event);

    // Project has been added to the workspace
    // Event type: Event
    // Use: Event::GetString() to get the name of project
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PROJ_ADDED,                    Event);

    // the following 2 events are used as "transaction"
    // the first event indicates that any "wxEVT_FILE_SAVED" event sent from this point
    // is due to build process which is about to starte
    // the later event, indicates the end of that transaction
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_SAVE_BY_BUILD_END,        wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_SAVE_BY_BUILD_START,      wxCommandEvent);

    // Sent when a file has been saved.
    // Event type: Event
    // Use: Event::GetString() or Event::GetFileName() to get the name of file
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_SAVED,                    Event);

    // A file was renamed
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_RENAMED,                  FileEvent);

    // A file was deleted
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_DELETED,                  FileEvent);

    // A folder was deleted
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FOLDER_DELETED,                FileEvent);

    // A file was created
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_CREATED,                  FileEvent);

    // A folder was created
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FOLDER_CREATED,                FileEvent);

    // A file was "Save as" by the user
    // This event can also be fired if the user selected "Duplicate Tab"
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_SAVEAS,                   FileEvent);

    // clientData is list of files which have been retagged (std::vector<wxFileName>*)
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_RETAGGED,                 wxCommandEvent);

    // The active editor was changed
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_ACTIVE_EDITOR_CHANGED,         wxCommandEvent);

    // clientData is closing editor (IEditor*)
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_EDITOR_CLOSING,                wxCommandEvent);

    // clientData is NULL
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_ALL_EDITORS_CLOSING,           wxCommandEvent);

    // clientData is NULL
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_ALL_EDITORS_CLOSED,            wxCommandEvent);

    // This event is sent when the user clicks inside an editor
    // this event can not be Veto()
    // clientData is NULL. You may query the clicked editor by calling to
    // IManager::GetActiveEditor()
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_EDITOR_CLICKED,                wxCommandEvent);

    // User dismissed the Editor's settings dialog with
    // Apply or OK (Settings | Editor)
    // clientData is NULL
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_EDITOR_SETTINGS_CHANGED,       wxCommandEvent);

    // This event is sent from plugins to the application to tell it to reload
    // any open files (and re-tag them as well)
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED, wxCommandEvent);

    // Same as wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED
    // just without prompting the user
    // this event only reload code files without
    // any reload to the workspace / project
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT, wxCommandEvent);

    // Sent by the project settings dialogs to indicate that
    // the project configurations are saved
    // Event type: ProjectSettingsEvent
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_PROJ_SETTINGS_SAVED,       ProjectSettingsEvent);

    // A user requested to execute the active project
    // Event type:
    // ExecuteEvent.
    // Get the target to run by using ExecuteEvent::GetTargetName
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_EXECUTE_ACTIVE_PROJECT,    ExecuteEvent);

    // A user requested to stop the previously executed program
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_STOP_EXECUTED_PROGRAM,     ExecuteEvent);

    // gsgs sends this event to query plugins incase there is a program running
    // use evet.SetAnswer(true) to indicate that the plugin has launched an executable
    // it is mainly used for displaying the 'Stop' button in the toolbar as active/disabled
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_IS_PROGRAM_RUNNING,        ExecuteEvent);

    // Event type: BuildEvent
    // Notify that a build has been started
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_BUILD_STARTED,                 BuildEvent);

    // Event type: BuildEvent
    // Notify that a build has been ended
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_BUILD_ENDED,                   BuildEvent);

    // Event type: BuildEvent
    // sent by the application to the plugins to indicate that a
    // build process is about to start. by handling this event
    // and by avoid calling event.Skip() gsgs will NOT start
    // the build process
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_BUILD_STARTING,                BuildEvent);

    // Event type: BuildEvent
    // return the project clean command
    // set the new clean command using: event.SetCommand( "new-clean-command" );
    // avoid calling event.Skip() to indicate that a new command was provided
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_GET_PROJECT_CLEAN_CMD,         BuildEvent);

    // Event type: BuildEvent
    // return the project build command
    // set the new build command using: event.SetCommand( "new-build-command" );
    // avoid calling event.Skip() to indicate that a new command was provided
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_GET_PROJECT_BUILD_CMD,         BuildEvent);

    // Event type: BuildEvent
    // Set the 'SetRunning' to true to reply to gsgs
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_GET_IS_BUILD_IN_PROGRESS,      BuildEvent);

    // Event type: BuildEvent
    // Stop the current build process. Call event.Skip(false) to indicate that the
    // 'stop' process is done by the plugin
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_STOP_BUILD,                    BuildEvent);

    // Event type: BuildEvent
    // the below two events are sent by the application to the plugins to query whether a
    // given project and build configuration are handled by the plugin.
    // the first event is whether the makefile file is generated by the plugin
    // and the second is to know whether the build process is also being
    // handled by the plugin.
    // the return answer is done by simply avoid calling event.Skip() (which will result in ProcessEvent() == true)
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_GET_IS_PLUGIN_MAKEFILE,        BuildEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_GET_IS_PLUGIN_BUILD,           BuildEvent);

    // Event type: BuildEvent
    // These events allows the plugins to concatenate a string
    // to the compilation/link line of the default build system
    // By using the event.SetCommand()/event.GetCommand()
    // Note, that the since all multiple plugins
    // might be interesting with this feature, it is recommened
    // to use it like this:
    // wxString content = event.GetCommand();
    // content << wxT(" -DMYMACRO ");
    // event.SetCommand( content );
    // event.Skip();
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_GET_ADDITIONAL_COMPILEFLAGS,   BuildEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_GET_ADDITIONAL_LINKFLAGS,      BuildEvent);

    // Evnet type: BuildEvent
    // Sent to the plugins to request to export the makefile
    // for the project + configuration
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PLUGIN_EXPORT_MAKEFILE,        BuildEvent);

     // sent when the debugger is about to start
     // clientData is a pointer to a DebuggerStartupInfo structure
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUG_STARTING,                DebugEvent);

    // sent right after the debugger started; program is not running yet
    // clientData is a pointer to a DebuggerStartupInfo structure
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUG_STARTED,                 DebugEvent);

    // sent just before the debugger stops
    // clientData is NULL
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUG_ENDING,                  DebugEvent);

    // sent after the debugger stopped
    // clientData is NULL
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUG_ENDED,                   DebugEvent);

    // set when the editor gains or loses
    // the control over the debugger
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUG_EDITOR_LOST_CONTROL,     wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUG_EDITOR_GOT_CONTROL,      wxCommandEvent);

    // Notify the debugger to update the active pane
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUGGER_REFRESH_PANE,         DebugEvent);

    // Instruct the debugger to update the memory
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUGGER_SET_MEMORY,           DebugEvent);

    // User selected an entry from the code completion box. call event.Skip(false)
    // if you wish to perform something unique instead of the default "insert selection into editor"
    // The selected string can be retrieved by calling: event.GetWord()
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CCBOX_SELECTION_MADE,          CodeCompleteEvent);

    // Code completion box is about to be shown. Let the plugins access the list before it is shown
    // to the user. Call event.Skip(false) if you modify the list. The modified list should be set
    // via the event.SetEntries();
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CCBOX_SHOWING,                 CodeCompleteEvent);

    // The code completion box has been dismissed
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CC_CODE_COMPLETE_BOX_DISMISSED, CodeCompleteEvent);

    // User has requested to display the current files' outline
    // Use m_mgr->GetActiveEditor() to get the active editor
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CC_SHOW_QUICK_OUTLINE,         CodeCompleteEvent);

    // Send a CodeCompleteEvent
    // gsgs is about to show the completion box for language keywords
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CC_CODE_COMPLETE_LANG_KEYWORD, CodeCompleteEvent);

    // Send CodeCompleteEvent
    // User clicked on hyper link inside the editor
    // If the plugin wants to handle it, simple avoid calling event.Skip()
    // Use event.GetString() to get the hyperlink text
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CC_JUMP_HYPER_LINK,            CodeCompleteEvent);

    //===----------------------------------------------------------------------------------------------
    //===----------------------------------------------------------------------------------------------
    //      Below events are processed via the ServiceManager class and NOT by EventManager!!
    //      To be able to receive them your handler must subclass ServiceProvider
    //===----------------------------------------------------------------------------------------------
    //===----------------------------------------------------------------------------------------------

    // Search the workspace for a symbol
    // Use evt.GetWord() to get the searched string
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CC_FIND_SYMBOL,                CodeCompleteEvent);

    // Find symbol declaration. Use evt.GetWord() to get the searched string
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CC_FIND_SYMBOL_DECLARATION,    CodeCompleteEvent);

    // Find symbol definition. Use evt.GetWord() to get the searched string
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CC_FIND_SYMBOL_DEFINITION,     CodeCompleteEvent);

    // Request for code completion
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CC_CODE_COMPLETE,              CodeCompleteEvent);

    /// User asked for "word completion" (non context code completion event)
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CC_WORD_COMPLETE,              CodeCompleteEvent);

    // A function calltip is requesed
    // clientData is set to the client data set by the user
    // the plugin returns the tooltip to the IDE using the:
    // evt.SetTooltip(..) method
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, CodeCompleteEvent);

    // User is hovering a text, display the typeinfo
    // IEditor* editor = dynamic_cast<IEditor*>(evt.GetEditor());
    // Hover position is set in the evt.GetPosition()
    // To pass a new tooltip, just call event.SetTooltip(...)
    // gsgs will display the tooltip if a non empty string is passed. Simple markup is allowed (<br> <hr> etc)
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CC_TYPEINFO_TIP,               CodeCompleteEvent);

    // Event type: CodeCompleteEvent
    // Sent by gsgs to generate documentation block for class or function
    // after the user has typed "/**" followed by ENTER
    // Use event.SetTooltip(...) to provide the doxygen block comment
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CC_GENERATE_DOXY_BLOCK,        CodeCompleteEvent);

    // An update to the navigation bar is required
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CC_UPDATE_NAVBAR,              CodeCompleteEvent);

    // User typed '@' in a block comment section
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CC_BLOCK_COMMENT_CODE_COMPLETE, CodeCompleteEvent);

    // User typed Ctrl-Space in a block comment section
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CC_BLOCK_COMMENT_WORD_COMPLETE, CodeCompleteEvent);


    // User selected an option to create a new workspace
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_CREATE_NEW_WORKSPACE,      Event);

    // Event type: Event
    // A workspace file was selected by the user.
    // use event.GetFileName() to get the file name
    // If the plugin wishes to process this file, it should call
    // event.Skip(false) on the event
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_OPEN_WORKSPACE,            Event);

    // Event type: Event
    // User requested to close the workspace.
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_CLOSE_WORKSPACE,           Event);

    // Event type: Event
    // This event is sent by gsgs to the plugins to query whether a
    // a custom workspace is opened (i.e. a worksapce which is completely managed
    // by the plugin) this allows gsgs to enable menu items which otherwise
    // will be left disabled
    // to return a true or false reply to gsgs, use
    // evt.SetAnswer( true / false )
    // The workspace name should also returned in the evt.SetString(..)
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_IS_WORKSPACE_OPEN,         Event);

    // User has requested a retagging for the workspace
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_RETAG_WORKSPACE,           wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_RETAG_WORKSPACE_FULL,      wxCommandEvent);

    // gsgs has requested a complete list of the workspace files.
    // The result should be wxArrayString that contains a list of files
    // in their ABSOLUTE path.
    // The wxArrayString IS OWNED by gsgs
    // and the plugin should NOT attempt to delete it
    // <code>
    // if(I_want_to_handle_this_event) {
    // 		wxArrayString *files = (wxArrayString *) event.GetClientData();
    // 		<fill the files* array ...>
    // } else {
    // 		event.Skip();
    // }
    // </code>
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_GET_WORKSPACE_FILES,       wxCommandEvent);
    // Same as the above event, however you should return
    // a list of the current active project files
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_GET_ACTIVE_PROJECT_FILES,  wxCommandEvent);
    // Same as the above event, however you should return
    // a list of the current files' project files list
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_GET_CURRENT_FILE_PROJECT_FILES, wxCommandEvent);

    // User requested to open the resource dialog
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_OPEN_RESOURCE,             wxCommandEvent);

    // gsgs is about to display the editor's context menu
    // A plugin can override the default menu display by catching this event and
    // handling it differently
    // event.GetEventObject() holds a pointer to the editor triggered
    // the menu
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_EDITOR_CONTEXT_MENU,       wxCommandEvent);

    // Sent when the parser thread has completed a tagging request
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_RETAG_COMPLETED,           wxCommandEvent);

    // send a wxNotifyEvent about page closing.
    // This event can be vetoed
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_NOTIFY_PAGE_CLOSING,           wxNotifyEvent);

    // send an wxCommandEvent indicating that the mainbook page has changed.
    // Unlike 'wxEVT_ACTIVE_EDITOR_CHANGED' this event is only sent when the
    // 'page' is not of type IEditor
    // use event.GetClientData() to get a pointer to the wxWindow*
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_PAGE_CHANGED,              wxCommandEvent);

    // Triggers a workspace view tree rebuild (useful when user has modified the workspace / project structure and he want
    // to reflect the changes in the tree view)
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_REBUILD_WORKSPACE_TREE,        wxCommandEvent);

    // Sent when user has changed the active project
    // Event type: ProjectSettingsEvent
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_ACTIVE_PROJECT_CHANGED,        ProjectSettingsEvent);

    // This event is fired by gsgs before the find bar is shown
    // The handler can pass a wxStyledTextCtrl class pointer to be associated with the
    // find bar by using the FindEvent::SetCtrl() method
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FINDBAR_ABOUT_TO_SHOW,         FindEvent);

    // A plugin *must* send this event to ask the find bar to release any window associated with it
    // This event should be used with  'wxEVT_FINDBAR_ABOUT_TO_SHOW'. If this event is not sent when the window
    // is destroyed - it might result in a crash
    // The window pointer is passed using event.SetCtrl()
    // If the editor managed by the find-bar is the same as event.GetCtrl() -> the find-bar will un-refernce it
    // but *IT DOES NOT FREE ITS MEMORY*
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FINDBAR_RELEASE_EDITOR,        FindEvent);

    // Fired when gsgs about to show the find-in-files dialog
    // A plugin can override the default 'file mask' and/or the 'search in' fields
    // to do that, it needs to set the new values in the event and call event.Skip(false) to indicate that the event
    // has been processed
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FINDINFILES_DLG_SHOWING,       FindInFilesEvent);

    // Fired when gsgs dismissed the find-in-files dialog
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FINDINFILES_DLG_DISMISSED,     FindInFilesEvent);

    // Instruct gsgs to build a project only ( no deps )
    // the project name is passed in the wxCommandEvent::GetString
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_BUILD_PROJECT_ONLY,            wxCommandEvent);
    // Instruct gsgs to clean a project only ( no deps )
    // the project name is passed in the wxCommandEvent::GetString
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_CLEAN_PROJECT_ONLY,            wxCommandEvent);

    // User changed the default theme color of gsgs
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_GSGS_THEME_CHANGED,                  wxCommandEvent);

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_LOAD_SESSION,                      wxCommandEvent);

    // Sent from the CodeFormatter plugin when a file indentation was completed
    // event.GetString() will return the name of the indented file
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CODEFORMATTER_INDENT_COMPLETED,    wxCommandEvent);

    // Sent from the CodeFormatter plugin when a file indentation is about to begin
    // event.GetString() will return the name of the indented file
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CODEFORMATTER_INDENT_STARTING,     wxCommandEvent);

    // Sent when gsgs's main frame gained the focus
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_GSGS_MAINFRAME_GOT_FOCUS,      wxCommandEvent);

    // User clicked on a project item in the treeview
    // the event.GetString() contains the project name that was clicked
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PROJECT_TREEITEM_CLICKED,          wxCommandEvent);

    // user has deleted all the breakpoints using the "Breakpoints" table
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_GSGS_ALL_BREAKPOINTS_DELETED,  wxCommandEvent);

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_NEW_DOCKPANE,                  wxCommandEvent);

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_DELETE_DOCKPANE,               wxCommandEvent);

    // User right click while holding the code-navigation modifier key (default to Shift)
    // send a CodeCompleteEvent
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CC_SHOW_QUICK_NAV_MENU,            CodeCompleteEvent);

    // Event type: Event
    // User requested to reload the workspace
    // simply avoid calling event.Skip() to indicate to gsgs that this event
    // was handled by a plugin
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_RELOAD_WORKSPACE,              Event);

    // Event type: ColourEvent
    // Sent by gsgs whenever it needs to colour a single tab
    // avoid calling event.Skip() to notify gsgs that the plugin wants to place a sepcial
    // colour. The colours (*plural*) should be passed using the 'event.SetFgColour()' and 'event.SetBgColour()'
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_COLOUR_TAB,                        ColourEvent);

    // Event type: Event
    // Sent by gsgs before it starts building the "Workspsace View" tree view.
    // User may provide a different image list by placing it inside the event
    // event.SetClientData() member and by calling event.Skip(false)
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WORKSPACE_VIEW_BUILD_STARTING,     Event);

    // Event type: ColourEvent
    // Sent by gsgs adding a project item to the file-view ("Workspace View"). Use event.GetString() to get the project
    // name
    // User may alter the following attributes:
    // . Project item background colour
    // . Project item text colour
    // . Project icon
    // The image id is returned as INT event.SetInt()
    // Avoid calling event.Skip() to notify gsgs that a new attributes were provided
    // IMPORTANT:
    // gsgs will not check the validity of the icon index. Its up to the plugin to provide a proper
    // index image. One can use the event wxEVT_WORKSPACE_VIEW_BUILD_STARTING to replace the default
    // image list with its own list
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WORKSPACE_VIEW_CUSTOMIZE_PROJECT,  ColourEvent);

    // Event type: ColourEvent
    // Sent by gsgs whenever it needs the pen colour for the tab area
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_GET_TAB_BORDER_COLOUR,             ColourEvent);

    // ---------------------------------------------------------------------
    // Debugger events
    // The following events are firing the DebugEvent class
    // If a plugin wishes to override gsgs's default debugger (gdb)
    // it simply needs to connect the event and avoid calling 'event.Skip();
    //----------------------------------------------------------------------
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_START, DebugEvent); // Start. This event is fired when a debug session is starting. The plugin should also set the
                       // "feaures" field to indicate which features are available by the debugger
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_CONTINUE, DebugEvent);  // Continue
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_STOP, DebugEvent);      // Stop the debugger
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_STEP_IN, DebugEvent);   // Step into function
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_STEP_I, DebugEvent);    // Step into instruction
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_STEP_OUT, DebugEvent);  // Step out of current frame
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_NEXT, DebugEvent);      // Next line
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_NEXT_INST, DebugEvent); // Next instruction
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_INTERRUPT, DebugEvent); // Interrupt the debugger execution
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_SHOW_CURSOR,
        DebugEvent); // Set the focus to the current debugger file/line
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_RESTART, DebugEvent); // Restart the debug session
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_IS_RUNNING, DebugEvent); // Use evet.SetAnswer() method to reply
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_TOGGLE_BREAKPOINT,
        DebugEvent); // Toggle breakpoint. Use event.GetFileName() / event.GetInt() for the file:line
    // Can gsgs interact with the debugger? use event.SetAnswer(true);
    // Note: by avoid calling Skip() gsgs will assume that the plugin is controlling the debug session
    // and it will use the event.IsAnswer() as the answer to the question to : CanDbgInteract()
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_CAN_INTERACT, DebugEvent);
    // Provide a tooltip for the expression under the caret. user event.GetString() to get the expression
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_EXPR_TOOLTIP, DebugEvent);

    // This event is sent by gsgs to all plugins to determine whether a plugin is actually a debugger.
    // A plugin should *always* call event.Skip() when handling this event. If the plugin is actually a debugger
    // plugin, it should add itself like this: event.GetStrings().Add("<the-debugger-name")
    // This string is later will be availe for gsgs to display it in various dialogs (e.g. Quick Debug, project settings
    // etc)
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_IS_PLUGIN_DEBUGGER,            DebugEvent);

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_QUICK_DEBUG,
        DebugEvent); // User clicked on the 'Quick Debug' button. Event type is DebugEvent
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_CORE_FILE,
        DebugEvent); // User selected to debug a core file. Event type is DebugEvent
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_ATTACH_TO_PROCESS,
        DebugEvent); // Attach to process. Use DebugEvent::GetInt() to get the process ID
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_DELETE_ALL_BREAKPOINTS,     DebugEvent);  // Delete all breakpoints
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_ENABLE_ALL_BREAKPOINTS,     DebugEvent);  // Enable all breakpoints
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DBG_UI_DISABLE_ALL_BREAKPOINTS,    DebugEvent); // Disable all breakpoints

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_OPEN_PROJ_SETTINGS,            Event);

    // Workspace reload started
    // event type: Event
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WORKSPACE_RELOAD_STARTED,          Event);

    // Workspace reload is done
    // event type: Event
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WORKSPACE_RELOAD_ENDED,            Event);

    // event type: NewProjectEvent
    // Use this event to add new templates / categories to the wizard
    // If you don't call event.Skip() gsgs will assume that the plugin
    // is replacing the dialog with its own and will do nothing
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_NEW_PROJECT_WIZARD_SHOWING,        NewProjectEvent);

    // event type: NewProjectEvent
    // User clicked on the 'Finish' button of the new project wizard dialog
    // call event.Skip( false ) if the plugin wants to handle the new project, otherwise
    // call event.Skip( true ) for gsgs to run the default behvior
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_NEW_PROJECT_WIZARD_FINISHED,       NewProjectEvent);

    // The compiler list was updated (e.g. a compiler was deleted, renamed etc)
    // Event type: CompilerEvent
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_COMPILER_LIST_UPDATED,             CompilerEvent);

    // User clicked on the 'Save All Editors' button
    // Editors will be saved, however, if a plugin has a tab which is not an editor
    // save it as well
    // event type: Event
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SAVE_ALL_EDITORS,                  Event);

    // Format an input string
    // Event type: SourceFormatEvent
    // Use SourceFormatEvent.SetInputString() / GetFormattedString()
    // to get and set the string to format
    // Note that the plugin should be "smart" enough to indent the file
    // by its type
    // Call event.Skip(false) to mark this event as completed
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FORMAT_STRING,                     SourceFormatEvent);

    // Format a file
    // Event type: SourceFormatEvent
    // Use event.SetFileName() to pass the file name to be formatted
    // Note that the plugin should be "smart" enough to indent the file
    // by its type
    // Call event.Skip(false) to mark this event as completed
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FORMAT_FILE,                       SourceFormatEvent);

    // gsgs is about to show the context menu for an editor
    // Event type: ContextMenuEvent. You can use event.GetEditor() to get the editor
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CONTEXT_MENU_EDITOR,               ContextMenuEvent);

    // gsgs is about to show the context menu for the editor margin
    // Event type: ContextMenuEvent. You can use event.GetEditor() to get the editor
    // Calling event.Skip(false) will cancel the menu. A plugin may also
    // alter the context menu
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CONTEXT_MENU_EDITOR_MARGIN,        ContextMenuEvent);

    // A context menu for a folder is about to be shown, you may alter the menu using event.GetMenu()
    // Use event.GetPath() to get the path of the folder
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CONTEXT_MENU_FOLDER,               ContextMenuEvent);

    // A context menu for a virtual folder is about to be shown, you may alter the menu using event.GetMenu()
    // Use event.GetPath() to get the virtual folder path
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CONTEXT_MENU_VIRTUAL_FOLDER,       ContextMenuEvent);

    // A context menu for a file is about to be shown, you may alter the menu using event.GetMenu()
    // use event.GetStrings() to get the list of files selected (since there can be multiple items, we use
    // event.GetStrings() a not event.GetFileName())
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CONTEXT_MENU_FILE,                 ContextMenuEvent);

    // A context menu for a project is about to be shown, you may alter the menu using event.GetMenu()
    // use event.GetFileName() to get the project file (if any). event.GetString() will return the project name
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CONTEXT_MENU_PROJECT,              ContextMenuEvent);

    // A context menu for a file is about to be shown, you may alter the menu using event.GetMenu()
    // use event.GetFileName() to get the workspace file (if any). event.GetString() will return the workspace name
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CONTEXT_MENU_WORKSPACE,            ContextMenuEvent);

    // A context menu for the active tab label header is about to be shown. You can get the active page
    // by calling GetManager()->GetActivePage()
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CONTEXT_MENU_TAB_LABEL,            ContextMenuEvent);

    // User modified the colours and font of the IDE
    // event type: Event
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CMD_COLOURS_FONTS_UPDATED,         Event);

    // File has been loaded into the IDE
    // User: clCommandEvnet::GetFileName() to get the file name
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_LOADED,                       Event);

    // File has been closed
    // User: clCommandEvnet::GetFileName() to get the file name
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_CLOSED,                       Event);

    // Sent when gsgs is about to set the main frame's title.
    // A plugin may alter the frame title by calling event.SetString(..)
    // To get the current frame title, a plugin may call event.GetString()
    // Event type: Event
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CL_FRAME_TITLE,                    Event);

    // Event: Event
    // gsgs is about to save a file. Use event.GetFilename() to get the file name
    // This event can be vetoed by calling event.Veto()
    // Note that this event is sent for editor files (i.e. it is not sent for internal files like project file being saved
    // etc) only for editors (hence the name)
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_BEFORE_EDITOR_SAVE, Event);

    // Editor has been modified. Use event.GetFilename() to get the file name of the editor
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_EDITOR_MODIFIED, Event);

    // Event: Event
    // Sent when clang code completion encountered an error
    // use: event.GetString() to get the error message
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CLANG_CODE_COMPLETE_MESSAGE, Event);

    // Event: Event
    // gsgs is going down. This event can not be vetoed
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_GOING_DOWN, Event);

    // Event: Event
    // Use GetString() to get the new project name
    // Use GetOldName() to get the old project name
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PROJ_RENAMED, Event);

    // An editor is being instantiated
    // gsgs will send a Event with the editor pointer
    // set in the event.GetEventObject() member
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_EDITOR_INITIALIZING, Event);

    // Event: FileEvent
    // Sent when the file system was modified externally (typically, this event is sent
    // after git pull, svn update etc)
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILE_SYSTEM_UPDATED, FileEvent);

    // Event: FileEvent
    // List of files were modified by the replace-in-files operation (this event will only include
    // the files that were modified on the file system directly and have no open editor)
    // Use FileEvent::GetStrings to get list of modified files
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_FILES_MODIFIED_REPLACE_IN_FILES, FileEvent);

    // Event: Event
    // Sent when gsgs requires to store the current workspace session
    // Call event.Skip(false) to instruct gsgs to skip the default session-save
    // action (this is useful if the session is managed by an external plugin)
    // A good example for this is the PHP plugin which manages its own session
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SAVE_SESSION_NEEDED, Event);

    // Event: Event
    // User modified its environment variables
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_ENVIRONMENT_VARIABLES_MODIFIED, Event);

    // Event: Event
    // user dropped a folder on the window
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DND_FOLDER_DROPPED, Event);

    // Event: Event
    // user dropped a file on the window
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DND_FILE_DROPPED, Event);

    // Event: Event
    // a gsgs restart is required
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_RESTART_GSGS, Event);

    // Event: Event
    // Toggle workspace view tab. Use event.IsSelected() to test whether we should hide/show the tab
    // In anycase, you should not destroy the window, just hide it
    // The tab name is set in the event.GetString()
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SHOW_WORKSPACE_TAB, Event);

    // Event: Event
    // Toggle output view tab. Use event.IsSelected() to test whether we should hide/show the tab
    // In anycase, you should not destroy the window, just hide it
    // The tab name is set in the event.GetString()
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SHOW_OUTPUT_TAB, Event);

    // Event: Event
    // Toggle output view tab. Use event.IsSelected() to test whether we should hide/show the tab
    // In anycase, you should not destroy the window, just hide it
    // The tab name is set in the event.GetString()
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SHOW_Perspective_TAB, Event);

    // Event: Event
    // Sent by gsgs when the active editor is _not_ an IEditor instance
    // but something else (probably provided by the plugins)
    // and gsgs has no way of telling if the "Save" icon should be enabled or not
    // In case the window in the question is modified and owned by the plugin, the plugin should:
    //     1. Call event.SetAnswer(true)
    //     2. Call event.Skip(false);
    //
    // In case the window in the question is not owned by the plugin, the plugin should:
    //     1. Call event.Skip();
    //
    // Use: event.GetClientData() to get the pointer to the window page
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PAGE_MODIFIED_UPDATE_UI, Event);

    // Sent when PHP settings have been saved to the file system
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PHP_SETTINGS_CHANGED, Event);

    // Build configuration for the workspace has changed
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WORKSPACE_BUILD_CONFIG_CHANGED, Event);

    // The navigation bar is about show the scope drop down menu
    // Build the menu using ContextMenuEvent::GetMenu()
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_NAVBAR_SCOPE_MENU_SHOWING, ContextMenuEvent);

    // User selected an entry from the scope drop down menu. GetString() contains the selected string
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_NAVBAR_SCOPE_MENU_SELECTION_MADE, Event);

    // A marker was added/removed from an editor. Event::GetFileName() will return the relevant file name
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_MARKER_CHANGED, Event);

    // User click a button in the info bar
    // Use: event.GetInt() to get the button ID clicked
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_INFO_BAR_BUTTON, Event);

    class _gsgsAPI EventManager : public wxEvtHandler
    {
    public:
        static EventManager& singleton();
        static void Release();

        virtual void AddPendingEvent(const wxEvent& event);
        virtual bool ProcessEvent(wxEvent &event);

        inline void DisableEvents(bool b)           { mDisable = b; }

        inline bool IsEventsDiabled() const         { return mDisable; }
#if wxUSE_GUI
        wxFrame* TopFrame();
#endif
        bool SendCommandEvent(int eventId, void *clientData);
        void PostCommandEvent(int eventId, void *clientData);
        bool SendCommandEvent(int eventId, void *clientData, const wxString &s);

        // --------------------------------------------------------
        // About the following functions:
        // the below functions are meant to provide an easy 
        // way to fire gsgs's plugins events without the need
        // to create the event on the stack.
        // Post* are meant as Asnyc event ("AddPendingEvent")
        // While Notify* are synchronous event ("ProcessEvent")
        //---------------------------------------------------------

        /**
         * @brief post a wxEVT_FILE_SAVED event
         */
        void PostFileSavedEvent(const wxString &filename);

        /**
         * @brief post a wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT or wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED
         * @param prompt
         */
        void PostReloadExternallyModifiedEvent(bool prompt = true);

        /**
         * @brief post a wxEVT_PROJ_FILE_REMOVED event
         */
        void PostFileRemovedEvent(const wxArrayString & files);

        /**
         * @brief send a wxEVT_WORKSPACE_RELOAD_STARTED event (sync event)
         */
        void NotifyWorkspaceReloadStartEvet(const wxString & workspaceFile);

        /**
         * @brief send a wxEVT_WORKSPACE_RELOAD_ENDED event (sync event)
         */
        void NotifyWorkspaceReloadEndEvent(const wxString & workspaceFile);
    private:
        EventManager();
        virtual ~EventManager();
    private:
        bool mDisable;
    };

    class _gsgsAPI ServiceProvider : public wxEvtHandler
    {
    public:
        typedef std::vector<ServiceProvider*> Vec_t;

    public:
        ServiceProvider(const wxString & name, eServiceType type);
        virtual ~ServiceProvider();
        inline void SetName(const wxString& name)      { m_name = name; }
        void SetPriority(int priority);
        inline void SetType(const eServiceType& type)  { m_type = type; }
        inline const wxString& GetName() const         { return m_name; }
        inline int GetPriority() const                 { return m_priority; }
        inline const eServiceType& GetType() const     { return m_type; }
    private:
        wxString m_name;
        eServiceType m_type = eServiceType::kUnknown;
        int m_priority = 50;
    };

    // Similar to gsgsEventManager class.
    // But process events in order. Each 'handler' (aka: ServiceProvider)
    // Registers itself for a service with a priority. The higher the prio, it will get
    // called first
    class _gsgsAPI ServiceManager : public wxEvtHandler
    {
    public:
        static ServiceManager& Get();

        void UnregisterAll();

        /**
         * @brief process service event
         */
        virtual bool ProcessEvent(wxEvent & event);
        
        /**
         * @brief sort the service providers for a given type
         */
        void Sort(eServiceType type);
    protected:
        ServiceManager();
        virtual ~ServiceManager();

        void Register(ServiceProvider * provider);
        void Unregister(ServiceProvider * provider);
        eServiceType GetServiceFromEvent(wxEvent & event);
    private:
        std::unordered_map<eServiceType, ServiceProvider::Vec_t> m_providers;
        friend class ServiceProvider;
    };
}
#define gsgs_Event() gsgs::EventManager::singleton()
#endif
