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
#ifndef GSGS_FRAME_H
#define GSGS_FRAME_H

#include "gsgsPreInclude.h"
#include "NewProjectDialog.h"
#include "gsgsThread.h"
#include "gsgsWindowManager.h"
#include "gsgsCommonHandler.h"
#include "gsgsMenuBar.h"
#include "gsgsStatusBar.h"
#include "gsgsWorkBench.h"
#include "gsgsEditor.h"
#include "gsgsProcess.h"
#include "gsgsParseWorker.h"
#include "gsgsRefactor.h"
#include "plugin.h"
#include "wx/aui/aui.h"
#include "wx/combobox.h"
#include "wx/timer.h"
#include <wx/cmndata.h>
#include <wx/dcbuffer.h>
#include <wx/html/htmlwin.h>
#include <wx/splash.h>
#include <wx/minifram.h>
#include <wx/infobar.h>

class GsGsApp;

#ifdef __WXMSW__
#ifdef NDEBUG
#define SINGLE_INSTANCE_PORT 33617
#else
#define SINGLE_INSTANCE_PORT 36617
#endif
#else 
#include <unistd.h>
#ifdef NDEBUG
#define SINGLE_INSTANCE_PORT ((getuid() % 57) + 13617)
#else
#define SINGLE_INSTANCE_PORT ((getuid() % 57) + 16617)
#endif
#endif

namespace gsgs
{
    class WebUpdateJob;
    class SInstanceThread;
    class WorkspacePane;
    class MenuBar;
    class GeneralInfo;
    class Frame;
    enum FrameMark {
        GSGS_MAXIMIZE_FRAME = 0x00000001,
        GSGS_SHOW_WELCOME_PAGE = 0x00000004,
        GSGS_USE_EOL_LF = 0x00000008,
        GSGS_USE_EOL_CR = 0x00000010,
        GSGS_USE_EOL_CRLF = 0x00000020,
        GSGS_SHOW_EOL = 0x00000040,
        GSGS_SHOW_SPLASH = 0x00000080
    };
    
    wxDECLARE_EVENT(wxEVT_CMD_NEW_VERSION_AVAILABLE, wxCommandEvent);
    wxDECLARE_EVENT(wxEVT_CMD_VERSION_UPTODATE, wxCommandEvent);
    wxDECLARE_EVENT(wxEVT_CMD_VERSION_CHECK_ERROR, wxCommandEvent);

    wxDECLARE_EVENT(wxEVT_CMD_SINGLE_INSTANCE_THREAD_OPEN_FILES, Event);
    wxDECLARE_EVENT(wxEVT_CMD_SINGLE_INSTANCE_THREAD_RAISE_APP, Event);
    
    class WebUpdateJobData
    {
    public:
        WebUpdateJobData(const wxString & url, const wxString & notes, wxString curVersion, wxString newVersion,
                         bool upToDate, bool showMessage)
            : m_url(url.c_str())
            , m_curVersion(curVersion)
            , m_newVersion(newVersion)
            , m_upToDate(upToDate)
            , m_releaseNotes(notes)
            , m_showMessage(showMessage)
        {
        }

        ~WebUpdateJobData() {}

        const wxString& GetCurVersion() const       { return m_curVersion; }
        const wxString& GetNewVersion() const       { return m_newVersion; }
        const wxString& GetUrl() const              { return m_url; }

        bool IsUpToDate() const                     { return m_upToDate; }

        void SetReleaseNotes(const wxString& notes) { this->m_releaseNotes = notes; }
        const wxString& GetReleaseNotes() const     { return m_releaseNotes; }
        bool GetShowMessage() const                 { return m_showMessage; }
    private:
        wxString m_url;
        wxString m_curVersion;
        wxString m_newVersion;
        wxString m_releaseNotes;
        bool m_upToDate;
        bool m_showMessage;
    };

    class MainFrameHelper : public wxEvtHandler
    {
    public:
        typedef SharedPtr<MainFrameHelper> Ptr_t;

    public:
        MainFrameHelper(Frame* frame, WindowManager* mag);
        ~MainFrameHelper();

        void SetDebuggerFeatures(size_t debuggerFeatures) {  m_debuggerFeatures = debuggerFeatures; }
        
        size_t GetDebuggerFeatures() const {  return m_debuggerFeatures; }
        
