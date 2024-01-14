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

#include "gsgsOutputPane.h"
#include "gsgsCommonHandler.h"
#include "gsgsWindowManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsStyleManager.h"
#include "gsgsLexerStyle.h"
#include "gsgsImageManager.h"
#include "gsgsToolBar.h"
#include "gsgsConfig.h"
#include "gsgsEditor.h"
#include "gsgsEvent.h"
#include "gsgsTagManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsFrame.h"
#include "gsgsOptionsConfig.h"
#include "gsgsPluginManager.h"
#include "gsgsFileUtil.h"
#include "gsgsLogManager.h"
#include "gsgsButton.h"
#include "globals.h"
#include "gsgsManager.h"
#include "gsgsBuildTab.h"
#include "gsgsWorkspaceManager.h"
#include "plugin.h"
#include "gsgsEditorConfig.h"
#include "gsgsProcess.h"
#include "wxterminal.h"
#include "imanager.h"

#include <wx/xrc/xmlres.h>
#include <wx/aui/framemanager.h>
#include <wx/dcbuffer.h>
#include <wx/fontmap.h>
#include <wx/tglbtn.h>
#include <wx/progdlg.h>
#include <wx/wupdlock.h>

namespace gsgs
{
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // TaskPanel
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    class TaskPanel : public FindResultsTab
    {
    public:
        TaskPanel(wxWindow* parent, wxWindowID id, const wxString& name);
        ~TaskPanel();
    protected:
        SearchData DoGetSearchData();
        void OnFindWhat(wxCommandEvent& e);
        void OnSearch(wxCommandEvent& e);
        void OnSearchUI(wxUpdateUIEvent& e);
        void OnRepeatOutput(wxCommandEvent& e);
        void OnHoldOpenUpdateUI(wxUpdateUIEvent& e);
        void OnEncodingSelected(wxCommandEvent& e);
        void OnSearchStart(wxCommandEvent& e);
        DECLARE_EVENT_TABLE()
    private:
        wxArrayString m_extensions;
        Button* m_findWhat;
        wxChoice* m_scope;
        wxChoice* m_choiceEncoding;
    };
    //-----------------------------------------------------------------------------
    // TaskPanel
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    BEGIN_EVENT_TABLE(TaskPanel, FindResultsTab)
    EVT_BUTTON(wxID_FIND, TaskPanel::OnSearch)
    EVT_BUTTON(XRCID("find_what"), TaskPanel::OnFindWhat)
    EVT_UPDATE_UI(wxID_FIND, TaskPanel::OnSearchUI)
    EVT_UPDATE_UI(XRCID("hold_pane_open"), TaskPanel::OnHoldOpenUpdateUI)
    END_EVENT_TABLE()

    TaskPanel::TaskPanel(wxWindow* parent, wxWindowID id, const wxString& name)
        : FindResultsTab(parent, id, name)
        , m_scope(NULL)
    {
        wxArrayString filters;
        filters.Add(wxString(wxT("C/C++ ")) + _("Sources"));
        m_extensions.Add(wxT("*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.hxx;*.hh;*.inl;*.inc;*.hh;*.js;*.php;*.phtml"));
        filters.Add(_("All Files"));
        m_extensions.Add(wxT("*.*"));

        m_tb->DeleteById(XRCID("repeat_output"));
        m_tb->DeleteById(XRCID("recent_searches"));
        m_tb->Realize();
        wxBoxSizer* verticalPanelSizer = new wxBoxSizer(wxVERTICAL);

        Button* btn = new Button(this, wxID_FIND, _("&Search"));
        verticalPanelSizer->Add(btn, 0, wxEXPAND | wxALL, 5);

        m_findWhat = new Button(this, XRCID("find_what"), _("Find What..."));
        verticalPanelSizer->Add(m_findWhat, 0, wxEXPAND | wxALL, 5);
        verticalPanelSizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxALL, 5);

        m_choiceEncoding = new wxChoice(this, wxID_ANY);
        verticalPanelSizer->Add(m_choiceEncoding, 0, wxEXPAND | wxALL, 5);
        m_choiceEncoding->SetToolTip(_("Encoding to use for the search"));

        TasksPanelData d;
        gsgs_EditorConfig().ReadObject(wxT("TasksPanelData"), &d);

        // Set encoding
        wxArrayString astrEncodings;
        wxFontEncoding fontEnc;
        int selection(0);

        size_t iEncCnt = wxFontMapper::GetSupportedEncodingsCount();
        for(size_t i = 0; i < iEncCnt; i++) {
            fontEnc = wxFontMapper::GetEncoding(i);
            if(wxFONTENCODING_SYSTEM == fontEnc) { // skip system, it is changed to UTF-8 in optionsconfig
                continue;
            }
            wxString encodingName = wxFontMapper::GetEncodingName(fontEnc);
            size_t pos = astrEncodings.Add(encodingName);

            if(d.GetEncoding() == encodingName) selection = static_cast<int>(pos);
        }

        m_choiceEncoding->Append(astrEncodings);
        if(m_choiceEncoding->IsEmpty() == false) m_choiceEncoding->SetSelection(selection);

