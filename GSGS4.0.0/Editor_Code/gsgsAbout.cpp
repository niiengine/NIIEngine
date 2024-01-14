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

#include "gsgsAbout.h"
#include "gsgsFrame.h"

#include "wx/generic/stattextg.h"

namespace gsgs
{
    class InfoFrame : public wxFrame
    {
    public:
        InfoFrame(wxWindow *parent, const wxString& message);

    private:
        wxDECLARE_NO_COPY_CLASS(InfoFrame);
    };


    InfoFrame::InfoFrame(wxWindow *parent, const wxString& message)
        : wxFrame(parent, wxID_ANY, wxT("Busy"),
            wxDefaultPosition, wxDefaultSize,
#if defined(__WXX11__)
            wxRESIZE_BORDER
#else
            wxSIMPLE_BORDER
#endif
            | wxFRAME_TOOL_WINDOW | wxSTAY_ON_TOP)
    {
        wxPanel *panel = new wxPanel(this);
#ifdef __WXGTK__
        wxGenericStaticText *text = new wxGenericStaticText(panel, wxID_ANY, message);
        text->SetForegroundColour(gsgs_Style().GetAppTextColour());
#else
        wxStaticText *text = gsgs_Style().createStaticText(panel, wxID_ANY, message);
#endif

        panel->SetCursor(*wxHOURGLASS_CURSOR);
        text->SetCursor(*wxHOURGLASS_CURSOR);

        // make the frame of at least the standard size (400*80) but big enough
        // for the text we show
        wxSize sizeText = text->GetBestSize();
#ifdef __WXPM__
        int                             nX = 0;
        int                             nY = 0;
        int                             nWidth = 0;
        int                             nHeight = 0;
        int                             nParentHeight = parent->GetClientSize().y;
        int                             nParentWidth = parent->GetClientSize().x;
        int                             nColor;

        SetBackgroundColour(wxT("WHITE"));
        nColor = (LONG)GetBackgroundColour().GetPixel();

        ::WinSetPresParam(GetHwnd()
            , PP_BACKGROUNDCOLOR
            , sizeof(LONG)
            , (PVOID)&nColor
        );
        panel->SetBackgroundColour(wxT("WHITE"));
        nColor = (LONG)panel->GetBackgroundColour().GetPixel();

        ::WinSetPresParam(GetHwndOf(panel)
            , PP_BACKGROUNDCOLOR
            , sizeof(LONG)
            , (PVOID)&nColor
        );
        nWidth = wxMax(sizeText.x, 340) + 60;
        nHeight = wxMax(sizeText.y, 40) + 40;
        nX = (nParentWidth - nWidth) / 2;
        nY = (nParentHeight / 2) - (nHeight / 2);
        nY = nParentHeight - (nY + nHeight);
        ::WinSetWindowPos(m_hFrame
            , HWND_TOP
            , nX
            , nY
            , nWidth
            , nHeight
            , SWP_SIZE | SWP_MOVE | SWP_ACTIVATE
        );
        text->SetBackgroundColour(wxT("WHITE"));
        nColor = (LONG)text->GetBackgroundColour().GetPixel();

        ::WinSetPresParam(GetHwndOf(text)
            , PP_BACKGROUNDCOLOR
            , sizeof(LONG)
            , (PVOID)&nColor
        );
        text->Center(wxBOTH);
#else
        SetClientSize(wxMax(sizeText.x, 340) + 60, wxMax(sizeText.y, 40) + 40);

        // need to size the panel correctly first so that text->Centre() works
        panel->SetSize(GetClientSize());

        text->Centre(wxBOTH);
        Centre(wxBOTH);
#endif
    }

    BusyInfo::BusyInfo(const wxString& message, wxWindow *parent)
    {
        m_InfoFrame = new InfoFrame(parent, message);
        m_InfoFrame->SetBackgroundColour(gsgs_Style().GetAppBgColour());
        m_InfoFrame->Show(true);
        m_InfoFrame->Refresh();
        m_InfoFrame->Update();
    }

    BusyInfo::~BusyInfo()
    {
        m_InfoFrame->Show(false);
        m_InfoFrame->Close();
    }