        /**
         * @brief is the toolbar visible?
         */
        bool IsToolbarShown() const;

        /**
         * @brief return true if the various docking windows captions are visible
         */
        bool IsCaptionsVisible() const;

        /**
         * @brief return true if the status bar is visible
         */
        bool IsStatusBarVisible() const;

    private:
        Frame* m_mainFrame;
        WindowManager* m_mgr;
        size_t m_debuggerFeatures;
    };
    
    class InfoBar : public wxInfoBar
    {
    public:
        InfoBar(wxWindow * parent, wxWindowID winid = wxID_ANY)
            : wxInfoBar(parent, winid)
        {
            SetBackgroundColour(gsgs_Style().GetActiveMarkerColour());
        }
        virtual ~InfoBar()
        {
        }

        void DisplayMessage(const wxString& message, int flags = wxICON_INFORMATION,
            const std::vector<std::pair<wxWindowID, wxString> >& buttons = {})
        {
            Clear();
            m_buttons = buttons;
            if (m_buttons.empty()) { m_buttons.push_back({ wxID_OK, "" }); }
            std::for_each(m_buttons.begin(), m_buttons.end(),
                [&](const std::pair<wxWindowID, wxString>& btn) { AddButton(btn.first, btn.second); });
            ShowMessage(message, flags);
        }
    protected:
        void Clear()
        {
            std::for_each(m_buttons.begin(), m_buttons.end(),
                [&](const std::pair<wxWindowID, wxString>& btn) { RemoveButton(btn.first); });
        }
    private:
        std::vector<std::pair<wxWindowID, wxString> > m_buttons;
    };

    extern const wxEventType wxEVT_LOAD_PERSPECTIVE;
    extern const wxEventType wxEVT_REFRESH_PERSPECTIVE_MENU;
    extern const wxEventType wxEVT_ACTIVATE_EDITOR;

    class ThemeHandler : public wxEvtHandler
    {
    public:
        ThemeHandler();
        virtual ~ThemeHandler();

        void UpdateNotebookColours(wxWindow* parent);
        void OnEditorThemeChanged(wxCommandEvent& e);
        void OnColoursChanged(Event& e);
        void OnInitDone(wxCommandEvent& e);
    protected:
        void UpdateColours();
    private:
        ThemeHandlerHelper::Ptr m_helper;
    };

    class Frame : public wxFrame
    {
    public:
        virtual ~Frame();

        static Frame * singleton();

        static Frame * setup(bool recover);

        inline wxMenuBar * GetMenuBar() const               { return mMenu->getMenu(); }

        inline ToolBar * GetMainToolBar() const             { return mToolBar; }

        inline InfoBar * GetMessageBar() const              { return mInfoBar; }

        inline StatusBar * GetMainStatusBar() const         { return mStatusBar; }

        void OnGotoAnything(wxCommandEvent& e);

        /**
         * @brief main book page navigation
         */
        void OnMainBookNavigating(wxCommandEvent& e);

        /**
         * @brief move the active tab right or left
         */
        void OnMainBookMovePage(wxCommandEvent& e);

        /**
         * @brief Return gsgs App object
         */
        GsGsApp* GetTheApp();

        /**
         * @brief update the parser (code completion) search paths using the
         * default compiler as the input provider
         */
        void UpdateParserSearchPathsFromDefaultCompiler();

        /**
         * \brief update various AUI parameters like Output Pane View caption visible and other
         */
        void UpdateAUI();

        /**
         * @brief update the environment status message:
         * which displays to the user the current environment set used + the active builder
         */
        void SelectBestEnvSet();

        void SetFrameFlag(bool set, int flag);

        inline TagsOptionsData& GetTagsOptions()            { return m_tagsOptionsData; }

        /**
         * @brief return true if the word under the caret should be highlighted
         * @return
         */
        inline bool GetHighlightWord() const                { return m_highlightWord; }

        void SetFrameTitle(Editor* editor);

