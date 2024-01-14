//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: newclasswizard.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "newclassbasedlg.h"


// Declare the bitmap loading function
extern void wxC3999InitBitmapResources();

static bool bBitmapLoaded = false;


NewClassBaseDlg::NewClassBaseDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC3999InitBitmapResources();
        bBitmapLoaded = true;
    }

    wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(bSizer1);

    wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(0, 3, 0, 0);
    fgSizer1->SetFlexibleDirection( wxBOTH );
    fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    fgSizer1->AddGrowableCol(1);

    bSizer1->Add(fgSizer1, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));

    m_staticText1 = new wxStaticText(this, wxID_ANY, _("Class Name:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    fgSizer1->Add(m_staticText1, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textClassName = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(400,-1)), 0);
    m_textClassName->SetToolTip(_("The class name"));
    m_textClassName->SetFocus();
#if wxVERSION_NUMBER >= 3000
    m_textClassName->SetHint(wxT(""));
#endif

    fgSizer1->Add(m_textClassName, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));

    m_staticText9 = new wxStaticText(this, wxID_ANY, _("*"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_staticText9->SetForegroundColour(wxColour(wxT("rgb(255,0,0)")));
    wxFont m_staticText9Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_staticText9Font.SetWeight(wxFONTWEIGHT_BOLD);
    m_staticText9->SetFont(m_staticText9Font);

    fgSizer1->Add(m_staticText9, 0, wxALL|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_staticText37 = new wxStaticText(this, wxID_ANY, _("File name:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);

    fgSizer1->Add(m_staticText37, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textCtrlFileName = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_textCtrlFileName->SetToolTip(_("This is the base-name for the file(s) that will be generated. If the new class is called Foo, by default the files will be Foo.cpp and Foo.h. If you'd prefer different names, type the base-name here."));
#if wxVERSION_NUMBER >= 3000
    m_textCtrlFileName->SetHint(wxT(""));
#endif

    fgSizer1->Add(m_textCtrlFileName, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));

    fgSizer1->Add(0, 0, 1, wxALL, WXC_FROM_DIP(5));

    m_staticTextNamespace = new wxStaticText(this, wxID_ANY, _("Namespace:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    fgSizer1->Add(m_staticTextNamespace, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textCtrlNamespace = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_textCtrlNamespace->SetToolTip(_("Place this class inside a namespace"));
#if wxVERSION_NUMBER >= 3000
    m_textCtrlNamespace->SetHint(wxT(""));
#endif

    fgSizer1->Add(m_textCtrlNamespace, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));

    m_buttonBrowseNamespaces = new wxButton(this, wxID_ANY, _("..."), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxBU_EXACTFIT);

    fgSizer1->Add(m_buttonBrowseNamespaces, 0, wxALL, WXC_FROM_DIP(5));

    m_staticText6 = new wxStaticText(this, wxID_ANY, _("Block Guard:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    fgSizer1->Add(m_staticText6, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textCtrlBlockGuard = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_textCtrlBlockGuard->SetToolTip(_("Set block guard to prevent multiple file inclusion. If left empty, the class name is used"));
#if wxVERSION_NUMBER >= 3000
    m_textCtrlBlockGuard->SetHint(wxT(""));
#endif

    fgSizer1->Add(m_textCtrlBlockGuard, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));

    fgSizer1->Add(0, 0, 1, wxEXPAND, WXC_FROM_DIP(5));

    m_staticText4 = new wxStaticText(this, wxID_ANY, _("Select Virtual Directory:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    fgSizer1->Add(m_staticText4, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textCtrlVD = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTE_READONLY);
    m_textCtrlVD->SetToolTip(_("The new class needs to be put somewhere. Select which of the project's virtual folders to use."));
#if wxVERSION_NUMBER >= 3000
    m_textCtrlVD->SetHint(wxT(""));
#endif

    fgSizer1->Add(m_textCtrlVD, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));

    m_buttonSelectVD = new wxButton(this, wxID_ANY, _("..."), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxBU_EXACTFIT);

    fgSizer1->Add(m_buttonSelectVD, 0, wxALL, WXC_FROM_DIP(5));

    m_staticText2 = new wxStaticText(this, wxID_ANY, _("Generated File(s) Path:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    fgSizer1->Add(m_staticText2, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textCtrlGenFilePath = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_textCtrlGenFilePath->SetToolTip(_("Where on the filesystem should the new class's files be put? This will normally be the directory corresponding to the Virtual Directory; but you can enter an alternative directory here if you wish."));
#if wxVERSION_NUMBER >= 3000
    m_textCtrlGenFilePath->SetHint(wxT(""));
#endif

    fgSizer1->Add(m_textCtrlGenFilePath, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));

    m_buttonBrowseFolder = new wxButton(this, wxID_ANY, _("..."), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxBU_EXACTFIT);

    fgSizer1->Add(m_buttonBrowseFolder, 0, wxALL, WXC_FROM_DIP(5));

    m_staticText8 = new wxStaticText(this, wxID_ANY, _("More options:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    wxFont m_staticText8Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_staticText8Font.SetWeight(wxFONTWEIGHT_BOLD);
    m_staticText8->SetFont(m_staticText8Font);

    bSizer1->Add(m_staticText8, 0, wxALL, WXC_FROM_DIP(5));

    wxFlexGridSizer* flexGridSizer35 = new wxFlexGridSizer(0, 2, 0, 0);
    flexGridSizer35->SetFlexibleDirection( wxBOTH );
    flexGridSizer35->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    bSizer1->Add(flexGridSizer35, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

    m_checkBoxInline = new wxCheckBox(this, wxID_ANY, _("Inline class"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_checkBoxInline->SetValue(false);
    m_checkBoxInline->SetToolTip(_("Put both the declaration and the implementation in the header file"));

    flexGridSizer35->Add(m_checkBoxInline, 0, wxALL, WXC_FROM_DIP(5));

    m_checkBoxHpp = new wxCheckBox(this, wxID_ANY, _("Create .hpp instead of .h"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_checkBoxHpp->SetValue(false);
    m_checkBoxHpp->SetToolTip(_("If checked, the generated header file will be foo.hpp instead of foo.h"));

    flexGridSizer35->Add(m_checkBoxHpp, 0, wxALL, WXC_FROM_DIP(5));

    m_checkBoxPragmaOnce = new wxCheckBox(this, wxID_ANY, _("Use #pragma once"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    m_checkBoxPragmaOnce->SetValue(false);

    flexGridSizer35->Add(m_checkBoxPragmaOnce, 0, wxALL, WXC_FROM_DIP(5));

    m_checkBoxLowercaseFileName = new wxCheckBox(this, wxID_ANY, _("Use lowercase file names"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    m_checkBoxLowercaseFileName->SetValue(false);

    flexGridSizer35->Add(m_checkBoxLowercaseFileName, 0, wxALL, WXC_FROM_DIP(5));

    m_collPane2 = new wxCollapsiblePane(this, wxID_ANY, _("Advanced"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxCP_DEFAULT_STYLE);

    bSizer1->Add(m_collPane2, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* boxSizer6 = new wxBoxSizer(wxVERTICAL);
    m_collPane2->GetPane()->SetSizer(boxSizer6);

    m_staticText7 = new wxStaticText(m_collPane2->GetPane(), wxID_ANY, _("Inherits:"), wxDefaultPosition, wxDLG_UNIT(m_collPane2->GetPane(), wxSize(-1, -1)), 0);

    boxSizer6->Add(m_staticText7, 0, wxALL, WXC_FROM_DIP(5));

    wxBoxSizer* bSizer15 = new wxBoxSizer(wxHORIZONTAL);

    boxSizer6->Add(bSizer15, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));

    m_listCtrl1 = new wxListCtrl(m_collPane2->GetPane(), wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_collPane2->GetPane(), wxSize(-1, -1)), wxLC_VRULES|wxLC_HRULES|wxLC_REPORT);

    bSizer15->Add(m_listCtrl1, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* bSizer16 = new wxBoxSizer(wxVERTICAL);

    bSizer15->Add(bSizer16, 0, wxEXPAND, WXC_FROM_DIP(5));

    m_buttonAddInheritance = new wxButton(m_collPane2->GetPane(), wxID_ANY, _("Add..."), wxDefaultPosition, wxDLG_UNIT(m_collPane2->GetPane(), wxSize(-1, -1)), 0);
    m_buttonAddInheritance->SetToolTip(_("Click to add a class from which to derive"));

    bSizer16->Add(m_buttonAddInheritance, 0, wxALL, WXC_FROM_DIP(5));

    m_buttonDelInheritance = new wxButton(m_collPane2->GetPane(), wxID_ANY, _("Delete"), wxDefaultPosition, wxDLG_UNIT(m_collPane2->GetPane(), wxSize(-1, -1)), 0);

    bSizer16->Add(m_buttonDelInheritance, 0, wxALL, WXC_FROM_DIP(5));

    wxFlexGridSizer* fgSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
    fgSizer2->SetFlexibleDirection( wxBOTH );
    fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    fgSizer2->AddGrowableCol(0);
    fgSizer2->AddGrowableCol(1);

    boxSizer6->Add(fgSizer2, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));

    m_checkBoxCopyable = new wxCheckBox(m_collPane2->GetPane(), wxID_ANY, _("Declare this class non-copyable"), wxDefaultPosition, wxDLG_UNIT(m_collPane2->GetPane(), wxSize(-1, -1)), 0);
    m_checkBoxCopyable->SetValue(false);

    fgSizer2->Add(m_checkBoxCopyable, 0, wxALL, WXC_FROM_DIP(5));

    m_checkBoxImplVirtual = new wxCheckBox(m_collPane2->GetPane(), wxID_ANY, _("Implement all virtual functions"), wxDefaultPosition, wxDLG_UNIT(m_collPane2->GetPane(), wxSize(-1, -1)), 0);
    m_checkBoxImplVirtual->SetValue(false);

    fgSizer2->Add(m_checkBoxImplVirtual, 0, wxALL, WXC_FROM_DIP(5));

    m_checkBoxSingleton = new wxCheckBox(m_collPane2->GetPane(), wxID_ANY, _("This is a singleton class"), wxDefaultPosition, wxDLG_UNIT(m_collPane2->GetPane(), wxSize(-1, -1)), 0);
    m_checkBoxSingleton->SetValue(false);

    fgSizer2->Add(m_checkBoxSingleton, 0, wxALL, WXC_FROM_DIP(5));

    m_checkBoxImplPureVirtual = new wxCheckBox(m_collPane2->GetPane(), wxID_ANY, _("Implement all pure virtual functions"), wxDefaultPosition, wxDLG_UNIT(m_collPane2->GetPane(), wxSize(-1, -1)), 0);
    m_checkBoxImplPureVirtual->SetValue(false);

    fgSizer2->Add(m_checkBoxImplPureVirtual, 0, wxALL, WXC_FROM_DIP(5));

    m_checkBoxVirtualDtor = new wxCheckBox(m_collPane2->GetPane(), wxID_ANY, _("Virtual destructor"), wxDefaultPosition, wxDLG_UNIT(m_collPane2->GetPane(), wxSize(-1, -1)), 0);
    m_checkBoxVirtualDtor->SetValue(false);

    fgSizer2->Add(m_checkBoxVirtualDtor, 0, wxALL, WXC_FROM_DIP(5));

    m_stdBtnSizer30 = new wxStdDialogButtonSizer();

    bSizer1->Add(m_stdBtnSizer30, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(10));

    m_button32 = new wxButton(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_stdBtnSizer30->AddButton(m_button32);

    m_button34 = new wxButton(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_button34->SetDefault();
    m_stdBtnSizer30->AddButton(m_button34);
    m_stdBtnSizer30->Realize();

    SetName(wxT("NewClassBaseDlg"));
    SetSize(wxDLG_UNIT(this, wxSize(-1,-1)));
    if (GetSizer()) {
        GetSizer()->Fit(this);
    }
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
    m_textClassName->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(NewClassBaseDlg::OnTextEnter), NULL, this);
    m_buttonBrowseNamespaces->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnBrowseNamespace), NULL, this);
    m_staticText6->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewClassBaseDlg::OnBlockGuardUI), NULL, this);
    m_textCtrlBlockGuard->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewClassBaseDlg::OnBlockGuardUI), NULL, this);
    m_buttonSelectVD->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnBrowseVD), NULL, this);
    m_buttonBrowseFolder->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnBrowseFolder), NULL, this);
    m_checkBoxInline->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnCheckInline), NULL, this);
    m_checkBoxLowercaseFileName->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnUseLowerCaseFileName), NULL, this);
    m_listCtrl1->Connect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(NewClassBaseDlg::OnListItemActivated), NULL, this);
    m_listCtrl1->Connect(wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler(NewClassBaseDlg::OnListItemDeSelected), NULL, this);
    m_listCtrl1->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(NewClassBaseDlg::OnListItemSelected), NULL, this);
    m_buttonAddInheritance->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnButtonAdd), NULL, this);
    m_buttonDelInheritance->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnButtonDelete), NULL, this);
    m_buttonDelInheritance->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewClassBaseDlg::OnButtonDeleteUI), NULL, this);
    m_checkBoxImplVirtual->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnCheckImpleAllVirtualFunctions), NULL, this);
    m_button34->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnButtonOK), NULL, this);

}

