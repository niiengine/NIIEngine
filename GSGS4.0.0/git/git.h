//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : git.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2011 by René Kraus (aka upCASE)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __git__
#define __git__

#include <wx/progdlg.h>

#include "plugin.h"
#include "gsgsProcess.h"
#include "gsgsEvent.h"
#include "gitentry.h"
#include "gitui.h"
#include "project.h"
#include "gsgsCommonHandler.h"
#include <wx/bitmap.h>
#include <wx/progdlg.h>

class GitBlameDlg;

class OverlayTool
{

    static wxBitmap ms_bmpOK;
    static wxBitmap ms_bmpConflict;
    static wxBitmap ms_bmpModified;
    
public:
    enum BmpType {
        Bmp_NoChange = -1,
        Bmp_OK       =  0, 
        Bmp_Modified =  1, 
        Bmp_Conflict =  2, 
    };
    
private:
    OverlayTool();
    virtual ~OverlayTool();
    
    wxBitmap DoAddBitmap(const wxBitmap& bmp, const wxColour& colour) const;
    
public:
    static OverlayTool& Get();
    
    wxBitmap CreateBitmap(const wxBitmap& orig, OverlayTool::BmpType type) const;
};

class gitAction
{
public:
    gitAction()
        : action(0)
        , arguments("")
        , workingDirectory("")
    {
    }
    gitAction(int act, const wxString& args)
        : action(act)
        , arguments(args)
    {
    }
    ~gitAction() {}
public:
    int action;
    wxString arguments;
    wxString workingDirectory;
};

class GitConsole;
class GitCommitListDlg;

struct GitCmd {
    wxString baseCommand;
    size_t processFlags;

    GitCmd(const wxString& cmd, size_t console)
        : baseCommand(cmd)
        , processFlags(console)
    {
    }
    GitCmd()
        : processFlags(IProcessCreateDefault)
    {
    }
    typedef std::vector<GitCmd> Vec_t;
};

class GitPlugin : public Plugin
{
    friend class GitConsole;
    friend class GitCommitListDlg;
    friend class GitCommitDlg;

    typedef std::map<int, int> IntMap_t;
    enum {
        gitNone = 0,
        gitUpdateRemotes,
        gitListAll,
        gitListModified,
        gitListRemotes,
        gitAddFile,
        gitDeleteFile,
        gitDiffFile,
        gitDiffRepoCommit,
        gitDiffRepoShow,
        gitResetFile,
        gitResetRepo,
        gitPull,
        gitPush,
        gitCommit,
        gitBranchCreate,
        gitBranchCurrent,
        gitBranchList,
        gitBranchListRemote,
        gitBranchSwitch,
        gitBranchSwitchRemote,
        gitCommitList,
        gitBlame,
        gitRevlist,
        gitRebase,
        gitGarbageCollection,
        gitClone,
        gitStatus,
        gitUndoAdd,
        gitRmFiles,
        gitApplyPatch,
        gitRevertCommit,
        gitStash,
        gitStashPop,
        gitConfig,
    };

    wxArrayString m_localBranchList;
    wxArrayString m_remoteBranchList;
    gsgsStringSet m_trackedFiles;
    gsgsStringSet m_modifiedFiles;
    bool m_addedFiles;
    wxArrayString m_remotes;
    wxColour m_colourTrackedFile;
    wxColour m_colourDiffFile;
    wxString m_pathGITExecutable;
    wxString m_pathGITKExecutable;
    wxString m_repositoryDirectory;
    wxString m_currentBranch;
    std::list<gitAction> m_gitActionQueue;
    wxTimer m_progressTimer;
    wxString m_progressMessage;
    wxString m_commandOutput;
    bool m_bActionRequiresTreUpdate;
    IProcess* m_process;
    wxEvtHandler* m_eventHandler;
    wxWindow* m_topWindow;
    gsgs::ToolBar* m_pluginToolbar;
    wxMenu* m_pluginMenu;
    IntMap_t m_treeImageMapping;
    int m_baseImageCount;
    GitConsole* m_console;
    wxFileName m_workspaceFilename;
    GitCommitListDlg* m_commitListDlg;
    wxArrayString m_filesSelected;
    wxString m_selectedFolder;
    gsgs::CmdPrc* m_commandProcessor;
    gsgs::TabTogglerHelper::Ptr_t m_tabToggler;
    GitBlameDlg* m_gitBlameDlg;

private:
    void DoCreateTreeImages();
    void DoShowDiffViewer(const wxString& headFile, const wxString& fileName);
    void DoExecuteCommands(const GitCmd::Vec_t& commands, const wxString& workingDir);
    bool DoExecuteCommandSync(const wxString& command, const wxString& workingDir, wxString& commandOutput);

