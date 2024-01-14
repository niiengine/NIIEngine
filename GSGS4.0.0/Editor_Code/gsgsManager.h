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
#ifndef MANAGER_H
#define MANAGER_H

#include "gsgsSingleton.h"
#include "debuggerobserver.h"
#include "gsgsWorkspace.h"
#include "gsgsShellCmd.h"
#include "gsgsProcess.h"
#include "gsgsBreakpointManager.h"
#include "gsgsPerspectiveManager.h"
#include "gsgsTagManager.h"
#include "gsgsPluginCommon.h"
#include "gsgsWindowManager.h"

class IProcess;

extern const wxEventType wxEVT_CMD_RESTART_GSGS;
extern const wxEventType wxEVT_CMD_DB_CONTENT_CACHE_COMPLETED;

class DisplayVariableDlg;

namespace gsgs
{
    class Editor;
    class DbgStackInfo
    {
    public:
        DbgStackInfo()
            : depth(wxString::npos)
            , func(wxT("")) {}

        ~DbgStackInfo() { Clear(); }

        void Clear()
        {
            func.Clear();
            depth = wxString::npos;
        }

        bool operator==(const DbgStackInfo & o) const { return func == o.func && depth == o.depth; }

        bool operator!=(const DbgStackInfo & o) const { return func != o.func || depth != o.depth; }

        bool IsValid() const { return !func.IsEmpty() && depth != wxString::npos; }
    public:
        size_t depth;
        wxString func;
    };

    class Manager : public wxEvtHandler, public IDebuggerObserver
    {
    public:
        enum
        {
            VD_OK,
            VD_EXISTS,
            VD_ERROR
        };
    public:
        DisplayVariableDlg * GetDebuggerTip();

        PerspectiveManager & GetPerspectiveManager()    { return m_perspectiveManager; }

        void SetRetagInProgress(bool set)               { m_retagInProgress = set; }
        bool GetRetagInProgress() const                 { return m_retagInProgress; }

        void SetShutdownInProgress(bool set)            { m_isShutdown = set; }
        bool IsShutdownInProgress() const               { return m_isShutdown; }

        void SetRepositionEditor(bool set)              { m_repositionEditor = set; }
        bool GetRepositionEditor() const                { return m_repositionEditor; }

        void OnRestart(Event & event);
        void OnCmdRestart(wxCommandEvent & event);
        void GenerateCompileCommands();
        void OnFindInFilesDismissed(FindInFilesEvent & event);
        void OnFindInFilesShowing(FindInFilesEvent & event);
        void OnUpdateDebuggerActiveView(DebugEvent & event);
        void OnDebuggerSetMemory(DebugEvent & event);

        bool IsWorkspaceClosing() const                 { return m_workspceClosing; }

        bool IsWorkspaceOpen() const                    { return m_workspaceOpen; }

        void CreateWorkspace(const wxString & name, const wxString & path);

        void OpenWorkspace(const wxString & path);

        void ReloadWorkspace();

        void CloseWorkspace();

        /**
         * @brief update the C++ parser search / exclude paths with the global paths
         * and the workspace specifc ones
         * @return true if the paths were modified, false otherwise
         */
        void UpdateParserPaths(bool notify);

        /**
         * @brief create an empty project
         */
        void CreateProject(ProjectData & data, const wxString & workspaceFolder);

        /**
         * Add an existing project to the workspace. If no workspace is open,
         * this function does nothing
         * \param path project file path name to add
         */
        void AddProject(const wxString & path);

        /**
         * Checks the active project for file entries that no longer exist on the filesystem,
         * and for existing files that are missing from the project
         * \param projectName the name of the project to reconcile. If "" do the currently-active project
         */
        void ReconcileProject(const wxString & projectName = "");

        /**
         * Import a MS Solution file and open it in the editor
         * \param path path to the .sln file
         */
        void ImportMSVSSolution(const wxString & path, const wxString & defaultCompiler);