        inline Book* GetMainBook() const                    { return mWorkBench->GetMainBook(); }
        inline OutputPane* GetOutputPane() const            { return mWorkBench->GetOutputPane(); }
        inline DebuggerPane * GetDebuggerPane() const       { return mWorkBench->GetDebuggerPane(); }
        inline WorkspacePane * GetWorkspacePane() const     { return mWorkBench->GetWorkspacePane(); }
        inline PerspectivePane * GetPerspectivePane() const { return mWorkBench->GetPerspectivePane(); }
        inline ScenePane * GetScenePane() const             { return mWorkBench->GetScenePane(); }
        inline WorkspaceTab * GetWorkspaceTab() const       { return GetWorkspacePane()->GetWorkspaceTab(); }
        inline FileExplorer * GetFileExplorer() const       { return GetWorkspacePane()->GetFileExplorer(); }
        inline ConfigurationPanel* GetConfigPanel() const   { return mConfigPanel; }
        inline wxAuiManager* GetAuiManager() const          { return mWinMag; }

        /**
         * Load session into LE
         */
        void LoadSession(const wxString& sessionName);

        /**
         * Compelete the main frame initialization
         * this method is called immediatly after the
         * main frame construction is over.
         */
        void CompleteInitialization();

        int getFrameMark() const;

        void OnSingleInstanceOpenFiles(Event & e);
        void OnSingleInstanceRaise(Event & e);

        /**
         * @brief rebuild the give project
         * @param projectName
         */
        void RebuildProject(const wxString & projectName);

        /**
         * @brief handle custom build targets events
         */
        void OnBuildCustomTarget(wxCommandEvent & event);

        /**
         * @brief save the current IDE layout and session
         */
        bool SaveLayoutAndSession();

        /**
         * @brief save settings (such as whether to show the splashscreen) which should be saved
         * regardless of whether SaveLayoutAndSession is called
         */
        void SaveGeneralSettings();

        /**
         * @brief create the recently-opened-workspaces menu
         */
        void OnRecentlyWorkspace(wxCommandEvent& e);

        void Bootstrap();

#ifdef __WXGTK__
        inline bool GetIsWaylandSession() const             { return m_isWaylandSession; }
#endif
    private:
        Frame(wxWindow* pParent, wxWindowID id, bool lastsession, 
            long style = wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxCAPTION | wxSYSTEM_MENU | wxRESIZE_BORDER | wxCLIP_CHILDREN);
        wxString CreateWorkspaceTable();
        wxString CreateFilesTable();

        void CreateViewImpl();

        /**
         * \brief update the path & name of the build tool
         * on windows, try to locate make, followed by mingw32-make
         */
        void UpdateBuildTools();
        /**
         * Helper function that prompt user with a simple wxTextEntry dialog
         * @param msg message to display to user
         * @return user's string or wxEmptyString if 'Cancel' pressed.
         */
        void DispatchCommandEvent(wxCommandEvent& event);
        void DispatchUpdateUIEvent(wxUpdateUIEvent& event);

        void OnSplitSelection(wxCommandEvent& event);
        void OnSplitSelectionUI(wxUpdateUIEvent& event);

        /// Toolbar management
        void CreateToolBar(int toolSize);
        void ToggleToolBars(bool all);

        //void CreateRecentlyOpenedFilesMenu();
        bool ReloadExternallyModifiedProjectFiles();
        //void DoEnableWorkspaceViewFlag(bool enable, int flag);
        //bool IsWorkspaceViewFlagEnabled(int flag);
        /**
         * @brief show the startup wizard
         * @return true if a restart is needed
         */
        bool StartSetupWizard(int page = 0);

        /**
         * @brief see if the wizard changed developer profile
         * @return true if the 'Save Perspective' dialog should not be shown
         */
        bool GetAndResetNoSavePerspectivePrompt()
        {
            bool ans = m_noSavePerspectivePrompt;
            m_noSavePerspectivePrompt = false;
            return ans;
        }
        /**
         * @brief mark not to show the 'Save Perspective' dialog on next close
         */
        inline void SetNoSavePerspectivePrompt(bool devProfileChanged) { m_noSavePerspectivePrompt = devProfileChanged; }

        void DoShowCaptions(bool show);
    public:
        void ShowOrHideCaptions();
        void OnCheckForUpdate(wxCommandEvent& e);
        void OnRunSetupWizard(wxCommandEvent& e);
    protected:
        void OnInfobarButton(wxCommandEvent& event);

