//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SSHAccountManagerDlg.cpp
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

#include "SSHAccountManagerDlg.h"
#if USE_SFTP

#include "gsgsCommonDialog.h"
#include "globals.h"
#include "sftp_settings.h"
#include "ssh_account_info.h"
#include "gsgsWindowManager.h"
#include "sftp_ui.h"
#include "gsgsSSH.h"
#include "gsgsException.h"

class _gsgsAPI AddSSHAcountDlg : public AddSSHAcountDlgBase
{
public:
    AddSSHAcountDlg(wxWindow* parent);
    AddSSHAcountDlg(wxWindow* parent, const SSHAccountInfo &account);
    virtual ~AddSSHAcountDlg();

public:
    void GetAccountInfo(SSHAccountInfo& info);

protected:
    virtual void OnHomeFolderUpdated(wxCommandEvent& event);
    virtual void OnTestConnectionUI(wxUpdateUIEvent& event);
    virtual void OnTestConnection(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};

AddSSHAcountDlg::AddSSHAcountDlg(wxWindow* parent)
    : AddSSHAcountDlgBase(parent)
{
    SetName("AddSSHAcountDlg");
    gsgs::WindowManager::Load(this);
}

AddSSHAcountDlg::AddSSHAcountDlg(wxWindow* parent, const SSHAccountInfo& account)
    : AddSSHAcountDlgBase(parent)
{
    m_textCtrlHost->ChangeValue(account.GetHost());
    m_textCtrlPassword->ChangeValue(account.GetPassword());
    m_textCtrlPort->ChangeValue(wxString() << account.GetPort());
    m_textCtrlUsername->ChangeValue(account.GetUsername());
    m_textCtrlName->ChangeValue(account.GetAccountName());
    m_textCtrlHomeFolder->ChangeValue(account.GetDefaultFolder());
    SetName("AddSSHAcountDlg");
    gsgs::WindowManager::Load(this);
}

AddSSHAcountDlg::~AddSSHAcountDlg() {}

void AddSSHAcountDlg::OnOKUI(wxUpdateUIEvent& event)
{
    wxString homeFolder = m_textCtrlHomeFolder->GetValue();
    bool homeFolderValid = (homeFolder.IsEmpty() || (homeFolder.StartsWith("/")));

    event.Enable(!m_textCtrlHost->IsEmpty() && !m_textCtrlPort->IsEmpty() && !m_textCtrlUsername->IsEmpty() &&
                 homeFolderValid);
}

void AddSSHAcountDlg::GetAccountInfo(SSHAccountInfo& info)
{
    info.SetHost(m_textCtrlHost->GetValue());
    info.SetPassword(m_textCtrlPassword->GetValue());

    long port = 22;
    m_textCtrlPort->GetValue().ToCLong(&port);
    info.SetPort(port);
    info.SetUsername(m_textCtrlUsername->GetValue());
    info.SetAccountName(m_textCtrlName->GetValue());
    info.SetDefaultFolder(m_textCtrlHomeFolder->GetValue());
}

void AddSSHAcountDlg::OnTestConnection(wxCommandEvent& event)
{
    SSHAccountInfo account;
    GetAccountInfo(account);
    gsgs::SSH::Ptr_t ssh(new gsgs::SSH(account.GetHost(), account.GetUsername(), account.GetPassword(), account.GetPort()));

    try {
        wxString message;
        ssh->Connect();
        if(!ssh->AuthenticateServer(message)) {
            if(::wxMessageBox(message, "SSH", wxYES_NO | wxCENTER | wxICON_QUESTION, this) == wxYES) {
                ssh->AcceptServerAuthentication();
            }
        }

        // Try the login methods:
        ssh->Login();
        ::wxMessageBox("Successfully connected to host!");

    } catch(gsgs::Exception& e) {
        ::wxMessageBox(e.What(), "SSH", wxICON_WARNING | wxOK, this);
    }
}
void AddSSHAcountDlg::OnTestConnectionUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlHost->IsEmpty() && !m_textCtrlPort->IsEmpty() && !m_textCtrlUsername->IsEmpty());
}
void AddSSHAcountDlg::OnHomeFolderUpdated(wxCommandEvent& event)
{
    wxString homeFolder = m_textCtrlHomeFolder->GetValue();
    if(!homeFolder.StartsWith("/")) {
        m_infobar->ShowMessage(_("Default folder must be set to full path (i.e. it should start with a '/')"),
                               wxICON_WARNING);
    }
}