    static void GetCxxPlugins(std::vector<wxString> & out)
    {
        //static std::vector<wxString> cxxPlugins;
        //if (cxxPlugins.empty()) {
        out.push_back("CMakePlugin");
        out.push_back("CScope");
        out.push_back("CallGraph");
        out.push_back("ContinuousBuild");
        out.push_back("CppChecker");
        out.push_back("LLDBDebuggerPlugin");
#ifdef __WXGTK__
        out.push_back("MemCheck");
#endif
        out.push_back("QMakePlugin");
        out.push_back("UnitTestPP");
        out.push_back("Wizards");
        out.push_back("wxFormBuilder");
        out.push_back("wxcrafter");
        out.push_back("EOSWiki");
        //}
        //return cxxPlugins;
    }
    //-----------------------------------------------------------------
    static void GetAllPlugins(std::vector<wxString> & out)
    {
        //static std::vector<wxString> allPlugins;
        static std::unordered_set<wxString> commonPlugins;
        if (commonPlugins.empty()) { commonPlugins.insert("Source Code Formatter"); }
        //if (allPlugins.empty()) {

            Config conf("plugins.conf");
            PluginInfoArray plugins;
            conf.ReadItem(&plugins);

            const PluginInfo::PluginMap_t& pluginsInfo = plugins.GetPlugins();
            std::for_each(pluginsInfo.begin(), pluginsInfo.end(), [&](const std::pair<wxString, PluginInfo>& item) {
                if (commonPlugins.count(item.second.GetName()) == 0) { out.push_back(item.first); }
            });
        //}
        //return allPlugins;
    }
    //-----------------------------------------------------------------
    class BootstrapWizardPluginData : public wxClientData
    {
    public:
        BootstrapWizardPluginData(const PluginInfo& pi)
            : checked(true)
            , pluginInfo(pi)
        {
        }
        virtual ~BootstrapWizardPluginData() {}

        wxString GetPluginSummary() const
        {
            wxString summary;
            summary << pluginInfo.GetName() << " " << pluginInfo.GetVersion() << "\n"
                << _("By: ") << pluginInfo.GetAuthor() << "\n\n"
                << pluginInfo.GetDescription();
            return summary;
        }
    public:
        bool checked;
        PluginInfo pluginInfo;
    };

#define DARK_THEME "Tomorrow Night"
#define LIGHT_THEME "Atom One Light"

