#ifndef NEWFILECOMPARISON_H
#define NEWFILECOMPARISON_H

#include "gsgsPreInclude.h"
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/dialog.h>
#include <wx/iconbndl.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/listbox.h>
#if wxVERSION_NUMBER >= 2900
#include <wx/persist.h>
#include <wx/persist/toplevel.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/treebook.h>
#endif
#include <wx/filename.h>

class NewFileComparison : public wxDialog
{
public:
    NewFileComparison(wxWindow* parent, const wxFileName& leftFile, wxWindowID id = wxID_ANY, const wxString& title = _("Select File for Comparison"), 
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = gsgsDlgSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    virtual ~NewFileComparison();
    
    wxStaticText* GetStaticText27() { return m_staticText27; }
    wxTextCtrl* GetTextCtrlLeftFile() { return m_textCtrlLeftFile; }
    wxStaticText* GetStaticText25() { return m_staticText25; }
    wxTextCtrl* GetTextCtrlFileName() { return m_textCtrlFileName; }
    wxButton* GetButton22() { return m_button22; }
    wxStaticText* GetStaticText33() { return m_staticText33; }
    wxListBox* GetListBox16() { return m_listBox16; }

protected:
    virtual void OnFileSelected(wxCommandEvent& event);
    virtual void OnBrowse(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
protected:
    wxStaticText* m_staticText27;
    wxTextCtrl* m_textCtrlLeftFile;
    wxStaticText* m_staticText25;
    wxTextCtrl* m_textCtrlFileName;
    wxButton* m_button22;
    wxStaticText* m_staticText33;
    wxListBox* m_listBox16;
    wxStdDialogButtonSizer* m_stdBtnSizer8;
    wxButton* m_button10;
    wxButton* m_button12;
};
#endif // NEWFILECOMPARISON_H
