///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 18 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "CommonView.h"

///////////////////////////////////////////////////////////////////////////

OptionAddView::OptionAddView( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );

	m_text = gsgs_Style().createTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB );
	bSizer23->Add( m_text, 1, wxALL|wxEXPAND, 5 );

	m_staticline9 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer23->Add( m_staticline9, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxHORIZONTAL );

	m_buttonOK = new Button( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer24->Add( m_buttonOK, 0, wxALL, 5 );

	m_buttonCancel = new Button( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer24->Add( m_buttonCancel, 0, wxALL, 5 );


	bSizer23->Add( bSizer24, 0, wxALIGN_RIGHT, 5 );


	this->SetSizer( bSizer23 );
	this->Layout();
}

OptionAddView::~OptionAddView()
{
}

OptionCommandAddView::OptionCommandAddView( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );

	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	m_staticText1 = gsgs_Style().createStaticText( this, wxID_ANY, _("Check the command line options you need"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	mainSizer->Add( m_staticText1, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

	wxBoxSizer* optionsSizer;
	optionsSizer = new wxBoxSizer( wxVERTICAL );

	wxArrayString m_checkListOptionsChoices;
	m_checkListOptions = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListOptionsChoices, wxLB_EXTENDED|wxLB_NEEDED_SB|wxLB_SORT );
	optionsSizer->Add( m_checkListOptions, 2, wxALL|wxEXPAND, 5 );

	m_staticText4 = gsgs_Style().createStaticText( this, wxID_ANY, _("Enter other options"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	optionsSizer->Add( m_staticText4, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

	m_textOptions = gsgs_Style().createTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	optionsSizer->Add( m_textOptions, 1, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* cmdlnSizer;
	cmdlnSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Command line") ), wxVERTICAL );

	m_textCmdLn = gsgs_Style().createTextCtrl( cmdlnSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,50 ), wxTE_MULTILINE|wxTE_NO_VSCROLL|wxTE_READONLY|wxBORDER_NONE );
	m_textCmdLn->SetMinSize( wxSize( -1,50 ) );

	cmdlnSizer->Add( m_textCmdLn, 0, wxEXPAND, 5 );


	optionsSizer->Add( cmdlnSizer, 0, wxEXPAND|wxALL, 5 );


	mainSizer->Add( optionsSizer, 1, wxEXPAND, 5 );

	m_staticline5 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );

	m_sdbSizer = new DialogButtonSizer();
	m_sdbSizerOK = new Button( this, wxID_OK );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizerCancel = new Button( this, wxID_CANCEL );
	m_sdbSizer->AddButton( m_sdbSizerCancel );
	m_sdbSizer->Realize();

	mainSizer->Add( m_sdbSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );


	this->SetSizer( mainSizer );
	this->Layout();

	// Connect Events
	m_checkListOptions->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( OptionCommandAddView::OnOptionToggled ), NULL, this );
	m_textOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( OptionCommandAddView::OnOptionsText ), NULL, this );
}

OptionCommandAddView::~OptionCommandAddView()
{
	// Disconnect Events
	m_checkListOptions->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( OptionCommandAddView::OnOptionToggled ), NULL, this );
	m_textOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( OptionCommandAddView::OnOptionsText ), NULL, this );

}