    const wxString sampleText = "#include \"NII.h\"\n\n"
        "class Helloworld {\n"
        "public:\n"
        "    /**\n"
        "     * 创建对象\n"
        "     * @param o The object\n"
        "     */\n"
        "    Helloworld(const Helloworld &o) {\n"
        "        // Initialise with Zero\n"
        "        int number = 0;\n"
        "        m_str = o.m_str;\n"
        "    }\n"
        "    \n"
        "private:\n"
        "    std::string m_str;\n"
        "    \n"
        "};";
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // BootstrapWizard
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    BootstrapWizard::BootstrapWizard(wxWindow* parent, int page)
        : Wizard(parent, wxID_ANY, _("Welcome!"), wxNullBitmap, wxDefaultPosition, gsgsDlgStyle)
        , m_selectedTheme(LIGHT_THEME)
        , m_developmentProfile(0)
    {
        SetBackgroundColour(gsgs_Style().GetAppBgColour());

        m_wizardPageWelcome = new WizardPageSimple(this, NULL, NULL, wxNullBitmap);
        m_pages.push_back(m_wizardPageWelcome);
        wxBoxSizer* boxSizer79 = new wxBoxSizer(wxVERTICAL);
        m_wizardPageWelcome->SetSizer(boxSizer79);

        m_banner81 = new wxBannerWindow(m_wizardPageWelcome, wxID_ANY, wxTOP, wxDefaultPosition,
            wxDLG_UNIT(m_wizardPageWelcome, wxSize(-1, -1)), 0);
        m_banner81->SetBitmap(wxNullBitmap);
        m_banner81->SetText(_("Setup wizard"), wxT("This wizard will setup something to 光栅故事"));
        m_banner81->SetForegroundColour(gsgs_Style().GetAppTextColour());
        m_banner81->SetGradient(gsgs_Style().GetAppBgColour(), gsgs_Style().GetAppBgColour());
        boxSizer79->Add(m_banner81, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer105 = new wxBoxSizer(wxVERTICAL);

        boxSizer79->Add(boxSizer105, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        boxSizer105->Add(0, 0, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_cmdLnkBtn107 = new wxCommandLinkButton(
            m_wizardPageWelcome, wxID_ANY, _("Do this Later!"), _("Use default, skip the Wizard"),
            wxDefaultPosition, wxDLG_UNIT(m_wizardPageWelcome, wxSize(-1, -1)), 0);
        m_cmdLnkBtn107->wxControl::SetBackgroundColour(gsgs_Style().GetAppBgColour());
        boxSizer105->Add(m_cmdLnkBtn107, 0, wxALL, WXC_FROM_DIP(5));

        boxSizer105->Add(0, 0, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_wizardPagePlugins = new WizardPageSimple(this, NULL, NULL, wxNullBitmap);
        m_pages.push_back(m_wizardPagePlugins);

        wxBoxSizer* boxSizer89 = new wxBoxSizer(wxVERTICAL);
        m_wizardPagePlugins->SetSizer(boxSizer89);

        m_banner811 = new wxBannerWindow(m_wizardPagePlugins, wxID_ANY, wxTOP, wxDefaultPosition,
            wxDLG_UNIT(m_wizardPagePlugins, wxSize(-1, -1)), 0);
        m_banner811->SetBitmap(wxNullBitmap);
        m_banner811->SetText(_("Development Profile"), _("Select the preference"));
        m_banner811->SetForegroundColour(gsgs_Style().GetAppTextColour());
        m_banner811->SetGradient(gsgs_Style().GetAppBgColour(), gsgs_Style().GetAppBgColour());
        boxSizer89->Add(m_banner811, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer93 = new wxBoxSizer(wxHORIZONTAL);

        boxSizer89->Add(boxSizer93, 1, wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer114 = new wxBoxSizer(wxVERTICAL);

        boxSizer93->Add(boxSizer114, 1, wxEXPAND, WXC_FROM_DIP(5));

        wxArrayString m_radioBoxProfileArr;
        m_radioBoxProfileArr.Add(_("Default (Don't change the current settings)"));
        m_radioBoxProfileArr.Add(_("Both C/C++ and Web development"));
        m_radioBoxProfileArr.Add(_("C/C++ development"));
        m_radioBoxProfileArr.Add(_("C/C++ development (Blockchain using EOSIO)"));
        m_radioBoxProfileArr.Add(_("Web development (PHP, JS etc)"));
        m_radioBoxProfile =
            new wxRadioBox(m_wizardPagePlugins, wxID_ANY, wxT(""), wxDefaultPosition,
                wxDLG_UNIT(m_wizardPagePlugins, wxSize(-1, -1)), m_radioBoxProfileArr, 2, wxRA_SPECIFY_COLS);
        m_radioBoxProfile->SetSelection(0);

        boxSizer114->Add(m_radioBoxProfile, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        //m_wizardPageCompilers = new WizardPageSimple(this, NULL, NULL, wxNullBitmap);
        //m_pages.push_back(m_wizardPageCompilers);

        //wxBoxSizer* boxSizer52 = new wxBoxSizer(wxVERTICAL);
        //m_wizardPageCompilers->SetSizer(boxSizer52);

        m_banner69 = new wxBannerWindow(m_wizardPagePlugins, wxID_ANY, wxTOP, wxDefaultPosition,
            wxDLG_UNIT(m_wizardPagePlugins, wxSize(-1, -1)), 0);
        m_banner69->SetBitmap(wxNullBitmap);
        m_banner69->SetText(_("Setup compilers"), _("Configure compilers or install"));
        m_banner69->SetForegroundColour(gsgs_Style().GetAppTextColour());
        m_banner69->SetGradient(gsgs_Style().GetAppBgColour(), gsgs_Style().GetAppBgColour());
        boxSizer114->Add(m_banner69, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_cmdLnkBtnScanForCompilers = new wxCommandLinkButton(
            m_wizardPagePlugins, wxID_ANY, _("Scan"), _("Click to scan your computer for installed compilers"),
            wxDefaultPosition, wxDLG_UNIT(m_wizardPagePlugins, wxSize(-1, -1)), 0);

        boxSizer114->Add(m_cmdLnkBtnScanForCompilers, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_cmdLnkBtnDownloadCompiler =
            new wxCommandLinkButton(m_wizardPagePlugins, wxID_ANY, _("Install"), _("Click to download a MinGW compiler"),
                wxDefaultPosition, wxDLG_UNIT(m_wizardPagePlugins, wxSize(-1, -1)), 0);

        boxSizer114->Add(m_cmdLnkBtnDownloadCompiler, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_dvListCtrlCompilers = new wxDataViewListCtrl(m_wizardPagePlugins, wxID_ANY, wxDefaultPosition,
            wxDLG_UNIT(m_wizardPagePlugins, wxSize(-1, -1)),
            wxDV_VERT_RULES | wxDV_ROW_LINES | wxDV_SINGLE);
        m_dvListCtrlCompilers->Hide();

        boxSizer114->Add(m_dvListCtrlCompilers, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_dvListCtrlCompilers->AppendTextColumn(_("Compiler Name"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(150), wxALIGN_LEFT,
            wxDATAVIEW_COL_RESIZABLE);
        m_dvListCtrlCompilers->AppendTextColumn(_("Installation Path"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2),
            wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
        m_wizardPageColoursAndFonts = new WizardPageSimple(this, NULL, NULL, wxNullBitmap);
        m_pages.push_back(m_wizardPageColoursAndFonts);

        wxBoxSizer* boxSizer12 = new wxBoxSizer(wxVERTICAL);
        m_wizardPageColoursAndFonts->SetSizer(boxSizer12);

        m_banner71 = new wxBannerWindow(m_wizardPageColoursAndFonts, wxID_ANY, wxTOP, wxDefaultPosition,
            wxDLG_UNIT(m_wizardPageColoursAndFonts, wxSize(-1, -1)), 0);
        m_banner71->SetBitmap(wxNullBitmap);
        m_banner71->SetText(_("Customise colours"), _("Select the editor theme"));
        m_banner71->SetForegroundColour(gsgs_Style().GetAppTextColour());
        m_banner71->SetGradient(gsgs_Style().GetAppBgColour(), gsgs_Style().GetAppBgColour());
        boxSizer12->Add(m_banner71, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxFlexGridSizer* flexGridSizer118 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer118->SetFlexibleDirection(wxBOTH);
        flexGridSizer118->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer118->AddGrowableCol(1);

        boxSizer12->Add(flexGridSizer118, 0, wxEXPAND, WXC_FROM_DIP(5));

        m_staticText122 = gsgs_Style().createStaticText(m_wizardPageColoursAndFonts, wxID_ANY, _("Theme:"), wxDefaultPosition,
            wxDLG_UNIT(m_wizardPageColoursAndFonts, wxSize(-1, -1)), 0);
        flexGridSizer118->Add(m_staticText122, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxArrayString m_themePickerArr;
        m_themePickerArr.Add(wxT("仲夏夜"));
        //m_themePickerArr.Add(wxT("Dark"));
        //m_themePickerArr.Add(wxT("Light"));
        m_themePicker = new Choice(m_wizardPageColoursAndFonts, wxID_ANY, wxDefaultPosition,
            wxDefaultSize, m_themePickerArr, 0);
        m_themePicker->SetToolTip(_("Set the IDE global colour\nThis colour affects various controls"));
        m_themePicker->SetSelection(0);

        flexGridSizer118->Add(m_themePicker, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        /*m_stc24 = new wxStyledTextCtrl(m_wizardPageColoursAndFonts, wxID_ANY, wxDefaultPosition,
                                       wxDLG_UNIT(m_wizardPageColoursAndFonts, wxSize(-1, -1)), 0);*/
        m_stc24 = new EditorScrolledPanel(m_wizardPageColoursAndFonts);
        m_stc24->Show();
        boxSizer12->Add(m_stc24, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));
        Editor * m_stc241 = m_stc24->getEditor();
        m_stc241->SetZoom(1);

        //m_wizardPageWhitespace = new WizardPageSimple(this, NULL, NULL, wxNullBitmap);
        //m_pages.push_back(m_wizardPageWhitespace);

        //wxBoxSizer* boxSizer30 = new wxBoxSizer(wxVERTICAL);
        //m_wizardPageWhitespace->SetSizer(boxSizer30);

        m_banner73 = new wxBannerWindow(m_wizardPageColoursAndFonts, wxID_ANY, wxTOP, wxDefaultPosition,
            wxDLG_UNIT(m_wizardPageColoursAndFonts, wxSize(-1, -1)), 0);
        m_banner73->SetBitmap(wxNullBitmap);
        m_banner73->SetText(_("Whitespace & Indentation"), _("Use TABS or SPACES for indentation?"));
        m_banner73->SetForegroundColour(gsgs_Style().GetAppTextColour());
        m_banner73->SetGradient(gsgs_Style().GetAppBgColour(), gsgs_Style().GetAppBgColour());
        boxSizer12->Add(m_banner73, 0, wxALL, WXC_FROM_DIP(5));

        //boxSizer12->Add(0, 0, 1, wxALL, WXC_FROM_DIP(5));

        wxArrayString m_radioBoxSpacesVsTabsArr;
        m_radioBoxSpacesVsTabsArr.Add(_("Indent using SPACES"));
        m_radioBoxSpacesVsTabsArr.Add(_("Indent using TABS"));
        m_radioBoxSpacesVsTabs = new wxRadioBox(m_wizardPageColoursAndFonts, wxID_ANY, _("Indentation"), wxDefaultPosition,
            wxDLG_UNIT(m_wizardPageColoursAndFonts, wxSize(200, -1)),
            m_radioBoxSpacesVsTabsArr, 1, wxRA_SPECIFY_COLS);
        m_radioBoxSpacesVsTabs->SetForegroundColour(gsgs_Style().GetAppTextColour());
        m_radioBoxSpacesVsTabs->SetSelection(0);

        boxSizer12->Add(m_radioBoxSpacesVsTabs, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxArrayString m_radioBoxWhitespaceVisibilityArr;
        m_radioBoxWhitespaceVisibilityArr.Add(_("Invisible"));
        m_radioBoxWhitespaceVisibilityArr.Add(_("Visible always"));
        m_radioBoxWhitespaceVisibilityArr.Add(_("Visible after indentation"));
        m_radioBoxWhitespaceVisibility = new wxRadioBox(
            m_wizardPageColoursAndFonts, wxID_ANY, _("Whitespace Visibility:"), wxDefaultPosition,
            wxDLG_UNIT(m_wizardPageColoursAndFonts, wxSize(200, -1)), m_radioBoxWhitespaceVisibilityArr, 1, wxRA_SPECIFY_COLS);
        m_radioBoxWhitespaceVisibility->SetForegroundColour(gsgs_Style().GetAppTextColour());
        m_radioBoxWhitespaceVisibility->SetSelection(1);

        boxSizer12->Add(m_radioBoxWhitespaceVisibility, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        //boxSizer12->Add(0, 0, 1, wxALL, WXC_FROM_DIP(5));

        if (m_pages.size() > 1) {
            for (size_t i = 1; i < m_pages.size(); i++) {
                WizardPageSimple::Chain(m_pages.at(i - 1), m_pages.at(i));
            }
        }
        GetPageAreaSizer()->Add(m_pages.at(page));

        if (GetParent()) {
            CentreOnParent(wxBOTH);
        }
        else {
            CentreOnScreen(wxBOTH);
        }
        // Connect events
        this->Connect(wxEVT_WIZARD_FINISHED, WizardEventHandler(BootstrapWizard::OnFinish), NULL, this);
        m_cmdLnkBtn107->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BootstrapWizard::OnCancelWizard),
            NULL, this);
        m_cmdLnkBtnScanForCompilers->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(BootstrapWizard::OnScanForCompilers), NULL, this);
        m_cmdLnkBtnDownloadCompiler->Connect(wxEVT_UPDATE_UI,
            wxUpdateUIEventHandler(BootstrapWizard::OnInstallCompilerUI), NULL, this);
        m_cmdLnkBtnDownloadCompiler->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(BootstrapWizard::OnInstallCompiler), NULL, this);
        /*m_themePicker->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(BootstrapWizard::OnThemeSelected),
            NULL, this);*/
        if (Colour::IsDark(gsgs_Style().GetAppBgColour())) { m_selectedTheme = DARK_THEME; }

        m_stc241->SetText(sampleText);
        m_stc241->SetSyntaxHighlight(wxString("c++"));
        //m_stc241->SetKeyWords(1, "NII Helloworld std string");

        m_stc241->SetEditable(false);

        gsgs_Config().Read("DevelopmentProfile", m_developmentProfile, m_developmentProfile);
        m_radioBoxProfile->SetSelection(m_developmentProfile);

#if PHP_BUILD
        m_radioBoxProfile->SetSelection(3); // PHP
        m_radioBoxProfile->Enable(false);
#endif
    }
    //-----------------------------------------------------------------
    BootstrapWizard::~BootstrapWizard()
    {
        this->Disconnect(wxEVT_WIZARD_FINISHED, WizardEventHandler(BootstrapWizard::OnFinish), NULL, this);
        m_cmdLnkBtn107->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(BootstrapWizard::OnCancelWizard), NULL, this);
        m_cmdLnkBtnScanForCompilers->Disconnect(
            wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BootstrapWizard::OnScanForCompilers), NULL, this);
        m_cmdLnkBtnDownloadCompiler->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(BootstrapWizard::OnInstallCompilerUI), NULL, this);
        m_cmdLnkBtnDownloadCompiler->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(BootstrapWizard::OnInstallCompiler), NULL, this);
        /*m_themePicker->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED,
            wxCommandEventHandler(BootstrapWizard::OnThemeSelected), NULL, this);*/
        gsgs_Config().Write("DevelopmentProfile", m_developmentProfile);
    }
    //-----------------------------------------------------------------
    void BootstrapWizard::OnThemeSelected(wxCommandEvent& event)
    {
        m_globalThemeChanged = true;
        Editor * m_stc241 = m_stc24->getEditor();
        //m_stc241->SetEditable(true);
        int themeID = m_themePicker->GetSelection();
        LexerStyle::Ptr_t lexer(nullptr);
        if (themeID == 0) {
            // OS default
            lexer = gsgs_Style().GetLexer("c++", m_selectedTheme);
            m_selectedTheme = LIGHT_THEME;
            if (Colour::IsDark(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE))) { m_selectedTheme = DARK_THEME; }
            gsgs_Config().Write("UseCustomBaseColour", false);
        }
        else {
            // Dark
            m_selectedTheme = (themeID == 1) ? DARK_THEME : LIGHT_THEME;
            lexer = gsgs_Style().GetLexer("c++", m_selectedTheme);
            wxColour bgColour = gsgs_Style().GetBackgroundColourFromLexer(lexer);
            if (bgColour.IsOk()) {
                gsgs_Config().Write("UseCustomBaseColour", true);
                gsgs_Config().Write("BaseColour", bgColour);
            }
            else {
                gsgs_Config().Write("UseCustomBaseColour", false);
            }
        }
        if (lexer) { lexer->Apply(m_stc241, true); }
        //m_stc241->SetKeyWords(1, "NII Helloworld std string");
        //::CalcHScrollBar(m_stc241);
        //m_stc241->SetEditable(false);
    }
    //-----------------------------------------------------------------
    void BootstrapWizard::OnScanForCompilers(wxCommandEvent& event)
    {
        wxBusyCursor bc;
        m_compilers.clear();

#ifndef __WXGTK__
        wxWindowUpdateLocker locker(m_wizardPagePlugins);
#endif

        ToolsetManager detector;
        if (detector.Locate()) {
            m_cmdLnkBtnScanForCompilers->Hide();
            m_cmdLnkBtnDownloadCompiler->Hide();
            m_dvListCtrlCompilers->Show();

            m_compilers = detector.GetCompilersFound();
            for (size_t i = 0; i < m_compilers.size(); ++i) {
                wxVector<wxVariant> cols;
                cols.push_back(m_compilers.at(i)->GetName());
                cols.push_back(m_compilers.at(i)->GetInstallationPath());
                m_dvListCtrlCompilers->AppendItem(cols);
            }

            if (!detector.FoundMinGWCompiler()) { ToolsetManager::MSWSuggestToDownloadMinGW(true); }

        }
        else {
            // nothing found on this machine, offer to download
            ToolsetManager::MSWSuggestToDownloadMinGW(true);
        }
        m_wizardPagePlugins->GetSizer()->Layout();
    }
    //-----------------------------------------------------------------
    BootstrapData BootstrapWizard::GetData()
    {
        BootstrapData data;
        data.compilers = m_compilers;
        data.selectedTheme = m_selectedTheme;
        data.useTabs = (m_radioBoxSpacesVsTabs->GetSelection() == 1);
        data.whitespaceVisibility = m_radioBoxWhitespaceVisibility->GetSelection();
        return data;
    }
    //-----------------------------------------------------------------
    void BootstrapWizard::OnInstallCompiler(wxCommandEvent& event)
    {
        ToolsetManager::MSWSuggestToDownloadMinGW(false);
    }
    //-----------------------------------------------------------------
    void BootstrapWizard::OnInstallCompilerUI(wxUpdateUIEvent& event)
    {
#ifdef __WXMSW__
        event.Enable(true);
#else
        event.Enable(false);
#endif
    }
    //-----------------------------------------------------------------
    bool BootstrapWizard::GetUnSelectedPlugins(wxArrayString& plugins)
    {
        int profile = m_radioBoxProfile->GetSelection();
        if (profile == 0) {
            // Default, dont change anything
            return false;
        }
        else if (profile == 1) {
            // Enable all
            plugins.Clear();
            return true;
        }
        else if (profile == 2) {
            // C/C++ developer
            std::vector<wxString> cxxPlugins;
            std::vector<wxString> allPlugins;
            std::vector<wxString> webPlugins;
            GetCxxPlugins(cxxPlugins);
            GetAllPlugins(allPlugins);
            std::sort(cxxPlugins.begin(), cxxPlugins.end());
            std::sort(allPlugins.begin(), allPlugins.end());
            std::set_difference(allPlugins.begin(), allPlugins.end(), cxxPlugins.begin(), cxxPlugins.end(),
                std::back_inserter(webPlugins));
            plugins.Clear();
            std::for_each(webPlugins.begin(), webPlugins.end(), [&](const wxString& plugin) { plugins.push_back(plugin); });
            return true;
        }
        else if (profile == 3) {
            // C/C++ developer for blockchain
            std::vector<wxString> cxxPlugins;
            std::vector<wxString> allPlugins;
            std::vector<wxString> webPlugins;
            GetCxxPlugins(cxxPlugins);
            GetAllPlugins(allPlugins);
            std::sort(cxxPlugins.begin(), cxxPlugins.end());
            std::sort(allPlugins.begin(), allPlugins.end());
            std::set_difference(allPlugins.begin(), allPlugins.end(), cxxPlugins.begin(), cxxPlugins.end(),
                std::back_inserter(webPlugins));
            plugins.Clear();
            std::for_each(webPlugins.begin(), webPlugins.end(), [&](const wxString& plugin) { plugins.push_back(plugin); });
            plugins.Add("wxcrafter");     // we don't want wxC enabled for this profile
            plugins.Add("wxFormBuilder"); // we don't want wxFB enabled for this profile
            return true;
        }
        else {
            // Web developer
            std::vector<wxString> cxxPlugins;
            GetCxxPlugins(cxxPlugins);
            std::for_each(cxxPlugins.begin(), cxxPlugins.end(), [&](const wxString& plugin) { plugins.push_back(plugin); });
            return true;
        }
    }
    //-----------------------------------------------------------------
    bool BootstrapWizard::IsRestartRequired()
    {
        return (m_developmentProfile != m_radioBoxProfile->GetSelection()) || m_globalThemeChanged;
    }
    //-----------------------------------------------------------------
    void BootstrapWizard::OnFinish(WizardEvent& event)
    {
        event.Skip();
        wxArrayString pluginsToDisable;
        if (IsRestartRequired() && GetUnSelectedPlugins(pluginsToDisable)) {
            // user changed plugins
            Config conf("plugins.conf");
            PluginInfoArray plugins;
            conf.ReadItem(&plugins);

            plugins.DisablePugins(pluginsToDisable);
            conf.WriteItem(&plugins);
        }
    }
    //-----------------------------------------------------------------
    void BootstrapWizard::OnCancelWizard(wxCommandEvent& event)
    {
        ::wxMessageBox(_("You can always run this setup wizard from the menu:\nHelp -> Run the Setup Wizard"), "gsgs",
            wxOK | wxCENTER | wxICON_INFORMATION, this);
        CallAfter(&BootstrapWizard::EndModal, wxID_CANCEL);
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // AboutDialog
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    AboutDialog::AboutDialog(wxWindow* parent, const wxString& version, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style)
    {
        SetBackgroundColour(gsgs_Style().GetAppBgColour());

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(mainSizer);

        m_notebook8 = new Notebook(this, wxID_ANY, wxDefaultSize, wxDefaultPosition, wxDefaultSize, gsgsNookbookStyle);
        mainSizer->Add(m_notebook8, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_panelAbout = new wxPanel(m_notebook8, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
        m_notebook8->AddPage(m_panelAbout, _("About"), true);

        wxBoxSizer* bSizer3323 = new wxBoxSizer(wxVERTICAL);
        m_panelAbout->SetSizer(bSizer3323);

        wxBitmap bitmap;
        bool re = bitmap.LoadFile(gsgs_Path().GetBinFolder() + _T("/images/splashscreen.png"), wxBITMAP_TYPE_PNG);
        if (bitmap.IsOk())
        {
            m_bitmap424 = new wxStaticBitmap(m_panelAbout, wxID_ANY, bitmap, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

            bSizer3323->Add(m_bitmap424, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));
        }
        mESP = new EditorScrolledPanel(m_notebook8);
        m_notebook8->AddPage(mESP, _("License"));

        m_paneUpdate = new wxPanel(m_notebook8, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
        m_notebook8->AddPage(m_paneUpdate, _("Update"));

        wxBoxSizer* bSizer5930 = new wxBoxSizer(wxVERTICAL);
        m_paneUpdate->SetSizer(bSizer5930);

        mUpdateButton = new Button(m_paneUpdate, -1, _("Update"));
        bSizer5930->Add(mUpdateButton, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_paneWizard = new wxPanel(m_notebook8, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
        m_notebook8->AddPage(m_paneWizard, _("Wizard"));

        wxBoxSizer* bSizer5931 = new wxBoxSizer(wxVERTICAL);
        m_paneWizard->SetSizer(bSizer5931);

        mWizardButton = new Button(m_paneWizard, -1, _("Wizard"));
        bSizer5931->Add(mWizardButton, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxStaticBoxSizer* sbSizer1 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("")), wxVERTICAL);

        mainSizer->Add(sbSizer1, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer1->SetFlexibleDirection(wxBOTH);
        fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

        sbSizer1->Add(fgSizer1, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));
        SetForegroundColour(gsgs_Style().GetAppTextColour());
        m_staticText40 = gsgs_Style().createStaticText(this, wxID_ANY, _("Home Page:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        fgSizer1->Add(m_staticText40, 1, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_hyperLink44 = new wxGenericHyperlinkCtrl(this, wxID_ANY, _("http://www.niiengine.com"), wxT("http://www.niiengine.com"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxHL_DEFAULT_STYLE);
        m_hyperLink44->SetNormalColour(gsgs_Style().GetAppTextColour());
        m_hyperLink44->SetVisitedColour(gsgs_Style().GetAppTextColour());
        m_hyperLink44->SetHoverColour(gsgs_Style().GetActiveMarkerColour());
        fgSizer1->Add(m_hyperLink44, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("光栅故事 Forum:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        fgSizer1->Add(m_staticText2, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_hyperlink2 = new wxGenericHyperlinkCtrl(this, wxID_ANY, _("http://www.niiengine.com/forum/"), wxT("http://www.niiengine.com/forum/"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxHL_DEFAULT_STYLE);
        m_hyperlink2->SetNormalColour(gsgs_Style().GetAppTextColour());
        m_hyperlink2->SetVisitedColour(gsgs_Style().GetAppTextColour());
        m_hyperlink2->SetHoverColour(gsgs_Style().GetActiveMarkerColour());
        fgSizer1->Add(m_hyperlink2, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_staticText36 = gsgs_Style().createStaticText(this, wxID_ANY, _("Version:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        fgSizer1->Add(m_staticText36, 0, wxALL | wxALIGN_RIGHT, WXC_FROM_DIP(5));

        m_staticTextVersion = gsgs_Style().createStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        fgSizer1->Add(m_staticTextVersion, 0, wxALL, WXC_FROM_DIP(5));

        wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

        mainSizer->Add(buttonSizer, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

        m_buttonOk = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_buttonOk->SetDefault();
        m_buttonOk->SetFocus();

        buttonSizer->Add(m_buttonOk, 0, wxALL, WXC_FROM_DIP(5));

        m_staticTextVersion->SetLabel(version);
        // Load the license file
        wxString fileContent;
        wxFileName license(gsgs_Path().GetDataDir(), "LICENSE");
        FileUtil::ReadFileContent(license, fileContent);
        m_stcLicense = mESP->getEditor();
        m_stcLicense->SetSyntaxHighlight("text");
        m_stcLicense->SetShowNumber(false);
        m_stcLicense->SetShowSymbol(false);
        m_stcLicense->SetText(fileContent);
        m_stcLicense->SetReadOnly(true);
        SetMinSize(size);
        if (GetParent()) {
            CentreOnParent(wxBOTH);
        }
        else {
            CentreOnScreen(wxBOTH);
        }
        // Connect events
        mUpdateButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Frame::OnCheckForUpdate), NULL, gsgs_Frame());
        mWizardButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Frame::OnRunSetupWizard), gsgs_Frame());
        m_buttonOk->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AboutDialog::OnOK), NULL, this);
    }
    //-----------------------------------------------------------------
    AboutDialog::~AboutDialog()
    {
        mUpdateButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Frame::OnCheckForUpdate), NULL, gsgs_Frame());
        mWizardButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Frame::OnRunSetupWizard), gsgs_Frame());
        m_buttonOk->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AboutDialog::OnOK), NULL, this);
    }
    //-----------------------------------------------------------------
}