        /**
         * Remove the a project from the workspace
         * \param name project name to remove
         * \return true on success false otherwise
         */
        bool RemoveProject(const wxString& name, bool notify);

        /**
         * Return all project names under this workspace
         */
        void GetProjectList(wxArrayString& list);

        /**
         * find project by name
         */
        ProjectPtr GetProject(const wxString& name) const;

        /**
         * \return active project name
         */
        wxString GetActiveProjectName();

        /**
         * Set project as active
         * \param name project name to set as active
         */
        void SetActiveProject(const wxString& name);

        /**
         * Return the workspace build matrix
         */
        BuildMatrixPtr GetWorkspaceBuildMatrix() const;

        /**
         * Set or update the workspace build matrix
         */
        void SetWorkspaceBuildMatrix(BuildMatrixPtr matrix);

        /**
         * @brief user modified the project settings dialog
         */
        void OnProjectSettingsModified(ProjectSettingsEvent& event);

        /**
         * return list of files in absolute path of the whole workspace
         */
        void GetWorkspaceFiles(wxArrayString& files);

        /**
         * @brief return list of files in a form of std::set
         */
        void GetWorkspaceFiles(std::set<wxString>& files);

        /**
         * return list of files that are part of the workspace
         */
        void GetWorkspaceFiles(std::vector<wxFileName>& files, bool absPath = false);

        /**
         * check if a file is part of the workspace
         * \param fileName the file name in absolute path
         */
        bool IsFileInWorkspace(const wxString& fileName);

        /**
         * Search for (non-absolute) file in the workspace
         */
        wxFileName FindFile(const wxString& fileName, const wxString& project = wxEmptyString);

        /**
         * retag workspace
         */
        void RetagWorkspace(TagsManager::RetagType type);

        /**
         * @brief the parser thread has completed to scan for include files to parse
         * @param event
         */
        void OnIncludeFilesScanDone(wxCommandEvent& event);

        void OnDbContentCacherLoaded(wxCommandEvent& event);

        /**
         * \brief retag a given file
         * \param filename
         */
        void RetagFile(const wxString& filename);

        /**
         * \brief Launch the ParseThread to update the preprocessor vizualisation
         * \param filename
         */
        void UpdatePreprocessorFile(Editor* editor);

        /**
         * Add new virtual directory to the workspace.
         * \param path a dot separated string of the new virtual directory full path up to the parent project
         *        for example: to add a new VD name VD3 under: Project1->VD1->VD2 path should contain: Project1.VD1.VD2.VD3
         */
        int AddVirtualDirectory(const wxString& path, bool createIt);

        /**
         * Remove virtual directory from the workspace.
         * \param path a dot separated string of the virtual directory to be removed
         */
        void RemoveVirtualDirectory(const wxString& path);

        /**
         * Create new file on the disk and open it in the main editor
         * \param fileName file full path (including directories)
         * \param vdFullPath path of the virtual directory
         */
        bool AddNewFileToProject(const wxString & fileName, const wxString & vdFullPath, bool openIt = true);

        /**
         * Add an existing file to workspace
         * \param fileName file full path (including directories)
         * \param vdFullPath path of the virtual directory
         */
        bool AddFileToProject(const wxString& fileName, const wxString & vdFullPath, bool openIt = false);

        /**
         * \brief
         * \param files
         * \param vdFullPath
         * \param actualAdded
         * \return
         */
        void AddFilesToProject(const wxArrayString& files, const wxString & vdFullPath, wxArrayString& actualAdded);

        /**
         * remove file from the workspace
         * \param fileName the full path of the file to be removed
         * \param vdFullPath the files' virtual directory path (including project)
         * \param fullpathRemoved [output] set the full path of the file removed
         * \param notify if set to true, this function will also fire the wxEVT_PROJ_FILE_REMOVED event
         */
        bool RemoveFile(const wxString& fileName, const wxString& vdFullPath, wxString& fullpathRemoved, bool notify = true);

