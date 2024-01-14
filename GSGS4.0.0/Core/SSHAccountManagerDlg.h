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

#ifndef SSHACCOUNTMANAGERDLG_H
#define SSHACCOUNTMANAGERDLG_H

#if USE_SFTP
#include "sftp_ui.h"
#include "ssh_account_info.h"
class _gsgsAPI SSHAccountManagerDlg : public SSHAccountManagerDlgBase
{
public:
    SSHAccountManagerDlg(wxWindow* parent);
    virtual ~SSHAccountManagerDlg();

protected:
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnEditAccountUI(wxUpdateUIEvent& event);
    virtual void OnDeleteAccountUI(wxUpdateUIEvent& event);
    void DoAddAccount(const SSHAccountInfo& account);
    void DoEditAccount(const wxDataViewItem& item);

public:
    SSHAccountInfo::Vect_t GetAccounts() const;

protected:
    virtual void OnAddAccount(wxCommandEvent& event);
    virtual void OnDeleteAccount(wxCommandEvent& event);
    virtual void OnEditAccount(wxCommandEvent& event);
};
#endif // USE_SFTP
#endif // SSHACCOUNTMANAGERDLG_H