SSHAccountManagerDlg::SSHAccountManagerDlg(wxWindow* parent)
    : SSHAccountManagerDlgBase(parent)
{
    SFTPSettings settings;
    settings.Load();

    const SSHAccountInfo::Vect_t& accounts = settings.GetAccounts();
    SSHAccountInfo::Vect_t::const_iterator iter = accounts.begin();
    for(; iter != accounts.end(); ++iter) {
        DoAddAccount(*iter);
    }
    SetName("SSHAccountManagerDlg");
    gsgs::WindowManager::Load(this);
}

SSHAccountManagerDlg::~SSHAccountManagerDlg()
{
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrl->RowToItem(i);
        SSHAccountInfo* pAccount = (SSHAccountInfo*)(m_dvListCtrl->GetItemData(item));
        delete pAccount;
        m_dvListCtrl->SetItemData(item, (wxUIntPtr)NULL);
    }
    m_dvListCtrl->DeleteAllItems();
}

void SSHAccountManagerDlg::OnAddAccount(wxCommandEvent& event)
{
    AddSSHAcountDlg dlg(this);
    if(dlg.ShowModal() == wxID_OK) {
        SSHAccountInfo account;
        dlg.GetAccountInfo(account);
        DoAddAccount(account);
    }
}

void SSHAccountManagerDlg::OnDeleteAccount(wxCommandEvent& event)
{
    wxDataViewItemArray sels;
    m_dvListCtrl->GetSelections(sels);

    if(::wxMessageBox(_("Are you sure you want to delete the selected accounts?"), "SFTP",
                      wxYES_NO | wxCENTER | wxCANCEL | wxICON_QUESTION | wxNO_DEFAULT) != wxYES) {
        return;
    }

    for(size_t i = 0; i < sels.GetCount(); ++i) {
        wxDataViewItem item = sels.Item(i);
        m_dvListCtrl->DeleteItem(m_dvListCtrl->ItemToRow(item));
    }

    m_dvListCtrl->Refresh();
}

void SSHAccountManagerDlg::OnEditAccount(wxCommandEvent& event)
{
    wxDataViewItemArray sels;
    m_dvListCtrl->GetSelections(sels);
    if(sels.GetCount() == 1) { DoEditAccount(sels.Item(0)); }
}

void SSHAccountManagerDlg::DoAddAccount(const SSHAccountInfo& account)
{
    wxVector<wxVariant> cols;
    cols.push_back(account.GetAccountName());
    cols.push_back(account.GetHost());
    cols.push_back(account.GetUsername());
    m_dvListCtrl->AppendItem(cols, (wxUIntPtr)(new SSHAccountInfo(account)));
}

SSHAccountInfo::Vect_t SSHAccountManagerDlg::GetAccounts() const
{
    SSHAccountInfo::Vect_t accounts;
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        accounts.push_back(*(SSHAccountInfo*)(m_dvListCtrl->GetItemData(m_dvListCtrl->RowToItem(i))));
    }
    return accounts;
}

void SSHAccountManagerDlg::OnDeleteAccountUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrl->GetSelectedItemsCount());
}

void SSHAccountManagerDlg::OnEditAccountUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrl->GetSelectedItemsCount());
}

void SSHAccountManagerDlg::OnItemActivated(wxDataViewEvent& event) { DoEditAccount(event.GetItem()); }

void SSHAccountManagerDlg::DoEditAccount(const wxDataViewItem& item)
{
    SSHAccountInfo* account = (SSHAccountInfo*)(m_dvListCtrl->GetItemData(item));
    if(account) {
        AddSSHAcountDlg dlg(this, *account);
        if(dlg.ShowModal() == wxID_OK) {
            // update the user info
            dlg.GetAccountInfo(*account);
            // update the UI
            m_dvListCtrl->SetItemText(item, account->GetAccountName(), 0);
            m_dvListCtrl->SetItemText(item, account->GetHost(), 1);
            m_dvListCtrl->SetItemText(item, account->GetUsername(), 2);
            m_dvListCtrl->Refresh();
        }
    }
}
#endif // USE_SFTP