ProjectBuildView::ProjectBuildView( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_staticText = gsgs_Style().createStaticText( this, wxID_ANY, _("Select the configurations to build/clean:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText->Wrap( -1 );
	bSizer1->Add( m_staticText, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	wxArrayString m_checkListConfigurationsChoices;
	m_checkListConfigurations = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListConfigurationsChoices, 0 );
	bSizer2->Add( m_checkListConfigurations, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	m_buttonBuild = new Button( this, wxID_ANY, _("&Build"), wxDefaultPosition, wxDefaultSize, 0 );

	m_buttonBuild->SetDefault();
	bSizer4->Add( m_buttonBuild, 0, wxALL, 5 );

	m_buttonClean = new Button( this, wxID_ANY, _("&Clean"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonClean, 0, wxALL, 5 );

	m_button3 = new Button( this, wxID_ANY, _("Check &All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button3, 0, wxALL, 5 );

	m_button4 = new Button( this, wxID_ANY, _("&Uncheck All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button4, 0, wxALL, 5 );


	bSizer3->Add( bSizer4, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	m_buttonDown = new Button( this, wxID_ANY, _("Move Up"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_buttonDown, 0, wxALL, 5 );

	m_buttonUp = new Button( this, wxID_ANY, _("Move Down"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_buttonUp, 0, wxALL, 5 );


	bSizer3->Add( bSizer5, 1, wxEXPAND, 5 );


	bSizer2->Add( bSizer3, 0, wxEXPAND, 5 );


	bSizer1->Add( bSizer2, 1, wxEXPAND, 5 );

	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	m_buttonClose = new Button( this, wxID_CANCEL, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_buttonClose, 0, wxALL, 5 );


	bSizer1->Add( bSizer6, 0, wxALIGN_RIGHT, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_checkListConfigurations->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ProjectBuildView::OnItemSelected ), NULL, this );
	m_checkListConfigurations->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( ProjectBuildView::OnItemToggled ), NULL, this );
	m_buttonBuild->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectBuildView::OnBuild ), NULL, this );
	m_buttonBuild->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ProjectBuildView::OnBuildUI ), NULL, this );
	m_buttonClean->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectBuildView::OnClean ), NULL, this );
	m_buttonClean->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ProjectBuildView::OnCleanUI ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectBuildView::OnCheckAll ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectBuildView::OnUnCheckAll ), NULL, this );
	m_buttonDown->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectBuildView::OnMoveUp ), NULL, this );
	m_buttonDown->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ProjectBuildView::OnMoveUpUI ), NULL, this );
	m_buttonUp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectBuildView::OnMoveDown ), NULL, this );
	m_buttonUp->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ProjectBuildView::OnMoveDownUI ), NULL, this );
	m_buttonClose->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectBuildView::OnClose ), NULL, this );
}

ProjectBuildView::~ProjectBuildView()
{
	// Disconnect Events
	m_checkListConfigurations->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ProjectBuildView::OnItemSelected ), NULL, this );
	m_checkListConfigurations->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( ProjectBuildView::OnItemToggled ), NULL, this );
	m_buttonBuild->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectBuildView::OnBuild ), NULL, this );
	m_buttonBuild->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ProjectBuildView::OnBuildUI ), NULL, this );
	m_buttonClean->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectBuildView::OnClean ), NULL, this );
	m_buttonClean->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ProjectBuildView::OnCleanUI ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectBuildView::OnCheckAll ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectBuildView::OnUnCheckAll ), NULL, this );
	m_buttonDown->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectBuildView::OnMoveUp ), NULL, this );
	m_buttonDown->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ProjectBuildView::OnMoveUpUI ), NULL, this );
	m_buttonUp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectBuildView::OnMoveDown ), NULL, this );
	m_buttonUp->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ProjectBuildView::OnMoveDownUI ), NULL, this );
	m_buttonClose->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectBuildView::OnClose ), NULL, this );

}

