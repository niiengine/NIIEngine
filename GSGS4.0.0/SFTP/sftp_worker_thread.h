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

#ifndef SFTPWRITERTHREAD_H
#define SFTPWRITERTHREAD_H

#include "gsgsSFTP.h"
#include "ssh_account_info.h"
#include "gsgsThread.h"

enum class eSFTPActions {
    kUpload,
    kDownload,
    kConnect,
    kDownloadAndOpenWithDefaultApp,
    kDownloadAndOpenContainingFolder,
    kRename,
    kDelete,
};

class SFTPThreadRequet : public ThreadRequest
{
    SSHAccountInfo m_account;
    wxString m_remoteFile;
    wxString m_localFile;
    size_t m_retryCounter = 0;
    bool m_uploadSuccess = false;
    eSFTPActions m_action;
    size_t m_permissions = 0;
    wxString m_newRemoteFile;
    int m_lineNumber = wxNOT_FOUND;

public:
    SFTPThreadRequet(const SSHAccountInfo& accountInfo, const wxString& remoteFile, const wxString& localFile,
                     size_t persmissions);
    SFTPThreadRequet(const SSHAccountInfo& accountInfo, const wxString& oldName, const wxString& newName);
    SFTPThreadRequet(const SSHAccountInfo& accountInfo, const wxString& fileToDelete);
    SFTPThreadRequet(const RemoteFileInfo& remoteFile);
    SFTPThreadRequet(const SSHAccountInfo& accountInfo);
    SFTPThreadRequet(const SFTPThreadRequet& other);
    SFTPThreadRequet& operator=(const SFTPThreadRequet& other);
    virtual ~SFTPThreadRequet();

    void SetUploadSuccess(bool uploadSuccess) { this->m_uploadSuccess = uploadSuccess; }
    bool IsUploadSuccess() const { return m_uploadSuccess; }
    void SetAccount(const SSHAccountInfo& account) { this->m_account = account; }
    void SetLocalFile(const wxString& localFile) { this->m_localFile = localFile; }
    void SetRemoteFile(const wxString& remoteFile) { this->m_remoteFile = remoteFile; }
    void SetRetryCounter(size_t retryCounter) { this->m_retryCounter = retryCounter; }
    size_t GetRetryCounter() const { return m_retryCounter; }
    const SSHAccountInfo& GetAccount() const { return m_account; }
    const wxString& GetRemoteFile() const { return m_remoteFile; }
    const wxString& GetLocalFile() const { return m_localFile; }
    eSFTPActions GetAction() const { return m_action; }
    void SetAction(eSFTPActions d) { m_action = d; }
    void SetPermissions(size_t permissions) { this->m_permissions = permissions; }
    size_t GetPermissions() const { return m_permissions; }
    ThreadRequest* Clone() const;
    void SetNewRemoteFile(const wxString& newRemoteFile) { this->m_newRemoteFile = newRemoteFile; }
    const wxString& GetNewRemoteFile() const { return m_newRemoteFile; }
    void SetLineNumber(int lineNumber) { this->m_lineNumber = lineNumber; }
    int GetLineNumber() const { return m_lineNumber; }
};

class SFTPThreadMessage
{

    int m_status;
    wxString m_message;
    wxString m_account;

public:
    enum {
        STATUS_NONE = -1,
        STATUS_OK,
        STATUS_ERROR,
    };

public:
    SFTPThreadMessage();
    virtual ~SFTPThreadMessage();

    void SetAccount(const wxString& account) { this->m_account = account; }
    void SetMessage(const wxString& message) { this->m_message = message; }
    const wxString& GetAccount() const { return m_account; }
    const wxString& GetMessage() const { return m_message; }
    void SetStatus(int status) { this->m_status = status; }
    int GetStatus() const { return m_status; }
};

class SFTPWorkerThread : public WorkerThread
{
    static SFTPWorkerThread* gInstance;
    gsgs::SFTP::Ptr_t m_sftp;
    SFTPPlugin* m_plugin;

public:
    static SFTPWorkerThread* Instance();
    static void Release();

private:
    SFTPWorkerThread();
    virtual ~SFTPWorkerThread();
    void DoConnect(SFTPThreadRequet* req);
    void DoReportMessage(const wxString& account, const wxString& message, int status);
    void DoReportStatusBarMessage(const wxString& message);

public:
    virtual void ProcessRequest(ThreadRequest* request);
    void SetSftpPlugin(SFTPPlugin * sftp);
};

#endif // SFTPWRITERTHREAD_H
