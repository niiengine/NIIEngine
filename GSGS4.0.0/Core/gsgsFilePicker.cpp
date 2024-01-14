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
#include "gsgsFilePicker.h"
#include <wx/dirdlg.h>
#include "gsgsLogManager.h"

namespace gsgs
{
    //-------------------------------------------------------------------------------
    BEGIN_EVENT_TABLE(DirPicker, wxPanel)
    EVT_TEXT(wxID_ANY, DirPicker::OnText)
    END_EVENT_TABLE()
    //-------------------------------------------------------------------------------
    DirPicker::DirPicker(wxWindow* parent,
                         wxWindowID id,
                         const wxString& buttonCaption,
                         const wxString& defaultPos,
                         const wxString& message,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style)
        : wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL | wxNO_BORDER)
        , m_path(NULL)
        , m_combo(NULL)
        , m_buttonCaption(buttonCaption)
        , m_dlgCaption(message)
        , m_style(style)
        , m_defaultPos(defaultPos)
    {
        CreateControls();
        ConnectEvents();
    }
    //-------------------------------------------------------------------------------
    DirPicker::~DirPicker() {}
    //-------------------------------------------------------------------------------
    void DirPicker::OnText(wxCommandEvent& event) { GetParent()->GetEventHandler()->ProcessEvent(event); }
    //-------------------------------------------------------------------------------
    void DirPicker::CreateControls()
    {
        wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
        SetSizer(mainSizer);

        size_t flags = wxRIGHT | wxTOP | wxBOTTOM | wxEXPAND;

        if(m_style & wxDP_USE_TEXTCTRL) {
            m_path = gsgs_Style().createTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
            mainSizer->Add(m_path, 1, flags, 5);
        } else {
            m_combo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
            mainSizer->Add(m_combo, 1, flags, 5);
        }

        m_button = new Button(this, wxID_ANY, m_buttonCaption, wxDefaultPosition, wxDefaultSize);
        m_button->SetToolTip(_("Browse for folder..."));
        mainSizer->Add(m_button, 0, wxEXPAND | wxALL, 5);

        Layout();
    }
    //-------------------------------------------------------------------------------
    void DirPicker::ConnectEvents() { ConnectButton(m_button, DirPicker::OnButtonClicked); }
    //-------------------------------------------------------------------------------
    void DirPicker::OnButtonClicked(wxCommandEvent& event)
    {
        wxUnusedVar(event);

        // no default position was provided by the user
        // try to take it from the text control
        wxString work_dir = GetPath();
        m_defaultPos.Empty();
        if(work_dir.IsEmpty() == false) {
            if(wxDir::Exists(work_dir)) {
                wxFileName fn(work_dir, wxEmptyString);
                gsgsLogMessage(work_dir + wxT(" exists"));
                fn.MakeAbsolute();
                m_defaultPos = fn.GetFullPath();
            }
        }

        if(m_defaultPos.IsEmpty()) {
            m_defaultPos = wxGetCwd();
        }

        gsgsLogMessage(wxT("setting working dir to : ") + m_defaultPos);

        wxDirDialog* dlg = new wxDirDialog(this, m_dlgCaption, m_defaultPos);
        if(dlg->ShowModal() == wxID_OK) {
            // Get the dirname
            wxString path = dlg->GetPath();
            SetPath(path);
        }
        dlg->Destroy();
    }
    //-------------------------------------------------------------------------------
    wxString DirPicker::GetPath()
    {
        if(m_style & wxDP_USE_TEXTCTRL)
            return m_path->GetValue();
        else {
            wxString path(m_combo->GetValue());
            // if the path is not whitin the combobox list, add it
            if(m_combo->FindString(path) == wxNOT_FOUND) {
                SetPath(path);
            }
            return path;
        }
    }
    //-------------------------------------------------------------------------------
    void DirPicker::SetPath(const wxString& path)
    {
        if(m_style & wxDP_USE_TEXTCTRL)
            m_path->SetValue(path);
        else {
            int where = m_combo->FindString(path);
            if(where != wxNOT_FOUND) {
                m_combo->SetSelection(where);
            } else {
                where = m_combo->Append(path);
                m_combo->SetSelection(where);
            }
        }
    }
    //-------------------------------------------------------------------------------
    void DirPicker::SetValues(const wxArrayString& values, int sel)
    {
        wxASSERT_MSG(m_style & wxDP_USE_COMBOBOX, wxT("SetValues is available only for wxDP_USE_COMBOBOX style"));
        m_combo->Append(values);
        m_combo->SetSelection(sel);
    }
    //-------------------------------------------------------------------------------
    wxArrayString DirPicker::GetValues() const
    {
        wxArrayString dummy;
        if(m_style & wxDP_USE_COMBOBOX) {
            return m_combo->GetStrings();
        } else {
            return dummy;
        }
    }
    //-------------------------------------------------------------------------------
    int DirPicker::GetCurrentSelection() const
    {
        wxCHECK_MSG(m_style & wxDP_USE_COMBOBOX,
                    wxNOT_FOUND,
                    wxT("GetCurrentSelection is available only for wxDP_USE_COMBOBOX style"));

        int index = wxNOT_FOUND;
        wxString stringsel = m_combo->GetValue();
        if(!stringsel.empty()) {
            index = m_combo->FindString(stringsel);
        }

        return index;
    }
    //-------------------------------------------------------------------------------
    FilePicker::FilePicker(wxWindow *parent, 
                           wxWindowID id, 
                           const wxString &defaultFile,
                           const wxString &message, 
                           const wxString &wildCard, 
                           const wxString &buttonCaption, 
                           const wxPoint& pos, 
                           const wxSize& size, 
                           long style)
    : wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL | wxNO_BORDER)
    , m_buttonCaption(buttonCaption)
    , m_dlgCaption(message)
    , m_dlgStyle(style)
    , m_defaultFile(defaultFile)
    , m_wildCard(wildCard)
    {

    #if defined(__WXGTK__) || defined (__WXMAC__)
        if(m_wildCard == wxT("*.*")){
            m_wildCard = wxT("*");
        }
    #endif

        CreateControls();
        ConnectEvents();
    }
    //-------------------------------------------------------------------------------
    FilePicker::~FilePicker()
    {
    }
    //-------------------------------------------------------------------------------
    void FilePicker::CreateControls()
    {
        wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
        SetSizer(mainSizer);

        size_t flags = wxRIGHT | wxTOP | wxBOTTOM | wxALIGN_CENTER | wxEXPAND;
        
        m_path = gsgs_Style().createTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
        mainSizer->Add(m_path, 1, flags, 5);

        m_button = new Button(this, wxID_ANY, m_buttonCaption);
        mainSizer->Add(m_button, 0, wxALL, 5);

        Layout();
    }
    //-------------------------------------------------------------------------------
    void FilePicker::ConnectEvents()
    {
        ConnectButton(m_button, FilePicker::OnButtonClicked);
    }
    //-------------------------------------------------------------------------------
    void FilePicker::OnButtonClicked(wxCommandEvent &event)
    {
        wxUnusedVar(event);
        wxFileDialog *dlg = new wxFileDialog(this, m_dlgCaption, wxEmptyString, m_defaultFile, m_wildCard, m_dlgStyle);
        if(dlg->ShowModal() == wxID_OK)
        {
            // Get the dirname
            wxString path = dlg->GetPath();
            m_path->SetValue(path);
        }
        dlg->Destroy();
    }
    //-------------------------------------------------------------------------------
}