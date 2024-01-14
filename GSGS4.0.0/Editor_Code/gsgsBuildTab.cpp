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

#include "gsgsStyleManager.h"
#include "gsgsNotebook.h"
#include "gsgsLexerStyle.h"
#include "gsgsImageManager.h"
#include "gsgsBuildConfig.h"
#include "gsgsCommonDialog.h"
#include "gsgsEditorConfig.h"
#include "gsgsLogManager.h"
#include "gsgsFrame.h"
#include "gsgsToolBar.h"
#include "gsgsBuildTab.h"
#include "gsgsOptionsConfig.h"
#include "gsgsOutputPane.h"
#include "gsgsPluginManager.h"
#include "gsgsShellCmd.h"
#include "gsgsWorkspace.h"
#include "gsgsCreateWorkspace.h"
#include "CommonController1.h"
#include "gsgsConfig.h"
#include "gsgsBook.h"
#include "globals.h"
#include "gsgsManager.h"
#include <imanager.h>

#include <wx/choicdlg.h>
#include <wx/dataview.h>
#include <wx/dcmemory.h>
#include <wx/fdrepdlg.h>
#include <wx/regex.h>
#include <wx/settings.h>
//-----------------------------------------------------------------------------
#define IS_VALID_LINE(lineNumber) ((lineNumber >= 0 && lineNumber < m_view->GetLineCount()))
#ifdef __WXMSW__
#define IS_WINDOWS true
#else
#define IS_WINDOWS false
#endif
//-----------------------------------------------------------------------------
// Helper function to post an event to the frame
/*void SetActive(Editor * editor)
{
    wxCHECK_RET(editor, wxT("Null editor parameter"));

    wxCommandEvent event(wxEVT_ACTIVATE_EDITOR);
    event.SetEventObject(editor);
    wxPostEvent(gsgs_Frame(), event);
}*/
///////////////////////////////////////////////////////////////////////////////
namespace gsgs
{
    class BuildTabTopPanel : public wxPanel
    {
    public:
        BuildTabTopPanel(wxWindow* parent);
        virtual ~BuildTabTopPanel();