        void OnRestoreDefaultLayout(wxCommandEvent& e);
        void OnIdle(wxIdleEvent& e);
        void OnBuildEnded(Event& event);
        void OnQuit(wxCommandEvent& event);
        void OnMaximize(wxCommandEvent& event);
        void OnMinimize(wxCommandEvent& event);
        void OnClose(wxCloseEvent& event);
        void OnCustomiseToolbar(wxCommandEvent& event);
        void OnSave(wxCommandEvent& event);
        void OnDuplicateTab(wxCommandEvent& event);
        void OnFileSaveUI(wxUpdateUIEvent& event);
        void OnSaveAs(wxCommandEvent& event);
        void OnFileReload(wxCommandEvent& event);
        void OnFileLoadTabGroup(wxCommandEvent& event);
        void OnNativeTBUnRedoDropdown(wxCommandEvent& event);
        void OnTBUnRedo(wxCommandEvent& event);
        void OnTBUnRedoMenu(wxCommandEvent& event);
        void OnCodeComplete(wxCommandEvent& event);
        void OnWordComplete(wxCommandEvent& event);
        void OnCompleteWordRefreshList(wxCommandEvent& event);
        void OnFunctionCalltip(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnReportIssue(wxCommandEvent& event);
        //void OnCheckForUpdate(wxCommandEvent& e);
        void OnFileNew(wxCommandEvent& event);
        void OnFileOpen(wxCommandEvent& event);
        void OnFileOpenFolder(wxCommandEvent& event);
        void OnFileClose(wxCommandEvent& event);
        void OnFileCloseUI(wxUpdateUIEvent& event);
        void OnFileSaveAll(wxCommandEvent& event);
        void OnFileSaveTabGroup(wxCommandEvent& event);
        void OnFileExistUpdateUI(wxUpdateUIEvent& event);
        void OnCopyFilePathRelativeToWorkspaceUI(wxUpdateUIEvent& event);
        void OnFileSaveAllUI(wxUpdateUIEvent& event);
        void OnCompleteWordUpdateUI(wxUpdateUIEvent& event);
        void OnFunctionCalltipUI(wxUpdateUIEvent& event);
        void OnIncrementalSearch(wxCommandEvent& event);
        void OnIncrementalReplace(wxCommandEvent& event);
        void OnIncrementalSearchUI(wxUpdateUIEvent& event);
        void OnPrint(wxCommandEvent& event);
        void OnPageSetup(wxCommandEvent& event);
        void OnRecentWorkspaceUI(wxUpdateUIEvent& e);
        void OnViewOptions(wxCommandEvent& event);
        void OnToggleMainTBars(wxCommandEvent& event);
        void OnTogglePluginTBars(wxCommandEvent& event);
        void OnTogglePanes(wxCommandEvent& event);
        void OnToggleMinimalView(wxCommandEvent& event);
        void OnToggleMinimalViewUI(wxUpdateUIEvent& event);
        void OnShowStatusBar(wxCommandEvent& event);
        void OnShowStatusBarUI(wxUpdateUIEvent& event);
        void OnShowToolbar(wxCommandEvent& event);
        void OnShowToolbarUI(wxUpdateUIEvent& event);
        void OnShowMenuBar(wxCommandEvent& event);
        void OnShowMenuBarUI(wxUpdateUIEvent& event);
        void OnShowTabBar(wxCommandEvent& event);
        void OnShowTabBarUI(wxUpdateUIEvent& event);
        void OnProjectNewWorkspace(wxCommandEvent& event);
        void OnProjectNewProject(wxCommandEvent& event);
        void OnReloadWorkspace(wxCommandEvent& event);
        //void OnReloadWorkspaceUI(wxUpdateUIEvent& event);
        void OnSwitchWorkspace(wxCommandEvent& event);
        void OnSwitchWorkspaceUI(wxUpdateUIEvent& event);
        void OnNewWorkspaceUI(wxUpdateUIEvent& event);
        void OnCloseWorkspace(wxCommandEvent& event);
        void OnProjectAddProject(wxCommandEvent& event);
        void OnReconcileProject(wxCommandEvent& event);
        void OnWorkspaceOpen(wxUpdateUIEvent& event);
        void OnNewProjectUI(wxUpdateUIEvent& event);
        void OnRetagWorkspaceUI(wxUpdateUIEvent& event);
        void OnAddEnvironmentVariable(wxCommandEvent& event);
        void OnAdvanceSettings(wxCommandEvent& event);
        void OnCtagsOptions(wxCommandEvent& event);
        void OnBuildProject(wxCommandEvent& event);
        void OnBuildProjectOnly(wxCommandEvent& event);
        void OnShowBuildMenu(wxCommandEvent& e);
        void OnBuildAndRunProject(wxCommandEvent& event);
        void OnRebuildProject(wxCommandEvent& event);
        void OnRetagWorkspace(wxCommandEvent& event);
        void OnBuildProjectUI(wxUpdateUIEvent& event);
        void OnStopBuild(wxCommandEvent& event);
        void OnStopBuildUI(wxUpdateUIEvent& event);
        void OnStopExecutedProgram(wxCommandEvent& event);
        void OnStopExecutedProgramUI(wxUpdateUIEvent& event);
        void OnCleanProject(wxCommandEvent& event);
        void OnCleanProjectOnly(wxCommandEvent& event);
        void OnCleanProjectUI(wxUpdateUIEvent& event);
        void OnExecuteNoDebug(wxCommandEvent& event);
        void OnExecuteNoDebugUI(wxUpdateUIEvent& event);
        void OnTimer(wxTimerEvent& event);
        void OnFileCloseAll(wxCommandEvent& event);
        void OnQuickOutline(wxCommandEvent& event);
        void OnImportMSVS(wxCommandEvent& e);
        void OnDebugAttach(wxCommandEvent& event);
        void OnCopyFilePath(wxCommandEvent& event);
        void OnCopyFilePathRelativeToWorkspace(wxCommandEvent& event);
        void OnCopyFilePathOnly(wxCommandEvent& event);
        void OnCopyFileName(wxCommandEvent& event);
        void OnHighlightWord(wxCommandEvent& event);
        void OnShowNavBar(wxCommandEvent& e);
        void OnShowNavBarUI(wxUpdateUIEvent& e);
        void OnOpenShellFromFilePath(wxCommandEvent& e);
        void OnOpenFileExplorerFromFilePath(wxCommandEvent& e);
        void OnDetachEditor(wxCommandEvent& e);
        void OnMarkEditorReadonly(wxCommandEvent& e);
        void OnMarkEditorReadonlyUI(wxUpdateUIEvent& e);
        void OnDetachEditorUI(wxUpdateUIEvent& e);
        void OnQuickDebug(wxCommandEvent& e);
        void OnQuickDebugUI(wxUpdateUIEvent& e);
        void OnDebugCoreDump(wxCommandEvent& e);
        void OnNextFiFMatch(wxCommandEvent& e);
        void OnPreviousFiFMatch(wxCommandEvent& e);
        void OnNextFiFMatchUI(wxUpdateUIEvent& e);
        void OnPreviousFiFMatchUI(wxUpdateUIEvent& e);
        void OnGrepWord(wxCommandEvent& e);
        void OnGrepWordUI(wxUpdateUIEvent& e);
        void OnWebSearchSelection(wxCommandEvent& e);
        void OnWebSearchSelectionUI(wxUpdateUIEvent& e);
        void OnTheme(wxCommandEvent& e);
        void OnEnvironmentVariablesModified(Event& e);
        void OnFindReferences(RefactoringEvent& e);
        void OnRenameSymbol(RefactoringEvent& e);

        // handle symbol tree events
        void OnParsingThreadMessage(wxCommandEvent& e);
        void OnDatabaseUpgrade(wxCommandEvent& e);
        void OnDatabaseUpgradeInternally(wxCommandEvent& e);
        void OnRefreshPerspectiveMenu(wxCommandEvent& e);
        void OnClearTagsCache(wxCommandEvent& e);
        void OnRetaggingCompelted(wxCommandEvent& e);
        void OnRetaggingProgress(wxCommandEvent& e);

        void OnRecentFile(wxCommandEvent& event);
        void OnRecentWorkspace(wxCommandEvent& event);
        void OnBackwardForward(wxCommandEvent& event);
        void OnBackwardForwardUI(wxUpdateUIEvent& event);

        void OnShowDebuggerWindow(wxCommandEvent& e);
        void OnShowDebuggerWindowUI(wxUpdateUIEvent& e);
        void OnDebug(wxCommandEvent& e);
        void OnDebugUI(wxUpdateUIEvent& e);
        void OnDebugRestart(wxCommandEvent& e);
        void OnDebugRestartUI(wxUpdateUIEvent& e);
        void OnDebugStop(wxCommandEvent& e);
        void OnDebugStopUI(wxUpdateUIEvent& e);
        void OnDebugManageBreakpointsUI(wxUpdateUIEvent& e);
        void OnDebugCmd(wxCommandEvent& e);
        void OnToggleReverseDebugging(wxCommandEvent& e);
        void OnToggleReverseDebuggingRecording(wxCommandEvent& e);
        void OnToggleReverseDebuggingUI(wxUpdateUIEvent& e);
        void OnToggleReverseDebuggingRecordingUI(wxUpdateUIEvent& e);
        void OnDebugCmdUI(wxUpdateUIEvent& e);

        void OnDebugRunToCursor(wxCommandEvent& e);
        void OnDebugJumpToCursor(wxCommandEvent& e);

        // Special UI handlers for debugger events
        void OnDebugStepInstUI(wxUpdateUIEvent& e);
        void OnDebugJumpToCursorUI(wxUpdateUIEvent& e);
        void OnDebugRunToCursorUI(wxUpdateUIEvent& e);
        void OnDebugInterruptUI(wxUpdateUIEvent& e);
        void OnDebugShowCursorUI(wxUpdateUIEvent& e);

        //void OnDebuggerSettings(wxCommandEvent& e);
        void OnLinkClicked(wxHtmlLinkEvent& e);
        void OnLoadSession(wxCommandEvent& e);
        void OnAppActivated(wxActivateEvent& event);
        void OnReloadExternallModified(wxCommandEvent& e);
        void OnReloadExternallModifiedNoPrompt(wxCommandEvent& e);
        void OnCompileFile(wxCommandEvent& e);
        void OnCompileFileUI(wxUpdateUIEvent& e);
        void OnCloseAllButThis(wxCommandEvent& e);
        void OnCloseTabsToTheRight(wxCommandEvent& e);
        void OnWorkspaceMenuUI(wxUpdateUIEvent& e);
        void OnUpdateBuildRefactorIndexBar(wxCommandEvent& e);
        void OnUpdateNumberOfBuildProcesses(wxCommandEvent& e);
        void OnBuildWorkspace(wxCommandEvent& e);
        void OnBuildWorkspaceUI(wxUpdateUIEvent& e);
        void OnCleanWorkspace(wxCommandEvent& e);
        void OnCleanWorkspaceUI(wxUpdateUIEvent& e);
        void OnReBuildWorkspace(wxCommandEvent& e);
        void OnReBuildWorkspaceUI(wxUpdateUIEvent& e);

        // Perspectives management
        void OnChangePerspective(wxCommandEvent& e);
        void OnChangePerspectiveUI(wxUpdateUIEvent& e);
        void OnManagePerspectives(wxCommandEvent& e);
        void OnSaveLayoutAsPerspective(wxCommandEvent& e);

        // EOL
        void OnConvertEol(wxCommandEvent& e);
        void OnViewDisplayEOL(wxCommandEvent& e);
        void OnViewWordWrap(wxCommandEvent& e);
        void OnViewWordWrapUI(wxUpdateUIEvent& e);
        void OnViewDisplayEOL_UI(wxUpdateUIEvent& e);

        // Docking windows events
        void OnAuiManagerRender(wxAuiManagerEvent& e);
        void OnDockablePaneClosed(wxAuiManagerEvent& e);
        void OnDetachWorkspaceViewTab(wxCommandEvent& e);
        void OnHideWorkspaceViewTab(wxCommandEvent& e);
        void OnHideOutputViewTab(wxCommandEvent& e);
        void OnDetachDebuggerViewTab(wxCommandEvent& e);
        void OnNewDetachedPane(wxCommandEvent& e);
        void OnDestroyDetachedPane(wxCommandEvent& e);

        void OnManagePlugins(wxCommandEvent& e);
        void OnCppContextMenu(wxCommandEvent& e);

        //void OnConfigureAccelerators(wxCommandEvent& e);
        void OnStartPageEvent(wxCommandEvent& e);
        void OnNewVersionAvailable(wxCommandEvent& e);
        void OnVersionCheckError(wxCommandEvent& e);
        void OnGotoDownloadPage(wxCommandEvent& e);
        void OnBatchBuild(wxCommandEvent& e);
        void OnBatchBuildUI(wxUpdateUIEvent& e);
        //void OnSyntaxHighlight(wxCommandEvent& e);
        void OnShowWhitespaceUI(wxUpdateUIEvent& e);
        void OnShowWhitespace(wxCommandEvent& e);
        void OnNextTab(wxCommandEvent& e);
        void OnPrevTab(wxCommandEvent& e);
        void OnNextPrevTab_UI(wxUpdateUIEvent& e);
        void OnShowFullScreen(wxCommandEvent& e);
        void OnFindResourceXXX(wxCommandEvent& e);
        void OnShowActiveProjectSettings(wxCommandEvent& e);
        void OnShowActiveProjectSettingsUI(wxUpdateUIEvent& e);
        void OnLoadPerspective(wxCommandEvent& e);
        void OnWorkspaceSettings(wxCommandEvent& e);
        void OnWorkspaceEditorPreferences(wxCommandEvent& e);
        void OnParserThreadReady(wxCommandEvent& e);

        // Clang
        //void OnPchCacheStarted(wxCommandEvent& e);
        //void OnPchCacheEnded(wxCommandEvent& e);

        // Misc
        void OnActivateEditor(wxCommandEvent& e);
        void OnActiveEditorChanged(wxCommandEvent& e);
        void OnWorkspaceLoaded(wxCommandEvent& e);
        void OnRefactoringCacheStatus(wxCommandEvent& e);
        void OnWorkspaceClosed(wxCommandEvent& e);
        void OnChangeActiveBookmarkType(wxCommandEvent& e);
        void OnSettingsChanged(wxCommandEvent& e);
        void OnEditMenuOpened(wxMenuEvent& e);
        void OnProjectRenamed(Event& event);

        // Search handlers
        void OnFindSelection(wxCommandEvent& event);
        void OnFindSelectionPrev(wxCommandEvent& event);
        //void OnFindWordAtCaret(wxCommandEvent& event);
        //void OnFindWordAtCaretPrev(wxCommandEvent& event);
        DECLARE_EVENT_TABLE()
    protected:
        bool IsEditorEvent(wxEvent& event);
        //void DoCreateBuildDropDownMenu(wxMenu* menu);
        void DoShowToolbars(bool show, bool update = true);
        void DoShowStatusBar(bool show, bool update = true);
        void loadLogo();
    public:
        static bool m_initCompleted;
    private:
        WindowManager * mWinMag;
        wxArrayString m_files;
        wxTimer* m_timer;
        TagsOptionsData m_tagsOptionsData;
        GeneralInfo * mFrameInfo;
        std::map<int, wxString> m_viewAsMap;
        std::map<int, wxString> m_toolbars;
        wxPanel* mPanel;
        wxString m_DownloadPageURL;
        wxString m_defaultLayout;
        wxSizer* m_horzSizer;
        MenuBar* mMenu;
        InfoBar* mInfoBar;
        ToolBar * mToolBar;
        WorkBench * mWorkBench;
        StatusBar * mStatusBar;
        ConfigurationPanel* mConfigPanel;

        wxMenu* m_bookmarksDropDownMenu;
        ThemeHandler m_themeHandler;
        bool m_workspaceRetagIsRequired;
        bool mRecover;
        bool m_highlightWord;
        bool m_buildAndRun;
        bool m_noSavePerspectivePrompt;
        bool m_toggleToolBar;
#ifndef __WXMSW__
        ZombieReaperPOSIX m_zombieReaper;
#endif

#ifdef __WXGTK__
        bool m_isWaylandSession;
#endif
        gsgsStringSet m_coreToolbars;
        SInstanceThread * m_singleInstanceThread;

        wxPrintDialogData m_printDlgData;
        MainFrameHelper::Ptr_t m_frameHelper;
        WebUpdateJob * m_webUpdate;
        static Frame* mMainFrame;
    };
}
#define gsgs_Frame()        gsgs::Frame::singleton()
#define gsgs_StatusBar()    gsgs::Frame::singleton()->GetMainStatusBar()
#define gsgs_MessageBar()   gsgs::Frame::singleton()->GetMessageBar()
#define gsgs_ToolBar()      gsgs::Frame::singleton()->GetMainToolBar()
#define gsgs_MenuBar()      gsgs::Frame::singleton()->GetMenuBar()
#endif
