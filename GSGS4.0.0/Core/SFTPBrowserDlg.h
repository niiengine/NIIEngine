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

#ifndef SFTPBROWSERDLG_H
#define SFTPBROWSERDLG_H

#if USE_SFTP

#include "gsgsImageManager.h"
#include "gsgsSFTP.h"
#include "sftp_ui.h"

class SFTPBrowserEntryClientData;
class FloatingTextCtrl;

class _gsgsAPI SFTPBrowserDlg : public SFTPBrowserBaseDlg
{
protected:
    virtual void OnSSHAccountManager(wxCommandEvent& event);
    virtual void OnCdUpUI(wxUpdateUIEvent& event);
    virtual void OnCdUp(wxCommandEvent& event);
    virtual void OnEnter(wxCommandEvent& event);
    virtual void OnFocusLost(wxFocusEvent& event);
    virtual void OnTextUpdated(wxCommandEvent& event);
    virtual void OnKeyDown(wxKeyEvent& event);
    virtual void OnItemSelected(wxDataViewEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnTextEnter(wxCommandEvent& event);
    virtual void OnItemActivated(wxDataViewEvent& event);
    void DoCloseSession();
    void DoDisplayEntriesForPath(const wxString& path = "");
    void ClearView();
    SFTPBrowserEntryClientData* DoGetItemData(const wxDataViewItem& item) const;

public:
    SFTPBrowserDlg(wxWindow* parent, const wxString& title, const wxString& filter,
                   size_t flags = gsgs::SFTP::SFTP_BROWSE_FILES | gsgs::SFTP::SFTP_BROWSE_FOLDERS);
    virtual ~SFTPBrowserDlg();

    void Initialize(const wxString& account, const wxString& path);
    wxString GetPath() const;
    wxString GetAccount() const;

    void OnInlineSearch();
    void OnInlineSearchEnter();
protected:
    virtual void OnRefresh(wxCommandEvent& event);
    virtual void OnRefreshUI(wxUpdateUIEvent& event);
private:
    gsgs::SFTP::Ptr_t m_sftp;
    wxString m_filter;
    size_t m_flags;
};
#endif // USE_SFTP

#endif // SFTPBROWSERDLG_H
