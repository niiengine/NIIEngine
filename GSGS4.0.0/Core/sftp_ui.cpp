//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: sftp_ui.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "sftp_ui.h"
#include "gsgsPreInclude.h"

// Declare the bitmap loading function
extern void wxCE8CInitBitmapResources();

static bool bBitmapLoaded = false;

SSHAccountManagerDlgBase::SSHAccountManagerDlgBase(wxWindow* parent, wxWindowID id, const wxString& title,
                                                   const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if(!bBitmapLoaded) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCE8CInitBitmapResources();
        bBitmapLoaded = true;
    }

    wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer2);

    wxBoxSizer* boxSizer10 = new wxBoxSizer(wxHORIZONTAL);

    boxSizer2->Add(boxSizer10, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_dvListCtrl = new gsgs::ThemedListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
                                        wxDV_ROW_LINES | wxDV_SINGLE);

    boxSizer10->Add(m_dvListCtrl, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_dvListCtrl->AppendTextColumn(_("Account"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                   wxDATAVIEW_COL_RESIZABLE);
    m_dvListCtrl->AppendTextColumn(_("Host"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                   wxDATAVIEW_COL_RESIZABLE);
    m_dvListCtrl->AppendTextColumn(_("User"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                   wxDATAVIEW_COL_RESIZABLE);
    wxBoxSizer* boxSizer15 = new wxBoxSizer(wxVERTICAL);

    boxSizer10->Add(boxSizer15, 0, wxEXPAND, WXC_FROM_DIP(5));

    m_buttonNew = new Button(this, wxID_ADD, _("&Add"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonNew->SetToolTip(_("Add new account"));

    boxSizer15->Add(m_buttonNew, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_buttonDelete =
        new Button(this, wxID_DELETE, _("&Delete"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonDelete->SetToolTip(_("Delete the selected accounts"));

    boxSizer15->Add(m_buttonDelete, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_buttonEdit = new Button(this, wxID_EDIT, _("&Edit"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonEdit->SetToolTip(_("Edit the selected account"));

    boxSizer15->Add(m_buttonEdit, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* boxSizer4 = new wxBoxSizer(wxHORIZONTAL);

    boxSizer2->Add(boxSizer4, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

    m_button6 = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_button6->SetDefault();

    boxSizer4->Add(m_button6, 0, wxALL, WXC_FROM_DIP(5));

    m_button8 = new Button(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    boxSizer4->Add(m_button8, 0, wxALL, WXC_FROM_DIP(5));

    SetName(wxT("SSHAccountManagerDlgBase"));
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    if(GetSizer()) { GetSizer()->Fit(this); }
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }
#if wxVERSION_NUMBER >= 2900
    if(!wxPersistenceManager::Get().Find(this)) {
        wxPersistenceManager::Get().RegisterAndRestore(this);
    } else {
        wxPersistenceManager::Get().Restore(this);
    }
#endif
    // Connect events
    m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
                          wxDataViewEventHandler(SSHAccountManagerDlgBase::OnItemActivated), NULL, this);
    m_buttonNew->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SSHAccountManagerDlgBase::OnAddAccount),
                         NULL, this);
    m_buttonDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                            wxCommandEventHandler(SSHAccountManagerDlgBase::OnDeleteAccount), NULL, this);
    m_buttonDelete->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SSHAccountManagerDlgBase::OnDeleteAccountUI), NULL,
                            this);
    m_buttonEdit->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SSHAccountManagerDlgBase::OnEditAccountUI), NULL,
                          this);
    m_buttonEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SSHAccountManagerDlgBase::OnEditAccount),
                          NULL, this);
}

SSHAccountManagerDlgBase::~SSHAccountManagerDlgBase()
{
    m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
                             wxDataViewEventHandler(SSHAccountManagerDlgBase::OnItemActivated), NULL, this);
    m_buttonNew->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SSHAccountManagerDlgBase::OnAddAccount),
                            NULL, this);
    m_buttonDelete->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
                               wxCommandEventHandler(SSHAccountManagerDlgBase::OnDeleteAccount), NULL, this);
    m_buttonDelete->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SSHAccountManagerDlgBase::OnDeleteAccountUI),
                               NULL, this);
    m_buttonEdit->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SSHAccountManagerDlgBase::OnEditAccountUI), NULL,
                             this);
    m_buttonEdit->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
                             wxCommandEventHandler(SSHAccountManagerDlgBase::OnEditAccount), NULL, this);
}

