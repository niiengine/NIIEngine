//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: phprefactoringdlg.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef _CODELITE_PHPREFACTORING_PHPREFACTORINGDLG_BASE_CLASSES_H
#define _CODELITE_PHPREFACTORING_PHPREFACTORINGDLG_BASE_CLASSES_H

#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/dialog.h>
#include <wx/iconbndl.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/FilePicker.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/stc/stc.h>
#if wxVERSION_NUMBER >= 2900
#include <wx/persist.h>
#include <wx/persist/toplevel.h>
#include <wx/persist/bookctrl.h>
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


class PHPRefactoringBaseDlg : public wxDialog
{
protected:
    wxStaticText* m_staticText2;
    wxFilePickerCtrl* m_filePickerPhprefactoringPhar;
    wxCheckBox* m_checkBoxSkipPreview;
    wxStdDialogButtonSizer* m_stdBtnSizer;
    wxButton* m_buttonOK;
    wxButton* m_buttonCancel;

protected:

public:
    wxStaticText* GetStaticText2() { return m_staticText2; }
    wxFilePickerCtrl* GetFilePickerPhprefactoringPhar() { return m_filePickerPhprefactoringPhar; }
    wxCheckBox* GetCheckBoxSkipPreview() { return m_checkBoxSkipPreview; }
    PHPRefactoringBaseDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("PHP Refactoring Options"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    virtual ~PHPRefactoringBaseDlg();
};


class PHPRefactoringPreviewBaseDlg : public wxDialog
{
protected:
    wxStaticText* m_staticText267;
    wxStyledTextCtrl* m_stcPreview;
    wxStdDialogButtonSizer* m_stdBtnSizer261;
    wxButton* m_button263;
    wxButton* m_button265;

protected:
    virtual void OnApplyChanges(wxCommandEvent& event) { event.Skip(); }

public:
    wxStaticText* GetStaticText267() { return m_staticText267; }
    wxStyledTextCtrl* GetStcPreview() { return m_stcPreview; }
    PHPRefactoringPreviewBaseDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("PHP Refcatoring Preview"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    virtual ~PHPRefactoringPreviewBaseDlg();
};

#endif
