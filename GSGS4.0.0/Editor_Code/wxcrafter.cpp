//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: wxcrafter.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "wxcrafter.h"


// Declare the bitmap loading function
extern void wxC3F25InitBitmapResources();

static bool bBitmapLoaded = false;


NavBarControlBaseClass::NavBarControlBaseClass(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC3F25InitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* boxSizer36 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer36);
    
    m_splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxSP_LIVE_UPDATE);
    m_splitter->SetSashGravity(0);
    m_splitter->SetMinimumPaneSize(10);
    
    boxSizer36->Add(m_splitter, 1, wxEXPAND, WXC_FROM_DIP(5));
    
    m_splitterPage39 = new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitter, wxSize(-1,-1)), wxTAB_TRAVERSAL);
    
    wxBoxSizer* boxSizer42 = new wxBoxSizer(wxVERTICAL);
    m_splitterPage39->SetSizer(boxSizer42);
    
    wxArrayString m_scopeArr;
    m_scope = new wxChoice(m_splitterPage39, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitterPage39, wxSize(-1,-1)), m_scopeArr, 0);
    
    boxSizer42->Add(m_scope, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_splitterPage41 = new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitter, wxSize(-1,-1)), wxTAB_TRAVERSAL);
    m_splitter->SplitVertically(m_splitterPage39, m_splitterPage41, 200);
    
    wxBoxSizer* boxSizer43 = new wxBoxSizer(wxVERTICAL);
    m_splitterPage41->SetSizer(boxSizer43);
    
    wxArrayString m_funcArr;
    m_func = new wxChoice(m_splitterPage41, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitterPage41, wxSize(-1,-1)), m_funcArr, 0);
    
    boxSizer43->Add(m_func, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    SetName(wxT("NavBarControlBaseClass"));
    SetSize(wxDLG_UNIT(this, wxSize(-1,-1)));
    if (GetSizer()) {
         GetSizer()->Fit(this);
    }
    // Connect events
    m_scope->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(NavBarControlBaseClass::OnScope), NULL, this);
    m_func->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(NavBarControlBaseClass::OnFunction), NULL, this);
    
}

NavBarControlBaseClass::~NavBarControlBaseClass()
{
    m_scope->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(NavBarControlBaseClass::OnScope), NULL, this);
    m_func->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(NavBarControlBaseClass::OnFunction), NULL, this);
    
}

NewProjImgList::NewProjImgList()
    : wxImageList(16, 16, false)
    , m_imagesWidth(16)
    , m_imagesHeight(16)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC3F25InitBitmapResources();
        bBitmapLoaded = true;
    }
    
}

NewProjImgList::~NewProjImgList()
{
}

AddFunctionsImplBaseDlg::AddFunctionsImplBaseDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC3F25InitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* boxSizer117 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer117);
    
    wxBoxSizer* boxSizer129 = new wxBoxSizer(wxHORIZONTAL);
    
    boxSizer117->Add(boxSizer129, 1, wxEXPAND, WXC_FROM_DIP(5));
    
    m_dvListCtrl = new gsgs::ThemedListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxDV_ROW_LINES|wxDV_SINGLE);
    
    boxSizer129->Add(m_dvListCtrl, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_dvListCtrl->AppendTextColumn(_("Function"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
    wxBoxSizer* boxSizer131 = new wxBoxSizer(wxVERTICAL);
    
    boxSizer129->Add(boxSizer131, 0, wxEXPAND, WXC_FROM_DIP(5));
    
    m_button133 = new Button(this, wxID_ANY, _("Check all"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    boxSizer131->Add(m_button133, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_button135 = new Button(this, wxID_ANY, _("Uncheck all"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    boxSizer131->Add(m_button135, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_filePicker = new wxFilePickerCtrl(this, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxFLP_DEFAULT_STYLE);
    m_filePicker->SetToolTip(_("The functions will be placed into this file"));
    
    boxSizer117->Add(m_filePicker, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    wxBoxSizer* boxSizer119 = new wxBoxSizer(wxHORIZONTAL);
    
    boxSizer117->Add(boxSizer119, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));
    
    m_button121 = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    m_button121->SetDefault();
    
    boxSizer119->Add(m_button121, 0, wxALL, WXC_FROM_DIP(5));
    
    m_button123 = new Button(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    boxSizer119->Add(m_button123, 0, wxALL, WXC_FROM_DIP(5));
    
    SetName(wxT("AddFunctionsImplBaseDlg"));
    SetMinClientSize(wxSize(500,300));
    SetSize(wxDLG_UNIT(this, wxSize(-1,-1)));
    if (GetSizer()) {
         GetSizer()->Fit(this);
    }
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }
    // Connect events
    m_button133->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AddFunctionsImplBaseDlg::OnCheckAll), NULL, this);
    m_button135->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AddFunctionsImplBaseDlg::OnUncheckAll), NULL, this);
    m_button121->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(AddFunctionsImplBaseDlg::OnOKUI), NULL, this);
    
}

AddFunctionsImplBaseDlg::~AddFunctionsImplBaseDlg()
{
    m_button133->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AddFunctionsImplBaseDlg::OnCheckAll), NULL, this);
    m_button135->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AddFunctionsImplBaseDlg::OnUncheckAll), NULL, this);
    m_button121->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(AddFunctionsImplBaseDlg::OnOKUI), NULL, this);
    
}

