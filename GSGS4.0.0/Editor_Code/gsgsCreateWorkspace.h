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

#include "gsgsPreInclude.h"
#include "gsgsButton.h"
#include "gsgsScrolledPanel.h"
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/dialog.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/dirdlg.h>
#if wxVERSION_NUMBER >= 2900
#include <wx/persist.h>
#include <wx/persist/toplevel.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/treebook.h>
#endif

namespace gsgs
{
    class TrdPartModule : public wxPanel
    {
    public:
        /** Constructor */
        TrdPartModule(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("TrdPartModule"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~TrdPartModule();

        void initItem(const wxArrayString& options, const WindowList & optionPanels, int selection = 0);

        const wxString & GetFilePath() const { return m_workspacePath; }

        Notebook * GetListCtrl() const { return mNotebook; }

    protected:
        void DoInitialise();
    protected:
        wxPanel* m_panelWorkspace;
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxStaticText* m_staticText4;
        wxStaticText* m_staticText5;
        wxStaticText* m_staticText6;
        wxTextCtrl* m_textCtrlWorkspaceName;
        wxTextCtrl* m_textCtrlBuildmPath;
        wxTextCtrl* m_textCtrlSystemPath;
        wxTextCtrl* m_textCtrlTempPath;
        wxComboBox* m_comboBoxPath;
        Button* m_buttonWorkspaceDirPicker;
        wxStaticText* m_staticTextWorkspaceFileName;
        wxStaticText* m_staticTextBuildPath;
        wxStaticText* m_staticTextSystemPath;
        wxStaticText* m_staticTextTempFileName;
        wxCheckBox* m_checkBoxCreateSeparateDir;
        Notebook* mNotebook;
        wxWindow * mOkButton;
        wxWindow * mCancelButton;
        wxArrayString m_options;
        WindowList m_optionPanels;
        bool mCreate;
    protected:
        virtual void OnWorkspacePathUpdated(wxCommandEvent& event);
        virtual void OnWorkspaceDirPicker(wxCommandEvent& event);
        virtual void OnButtonCreate(wxCommandEvent& event);
        virtual void OnOKUI(wxUpdateUIEvent& event);

    public:
        wxStaticText* GetStaticText1() const { return m_staticText1; }
        wxTextCtrl* GetTextCtrlWorkspaceName() const { return m_textCtrlWorkspaceName; }
        wxStaticText* GetStaticText3() const { return m_staticText3; }
        wxComboBox* GetComboBoxPath() const { return m_comboBoxPath; }
        Button* GetButtonWorkspaceDirPicker() const { return m_buttonWorkspaceDirPicker; }
        wxStaticText* GetStaticTextWorkspaceFileName() const { return m_staticTextWorkspaceFileName; }
        wxCheckBox* GetCheckBoxCreateSeparateDir() const { return m_checkBoxCreateSeparateDir; }
        wxPanel* GetPanelWorkspace() const { return m_panelWorkspace; }
    protected:
        wxString m_workspacePath;
    };

    /*
    @version
    */
    class CreateWorkspaceDialog : public SingleChoiceDialog
    {
    public:
        CreateWorkspaceDialog(wxWindow* parent);
        virtual ~CreateWorkspaceDialog();

        void CreateWorkspaceDialog::OnButtonCreate(wxCommandEvent& event);

        bool isUnChoice() const;
    protected:
        bool mCreate;
    };
}