        /**
         * remove file from the workspace
         * \param origName the full path of the file to be renamed
         * \param newName the full path the file must be renamed to
         * \param vdFullPath the files' virtual directory path (including project)
         */
        bool RenameFile(const wxString & origName, const wxString & newName, const wxString& vdFullPath);

        bool MoveFileToVD(const wxString & fileName, const wxString & srcVD, const wxString& targetVD);

        /**
         * Rebuild the database by removing all entries from the database
         * that belongs to a given project, and then re-index all files
         * \param projectName project to re-tag
         */
        void RetagProject(const wxString& projectName, bool quickRetag);

        /**
         * return list of files in absolute path of a given project
         * \param project project name
         */
        void GetProjectFiles(const wxString& project, wxArrayString& files);

        /**
         * @brief return list of files belonged the active project (same as running: GetProjectFiles(GetActiveProjectName(),
         * files)
         */
        void GetActiveProjectFiles(wxArrayString& files);

        /**
         * @brief return the currently opened file's project files
         */
        void GetActiveFileProjectFiles(wxArrayString& files);

        /**
         * @brief return the project name that 'fullPathFileName' belongs to. if 2 matches are found, return
         * the first one, or empty string if no match is found
         * \param fullPathFileName the filepath to search with
         * \param caseSensitive do a case-sensitive search
         */
        wxString GetProjectNameByFile(const wxString& fullPathFileName, bool caseSensitive = false);
        /**
         * @brief return the project name that 'fullPathFileName' belongs to. if 2 matches are found, return
         * the first one, or empty string if no match is found
         * \param fullPathFileName the filepath to search with
         * \param caseSensitive do a case-sensitive search
         */
        wxString GetProjectNameByFile(wxString& fullPathFileName, bool caseSensitive = false);

        /**
         * @brief if a workspace is opened, return the current build configuration
         * of the active project
         * @return active build configuration or NULL
         */
        BuildConfigPtr GetCurrentBuildConf();

        /**
         * Return a project working directory
         * \param project project name
         */
        wxString GetProjectCwd(const wxString& project) const;

        /**
         * Return project settings by name
         * \param projectName project name
         * \return project settings smart prt
         */
        ProjectSettingsPtr GetProjectSettings(const wxString& projectName) const;

        /**
         * Set project settings
         * \param projectName project name
         * \param settings settings to update
         */
        void SetProjectSettings(const wxString& projectName, ProjectSettingsPtr settings);

        /**
         * Set project global settings
         * \param projectName project name
         * \param settings global settings to update
         */
        void SetProjectGlobalSettings(const wxString& projectName, BuildConfigCommonPtr settings);

        /**
         * \brief return the project excution command as it appears in the project settings
         * \param projectName
         * \param wd the working directory that the command should be running from
         * \param considerPauseWhenExecuting when set to true (default) gsgs will take into consideration the value set
         * in the project
         * settings 'Pause when execution ends'
         * \return project execution command or wxEmptyString if the project does not exist
         */
        wxString GetProjectExecutionCommand(const wxString & projectName, wxString & wd, bool considerPauseWhenExecuting = true);

        bool DoFindDockInfo(const wxString & saved_perspective, const wxString & dock_name, wxString & dock_info);

        /**
         * \brief test if pane_name is resides in the wxAuiManager and is visible
         * \param pane_name pane name to search for
         * \return true on success (exist in the AUI manager and visible), false otherwise
         */
        bool IsPaneVisible(const wxString& pane_name);

        /**
         * Show output pane and set focus to focusWin
         * \param focusWin tab name to set the focus
         * \return return true if the output pane was hidden and this function forced it to appear. false if the window was
         * already
         * shown and nothing needed to be done
         */
        bool ShowOutputPane(wxString focusWin = wxEmptyString, bool commit = true);

        /**
         * Show the debugger pane
         */
        void ShowDebuggerPane(bool commit = true);