EditorFrameBase::EditorFrameBase(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC3F25InitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(mainSizer);
    
    m_mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTAB_TRAVERSAL);
    
    mainSizer->Add(m_mainPanel, 1, wxEXPAND, WXC_FROM_DIP(5));
    
    wxBoxSizer* boxSizer324 = new wxBoxSizer(wxVERTICAL);
    m_mainPanel->SetSizer(boxSizer324);
    
    m_toolbar = new gsgs::ToolBar(m_mainPanel, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_mainPanel, wxSize(-1,-1)), wxTB_NOALIGN|wxTB_FLAT);
    m_toolbar->SetToolBitmapSize(wxSize(24,24));
    
    boxSizer324->Add(m_toolbar, 0, wxEXPAND, WXC_FROM_DIP(5));
    
    m_menuBar587 = new wxMenuBar(0);
    this->SetMenuBar(m_menuBar587);
    
    FILE = new wxMenu();
    m_menuBar587->Append(FILE, _("File"));
    
    m_menuItemSave = new wxMenuItem(FILE, XRCID("file-save"), _("Save\tCtrl-S"), wxT(""), wxITEM_NORMAL);
    FILE->Append(m_menuItemSave);
    
    m_menuItemReload = new wxMenuItem(FILE, XRCID("reload_file"), _("Reload File\tCtrl-R"), wxT(""), wxITEM_NORMAL);
    FILE->Append(m_menuItemReload);
    
    FILE->AppendSeparator();
    
    m_menuItemClose = new wxMenuItem(FILE, XRCID("file-close"), _("Close\tCtrl-W"), wxT(""), wxITEM_NORMAL);
    FILE->Append(m_menuItemClose);
    
    EDIT = new wxMenu();
    m_menuBar587->Append(EDIT, _("Edit"));
    
    m_menuItemFind = new wxMenuItem(EDIT, XRCID("show-find-bar"), _("Find\tCtrl-F"), wxT(""), wxITEM_NORMAL);
    EDIT->Append(m_menuItemFind);
    
    m_menuItemUndo = new wxMenuItem(EDIT, wxID_UNDO, _("Undo"), wxT(""), wxITEM_NORMAL);
    EDIT->Append(m_menuItemUndo);
    
    m_menuItemRedo = new wxMenuItem(EDIT, wxID_REDO, _("Redo"), wxT(""), wxITEM_NORMAL);
    EDIT->Append(m_menuItemRedo);
    
    SetName(wxT("EditorFrameBase"));
    SetMinClientSize(wxSize(800,600));
    SetSize(wxDLG_UNIT(this, wxSize(800,600)));
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
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(EditorFrameBase::OnCloseWindow), NULL, this);
    this->Connect(m_menuItemSave->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorFrameBase::OnSave), NULL, this);
    this->Connect(m_menuItemSave->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorFrameBase::OnSaveUI), NULL, this);
    this->Connect(m_menuItemReload->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorFrameBase::OnReload), NULL, this);
    this->Connect(m_menuItemClose->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorFrameBase::OnClose), NULL, this);
    this->Connect(m_menuItemClose->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorFrameBase::OnCloseUI), NULL, this);
    this->Connect(m_menuItemFind->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorFrameBase::OnFind), NULL, this);
    this->Connect(m_menuItemFind->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorFrameBase::OnFindUI), NULL, this);
    this->Connect(m_menuItemUndo->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorFrameBase::OnUndo), NULL, this);
    this->Connect(m_menuItemUndo->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorFrameBase::OnUndoUI), NULL, this);
    this->Connect(m_menuItemRedo->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorFrameBase::OnRedo), NULL, this);
    this->Connect(m_menuItemRedo->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorFrameBase::OnRedoUI), NULL, this);
    
}

