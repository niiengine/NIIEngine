
#if USE_SFTP
#include "SFTPBrowserDlg.h"
#include "SSHAccountManagerDlg.h"
#include "gsgsFileUtil.h"
#include "globals.h"
#include "imanager.h"
#include "sftp_settings.h"
#include "ssh_account_info.h"
#include "gsgsWindowManager.h"

// ================================================================================
// ================================================================================

class SFTPBrowserEntryClientData : public wxClientData
{
    SFTPAttribute::Ptr_t m_attribute;
    wxString m_fullpath;

public:
    SFTPBrowserEntryClientData(SFTPAttribute::Ptr_t attr, const wxString& fullpath)
        : m_attribute(attr)
        , m_fullpath(fullpath)
    {
        wxFileName fn;
        if(m_attribute->IsFolder()) {
            fn = wxFileName(fullpath, "", wxPATH_UNIX);
            fn.Normalize();
            m_fullpath = fn.GetPath(false, wxPATH_UNIX);
        } else {
            fn = wxFileName(fullpath, wxPATH_UNIX);
            fn.Normalize();
            m_fullpath = fn.GetFullPath(wxPATH_UNIX);
        }
    }

    virtual ~SFTPBrowserEntryClientData() {}

    const wxString& GetDisplayName() const { return GetAttribute()->GetName(); }
    void SetAttribute(const SFTPAttribute::Ptr_t& attribute) { this->m_attribute = attribute; }
    void SetFullpath(const wxString& fullpath) { this->m_fullpath = fullpath; }
    const SFTPAttribute::Ptr_t& GetAttribute() const { return m_attribute; }
    const wxString& GetFullpath() const { return m_fullpath; }
};

// ================================================================================
// ================================================================================

SFTPBrowserDlg::SFTPBrowserDlg(wxWindow* parent, const wxString& title, const wxString& filter, size_t flags)
    : SFTPBrowserBaseDlg(parent)
    , m_sftp(NULL)
    , m_filter(filter)
    , m_flags(flags)
{
    SetLabel(title);
    SFTPSettings settings;
    settings.Load();
    m_dataview->SetBitmaps(&gsgs_Image().GetBitmaps());
    const SSHAccountInfo::Vect_t& accounts = settings.GetAccounts();
    for(const auto& account : accounts) {
        m_choiceAccount->Append(account.GetAccountName());
    }

    m_toolbar->AddTool(XRCID("ID_CD_UP"), _("Parent Folder"), gsgs_Image().LoadBitmap("up"));
    m_toolbar->AddTool(XRCID("ID_SSH_ACCOUNT_MANAGER"), _("Open SSH Account Manager"),
                       gsgs_Image().LoadBitmap("folder-users"));
    m_toolbar->Realize();
    m_toolbar->Bind(wxEVT_TOOL, &SFTPBrowserDlg::OnCdUp, this, XRCID("ID_CD_UP"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &SFTPBrowserDlg::OnCdUpUI, this, XRCID("ID_CD_UP"));
    m_toolbar->Bind(wxEVT_TOOL, &SFTPBrowserDlg::OnSSHAccountManager, this, XRCID("ID_SSH_ACCOUNT_MANAGER"));

    if(!m_choiceAccount->IsEmpty()) { m_choiceAccount->SetSelection(0); }
    SetName("SFTPBrowserDlg");
    gsgs::WindowManager::Load(this);
}

SFTPBrowserDlg::~SFTPBrowserDlg() {}

void SFTPBrowserDlg::OnRefresh(wxCommandEvent& event)
{
    DoCloseSession();
    wxString accountName = m_choiceAccount->GetStringSelection();

    SFTPSettings settings;
    settings.Load();

    SSHAccountInfo account;
    if(!settings.GetAccount(accountName, account)) {
        ::wxMessageBox(wxString() << _("Could not find account: ") << accountName, "gsgs", wxICON_ERROR | wxOK,
                       this);
        return;
    }

    gsgs::SSH::Ptr_t ssh(new gsgs::SSH(account.GetHost(), account.GetUsername(), account.GetPassword(), account.GetPort()));
    try {
        wxString message;
        ssh->Connect();
        if(!ssh->AuthenticateServer(message)) {
            if(::wxMessageBox(message, "SSH", wxYES_NO | wxCENTER | wxICON_QUESTION, this) == wxYES) {
                ssh->AcceptServerAuthentication();
            }
        }

        ssh->Login();
        m_sftp.reset(new gsgs::SFTP(ssh));
        m_sftp->Initialize();

        DoDisplayEntriesForPath();

    } catch(gsgs::Exception& e) {
        ::wxMessageBox(e.What(), "gsgs", wxICON_ERROR | wxOK, this);
        DoCloseSession();
    }
}

void SFTPBrowserDlg::OnRefreshUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlRemoteFolder->IsEmpty()); }

