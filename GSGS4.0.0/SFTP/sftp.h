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

#ifndef __SFTP__
#define __SFTP__

#include "gsgsPreInclude.h"
#include "gsgsFileUtil.h"
#include "gsgsEvent.h"
#include "plugin.h"
#include "gsgsSFTP.h"
#include "ssh_account_info.h"
#include "gsgsConfig.h"

class SFTPStatusPage;
class SFTPTreeView;

class SFTPWorkspaceSettings : public gsgs::ObjectData
{
public:
    SFTPWorkspaceSettings();
    virtual ~SFTPWorkspaceSettings();

    static void Load(SFTPWorkspaceSettings& settings, const wxFileName& workspaceFile);
    static void Save(const SFTPWorkspaceSettings& settings, const wxFileName& workspaceFile);

    void SetAccount(const wxString& account) {
        this->m_account = account;
    }
    void SetRemoteWorkspacePath(const wxString& remoteWorkspacePath) {
        this->m_remoteWorkspacePath = remoteWorkspacePath;
    }
    const wxString& GetAccount() const {
        return m_account;
    }
    const wxString& GetRemoteWorkspacePath() const {
        return m_remoteWorkspacePath;
    }
    
    void Clear();
    
    bool IsOk() const {
        return !m_account.IsEmpty() && !m_remoteWorkspacePath.IsEmpty();
    }
    
    virtual void FromJSON(const JSONObject& json);
    virtual JSONObject ToJSON() const;
private:
    wxString m_account;
    wxString m_remoteWorkspacePath;
};

class SFTPClientData : public wxClientData
{
    wxString localPath;
    wxString remotePath;
    size_t permissions;
    int lineNumber = wxNOT_FOUND;

public:
    SFTPClientData()
        : permissions(0)
    {
    }
    virtual ~SFTPClientData() {}

    void SetLocalPath(const wxString& localPath)            { this->localPath = localPath; }
    void SetRemotePath(const wxString& remotePath)          { this->remotePath = remotePath; }
    const wxString& GetLocalPath() const                    { return localPath; }
    const wxString& GetRemotePath() const                   { return remotePath; }
    void SetPermissions(size_t permissions)                 { this->permissions = permissions; }
    size_t GetPermissions() const                           { return permissions; }
    void SetLineNumber(int lineNumber)                      { this->lineNumber = lineNumber; }
    int GetLineNumber() const                               { return lineNumber; }
};

class SFTPPlugin : public gsgs::Plugin
{
    wxFileName m_workspaceFile;
    SFTPWorkspaceSettings m_workspaceSettings;
    SFTPStatusPage* m_outputPane;
    SFTPTreeView* m_treeView;
    RemoteFileInfo::Map_t m_remoteFiles;
    gsgs::TabTogglerHelper::Ptr_t m_tabToggler;

public:
    SFTPPlugin(IManager* manager);
    ~SFTPPlugin();

    void FileDownloadedSuccessfully(const SFTPClientData& cd);
    void OpenWithDefaultApp(const wxString& localFileName);
    void OpenContainingFolder(const wxString& localFileName);
    void AddRemoteFile(const RemoteFileInfo& remoteFile);
    SFTPStatusPage* GetOutputPane() { return m_outputPane; }
    SFTPTreeView* GetTreeView() { return m_treeView; }

    void OpenFile(const wxString& remotePath, int lineNumber = wxNOT_FOUND);

protected:
    void OnReplaceInFiles(gsgs::FileEvent& e);
    void OnAccountManager(wxCommandEvent& e);
    void OnSettings(wxCommandEvent& e);
    void OnSetupWorkspaceMirroring(wxCommandEvent& e);
    void OnDisableWorkspaceMirroring(wxCommandEvent& e);
    void OnDisableWorkspaceMirroringUI(wxUpdateUIEvent& e);
    void OnWorkspaceOpened(wxCommandEvent& e);
    void OnWorkspaceClosed(wxCommandEvent& e);
    void OnFileSaved(gsgs::Event& e);
    void OnFileRenamed(gsgs::FileEvent& e);
    void OnFileDeleted(gsgs::FileEvent& e);
    void OnEditorClosed(wxCommandEvent& e);
    void MSWInitiateConnection();

    void DoFileSaved(const wxString& filename);

    bool IsWorkspaceOpened() const { return m_workspaceFile.IsOk(); }
    void DoSaveRemoteFile(const RemoteFileInfo& remoteFile);
    bool IsPaneDetached(const wxString& name) const;

    // API calls

    // Save remote file content to match the content of a local file
    // e.GetLocalFile() -> the local file
    // e.GetRemoteFile() -> the target file
    void OnSaveFile(gsgs::SFTPEvent& e);

    // Rename a remote file
    // e.GetRemoteFile() -> the "old" remote file path
    // e.GetNewRemoteFile() -> the "new" remote file path
    void OnRenameFile(gsgs::SFTPEvent& e);

    // Delete a remote file
    // e.GetRemoteFile() -> the file to be deleted
    void OnDeleteFile(gsgs::SFTPEvent& e);

private:
    bool IsCxxWorkspaceMirrorEnabled() const;
    void DoFileDeleted(const wxString& filepath);
    wxString GetRemotePath(const wxString& localpath) const;

public:
    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(gsgs::ToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();
    IManager* GetManager() { return m_mgr; }

    // Callbacks
    void OnFileWriteOK(const wxString& message);
    void OnFileWriteError(const wxString& errorMessage);
};

#endif // SFTPPlugin
