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

#include "gsgsPresent.h"
#include "gsgsFrame.h"
#include <wx/wupdlock.h>

namespace gsgs
{
    class SelectIndexRefData : public wxObjectRefData
    {
        friend class SelectIndexObject;
    public:
        SelectIndexRefData() :mIndex(0) {}
    protected:
        int mIndex;
    };
    class SelectIndexObject : public wxObject
    {
    public:
        SelectIndexObject() { AllocExclusive(); }
    protected:
        virtual wxObjectRefData *CreateRefData() const
        {
            return new SelectIndexRefData();
        }

        // create a new m_refData initialized with the given one
        virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const
        {
            SelectIndexRefData * temp = new SelectIndexRefData();
            temp->mIndex = static_cast<const SelectIndexRefData *>(data)->mIndex;
            return temp;
        }
    };
    //------------------------------------------------------------------------
    Present::Present(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style)
    {
        Hide();
        SetAutoLayout(false);
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        SetForegroundColour(gsgs_Style().GetAppTextColour());
        wxBoxSizer* boxSizer149 = new wxBoxSizer(wxHORIZONTAL);
        this->SetSizer(boxSizer149);

        wxBoxSizer* boxSizer625 = new wxBoxSizer(wxVERTICAL);
        boxSizer625->SetMinSize(500, 500);

        boxSizer149->Add(boxSizer625, 1, wxLEFT | wxRIGHT | wxTOP | wxEXPAND | wxALIGN_LEFT | wxALIGN_TOP, WXC_FROM_DIP(1));

        m_staticBitmap1 = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0);

        boxSizer625->Add(m_staticBitmap1, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

        m_staticLine7954 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);

