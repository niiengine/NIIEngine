#if USE_SFTP

#include "PhpSFTPHandler.h"
#include "gsgsEvent.h"
#include "gsgsLogManager.h"
#include "gsgsSFTP.h"
#include "php_workspace.h"
#include "ssh_workspace_settings.h"
#include "sftp_settings.h"
#include <wx/msgdlg.h>

PhpSFTPHandler::PhpSFTPHandler()
{
    gsgs_Event().Bind(wxEVT_FILE_SAVED, &PhpSFTPHandler::OnFileSaved, this);
    gsgs_Event().Bind(wxEVT_FILE_RENAMED, &PhpSFTPHandler::OnFileRenamed, this);
    gsgs_Event().Bind(wxEVT_FILES_MODIFIED_REPLACE_IN_FILES, &PhpSFTPHandler::OnReplaceInFiles, this);
    gsgs_Event().Bind(wxEVT_FILE_DELETED, &PhpSFTPHandler::OnFileDeleted, this);
}

PhpSFTPHandler::~PhpSFTPHandler()
{
    gsgs_Event().Unbind(wxEVT_FILE_SAVED, &PhpSFTPHandler::OnFileSaved, this);
    gsgs_Event().Unbind(wxEVT_FILE_RENAMED, &PhpSFTPHandler::OnFileRenamed, this);
    gsgs_Event().Unbind(wxEVT_FILES_MODIFIED_REPLACE_IN_FILES, &PhpSFTPHandler::OnReplaceInFiles, this);
    gsgs_Event().Unbind(wxEVT_FILE_DELETED, &PhpSFTPHandler::OnFileDeleted, this);
}

void PhpSFTPHandler::OnFileSaved(gsgs::Event& e)
{
    e.Skip();
    if(!PHPWorkspace::Get()->IsOpen()) { return; }
    DoSyncFileWithRemote(e.GetFileName());
}

void PhpSFTPHandler::OnReplaceInFiles(gsgs::FileEvent& e)
{
    e.Skip();
    if(!PHPWorkspace::Get()->IsOpen()) { return; }
    
    SSHWorkspaceSettings settings;
    settings.Load();
    
    if(!EnsureAccountExists(settings)) { return; }
    
    const wxArrayString& files = e.GetStrings();
    for(size_t i = 0; i < files.size(); ++i) {
        DoSyncFileWithRemote(files.Item(i));
    }
}

void PhpSFTPHandler::OnFileRenamed(gsgs::FileEvent& e)
{
    e.Skip();
    if(!PHPWorkspace::Get()->IsOpen()) { return; }

    SSHWorkspaceSettings settings;
    settings.Load();
    
    if(!EnsureAccountExists(settings)) { return; }
    
    wxString oldPath = GetRemotePath(settings, e.GetPath());
    wxString newPath = GetRemotePath(settings, e.GetNewpath());
    if(oldPath.IsEmpty() || newPath.IsEmpty()) { return; }

    gsgsDEBUG() << "PHP SFTP: Renaming:" << oldPath << "->" << newPath;

    // Fire this event, if the sftp plugin is ON, it will handle it
    gsgs::SFTPEvent eventRename(wxEVT_SFTP_RENAME_FILE);
    eventRename.SetAccount(settings.GetAccount());
    eventRename.SetRemoteFile(oldPath);
    eventRename.SetNewRemoteFile(newPath);
    gsgs_Event().AddPendingEvent(eventRename);
}

void PhpSFTPHandler::DoSyncFileWithRemote(const wxFileName& localFile)
{
    // Check to see if we got a remote-upload setup
    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProjectForFile(localFile);
    if(!pProject) {
        // Not a workspace file
        gsgsDEBUG() << localFile << "is not a PHP workspace file, will not sync it with remote" << clEndl;
        return;
    }

    SSHWorkspaceSettings workspaceSettings;
    workspaceSettings.Load();
    
    if(!EnsureAccountExists(workspaceSettings)) { return; }
    
    // Convert the local path to remote path
    wxString remotePath = GetRemotePath(workspaceSettings, localFile.GetFullPath());
    if(remotePath.IsEmpty()) { return; }

    // Fire this event, if the sftp plugin is ON, it will handle it
    gsgs::SFTPEvent eventSave(wxEVT_SFTP_SAVE_FILE);
    eventSave.SetAccount(workspaceSettings.GetAccount());
    eventSave.SetLocalFile(localFile.GetFullPath());
    eventSave.SetRemoteFile(remotePath);
    gsgs_Event().AddPendingEvent(eventSave);
}

wxString PhpSFTPHandler::GetRemotePath(const SSHWorkspaceSettings& sshSettings, const wxString& localpath) const
{
    if(!sshSettings.IsRemoteUploadEnabled() || !sshSettings.IsRemoteUploadEnabled()) { return ""; }
    wxFileName fnLocalFile = localpath;
    fnLocalFile.MakeRelativeTo(PHPWorkspace::Get()->GetFilename().GetPath());
    fnLocalFile.MakeAbsolute(wxFileName(sshSettings.GetRemoteFolder(), "", wxPATH_UNIX).GetPath());
    return fnLocalFile.GetFullPath(wxPATH_UNIX);
}

void PhpSFTPHandler::OnFileDeleted(gsgs::FileEvent& e)
{
    e.Skip();
    if(!PHPWorkspace::Get()->IsOpen()) { return; }

    SSHWorkspaceSettings settings;
    settings.Load();
    
    if(!EnsureAccountExists(settings)) { return; }
    
    const wxArrayString& paths = e.GetPaths();
    if(paths.IsEmpty()) { return; }
    
    for(size_t i = 0; i < paths.size(); ++i) {
        wxString remotePath = GetRemotePath(settings, paths.Item(i));
        if(remotePath.IsEmpty()) { return; }
        
        // Fire this event, if the sftp plugin is ON, it will handle it
        gsgs::SFTPEvent eventDelete(wxEVT_SFTP_DELETE_FILE);
        eventDelete.SetAccount(settings.GetAccount());
        eventDelete.SetRemoteFile(remotePath);
        gsgs_Event().AddPendingEvent(eventDelete);
    }
}

bool PhpSFTPHandler::EnsureAccountExists(SSHWorkspaceSettings& workspaceSettings)
{
    // Do we need to sync?
    if(!(workspaceSettings.IsRemoteUploadSet() && workspaceSettings.IsRemoteUploadEnabled())) { return false; }
    
    SFTPSettings sftpSettings;
    sftpSettings.Load();
    
    // Try to locate hte SSH account for this workspace
    SSHAccountInfo account;
    if(!sftpSettings.GetAccount(workspaceSettings.GetAccount(), account)) {
        // Failed to locate the SSH account, disable sync
        wxString msg;
        msg << _("Failed to locate SSH account: ") << workspaceSettings.GetAccount() << "\n";
        ::wxMessageBox(msg, _("SFTP"), wxOK | wxICON_ERROR);
        // Clear the sync settings and return
        workspaceSettings.Reset();
        workspaceSettings.Save();
        return false;
    }
    return true;
}

#endif //USE_SFTP
