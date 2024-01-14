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

#ifndef _GSGS_ABOUT_H
#define _GSGS_ABOUT_H

#include "gsgsPreInclude.h"
#include "gsgsToolsetManager.h"
#include "gsgsNotebook.h"
#include "gsgsButton.h"
#include "gsgsEditor.h"
#include "gsgsWizard.h"
#include <wx/dialog.h>
#include <wx/statbmp.h>
#include <wx/stc/stc.h>
#include <wx/stattext.h>
#include <wx/hyperlink.h>
#include <wx/bannerwindow.h>
#include <wx/commandlinkbutton.h>

namespace gsgs
{
    //--------------------------------------------------------------------------------
    // wxBusyInfo
    //                  Displays progress information
    //                  Can be used in exactly same way as wxBusyCursor
    //--------------------------------------------------------------------------------

    class BusyInfo : public wxObject
    {
    public:
        BusyInfo(const wxString& message, wxWindow *parent = NULL);

        virtual ~BusyInfo();

    private:
        wxFrame *m_InfoFrame;

        wxDECLARE_NO_COPY_CLASS(BusyInfo);
    };

    class BootstrapData
    {
    public:
        wxString selectedTheme;
        ToolsetLocator::CompilerVec_t compilers;
        int whitespaceVisibility;
        bool useTabs;
    };

    class BootstrapWizard : public Wizard
    {
    public:
        BootstrapWizard(wxWindow* parent, int page);
        virtual ~BootstrapWizard();

        wxBannerWindow* GetBanner81() { return m_banner81; }
        wxCommandLinkButton* GetCmdLnkBtn107() { return m_cmdLnkBtn107; }
        WizardPageSimple* GetWizardPageWelcome() { return m_wizardPageWelcome; }
        wxBannerWindow* GetBanner811() { return m_banner811; }
        wxRadioBox* GetRadioBoxProfile() { return m_radioBoxProfile; }
        WizardPageSimple* GetWizardPagePlugins() { return m_wizardPagePlugins; }
        wxBannerWindow* GetBanner69() { return m_banner69; }
        wxCommandLinkButton* GetCmdLnkBtnScanForCompilers() { return m_cmdLnkBtnScanForCompilers; }
        wxCommandLinkButton* GetCmdLnkBtnDownloadCompiler() { return m_cmdLnkBtnDownloadCompiler; }
        wxDataViewListCtrl* GetDvListCtrlCompilers() { return m_dvListCtrlCompilers; }
        //WizardPageSimple* GetWizardPageCompilers() { return m_wizardPageCompilers; }
        wxBannerWindow* GetBanner71() { return m_banner71; }
        wxStaticText* GetStaticText122() { return m_staticText122; }
        Choice* GetThemePicker() { return m_themePicker; }
        //wxStyledTextCtrl* GetStc24()                        { return m_stc24; }
        WizardPageSimple* GetWizardPageColoursAndFonts() { return m_wizardPageColoursAndFonts; }
        wxBannerWindow* GetBanner73() { return m_banner73; }
        wxRadioBox* GetRadioBoxSpacesVsTabs() { return m_radioBoxSpacesVsTabs; }
        wxRadioBox* GetRadioBoxWhitespaceVisibility() { return m_radioBoxWhitespaceVisibility; }
        //WizardPageSimple* GetWizardPageWhitespace() { return m_wizardPageWhitespace; }

        WizardPageSimple* GetFirstPage() const
        {
            if (!m_pages.empty()) return m_pages.at(0);
            return NULL;
        }
        WizardPageSimple* GetPage(int i) const
        {
            if (!m_pages.empty()) return m_pages.at(i);
            return NULL;
        }

        BootstrapData GetData();
        wxArrayString GetSelectedPlugins();
        bool GetUnSelectedPlugins(wxArrayString& plugins);

