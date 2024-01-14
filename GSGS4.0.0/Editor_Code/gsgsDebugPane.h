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
#ifndef gsgsDEBUGGERPANE_H
#define gsgsDEBUGGERPANE_H

#include "gsgsPreInclude.h"
#include "gsgsNotebook.h"
#include "gsgsConfig.h"
#include "gsgsDebuggerManager.h"
#include "gsgsCompilerSettings.h"
#include <wx/imaglist.h>
#include <wx/splitter.h>

class DebuggerCallstackView;
class BreakpointDlg;
class ThreadListPanel;
class wxAuiManager;

#define LIST_LOCALS_CHILDS              600
#define QUERY_LOCALS_CHILDS             601
#define QUERY_LOCALS_CHILDS_FAKE_NODE   602

namespace gsgs
{
    class DebugTab;
    class MemoryView;

    class DebuggerTreeListCtrlBase : public wxPanel
    {
    public:
        DebuggerTreeListCtrlBase(wxWindow* parent, wxWindowID id = wxID_ANY, bool withButtonsPane = true, 
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500, 300), long style = wxTAB_TRAVERSAL);

        virtual ~DebuggerTreeListCtrlBase();
        ToolBar* GetToolbar() const                         { return m_toolbar; }
        DefaultTreeCtrl* GetListTable() const               { return m_listTable; }
        
        virtual IDebugger* DoGetDebugger();
        virtual void DoResetItemColour(const wxTreeItemId& item, size_t itemKind);
        virtual void OnEvaluateVariableObj(const DebuggerEventData& event);
        virtual void OnCreateVariableObjError(const DebuggerEventData& event);
        virtual void DoRefreshItemRecursively(IDebugger* dbgr, const wxTreeItemId& item, wxArrayString& itemsToRefresh);
        virtual void Clear();
        virtual void DoRefreshItem(IDebugger* dbgr, const wxTreeItemId& item, bool forceCreate);
        virtual wxString DoGetGdbId(const wxTreeItemId& item);
        virtual wxTreeItemId DoFindItemByGdbId(const wxString& gdbId);
        virtual void DoDeleteWatch(const wxTreeItemId& item);
        virtual wxTreeItemId DoFindItemByExpression(const wxString& expr);
        virtual void ResetTableColors();
        virtual wxString GetItemPath(const wxTreeItemId& item);
    protected:
        virtual void OnListEditLabelBegin(wxTreeEvent& event);
        virtual void OnListEditLabelEnd(wxTreeEvent& event);
        virtual void OnItemRightClick(wxTreeEvent& event);
        virtual void OnListKeyDown(wxTreeEvent& event);
        virtual void OnItemExpanding(wxTreeEvent& event);
        virtual void OnSortItems(wxCommandEvent& event);
        virtual void OnRefreshUI(wxUpdateUIEvent& event);
        virtual void OnDeleteWatch(wxCommandEvent& event);
        virtual void OnDeleteWatchUI(wxUpdateUIEvent& event);
        virtual void OnNewWatch(wxCommandEvent& event);
        virtual void OnNewWatchUI(wxUpdateUIEvent& event);
        virtual void OnRefresh(wxCommandEvent& event);
    protected:
        std::map<wxString, wxTreeItemId> m_gdbIdToTreeId;
        std::map<wxString, wxTreeItemId> m_listChildItemId;
        std::map<wxString, wxTreeItemId> m_createVarItemId;
        DbgStackInfo m_curStackInfo;

