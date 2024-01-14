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

#ifndef SFTPTREEVIEW_H
#define SFTPTREEVIEW_H

#include "SFTPSessionInfo.h"
#include "UI.h"
#include "gsgsImageManager.h"
#include "sftp.h"
#include "ssh_account_info.h"

class MyClientData;

typedef std::vector<MyClientData*> MyClientDataVect_t;

class SFTPTreeView : public SFTPTreeViewBase
{
public:
    enum {
        ID_SFTP_BOOKMARK_FIRST = 13000,
        ID_SFTP_BOOKMARK_LAST = 13100,
        ID_SFTP_BOOKMARK_SETTINGS = 13101,
    };

public:
    SFTPTreeView(wxWindow* parent, SFTPPlugin* plugin);
    virtual ~SFTPTreeView();
    bool IsConnected() const { return m_sftp && m_sftp->IsConnected(); }
    const SSHAccountInfo& GetAccount() const { return m_account; }

protected:
    virtual void OnSftpSettings(wxCommandEvent& event);
    virtual void OnOpenTerminal(wxCommandEvent& event);
    virtual void OnOpenTerminalMenu(wxCommandEvent& event);
    virtual void OnOpenTerminalUI(wxUpdateUIEvent& event);
    virtual void OnConnection(wxCommandEvent& event);
    virtual void OnSelectionChanged(wxTreeEvent& event);
    virtual void OnChoiceAccount(wxCommandEvent& event);
    virtual void OnChoiceAccountUI(wxUpdateUIEvent& event);
    virtual void OnGotoLocation(wxCommandEvent& event);
    virtual void OnGotoLocationUI(wxUpdateUIEvent& event);
    virtual void OnAddBookmark(wxCommandEvent& event);
    virtual void OnAddBookmarkMenu(wxCommandEvent& event);
    virtual void OnAddBookmarkUI(wxUpdateUIEvent& event);
    virtual void OnContextMenu(wxContextMenuEvent& event);
    virtual void OnDisconnect(wxCommandEvent& event);
    virtual void OnDisconnectUI(wxUpdateUIEvent& event);
    virtual void OnConnect(wxCommandEvent& event);
    virtual void OnMenuNew(wxCommandEvent& event);
    virtual void OnMenuOpen(wxCommandEvent& event);
    virtual void OnMenuDelete(wxCommandEvent& event);
    virtual void OnMenuRename(wxCommandEvent& event);
    virtual void OnMenuNewFile(wxCommandEvent& event);
    virtual void OnMenuOpenWithDefaultApplication(wxCommandEvent& event);
    virtual void OnMenuOpenContainingFolder(wxCommandEvent& event);
    virtual void OnMenuRefreshFolder(wxCommandEvent& event);
    virtual void OnExecuteCommand(wxCommandEvent& event);
    void OnFileDropped(gsgs::Event& event);
    void OnEditorClosing(wxCommandEvent& evt);
    void OnRemoteFind(wxCommandEvent& event);
    // Edit events
    void OnCopy(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnSelectAll(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);

    // Find events
    void OnFindOutput(gsgs::Event& event);
    void OnFindFinished(gsgs::Event& event);
    void OnFindError(gsgs::Event& event);

    void DoCloseSession();
    void DoOpenSession();
    bool DoExpandItem(const wxTreeItemId& item);
    void DoBuildTree(const wxString& initialFolder);
    void ManageBookmarks();
    /**
     * @brief open remote file path
     * @param path the remote file path
     */
    void DoOpenFile(const wxString& path);

    wxTreeItemId DoAddFolder(const wxTreeItemId& parent, const wxString& path);
    wxTreeItemId DoAddFile(const wxTreeItemId& parent, const wxString& path);

    MyClientData* GetItemData(const wxTreeItemId& item);
    MyClientDataVect_t GetSelectionsItemData();
    bool DoOpenFile(const wxTreeItemId& item);
    void DoDeleteColumn(int colIdx);
    bool GetAccountFromUser(SSHAccountInfo& account);
    SFTPSessionInfo& GetSession(bool createIfMissing);
    void DoLoadSession();

protected:
    virtual void OnItemActivated(wxTreeEvent& event);
    virtual void OnItemExpanding(wxTreeEvent& event);
    virtual void OnOpenAccountManager(wxCommandEvent& event);
private:
    gsgs::SFTP::Ptr_t m_sftp;
    gsgs::SSHChannel::Ptr_t m_channel;
    gsgs::ImageManager* m_bmpLoader;
    SSHAccountInfo m_account;
    SFTPPlugin * m_plugin;
    wxString m_commandOutput;
    SFTPSessionInfoList m_sessions;
};
#endif // SFTPTREEVIEW_H
