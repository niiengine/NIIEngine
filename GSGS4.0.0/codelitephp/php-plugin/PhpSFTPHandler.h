#ifndef PHPSFTPHANDLER_H
#define PHPSFTPHANDLER_H

#if USE_SFTP

#include "gsgsFileUtil.h"
#include "gsgsEvent.h"
#include "ssh_workspace_settings.h"
#include <wx/event.h>
#include <wx/filename.h>
#include <wx/sharedptr.h>
#include "ssh_workspace_settings.h"

class PhpSFTPHandler : public wxEvtHandler
{
public:
    typedef wxSharedPtr<PhpSFTPHandler> Ptr_t;

protected:
    void DoSyncFileWithRemote(const wxFileName& localFile);
    wxString GetRemotePath(const SSHWorkspaceSettings& sshSettings, const wxString& localpath) const;
    bool EnsureAccountExists(SSHWorkspaceSettings& workspaceSettings);
    
public:
    PhpSFTPHandler();
    virtual ~PhpSFTPHandler();

    void OnFileSaved(gsgs::Event& e);
    void OnFileRenamed(gsgs::FileEvent& e);
    void OnReplaceInFiles(gsgs::FileEvent& e);
    void OnFileDeleted(gsgs::FileEvent& e);
};

#endif //USE_SFTP

#endif // PHPSFTPHANDLER_H