    protected:
        virtual void OnAutoScrollUI(wxUpdateUIEvent& event);
        virtual void OnAutoScroll(wxCommandEvent& event);
        virtual void OnPaste(wxCommandEvent& event);
        virtual void OnPasteUI(wxUpdateUIEvent& event);
        virtual void OnCopyBuildOutput(wxCommandEvent& event);
        virtual void OnCopyBuildOutputUI(wxUpdateUIEvent& event);
        virtual void OnClearBuildOutput(wxCommandEvent& event);
        virtual void OnClearBuildOutputUI(wxUpdateUIEvent& event);
        virtual void OnSaveBuildOutput(wxCommandEvent& event);
        virtual void OnSaveBuildOutputUI(wxUpdateUIEvent& event);
        virtual void OnToolPinCommandToolClicked(wxCommandEvent& event);
    private:
        NewBuildTab* m_buildTab;
        ToolBar* m_toolbar;
    };
    //-----------------------------------------------------------------------------
    BuildTabTopPanel::BuildTabTopPanel(wxWindow* parent)
        : wxPanel(parent)
    {
        m_buildTab = dynamic_cast<NewBuildTab*>(parent);
        ImageManager* bmps = &gsgs_Image();
        SetSizer(new wxBoxSizer(wxVERTICAL));
        m_toolbar = new ToolBar(this, wxID_ANY);
        m_toolbar->AddTool(XRCID("build_tool_pin"), _("Pin build view"), bmps->LoadBitmap("ToolPin"), _("Pin build view"),
                           wxITEM_CHECK);
        m_toolbar->AddTool(XRCID("build_tool_scroll_view"), _("Scroll to last line"), bmps->LoadBitmap("link_editor"),
                           _("Scroll to last line"), wxITEM_CHECK);
        m_toolbar->AddSeparator();
        m_toolbar->AddTool(XRCID("build_tool_clear"), _("Clear view"), bmps->LoadBitmap("clear"), _("Clear view"));
        m_toolbar->AddTool(XRCID("build_tool_save"), _("Save build log"), bmps->LoadBitmap("file_save"),
                           _("Save build log"));
        m_toolbar->AddTool(XRCID("build_tool_copy"), _("Copy build log"), bmps->LoadBitmap("copy"), _("Copy build log"));
        m_toolbar->AddTool(XRCID("build_tool_paste"), _("Paste build log"), bmps->LoadBitmap("paste"),
                           _("Paste build log"));
        m_toolbar->Realize();
        GetSizer()->Add(m_toolbar, 0, wxEXPAND);

        m_toolbar->Bind(wxEVT_MENU, &BuildTabTopPanel::OnToolPinCommandToolClicked, this, XRCID("build_tool_pin"));
        m_toolbar->Bind(wxEVT_MENU, &BuildTabTopPanel::OnAutoScroll, this, XRCID("build_tool_scroll_view"));
        m_toolbar->Bind(wxEVT_MENU, &BuildTabTopPanel::OnClearBuildOutput, this, XRCID("build_tool_clear"));
        m_toolbar->Bind(wxEVT_MENU, &BuildTabTopPanel::OnSaveBuildOutput, this, XRCID("build_tool_save"));
        m_toolbar->Bind(wxEVT_MENU, &BuildTabTopPanel::OnCopyBuildOutput, this, XRCID("build_tool_copy"));
        m_toolbar->Bind(wxEVT_MENU, &BuildTabTopPanel::OnPaste, this, XRCID("build_tool_paste"));

        m_toolbar->Bind(wxEVT_UPDATE_UI, &BuildTabTopPanel::OnAutoScrollUI, this, XRCID("build_tool_scroll_view"));
        m_toolbar->Bind(wxEVT_UPDATE_UI, &BuildTabTopPanel::OnClearBuildOutputUI, this, XRCID("build_tool_clear"));
        m_toolbar->Bind(wxEVT_UPDATE_UI, &BuildTabTopPanel::OnSaveBuildOutputUI, this, XRCID("build_tool_save"));
        m_toolbar->Bind(wxEVT_UPDATE_UI, &BuildTabTopPanel::OnCopyBuildOutputUI, this, XRCID("build_tool_copy"));
        m_toolbar->Bind(wxEVT_UPDATE_UI, &BuildTabTopPanel::OnPasteUI, this, XRCID("build_tool_paste"));
    }
    //-----------------------------------------------------------------------------
    BuildTabTopPanel::~BuildTabTopPanel() {}
    //-----------------------------------------------------------------------------
    void BuildTabTopPanel::OnToolPinCommandToolClicked(wxCommandEvent& event)
    {
        gsgs_EditorConfig().SetPaneStickiness("Build", event.IsChecked());
    }
    //-----------------------------------------------------------------------------
    void BuildTabTopPanel::OnClearBuildOutput(wxCommandEvent& WXUNUSED(event))
    {
        if(m_buildTab) { m_buildTab->Clear(); }
    }
    //-----------------------------------------------------------------------------
    void BuildTabTopPanel::OnClearBuildOutputUI(wxUpdateUIEvent& event)
    {
        if(!m_buildTab) {
            event.Enable(false);

        } else {
            event.Enable(!m_buildTab->IsEmpty() && !m_buildTab->IsBuildInProgress());
        }
    }
    //-----------------------------------------------------------------------------
    void BuildTabTopPanel::OnSaveBuildOutput(wxCommandEvent& WXUNUSED(event))
    {
        wxString filename = ::wxFileSelector(_("Select a file"), wxEmptyString, wxT("BuildLog.txt"), wxEmptyString,
                                             wxFileSelectorDefaultWildcardStr, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if(filename.IsEmpty()) return;

        wxFileName fn(filename);
        wxFFile fp(fn.GetFullPath(), wxT("w+b"));
        if(fp.IsOpened()) {
            fp.Write(m_buildTab->GetBuildContent(), wxConvUTF8);
            fp.Close();

            ::wxMessageBox(_("Saved build log to file:\n") + fn.GetFullPath());
        }
    }
    //-----------------------------------------------------------------------------
    void BuildTabTopPanel::OnSaveBuildOutputUI(wxUpdateUIEvent& event)
    {
        if(!m_buildTab) {
            event.Enable(false);

        } else {
            event.Enable(!m_buildTab->IsEmpty() && !m_buildTab->IsBuildInProgress());
        }
    }
    //-----------------------------------------------------------------------------
    void BuildTabTopPanel::OnCopyBuildOutput(wxCommandEvent& event)
    {
        wxString content = m_buildTab->GetBuildContent();
        ::CopyToClipboard(content);
    }
    //-----------------------------------------------------------------------------
    void BuildTabTopPanel::OnCopyBuildOutputUI(wxUpdateUIEvent& event)
    {
        if(!m_buildTab) {
            event.Enable(false);

        } else {
            event.Enable(!m_buildTab->IsEmpty());
        }
    }
    //-----------------------------------------------------------------------------
    void BuildTabTopPanel::OnPaste(wxCommandEvent& event)
    {
        wxString content = m_buildTab->GetBuildContent();
        Editor* editor = gsgs_Frame()->GetMainBook()->NewEditor();
        if(editor) { editor->SetText(content); }
    }
    //-----------------------------------------------------------------------------
    void BuildTabTopPanel::OnPasteUI(wxUpdateUIEvent& event)
    {
        if(!m_buildTab) {
            event.Enable(false);

        } else {
            event.Enable(!m_buildTab->IsEmpty());
        }
    }
    //-----------------------------------------------------------------------------
    void BuildTabTopPanel::OnAutoScroll(wxCommandEvent& event)
    {
        gsgs_Config().Write(kConfigBuildAutoScroll, event.IsChecked());
        if(event.IsChecked()) { m_buildTab->ScrollToBottom(); }
    }
    //-----------------------------------------------------------------------------
    void BuildTabTopPanel::OnAutoScrollUI(wxUpdateUIEvent& event)
    {
        bool temp;
        gsgs_Config().Read(kConfigBuildAutoScroll, temp, true);
        event.Check(temp);
    }
    //-----------------------------------------------------------------------------
    struct AnnotationInfo {
        int line;
        LINE_SEVERITY severity;
        std::set<wxString> text;
    };
    typedef std::map<int, AnnotationInfo> AnnotationInfoByLineMap_t;
    //-----------------------------------------------------------------------------
    #define LEX_GCC_DEFAULT 0
    #define LEX_GCC_ERROR 1
    #define LEX_GCC_WARNING 2
    #define LEX_GCC_INFO 3

    #define LEX_GCC_MARKER 1
    //-----------------------------------------------------------------------------
    NewBuildTab::NewBuildTab(wxWindow* parent)
        : wxPanel(parent)
        , m_warnCount(0)
        , m_errorCount(0)
        , m_buildInterrupted(false)
        , m_autoHide(false)
        , m_showMe(BuildTabSettingsData::ShowOnStart)
        , m_skipWarnings(false)
        , m_buildpaneScrollTo(ScrollToFirstError)
        , m_buildInProgress(false)
        , m_maxlineWidth(wxNOT_FOUND)
        , m_lastLineColoured(wxNOT_FOUND)
    {
        SetSize(wxNOT_FOUND, 400);
        m_curError = m_errorsAndWarningsList.end();
        wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
        SetSizer(bs);

        m_view = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
        // We dont really want to collect undo in the output tabs...
        InitView();
        Bind(wxEVT_IDLE, &NewBuildTab::OnIdle, this);

        m_view->Bind(wxEVT_STC_HOTSPOT_CLICK, &NewBuildTab::OnHotspotClicked, this);
        gsgs_Event().Bind(wxEVT_GSGS_THEME_CHANGED, &NewBuildTab::OnTheme, this);

        bs->Add(m_view, 1, wxEXPAND | wxALL);

        BuildTabTopPanel* toolbox = new BuildTabTopPanel(this);
        toolbox->Hide();
        bs->Insert(0, toolbox, 0, wxEXPAND);

        gsgs_Event().Connect(wxEVT_SHELL_COMMAND_STARTED, gsgsEventHandler(NewBuildTab::OnBuildStarted), NULL,
                                      this);
        gsgs_Event().Connect(wxEVT_SHELL_COMMAND_STARTED_NOCLEAN,
                                      gsgsEventHandler(NewBuildTab::OnBuildStarted), NULL, this);
        gsgs_Event().Connect(wxEVT_SHELL_COMMAND_ADDLINE, gsgsEventHandler(NewBuildTab::OnBuildAddLine), NULL,
                                      this);
        gsgs_Event().Connect(wxEVT_SHELL_COMMAND_PROCESS_ENDED, gsgsEventHandler(NewBuildTab::OnBuildEnded),
                                      NULL, this);

        gsgs_Event().Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(NewBuildTab::OnWorkspaceLoaded), NULL,
                                      this);
        gsgs_Event().Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(NewBuildTab::OnWorkspaceClosed), NULL,
                                      this);