    void DoSetTreeItemImage(gsgs::TreeCtrl* ctrl, const wxTreeItemId& item, OverlayTool::BmpType bmpType) const;
    void InitDefaults();
    void AddDefaultActions();
    void LoadDefaultGitCommands(GitEntry& data, bool overwrite = false);
    void ProcessGitActionQueue();
    void ColourFileTree(gsgs::TreeCtrl* tree, const gsgsStringSet& files, OverlayTool::BmpType bmpType) const;
    void CreateFilesTreeIDsMap(std::map<wxString, wxTreeItemId>& IDs, bool ifmodified = false) const;
    void DoShowCommitDialog(const wxString& diff, wxString& commitArgs);
    void DoRefreshView(bool ensureVisible);

    /// Workspace management
    bool IsWorkspaceOpened() const;
    wxString GetWorkspaceName() const;
    wxFileName GetWorkspaceFileName() const;

    void FinishGitListAction(const gitAction& ga);
    void ListBranchAction(const gitAction& ga);
    void GetCurrentBranchAction(const gitAction& ga);
    void UpdateFileTree();

    void ShowProgress(const wxString& message, bool pulse = true);
    void HideProgress();
    void DoCleanup();
    void DoAddFiles(const wxArrayString& files);
    void DoResetFiles(const wxArrayString& files);
    void DoGetFileViewSelectedFiles(wxArrayString& files, bool relativeToRepo);
    void DoShowDiffsForFiles(const wxArrayString& files, bool useFileAsBase = false);
    void DoSetRepoPath(const wxString& repoPath = "", bool promptUser = true);
    void DoRecoverFromGitCommandError();

    DECLARE_EVENT_TABLE()

    // Event handlers
    void OnInitDone(wxCommandEvent& e);
    void OnProgressTimer(wxTimerEvent& Event);
    void OnProcessTerminated(gsgs::ProcessEvent& event);
    void OnProcessOutput(gsgs::ProcessEvent& event);
    void OnFileMenu(ContextMenuEvent& event);
    void OnFolderMenu(ContextMenuEvent& event);