        boxSizer625->Add(m_staticLine7954, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText7991 = new wxStaticText(this, wxID_ANY, _("Getting Started:"), wxDefaultPosition, wxDefaultSize, 0);
        m_staticText7991->SetForegroundColour(gsgs_Style().GetAppTextColour());
        //wxFont m_staticText7991Font(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("宋体"));
        m_staticText7991->SetFont(gsgsTitleFont);

        boxSizer625->Add(m_staticText7991, 0, wxALL, WXC_FROM_DIP(5));

        m_hyperLink862 = new wxGenericHyperlinkCtrl(this, wxID_ANY, _("光栅故事 IDE"), wxT("http://www.niiengine.com/game/"),wxDefaultPosition, wxDefaultSize);
        m_hyperLink862->SetNormalColour(gsgs_Style().GetAppTextColour());
        m_hyperLink862->SetVisitedColour(gsgs_Style().GetAppTextColour());
        m_hyperLink862->SetHoverColour(gsgs_Style().GetActiveMarkerColour());
        boxSizer625->Add(m_hyperLink862, 0, wxALL, WXC_FROM_DIP(5));
        m_hyperLink864 = new wxGenericHyperlinkCtrl(this, wxID_ANY, _("光栅故事 Forums"), wxT("http://www.niiengine.com/forum/"), wxDefaultPosition, wxDefaultSize);
        m_hyperLink864->SetNormalColour(gsgs_Style().GetAppTextColour());
        m_hyperLink864->SetVisitedColour(gsgs_Style().GetAppTextColour());
        m_hyperLink864->SetHoverColour(gsgs_Style().GetActiveMarkerColour());
        boxSizer625->Add(m_hyperLink864, 0, wxALL, WXC_FROM_DIP(5));
        m_hyperLink866 = new wxGenericHyperlinkCtrl(this, wxID_ANY, _("光栅故事 GameAPI"), wxT("https://github.com/niiengine/NIIEngine"), wxDefaultPosition, wxDefaultSize);
        m_hyperLink866->SetNormalColour(gsgs_Style().GetAppTextColour());
        m_hyperLink866->SetVisitedColour(gsgs_Style().GetAppTextColour());
        m_hyperLink866->SetHoverColour(gsgs_Style().GetActiveMarkerColour());
        boxSizer625->Add(m_hyperLink866, 0, wxALL, WXC_FROM_DIP(5));
        m_hyperLink868 = new wxGenericHyperlinkCtrl(this, wxID_ANY, _("光栅故事 Manual"), wxT("https://github.com/niiengine/NIIEngine"), wxDefaultPosition, wxDefaultSize);
        m_hyperLink868->SetNormalColour(gsgs_Style().GetAppTextColour());
        m_hyperLink868->SetVisitedColour(gsgs_Style().GetAppTextColour());
        m_hyperLink868->SetHoverColour(gsgs_Style().GetActiveMarkerColour());
        boxSizer625->Add(m_hyperLink868, 0, wxALL, WXC_FROM_DIP(5));
        m_hyperLink870 = new wxGenericHyperlinkCtrl(this, wxID_ANY, _("光栅故事 License"), wxT("http://www.niiengine.com/license"), wxDefaultPosition, wxDefaultSize);
        m_hyperLink870->SetNormalColour(gsgs_Style().GetAppTextColour());
        m_hyperLink870->SetVisitedColour(gsgs_Style().GetAppTextColour());
        m_hyperLink870->SetHoverColour(gsgs_Style().GetActiveMarkerColour());
        boxSizer625->Add(m_hyperLink870, 0, wxALL, WXC_FROM_DIP(5));

        m_scrollWin2471 = new DefaultScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, -1));
        mWorkspacePanel = new wxPanel(m_scrollWin2471, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN);
        m_scrollWin2471->SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
        m_scrollWin2471->setClient(mWorkspacePanel);
        m_scrollWin2471->SetScrollRate(5, 5);
        m_scrollWin2471->SetConrnerColour(gsgs_Style().GetAppBgColour());
        boxSizer625->Add(m_scrollWin2471, 1, wxEXPAND | wxALL, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer2492 = new wxBoxSizer(wxVERTICAL);
        mWorkspacePanel->SetSizer(boxSizer2492);

        m_staticText810 = new wxStaticText(mWorkspacePanel, wxID_ANY, _("Open :"), wxDefaultPosition, wxDefaultSize, 0);
        m_staticText810->SetForegroundColour(gsgs_Style().GetAppTextColour());
        m_staticText810->SetFont(gsgsTitleFont);
        boxSizer2492->Add(m_staticText810, 0, wxALL, WXC_FROM_DIP(5));

        mOpenWS = new wxCommandLinkButton(mWorkspacePanel, wxID_ANY, _("Open Workspace"), _("open GSGS workspace file"), wxDefaultPosition, wxDefaultSize, wxBU_LEFT);
        boxSizer2492->Add(mOpenWS, 0, wxALL, WXC_FROM_DIP(5));

        m_staticText811 = new wxStaticText(mWorkspacePanel, wxID_ANY, _("Recently :"), wxDefaultPosition, wxDefaultSize, 0);
        m_staticText811->SetForegroundColour(gsgs_Style().GetAppTextColour());
        m_staticText811->SetFont(gsgsTitleFont);
        m_staticText811->Hide();
        boxSizer2492->Add(m_staticText811, 0, wxALL, WXC_FROM_DIP(5));

        m_searchCtrl815 = new wxSearchCtrl(mWorkspacePanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
        m_searchCtrl815->ShowSearchButton(true);
        m_searchCtrl815->ShowCancelButton(false);
        m_searchCtrl815->Hide();
        boxSizer2492->Add(m_searchCtrl815, 0, wxALL, WXC_FROM_DIP(5));

        m_scrollWin813 = new DefaultScrolledWindow(mWorkspacePanel, wxID_ANY, wxDefaultPosition, wxSize(-1, - 1));
        m_scrollWin813->SetSizeHints(wxSize(500, 250), wxSize(500, 250));
        mRecentPanel = new wxPanel(m_scrollWin813, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN);
        mRecentPanel->SetSizer(new wxBoxSizer(wxVERTICAL));

        m_scrollWin813->SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
        m_scrollWin813->setClient(mRecentPanel);
        m_scrollWin813->SetScrollRate(5, 5);
        m_scrollWin813->SetConrnerColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
        boxSizer2492->Add(m_scrollWin813, 0, wxALL, WXC_FROM_DIP(5));
        m_scrollWin813->Hide();
        m_staticText835 = new wxStaticText(mWorkspacePanel, wxID_ANY, _("Creation :"), wxDefaultPosition, wxDefaultSize, 0);
        m_staticText835->SetForegroundColour(gsgs_Style().GetAppTextColour());
        m_staticText835->SetFont(gsgsTitleFont);

        boxSizer2492->Add(m_staticText835, 0, wxALL, WXC_FROM_DIP(5));

        m_scrollWin837 = new DefaultScrolledWindow(mWorkspacePanel, wxID_ANY, wxDefaultPosition, wxSize(-1, -1));
        m_scrollWin837->SetSizeHints(wxSize(500, 550), wxSize(500, 800));
        mCreationPanel = new wxPanel(m_scrollWin837, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN);
        mCreationPanel->SetSizer(new wxBoxSizer(wxVERTICAL));
        
        m_scrollWin837->SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
        m_scrollWin837->setClient(mCreationPanel);
        m_scrollWin837->SetScrollRate(5, 5);
        m_scrollWin837->SetConrnerColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));

        boxSizer2492->Add(m_scrollWin837, 0, wxALL, WXC_FROM_DIP(5));

        m_panel1913 = new wxPanel(mWorkspacePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        boxSizer2492->Add(m_panel1913, 1, wxEXPAND | wxALIGN_LEFT | wxALIGN_TOP, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer1954 = new wxBoxSizer(wxVERTICAL);
        m_panel1913->SetSizer(boxSizer1954);

        wxBoxSizer* boxSizer5855 = new wxBoxSizer(wxVERTICAL);

        boxSizer1954->Add(boxSizer5855, 0, wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

        wxGridSizer* gridSizer5747 = new wxGridSizer(0, 2, 0, 0);

        boxSizer5855->Add(gridSizer5747, 0, wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

        mWorkspacePanel->SetMinSize(wxSize(500, 500));
        boxSizer625->SetMinSize(wxSize(500, 500));

        wxBoxSizer* boxSizer6271 = new wxBoxSizer(wxVERTICAL);
        boxSizer6271->SetMinSize(400, 500);

        boxSizer149->Add(boxSizer6271, 1, wxLEFT | wxRIGHT | wxTOP | wxEXPAND | wxALIGN_LEFT | wxALIGN_TOP, WXC_FROM_DIP(1));

        m_staticBitmap2 = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0);

        boxSizer6271->Add(m_staticBitmap2, 0, wxUP | wxDOWN | wxRIGHT, WXC_FROM_DIP(5));

        m_staticLine6593 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(400, -1), wxLI_HORIZONTAL);

        boxSizer6271->Add(m_staticLine6593, 0, wxUP | wxDOWN | wxRIGHT | wxEXPAND, WXC_FROM_DIP(5));
        m_staticLine6593->SetMinSize(wxSize(400, -1));

        m_staticText6534 = new wxStaticText(this, wxID_ANY, _("Engine Info:"), wxDefaultPosition, wxDefaultSize, 0);
        //wxFont m_staticText6534Font(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("宋体"));
        m_staticText6534->SetFont(gsgsBigTitleFont);

        boxSizer6271->Add(m_staticText6534, 0, wxUP | wxDOWN | wxRIGHT, WXC_FROM_DIP(5));

        wxFlexGridSizer* flexGridSizer679 = new wxFlexGridSizer(0, 2, 1, 1);
        flexGridSizer679->SetFlexibleDirection(wxBOTH);
        flexGridSizer679->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

        boxSizer6271->Add(flexGridSizer679, 0, wxUP | wxDOWN | wxRIGHT, WXC_FROM_DIP(5));

        m_staticText681 = new wxStaticText(this, wxID_ANY, _("Version:"), wxDefaultPosition, wxSize(100, -1), 0);
        //wxFont m_staticText681Font(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("宋体"));
        m_staticText681->SetFont(gsgsTitleFont);
        m_staticText681->SetMinSize(wxSize(100, -1));

        flexGridSizer679->Add(m_staticText681, 0, wxUP | wxDOWN | wxRIGHT, WXC_FROM_DIP(5));

        m_staticText685 = new wxStaticText(this, wxID_ANY, _("6.5.0"), wxDefaultPosition, wxDefaultSize, 0);
        //wxFont m_staticText685Font(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("宋体"));
        m_staticText685->SetFont(gsgsTitleFont);

        flexGridSizer679->Add(m_staticText685, 0, wxUP | wxDOWN | wxRIGHT, WXC_FROM_DIP(5));

        m_staticText687 = new wxStaticText(this, wxID_ANY, _("Agreement:"), wxDefaultPosition, wxDefaultSize, 0);
        //wxFont m_staticText687Font(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("宋体"));
        m_staticText687->SetFont(gsgsTitleFont);

        flexGridSizer679->Add(m_staticText687, 0, wxUP | wxDOWN | wxRIGHT, WXC_FROM_DIP(5));

        m_staticText689 = new wxStaticText(this, wxID_ANY, _("http://www.niiengine.com/license"), wxDefaultPosition, wxDefaultSize, 0);
        //wxFont m_staticText689Font(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("宋体"));
        m_staticText689->SetFont(gsgsTitleFont);

        flexGridSizer679->Add(m_staticText689, 0, wxUP | wxDOWN | wxRIGHT, WXC_FROM_DIP(5));

        m_scrollWin6655 = new DefaultScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);
        mPluginPanel = new wxPanel(m_scrollWin6655, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN);
        mPluginPanel->SetSizer(new wxBoxSizer(wxVERTICAL));

        m_scrollWin6655->SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
        m_scrollWin6655->setClient(mPluginPanel);
        m_scrollWin6655->SetScrollRate(5, 5);
        m_scrollWin6655->SetConrnerColour(gsgs_Style().GetAppBgColour());
        boxSizer6271->Add(m_scrollWin6655, 1, wxEXPAND| wxUP | wxDOWN | wxRIGHT, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer643 = new wxBoxSizer(wxVERTICAL);
        boxSizer643->SetMinSize(400, 500);

        boxSizer149->Add(boxSizer643, 1, wxLEFT | wxRIGHT | wxTOP | wxEXPAND | wxALIGN_LEFT | wxALIGN_TOP, WXC_FROM_DIP(1));

        m_staticBitmap3 = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0);

        boxSizer643->Add(m_staticBitmap3, 0, wxUP | wxDOWN | wxRIGHT, WXC_FROM_DIP(5));

        m_staticLine661 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(400, -1)), wxLI_HORIZONTAL);

        boxSizer643->Add(m_staticLine661, 0, wxUP | wxDOWN | wxRIGHT | wxEXPAND, WXC_FROM_DIP(5));
        m_staticLine661->SetMinSize(wxSize(400, -1));

        m_staticText657 = new wxStaticText(this, wxID_ANY, _("Server Info:"), wxDefaultPosition, wxDefaultSize, 0);
        //wxFont m_staticText657Font(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("宋体"));
        m_staticText657->SetFont(gsgsBigTitleFont);

        boxSizer643->Add(m_staticText657, 0, wxUP | wxDOWN | wxRIGHT, WXC_FROM_DIP(5));

        m_scrollWin667 = new DefaultScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);
        mServerPanel = new wxPanel(m_scrollWin667, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN);
        mServerPanel->SetSizer(new wxBoxSizer(wxVERTICAL));

        m_scrollWin667->SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
        m_scrollWin667->setClient(mServerPanel);
        m_scrollWin667->SetScrollRate(5, 5);
        m_scrollWin667->SetConrnerColour(gsgs_Style().GetAppBgColour());
        boxSizer643->Add(m_scrollWin667, 1, wxEXPAND | wxUP | wxDOWN | wxRIGHT, WXC_FROM_DIP(5));

        InitItem();
        // Connect events
        this->Connect(wxEVT_SIZE, wxSizeEventHandler(Present::OnSize), NULL, this);
        mOpenWS->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnOpenWorkspace), NULL, this);
        gsgs_Event().Connect(wxEVT_GSGS_THEME_CHANGED, wxCommandEventHandler(Present::OnTheme), NULL, this);
        m_staticBitmap1->SetBitmap(gsgs_Image().LoadBitmap("gsgs", 256));
        m_staticBitmap2->SetBitmap(gsgs_Image().LoadBitmap("NiiEngine"));
        m_staticBitmap3->SetBitmap(gsgs_Image().LoadBitmap("earth"));
    }
    //------------------------------------------------------------------------
    Present::~Present()
    {
        UninitItem();
        this->Disconnect(wxEVT_SIZE, wxSizeEventHandler(Present::OnSize), NULL, this);
        mOpenWS->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnOpenWorkspace), NULL, this);
        gsgs_Event().Disconnect(wxEVT_GSGS_THEME_CHANGED, wxCommandEventHandler(Present::OnTheme), NULL, this);
    }
    //------------------------------------------------------------------------
    void Present::InitItem()
    {
        RefreshRecentList();

        RefreshCreationList();

        RefreshPluginList();

        RefreshServerList();
    }
    //------------------------------------------------------------------------
    void Present::UninitItem()
    {
        m_cmdLnkBtn697->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        m_cmdLnkBtn699->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        m_cmdLnkBtn701->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        m_cmdLnkBtn747->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        m_cmdLnkBtn749->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        m_cmdLnkBtn751->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        m_cmdLnkBtn753->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        m_cmdLnkBtn755->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        m_cmdLnkBtn757->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        m_cmdLnkBtn759->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        m_cmdLnkBtn761->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        m_cmdLnkBtn763->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        m_cmdLnkBtn765->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        m_cmdLnkBtn767->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        m_cmdLnkBtn769->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        m_cmdLnkBtn771->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        m_cmdLnkBtn773->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        m_cmdLnkBtn775->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
    }
    //------------------------------------------------------------------------
    void Present::SetRecentItem(const wxArrayString & blist)
    {
        wxSizer* boxSizer819 = mRecentPanel->GetSizer();
        if (boxSizer819)
        {
            wxWindowUpdateLocker templ(mRecentPanel);
            LinkButtonList::iterator j, jend = mRecentList.end();
            for (j = mRecentList.begin(); j != jend; ++j)
            {
                wxCommandLinkButton * tempb = *j;
                tempb->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnRecentItem), NULL, this);
                boxSizer819->Detach(tempb);
                tempb->Destroy();
            }
            mRecentList.clear();

            wxArrayString::const_iterator i, iend = mRecentFileList.end();
            for (i = mRecentFileList.begin(); i != iend; ++i)
            {
                wxFileName tempf(*i);
                if (tempf.FileExists())
                {
                    wxCommandLinkButton * tempb = 0;
                    if (tempf.GetExt() == "gsgsws")
                    {
                        tempb = new wxCommandLinkButton(mRecentPanel, wxID_ANY, tempf.GetName(), *i, wxDefaultPosition, wxDefaultSize, wxBU_LEFT | wxNO_BORDER);
                        tempb->SetBackgroundColour(gsgs_Style().GetAppBgColour());
                        tempb->SetForegroundColour(gsgs_Style().GetAppTextColour());
                    }
                    else
                    {
                        tempb = new wxCommandLinkButton(mRecentPanel, wxID_ANY, tempf.GetName(), *i, wxDefaultPosition, wxDefaultSize, wxBU_LEFT);
                    }
                    tempb->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnRecentItem), NULL, this);
                    boxSizer819->Add(tempb, 0, wxALL, WXC_FROM_DIP(5));
                    mRecentList.push_back(tempb);
                }
            }
        }
        if (mRecentFileList.size())
        {
            if (!m_scrollWin813->IsShown())
            {
                m_scrollWin813->Show();
                m_staticText811->Show();
                m_searchCtrl815->Show();
                mWorkspacePanel->Layout();
                m_scrollWin2471->SendSizeEvent();
            }
            else
            {
                m_scrollWin813->SendSizeEvent();
                mRecentPanel->SendSizeEvent();
            }
        }
        else
        {
            if (m_scrollWin813->IsShown())
            {
                m_scrollWin813->Hide();
                m_staticText811->Hide();
                m_searchCtrl815->Hide();
                mWorkspacePanel->Layout();
                m_scrollWin2471->SendSizeEvent();
            }
        }
    }
    //------------------------------------------------------------------------
    void Present::SetCreationItem(const LinkButtonList & blist)
    {
        wxWindowUpdateLocker templ(mCreationPanel);
        wxSizer* boxSizer693 = mCreationPanel->GetSizer();
        LinkButtonList::const_iterator i, iend = blist.end();
        for (i = blist.begin(); i != iend; ++i)
        {
            boxSizer693->Add(*i, 0, wxALL, WXC_FROM_DIP(5));
            mCreationList.push_back(*i);
        }
        m_scrollWin837->SendSizeEvent();
    }
    //------------------------------------------------------------------------
    void Present::SetPluginItem(const LinkButtonList & blist)
    {
        wxWindowUpdateLocker temp(mPluginPanel);
        wxSizer* boxSizer839 = mPluginPanel->GetSizer();

        m_staticText745 = new wxStaticText(mPluginPanel, wxID_ANY, _("Plugins:"), wxDefaultPosition, wxDefaultSize, 0);
        m_staticText745->SetForegroundColour(gsgs_Style().GetAppActiveTextColour());
        //wxFont m_staticText745Font(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("宋体"));
        m_staticText745->SetFont(gsgsTitleFont);

        boxSizer839->Add(m_staticText745, 0, wxALL, WXC_FROM_DIP(5));

        LinkButtonList::const_iterator i, iend = blist.end();
        for (i = blist.begin(); i != iend; ++i)
        {
            boxSizer839->Add(*i, 0, wxALL, WXC_FROM_DIP(5));
            mPluginList.push_back(*i);
        }
        m_scrollWin6655->SendSizeEvent();
    }
    //------------------------------------------------------------------------
    void Present::SetServerItem(const LinkButtonList & blist)
    {
        wxWindowUpdateLocker temp(mServerPanel);
        wxSizer* boxSizer695 = mServerPanel->GetSizer();
        LinkButtonList::const_iterator i, iend = blist.end();
        for (i = blist.begin(); i != iend; ++i)
        {
            boxSizer695->Add(*i, 0, wxALL, WXC_FROM_DIP(5));
            mServerList.push_back(*i);
        }
        m_scrollWin667->SendSizeEvent();
    }
    //------------------------------------------------------------------------
    /*void Present::OnShowFileseMenu(wxCommandEvent& event)
    {
        wxArrayString recentFiles; 
        gsgs_Config().GetRecentOpen(recentFiles);

        int id = DoGetPopupMenuSelection(m_cmdLnkBtnFilesMenu, recentFiles, _("Select file to open"));
        if (id != wxID_NONE) {
            wxString filename = m_idToName[id];
            gsgs_Frame()->GetMainBook()->OpenFile(filename);
        }
    }*/
    //------------------------------------------------------------------------
    /*void Present::OnShowWorkspaceMenu(wxCommandEvent& event)
    {
        wxArrayString files;
        gsgs_Config().GetRecentOpen(files);

        wxArrayString recentWorkspaces;
        for (size_t i = 0; i < files.size(); ++i) {
            wxFileName fn(files.Item(i));
            recentWorkspaces.Add(fn.GetName() + " @ " + fn.GetFullPath());
        }

        int id = DoGetPopupMenuSelection(m_cmdLnkBtnWorkspaces, recentWorkspaces, _("Open workspace"));
        if (id != wxID_NONE) {
            wxString filename = m_idToName[id];
            filename = filename.AfterFirst('@');
            filename.Trim().Trim(false);

            // post an event to the main frame requesting a workspace open
            wxCommandEvent evtOpenworkspace(wxEVT_COMMAND_MENU_SELECTED, XRCID("switch_to_workspace"));
            evtOpenworkspace.SetString(filename);
            evtOpenworkspace.SetEventObject(gsgs_Frame());
            gsgs_Frame()->GetEventHandler()->AddPendingEvent(evtOpenworkspace);
        }
    }*/
    //------------------------------------------------------------------------
    void Present::OnSize(wxSizeEvent& event)
    {
        event.Skip();
        //m_staticBitmap1->Refresh();
        //m_staticBitmap2->Refresh();
    }
    //------------------------------------------------------------------------
    void Present::OnOpenHomepage(wxCommandEvent&)
    {
        ::wxLaunchDefaultBrowser("http://www.niiengine.com/game/");
    }
    //------------------------------------------------------------------------
    void Present::OnOpenForums(wxCommandEvent&)
    {
        ::wxLaunchDefaultBrowser("http://www.niiengine.com/forum/");
    }
    //------------------------------------------------------------------------
    void Present::OnOpenApi(wxCommandEvent&)
    {
        ::wxLaunchDefaultBrowser("https://github.com/niiengine/NIIEngine");
    }
    //------------------------------------------------------------------------
    void Present::OnOpenManual(wxCommandEvent&)
    {
        ::wxLaunchDefaultBrowser("https://github.com/niiengine/NIIEngine");
    }
    //------------------------------------------------------------------------
    void Present::OnOpenLicense(wxCommandEvent& event)
    {
        ::wxLaunchDefaultBrowser("http://www.niiengine.com/license");
    }
    //------------------------------------------------------------------------
    void Present::OnTheme(wxCommandEvent& e) 
    { 
        e.Skip(); 
    }
    //------------------------------------------------------------------------
    void Present::OnOpenWorkspace(wxCommandEvent&)
    {
        wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("switch_to_workspace"));
        e.SetEventObject(gsgs_Frame());
        gsgs_Frame()->GetEventHandler()->AddPendingEvent(e);
    }
    //------------------------------------------------------------------------
    void Present::OnNewWorkspace(wxCommandEvent& event)
    {
        wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("new_workspace"));
        e.SetEventObject(event.GetEventObject());
        gsgs_Frame()->GetEventHandler()->AddPendingEvent(e);
    }
    //------------------------------------------------------------------------
    void Present::OnRecentUI(wxUpdateUIEvent& event)
    {
    }
    //------------------------------------------------------------------------
    void Present::RefreshRecentList()
    {
        mRecentFileList.clear();
        gsgs_Config().GetRecentOpen(mRecentFileList);
        SetRecentItem(mRecentFileList);
    }
    //------------------------------------------------------------------------
    void Present::RefreshCreationList()
    {
        LinkButtonList temp;
        wxWindowUpdateLocker templ(mCreationPanel);
        m_cmdLnkBtn697 = new SelectIndexLinkButton(mCreationPanel, 0, wxID_ANY, _("荒洲API"), _("Create blockchain concept commercial network 3D space"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn697->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn697);

        m_cmdLnkBtn699 = new SelectIndexLinkButton(mCreationPanel, 1, wxID_ANY, _("RPG"), _("Create role-playing game"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn699->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn699);
        m_cmdLnkBtn701 = new SelectIndexLinkButton(mCreationPanel, 2, wxID_ANY, _("ATG"), _("Create action game"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn701->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn701);
        m_cmdLnkBtn747 = new SelectIndexLinkButton(mCreationPanel, 3, wxID_ANY, _("STG"), _("Create shooting game"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn747->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn747);
        m_cmdLnkBtn749 = new SelectIndexLinkButton(mCreationPanel, 4, wxID_ANY, _("FTG"), _("Create fighting game"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn749->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn749);
        m_cmdLnkBtn751 = new SelectIndexLinkButton(mCreationPanel, 5, wxID_ANY, _("AVG"), _("Create adventure game"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn751->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn751);
        m_cmdLnkBtn753 = new SelectIndexLinkButton(mCreationPanel, 6, wxID_ANY, _("SLG"), _("Create simulation game"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn753->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn753);
        m_cmdLnkBtn755 = new SelectIndexLinkButton(mCreationPanel, 7, wxID_ANY, _("SG"), _("Create strategy game"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn755->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn755);
        m_cmdLnkBtn757 = new SelectIndexLinkButton(mCreationPanel, 8, wxID_ANY, _("MG"), _("Create Music game"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn757->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn757);
        m_cmdLnkBtn759 = new SelectIndexLinkButton(mCreationPanel, 9, wxID_ANY, _("CG"), _("Create casual game"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn759->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn759);
        m_cmdLnkBtn761 = new SelectIndexLinkButton(mCreationPanel, 10, wxID_ANY, _("SG"), _("Create sport game"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn761->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn761);
        m_cmdLnkBtn763 = new SelectIndexLinkButton(mCreationPanel, 11, wxID_ANY, _("RG"), _("Create racing Game"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn763->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn763);
        m_cmdLnkBtn765 = new SelectIndexLinkButton(mCreationPanel, 12, wxID_ANY, _("Terrain"), _("Create terrain"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn765->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn765);
        m_cmdLnkBtn767 = new SelectIndexLinkButton(mCreationPanel, 13, wxID_ANY, _("Geometric"), _("Create or modify geometric-model"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn767->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn767);
        m_cmdLnkBtn769 = new SelectIndexLinkButton(mCreationPanel, 14, wxID_ANY, _("Mathematical"), _("Create or modify mathematical-model"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn769->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn769);
        m_cmdLnkBtn771 = new SelectIndexLinkButton(mCreationPanel, 15, wxID_ANY, _("Behavior tree"), _("Create or modify behavior-tree"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn771->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn771);
        m_cmdLnkBtn773 = new SelectIndexLinkButton(mCreationPanel, 16, wxID_ANY, _("AI"), _("Create or modify artificial intelligence model"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn773->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn773);
        m_cmdLnkBtn775 = new SelectIndexLinkButton(mCreationPanel, 17, wxID_ANY, _("3rd"), _("Create 3rd party modules project"), wxDefaultPosition, wxDefaultSize, 0);
        m_cmdLnkBtn775->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Present::OnNewWorkspace), NULL, this);
        temp.push_back(m_cmdLnkBtn775);

        SetCreationItem(temp);
    }
    //------------------------------------------------------------------------
    void Present::RefreshPluginList()
    {
        LinkButtonList temp;
        wxWindowUpdateLocker templ(mPluginPanel);
        m_cmdLnkBtn855 = new wxCommandLinkButton(mPluginPanel, wxID_ANY, _("Zip FileSystem"), wxT("FileSystem Plugin"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn855);
        m_cmdLnkBtn856 = new wxCommandLinkButton(mPluginPanel, wxID_ANY, _("Png Image Codec"), wxT("Image Codec Plugin"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn856);
        m_cmdLnkBtn857 = new wxCommandLinkButton(mPluginPanel, wxID_ANY, _("JoyPad Input"), wxT("Input Pattern Plugin"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn857);
        m_cmdLnkBtn858 = new wxCommandLinkButton(mPluginPanel, wxID_ANY, _("Mouse Input"), wxT("Input Pattern Plugin"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn858);
        m_cmdLnkBtn859 = new wxCommandLinkButton(mPluginPanel, wxID_ANY, _("Keyboard Input"), wxT("Input Pattern Plugin"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn859);
        m_cmdLnkBtn860 = new wxCommandLinkButton(mPluginPanel, wxID_ANY, _("Touch Imput"), wxT("Input Pattern Plugin"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn860);
        m_cmdLnkBtn861 = new wxCommandLinkButton(mPluginPanel, wxID_ANY, _("Opengl3 RenderSystem"), wxT("RenderSystem plugin"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn861);
        m_cmdLnkBtn862 = new wxCommandLinkButton(mPluginPanel, wxID_ANY, _("Opengl es2 RenderSystem"), wxT("RenderSystem plugin"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn862);
        m_cmdLnkBtn863 = new wxCommandLinkButton(mPluginPanel, wxID_ANY, _("Jpeg Image Code"), wxT("Image Codec Plugin"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn863);
        m_cmdLnkBtn864 = new wxCommandLinkButton(mPluginPanel, wxID_ANY, _("Ogg Voice Code"), wxT("Voice Codec Plugin"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn864);
        m_cmdLnkBtn865 = new wxCommandLinkButton(mPluginPanel, wxID_ANY, _("Speex Voice Code"), wxT("Voice Codec Plugin"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn865);
        m_cmdLnkBtn866 = new wxCommandLinkButton(mPluginPanel, wxID_ANY, _("MP3 Voice Code"), wxT("Voice Codec Plugin"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn866);

        SetPluginItem(temp);
    }
    //------------------------------------------------------------------------
    void Present::RefreshServerList()
    {
        LinkButtonList temp;
        wxWindowUpdateLocker templ(mServerPanel);
        m_cmdLnkBtn703 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test1"), _("layout testlayout testlayout testlayout testlayout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, wxBU_LEFT);
        temp.push_back(m_cmdLnkBtn703);
        m_cmdLnkBtn705 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test2"), _("layout testlayout testlayout testlayout testlayout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, wxBU_LEFT);
        temp.push_back(m_cmdLnkBtn705);
        m_cmdLnkBtn707 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test3"), _("layout testlayout testlayout testlayout testlayout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, wxBU_LEFT);
        temp.push_back(m_cmdLnkBtn707);
        m_cmdLnkBtn709 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test4"), _("layout testlayout testlayout testlayout testlayout testlayout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, wxBU_LEFT);
        temp.push_back(m_cmdLnkBtn709);
        m_cmdLnkBtn711 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test5"), _("layout testlayout testlayout testlayout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, wxBU_LEFT);
        temp.push_back(m_cmdLnkBtn711);
        m_cmdLnkBtn713 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test6"), _("layout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn713);
        m_cmdLnkBtn715 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test7"), _("layout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn715);
        m_cmdLnkBtn717 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test8"), _("layout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn717);
        m_cmdLnkBtn719 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test9"), _("layout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn719);
        m_cmdLnkBtn721 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test10"), _("layout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn721);
        m_cmdLnkBtn723 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test11"), _("layout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn723);
        m_cmdLnkBtn725 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test12"), _("layout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn725);
        m_cmdLnkBtn727 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test13"), _("layout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn727);
        m_cmdLnkBtn729 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test14"), _("layout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn729);
        m_cmdLnkBtn731 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test15"), _("layout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn731);
        m_cmdLnkBtn733 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test16"), _("layout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn733);
        m_cmdLnkBtn735 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test17"), _("layout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn735);
        m_cmdLnkBtn737 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test18"), _("layout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn737);
        m_cmdLnkBtn739 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test19"), _("layout testlayout testlayout testlayout testlayout testlayout testlayout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn739);
        m_cmdLnkBtn741 = new wxCommandLinkButton(mServerPanel, wxID_ANY, _("layout test20"), _("layout testlayout testlayout testlayout testlayout testlayout testlayout testlayout testlayout test"), wxDefaultPosition, wxDefaultSize, 0);
        temp.push_back(m_cmdLnkBtn741);

        SetServerItem(temp);
    }
    //------------------------------------------------------------------------
    void Present::OnRecentItem(wxCommandEvent& event)
    {
        wxCommandLinkButton * eobj = dynamic_cast<wxCommandLinkButton *>(event.GetEventObject());

        wxString file = eobj->GetNote();
        wxFileName ffile(file);
        if (ffile.GetExt() == "gsgsws")
        {
            // post an event to the main frame requesting a workspace open
            wxCommandEvent evtOpenworkspace(wxEVT_COMMAND_MENU_SELECTED, XRCID("switch_to_workspace"));
            evtOpenworkspace.SetString(file);
            evtOpenworkspace.SetEventObject(gsgs_Frame());
            gsgs_Frame()->GetEventHandler()->AddPendingEvent(evtOpenworkspace);
        }
        else
        {
            gsgs_Frame()->GetMainBook()->OpenFile(file);
        }
    }
    //------------------------------------------------------------------------
    int Present::DoGetPopupMenuSelection(wxCommandLinkButton* btn, const wxArrayString& strings, const wxString&)
    {
        ImageManager * loader = &gsgs_Image();

        m_idToName.clear();
        wxMenu menu;

        for (size_t i = 0; i < strings.GetCount(); ++i) {
            wxBitmap bmp = loader->GetBitmap("a.txt");
            wxString filename = strings.Item(i);
            if (filename.Find("@") != wxNOT_FOUND) { filename = filename.AfterFirst('@'); }
            filename.Trim().Trim(false);

            // Ensure that the file exists...
            if (!wxFileName(filename).Exists()) {
                // Don't show non existing files in the menu
                continue;
            }
            bmp = loader->GetBitmap(filename);
            wxMenuItem* item = new wxMenuItem(&menu, wxID_ANY, strings.Item(i));
            item->SetBitmap(bmp);
            m_idToName.insert(std::make_pair(item->GetId(), strings.Item(i)));
            menu.Append(item);
        }

        // get the best position to show the menu
        wxPoint pos = btn->GetPosition();
        //pos = m_scrollWin2471->CalcScrolledPosition(pos);

        pos.y += btn->GetSize().y;
#ifdef __WXGTK__
        pos.y += 5;
        pos.x += 5;
#elif defined(__WXMAC__)
        pos.y += 10;
        pos.x += 10;
#else // MSW
        pos.y += 5;
        pos.x += 5;
#endif
        return GetPopupMenuSelectionFromUser(menu, pos);
    }
    //------------------------------------------------------------------------
}