        m_choiceEncoding->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(TaskPanel::OnEncodingSelected), NULL,
                                  this);

        wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);

        // grab the base class scintilla and put our sizer in its place
        wxSizer* mainSizer = m_vSizer;
        mainSizer->Detach(m_sci);
        hSizer->Add(m_sci, 1, wxEXPAND | wxALL, 1);
        hSizer->Add(verticalPanelSizer, 0, wxEXPAND | wxALL, 1);

        mainSizer->Add(hSizer, 1, wxEXPAND | wxALL, 1);
        mainSizer->Layout();
    }
    //-----------------------------------------------------------------------------
    TaskPanel::~TaskPanel()
    {
        m_choiceEncoding->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(TaskPanel::OnEncodingSelected),
                                     NULL, this);
    }
    //-----------------------------------------------------------------------------
    SearchData TaskPanel::DoGetSearchData()
    {
        SearchData data;
        data.SetDisplayScope(true);
        data.SetRegularExpression(true);
        data.SetMatchCase(false);
        data.SetMatchWholeWord(false);
        data.SetEncoding(m_choiceEncoding->GetStringSelection());
        data.SetOwner(this);

        wxString sfind;

        // Load all info from disk
        TasksPanelData d;
        gsgs_EditorConfig().ReadObject(wxT("TasksPanelData"), &d);

        gsgsStringMap::const_iterator iter = d.GetTasks().begin();
        for(; iter != d.GetTasks().end(); iter++) {
            wxString name = iter->first;
            wxString regex = iter->second;
            bool enabled = (d.GetEnabledItems().Index(iter->first) != wxNOT_FOUND);

            regex.Trim().Trim(false);
            wxRegEx re(regex);
            if(enabled && !regex.IsEmpty() && re.IsValid()) sfind << wxT("(") << regex << wxT(")|");
        }

        if(sfind.empty() == false) sfind.RemoveLast();

        data.SetFindString(sfind);

        wxString rootDir = gsgs_Workspace().GetWorkspace()->GetFileName().GetPath();
        wxArrayString rootDirs;
        rootDirs.push_back(rootDir);
        data.SetRootDirs(rootDirs);
        data.SetExtensions(wxT("*.*"));
        return data;
    }
    //-----------------------------------------------------------------------------
    void TaskPanel::OnSearch(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        SearchData sd = DoGetSearchData();
        SearchThreadST::Get()->PerformSearch(sd);
    }
    //-----------------------------------------------------------------------------
    void TaskPanel::OnSearchUI(wxUpdateUIEvent& e) { e.Enable(gsgs_Workspace().IsValid()); }
    //-----------------------------------------------------------------------------
    void TaskPanel::OnRepeatOutput(wxCommandEvent& e) { OnSearch(e); }
    //-----------------------------------------------------------------------------
    void TaskPanel::OnFindWhat(wxCommandEvent& e)
    {
        TasksFindWhatDlg dlg(wxTheApp->GetTopWindow());
        dlg.ShowModal();
    }
    //-----------------------------------------------------------------------------
    void TaskPanel::OnHoldOpenUpdateUI(wxUpdateUIEvent& e)
    {
        int sel = gsgs_Frame()->GetOutputPane()->GetNotebook()->GetSelection();
        if(gsgs_Frame()->GetOutputPane()->GetNotebook()->GetPage(sel) != this) {
            return;
        }

        if(gsgs_EditorConfig().GetOptions()->GetHideOutpuPaneOnUserClick()) {
            e.Enable(true);
            e.Check(gsgs_EditorConfig().GetOptions()->GetHideOutputPaneNotIfTasks());

        } else {
            e.Enable(false);
            e.Check(false);
        }
    }
    //-----------------------------------------------------------------------------
    void TaskPanel::OnEncodingSelected(wxCommandEvent& e)
    {
        wxUnusedVar(e);

        TasksPanelData d;
        gsgs_EditorConfig().ReadObject(wxT("TasksPanelData"), &d);

        d.SetEncoding(m_choiceEncoding->GetStringSelection());
        gsgs_EditorConfig().WriteObject(wxT("TasksPanelData"), &d);
    }
    //-----------------------------------------------------------------------------
    void TaskPanel::OnSearchStart(wxCommandEvent& e)
    {
        m_searchInProgress = true;
        Clear();
        SetStyles(m_sci);
        SearchData* data = (SearchData*)e.GetClientData();
        wxDELETE(data);

        // Make sure that the Output view & the "Replace" tab
        // are visible

        Event event(wxEVT_SHOW_OUTPUT_TAB);
        event.SetSelected(true).SetString(TASKS);
        gsgs_Event().AddPendingEvent(event);
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // FindUsageTab
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    FindUsageTab::FindUsageTab(wxWindow* parent, const wxString& name)
        : OutputTabWindow(parent, wxID_ANY, name)
    {
        m_styler->SetStyles(m_sci);
        m_sci->HideSelection(true);
        m_sci->Connect(wxEVT_STC_STYLENEEDED, wxStyledTextEventHandler(FindUsageTab::OnStyleNeeded), NULL, this);
        m_tb->DeleteById(XRCID("repeat_output"));
        m_tb->Realize();
        gsgs_Event().Connect(
            wxEVT_GSGS_THEME_CHANGED, wxCommandEventHandler(FindUsageTab::OnTheme), NULL, this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_CLOSED, &FindUsageTab::OnWorkspaceClosed, this);
    }
    //-----------------------------------------------------------------------------
    FindUsageTab::~FindUsageTab()
    {
        gsgs_Event().Disconnect(
            wxEVT_GSGS_THEME_CHANGED, wxCommandEventHandler(FindUsageTab::OnTheme), NULL, this);
        gsgs_Event().Unbind(wxEVT_WORKSPACE_CLOSED, &FindUsageTab::OnWorkspaceClosed, this);
    }
    //-----------------------------------------------------------------------------
    void FindUsageTab::OnStyleNeeded(wxStyledTextEvent& e)
    {
        wxStyledTextCtrl* ctrl = dynamic_cast<wxStyledTextCtrl*>(e.GetEventObject());
        if(!ctrl) return;
        m_styler->StyleText(ctrl, e, true);
    }
    //-----------------------------------------------------------------------------
    void FindUsageTab::Clear()
    {
        m_matches.clear();
        m_styler->Reset();
        OutputTabWindow::Clear();
    }
    //-----------------------------------------------------------------------------
    void FindUsageTab::OnClearAll(wxCommandEvent& e)
    { 
        Clear(); 
    }
    //-----------------------------------------------------------------------------
    void FindUsageTab::OnMouseDClick(wxStyledTextEvent& e)
    {
        int clickedLine = wxNOT_FOUND;
        m_styler->HitTest(m_sci, e, clickedLine);

        // Did we clicked on a togglable line?
        int toggleLine = m_styler->TestToggle(m_sci, e);
        if(toggleLine != wxNOT_FOUND) {
            m_sci->ToggleFold(toggleLine);

        } else {
            UsageResultsMap::const_iterator iter = m_matches.find(clickedLine);
            if(iter != m_matches.end()) {
                DoOpenResult(iter->second);
            }
        }
    }
    //-----------------------------------------------------------------------------
    void FindUsageTab::OnClearAllUI(wxUpdateUIEvent& e) 
    { 
        e.Enable(m_sci && m_sci->GetLength()); 
    }
    //-----------------------------------------------------------------------------
    void FindUsageTab::ShowUsage(const CppToken::Vec_t& matches, const wxString& searchWhat)
    {
        Clear();
        int lineNumber(0);
        wxString text;
        wxString curfile;
        wxString curfileContent;
        wxArrayString lines;

        text = wxString::Format(_("===== Finding references of '%s' =====\n"), searchWhat.c_str());
        lineNumber++;

        CppToken::Vec_t::const_iterator iter = matches.begin();
        for(; iter != matches.end(); ++iter) {

            // Print the line number
            wxString file_name(iter->getFilename());
            if(curfile != file_name) {
                curfile = file_name;
                wxFileName fn(file_name);
                fn.MakeRelativeTo();

                text << fn.GetFullPath() << wxT("\n");
                lineNumber++;

                // Load the file content
                wxLogNull nolog;
                wxFFile thefile(file_name, wxT("rb"));
                if(thefile.IsOpened()) {

                    wxFileOffset size = thefile.Length();
                    wxString fileData;
                    fileData.Alloc(size);
                    curfileContent.Clear();

                    wxCSConv fontEncConv(wxFONTENCODING_ISO8859_1);
                    thefile.ReadAll(&curfileContent, fontEncConv);

                    // break the current file into lines, a line can be an empty string
                    lines = wxStringTokenize(curfileContent, wxT("\n"), wxTOKEN_RET_EMPTY_ALL);
                }
            }

            // Keep the match
            m_matches[lineNumber] = *iter;

            // Format the message
            wxString linenum = wxString::Format(wxT(" %5u: "), (unsigned int)iter->getLineNumber() + 1);
            wxString scopeName(wxT("<global>"));
            TagEntryPtr tag = TagsManagerST::Get()->FunctionFromFileLine(iter->getFilename(), iter->getLineNumber());
            if(tag) {
                scopeName = tag->GetPath();
            }

            text << linenum << wxT("[ ") << scopeName << wxT(" ] ");
            if(lines.GetCount() > iter->getLineNumber()) {
                text << lines.Item(iter->getLineNumber()).Trim().Trim(false);
            }

            text << wxT("\n");
            lineNumber++;
        }
        text << wxString::Format(_("===== Found total of %u matches =====\n"), (unsigned int)m_matches.size());
        AppendText(text);
    }
    //-----------------------------------------------------------------------------
    void FindUsageTab::DoOpenResult(const CppToken& token)
    {
        if(!token.getFilename().empty()) {
            Editor* editor =
                gsgs_Frame()->GetMainBook()->OpenFile(token.getFilename(), wxEmptyString, token.getLineNumber());
            if(editor) {
                editor->SetLineVisible(token.getLineNumber());
                editor->ScrollToColumn(0);
                editor->SetSelection(token.getOffset(), token.getOffset() + token.getName().length());
            }
        }
    }
    //-----------------------------------------------------------------------------
    void FindUsageTab::OnHoldOpenUpdateUI(wxUpdateUIEvent& e)
    {
        if(gsgs_EditorConfig().GetOptions()->GetHideOutpuPaneOnUserClick()) 
        {
            e.Enable(true);
            e.Check(gsgs_EditorConfig().GetOptions()->GetHideOutputPaneNotIfReferences());

        } 
        else 
        {
            e.Enable(false);
            e.Check(false);
        }
    }
    //-----------------------------------------------------------------------------
    void FindUsageTab::OnTheme(wxCommandEvent& e)
    {
        e.Skip();
        m_styler->SetStyles(m_sci);
    }
    //-----------------------------------------------------------------------------
    void FindUsageTab::OnWorkspaceClosed(wxCommandEvent& event)
    {
        event.Skip();
        Clear();
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // FindResultsTab
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    BEGIN_EVENT_TABLE(FindResultsTab, OutputTabWindow)
        EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHSTARTED, FindResultsTab::OnSearchStart)
        EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_MATCHFOUND, FindResultsTab::OnSearchMatch)
        EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHEND, FindResultsTab::OnSearchEnded)
        EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHCANCELED, FindResultsTab::OnSearchCancel)
        EVT_UPDATE_UI(XRCID("hold_pane_open"), FindResultsTab::OnHoldOpenUpdateUI)
        END_EVENT_TABLE()
    //-----------------------------------------------------------------------------
    FindResultsTab::FindResultsTab(wxWindow* parent, wxWindowID id, const wxString& name) : 
        OutputTabWindow(parent, id, name), 
        m_searchInProgress(false)
    {
        m_sci->Connect(wxEVT_STC_STYLENEEDED, wxStyledTextEventHandler(FindResultsTab::OnStyleNeeded), NULL, this);

        ImageManager& loader = gsgs_Image();

        wxTheApp->Connect(XRCID("find_in_files"), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(FindResultsTab::OnFindInFiles), NULL, this);
        m_tb->Bind(wxEVT_TOOL_DROPDOWN, &FindResultsTab::OnRecentSearches, this, XRCID("recent_searches"));
        m_tb->Bind(wxEVT_UPDATE_UI, &FindResultsTab::OnRecentSearchesUI, this, XRCID("recent_searches"));

        m_tb->AddTool(XRCID("stop_search"), _("Stop current search"), loader.LoadBitmap("stop"), _("Stop current search"));
        m_tb->AddTool(XRCID("recent_searches"), _("Show Recent Searches"), loader.LoadBitmap("history"),
            _("Show Recent Searches"), wxITEM_DROPDOWN);

        Connect(XRCID("stop_search"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FindResultsTab::OnStopSearch),
            NULL, this);
        Connect(XRCID("stop_search"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FindResultsTab::OnStopSearchUI), NULL, this);
        m_tb->Realize();
        gsgs_Event().Connect(wxEVT_GSGS_THEME_CHANGED, wxCommandEventHandler(FindResultsTab::OnTheme), NULL,
            this);

        // Use the same eventhandler for editor config changes too e.g. show/hide whitespace
        gsgs_Event().Bind(wxEVT_EDITOR_CONFIG_CHANGED, &FindResultsTab::OnTheme, this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_CLOSED, &FindResultsTab::OnWorkspaceClosed, this);
    }
    //-----------------------------------------------------------------------------
    FindResultsTab::~FindResultsTab()
    {
        gsgs_Event().Connect(wxEVT_GSGS_THEME_CHANGED, wxCommandEventHandler(FindResultsTab::OnTheme), NULL,
            this);
        wxTheApp->Disconnect(XRCID("find_in_files"), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(FindResultsTab::OnFindInFiles), NULL, this);
        gsgs_Event().Unbind(wxEVT_WORKSPACE_CLOSED, &FindResultsTab::OnWorkspaceClosed, this);
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::SetStyles(wxStyledTextCtrl* sci) { m_styler->SetStyles(sci); }
    //-----------------------------------------------------------------------------
    void FindResultsTab::AppendText(const wxString& line)
    {
        m_sci->SetIndicatorCurrent(1);
        OutputTabWindow::AppendText(line);
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::Clear()
    {
        m_matchInfo.clear();
        m_indicators.clear();
        m_searchTitle.clear();
        OutputTabWindow::Clear();
        m_styler->Reset();
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnFindInFiles(wxCommandEvent& e)
    {
        if (m_searchInProgress) {
            ::wxMessageBox(_("The search thread is currently busy"), _("光栅故事"), wxICON_INFORMATION | wxOK);
            return;
        }

        // Fire the wxEVT_CMD_FIND_IN_FILES_SHOWING showing event
        FindInFilesEvent eventFifShowing(wxEVT_FINDINFILES_DLG_SHOWING);
        gsgs_Event().ProcessEvent(eventFifShowing);

        // Display the Find In Files dialog
        FindReplaceData frd;
        frd.SetName("FindInFilesData");
        gsgs_Config().ReadItem(&frd);

        // Allocate the 'Find In Files' in an inner block
        // We do this because the 'FindReplaceData' will be updated upon the destruction of the dialog
        {
            bool sendDismissEvent = true;
            const wxString& mask = eventFifShowing.GetFileMask();

            // plugins provided paths
            const wxString& paths = eventFifShowing.GetPaths();

            // transient paths take precedence over the normal paths. However, they are not persistent
            // Usually these paths are given when the a tree view like control has focus and user selected folders in it
            const wxString& transientPaths = eventFifShowing.GetTransientPaths();

            wxString fifPaths = paths;
            if (!transientPaths.IsEmpty()) {
                fifPaths = transientPaths;
                sendDismissEvent = false;
            }

            // Prepare the fif dialog
            FindInFilesDialog dlg(gsgs_Event().TopFrame(), frd);
            if (!fifPaths.IsEmpty()) { dlg.SetSearchPaths(fifPaths, !transientPaths.IsEmpty()); }
            if (!mask.IsEmpty()) { dlg.SetFileMask(mask); }

            // Show it
            if ((dlg.ShowDialog() == wxID_OK) && sendDismissEvent) {
                // Notify about the dialog dismissal
                FindInFilesEvent eventDismiss(wxEVT_FINDINFILES_DLG_DISMISSED);
                eventDismiss.SetFileMask(frd.GetSelectedMask());
                eventDismiss.SetPaths(frd.GetSearchPaths());
                gsgs_Event().ProcessEvent(eventDismiss);
            }

        }
        // And we alway store the global find-in-files data (it keeps the 'find-what', 'replace with' fields, etc...)
        gsgs_Config().WriteItem(&frd);
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnSearchStart(wxCommandEvent& e)
    {
        m_searchInProgress = true;
        Clear();
        SetStyles(m_sci);
        SearchData* data = (SearchData*)e.GetClientData();
        if (data) {
            m_searchData = *data;
            m_searchTitle = data->GetFindString();

            wxString message;
            message << _("====== Searching for: '") << data->GetFindString() << _("'; Match case: ")
                << (data->IsMatchCase() ? _("true") : _("false")) << _(" ; Match whole word: ")
                << (data->IsMatchWholeWord() ? _("true") : _("false")) << _(" ; Regular expression: ")
                << (data->IsRegularExpression() ? _("true") : _("false")) << wxT(" ======\n");
            AppendText(message);
        }
        wxDELETE(data);

        // Make sure that the Output view & the "Replace" tab
        // are visible

        Event event(wxEVT_SHOW_OUTPUT_TAB);
        event.SetSelected(true).SetString(FIND_IN_FILES_WIN);
        gsgs_Event().AddPendingEvent(event);
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnSearchMatch(wxCommandEvent& e)
    {
        SearchResultList* res = (SearchResultList*)e.GetClientData();
        if (!res) return;

        SearchResultList::iterator iter = res->begin();
        for (; iter != res->end(); ++iter) {
            if (m_matchInfo.empty() || m_matchInfo.rbegin()->second.GetFileName() != iter->GetFileName()) {
                if (!m_matchInfo.empty()) { AppendText("\n"); }
                wxFileName fn(iter->GetFileName());
                fn.MakeRelativeTo();
                AppendText(fn.GetFullPath() + wxT("\n"));
            }

            int lineno = m_sci->GetLineCount() - 1;
            m_matchInfo.insert(std::make_pair(lineno, *iter));
            wxString text = iter->GetPattern();
            // int delta = -text.Length();
            // text.Trim(false);
            // delta += text.Length();
            // text.Trim();

            wxString linenum = wxString::Format(wxT(" %5u: "), iter->GetLineNumber());
            SearchData* d = GetSearchData();
            // Print the scope name
            if (d->GetDisplayScope()) {
                TagEntryPtr tag = TagsManagerST::Get()->FunctionFromFileLine(iter->GetFileName(), iter->GetLineNumber());
                wxString scopeName(wxT("global"));
                if (tag) { scopeName = tag->GetPath(); }

                linenum << wxT("[ ") << scopeName << wxT(" ] ");
                iter->SetScope(scopeName);
            }

            AppendText(linenum + text + wxT("\n"));
            int indicatorStartPos = m_sci->PositionFromLine(lineno) + iter->GetColumn() + linenum.Length();
            int indicatorLen = iter->GetLen();
            m_indicators.push_back(indicatorStartPos);
            m_sci->IndicatorFillRange(indicatorStartPos, indicatorLen);
        }
        wxDELETE(res);
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnSearchEnded(wxCommandEvent& e)
    {
        m_searchInProgress = false;
        SearchSummary* summary = (SearchSummary*)e.GetClientData();
        if (!summary) return;

        // did the page closed before the search ended?
        AppendText(summary->GetMessage() + wxT("\n"));

        if (m_tb->FindById(XRCID("scroll_on_output")) && m_tb->FindById(XRCID("scroll_on_output"))->IsChecked()) {
            m_sci->GotoLine(0);
        }

        if (!gsgs_EditorConfig().GetOptions()->GetDontAutoFoldResults()) {
            OutputTabWindow::OnCollapseAll(e);
            // Uncollapse the first file's matches
            int maxLine = m_sci->GetLineCount();
            for (int line = 0; line < maxLine; line++) {
                int foldLevel = (m_sci->GetFoldLevel(line) & wxSTC_FOLDLEVELNUMBERMASK) - wxSTC_FOLDLEVELBASE;
                if (foldLevel == 2 && !m_sci->GetFoldExpanded(line)) {
                    m_sci->ToggleFold(line);
                    break;
                }
            }
        }

        wxDELETE(summary);
        SaveSearchData();

        // We need to tell all editors that there's been a (new) search
        // This lets them clear any already-saved line-changes,
        // which a new save will have taken into account
        EditorVec_t editors;
        gsgs_Frame()->GetMainBook()->GetAllEditors(editors, Book::kGetAll_IncludeDetached);
        for (size_t n = 0; n < editors.size(); ++n) {
            Editor* editor = dynamic_cast<Editor*>(*(editors.begin() + n));
            if (editor) { editor->OnFindInFiles(); }
        }
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnSearchCancel(wxCommandEvent& e) { AppendText(_("====== Search cancelled by user ======\n")); }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnClearAll(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        SearchThreadST::Get()->StopSearch();
        Clear();
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnClearAllUI(wxUpdateUIEvent& e) { e.Enable(!m_searchInProgress && !m_sci->IsEmpty()); }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnRepeatOutput(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        SearchData* searchData = GetSearchData();
        searchData->UseNewTab(false);
        SearchThreadST::Get()->PerformSearch(*searchData);
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnRepeatOutputUI(wxUpdateUIEvent& e) { e.Enable(m_sci->GetLength() > 0); }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnMouseDClick(wxStyledTextEvent& e)
    {
        int clickedLine = wxNOT_FOUND;
        m_styler->HitTest(m_sci, e, clickedLine);

        // Did we clicked on a togglable line?
        int toggleLine = m_styler->TestToggle(m_sci, e);
        if (toggleLine != wxNOT_FOUND) {
            m_sci->ToggleFold(toggleLine);

        }
        else {
            MatchInfo_t::const_iterator m = m_matchInfo.find(clickedLine);
            if (m != m_matchInfo.end()) { DoOpenSearchResult(m->second, m_sci, m->first); }
        }
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::NextMatch()
    {
        // locate the last match
        int firstLine = m_sci->MarkerNext(0, 255);
        if (firstLine == wxNOT_FOUND) { firstLine = 0; }

        // We found the last marker
        for (int i = firstLine + 1; i < m_sci->GetLineCount(); i++) {

            // Find the next match
            MatchInfo_t::const_iterator iter = m_matchInfo.find(i);
            if (iter != m_matchInfo.end()) {
                SearchResult sr = iter->second;

                // open the new searchresult in the editor
                DoOpenSearchResult(sr, m_sci, i);
                return;
            }
        }

        // if we are here, it means we are the end of the search results list, add a status message
        gsgs_Frame()->GetMainStatusBar()->SetMessage(_("Reached the end of the 'Find In Files' results"));
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::PrevMatch()
    {
        // locate the last match
        int firstLine = m_sci->MarkerPrevious(m_sci->GetLineCount() - 1, 255);
        if (firstLine == wxNOT_FOUND) { firstLine = m_sci->GetLineCount(); }

        // We found the last marker
        for (int i = firstLine - 1; i >= 0; i--) {

            // Find the next match
            MatchInfo_t::const_iterator iter = m_matchInfo.find(i);
            if (iter != m_matchInfo.end()) {
                SearchResult sr = iter->second;

                // open the new searchresult in the editor
                DoOpenSearchResult(sr, m_sci, i);
                return;
            }
        }
        // if we are here, it means we are the top of the search results list, add a status message
        gsgs_Frame()->GetMainStatusBar()->SetMessage(_("Reached the start of the 'Find In Files' results"));
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::DoOpenSearchResult(const SearchResult& result, wxStyledTextCtrl* sci, int markerLine)
    {
        if (!result.GetFileName().IsEmpty()) {
            Editor* editor = gsgs_Frame()->GetMainBook()->OpenFile(result.GetFileName());
            if (editor && result.GetLen() >= 0) {
                // Update the destination position if there have been subsequent changes in the editor
                int position = result.GetPosition();
                std::vector<int> changes;
                editor->GetChanges(changes);
                unsigned int changesTotal = changes.size();
                int changePosition = 0;
                int changeLength = 0;
                int resultLength = result.GetLen();
                bool removed = false;
                for (unsigned int i = 0; i < changesTotal; i += 2) {
                    changePosition = changes.at(i);
                    changeLength = changes.at(i + 1);
                    if ((changeLength < 0) && (changePosition - changeLength > position) &&
                        (changePosition < position + resultLength)) {
                        // It looks like the data corresponding to this search result has been deleted
                        // While it's possible that it's been cut, then (later in the changes) re-pasted
                        // so that the result still matches, it's more likely to have been replaced by different text
                        // We can't easily tell, so assume the worst and label the result invalid
                        removed = true;
                        // Explain the failure
                        gsgs_Frame()->GetMainStatusBar()->SetMessage(_("Search result is no longer valid"));
                        break;
                    }
                    else if (changePosition <= position) {
                        position += changeLength;
                    }
                }
                if (!removed) {
                    editor->SetEnsureCaretIsVisible(
                        position + resultLength, true,
                        true); // The 3rd parameter sets a small delay, otherwise it fails for long folded files
                    int lineNumber = editor->LineFromPos(position);
                    if (lineNumber) { lineNumber--; }
                    editor->SetLineVisible(lineNumber);
                    editor->SetSelection(position, position + resultLength);

#ifdef __WXGTK__
                    editor->ScrollToColumn(0);
#endif

                    if (sci) {
                        // remove the previous marker and add the new one
                        sci->MarkerDeleteAll(7);
                        sci->MarkerAdd(markerLine, 7);
                        sci->EnsureVisible(markerLine);
                        sci->GotoLine(markerLine);
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnStopSearch(wxCommandEvent& e)
    {
        // stop the search thread
        SearchThreadST::Get()->StopSearch();
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnStopSearchUI(wxUpdateUIEvent& e) { e.Enable(m_searchInProgress); }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnHoldOpenUpdateUI(wxUpdateUIEvent& e)
    {
        int sel = gsgs_Frame()->GetOutputPane()->GetNotebook()->GetSelection();
        if (gsgs_Frame()->GetOutputPane()->GetNotebook()->GetPage(sel) != this) { return; }

        if (gsgs_EditorConfig().GetOptions()->GetHideOutpuPaneOnUserClick()) {
            e.Enable(true);
            e.Check(gsgs_EditorConfig().GetOptions()->GetHideOutputPaneNotIfSearch());

        }
        else {
            e.Enable(false);
            e.Check(false);
        }
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnStyleNeeded(wxStyledTextEvent& e)
    {
        wxStyledTextCtrl* ctrl = dynamic_cast<wxStyledTextCtrl*>(e.GetEventObject());
        if (!ctrl) return;
        StyleText(ctrl, e);
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::StyleText(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e, bool hasSope)
    {
        m_styler->StyleText(ctrl, e, hasSope);
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnTheme(wxCommandEvent& e)
    {
        e.Skip();
        SetStyles(m_sci);
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnRecentSearches(wxCommandEvent& e)
    {
        // Show the menu
        wxMenu menu;
        const int firstID = 8000;
        int counter = 0;
        std::map<int, History> entries;
        std::for_each(m_history.Begin(), m_history.End(), [&](const std::pair<wxString, History>& p) {
            menu.Prepend(firstID + counter, p.first, "", wxITEM_CHECK)->Check(m_searchTitle == p.first);
            entries.insert(std::make_pair(firstID + counter, p.second));
            ++counter;
        });

        ToolBarButton* button = m_tb->FindById(e.GetId());
        CHECK_PTR_RET(button);

        menu.AppendSeparator();
        int clearHistory = ::wxNewId();
        menu.Append(clearHistory, _("Clear History"));
        int sel = m_tb->GetMenuSelectionFromUser(XRCID("recent_searches"), &menu);
        if (sel == wxID_NONE) { return; }
        if (sel == clearHistory) {
            m_history.Clear();

        }
        else if (entries.count(sel)) {
            const History& h = entries.find(sel)->second;
            LoadSearch(h);
        }
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::SaveSearchData()
    {
        History entry;
        entry.text = m_sci->GetText();
        entry.searchData = m_searchData;
        entry.title = m_searchTitle;
        entry.matchInfo = m_matchInfo;

        // Save the indicators as well
        entry.indicators = m_indicators;

        // search for an entry with the same title
        if (m_history.Contains(entry.title)) { m_history.Remove(entry.title); }
        m_history.PushBack(entry.title, entry);
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::LoadSearch(const History& h)
    {
        m_searchData = h.searchData;
        m_matchInfo = h.matchInfo;
        m_searchTitle = h.title;
        m_sci->SetEditable(true);
        m_sci->ClearAll();
        m_sci->SetText(h.text);

        // restore the indicators
        std::for_each(h.indicators.begin(), h.indicators.end(),
            [&](int pos) { m_sci->IndicatorFillRange(pos, h.title.length()); });
        m_sci->SetFirstVisibleLine(0);
        m_sci->SetEditable(false);
    }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnRecentSearchesUI(wxUpdateUIEvent& e) { e.Enable(!m_history.IsEmpty() && !m_searchInProgress); }
    //-----------------------------------------------------------------------------
    void FindResultsTab::ResetStyler() { m_styler->Reset(); }
    //-----------------------------------------------------------------------------
    void FindResultsTab::OnWorkspaceClosed(wxCommandEvent& event)
    {
        event.Skip();
        Clear();
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // ReplaceInFilesPanel
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    ReplaceInFilesPanel::ReplaceInFilesPanel(wxWindow* parent, int id, const wxString& name)
        : FindResultsTab(parent, id, name)
    {
        gsgs_Style().RegisterWindow(this);
        Bind(wxEVT_UPDATE_UI, &ReplaceInFilesPanel::OnHoldOpenUpdateUI, this, XRCID("hold_pane_open"));
        wxBoxSizer* horzSizer = new wxBoxSizer(wxHORIZONTAL);

        DefaultButton* unmark = new DefaultButton(this, wxID_ANY, _("&Unmark All"));
        horzSizer->Add(unmark, 0, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 5);
        unmark->Bind(wxEVT_BUTTON, &ReplaceInFilesPanel::OnUnmarkAll, this);
        unmark->Bind(wxEVT_UPDATE_UI, &ReplaceInFilesPanel::OnUnmarkAllUI, this);

        DefaultButton* mark = new DefaultButton(this, wxID_ANY, _("Mark &All"));
        horzSizer->Add(mark, 0, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 5);
        mark->Bind(wxEVT_BUTTON, &ReplaceInFilesPanel::OnMarkAll, this);
        mark->Bind(wxEVT_UPDATE_UI, &ReplaceInFilesPanel::OnMarkAllUI, this);

        m_replaceWithText = gsgs_Style().createStaticText(this, wxID_ANY, _("Replace With:"));
        horzSizer->Add(m_replaceWithText, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxLEFT, 5);
        m_replaceWithText->Bind(wxEVT_UPDATE_UI, &ReplaceInFilesPanel::OnReplaceWithComboUI, this);

        m_replaceWith = new wxComboBox(this, wxID_ANY);
        horzSizer->Add(m_replaceWith, 2, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxLEFT, 5);
        m_replaceWith->Bind(wxEVT_UPDATE_UI, &ReplaceInFilesPanel::OnReplaceWithComboUI, this);

        DefaultButton* repl = new DefaultButton(this, wxID_ANY, _("&Replace Marked"));
        horzSizer->Add(repl, 0, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 5);
        repl->Bind(wxEVT_BUTTON, &ReplaceInFilesPanel::OnReplace, this);
        repl->Bind(wxEVT_UPDATE_UI, &ReplaceInFilesPanel::OnReplaceUI, this);

        m_progress = new wxGauge(this, wxID_ANY, 1, wxDefaultPosition, wxSize(-1, 15), wxGA_HORIZONTAL);
        horzSizer->Add(m_progress, 1, wxALIGN_CENTER_VERTICAL | wxALL | wxGA_SMOOTH, 5);

        wxBoxSizer* vertSizer = new wxBoxSizer(wxVERTICAL);
        vertSizer->Add(horzSizer, 0, wxEXPAND | wxTOP | wxBOTTOM);

        // grab the base class scintilla and put our sizer in its place
        wxSizer* mainSizer = m_vSizer;
        mainSizer->Detach(m_sci);
        vertSizer->Add(m_sci, 1, wxEXPAND | wxALL, 1);

        m_tb->DeleteById(XRCID("repeat_output"));
        m_tb->DeleteById(XRCID("recent_searches"));
        m_tb->Realize();
#ifdef __WXMAC__
        mainSizer->Insert(0, vertSizer, 1, wxEXPAND);
#else
        mainSizer->Add(vertSizer, 1, wxEXPAND);
#endif
        mainSizer->Layout();

        m_sci->SetMarginMask(4, 7 << 0x7 | wxSTC_MASK_FOLDERS);
        DefineMarker(m_sci, 0x7, wxSTC_MARK_SMALLRECT, wxColor(0x00, 0x80, 0x00),
            wxColor(0x00, 0xc0, 0x00));                                                           // user selection
        DefineMarker(m_sci, 0x8, wxSTC_MARK_CIRCLE, wxColor(0x80, 0x00, 0x00), wxColor(0xff, 0x00, 0x00)); // error occurred
        DefineMarker(m_sci, 0x9, wxSTC_MARK_EMPTY, wxColor(0x00, 0x00, 0x00),
            wxColor(0x00, 0x00, 0x00)); // replacement successful
    }
    //-----------------------------------------------------------------------------
    ReplaceInFilesPanel::~ReplaceInFilesPanel() { gsgs_Style().UnRegisterWindow(this); }
    //-----------------------------------------------------------------------------
    void ReplaceInFilesPanel::OnSearchStart(wxCommandEvent& e)
    {
        e.Skip();
        // set the "Replace With" field with the user value
        SearchData* data = (SearchData*)e.GetClientData();
        m_replaceWith->ChangeValue(data->GetReplaceWith());
        FindResultsTab::OnSearchStart(e);

        // Make sure that the Output view & the "Replace" tab
        // are visible
        Event event(wxEVT_SHOW_OUTPUT_TAB);
        event.SetSelected(true).SetString(REPLACE_IN_FILES);
        gsgs_Event().AddPendingEvent(event);
    }
    //-----------------------------------------------------------------------------
    void ReplaceInFilesPanel::OnSearchMatch(wxCommandEvent& e)
    {
        e.Skip();
        FindResultsTab::OnSearchMatch(e);
        if (m_matchInfo.size() != 1 || !m_replaceWith->GetValue().IsEmpty()) return;
        m_replaceWith->SetValue(m_matchInfo.begin()->second.GetFindWhat());
        m_replaceWith->SetFocus();
    }
    //-----------------------------------------------------------------------------
    void ReplaceInFilesPanel::OnSearchEnded(wxCommandEvent& e)
    {
        e.Skip();
        SearchSummary* summary = (SearchSummary*)e.GetClientData();
        CHECK_PTR_RET(summary);

        // set the "Replace With" field with the user value
        m_replaceWith->ChangeValue(summary->GetReplaceWith());

        FindResultsTab::OnSearchEnded(e);
        OnMarkAll(e);

        // Set the focus to the "Replace With" field
        m_replaceWith->CallAfter(&wxComboBox::SetFocus);
    }
    //-----------------------------------------------------------------------------
    void ReplaceInFilesPanel::OnMarginClick(wxStyledTextEvent& e)
    {
        int line = m_sci->LineFromPosition(e.GetPosition());
        if (m_matchInfo.find(line) == m_matchInfo.end()) {
            FindResultsTab::OnMarginClick(e);

        }
        else if (m_sci->MarkerGet(line) & 7 << 0x7) {
            m_sci->MarkerDelete(line, 0x7);
        }
        else {
            m_sci->MarkerAdd(line, 0x7);
        }
    }
    //-----------------------------------------------------------------------------
    void ReplaceInFilesPanel::OnMarkAll(wxCommandEvent& e)
    {
        MatchInfo_t::const_iterator i = m_matchInfo.begin();
        for (; i != m_matchInfo.end(); ++i) {
            if (m_sci->MarkerGet(i->first) & 7 << 0x7) continue;
            m_sci->MarkerAdd(i->first, 0x7);
        }
    }
    //-----------------------------------------------------------------------------
    void ReplaceInFilesPanel::OnMarkAllUI(wxUpdateUIEvent& e) { e.Enable((m_sci->GetLength() > 0) && !m_searchInProgress); }
    //-----------------------------------------------------------------------------
    void ReplaceInFilesPanel::OnUnmarkAll(wxCommandEvent& e) { m_sci->MarkerDeleteAll(0x7); }
    //-----------------------------------------------------------------------------
    void ReplaceInFilesPanel::OnUnmarkAllUI(wxUpdateUIEvent& e)
    {
        e.Enable((m_sci->GetLength() > 0) && !m_searchInProgress);
    }
    //-----------------------------------------------------------------------------
    void ReplaceInFilesPanel::DoSaveResults(wxStyledTextCtrl* sci, std::map<int, SearchResult>::iterator begin,
        std::map<int, SearchResult>::iterator end)
    {
        if (!sci || begin == end) return;
        bool ok = true;
        if (dynamic_cast<Editor*>(sci) == NULL) {
            // it's a temp editor, check if we have any changes to save
            if (sci->GetModify() && !WriteFileWithBackup(begin->second.GetFileName(), sci->GetText(), false)) {
                wxMessageBox(_("Failed to save file:\n") + begin->second.GetFileName(), _("gsgs - Replace"),
                    wxICON_ERROR | wxOK);
                gsgsDEBUG() << "Replace: Failed to write file" << begin->second.GetFileName();
                ok = false;
            }

            if (sci && ok) {
                // Keep the modified file name
                m_filesModified.Add(begin->second.GetFileName());
            }

            delete sci;
        }
        for (; begin != end; begin++) {
            if ((m_sci->MarkerGet(begin->first) & 7 << 0x7) == 1 << 0x7) { m_sci->MarkerAdd(begin->first, ok ? 0x9 : 0x8); }
        }
    }
    //-----------------------------------------------------------------------------
    wxStyledTextCtrl* ReplaceInFilesPanel::DoGetEditor(const wxString& fileName)
    {
        // look for open editor first
        wxStyledTextCtrl* sci = gsgs_Frame()->GetMainBook()->FindEditor(fileName);
        if (sci) {
            // FIXME: if editor is already modified, the found locations may not be accurate
            return sci;
        }

        // not open for editing, so make our own temp editor
        wxString content;
        if (!ReadFileWithConversion(fileName, content)) {
            wxMessageBox(_("Failed to open file:\n") + fileName, _("gsgs - Replace"), wxICON_ERROR | wxOK);
            gsgsDEBUG() << "Replace: Failed to read file" << fileName;
            return NULL;
        }

        sci = new wxStyledTextCtrl(this);
        sci->Hide();
        sci->SetText(content);
        return sci;
    }
    //-----------------------------------------------------------------------------
    void ReplaceInFilesPanel::OnReplace(wxCommandEvent& e)
    {
        m_filesModified.clear();
        // FIX bug#2770561
        int lineNumber(0);
        Editor* activeEditor = gsgs_Frame()->GetMainBook()->GetActiveEditor();
        if (activeEditor) { lineNumber = activeEditor->GetCurrentLine(); }

        if (m_replaceWith->FindString(m_replaceWith->GetValue(), true) == wxNOT_FOUND) {
            m_replaceWith->Append(m_replaceWith->GetValue());
        }

        // Step 1: apply selected replacements

        wxStyledTextCtrl* sci = NULL; // file that is being altered by replacements

        wxString lastFile; // track offsets of pending substitutions caused by previous substitutions
        long lastLine = 0;
        long delta = 0;

        // remembers first entry in the file being updated
        MatchInfo_t::iterator firstInFile = m_matchInfo.begin();

        m_progress->SetRange(m_matchInfo.size());

        // Disable the 'buffer limit' feature during replace
        gsgs_Frame()->GetMainBook()->SetUseBuffereLimit(false);
        MatchInfo_t::iterator i = firstInFile;
        for (; i != m_matchInfo.end(); ++i) {
            m_progress->SetValue(m_progress->GetValue() + 1);
            m_progress->Update();

            if (i->second.GetFileName() != lastFile) {
                // about to start a different file, save current results
                DoSaveResults(sci, firstInFile, i);
                firstInFile = i;
                lastFile = i->second.GetFileName();
                lastLine = 0;
                sci = NULL;
            }

            if (i->second.GetLineNumber() == lastLine) {
                // prior substitutions affected the location of this one
                i->second.SetColumn(i->second.GetColumn() + delta);
            }
            else {
                delta = 0;
            }
            if ((m_sci->MarkerGet(i->first) & 1 << 0x7) == 0)
                // not selected for application
                continue;

            // extract originally matched text for safety check later
            wxString text = i->second.GetPattern().Mid(i->second.GetColumn() - delta, i->second.GetLen());
            if (text == m_replaceWith->GetValue()) continue; // no change needed

            // need an editor for this file (try only once per file though)
            if (!sci && lastLine == 0) {
                sci = DoGetEditor(i->second.GetFileName());
                lastLine = i->second.GetLineNumber();
            }
            if (!sci) {
                // couldn't open file
                m_sci->MarkerAdd(i->first, 0x8);
                continue;
            }

            long pos = sci->PositionFromLine(i->second.GetLineNumber() - 1);
            if (pos < 0) {
                // invalid line number
                m_sci->MarkerAdd(i->first, 0x8);
                continue;
            }
            pos += i->second.GetColumn();

            sci->SetSelection(pos, pos + i->second.GetLen());
            if (sci->GetSelectedText() != text) {
                // couldn't locate the original match (file may have been modified)
                m_sci->MarkerAdd(i->first, 0x8);
                continue;
            }
            sci->ReplaceSelection(m_replaceWith->GetValue());

            delta += m_replaceWith->GetValue().Length() - i->second.GetLen();
            lastLine = i->second.GetLineNumber();

            i->second.SetPattern(m_sci->GetLine(i->first)); // includes prior updates to same line
            i->second.SetLen(m_replaceWith->GetValue().Length());
        }
        m_progress->SetValue(0);
        DoSaveResults(sci, firstInFile, m_matchInfo.end());

        // Disable the 'buffer limit' feature during replace
        gsgs_Frame()->GetMainBook()->SetUseBuffereLimit(true);

        // Step 2: Update the Replace pane

        std::set<wxString> updatedEditors;
        delta = 0;    // offset from old line number to new
        lastLine = 1; // points to the filename line
        lastFile.Clear();
        m_sci->MarkerDeleteAll(0x7);
        m_sci->SetReadOnly(false);

        std::vector<int> itemsToRemove;
        i = m_matchInfo.begin();
        for (; i != m_matchInfo.end(); i++) {
            int line = i->first + delta;
            if (i->second.GetFileName() != lastFile) {
                if (lastLine == line - 2) {
                    // previous file's replacements are all done, so remove its filename line
                    m_sci->SetCurrentPos(m_sci->PositionFromLine(lastLine));
                    m_sci->LineDelete();
                    delta--;
                    line--;
                }
                else {
                    lastLine = line - 1;
                }
                lastFile = i->second.GetFileName();
            }

            if (m_sci->MarkerGet(line) & 1 << 0x9) {
                Editor* editor = gsgs_Frame()->GetMainBook()->FindEditor(lastFile);
                if (editor && editor->GetModify()) { updatedEditors.insert(lastFile); }
                // replacement done, remove from map
                m_sci->MarkerDelete(line, 0x9);
                m_sci->SetCurrentPos(m_sci->PositionFromLine(line));
                m_sci->LineDelete();
                itemsToRemove.push_back(i->first);
                delta--;
            }
            else if (line != i->first) {
                // need to adjust line number
                m_matchInfo[line] = i->second;
                itemsToRemove.push_back(i->first);
            }
        }

        // update the match info map
        for (std::vector<int>::size_type i = 0; i < itemsToRemove.size(); i++) {
            MatchInfo_t::iterator iter = m_matchInfo.find(itemsToRemove.at(i));
            if (iter != m_matchInfo.end()) { m_matchInfo.erase(iter); }
        }

        m_sci->SetReadOnly(true);
        m_sci->GotoLine(0);
        if (m_matchInfo.empty()) { Clear(); }

        // Step 3: Notify user of changes to already opened files, ask to save
        std::vector<std::pair<wxFileName, bool> > filesToSave;
        for (std::set<wxString>::iterator i = updatedEditors.begin(); i != updatedEditors.end(); i++) {
            filesToSave.push_back(std::make_pair(wxFileName(*i), true));
        }
        if (!filesToSave.empty() && gsgs_Frame()->GetMainBook()->UserSelectFiles(
            filesToSave, _("Save Modified Files"),
            _("Some files are modified.\nChoose the files you would like to save."), true)) {
            for (size_t i = 0; i < filesToSave.size(); i++) {
                if (filesToSave[i].second) {
                    Editor* editor = gsgs_Frame()->GetMainBook()->FindEditor(filesToSave[i].first.GetFullPath());
                    if (editor) { editor->SaveFile(); }
                }
            }
        }

        // FIX bug#2770561
        if (activeEditor) {

            gsgs_Frame()->GetMainBook()->SelectPage(activeEditor);

            // restore the line
            activeEditor->GotoLine(lineNumber);
        }

        if (!m_filesModified.IsEmpty()) {
            // Some files were modified directly on the file system, notify about it to the plugins
            FileEvent event(wxEVT_FILES_MODIFIED_REPLACE_IN_FILES);
            event.SetStrings(m_filesModified);
            gsgs_Event().AddPendingEvent(event);
            m_filesModified.clear();
        }
    }
    //-----------------------------------------------------------------------------
    void ReplaceInFilesPanel::OnReplaceUI(wxUpdateUIEvent& e) { e.Enable((m_sci->GetLength() > 0) && !m_searchInProgress); }
    //-----------------------------------------------------------------------------
    void ReplaceInFilesPanel::OnReplaceWithComboUI(wxUpdateUIEvent& e)
    {
        e.Enable((m_sci->GetLength() > 0) && !m_searchInProgress);
    }
    //-----------------------------------------------------------------------------
    void ReplaceInFilesPanel::OnHoldOpenUpdateUI(wxUpdateUIEvent& e)
    {
        int sel = gsgs_Frame()->GetOutputPane()->GetNotebook()->GetSelection();
        if (gsgs_Frame()->GetOutputPane()->GetNotebook()->GetPage(sel) != this) { return; }

        if (gsgs_EditorConfig().GetOptions()->GetHideOutpuPaneOnUserClick()) {
            e.Enable(true);
            e.Check(gsgs_EditorConfig().GetOptions()->GetHideOutputPaneNotIfReplace());

        }
        else {
            e.Enable(false);
            e.Check(false);
        }
    }
    //-----------------------------------------------------------------------------
    void ReplaceInFilesPanel::OnMouseDClick(wxStyledTextEvent& e)
    {
        int clickedLine = wxNOT_FOUND;
        m_styler->HitTest(m_sci, e, clickedLine);

        // Did we clicked on a togglable line?
        int toggleLine = m_styler->TestToggle(m_sci, e);
        if (toggleLine != wxNOT_FOUND)
        {
            m_sci->ToggleFold(toggleLine);

        }
        else
        {
            MatchInfo_t::const_iterator m = m_matchInfo.find(clickedLine);
            if (m != m_matchInfo.end()) { DoOpenSearchResult(m->second, NULL, m->first); }
        }
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // OutputDebugStringThread
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    wxDECLARE_EVENT(wxEVT_OUTPUT_DEBUG_STRING, Event);
    class OutputDebugStringThread : public wxThread
    {
#ifdef __WXMSW__
        struct dbwin_buffer {
            DWORD dwProcessId;
            char data[4096 - sizeof(DWORD)];
        };
        HANDLE m_hDBWinMutex;
        HANDLE m_hDBMonBuffer;
        HANDLE m_hEventBufferReady;
        HANDLE m_hEventDataReady;
        dbwin_buffer* m_pDBBuffer;
#endif
        bool m_collecting;

    public:
        OutputDebugStringThread();
        virtual ~OutputDebugStringThread();

    public:
        /**
         * @brief the thread entry point
         */
        virtual void* Entry();

        /**
         * Stops the thread
         * This function returns only when the thread is terminated.
         * \note This call must be called from the context of other thread (e.g. main thread)
         */
        void Stop();

        /**
         * Start the thread as joinable thread.
         * \note This call must be called from the context of other thread (e.g. main thread)
         */
        void Start(int priority = WXTHREAD_DEFAULT_PRIORITY);

        /**
         * @brief enable collection
         */
        void SetCollecting(bool collecting) { this->m_collecting = collecting; }
    };
    //-----------------------------------------------------------------------------
    wxDEFINE_EVENT(wxEVT_OUTPUT_DEBUG_STRING, Event);
    //-----------------------------------------------------------------------------
    OutputDebugStringThread::OutputDebugStringThread()
        : wxThread(wxTHREAD_JOINABLE)
#ifdef __WXMSW__
        , m_hDBWinMutex(INVALID_HANDLE_VALUE)
        , m_hDBMonBuffer(INVALID_HANDLE_VALUE)
        , m_hEventBufferReady(INVALID_HANDLE_VALUE)
        , m_hEventDataReady(INVALID_HANDLE_VALUE)
        , m_pDBBuffer(NULL)
#endif
        , m_collecting(false)
    {
#ifdef __WXMSW__
        SetLastError(0);

        // Mutex: DBWin
        // ---------------------------------------------------------
        m_hDBWinMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"DBWinMutex");

        if (m_hDBWinMutex == NULL) {
            gsgsWARNING() << "Failed to open mutex: 'DBWinMutex'." << GetLastError() << clEndl;
            return;
        }

        m_hEventBufferReady = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, L"DBWIN_BUFFER_READY");

        if (m_hEventBufferReady == NULL) {
            m_hEventBufferReady = ::CreateEvent(NULL,
                FALSE, // auto-reset
                TRUE,  // initial state: signaled
                L"DBWIN_BUFFER_READY");

            if (m_hEventBufferReady == NULL) {
                gsgsWARNING() << "Failed to create event: 'DBWIN_BUFFER_READY'." << GetLastError() << clEndl;
                return;
            }
        }
        // Event: data ready
        // ---------------------------------------------------------
        m_hEventDataReady = ::OpenEvent(SYNCHRONIZE, FALSE, L"DBWIN_DATA_READY");

        if (m_hEventDataReady == NULL) {
            m_hEventDataReady = ::CreateEvent(NULL,
                FALSE, // auto-reset
                FALSE, // initial state: nonsignaled
                L"DBWIN_DATA_READY");

            if (m_hEventDataReady == NULL) {
                gsgsWARNING() << "Failed to create event: 'DBWIN_DATA_READY'." << GetLastError() << clEndl;
                return;
            }
        }

        // Shared memory
        // ---------------------------------------------------------
        wxString DBWIN_BUFFER = L"DBWIN_BUFFER";
        m_hDBMonBuffer = ::OpenFileMapping(FILE_MAP_READ, FALSE, DBWIN_BUFFER);

        if (m_hDBMonBuffer == NULL) {
            m_hDBMonBuffer = ::CreateFileMapping(
                INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(struct dbwin_buffer), DBWIN_BUFFER);

            if (m_hDBMonBuffer == NULL) {
                gsgsWARNING() << "Failed to CreateFileMapping:" << DBWIN_BUFFER << "." << GetLastError() << clEndl;
                return;
            }
        }

        m_pDBBuffer = (struct dbwin_buffer*)::MapViewOfFile(m_hDBMonBuffer, SECTION_MAP_READ, 0, 0, 0);

        if (m_pDBBuffer == NULL) {
            gsgsWARNING() << "Failed to MapViewOfFile:" << DBWIN_BUFFER << "." << GetLastError() << clEndl;
            return;
        }
#endif
    }
    //-----------------------------------------------------------------------------
    OutputDebugStringThread::~OutputDebugStringThread()
    {
#ifdef __WXMSW__
        if (m_hDBWinMutex != NULL) {
            CloseHandle(m_hDBWinMutex);
            m_hDBWinMutex = NULL;
        }

        if (m_hDBMonBuffer != NULL) {
            ::UnmapViewOfFile(m_pDBBuffer);
            CloseHandle(m_hDBMonBuffer);
            m_hDBMonBuffer = NULL;
        }

        if (m_hEventBufferReady != NULL) {
            CloseHandle(m_hEventBufferReady);
            m_hEventBufferReady = NULL;
        }

        if (m_hEventDataReady != NULL) {
            CloseHandle(m_hEventDataReady);
            m_hEventDataReady = NULL;
        }

        m_pDBBuffer = NULL;
#endif
    }
    //-----------------------------------------------------------------------------
    void* OutputDebugStringThread::Entry()
    {
#ifdef __WXMSW__
        while (!TestDestroy()) {
            if (m_collecting) {
                DWORD ret = 0;

                // wait for data ready
                ret = ::WaitForSingleObject(m_hEventDataReady, 100);

                if (ret == WAIT_OBJECT_0) {
                    // Send the data
                    wxString data = m_pDBBuffer->data;
                    int processID = m_pDBBuffer->dwProcessId;
                    Event event(wxEVT_OUTPUT_DEBUG_STRING);
                    event.SetInt(processID);
                    event.SetString(data);

                    gsgs_Event().AddPendingEvent(event);
                    // signal buffer ready
                    SetEvent(m_hEventBufferReady);
                }

            }
            else {
                wxMilliSleep(100);
            }
        }
#endif
        return NULL;
    }
    //-----------------------------------------------------------------------------
    void OutputDebugStringThread::Stop()
    {
        // Notify the thread to exit and
        // wait for it
        if (IsAlive()) {
            Delete(NULL, wxTHREAD_WAIT_BLOCK);

        }
        else {
            Wait(wxTHREAD_WAIT_BLOCK);
        }
    }
    //-----------------------------------------------------------------------------
    void OutputDebugStringThread::Start(int priority)
    {
        Create();
        SetPriority(priority);
        Run();
    }

    //-----------------------------------------------------------------------------
    BEGIN_EVENT_TABLE(ShellTab, OutputTabWindow)
        EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_STARTED, ShellTab::OnProcStarted)
        EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ADDLINE, ShellTab::OnProcOutput)
        EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ADDERRLINE, ShellTab::OnProcError)
        EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ENDED, ShellTab::OnProcEnded)
        EVT_BUTTON(XRCID("send_input"), ShellTab::OnSendInput)
        EVT_BUTTON(XRCID("stop_process"), ShellTab::OnStopProc)
        EVT_UPDATE_UI(XRCID("send_input"), ShellTab::OnUpdateUI)
        EVT_UPDATE_UI(XRCID("stop_process"), ShellTab::OnUpdateUI)
        EVT_UPDATE_UI(XRCID("hold_pane_open"), ShellTab::OnHoldOpenUpdateUI)
        END_EVENT_TABLE()
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        // ShellTab
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        ShellTab::ShellTab(wxWindow* parent, wxWindowID id, const wxString& name)
        : OutputTabWindow(parent, id, name)
        , m_inputSizer(NULL)
        , m_input(NULL)
        , m_cmd(NULL)
    {
        m_inputSizer = new wxBoxSizer(wxHORIZONTAL);

        wxStaticText* text = gsgs_Style().createStaticText(this, wxID_ANY, _("Send:"));
        m_inputSizer->Add(text, 0, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 0);

        m_input =
            new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER);
        m_input->SetMinSize(wxSize(200, -1));
        m_input->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ShellTab::OnEnter), NULL, this);
        m_input->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(ShellTab::OnKeyDown), NULL, this);

        m_inputSizer->Add(m_input, 1, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

        Button* btn;
        btn = new Button(this, XRCID("send_input"), _("Send"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
        m_inputSizer->Add(btn, 0, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

        btn = new Button(this, XRCID("stop_process"), _("Stop"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
        m_inputSizer->Add(btn, 0, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

        // grab the base class scintilla and put our sizer in its place
        wxSizer* mainSizer = m_vSizer;
        mainSizer->Detach(m_sci);

        m_vertSizer = new wxBoxSizer(wxVERTICAL);
        m_vertSizer->Add(m_sci, 1, wxEXPAND | wxALL, 1);
        m_vertSizer->Add(m_inputSizer, 0, wxEXPAND | wxALL, 1);
        mainSizer->Add(m_vertSizer, 1, wxEXPAND | wxALL, 1);

        mainSizer->Layout();
        InitStyle(m_sci);
    }
    //-----------------------------------------------------------------------------
    ShellTab::~ShellTab()
    {
    }
    //-----------------------------------------------------------------------------
    void ShellTab::InitStyle(wxStyledTextCtrl* sci)
    {
        LexerStyle::Ptr_t text = gsgs_EditorConfig().GetLexer("text");
        text->Apply(sci);
    }
    //-----------------------------------------------------------------------------
    bool ShellTab::DoSendInput(const wxString& line)
    {
        return m_cmd && m_cmd->IsBusy() && m_cmd->GetProcess()->Write(line + wxT('\n'));
    }
    //-----------------------------------------------------------------------------
    void ShellTab::OnProcStarted(wxCommandEvent& e)
    {
        if (m_cmd && m_cmd->IsBusy()) { return; }
        m_cmd = (ExecCmd*)e.GetEventObject();
        AppendText("\n");
        AppendText(e.GetString());
        m_input->Clear();
    }
    //-----------------------------------------------------------------------------
    void ShellTab::OnProcOutput(wxCommandEvent& e)
    {
        ExecCmd* cmd = (ExecCmd*)e.GetEventObject();
        if (cmd != m_cmd)
        {
            // TODO: log message
            return;
        }
        AppendText(e.GetString());
    }
    //-----------------------------------------------------------------------------
    void ShellTab::OnProcError(wxCommandEvent& e)
    {
        ExecCmd* cmd = (ExecCmd*)e.GetEventObject();
        if (cmd != m_cmd)
        {
            // TODO: log message
            return;
        }
        AppendText(e.GetString());
    }
    //-----------------------------------------------------------------------------
    void ShellTab::OnProcEnded(wxCommandEvent& e)
    {
        ExecCmd* cmd = (ExecCmd*)e.GetEventObject();
        if (cmd != m_cmd)
        {
            // TODO: log message
            return;
        }
        AppendText(e.GetString());
        m_cmd = NULL;
    }
    //-----------------------------------------------------------------------------
    void ShellTab::OnSendInput(wxCommandEvent& e)
    {
        wxUnusedVar(e);

        wxString line = m_input->GetValue();
        if (DoSendInput(line))
        {
            if (m_input->FindString(line) == wxNOT_FOUND) { m_input->Append(line); }
            m_input->SetValue(wxEmptyString);
            m_input->SetFocus(); // in case lost by editor changing
        }
    }
    //-----------------------------------------------------------------------------
    void ShellTab::OnStopProc(wxCommandEvent& e)
    {
        wxUnusedVar(e);

        if (m_cmd && m_cmd->IsBusy()) { m_cmd->Terminate(); }
    }
    //-----------------------------------------------------------------------------
    void ShellTab::OnUpdateUI(wxUpdateUIEvent& e) { e.Enable(m_cmd && m_cmd->IsBusy()); }
    //-----------------------------------------------------------------------------
    void ShellTab::OnKeyDown(wxKeyEvent& e)
    {
        wxCommandEvent dummy;
        switch (e.GetKeyCode())
        {
        case wxT('c'):
        case wxT('C'):
            if (e.GetModifiers() == wxMOD_CONTROL)
            {
                OnStopProc(dummy);
            }
            else {
                e.Skip();
            }
            break;
        default:
            e.Skip();
            break;
        }
    }
    //-----------------------------------------------------------------------------
    void ShellTab::OnEnter(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        wxCommandEvent dummy;
        OnSendInput(dummy);
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // DebugTab
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    DebugTab::DebugTab(wxWindow* parent, wxWindowID id, const wxString& name)
        : wxPanel(parent, id)
        , m_terminal(new wxTerminal(this))
    {
        wxUnusedVar(name);
        m_terminal->SetInteractive(true);

        // Toolbar
        ImageManager* bmpLoader = &gsgs_Image();
        m_toolbar = new ToolBar(this);
        wxCheckBox* checkBoxLog = new wxCheckBox(m_toolbar, wxID_ANY, _("Enable Log"));
        checkBoxLog->Bind(wxEVT_CHECKBOX, &DebugTab::OnEnableDbgLog, this);
        checkBoxLog->Bind(wxEVT_UPDATE_UI, &DebugTab::OnEnableDbgLogUI, this);
        m_toolbar->AddTool(XRCID("hold_pane_open"), _("Keep open"), bmpLoader->LoadBitmap("ToolPin"),
            _("Don't close this pane when an editor gets focus"), wxITEM_CHECK);
        m_toolbar->AddTool(XRCID("word_wrap_output"), _("Word Wrap"), bmpLoader->LoadBitmap("word_wrap"), _("Word Wrap"),
            wxITEM_CHECK);
        m_toolbar->AddTool(XRCID("clear_all_output"), _("Clear All"), bmpLoader->LoadBitmap("clear"), _("Clear All"));
        m_toolbar->AddSeparator();
        m_toolbar->AddControl(checkBoxLog);
        m_toolbar->Realize();

        m_toolbar->Bind(wxEVT_TOOL,
            [&](wxCommandEvent& event) {
            int sel = wxNOT_FOUND;
            Notebook* book = gsgs_Frame()->GetOutputPane()->GetNotebook();
            if (book && (sel = book->GetSelection()) != wxNOT_FOUND) {
                gsgs_EditorConfig().SetPaneStickiness(book->GetPageText(sel), event.IsChecked());
            }
        },
            XRCID("hold_pane_open"));
        m_toolbar->Bind(wxEVT_UPDATE_UI,
            [&](wxUpdateUIEvent& event) {
            int sel = wxNOT_FOUND;
            Notebook* book = gsgs_Frame()->GetOutputPane()->GetNotebook();
            if (book && (sel = book->GetSelection()) != wxNOT_FOUND) {
                event.Check(gsgs_EditorConfig().GetPaneStickiness(book->GetPageText(sel)));
            }
        },
            XRCID("hold_pane_open"));
        m_toolbar->Bind(wxEVT_TOOL,
            [&](wxCommandEvent& event) {
            m_terminal->GetCtrl()->SetWrapMode(event.IsChecked() ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE);
        },
            XRCID("word_wrap_output"));
        m_toolbar->Bind(
            wxEVT_UPDATE_UI,
            [&](wxUpdateUIEvent& event) { event.Check(m_terminal->GetCtrl()->GetWrapMode() == wxSTC_WRAP_WORD); },
            XRCID("word_wrap_output"));

        m_toolbar->Bind(wxEVT_TOOL, [&](wxCommandEvent& event) { m_terminal->Clear(); }, XRCID("clear_all_output"));
        m_toolbar->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) { event.Enable(!m_terminal->IsEmpty()); },
            XRCID("clear_all_output"));
        SetSizer(new wxBoxSizer(wxVERTICAL));
        GetSizer()->Add(m_toolbar, 0, wxEXPAND);
        GetSizer()->Add(m_terminal, 1, wxEXPAND);

        m_terminal->Bind(wxEVT_TERMINAL_CTRL_C, &DebugTab::OnCtrlC, this);
        m_terminal->Bind(wxEVT_TERMINAL_EXECUTE_COMMAND, &DebugTab::OnExecuteCommand, this);
        Bind(wxEVT_UPDATE_UI, &DebugTab::OnHoldOpenUpdateUI, this, XRCID("hold_pane_open"));
    }
    //-----------------------------------------------------------------------------
    DebugTab::~DebugTab()
    {
        m_terminal->Unbind(wxEVT_TERMINAL_CTRL_C, &DebugTab::OnCtrlC, this);
        m_terminal->Unbind(wxEVT_TERMINAL_EXECUTE_COMMAND, &DebugTab::OnExecuteCommand, this);
        Unbind(wxEVT_UPDATE_UI, &DebugTab::OnHoldOpenUpdateUI, this, XRCID("hold_pane_open"));
    }
    //-----------------------------------------------------------------------------
    void DebugTab::AppendLine(const wxString& line) { m_terminal->AddTextWithEOL(line); }
    //-----------------------------------------------------------------------------
    void DebugTab::OnUpdateUI(wxUpdateUIEvent& e)
    {
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        e.Enable(dbgr && dbgr->IsRunning());
    }
    //-----------------------------------------------------------------------------
    void DebugTab::OnEnableDbgLog(wxCommandEvent& event)
    {
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if (dbgr) {
            dbgr->EnableLogging(event.IsChecked());
            DebuggerInformation info = dbgr->GetDebuggerInformation();
            info.enableDebugLog = event.IsChecked();
            DebuggerMgr::Get().SetDebuggerInformation(dbgr->GetName(), info);
        }
    }
    //-----------------------------------------------------------------------------
    void DebugTab::OnEnableDbgLogUI(wxUpdateUIEvent& event)
    {
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if (dbgr) {
            DebuggerInformation info = dbgr->GetDebuggerInformation();
            event.Check(info.enableDebugLog);
        }
    }
    //-----------------------------------------------------------------------------
    void DebugTab::OnHoldOpenUpdateUI(wxUpdateUIEvent& e)
    {
        int sel = gsgs_Frame()->GetOutputPane()->GetNotebook()->GetSelection();
        if (gsgs_Frame()->GetOutputPane()->GetNotebook()->GetPage(sel) != this) { return; }

        if (gsgs_EditorConfig().GetOptions()->GetHideOutpuPaneOnUserClick()) {
            e.Enable(true);
            e.Check(gsgs_EditorConfig().GetOptions()->GetHideOutputPaneNotIfDebug());

        }
        else {
            e.Enable(false);
            e.Check(false);
        }
    }
    //-----------------------------------------------------------------------------
    void DebugTab::OnCtrlC(Event& event)
    {
        wxUnusedVar(event);
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if (dbgr && dbgr->IsRunning()) { gsgs_Manager()->DbgDoSimpleCommand(DBG_PAUSE); }
    }
    //-----------------------------------------------------------------------------
    void DebugTab::OnExecuteCommand(Event& event)
    {
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if (!dbgr || !dbgr->IsRunning()) { return; }
        bool contIsNeeded = gsgs_Manager()->GetBreakpointsMgr()->PauseDebuggerIfNeeded();
        dbgr->ExecuteCmd(event.GetString());
        if (contIsNeeded) { gsgs_Manager()->DbgContinue(); }
    }
    //-----------------------------------------------------------------------------
    void ShellTab::OnHoldOpenUpdateUI(wxUpdateUIEvent& e)
    {
        int sel = gsgs_Frame()->GetOutputPane()->GetNotebook()->GetSelection();
        if (gsgs_Frame()->GetOutputPane()->GetNotebook()->GetPage(sel) != this) { return; }

        if (gsgs_EditorConfig().GetOptions()->GetHideOutpuPaneOnUserClick()) {
            e.Enable(true);
            e.Check(gsgs_EditorConfig().GetOptions()->GetHideOutputPaneNotIfOutput());

        }
        else {
            e.Enable(false);
            e.Check(false);
        }
    }
    //-----------------------------------------------------------------------------
    void ShellTab::OnTheme(wxCommandEvent& e)
    {
        e.Skip();
        LexerStyle::Ptr_t l = gsgs_Style().GetLexer("text");
        l->Apply(m_sci);
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // OutputTab class
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    OutputTab::OutputTab(wxWindow* parent, wxWindowID id, const wxString& name)
        : ShellTab(parent, id, name)
        , m_thread(NULL)
        , m_outputDebugStringActive(false)
    {
        m_inputSizer->Show(false);
        GetSizer()->Layout();
        gsgs_Event().Bind(wxEVT_OUTPUT_DEBUG_STRING, &OutputTab::OnOutputDebugString, this);
        gsgs_Event().Bind(wxEVT_DEBUG_STARTED, &OutputTab::OnDebugStarted, this);
        gsgs_Event().Bind(wxEVT_DEBUG_ENDED, &OutputTab::OnDebugStopped, this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_CLOSED, &OutputTab::OnWorkspaceClosed, this);
    }
    //-----------------------------------------------------------------------------
    OutputTab::~OutputTab()
    {
        gsgs_Event().Unbind(wxEVT_OUTPUT_DEBUG_STRING, &OutputTab::OnOutputDebugString, this);
        gsgs_Event().Unbind(wxEVT_DEBUG_STARTED, &OutputTab::OnDebugStarted, this);
        gsgs_Event().Unbind(wxEVT_DEBUG_ENDED, &OutputTab::OnDebugStopped, this);
        gsgs_Event().Unbind(wxEVT_WORKSPACE_CLOSED, &OutputTab::OnWorkspaceClosed, this);
        if (m_thread) {
            m_thread->Stop();
            wxDELETE(m_thread);
        }
    }
    //-----------------------------------------------------------------------------
    void OutputTab::OnOutputDebugString(Event& event)
    {
        event.Skip();
        if (!m_outputDebugStringActive) return;

        wxString msg = event.GetString();
        msg.Trim().Trim(false);
        if (msg.IsEmpty()) return;

        wxString formattedMessage;
        formattedMessage << "[" << event.GetInt() << "] " << msg << "\n";
        AppendText(formattedMessage);
    }
    //-----------------------------------------------------------------------------
    void OutputTab::OnDebugStarted(DebugEvent& event)
    {
        event.Skip();
        m_outputDebugStringActive = true;
        m_autoAppear = false;
        DoSetCollecting(true);
    }
    //-----------------------------------------------------------------------------
    void OutputTab::OnDebugStopped(DebugEvent& event)
    {
        event.Skip();
        m_outputDebugStringActive = false;
        DoSetCollecting(false);
        m_autoAppear = true;
    }
    //-----------------------------------------------------------------------------
    void OutputTab::OnProcStarted(wxCommandEvent& e)
    {
        ShellTab::OnProcStarted(e);
        m_outputDebugStringActive = true;
        DoSetCollecting(true);
    }
    //-----------------------------------------------------------------------------
    void OutputTab::OnProcEnded(wxCommandEvent& e)
    {
        ShellTab::OnProcEnded(e);
        m_outputDebugStringActive = false;
        DoSetCollecting(false);
    }
    //-----------------------------------------------------------------------------
    void OutputTab::OnWorkspaceClosed(wxCommandEvent& event)
    {
        event.Skip();
        Clear();
    }
    //-----------------------------------------------------------------------------
    void OutputTab::DoSetCollecting(bool b)
    {
#ifdef __WXMSW__
        if (b)
        {
            if (m_thread)
            {
                m_thread->Stop();
                wxDELETE(m_thread);
            }
            m_thread = new OutputDebugStringThread();
            m_thread->Start();
            m_thread->SetCollecting(true);
        }
        else
        {
            if (m_thread)
            {
                m_thread->Stop();
                wxDELETE(m_thread);
            }
        }
#endif
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // OutputPane
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    OutputPane::OutputPane(wxWindow* parent, const wxString& caption)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(200, 250))
        , m_caption(caption)
        , m_buildInProgress(false)
    {
        wxWindowUpdateLocker temp(this);
        CreateViewImpl();
        gsgs_Event().Connect(wxEVT_EDITOR_CLICKED, wxCommandEventHandler(OutputPane::OnEditorFocus), NULL, this);
        gsgs_Event().Connect(wxEVT_BUILD_STARTED, gsgsBuildEventHandler(OutputPane::OnBuildStarted), NULL, this);
        gsgs_Event().Connect(wxEVT_BUILD_ENDED, gsgsBuildEventHandler(OutputPane::OnBuildEnded), NULL, this);
        gsgs_Event().Bind(wxEVT_EDITOR_CONFIG_CHANGED, &OutputPane::OnSettingsChanged, this);
        gsgs_Event().Bind(wxEVT_SHOW_OUTPUT_TAB, &OutputPane::OnToggleTab, this);
        //SetSize(-1, 250);
    }
    //-----------------------------------------------------------------------------
    OutputPane::~OutputPane()
    {
        gsgs_Event().Disconnect(wxEVT_EDITOR_CLICKED, wxCommandEventHandler(OutputPane::OnEditorFocus), NULL, this);
        gsgs_Event().Disconnect(wxEVT_BUILD_STARTED, gsgsBuildEventHandler(OutputPane::OnBuildStarted), NULL, this);
        gsgs_Event().Disconnect(wxEVT_BUILD_ENDED, gsgsBuildEventHandler(OutputPane::OnBuildEnded), NULL, this);
        gsgs_Event().Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &OutputPane::OnSettingsChanged, this);
        gsgs_Event().Unbind(wxEVT_SHOW_OUTPUT_TAB, &OutputPane::OnToggleTab, this);
    }
    //-----------------------------------------------------------------------------
    void OutputPane::CreateViewImpl()
    {
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        SetSizer(mainSizer);
        SetMinClientSize(wxSize(-1, 250));
        long style = kNotebook_Default | kNotebook_AllowDnD | kNotebook_UnderlineActiveTab;

        wxDirection tempdir = gsgs_EditorConfig().GetOptions()->GetOutputTabsDirection();
        if (tempdir == wxBOTTOM)
            style |= kNotebook_BottomTabs;
        else if (tempdir == wxRIGHT)
            style |= kNotebook_RightTabs;
        else if (tempdir == wxLEFT)
            style |= kNotebook_LeftTabs;

        if(gsgs_EditorConfig().GetOptions()->IsTabColourDark()) 
        {
            style &= ~kNotebook_LightTabs;
            style |= kNotebook_DarkTabs;
        }
        if(gsgs_EditorConfig().GetOptions()->IsMouseScrollSwitchTabs()) 
        { 
            style |= kNotebook_MouseScrollSwitchTabs; 
        }
        m_book = new Notebook(this, wxID_ANY, wxDefaultSize, wxDefaultPosition, wxDefaultSize, style);
        m_book->Bind(wxEVT_BOOK_FILELIST_BUTTON_CLICKED, &OutputPane::OnOutputBookFileListMenu, this);

        ImageManager* bmpLoader = &gsgs_Image();

        // Calculate the widest tab (the one with the 'Workspace' label) TODO: What happens with translations?
        mainSizer->Add(m_book, 1, wxEXPAND | wxALL | wxGROW, 0);

        // the IManager instance
        IManager* mgr = PluginManager::Get();

        // Build tab
        m_buildWin = new NewBuildTab(m_book);
    #if PHP_BUILD
        m_buildWin->Hide();
    #else
        m_book->AddPage(m_buildWin, wxGetTranslation(BUILD_WIN), true, bmpLoader->LoadBitmap(wxT("build")));
        m_tabs.insert(std::make_pair(wxGetTranslation(BUILD_WIN), Tab(wxGetTranslation(BUILD_WIN), m_buildWin, bmpLoader->LoadBitmap(wxT("build")))));
        mgr->AddOutputTab(wxGetTranslation(BUILD_WIN));
    #endif

        // Output tab
        m_outputWind = new OutputTab(m_book, wxID_ANY, wxGetTranslation(OUTPUT_WIN));
        m_book->AddPage(m_outputWind, wxGetTranslation(OUTPUT_WIN), false, bmpLoader->LoadBitmap(wxT("console")));
        m_tabs.insert(std::make_pair(wxGetTranslation(OUTPUT_WIN), Tab(wxGetTranslation(OUTPUT_WIN), m_outputWind,
            bmpLoader->LoadBitmap(wxT("console")))));
        mgr->AddOutputTab(wxGetTranslation(OUTPUT_WIN));

        // Find in files
        m_findResultsTab = new FindResultsTab(m_book, wxID_ANY, wxGetTranslation(FIND_IN_FILES_WIN));
        m_book->AddPage(m_findResultsTab, wxGetTranslation(FIND_IN_FILES_WIN), false, bmpLoader->LoadBitmap(wxT("find")));
        m_tabs.insert(std::make_pair(wxGetTranslation(FIND_IN_FILES_WIN), Tab(wxGetTranslation(FIND_IN_FILES_WIN), m_findResultsTab, bmpLoader->LoadBitmap(wxT("find")))));
        mgr->AddOutputTab(wxGetTranslation(FIND_IN_FILES_WIN));

        // Replace In Files
        m_replaceResultsTab = new ReplaceInFilesPanel(m_book, wxID_ANY, wxGetTranslation(REPLACE_IN_FILES));
        m_book->AddPage(m_replaceResultsTab, wxGetTranslation(REPLACE_IN_FILES), false, bmpLoader->LoadBitmap(wxT("find_and_replace")));
        m_tabs.insert(std::make_pair(REPLACE_IN_FILES, Tab(REPLACE_IN_FILES, m_replaceResultsTab, bmpLoader->LoadBitmap(wxT("find_and_replace")))));
        mgr->AddOutputTab(REPLACE_IN_FILES);

        // Show Usage ("References")
        m_showUsageTab = new FindUsageTab(m_book, wxGetTranslation(SHOW_USAGE));
    #if PHP_BUILD
        m_showUsageTab->Hide();
    #else
        m_book->AddPage(m_showUsageTab, wxGetTranslation(SHOW_USAGE), false, bmpLoader->LoadBitmap(wxT("find")));
        m_tabs.insert(std::make_pair(wxGetTranslation(SHOW_USAGE), Tab(wxGetTranslation(SHOW_USAGE), m_showUsageTab,
                                                                       bmpLoader->LoadBitmap(wxT("find")))));
        mgr->AddOutputTab(wxGetTranslation(SHOW_USAGE));
    #endif

        // Tasks panel
        m_taskPanel = new TaskPanel(m_book, wxID_ANY, wxGetTranslation(TASKS));
        m_book->AddPage(m_taskPanel, wxGetTranslation(TASKS), false, bmpLoader->LoadBitmap("tasks"));
        m_tabs.insert(std::make_pair(wxGetTranslation(TASKS), Tab(wxGetTranslation(TASKS), m_taskPanel, bmpLoader->LoadBitmap("tasks"))));
        mgr->AddOutputTab(wxGetTranslation(TASKS));

        //SetMinSize(wxSize(200, 100));
        //mainSizer->Layout();
    }
    //-----------------------------------------------------------------------------
    void OutputPane::OnEditorFocus(wxCommandEvent& e)
    {
        e.Skip();
        if(gsgs_EditorConfig().GetOptions()->GetHideOutpuPaneOnUserClick()) 
        {
            // Optionally don't hide the various panes (sometimes it's irritating, you click to do something and...)
            int cursel(m_book->GetSelection());
            if(cursel != wxNOT_FOUND && gsgs_EditorConfig().GetPaneStickiness(m_book->GetPageText(cursel))) 
            { 
                return; 
            }

            if(m_buildInProgress) 
                return;
            wxAuiPaneInfo& info = PluginManager::Get()->GetAuiManager()->GetPane(m_caption);
            WindowManager::HackHidePane(true, info, PluginManager::Get()->GetAuiManager());
        }
    }
    //-----------------------------------------------------------------------------
    void OutputPane::OnBuildStarted(BuildEvent& e)
    {
        e.Skip();
        m_buildInProgress = true;
    }
    //-----------------------------------------------------------------------------
    void OutputPane::OnBuildEnded(BuildEvent& e)
    {
        e.Skip();
        m_buildInProgress = false;
    }
    //-----------------------------------------------------------------------------
    void OutputPane::SaveTabOrder()
    {
    #if USE_AUI_NOTEBOOK
        wxArrayString panes = m_book->GetAllTabsLabels();
    #else
        wxArrayString panes;
        TabInfoVec_t tabs;
        m_book->GetAllTabs(tabs);
        std::for_each(tabs.begin(), tabs.end(), [&](TabInfoPtr_t t) { panes.Add(t->GetLabel()); });
    #endif
        gsgs_Config().SetOutputTabOrder(panes, m_book->GetSelection(), false);
    }
    //-----------------------------------------------------------------------------
    typedef struct 
    {
        wxString text;
        wxWindow* win;
        wxBitmap bmp;
    } tagTabInfo;
    //-----------------------------------------------------------------------------
    void OutputPane::ApplySavedTabOrder() const
    {
        wxArrayString tabs;
        int index = -1;
        if(!gsgs_Config().GetOutputTabOrder(tabs, index)) 
            return;

        std::vector<tagTabInfo> vTempstore;
        for(size_t t = 0; t < tabs.GetCount(); ++t) 
        {
            wxString title = tabs.Item(t);
            if(title.empty()) { continue; }
            for(size_t n = 0; n < m_book->GetPageCount(); ++n) 
            {
                if(title == m_book->GetPageText(n)) 
                {
                    tagTabInfo Tab;
                    Tab.text = title;
                    Tab.win = m_book->GetPage(n);
                    Tab.bmp = m_book->GetPageBitmap(n);

                    vTempstore.push_back(Tab);
                    m_book->RemovePage(n);
                    break;
                }
            }
            // If we reach here without finding title, presumably that tab is no longer available and will just be ignored
        }

        // All the matched tabs are now stored in the vector. Any left in m_book are presumably new additions
        // Now prepend the ordered tabs, so that any additions will effectively be appended
        for(size_t n = 0; n < vTempstore.size(); ++n) 
        {
            m_book->InsertPage(n, vTempstore.at(n).win, vTempstore.at(n).text, false, vTempstore.at(n).bmp);
        }

        // wxPrintf("After load");for (size_t n=0; n < m_book->GetPageCount(); ++n)  CL_DEBUG1(wxString::Format("Tab %i:
        // %zs",(int)n,m_book->GetPageText(n)));

        // Restore any saved last selection
        // NB: this doesn't actually work atm: the selection is set correctly, but presumably something else changes is
        // later
        // I've left the code in case anyone ever has time/inclination to fix it
        if((index >= 0) && (index < (int)m_book->GetPageCount())) 
        {
            m_book->SetSelection(index);
        } 
        else if(m_book->GetPageCount()) 
        {
            m_book->SetSelection(0);
        }
    }
    //-----------------------------------------------------------------------------
    void OutputPane::OnSettingsChanged(wxCommandEvent& event)
    {
        event.Skip();
        long style = 0;
        wxDirection tempdir = gsgs_EditorConfig().GetOptions()->GetOutputTabsDirection();
        if (tempdir == wxBOTTOM)
            style |= kNotebook_BottomTabs;
        else if (tempdir == wxRIGHT)
            style |= kNotebook_RightTabs;
        else if (tempdir == wxLEFT)
            style |= kNotebook_LeftTabs;
        //m_book->SetTabDirection(gsgs_EditorConfig().GetOptions()->GetOutputTabsDirection());
    #if !USE_AUI_NOTEBOOK
        if(gsgs_EditorConfig().GetOptions()->IsTabColourDark()) 
        {
            m_book->SetStyle((m_book->GetStyle() & ~kNotebook_LightTabs) | kNotebook_DarkTabs | style);
        } 
        else 
        {
            m_book->SetStyle((m_book->GetStyle() & ~kNotebook_DarkTabs) | kNotebook_LightTabs | style);
        }
    #endif
    }
    //-----------------------------------------------------------------------------
    void OutputPane::OnToggleTab(Event& event)
    {
        // Handle the core tabs
        if(m_tabs.count(event.GetString()) == 0) {
            event.Skip();
            return;
        }

        const Tab& t = m_tabs.find(event.GetString())->second;
        if(event.IsSelected()) {
            // Insert the page
            int where = TabTogglerHelper::IsTabInNotebook(GetNotebook(), t.m_label);
            if(where == wxNOT_FOUND) {
                GetNotebook()->AddPage(t.m_window, t.m_label, true, t.m_bmp);
            } else {
                GetNotebook()->SetSelection(where);
            }
        } else {
            // hide the tab
            int where = GetNotebook()->GetPageIndex(t.m_label);
            if(where != wxNOT_FOUND) { GetNotebook()->RemovePage(where); }
        }
    }
    //-----------------------------------------------------------------------------
    void OutputPane::OnOutputBookFileListMenu(ContextMenuEvent& event)
    {
        wxMenu* menu = event.GetMenu();
        menu->AppendSeparator();

        DetachedPanesInfo dpi;
        gsgs_EditorConfig().ReadObject("DetachedPanesList", &dpi);

        wxMenu* hiddenTabsMenu = new wxMenu();
        const wxArrayString& tabs = GetManager()->GetOutputTabs();
        for(size_t i = 0; i < tabs.size(); ++i) 
        {
            const wxString& label = tabs.Item(i);
            if((m_book->GetPageIndex(label) != wxNOT_FOUND)) 
            {
                continue;
            }
            
            if(hiddenTabsMenu->GetMenuItemCount() == 0) 
            {
                // we are adding the first menu item
                menu->AppendSeparator();
            }
            
            int tabId = wxXmlResource::GetXRCID(wxString() << "output_tab_" << label);
            wxMenuItem* item = new wxMenuItem(hiddenTabsMenu, tabId, label);
            hiddenTabsMenu->Append(item);

            // Output pane does not support "detach"
            if(dpi.GetPanes().Index(label) != wxNOT_FOUND) { item->Enable(false); }

            hiddenTabsMenu->Bind(wxEVT_MENU,
                                 // Use lambda by value here so we make a copy
                                 [=](wxCommandEvent& e) {
                                     Event eventShow(wxEVT_SHOW_OUTPUT_TAB);
                                     eventShow.SetSelected(true).SetString(label);
                                     gsgs_Event().AddPendingEvent(eventShow);
                                 },
                                 tabId);
        }

        if(hiddenTabsMenu->GetMenuItemCount() == 0) {
            wxDELETE(hiddenTabsMenu);
        } else {
            menu->AppendSubMenu(hiddenTabsMenu, _("Hidden Tabs"), _("Hidden Tabs"));
        }
    }
    //-----------------------------------------------------------------------------
}