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

#ifndef gsgsCommonDialog_H
#define gsgsCommonDialog_H

#include "gsgsPreInclude.h"
#include "wxcrafter_plugin.h"
#include "gsgsStyleManager.h"
#include "gsgsWorkspace.h"
#include "NewProjectDialog.h"
#include <wx/colour.h>
#include <wx/font.h>

namespace gsgs
{
    class _gsgsAPI EditDlg : public wxDialog
    {
    public:
        EditDlg(wxWindow* parent, const wxString &text, wxWindowID id = wxID_ANY, const wxString& title = _("Edit Text"),
                    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(300, 300),
                    long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
        virtual ~EditDlg();
        
                wxStyledTextCtrl* GetStc10() { return m_stc10; }
        Button* GetButton6() { return m_button6; }
        Button* GetButton8() { return m_button8; }
        
        wxString GetText() const {
            return m_stc10->GetText();
        }
        
        static wxString GetUserString(const wxString &initialValue, wxWindow* parent = NULL);
    protected:
        wxStyledTextCtrl* m_stc10;
        Button* m_button6;
        Button* m_button8;
    };
    
    class EnvVarImporterDlg : public wxDialog
    {
    public:
        EnvVarImporterDlg(wxWindow* parent,
                          const wxString& projectName,
                          const wxString& cfgName,
                          std::set<wxString> listEnvVar,
                          BuildConfigPtr le_conf,
                          bool* showDlg);
        virtual ~EnvVarImporterDlg();

        wxStaticText* GetStaticText26() { return m_staticText26; }
        wxStaticText* GetProjectName() { return m_projectName; }
        wxStaticText* GetStaticText20() { return m_staticText20; }
        wxStaticText* GetConfName() { return m_confName; }
        wxStaticText* GetStaticText6() { return m_staticText6; }
        wxTextCtrl* GetEnvVars() { return m_envVars; }
        gsgs::Button* GetBtnImport() { return m_btnImport; }
        gsgs::Button* GetBtnContinue() { return m_btnContinue; }
        gsgs::Button* GetBtnNotImport() { return m_btnNotImport; }
    protected:
        virtual void OnImport(wxCommandEvent& event);
        virtual void OnContinue(wxCommandEvent& event);
        virtual void OnSkip(wxCommandEvent& event);
    private:
        wxBoxSizer* boxSizer4;
        wxBoxSizer* boxSizer18;
        wxStaticText* m_staticText26;
        wxStaticText* m_projectName;
        wxStaticText* m_staticText20;
        wxStaticText* m_confName;
        wxBoxSizer* boxSizer22;
        wxStaticText* m_staticText6;
        wxTextCtrl* m_envVars;
        wxBoxSizer* boxSizer16;
        gsgs::Button* m_btnImport;
        gsgs::Button* m_btnContinue;
        gsgs::Button* m_btnNotImport;
        BuildConfigPtr le_conf;
        bool* showDlg;
    };
    
    class TableLineEditorDlg : public TableLineEditorBaseDlg
    {
    public:
        TableLineEditorDlg(wxWindow* parent, const wxArrayString& columns, const wxArrayString& data);
        virtual ~TableLineEditorDlg();

    protected:
        virtual void OnColumnSelected(wxCommandEvent& event);
        void DoItemSelected(int index);
    private:
        const wxArrayString& m_columns;
        const wxArrayString& m_data;
    };
    
    class DiffSelectFoldersDlg : public wxDialog
    {
    public:
        DiffSelectFoldersDlg(wxWindow* parent, const wxString& left = "", const wxString& right = "", 
            wxWindowID id = wxID_ANY, const wxString& title = _("Select Folder"),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500, 300),
            long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
        virtual ~DiffSelectFoldersDlg();

        wxStaticText* GetStaticText54() { return m_staticText54; }
        wxDirPickerCtrl* GetDirPickerLeft() { return m_dirPickerLeft; }
        wxDirPickerCtrl* GetDirPickerRight() { return m_dirPickerRight; }
        wxPanel* GetPanel46() { return m_panel46; }
    protected:
        virtual void OnOKUI(wxUpdateUIEvent& event);
    private:
        wxPanel* m_panel46;
        wxStaticText* m_staticText54;
        wxDirPickerCtrl* m_dirPickerLeft;
        wxDirPickerCtrl* m_dirPickerRight;
        DialogButtonSizer* m_stdBtnSizer40;
        Button* m_buttonOK;
        Button* m_buttonCancel;
    };

    class _gsgsAPI EditorBar : public EditorBarBase
    {
    public:
        EditorBar(wxWindow* parent);
        virtual ~EditorBar();
        void SetMessage(const wxString& className, const wxString& function);
        void DoShow(bool s);

    protected:
        virtual void OnButtonActions(wxCommandEvent& event);
        virtual void OnButtonBookmarks(wxCommandEvent& event);
        virtual void OnButtonScope(wxCommandEvent& event);
        void OnMarkerChanged(Event& event);
        void OnEditorChanged(wxCommandEvent& e);
        void OnTheme(wxCommandEvent& e);

    private:
        void DoRefreshColoursAndFonts();
        void DoRefresh();
        void CreateBookmarksBitmap();

    private:
        wxString m_classname;
        wxString m_function;
        wxFont m_textFont;
        wxRect m_scopeRect;
        wxBitmap m_functionBmp;

        // Breadcrumbs
        wxString m_filename;
        wxString m_filenameRelative;
        wxString m_projectFile;
        wxString m_projectName;
        wxArrayString m_breadcrumbs;

        wxRect m_filenameRect;

        // Bookmarks button
        wxBitmap m_bookmarksBmp;
        wxRect m_bookmarksRect;
        std::vector<std::pair<int, wxString> > m_bookmarks;

        bool m_shouldShow = false;
    };
}
#endif