void SFTPBrowserDlg::DoDisplayEntriesForPath(const wxString& path)
{
    try {
        wxString folder;
        SFTPAttribute::List_t attributes;
        if(path.IsEmpty()) {
            folder = m_textCtrlRemoteFolder->GetValue();
            attributes = m_sftp->List(folder, m_flags, m_filter);

        } else if(path == "..") {
            // cd up
            attributes = m_sftp->CdUp(m_flags, m_filter);
            m_textCtrlRemoteFolder->ChangeValue(m_sftp->GetCurrentFolder());
            folder = m_sftp->GetCurrentFolder();

        } else {
            folder = path;
            attributes = m_sftp->List(folder, m_flags, m_filter);
        }

        SFTPAttribute::List_t::iterator iter = attributes.begin();
        for(; iter != attributes.end(); ++iter) {

            // Set the columns Name (icontext) | Type (text) | Size (text)
            wxVector<wxVariant> cols;

            // determine the bitmap type
            gsgs::ImageManager* loader = &gsgs_Image();
            int imgid = loader->GetMimeImageId(FileUtil::TypeText);
            wxString fullname;
            fullname << folder << "/" << (*iter)->GetName();

            if((*iter)->IsFolder()) {
                imgid = loader->GetMimeImageId(FileUtil::TypeFolder);
            } else {
                wxFileName fn(fullname);
                imgid = loader->GetMimeImageId(fn.GetFullName());
            }

            cols.push_back(::MakeBitmapIndexText((*iter)->GetName(), imgid));
            cols.push_back((*iter)->GetTypeAsString());
            cols.push_back(wxString() << (*iter)->GetSize());

            SFTPBrowserEntryClientData* cd = new SFTPBrowserEntryClientData((*iter), fullname);
            m_dataview->AppendItem(cols, (wxUIntPtr)cd);
        }
        m_dataview->SetFocus();

    } catch(gsgs::Exception& e) {
        ::wxMessageBox(e.What(), "SFTP", wxICON_ERROR | wxOK);
        DoCloseSession();
    }
}

void SFTPBrowserDlg::DoCloseSession()
{
    m_sftp.reset(NULL);
    ClearView();
}

void SFTPBrowserDlg::OnItemActivated(wxDataViewEvent& event)
{
    if(!m_sftp) {
        DoCloseSession();
        return;
    }

    SFTPBrowserEntryClientData* cd = DoGetItemData(event.GetItem());
    if(cd && cd->GetAttribute()->IsFolder()) {
        m_textCtrlRemoteFolder->ChangeValue(cd->GetFullpath());
        ClearView();
        DoDisplayEntriesForPath();
    }
}

void SFTPBrowserDlg::OnTextEnter(wxCommandEvent& event)
{
    if(!m_sftp) {
        OnRefresh(event);
    } else {
        ClearView();
        DoDisplayEntriesForPath();
    }
}
void SFTPBrowserDlg::OnOKUI(wxUpdateUIEvent& event)
{
    wxString selection = m_textCtrlRemoteFolder->GetValue();
    if(m_filter.IsEmpty()) { // folder / files - and there is a selection
        event.Enable(!selection.IsEmpty());

    } else if(!(m_flags & gsgs::SFTP::SFTP_BROWSE_FILES)) { // folders only
        event.Enable(!selection.IsEmpty());

    } else if(!selection.IsEmpty() && (m_flags & gsgs::SFTP::SFTP_BROWSE_FILES) && ::wxMatchWild(m_filter, selection)) {
        event.Enable(true);

    } else {
        event.Enable(false);
    }
}

SFTPBrowserEntryClientData* SFTPBrowserDlg::DoGetItemData(const wxDataViewItem& item) const
{
    if(!item.IsOk()) { return NULL; }
    SFTPBrowserEntryClientData* cd = reinterpret_cast<SFTPBrowserEntryClientData*>(m_dataview->GetItemData(item));
    return cd;
}

wxString SFTPBrowserDlg::GetPath() const { return m_textCtrlRemoteFolder->GetValue(); }

