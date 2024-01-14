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

#ifndef SFTPQUICKCONNECTDLG_H
#define SFTPQUICKCONNECTDLG_H
#include "UI.h"
#include "ssh_account_info.h"

class SFTPQuickConnectDlg : public SFTPQuickConnectBaseDlg
{
public:
    SFTPQuickConnectDlg(wxWindow* parent);
    virtual ~SFTPQuickConnectDlg();
    SSHAccountInfo GetSelectedAccount() const;

protected:
    virtual void OnCheckboxChooseAccount(wxCommandEvent& event);
    virtual void OnChooseAccountUI(wxUpdateUIEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnQuickConnect(wxCommandEvent& event);
    virtual void OnQuickConnectUI(wxUpdateUIEvent& event);
};
#endif // SFTPQUICKCONNECTDLG_H