        /**
         * Show the workspace pane and set focus to focusWin
         * \param focusWin tab name to set the focus
         */
        void ShowWorkspacePane(wxString focusWin = wxEmptyString, bool commit = true);

        /**
         * Show the workspace pane and set focus to focusWin
         * \param focusWin tab name to set the focus
         */
        void ShowPerspectivePane(wxString focusWin = wxEmptyString, bool commit = true);

        /**
         * Show the workspace pane and set focus to focusWin
         * \param focusWin tab name to set the focus
         */
        void ShowScenePane(bool commit = true);

        /**
         * Hide pane
         */
        void HidePane(const wxString& paneName, bool commit = true);

        /**
         * Hide/Show all panes. This function saves the current prespective and
         * then hides all panes, when called again, all panes are restored
         */
        bool TogglePanes();

        bool IsShowAllPanes() const { return m_ShowAllPanes; }

        /**
         * \brief update the menu bar accelerators
         */
        void UpdateMenuAccelerators(wxFrame* frame = NULL);

        /**
         * return true a child program is running
         */
        bool IsProgramRunning() const;

        /**
         * Execute the project with no debugger
         */
        void ExecuteNoDebug(const wxString& projectName);

        /**
         * Kill child program which is running
         */
        void KillProgram();
    protected:
        void OnProcessEnd(gsgs::ProcessEvent& event);
        void OnProcessOutput(gsgs::ProcessEvent& event);
        void OnBuildEnded(BuildEvent& event);
        void OnBuildStarting(BuildEvent& event);
    protected:
        void DoUpdateDebuggerTabControl(wxWindow* curpage);
        bool DebuggerPaneWasShown;

        wxFileName FindFile(const wxArrayString& files, const wxFileName& fn);

        void DoRestartGSGS();
        void DoSetupWorkspace(const wxString& path);

        void OnAddWorkspaceToRecentlyUsedList(wxCommandEvent& e);
        void OnParserThreadSuggestColourTokens(Event& event);
        /**
         * @brief a project was renamed, reload the workspace
         */
        void OnProjectRenamed(Event& event);

    public:
        BreakPointManager* GetBreakpointsMgr() { return m_breakptsmgr; }

        /**
         * @brief start a terminal for the debugger and return its TTY
         * @param title terminal title
         * @param tty [output] the terminal TTY
         * @return true on success, false otherwise
         */
        bool StartTTY(const wxString& title, wxString& tty);

        void UpdateDebuggerPane();

        void SetMemory(const wxString& address, size_t count, const wxString& hex_value);

        /**
         * Stores the debugger pane status when the debug session started
         */
        void SetDebuggerPaneOriginallyVisible(bool shown) { DebuggerPaneWasShown = shown; }

        /**
         * Returns true if the debugger pane was already shown when the debug session started
         */
        bool GetDebuggerPaneOriginallyVisible() const { return DebuggerPaneWasShown; }

        void DbgStart(long pid = wxNOT_FOUND);
        void DbgStop();
        void DbgContinue();
        void DbgMarkDebuggerLine(const wxString& fileName, int lineno);
        void DbgUnMarkDebuggerLine();
        void DbgDoSimpleCommand(int cmd);
        void DbgSetFrame(int frame, int lineno);
        void DbgSetThread(long threadId);
        bool DbgCanInteract();
        void DbgClearWatches();
        void DbgRestoreWatches();
        const DbgStackInfo & DbgGetCurrentFrameInfo() { return m_dbgCurrentFrameInfo; }

        //---------------------------------------------------
        // Internal implementaion for various debugger events
        //---------------------------------------------------

        void UpdateAddLine(const wxString & line, const bool OnlyIfLoggingOn = false);
        void UpdateFileLine(const wxString & file, int lineno, bool repositionEditor = true);
        void UpdateGotControl(const DebuggerEventData & e);
        void UpdateLostControl();
        void UpdateRemoteTargetConnected(const wxString & line);
        void UpdateTypeReolsved(const wxString & expression, const wxString & type);
        void UpdateAsciiViewer(const wxString & expression, const wxString & tip);