DebugCommandView::DebugCommandView( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText2 = gsgs_Style().createStaticText( this, wxID_ANY, _("Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1 );

	m_textCtrlName = gsgs_Style().createTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrlName, 0, wxALL|wxEXPAND, 2 );

	m_staticText3 = gsgs_Style().createStaticText( this, wxID_ANY, _("Expression:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1 );

	m_textCtrlCommand = gsgs_Style().createTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrlCommand, 0, wxALL|wxEXPAND, 2 );

	m_staticText4 = gsgs_Style().createStaticText( this, wxID_ANY, _("Debugger command:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );

	m_textCtrtDebuggerCommand = gsgs_Style().createTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrtDebuggerCommand, 0, wxALL|wxEXPAND, 2 );


	bSizer7->Add( fgSizer1, 1, wxEXPAND|wxALL, 1 );

	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer7->Add( m_staticline2, 0, wxEXPAND | wxALL, 1 );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );

	m_buttonOk = new Button( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );

	m_buttonOk->SetDefault();
	bSizer8->Add( m_buttonOk, 0, wxALL, 1 );

	m_buttonCancel = new Button( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonCancel, 0, wxALL, 1 );


	bSizer7->Add( bSizer8, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1 );


	this->SetSizer( bSizer7 );
	this->Layout();

	this->Centre( wxBOTH );
}

DebugCommandView::~DebugCommandView()
{
}

ProjectDependView::ProjectDependView( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	m_book = new wxChoicebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT );
	mainSizer->Add( m_book, 1, wxEXPAND | wxALL, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* btnSizer;
	btnSizer = new wxBoxSizer( wxHORIZONTAL );

	m_buttonOK = new Button( this, wxID_ANY, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_buttonOK, 0, wxALL, 5 );

	m_buttonCancel = new Button( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_buttonCancel, 0, wxALL, 5 );


	mainSizer->Add( btnSizer, 0, wxALIGN_RIGHT, 5 );


	this->SetSizer( mainSizer );
	this->Layout();
}

ProjectDependView::~ProjectDependView()
{
}

DebugCoreDumpView::DebugCoreDumpView( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText2 = gsgs_Style().createStaticText( this, wxID_ANY, _("Core dump to be opened:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 2 );

	m_Core = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_Core->SetToolTip( _("Enter the full filepath of the core dump to be examined.\nOr, if you enter the correct working directory below, just the filename will suffice.") );

	fgSizer1->Add( m_Core, 0, wxALL|wxEXPAND, 5 );

	m_buttonBrowseCore = new Button( this, wxID_ANY, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonBrowseCore, 0, wxALL, 2 );

	m_staticText21 = gsgs_Style().createStaticText( this, wxID_ANY, _("Corresponding executable:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	fgSizer1->Add( m_staticText21, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );

	m_ExeFilepath = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_ExeFilepath->SetToolTip( _("Enter the full filepath of the executable that crashed to cause the core dump.\nOr, if you enter the correct working directory below, just the filename will suffice.") );

	fgSizer1->Add( m_ExeFilepath, 0, wxALL|wxEXPAND, 2 );

	m_buttonBrowseExe = new Button( this, wxID_ANY, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonBrowseExe, 0, wxALL, 2 );

	m_staticText5 = gsgs_Style().createStaticText( this, wxID_ANY, _("Working directory (optional):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 2 );

	m_WD = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	fgSizer1->Add( m_WD, 0, wxALL|wxEXPAND, 5 );

	m_buttonBrowseWD = new Button( this, wxID_ANY, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonBrowseWD, 0, wxALL, 2 );

	m_staticText3 = gsgs_Style().createStaticText( this, wxID_ANY, _("Select debugger:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 2 );

	wxArrayString m_choiceDebuggersChoices;
	m_choiceDebuggers = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDebuggersChoices, 0 );
	m_choiceDebuggers->SetSelection( 0 );
	fgSizer1->Add( m_choiceDebuggers, 0, wxALL|wxEXPAND, 2 );


	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );


	bSizer3->Add( fgSizer1, 0, wxEXPAND, 5 );


	bSizer1->Add( bSizer3, 1, wxEXPAND, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 2 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	m_buttonDebug = new Button( this, wxID_ANY, _("Debug"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonDebug, 0, wxALL, 2 );

	m_buttonCancel = new Button( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonCancel, 0, wxALL, 2 );


	bSizer1->Add( bSizer2, 0, wxALIGN_CENTER_HORIZONTAL, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_buttonBrowseCore->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpView::OnButtonBrowseCore ), NULL, this );
	m_buttonBrowseExe->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpView::OnButtonBrowseExe ), NULL, this );
	m_buttonBrowseWD->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpView::OnButtonBrowseWD ), NULL, this );
	m_buttonDebug->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpView::OnButtonDebug ), NULL, this );
	m_buttonDebug->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebugCoreDumpView::OnDebugBtnUpdateUI ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpView::OnButtonCancel ), NULL, this );
}

DebugCoreDumpView::~DebugCoreDumpView()
{
	// Disconnect Events
	m_buttonBrowseCore->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpView::OnButtonBrowseCore ), NULL, this );
	m_buttonBrowseExe->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpView::OnButtonBrowseExe ), NULL, this );
	m_buttonBrowseWD->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpView::OnButtonBrowseWD ), NULL, this );
	m_buttonDebug->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpView::OnButtonDebug ), NULL, this );
	m_buttonDebug->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebugCoreDumpView::OnDebugBtnUpdateUI ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpView::OnButtonCancel ), NULL, this );

}