void SFTPBrowserDlg::OnItemSelected(wxDataViewEvent& event)
{
    SFTPBrowserEntryClientData* cd = DoGetItemData(m_dataview->GetSelection());
    if(cd) { m_textCtrlRemoteFolder->ChangeValue(cd->GetFullpath()); }
}

wxString SFTPBrowserDlg::GetAccount() const { return m_choiceAccount->GetStringSelection(); }

void SFTPBrowserDlg::Initialize(const wxString& account, const wxString& path)
{
    m_textCtrlRemoteFolder->ChangeValue(path);
    int where = m_choiceAccount->FindString(account);
    if(where != wxNOT_FOUND) { m_choiceAccount->SetSelection(where); }
}

void SFTPBrowserDlg::OnKeyDown(wxKeyEvent& event)
{
    event.Skip();
    wxChar ch = (wxChar)event.GetKeyCode();

    if(::wxIsprint(ch)) {
        if(!m_textCtrlInlineSearch->IsShown()) {
            m_textCtrlInlineSearch->SetFocus();
            m_textCtrlInlineSearch->Clear();
#ifdef __WXMSW__
            m_textCtrlInlineSearch->ChangeValue(wxString() << (wxChar)event.GetKeyCode());
#endif
            m_textCtrlInlineSearch->SetInsertionPoint(m_textCtrlInlineSearch->GetLastPosition());
            m_textCtrlInlineSearch->Show();
            GetSizer()->Layout();
        }
    }
}

void SFTPBrowserDlg::OnInlineSearch()
{
    wxString text = m_textCtrlInlineSearch->GetValue();
    wxDataViewItem matchedItem = m_dataview->FindNext(wxDataViewItem(nullptr), text);
    if(matchedItem.IsOk()) {
        m_dataview->Select(matchedItem);
        m_dataview->EnsureVisible(matchedItem);
    }
}

void SFTPBrowserDlg::OnInlineSearchEnter()
{
    wxDataViewItem item = m_dataview->GetSelection();
    if(!item.IsOk()) { return; }

    SFTPBrowserEntryClientData* cd = DoGetItemData(item);
    if(cd && cd->GetAttribute()->IsFolder()) {
        m_textCtrlRemoteFolder->ChangeValue(cd->GetFullpath());
        ClearView();
        DoDisplayEntriesForPath();
        m_dataview->SetFocus();
    }
}
void SFTPBrowserDlg::OnEnter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    CallAfter(&SFTPBrowserDlg::OnInlineSearchEnter);
}

void SFTPBrowserDlg::OnFocusLost(wxFocusEvent& event)
{
    event.Skip();
    m_textCtrlInlineSearch->Hide();
    GetSizer()->Layout();
}

void SFTPBrowserDlg::OnTextUpdated(wxCommandEvent& event)
{
    wxUnusedVar(event);
    CallAfter(&SFTPBrowserDlg::OnInlineSearch);
}

void SFTPBrowserDlg::OnCdUp(wxCommandEvent& event)
{
    ClearView();
    DoDisplayEntriesForPath("..");
}

void SFTPBrowserDlg::OnCdUpUI(wxUpdateUIEvent& event) { event.Enable(m_sftp); }

void SFTPBrowserDlg::OnSSHAccountManager(wxCommandEvent& event)
{
    SSHAccountManagerDlg dlg(this);
    if(dlg.ShowModal() == wxID_OK) {

        SFTPSettings settings;
        settings.Load().SetAccounts(dlg.GetAccounts());
        settings.Save();

        // Update the selections at the top
        wxString curselection = m_choiceAccount->GetStringSelection();

        m_choiceAccount->Clear();
        const SSHAccountInfo::Vect_t& accounts = settings.GetAccounts();
        if(accounts.empty()) {
            DoCloseSession();
            return;

        } else {
            SSHAccountInfo::Vect_t::const_iterator iter = accounts.begin();
            for(; iter != accounts.end(); ++iter) {
                m_choiceAccount->Append(iter->GetAccountName());
            }

            int where = m_choiceAccount->FindString(curselection);
            if(where == wxNOT_FOUND) {
                // Our previous session is no longer available, close the session
                DoCloseSession();
                where = 0;
            }

            m_choiceAccount->SetSelection(where);
        }
    }
}

void SFTPBrowserDlg::ClearView()
{
    for(size_t i = 0; i < m_dataview->GetItemCount(); ++i) {
        SFTPBrowserEntryClientData* cd = DoGetItemData(m_dataview->RowToItem(i));
        wxDELETE(cd);
    }
    m_dataview->DeleteAllItems();
}

#endif // USE_SFTP