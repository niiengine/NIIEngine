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
#ifndef gsgsOUTPUT_PANE_H
#define gsgsOUTPUT_PANE_H

#include "gsgsPreInclude.h"
#include "gsgsSerializer.h"
#include "gsgsOutputTabWindow.h"
#include "gsgsSearchWorker.h"
#include "CommonController1.h"
#include <wx/aui/auibar.h>
#include <wx/gauge.h>
#include <wx/stc/stc.h>
#include <wx/thread.h> // Base class: wxThread
#include "wx/debug.h"
#include "cpptoken.h"

class wxTerminal;
class ExecCmd;

namespace gsgs
{
    class OutputDebugStringThread;
    class ClangOutputTab;
    class NewBuildTab;
    class ShellTab;
    class TaskPanel;

    #define FIND_IN_FILES_WIN   _("Find")
    #define BUILD_WIN           _("Build")
    #define OUTPUT_WIN          _("Output")
    #define OUTPUT_DEBUG        _("Debug")
    #define REPLACE_IN_FILES    _("Replace")
    #define TASKS               _("Tasks")
    #define TRACE_TAB           _("Trace")
    #define SHOW_USAGE          _("References")
    #define CLANG_TAB           _("Clang")
    
    class TasksPanelData : public ObjectData
    {
        gsgsStringMap m_tasks;
        wxArrayString m_enabledItems;
        wxString m_encoding;

    public:
        TasksPanelData()
        {
            // Set the default search patterns
            m_tasks.clear();
            m_tasks[wxT("TODO")] = wxT("/[/\\*]+ *TODO");
            m_tasks[wxT("BUG")] = wxT("/[/\\*]+ *BUG");
            m_tasks[wxT("ATTN")] = wxT("/[/\\*]+ *ATTN");
            m_tasks[wxT("FIXME")] = wxT("/[/\\*]+ *FIXME");

            m_enabledItems.Clear();
            m_enabledItems.Add(wxT("TODO"));
            m_enabledItems.Add(wxT("BUG"));
            m_enabledItems.Add(wxT("FIXME"));
            m_enabledItems.Add(wxT("ATTN"));
        }

        virtual ~TasksPanelData() {}

    public:
        void SetEncoding(const wxString& encoding) { this->m_encoding = encoding; }
        const wxString& GetEncoding() const { return m_encoding; }
        void SetEnabledItems(const wxArrayString& enabledItems) { this->m_enabledItems = enabledItems; }
        void SetTasks(const gsgsStringMap& tasks) { this->m_tasks = tasks; }
        const wxArrayString& GetEnabledItems() const { return m_enabledItems; }
        const gsgsStringMap& GetTasks() const { return m_tasks; }

        virtual void DeSerialize(Serializer& arch)
        {
            arch.Read(wxT("m_tasks"), m_tasks);
            arch.Read(wxT("m_enabledItems"), m_enabledItems);
            arch.Read(wxT("m_encoding"), m_encoding);
        }

        virtual void Serialize(Serializer& arch)
        {
            arch.Write(wxT("m_tasks"), m_tasks);
            arch.Write(wxT("m_enabledItems"), m_enabledItems);
            arch.Write(wxT("m_encoding"), m_encoding);
        }
    };

    typedef std::unordered_map<int, CppToken> UsageResultsMap;

    class FindUsageTab : public OutputTabWindow
    {
    public:
        FindUsageTab(wxWindow* parent, const wxString& name);
        virtual ~FindUsageTab();

        virtual void Clear();
        virtual void OnClearAllUI(wxUpdateUIEvent& e);
        virtual void OnClearAll(wxCommandEvent& e);
        virtual void OnMouseDClick(wxStyledTextEvent& e);
        virtual void OnHoldOpenUpdateUI(wxUpdateUIEvent& e);
        virtual void OnStyleNeeded(wxStyledTextEvent& e);
        virtual void OnTheme(wxCommandEvent& e);
        void OnWorkspaceClosed(wxCommandEvent& event);
    public:
        void ShowUsage(const CppToken::Vec_t& matches, const wxString& searchWhat);
    protected:
        void DoOpenResult(const CppToken& token);
    private:
        UsageResultsMap m_matches;
    };

    // Map between the line numbers and a search results
    typedef std::map<int, SearchResult> MatchInfo_t;

