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

#include "gsgsPluginManagerDlg.h"
#include "gsgsWindowManager.h"
#include "gsgsButton.h"
#include "plugin.h"
#include "globals.h"
#include <wx/stattext.h>

namespace gsgs
{
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    // PluginMgrDlg
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    PluginMgrDlg::PluginMgrDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
        const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style)
    {
        wxWindowUpdateLocker ctlock(this);
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer1);

        wxBoxSizer* boxSizer16 = new wxBoxSizer(wxHORIZONTAL);

        bSizer1->Add(boxSizer16, 1, wxEXPAND, 5);

        m_splitter36 = new SplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_THIN_SASH);
        m_splitter36->SetBackgroundColour(gsgs_Style().GetAppBgColour());
        m_splitter36->SetSashGravity(0);
        m_splitter36->SetMinimumPaneSize(10);

        boxSizer16->Add(m_splitter36, 1, wxALL | wxEXPAND, 5);

        m_splitterPage40 = new wxPanel(m_splitter36, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxTAB_TRAVERSAL | wxSIMPLE_BORDER);
        m_splitterPage40->SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* boxSizer12 = new wxBoxSizer(wxVERTICAL);
        m_splitterPage40->SetSizer(boxSizer12);

        m_dvListCtrl = new ThemedListCtrl(m_splitterPage40, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
            wxDV_NO_HEADER | wxDV_ROW_LINES | wxDV_SINGLE);

        boxSizer12->Add(m_dvListCtrl, 1, wxEXPAND, 5);

        m_dvListCtrl->AppendIconTextColumn(_("Plugins"), wxDATAVIEW_CELL_INERT, -2, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);

        m_splitterPage44 = new wxPanel(m_splitter36, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxTAB_TRAVERSAL | wxSIMPLE_BORDER);
        m_splitterPage44->SetBackgroundColour(gsgs_Style().GetAppBgColour());
        m_splitter36->SplitVertically(m_splitterPage40, m_splitterPage44, 250);

        wxBoxSizer* boxSizer14 = new wxBoxSizer(wxVERTICAL);
        m_splitterPage44->SetSizer(boxSizer14);

        m_richTextCtrl = new wxRichTextCtrl(m_splitterPage44, wxID_ANY, wxT("gsgsRichTextCtrl"), wxDefaultPosition,
            wxSize(-1, 350), wxTE_MULTILINE | wxTE_PROCESS_TAB | wxTE_PROCESS_ENTER | wxWANTS_CHARS | wxNO_BORDER);
        m_richTextCtrl->SetBackgroundColour(gsgs_Style().GetAppBgColour());
        m_richTextCtrl->Disable();
        m_richTextCtrl->SetDefaultStyle(wxTextAttr(gsgs_Style().GetActiveMarkerColour()));

        boxSizer14->Add(m_richTextCtrl, 1, wxEXPAND, 5);

        wxFlexGridSizer* flexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer9->SetFlexibleDirection(wxBOTH);
        flexGridSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer9->AddGrowableCol(1, 1);
        boxSizer14->Add(flexGridSizer9, 0, wxALL | wxEXPAND, 5);

        wxStaticText * m_staticText1 = gsgs_Style().createStaticText(m_splitterPage44, wxID_ANY, _("Get Cmd:"), wxDefaultPosition, wxDefaultSize, 0);
        flexGridSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        wxTextCtrl * m_textCtrlWorkspaceName1 = gsgs_Style().createTextCtrl(m_splitterPage44, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize);
        flexGridSizer9->Add(m_textCtrlWorkspaceName1, 0, wxALL | wxEXPAND, 5);

        wxStaticText * m_staticText4 = gsgs_Style().createStaticText(m_splitterPage44, wxID_ANY, _("Install Cmd:"), wxDefaultPosition, wxDefaultSize, 0);
        flexGridSizer9->Add(m_staticText4, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        wxTextCtrl * m_textCtrlWorkspaceName4 = gsgs_Style().createTextCtrl(m_splitterPage44, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
        flexGridSizer9->Add(m_textCtrlWorkspaceName4, 0, wxALL | wxEXPAND, 5);

        wxStaticText * m_staticText5 = gsgs_Style().createStaticText(m_splitterPage44, wxID_ANY, _("UnInstall Cmd:"), wxDefaultPosition, wxDefaultSize, 0);
        flexGridSizer9->Add(m_staticText5, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        wxTextCtrl * m_textCtrlWorkspaceName5 = gsgs_Style().createTextCtrl(m_splitterPage44, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
        flexGridSizer9->Add(m_textCtrlWorkspaceName5, 0, wxALL | wxEXPAND, 5);

        wxStaticText * m_staticText2 = gsgs_Style().createStaticText(m_splitterPage44, wxID_ANY, _("Update Cmd:"), wxDefaultPosition, wxDefaultSize, 0);
        flexGridSizer9->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        wxTextCtrl * m_textCtrlWorkspaceName2 = gsgs_Style().createTextCtrl(m_splitterPage44, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
        flexGridSizer9->Add(m_textCtrlWorkspaceName2, 0, wxALL | wxEXPAND, 5);

        wxStaticText * m_staticText3 = gsgs_Style().createStaticText(m_splitterPage44, wxID_ANY, _("Upload Cmd:"), wxDefaultPosition, wxDefaultSize, 0);
        flexGridSizer9->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        wxTextCtrl * m_textCtrlWorkspaceName3 = gsgs_Style().createTextCtrl(m_splitterPage44, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
        flexGridSizer9->Add(m_textCtrlWorkspaceName3, 0, wxALL | wxEXPAND, 5);

        DefaultTreeCtrl * m_listTable = new DefaultTreeCtrl(m_splitterPage44, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxTR_SINGLE | wxTR_ROW_LINES | wxTR_HIDE_ROOT | wxTR_ENABLE_SEARCH);

        m_listTable->AddHeader(_("Version Number"));
        m_listTable->AddHeader(_("Version Name"));
        m_listTable->AddHeader(_("Version Machine"));
        m_listTable->AddHeader(_("Version Linsence"));
        m_listTable->AddHeader(_("Version Branch"));

        // Only sort top level items, don't sort their children
        m_listTable->AddTreeStyle(wxTR_SORT_TOP_LEVEL);
        m_listTable->SetShowHeader(true);

        wxTreeItemId lroot = m_listTable->AddRoot(_("Engine"));
        wxTreeItemId tempid = m_listTable->AppendItem(lroot, wxT("6.5.6"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "ideal", 1);
        m_listTable->SetItemText(tempid, "x86", 2);
        m_listTable->SetItemText(tempid, "Personal/Competition/Commerce", 3);
        m_listTable->SetItemText(tempid, "main", 4);
        tempid = m_listTable->AppendItem(lroot, wxT("6.4.0"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "ideal", 1);
        m_listTable->SetItemText(tempid, "x86", 2);
        m_listTable->SetItemText(tempid, "Personal/Competition/Commerce", 3);
        m_listTable->SetItemText(tempid, "main", 4);
        tempid = m_listTable->AppendItem(lroot, wxT("6.3.0"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "ideal", 1);
        m_listTable->SetItemText(tempid, "x86", 2);
        m_listTable->SetItemText(tempid, "Personal/Competition/Commerce", 3);
        m_listTable->SetItemText(tempid, "main", 4);
        tempid = m_listTable->AppendItem(lroot, wxT("6.2.0"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "ideal", 1);
        m_listTable->SetItemText(tempid, "x86", 2);
        m_listTable->SetItemText(tempid, "Personal/Competition/Commerce", 3);
        m_listTable->SetItemText(tempid, "main", 4);
        tempid = m_listTable->AppendItem(lroot, wxT("6.1.4"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "ideal", 1);
        m_listTable->SetItemText(tempid, "x86", 2);
        m_listTable->SetItemText(tempid, "Personal/Competition/Commerce", 3);
        m_listTable->SetItemText(tempid, "main", 4);
        tempid = m_listTable->AppendItem(lroot, wxT("6.1.3"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "ideal", 1);
        m_listTable->SetItemText(tempid, "x86", 2);
        m_listTable->SetItemText(tempid, "Personal/Competition/Commerce", 3);
        m_listTable->SetItemText(tempid, "main", 4);
        tempid = m_listTable->AppendItem(lroot, wxT("6.1.2"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "ideal", 1);
        m_listTable->SetItemText(tempid, "x86", 2);
        m_listTable->SetItemText(tempid, "Personal/Competition/Commerce", 3);
        m_listTable->SetItemText(tempid, "main", 4);
        tempid = m_listTable->AppendItem(lroot, wxT("6.1.1"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "ideal", 1);
        m_listTable->SetItemText(tempid, "x86", 2);
        m_listTable->SetItemText(tempid, "Personal/Competition/Commerce", 3);
        m_listTable->SetItemText(tempid, "main", 4);
        tempid = m_listTable->AppendItem(lroot, wxT("6.1.0"));
        m_listTable->Check(tempid, true, 0);
        m_listTable->SetItemText(tempid, "ideal", 1);
        m_listTable->SetItemText(tempid, "x86", 2);
        m_listTable->SetItemText(tempid, "Personal/Competition/Commerce", 3);
        m_listTable->SetItemText(tempid, "main", 4);
        boxSizer14->Add(m_listTable, 1, wxALL | wxEXPAND, 10);

        wxBoxSizer* bSizer3 = new wxBoxSizer(wxHORIZONTAL);
        boxSizer14->Add(bSizer3, 0, wxRIGHT | wxLEFT | wxEXPAND, 200);

        Button * b1 = new Button(m_splitterPage44, wxID_ANY, _("Install"), wxDefaultPosition, wxDefaultSize, 0);
        bSizer3->Add(b1, 0, wxALL | wxEXPAND, 5);
        bSizer3->AddSpacer(5);
        Button * b2 = new Button(m_splitterPage44, wxID_ANY, _("UnInstall"), wxDefaultPosition, wxDefaultSize, 0);
        bSizer3->Add(b2, 0, wxALL | wxEXPAND, 5);
        bSizer3->AddSpacer(5);
        Button * b3 = new Button(m_splitterPage44, wxID_ANY, _("Update"), wxDefaultPosition, wxDefaultSize, 0);
        bSizer3->Add(b3, 0, wxALL | wxEXPAND, 5);
        bSizer3->AddSpacer(5);

        wxBoxSizer* boxSizer18 = new wxBoxSizer(wxVERTICAL);

        boxSizer16->Add(boxSizer18, 0, wxEXPAND, 5);

        m_button25 = new Button(this, wxID_ANY, _("Install(From File)"), wxDefaultPosition, wxDefaultSize, 0);

        boxSizer18->Add(m_button25, 0, wxALL | wxEXPAND, 5);

        m_button27 = new Button(this, wxID_ANY, _("Clear History(All)"), wxDefaultPosition, wxDefaultSize, 0);

        boxSizer18->Add(m_button27, 0, wxALL | wxEXPAND, 5);

        m_button26 = new Button(this, wxID_ANY, _("ReInstall(All)"), wxDefaultPosition, wxDefaultSize, 0);

        boxSizer18->Add(m_button26, 0, wxALL | wxEXPAND, 5);

        m_button24 = new Button(this, wxID_ANY, _("UnInstall(All)"), wxDefaultPosition, wxDefaultSize, 0);

        boxSizer18->Add(m_button24, 0, wxALL | wxEXPAND, 5);

        boxSizer18->AddSpacer(20);

        m_button23 = new Button(this, wxID_ANY, _("Update(All)"), wxDefaultPosition, wxDefaultSize, 0);

        boxSizer18->Add(m_button23, 0, wxALL | wxEXPAND, 5);

        boxSizer18->AddSpacer(20);

        m_button20 = new Button(this, wxID_ANY, _("Load(All)"), wxDefaultPosition, wxDefaultSize, 0);

        boxSizer18->Add(m_button20, 0, wxALL | wxEXPAND, 5);

        m_button22 = new Button(this, wxID_ANY, _("UnLoad(All)"), wxDefaultPosition, wxDefaultSize, 0);

        boxSizer18->Add(m_button22, 0, wxALL | wxEXPAND, 5);

        m_stdBtnSizer26 = new DialogButtonSizer();

        bSizer1->Add(m_stdBtnSizer26, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);

        m_buttonOK = new Button(this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
        m_buttonOK->SetDefault();
        m_stdBtnSizer26->AddButton(m_buttonOK);

        m_button30 = new Button(this, wxID_CANCEL, wxT("CANCEL"), wxDefaultPosition, wxDefaultSize, 0);
        m_stdBtnSizer26->AddButton(m_button30);
        m_stdBtnSizer26->Realize();

        // Connect events
        m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler(PluginMgrDlg::OnItemSelected), NULL, this);
        m_button20->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PluginMgrDlg::OnCheckAll), NULL, this);
        m_button20->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(PluginMgrDlg::OnCheckAllUI), NULL, this);
        m_button22->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PluginMgrDlg::OnUncheckAll), NULL, this);
        m_button22->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(PluginMgrDlg::OnUncheckAllUI), NULL, this);
        m_buttonOK->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PluginMgrDlg::OnButtonOK), NULL, this);

        if (GetParent()) {
            CentreOnParent(wxBOTH);
        }
        else {
            CentreOnScreen(wxBOTH);
        }
        this->Initialize();
    }
    //-----------------------------------------------------------------------------------
    PluginMgrDlg::~PluginMgrDlg()
    {
        m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler(PluginMgrDlg::OnItemSelected), NULL, this);
        m_button20->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PluginMgrDlg::OnCheckAll), NULL, this);
        m_button20->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(PluginMgrDlg::OnCheckAllUI), NULL, this);
        m_button22->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PluginMgrDlg::OnUncheckAll), NULL, this);
        m_button22->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(PluginMgrDlg::OnUncheckAllUI), NULL, this);
        m_buttonOK->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PluginMgrDlg::OnButtonOK), NULL, this);
    }
    //-----------------------------------------------------------------------------------
    void PluginMgrDlg::Initialize()
    {
        Config conf("plugins.conf");
        PluginInfoArray plugins;
        conf.ReadItem(&plugins);

        m_initialDisabledPlugins = plugins.GetDisabledPlugins();
        std::sort(m_initialDisabledPlugins.begin(), m_initialDisabledPlugins.end());

        const PluginInfo::PluginMap_t& pluginsMap = plugins.GetPlugins();

        // Clear the list
        m_dvListCtrl->DeleteAllItems();

        wxDataViewItem temp1 = m_dvListCtrl->AppendItem("Engine Core", true, -1, -1, 0);

        wxDataViewItem temp2 = m_dvListCtrl->AppendItem("Engine Plugin");
        wxDataViewItem temp21 = m_dvListCtrl->AppendItem(temp2, "Render System");
        wxDataViewItem temp25 = m_dvListCtrl->AppendItem(temp2, "File System");
        wxDataViewItem temp22 = m_dvListCtrl->AppendItem(temp2, "Input System");
        wxDataViewItem temp23 = m_dvListCtrl->AppendItem(temp2, "Voice Code");
        wxDataViewItem temp24 = m_dvListCtrl->AppendItem(temp2, "Image Code");
        m_dvListCtrl->AppendItem(temp25, "Zip FileSystem", true, -1, -1 , 0);
        m_dvListCtrl->AppendItem(temp24, "Png Image Codec", true, -1, -1, 0);
        m_dvListCtrl->AppendItem(temp22, "JoyPad Input", true, -1, -1, 0);
        m_dvListCtrl->AppendItem(temp22, "Mouse Input", true, -1, -1, 0);
        m_dvListCtrl->AppendItem(temp22, "Keyboard Input", true, -1, -1, 0);
        m_dvListCtrl->AppendItem(temp22, "Touch Imput", true, -1, -1, 0);
        m_dvListCtrl->AppendItem(temp21, "Opengl3 RenderSystem", true, -1, -1, 0);
        m_dvListCtrl->AppendItem(temp21, "Opengl es2 RenderSystem", true, -1, -1, 0);
        m_dvListCtrl->AppendItem(temp24, "Jpeg Image Code", true, -1, -1, 0);
        m_dvListCtrl->AppendItem(temp23, "Ogg Voice Code", true, -1, -1, 0);
        m_dvListCtrl->AppendItem(temp23, "Speex Voice Code", true, -1, -1, 0);
        m_dvListCtrl->AppendItem(temp23, "MP3 Voice Code", true, -1, -1, 0);
        wxDataViewItem temp3 = m_dvListCtrl->AppendItem("Engine Framework");
        wxDataViewItem temp31 = m_dvListCtrl->AppendItem(temp3, "RPG template");
        m_dvListCtrl->AppendItem(temp31, "RPG template");
        m_dvListCtrl->AppendItem(temp3, "ATG template");
        m_dvListCtrl->AppendItem(temp3, "STG template");
        m_dvListCtrl->AppendItem(temp3, "FTG template");
        m_dvListCtrl->AppendItem(temp3, "AVG template");
        m_dvListCtrl->AppendItem(temp3, "SLG template");
        m_dvListCtrl->AppendItem(temp3, "Strategy Game template");
        m_dvListCtrl->AppendItem(temp3, "Music Game template");
        m_dvListCtrl->AppendItem(temp3, "Casual Game template");
        m_dvListCtrl->AppendItem(temp3, "Sport Game template");
        m_dvListCtrl->AppendItem(temp3, "Racing Game template");
        wxDataViewItem temp4 = m_dvListCtrl->AppendItem("Engine Material Resource");
        m_dvListCtrl->AppendItem(temp4, wxT("ÖÙÏÄÒ¹"));
        m_dvListCtrl->AppendItem(temp4, wxT("¹í÷È"));
        m_dvListCtrl->AppendItem(temp4, wxT("À¯±Ê»­"));
        m_dvListCtrl->AppendItem(temp4, wxT("ÓÍ»­"));
        m_dvListCtrl->AppendItem(temp4, wxT("Ë®Ä«»­"));
        m_dvListCtrl->AppendItem(temp4, wxT("Colour1"));
        m_dvListCtrl->AppendItem(temp4, wxT("Colour2"));
        m_dvListCtrl->AppendItem(temp4, wxT("Colour3"));
        m_dvListCtrl->AppendItem(temp4, wxT("Colour4"));
        m_dvListCtrl->AppendItem(temp4, wxT("Colour5"));
        wxDataViewItem temp5 = m_dvListCtrl->AppendItem("GSGS IDE Plugin");
        PluginInfo::PluginMap_t::const_iterator iter = pluginsMap.begin();
        for (const auto& vt : pluginsMap) {
            const PluginInfo& info = vt.second;
            wxVector<wxVariant> cols;
            cols.push_back(::MakeCheckboxVariant(info.GetName(), plugins.CanLoad(info), wxNOT_FOUND));
            m_dvListCtrl->AppendItem(temp5, cols);
        }
        wxDataViewItem temp6 = m_dvListCtrl->AppendItem("GSGS IDE Assist");
        m_dvListCtrl->AppendItem(temp6, wxT("Geometric Model Viewer"));
        m_dvListCtrl->AppendItem(temp6, wxT("Mathematical Model Editor"));
        m_dvListCtrl->AppendItem(temp6, wxT("Behavior Tree Editor"));
        m_dvListCtrl->AppendItem(temp6, wxT("Docker"));
        m_dvListCtrl->AppendItem(temp6, wxT("Virtual Machine"));
        if (!m_dvListCtrl->IsEmpty()) {
            m_dvListCtrl->Select(m_dvListCtrl->RowToItem(0));
            CreateInfoPage(temp1);
        }

    }
    //-----------------------------------------------------------------------------------
    void PluginMgrDlg::OnItemSelected(wxDataViewEvent& event)
    {
        wxDataViewItem item = event.GetItem();
        //CreateInfoPage(m_dvListCtrl->ItemToRow(item));
        CreateInfoPage(item);
    }
    //-----------------------------------------------------------------------------------
    void PluginMgrDlg::OnButtonOK(wxCommandEvent& event)
    {
        Config conf("plugins.conf");
        PluginInfoArray plugins;
        conf.ReadItem(&plugins);

        wxArrayString disabledPlugins;
        for (size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
            wxDataViewItem item = m_dvListCtrl->RowToItem(i);
            if (!m_dvListCtrl->IsItemChecked(item)) { disabledPlugins.Add(m_dvListCtrl->GetItemText(item)); }
        }

        std::sort(disabledPlugins.begin(), disabledPlugins.end());
        plugins.DisablePugins(disabledPlugins);
        conf.WriteItem(&plugins);

        EndModal(disabledPlugins == m_initialDisabledPlugins ? wxID_CANCEL : wxID_OK);
    }
    //-----------------------------------------------------------------------------------
    void PluginMgrDlg::WritePropertyLine(const wxString& text)
    {
        m_richTextCtrl->Newline();
        m_richTextCtrl->BeginBold();
        m_richTextCtrl->WriteText(text);
        m_richTextCtrl->EndBold();
    }
    //-----------------------------------------------------------------------------------
    void PluginMgrDlg::WritePropertyLine(const wxString& label, const wxString& text)
    {
        m_richTextCtrl->Newline();
        m_richTextCtrl->BeginBold();
        m_richTextCtrl->WriteText(label + " : ");
        m_richTextCtrl->EndBold();
        m_richTextCtrl->WriteText(text);
    }
    //-----------------------------------------------------------------------------------
    void PluginMgrDlg::WritePropertyNewLine(const wxString& text)
    {
        m_richTextCtrl->Newline();
        m_richTextCtrl->BeginBold();
        m_richTextCtrl->WriteText(text);
        m_richTextCtrl->EndBold();
        m_richTextCtrl->Newline();
    }
    //-----------------------------------------------------------------------------------
    void PluginMgrDlg::WritePropertyNewLine(const wxString& label, const wxString& text)
    {
        m_richTextCtrl->Newline();
        m_richTextCtrl->BeginBold();
        m_richTextCtrl->WriteText(label + " : ");
        m_richTextCtrl->EndBold();
        m_richTextCtrl->Newline();
        m_richTextCtrl->WriteText(text);
    }
    //-----------------------------------------------------------------------------------
    void PluginMgrDlg::CreateInfoPage(const wxDataViewItem& item)
    {
        Config conf("plugins.conf");
        PluginInfoArray plugins;
        conf.ReadItem(&plugins);

        m_richTextCtrl->Clear();
        m_richTextCtrl->Freeze();
        // get the plugin name
        //wxString pluginName = m_dvListCtrl->GetItemText(m_dvListCtrl->RowToItem(index));
        const wxString & pluginName = m_dvListCtrl->GetItemText(item);
        PluginInfo::PluginMap_t::const_iterator iter = plugins.GetPlugins().find(pluginName);
        if (iter != plugins.GetPlugins().end()) 
        {
            const PluginInfo & info = iter->second;
            m_richTextCtrl->BeginBold();
            m_richTextCtrl->WriteText("Name : " + info.GetName());
            m_richTextCtrl->EndBold();
            WritePropertyLine(_("Version"), info.GetVersion());

            m_richTextCtrl->Newline();

            WritePropertyLine(_("Author"), info.GetAuthor());
            WritePropertyLine(_("License"), info.GetLicense());
            WritePropertyLine(_("Dependence"), info.GetDependence());

            m_richTextCtrl->Newline();

            WritePropertyNewLine(_("Description"), info.GetDescription());

            m_richTextCtrl->Newline();

            WritePropertyLine(_("Status"), plugins.CanLoad(info) ? _("Loaded") : _("UnLoaded"));
        }
        else
        {
            m_richTextCtrl->BeginBold();
            m_richTextCtrl->WriteText("Name : Unknow");
            m_richTextCtrl->EndBold();
            WritePropertyLine(_("Version : Unknow"));

            m_richTextCtrl->Newline();

            WritePropertyLine(_("Author : Unknow"));
            WritePropertyLine(_("License : Unknow"));
            WritePropertyLine(_("Dependence: Unknow"));

            m_richTextCtrl->Newline();

            WritePropertyNewLine(_("Description : Unknow"));

            m_richTextCtrl->Newline();

            WritePropertyLine(_("Status : Unknow"));
        }
        m_richTextCtrl->Thaw();
        m_richTextCtrl->Show();
    }
    //-----------------------------------------------------------------------------------
    void PluginMgrDlg::OnCheckAll(wxCommandEvent& event)
    {
        for (size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) 
        {
            m_dvListCtrl->SetItemChecked(m_dvListCtrl->RowToItem(i), true);
        }
    }
    //-----------------------------------------------------------------------------------
    void PluginMgrDlg::OnCheckAllUI(wxUpdateUIEvent& event)
    {
        bool atLeastOneIsUnChecked = false;
        for (size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) 
        {
            wxDataViewItem item = m_dvListCtrl->RowToItem(i);
            if (!m_dvListCtrl->IsItemChecked(item)) {
                atLeastOneIsUnChecked = true;
                break;
            }
        }
        event.Enable(atLeastOneIsUnChecked);
    }
    //-----------------------------------------------------------------------------------
    void PluginMgrDlg::OnUncheckAll(wxCommandEvent& event)
    {
        for (size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) 
        {
            m_dvListCtrl->SetItemChecked(m_dvListCtrl->RowToItem(i), false);
        }
    }
    //-----------------------------------------------------------------------------------
    void PluginMgrDlg::OnUncheckAllUI(wxUpdateUIEvent& event)
    {
        bool atLeastOneIsChecked = false;
        for (size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
            wxDataViewItem item = m_dvListCtrl->RowToItem(i);
            if (m_dvListCtrl->IsItemChecked(item)) {
                atLeastOneIsChecked = true;
                break;
            }
        }
        event.Enable(atLeastOneIsChecked);
    }
    //-----------------------------------------------------------------------------------
}