        bool IsRestartRequired();
    protected:
        virtual void OnCancelWizard(wxCommandEvent& event);
        virtual void OnFinish(WizardEvent& event);
        virtual void OnInstallCompiler(wxCommandEvent& event);
        virtual void OnInstallCompilerUI(wxUpdateUIEvent& event);
        virtual void OnScanForCompilers(wxCommandEvent& event);
        virtual void OnThemeSelected(wxCommandEvent& event);
    public:
        int m_developmentProfile = 0;
    private:
        std::vector<WizardPageSimple*> m_pages;

        WizardPageSimple* m_wizardPageWelcome;
        wxBannerWindow* m_banner81;
        wxCommandLinkButton* m_cmdLnkBtn107;
        WizardPageSimple* m_wizardPagePlugins;
        wxBannerWindow* m_banner811;
        wxRadioBox* m_radioBoxProfile;
        //WizardPageSimple* m_wizardPageCompilers;
        wxBannerWindow* m_banner69;
        wxCommandLinkButton* m_cmdLnkBtnScanForCompilers;
        wxCommandLinkButton* m_cmdLnkBtnDownloadCompiler;
        wxDataViewListCtrl* m_dvListCtrlCompilers;
        WizardPageSimple* m_wizardPageColoursAndFonts;
        wxBannerWindow* m_banner71;
        wxStaticText* m_staticText122;
        Choice* m_themePicker;
        //wxStyledTextCtrl* m_stc24;
        EditorScrolledPanel * m_stc24;
        //WizardPageSimple* m_wizardPageWhitespace;
        wxBannerWindow* m_banner73;
        wxRadioBox* m_radioBoxSpacesVsTabs;
        wxRadioBox* m_radioBoxWhitespaceVisibility;

        ToolsetLocator::CompilerVec_t m_compilers;
        wxString m_selectedTheme;
        bool m_globalThemeChanged = false;
    };

    class AboutDialog : public wxDialog
    {
    public:
        wxStaticBitmap* GetBitmap424() { return m_bitmap424; }
        wxPanel* GetPanelAbout() { return m_panelAbout; }
        wxStyledTextCtrl* GetStcLicense() { return m_stcLicense; }
        wxPanel* GetPaneLicense() { return m_paneLicense; }
        Notebook* GetNotebook8() { return m_notebook8; }
        wxStaticText* GetStaticText40() { return m_staticText40; }
        wxGenericHyperlinkCtrl* GetHyperLink44() { return m_hyperLink44; }
        wxStaticText* GetStaticText2() { return m_staticText2; }
        wxGenericHyperlinkCtrl* GetHyperlink2() { return m_hyperlink2; }
        wxStaticText* GetStaticText36() { return m_staticText36; }
        wxStaticText* GetStaticTextVersion() { return m_staticTextVersion; }
        Button* GetButtonOk() { return m_buttonOk; }
        AboutDialog(wxWindow* parent, const wxString& version, wxWindowID id = wxID_ANY, const wxString& title = _("About π‚’§π  ¬"),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = gsgsDlgSize, long style = wxSIMPLE_BORDER);
        virtual ~AboutDialog();
    protected:
        virtual void OnOK(wxCommandEvent& event) { event.Skip(); }
    protected:
        Notebook* m_notebook8;
        wxPanel* m_panelAbout;
        wxStaticBitmap* m_bitmap424;
        wxPanel* m_paneLicense;
        wxPanel* m_paneUpdate;
        wxPanel* m_paneWizard;
        EditorScrolledPanel * mESP;
        Editor* m_stcLicense;
        Button * mUpdateButton;
        Button * mWizardButton;
        wxStaticText* m_staticText40;
        wxGenericHyperlinkCtrl* m_hyperLink44;
        wxStaticText* m_staticText2;
        wxGenericHyperlinkCtrl* m_hyperlink2;
        wxStaticText* m_staticText36;
        wxStaticText* m_staticTextVersion;
        Button* m_buttonOk;
    };
}
#endif
