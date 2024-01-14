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

#include "gsgsCreateWorkspace.h"
#include "gsgsFileUtil.h"
#include "gsgsNotebook.h"
#include "gsgsWindowManager.h"
#include "gsgsTreeCtrl.h"
#include "gsgsManager.h"
#include <wx/wupdlock.h>
#include <wx/dcbuffer.h>

namespace gsgs
{
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // HZapi
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class HZapi : public wxPanel
    {
    public:
        HZapi(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("HZapi"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~HZapi();

    protected:
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxStaticText* m_staticText4;
        wxStaticText* m_staticText5;
        wxStaticText* m_staticText6;
        wxStaticText* m_staticText7;
        wxStaticText* m_staticText8;
        wxStaticText* m_staticText9;
        wxStaticText* m_staticText10;
        wxStaticText* m_staticText11;
        wxStaticText* m_staticText12;
        wxStaticText* m_staticText13;
        wxStaticText* m_staticText14;
        wxStaticText* m_staticText15;
        wxStaticText* m_staticText16;
        wxStaticText* m_staticText17;
        wxStaticText* m_staticText18;
        wxStaticText* m_staticText19;
        wxStaticText* m_staticText20;
        wxTextCtrl* m_textCtrl1;
        wxTextCtrl* m_textCtrl2;
        wxTextCtrl* m_textCtrl3;
        wxTextCtrl* m_textCtrl4;
        wxTextCtrl* m_textCtrl5;
        wxTextCtrl* m_textCtrl6;
        wxTextCtrl* m_textCtrl7;
        wxTextCtrl* m_textCtrl8;
        wxCheckBox * m_checkBox1;
        wxCheckBox * m_checkBox2;
        wxCheckBox * m_checkBox3;
        Choice * mDNAChoice;
        Choice * mTemplateChoice;
        wxComboBox* m_comboBoxPath;
        DefaultTreeCtrl* m_listTable;
        Button* m_buttonWorkspaceDirPicker;
    };
    //----------------------------------------------------------------------------
    HZapi::HZapi(wxWindow* parent, wxWindow * ok, wxWindow * cancel,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, title)
    {
        wxWindowUpdateLocker temp(this);
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);

        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Node Name :"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText6 = gsgs_Style().createStaticText(this, wxID_ANY, _("Node Type :"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText6, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        wxBoxSizer* bSizer3 = new wxBoxSizer(wxHORIZONTAL);
        flexGridSizer9->Add(bSizer3, 0, wxALL | wxEXPAND, 5);

        m_checkBox1 = new wxCheckBox(this, wxID_ANY, _("Independent Node"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBox1->SetForegroundColour(gsgs_Style().GetAppTextColour());
        m_checkBox1->SetValue(true);
        bSizer3->Add(m_checkBox1, 0, wxALL | wxEXPAND, 5);

        m_checkBox2 = new wxCheckBox(this, wxID_ANY, _("Follower Node"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBox2->SetForegroundColour(gsgs_Style().GetAppTextColour());
        m_checkBox2->SetValue(true);
        bSizer3->Add(m_checkBox2, 0, wxALL | wxEXPAND, 5);

        m_checkBox3 = new wxCheckBox(this, wxID_ANY, _("Drift Node"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBox3->SetForegroundColour(gsgs_Style().GetAppTextColour());
        m_checkBox3->SetValue(true);
        bSizer3->Add(m_checkBox3, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Owner Name :"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Owner Identifier :"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl3 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);

        m_staticText4 = gsgs_Style().createStaticText(this, wxID_ANY, _("Owner Digital Certificate :"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText4, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl4 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl4, 0, wxALL | wxEXPAND, 5);

        m_staticText8 = gsgs_Style().createStaticText(this, wxID_ANY, _("Blockchain Authorization File:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText8, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        bSizer3 = new wxBoxSizer(wxHORIZONTAL);

        flexGridSizer9->Add(bSizer3, 0, wxEXPAND, 5);

        wxArrayString m_comboBoxPathArr;
        m_comboBoxPath = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), m_comboBoxPathArr, 0);
#if wxVERSION_NUMBER >= 3000
        m_comboBoxPath->SetHint(wxT(""));
#endif

        bSizer3->Add(m_comboBoxPath, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        m_buttonWorkspaceDirPicker = new Button(this, wxID_ANY, _("..."), wxDefaultPosition, wxSize(-1, -1), wxBU_EXACTFIT);
        m_buttonWorkspaceDirPicker->SetForegroundColour(gsgs_Style().GetAppTextColour());
        m_buttonWorkspaceDirPicker->SetDefault();
        m_buttonWorkspaceDirPicker->SetToolTip(_("Browse for folder"));

        bSizer3->Add(m_buttonWorkspaceDirPicker, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        m_staticText9 = gsgs_Style().createStaticText(this, wxID_ANY, _("Blockchain Authorization Code:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText9, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl8 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl8, 0, wxALL | wxEXPAND, 5);

        m_staticText5 = gsgs_Style().createStaticText(this, wxID_ANY, _("DNA Model :"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText5, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        wxArrayString items3;
        items3.Add(wxT("RGB-Normal-ORC-001"));
        items3.Add(wxT("RGB-Normal-ERC-002"));
        items3.Add(wxT("RGB-Normal-QRC-003"));
        mDNAChoice = new Choice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, items3, wxBU_EXACTFIT);
        flexGridSizer9->Add(mDNAChoice, 0, wxSHRINK, 0);

        m_staticText7 = gsgs_Style().createStaticText(this, wxID_ANY, _("Template :"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText7, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        wxArrayString items4;
        items4.Add(wxT("RGB-ATG Template1"));
        items4.Add(wxT("RGB-ATG Template2"));
        items4.Add(wxT("RGB-ATG Template3"));
        mTemplateChoice = new Choice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, items4, wxBU_EXACTFIT);
        flexGridSizer9->Add(mTemplateChoice, 0, wxSHRINK, 0);

        bSizer3 = new wxBoxSizer(wxHORIZONTAL);

        bSizer2->Add(bSizer3, 1, wxEXPAND, 5);
        m_staticText16 = gsgs_Style().createStaticText(this, wxID_ANY, _("Install Server :"), wxDefaultPosition, wxSize(-1, -1), 0);
        bSizer3->Add(m_staticText16, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_listTable = new DefaultTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxTR_SINGLE | wxTR_ROW_LINES | wxTR_HIDE_ROOT | wxTR_ENABLE_SEARCH);
        wxWindowUpdateLocker temp2(m_listTable);
        m_listTable->AddRoot(_("Server"));
        m_listTable->AddHeader(_("Server Name"));
        m_listTable->AddHeader(_("Type"));
        m_listTable->AddHeader(_("Description"));
        m_listTable->AddHeader(_("Version"));

        // Only sort top level items, don't sort their children
        m_listTable->AddTreeStyle(wxTR_SORT_TOP_LEVEL);
        m_listTable->SetShowHeader(true);

        wxTreeItemId tempid = m_listTable->AppendItem(m_listTable->GetRootItem(), wxT("EngineInfo"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "Server(x86)", 1);
        m_listTable->SetItemText(tempid, "None", 2);
        m_listTable->SetItemText(tempid, "1.0.0", 3);
        tempid = m_listTable->AppendItem(m_listTable->GetRootItem(), wxT("EnginePlugin"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "Server(x86)", 1);
        m_listTable->SetItemText(tempid, "None", 2);
        m_listTable->SetItemText(tempid, "1.0.0", 3);
        tempid = m_listTable->AppendItem(m_listTable->GetRootItem(), wxT("EngineStuff"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "Server(x86)", 1);
        m_listTable->SetItemText(tempid, "None", 2);
        m_listTable->SetItemText(tempid, "1.0.0", 3);
        tempid = m_listTable->AppendItem(m_listTable->GetRootItem(), wxT("EngineTerrain"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "Server(x86)", 1);
        m_listTable->SetItemText(tempid, "None", 2);
        m_listTable->SetItemText(tempid, "1.0.0", 3);
        tempid = m_listTable->AppendItem(m_listTable->GetRootItem(), wxT("BLE"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "Server(x86)", 1);
        m_listTable->SetItemText(tempid, "None", 2);
        m_listTable->SetItemText(tempid, "1.0.0", 3);
        tempid = m_listTable->AppendItem(m_listTable->GetRootItem(), wxT("Cooperation"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "Server(x86)", 1);
        m_listTable->SetItemText(tempid, "None", 2);
        m_listTable->SetItemText(tempid, "1.0.0", 3);
        tempid = m_listTable->AppendItem(m_listTable->GetRootItem(), wxT("Db"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "Server(x86)", 1);
        m_listTable->SetItemText(tempid, "None", 2);
        m_listTable->SetItemText(tempid, "1.0.0", 3);
        tempid = m_listTable->AppendItem(m_listTable->GetRootItem(), wxT("File"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "Server(x86)", 1);
        m_listTable->SetItemText(tempid, "None", 2);
        m_listTable->SetItemText(tempid, "1.0.0", 3);
        tempid = m_listTable->AppendItem(m_listTable->GetRootItem(), wxT("IOT"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "Server(x86)", 1);
        m_listTable->SetItemText(tempid, "None", 2);
        m_listTable->SetItemText(tempid, "1.0.0", 3);
        tempid = m_listTable->AppendItem(m_listTable->GetRootItem(), wxT("Login"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "Server(x86)", 1);
        m_listTable->SetItemText(tempid, "None", 2);
        m_listTable->SetItemText(tempid, "1.0.0", 3);
        tempid = m_listTable->AppendItem(m_listTable->GetRootItem(), wxT("Msg"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "Server(x86)", 1);
        m_listTable->SetItemText(tempid, "None", 2);
        m_listTable->SetItemText(tempid, "1.0.0", 3);
        tempid = m_listTable->AppendItem(m_listTable->GetRootItem(), wxT("MsgFile"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "Server(x86)", 1);
        m_listTable->SetItemText(tempid, "None", 2);
        m_listTable->SetItemText(tempid, "1.0.0", 3);
        tempid = m_listTable->AppendItem(m_listTable->GetRootItem(), wxT("MsgHttp"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "Server(x86)", 1);
        m_listTable->SetItemText(tempid, "None", 2);
        m_listTable->SetItemText(tempid, "1.0.0", 3);
        tempid = m_listTable->AppendItem(m_listTable->GetRootItem(), wxT("Permission"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "Server(x86)", 1);
        m_listTable->SetItemText(tempid, "None", 2);
        m_listTable->SetItemText(tempid, "1.0.0", 3);
        tempid = m_listTable->AppendItem(m_listTable->GetRootItem(), wxT("Push"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "Server(x86)", 1);
        m_listTable->SetItemText(tempid, "None", 2);
        m_listTable->SetItemText(tempid, "1.0.0", 3);
        tempid = m_listTable->AppendItem(m_listTable->GetRootItem(), wxT("Route"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "Server(x86)", 1);
        m_listTable->SetItemText(tempid, "None", 2);
        m_listTable->SetItemText(tempid, "1.0.0", 3);
        tempid = m_listTable->AppendItem(m_listTable->GetRootItem(), wxT("Switch"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "Server(x86)", 1);
        m_listTable->SetItemText(tempid, "None", 2);
        m_listTable->SetItemText(tempid, "1.0.0", 3);
        bSizer3->Add(m_listTable, 1, wxALL | wxEXPAND, 5);

        wxStaticBox * wssb = new wxStaticBox(this, wxID_ANY, _("Blockchain Info:"));
        wssb->SetForegroundColour(gsgs_Style().GetAppTextColour());
        wxStaticBoxSizer* sbSizer1 = new wxStaticBoxSizer(wssb, wxVERTICAL);

        bSizer2->Add(sbSizer1, 0, wxALL | wxEXPAND, 5);

        m_staticText10 = new wxStaticText(this, wxID_ANY, wxT("Organization name : "), wxDefaultPosition, wxSize(-1, -1), 0);
        m_staticText10->SetForegroundColour(gsgs_Style().GetActiveMarkerColour());
        sbSizer1->Add(m_staticText10, 0, wxALL | wxEXPAND, 5);

        m_staticText11 = new wxStaticText(this, wxID_ANY, wxT("Business documents : "), wxDefaultPosition, wxSize(-1, -1), 0);
        m_staticText11->SetForegroundColour(gsgs_Style().GetActiveMarkerColour());
        sbSizer1->Add(m_staticText11, 0, wxALL | wxEXPAND, 5);

        m_staticText12 = new wxStaticText(this, wxID_ANY, wxT("Access authorization : "), wxDefaultPosition, wxSize(-1, -1), 0);
        m_staticText12->SetForegroundColour(gsgs_Style().GetActiveMarkerColour());
        sbSizer1->Add(m_staticText12, 0, wxALL | wxEXPAND, 5);

        m_staticText13 = new wxStaticText(this, wxID_ANY, wxT("Install Server : "), wxDefaultPosition, wxSize(-1, -1), 0);
        m_staticText13->SetForegroundColour(gsgs_Style().GetActiveMarkerColour());
        sbSizer1->Add(m_staticText13, 0, wxALL | wxEXPAND, 5);

        m_staticText14 = new wxStaticText(this, wxID_ANY, wxT("DNA Model : "), wxDefaultPosition, wxSize(-1, -1), 0);
        m_staticText14->SetForegroundColour(gsgs_Style().GetActiveMarkerColour());
        sbSizer1->Add(m_staticText14, 0, wxALL | wxEXPAND, 5);

        m_staticText15 = new wxStaticText(this, wxID_ANY, wxT("Template : "), wxDefaultPosition, wxSize(-1, -1), 0);
        m_staticText15->SetForegroundColour(gsgs_Style().GetActiveMarkerColour());
        sbSizer1->Add(m_staticText15, 0, wxALL | wxEXPAND, 5);
    }
    //----------------------------------------------------------------------------
    HZapi::~HZapi()
    {

    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // RGBGame
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class RGBGame : public wxPanel
    {
    public:
        RGBGame(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("RGBGame"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~RGBGame();

    protected:
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrl1;
        wxTextCtrl* m_textCtrl2;
        wxTextCtrl* m_textCtrl3;
    };
    //----------------------------------------------------------------------------
    RGBGame::RGBGame(wxWindow* parent, wxWindow * ok, wxWindow * cancel,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, title)
    {
        wxWindowUpdateLocker temp(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);

        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl3 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);
    }
    //----------------------------------------------------------------------------
    RGBGame::~RGBGame()
    {

    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // ATGGame
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class ATGGame : public wxPanel
    {
    public:
        ATGGame(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("ATGGame"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~ATGGame();

    protected:
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrl1;
        wxTextCtrl* m_textCtrl2;
        wxTextCtrl* m_textCtrl3;
    };
    //----------------------------------------------------------------------------
    ATGGame::ATGGame(wxWindow* parent, wxWindow * ok, wxWindow * cancel,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, title)
    {
        wxWindowUpdateLocker temp(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);

        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl3 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);
    }
    //----------------------------------------------------------------------------
    ATGGame::~ATGGame()
    {

    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // STGGame
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class STGGame : public wxPanel
    {
    public:
        STGGame(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("STGGame"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~STGGame();

    protected:
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrl1;
        wxTextCtrl* m_textCtrl2;
        wxTextCtrl* m_textCtrl3;
    };
    //----------------------------------------------------------------------------
    STGGame::STGGame(wxWindow* parent, wxWindow * ok, wxWindow * cancel,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, title)
    {
        wxWindowUpdateLocker temp(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);

        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl3 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);
    }
    //----------------------------------------------------------------------------
    STGGame::~STGGame()
    {

    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // FTGGame
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class FTGGame : public wxPanel
    {
    public:
        FTGGame(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("FTGGame"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~FTGGame();

    protected:
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrl1;
        wxTextCtrl* m_textCtrl2;
        wxTextCtrl* m_textCtrl3;
    };
    //----------------------------------------------------------------------------
    FTGGame::FTGGame(wxWindow* parent, wxWindow * ok, wxWindow * cancel,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, title)
    {
        wxWindowUpdateLocker temp(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);

        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl3 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);
    }
    //----------------------------------------------------------------------------
    FTGGame::~FTGGame()
    {

    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // AVGGame
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class AVGGame : public wxPanel
    {
    public:
        AVGGame(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("AVGGame"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~AVGGame();

    protected:
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrl1;
        wxTextCtrl* m_textCtrl2;
        wxTextCtrl* m_textCtrl3;
    };
    //----------------------------------------------------------------------------
    AVGGame::AVGGame(wxWindow* parent, wxWindow * ok, wxWindow * cancel,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, title)
    {
        wxWindowUpdateLocker temp(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);

        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl3 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);
    }
    //----------------------------------------------------------------------------
    AVGGame::~AVGGame()
    {

    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // SLGGame
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class SLGGame : public wxPanel
    {
    public:
        SLGGame(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("SLGGame"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~SLGGame();

    protected:
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrl1;
        wxTextCtrl* m_textCtrl2;
        wxTextCtrl* m_textCtrl3;
    };
    //----------------------------------------------------------------------------
    SLGGame::SLGGame(wxWindow* parent, wxWindow * ok, wxWindow * cancel,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, title)
    {
        wxWindowUpdateLocker temp(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);

        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl3 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer9->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);
    }
    //----------------------------------------------------------------------------
    SLGGame::~SLGGame()
    {

    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // StrategyGame
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class StrategyGame : public wxPanel
    {
    public:
        StrategyGame(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("StrategyGame"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~StrategyGame();

    protected:
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrl1;
        wxTextCtrl* m_textCtrl2;
        wxTextCtrl* m_textCtrl3;
    };
    //----------------------------------------------------------------------------
    StrategyGame::StrategyGame(wxWindow* parent, wxWindow * ok, wxWindow * cancel,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, title)
    {
        wxWindowUpdateLocker temp(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);

        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl3 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);
    }
    //----------------------------------------------------------------------------
    StrategyGame::~StrategyGame()
    {

    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // MusicGame
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class MusicGame : public wxPanel
    {
    public:
        MusicGame(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("MusicGame"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~MusicGame();

    protected:
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrl1;
        wxTextCtrl* m_textCtrl2;
        wxTextCtrl* m_textCtrl3;
    };
    //----------------------------------------------------------------------------
    MusicGame::MusicGame(wxWindow* parent, wxWindow * ok, wxWindow * cancel,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, title)
    {
        wxWindowUpdateLocker temp(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);

        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl3 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);
    }
    //----------------------------------------------------------------------------
    MusicGame::~MusicGame()
    {

    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // CasualGame
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class CasualGame : public wxPanel
    {
    public:
        CasualGame(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("CasualGame"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~CasualGame();

    protected:
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrl1;
        wxTextCtrl* m_textCtrl2;
        wxTextCtrl* m_textCtrl3;
    };
    //----------------------------------------------------------------------------
    CasualGame::CasualGame(wxWindow* parent, wxWindow * ok, wxWindow * cancel,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, title)
    {
        wxWindowUpdateLocker temp(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);

        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl3 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);
    }
    //----------------------------------------------------------------------------
    CasualGame::~CasualGame()
    {

    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // SportGame
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class SportGame : public wxPanel
    {
    public:
        SportGame(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("SportGame"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~SportGame();

    protected:
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrl1;
        wxTextCtrl* m_textCtrl2;
        wxTextCtrl* m_textCtrl3;
    };
    //----------------------------------------------------------------------------
    SportGame::SportGame(wxWindow* parent, wxWindow * ok, wxWindow * cancel,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, title)
    {
        wxWindowUpdateLocker temp(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);

        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl3 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);
    }
    //----------------------------------------------------------------------------
    SportGame::~SportGame()
    {

    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // RacingGame
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class RacingGame : public wxPanel
    {
    public:
        RacingGame(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("RacingGame"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~RacingGame();

    protected:
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrl1;
        wxTextCtrl* m_textCtrl2;
        wxTextCtrl* m_textCtrl3;
    };
    //----------------------------------------------------------------------------
    RacingGame::RacingGame(wxWindow* parent, wxWindow * ok, wxWindow * cancel,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, title)
    {
        wxWindowUpdateLocker temp(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);

        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl3 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);
    }
    //----------------------------------------------------------------------------
    RacingGame::~RacingGame()
    {

    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // OtherTerrain
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class OtherTerrain : public wxPanel
    {
    public:
        OtherTerrain(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("OtherTerrain"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~OtherTerrain();

    protected:
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrl1;
        wxTextCtrl* m_textCtrl2;
        wxTextCtrl* m_textCtrl3;
    };
    //----------------------------------------------------------------------------
    OtherTerrain::OtherTerrain(wxWindow* parent, wxWindow * ok, wxWindow * cancel,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, title)
    {
        wxWindowUpdateLocker temp(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);

        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl3 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);
    }
    //----------------------------------------------------------------------------
    OtherTerrain::~OtherTerrain()
    {

    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // OtherGeometricModel
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class OtherGeometricModel : public wxPanel
    {
    public:
        OtherGeometricModel(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("OtherGeometricModel"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~OtherGeometricModel();

    protected:
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrl1;
        wxTextCtrl* m_textCtrl2;
        wxTextCtrl* m_textCtrl3;
    };
    //----------------------------------------------------------------------------
    OtherGeometricModel::OtherGeometricModel(wxWindow* parent, wxWindow * ok, wxWindow * cancel,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, title)
    {
        wxWindowUpdateLocker temp(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);

        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl3 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);
    }
    //----------------------------------------------------------------------------
    OtherGeometricModel::~OtherGeometricModel()
    {

    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // OtherMathematicalModel
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class OtherMathematicalModel : public wxPanel
    {
    public:
        OtherMathematicalModel(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("OtherMathematicalModel"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~OtherMathematicalModel();

    protected:
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrl1;
        wxTextCtrl* m_textCtrl2;
        wxTextCtrl* m_textCtrl3;
    };
    //----------------------------------------------------------------------------
    OtherMathematicalModel::OtherMathematicalModel(wxWindow* parent, wxWindow * ok, wxWindow * cancel,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, title)
    {
        wxWindowUpdateLocker temp(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);

        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl3 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);
    }
    //----------------------------------------------------------------------------
    OtherMathematicalModel::~OtherMathematicalModel()
    {

    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // OtherBehaviorTree
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class OtherBehaviorTree : public wxPanel
    {
    public:
        OtherBehaviorTree(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("OtherBehaviorTree"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~OtherBehaviorTree();

    protected:
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrl1;
        wxTextCtrl* m_textCtrl2;
        wxTextCtrl* m_textCtrl3;
    };
    //----------------------------------------------------------------------------
    OtherBehaviorTree::OtherBehaviorTree(wxWindow* parent, wxWindow * ok, wxWindow * cancel,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, title)
    {
        wxWindowUpdateLocker temp(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);

        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl3 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);
    }
    //----------------------------------------------------------------------------
    OtherBehaviorTree::~OtherBehaviorTree()
    {

    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // ArtificialIntelligence
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class ArtificialIntelligence : public wxPanel
    {
    public:
        ArtificialIntelligence(wxWindow* parent, wxWindow * ok = 0, wxWindow * cancel = 0,
            wxWindowID id = wxID_ANY, const wxString& title = _("ArtificialIntelligence"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = 0);
        virtual ~ArtificialIntelligence();

    protected:
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrl1;
        wxTextCtrl* m_textCtrl2;
        wxTextCtrl* m_textCtrl3;
    };
    //----------------------------------------------------------------------------
    ArtificialIntelligence::ArtificialIntelligence(wxWindow* parent, wxWindow * ok, wxWindow * cancel,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, title)
    {
        wxWindowUpdateLocker temp(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);

        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl3 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);
    }
    //----------------------------------------------------------------------------
    ArtificialIntelligence::~ArtificialIntelligence()
    {

    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // NoExtendedOptionsPanel
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class NoExtendedOptionsPanel : public wxPanel
    {
    public:
        NoExtendedOptionsPanel(wxWindow* parent) :wxPanel(parent)
        {
            SetBackgroundStyle(wxBG_STYLE_PAINT);
            Bind(wxEVT_PAINT, &NoExtendedOptionsPanel::OnPaint, this);
            Bind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent&) {});
        }

        ~NoExtendedOptionsPanel()
        {
            Unbind(wxEVT_PAINT, &NoExtendedOptionsPanel::OnPaint, this);
            Unbind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent&) {});
        }

        void OnPaint(wxPaintEvent& e)
        {
            wxAutoBufferedPaintDC dc(this);
            wxRect clientRect(GetClientRect());
            wxColour bgColour = gsgs_Style().GetAppBgColour();
            wxColour penColour = gsgs_Style().GetActiveMarkerColour();

            dc.SetBrush(bgColour);
            dc.SetPen(bgColour);
            dc.DrawRectangle(clientRect);

            wxFont font(28, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("ËÎÌå"));
            dc.SetTextForeground(penColour);
            dc.SetFont(font);

            wxSize sz = dc.GetTextExtent("No Extended Options");
            int textX = 0;
            int textY = 0;
            if (clientRect.GetWidth() - sz.x < 0)
                textX = ((sz.x - clientRect.GetWidth()) / 2);
            else
                textX = ((clientRect.GetWidth() - sz.x) / 2);

            if (clientRect.GetHeight() - sz.y < 0)
                textY = ((sz.y - clientRect.GetHeight()) / 2);
            else
                textY = ((clientRect.GetHeight() - sz.y) / 2);
            dc.DrawText("No Extended Options", textX, textY);
        }
    };
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // TrdPartModule
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    TrdPartModule::TrdPartModule(wxWindow* parent, wxWindow * okbutton, wxWindow * cancelbutton,
        wxWindowID id,  const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, "TrdPartModule"),
        mOkButton(okbutton),
        mCancelButton(cancelbutton),
        mCreate(false)
    {
        wxWindowUpdateLocker temp(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        SetSizer(bSizer2);

        mNotebook = new Notebook(this, wxID_ANY, wxSize(128, 128), wxDefaultPosition, wxDefaultSize,kNotebook_ScrollTabs, wxTAB_TRAVERSAL | wxSIMPLE_BORDER,
            Notebook::TT_Scroll);
        bSizer2->Add(mNotebook, 1, wxALL | wxEXPAND, 5);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);
        bSizer2->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrlWorkspaceName = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrlWorkspaceName, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Workspace Path:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        wxBoxSizer* bSizer3 = new wxBoxSizer(wxHORIZONTAL);
        flexGridSizer9->Add(bSizer3, 0, wxEXPAND, 5);

        wxArrayString m_comboBoxPathArr;
        m_comboBoxPath = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), m_comboBoxPathArr, 0);
#if wxVERSION_NUMBER >= 3000
        m_comboBoxPath->SetHint(wxT(""));
#endif
        bSizer3->Add(m_comboBoxPath, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        m_buttonWorkspaceDirPicker = new Button(this, wxID_ANY, _("..."), wxDefaultPosition, wxSize(-1, -1), wxBU_EXACTFIT);
        m_buttonWorkspaceDirPicker->SetForegroundColour(gsgs_Style().GetAppTextColour());
        m_buttonWorkspaceDirPicker->SetDefault();
        m_buttonWorkspaceDirPicker->SetToolTip(_("Browse for folder"));
        bSizer3->Add(m_buttonWorkspaceDirPicker, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        m_checkBoxCreateSeparateDir = new wxCheckBox(this, wxID_ANY, _("Separate directory"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxCreateSeparateDir->SetForegroundColour(gsgs_Style().GetAppTextColour());
        m_checkBoxCreateSeparateDir->SetValue(true);
        bSizer3->Add(m_checkBoxCreateSeparateDir, 0, wxALL | wxEXPAND, 5);

        m_staticText6 = gsgs_Style().createStaticText(this, wxID_ANY, _("Build Path:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText6, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrlBuildmPath = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrlBuildmPath, 0, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("System Path:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrlSystemPath = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrlSystemPath, 0, wxALL | wxEXPAND, 5);

        m_staticText4 = gsgs_Style().createStaticText(this, wxID_ANY, _("Temp Path:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_staticText4, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrlTempPath = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer9->Add(m_textCtrlTempPath, 0, wxALL | wxEXPAND, 5);

        wxStaticBox * wssb = new wxStaticBox(this, wxID_ANY, _("File Info:"));
        wssb->SetForegroundColour(gsgs_Style().GetAppTextColour());
        wxStaticBoxSizer* sbSizer1 = new wxStaticBoxSizer(wssb, wxVERTICAL);

        bSizer2->Add(sbSizer1, 0, wxALL | wxEXPAND, 5);

        m_staticTextWorkspaceFileName = new wxStaticText(this, wxID_ANY, wxT("Workspace File : "), wxDefaultPosition, wxSize(-1, -1), 0);
        m_staticTextWorkspaceFileName->SetForegroundColour(gsgs_Style().GetActiveMarkerColour());
        sbSizer1->Add(m_staticTextWorkspaceFileName, 0, wxALL | wxEXPAND, 5);

        m_staticTextBuildPath = new wxStaticText(this, wxID_ANY, wxT("Build Path : "), wxDefaultPosition, wxSize(-1, -1), 0);
        m_staticTextBuildPath->SetForegroundColour(gsgs_Style().GetActiveMarkerColour());
        sbSizer1->Add(m_staticTextBuildPath, 0, wxALL | wxEXPAND, 5);

        m_staticTextSystemPath = new wxStaticText(this, wxID_ANY, wxT("System Path : "), wxDefaultPosition, wxSize(-1, -1), 0);
        m_staticTextSystemPath->SetForegroundColour(gsgs_Style().GetActiveMarkerColour());
        sbSizer1->Add(m_staticTextSystemPath, 0, wxALL | wxEXPAND, 5);

        m_staticTextTempFileName = new wxStaticText(this, wxID_ANY, wxT("Temporary File : "), wxDefaultPosition, wxSize(-1, -1), 0);
        m_staticTextTempFileName->SetForegroundColour(gsgs_Style().GetActiveMarkerColour());
        sbSizer1->Add(m_staticTextTempFileName, 0, wxALL | wxEXPAND, 5);

        bSizer2->Add(0, 0, 0, wxALL, 5);

        // Connect events
        m_textCtrlWorkspaceName->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(TrdPartModule::OnWorkspacePathUpdated), NULL, this);
        m_comboBoxPath->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(TrdPartModule::OnWorkspacePathUpdated), NULL, this);
        m_checkBoxCreateSeparateDir->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(TrdPartModule::OnWorkspacePathUpdated), NULL, this);
        m_buttonWorkspaceDirPicker->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TrdPartModule::OnWorkspaceDirPicker), NULL, this);
        
        if (mOkButton)
        {
            mOkButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TrdPartModule::OnButtonCreate), NULL, this);
            mOkButton->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(TrdPartModule::OnOKUI), NULL, this);
        }
        wxArrayString history;
        gsgs_Config().Read("C++NewWorkspace/Paths", history, history);

        wxString defaultPath;
        gsgs_Config().Read("C++NewWorkspace/DefaultLocation", defaultPath, gsgs_Path().GetDocumentsDir());
        m_comboBoxPath->SetValue(defaultPath);
        m_comboBoxPath->Append(history);

        m_textCtrlWorkspaceName->SetFocus();
    }
    //----------------------------------------------------------------------------
    TrdPartModule::~TrdPartModule()
    {
        m_textCtrlWorkspaceName->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(TrdPartModule::OnWorkspacePathUpdated), NULL, this);
        m_comboBoxPath->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(TrdPartModule::OnWorkspacePathUpdated), NULL, this);
        m_checkBoxCreateSeparateDir->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(TrdPartModule::OnWorkspacePathUpdated), NULL, this);
        m_buttonWorkspaceDirPicker->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TrdPartModule::OnWorkspaceDirPicker), NULL, this);

        if (mOkButton)
        {
            mOkButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TrdPartModule::OnButtonCreate), NULL, this);
            mOkButton->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(TrdPartModule::OnOKUI), NULL, this);
        }
        // store the recent locations, we keep up to 20 locations
        wxArrayString history = m_comboBoxPath->GetStrings();
        history.Insert(m_comboBoxPath->GetValue(),
            0); // Place the current value at the top so we make sure it gets stored in the history
        wxArrayString uniqueArr;
        for (size_t i = 0; i < history.size(); ++i) {
            if (uniqueArr.Index(history.Item(i)) == wxNOT_FOUND && (uniqueArr.size() < 20)) {
                uniqueArr.Add(history.Item(i));
            }
        }
        gsgs_Config().Write("C++NewWorkspace/Paths", uniqueArr);
        gsgs_Config().Write("C++NewWorkspace/DefaultLocation", m_comboBoxPath->GetValue());
    }
    //--------------------------------------------------------------------------
    void TrdPartModule::initItem(const wxArrayString& options, const WindowList & optionPanels, int selection)
    {
        m_options = options;
        m_optionPanels = optionPanels;

        DoInitialise();

        if (selection >= 0 && selection < (int)options.size()) {
            mNotebook->SetSelection(selection);
        }
    }
    //--------------------------------------------------------------------------
    void TrdPartModule::OnWorkspacePathUpdated(wxCommandEvent& event)
    {
        // update the static text control with the actual path
        m_workspacePath.Empty();
        //wxString workspacePath;
        m_workspacePath << m_comboBoxPath->GetValue();

        m_workspacePath = m_workspacePath.Trim().Trim(false);

        //wxString tmpSep(gsgsPathSep);
        if (!m_workspacePath.EndsWith(gsgsPathSep) && m_workspacePath.IsEmpty() == false) {
            m_workspacePath << gsgsPathSep;
        }

        if (m_textCtrlWorkspaceName->GetValue().Trim().Trim(false).IsEmpty()) {
            m_staticTextWorkspaceFileName->SetLabel("Workspace File : None");
            m_staticTextBuildPath->SetLabel("Build Path : None");
            m_staticTextTempFileName->SetLabel("Temporary File : None");
            m_textCtrlSystemPath->SetLabel("None");
            m_textCtrlBuildmPath->SetLabel("None");
            m_textCtrlTempPath->SetValue(gsgs_Path().GetTempDir());
            return;
        }
        wxString syspath;
        wxGetEnv("PATH", &syspath);

        if (m_checkBoxCreateSeparateDir->IsChecked()) {
            m_workspacePath << m_textCtrlWorkspaceName->GetValue();
            m_workspacePath << gsgsPathSep;
        }
        m_textCtrlBuildmPath->SetLabel(m_workspacePath + "build");
        m_textCtrlTempPath->SetLabel(gsgs_Path().GetTempDir() + gsgsPathSep + m_textCtrlWorkspaceName->GetValue());
        //m_textCtrlSystemPath->SetLabel(::wxGetCwd());
        m_textCtrlSystemPath->SetLabel(syspath);
        m_workspacePath << m_textCtrlWorkspaceName->GetValue();
        m_workspacePath << wxT(".gsgsws");
        m_staticTextWorkspaceFileName->SetLabel("Workspace File : " + m_workspacePath);
        m_staticTextSystemPath->SetLabel("System File : " + syspath);
        m_staticTextBuildPath->SetLabel("Build Path : " + m_textCtrlBuildmPath->GetValue());
        m_staticTextTempFileName->SetLabel("Temporary File : " + gsgs_Path().GetTempDir() + gsgsPathSep + m_textCtrlWorkspaceName->GetValue());
    }
    //--------------------------------------------------------------------------
    void TrdPartModule::OnWorkspaceDirPicker(wxCommandEvent& event)
    {
        const wxString& dir = ::wxDirSelector(_("Choose a folder:"), m_comboBoxPath->GetValue());
        if (!dir.empty()) {

            static wxString INVALID_CHARS = " ,'()";
            if (dir.find_first_of(INVALID_CHARS) != wxString::npos) {
                int answer = ::wxMessageBox(wxString() << _("The selected project path '") << dir
                    << _("'\nContains some invalid characters\nContinue ?"),
                    "gsgs", wxYES_NO | wxCANCEL | wxICON_WARNING, this);
                if (answer != wxYES) {
                    return;
                }
            }

            // Use SetValue to ensure that an TEXT_UPDATE event is fired
            m_comboBoxPath->SetValue(dir);
        }
    }
    //--------------------------------------------------------------------------
    void TrdPartModule::OnButtonCreate(wxCommandEvent& event)
    {
        event.Skip();
        if (mNotebook->GetSelection() == 0)
        {
            // validate that the path part is valid
            //m_workspacePath = m_staticTextWorkspaceFileName->GetLabel();
            wxFileName fn(m_workspacePath);

            if (m_checkBoxCreateSeparateDir->IsChecked()) {
                // dont test the result
                fn.Mkdir(fn.GetPath(), 0777, wxPATH_MKDIR_FULL);
            }

            if (wxDirExists(fn.GetPath())) 
            {
                gsgs_Manager()->CreateWorkspace(fn.GetName(), fn.GetPath());
                mCreate = true;
            }
            else
            {
                wxMessageBox(_("Invalid path: ") + fn.GetPath(), _("Create file error"), wxOK | wxICON_HAND);
            }
        }
        else
        {
            mCreate = false;
        }
        event.SetClientData(&mCreate);
    }
    //--------------------------------------------------------------------------
    void TrdPartModule::OnOKUI(wxUpdateUIEvent& event) 
    { 
        event.Enable(!m_textCtrlWorkspaceName->IsEmpty()); 
    }
    //--------------------------------------------------------------------------
    void TrdPartModule::DoInitialise()
    {
        wxWindowUpdateLocker winlok(mNotebook);
        mNotebook->DeleteAllPages();

        //wxString filter = m_searchCtrl->GetValue();
        wxBitmap dummyBmp = gsgs_Image().LoadBitmap("101010");
        if (dummyBmp.IsOk() && m_options.size() == m_optionPanels.size())
        {
            for (size_t i = 0; i < m_options.size(); ++i) {
                //if (FileUtil::FuzzyMatch(filter, m_options.Item(i))) {
                    wxWindow * tpanel = m_optionPanels[i];
                    if (tpanel == 0)
                        tpanel = new NoExtendedOptionsPanel(mNotebook);
                    else
                        tpanel->Reparent(mNotebook);
                    mNotebook->AddPage(tpanel, m_options[i], false, dummyBmp);
                //}
            }
        }
    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // CreateWorkspaceDialog
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    #define WorkspaceADD(n, a, b) options##n.Add(a); optionPanels##n.push_back(b);
    CreateWorkspaceDialog::CreateWorkspaceDialog(wxWindow* parent):
        mCreate(false)
    {
        wxWindowUpdateLocker tt(this);
        SingleChoiceDialog::Create(parent);
        if(mOkButton)
            mOkButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CreateWorkspaceDialog::OnButtonCreate), NULL, this);
        
        wxArrayString options1;
        WindowList optionPanels1;
        WorkspaceADD(1,"»ÄÖÞAPI", new HZapi(GetListCtrl(), mOkButton, mCancelButton));
        WorkspaceADD(1, "Role-playing game(RPG)", new RGBGame(GetListCtrl(), mOkButton, mCancelButton));
        WorkspaceADD(1, "Action Game(ATG)", new ATGGame(GetListCtrl(), mOkButton, mCancelButton));
        WorkspaceADD(1, "Shooting Game(STG)", new STGGame(GetListCtrl(), mOkButton, mCancelButton));
        WorkspaceADD(1, "Fighting Game(FTG)", new FTGGame(GetListCtrl(), mOkButton, mCancelButton));
        WorkspaceADD(1, "Adventure Game(AVG)", new AVGGame(GetListCtrl(), mOkButton, mCancelButton));
        WorkspaceADD(1, "Simulation Game(SLG)", new SLGGame(GetListCtrl(), mOkButton, mCancelButton));
        WorkspaceADD(1, "Strategy Game", new StrategyGame(GetListCtrl(), mOkButton, mCancelButton));
        WorkspaceADD(1, "Music Game", new MusicGame(GetListCtrl(), mOkButton, mCancelButton));
        WorkspaceADD(1, "Casual Game", new CasualGame(GetListCtrl(), mOkButton, mCancelButton));
        WorkspaceADD(1, "Sport Game", new SportGame(GetListCtrl(), mOkButton, mCancelButton));
        WorkspaceADD(1, "Racing Game", new RacingGame(GetListCtrl(), mOkButton, mCancelButton));
        WorkspaceADD(1, "Other-Terrain", new OtherTerrain(GetListCtrl(), mOkButton, mCancelButton));
        WorkspaceADD(1, "Other-Geometric Model", new OtherGeometricModel(GetListCtrl(), mOkButton, mCancelButton));
        WorkspaceADD(1, "Other-Mathematical Model", new OtherMathematicalModel(GetListCtrl(), mOkButton, mCancelButton));
        WorkspaceADD(1, "Other-Behavior tree", new OtherBehaviorTree(GetListCtrl(), mOkButton, mCancelButton));
        WorkspaceADD(1, "Artificial Intelligence", new ArtificialIntelligence(GetListCtrl(), mOkButton, mCancelButton));
        
        TrdPartModule * nwd = new TrdPartModule(GetListCtrl(), mOkButton, mCancelButton);
        wxArrayString options2 = gsgs_Workspace().GetAllWorkspaces();
        WindowList optionPanels2;
        optionPanels2.resize(options2.size(), 0);
        WorkspaceADD(2, "Python", 0);
        WorkspaceADD(2, "C#", 0);
        WorkspaceADD(2, "Java", 0);
        WorkspaceADD(2, "Go", 0);
        WorkspaceADD(2, "Ruby", 0);
        WorkspaceADD(2, "TypeScript", 0);
        WorkspaceADD(2, "Rust", 0);
        nwd->initItem(options2, optionPanels2);
        WorkspaceADD(1, "3rd Party Modules", nwd);

        initItem(options1, optionPanels1, 0);
    }
    //----------------------------------------------------------------------------
    CreateWorkspaceDialog::~CreateWorkspaceDialog()
    {
        if(mOkButton)
            mOkButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CreateWorkspaceDialog::OnButtonCreate), NULL, this);
    }
    //--------------------------------------------------------------------------
    void CreateWorkspaceDialog::OnButtonCreate(wxCommandEvent& event)
    {
        event.Skip();
        void * temp = event.GetClientData();
        mCreate = *(bool *)temp;
    }
    //----------------------------------------------------------------------------
    bool CreateWorkspaceDialog::isUnChoice() const
    {
        return !mCreate;
    }
    //----------------------------------------------------------------------------
}