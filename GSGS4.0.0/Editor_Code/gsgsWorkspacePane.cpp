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
#include "gsgsWorkspacePane.h"
#include "gsgsLogManager.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsConfig.h"
#include "gsgsFrame.h"
#include "gsgsFileUtil.h"
#include "gsgsPluginCommon.h"
#include "gsgsEditor.h"
#include "gsgsWindowManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsParseWorker.h"
#include "gsgsPluginManager.h"
#include "gsgsPerspectivePane.h"
#include "gsgsImageManager.h"
#include "gsgsStyleManager.h"
#include "gsgsToolBar.h"
#include "gsgsTreeCtrl.h"
#include "gsgsSharedPtr.h"
#include "gsgsFileView.h"
#include "gsgsManager.h"
#include "globals.h"
#include "plugin.h"
#include "imanager.h"
#include "ieditor.h"
#include "gsgsTabgroupManager.h"
#include "xmlutils.h"
#include "CommonController.h"

#include <wx/generic/textdlgg.h>
#include <wx/simplebook.h>
#include <wx/dcbuffer.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/regex.h>
#include <wx/textdlg.h>
#include <wx/xml/xml.h>
#include <wx/dirdlg.h>
#include <wx/clntdata.h>
#include <wx/stc/stc.h>
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/stattext.h>
#include <wx/xrc/xmlres.h>
#include <wx/gauge.h>

#ifdef __WXGTK20__
// We need this ugly hack to workaround a gtk2-wxGTK name-clash
// See http://trac.wxwidgets.org/ticket/10883
#define GSocket GlibGSocket
#include <gtk/gtk.h>
#undef GSocket
#endif

namespace gsgs
{
    class EditWorkspaceConfDlg : public wxDialog
    {
    public:
        EditWorkspaceConfDlg(wxWindow * parent, wxWindowID id = wxID_ANY, const wxString& title = _("Edit Config"),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = gsgsMinDlgSize, long style = gsgsDlgStyle) : 
                wxDialog(parent, id, title, pos, size, style)
        {
            wxWindowUpdateLocker templ(this);
            SetBackgroundColour(gsgs_Style().GetAppBgColour());
            this->SetSizeHints(wxDefaultSize, wxDefaultSize);

            wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

            m_wspConfList = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
            mainSizer->Add(m_wspConfList, 1, wxALL | wxEXPAND, 5);

            wxBoxSizer* btnSizer;
            btnSizer = new wxBoxSizer(wxVERTICAL);

            m_buttonRename = new Button(this, wxID_ANY, _("&Rename"), wxDefaultPosition, wxDefaultSize, 0);
            btnSizer->Add(m_buttonRename, 0, wxALL, 5);

            m_buttonDelete = new Button(this, wxID_ANY, _("&Delete"), wxDefaultPosition, wxDefaultSize, 0);
            btnSizer->Add(m_buttonDelete, 0, wxALL, 5);

            m_buttonClose = new Button(this, wxID_CANCEL, _("&Close"), wxDefaultPosition, wxDefaultSize, 0);
            btnSizer->Add(m_buttonClose, 0, wxALL, 5);

            mainSizer->Add(btnSizer, 0, wxEXPAND, 5);

            FillList();

            ConnectClick(m_buttonDelete, EditWorkspaceConfDlg::OnDelete);
            ConnectClick(m_buttonRename, EditWorkspaceConfDlg::OnRename);
            ConnectListDClick(m_wspConfList, EditWorkspaceConfDlg::OnRename);

            this->SetSizer(mainSizer);
            CenterOnScreen();
        }
        ~EditWorkspaceConfDlg()
        {
            UnConnectClick(m_buttonDelete, EditWorkspaceConfDlg::OnDelete);
            UnConnectClick(m_buttonRename, EditWorkspaceConfDlg::OnRename);
            ConnectListDClick(m_wspConfList, EditWorkspaceConfDlg::OnRename);
        }
    protected:
        void FillList()
        {
            m_wspConfList->Clear();
            BuildMatrixPtr matrix = gsgs_Manager()->GetWorkspaceBuildMatrix();
            std::list<WorkspaceConfigurationPtr> confs;

            confs = matrix->GetConfigurations();
            std::list<WorkspaceConfigurationPtr>::iterator iter = confs.begin();

            for (; iter != confs.end(); iter++) {
                m_wspConfList->Append((*iter)->GetName());
                m_wspConfList->SetSelection(0);
            }
        }
        void DoRename(const wxString &selItem)
        {
            wxTextEntryDialog *dlg = new wxTextEntryDialog(this, _("New Configuration Name:"), _("Rename"), selItem);
            if (dlg->ShowModal() == wxID_OK) {
                wxString newName = dlg->GetValue();
                TrimString(newName);
                if (!newName.IsEmpty()) {
                    BuildMatrixPtr matrix = gsgs_Manager()->GetWorkspaceBuildMatrix();
                    WorkspaceConfigurationPtr conf = matrix->GetConfigurationByName(selItem);
                    //rename the configuration
                    conf->SetName(newName);
                    matrix->SetConfiguration(conf);
                    matrix->RemoveConfiguration(selItem);

                    //apply changes
                    gsgs_Manager()->SetWorkspaceBuildMatrix(matrix);
                    //refresh list
                    FillList();
                }
            }
            dlg->Destroy();
        }

        void OnDelete(wxCommandEvent &event)
        {
            wxUnusedVar(event);
            if (m_wspConfList->GetCount() == 0) {
                return;
            }
            wxString delMe = m_wspConfList->GetStringSelection();
            if (delMe.IsEmpty()) {
                return;
            }

            //remove the requested workspace build configuration
            BuildMatrixPtr matrix = gsgs_Manager()->GetWorkspaceBuildMatrix();
            wxString msg;
            msg << _("Remove workspace configuration '") << delMe << wxT("' ?");
            if (wxMessageBox(msg, _("光栅故事"), wxICON_QUESTION | wxYES_NO | wxCANCEL) == wxYES) {
                matrix->RemoveConfiguration(delMe);
                //apply changes
                gsgs_Manager()->SetWorkspaceBuildMatrix(matrix);

                //refresh list
                FillList();
            }
        }
        void OnRename(wxCommandEvent &)
        {
            wxString changeMe;
            if (m_wspConfList->GetCount() == 0) {
                return;
            }

            changeMe = m_wspConfList->GetStringSelection();
            if (changeMe.IsEmpty()) {
                return;
            }

            DoRename(changeMe);
        }
        void OnListBoxDClick(wxCommandEvent &event)
        {
            DoRename(event.GetString());
        }
    protected:
        wxListBox* m_wspConfList;
        Button* m_buttonRename;
        Button* m_buttonDelete;
        Button* m_buttonClose;
    };
    //---------------------------------------------------------------------------------
    enum tabgrouptype 
    { 
        TGT_group, TGT_item 
    };
    //---------------------------------------------------------------------------------
    class TabGrpTreeItemData : public wxTreeItemData
    {
    public:
        TabGrpTreeItemData() {}
        TabGrpTreeItemData(const wxString& fp, enum tabgrouptype type = TGT_item)
            : m_filepath(fp)
            , m_type(type)
        {
        }

        inline const wxString & GetFilepath() const     { return m_filepath; }
        inline tabgrouptype GetType() const             { return m_type; }