SymbolsView::SymbolsView( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	m_results = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	mainSizer->Add( m_results, 1, wxALL|wxEXPAND, 5 );

	statLine = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( statLine, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* btnSizer;
	btnSizer = new wxBoxSizer( wxHORIZONTAL );

	m_okButton = new Button( this, wxID_ANY, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );

	m_okButton->SetDefault();
	btnSizer->Add( m_okButton, 0, wxALL, 5 );

	m_cancel = new Button( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_cancel, 0, wxALL, 5 );


	mainSizer->Add( btnSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );


	this->SetSizer( mainSizer );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_results->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( SymbolsView::OnItemActivated ), NULL, this );
	m_results->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( SymbolsView::OnItemSelected ), NULL, this );
	m_okButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SymbolsView::OnButtonOK ), NULL, this );
	m_cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SymbolsView::OnButtonCancel ), NULL, this );
}

SymbolsView::~SymbolsView()
{
	// Disconnect Events
	m_results->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( SymbolsView::OnItemActivated ), NULL, this );
	m_results->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( SymbolsView::OnItemSelected ), NULL, this );
	m_okButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SymbolsView::OnButtonOK ), NULL, this );
	m_cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SymbolsView::OnButtonCancel ), NULL, this );

}

NewItemView::NewItemView( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* m_mainSizer;
	m_mainSizer = new wxBoxSizer( wxVERTICAL );

	m_fileType = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	m_mainSizer->Add( m_fileType, 1, wxALL|wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 3, 2, 5 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText6 = gsgs_Style().createStaticText( this, wxID_ANY, _("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer2->Add( m_staticText6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_fileName = gsgs_Style().createTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_fileName, 1, wxALL|wxEXPAND, 5 );


	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );

	m_staticText3 = gsgs_Style().createStaticText( this, wxID_ANY, _("Location:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer2->Add( m_staticText3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_location = gsgs_Style().createTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_location->SetMinSize( wxSize( 300,-1 ) );

	fgSizer2->Add( m_location, 1, wxALL|wxEXPAND, 5 );

	m_browseBtn = new Button( this, wxID_ANY, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_browseBtn, 0, wxALL, 5 );


	m_mainSizer->Add( fgSizer2, 0, wxEXPAND|wxALL, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_mainSizer->Add( m_staticline1, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	m_okButton = new Button( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );

	m_okButton->SetDefault();
	bSizer6->Add( m_okButton, 0, wxALL, 5 );

	m_cancel = new Button( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_cancel, 0, wxALL, 5 );


	m_mainSizer->Add( bSizer6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );


	this->SetSizer( m_mainSizer );
	this->Layout();

	this->Centre( wxBOTH );
}

NewItemView::~NewItemView()
{
}

EditConfigurationView::EditConfigurationView( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );

	m_panel6 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );

	m_configurationsList = new wxListBox( m_panel6, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	bSizer17->Add( m_configurationsList, 1, wxALL|wxEXPAND, 2 );

	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxVERTICAL );

	m_buttonDelete = new Button( m_panel6, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_buttonDelete, 0, wxALL, 2 );

	m_buttonRename = new Button( m_panel6, wxID_ANY, _("Rename"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_buttonRename, 0, wxALL, 2 );


	bSizer17->Add( bSizer18, 0, wxEXPAND, 5 );


	m_panel6->SetSizer( bSizer17 );
	m_panel6->Layout();
	bSizer17->Fit( m_panel6 );
	bSizer15->Add( m_panel6, 1, wxALL|wxEXPAND, 5 );

	m_staticline9 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer15->Add( m_staticline9, 0, wxEXPAND | wxALL, 2 );

	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxHORIZONTAL );

	m_buttonClose = new Button( this, wxID_OK, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_buttonClose, 0, wxALL, 2 );


	bSizer15->Add( bSizer16, 0, wxALIGN_CENTER, 5 );


	this->SetSizer( bSizer15 );
	this->Layout();
}

EditConfigurationView::~EditConfigurationView()
{
}

FreeTextView::FreeTextView( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );

	m_text = gsgs_Style().createTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB );
	bSizer23->Add( m_text, 1, wxALL|wxEXPAND, 5 );

	m_staticline9 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer23->Add( m_staticline9, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxHORIZONTAL );

	m_buttonOK = new Button( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer24->Add( m_buttonOK, 0, wxALL, 5 );

	m_buttonCancel = new Button( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer24->Add( m_buttonCancel, 0, wxALL, 5 );


	bSizer23->Add( bSizer24, 0, wxALIGN_RIGHT, 5 );


	this->SetSizer( bSizer23 );
	this->Layout();
}

FreeTextView::~FreeTextView()
{
}

NewConfigurationView::NewConfigurationView( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	m_staticText1 = gsgs_Style().createStaticText( m_panel1, wxID_ANY, _("Configuration Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer3->Add( m_staticText1, 0, wxALL, 5 );

	m_textConfigurationName = gsgs_Style().createTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_textConfigurationName, 0, wxALL|wxEXPAND, 5 );

	m_staticText2 = gsgs_Style().createStaticText( m_panel1, wxID_ANY, _("Copy Settings from:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer3->Add( m_staticText2, 0, wxALL, 5 );

	wxArrayString m_choiceCopyConfigurationsChoices;
	m_choiceCopyConfigurations = new wxChoice( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceCopyConfigurationsChoices, 0 );
	m_choiceCopyConfigurations->SetSelection( 0 );
	bSizer3->Add( m_choiceCopyConfigurations, 0, wxALL|wxEXPAND, 5 );


	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 1, wxEXPAND | wxALL, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	m_buttonOK = new Button( this, wxID_ANY, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonOK, 0, wxALL, 5 );

	m_buttonCancel = new Button( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonCancel, 0, wxALL, 5 );


	bSizer1->Add( bSizer2, 0, wxALIGN_RIGHT, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();
}

NewConfigurationView::~NewConfigurationView()
{
}

RenameFileView::RenameFileView( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText8 = gsgs_Style().createStaticText( this, wxID_ANY, _("Replace With:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	bSizer3->Add( m_staticText8, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrlReplaceWith = gsgs_Style().createTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	bSizer3->Add( m_textCtrlReplaceWith, 1, wxALL|wxEXPAND, 5 );


	mainSizer->Add( bSizer3, 0, wxEXPAND, 5 );

	m_staticText1 = gsgs_Style().createStaticText( this, wxID_ANY, _("The following files will be updated:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	mainSizer->Add( m_staticText1, 0, wxALL|wxEXPAND, 5 );

	wxArrayString m_checkListMatchesChoices;
	m_checkListMatches = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListMatchesChoices, 0 );
	mainSizer->Add( m_checkListMatches, 1, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxVERTICAL );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText3 = gsgs_Style().createStaticText( sbSizer1->GetStaticBox(), wxID_ANY, _("Included From:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	m_staticText3->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	fgSizer1->Add( m_staticText3, 0, wxALL, 5 );

	m_staticTextIncludedInFile = gsgs_Style().createStaticText( sbSizer1->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextIncludedInFile->Wrap( -1 );
	fgSizer1->Add( m_staticTextIncludedInFile, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText5 = gsgs_Style().createStaticText( sbSizer1->GetStaticBox(), wxID_ANY, _("Line:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	m_staticText5->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	fgSizer1->Add( m_staticText5, 0, wxALL, 5 );

	m_staticTextFoundInLine = gsgs_Style().createStaticText( sbSizer1->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_staticTextFoundInLine, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText9 = gsgs_Style().createStaticText( sbSizer1->GetStaticBox(), wxID_ANY, _("Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	fgSizer1->Add( m_staticText9, 0, wxALL, 5 );

	m_staticTextPattern = gsgs_Style().createStaticText( sbSizer1->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_staticTextPattern, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );


	sbSizer1->Add( fgSizer1, 0, wxEXPAND|wxALL, 5 );


	mainSizer->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );

	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );

	m_buttonOk = new Button( this, wxID_OK, _("&Ok"), wxDefaultPosition, wxDefaultSize, 0 );

	m_buttonOk->SetDefault();
	buttonSizer->Add( m_buttonOk, 0, wxALL, 5 );

	m_buttonCancel = new Button( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxALL, 5 );


	mainSizer->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );


	this->SetSizer( mainSizer );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_checkListMatches->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( RenameFileView::OnFileSelected ), NULL, this );
	m_checkListMatches->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( RenameFileView::OnFileToggeled ), NULL, this );
}

RenameFileView::~RenameFileView()
{
	// Disconnect Events
	m_checkListMatches->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( RenameFileView::OnFileSelected ), NULL, this );
	m_checkListMatches->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( RenameFileView::OnFileToggeled ), NULL, this );

}
