//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: NewToolBase.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef _CODELITE_FORMBUILDER_NEWTOOLBASE_BASE_CLASSES_H
#define _CODELITE_FORMBUILDER_NEWTOOLBASE_BASE_CLASSES_H

#include <wx/arrstr.h>
#include <wx/artprov.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/iconbndl.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/xrc/xmlres.h>
#if wxVERSION_NUMBER >= 2900
#include <wx/persist.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/toplevel.h>
#include <wx/persist/treebook.h>
#endif

#ifdef WXC_FROM_DIP
#undef WXC_FROM_DIP
#endif
#if wxVERSION_NUMBER >= 3100
#define WXC_FROM_DIP(x) wxWindow::FromDIP(x, NULL)
#else
#define WXC_FROM_DIP(x) x
#endif

class NewToolBase : public wxDialog
{
protected:
    wxStaticText* m_staticText5;
    wxChoice* m_choiceId;
    wxStaticText* m_staticText6;
    wxTextCtrl* m_textCtrlName;
    wxStaticText* m_staticText1;
    wxTextCtrl* m_textCtrlPath;
    wxButton* m_buttonBrowsePath;
    wxStaticText* m_staticText2;
    wxTextCtrl* m_textCtrlWd;
    wxButton* m_buttonBrowseWd;
    wxStaticText* m_staticText8;
    wxTextCtrl* m_textCtrlIcon24;
    wxButton* m_buttonBrowseIcon24;
    wxStaticText* m_staticText7;
    wxTextCtrl* m_textCtrlIcon16;
    wxButton* m_buttonBrowseIcon16;
    wxCheckBox* m_checkBoxSaveAllFilesBefore;
    wxCheckBox* m_checkBoxCaptureOutput;
    wxCheckBox* m_checkBoxCallOnFileSave;
    wxStdDialogButtonSizer* m_stdBtnSizer2;
    wxButton* m_button4;
    wxButton* m_button6;
    wxButton* m_button8;

protected:
    virtual void OnIdSelected(wxCommandEvent& event) { event.Skip(); }
    virtual void OnButtonBrowsePath(wxCommandEvent& event) { event.Skip(); }
    virtual void OnButtonBrowseWD(wxCommandEvent& event) { event.Skip(); }
    virtual void OnButtonBrowseIcon24(wxCommandEvent& event) { event.Skip(); }
    virtual void OnButtonBrowseIcon16(wxCommandEvent& event) { event.Skip(); }
    virtual void OnButtonOk(wxCommandEvent& event) { event.Skip(); }
    virtual void OnButtonOkUI(wxUpdateUIEvent& event) { event.Skip(); }
    virtual void OnButtonCancel(wxCommandEvent& event) { event.Skip(); }
    virtual void OnButtonHelp(wxCommandEvent& event) { event.Skip(); }

public:
    wxStaticText* GetStaticText5() { return m_staticText5; }
    wxChoice* GetChoiceId() { return m_choiceId; }
    wxStaticText* GetStaticText6() { return m_staticText6; }
    wxTextCtrl* GetTextCtrlName() { return m_textCtrlName; }
    wxStaticText* GetStaticText1() { return m_staticText1; }
    wxTextCtrl* GetTextCtrlPath() { return m_textCtrlPath; }
    wxButton* GetButtonBrowsePath() { return m_buttonBrowsePath; }
    wxStaticText* GetStaticText2() { return m_staticText2; }
    wxTextCtrl* GetTextCtrlWd() { return m_textCtrlWd; }
    wxButton* GetButtonBrowseWd() { return m_buttonBrowseWd; }
    wxStaticText* GetStaticText8() { return m_staticText8; }
    wxTextCtrl* GetTextCtrlIcon24() { return m_textCtrlIcon24; }
    wxButton* GetButtonBrowseIcon24() { return m_buttonBrowseIcon24; }
    wxStaticText* GetStaticText7() { return m_staticText7; }
    wxTextCtrl* GetTextCtrlIcon16() { return m_textCtrlIcon16; }
    wxButton* GetButtonBrowseIcon16() { return m_buttonBrowseIcon16; }
    wxCheckBox* GetCheckBoxSaveAllFilesBefore() { return m_checkBoxSaveAllFilesBefore; }
    wxCheckBox* GetCheckBoxCaptureOutput() { return m_checkBoxCaptureOutput; }
    wxCheckBox* GetCheckBoxCallOnFileSave() { return m_checkBoxCallOnFileSave; }
    NewToolBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("External Tool"),
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~NewToolBase();
};

#endif