    void OnFileCreated(gsgs::FileEvent& event);
    void OnReplaceInFiles(gsgs::FileEvent& event);
    void OnFileSaved(gsgs::Event& e);
    void OnFilesAddedToProject(gsgs::Event& e);
    void OnFilesRemovedFromProject(gsgs::Event& e);
    void OnWorkspaceLoaded(wxCommandEvent& e);
    void OnWorkspaceClosed(wxCommandEvent& e);
    void OnWorkspaceConfigurationChanged(wxCommandEvent& e);
    void OnMainFrameTitle(gsgs::Event& e);
    void OnSetGitRepoPath(wxCommandEvent& e);
    void OnSettings(wxCommandEvent& e);
    void OnFileDiffSelected(wxCommandEvent& e);
    void OnFileResetSelected(wxCommandEvent& e);
    void OnFileAddSelected(wxCommandEvent& e);
    void OnFileDeleteSelected(wxCommandEvent& e);
    void OnSwitchLocalBranch(wxCommandEvent& e);
    void OnSwitchRemoteBranch(wxCommandEvent& e);
    void OnCreateBranch(wxCommandEvent& e);
    void OnCommit(wxCommandEvent& e);
    void OnCommitList(wxCommandEvent& e);
    void OnShowDiffs(wxCommandEvent& e);
    void OnApplyPatch(wxCommandEvent& e);
    void OnPush(wxCommandEvent& e);
    void OnRebase(wxCommandEvent& e);
    void OnPull(wxCommandEvent& e);
    void OnResetRepository(wxCommandEvent& e);
    void OnStartGitk(wxCommandEvent& e);
    void OnStartGitkUI(wxUpdateUIEvent& e);
    void OnListModified(wxCommandEvent& e);
    void OnGitBlame(wxCommandEvent& e);
    void OnRefresh(wxCommandEvent& e);
    void OnGarbageColletion(wxCommandEvent& e);
    void OnOpenMSYSGit(wxCommandEvent& e);
    void OnActiveProjectChanged(gsgs::ProjectSettingsEvent& event);
    void OnFileGitBlame(wxCommandEvent& event);
    void OnAppActivated(wxCommandEvent& event);

#if 0
    void OnBisectStart(wxCommandEvent& e);
    void OnBisectGood(wxCommandEvent& e);
    void OnBisectBad(wxCommandEvent& e);
    void OnBisectReset(wxCommandEvent& e);
#endif
    void OnEnableGitRepoExists(wxUpdateUIEvent& e);
    void OnClone(wxCommandEvent& e);

    // Event handlers from folder context menu
    void OnFolderPullRebase(wxCommandEvent& event);
    void OnFolderCommit(wxCommandEvent& event);
    void OnFolderPush(wxCommandEvent& event);
    void OnFolderStash(wxCommandEvent& event);
    void OnFolderStashPop(wxCommandEvent& event);
    void OnFolderGitBash(wxCommandEvent& event);

public:
    GitPlugin(IManager* manager);
    ~GitPlugin();

    void StoreWorkspaceRepoDetails();
    void WorkspaceClosed();

    /**
     * @brief is git enabled for the current workspace?
     */
    bool IsGitEnabled() const;

    /**
     * @brief fetch the next 100 commits (skip 'skip' first commits)
     * and show them in the commit list dialog
     * @param skip number of first commits to skip
     */
    void FetchNextCommits(int skip, const wxString& args);

    GitConsole* GetConsole() { return m_console; }
    const wxString& GetRepositoryDirectory() const { return m_repositoryDirectory; }
    IProcess* GetProcess() { return m_process; }
    gsgs::CmdPrc* GetFolderProcess() { return m_commandProcessor; }

    IManager* GetManager() { return m_mgr; }

    void ShowDiff(const wxArrayString& files) { DoShowDiffsForFiles(files); }

    void ApplyPatch(const wxString& filename, const wxString& extraFlags);
    void AddFiles(const wxArrayString& files) { DoAddFiles(files); }

    void RevertCommit(const wxString& commitId);

    void ResetFiles(const wxArrayString& files) { DoResetFiles(files); }

    void UndoAddFiles(const wxArrayString& files);

    void RefreshFileListView();
    /**
     * @brief display a message in the console
     * used by outside classes e.g. GitDiffDlg
     * @param message to display
     */
    void DisplayMessage(const wxString& message) const;

    void DoGitBlame(const wxString& args);      // Called by OnGitBlame or the git blame dialog
    wxString GetEditorRelativeFilepath() const; // Called by OnGitBlame or the git blame dialog
    void OnGitBlameRevList(const wxString& arg, const wxString& filepath,
                           const wxString& commit = ""); // Called by the git blame dialog

    /**
     * @brief simple git command executioin completed. Display its output etc
     */
    void OnCommandOutput(gsgs::Event& event);
    void OnCommandEnded(gsgs::Event& event);

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(gsgs::ToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();
};

#endif // git