NewClassBaseDlg::~NewClassBaseDlg()
{
    m_textClassName->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(NewClassBaseDlg::OnTextEnter), NULL, this);
    m_buttonBrowseNamespaces->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnBrowseNamespace), NULL, this);
    m_staticText6->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewClassBaseDlg::OnBlockGuardUI), NULL, this);
    m_textCtrlBlockGuard->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewClassBaseDlg::OnBlockGuardUI), NULL, this);
    m_buttonSelectVD->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnBrowseVD), NULL, this);
    m_buttonBrowseFolder->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnBrowseFolder), NULL, this);
    m_checkBoxInline->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnCheckInline), NULL, this);
    m_checkBoxLowercaseFileName->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnUseLowerCaseFileName), NULL, this);
    m_listCtrl1->Disconnect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(NewClassBaseDlg::OnListItemActivated), NULL, this);
    m_listCtrl1->Disconnect(wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler(NewClassBaseDlg::OnListItemDeSelected), NULL, this);
    m_listCtrl1->Disconnect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(NewClassBaseDlg::OnListItemSelected), NULL, this);
    m_buttonAddInheritance->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnButtonAdd), NULL, this);
    m_buttonDelInheritance->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnButtonDelete), NULL, this);
    m_buttonDelInheritance->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewClassBaseDlg::OnButtonDeleteUI), NULL, this);
    m_checkBoxImplVirtual->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnCheckImpleAllVirtualFunctions), NULL, this);
    m_button34->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewClassBaseDlg::OnButtonOK), NULL, this);

}