    class FindResultsTab : public OutputTabWindow
    {
    public:
        FindResultsTab(wxWindow* parent, wxWindowID id, const wxString& name);
        ~FindResultsTab();

        void SetStyles(wxStyledTextCtrl* sci);
        void StyleText(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e, bool hasSope = false);
        void ResetStyler();

        void NextMatch();
        void PrevMatch();
    protected:
        struct History
        {
            wxString title;
            SearchData searchData;
            wxString text;
            MatchInfo_t matchInfo;
            std::list<int> indicators;
            typedef wxOrderedMap<wxString, History> Map_t;
        };

        void AppendText(const wxString& line);
        void Clear();
        void SaveSearchData();
        void LoadSearch(const History& h);
        virtual void OnFindInFiles(wxCommandEvent& e);
        virtual void OnRecentSearches(wxCommandEvent& e);
        virtual void OnSearchStart(wxCommandEvent& e);
        virtual void OnSearchMatch(wxCommandEvent& e);
        virtual void OnSearchEnded(wxCommandEvent& e);
        virtual void OnSearchCancel(wxCommandEvent& e);
        virtual void OnClearAll(wxCommandEvent& e);
        virtual void OnRepeatOutput(wxCommandEvent& e);

        virtual void OnClearAllUI(wxUpdateUIEvent& e);
        virtual void OnRecentSearchesUI(wxUpdateUIEvent& e);
        virtual void OnRepeatOutputUI(wxUpdateUIEvent& e);
        virtual void OnMouseDClick(wxStyledTextEvent& e);

        virtual void OnStopSearch(wxCommandEvent& e);
        virtual void OnStopSearchUI(wxUpdateUIEvent& e);
        virtual void OnHoldOpenUpdateUI(wxUpdateUIEvent& e);
        virtual void OnStyleNeeded(wxStyledTextEvent& e);
        SearchData* GetSearchData() { return &m_searchData; }
        void DoOpenSearchResult(const SearchResult& result, wxStyledTextCtrl* sci, int markerLine);
        void OnTheme(wxCommandEvent& e);
        void OnWorkspaceClosed(wxCommandEvent& event);
        DECLARE_EVENT_TABLE()
    protected:
        MatchInfo_t m_matchInfo;
        SearchData m_searchData;
        wxString m_searchTitle;
        std::list<int> m_indicators;
        bool m_searchInProgress;

        History::Map_t m_history;
    };

    class ReplaceInFilesPanel : public FindResultsTab
    {
    public:
        ReplaceInFilesPanel(wxWindow* parent, wxWindowID id, const wxString& name);
        virtual ~ReplaceInFilesPanel();
    protected:
        void DoSaveResults(wxStyledTextCtrl* sci, std::map<int, SearchResult>::iterator begin,
            std::map<int, SearchResult>::iterator end);

        wxStyledTextCtrl* DoGetEditor(const wxString& fileName);

        // Event handlers
        virtual void OnSearchStart(wxCommandEvent& e);
        virtual void OnSearchMatch(wxCommandEvent& e);
        virtual void OnSearchEnded(wxCommandEvent& e);
        virtual void OnMarginClick(wxStyledTextEvent& e);

        virtual void OnMarkAll(wxCommandEvent& e);
        virtual void OnUnmarkAll(wxCommandEvent& e);
        virtual void OnReplace(wxCommandEvent& e);

        virtual void OnMarkAllUI(wxUpdateUIEvent& e);
        virtual void OnUnmarkAllUI(wxUpdateUIEvent& e);
        virtual void OnReplaceUI(wxUpdateUIEvent& e);
        virtual void OnReplaceWithComboUI(wxUpdateUIEvent& e);
        virtual void OnHoldOpenUpdateUI(wxUpdateUIEvent& e);
        virtual void OnMouseDClick(wxStyledTextEvent& e);
    protected:
        wxComboBox* m_replaceWith;
        wxGauge* m_progress;
        wxStaticText* m_replaceWithText;
        wxArrayString m_filesModified;
    };

    class ShellTab : public OutputTabWindow
    {
    public:
        ShellTab(wxWindow* parent, wxWindowID id, const wxString& name);
        virtual ~ShellTab();
    protected:
        static void InitStyle(wxStyledTextCtrl * sci);

        virtual bool DoSendInput(const wxString & line);