EditorFrameBase::~EditorFrameBase()
{
    this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(EditorFrameBase::OnCloseWindow), NULL, this);
    this->Disconnect(m_menuItemSave->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorFrameBase::OnSave), NULL, this);
    this->Disconnect(m_menuItemSave->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorFrameBase::OnSaveUI), NULL, this);
    this->Disconnect(m_menuItemReload->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorFrameBase::OnReload), NULL, this);
    this->Disconnect(m_menuItemClose->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorFrameBase::OnClose), NULL, this);
    this->Disconnect(m_menuItemClose->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorFrameBase::OnCloseUI), NULL, this);
    this->Disconnect(m_menuItemFind->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorFrameBase::OnFind), NULL, this);
    this->Disconnect(m_menuItemFind->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorFrameBase::OnFindUI), NULL, this);
    this->Disconnect(m_menuItemUndo->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorFrameBase::OnUndo), NULL, this);
    this->Disconnect(m_menuItemUndo->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorFrameBase::OnUndoUI), NULL, this);
    this->Disconnect(m_menuItemRedo->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorFrameBase::OnRedo), NULL, this);
    this->Disconnect(m_menuItemRedo->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorFrameBase::OnRedoUI), NULL, this);
    
}

ClangOutputTabBase::ClangOutputTabBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC3F25InitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* boxSizer424 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer424);
    
    m_toolbar578 = new gsgs::ToolBar(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    m_toolbar578->SetToolBitmapSize(wxSize(16,16));
    
    boxSizer424->Add(m_toolbar578, 0, wxEXPAND, WXC_FROM_DIP(5));
    
    m_checkBoxEnableClang = new wxCheckBox(m_toolbar578, wxID_ANY, _("Enable Clang"), wxDefaultPosition, wxDLG_UNIT(m_toolbar578, wxSize(-1,-1)), 0);
    m_checkBoxEnableClang->SetValue(false);
    m_checkBoxEnableClang->SetToolTip(_("Enable Clang code completion"));
    m_toolbar578->AddControl(m_checkBoxEnableClang);
    
    m_checkBoxShowErrors = new wxCheckBox(m_toolbar578, wxID_ANY, _("Inline Errors"), wxDefaultPosition, wxDLG_UNIT(m_toolbar578, wxSize(-1,-1)), 0);
    m_checkBoxShowErrors->SetValue(false);
    m_checkBoxShowErrors->SetToolTip(_("Display Clang errors as text annotations inside the editor (i.e. as an inline messages)"));
    m_toolbar578->AddControl(m_checkBoxShowErrors);
    
    m_toolbar578->AddTool(ID_TOOL_CLEAR_ALL, _("Clear Clang Cache"), wxXmlResource::Get()->LoadBitmap(wxT("16-clean")), wxNullBitmap, wxITEM_NORMAL, _("Clear Clang Cache"), _("Clear Clang Cache"), NULL);
    
    m_toolbar578->AddTool(ID_TOOL_CLEAR_LOG, _("Clear Log"), wxXmlResource::Get()->LoadBitmap(wxT("16-clear")), wxNullBitmap, wxITEM_NORMAL, _("Clear Log"), _("Clear Log"), NULL);
    m_toolbar578->Realize();
    m_stc = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxBORDER_STATIC);
    // Configure the fold margin
    m_stc->SetMarginType     (4, wxSTC_MARGIN_SYMBOL);
    m_stc->SetMarginMask     (4, wxSTC_MASK_FOLDERS);
    m_stc->SetMarginSensitive(4, true);
    m_stc->SetMarginWidth    (4, 0);
    
    // Configure the tracker margin
    m_stc->SetMarginWidth(1, 0);
    
    // Configure the symbol margin
    m_stc->SetMarginType (2, wxSTC_MARGIN_SYMBOL);
    m_stc->SetMarginMask (2, ~(wxSTC_MASK_FOLDERS));
    m_stc->SetMarginWidth(2, 0);
    m_stc->SetMarginSensitive(2, true);
    
    // Configure the line numbers margin
    m_stc->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_stc->SetMarginWidth(0,0);
    
    // Configure the line symbol margin
    m_stc->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_stc->SetMarginMask(3, 0);
    m_stc->SetMarginWidth(3,0);
    // Select the lexer
    m_stc->SetLexer(wxSTC_LEX_NULL);
    // Set default font / styles
    m_stc->StyleClearAll();
    m_stc->SetWrapMode(0);
    m_stc->SetIndentationGuides(0);
    m_stc->SetKeyWords(0, wxT(""));
    m_stc->SetKeyWords(1, wxT(""));
    m_stc->SetKeyWords(2, wxT(""));
    m_stc->SetKeyWords(3, wxT(""));
    m_stc->SetKeyWords(4, wxT(""));
    
    boxSizer424->Add(m_stc, 1, wxEXPAND, WXC_FROM_DIP(2));
    
    SetName(wxT("ClangOutputTabBase"));
    SetSize(wxDLG_UNIT(this, wxSize(-1,-1)));
    if (GetSizer()) {
         GetSizer()->Fit(this);
    }
    // Connect events
    m_checkBoxEnableClang->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ClangOutputTabBase::OnEnableClang), NULL, this);
    m_checkBoxEnableClang->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ClangOutputTabBase::OnEnableClangUI), NULL, this);
    m_checkBoxShowErrors->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ClangOutputTabBase::OnShowAnnotations), NULL, this);
    m_checkBoxShowErrors->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ClangOutputTabBase::OnShowAnnotationsUI), NULL, this);
    this->Connect(ID_TOOL_CLEAR_ALL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(ClangOutputTabBase::OnClearCache), NULL, this);
    this->Connect(ID_TOOL_CLEAR_ALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ClangOutputTabBase::OnClearCacheUI), NULL, this);
    this->Connect(ID_TOOL_CLEAR_LOG, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(ClangOutputTabBase::OnClearText), NULL, this);
    this->Connect(ID_TOOL_CLEAR_LOG, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ClangOutputTabBase::OnClearTextUI), NULL, this);
    
}