        //---------------------------------------------------
        // Handle debugger event
        //---------------------------------------------------

        void DebuggerUpdate(const DebuggerEventData & event);
        void DoShowQuickWatchDialog(const DebuggerEventData & event);
    public:
        /**
         * return true if a compilation is in process (either clean or build)
         */
        bool IsBuildInProgress() const;

        /**
         * Stop current build process
         */
        void StopBuild();

        /**
         * \brief add build job to the internal queue
         * \param buildInfo
         */
        void PushQueueCommand(const QueueCommand & buildInfo);

        /**
         * \brief process the next build job
         */
        void ProcessCommandQueue();

        /**
         * \brief build the entire workspace. This operation is equal to
         * manually right clicking on each project in the workspace and selecting
         * 'build'
         */
        void BuildWorkspace();

        /**
         * \brief clean the entire workspace. This operation is equal to
         * manually right clicking on each project in the workspace and selecting
         * 'clean'
         */
        void CleanWorkspace();

        /**
         * \brief clean, followed by buid of the entire workspace. This operation is equal to
         * manually right clicking on each project in the workspace and selecting
         * 'clean'
         */
        void RebuildWorkspace();

        /**
         * \brief when building using custom build, execute the makefile generation command provided by the user
         * \param project project to execute it for
         */
        void RunCustomPreMakeCommand(const wxString& project);

        /**
         * compile single file from a given
         * given project
         */
        void CompileFile(const wxString& project, const wxString& fileName, bool preprocessOnly = false);

        /**
         * return true if the last buid ended successfully
         */
        bool IsBuildEndedSuccessfully() const;

        /**
         * @brief return the currently active project && configuration
         * @param project [output]
         * @param conf [output]
         */
        void GetActiveProjectAndConf(wxString& project, wxString& conf);
        /**
         * @brief return true if debugger view is visible
         * This can be true incase the view is the selected tab in the debuggger pane notebook
         * or incase it is detached and visible
         */
        bool IsDebuggerViewVisible(const wxString& name);

        /**
         * @brief show the new project wizard.
         * @param workspaceFolder the new project will be placed inside this workspace folder.
         * If left empty (the default) place the new project directly under the workspace
         */
        void ShowNewProjectWizard(const wxString &workspaceFolder = wxEmptyString);
    protected:
        Manager();
        virtual ~Manager();

        void OnHideGdbTooltip(Event & event);

        void DoBuildProject(const QueueCommand & buildInfo);
        void DoCleanProject(const QueueCommand & buildInfo);
        void DoCustomBuild(const QueueCommand & buildInfo);
        void DoCmdWorkspace(int cmd);
        void DoSaveAllFilesBeforeBuild();
    protected:
        friend class Singleton<Manager>;

        ShellCommand * m_shellProcess;
        IProcess * m_programProcess;
        BreakPointManager * m_breakptsmgr;
        long m_tipWinPos;
        int m_frameLineno;
        std::list<QueueCommand> m_buildQueue;
        wxArrayString m_dbgWatchExpressions;
        DisplayVariableDlg * m_watchDlg;
        DbgStackInfo m_dbgCurrentFrameInfo;
        PerspectiveManager m_perspectiveManager;
        DebuggerTerminalPOSIX m_debuggerTerminal;
        bool m_retagInProgress;
        bool m_repositionEditor; // flag used for debugging, should editor be repositioned after user updates like "add watch"
        bool m_isShutdown;
        bool m_workspceClosing;
        bool m_dbgCanInteract;
        bool m_useTipWin;
        bool m_ShowAllPanes;
        bool m_workspaceOpen;
    };
    typedef Singleton<Manager> ManagerST;

    #define gsgs_Manager() ManagerST::Get()
}
#endif