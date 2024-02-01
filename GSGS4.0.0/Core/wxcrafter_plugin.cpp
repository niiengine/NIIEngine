//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: wxcrafter.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "wxcrafter_plugin.h"
#include "gsgsPreInclude.h"

// Declare the bitmap loading function
extern void wxC9D6CInitBitmapResources();

static bool bBitmapLoaded = false;

GetTextFromUserBaseDialog::GetTextFromUserBaseDialog(wxWindow* parent, wxWindowID id, const wxString& title,
                                                         const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if(!bBitmapLoaded) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC9D6CInitBitmapResources();
        bBitmapLoaded = true;
    }

    wxBoxSizer* boxSizer135 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer135);

    m_staticTextCaption =
        gsgs_Style().createStaticText(this, wxID_ANY, _("Caption"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    boxSizer135->Add(m_staticTextCaption, 0, wxALL, WXC_FROM_DIP(5));

    m_textCtrl = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(300, -1)), 0);

    boxSizer135->Add(m_textCtrl, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    boxSizer135->Add(0, 0, 1, wxALL, WXC_FROM_DIP(5));

    m_stdBtnSizer137 = new DialogButtonSizer();

    boxSizer135->Add(m_stdBtnSizer137, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(10));

    m_buttonCancel = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_stdBtnSizer137->AddButton(m_buttonCancel);

    m_buttonOK = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonOK->SetDefault();
    m_stdBtnSizer137->AddButton(m_buttonOK);
    m_stdBtnSizer137->Realize();

    SetName(wxT("GetTextFromUserBaseDialog"));
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
}

GetTextFromUserBaseDialog::~GetTextFromUserBaseDialog() {}

NotebookNavigationDlgBase::NotebookNavigationDlgBase(wxWindow* parent, wxWindowID id, const wxString& title,
                                                     const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if(!bBitmapLoaded) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC9D6CInitBitmapResources();
        bBitmapLoaded = true;
    }

    wxBoxSizer* boxSizer157 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer157);

    m_panel161 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
                             wxWANTS_CHARS | wxTAB_TRAVERSAL);

    boxSizer157->Add(m_panel161, 1, wxEXPAND, WXC_FROM_DIP(0));

    wxBoxSizer* boxSizer163 = new wxBoxSizer(wxVERTICAL);
    m_panel161->SetSizer(boxSizer163);

    m_dvListCtrl = new gsgs::ThemedListCtrl(m_panel161, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel161, wxSize(-1, -1)),
                                        wxDV_NO_HEADER | wxDV_ROW_LINES | wxDV_SINGLE | wxWANTS_CHARS |
                                            wxTAB_TRAVERSAL | wxBORDER_STATIC);
    m_dvListCtrl->SetFocus();

    boxSizer163->Add(m_dvListCtrl, 1, wxEXPAND, WXC_FROM_DIP(2));

    m_dvListCtrl->AppendTextColumn(_("Modified"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(30), wxALIGN_CENTER,
                                   wxDATAVIEW_COL_RESIZABLE);
    m_dvListCtrl->AppendIconTextColumn(_("Text"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                       wxDATAVIEW_COL_RESIZABLE);

    SetName(wxT("NotebookNavigationDlgBase"));
    SetMinClientSize(wxSize(400, 200));
    SetSize(wxDLG_UNIT(this, wxSize(400, 200)));
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
    this->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(NotebookNavigationDlgBase::OnKeyDown), NULL, this);
    this->Connect(wxEVT_KEY_UP, wxKeyEventHandler(NotebookNavigationDlgBase::OnKeyUp), NULL, this);
    m_dvListCtrl->Connect(wxEVT_KEY_UP, wxKeyEventHandler(NotebookNavigationDlgBase::OnKeyUp), NULL, this);
    m_dvListCtrl->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(NotebookNavigationDlgBase::OnKeyDown), NULL, this);
    m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
                          wxDataViewEventHandler(NotebookNavigationDlgBase::OnItemActivated), NULL, this);
}

NotebookNavigationDlgBase::~NotebookNavigationDlgBase()
{
    this->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(NotebookNavigationDlgBase::OnKeyDown), NULL, this);
    this->Disconnect(wxEVT_KEY_UP, wxKeyEventHandler(NotebookNavigationDlgBase::OnKeyUp), NULL, this);
    m_dvListCtrl->Disconnect(wxEVT_KEY_UP, wxKeyEventHandler(NotebookNavigationDlgBase::OnKeyUp), NULL, this);
    m_dvListCtrl->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(NotebookNavigationDlgBase::OnKeyDown), NULL, this);
    m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
                             wxDataViewEventHandler(NotebookNavigationDlgBase::OnItemActivated), NULL, this);
}