ClangOutputTabBase::~ClangOutputTabBase()
{
    m_checkBoxEnableClang->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ClangOutputTabBase::OnEnableClang), NULL, this);
    m_checkBoxEnableClang->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ClangOutputTabBase::OnEnableClangUI), NULL, this);
    m_checkBoxShowErrors->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ClangOutputTabBase::OnShowAnnotations), NULL, this);
    m_checkBoxShowErrors->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ClangOutputTabBase::OnShowAnnotationsUI), NULL, this);
    this->Disconnect(ID_TOOL_CLEAR_ALL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(ClangOutputTabBase::OnClearCache), NULL, this);
    this->Disconnect(ID_TOOL_CLEAR_ALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ClangOutputTabBase::OnClearCacheUI), NULL, this);
    this->Disconnect(ID_TOOL_CLEAR_LOG, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(ClangOutputTabBase::OnClearText), NULL, this);
    this->Disconnect(ID_TOOL_CLEAR_LOG, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ClangOutputTabBase::OnClearTextUI), NULL, this);
    
}

OpenFolderDlgBase::OpenFolderDlgBase(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC3F25InitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* boxSizer471 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer471);
    
    wxFlexGridSizer* flexGridSizer483 = new wxFlexGridSizer(0, 2, 0, 0);
    flexGridSizer483->SetFlexibleDirection( wxBOTH );
    flexGridSizer483->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer483->AddGrowableCol(1);
    
    boxSizer471->Add(flexGridSizer483, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_staticText479 = new wxStaticText(this, wxID_ANY, _("Goto Folder:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    flexGridSizer483->Add(m_staticText479, 0, wxALL|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));
    
    m_textCtrlFolder = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(400,-1)), 0);
    #if wxVERSION_NUMBER >= 3000
    m_textCtrlFolder->SetHint(_("Type the folder path"));
    #endif
    m_textCtrlFolder->AutoCompleteDirectories();
    
    flexGridSizer483->Add(m_textCtrlFolder, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_stdBtnSizer473 = new DialogButtonSizer();
    
    boxSizer471->Add(m_stdBtnSizer473, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));
    
    m_button475 = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_button475->SetDefault();
    m_stdBtnSizer473->AddButton(m_button475);
    
    m_button477 = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_stdBtnSizer473->AddButton(m_button477);
    m_stdBtnSizer473->Realize();
    
    SetName(wxT("OpenFolderDlgBase"));
    SetMinClientSize(wxSize(-1,100));
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
}

OpenFolderDlgBase::~OpenFolderDlgBase()
{
}