AddSSHAcountDlgBase::AddSSHAcountDlgBase(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
                                         const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if(!bBitmapLoaded) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCE8CInitBitmapResources();
        bBitmapLoaded = true;
    }

    wxBoxSizer* boxSizer23 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer23);

    m_infobar = new wxInfoBar(this, wxID_ANY);
    m_infobar->SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));

    boxSizer23->Add(m_infobar, 0, wxEXPAND, WXC_FROM_DIP(5));

    wxFlexGridSizer* flexGridSizer31 = new wxFlexGridSizer(0, 2, 0, 0);
    flexGridSizer31->SetFlexibleDirection(wxBOTH);
    flexGridSizer31->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer31->AddGrowableCol(1);

    boxSizer23->Add(flexGridSizer31, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText98 =
        gsgs_Style().createStaticText(this, wxID_ANY, _("Account Name:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_staticText98->SetToolTip(_("Give this account a unique name"));

    flexGridSizer31->Add(m_staticText98, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textCtrlName = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(250, -1)), 0);
    m_textCtrlName->SetToolTip(_("Give this account a unique name"));

    flexGridSizer31->Add(m_textCtrlName, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticHostIp =
        gsgs_Style().createStaticText(this, wxID_ANY, _("Host / IP:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    flexGridSizer31->Add(m_staticHostIp, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textCtrlHost = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_textCtrlHost->SetToolTip(_("The remote host ip address or its known name"));

    flexGridSizer31->Add(m_textCtrlHost, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText45 =
        gsgs_Style().createStaticText(this, wxID_ANY, _("Port:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    flexGridSizer31->Add(m_staticText45, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textCtrlPort = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT("22"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_textCtrlPort->SetToolTip(_("The SSH port. If you don't know it, leave it as 22 (SSH default port)"));

    flexGridSizer31->Add(m_textCtrlPort, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText37 =
        gsgs_Style().createStaticText(this, wxID_ANY, _("Username:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    flexGridSizer31->Add(m_staticText37, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textCtrlUsername = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_textCtrlUsername->SetToolTip(_("Login user name"));

    flexGridSizer31->Add(m_textCtrlUsername, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText41 =
        gsgs_Style().createStaticText(this, wxID_ANY, _("Password:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    flexGridSizer31->Add(m_staticText41, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textCtrlPassword = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTE_PASSWORD);
    m_textCtrlPassword->SetToolTip(_("This field is optional. By leaving this field empty, gsgs will attempt to "
                                     "connect only using public key authentication"));

    flexGridSizer31->Add(m_textCtrlPassword, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText110 =
        gsgs_Style().createStaticText(this, wxID_ANY, _("Default folder:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    flexGridSizer31->Add(m_staticText110, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textCtrlHomeFolder = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_textCtrlHomeFolder->SetToolTip(_("Set the home folder for this account"));

    flexGridSizer31->Add(m_textCtrlHomeFolder, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_button51 =
        new Button(this, wxID_ANY, _("Test Connection"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
#if wxVERSION_NUMBER >= 2904
    m_button51->SetBitmap(wxXmlResource::Get()->LoadBitmap(wxT("16-connected")));
    //m_button51->SetBitmapMargins(5, 5);
#endif

    boxSizer23->Add(m_button51, 0, wxALL | wxEXPAND, WXC_FROM_DIP(10));

    boxSizer23->Add(0, 0, 1, wxALL, WXC_FROM_DIP(5));

    wxBoxSizer* boxSizer29 = new wxBoxSizer(wxHORIZONTAL);

    boxSizer23->Add(boxSizer29, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

    m_button27 = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_button27->SetDefault();

    boxSizer29->Add(m_button27, 0, wxALL, WXC_FROM_DIP(5));

    m_button25 = new Button(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    boxSizer29->Add(m_button25, 0, wxALL, WXC_FROM_DIP(5));

    SetName(wxT("AddSSHAcountDlgBase"));
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    if(GetSizer()) { GetSizer()->Fit(this); }
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }
#if wxVERSION_NUMBER >= 2900
    if(!wxPersistenceManager::Get().Find(this)) {
        wxPersistenceManager::Get().RegisterAndRestore(this);
    } else {
        wxPersistenceManager::Get().Restore(this);
    }
#endif
    // Connect events
    m_textCtrlHomeFolder->Connect(wxEVT_COMMAND_TEXT_UPDATED,
                                  wxCommandEventHandler(AddSSHAcountDlgBase::OnHomeFolderUpdated), NULL, this);
    m_button51->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AddSSHAcountDlgBase::OnTestConnection),
                        NULL, this);
    m_button51->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(AddSSHAcountDlgBase::OnTestConnectionUI), NULL, this);
    m_button27->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(AddSSHAcountDlgBase::OnOKUI), NULL, this);
}

AddSSHAcountDlgBase::~AddSSHAcountDlgBase()
{
    m_textCtrlHomeFolder->Disconnect(wxEVT_COMMAND_TEXT_UPDATED,
                                     wxCommandEventHandler(AddSSHAcountDlgBase::OnHomeFolderUpdated), NULL, this);
    m_button51->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AddSSHAcountDlgBase::OnTestConnection),
                           NULL, this);
    m_button51->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(AddSSHAcountDlgBase::OnTestConnectionUI), NULL,
                           this);
    m_button27->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(AddSSHAcountDlgBase::OnOKUI), NULL, this);
}

SFTPBrowserBaseDlg::SFTPBrowserBaseDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
                                       const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if(!bBitmapLoaded) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCE8CInitBitmapResources();
        bBitmapLoaded = true;
    }

    wxBoxSizer* boxSizer62 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer62);

    wxGridBagSizer* gridBagSizer80 = new wxGridBagSizer(0, 0);

    boxSizer62->Add(gridBagSizer80, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText66 =
        gsgs_Style().createStaticText(this, wxID_ANY, _("Path:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    gridBagSizer80->Add(m_staticText66, wxGBPosition(1, 0), wxGBSpan(1, 1),
                        wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textCtrlRemoteFolder = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT("/"), wxDefaultPosition,
                                            wxDLG_UNIT(this, wxSize(-1, -1)), wxTE_PROCESS_ENTER);
    m_textCtrlRemoteFolder->SetToolTip(_("Set the remote folder to browse and click on the 'Refresh' button"));

    gridBagSizer80->Add(m_textCtrlRemoteFolder, wxGBPosition(1, 1), wxGBSpan(1, 1),
                        wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_buttonRefresh =
        new Button(this, wxID_REFRESH, _("Connect..."), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    gridBagSizer80->Add(m_buttonRefresh, wxGBPosition(1, 2), wxGBSpan(1, 1), wxALL | wxALIGN_CENTER_VERTICAL,
                        WXC_FROM_DIP(5));

    m_staticText82 =
        gsgs_Style().createStaticText(this, wxID_ANY, _("Account:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    gridBagSizer80->Add(m_staticText82, wxGBPosition(0, 0), wxGBSpan(1, 1),
                        wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    wxArrayString m_choiceAccountArr;
    m_choiceAccount =
        new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), m_choiceAccountArr, 0);

    gridBagSizer80->Add(m_choiceAccount, wxGBPosition(0, 1), wxGBSpan(1, 2), wxALL | wxEXPAND, WXC_FROM_DIP(5));
    gridBagSizer80->AddGrowableCol(1);
    m_toolbar =
        new gsgs::ToolBar(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTB_NODIVIDER | wxTB_FLAT);
    m_toolbar->SetToolBitmapSize(wxSize(16, 16));

    boxSizer62->Add(m_toolbar, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_dataview = new gsgs::ThemedListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(500, 300)),
                                      wxDV_ROW_LINES | wxDV_SINGLE | wxBORDER_THEME);

    boxSizer62->Add(m_dataview, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_dataview->AppendIconTextColumn(_("Name"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                     wxDATAVIEW_COL_RESIZABLE);
    m_dataview->AppendTextColumn(_("Type"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                 wxDATAVIEW_COL_RESIZABLE);
    m_dataview->AppendTextColumn(_("Size"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                 wxDATAVIEW_COL_RESIZABLE);
    wxBoxSizer* boxSizer94 = new wxBoxSizer(wxVERTICAL);

    boxSizer62->Add(boxSizer94, 0, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, WXC_FROM_DIP(5));

    m_textCtrlInlineSearch = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition,
                                            wxDLG_UNIT(this, wxSize(-1, -1)), wxTE_PROCESS_ENTER);
    m_textCtrlInlineSearch->Hide();

    boxSizer94->Add(m_textCtrlInlineSearch, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* boxSizer57 = new wxBoxSizer(wxHORIZONTAL);

    boxSizer62->Add(boxSizer57, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

    m_button59 = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_button59->SetDefault();

    boxSizer57->Add(m_button59, 0, wxALL, WXC_FROM_DIP(5));

    m_button61 = new Button(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    boxSizer57->Add(m_button61, 0, wxALL, WXC_FROM_DIP(5));

    SetName(wxT("SFTPBrowserBaseDlg"));
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    if(GetSizer()) { GetSizer()->Fit(this); }
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }
#if wxVERSION_NUMBER >= 2900
    if(!wxPersistenceManager::Get().Find(this)) {
        wxPersistenceManager::Get().RegisterAndRestore(this);
    } else {
        wxPersistenceManager::Get().Restore(this);
    }
#endif
    // Connect events
    m_textCtrlRemoteFolder->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(SFTPBrowserBaseDlg::OnTextEnter),
                                    NULL, this);
    m_buttonRefresh->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SFTPBrowserBaseDlg::OnRefresh), NULL,
                             this);
    m_buttonRefresh->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SFTPBrowserBaseDlg::OnRefreshUI), NULL, this);
    m_dataview->Connect(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED,
                        wxDataViewEventHandler(SFTPBrowserBaseDlg::OnItemSelected), NULL, this);
    m_dataview->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
                        wxDataViewEventHandler(SFTPBrowserBaseDlg::OnItemActivated), NULL, this);
    m_textCtrlInlineSearch->Connect(wxEVT_COMMAND_TEXT_UPDATED,
                                    wxCommandEventHandler(SFTPBrowserBaseDlg::OnTextUpdated), NULL, this);
    m_textCtrlInlineSearch->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(SFTPBrowserBaseDlg::OnEnter), NULL,
                                    this);
    m_textCtrlInlineSearch->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(SFTPBrowserBaseDlg::OnFocusLost), NULL, this);
    m_button59->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SFTPBrowserBaseDlg::OnOKUI), NULL, this);
}

SFTPBrowserBaseDlg::~SFTPBrowserBaseDlg()
{
    m_textCtrlRemoteFolder->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(SFTPBrowserBaseDlg::OnTextEnter),
                                       NULL, this);
    m_buttonRefresh->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SFTPBrowserBaseDlg::OnRefresh),
                                NULL, this);
    m_buttonRefresh->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SFTPBrowserBaseDlg::OnRefreshUI), NULL, this);
    m_dataview->Disconnect(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED,
                           wxDataViewEventHandler(SFTPBrowserBaseDlg::OnItemSelected), NULL, this);
    m_dataview->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
                           wxDataViewEventHandler(SFTPBrowserBaseDlg::OnItemActivated), NULL, this);
    m_textCtrlInlineSearch->Disconnect(wxEVT_COMMAND_TEXT_UPDATED,
                                       wxCommandEventHandler(SFTPBrowserBaseDlg::OnTextUpdated), NULL, this);
    m_textCtrlInlineSearch->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(SFTPBrowserBaseDlg::OnEnter),
                                       NULL, this);
    m_textCtrlInlineSearch->Disconnect(wxEVT_KILL_FOCUS, wxFocusEventHandler(SFTPBrowserBaseDlg::OnFocusLost), NULL,
                                       this);
    m_button59->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SFTPBrowserBaseDlg::OnOKUI), NULL, this);
}