    protected:
        wxString m_filepath;
        tabgrouptype m_type; // Is this item a tabgroup or a contained tab?
    };
    //---------------------------------------------------------------------------------
    static int wxStringCmpFunc(const wxString& item1, const wxString& item2)
    {
        return item1.CmpNoCase(item2);
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // ConfigurationManagerDlg
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    class ConfigurationManagerDlg : public wxDialog
    {
    public:
        ConfigurationManagerDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Configuration Manager"),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = gsgsMinDlgSize,
                long style = gsgsDlgStyle) : 
            wxDialog(parent, id, title, pos, size, style), 
            m_dirty(false)
        {
            wxWindowUpdateLocker templ(this);
            SetBackgroundColour(gsgs_Style().GetAppBgColour());
            wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);
            this->SetSizer(boxSizer2);

            wxBoxSizer* boxSizer12 = new wxBoxSizer(wxVERTICAL);

            boxSizer2->Add(boxSizer12, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

            m_staticText18 =
                gsgs_Style().createStaticText(this, wxID_ANY, _("Select the workspace configuration to edit from the drop down list:"),
                    wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

            boxSizer12->Add(m_staticText18, 0, wxALL, WXC_FROM_DIP(5));

            wxArrayString m_choiceConfigurationsArr;
            m_choiceConfigurations =
                new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), m_choiceConfigurationsArr, 0);
            m_choiceConfigurations->SetFocus();

            boxSizer12->Add(m_choiceConfigurations, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

            m_dvListCtrl = new ThemedListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, 200)),
                wxDV_ROW_LINES | wxDV_SINGLE);

            boxSizer12->Add(m_dvListCtrl, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

            m_dvListCtrl->AppendTextColumn(_("Project"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                wxDATAVIEW_COL_RESIZABLE);
            m_dvListCtrl->AppendTextColumn(_("Configuration"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                wxDATAVIEW_COL_RESIZABLE);
            m_stdBtnSizer4 = new DialogButtonSizer();

            boxSizer2->Add(m_stdBtnSizer4, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(10));

            m_button8 = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
            m_button8->SetDefault();
            m_stdBtnSizer4->AddButton(m_button8);

            m_button6 = new Button(this, wxID_APPLY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
            m_stdBtnSizer4->AddButton(m_button6);

            m_button10 = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
            m_stdBtnSizer4->AddButton(m_button10);
            m_stdBtnSizer4->Realize();

            if (GetParent()) {
                CentreOnParent(wxBOTH);
            }
            else {
                CentreOnScreen(wxBOTH);
            }

            // Connect events
            m_choiceConfigurations->Connect(wxEVT_COMMAND_CHOICE_SELECTED,
                wxCommandEventHandler(ConfigurationManagerDlg::OnWorkspaceConfigSelected), NULL, this);
            m_button8->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationManagerDlg::OnButtonOK), NULL,
                this);
            m_button6->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationManagerDlg::OnButtonApply), NULL,
                this);
            m_button6->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ConfigurationManagerDlg::OnButtonApplyUI), NULL, this);
        
            m_dvListCtrl->Bind(wxEVT_DATAVIEW_CHOICE_BUTTON, &ConfigurationManagerDlg::OnShowConfigList, this);
            m_dvListCtrl->Bind(wxEVT_DATAVIEW_CHOICE, &ConfigurationManagerDlg::OnValueChanged, this);

            PopulateConfigurations();
            CentreOnParent();
        }

        virtual ~ConfigurationManagerDlg()
        {
            m_choiceConfigurations->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED,
                wxCommandEventHandler(ConfigurationManagerDlg::OnWorkspaceConfigSelected), NULL, this);
            m_button8->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationManagerDlg::OnButtonOK), NULL,
                this);
            m_button6->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationManagerDlg::OnButtonApply),
                NULL, this);
            m_button6->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ConfigurationManagerDlg::OnButtonApplyUI), NULL, this);

            m_dvListCtrl->DeleteAllItems([](wxUIntPtr d) {
                wxArrayString* choices = (wxArrayString*)d;
                wxDELETE(choices);
            });
            m_dvListCtrl->Unbind(wxEVT_DATAVIEW_CHOICE_BUTTON, &ConfigurationManagerDlg::OnShowConfigList, this);
            m_dvListCtrl->Unbind(wxEVT_DATAVIEW_CHOICE, &ConfigurationManagerDlg::OnValueChanged, this);
        }

        wxStaticText* GetStaticText18()         { return m_staticText18; }

        wxChoice* GetChoiceConfigurations()     { return m_choiceConfigurations; }

        ThemedListCtrl* GetDvListCtrl()         { return m_dvListCtrl; }
    protected:
        virtual void OnValueChanged(wxDataViewEvent& event)
        {
            wxString projectName = m_dvListCtrl->GetItemText(event.GetItem(), 0);
            wxString selection = event.GetString(); // the new selection

            if (selection == wxGetTranslation(clCMD_NEW)) {
                // popup the 'New Configuration' dialog
                NewConfigurationDlg dlg(this, projectName);
                if (dlg.ShowModal() == wxID_OK) {

                    // clCMD_NEW does not mark the page as dirty !
                    PopulateConfigurations();
                }
                event.Veto(); // prevent the change from taking place
            }
            else if (selection == wxGetTranslation(clCMD_EDIT)) {
                EditConfigurationDialog dlg(this, projectName);
                if (dlg.ShowModal() == wxID_OK) {
                    m_dirty = true;
                    PopulateConfigurations();
                }
                event.Veto(); // prevent the change from taking place
            }
            else {
                // just mark the page as dirty
                m_dirty = true;
            }
        }

        virtual void OnShowConfigList(wxDataViewEvent& event)
        {
            event.Skip();
            wxDataViewItem item = event.GetItem();
            wxArrayString* choices = reinterpret_cast<wxArrayString*>(m_dvListCtrl->GetItemData(item));
            if (choices)
            {
                m_dvListCtrl->ShowStringSelectionMenu(item, *choices, event.GetColumn());
            }
        }

        /**
         * @brief return list of build configurations for a project. In addition, return
         * the project conifugration name that matches the workspace configuration that is selected
         * in the m_choiceConfigurations drop down
         */
        virtual wxArrayString GetChoicesForProject(const wxString& projectName, const wxString& workspaceConfig, size_t& index)
        {
            wxArrayString choices;
            ProjectPtr project = gsgs_Manager()->GetProject(projectName);
            if (!project) { return choices; }

            // Get the workspace configuration
            wxString projectConfig = gsgs_CppWorker().GetBuildMatrix()->GetProjectSelectedConf(workspaceConfig, projectName);
            if (projectConfig.IsEmpty()) { return choices; }

            // Get all configuration of the project
            ProjectSettingsPtr settings = project->GetSettings();
            size_t counter = 0;
            if (settings) {
                ProjectSettingsCookie cookie;
                BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
                while (bldConf) {
                    choices.Add(bldConf->GetName());
                    if (projectConfig == bldConf->GetName()) { index = counter; }
                    bldConf = settings->GetNextBuildConfiguration(cookie);
                    ++counter;
                }
            }
            choices.Add(wxGetTranslation(clCMD_NEW));
            choices.Add(wxGetTranslation(clCMD_EDIT));
            return choices;
        }

        virtual void OnButtonNew(wxCommandEvent& event)
        {
            wxUnusedVar(event);
            wxTextEntryDialog* dlg = new wxTextEntryDialog(this, _("Enter New Configuration Name:"), _("New Configuration"));
            if (dlg->ShowModal() == wxID_OK) {
                wxString value = dlg->GetValue();
                TrimString(value);
                if (value.IsEmpty() == false) {
                    BuildMatrixPtr matrix = gsgs_Manager()->GetWorkspaceBuildMatrix();
                    if (!matrix) { return; }

                    WorkspaceConfigurationPtr conf(new WorkspaceConfiguration(NULL));
                    conf->SetName(value);
                    conf->SetConfigMappingList(GetCurrentSettings());
                    matrix->SetConfiguration(conf);
                    // save changes
                    gsgs_Manager()->SetWorkspaceBuildMatrix(matrix);
                }
            }
            PopulateConfigurations();
            dlg->Destroy();
        }

        virtual void OnButtonOK(wxCommandEvent& event)
        {
            OnButtonApply(event);
            EndModal(wxID_OK);
        }

        virtual void OnWorkspaceConfigSelected(wxCommandEvent& event)
        {
            if (event.GetString() == wxGetTranslation(clCMD_NEW)) {
                OnButtonNew(event);
            }
            else if (event.GetString() == wxGetTranslation(clCMD_EDIT)) {
                // popup the delete dialog for configurations
                EditWorkspaceConfDlg* dlg = new EditWorkspaceConfDlg(this);
                dlg->ShowModal();
                dlg->Destroy();

                // once done, restore dialog
                PopulateConfigurations();
            }
            else {
                if (m_dirty) {
                    if (wxMessageBox(
                        wxString::Format(
                            _("Settings for workspace configuration '%s' have changed, would you like to save them?"),
                            m_currentWorkspaceConfiguration.GetData()),
                        _("光栅故事"), wxYES | wxCANCEL | wxYES_DEFAULT | wxICON_QUESTION) != wxYES) {
                        return;
                    }
                    SaveCurrentSettings();
                    m_dirty = false;
                }
                m_currentWorkspaceConfiguration = event.GetString();
                LoadWorkspaceConfiguration(event.GetString());
            }
        }

        virtual void OnButtonApply(wxCommandEvent & event)
        {
            wxUnusedVar(event);
            SaveCurrentSettings();
        }

        virtual void OnButtonApplyUI(wxUpdateUIEvent & event)
        {
            event.Enable(m_dirty);
        }

        virtual void LoadWorkspaceConfiguration(const wxString & confName)
        {
            m_choiceConfigurations->SetStringSelection(confName);
            PopulateConfigurations();
        }

        void PopulateConfigurations()
        {
            // popuplate the configurations
            BuildMatrixPtr matrix = gsgs_Manager()->GetWorkspaceBuildMatrix();
            if (!matrix) { return; }

            wxWindowUpdateLocker locker(this);
            m_dvListCtrl->DeleteAllItems([](wxUIntPtr d) {
                wxArrayString* choices = (wxArrayString*)d;
                wxDELETE(choices);
            });

            std::list<WorkspaceConfigurationPtr> configs = matrix->GetConfigurations();
            m_choiceConfigurations->Clear();
            for (WorkspaceConfigurationPtr config : configs) {
                m_choiceConfigurations->Append(config->GetName());
            }

            // append the 'New' & 'Delete' commands
            m_choiceConfigurations->Append(wxGetTranslation(clCMD_NEW));
            m_choiceConfigurations->Append(wxGetTranslation(clCMD_EDIT));

            int sel = m_choiceConfigurations->FindString(m_currentWorkspaceConfiguration.IsEmpty()
                ? matrix->GetSelectedConfigurationName()
                : m_currentWorkspaceConfiguration);
            if (sel != wxNOT_FOUND) {
                m_choiceConfigurations->SetSelection(sel);
            }
            else if (m_choiceConfigurations->GetCount() > 2) {
                m_choiceConfigurations->SetSelection(2);
            }
            else {
                m_choiceConfigurations->Append(_("Debug"));
                m_choiceConfigurations->SetSelection(2);
            }

            // keep the current workspace configuration
            m_currentWorkspaceConfiguration = m_choiceConfigurations->GetStringSelection();

            wxArrayString projects;
            gsgs_Manager()->GetProjectList(projects);
            projects.Sort(wxStringCmpFunc);
            for (size_t i = 0; i < projects.GetCount(); ++i) {
                size_t index = wxString::npos;
                wxArrayString choices = GetChoicesForProject(projects[i], m_currentWorkspaceConfiguration, index);
                DataViewChoice c(index != wxString::npos ? choices[index] : "");
                wxVariant v;
                v << c;
                wxVector<wxVariant> cols;
                cols.push_back(projects[i]);
                cols.push_back(v);
                m_dvListCtrl->AppendItem(cols, (wxUIntPtr) new wxArrayString(choices));
            }
        }
        WorkspaceConfiguration::ConfigMappingList GetCurrentSettings()
        {
            // return the current settings as described by the dialog
            WorkspaceConfiguration::ConfigMappingList list;

            for (size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
                wxDataViewItem item = m_dvListCtrl->RowToItem(i);
                wxString projectName = m_dvListCtrl->GetItemText(item, 0);
                wxString configName = m_dvListCtrl->GetItemText(item, 1);
                if ((configName != wxGetTranslation(clCMD_NEW)) && (configName != wxGetTranslation(clCMD_EDIT))) {
                    ConfigMappingEntry entry(projectName, configName);
                    list.push_back(entry);
                }
            }
            return list;
        }

        void SaveCurrentSettings()
        {
            m_currentWorkspaceConfiguration = m_currentWorkspaceConfiguration.Trim().Trim(false);

            BuildMatrixPtr matrix = gsgs_Manager()->GetWorkspaceBuildMatrix();
            if (!matrix) { return; }

            matrix->SetSelectedConfigurationName(m_currentWorkspaceConfiguration);

            WorkspaceConfigurationPtr conf = matrix->GetConfigurationByName(m_currentWorkspaceConfiguration);
            if (!conf) {
                // create new configuration
                conf = new WorkspaceConfiguration(NULL);
                conf->SetName(m_currentWorkspaceConfiguration);
                matrix->SetConfiguration(conf);
            }

            conf->SetConfigMappingList(GetCurrentSettings());

            // save changes
            gsgs_Manager()->SetWorkspaceBuildMatrix(matrix);
            m_dirty = false;
        }
    private:
        wxString m_currentWorkspaceConfiguration;
        wxStaticText* m_staticText18;
        wxChoice* m_choiceConfigurations;
        ThemedListCtrl* m_dvListCtrl;
        DialogButtonSizer* m_stdBtnSizer4;
        Button* m_button8;
        Button* m_button6;
        Button* m_button10;
        bool m_dirty;
    };
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // SelectDropTargetDlg
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    class SelectDropTargetDlg : public wxDialog
    {
    public:
        SelectDropTargetDlg(wxWindow* parent, const wxArrayString& folders, wxWindowID id = wxID_ANY, const wxString& title = _("Select View"),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(400,400), long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP)
            : wxDialog(parent, id, title, pos, size, style)
            , m_folders(folders)
            , m_selectedView(NULL)
        {
            wxBoxSizer* boxSizer527 = new wxBoxSizer(wxVERTICAL);
            this->SetSizer(boxSizer527);
            
            m_banner = new wxBannerWindow(this, wxID_ANY, wxTOP, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
            m_banner->SetBitmap(wxNullBitmap);
            m_banner->SetText(_("Select View"), _("Choose the view to use for this folder from the list below"));
            m_banner->SetGradient(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION), wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
            m_banner->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
            
            boxSizer527->Add(m_banner, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
            
            m_dvListCtrl = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(400,200)), wxDV_NO_HEADER|wxDV_ROW_LINES|wxDV_SINGLE);
            
            boxSizer527->Add(m_dvListCtrl, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
            
            m_dvListCtrl->AppendTextColumn(_("View"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(400), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
            m_stdBtnSizer543 = new DialogButtonSizer();
            
            boxSizer527->Add(m_stdBtnSizer543, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));
            
            m_button545 = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
            m_stdBtnSizer543->AddButton(m_button545);
            
            m_button547 = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
            m_button547->SetDefault();
            m_stdBtnSizer543->AddButton(m_button547);
            m_stdBtnSizer543->Realize();

            if(GetParent()) {
                CentreOnParent(wxBOTH);
            } else {
                CentreOnScreen(wxBOTH);
            }

            // Connect events
            m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(SelectDropTargetDlg::OnSelectionActivated), NULL, this);
            m_button547->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SelectDropTargetDlg::OnOKUI), NULL, this);
            m_button547->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SelectDropTargetDlg::OnOK), NULL, this);
            Initialize();
        }
        virtual ~SelectDropTargetDlg() 
        {
            m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(SelectDropTargetDlg::OnSelectionActivated), NULL, this);
            m_button547->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SelectDropTargetDlg::OnOKUI), NULL, this);
            m_button547->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SelectDropTargetDlg::OnOK), NULL, this);
        }

        wxBannerWindow* GetBanner() { return m_banner; }
        wxDataViewListCtrl* GetDvListCtrl() { return m_dvListCtrl; }
    protected:
        virtual void OnSelectionActivated(wxDataViewEvent& event)
        {
            ActivateSelection();
            EndModal(wxID_OK);
        }
        virtual void OnOK(wxCommandEvent& event)
        {
            event.Skip();
            ActivateSelection();
            EndModal(wxID_OK);
        }
        virtual void OnOKUI(wxUpdateUIEvent& event)
        {
            event.Enable(m_dvListCtrl->HasSelection());
        }
        void Initialize()
        {
            m_views.clear();
            GetManager()->GetWorkspaceView()->GetAllPages(m_views);
            std::for_each(m_views.begin(), m_views.end(), [&](const std::pair<wxString, wxWindow*>& p) {
                wxVector<wxVariant> cols;
                cols.push_back(p.first);
                m_dvListCtrl->AppendItem(cols, (wxUIntPtr)p.second);
            });
        }
        void ActivateSelection()
        {
            wxDataViewItem item = m_dvListCtrl->GetSelection();
            if (item.IsOk()) {
                wxWindow* page = reinterpret_cast<wxWindow*>(m_dvListCtrl->GetItemData(item));
                if (page) {
                    Event event(wxEVT_DND_FOLDER_DROPPED);
                    event.SetStrings(m_folders);
                    page->GetEventHandler()->AddPendingEvent(event);
                }
            }
        }
    private:
        std::map<wxString, wxWindow*> m_views;
        wxArrayString m_folders;
        wxWindow* m_selectedView;
        wxBannerWindow* m_banner;
        wxDataViewListCtrl* m_dvListCtrl;
        DialogButtonSizer* m_stdBtnSizer543;
        Button* m_button545;
        Button* m_button547;
    };
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // DefaultWorkspacePage
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    class DefaultWorkspacePage : public wxPanel
    {
    public:
        DefaultWorkspacePage(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxBORDER_NONE)
            : wxPanel(parent, id, pos, size, style)
        {
            wxWindowUpdateLocker llt(this);
            SetBackgroundStyle(wxBG_STYLE_PAINT);
            SetBackgroundColour(gsgs_Style().GetAppBgColour());
            m_colours.setBase(gsgs_Style().GetAppBgColour());
            wxBoxSizer* boxSizer515 = new wxBoxSizer(wxVERTICAL);
            this->SetSizer(boxSizer515);

            boxSizer515->Add(0, 0, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

            mWSBitmap = new wxStaticBitmap(this, wxID_ANY, gsgs_Image().LoadBitmap(wxT("target-logo")), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
            GetSizer()->Add(mWSBitmap, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

            GetSizer()->Add(0, 0, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

            // Allow the PHP view to accepts folders
            SetDropTarget(new FileOrFolderDropTarget(this));
            Bind(wxEVT_PAINT, &DefaultWorkspacePage::OnPaint, this);
            Bind(wxEVT_DND_FOLDER_DROPPED, &DefaultWorkspacePage::OnFolderDropped, this);
            Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
            gsgs_Event().Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &DefaultWorkspacePage::OnColoursChanged, this);
        }
        virtual ~DefaultWorkspacePage()
        {
            Unbind(wxEVT_PAINT, &DefaultWorkspacePage::OnPaint, this);
            Unbind(wxEVT_DND_FOLDER_DROPPED, &DefaultWorkspacePage::OnFolderDropped, this);
            Unbind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
            gsgs_Event().Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &DefaultWorkspacePage::OnColoursChanged, this);
        }
    protected:
        wxStaticBitmap* GetStaticBitmap521() { return mWSBitmap; }

        void OnFolderDropped(Event& event)
        {
            const wxArrayString& folders = event.GetStrings();
            CallAfter(&DefaultWorkspacePage::DoDropFolders, folders);
        }
        void DoDropFolders(const wxArrayString& folders)
        {
            SelectDropTargetDlg dropTargetDlg(gsgs_Event().TopFrame(), folders);
            dropTargetDlg.ShowModal();
        }
        void OnPaint(wxPaintEvent& event)
        {
            wxAutoBufferedPaintDC dc(this);
            dc.SetBrush(m_colours.GetBgColour());
            dc.SetPen(m_colours.GetBgColour());
            dc.DrawRectangle(GetClientRect());
        }
        void OnColoursChanged(Event& event)
        {
            event.Skip();
            m_colours.setBase(gsgs_Style().GetAppBgColour());
        }
    private:
        wxStaticBitmap* mWSBitmap;
        Colour m_colours;
    };
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // FileExplorer
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    FileExplorer::FileExplorer(wxWindow* parent, const wxString& caption) : 
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(250, 300)), 
        m_caption(caption)
    {
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        SetSizer(mainSizer);
        m_view = new TreeCtrlPanel(this);
        m_view->GetToolBar()->Add(new DefaultToolBarButton(m_view->GetToolBar(), wxID_OPEN, gsgs_Image().AddInterMap(gsgs_Image().LoadBitmap("folder")), _("Open folder")));
        m_view->GetToolBar()->Realize();
        m_view->GetToolBar()->Bind(wxEVT_TOOL, &FileExplorer::OnOpenFolder, this, wxID_OPEN);
        // For the file explorer we use the standard configuration tool
        m_view->SetConfig(&gsgs_Config());
        m_view->SetViewName(_("File Explorer"));
        mainSizer->Add(m_view, 1, wxEXPAND | wxALL, 0);

        m_themeHelper = 0;// new ThemeHandlerHelper(this);
        SetDropTarget(new FileOrFolderDropTarget(this));
        Bind(wxEVT_DND_FOLDER_DROPPED, &FileExplorer::OnFolderDropped, this);
    }
    //-----------------------------------------------------------------
    FileExplorer::~FileExplorer()
    {
        wxDELETE(m_themeHelper);
        Unbind(wxEVT_DND_FOLDER_DROPPED, &FileExplorer::OnFolderDropped, this);
    }
    //-----------------------------------------------------------------
    void FileExplorer::OnFolderDropped(Event& event)
    {
        const wxArrayString& folders = event.GetStrings();
        for (size_t i = 0; i < folders.size(); ++i) 
        {
            m_view->AddFolder(folders.Item(i));
        }
        size_t index = GetManager()->GetWorkspacePaneNotebook()->GetPageIndex(_("Resource"));
        if (index != wxString::npos) 
        { 
            GetManager()->GetWorkspacePaneNotebook()->ChangeSelection(index); 
        }
    }
    //-----------------------------------------------------------------
    void FileExplorer::OpenFolder(const wxString& path) 
    { 
        m_view->AddFolder(path); 
    }
    //-----------------------------------------------------------------
    void FileExplorer::OnOpenFolder(wxCommandEvent& event)
    {
        wxString path = ::wxDirSelector(_("Select folder to open"));
        if (path.IsEmpty()) { return; }
        OpenFolder(path);
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // MyDropTarget
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    class MyDropTarget : public wxTextDropTarget
    {
    public:
        MyDropTarget(TabgroupsTab* pane)
            : m_pane(pane)
        {
        }
        virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data)
        {
            static wxRegEx re("\\{Class:Notebook,TabIndex:([0-9]+)\\}\\{(.*?)\\}", wxRE_ADVANCED);
            if (re.Matches(data)) { m_pane->FileDropped(re.GetMatch(data, 2)); }
            return true;
        }
    private:
        TabgroupsTab* m_pane;
    };
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // TabgroupsTab
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    TabgroupsTab::TabgroupsTab(wxWindow* parent, const wxString& caption)
        : wxPanel(parent, wxID_ANY)
    {
        m_node = NULL;
        wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
        SetSizer(sz);
        m_tree = new DefaultTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_HIDE_ROOT);
        m_tree->SetDropTarget(new MyDropTarget(this));

        // Add icons to the tree
        m_tree->SetBitmaps(&gsgs_Image().GetBitmaps());

        sz->Add(m_tree, 1, wxEXPAND);

        // DisplayTabgroups(); No, we can't do this here: it's too soon, a 'session' hasn't been loaded
        //SetSizer(sz);
        //sz->Layout();

        // We can't use an accelerator table here for Cut/Copy/Paste, as the events are hijacked by the frame's shortcuts
        // But delete isn't, so..
        wxAcceleratorEntry entries[1];
        entries[0].Set(wxACCEL_NORMAL, WXK_DELETE, wxID_DELETE);
        wxAcceleratorTable accel(1, entries);
        m_tree->SetAcceleratorTable(accel);

        m_tree->AddRoot(wxT("Tab Groups"), -1, -1, NULL);

        m_tree->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(TabgroupsTab::OnItemActivated), NULL, this);
        m_tree->Connect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(TabgroupsTab::OnItemRtClick), NULL, this);
        m_tree->Connect(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler(TabgroupsTab::OnBeginLabelEdit), NULL,
            this);
        m_tree->Connect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(TabgroupsTab::OnEndLabelEdit), NULL, this);
        m_tree->Connect(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(TabgroupsTab::OnBeginDrag), NULL, this);
        m_tree->Connect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(TabgroupsTab::OnEndDrag), NULL, this);
        // Context menu:
        Connect(TGM_ID_Add, TGM_ID_RemoveItem, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(TabgroupsTab::OnContextMenu), NULL, this);
        // Shortcuts:
        m_tree->Connect(wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TabgroupsTab::OnCopy), NULL, this);
        m_tree->Connect(wxID_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TabgroupsTab::OnPaste), NULL, this);
        m_tree->Connect(wxID_CUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TabgroupsTab::OnCut), NULL, this);
        m_tree->Connect(wxID_DELETE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TabgroupsTab::OnDelete), NULL,
            this);

        m_themeHelper = 0;// new ThemeHandlerHelper(this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_CLOSED, &TabgroupsTab::OnWorkspaceClosed, this);
        gsgs_Event().Bind(wxEVT_INIT_DONE, &TabgroupsTab::OnInitDone, this);
    }
    //-----------------------------------------------------------------
    TabgroupsTab::~TabgroupsTab()
    {
        wxDELETE(m_themeHelper);
        m_tree->Disconnect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(TabgroupsTab::OnItemActivated), NULL,
            this);
        m_tree->Disconnect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(TabgroupsTab::OnItemRtClick), NULL, this);
        m_tree->Disconnect(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler(TabgroupsTab::OnBeginLabelEdit), NULL,
            this);
        m_tree->Disconnect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(TabgroupsTab::OnEndLabelEdit), NULL,
            this);
        m_tree->Disconnect(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(TabgroupsTab::OnBeginDrag), NULL, this);
        m_tree->Disconnect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(TabgroupsTab::OnEndDrag), NULL, this);
        // Context menu:
        Disconnect(TGM_ID_Add, TGM_ID_RemoveItem, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(TabgroupsTab::OnContextMenu), NULL, this);
        // Shortcuts:
        m_tree->Disconnect(wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TabgroupsTab::OnCopy), NULL,
            this);
        m_tree->Disconnect(wxID_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TabgroupsTab::OnPaste), NULL,
            this);
        m_tree->Disconnect(wxID_CUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TabgroupsTab::OnCut), NULL, this);
        m_tree->Disconnect(wxID_DELETE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TabgroupsTab::OnDelete), NULL,
            this);

        delete m_node;

        gsgs_Event().Unbind(wxEVT_WORKSPACE_CLOSED, &TabgroupsTab::OnWorkspaceClosed, this);
        gsgs_Event().Unbind(wxEVT_INIT_DONE, &TabgroupsTab::OnInitDone, this);//gsgs
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::OnInitDone(wxCommandEvent& e)
    {
        e.Skip();
        DisplayTabgroups(true); // Display any Global tabgroups
    }
    //-----------------------------------------------------------------
    bool sortfunction(const spTabGrp & x, const spTabGrp & y) 
    { 
        return x.first.CmpNoCase(y.first) < 0; 
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::DisplayTabgroups(bool isGlobal /*=false*/)
    {
        vTabGrps& tabgroups = TabGroupsManager::Get()->GetTabgroups(isGlobal);
        std::sort(tabgroups.begin(), tabgroups.end(), sortfunction);
        vTabGrps::const_iterator iter = tabgroups.begin();
        for (; iter != tabgroups.end(); ++iter) {
            AddTreeItem(isGlobal, iter->first, iter->second);
        }
        m_tree->Expand(GetRootItemForTabgroup(isGlobal));
        GetSizer()->Layout();
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::AddTreeItem(bool isGlobal, const wxString& tabgroupfpath, const wxArrayString& tabfilepaths,
        const wxTreeItemId insertafter)
    {
        wxCHECK_RET(!tabgroupfpath.IsEmpty(), wxT("A tabgroup with an empty name in TabgroupsTab::AddTreeItem"));

        // Both for the tabgroup and its constituent files, we display the filename but save the filepaths in the
        // TreeItemData
        wxString tabgroupname = tabgroupfpath.AfterLast(wxFILE_SEP_PATH).BeforeLast('.');
        wxTreeItemId tbnameId;

        int folderImgID = gsgs_Image().GetMimeImageId(FileUtil::TypeFolder);
        if (insertafter.IsOk()) {
            // There's a valid id to insert after, so..
            tbnameId = m_tree->InsertItem(GetRootItemForTabgroup(isGlobal), insertafter, tabgroupname, folderImgID,
                folderImgID, new TabGrpTreeItemData(tabgroupfpath, TGT_group));
        }
        else {
            // Otherwise, append
            tbnameId = m_tree->AppendItem(GetRootItemForTabgroup(isGlobal), tabgroupname, folderImgID, folderImgID,
                new TabGrpTreeItemData(tabgroupfpath, TGT_group));
        }
        wxCHECK_RET(tbnameId.IsOk(), wxT("Failed to add the tabgroup to the tree"));
        // NB. I'm not going to try to cater for idiots who have >1 tab with the same filename!
        for (size_t n = 0; n < tabfilepaths.GetCount(); ++n) {
            wxString name = tabfilepaths.Item(n).AfterLast(wxFILE_SEP_PATH);
            int imgIdx = DoGetIconIndex(name);
            m_tree->AppendItem(tbnameId, name, imgIdx, imgIdx, new TabGrpTreeItemData(tabfilepaths.Item(n)));
        }
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::OnItemActivated(wxTreeEvent& event)
    {
        wxTreeItemId item = event.GetItem();
        if (!item.IsOk()) { return; }
        TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
        if (!data) { return; }
        wxString filepath = data->GetFilepath();
        wxFileName fn(filepath);

        if (data->GetType() == TGT_group) { // Activating a tabgroup means offer to load it
            wxArrayString previousgroups;
            gsgs_EditorConfig().GetRecentItems(previousgroups, wxT("RecentTabgroups"));

            if (!fn.FileExists()) {
                wxMessageBox(_("I'm afraid that tabgroup no longer exists :/"), _("光栅故事"), wxICON_ERROR | wxOK, this);
                m_tree->DeleteChildren(item);
                m_tree->Delete(item);
                // Remove it from the RecentTabgroups list too
                int index = previousgroups.Index(filepath);
                if (index != wxNOT_FOUND) {
                    previousgroups.RemoveAt(index);
                    gsgs_EditorConfig().SetRecentItems(previousgroups, wxT("RecentTabgroups"));
                }
                return;
            }

            std::vector<Editor*> editors;
            gsgs_Frame()->GetMainBook()->GetAllEditors(editors, Book::kGetAll_IncludeDetached |
                Book::kGetAll_RetainOrder);
            if (editors.size() > 0) {
                // If there are editors currently loaded, ask if they are to be replaced or added to
                wxString msg(_("Do you want to replace the existing editors? (Say 'No' to load the new ones alongside)"));
                int ans = wxMessageBox(msg, _("Load the tabgroup"), wxICON_QUESTION | wxYES_NO | wxCANCEL, this);
                if (ans == wxCANCEL) { return; }
                if (ans == wxYES) { gsgs_Frame()->GetMainBook()->CloseAll(true); }
            }

            WindowUpdateLocker locker(gsgs_Frame());
            TabGroupEntry session;
            if (gsgs_Session().GetSession(filepath.BeforeLast(wxT('.')), session, wxString(wxT("tabgroup")),
                tabgroupTag)) {
                gsgs_Frame()->GetMainBook()->RestoreSession(session);

                // Remove any previous instance of this group from the history, then prepend it and save
                int index = previousgroups.Index(filepath);
                if (index != wxNOT_FOUND) { previousgroups.RemoveAt(index); }
                previousgroups.Insert(filepath, 0);
                gsgs_EditorConfig().SetRecentItems(previousgroups, wxT("RecentTabgroups"));
            }
        }
        else {
            // Activating an item means load just it, as an editor
            if (!fn.FileExists()) {
                wxMessageBox(_("I'm afraid that tabgroup item no longer exists :/"), _("光栅故事"), wxICON_ERROR | wxOK,
                    this);
                DeleteTabgroupItem();
                return;
            }

            gsgs_Frame()->GetMainBook()->OpenFile(filepath);
        }
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::OnItemRtClick(wxTreeEvent& event)
    {
        wxTreeItemId item = event.GetItem();
        if (item.IsOk())
        {
            TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
            if (data)
            {
                wxMenu menu; // Tabgroup Menu
                if (data->GetType() == TGT_group) {
                    menu.Append(TGM_ID_Add, _("&Add a new item to this tabgroup"));
                    if (m_node) { menu.Append(TGM_ID_Paste, _("&Paste an item into this tabgroup")); }
                    menu.AppendSeparator();
                    menu.Append(TGM_ID_Duplicate, _("Du&plicate this tabgroup"));
                    menu.Append(TGM_ID_Delete, _("&Delete this tabgroup"));
                }
                else {
                    menu.Append(TGM_ID_CopyItem, _("&Copy this item, to be pasted to another tabgroup"));
                    menu.Append(TGM_ID_CutItem, _("Cu&t this item, to be pasted to another tabgroup"));
                    if (m_node) { menu.Append(TGM_ID_Paste, _("&Paste an item into this tabgroup")); }
                    menu.Append(TGM_ID_RemoveItem, _("&Remove this item from the tabgroup"));
                }
                PopupMenu(&menu);
            }
        }
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::OnContextMenu(wxCommandEvent& event)
    {
        switch (event.GetId()) {
        case TGM_ID_Add:
            AddTabgroupItem();
            break;
        case TGM_ID_Paste:
            PasteTabgroupItem();
            break;
        case TGM_ID_Delete:
            DeleteTabgroup();
            break;
        case TGM_ID_Duplicate:
            DuplicateTabgroup();
            break;
        case TGM_ID_CopyItem:
            CopyTabgroupItem();
            break;
        case TGM_ID_CutItem:
            DeleteTabgroupItem(true);
            break;
        case TGM_ID_RemoveItem:
            DeleteTabgroupItem(false);
            break;
        }
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::OnDelete(wxCommandEvent&) // From a keypress, not the context menu
    {
        wxTreeItemId item = m_tree->GetSelection();
        if (item.IsOk()) {
            TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
            if (!data) { return; }
            if (data->GetType() == TGT_group) {
                DeleteTabgroup();
            }
            else {
                DeleteTabgroupItem();
            }
        }
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::OnBeginLabelEdit(wxTreeEvent& event)
    {
        wxTreeItemId item = event.GetItem();
        if (item.IsOk()) {
            TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
            if (!data || data->GetType() == TGT_group) {
                // Only allow renaming of a group; renaming an item makes little sense: the name is the filename
                return;
            }
        }

        event.Veto();
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::OnEndLabelEdit(wxTreeEvent& event)
    {
        wxTreeItemId item = event.GetItem();
        if (!item.IsOk()) {
            event.Veto();
            return;
        }

        TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
        if (!data) { return; }
        wxFileName oldfilepath(data->GetFilepath());

        wxString newfilename = event.GetLabel();
        if (newfilename.IsEmpty() || newfilename == oldfilepath.GetFullName()) {
            event.Veto();
            return;
        }

        wxFileName newfilepath(oldfilepath.GetPath(), newfilename);
        if (newfilepath.FileExists()) {
            wxString msg(_("There is already a file with this name. Overwrite it?"));
            if (wxMessageBox(msg, _("Overwrite?"), wxICON_QUESTION | wxYES_NO, this) != wxYES) {
                event.Veto(); // The user doesn't want to overwrite
                return;
            }
        }
        // If we're here, the event won't be vetoed, so the tree shows the new name
        // Update the file system correspondingly
        if (wxRenameFile(oldfilepath.GetFullPath(), newfilepath.GetFullPath(), true)) {
            gsgs_Frame()->GetMainStatusBar()->SetMessage(_("Tabgroup renamed"));
            return;
        }
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::OnBeginDrag(wxTreeEvent& event)
    {
        wxTreeItemId item = event.GetItem();
        if (item.IsOk()) {
            TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
            if (data && data->GetType() == TGT_item) {
                // Only allow dragging of an item, not a group
                m_draggedItem = item;
                event.Allow(); // need explicitly to allow drag
            }
        }
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::OnEndDrag(wxTreeEvent& event)
    {
        if (!m_draggedItem.IsOk()) { return; }

        wxTreeItemId ItemSrc = m_draggedItem;
        m_draggedItem = wxTreeItemId(); // Reset the dragged-item store

        if (wxGetKeyState(WXK_ESCAPE)) {
            return; // If ESC is being pressed, take this as a hint
        }

        wxTreeItemId ItemDest = event.GetItem();
        if (!ItemDest.IsOk()) { return; }

        // Are we doing a Move or a Copy/Paste? Use the Ctrl/Alt key to decide
        wxKeyCode copykey =
    #ifdef __WXMAC__
            WXK_ALT; // Apparently the ALT key is used for this in Macs
    #else
            WXK_CONTROL;
    #endif
        if (wxGetKeyState(copykey)) {
            CopyTabgroupItem(ItemSrc);
        }
        else {
            DeleteTabgroupItem(true, ItemSrc);
        }

        PasteTabgroupItem(ItemDest);
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::AddTabgroupItem()
    {
        wxTreeItemId selection = m_tree->GetSelection();
        if (!selection.IsOk()) { return; }

        wxString defaultPath;
        IEditor* editor = PluginManager::Get()->GetActiveEditor();
        if (editor) { defaultPath = editor->GetFileName().GetPath(); }

        wxString newfilepath = ::wxFileSelector(_("Select the file you want to add"), defaultPath);
        if (newfilepath.IsEmpty()) { return; }

        if (!wxFileExists(newfilepath)) {
            if (wxMessageBox(_("That filepath doesn't seem to exist. Are you sure?"), _("光栅故事"),
                wxICON_QUESTION | wxYES_NO, this) != wxYES) {
                return;
            }
        }

        AddFile(newfilepath);
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::PasteTabgroupItem(wxTreeItemId itemtopaste /*= wxTreeItemId()*/)
    {
        if (m_node == NULL) {
            return; // The data to be pasted should have been stored here, so...
        }

        wxTreeItemId GroupId;
        if (itemtopaste.IsOk()) {
            GroupId = itemtopaste; // We got here from D'n'D, so the item was passed
        }
        else {
            GroupId = m_tree->GetSelection(); // Just a plain 'Paste'
        }
        if (!GroupId.IsOk()) { return; }

        // We don't yet know if we're pasting onto a tabgroup or a contained item, so duplicate
        wxTreeItemId ItemId(GroupId);

        TabGrpTreeItemData* groupdata = (TabGrpTreeItemData*)m_tree->GetItemData(GroupId);
        if (!groupdata) { return; }
        if (groupdata->GetType() == TGT_item) {
            GroupId = m_tree->GetItemParent(ItemId);
            wxCHECK_RET(GroupId.IsOk(), wxT("Trying to paste on something that isn't a tabgroup"));
            groupdata = (TabGrpTreeItemData*)m_tree->GetItemData(GroupId);
        }
        else {
            // The paste *was* on the group, so null ItemId: we use ItemId.IsOk() later as a flag
            ItemId = wxTreeItemId();
        }
        if (!groupdata) { return; }
        wxCHECK_RET(groupdata->GetType() == TGT_group, wxT("Trying to paste on something that isn't a tabgroup"));

        wxString filepath = groupdata->GetFilepath();
        wxXmlDocument doc(filepath);
        if (!doc.IsOk()) {
            wxMessageBox(_("Failed to load the destination tabgroup :/"), _("光栅故事"), wxICON_ERROR | wxOK, this);
            return;
        }

        bool NeedToChangeTree = true;
        wxXmlNode* node = TabGroupsManager::Get()->FindTabgroupItem(doc, filepath, m_copieditem_filepath);
        if (node) {
            if (wxMessageBox(_("There is already an item with this filepath in the tabgroup. Overwrite it?"), _("光栅故事"),
                wxICON_QUESTION | wxYES_NO, this) == wxYES) {
                wxXmlNode* parent = node->GetParent();
                if (parent) { parent->RemoveChild(node); }
                NeedToChangeTree = false;
            }
            else {
                return; // The user doesn't want to overwrite
            }
        }

        // If the paste was onto an item (not the group) and there wasn't already an item with this filepath,
        // pass info to DoAddItemToTabgroup() to find the node corresponding to the item, so that we can insert after it in
        // the doc
        wxString nextitemfilepath;
        if (ItemId.IsOk() && NeedToChangeTree) {
            TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(ItemId);
            if (data) { nextitemfilepath = data->GetFilepath(); }
        }

        if (TabGroupsManager::Get()->DoAddItemToTabgroup(doc, new wxXmlNode(*m_node), filepath, nextitemfilepath)) {
            doc.Save(filepath);

            if (NeedToChangeTree) {
                // We're not overwriting a pre-existing tab, so add the pasted tab to the tree
                wxString name = m_copieditem_filepath.AfterLast(wxFILE_SEP_PATH);
                int imgIdx = DoGetIconIndex(name);
                if (ItemId.IsOk()) {
                    // either after the selected item (if there was one)..
                    m_tree->InsertItem(GroupId, ItemId, name, imgIdx, imgIdx,
                        new TabGrpTreeItemData(m_copieditem_filepath));
                }
                else {
                    // ..or append to the group
                    m_tree->AppendItem(GroupId, name, imgIdx, imgIdx, new TabGrpTreeItemData(m_copieditem_filepath));
                }
            }

            gsgs_Frame()->GetMainStatusBar()->SetMessage(_("Tabgroup item pasted"));
        }
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::DeleteTabgroup()
    {
        wxTreeItemId item = m_tree->GetSelection();
        if (!item.IsOk()) { return; }
        TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
        if (!data) { return; }
        wxCHECK_RET(data->GetType() == TGT_group, wxT("The selection wasn't a tabgroup"));

        wxString filepath = data->GetFilepath();
        if (!wxFileName::FileExists(filepath)) {
            m_tree->DeleteChildren(item);
            m_tree->Delete(item);
            return;
        }

        wxString msg = wxString::Format(_("Delete tabgroup %s?"), m_tree->GetItemText(item).c_str());
        if (wxMessageBox(msg, _("光栅故事"), wxICON_QUESTION | wxYES_NO, this) == wxYES) {
            m_tree->Delete(item);

            // Remove it from the RecentTabgroups list too
            wxArrayString previousgroups;
            gsgs_EditorConfig().GetRecentItems(previousgroups, wxT("RecentTabgroups"));
            int index = previousgroups.Index(filepath);
            if (index != wxNOT_FOUND) {
                previousgroups.RemoveAt(index);
                gsgs_EditorConfig().SetRecentItems(previousgroups, wxT("RecentTabgroups"));
            }

            gsgsRemoveFile(filepath);

            gsgs_Frame()->GetMainStatusBar()->SetMessage(_("Tabgroup deleted"));
        }
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::DuplicateTabgroup()
    {
        wxTreeItemId selection = m_tree->GetSelection();
        if (!selection.IsOk()) { return; }
        TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(selection);
        if (!data) { return; }

        wxCHECK_RET(data->GetType() == TGT_group, wxT("The selection wasn't a tabgroup"));

        wxFileName oldfilepath(data->GetFilepath());
        wxCHECK_RET(oldfilepath.FileExists(), wxT("There was no file corresponding to the tree selection"));

        wxString newname = wxGetTextFromUser(_("What name to you want to give the duplicated tabgroup?"),
            _("Duplicate a tabgroup"), oldfilepath.GetFullName(), this);
        if (newname.IsEmpty() || newname == oldfilepath.GetFullName()) { return; }

        if (!newname.EndsWith(".tabgroup")) {
            newname << ".tabgroup"; // Otherwise things break
        }
        wxFileName newfilepath(oldfilepath.GetPath(), newname);
        if (newfilepath.FileExists()) {
            wxMessageBox(_("Sorry, there is already a tabgroup with this name"), _("光栅故事"), wxICON_INFORMATION | wxOK,
                this);
            return;
        }

        if (!wxCopyFile(oldfilepath.GetFullPath(), newfilepath.GetFullPath())) {
            wxMessageBox(_("Sorry, duplication of the tabgroup failed :/"), _("光栅故事"), wxICON_ERROR | wxOK, this);
            return;
        }
        wxTreeItemId parent = m_tree->GetItemParent(selection);
        wxCHECK_RET(parent.IsOk(), "A parentless tabgroup");
        TabGrpTreeItemData* parentdata = (TabGrpTreeItemData*)m_tree->GetItemData(parent);
        wxCHECK_RET(parentdata, "An unnamed tabgroup parent");
        bool isGlobal = parentdata->GetFilepath().StartsWith(_("Global"));

        // Do the rest in a separate method, which is also called by Frame::OnFileSaveTabGroup
        if (AddNewTabgroupToTree(isGlobal, newfilepath.GetFullPath(), selection)) {
            gsgs_Frame()->GetMainStatusBar()->SetMessage(_("Tabgroup duplicated"));
        }
    }
    //-----------------------------------------------------------------
    bool TabgroupsTab::AddNewTabgroupToTree(bool isGlobal, const wxString& newfilepath,
        wxTreeItemId selection /*=wxTreeItemId()*/)
    {
        // Tell TabGroupsManager to load the new group. Probably a good idea anyway, but we'll need it below
        TabGroupsManager::Get()->LoadTabgroupData(isGlobal, newfilepath);
        // Now add the newly-loaded info to the tree
        wxArrayString items;
        if (TabGroupsManager::Get()->FindTabgroup(isGlobal, newfilepath, items)) {
            AddTreeItem(isGlobal, newfilepath, items, selection);
            return true;
        }

        return false;
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::CopyTabgroupItem(wxTreeItemId itemtocopy /*= wxTreeItemId()*/)
    {
        wxTreeItemId item;
        if (itemtocopy.IsOk()) {
            item = itemtocopy; // We got here from D'n'D, so the item was passed
        }
        else {
            item = m_tree->GetSelection(); // Just a plain 'Copy'
        }
        if (!item.IsOk()) { return; }

        TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
        if (!data) { return; }
        if (data->GetType() != TGT_item) {
            return; // I can't think of any reason to copy a whole group
        }

        wxTreeItemId GroupId = m_tree->GetItemParent(item);
        TabGrpTreeItemData* groupdata = (TabGrpTreeItemData*)m_tree->GetItemData(GroupId);
        if (!groupdata) { return; }

        wxString filepath = groupdata->GetFilepath();
        wxXmlDocument doc(filepath);
        wxCHECK_RET(doc.IsOk(), wxT("Failed to load the tabgroup to be copied :/"));

        wxString itemfilepath = data->GetFilepath();
        wxXmlNode* node = TabGroupsManager::Get()->FindTabgroupItem(doc, filepath, itemfilepath);
        wxCHECK_RET(node != NULL, wxT("Failed to find the tab-item to be copied :/"));

        // All is well, so store the data in m_node and m_copieditem_filepath
        delete m_node;
        m_node = new wxXmlNode(*node);
        m_copieditem_filepath = itemfilepath;
        gsgs_Frame()->GetMainStatusBar()->SetMessage(_("Tabgroup item copied"));
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::DeleteTabgroupItem(bool DoCut /*=false*/, wxTreeItemId itemtocut /*= wxTreeItemId()*/)
    {
        wxTreeItemId item;
        if (itemtocut.IsOk()) {
            item = itemtocut; // We got here from D'n'D, so the item was passed
        }
        else {
            item = m_tree->GetSelection(); // Just a plain 'Cut' or 'Delete'
        }
        if (!item.IsOk()) { return; }
        TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(item);
        if (!data) { return; }
        wxCHECK_RET(data->GetType() == TGT_item, wxT("The selection was a tabgroup, not a tabgroup item"));
        wxString itemfilepath = data->GetFilepath(); // Need to do this here, before the item is deleted!
        wxTreeItemId parent = m_tree->GetItemParent(item);
        TabGrpTreeItemData* tabgrpdata = (TabGrpTreeItemData*)m_tree->GetItemData(parent);
        if (!tabgrpdata) { return; }
        wxString filepath = tabgrpdata->GetFilepath();
        if (!wxFileName::FileExists(filepath)) { return; }

        if (!DoCut) { // If we're deleting, check the user really meant it
            wxString msg = wxString::Format(_("Remove item %s from %s?"), m_tree->GetItemText(item).c_str(),
                m_tree->GetItemText(parent).c_str());
            if (wxMessageBox(msg, _("光栅故事"), wxICON_QUESTION | wxYES_NO, this) != wxYES) { return; }
        }
        m_tree->Delete(item);

        wxXmlDocument doc(filepath);
        if (doc.IsOk()) {
            wxXmlNode* TabInfoNode = TabGroupsManager::Get()->DoDeleteTabgroupItem(doc, filepath, itemfilepath);
            if (TabInfoNode) {
                if (DoCut) {
                    // If we're cutting, store the deleted node ready for paste
                    m_node = new wxXmlNode(*TabInfoNode);
                    m_copieditem_filepath = itemfilepath;
                    gsgs_Frame()->GetMainStatusBar()->SetMessage(_("Tabgroup item Cut"));
                }
                else {
                    gsgs_Frame()->GetMainStatusBar()->SetMessage(_("Tabgroup item deleted"));
                }
                return;
            }
        }

        // Either !doc.IsOk() or !TabInfoNode
        wxMessageBox(_("Failed to amend the tabgroup :/"), _("光栅故事"), wxICON_ERROR | wxOK, this);
    }
    //-----------------------------------------------------------------
    int TabgroupsTab::DoGetIconIndex(const wxString& filename)
    {
        int imgIdx = gsgs_Image().GetMimeImageId(filename);
        if (imgIdx == wxNOT_FOUND) { imgIdx = gsgs_Image().GetMimeImageId(FileUtil::TypeText); }
        return imgIdx;
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::OnWorkspaceClosed(wxCommandEvent& e)
    {
        e.Skip();
        m_tree->DeleteChildren(GetRootItemForTabgroup(false));
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::FileDropped(const wxString& filename)
    {
        // An editor was DnD to the TabGroup pane view
        AddFile(filename);
    }
    //-----------------------------------------------------------------
    void TabgroupsTab::AddFile(const wxString& filename)
    {
        // Create a suitable node, then piggyback on PasteTabgroupItem() to do the real work
        // Reuse m_copieditem_filepath and m_node for this, so cache them first
        wxString oldcopieditem_filepath = m_copieditem_filepath;
        wxXmlNode* oldnode = m_node;

        m_copieditem_filepath = filename;
        m_node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("TabInfo"));
        wxXmlNode* fp = new wxXmlNode(m_node, wxXML_ELEMENT_NODE, wxT("wxString"));
        fp->XmlAddProperty(wxT("Value"), filename);
        fp->XmlAddProperty(wxT("Name"), wxT("FileName"));
        wxXmlNode* fvl = new wxXmlNode(m_node, wxXML_ELEMENT_NODE, wxT("int"));
        fvl->XmlAddProperty(wxT("Name"), wxT("FirstVisibleLine"));
        wxXmlNode* cl = new wxXmlNode(m_node, wxXML_ELEMENT_NODE, wxT("int"));
        cl->XmlAddProperty(wxT("Name"), wxT("CurrentLine"));
        wxXmlNode* bm = new wxXmlNode(m_node, wxXML_ELEMENT_NODE, wxT("wxArrayString"));
        bm->XmlAddProperty(wxT("Name"), wxT("Bookmarks"));

        PasteTabgroupItem();

        // Restore the old values
        m_copieditem_filepath = oldcopieditem_filepath;
        wxDELETE(m_node);
        m_node = oldnode;
    }
    //-----------------------------------------------------------------
    wxTreeItemId TabgroupsTab::GetRootItemForTabgroup(bool global)
    {
        wxString label = global ? "Global tabgroups" : "Workspace tabgroups";

        // First see if the appropriate tabgroup base-item has already been created
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_tree->GetFirstChild(m_tree->GetRootItem(), cookie);
        while (child.IsOk()) {
            TabGrpTreeItemData* data = (TabGrpTreeItemData*)m_tree->GetItemData(child);
            if (data && data->GetFilepath() == label) { return child; }
            child = m_tree->GetNextSibling(child);
        }
        // Nope, so create it
        int folderImgID = gsgs_Image().GetMimeImageId(FileUtil::TypeFolder);
        return m_tree->AppendItem(m_tree->GetRootItem(), label, folderImgID, folderImgID,
            new TabGrpTreeItemData(label, TGT_group));
    }
    //-----------------------------------------------------------------
    BEGIN_EVENT_TABLE(OpenWindowsTab, wxPanel)
    EVT_MENU(XRCID("wxID_CLOSE_SELECTED"), OpenWindowsTab::OnCloseSelectedFiles)
    EVT_MENU(XRCID("wxID_SAVE_SELECTED"), OpenWindowsTab::OnSaveSelectedFiles)
    END_EVENT_TABLE()
    //-----------------------------------------------------------------
    struct TabSorter {
        bool operator()(const Tab& t1, const Tab& t2)
        {
            wxString file1, file2;
            if (t1.isFile) {
                file1 = t1.filename.GetFullName().Lower();
            }
            else {
                file1 = t1.text;
            }

            if (t2.isFile) {
                file2 = t2.filename.GetFullName().Lower();
            }
            else {
                file2 = t2.text;
            }
            return file1.CmpNoCase(file2) < 0;
        }
    };
    //-----------------------------------------------------------------
    struct TabClientData : public wxClientData {
        Tab tab;
        bool isModified;
        TabClientData() {}
        TabClientData(const Tab& t)
            : tab(t)
            , isModified(false)
        {
        }
        virtual ~TabClientData() {}
        bool IsFile() const { return tab.isFile; }
    };
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // OpenWindowsTab
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    OpenWindowsTab::OpenWindowsTab(wxWindow* parent, const wxString& caption, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, wxT("OpenWindowsTab"))
        , m_caption(caption)
        , m_mgr(PluginManager::Get())
        , m_initDone(false)
        , m_workspaceClosing(false)
        , m_workspaceOpened(false)
    {
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(mainSizer);

        m_dvListCtrl = new ThemedListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
                                            wxDV_NO_HEADER | wxDV_ROW_LINES | wxDV_MULTIPLE);

        mainSizer->Add(m_dvListCtrl, 1, wxALL | wxEXPAND, WXC_FROM_DIP(0));

        m_dvListCtrl->AppendIconTextColumn(_("Path"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                           wxDATAVIEW_COL_RESIZABLE);

        // Connect events
        m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED,
                              wxDataViewEventHandler(OpenWindowsTab::OnTabSelected), NULL, this);
        m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
                              wxDataViewEventHandler(OpenWindowsTab::OnTabActivated), NULL, this);
        m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU,
                              wxDataViewEventHandler(OpenWindowsTab::OnMenu), NULL, this);

        // Disable the built in sorting, this control will take care of it
        m_dvListCtrl->SetSortFunction(nullptr);
        m_dvListCtrl->SetBitmaps(&gsgs_Image().GetBitmaps());
        m_toolbar = new ToolBar(this);
        m_toolbar->AddTool(wxID_SORT_ASCENDING, _("Sort"), gsgs_Image().LoadBitmap("sort"), "",
            wxITEM_CHECK);
        m_toolbar->Realize();
        GetSizer()->Insert(0, m_toolbar, 0, wxEXPAND);

        m_toolbar->Bind(wxEVT_TOOL, &OpenWindowsTab::OnSortItems, this, wxID_SORT_ASCENDING);
        m_toolbar->Bind(wxEVT_UPDATE_UI, &OpenWindowsTab::OnSortItemsUpdateUI, this, wxID_SORT_ASCENDING);

        bool tmp;
        gsgs_Config().Read(kConfigTabsPaneSortAlphabetically, tmp, true);
        m_toolbar->ToggleTool(XRCID("TabsSortTool"), tmp);

        //gsgs_Event().Connect(wxEVT_INIT_DONE, wxCommandEventHandler(OpenWindowsTab::OnInitDone), NULL, this);
        // Connect the events only after the initialization has completed
        gsgs_Event().Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(OpenWindowsTab::OnActiveEditorChanged), NULL, this);
        gsgs_Event().Connect(wxEVT_CMD_PAGE_CHANGED, wxCommandEventHandler(OpenWindowsTab::OnActiveEditorChanged), NULL, this);
        gsgs_Event().Connect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(OpenWindowsTab::OnAllEditorsClosed), NULL, this);

        gsgs_Event().Bind(wxEVT_EDITOR_MODIFIED, &OpenWindowsTab::OnEditorModified, this);
        gsgs_Event().Bind(wxEVT_FILE_SAVED, &OpenWindowsTab::OnEditorSaved, this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_CLOSED, &OpenWindowsTab::OnWorkspaceClosed, this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_CLOSING, &OpenWindowsTab::OnWorkspaceClosing, this);
        gsgs_Event().Connect(wxEVT_INIT_DONE, wxIdleEventHandler(OpenWindowsTab::OnInitDone), NULL, this);
        //Bind(wxEVT_IDLE, &OpenWindowsTab::OnIdle, this);
    }
    //-----------------------------------------------------------------
    OpenWindowsTab::~OpenWindowsTab()
    {
        m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED,
                                 wxDataViewEventHandler(OpenWindowsTab::OnTabSelected), NULL, this);
        m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
                                 wxDataViewEventHandler(OpenWindowsTab::OnTabActivated), NULL, this);
        m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU,
                                 wxDataViewEventHandler(OpenWindowsTab::OnMenu), NULL, this);
                                 
        m_toolbar->Unbind(wxEVT_TOOL, &OpenWindowsTab::OnSortItems, this, wxID_SORT_ASCENDING);
        m_toolbar->Unbind(wxEVT_UPDATE_UI, &OpenWindowsTab::OnSortItemsUpdateUI, this, wxID_SORT_ASCENDING);

        // clear list now, or wxGTK seems to crash on exit
        Clear();
        //gsgs_Event().Disconnect(wxEVT_INIT_DONE, wxCommandEventHandler(OpenWindowsTab::OnInitDone), NULL, this);
        // Connect the events only after the initialization has completed
        gsgs_Event().Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(OpenWindowsTab::OnActiveEditorChanged), NULL, this);
        gsgs_Event().Disconnect(wxEVT_CMD_PAGE_CHANGED, wxCommandEventHandler(OpenWindowsTab::OnActiveEditorChanged), NULL, this);
        gsgs_Event().Disconnect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(OpenWindowsTab::OnAllEditorsClosed), NULL, this);

        gsgs_Event().Unbind(wxEVT_EDITOR_MODIFIED, &OpenWindowsTab::OnEditorModified, this);
        gsgs_Event().Unbind(wxEVT_FILE_SAVED, &OpenWindowsTab::OnEditorSaved, this);
        gsgs_Event().Unbind(wxEVT_WORKSPACE_CLOSED, &OpenWindowsTab::OnWorkspaceClosed, this);
        gsgs_Event().Unbind(wxEVT_WORKSPACE_CLOSING, &OpenWindowsTab::OnWorkspaceClosing, this);

        gsgs_Event().Disconnect(wxEVT_INIT_DONE, wxIdleEventHandler(OpenWindowsTab::OnInitDone), NULL, this);
        gsgs_Event().Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED,
            wxCommandEventHandler(OpenWindowsTab::OnActiveEditorChanged), NULL, this);
        gsgs_Event().Disconnect(wxEVT_CMD_PAGE_CHANGED,
            wxCommandEventHandler(OpenWindowsTab::OnActivePageChanged), NULL, this);
        gsgs_Event().Disconnect(wxEVT_ALL_EDITORS_CLOSED,
            wxCommandEventHandler(OpenWindowsTab::OnAllEditorsClosed), NULL, this);
        gsgs_Event().Unbind(wxEVT_EDITOR_MODIFIED, &OpenWindowsTab::OnEditorModified, this);
        gsgs_Event().Unbind(wxEVT_FILE_SAVED, &OpenWindowsTab::OnEditorSaved, this);
        gsgs_Event().Unbind(wxEVT_WORKSPACE_CLOSED, &OpenWindowsTab::OnWorkspaceClosed, this);
        gsgs_Event().Unbind(wxEVT_WORKSPACE_CLOSING, &OpenWindowsTab::OnWorkspaceClosing, this);
        //Unbind(wxEVT_IDLE, &OpenWindowsTab::OnIdle, this);
    }
    //-----------------------------------------------------------------
    #define CHECK_WORKSPACE_CLOSING() if(m_workspaceClosing) return
    //-----------------------------------------------------------------
    void OpenWindowsTab::OnActivePageChanged(wxCommandEvent& e)
    {
        e.Skip();
        CHECK_WORKSPACE_CLOSING();

        PopulateView();
        DoSelectItem((wxWindow*)e.GetClientData());
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::OnActiveEditorChanged(wxCommandEvent& e)
    {
        e.Skip();
        CHECK_WORKSPACE_CLOSING();

        PopulateView();
        if (m_mgr->GetActiveEditor()) { DoSelectItem(m_mgr->GetActiveEditor()); }
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::OnAllEditorsClosed(wxCommandEvent& e)
    {
        e.Skip();
        CHECK_WORKSPACE_CLOSING();

        Clear();
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::OnCloseSelectedFiles(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        wxDataViewItemArray items;
        m_dvListCtrl->GetSelections(items);

        if (items.IsEmpty()) { return; }

        // Close the page, it will be removed from the UI later
        std::vector<wxFileName> files;
        wxArrayString pages;
        for (size_t i = 0; i < items.GetCount(); ++i) {
            TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(items.Item(i)));
            if (data && data->IsFile()) {
                files.push_back(data->tab.filename);
            }
            else if (data) {
                pages.push_back(data->tab.text);
            }
        }

        // Close the files
        for (size_t i = 0; i < files.size(); ++i) {
            m_mgr->ClosePage(files[i]);
        }

        // Close non files editors
        for (size_t i = 0; i < pages.size(); ++i) {
            m_mgr->ClosePage(pages.Item(i));
        }
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::OnSaveSelectedFiles(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        wxDataViewItemArray items;
        m_dvListCtrl->GetSelections(items);

        if (items.IsEmpty()) return;

        for (size_t i = 0; i < items.GetCount(); ++i) {
            DoSaveItem(items.Item(i));
        }
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::DoSaveItem(wxDataViewItem item)
    {
        TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
        if (data) {
            if (data->IsFile()) {
                IEditor* editor = m_mgr->FindEditor(data->tab.filename.GetFullPath());
                if (editor) { editor->Save(); }
            }
        }
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::DoCloseItem(wxDataViewItem item)
    {
        TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
        if (data) {
            if (data->IsFile()) {
                m_mgr->ClosePage(data->tab.filename);
            }
            else {
                m_mgr->ClosePage(data->tab.text);
            }
        }
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::DoSelectItem(IEditor* editor)
    {
        CHECK_PTR_RET(editor);
        for (size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
            wxDataViewItem item = m_dvListCtrl->RowToItem(i);
            TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
            if (data->IsFile()) {
                if (editor->GetFileName() == data->tab.filename) {
                    m_dvListCtrl->Select(item);
                    break;
                }
            }
        }
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::OnSortItems(wxCommandEvent& event)
    {
        if (event.IsChecked()) {
            SortAlphabetically();
        }
        else {
            SortByEditorOrder();
        }

        IEditor* editor = m_mgr->GetActiveEditor();
        DoSelectItem(editor);

        gsgs_Config().Write(kConfigTabsPaneSortAlphabetically, event.IsChecked());
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::OnSortItemsUpdateUI(wxUpdateUIEvent& event) 
    { 
        event.Enable(m_dvListCtrl->GetItemCount()); 
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::SortAlphabetically()
    {
        wxWindowUpdateLocker locker(m_dvListCtrl);
        TabVec_t tabs;
        m_mgr->GetAllTabs(tabs);

        // Sort editors
        std::sort(tabs.begin(), tabs.end(), TabSorter());
        Clear();

        TabVec_t::iterator iter = tabs.begin();
        for (; iter != tabs.end(); ++iter) {
            AppendEditor(*iter);
        }
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::SortByEditorOrder()
    {
        wxWindowUpdateLocker locker(m_dvListCtrl);
        TabVec_t tabs;
        m_mgr->GetAllTabs(tabs);

        // Sort editors
        Clear();

        TabVec_t::iterator iter = tabs.begin();
        for (; iter != tabs.end(); ++iter) {
            AppendEditor(*iter);
        }
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::OnTabActivated(wxDataViewEvent& event)
    {
        event.Skip();
        CHECK_WORKSPACE_CLOSING();

        wxDataViewItem item = event.GetItem();
        CHECK_ITEM_RET(item);

        TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
        if (data->IsFile()) {
            m_mgr->OpenFile(data->tab.filename.GetFullPath());
        }
        else {
            m_mgr->SelectPage(m_mgr->FindPage(data->tab.text));
        }
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::OnTabSelected(wxDataViewEvent& event)
    {
        event.Skip();
        CHECK_WORKSPACE_CLOSING();

        if (::wxGetKeyState(WXK_CONTROL)) {
            // multiple selection in progress
            return;
        }

        /*wxDataViewItem item = event.GetItem();
        CHECK_ITEM_RET(item);
        TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
        if (data->IsFile()) {
            m_mgr->OpenFile(data->tab.filename.GetFullPath());
        }
        else {
            wxWindow* page = m_mgr->FindPage(data->tab.text);
            if (page) { m_mgr->SelectPage(page); }
        }*/
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::AppendEditor(const Tab& tab)
    {
        TabClientData* data = new TabClientData(tab);
        wxVariant value = PrepareValue(tab);

        // the row index is the same as the row count (before we add the new entry)
        int itemIndex = m_dvListCtrl->GetItemCount();

        wxVector<wxVariant> cols;
        cols.push_back(value);
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr)data);
        if (tab.isFile) { m_editors.insert(std::make_pair(tab.filename.GetFullPath(), m_dvListCtrl->RowToItem(itemIndex))); }
    }
    //-----------------------------------------------------------------
    wxString OpenWindowsTab::GetEditorPath(wxDataViewItem item)
    {
        TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
        return data->tab.text;
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::Clear()
    {
        for (size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
            wxDataViewItem item = m_dvListCtrl->RowToItem(i);
            TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
            wxDELETE(data);
        }
        m_dvListCtrl->DeleteAllItems();
        m_editors.clear();
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::PopulateView()
    {
        if (m_toolbar->FindById(wxID_SORT_ASCENDING)->IsChecked()) {
            SortAlphabetically();
        }
        else {
            SortByEditorOrder();
        }
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::OnMenu(wxDataViewEvent& event)
    {
        wxDataViewItemArray items;
        m_dvListCtrl->GetSelections(items);
        if (items.IsEmpty()) return;

        wxMenu* menu;
        if (items.GetCount() == 1) {
            // single item
            menu = wxXmlResource::Get()->LoadMenu(wxT("editor_tab_right_click"));
        }
        else {
            menu = wxXmlResource::Get()->LoadMenu(wxT("tabs_multi_sels_menu"));
        }
        m_dvListCtrl->PopupMenu(menu);
        wxDELETE(menu);
    }
    //-----------------------------------------------------------------
    bool OpenWindowsTab::IsEditor(wxDataViewItem item) const
    {
        TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
        return data->IsFile();
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::DoSelectItem(wxWindow* win)
    {
        CHECK_PTR_RET(win);
        for (size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
            wxDataViewItem item = m_dvListCtrl->RowToItem(i);
            TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
            if (data->tab.window == win) {
                m_dvListCtrl->Select(item);
                break;
            }
        }
    }
    //-----------------------------------------------------------------
    /*void OpenWindowsTab::OnInitDone(wxCommandEvent& event)
    {
        event.Skip();
        m_initDone = true;

        // Connect the events only after the initialization has completed
        gsgs_Event().Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(OpenWindowsTab::OnActiveEditorChanged), NULL, this);
        gsgs_Event().Connect(wxEVT_CMD_PAGE_CHANGED, wxCommandEventHandler(OpenWindowsTab::OnActiveEditorChanged), NULL, this);
        gsgs_Event().Connect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(OpenWindowsTab::OnAllEditorsClosed), NULL, this);

        gsgs_Event().Bind(wxEVT_EDITOR_MODIFIED, &OpenWindowsTab::OnEditorModified, this);
        gsgs_Event().Bind(wxEVT_FILE_SAVED, &OpenWindowsTab::OnEditorSaved, this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_CLOSED, &OpenWindowsTab::OnWorkspaceClosed, this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_CLOSING, &OpenWindowsTab::OnWorkspaceClosing, this);
    }*/
    //-----------------------------------------------------------------
    void OpenWindowsTab::OnInitDone(wxIdleEvent& event)
    {
        event.Skip();
        //static bool once = false;
        if (/*!once && m_initDone &&*/ m_mgr->GetPageCount() && m_dvListCtrl->GetItemCount() == 0) {
            // populate the control
            //once = true;
            PopulateView();
            m_initDone = true;
        }
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::OnEditorModified(Event& event)
    {
        event.Skip();
        if (m_initDone)
        {
            IEditor* editor = m_mgr->FindEditor(event.GetFileName());
            if (editor) 
            { 
                DoMarkModify(event.GetFileName(), editor->IsModified()); 
            }
        }
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::OnEditorSaved(Event& event)
    {
        event.Skip();
        if (m_initDone)
            DoMarkModify(event.GetFileName(), false);
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::DoMarkModify(const wxString& filename, bool b)
    {
        std::map<wxString, wxDataViewItem>::iterator iter = m_editors.find(filename);
        if (iter == m_editors.end()) return;
        wxDataViewItem item = iter->second;

        wxBitmap bmp;
        TabClientData* cd = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
        const Tab& tab = cd->tab;
        wxVariant value = PrepareValue(tab);
        m_dvListCtrl->SetValue(value, m_dvListCtrl->ItemToRow(item), 0);
        m_dvListCtrl->Refresh();
    }
    //-----------------------------------------------------------------
    wxVariant OpenWindowsTab::PrepareValue(const Tab& tab)
    {
        wxString title;
        wxStyledTextCtrl* editor(NULL);
        if (tab.isFile) {
            title = tab.filename.GetFullName();
            IEditor* i_editor = GetManager()->FindEditor(tab.filename.GetFullPath());
            if (i_editor) { editor = i_editor->GetCtrl(); }
        }
        else {
            title = tab.text;
        }

        FileUtil::FileType ft = FileUtil::GetType(title, FileUtil::TypeText);
        int imgId = gsgs_Image().GetMimeImageId(ft);
        if (editor && editor->GetModify()) { title.Prepend("*"); }

        wxVariant value = ::MakeBitmapIndexText(title, imgId);
        return value;
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::OnWorkspaceClosed(wxCommandEvent& event)
    {
        event.Skip();
        m_workspaceClosing = false;
    }
    //-----------------------------------------------------------------
    void OpenWindowsTab::OnWorkspaceClosing(wxCommandEvent& event)
    {
        event.Skip();
        Clear();
        m_workspaceClosing = true;
    }
    //-----------------------------------------------------------------
    static bool SortPinnedProjects(RowEntry* a, RowEntry* b)
    {
        wxString a_label = a->GetLabel(0);
        wxString b_label = b->GetLabel(0);
        return a_label.CmpNoCase(b_label) < 0;
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // WorkspaceTab
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    WorkspaceTab::WorkspaceTab(wxWindow* parent, const wxString& caption, wxWindowID id,
        const wxPoint& pos, const wxSize& size, long style)
        :wxPanel(parent, id, pos, size, style, wxT("WorkspaceTab"))
        , m_caption(caption)
        , m_isLinkedToEditor(true)
        , m_dlg(NULL)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetBackgroundColour(gsgs_Style().GetAppBgColour());

        SetDropTarget(new FileOrFolderDropTarget(this));

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(mainSizer);

        m_simpleBook = new wxSimplebook(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxBORDER_NONE);
        m_simpleBook->SetEffect(wxSHOW_EFFECT_NONE);

        mainSizer->Add(m_simpleBook, 1, wxEXPAND, WXC_FROM_DIP(5));

        m_view = new WorkspaceView(m_simpleBook);
        m_panelCxx = new wxPanel(m_simpleBook, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_simpleBook, wxSize(200, 200)), wxTAB_TRAVERSAL);
        m_panelCxx->SetBackgroundStyle(wxBG_STYLE_PAINT);

        m_view->AddPage(new DefaultWorkspacePage(m_simpleBook), _("Default"));
        m_view->AddPage(m_panelCxx, _("C++ Workspace"));
        m_view->SelectPage(_("Default"));
        m_view->SetDefaultPage(_("Default"));

        int index = m_simpleBook->FindPage(m_panelCxx);
        if (index != wxNOT_FOUND) {
            // set the C++ workspace view name to fit its workspace type
            //// Construct the tree
            m_simpleBook->SetPageText(index, gsgs_CppWorker().GetWorkspaceType());
        }

        wxBoxSizer* boxSizer505 = new wxBoxSizer(wxVERTICAL);
        m_panelCxx->SetSizer(boxSizer505);

        m_toolbar580 = new ToolBar(m_panelCxx, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panelCxx, wxSize(-1, -1)), wxTB_NODIVIDER | wxTB_FLAT);
        m_toolbar580->SetToolBitmapSize(wxSize(16, 16));

        boxSizer505->Add(m_toolbar580, 0, wxEXPAND, WXC_FROM_DIP(5));

        m_configPanel = gsgs_Frame()->GetConfigPanel();
        m_splitter = new SplitterWindow(m_panelCxx, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panelCxx, wxSize(-1, -1)), wxSP_LIVE_UPDATE);
        m_splitter->SetSashGravity(0);
        m_splitter->SetMinimumPaneSize(10);

        boxSizer505->Add(m_splitter, 1, wxEXPAND, WXC_FROM_DIP(5));

        //m_splitterPagePinnedProjects = new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitter, wxSize(-1, -1)), wxTAB_TRAVERSAL);

        //wxBoxSizer* boxSizer621 = new wxBoxSizer(wxVERTICAL);
        //m_splitterPagePinnedProjects->SetSizer(boxSizer621);

        m_dvListCtrlPinnedProjects = new ThemedListCtrl(m_splitter, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxDV_NO_HEADER | wxDV_ROW_LINES | wxDV_SINGLE | wxBORDER_NONE);

        //boxSizer621->Add(m_dvListCtrlPinnedProjects, 1, wxEXPAND, WXC_FROM_DIP(5));

        m_dvListCtrlPinnedProjects->AppendTextColumn(_("Project"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);

        //m_splitterPageTreeView = new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitter, wxSize(-1, -1)), wxTAB_TRAVERSAL);
        //m_splitter->SplitHorizontally(m_splitterPagePinnedProjects, m_splitterPageTreeView, 100);


        //wxBoxSizer* boxSizer619 = new wxBoxSizer(wxVERTICAL);
        //m_splitterPageTreeView->SetSizer(boxSizer619);

        m_fileView = new FileViewTree(m_splitter, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTR_MULTIPLE | wxTR_NO_LINES | wxTR_HAS_BUTTONS);
        m_splitter->SplitHorizontally(m_dvListCtrlPinnedProjects, m_fileView, 100);
        //boxSizer619->Add(m_fileView, 1, wxEXPAND, WXC_FROM_DIP(2));

        long link = gsgs_EditorConfig().GetInteger(wxT("LinkWorkspaceViewToEditor"), 1);
        m_isLinkedToEditor = link ? true : false;

        ImageManager * bmps = &gsgs_Image();/*
        m_toolbar580->AddTool(XRCID("ID_TOOL_LINK_EDITOR"), _("Link Editor"), bmps->LoadBitmap("link_editor"), "",
            wxITEM_CHECK);
        m_toolbar580->AddTool(XRCID("ID_TOOL_COLLAPSE_ALL"), _("Collapse All"), bmps->LoadBitmap("fold"));*/
        m_toolbar580->AddTool(XRCID("ID_TOOL_GOTO_ACTIVE_PROJECT"), _("Goto Active Project"), bmps->LoadBitmap("home"));
        m_toolbar580->AddTool(XRCID("ID_TOOL_ACTIVE_PROJECT_SETTINGS"),
            _("Open selected project settings. If there is no project selected, open the parent project "
                "of the selected item in the tree"), bmps->LoadBitmap("cog"));
        /*m_toolbar580->AddSpacer();
        m_toolbar580->AddTool(XRCID("execute_no_debug"), _("Run Active Project"), bmps->LoadBitmap("execute"),
            _("Run Active Project"));
        m_toolbar580->AddTool(XRCID("build_active_project"), _("Build Active Project"), bmps->LoadBitmap("build"),
            _("Build Active Project"), wxITEM_DROPDOWN);
        m_toolbar580->AddTool(XRCID("stop_active_project_build"), _("Stop Current Build"), bmps->LoadBitmap("stop"),
            _("Stop Current Build"));*/
        m_toolbar580->Realize();

        // Connect events
        wxTheApp->Connect(XRCID("show_in_workspace"), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WorkspaceTab::OnShowFile), NULL, this);
        wxTheApp->Connect(XRCID("show_in_workspace"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(WorkspaceTab::OnShowFileUI),
            NULL, this);
        gsgs_Event().Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(WorkspaceTab::OnWorkspaceLoaded), NULL,
            this);
        gsgs_Event().Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(WorkspaceTab::OnWorkspaceClosed), NULL,
            this);
        gsgs_Event().Connect(wxEVT_PROJ_ADDED, gsgsEventHandler(WorkspaceTab::OnProjectAdded), NULL, this);
        gsgs_Event().Connect(wxEVT_PROJ_REMOVED, gsgsEventHandler(WorkspaceTab::OnProjectRemoved), NULL,
            this);
        gsgs_Event().Connect(wxEVT_ACTIVE_EDITOR_CHANGED,
            wxCommandEventHandler(WorkspaceTab::OnActiveEditorChanged), NULL, this);
        gsgs_Event().Connect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(WorkspaceTab::OnEditorClosing), NULL,
            this);
        gsgs_Event().Connect(wxEVT_WORKSPACE_CONFIG_CHANGED,
            wxCommandEventHandler(WorkspaceTab::OnWorkspaceConfig), NULL, this);
        gsgs_Event().Connect(wxEVT_ACTIVE_PROJECT_CHANGED,
            clProjectSettingsEventHandler(WorkspaceTab::OnActiveProjectChanged), NULL, this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_BUILD_CONFIG_CHANGED, &WorkspaceTab::OnConfigChanged, this);

        wxTheApp->Connect(XRCID("configuration_manager"), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WorkspaceTab::OnConfigurationManager), NULL, this);
        wxTheApp->Connect(XRCID("configuration_manager"), wxEVT_UPDATE_UI,
            wxUpdateUIEventHandler(WorkspaceTab::OnProjectSettingsUI), NULL, this);

        m_dvListCtrlPinnedProjects->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEventHandler(WorkspaceTab::OnPinnedCxxProjectContextMenu), NULL, this);
        m_dvListCtrlPinnedProjects->Connect(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler(WorkspaceTab::OnPinnedCxxProjectSelected), NULL, this);
        m_toolbar580->Bind(wxEVT_TOOL, &WorkspaceTab::OnCollapseAll, this, XRCID("ID_TOOL_COLLAPSE_ALL"));
        m_toolbar580->Bind(wxEVT_UPDATE_UI, &WorkspaceTab::OnCollapseAllUI, this, XRCID("ID_TOOL_COLLAPSE_ALL"));
        m_toolbar580->Bind(wxEVT_TOOL, &WorkspaceTab::OnLinkEditor, this, XRCID("ID_TOOL_LINK_EDITOR"));
        m_toolbar580->Bind(wxEVT_UPDATE_UI, &WorkspaceTab::OnLinkEditorUI, this, XRCID("ID_TOOL_LINK_EDITOR"));
        m_toolbar580->Bind(wxEVT_TOOL, &WorkspaceTab::OnProjectSettings, this, XRCID("ID_TOOL_ACTIVE_PROJECT_SETTINGS"));
        m_toolbar580->Bind(wxEVT_UPDATE_UI, &WorkspaceTab::OnProjectSettingsUI, this, XRCID("ID_TOOL_ACTIVE_PROJECT_SETTINGS"));
        m_toolbar580->Bind(wxEVT_TOOL, &WorkspaceTab::OnGoHome, this, XRCID("ID_TOOL_GOTO_ACTIVE_PROJECT"));
        m_toolbar580->Bind(wxEVT_UPDATE_UI, &WorkspaceTab::OnGoHomeUI, this, XRCID("ID_TOOL_GOTO_ACTIVE_PROJECT"));
        Bind(wxEVT_DND_FOLDER_DROPPED, &WorkspaceTab::OnFolderDropped, this);

        m_themeHelper = NULL; // new ThemeHandlerHelper(this);

        m_panelCxx->Bind(wxEVT_PAINT, [&](wxPaintEvent& e) {
            wxAutoBufferedPaintDC dc(m_panelCxx);
            dc.SetBrush(m_bgColour);
            dc.SetPen(m_bgColour);
            dc.DrawRectangle(m_panelCxx->GetClientRect());
        });
        
        m_bgColour = gsgs_Style().GetAppBgColour();
        gsgs_Event().Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, [&](Event& event) {
            event.Skip();
            m_bgColour = gsgs_Style().GetAppBgColour();
            Refresh();
            //m_panelCxx->Refresh();
        });
        m_bitmaps.push_back(gsgs_Image().LoadBitmap("project"));
        m_dvListCtrlPinnedProjects->SetBitmaps(&m_bitmaps);
    }
    //-----------------------------------------------------------------
    WorkspaceTab::~WorkspaceTab()
    {
        wxDELETE(m_themeHelper);

        wxTheApp->Disconnect(XRCID("show_in_workspace"), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WorkspaceTab::OnShowFile), NULL, this);
        wxTheApp->Disconnect(XRCID("show_in_workspace"), wxEVT_UPDATE_UI,
            wxUpdateUIEventHandler(WorkspaceTab::OnShowFileUI), NULL, this);

        gsgs_Event().Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(WorkspaceTab::OnWorkspaceLoaded),
            NULL, this);
        gsgs_Event().Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(WorkspaceTab::OnWorkspaceClosed),
            NULL, this);
        gsgs_Event().Disconnect(wxEVT_PROJ_ADDED, gsgsEventHandler(WorkspaceTab::OnProjectAdded), NULL, this);
        gsgs_Event().Disconnect(wxEVT_PROJ_REMOVED, gsgsEventHandler(WorkspaceTab::OnProjectRemoved), NULL,
            this);
        gsgs_Event().Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED,
            wxCommandEventHandler(WorkspaceTab::OnActiveEditorChanged), NULL, this);
        gsgs_Event().Disconnect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(WorkspaceTab::OnEditorClosing), NULL,
            this);
        gsgs_Event().Disconnect(wxEVT_WORKSPACE_CONFIG_CHANGED,
            wxCommandEventHandler(WorkspaceTab::OnWorkspaceConfig), NULL, this);
        gsgs_Event().Disconnect(wxEVT_ACTIVE_PROJECT_CHANGED,
            clProjectSettingsEventHandler(WorkspaceTab::OnActiveProjectChanged), NULL, this);
        gsgs_Event().Unbind(wxEVT_WORKSPACE_BUILD_CONFIG_CHANGED, &WorkspaceTab::OnConfigChanged, this);

        wxTheApp->Disconnect(XRCID("configuration_manager"), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WorkspaceTab::OnConfigurationManager), NULL, this);
        wxTheApp->Disconnect(XRCID("configuration_manager"), wxEVT_UPDATE_UI,
            wxUpdateUIEventHandler(WorkspaceTab::OnProjectSettingsUI), NULL, this);

        m_dvListCtrlPinnedProjects->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEventHandler(WorkspaceTab::OnPinnedCxxProjectContextMenu), NULL, this);
        m_dvListCtrlPinnedProjects->Disconnect(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler(WorkspaceTab::OnPinnedCxxProjectSelected), NULL, this);
        m_toolbar580->Unbind(wxEVT_TOOL, &WorkspaceTab::OnCollapseAll, this, XRCID("ID_TOOL_COLLAPSE_ALL"));
        m_toolbar580->Unbind(wxEVT_UPDATE_UI, &WorkspaceTab::OnCollapseAllUI, this, XRCID("ID_TOOL_COLLAPSE_ALL"));
        m_toolbar580->Unbind(wxEVT_TOOL, &WorkspaceTab::OnLinkEditor, this, XRCID("ID_TOOL_LINK_EDITOR"));
        m_toolbar580->Unbind(wxEVT_UPDATE_UI, &WorkspaceTab::OnLinkEditorUI, this, XRCID("ID_TOOL_LINK_EDITOR"));
        m_toolbar580->Unbind(wxEVT_TOOL, &WorkspaceTab::OnProjectSettings, this, XRCID("ID_TOOL_ACTIVE_PROJECT_SETTINGS"));
        m_toolbar580->Unbind(wxEVT_UPDATE_UI, &WorkspaceTab::OnProjectSettingsUI, this, XRCID("ID_TOOL_ACTIVE_PROJECT_SETTINGS"));
        m_toolbar580->Unbind(wxEVT_TOOL, &WorkspaceTab::OnGoHome, this, XRCID("ID_TOOL_GOTO_ACTIVE_PROJECT"));
        m_toolbar580->Unbind(wxEVT_UPDATE_UI, &WorkspaceTab::OnGoHomeUI, this, XRCID("ID_TOOL_GOTO_ACTIVE_PROJECT"));
        Unbind(wxEVT_DND_FOLDER_DROPPED, &WorkspaceTab::OnFolderDropped, this);
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::FreezeThaw(bool freeze /*=true*/)
    {
        // If the selected file is linked to which editor is displayed, the selection changes rapidly when a workspace is
        // loaded
        // Prevent this by temporarily unlinking, then relink after
        static bool frozen = false;
        static bool was_linked;

        wxCHECK_RET(!(freeze && frozen), wxT("Trying to re-freeze a frozen workspace tab"));
        wxCHECK_RET(!(!freeze && !frozen), wxT("Trying to thaw a warm workspace tab"));
        frozen = freeze;

        if (freeze) {
            was_linked = m_isLinkedToEditor;
            m_isLinkedToEditor = false;
        }
        else {
            m_isLinkedToEditor = was_linked;
            // I expected we'd need to call OnActiveEditorChanged() here, but it doesn't seem necessary (atm)
        }
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnLinkEditor(wxCommandEvent& e)
    {
        m_isLinkedToEditor = e.IsChecked();
        gsgs_EditorConfig().SetInteger(wxT("LinkWorkspaceViewToEditor"), m_isLinkedToEditor ? 1 : 0);
        if (m_isLinkedToEditor) { OnActiveEditorChanged(e); }
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnCollapseAll(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        if (m_fileView->GetRootItem().IsOk())
        {
            m_fileView->CollapseAll();

            // count will probably be 0 below, so ensure we can at least see the root item
            m_fileView->EnsureVisible(m_fileView->GetRootItem());
            m_fileView->SelectItem(m_fileView->GetRootItem());
            // Expand the workspace
            m_fileView->Expand(m_fileView->GetRootItem());

            wxArrayTreeItemIds arr;
            if (m_fileView->GetSelections(arr) == 1) {
                wxTreeItemId sel = arr.Item(0);
                if (sel.IsOk()) { m_fileView->EnsureVisible(sel); }
            }
        }
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnCollapseAllUI(wxUpdateUIEvent& e) 
    { 
        e.Enable(gsgs_Workspace().IsOpen()); 
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnGoHome(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        wxString activeProject = gsgs_Manager()->GetActiveProjectName();
        if (activeProject.IsEmpty()) return;
        m_fileView->ExpandToPath(activeProject, wxFileName());

        wxArrayTreeItemIds arr;
        if (m_fileView->GetSelections(arr) == 1) {
            wxTreeItemId sel = arr.Item(0);
            if (sel.IsOk() && m_fileView->ItemHasChildren(sel)) 
                m_fileView->Expand(sel);
        }
        gsgs_Manager()->ShowWorkspacePane(m_caption);
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnGoHomeUI(wxUpdateUIEvent& e) 
    { 
        e.Enable(!gsgs_Manager()->GetActiveProjectName().IsEmpty()); 
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnProjectSettings(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        ProjectPtr proj = m_fileView->GetSelectedProject();
        OpenProjectSettings(proj ? proj->GetName() : "");
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnProjectSettingsUI(wxUpdateUIEvent& e)
    {
        e.Enable(!gsgs_Manager()->GetActiveProjectName().IsEmpty());
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnShowFile(wxCommandEvent& e)
    {
        Editor* editor = gsgs_Frame()->GetMainBook()->GetActiveEditor();
        if (editor && !editor->GetProject().IsEmpty()) {
            m_fileView->ExpandToPath(editor->GetProject(), editor->GetFileName());
            gsgs_Manager()->ShowWorkspacePane(m_caption);
        }
        e.Skip();
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnShowFileUI(wxUpdateUIEvent& e)
    {
        Editor* editor = gsgs_Frame()->GetMainBook()->GetActiveEditor();
        e.Enable(editor && !editor->GetProject().IsEmpty());
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnActiveEditorChanged(wxCommandEvent& e)
    {
        e.Skip();
        if (m_isLinkedToEditor) {
            Book* mainbook = gsgs_Frame()->GetMainBook();
            Editor* editor = mainbook->GetActiveEditor();
            if (editor && !editor->GetProject().IsEmpty()) {
                m_fileView->ExpandToPath(editor->GetProject(), editor->GetFileName());
            }

            Notebook* book = gsgs_Frame()->GetWorkspacePane()->GetNotebook();
            if (book)
            {
                size_t index = book->GetPageIndex("wxCrafter");
                if (index == (size_t)book->GetSelection())
                {
                    book->SetSelection(0); // The most likely to be wanted
                }
            }
        }
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnWorkspaceLoaded(wxCommandEvent& e)
    {
        e.Skip();
        if (gsgs_Workspace().IsOpen()) {
            m_configPanel->Enable(true);
            DoWorkspaceConfig();
            DoUpdateChoiceWithProjects();

            // Tree construction
            m_fileView->BuildTree();
            CallAfter(&WorkspaceTab::DoGoHome);
            SendCmdEvent(wxEVT_FILE_VIEW_INIT_DONE);

            // Load the C++ pinned projects list
            LoadCxxPinnedProjects();
        }
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnEditorClosing(wxCommandEvent& e) { e.Skip(); }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnWorkspaceClosed(wxCommandEvent& e)
    {
        e.Skip();
        m_configPanel->Clear();
        m_configPanel->Enable(false);
        m_fileView->DeleteAllItems();

        // Clear the pinned projects view
        m_dvListCtrlPinnedProjects->DeleteAllItems();
        SaveCxxPinnedProjects();
        m_cxxPinnedProjects.clear();

        SendCmdEvent(wxEVT_FILE_VIEW_INIT_DONE);
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnProjectAdded(Event& e)
    {
        e.Skip();
        const wxString& projName = e.GetString();
        m_fileView->BuildTree();
        if (!projName.IsEmpty()) { m_fileView->ExpandToPath(projName, wxFileName()); }
        DoUpdateChoiceWithProjects();
        SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnProjectRemoved(Event& e)
    {
        e.Skip();
        m_fileView->BuildTree();
        CallAfter(&WorkspaceTab::DoGoHome);
        DoUpdateChoiceWithProjects();
        SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
    }
    //-----------------------------------------------------------------
    struct wxStringSorter {
        bool operator()(WorkspaceConfigurationPtr one, WorkspaceConfigurationPtr two) const
        {
            return one->GetName().Lower().CmpNoCase(two->GetName().Lower()) < 0;
        }
    };
    //-----------------------------------------------------------------
    void WorkspaceTab::DoWorkspaceConfig()
    {
        // Update the workspace configuration
        BuildMatrixPtr matrix = gsgs_CppWorker().GetBuildMatrix();
        std::list<WorkspaceConfigurationPtr> confs = matrix->GetConfigurations();

        confs.sort(wxStringSorter());
        wxArrayString configurations;
        std::for_each(confs.begin(), confs.end(),
            [&](WorkspaceConfigurationPtr conf) { configurations.push_back(conf->GetName()); });

        m_configPanel->SetConfigurations(configurations);
        wxString activeConfig = configurations.IsEmpty() ? "" : matrix->GetSelectedConfigurationName();
        m_configPanel->SetActiveConfiguration(activeConfig);

        gsgs_Frame()->SelectBestEnvSet();
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnWorkspaceConfig(wxCommandEvent& e)
    {
        e.Skip();
        DoWorkspaceConfig();
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnConfigurationManager(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        ConfigurationManagerDlg dlg(gsgs_Event().TopFrame());
        dlg.ShowModal();

        BuildMatrixPtr matrix = gsgs_Manager()->GetWorkspaceBuildMatrix();
        m_configPanel->SetActiveConfiguration(matrix->GetSelectedConfigurationName());
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnLinkEditorUI(wxUpdateUIEvent& event)
    {
        event.Enable(gsgs_Workspace().IsOpen());
        event.Check(m_isLinkedToEditor);
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::DoUpdateChoiceWithProjects()
    {
        if (gsgs_CppWorker().IsOpen()) {
            m_configPanel->SetActiveProject(gsgs_Manager()->GetActiveProjectName());
        }
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnWorkspaceOpenUI(wxUpdateUIEvent& event) 
    { 
        event.Enable(gsgs_Workspace().IsOpen()); 
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OpenProjectSettings(const wxString& project)
    {
        if (m_dlg) {
            m_dlg->Raise();
            return;
        }

        wxString projectName = project.IsEmpty() ? gsgs_Manager()->GetActiveProjectName() : project;
        wxString title(projectName);
        title << _(" Project Settings");

        // Allow plugins to process this event first
        Event openEvent(wxEVT_CMD_OPEN_PROJ_SETTINGS);
        openEvent.SetString(project);
        if (gsgs_Event().ProcessEvent(openEvent)) { return; }

        // open the project properties dialog
        BuildMatrixPtr matrix = gsgs_Manager()->GetWorkspaceBuildMatrix();

    #ifdef __WXMAC__
        // On OSX we use a modal version of the project settings
        // since otherwise we get some weird focus issues when the project
        // settings dialog popups helper dialogs
        ProjectSettingsDlg dlg(gsgs_Frame(), this,
            matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(), projectName),
            projectName, title);
        dlg.ShowModal();

    #else
        // Find the project configuration name that matches the workspace selected configuration
        m_dlg = new ProjectSettingsDlg(gsgs_Frame(), this,
            matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(), projectName),
            projectName, title);
        m_dlg->Show();

    #endif

        // Mark this project as modified
        ProjectPtr proj = gsgs_Manager()->GetProject(projectName);
        if (proj) { proj->SetModified(true); }
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::ProjectSettingsDlgClosed() { m_dlg = NULL; }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnActiveProjectChanged(ProjectSettingsEvent& e)
    {
        e.Skip();

        // Update the choice control
        m_configPanel->SetActiveProject(e.GetProjectName());
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::DoGoHome()
    {
        wxString activeProject = gsgs_Manager()->GetActiveProjectName();
        if (activeProject.IsEmpty()) return;
        m_fileView->ExpandToPath(activeProject, wxFileName());

        wxArrayTreeItemIds arr;
        if (m_fileView->GetSelections(arr) == 1) {
            wxTreeItemId sel = arr.Item(0);
            if (sel.IsOk() && m_fileView->ItemHasChildren(sel)) m_fileView->Expand(sel);
        }
        // gsgs_Manager()->ShowWorkspacePane(m_caption);
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::DoConfigChanged(const wxString& newConfigName)
    {
        wxBusyCursor bc;
        BuildMatrixPtr matrix = gsgs_Manager()->GetWorkspaceBuildMatrix();
        matrix->SetSelectedConfigurationName(newConfigName);
        gsgs_Manager()->SetWorkspaceBuildMatrix(matrix);

        // Set the focus to the active editor if any
        Editor* editor = gsgs_Frame()->GetMainBook()->GetActiveEditor();
        if (editor) { editor->SetActive(); }

        gsgs_Manager()->UpdateParserPaths(true);
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnFolderDropped(Event& event)
    {
        // pass it on to the tree view
        m_fileView->CallAfter(&FileViewTree::FolderDropped, event.GetStrings());
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnConfigChanged(Event& e)
    {
        e.Skip();
        wxString selection = e.GetString();
        if (selection == OPEN_CONFIG_MGR_STR) {
            e.Skip(false);
            wxCommandEvent e(wxEVT_MENU, XRCID("configuration_manager"));
            e.SetEventObject(this);
            ProcessEvent(e);
            return;
        }
        DoConfigChanged(selection);
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnPaint(wxPaintEvent& event)
    {
        wxAutoBufferedPaintDC dc(this);
        dc.SetBrush(m_bgColour);
        dc.SetPen(m_bgColour);
        dc.DrawRectangle(GetClientRect());
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::LoadCxxPinnedProjects()
    {
        m_cxxPinnedProjects.clear();
        gsgs_CppWorker().GetLocalWorkspace()->GetPinnedProjects(m_cxxPinnedProjects);

        // We got the pinned projects loaded, update the view
        if (m_cxxPinnedProjects.empty()) {
            // hide the to view
            if (m_splitter->IsSplit()) { m_splitter->Unsplit(m_dvListCtrlPinnedProjects); }
        }
        else {
            // ensure the view is visible
            if (!m_splitter->IsSplit()) {
                m_splitter->SplitHorizontally(m_dvListCtrlPinnedProjects, m_fileView, 150);
                m_splitter->CallAfter(&wxSplitterWindow::UpdateSize);
            }

            m_dvListCtrlPinnedProjects->DeleteAllItems();
            m_dvListCtrlPinnedProjects->SetSortFunction(nullptr);
            for (const wxString& project : m_cxxPinnedProjects) {
                wxVector<wxVariant> V;
                V.push_back(::MakeBitmapIndexText(project, 0));
                m_dvListCtrlPinnedProjects->AppendItem(V);
            }
            // apply the sort
            m_dvListCtrlPinnedProjects->SetSortFunction(SortPinnedProjects);
        }
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::SaveCxxPinnedProjects()
    {
        gsgs_CppWorker().GetLocalWorkspace()->SetPinnedProjects(m_cxxPinnedProjects);
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnPinnedCxxProjectContextMenu(wxDataViewEvent& event)
    {
        wxDataViewItem item = event.GetItem();
        if (item.IsOk())
        {
            wxString project = m_dvListCtrlPinnedProjects->GetItemText(item);

            m_fileView->ExpandToPath(project, wxFileName()); // Select the project
            ShowPinnedProjectMenu(project);
        }
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::OnPinnedCxxProjectSelected(wxDataViewEvent& event)
    {
        // we MUST sync the views, otherwise, the context menu wont work
        // since it reliese on the current selection in the tree
        const wxDataViewItem& item = event.GetItem();
        if (item.IsOk())
        {
            wxString project = m_dvListCtrlPinnedProjects->GetItemText(item);
            m_fileView->ExpandToPath(project, wxFileName()); // Select the project
        }
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::AddPinnedProject(const wxString& project)
    {
        if (m_cxxPinnedProjects.Index(project) == wxNOT_FOUND)
        {
            m_cxxPinnedProjects.Add(project);

            // Store the pinned projehcts
            SaveCxxPinnedProjects();

            // Refresh the view
            LoadCxxPinnedProjects();
        }
    }
    //-----------------------------------------------------------------
    void WorkspaceTab::ShowPinnedProjectMenu(const wxString& project)
    {
        wxMenu menu;
        menu.Append(XRCID("unpin_project"), _("Unpin Project"));
        menu.AppendSeparator();
        // Build the rest of the menu
        m_fileView->CreateProjectContextMenu(menu, project, false);
        menu.Bind(wxEVT_MENU, [&](wxCommandEvent& menuEvent) 
        {
            if (menuEvent.GetId() == XRCID("unpin_project")) 
            {
                wxDataViewItem item = m_dvListCtrlPinnedProjects->GetSelection();
                if (item.IsOk()) 
                {
                    m_dvListCtrlPinnedProjects->DeleteItem(m_dvListCtrlPinnedProjects->ItemToRow(item));
                    int where = m_cxxPinnedProjects.Index(project);
                    if (where != wxNOT_FOUND) 
                    {
                        m_cxxPinnedProjects.RemoveAt(where);
                        SaveCxxPinnedProjects();
                        CallAfter(&WorkspaceTab::LoadCxxPinnedProjects);
                    }
                }
            }
            else 
            {
                // Pass all the context menu events to the tree tree view
                m_fileView->GetEventHandler()->ProcessEvent(menuEvent);
            }
        });
        m_dvListCtrlPinnedProjects->PopupMenu(&menu);
    }
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    // WorkspacePane
    //-------------------------------------------------------------------------------------
    //-----------------------------------------------------------------
    #define IS_DETACHED(name) (detachedPanes.Index(name) != wxNOT_FOUND) ? true : false
    //-------------------------------------------------------------------------------------
    WorkspacePane::WorkspacePane(wxWindow * parent, const wxString & caption, wxAuiManager * mag)
        : wxPanel(parent)
        , m_caption(caption)
        , m_mgr(mag)
    {
        wxWindowUpdateLocker templ(this);
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer * mainSizer = new wxBoxSizer(wxVERTICAL);
        SetSizer(mainSizer);
        long style = kNotebook_UnderlineActiveTab;
#if USE_AUI_NOTEBOOK
        style |= wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_WINDOWLIST_BUTTON | wxAUI_NB_TAB_SPLIT;
#else
        // add notebook for tabs
#ifdef __WXOSX__
        style |= (kNotebook_Default | kNotebook_AllowDnD | kNotebook_LeftTabs);
#else
        style |= (kNotebook_Default | kNotebook_AllowDnD);
#endif
        wxDirection tempdir = gsgs_EditorConfig().GetOptions()->GetWorkspaceTabsDirection();
        if (tempdir == wxBOTTOM)
            style |= kNotebook_BottomTabs;
        else if (tempdir == wxRIGHT)
            style |= kNotebook_RightTabs;
        else if (tempdir == wxLEFT)
            style |= kNotebook_LeftTabs;

        if (gsgs_EditorConfig().GetOptions()->IsTabColourDark())
        {
            style &= ~kNotebook_LightTabs;
            style |= kNotebook_DarkTabs;
        }

        if (gsgs_EditorConfig().GetOptions()->IsMouseScrollSwitchTabs())
        {
            style |= kNotebook_MouseScrollSwitchTabs;
        }

#endif
        m_book = new Notebook(this, wxID_ANY, wxDefaultSize, wxDefaultPosition, wxDefaultSize, style);

        /*#if !USE_AUI_NOTEBOOK
            m_book->SetArt(GetNotebookRenderer());
        #endif*/

        // Calculate the widest tab (the one with the 'Workspace' label)
        //int xx, yy;
        //wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        //wxWindow::GetTextExtent(_("Workspace"), &xx, &yy, NULL, NULL, &fnt);

        mainSizer->Add(m_book, 1, wxEXPAND | wxALL, 0);

        // Add the parsing progress controls
        //m_staticText = gsgs_Style().createStaticText(this, wxID_ANY, _("Analyze..."));
        //mainSizer->Add(m_staticText, 0, wxEXPAND | wxALL, 2);
        //m_staticText->Hide();
        //gsgs_StatusBar()->SetState(_("Analyze workspace..."));
        m_parsingProgress = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(-1, 15), wxGA_HORIZONTAL | wxGA_SMOOTH);
        mainSizer->Add(m_parsingProgress, 0, wxEXPAND | wxALL, 1);
        m_parsingProgress->Hide();

        // create tabs (possibly detached)
        DetachedPanesInfo dpi;
        gsgs_EditorConfig().ReadObject(wxT("DetachedPanesList"), &dpi);

        wxArrayString detachedPanes;
        detachedPanes = dpi.GetPanes();

        IManager* mgr = PluginManager::Get();

        wxString name = _("Workspace");
        if (IS_DETACHED(name))
        {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNullBitmap, wxSize(200, 200));
            m_workspaceTab = new WorkspaceTab(cp, name);
            cp->SetChildNoReparent(m_workspaceTab);
        }
        else
        {
            m_workspaceTab = new WorkspaceTab(m_book, name);
            m_book->AddPage(m_workspaceTab, name, true);
        }
        m_tabs.insert(std::make_pair(name, Tab(name, m_workspaceTab)));
        mgr->AddWorkspaceTab(name);

        // Add the explorer tab
        name = _("Resource");
        if (IS_DETACHED(name))
        {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNullBitmap, wxSize(200, 200));
            m_explorer = new FileExplorer(cp, name);
            cp->SetChildNoReparent(m_explorer);
        }
        else
        {
            m_explorer = new FileExplorer(m_book, name);
            m_book->AddPage(m_explorer, name);
        }
        m_tabs.insert(std::make_pair(name, Tab(name, m_explorer)));
        mgr->AddWorkspaceTab(name);

        // Add the "File Explorer" view to the list of files managed by the workspace-view
        m_workspaceTab->GetView()->AddPage(m_explorer, _("File Explorer"), false);

        // Add the Open Windows Panel (Tabs)
#ifndef __WXOSX__
        name = _("Team");
        if (IS_DETACHED(name))
        {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNullBitmap, wxSize(200, 200));
            m_openWindowsPane = new OpenWindowsTab(cp, name);
            cp->SetChildNoReparent(m_openWindowsPane);
        }
        else
        {
            m_openWindowsPane = new OpenWindowsTab(m_book, name);
            m_book->AddPage(m_openWindowsPane, name);
        }
        m_tabs.insert(std::make_pair(name, Tab(name, m_openWindowsPane)));
        mgr->AddWorkspaceTab(name);
#endif

        // Add the Tabgroups tab
        /*name = _("SFTP");
        if(IS_DETACHED(name))
        {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNullBitmap, wxSize(200, 200));
            m_TabgroupsPane = new TabgroupsTab(cp, name);
            cp->SetChildNoReparent(m_TabgroupsPane);
        }
        else
        {
            m_TabgroupsPane = new TabgroupsTab(m_book, name);
            m_book->AddPage(m_TabgroupsPane, name);
        }
        m_tabs.insert(std::make_pair(name, Tab(name, m_TabgroupsPane)));
        mgr->AddWorkspaceTab(name);
        */
        // Add the Service tab
        name = _("Service");
        if (IS_DETACHED(name))
        {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNullBitmap, wxSize(200, 200));
            m_ServicePane = new TabgroupsTab(cp, name);
            cp->SetChildNoReparent(m_ServicePane);
        }
        else
        {
            m_ServicePane = new TabgroupsTab(m_book, name);
            m_book->AddPage(m_ServicePane, name);
        }
        m_tabs.insert(std::make_pair(name, Tab(name, m_ServicePane)));
        mgr->AddWorkspaceTab(name);

        if (m_book->GetPageCount() > 0)
        {
            m_book->SetSelection((size_t)0);
        }
        m_book->Bind(wxEVT_BOOK_FILELIST_BUTTON_CLICKED, &WorkspacePane::OnWorkspaceBookFileListMenu, this);
        //gsgs_Event().Bind(wxEVT_INIT_DONE, &WorkspacePane::OnInitDone, this);
        //gsgs_Event().Bind(wxEVT_EDITOR_CONFIG_CHANGED, &WorkspacePane::OnSettingsChanged, this);
        gsgs_Event().Bind(wxEVT_SHOW_WORKSPACE_TAB, &WorkspacePane::OnToggleWorkspaceTab, this);
    }
    //-----------------------------------------------------------------
    WorkspacePane::~WorkspacePane()
    {
        m_book->Unbind(wxEVT_BOOK_FILELIST_BUTTON_CLICKED, &WorkspacePane::OnWorkspaceBookFileListMenu, this);
        //gsgs_Event().Unbind(wxEVT_INIT_DONE, &WorkspacePane::OnInitDone, this);
        //gsgs_Event().Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &WorkspacePane::OnSettingsChanged, this);
        gsgs_Event().Unbind(wxEVT_SHOW_WORKSPACE_TAB, &WorkspacePane::OnToggleWorkspaceTab, this);
    }
    //-----------------------------------------------------------------
    void WorkspacePane::ClearProgress()
    {
        m_parsingProgress->SetValue(0);
        m_parsingProgress->Hide();

        gsgs_StatusBar()->SetState(_(""));
        //m_staticText->Hide();
        Layout();
    }
    //-----------------------------------------------------------------
    void WorkspacePane::UpdateProgress(int val)
    {
        if(m_parsingProgress->IsShown() == false) 
        {
            m_parsingProgress->Show();
            //m_staticText->Show();
            gsgs_StatusBar()->SetState(_("Analyze workspace..."));
            Layout();
        }

        gsgs_StatusBar()->SetState(wxString::Format(_("Analyze workspace: %d%% completed"), val));
        m_parsingProgress->SetValue(val);
        m_parsingProgress->Update();
    }
    //-----------------------------------------------------------------
    typedef struct {
        wxString text;
        wxWindow* win;
        wxBitmap bmp;
    } tagTabInfo;
    //-----------------------------------------------------------------
    void WorkspacePane::ApplySavedTabOrder() const
    {
        wxArrayString tabs;
        int index = -1;
        if(!gsgs_Config().GetWorkspaceTabOrder(tabs, index)) return;

        // There are (currently) 4 'standard' panes and a variable number of plugin ones
        // NB Since we're only dealing with panes currently in the notebook, this shouldn't
        // be broken by floating panes or non-loaded plugins
        std::vector<tagTabInfo> vTempstore;
        for(size_t t = 0; t < tabs.GetCount(); ++t) 
        {
            wxString title = tabs.Item(t);
            if (title.size())
            {
                for (size_t n = 0; n < m_book->GetPageCount(); ++n)
                {
                    if (title == m_book->GetPageText(n))
                    {
                        tagTabInfo Tab;
                        Tab.text = title;
                        Tab.win = m_book->GetPage(n);
                        Tab.bmp = m_book->GetPageBitmap(n);

                        vTempstore.push_back(Tab);
                        m_book->RemovePage(n);
                        break;
                    }
                }
            }
            // If we reach here without finding title, presumably that tab is no longer available and will just be ignored
        }

        // All the matched tabs are now stored in the vector. Any left in m_book are presumably new additions
        // Now prepend the ordered tabs, so that any additions will effectively be appended
        for(size_t n = 0; n < vTempstore.size(); ++n) 
        {
            m_book->InsertPage(n, vTempstore.at(n).win, vTempstore.at(n).text, false, vTempstore.at(n).bmp);
        }

        // wxPrintf("After load");for (size_t n=0; n < m_book->GetPageCount(); ++n)  CL_DEBUG1(wxString::Format("Tab %i:
        // %zs",(int)n,m_book->GetPageText(n)));

        // Restore any saved last selection
        // NB: this doesn't actually work atm: the selection is set correctly, but presumably something else changes is
        // later
        // I've left the code in case anyone ever has time/inclination to fix it
        if((index >= 0) && (index < (int)m_book->GetPageCount())) 
        {
            m_book->SetSelection(index);
        } 
        else if(m_book->GetPageCount()) 
        {
            m_book->SetSelection(0);
        }
        m_mgr->Update();
    }
    //-----------------------------------------------------------------
    void WorkspacePane::SaveWorkspaceViewTabOrder() const
    {
    #if USE_AUI_NOTEBOOK
        wxArrayString panes = m_book->GetAllTabsLabels();
    #else
        wxArrayString panes;
        TabInfoVec_t tabs;
        m_book->GetAllTabs(tabs);
        std::for_each(tabs.begin(), tabs.end(), [&](TabInfoPtr_t t) { panes.Add(t->GetLabel()); });
    #endif
        gsgs_Config().SetWorkspaceTabOrder(panes, m_book->GetSelection(), false);
    }
    //-----------------------------------------------------------------
    void WorkspacePane::DoShowTab(bool show, const wxString& title)
    {
        if(!show) 
        {
            for(size_t i = 0; i < m_book->GetPageCount(); i++) 
            {
                if(m_book->GetPageText(i) == title) 
                {
                    // we've got a match
                    m_book->RemovePage(i);
                    wxWindow * win = DoGetControlByName(title);
                    if(win) { win->Show(false); }
                    break;
                }
            }
        } 
        else 
        {
            for(size_t i = 0; i < m_book->GetPageCount(); i++) 
            {
                if(m_book->GetPageText(i) == title) 
                {
                    // requested to add a page which already exists
                    return;
                }
            }

            // Fetch the list of detached panes
            // If the mainframe is NULL, read the
            // list from the disk, otherwise use the
            // dockable pane menu

            // Read it from the disk
            DetachedPanesInfo dpi;
            gsgs_EditorConfig().ReadObject(wxT("DetachedPanesList"), &dpi);
            wxArrayString detachedPanes;
            detachedPanes = dpi.GetPanes();

            if(IS_DETACHED(title)) 
                return;

            wxWindow* win = DoGetControlByName(title);
            if(win) 
            {
                win->Show(true);
                m_book->InsertPage(0, win, title, true);
            }
        }
    }
    //-----------------------------------------------------------------
    wxWindow* WorkspacePane::DoGetControlByName(const wxString & title)
    {
        if(title == _("Resource"))
            return m_explorer;
        else if(title == _("Workspace"))
            return m_workspaceTab;
    #ifndef __WXOSX__
        else if(title == _("Team"))
            return m_openWindowsPane;
    #endif
        //else if(title == _("SFTP"))
        //    return m_TabgroupsPane;
        else if (title == _("Service"))
            return m_ServicePane;
        return NULL;
    }
    //-----------------------------------------------------------------
    bool WorkspacePane::IsTabVisible(int flag)
    {
        wxWindow * win(NULL);
        wxString title;

        switch(flag) 
        {
        case View_Show_Workspace_Tab:
            title = _("Workspace");
            win = DoGetControlByName(_("Workspace"));
            break;
        case View_Show_Explorer_Tab:
            title = _("Resource");
            win = DoGetControlByName(_("Resource"));
            break;
    #ifndef __WXOSX__
        case View_Show_Tabs_Tab:
            title = _("Team");
            win = DoGetControlByName(_("Team"));
            break;
    #endif
        /*case View_Show_Tabgroups_Tab:
            title = _("SFTP");
            win = DoGetControlByName(_("SFTP"));
            break;*/
        case View_Show_Service_Tab:
            title = _("Service");
            win = DoGetControlByName(_("Service"));
            break;
        }

        if(!win || title.IsEmpty()) 
            return false;

        // if the control exists in the notebook, return true
        for(size_t i = 0; i < m_book->GetPageCount(); ++i) 
        {
            if(m_book->GetPageText(i) == title) 
            { 
                return true; 
            }
        }
        return win && win->IsShown();
    }
    //-----------------------------------------------------------------
    void WorkspacePane::OnInitDone(wxCommandEvent& event)
    {
        event.Skip();
        //m_captionEnabler.Initialize(this, m_caption, gsgs_Frame()->GetAuiManager());
    }
    //-----------------------------------------------------------------
    void WorkspacePane::SelectTab(const wxString& tabTitle)
    {
        for(size_t i = 0; i < m_book->GetPageCount(); i++) 
        {
            if(m_book->GetPageText(i) == tabTitle)
            {
                m_book->SetSelection(i);
                break;
            }
        }
    }
    //-----------------------------------------------------------------
    void WorkspacePane::OnSettingsChanged(wxCommandEvent& event)
    {
        event.Skip();
        long style = 0;
        wxDirection tempdir = gsgs_EditorConfig().GetOptions()->GetWorkspaceTabsDirection();
        if (tempdir == wxBOTTOM)
            style = kNotebook_BottomTabs;
        else if (tempdir == wxRIGHT)
            style = kNotebook_RightTabs;
        else if (tempdir == wxLEFT)
            style = kNotebook_LeftTabs;
    /*#if !USE_AUI_NOTEBOOK
        m_book->SetArt(GetNotebookRenderer());
    #endif*/
        if(gsgs_EditorConfig().GetOptions()->IsTabColourDark()) {
            m_book->SetStyle((m_book->GetStyle() & ~kNotebook_LightTabs) | kNotebook_DarkTabs | kNotebook_BottomTabs | style);
        } 
        else 
        {
            m_book->SetStyle((m_book->GetStyle() & ~kNotebook_DarkTabs) | kNotebook_LightTabs | kNotebook_BottomTabs | style);
        }
    }
    //-----------------------------------------------------------------
    void WorkspacePane::OnToggleWorkspaceTab(Event& event)
    {
        // Handle the core tabs
        if(m_tabs.count(event.GetString()) == 0) 
        {
            event.Skip();
            return;
        }

        const Tab& t = m_tabs.find(event.GetString())->second;
        if(event.IsSelected()) 
        {
            // Insert the page
            int where = TabTogglerHelper::IsTabInNotebook(GetNotebook(), t.m_label);
            if(where == wxNOT_FOUND) 
            {
                GetNotebook()->AddPage(t.m_window, t.m_label, true, t.m_bmp);
            } 
            else 
            {
                GetNotebook()->SetSelection(where);
            }
        } 
        else 
        {
            // hide the tab
            int where = GetNotebook()->GetPageIndex(t.m_label);
            if(where != wxNOT_FOUND) 
            { 
                GetNotebook()->RemovePage(where); 
            }
        }
    }
    //-----------------------------------------------------------------
    /*TabViewPtr_t WorkspacePane::GetNotebookRenderer() 
    { 
        return TabView::CreateRenderer(m_book->GetStyle()); 
    }*/
    //-----------------------------------------------------------------
    void WorkspacePane::OnWorkspaceBookFileListMenu(ContextMenuEvent& event)
    {
        wxMenu* menu = event.GetMenu();
        DetachedPanesInfo dpi;
        gsgs_EditorConfig().ReadObject("DetachedPanesList", &dpi);

        wxMenu* hiddenTabsMenu = new wxMenu();
        const wxArrayString& tabs = GetManager()->GetWorkspaceTabs();
        for(size_t i = 0; i < tabs.size(); ++i) 
        {
            const wxString& label = tabs.Item(i);
            if((m_book->GetPageIndex(label) != wxNOT_FOUND)) 
            {
                continue;
            }
            
            if(hiddenTabsMenu->GetMenuItemCount() == 0) {
                // we are adding the first menu item
                menu->AppendSeparator();
            }
            
            int tabId = wxXmlResource::GetXRCID(wxString() << "workspace_tab_" << label);
            hiddenTabsMenu->Append(tabId, label);

            // If the tab is detached, disable it's menu entry
            if(dpi.GetPanes().Index(label) != wxNOT_FOUND) { hiddenTabsMenu->Enable(tabId, false); }

            // Bind the event
            hiddenTabsMenu->Bind(wxEVT_MENU,
                                 // Use lambda by value here so we make a copy
                                 [=](wxCommandEvent& e) {
                                     Event eventShow(wxEVT_SHOW_WORKSPACE_TAB);
                                     eventShow.SetSelected(true).SetString(label);
                                     gsgs_Event().AddPendingEvent(eventShow);
                                 },
                                 tabId);
        }
        if(hiddenTabsMenu->GetMenuItemCount() == 0) 
        {
            wxDELETE(hiddenTabsMenu);
        } 
        else 
        {
            menu->AppendSubMenu(hiddenTabsMenu, _("Hidden Tabs"), _("Hidden Tabs"));
        }
    }
    //-----------------------------------------------------------------
}