        virtual void OnProcStarted(wxCommandEvent& e);
        virtual void OnProcOutput(wxCommandEvent& e);
        virtual void OnProcError(wxCommandEvent& e);
        virtual void OnProcEnded(wxCommandEvent& e);

        virtual void OnSendInput(wxCommandEvent& e);
        virtual void OnStopProc(wxCommandEvent& e);
        virtual void OnKeyDown(wxKeyEvent& e);
        virtual void OnEnter(wxCommandEvent& e);
        virtual void OnUpdateUI(wxUpdateUIEvent& e);
        virtual void OnHoldOpenUpdateUI(wxUpdateUIEvent& e);
        void OnTheme(wxCommandEvent& e);

        DECLARE_EVENT_TABLE()
    protected:
        wxSizer* m_inputSizer;
        wxSizer* m_vertSizer;
        wxComboBox* m_input;
        ExecCmd* m_cmd;
    };

    class OutputTab : public ShellTab
    {
    public:
        OutputTab(wxWindow* parent, wxWindowID id, const wxString& name);
        virtual ~OutputTab();

        /**
         * @brief Windows only. Report a string from a debuggee process which used the "OuptutDebugString" method
         */
        void OnOutputDebugString(Event& event);

        void OnDebugStarted(DebugEvent& event);
        void OnDebugStopped(DebugEvent& event);
        virtual void OnProcStarted(wxCommandEvent& e);
        virtual void OnProcEnded(wxCommandEvent& e);
        void OnWorkspaceClosed(wxCommandEvent& event);
    protected:
        void DoSetCollecting(bool b);
    private:
        OutputDebugStringThread* m_thread;
        bool m_outputDebugStringActive;
    };

    class DebugTab : public wxPanel
    {
    public:
        DebugTab(wxWindow* parent, wxWindowID id, const wxString& name);
        virtual ~DebugTab();
        ToolBar* GetToolBar() { return m_toolbar; }
        void AppendLine(const wxString& line);
    protected:
        void OnUpdateUI(wxUpdateUIEvent& e);
        void OnHoldOpenUpdateUI(wxUpdateUIEvent& e);
        void OnEnableDbgLog(wxCommandEvent& event);
        void OnEnableDbgLogUI(wxUpdateUIEvent& event);
        void OnCtrlC(Event& event);
        void OnExecuteCommand(Event& event);
    private:
        wxTerminal* m_terminal;
        ToolBar* m_toolbar;
    };

    class OutputPane : public wxPanel
    {
    protected:
        struct Tab 
        {
            Tab(const wxString& label, wxWindow* win, const wxBitmap& bmp = wxNullBitmap)
                : m_label(label)
                , m_window(win)
                , m_bmp(bmp)
            {
            }

            Tab()
                : m_window(NULL)
            {
            }

            wxString m_label;
            wxWindow* m_window;
            wxBitmap m_bmp;
        };
        std::map<wxString, Tab> m_tabs;
    public:
        OutputPane(wxWindow* parent, const wxString& caption);
        virtual ~OutputPane();

        void SaveTabOrder();

        void ApplySavedTabOrder() const;

        const wxString& GetCaption() const          { return m_caption; }

        Notebook* GetNotebook()                     { return m_book; }

        FindResultsTab* GetFindResultsTab()         { return m_findResultsTab; }
        ReplaceInFilesPanel* GetReplaceResultsTab() { return m_replaceResultsTab; }
        NewBuildTab* GetBuildTab()                  { return m_buildWin; }
        ShellTab* GetOutputWindow()                 { return m_outputWind; }
        FindUsageTab* GetShowUsageTab()             { return m_showUsageTab; }
    protected:
        void CreateViewImpl();
        void OnEditorFocus(wxCommandEvent& e);
        void OnBuildStarted(BuildEvent& e);
        void OnBuildEnded(BuildEvent& e);
        void OnSettingsChanged(wxCommandEvent& event);
        void OnToggleTab(Event& event);
        void OnOutputBookFileListMenu(ContextMenuEvent& event);
    private:
        wxString m_caption;
        Notebook* m_book;
        FindResultsTab* m_findResultsTab;
        ReplaceInFilesPanel* m_replaceResultsTab;

        NewBuildTab * m_buildWin;
        ShellTab * m_outputWind;
        TaskPanel * m_taskPanel;
        FindUsageTab * m_showUsageTab;
        ClangOutputTab * m_clangOutputTab;

        bool m_buildInProgress;
    };
}
#endif