        wxTheApp->Connect(XRCID("next_build_error"), wxEVT_COMMAND_MENU_SELECTED,
                          wxCommandEventHandler(NewBuildTab::OnNextBuildError), NULL, this);
        wxTheApp->Connect(XRCID("next_build_error"), wxEVT_UPDATE_UI,
                          wxUpdateUIEventHandler(NewBuildTab::OnNextBuildErrorUI), NULL, this);
    }
    //-----------------------------------------------------------------------------
    NewBuildTab::~NewBuildTab()
    {
        gsgs_Event().Unbind(wxEVT_GSGS_THEME_CHANGED, &NewBuildTab::OnTheme, this);
        gsgs_Event().Disconnect(wxEVT_SHELL_COMMAND_STARTED, gsgsEventHandler(NewBuildTab::OnBuildStarted),
                                         NULL, this);
        gsgs_Event().Disconnect(wxEVT_SHELL_COMMAND_STARTED_NOCLEAN,
                                         gsgsEventHandler(NewBuildTab::OnBuildStarted), NULL, this);
        gsgs_Event().Disconnect(wxEVT_SHELL_COMMAND_ADDLINE, gsgsEventHandler(NewBuildTab::OnBuildAddLine),
                                         NULL, this);
        gsgs_Event().Disconnect(wxEVT_SHELL_COMMAND_PROCESS_ENDED,
                                         gsgsEventHandler(NewBuildTab::OnBuildEnded), NULL, this);
        wxTheApp->Disconnect(XRCID("next_build_error"), wxEVT_COMMAND_MENU_SELECTED,
                             wxCommandEventHandler(NewBuildTab::OnNextBuildError), NULL, this);
        wxTheApp->Disconnect(XRCID("next_build_error"), wxEVT_UPDATE_UI,
                             wxUpdateUIEventHandler(NewBuildTab::OnNextBuildErrorUI), NULL, this);
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::OnBuildEnded(Event& e)
    {
        e.Skip();
        gsgs_Debug("Build Ended!");
        m_buildInProgress = false;

        DoProcessOutput(true, false);

        std::vector<Editor*> editors;
        gsgs_Frame()->GetMainBook()->GetAllEditors(editors, Book::kGetAll_Default);
        for(size_t i = 0; i < editors.size(); i++) {
            MarkEditor(editors.at(i));
        }

        // Add a summary line
        wxString problemcount =
            wxString::Format(wxT("%d %s, %d %s"), m_errorCount, _("errors"), m_warnCount, _("warnings"));
        wxString term = problemcount;
        long elapsed = m_sw.Time() / 1000;
        if(elapsed > 10) {
            long sec = elapsed % 60;
            long hours = elapsed / 3600;
            long minutes = (elapsed % 3600) / 60;
            term << wxString::Format(wxT(", %s: %02ld:%02ld:%02ld %s"), _("total time"), hours, minutes, sec, _("seconds"));
        }

        m_output = term;
        DoProcessOutput(true, true);

        if(m_buildInterrupted) {
            wxString InterruptedMsg;
            InterruptedMsg << _("(Build Cancelled)") << wxT("\n\n");
            m_output = InterruptedMsg;
            DoProcessOutput(true, false);
        }

        // Hide / Show the build tab according to the settings
        DoToggleWindow();

        // make it invalid
        m_curError = m_errorsAndWarningsList.begin();
        gsgs_Debug("Posting wxEVT_BUILD_ENDED event");

        // 0 = first error
        // 1 = first error or warning
        // 2 = to the end
        if(m_buildTabSettings.GetBuildPaneScrollDestination() == ScrollToFirstError && !m_errorsList.empty()) {
            BuildLineInfo* bli = m_errorsList.front();
            DoSelectAndOpen(bli->GetLineInBuildTab(), true);
        }

        if(m_buildTabSettings.GetBuildPaneScrollDestination() == ScrollToFirstItem && !m_errorsAndWarningsList.empty()) {
            BuildLineInfo* bli = m_errorsAndWarningsList.front();
            DoSelectAndOpen(bli->GetLineInBuildTab(), true);
        }

        if(m_buildTabSettings.GetBuildPaneScrollDestination() == ScrollToEnd) { m_view->ScrollToEnd(); }

        // notify the plugins that the build has ended
        BuildEvent buildEvent(wxEVT_BUILD_ENDED);
        buildEvent.SetErrorCount(m_errorCount);
        buildEvent.SetWarningCount(m_warnCount);
        gsgs_Event().AddPendingEvent(buildEvent);
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::OnBuildStarted(Event& e)
    {
        e.Skip();

        if(IS_WINDOWS) {
            m_cygwinRoot.Clear();
            EnvSetter es;
            wxString cmd;
            cmd << "cygpath -w /";
            wxArrayString arrOut;
            ProcUtils::SafeExecuteCommand(cmd, arrOut);

            if(arrOut.IsEmpty() == false) { m_cygwinRoot = arrOut.Item(0); }
        }

        m_buildInProgress = true;

        // Reload the build settings data
        gsgs_EditorConfig().ReadObject(wxT("build_tab_settings"), &m_buildTabSettings);
        InitView();

        m_autoHide = m_buildTabSettings.GetAutoHide();
        m_showMe = (BuildTabSettingsData::ShowBuildPane)m_buildTabSettings.GetShowBuildPane();
        m_skipWarnings = m_buildTabSettings.GetSkipWarnings();

        if(e.GetEventType() != wxEVT_SHELL_COMMAND_STARTED_NOCLEAN) 
        {
            DoClear();
            DoCacheRegexes();
        }

        // Show the tab if needed
        OutputPane* opane = gsgs_Frame()->GetOutputPane();

        wxWindow* win(NULL);
        int sel = opane->GetNotebook()->GetSelection();
        if(sel != wxNOT_FOUND) { win = opane->GetNotebook()->GetPage(sel); }

        if(m_showMe == BuildTabSettingsData::ShowOnStart) {
            gsgs_Manager()->ShowOutputPane(BUILD_WIN, true);

        } 
        else if(m_showMe == BuildTabSettingsData::ShowOnEnd && m_autoHide &&
            gsgs_Manager()->IsPaneVisible(opane->GetCaption()) && win == this) 
        {
            // user prefers to see build/errors tabs only at end of unsuccessful build
            gsgs_Manager()->HidePane(opane->GetName());
        }
        m_sw.Start();

        m_cmp.Reset(NULL);
        BuildEventDetails* bed = dynamic_cast<BuildEventDetails*>(e.GetClientObject());
        if(bed) 
        {
            BuildConfigPtr buildConfig = gsgs_CppWorker().GetProjBuildConf(bed->GetProjectName(), bed->GetConfiguration());
            if(buildConfig) 
            { 
                m_cmp = buildConfig->GetCompiler(); 
            }

            // notify the plugins that the build had started
            BuildEvent buildEvent(wxEVT_BUILD_STARTED);
            buildEvent.SetProjectName(bed->GetProjectName());
            buildEvent.SetConfigurationName(bed->GetConfiguration());
            gsgs_Event().AddPendingEvent(buildEvent);
        }
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::OnBuildAddLine(Event& e)
    {
        e.Skip(); // Always call skip..
        m_output << e.GetString();
        DoProcessOutput(false, false);
    }
    //-----------------------------------------------------------------------------
    BuildLineInfo* NewBuildTab::DoProcessLine(const wxString& line)
    {
        BuildLineInfo* buildLineInfo = new BuildLineInfo();
        LINE_SEVERITY severity;
        // Get the matching regex for this line
        CmpPatternPtr cmpPatterPtr = GetMatchingRegex(line, severity);
        buildLineInfo->SetSeverity(severity);
        BuildLineInfo bli;
        if(cmpPatterPtr && cmpPatterPtr->Matches(line, bli)) {
            buildLineInfo->SetFilename(bli.GetFilename());
            buildLineInfo->SetSeverity(bli.GetSeverity());
            buildLineInfo->SetLineNumber(bli.GetLineNumber());
            buildLineInfo->NormalizeFilename(m_directories, m_cygwinRoot);
            buildLineInfo->SetRegexLineMatch(bli.GetRegexLineMatch());
            buildLineInfo->SetColumn(bli.GetColumn());
            if(severity == SV_WARNING) {
                // Warning
                m_errorsAndWarningsList.push_back(buildLineInfo);
                m_warnCount++;
            } else {
                // Error
                m_errorsAndWarningsList.push_back(buildLineInfo);
                m_errorsList.push_back(buildLineInfo);
                m_errorCount++;
            }
        }
        return buildLineInfo;
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::DoCacheRegexes()
    {
        m_cmpPatterns.clear();

        // Loop over all known compilers and cache the regular expressions
        BuildSettingsConfigCookie cookie;
        CompilerPtr cmp = gsgs_Settings().GetFirstCompiler(cookie);
        while(cmp) {
            CmpPatterns cmpPatterns;
            const Compiler::CmpListInfoPattern& errPatterns = cmp->GetErrPatterns();
            const Compiler::CmpListInfoPattern& warnPatterns = cmp->GetWarnPatterns();
            Compiler::CmpListInfoPattern::const_iterator iter;
            for(iter = errPatterns.begin(); iter != errPatterns.end(); iter++) {

                CmpPatternPtr compiledPatternPtr(new CmpPattern(new wxRegEx(iter->pattern, wxRE_ADVANCED | wxRE_ICASE),
                                                                iter->fileNameIndex, iter->lineNumberIndex,
                                                                iter->columnIndex, SV_ERROR));
                if(compiledPatternPtr->GetRegex()->IsValid()) { cmpPatterns.errorsPatterns.push_back(compiledPatternPtr); }
            }

            for(iter = warnPatterns.begin(); iter != warnPatterns.end(); iter++) {

                CmpPatternPtr compiledPatternPtr(new CmpPattern(new wxRegEx(iter->pattern, wxRE_ADVANCED | wxRE_ICASE),
                                                                iter->fileNameIndex, iter->lineNumberIndex,
                                                                iter->columnIndex, SV_WARNING));
                if(compiledPatternPtr->GetRegex()->IsValid()) { cmpPatterns.warningPatterns.push_back(compiledPatternPtr); }
            }

            m_cmpPatterns.insert(std::make_pair(cmp->GetName(), cmpPatterns));
            cmp = gsgs_Settings().GetNextCompiler(cookie);
        }
    }
    //-----------------------------------------------------------------------------
    bool NewBuildTab::DoGetCompilerPatterns(const wxString& compilerName, CmpPatterns& patterns)
    {
        MapCmpPatterns_t::iterator iter = m_cmpPatterns.find(compilerName);
        if(iter == m_cmpPatterns.end()) { return false; }
        patterns = iter->second;
        return true;
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::DoClear()
    {
        wxFont font = DoGetFont();
        m_lastLineColoured = wxNOT_FOUND;
        m_maxlineWidth = wxNOT_FOUND;
        m_buildInterrupted = false;
        m_directories.Clear();
        m_buildInfoPerFile.clear();
        m_warnCount = 0;
        m_errorCount = 0;
        m_errorsAndWarningsList.clear();
        m_errorsList.clear();
        m_cmpPatterns.clear();

        // Delete all the user data
        std::for_each(m_viewData.begin(), m_viewData.end(), [&](std::pair<int, BuildLineInfo*> p) { delete p.second; });
        m_viewData.clear();

        m_view->SetEditable(true);
        m_view->ClearAll();
        m_view->SetEditable(false);

        // Clear all markers from open editors
        std::vector<Editor*> editors;
        gsgs_Frame()->GetMainBook()->GetAllEditors(editors, Book::kGetAll_Default);
        for(size_t i = 0; i < editors.size(); i++) 
        {
            editors.at(i)->DelAllCompilerMarkers();
            editors.at(i)->AnnotationClearAll();
        }
        m_curError = m_errorsAndWarningsList.end();
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::MarkEditor(Editor* editor)
    {
        if(!editor) return;

        editor->DelAllCompilerMarkers();
        editor->AnnotationClearAll();
        editor->AnnotationSetVisible(2); // Visible with box around it

        BuildTabSettingsData options;
        gsgs_EditorConfig().ReadObject(wxT("build_tab_settings"), &options);

        // Are markers or annotations enabled?
        if(options.GetErrorWarningStyle() == BuildTabSettingsData::EWS_NoMarkers) { return; }

        std::pair<MultimapBuildInfo_t::iterator, MultimapBuildInfo_t::iterator> iter =
            m_buildInfoPerFile.equal_range(editor->GetFileName().GetFullPath());
        if(iter.first == iter.second) {
            // could not find any, try the fullname
            iter = m_buildInfoPerFile.equal_range(editor->GetFileName().GetFullName());
    #if defined(__WXGTK__)
            if(iter.first == iter.second) {
                // Nope. Perhaps it's a symlink
                iter = m_buildInfoPerFile.equal_range(RealPath(editor->GetFileName().GetFullPath()));
            }
    #endif
        }

        // Merge all the errors from the same line into a single error
        AnnotationInfoByLineMap_t annotations;

        for(; iter.first != iter.second; ++iter.first) {
            BuildLineInfo* bli = iter.first->second;
            wxString text = m_view->GetLine(bli->GetLineInBuildTab()).Trim().Trim(false);

            // remove the line part from the text
            text = text.Mid(bli->GetRegexLineMatch());

            // if the line starts with ':' remove it as well
            text.StartsWith(":", &text);
            text.Trim().Trim(false);

            if(!text.IsEmpty()) {
                if(bli && (bli->GetSeverity() == SV_ERROR || bli->GetSeverity() == SV_WARNING)) {
                    const auto annotationIter = annotations.lower_bound(bli->GetLineNumber());
                    if ((annotationIter == annotations.end()) || (annotationIter->first != bli->GetLineNumber()))
                    {
                        // insert new one
                        AnnotationInfo info;
                        info.line = bli->GetLineNumber();
                        info.severity = bli->GetSeverity();
                        info.text.emplace(std::move(text));
                        annotations.emplace_hint(annotationIter, bli->GetLineNumber(), std::move(info));
                    }
                    else {
                        // we already have an error on this line, concatenate the message
                        AnnotationInfo& info = annotationIter->second;
                        info.text.emplace(std::move(text));

                        if(bli->GetSeverity() == SV_ERROR) {
                            // override the severity to ERROR
                            info.severity = SV_ERROR;
                        }
                    }
                }
            }
        }

        AnnotationInfoByLineMap_t::iterator annIter = annotations.begin();
        for(; annIter != annotations.end(); ++annIter) {
            wxString concatText;
            for (auto &text : annIter->second.text) {
                if (!concatText.IsEmpty())
                    concatText << "\n";
                concatText << text;
            }

            if(annIter->second.severity == SV_ERROR) {
                editor->SetErrorMarker(annIter->first, concatText);
            } else {
                editor->SetWarningMarker(annIter->first, concatText);
            }
        }
        // now place the errors
        editor->Refresh();
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::DoSearchForDirectory(const wxString& line)
    {
        // Check for makefile directory changes lines
        if(line.Contains(wxT("Entering directory `"))) {
            wxString currentDir = line.AfterFirst(wxT('`'));
            currentDir = currentDir.BeforeLast(wxT('\''));

            // Collect the m_baseDir
            m_directories.Add(currentDir);

        } else if(line.Contains(wxT("Entering directory '"))) {
            wxString currentDir = line.AfterFirst(wxT('\''));
            currentDir = currentDir.BeforeLast(wxT('\''));

            // Collect the m_baseDir
            m_directories.Add(currentDir);
        }
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::OnWorkspaceClosed(wxCommandEvent& e)
    {
        e.Skip();
        DoClear();
        InitView();
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::OnWorkspaceLoaded(wxCommandEvent& e)
    {
        e.Skip();
        DoClear();
        InitView();
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::DoProcessOutput(bool compilationEnded, bool isSummaryLine)
    {
        wxUnusedVar(isSummaryLine);
        if(!compilationEnded && m_output.Find(wxT("\n")) == wxNOT_FOUND) {
            // still dont have a complete line
            return;
        }

        wxArrayString lines = ::wxStringTokenize(m_output, wxT("\n"), wxTOKEN_RET_DELIMS);
        m_output.Clear();

        // Process only completed lines (i.e. a line that ends with '\n')
        for(size_t i = 0; i < lines.GetCount(); ++i) {
            if(!compilationEnded && !lines.Item(i).EndsWith(wxT("\n"))) {
                m_output << lines.Item(i);
                return;
            }

            wxString buildLine = lines.Item(i); //.Trim().Trim(false);
            // If this is a line similar to 'Entering directory `'
            // add the path in the directories array
            DoSearchForDirectory(buildLine);
            BuildLineInfo* buildLineInfo = DoProcessLine(buildLine);

            // keep the line info
            if(buildLineInfo->GetFilename().IsEmpty() == false) {
                m_buildInfoPerFile.insert(std::make_pair(buildLineInfo->GetFilename(), buildLineInfo));
            }

            if(isSummaryLine) {
                buildLine.Trim();
                buildLine.Prepend("====");
                buildLine.Append("====");
                buildLineInfo->SetSeverity(SV_NONE);
            }

            // Keep the line number in the build tab
            buildLineInfo->SetLineInBuildTab(m_view->GetLineCount() - 1); // -1 because the view always has 1 extra "\n"
            // Store the line info *before* we add the text
            // it is needed in the OnStyle function
            m_viewData.insert(std::make_pair(buildLineInfo->GetLineInBuildTab(), buildLineInfo));

            m_view->SetEditable(true);
            buildLine.Trim();
            wxString modText;
            ::StripTerminalColouring(buildLine, modText);

            int curline = m_view->GetLineCount() - 1;
            m_view->AppendText(modText + "\n");

            // get the newly added line width
            int endPosition = m_view->GetLineEndPosition(curline); // get character position from begin
            int beginPosition = m_view->PositionFromLine(curline); // and end of line

            wxPoint beginPos = m_view->PointFromPosition(beginPosition);
            wxPoint endPos = m_view->PointFromPosition(endPosition);

            int curLen = (endPos.x - beginPos.x) + 10;
            m_maxlineWidth = wxMax(m_maxlineWidth, curLen);
            if(m_maxlineWidth > 0) 
            { 
                m_view->SetScrollWidth(m_maxlineWidth); 
            }
            m_view->SetEditable(false);
            bool temprr;
            gsgs_Config().Read(kConfigBuildAutoScroll, temprr, true);
            if(temprr) 
            { 
                m_view->ScrollToEnd(); 
            }
        }
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::CenterLineInView(int line)
    {
        if(line > m_view->GetLineCount()) return;
        int linesOnScreen = m_view->LinesOnScreen();
        // To place our line in the middle, the first visible line should be
        // the: line - (linesOnScreen / 2)
        int firstVisibleLine = line - (linesOnScreen / 2);
        if(firstVisibleLine < 0) { firstVisibleLine = 0; }
        m_view->EnsureVisible(firstVisibleLine);
        m_view->SetFirstVisibleLine(firstVisibleLine);
        m_view->ClearSelections();
        m_view->SetCurrentPos(m_view->PositionFromLine(line));
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::DoToggleWindow()
    {
        bool success = m_errorCount == 0 && (m_skipWarnings || m_warnCount == 0);
        bool viewing = gsgs_Manager()->IsPaneVisible(wxT("Output View")) &&
                       (gsgs_Frame()->GetOutputPane()->GetNotebook()->GetCurrentPage() == this);
        bool skipwarnings(false);

        if(!success) {
            if(viewing) {
                if(m_buildpaneScrollTo != ScrollToEnd) {
                    // The user may have opted to go to the first error, the first item, or /dev/null
                    skipwarnings = (m_errorCount > 0) && (m_buildpaneScrollTo == ScrollToFirstError);
                    BuildLineInfo* bli = NULL;
                    if(skipwarnings && !m_errorsList.empty()) {
                        bli = m_errorsList.front();

                    } else if(!m_errorsAndWarningsList.empty()) {
                        bli = m_errorsAndWarningsList.front();
                    }

                    // Sanity
                    if(bli) { CenterLineInView(bli->GetLineInBuildTab()); }
                }
            }

        } else if(m_autoHide && viewing && !m_buildInterrupted) {
            gsgs_Manager()->HidePane(gsgs_Frame()->GetOutputPane()->GetCaption());

        } else if(m_showMe == BuildTabSettingsData::ShowOnEnd && !m_autoHide) {
            gsgs_Manager()->ShowOutputPane(BUILD_WIN);
        }
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::OnNextBuildError(wxCommandEvent& e)
    {
        // if we are here, we have at least something in the list of errors
        if(m_errorsAndWarningsList.empty()) return;

        gsgs_EditorConfig().ReadObject(wxT("build_tab_settings"), &m_buildTabSettings);
        bool skipWarnings = m_buildTabSettings.GetSkipWarnings();

        if(m_curError == m_errorsAndWarningsList.end()) {

            m_curError = m_errorsAndWarningsList.begin();
            if(m_curError == m_errorsAndWarningsList.end())
                // nothing to point to
                return;
        }

        // m_curError is valid
        if(skipWarnings) {

            do {
                if((*m_curError)->GetSeverity() == SV_ERROR) {
                    // get the wxDataViewItem
                    int line = (*m_curError)->GetLineInBuildTab();
                    if(IS_VALID_LINE(line)) {
                        DoSelectAndOpen(line, true);
                        ++m_curError;
                        return;
                    }

                } else {
                    ++m_curError;
                }

            } while(m_curError != m_errorsAndWarningsList.end());

        } else {
            int line = (*m_curError)->GetLineInBuildTab();
            if(IS_VALID_LINE(line)) {
                DoSelectAndOpen(line, true);
                ++m_curError;
            }
        }
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::OnNextBuildErrorUI(wxUpdateUIEvent& e)
    {
        e.Enable(!m_errorsAndWarningsList.empty() && !m_buildInProgress);
    }
    //-----------------------------------------------------------------------------
    bool NewBuildTab::DoSelectAndOpen(int buildViewLine, bool centerLine)
    {
        if(!m_viewData.count(buildViewLine)) { return false; }

        BuildLineInfo* bli = m_viewData.find(buildViewLine)->second;
        if(bli) {
            wxFileName fn(bli->GetFilename());

            // Highlight the clicked line on the view
            m_view->MarkerDeleteAll(LEX_GCC_MARKER);
            m_view->MarkerAdd(bli->GetLineInBuildTab(), LEX_GCC_MARKER);

            if(!fn.IsAbsolute()) {
                std::set<wxString> files;
                std::vector<wxFileName> candidates;
                gsgs_Manager()->GetWorkspaceFiles(files);

                std::for_each(files.begin(), files.end(), [&](const wxString& filepath) {
                    wxFileName fnFilePath(filepath);
                    if(fnFilePath.GetFullName() == fn.GetFullName()) { candidates.push_back(fnFilePath); }
                });

                if(candidates.empty()) { return false; }

                if(candidates.size() == 1) {
                    fn = candidates.at(0);

                } else {
                    // prompt the user
                    wxArrayString fileArr;
                    WindowList filePanelArr;
                    for(size_t i = 0; i < candidates.size(); ++i) {
                        fileArr.Add(candidates.at(i).GetFullPath());
                    }
                    filePanelArr.resize(fileArr.size());
                    SingleChoiceDialog dlg(gsgs_Event().TopFrame());
                    dlg.initItem(fileArr, filePanelArr, 0);
                    dlg.SetLabel(_("Select a file to open"));
                    if(dlg.ShowModal() != wxID_OK) return false;

                    wxString selection = dlg.GetSelection();
                    if(selection.IsEmpty()) return false;

                    fn = wxFileName(selection);
                    // if we resolved it now, open the file there is no point in searching this file
                    // in m_buildInfoPerFile since the key on this map is kept as full name
                    Editor* editor = gsgs_Frame()->GetMainBook()->FindEditor(fn.GetFullPath());
                    if(!editor) {
                        editor = gsgs_Frame()->GetMainBook()->OpenFile(fn.GetFullPath(), wxT(""),
                                                                             bli->GetLineNumber(), wxNOT_FOUND, OF_AddJump);
                    }

                    if(editor) {
                        DoCentreErrorLine(bli, editor, centerLine);
                        return true;
                    }
                }
            }

            if(fn.IsAbsolute()) {

                // try to locate the editor first
                Editor* editor = gsgs_Frame()->GetMainBook()->FindEditor(fn.GetFullPath());
                if(!editor) {
                    // Open it
                    editor = gsgs_Frame()->GetMainBook()->OpenFile(bli->GetFilename(), wxT(""), bli->GetLineNumber(),
                                                                         wxNOT_FOUND, OF_AddJump);
                }

                if(editor) {
                    if(!editor->HasCompilerMarkers()) MarkEditor(editor);

                    int lineNumber = bli->GetLineNumber();
                    if(lineNumber > 0) { lineNumber--; }

                    DoCentreErrorLine(bli, editor, centerLine);
                    return true;
                }
            }
        }
        return false;
    }
    //-----------------------------------------------------------------------------
    wxString NewBuildTab::GetBuildContent() const { return m_view->GetText(); }
    //-----------------------------------------------------------------------------
    wxFont NewBuildTab::DoGetFont() const
    {
        wxFont font = wxNullFont;
        LexerStyle::Ptr_t lexerConf = gsgs_EditorConfig().GetLexer("C++");
        if(lexerConf) { font = lexerConf->GetFontForSyle(wxSTC_C_DEFAULT); }

        if(font.IsOk() == false) { font = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT); }

        // if the user selected a font in the settings, use it instead
        wxFont userFont = m_buildTabSettings.GetBuildFont();
        if(userFont.IsOk()) { font = userFont; }
        return font;
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::OnMenu(wxContextMenuEvent& e)
    {
        wxMenu menu;
        menu.Append(wxID_COPY, _("Copy Selected Line"));
        menu.Append(XRCID("copy_all"), _("Copy Entire Build Output To Clipboard"));
        menu.Append(wxID_PASTE, _("Open Build Output in an Empty Editor"));
        menu.AppendSeparator();
        menu.Append(wxID_CLEAR, _("Clear"));
        m_view->PopupMenu(&menu);
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::OnOpenInEditor(wxCommandEvent& e)
    {
        wxString content = this->GetBuildContent();
        Editor* editor = gsgs_Frame()->GetMainBook()->NewEditor();
        if(editor) { editor->SetText(content); }
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::OnOpenInEditorUI(wxUpdateUIEvent& e) { e.Enable(!m_buildInProgress && !m_view->IsEmpty()); }
    //-----------------------------------------------------------------------------
    void NewBuildTab::OnClear(wxCommandEvent& e) { Clear(); }
    //-----------------------------------------------------------------------------
    void NewBuildTab::OnClearUI(wxUpdateUIEvent& e) { e.Enable(!m_buildInProgress && !IsEmpty()); }
    //-----------------------------------------------------------------------------
    void NewBuildTab::ScrollToBottom() { m_view->ScrollToEnd(); }
    //-----------------------------------------------------------------------------
    void NewBuildTab::AppendLine(const wxString& text)
    {
        m_output << text;
        DoProcessOutput(false, false);
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::OnStyleNeeded(wxStyledTextEvent& event)
    {
        int startPos = m_view->GetEndStyled();
        int endPos = event.GetPosition();
        wxString text = m_view->GetTextRange(startPos, endPos);
    #if wxCHECK_VERSION(3, 1, 1) && !defined(__WXOSX__)
        // The scintilla syntax in e.g. wx3.1.1 changed
        m_view->StartStyling(startPos);
    #else
        m_view->StartStyling(startPos, 0x1f);
    #endif

        int curline = m_view->GetLineCount();
        curline -= 1; // The view always ends with a "\n", we don't count it as a line
        wxArrayString lines = ::wxStringTokenize(text, wxT("\n"), wxTOKEN_RET_DELIMS);

        // the last line that we coloured
        curline -= lines.size();

        for(size_t i = 0; i < lines.size(); ++i) {
            const wxString& strLine = lines.Item(i);
            if(m_viewData.count(curline)) {
                BuildLineInfo* b = m_viewData.find(curline)->second;
                switch(b->GetSeverity()) {
                case SV_WARNING:
                    m_view->SetStyling(strLine.length(), LEX_GCC_WARNING);
                    break;
                case SV_ERROR:
                    m_view->SetStyling(strLine.length(), LEX_GCC_ERROR);
                    break;
                case SV_SUCCESS:
                    m_view->SetStyling(strLine.length(), LEX_GCC_DEFAULT);
                    break;
                case SV_DIR_CHANGE:
                    m_view->SetStyling(strLine.length(), LEX_GCC_INFO);
                    break;
                case SV_NONE:
                default:
                    m_view->SetStyling(strLine.length(), LEX_GCC_DEFAULT);
                    break;
                }
            } else {
                m_view->SetStyling(strLine.length(), LEX_GCC_DEFAULT);
            }
            ++curline;
        }
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::InitView(const wxString& theme)
    {
        LexerStyle::Ptr_t lexText = gsgs_Style().GetLexer("text", theme);
        lexText->Apply(m_view);

        m_view->SetUndoCollection(false);
        m_view->EmptyUndoBuffer();
        m_view->HideSelection(true);
        m_view->SetEditable(false);

        StyleProperty::Map_t& props = lexText->GetLexerProperties();
        const StyleProperty& defaultStyle = lexText->GetProperty(0);

        // reset the styles
        m_view->SetLexer(wxSTC_LEX_CONTAINER);
        wxFont defaultFont = lexText->GetFontForSyle(0);
        for(size_t i = 0; i < wxSTC_STYLE_MAX; ++i) {
            m_view->StyleSetForeground(i, defaultStyle.GetFgColour());
            //m_view->StyleSetBackground(i, defaultStyle.GetBgColour());
            m_view->StyleSetBackground(i, gsgs_Style().GetAppBgColour());
            m_view->StyleSetFont(i, defaultFont);
        }
        
        bool is_dark = Colour::IsDark(props[0].GetBgColour());
        wxColour textColour = props[0].GetFgColour();
        wxColour infoMsgColour = textColour.ChangeLightness(is_dark ? 50 : 150);
        m_view->StyleSetForeground(LEX_GCC_INFO, infoMsgColour);
        m_view->StyleSetForeground(LEX_GCC_DEFAULT, textColour);
        m_view->StyleSetForeground(LEX_GCC_WARNING, this->m_buildTabSettings.GetWarnColour());
        m_view->StyleSetForeground(LEX_GCC_ERROR, this->m_buildTabSettings.GetErrorColour());

        m_view->StyleSetHotSpot(LEX_GCC_ERROR, true);
        m_view->StyleSetHotSpot(LEX_GCC_WARNING, true);
        m_view->SetHotspotActiveUnderline(false); // No underline

        m_view->MarkerDefine(LEX_GCC_MARKER, wxSTC_MARK_BACKGROUND);
        if(lexText->IsDark()) {
            m_view->MarkerSetBackground(LEX_GCC_MARKER, wxColour(defaultStyle.GetBgColour()).ChangeLightness(110));
        } else {
            m_view->MarkerSetBackground(LEX_GCC_MARKER, wxColour(defaultStyle.GetBgColour()).ChangeLightness(90));
        }

        // Hide all margins
        for(int i = 0; i <= wxSTC_MARGIN_RTEXT; ++i) {
            m_view->SetMarginWidth(i, 0);
        }

        // make the symbol margin 5 pixel width
        m_view->SetMarginType(0, wxSTC_MARGIN_SYMBOL);
        m_view->SetMarginWidth(0, 5);
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::OnHotspotClicked(wxStyledTextEvent& event)
    {
        long pos = event.GetPosition();
        int line = m_view->LineFromPosition(pos);
        DoSelectAndOpen(line, false);
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::OnTheme(wxCommandEvent& event)
    {
        event.Skip();
        InitView();
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::DoCentreErrorLine(BuildLineInfo* bli, Editor* editor, bool centerLine)
    {
        // We already got compiler markers set here, just goto the line
        gsgs_Frame()->GetMainBook()->SelectPage(editor);
        CHECK_PTR_RET(bli);

        // Convert the compiler column to scintilla's position
        if(bli->GetColumn() != wxNOT_FOUND) {
            editor->CenterLine(bli->GetLineNumber(), bli->GetColumn() - 1);
        } else {
            editor->CenterLine(bli->GetLineNumber());
        }

        if(centerLine) {
            // If the line in the build error tab is not visible, ensure it is
            int linesOnScreen = m_view->LinesOnScreen();

            // Our line is not visible
            int firstVisibleLine = bli->GetLineInBuildTab() - (linesOnScreen / 2);
            if(firstVisibleLine < 0) { firstVisibleLine = 0; }
            m_view->EnsureVisible(firstVisibleLine);
            m_view->SetFirstVisibleLine(firstVisibleLine);
        }
        //SetActive(editor);
        wxCHECK_RET(editor, wxT("Null editor parameter"));

        wxCommandEvent event(wxEVT_ACTIVATE_EDITOR);
        event.SetEventObject(editor);
        wxPostEvent(gsgs_Frame(), event);
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::ColourOutput()
    {
        // Loop over the lines and colour them
        int fromLine = (m_lastLineColoured == wxNOT_FOUND) ? 0 : m_lastLineColoured;
        int untilLine = (m_view->GetLineCount() - 1);

        if(fromLine == untilLine) { return; }

        for(int i = fromLine; (i < untilLine) && (untilLine >= fromLine); ++i) {
            int startPos = m_view->PositionFromLine(i);
            int lineEndPos = m_view->GetLineEndPosition(i);

    #if wxCHECK_VERSION(3, 1, 1) && !defined(__WXOSX__)
            // The scintilla syntax in e.g. wx3.1.1 changed
            m_view->StartStyling(startPos);
    #else
            m_view->StartStyling(startPos, 0x1f);
    #endif

            // Run the regexes
            wxString lineText = m_view->GetLine(i);
            LINE_SEVERITY severity;
            GetMatchingRegex(lineText, severity);
            switch(severity) {
            case SV_WARNING:
                m_view->SetStyling((lineEndPos - startPos), LEX_GCC_WARNING);
                break;
            case SV_ERROR:
                m_view->SetStyling((lineEndPos - startPos), LEX_GCC_ERROR);
                break;
            case SV_SUCCESS:
                m_view->SetStyling((lineEndPos - startPos), LEX_GCC_DEFAULT);
                break;
            case SV_DIR_CHANGE:
                m_view->SetStyling((lineEndPos - startPos), LEX_GCC_INFO);
                break;
            case SV_NONE:
            default:
                m_view->SetStyling((lineEndPos - startPos), LEX_GCC_DEFAULT);
                break;
            }
        }
        m_lastLineColoured = untilLine;
    }
    //-----------------------------------------------------------------------------
    CmpPatternPtr NewBuildTab::GetMatchingRegex(const wxString& lineText, LINE_SEVERITY& severity)
    {
        if(lineText.Lower().Contains("entering directory") || lineText.Lower().Contains("leaving directory")) {
            severity = SV_DIR_CHANGE;
            return NULL;

        } else if(lineText.StartsWith("====")) {
            severity = SV_NONE;
            return NULL;

        } else {

            // Find *warnings* first
            bool isWarning = false;

            if(!m_cmp) {
                severity = SV_NONE;
                return NULL;
            }

            CmpPatterns cmpPatterns;
            if(!DoGetCompilerPatterns(m_cmp->GetName(), cmpPatterns)) {
                severity = SV_NONE;
                return NULL;
            }

            // If it is not an error, maybe it's a warning
            for(size_t i = 0; i < cmpPatterns.warningPatterns.size(); i++) {
                CmpPatternPtr cmpPatterPtr = cmpPatterns.warningPatterns.at(i);
                BuildLineInfo bli;
                if(cmpPatterPtr->Matches(lineText, bli)) {
                    severity = SV_WARNING;
                    return cmpPatterPtr;
                }
            }
            if(!isWarning) {
                for(size_t i = 0; i < cmpPatterns.errorsPatterns.size(); i++) {
                    BuildLineInfo bli;
                    CmpPatternPtr cmpPatterPtr = cmpPatterns.errorsPatterns.at(i);
                    if(cmpPatterPtr->Matches(lineText, bli)) {
                        severity = SV_ERROR;
                        return cmpPatterPtr;
                    }
                }
            }
        }

        // Default
        severity = SV_NONE;
        return NULL;
    }
    //-----------------------------------------------------------------------------
    void NewBuildTab::OnIdle(wxIdleEvent& event)
    {
        if(m_view->IsEmpty()) { return; }
        ColourOutput();
    }
    //-----------------------------------------------------------------------------
    bool CmpPattern::Matches(const wxString& line, BuildLineInfo& lineInfo)
    {
        long fidx, lidx;
        if(!m_fileIndex.ToLong(&fidx) || !m_lineIndex.ToLong(&lidx)) return false;

        if(!m_regex || !m_regex->IsValid()) return false;
        if(!m_regex->Matches(line)) return false;

        long colIndex;
        if(!m_colIndex.ToLong(&colIndex)) return false;

        lineInfo.SetSeverity(m_severity);
        if(m_regex->GetMatchCount() > (size_t)fidx) { lineInfo.SetFilename(m_regex->GetMatch(line, fidx)); }

        // keep the match length
        lineInfo.SetRegexLineMatch(m_regex->GetMatch(line, 0).length());

        if(m_regex->GetMatchCount() > (size_t)lidx) {
            long lineNumber;
            wxString strLine = m_regex->GetMatch(line, lidx);
            strLine.ToLong(&lineNumber);
            lineInfo.SetLineNumber(--lineNumber);
        }

        if(m_regex->GetMatchCount() > (size_t)colIndex) {
            long column;
            wxString strCol = m_regex->GetMatch(line, colIndex);
            if(strCol.StartsWith(":")) { strCol.Remove(0, 1); }

            if(!strCol.IsEmpty() && strCol.ToLong(&column)) { lineInfo.SetColumn(column); }
        }
        return true;
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // BuildTabSettingsData
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    BuildTabSettingsData::BuildTabSettingsData()
        : m_skipWarnings(false)
        , m_boldErrFont(false)
        , m_boldWarnFont(false)
        , m_showBuildPane(0)
        , m_autoHide(false)
        , m_autoShow(false)
        , m_errorsFirstLine(false)
        , m_errorWarningStyle(EWS_Bookmarks)
        , m_buildpaneScrollTo(0)
    {
        wxColour errColour(wxT("RED"));
        wxColour wrnColour(wxT("#AA9B49"));

        m_errorColour = errColour.GetAsString(wxC2S_HTML_SYNTAX);
        m_warnColour = wrnColour.GetAsString(wxC2S_HTML_SYNTAX);

        m_warnColourBg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).GetAsString();
        m_errorColourBg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).GetAsString();
    }
    //-----------------------------------------------------------------------------
    BuildTabSettingsData::~BuildTabSettingsData() {}
    //-----------------------------------------------------------------------------
    void BuildTabSettingsData::Serialize(Serializer& arch)
    {
        arch.Write(wxT("m_errorColour"), m_errorColour);
        arch.Write(wxT("m_warnColour"), m_warnColour);
        arch.Write(wxT("m_errorColourBg"), m_errorColourBg);
        arch.Write(wxT("m_warnColourBg"), m_warnColourBg);
        arch.Write(wxT("m_skipWarnings"), m_skipWarnings);
        arch.Write(wxT("m_boldErrFont"), m_boldErrFont);
        arch.Write(wxT("m_boldWarnFont"), m_boldWarnFont);
        arch.Write(wxT("m_autoHide"), m_autoHide);
        arch.Write(wxT("m_autoShow"), m_autoShow);
        arch.Write(wxT("m_errorsFirstLine"), m_errorsFirstLine);
        arch.Write(wxT("m_showBuildPane"), m_showBuildPane);
        arch.Write(wxT("m_errorWarningStyle"), m_errorWarningStyle);
        arch.Write(wxT("m_buildpaneScrollTo"), m_buildpaneScrollTo);
        arch.Write("m_buildFont", m_buildFont);
    }
    //-----------------------------------------------------------------------------
    void BuildTabSettingsData::DeSerialize(Serializer& arch)
    {
        arch.Read(wxT("m_errorColour"), m_errorColour);
        arch.Read(wxT("m_warnColour"), m_warnColour);
        arch.Read(wxT("m_errorColourBg"), m_errorColourBg);
        arch.Read(wxT("m_warnColourBg"), m_warnColourBg);
        arch.Read(wxT("m_skipWarnings"), m_skipWarnings);
        arch.Read(wxT("m_boldErrFont"), m_boldErrFont);
        arch.Read(wxT("m_boldWarnFont"), m_boldWarnFont);
        arch.Read(wxT("m_autoHide"), m_autoHide);
        arch.Read(wxT("m_autoShow"), m_autoShow);
        arch.Read(wxT("m_errorsFirstLine"), m_errorsFirstLine);
        arch.Read(wxT("m_showBuildPane"), m_showBuildPane);
        arch.Read(wxT("m_errorWarningStyle"), m_errorWarningStyle);
        arch.Read(wxT("m_buildpaneScrollTo"), m_buildpaneScrollTo);
        arch.Read("m_buildFont", m_buildFont);
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // BuildLineInfo
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    void BuildLineInfo::NormalizeFilename(const wxArrayString& directories, const wxString& cygwinPath)
    {
        wxFileName fn(this->GetFilename());

        if(fn.IsAbsolute()) {
            SetFilename(fn.GetFullPath());
            return;

        } else if(fn.IsAbsolute(wxPATH_UNIX) && IS_WINDOWS && !cygwinPath.IsEmpty()) {

            wxFileName cygfile(fn);
            wxString path = cygwinPath + cygfile.GetFullPath();
            SetFilename(wxFileName(path).GetFullPath());
            return;
        }

        if(directories.IsEmpty()) {
            SetFilename(fn.GetFullName());
            return;
        }

        // we got a relative file name
        int dircount = directories.GetCount();
        for(int i = dircount - 1; i >= 0; --i) {
            wxFileName tmp = fn;
            if(tmp.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_LONG)) {
                // Windows sanity
                if(IS_WINDOWS && tmp.GetVolume().length() > 1) {
                    // Invalid file path
                    SetFilename("");
                    return;
                }

                if(tmp.FileExists() && tmp.MakeAbsolute(directories.Item(i))) {
                    SetFilename(tmp.GetFullPath());
                    return;
                }
            }
        }

        // One more try: the above will fail if the project isn't stored in the cwd that Normalize() assumes
        // So see if one of 'directories' is the correct path to use
        for(int i = dircount - 1; i >= 0; --i) {
            wxFileName tmp = fn;
            if(tmp.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_LONG, directories.Item(i))) {
                // Windows sanity
                if(IS_WINDOWS && tmp.GetVolume().length() > 1) {
                    // Invalid file path
                    SetFilename("");
                    return;
                }

                if(tmp.FileExists()) {
                    SetFilename(tmp.GetFullPath());
                    return;
                }
            }
        }

        // failed.. keep it as fullname only
        SetFilename(fn.GetFullName());
    }
    //-----------------------------------------------------------------------------
    void BuildLineInfo::SetFilename(const wxString& filename)
    {
    #if defined(__WXGTK__)
        // RealPath copes with any symlinks in the filepath
        m_filename = RealPath(filename);
    #else
        this->m_filename = filename;
    #endif
    }
    //-----------------------------------------------------------------------------
}