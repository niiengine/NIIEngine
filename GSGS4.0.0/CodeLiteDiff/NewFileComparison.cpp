#include "NewFileComparison.h"
#include <wx/filedlg.h>
#include <algorithm>
#include "ieditor.h"
#include "globals.h"
#include "imanager.h"

NewFileComparison::NewFileComparison(wxWindow* parent, const wxFileName& leftFile, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    wxBoxSizer* boxSizer6 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer6);
    
    wxFlexGridSizer* flexGridSizer23 = new wxFlexGridSizer(0, 3, 0, 0);
    flexGridSizer23->SetFlexibleDirection( wxBOTH );
    flexGridSizer23->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer23->AddGrowableCol(1);
    
    boxSizer6->Add(flexGridSizer23, 0, wxEXPAND, 5);
    
    m_staticText27 = new wxStaticText(this, wxID_ANY, _("Left FIle:"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    flexGridSizer23->Add(m_staticText27, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    
    m_textCtrlLeftFile = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1,-1), 0);
    m_textCtrlLeftFile->Enable(false);
    #if wxVERSION_NUMBER >= 3000
    m_textCtrlLeftFile->SetHint(wxT(""));
    #endif
    
    flexGridSizer23->Add(m_textCtrlLeftFile, 0, wxALL|wxEXPAND, 5);
    
    flexGridSizer23->Add(0, 0, 1, wxALL, 5);
    
    m_staticText25 = new wxStaticText(this, wxID_ANY, _("Right File:"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    flexGridSizer23->Add(m_staticText25, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    
    m_textCtrlFileName = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1,-1), 0);
    m_textCtrlFileName->SetFocus();
    #if wxVERSION_NUMBER >= 3000
    m_textCtrlFileName->SetHint(wxT(""));
    #endif
    m_textCtrlFileName->AutoCompleteFileNames();
    
    flexGridSizer23->Add(m_textCtrlFileName, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5);
    
    m_button22 = new wxButton(this, wxID_ANY, _("..."), wxDefaultPosition, wxSize(-1,-1), wxBU_EXACTFIT);
    m_button22->SetToolTip(_("Choose a file..."));
    
    flexGridSizer23->Add(m_button22, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    
    m_staticText33 = new wxStaticText(this, wxID_ANY, _("Opened files:"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    boxSizer6->Add(m_staticText33, 0, wxALL|wxALIGN_LEFT, 5);
    
    wxArrayString m_listBox16Arr;
    m_listBox16 = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), m_listBox16Arr, wxLB_SINGLE);
    
    boxSizer6->Add(m_listBox16, 1, wxALL|wxEXPAND, 5);
    
    m_stdBtnSizer8 = new wxStdDialogButtonSizer();
    
    boxSizer6->Add(m_stdBtnSizer8, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 10);
    
    m_button10 = new wxButton(this, wxID_OK, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_button10->SetDefault();
    m_stdBtnSizer8->AddButton(m_button10);
    
    m_button12 = new wxButton(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_stdBtnSizer8->AddButton(m_button12);
    m_stdBtnSizer8->Realize();
    
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }

    // Connect events
    m_button22->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewFileComparison::OnBrowse), NULL, this);
    m_listBox16->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(NewFileComparison::OnFileSelected), NULL, this);
    m_button10->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewFileComparison::OnOKUI), NULL, this);
    
    gsgs::IEditor::List_t editors;
    GetManager()->GetAllEditors(editors);
    m_textCtrlLeftFile->ChangeValue(leftFile.GetFullPath());
    std::for_each(editors.begin(), editors.end(), [&](gsgs::IEditor* editor) {
        m_listBox16->Append(editor->GetFileName().GetFullPath());
    });
}

NewFileComparison::~NewFileComparison() 
{
    m_button22->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewFileComparison::OnBrowse), NULL, this);
    m_listBox16->Disconnect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(NewFileComparison::OnFileSelected), NULL, this);
    m_button10->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewFileComparison::OnOKUI), NULL, this);
}

void NewFileComparison::OnBrowse(wxCommandEvent& event)
{
    static wxString lastPath;
    wxString initialPath;
    m_textCtrlFileName->IsEmpty() ? initialPath = lastPath : initialPath =
                                                                 wxFileName(m_textCtrlFileName->GetValue()).GetPath();
    wxString file = wxFileSelector(_("Select file:"), initialPath);
    if(!file.IsEmpty()) {
        wxFileName selectedFile(file);
        lastPath = selectedFile.GetPath();
        m_textCtrlFileName->ChangeValue(selectedFile.GetFullPath());
    }
}

void NewFileComparison::OnOKUI(wxUpdateUIEvent& event)
{
    wxFileName fn(m_textCtrlFileName->GetValue());
    event.Enable(fn.Exists() || m_textCtrlFileName->GetValue().StartsWith("new"));
}

void NewFileComparison::OnFileSelected(wxCommandEvent& event)
{
    wxFileName fn(m_listBox16->GetStringSelection());
    m_textCtrlFileName->ChangeValue(fn.GetFullPath());
}