EditorBarBase::EditorBarBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
    if(!bBitmapLoaded) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC9D6CInitBitmapResources();
        bBitmapLoaded = true;
    }

    wxBoxSizer* boxSizer346 = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(boxSizer346);

    wxFlexGridSizer* flexGridSizer374 = new wxFlexGridSizer(0, 3, 0, 0);
    flexGridSizer374->SetFlexibleDirection(wxBOTH);
    flexGridSizer374->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    boxSizer346->Add(flexGridSizer374, 1, wxEXPAND, WXC_FROM_DIP(5));

    m_buttonScope =
        new DefaultButton(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(200, -1)), 0);

    flexGridSizer374->Add(m_buttonScope, 1, wxRIGHT | wxEXPAND, WXC_FROM_DIP(5));

    m_buttonFilePath =
        new DefaultButton(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(200, -1)), 0);

    flexGridSizer374->Add(m_buttonFilePath, 1, wxEXPAND, WXC_FROM_DIP(5));

    m_buttonBookmarks =
        new DefaultButton(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(200, -1)), 0);

    flexGridSizer374->Add(m_buttonBookmarks, 0, wxLEFT, WXC_FROM_DIP(5));

    SetName(wxT("EditorBarBase"));
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    if(GetSizer()) { GetSizer()->Fit(this); }
    // Connect events
    m_buttonScope->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditorBarBase::OnButtonScope), NULL,
                           this);
    m_buttonFilePath->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditorBarBase::OnButtonActions),
                              NULL, this);
    m_buttonBookmarks->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditorBarBase::OnButtonBookmarks),
                               NULL, this);
}

EditorBarBase::~EditorBarBase()
{
    m_buttonScope->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditorBarBase::OnButtonScope), NULL,
                              this);
    m_buttonFilePath->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditorBarBase::OnButtonActions),
                                 NULL, this);
    m_buttonBookmarks->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
                                  wxCommandEventHandler(EditorBarBase::OnButtonBookmarks), NULL, this);
}

TableLineEditorBaseDlg::TableLineEditorBaseDlg(wxWindow* parent, wxWindowID id, const wxString& title,
                                                   const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style)
{
    if(!bBitmapLoaded) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC9D6CInitBitmapResources();
        bBitmapLoaded = true;
    }

    wxBoxSizer* boxSizer336 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer336);

    m_panel344 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);

    boxSizer336->Add(m_panel344, 1, wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* boxSizer342 = new wxBoxSizer(wxHORIZONTAL);
    m_panel344->SetSizer(boxSizer342);

    wxArrayString m_listBoxColumnsArr;
    m_listBoxColumns = new wxListBox(m_panel344, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel344, wxSize(150, -1)),
                                     m_listBoxColumnsArr, wxLB_SINGLE);

    boxSizer342->Add(m_listBoxColumns, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_stc = new wxStyledTextCtrl(m_panel344, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel344, wxSize(-1, -1)), 0);
    // Configure the fold margin
    m_stc->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
    m_stc->SetMarginMask(4, wxSTC_MASK_FOLDERS);
    m_stc->SetMarginSensitive(4, true);
    m_stc->SetMarginWidth(4, 0);

    // Configure the tracker margin
    m_stc->SetMarginWidth(1, 0);

    // Configure the symbol margin
    m_stc->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
    m_stc->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
    m_stc->SetMarginWidth(2, 0);
    m_stc->SetMarginSensitive(2, true);

    // Configure the line numbers margin
    m_stc->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_stc->SetMarginWidth(0, 0);

    // Configure the line symbol margin
    m_stc->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_stc->SetMarginMask(3, 0);
    m_stc->SetMarginWidth(3, 0);
    // Select the lexer
    m_stc->SetLexer(wxSTC_LEX_NULL);
    // Set default font / styles
    m_stc->StyleClearAll();
    m_stc->SetWrapMode(1);
    m_stc->SetIndentationGuides(0);
    m_stc->SetKeyWords(0, wxT(""));
    m_stc->SetKeyWords(1, wxT(""));
    m_stc->SetKeyWords(2, wxT(""));
    m_stc->SetKeyWords(3, wxT(""));
    m_stc->SetKeyWords(4, wxT(""));

    boxSizer342->Add(m_stc, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    SetName(wxT("TableLineEditorBaseDlg"));
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
    m_listBoxColumns->Connect(wxEVT_COMMAND_LISTBOX_SELECTED,
                              wxCommandEventHandler(TableLineEditorBaseDlg::OnColumnSelected), NULL, this);
}

TableLineEditorBaseDlg::~TableLineEditorBaseDlg()
{
    m_listBoxColumns->Disconnect(wxEVT_COMMAND_LISTBOX_SELECTED,
                                 wxCommandEventHandler(TableLineEditorBaseDlg::OnColumnSelected), NULL, this);
}