        int m_DBG_USERR;
        int m_QUERY_NUM_CHILDS;
        int m_LIST_CHILDS;
        ToolBar* m_toolbar;
        DefaultTreeCtrl* m_listTable;
        bool m_withButtons;
    };
    
    class LocalsTable : public DebuggerTreeListCtrlBase
    {
    public:
        LocalsTable(wxWindow * parent);
        virtual ~LocalsTable();

        /**
         * @brief callback to IDebugger::CreateVariableObject
         * @param event
         */
        void OnCreateVariableObj(const DebuggerEventData& event);
        /**
         * @brief callback to IDebugger::ListChildren
         */
        void OnListChildren(const DebuggerEventData& event);
        /**
         * @brief called to IDEbugger::UpdateVariableObject
         */
        void OnVariableObjUpdate(const DebuggerEventData& event);

        void UpdateLocals(const LocalVariables& locals);
        void UpdateFrameInfo();

        void UpdateFuncArgs(const LocalVariables& args);
        void UpdateFuncReturnValue(const wxString& retValueGdbId);
        void Initialize();

        DECLARE_EVENT_TABLE()
    protected:
        void DoClearNonVariableObjectEntries(wxArrayString& itemsNotRemoved, size_t flags,
            std::map<wxString, wxString>& oldValues);
        void DoUpdateLocals(const LocalVariables& locals, size_t kind);

        // Events
        void OnItemExpanding(wxTreeEvent& event);
        void OnRefresh(wxCommandEvent& event);
        void OnRefreshUI(wxUpdateUIEvent& event);
        void OnItemRightClick(wxTreeEvent& event);
        void OnEditValue(wxCommandEvent& event);
        void OnEditValueUI(wxUpdateUIEvent& event);
        void OnStackSelected(Event& event);
        void OnSortItems(wxCommandEvent& event);
        void SetSortingFunction();
    protected:
        DebuggerPreDefinedTypes m_preDefTypes;
        bool m_resolveLocals;
        bool m_arrayAsCharPtr;
        bool m_sortAsc;
        bool m_defaultHexDisplay;
    };

    /** Implementing SimpleTableBase */
    class WatchesTable : public DebuggerTreeListCtrlBase
    {
        wxMenu* m_rclickMenu;
    public:
        /** Constructor */
        WatchesTable(wxWindow* parent);
        virtual ~WatchesTable();

        void AddExpression(const wxString& expr);
        wxArrayString GetExpressions();
        void Clear();
        void RefreshValues(bool repositionEditor = true);
    public:
        virtual void OnTypeResolveError(Event& event);
        virtual void OnRefreshUI(wxUpdateUIEvent& event);
        virtual void OnRefresh(wxCommandEvent& event);
        /**
         * @brief a callback called from the global manager when the debugger
         * responded for our CreateVariableObject call
         * @param event
         */
        void OnCreateVariableObject(const DebuggerEventData& event);

        /**
         * @brief a type resolving is completed, we can now add the watch
         */
        void OnTypeResolved(const DebuggerEventData& event);
        /**
         * @brief a callback called from the global manager when the debugger
         * responded for our ListChildren call
         * @param event
         */
        void OnListChildren(const DebuggerEventData& event);
        void OnUpdateVariableObject(const DebuggerEventData& event);

        void UpdateVariableObjects();
    protected:
        // Handlers for SimpleTableBase events.
        void OnListKeyDown(wxTreeEvent& event);
        void OnItemRightClick(wxTreeEvent& event);
        void OnItemExpanding(wxTreeEvent& event);
        void OnListEditLabelBegin(wxTreeEvent& event);
        void OnListEditLabelEnd(wxTreeEvent& event);

        // Menu events
        void OnNewWatch(wxCommandEvent& event);
        void OnNewWatchUI(wxUpdateUIEvent& event);
        void OnDeleteAll(wxCommandEvent& event);
        void OnDeleteAllUI(wxUpdateUIEvent& event);
        void OnDeleteWatch(wxCommandEvent& event);
        void OnDeleteWatchUI(wxUpdateUIEvent& event);
        void OnMenuEditExpr(wxCommandEvent& event);
        void OnMenuEditExprUI(wxUpdateUIEvent& event);
        void OnMenuCopyValue(wxCommandEvent& event);
        void OnMenuCopyBoth(wxCommandEvent& event);
        void OnMenuDisplayFormat(wxCommandEvent& event);
        void OnNewWatch_Internal(wxCommandEvent& event);
    protected:
        void InitTable();
        void DoUpdateExpression(const wxTreeItemId& item, const wxString& newExpr);
    };
    
    /** Implementing DebuggerAsciiViewerBase */
    class DebuggerAsciiViewer : public wxPanel
    {
    public:
        /** Constructor */
        DebuggerAsciiViewer(wxWindow * parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 225,95 ), long style = wxTAB_TRAVERSAL );
        virtual ~DebuggerAsciiViewer();
        void UpdateView(const wxString &expr, const wxString &value);
    protected:
        void OnClearView(wxCommandEvent &e);
        void OnEdit(wxCommandEvent &e);
        void OnEditUI(wxUpdateUIEvent &e);
        void OnThemeColourChanged(wxCommandEvent &e);
        bool IsFocused();
    private:
        wxStaticText* m_staticText1;
        wxTextCtrl* m_textCtrlExpression;
        Button* m_buttonClear;
        wxStyledTextCtrl *m_textView;
    };
    
    //++++++++++----------------------------------
    // Our custom model
    //++++++++++----------------------------------

    class RegistersViewModelClientData : public wxClientData
    {
    public:
        RegistersViewModelClientData(bool firstModified, bool secondModified)
            : m_firstColModified(firstModified)
            , m_secondColModified(secondModified)
        {
        }

        void SetFirstColModified(bool firstColModified)             { this->m_firstColModified = firstColModified; }
        void SetSecondColModified(bool secondColModified)           { this->m_secondColModified = secondColModified; }
        bool IsFirstColModified() const                             { return m_firstColModified; }
        bool IsSecondColModified() const                            { return m_secondColModified; }
    private:
        bool m_firstColModified;
        bool m_secondColModified;
    };

    //++++++++++----------------------------------
    // DebuggerDisassemblyTab
    //++++++++++----------------------------------
    
    class DebuggerDisassemblyTab : public wxPanel
    {
    public:
        DebuggerDisassemblyTab(wxWindow* parent, const wxString& label, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxSize(500, 300), long style = wxTAB_TRAVERSAL);
        virtual ~DebuggerDisassemblyTab();

        void SetTitle(const wxString& title)                        { this->m_title = title; }
        const wxString& GetTitle() const                            { return m_title; }
        wxArrayString GetRegisterNames() const;
        
        wxStaticText* GetStaticText24()                             { return m_staticText24; }
        wxTextCtrl* GetTextCtrlCurFunction()                        { return m_textCtrlCurFunction; }
        wxStyledTextCtrl* GetStc()                                  { return m_stc; }
        wxPanel* GetSplitterPage55()                                { return m_splitterPage55; }
        ThemedListCtrl* GetDvListCtrlRegisters()                    { return m_dvListCtrlRegisters; }
        wxPanel* GetSplitterPage59()                                { return m_splitterPage59; }
        wxSplitterWindow* GetSplitter51()                           { return m_splitter51; }
    protected:
        virtual void OnMarginClicked(wxStyledTextEvent& event);

        void DoClearRegistersView();
        void DoClearDisassembleView();
        void DoCentrLine(int line);

        void OnOutput(Event& e);
        void OnCurLine(Event& e);
        void OnQueryFileLineDone(Event& e);
        void OnRefreshView(Event& e);
        void OnShowRegisters(Event& e);
        void OnDebuggerStopped(DebugEvent& e);
        void OnAllBreakpointsDeleted(wxCommandEvent& e);
    private:
        wxStaticText* m_staticText24;
        wxTextCtrl* m_textCtrlCurFunction;
        wxSplitterWindow* m_splitter51;
        wxPanel* m_splitterPage55;
        wxStyledTextCtrl* m_stc;
        wxPanel* m_splitterPage59;
        ThemedListCtrl* m_dvListCtrlRegisters;
        wxString m_title;
        DisassembleEntryVec_t m_lines;
        gsgsStringMap m_oldValues;
    };

    class DebuggerPaneConfig : public ObjectData
    {
    public:
        enum eDebuggerWindows 
        {
            None            = 0x0,
            Locals          = 0x01,
            Watches         = 0x02,
            Callstack       = 0x04,
            Object          = 0x08,
            Memory          = 0x10,
            AsciiViewer     = 0x20,
            Breakpoint      = 0x40,
            Output          = 0x80,
            Thread          = 0x100,
            Disassemble     = 0x200,
            AnimationObject = 0x400,
            Animation       = 0x800,
            TimeLine        = 0x1000,
            All             = 0xFFFFFFFF,
        };

        DebuggerPaneConfig();
        virtual ~DebuggerPaneConfig();

        virtual void FromJSON(const JSONObject & json);

        virtual JSONObject ToJSON() const;

        void SetWindows(size_t windows) { m_windows = windows; }

        size_t GetWindows() const { return m_windows; }

        /**
         * @brief convert debugger window flag id to its visual name
         */
        wxString WindowName(eDebuggerWindows flag) const;

        void ShowDebuggerWindow(eDebuggerWindows win, bool show)
        {
            if(show) 
            {
                m_windows |= win;
            } 
            else 
            {
                m_windows &= ~win;
            }
        }

        bool IsDebuggerWindowShown(eDebuggerWindows win) const { return m_windows & win; }
    private:
        size_t m_windows;
    };

    class DebuggerPane : public wxPanel
    {
    public:
        DebuggerPane(wxWindow * parent, const wxString & caption, wxAuiManager * mgr);
        virtual ~DebuggerPane();

        const wxString & GetCaption() const { return m_caption; }

        Notebook * GetNotebook() { return m_book; }

        LocalsTable * GetLocalsTable() { return m_localsTable; }

        WatchesTable * GetWatchesTable() { return m_watchesTable; }

        DebuggerCallstackView * GetFrameListView() { return m_frameList; }

        BreakpointDlg * GetBreakpointView() { return m_breakpoints; }

        ThreadListPanel * GetThreadsView() { return m_threads; }

        MemoryView * GetMemoryView() { return m_memory; }

        DebuggerAsciiViewer * GetAsciiViewer() { return m_asciiViewer; }

        DebuggerDisassemblyTab * GetDisassemblyTab() { return m_disassemble; }

        DebugTab * GetDebugWindow() { return m_outputDebug; }

        void SelectTab(const wxString & tabName);

        void Clear();

        void OnPageChanged(wxBookCtrlEvent & event);
    public:
        static const wxString LOCALS;
        static const wxString WATCHES;
        static const wxString FRAMES;
        static const wxString BREAKPOINT;
        static const wxString Object;
        static const wxString AnimationObject;
        static const wxString THREAD;
        static const wxString MEMORY;
        static const wxString ASCII_VIEWER;
        static const wxString DEBUGGER_OUTPUT;
        static const wxString DISASSEMBLY;
        static const wxString Animation;
        static const wxString TimeLine;
        static const wxString VBO;
        static const wxString FBO;
    private:
        void CreateViewImpl();
        void OnSettingsChanged(wxCommandEvent& event);
    private:
        wxString m_caption;
        Notebook * m_book;
        LocalsTable * m_localsTable;
        WatchesTable * m_watchesTable;
        DebuggerCallstackView * m_frameList;
        BreakpointDlg * m_breakpoints;
        ThreadListPanel * m_threads;
        MemoryView * m_memory;
        DebuggerAsciiViewer * m_asciiViewer;
        DebuggerDisassemblyTab * m_disassemble;
        wxAuiManager * m_mgr;
        DebugTab * m_outputDebug;
        gsgsStringSet m_visibleWindows;
        bool m_initDone;
    };
}
#endif
