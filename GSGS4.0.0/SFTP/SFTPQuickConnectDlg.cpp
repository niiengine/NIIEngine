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

#include "SFTPQuickConnectDlg.h"
#include "sftp_settings.h"
#include "gsgsWindowManager.h"

SFTPQuickConnectDlg::SFTPQuickConnectDlg(wxWindow* parent)
    : SFTPQuickConnectBaseDlg(parent)
{
    SFTPSettings settings;
    settings.Load();

    // Read previous settings
    bool selecteExistingAccount = true;
    wxString selectedAccount;
    wxString host;
    wxString user;
    wxString port;

    gsgs_Config().Read("SFTPQuickConnect/ChooseExistingAccount", selecteExistingAccount, selecteExistingAccount);
    gsgs_Config().Read("SFTPQuickConnect/SelectedAccount", selectedAccount, selectedAccount);
    gsgs_Config().Read("SFTPQuickConnect/Host", host, wxString());
    gsgs_Config().Read("SFTPQuickConnect/User", user, wxString());
    gsgs_Config().Read("SFTPQuickConnect/Port", port, wxString("22"));

    const SSHAccountInfo::Vect_t& accounts = settings.GetAccounts();
    SSHAccountInfo::Vect_t::const_iterator iter = accounts.begin();
    for(; iter != accounts.end(); ++iter) {
        m_choiceAccount->Append(iter->GetAccountName());
    }
    int where = m_choiceAccount->FindString(selectedAccount);
    if(where != wxNOT_FOUND) { m_choiceAccount->SetSelection(where); }

    m_checkBoxChooseAccount->SetValue(selecteExistingAccount);
    m_checkBoxQuickConnect->SetValue(!selecteExistingAccount);
    m_textCtrlHost->ChangeValue(host);
    m_textCtrlUsername->ChangeValue(user);
    m_textCtrlPort->ChangeValue(port);
    gsgs::WindowManager::Load(this);
}

SFTPQuickConnectDlg::~SFTPQuickConnectDlg()
{
    gsgs_Config().Write("SFTPQuickConnect/ChooseExistingAccount", m_checkBoxChooseAccount->IsChecked());
    gsgs_Config().Write("SFTPQuickConnect/SelectedAccount", m_choiceAccount->GetStringSelection());
    gsgs_Config().Write("SFTPQuickConnect/Host", m_textCtrlHost->GetValue());
    gsgs_Config().Write("SFTPQuickConnect/User", m_textCtrlUsername->GetValue());
    gsgs_Config().Write("SFTPQuickConnect/Port", m_textCtrlPort->GetValue());
}

void SFTPQuickConnectDlg::OnCheckboxChooseAccount(wxCommandEvent& event)
{
    m_checkBoxQuickConnect->SetValue(!event.IsChecked());
}

void SFTPQuickConnectDlg::OnChooseAccountUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxChooseAccount->IsChecked());
}

void SFTPQuickConnectDlg::OnOKUI(wxUpdateUIEvent& event) {}

void SFTPQuickConnectDlg::OnQuickConnect(wxCommandEvent& event)
{
    m_checkBoxChooseAccount->SetValue(!event.IsChecked());
}

void SFTPQuickConnectDlg::OnQuickConnectUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxQuickConnect->IsChecked());
}

SSHAccountInfo SFTPQuickConnectDlg::GetSelectedAccount() const
{
    SSHAccountInfo acc;
    if(m_checkBoxChooseAccount->IsChecked()) {
        SFTPSettings settings;
        settings.Load();
        settings.GetAccount(m_choiceAccount->GetStringSelection(), acc);
    } else {
        // Create a dummy account
        acc.SetUsername(m_textCtrlUsername->GetValue());
        acc.SetAccountName(wxString() << m_textCtrlUsername->GetValue() << "@" << m_textCtrlHost->GetValue());
        acc.SetPassword(m_textCtrlPassword->GetValue());
        acc.SetHost(m_textCtrlHost->GetValue());
        long nPort = 22; // Default is 22
        if(!m_textCtrlPort->GetValue().ToCLong(&nPort)) { nPort = 22; }
        acc.SetPort(nPort);
    }
    return acc;
}