NewIneritanceBaseDlg::NewIneritanceBaseDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC3999InitBitmapResources();
        bBitmapLoaded = true;
    }

    wxBoxSizer* boxSizer10 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer10);

    wxFlexGridSizer* flexGridSizer12 = new wxFlexGridSizer(0, 3, 0, 0);
    flexGridSizer12->SetFlexibleDirection( wxBOTH );
    flexGridSizer12->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer12->AddGrowableCol(1);

    boxSizer10->Add(flexGridSizer12, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));

    m_staticText20 = new wxStaticText(this, wxID_ANY, _("Select Parent Class:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);

    flexGridSizer12->Add(m_staticText20, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textCtrlInhertiance = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    m_textCtrlInhertiance->SetToolTip(_("Type the name of the parent class"));
#if wxVERSION_NUMBER >= 3000
    m_textCtrlInhertiance->SetHint(wxT(""));
#endif

    flexGridSizer12->Add(m_textCtrlInhertiance, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));

    m_button24 = new wxButton(this, wxID_ANY, _("..."), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    m_button24->SetToolTip(_("Click here to open the open resource dialog"));

    flexGridSizer12->Add(m_button24, 0, wxALL|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_staticText26 = new wxStaticText(this, wxID_ANY, _("Inheritance Access:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);

    flexGridSizer12->Add(m_staticText26, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    wxArrayString m_choiceAccessArr;
    m_choiceAccess = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), m_choiceAccessArr, 0);

    flexGridSizer12->Add(m_choiceAccess, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_stdBtnSizer14 = new wxStdDialogButtonSizer();

    boxSizer10->Add(m_stdBtnSizer14, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

    m_button16 = new wxButton(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_button16->SetDefault();
    m_stdBtnSizer14->AddButton(m_button16);

    m_button18 = new wxButton(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_stdBtnSizer14->AddButton(m_button18);
    m_stdBtnSizer14->Realize();

    SetName(wxT("NewIneritanceBaseDlg"));
    SetSize(wxDLG_UNIT(this, wxSize(-1,-1)));
    if (GetSizer()) {
        GetSizer()->Fit(this);
    }
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
    m_button24->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewIneritanceBaseDlg::OnButtonMore), NULL, this);

}

NewIneritanceBaseDlg::~NewIneritanceBaseDlg()
{
    m_button24->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewIneritanceBaseDlg::OnButtonMore), NULL, this);

}
