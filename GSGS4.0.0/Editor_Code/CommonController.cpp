/*
Copyright (c) "2018-2019", Shenzhen Mindeng Technology Co., Ltd(www.niiengine.com),
        Mindeng Base Communication Application Framework
All rights reserved.
    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
    Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice, this list
of conditions and the following disclaimer in the documentation and/or other materials
provided with the distribution.
    Neither the name of the "ORGANIZATION" nor the names of its contributors may be used
to endorse or promote products derived from this software without specific prior written
permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "CommonController.h"
#include "CommonController1.h"
#include "gsgsWindowManager.h"
#include "gsgsException.h"
#include "gsgsWorkspace.h"
#include "gsgsMacroManager.h"
#include "gsgsPluginManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsDebuggerManager.h"
#include "gsgsStyleManager.h"
#include "gsgsSerializer.h"
#include "NewProjectDialog.h"
#include "gsgsBuildConfig.h"
#include "gsgsToolsetManager.h"
#include "gsgsComment.h"
#include "gsgsFilePicker.h"
#include "gsgsCommon.h"
#include "gsgsFrame.h"
#include "gsgsDebugPane.h"
#include "debugger.h"
#include "plugin.h"
#include "gsgsLexerStyle.h"
#include "gsgsOptionsConfig.h"
#include "gsgsBookmarkManager.h"
#include "gsgsOutputPane.h"
#include "gsgsConfig.h"
#include "gsgsEditor.h"
#include "gsgsBook.h"
#include "gsgsQuickFindBar.h"
#include "gsgsCommonDialog.h"
#include "gsgsTagManager.h"
#include "gsgsLogManager.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsConsole.h"
#include "gsgsPerspectiveManager.h"
#include "gsgsManager.h"
#include "globals.h"
#include "project.h"
#include "context_cpp.h"
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/imaglist.h>
#include <wx/wupdlock.h>
#include <wx/txtstrm.h>
#include <wx/sstream.h>
#include <wx/choicebk.h>
#include <wx/display.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/textctrl.h>
#include <wx/imaglist.h>
#include <wx/generic/textdlgg.h>
#include <wx/xrc/xmlres.h>
#include <wx/stc/stc.h>
#include <wx/wxprec.h>
#include <wx/fontmap.h>
#include <wx/regex.h>
#include <wx/intl.h>
#include <memory>

#ifdef __WXMSW__
#include <wx/msw/uxtheme.h>
#endif
class NewTaskDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxStaticText* m_staticText2;
		
		wxStaticLine* m_staticline2;
		Button* m_button5;
		Button* m_button6;
	
	public:
		wxTextCtrl* m_name;
		wxTextCtrl* m_regex;
		
		NewTaskDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("New Task"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER )
            :wxDialog( parent, id, title, pos, size, style )
        {
            this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
            wxBoxSizer* bSizer5;
            bSizer5 = new wxBoxSizer( wxVERTICAL );
            
            wxFlexGridSizer* fgSizer1;
            fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
            fgSizer1->AddGrowableCol( 1 );
            fgSizer1->SetFlexibleDirection( wxBOTH );
            fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
            
            m_staticText1 = gsgs_Style().createStaticText( this, wxID_ANY, _("Task Name:"), wxDefaultPosition, wxDefaultSize, 0 );
            m_staticText1->Wrap( -1 );
            fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
            
            m_name = gsgs_Style().createTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
            fgSizer1->Add( m_name, 0, wxALL|wxEXPAND, 5 );
            
            m_staticText2 = gsgs_Style().createStaticText( this, wxID_ANY, _("Regular Expression:"), wxDefaultPosition, wxDefaultSize, 0 );
            m_staticText2->Wrap( -1 );
            fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
            
            m_regex = gsgs_Style().createTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
            fgSizer1->Add( m_regex, 0, wxALL|wxEXPAND, 5 );
            
            bSizer5->Add( fgSizer1, 0, wxEXPAND, 5 );
            
            
            bSizer5->Add( 0, 0, 1, wxEXPAND, 5 );
            
            m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
            bSizer5->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
            
            wxBoxSizer* bSizer6;
            bSizer6 = new wxBoxSizer( wxHORIZONTAL );
            
            m_button5 = new Button( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
            m_button5->SetDefault(); 
            bSizer6->Add( m_button5, 0, wxALL, 5 );
            
            m_button6 = new Button( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
            bSizer6->Add( m_button6, 0, wxALL, 5 );
            
            bSizer5->Add( bSizer6, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

            
            this->Centre( wxBOTH );
        }
		~NewTaskDialog(){}
	
};
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// AddOptionCheckDlg
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
OptionAddDlg::OptionAddDlg() :
    OptionAddView(NULL) {}

void OptionAddDlg::SetValue(const wxString & value)
{
    wxStringTokenizer tkz(value, wxT(";"));
    while (tkz.HasMoreTokens()) {
        wxString token = tkz.GetNextToken();
        token = token.Trim();
        token = token.Trim(false);
        m_text->AppendText(token + wxT("\n"));
    }
    this->Layout();
}
//---------------------------------------------------------------------------------
wxString OptionAddDlg::GetValue() const
{
    wxStringInputStream input(m_text->GetValue());
    wxTextInputStream text(input);

    wxString value;
    while (!input.Eof()) {

        // Read the next line
        value += text.ReadLine();
        value += wxT(";");
    }
    return value.BeforeLast(wxT(';'));
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// AddOptionCheckDlg
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
AddOptionCheckDlg::AddOptionCheckDlg(wxWindow* parent,
    const wxString& title,
    const Compiler::CmpCmdLineOptions& cmpOptions,
    const wxString& value)
    : OptionCommandAddView(parent, wxID_ANY, title)
    , m_cmpOptions(cmpOptions)
{
    SetName("AddOptionCheckDlg");
    gsgs::WindowManager::Load(this);

    // Fill the list of available options
    Compiler::CmpCmdLineOptions::const_iterator itOption = m_cmpOptions.begin();
    for (; itOption != m_cmpOptions.end(); ++itOption) {
        const Compiler::CmpCmdLineOption& cmpOption = itOption->second;
        m_checkListOptions->Append(cmpOption.help + wxT(" [") + cmpOption.name + wxT("]"));
    }

    // Update controls
    SetValue(value);
}
//---------------------------------------------------------------------------------
AddOptionCheckDlg::~AddOptionCheckDlg() {}
//---------------------------------------------------------------------------------
void AddOptionCheckDlg::SetValue(const wxString& value)
{
    m_textCmdLn->SetValue(value);

    UpdateOptions();
    UpdateCmdLine();
}
//---------------------------------------------------------------------------------
wxString AddOptionCheckDlg::GetValue() const { return m_textCmdLn->GetValue(); }
//---------------------------------------------------------------------------------
void AddOptionCheckDlg::OnOptionToggled(wxCommandEvent& event)
{
    UpdateCmdLine();
    UpdateOptions();
}
//---------------------------------------------------------------------------------
void AddOptionCheckDlg::OnOptionsText(wxCommandEvent& event) { UpdateCmdLine(); }
//---------------------------------------------------------------------------------
void AddOptionCheckDlg::UpdateOptions()
{
    // Remove all check boxes
    m_checkListOptions->Freeze();

    for (unsigned int idx = 0; idx < m_checkListOptions->GetCount(); ++idx) {
        m_checkListOptions->Check(idx, false);
    }

    // Check all options entered
    wxString customOptions;
    wxStringTokenizer tkz(m_textCmdLn->GetValue(), wxT(";"));
    while (tkz.HasMoreTokens()) {
        wxString token = tkz.GetNextToken();
        token = token.Trim().Trim(false);
        if (!token.empty()) {
            Compiler::CmpCmdLineOptions::const_iterator itOption = m_cmpOptions.find(token);
            if (itOption != m_cmpOptions.end()) {
                const Compiler::CmpCmdLineOption& cmpOption = itOption->second;
                m_checkListOptions->Check(
                    m_checkListOptions->FindString(cmpOption.help + wxT(" [") + cmpOption.name + wxT("]")));
            }
            else {
                if (!customOptions.empty()) customOptions << wxT(";");
                customOptions << token;
            }
        }
    }
    m_textOptions->ChangeValue(customOptions);

    m_checkListOptions->Thaw();
}
//---------------------------------------------------------------------------------
void AddOptionCheckDlg::UpdateCmdLine()
{
    // Store all actual options
    std::list<wxString> options;
    wxStringInputStream input(m_textOptions->GetValue());
    wxTextInputStream text(input);
    while (!input.Eof()) {
        wxString option = text.ReadLine().Trim().Trim(false);
        if (!option.empty()) {
            options.push_back(option);
        }
    }

    // Read check box options
    for (unsigned int idx = 0; idx < m_checkListOptions->GetCount(); ++idx) {
        wxString value = m_checkListOptions->GetString(idx).AfterLast(wxT('[')).BeforeLast(wxT(']'));
        if (m_checkListOptions->IsChecked(idx)) {
            // If the option doesn't exist actually, add it
            if (std::find(options.begin(), options.end(), value) == options.end()) {
                // options.push_back(value.AfterLast(wxT('[')).BeforeLast(wxT(']')));
                options.insert(options.begin(), value);
            }
            else
                // uncheck the option if already defined manualy
                m_checkListOptions->Check(idx, false);
        }
    }

    // Update the options textctrl
    wxString value;
    std::list<wxString>::const_iterator itOption = options.begin();
    for (; itOption != options.end(); ++itOption) {
        if (!value.Contains(*itOption + wxT(";"))) {
            if (!value.empty()) value << wxT(";");
            value << *itOption;
        }
    }
    m_textCmdLn->SetValue(value);
}

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// AddOptionCheckDlg
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

ProjectBuildDlg::ProjectBuildDlg(wxWindow* parent)
    : ProjectBuildView(parent)
{
    m_checkListConfigurations->SetFocus();
    DoInitialize();
}
//---------------------------------------------------------------------------------
void ProjectBuildDlg::OnItemSelected(wxCommandEvent& event)
{
    event.Skip();
}
//---------------------------------------------------------------------------------
void ProjectBuildDlg::OnItemToggled(wxCommandEvent& event)
{
    wxUnusedVar(event);
}
//---------------------------------------------------------------------------------
void ProjectBuildDlg::OnBuild(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_cmd = QueueCommand::kBuild;
    DoSaveBatchBuildOrder();
    EndModal(wxID_OK);
}
//---------------------------------------------------------------------------------
void ProjectBuildDlg::OnBuildUI(wxUpdateUIEvent& event)
{
    bool enable(false);
    for (unsigned int i = 0; i < m_checkListConfigurations->GetCount(); i++) {
        if (m_checkListConfigurations->IsChecked(i)) {
            enable = true;
            break;
        }
    }

    event.Enable(enable);
}
//---------------------------------------------------------------------------------
void ProjectBuildDlg::OnClean(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_cmd = QueueCommand::kClean;
    DoSaveBatchBuildOrder();
    EndModal(wxID_OK);
}
//---------------------------------------------------------------------------------
void ProjectBuildDlg::OnCleanUI(wxUpdateUIEvent& event)
{
    bool enable(false);
    for (unsigned int i = 0; i < m_checkListConfigurations->GetCount(); i++) {
        if (m_checkListConfigurations->IsChecked(i)) {
            enable = true;
            break;
        }
    }

    event.Enable(enable);
}
//---------------------------------------------------------------------------------
void ProjectBuildDlg::OnCheckAll(wxCommandEvent& event)
{
    wxUnusedVar(event);
    for (unsigned int i = 0; i < m_checkListConfigurations->GetCount(); i++) {
        m_checkListConfigurations->Check(i, true);
    }
}
//---------------------------------------------------------------------------------
void ProjectBuildDlg::OnUnCheckAll(wxCommandEvent& event)
{
    wxUnusedVar(event);
    for (unsigned int i = 0; i < m_checkListConfigurations->GetCount(); i++) {
        m_checkListConfigurations->Check(i, false);
    }
}
//---------------------------------------------------------------------------------
void ProjectBuildDlg::OnMoveUp(wxCommandEvent& event)
{
    wxString selectedString = m_checkListConfigurations->GetStringSelection();

    int sel = m_checkListConfigurations->GetSelection();
    if (sel == wxNOT_FOUND) {
        return;
    }
    bool checked = m_checkListConfigurations->IsChecked(sel);
    sel--;
    if (sel < 0) {
        return;
    }

    // sel contains the new position we want to place the selection string
    m_checkListConfigurations->Delete(sel + 1);
    m_checkListConfigurations->Insert(selectedString, sel);
    m_checkListConfigurations->Check(sel, checked);
    m_checkListConfigurations->Select(sel);
}
//---------------------------------------------------------------------------------
void ProjectBuildDlg::OnMoveUpUI(wxUpdateUIEvent& event)
{
    event.Skip();
}
//---------------------------------------------------------------------------------
void ProjectBuildDlg::OnMoveDown(wxCommandEvent& event)
{
    int sel = m_checkListConfigurations->GetSelection();
    if (sel == wxNOT_FOUND) {
        return;
    }
    bool checked = m_checkListConfigurations->IsChecked(sel);
    sel++;
    if (sel >= (int)m_checkListConfigurations->GetCount()) {
        return;
    }

    // sel contains the new position we want to place the selection string
    wxString oldStr = m_checkListConfigurations->GetString(sel);

    m_checkListConfigurations->Delete(sel);
    m_checkListConfigurations->Insert(oldStr, sel - 1);
    m_checkListConfigurations->Check(sel - 1, checked);
    m_checkListConfigurations->Select(sel);
}
//---------------------------------------------------------------------------------
void ProjectBuildDlg::OnMoveDownUI(wxUpdateUIEvent& event)
{
    event.Skip();
}
//---------------------------------------------------------------------------------
void ProjectBuildDlg::OnClose(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoSaveBatchBuildOrder();
    EndModal(wxID_CANCEL);
}
//---------------------------------------------------------------------------------
void ProjectBuildDlg::GetBuildInfoList(std::list<QueueCommand>& buildInfoList)
{
    bool clean_log(true);
    for (unsigned int i = 0; i < m_checkListConfigurations->GetCount(); i++) {
        if (m_checkListConfigurations->IsChecked(i)) {
            wxString text = m_checkListConfigurations->GetString(i);
            wxString project = text.BeforeFirst(wxT('|'));
            wxString config = text.AfterFirst(wxT('|'));

            project.Trim().Trim(false);
            config.Trim().Trim(false);

            // get the selected configuration to be built
            BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(project, config);
            if (bldConf) {

                QueueCommand buildInfo(project, config, true, m_cmd);
                // handle custom build projects properly
                if (bldConf->IsCustomBuild()) {
                    buildInfo.SetKind(QueueCommand::kCustomBuild);
                    switch (m_cmd) {
                    case QueueCommand::kBuild:
                        buildInfo.SetCustomBuildTarget(wxT("Build"));
                        break;
                    case QueueCommand::kClean:
                        buildInfo.SetCustomBuildTarget(wxT("Clean"));
                        break;
                    }
                }
                buildInfo.SetCleanLog(clean_log);
                buildInfoList.push_back(buildInfo);
                clean_log = false;
            }
        }
    }
}
//---------------------------------------------------------------------------------
void ProjectBuildDlg::DoInitialize()
{
    // load the previously saved batch build file
    wxFileName fn(gsgs_CppWorker().GetFileName());
    fn.SetExt(wxT("batch_build"));

    wxString content;
    wxArrayString arr;
    if (ReadFileWithConversion(fn.GetFullPath(), content)) {
        arr = wxStringTokenize(content, wxT("\n"), wxTOKEN_STRTOK);
        for (size_t i = 0; i < arr.GetCount(); i++) {
            int idx = m_checkListConfigurations->Append(arr.Item(i));
            m_checkListConfigurations->Check((unsigned int)idx);
        }
    }

    // loop over all projects, for each project collect all available
    // build configurations and add them to the check list control
    wxArrayString projects;
    gsgs_CppWorker().GetProjectList(projects);
    for (size_t i = 0; i < projects.GetCount(); i++) {
        ProjectPtr p = gsgs_Manager()->GetProject(projects.Item(i));
        if (p) {
            ProjectSettingsPtr settings = p->GetSettings();
            if (settings) {
                ProjectSettingsCookie cookie;
                BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
                while (bldConf) {
                    wxString item(p->GetName() + wxT(" | ") + bldConf->GetName());

                    int where = arr.Index(item);
                    if (where == wxNOT_FOUND) {
                        // append this item
                        m_checkListConfigurations->Append(item);
                    }
                    else {
                        // this item already been added,
                        // remove it from the arr and continue
                        arr.RemoveAt((size_t)where);
                    }

                    bldConf = settings->GetNextBuildConfiguration(cookie);
                }
            }
        }
    }

    // check to see which configuration was left in 'arr'
    // and remove them from the checklistbox
    for (size_t i = 0; i < arr.GetCount(); i++) {
        int where = m_checkListConfigurations->FindString(arr.Item(i));
        if (where != wxNOT_FOUND) {
            m_checkListConfigurations->Delete((unsigned int)where);
        }
    }
    arr.clear();

    if (m_checkListConfigurations->GetCount() > 0) {
        m_checkListConfigurations->Select(0);
    }
}
//---------------------------------------------------------------------------------
void ProjectBuildDlg::DoSaveBatchBuildOrder()
{
    wxFileName fn(gsgs_CppWorker().GetFileName());
    fn.SetExt(wxT("batch_build"));

    wxString content;
    for (unsigned int i = 0; i < m_checkListConfigurations->GetCount(); i++) {
        if (m_checkListConfigurations->IsChecked(i)) {
            content << m_checkListConfigurations->GetString(i) << wxT("\n");
        }
    }

    WriteFileWithBackup(fn.GetFullPath(), content, false);
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// DbgCommandDlg
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
DbgCommandDlg::DbgCommandDlg(wxWindow* parent)
    : DebugCommandView(parent)
{
    Centre();
    GetSizer()->Fit(this);
    m_textCtrlName->SetFocus();

    SetName("DbgCommandDlg");
    gsgs::WindowManager::Load(this);
}
//---------------------------------------------------------------------------------
DbgCommandDlg::~DbgCommandDlg() {}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// DependenciesDlg
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
class clProjectDependecySorter
{
public:
    enum eVisit { kNone, kTemp, kPerm };
    struct Node
    {
        eVisit marker;
        wxString name;
        std::vector<Node*> adjacents;
        Node()
            : marker(kNone)
        {
        }
    };
    typedef std::unordered_map<std::string, clProjectDependecySorter::Node> Graph_t;
public:
    clProjectDependecySorter();
    virtual ~clProjectDependecySorter();

    /**
     * @brief return the build order for a given project taking its dependencies into consideration
     * @param projectName
     * @param buildOrder [output]
     * @return the build order. Throws gsgs::Exception in case of an error
     */
    void GetProjectBuildOrder(const wxString& projectName, const wxString& configName, wxArrayString& buildOrder);
protected:
    void Visit(clProjectDependecySorter::Node* node, wxArrayString& buildOrder);
    clProjectDependecySorter::Node* GetNodeCreateIfNeeded(Graph_t& G, const wxString& name);
};
//---------------------------------------------------------------------------------
clProjectDependecySorter::clProjectDependecySorter() {}
//---------------------------------------------------------------------------------
clProjectDependecySorter::~clProjectDependecySorter() {}
//---------------------------------------------------------------------------------
void clProjectDependecySorter::Visit(clProjectDependecySorter::Node* node, wxArrayString& buildOrder)
{
    if(node->marker == kPerm) return;
    if(node->marker == kTemp) throw gsgs::Exception("Dependency loop found for node: " + node->name);

    node->marker = kTemp;
    std::for_each(node->adjacents.begin(), node->adjacents.end(), [&](Node* adj) { Visit(adj, buildOrder); });
    node->marker = kPerm;
    buildOrder.Insert(node->name, 0); // add as the first entry
}
//---------------------------------------------------------------------------------
void clProjectDependecySorter::GetProjectBuildOrder(const wxString& projectName, const wxString& configName,
                                                    wxArrayString& buildOrder)
{
    Graph_t G;
    wxArrayString projects;
    gsgs_CppWorker().GetProjectList(projects);
    for(size_t i = 0; i < projects.size(); ++i) {
        ProjectPtr p = gsgs_CppWorker().GetProject(projects.Item(i));
        if(!p) {
            throw gsgs::Exception("Could not find project: " + projects.Item(i));
        }
        Node* projectNode = GetNodeCreateIfNeeded(G, p->GetName());
        wxArrayString deps = p->GetDependencies(configName);
        std::for_each(deps.begin(), deps.end(), [&](const wxString& dep) {
            Node* adj = GetNodeCreateIfNeeded(G, dep);
            adj->adjacents.push_back(projectNode);
        });
    }

    std::for_each(G.begin(), G.end(), [&](Graph_t::value_type& vt) {
        Node* n = &(vt.second);
        if(n->marker == clProjectDependecySorter::kNone) {
            Visit(n, buildOrder);
        }
    });
}
//---------------------------------------------------------------------------------
clProjectDependecySorter::Node* clProjectDependecySorter::GetNodeCreateIfNeeded(Graph_t& G, const wxString& name)
{
    std::string key = name.mb_str(wxConvUTF8).data();
    if(G.count(key) == 0) {
        clProjectDependecySorter::Node n;
        n.name = key;
        G[key] = n;
    }
    return &G[key];
}
//---------------------------------------------------------------------------------
DependenciesDlg::DependenciesDlg(
    wxWindow* parent, const wxString& prjname, int id, wxString title, wxPoint pos, wxSize size, int style)
    : ProjectDependView(parent, id, title, pos, size, style)
    , m_projectName(prjname)
{
    m_book->GetChoiceCtrl()->SetFocus();
    Centre();
    Init();

    SetName("DependenciesDlg");
    gsgs::WindowManager::Load(this);
}
//---------------------------------------------------------------------------------
void DependenciesDlg::setProjectName(const wxString & name)
{
    m_projectName = name;
}
//---------------------------------------------------------------------------------
const wxString & DependenciesDlg::getProjectName() const
{
    return m_projectName;
}
//---------------------------------------------------------------------------------
void DependenciesDlg::Init()
{
    // fill the pages of the choice book
    wxArrayString projects;
    gsgs_Manager()->GetProjectList(projects);
    for (size_t i = 0; i < projects.GetCount(); i++) {
        m_book->AddPage(new DependenciesPage(m_book, projects.Item(i)), projects.Item(i), false);
    }

    CallAfter(&DependenciesDlg::DoSelectProject);

    // connect events
    ConnectButton(m_buttonOK, DependenciesDlg::OnButtonOK);
    ConnectButton(m_buttonCancel, DependenciesDlg::OnButtonCancel);
}
//---------------------------------------------------------------------------------
void DependenciesDlg::OnButtonOK(wxCommandEvent& event)
{
    wxUnusedVar(event);
    for (size_t i = 0; i < m_book->GetPageCount(); i++) {
        DependenciesPage* page = dynamic_cast<DependenciesPage*>(m_book->GetPage(i));
        if (page) {
            page->Save();
        }
    }
    EndModal(wxID_OK);
}
//---------------------------------------------------------------------------------
void DependenciesDlg::OnButtonCancel(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_CANCEL);
}
//---------------------------------------------------------------------------------
void DependenciesDlg::DoSelectProject()
{
    wxArrayString projects;
    gsgs_Manager()->GetProjectList(projects);
    int sel = projects.Index(m_projectName);
    if (sel != wxNOT_FOUND) {
        m_book->ChangeSelection(sel);
    }

    //clProjectDependecySorter sorter;
    //wxArrayString buildOrder;
    //sorter.GetProjectBuildOrder(m_projectName, "Win_x64_Release", buildOrder);;
}

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// DebugCoreDumpInfo DebugCoreDumpDlg
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

class DebugCoreDumpInfo : public ObjectData
{
public:
    DebugCoreDumpInfo() : m_selectedDbg(0) {}
    ~DebugCoreDumpInfo() {}

    virtual void DeSerialize(Serializer &arch)
    {
        arch.Read(wxT("m_coreName"), m_coreFilepaths);
        arch.Read(wxT("m_exeFilePaths"), m_exeFilepaths);
        arch.Read(wxT("m_wds"), m_wds);
        arch.Read(wxT("m_selectedDbg"), m_selectedDbg);
    }
    virtual void Serialize(Serializer &arch)
    {
        arch.Write(wxT("m_coreName"), m_coreFilepaths);
        arch.Write(wxT("m_exeFilePaths"), m_exeFilepaths);
        arch.Write(wxT("m_wds"), m_wds);
        arch.Write(wxT("m_selectedDbg"), m_selectedDbg);
    }

    void SetCoreFilepaths(const wxArrayString& coreFilepaths) {
        this->m_coreFilepaths = coreFilepaths;
    }
    void SetExeFilepaths(const wxArrayString& exeFilepaths) {
        this->m_exeFilepaths = exeFilepaths;
    }
    void SetSelectedDbg(const int& selectedDbg) {
        this->m_selectedDbg = selectedDbg;
    }
    void SetWDs(const wxArrayString& wds) {
        this->m_wds = wds;
    }

    const wxArrayString& GetCoreFilepaths() const {
        return m_coreFilepaths;
    }
    const wxArrayString& GetExeFilepaths() const {
        return m_exeFilepaths;
    }
    const int& GetSelectedDbg() const {
        return m_selectedDbg;
    }
    const wxArrayString& GetWds() const {
        return m_wds;
    }

protected:
    wxArrayString m_coreFilepaths;
    wxArrayString m_exeFilepaths;
    wxArrayString m_wds;
    int m_selectedDbg;
};
//---------------------------------------------------------------------------------
DebugCoreDumpDlg::DebugCoreDumpDlg(wxWindow* parent)
    : DebugCoreDumpView(parent)
{
    // Prevent enormously-long strings from crowding the browse buttons off the screen
    wxDisplay display;
    int width = display.GetClientArea().GetWidth();
    wxSize size(width / 4, -1);
    m_Core->SetInitialSize(size);
    m_ExeFilepath->SetInitialSize(size);
    m_WD->SetInitialSize(size);

    SetMaxSize(wxSize(width * 2 / 3, -1));
    GetSizer()->Layout();
    GetSizer()->Fit(this);

    SetName("DebugCoreDumpDlg");
    gsgs::WindowManager::Load(this);

    Initialize();
    if (m_Core->GetCount() == 0) {
        // If there's no known core yet, set focus here so one can be added
        m_Core->SetFocus();
    }
    else {
        m_buttonDebug->SetFocus();
    }
}
//---------------------------------------------------------------------------------
void DebugCoreDumpDlg::Initialize()
{
    DebugCoreDumpInfo info;
    gsgs_EditorConfig().ReadObject(wxT("DebugCoreDumpDlg"), &info);

    m_choiceDebuggers->Append(DebuggerMgr::Get().GetAvailableDebuggers());
    if (m_choiceDebuggers->GetCount()) {
        m_choiceDebuggers->SetSelection(0);
    }
    if (m_choiceDebuggers->GetCount() > (unsigned int)info.GetSelectedDbg()) {
        m_choiceDebuggers->SetSelection(info.GetSelectedDbg());
    }

    m_Core->Append(info.GetCoreFilepaths());
    if (m_Core->GetCount() > 0) {
        m_Core->SetSelection(0);
    }

    m_WD->Append(info.GetWds());
    if (m_WD->GetCount() > 0) {
        m_WD->SetSelection(0);
    }

    m_ExeFilepath->Append(info.GetExeFilepaths());
    if (m_ExeFilepath->GetCount() > 0) {
        m_ExeFilepath->SetSelection(0);
    }
    else {
        // determine the executable to debug:
        // - If the 'Program' field is set - we use it
        // - Else we use the project's output name
        wxString activename, conf;
        gsgs_Manager()->GetActiveProjectAndConf(activename, conf);
        BuildConfigPtr buildConf = gsgs_CppWorker().GetProjBuildConf(activename, conf);
        if (buildConf) {
            // expand all macros with their values
            wxString programToDebug = buildConf->GetCommand();
            programToDebug.Trim().Trim(false);

            if (programToDebug.IsEmpty()) {
                programToDebug = buildConf->GetOutputFileName();
            }
            wxString outputFile =
                MacroManager::Instance()->Expand(programToDebug, PluginManager::Get(), activename, conf);

            if (m_ExeFilepath->Append(outputFile) != wxNOT_FOUND) {
                m_ExeFilepath->SetSelection(0);
            }

            // determine the working directory
            // if we have a working directory set in the project settings, use it (if it is not an
            // absolute path, it will be appended to the project's path)
            wxString projWD = MacroManager::Instance()->Expand(
                buildConf->GetWorkingDirectory(), PluginManager::Get(), activename, conf);
            projWD.Trim().Trim(false);
            wxString wd;
            ProjectPtr proj = gsgs_Manager()->GetProject(activename);
            if (proj) {
                if (projWD.IsEmpty() || !wxFileName(projWD).IsAbsolute()) {
                    wxString basePath = proj->GetFileName().GetPath();
                    wd << basePath << gsgsPathSep;
                }
            }
            wd << projWD;

            if (m_WD->Insert(wd, 0) != wxNOT_FOUND) {
                m_WD->SetSelection(0);
            }
        }
    }
}
//---------------------------------------------------------------------------------
void DebugCoreDumpDlg::OnButtonBrowseCore(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString path, ans;
    wxFileName fn(GetCore());
    if (fn.FileExists()) {
        // Use the serialised path as the wxFileSelector default path
        path = fn.GetPath();
    }
    else {
        // Otherwise use any working dir entry, which might just have been altered
        path = GetWorkingDirectory();
    }

    ans = wxFileSelector(_("Select core dump:"), path);
    if (!ans.empty()) {
        m_Core->Insert(ans, 0);
        m_Core->SetSelection(0);
    }
}
//---------------------------------------------------------------------------------
void DebugCoreDumpDlg::OnButtonBrowseExe(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString path, ans;
    wxFileName fn(GetExe());
    if (fn.FileExists()) {
        // Use the serialised path as the wxFileSelector default path
        path = fn.GetPath();
    }
    else {
        // Otherwise use any working dir entry, which might just have been altered
        path = GetWorkingDirectory();
    }

    ans = wxFileSelector(_("Select file:"), path);
    if (!ans.empty()) {
        m_ExeFilepath->Insert(ans, 0);
        m_ExeFilepath->SetSelection(0);
    }
}
//---------------------------------------------------------------------------------
void DebugCoreDumpDlg::OnButtonBrowseWD(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString ans, path(GetWorkingDirectory());
    if (!wxFileName::DirExists(path)) {
        path = wxGetCwd();
    }

    ans = wxDirSelector(_("Select working directory:"), path);
    if (!ans.empty()) {
        m_WD->Insert(ans, 0);
        m_WD->SetSelection(0);
    }
}
//---------------------------------------------------------------------------------
void DebugCoreDumpDlg::OnButtonCancel(wxCommandEvent& event)
{
    wxUnusedVar(event);

    EndModal(wxID_CANCEL);
}
//---------------------------------------------------------------------------------
void DebugCoreDumpDlg::OnButtonDebug(wxCommandEvent& event)
{
    wxUnusedVar(event);

    // save values
    const size_t MAX_NO_ITEMS = 10;
    DebugCoreDumpInfo info;
    info.SetCoreFilepaths(ReturnWithStringPrepended(m_Core->GetStrings(), GetCore(), MAX_NO_ITEMS));
    info.SetExeFilepaths(ReturnWithStringPrepended(m_ExeFilepath->GetStrings(), GetExe(), MAX_NO_ITEMS));
    info.SetWDs(ReturnWithStringPrepended(m_WD->GetStrings(), GetWorkingDirectory(), MAX_NO_ITEMS));
    info.SetSelectedDbg(m_choiceDebuggers->GetSelection());

    gsgs_EditorConfig().WriteObject(wxT("DebugCoreDumpDlg"), &info);

    EndModal(wxID_OK);
}
//---------------------------------------------------------------------------------
void DebugCoreDumpDlg::OnDebugBtnUpdateUI(wxUpdateUIEvent& event)
{
    // gdb needs both a core and an exe for useful debugging
    event.Enable(!GetCore().empty() && !GetExe().empty());
}

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// EditCmpFileInfo
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
EditCmpFileInfo::EditCmpFileInfo(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    wxWindowUpdateLocker llt(this);
    SetBackgroundColour(gsgs_Style().GetAppBgColour());
    //this->SetSizeHints(wxSize(-1, -1), wxSize(-1, -1));

    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer(wxVERTICAL);

    wxFlexGridSizer* fgSizer1;
    fgSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
    fgSizer1->AddGrowableCol(1);
    fgSizer1->SetFlexibleDirection(wxBOTH);
    fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("File type:"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText3->Wrap(-1);
    fgSizer1->Add(m_staticText3, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    m_textCtrlFileType = gsgs_Style().createTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    fgSizer1->Add(m_textCtrlFileType, 0, wxALL | wxEXPAND, 5);

    m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Handle file has:"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText2->Wrap(-1);
    fgSizer1->Add(m_staticText2, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    wxString m_choiceFileTypeIsChoices[] = { _("Source"), _("Resource") };
    int m_choiceFileTypeIsNChoices = sizeof(m_choiceFileTypeIsChoices) / sizeof(wxString);
    m_choiceFileTypeIs = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceFileTypeIsNChoices, m_choiceFileTypeIsChoices, 0);
    m_choiceFileTypeIs->SetSelection(0);
    fgSizer1->Add(m_choiceFileTypeIs, 0, wxALL | wxEXPAND, 5);

    m_staticText4 = gsgs_Style().createStaticText(this, wxID_ANY, _("Compilation line:"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText4->Wrap(-1);
    fgSizer1->Add(m_staticText4, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    m_textCtrl2 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 | wxTE_WORDWRAP);
    m_textCtrl2->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString));
    m_textCtrl2->SetMinSize(wxSize(600, -1));

    fgSizer1->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);

    bSizer1->Add(fgSizer1, 0, wxEXPAND, 5);

    wxBoxSizer* bSizer2;
    bSizer2 = new wxBoxSizer(wxHORIZONTAL);

    m_buttonHelp = new Button(this, wxID_ANY, _("Help..."), wxDefaultPosition, wxDefaultSize, 0);
    bSizer2->Add(m_buttonHelp, 0, wxALL, 5);

    m_buttonOk = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0);

    m_buttonOk->SetDefault();
    bSizer2->Add(m_buttonOk, 0, wxALL, 5);

    m_buttonCancel = new Button(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer2->Add(m_buttonCancel, 0, wxALL, 5);


    bSizer1->Add(bSizer2, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    this->SetSizer(bSizer1);
    //bSizer1->Fit(this);

    this->Centre(wxBOTH);
    this->Layout();

    // Connect Events
    m_textCtrlFileType->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(EditCmpFileInfo::OnFileTypeText), NULL, this);
    m_textCtrl2->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(EditCmpFileInfo::OnCompilationLine), NULL, this);
    m_buttonHelp->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditCmpFileInfo::OnButtonHelp), NULL, this);
    m_buttonOk->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditCmpFileInfo::OnButtonOk), NULL, this);
    m_buttonOk->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditCmpFileInfo::OnButtonOkUI), NULL, this);
    m_buttonCancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditCmpFileInfo::OnButtonCancel), NULL, this);
}
//---------------------------------------------------------------------------------
void EditCmpFileInfo::OnFileTypeText(wxCommandEvent& event) { event.Skip(); }
//---------------------------------------------------------------------------------
void EditCmpFileInfo::OnCompilationLine(wxCommandEvent& event) { event.Skip(); }
//---------------------------------------------------------------------------------
void EditCmpFileInfo::OnButtonOk(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_OK);
}
//---------------------------------------------------------------------------------
void EditCmpFileInfo::OnButtonOkUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrl2->IsEmpty() && !m_textCtrlFileType->IsEmpty());
}
//---------------------------------------------------------------------------------
void EditCmpFileInfo::OnButtonCancel(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_CANCEL);
}
//---------------------------------------------------------------------------------
void EditCmpFileInfo::SetCompilationLine(const wxString& s) { m_textCtrl2->SetValue(s); }
//---------------------------------------------------------------------------------
void EditCmpFileInfo::SetExtension(const wxString& s) { m_textCtrlFileType->SetValue(s); }
//---------------------------------------------------------------------------------
void EditCmpFileInfo::SetKind(const wxString& s)
{
    int where = m_choiceFileTypeIs->FindString(s);
    if (where != wxNOT_FOUND) {
        m_choiceFileTypeIs->Select(where);
    }
}
//---------------------------------------------------------------------------------
void EditCmpFileInfo::OnButtonHelp(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString projectName = gsgs_Manager()->GetActiveProjectName();
    ProjectPtr project = gsgs_Manager()->GetProject(projectName);
    gsgs::IEditor* editor = PluginManager::Get()->GetActiveEditor();

    MacrosDlg dlg(this, MacrosDlg::MacrosCompiler, project, editor);
    dlg.ShowModal();
}
//---------------------------------------------------------------------------------
EditCmpFileInfo::~EditCmpFileInfo() 
{
    m_textCtrlFileType->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(EditCmpFileInfo::OnFileTypeText), NULL, this);
    m_textCtrl2->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(EditCmpFileInfo::OnCompilationLine), NULL, this);
    m_buttonHelp->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditCmpFileInfo::OnButtonHelp), NULL, this);
    m_buttonOk->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditCmpFileInfo::OnButtonOk), NULL, this);
    m_buttonOk->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditCmpFileInfo::OnButtonOkUI), NULL, this);
    m_buttonCancel->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditCmpFileInfo::OnButtonCancel), NULL, this);
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// SymbolsDialog
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(SymbolsDialog, SymbolsView)
EVT_CHAR_HOOK(SymbolsDialog::OnCharHook)
END_EVENT_TABLE()
//---------------------------------------------------------------------------------
SymbolsDialog::SymbolsDialog(wxWindow* parent)
    : SymbolsView(parent)
    , m_line(wxNOT_FOUND)
    , m_file(wxEmptyString)
    , m_selectedItem(wxNOT_FOUND)
{
    // Initialise the list control
    m_results->InsertColumn(0, _("Symbol"));
    m_results->InsertColumn(1, _("Kind"));
    m_results->InsertColumn(2, _("File"));
    m_results->InsertColumn(3, _("Line"));
    m_results->InsertColumn(4, _("Pattern"));

    m_results->Connect(
        wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler(SymbolsDialog::OnItemDeselected), NULL, this);
    CentreOnParent();
    SetName("SymbolsDialog");
    gsgs::WindowManager::Load(this);
}
//---------------------------------------------------------------------------------
SymbolsDialog::~SymbolsDialog() {  }
//---------------------------------------------------------------------------------
void SymbolsDialog::AddSymbol(const TagEntryPtr& tag, bool sel)
{
    //-------------------------------------------------------
    // Populate the columns
    //-------------------------------------------------------

    wxString line;
    line << tag->GetLine();

    long index = AppendListCtrlRow(m_results);
    SetColumnText(m_results, index, 0, tag->GetFullDisplayName());
    SetColumnText(m_results, index, 1, tag->GetKind());
    SetColumnText(m_results, index, 2, tag->GetFile());
    SetColumnText(m_results, index, 3, line);
    SetColumnText(m_results, index, 4, tag->GetPattern());

    // list ctrl can reorder items, so use returned index to insert tag
    m_tags.insert(m_tags.begin() + index, tag);
}
//---------------------------------------------------------------------------------
void SymbolsDialog::AddSymbols(const std::vector<TagEntryPtr>& tags, size_t sel)
{
    wxUnusedVar(sel);
    for (size_t i = 0; i < tags.size(); i++) {
        AddSymbol(tags[i], false);
    }
    m_results->SetColumnWidth(0, wxLIST_AUTOSIZE);
    m_results->SetColumnWidth(1, wxLIST_AUTOSIZE);
    m_results->SetColumnWidth(2, wxLIST_AUTOSIZE);
    m_results->SetColumnWidth(3, wxLIST_AUTOSIZE);
    m_results->SetColumnWidth(4, wxLIST_AUTOSIZE);

    m_results->SetFocus();
    if (tags.empty() == false) {
        m_results->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        m_results->SetItemState(0, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
        m_tag = m_tags[m_selectedItem = 0];
    }
}
//---------------------------------------------------------------------------------
void SymbolsDialog::UpdateFileAndLine(wxListEvent& event)
{
    wxString line_number;
    m_file = GetColumnText(m_results, event.m_itemIndex, 2);
    line_number = GetColumnText(m_results, event.m_itemIndex, 3);
    m_pattern = GetColumnText(m_results, event.m_itemIndex, 4);
    line_number.ToLong(&m_line);
}
//---------------------------------------------------------------------------------
void SymbolsDialog::OnItemSelected(wxListEvent& event)
{
    UpdateFileAndLine(event);
    m_tag = m_tags[m_selectedItem = event.m_itemIndex];
}
//---------------------------------------------------------------------------------
void SymbolsDialog::OnItemActivated(wxListEvent& event)
{
    UpdateFileAndLine(event);
    EndModal(wxID_OK);
}
//---------------------------------------------------------------------------------
void SymbolsDialog::OnButtonCancel(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_CANCEL);
}
//---------------------------------------------------------------------------------
void SymbolsDialog::OnButtonOK(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_OK);
}
//---------------------------------------------------------------------------------
void SymbolsDialog::OnItemDeselected(wxListEvent& event)
{
    wxUnusedVar(event);
    m_selectedItem = wxNOT_FOUND;
    m_tag = NULL;
}
//---------------------------------------------------------------------------------
void SymbolsDialog::OnCharHook(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_DOWN) {

        if (m_selectedItem == wxNOT_FOUND && m_results->GetItemCount() > 0) {
            m_tag = m_tags[m_selectedItem = 0];
        }

        if (m_selectedItem == wxNOT_FOUND) return;

        if (m_results->GetItemCount() > m_selectedItem + 1) {
            m_results->SetItemState(m_selectedItem, wxLIST_STATE_SELECTED, wxLIST_STATE_DROPHILITED);
            m_tag = m_tags[++m_selectedItem];

            m_results->SetItemState(m_selectedItem, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            m_results->EnsureVisible(m_selectedItem);
            return;
        }
    }
    else if (event.GetKeyCode() == WXK_UP) {
        if (m_selectedItem == wxNOT_FOUND && m_results->GetItemCount() > 0) {
            m_tag = m_tags[m_selectedItem = 0];
        }

        if (m_selectedItem == wxNOT_FOUND) return;

        // select the previous one if we can
        if ((m_selectedItem - 1) >= 0) {
            // we can select the next one
            m_results->SetItemState(m_selectedItem, wxLIST_STATE_SELECTED, wxLIST_STATE_DROPHILITED);
            m_tag = m_tags[--m_selectedItem];

            m_results->SetItemState(m_selectedItem, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            m_results->EnsureVisible(m_selectedItem);
        }
        return;
    }
    event.Skip();
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// NewItemDlg
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
static const wxString FileTypeCpp = wxT("C++ Source File (.cpp)");
static const wxString FileTypeC = wxT("C Source File (.c)");
static const wxString FileTypeHeader = wxT("Header File (.h)");
static const wxString FileTypeAny = wxT("Any File");

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(NewItemDlg, NewItemView)
EVT_CHAR_HOOK(NewItemDlg::OnCharHook)
END_EVENT_TABLE()
//---------------------------------------------------------------------------------
NewItemDlg::NewItemDlg(wxWindow* parent, const wxString & cwd)
    : NewItemView(parent)
{
    m_cwd = cwd;
    m_fileType->InsertColumn(0, _("File Type"));
    m_fileType->SetColumnWidth(0, 300);

    // Initialise images map
    wxImageList *images = new wxImageList(16, 16, true);
    gsgs::ImageManager *bmpLoader = &gsgs_Image();
    images->Add(bmpLoader->LoadBitmap(wxT("mime/16/c")));     //0
    images->Add(bmpLoader->LoadBitmap(wxT("mime/16/cpp")));   //1
    images->Add(bmpLoader->LoadBitmap(wxT("mime/16/h")));     //2
    images->Add(bmpLoader->LoadBitmap(wxT("mime/16/text")));  //3

    m_fileType->AssignImageList(images, wxIMAGE_LIST_SMALL);

    //-----------------------------------
    // Populate the list:
    m_fileTypeValue = FileTypeCpp;

    long row = AppendListCtrlRow(m_fileType);
    SetColumnText(m_fileType, row, 0, FileTypeCpp, 1);

    row = AppendListCtrlRow(m_fileType);
    SetColumnText(m_fileType, row, 0, FileTypeC, 0);

    row = AppendListCtrlRow(m_fileType);
    SetColumnText(m_fileType, row, 0, FileTypeHeader, 2);

    row = AppendListCtrlRow(m_fileType);
    SetColumnText(m_fileType, row, 0, FileTypeAny, 3);
    m_fileTypeValue = FileTypeAny;

    //select the last item
    m_fileType->SetItemState(row, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    m_fileType->SetItemState(row, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);

    m_location->SetValue(m_cwd);
    m_fileName->SetFocus();

    // Attach events
    ConnectEvents();
    SetName("NewItemDlg");
    gsgs::WindowManager::Load(this);
}
//---------------------------------------------------------------------------------
void NewItemDlg::ConnectEvents()
{
    m_cancel->Connect(m_cancel->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewItemDlg::OnClick), NULL, this);
    m_okButton->Connect(m_okButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewItemDlg::OnClick), NULL, this);
    m_browseBtn->Connect(m_browseBtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewItemDlg::OnClick), NULL, this);
    m_fileType->Connect(m_fileType->GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(NewItemDlg::OnListItemSelected), NULL, this);
}

//---------------------------------------------------------------------------------
void NewItemDlg::OnClick(wxCommandEvent &event)
{
    int id = event.GetId();
    if (id == m_okButton->GetId()) {
        DoCreateFile();
    }
    else if (id == m_cancel->GetId()) {
        EndModal(wxID_CANCEL);
    }
    else if (id == m_browseBtn->GetId()) {
        DirSaver ds;
        wxDirDialog *dlg = new wxDirDialog(this, _("Location:"), m_cwd);
        if (dlg->ShowModal() == wxID_OK) {
            m_location->SetValue(dlg->GetPath());
        }
        dlg->Destroy();
    }
}
//---------------------------------------------------------------------------------
void NewItemDlg::DoCreateFile()
{
    wxString errMsg;
    if (!Validate(errMsg)) {
        wxMessageBox(errMsg, _("光栅故事"), wxICON_INFORMATION | wxOK);
        return;
    }

    // Construct the file name
    wxString fileName(m_fileName->GetValue());
    TrimString(fileName);

    if (m_fileTypeValue == FileTypeAny) {
        m_newFileName = wxFileName(m_location->GetValue(), fileName);

    }
    else if (m_fileTypeValue == FileTypeC) {
        // If user already provided suffix, dont add another one on top of it
        if (fileName.Find(wxT(".")) == wxNOT_FOUND) {
            m_newFileName = wxFileName(m_location->GetValue(), fileName, wxT("c"));
        }
        else {
            m_newFileName = wxFileName(m_location->GetValue(), fileName);
        }

    }
    else if (m_fileTypeValue == FileTypeCpp) {
        // If user already provided suffix, dont add another one on top of it
        if (fileName.Find(wxT(".")) == wxNOT_FOUND) {
            m_newFileName = wxFileName(m_location->GetValue(), fileName, wxT("cpp"));
        }
        else {
            m_newFileName = wxFileName(m_location->GetValue(), fileName);
        }

    }
    else if (m_fileTypeValue == FileTypeHeader) {
        // If user already provided suffix, dont add another one on top of it
        if (fileName.Find(wxT(".")) == wxNOT_FOUND) {
            m_newFileName = wxFileName(m_location->GetValue(), fileName, wxT("h"));
        }
        else {
            m_newFileName = wxFileName(m_location->GetValue(), fileName);
        }
    }
    EndModal(wxID_OK);
}
//---------------------------------------------------------------------------------
bool NewItemDlg::Validate(wxString &errMsg)
{
    // make sure we have file name & path set up correctly
    wxFileName fn(m_location->GetValue() + gsgsPathSep);

    if (m_location->GetValue().Trim().IsEmpty()) {
        errMsg = _("Missing location");
        return false;
    }

    fn = wxFileName(m_location->GetValue(), m_fileName->GetValue());
    if (fn.FileExists()) {
        errMsg = _("A file with that name already exists. Please choose a different name");
        return false;
    }

    if (m_fileName->GetValue().Trim().IsEmpty()) {
        errMsg = _("Missing file name");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------------
void NewItemDlg::OnListItemSelected(wxListEvent &event)
{
    m_fileTypeValue = event.GetText();
}
//---------------------------------------------------------------------------------
void NewItemDlg::OnCharHook(wxKeyEvent &event)
{
    if (event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER) {
        DoCreateFile();
    }
    event.Skip();
}
//---------------------------------------------------------------------------------
NewItemDlg::~NewItemDlg()
{

}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// EditConfigurationDialog
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
EditConfigurationDialog::EditConfigurationDialog(wxWindow* parent, const wxString& projectName, int id, wxString title,
    wxPoint pos, wxSize size, int style)
    : EditConfigurationView(parent, id, title, pos, size, style)
    , m_projectName(projectName)
{
    ProjectSettingsPtr settings = gsgs_Manager()->GetProjectSettings(m_projectName);
    if (settings) {//
        ProjectSettingsCookie cookie;
        BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
        while (bldConf) {
            m_configurationsList->Append(bldConf->GetName());
            bldConf = settings->GetNextBuildConfiguration(cookie);
        }
    }
    if (m_configurationsList->GetCount() > 0) m_configurationsList->SetSelection(0);

    CentreOnParent();//
    ConnectListBoxDClick(m_configurationsList, EditConfigurationDialog::OnItemDclick);
    ConnectButton(m_buttonClose, EditConfigurationDialog::OnButtonClose);
    ConnectButton(m_buttonRename, EditConfigurationDialog::OnButtonRename);
    ConnectButton(m_buttonDelete, EditConfigurationDialog::OnButtonDelete);
}
//---------------------------------------------------------------------------------
void EditConfigurationDialog::RenameConfiguration(const wxString& oldName, const wxString& newName)
{
    ProjectSettingsPtr settings = gsgs_Manager()->GetProjectSettings(m_projectName);
    if (settings) {
        BuildConfigPtr bldConf = settings->GetBuildConfiguration(oldName);
        if (bldConf) {
            settings->RemoveConfiguration(oldName);
            bldConf->SetName(newName);
            settings->SetBuildConfiguration(bldConf);
            // save changes
            gsgs_Manager()->SetProjectSettings(m_projectName, settings);

            // update the control
            m_configurationsList->Clear();
            ProjectSettingsCookie cookie;
            BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
            while (bldConf) {
                m_configurationsList->Append(bldConf->GetName());
                bldConf = settings->GetNextBuildConfiguration(cookie);
            }
            if (m_configurationsList->GetCount() > 0) m_configurationsList->SetSelection(0);
        }
    }
}
//---------------------------------------------------------------------------------
void EditConfigurationDialog::OnItemDclick(wxCommandEvent& event)
{
    wxString oldName = event.GetString();
    wxTextEntryDialog* dlg = new wxTextEntryDialog(this, _("Enter New Name:"), _("Rename"), oldName);
    dlg->SetTextValidator(wxFILTER_ASCII);

    if (dlg->ShowModal() == wxID_OK) {
        wxString newName = dlg->GetValue();
        RenameConfiguration(oldName, newName);
    }
}
//---------------------------------------------------------------------------------
void EditConfigurationDialog::OnButtonClose(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_OK);
}
//---------------------------------------------------------------------------------
void EditConfigurationDialog::OnButtonRename(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString oldName = m_configurationsList->GetStringSelection();
    if (oldName.IsEmpty()) { return; }
    wxTextEntryDialog* dlg = new wxTextEntryDialog(this, _("Enter New Name:"), _("Rename"), oldName);
    dlg->SetTextValidator(wxFILTER_ASCII);

    if (dlg->ShowModal() == wxID_OK) {
        wxString newName = dlg->GetValue();
        RenameConfiguration(oldName, newName);
    }
}
//---------------------------------------------------------------------------------
void EditConfigurationDialog::OnButtonDelete(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString selection = m_configurationsList->GetStringSelection();
    if (selection.IsEmpty()) { return; }
    wxString msg(_("Remove configuration '"));
    msg << selection << _("' ?");
    if (wxMessageBox(msg, _("Confirm"), wxYES_NO | wxCANCEL | wxICON_QUESTION) == wxYES) {
        ProjectSettingsPtr settings = gsgs_Manager()->GetProjectSettings(m_projectName);
        if (settings) {
            settings->RemoveConfiguration(selection);
            m_configurationsList->Delete(m_configurationsList->GetSelection());
            if (m_configurationsList->GetCount() > 0) m_configurationsList->SetSelection(0);

            // save changes
            gsgs_Manager()->SetProjectSettings(m_projectName, settings);
        }
    }
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// FreeTextDialog
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
FreeTextDialog::FreeTextDialog(wxWindow* parent, wxString value)
    : FreeTextView(parent)
{
    m_text->SetValue(value);
    m_text->SetFocus();
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// DbgCommandDlg
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
NewConfigurationDlg::NewConfigurationDlg(wxWindow* parent, const wxString& projectName)
    : NewConfigurationView(parent)
    , m_projectName(projectName)
{
    wxTextValidator validator(wxFILTER_ASCII);
    m_textConfigurationName->SetValidator(validator);
    m_textConfigurationName->SetFocus();

    // Get all configuration of the project
    m_choiceCopyConfigurations->Append(_("-- None --"));
    ProjectSettingsPtr settings = gsgs_Manager()->GetProjectSettings(m_projectName);
    if (settings) {
        ProjectSettingsCookie cookie;
        BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
        while (bldConf) {
            m_choiceCopyConfigurations->Append(bldConf->GetName());
            bldConf = settings->GetNextBuildConfiguration(cookie);
        }
    }

    ConnectButton(m_buttonOK, NewConfigurationDlg::OnButtonOK);
    m_buttonOK->SetDefault();
    CentreOnScreen();
    GetSizer()->Fit(this);
    CenterOnParent();
}
//---------------------------------------------------------------------------------
void NewConfigurationDlg::OnButtonOK(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString newConfName;
    newConfName = m_textConfigurationName->GetValue();
    newConfName = newConfName.Trim();
    newConfName = newConfName.Trim(false);
    if (newConfName.IsEmpty()) {
        wxMessageBox(_("Configuration Name is empty"), _("光栅故事"), wxOK | wxICON_INFORMATION);
        return;
    }

    ProjectSettingsPtr settings = gsgs_Manager()->GetProjectSettings(m_projectName);
    wxString copyFrom = m_choiceCopyConfigurations->GetStringSelection();
    BuildConfigPtr newBuildConf;

    if (copyFrom == _("-- None --")) {
        newBuildConf = new BuildConfig(NULL);
    }
    else {
        BuildConfigPtr origBuildConf = settings->GetBuildConfiguration(copyFrom);
        newBuildConf = origBuildConf->Clone();
    }

    newBuildConf->SetName(newConfName);
    settings->SetBuildConfiguration(newBuildConf);
    // save the settings
    gsgs_Manager()->SetProjectSettings(m_projectName, settings);
    EndModal(wxID_OK);
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// RenameFileDlg
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
RenameFileDlg::RenameFileDlg(wxWindow* parent, const wxString& replaceWith, IStatementList& matches)
    : RenameFileView(parent)
{
    m_textCtrlReplaceWith->SetValue(replaceWith);

    for (size_t i = 0; i < matches.size(); i++) {
        wxString displayString;
        IncludeStatement is = matches.at(i);

        displayString << wxString(is.includedFrom.c_str(), wxConvUTF8) << wxT(":") << is.line;
        int idx = m_checkListMatches->Append(displayString);

        // Keep the information about this entry
        m_entries[idx] = is;
        m_checkListMatches->Check(idx);
    }

    if (m_checkListMatches->GetCount()) {
        m_checkListMatches->Select(0);
        DoSelectItem(0);
    }
    SetName("RenameFileDlg");
    gsgs::WindowManager::Load(this);
}
//---------------------------------------------------------------------------------
void RenameFileDlg::OnFileSelected(wxCommandEvent& event) { DoSelectItem(event.GetSelection()); }
//---------------------------------------------------------------------------------
RenameFileDlg::~RenameFileDlg() {}
//---------------------------------------------------------------------------------
void RenameFileDlg::DoSelectItem(int idx)
{
    std::map<int, IncludeStatement>::iterator iter = m_entries.find(idx);
    if (iter != m_entries.end()) {
        IncludeStatement ise = iter->second;
        wxString line;
        line << ise.line;
        m_staticTextFoundInLine->SetLabel(line);

        m_staticTextIncludedInFile->SetLabel(wxString(ise.includedFrom.c_str(), wxConvUTF8));
        m_staticTextPattern->SetLabel(
            wxString::Format(wxT("#include %s"), wxString(ise.pattern.c_str(), wxConvUTF8).c_str()));
    }
}
//---------------------------------------------------------------------------------
IStatementList RenameFileDlg::GetMatches() const
{
    IStatementList matches;
    for (unsigned int i = 0; i < m_checkListMatches->GetCount(); i++) {
        if (m_checkListMatches->IsChecked(i)) {
            std::map<int, IncludeStatement>::const_iterator iter = m_entries.find((int)i);
            if (iter != m_entries.end()) {
                matches.push_back(iter->second);
            }
        }
    }
    return matches;
}
//--------------------------------------------------------------------------------
wxString RenameFileDlg::GetReplaceWith() const { return m_textCtrlReplaceWith->GetValue(); }
//-----------------------------------------------------------------------------

// Declare the bitmap loading function
extern void wxCF3AAInitBitmapResources();

static bool bBitmapLoaded2 = false;
//-----------------------------------------------------------------------------
DebuggerBtImgList::DebuggerBtImgList()
    : wxImageList(16, 16, true)
    , m_imagesWidth(16)
    , m_imagesHeight(16)
{
    if(!bBitmapLoaded2) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCF3AAInitBitmapResources();
        bBitmapLoaded2 = true;
    }

    {
        wxBitmap bmp;
        wxIcon icn;
        bmp = wxXmlResource::Get()->LoadBitmap(wxT("arrowActive"));
        if(bmp.IsOk()) {
            if((m_imagesWidth == bmp.GetWidth()) && (m_imagesHeight == bmp.GetHeight())) {
                icn.CopyFromBitmap(bmp);
                this->Add(icn);
            }
            m_bitmaps.insert(std::make_pair(wxT("arrowActive"), bmp));
        }
    }
}

DebuggerBtImgList::~DebuggerBtImgList() {}

//---------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// DebuggerCallstackView
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DebuggerCallstackView::DebuggerCallstackView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                     long style)
    : wxPanel(parent, id, pos, size, style)
    , m_currLevel(0)
{
    if(!bBitmapLoaded2) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCF3AAInitBitmapResources();
        bBitmapLoaded2 = true;
    }

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(mainSizer);

    m_dvListCtrl = new gsgs::ThemedListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
                                        wxDV_ROW_LINES | wxDV_SINGLE);

    mainSizer->Add(m_dvListCtrl, 1, wxEXPAND, WXC_FROM_DIP(5));

    m_dvListCtrl->AppendIconTextColumn(_("#"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                       wxDATAVIEW_COL_RESIZABLE);
    m_dvListCtrl->AppendTextColumn(_("Function"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                   wxDATAVIEW_COL_RESIZABLE);
    m_dvListCtrl->AppendTextColumn(_("File"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                   wxDATAVIEW_COL_RESIZABLE);
    m_dvListCtrl->AppendTextColumn(_("Line"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                   wxDATAVIEW_COL_RESIZABLE);
    m_dvListCtrl->AppendTextColumn(_("Address"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                   wxDATAVIEW_COL_RESIZABLE);

    // Connect events
    m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
                          wxDataViewEventHandler(DebuggerCallstackView::OnItemActivated), NULL, this);
    m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEventHandler(DebuggerCallstackView::OnMenu),
                          NULL, this);
                          
    m_bitmaps.push_back(m_images.Bitmap("arrowActive"));
    m_bitmaps.push_back(m_images.Bitmap("arrowInactive"));
    m_dvListCtrl->SetBitmaps(&m_bitmaps);

    // The first column contains the frame ID
    // We want the lowest frames first
    m_dvListCtrl->SetSortFunction([](gsgs::RowEntry* a, gsgs::RowEntry* b) {
        const wxString& cellA = a->GetLabel(0);
        const wxString& cellB = b->GetLabel(0);
        long frameIdA = 0, frameIdB = 0;
        cellA.ToCLong(&frameIdA);
        cellB.ToCLong(&frameIdA);
        return frameIdA < frameIdB;
    });
    gsgs_Event().Bind(wxEVT_DEBUGGER_LIST_FRAMES, &DebuggerCallstackView::OnUpdateBacktrace, this);
    gsgs_Event().Bind(wxEVT_DEBUGGER_FRAME_SELECTED, &DebuggerCallstackView::OnFrameSelected, this);
}
//-----------------------------------------------------------------------------
DebuggerCallstackView::~DebuggerCallstackView()
{
    m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
                             wxDataViewEventHandler(DebuggerCallstackView::OnItemActivated), NULL, this);
    m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU,
                             wxDataViewEventHandler(DebuggerCallstackView::OnMenu), NULL, this);
    gsgs_Event().Unbind(wxEVT_DEBUGGER_LIST_FRAMES, &DebuggerCallstackView::OnUpdateBacktrace, this);
    gsgs_Event().Unbind(wxEVT_DEBUGGER_FRAME_SELECTED, &DebuggerCallstackView::OnFrameSelected, this);
}
//-----------------------------------------------------------------------------
void DebuggerCallstackView::OnItemActivated(wxDataViewEvent& event)
{
    int row = m_dvListCtrl->ItemToRow(event.GetItem());

    StackEntry* entry = reinterpret_cast<StackEntry*>(m_dvListCtrl->GetItemData(event.GetItem()));
    if(entry) {
        long frame, frameLine;
        if(!entry->level.ToLong(&frame)) { frame = 0; }

        // Remove the currently selected item
        wxDataViewItem curitem = m_dvListCtrl->RowToItem(m_currLevel);
        if(curitem.IsOk()) {
            wxVariant v;
            v = ::MakeBitmapIndexText(wxString() << m_currLevel, 1);
            m_dvListCtrl->SetValue(v, m_currLevel, 0);
        }

        entry->line.ToLong(&frameLine);
        SetCurrentLevel(row);
        gsgs_Manager()->DbgSetFrame(frame, frameLine);

        // At this point m_currLevel is pointing to the new stack level
        // set it as the active one
        curitem = m_dvListCtrl->RowToItem(m_currLevel);
        if(curitem.IsOk()) {
            wxVariant v;
            v = ::MakeBitmapIndexText(wxString() << m_currLevel, 0);
            m_dvListCtrl->SetValue(v, m_currLevel, 0);
        }
    }
}
//-----------------------------------------------------------------------------
void DebuggerCallstackView::Update(const StackEntryArray& stackArr)
{
    Clear();
    m_stack.insert(m_stack.end(), stackArr.begin(), stackArr.end());
    int activeFrame(-1);
    for(int i = 0; i < (int)m_stack.size(); i++) {
        bool isactive = (i == m_currLevel);
        StackEntry entry = m_stack.at(i);
        wxVector<wxVariant> cols;
        cols.push_back(::MakeBitmapIndexText(entry.level, isactive ? 0 : 1));
        cols.push_back(entry.function);
        cols.push_back(wxFileName(entry.file).GetFullPath());
        cols.push_back(entry.line);
        cols.push_back(entry.address);
        StackEntry* d = new StackEntry(entry);
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr)d);
        if(isactive) { activeFrame = i; }
    }

    // Make sure that everything is drawn before we attempt to select anything
    m_dvListCtrl->Update();
    if(activeFrame != wxNOT_FOUND) { CallAfter(&DebuggerCallstackView::EnsureRowVisible, activeFrame); }
}
//-----------------------------------------------------------------------------
void DebuggerCallstackView::SetCurrentLevel(const int level)
{
    // Set m_currLevel to level, or 0 if level is out of bounds
    m_currLevel = (level >= 0 && level < (int)m_dvListCtrl->GetItemCount()) ? level : 0;
}
//-----------------------------------------------------------------------------
void DebuggerCallstackView::Clear()
{
    m_stack.clear();
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrl->RowToItem(i);
        if(item.IsOk()) {
            StackEntry* entry = reinterpret_cast<StackEntry*>(m_dvListCtrl->GetItemData(item));
            wxDELETE(entry);
        }
    }
    m_dvListCtrl->DeleteAllItems();
}
//-----------------------------------------------------------------------------
void DebuggerCallstackView::OnMenu(wxDataViewEvent& event)
{
    // Popup the menu
    wxMenu menu;

    menu.Append(XRCID("stack_copy_backtrace"), _("Copy Backtrace to Clipboard"));
    menu.Connect(XRCID("stack_copy_backtrace"), wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(DebuggerCallstackView::OnCopyBacktrace), NULL, this);
    m_dvListCtrl->PopupMenu(&menu);
}
//-----------------------------------------------------------------------------
void DebuggerCallstackView::OnCopyBacktrace(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString trace;
    for(size_t i = 0; i < m_stack.size(); ++i) {
        trace << m_stack.at(i).level << wxT("  ") << m_stack.at(i).address << wxT("  ") << m_stack.at(i).function
              << wxT("  ") << m_stack.at(i).file << wxT("  ") << m_stack.at(i).line << wxT("\n");
    }
    trace.RemoveLast();
    ::CopyToClipboard(trace);
}
//-----------------------------------------------------------------------------
void DebuggerCallstackView::OnUpdateBacktrace(gsgs::Event& e)
{
    e.Skip();
    Update(static_cast<DebuggerEventData*>(e.GetClientObject())->m_stack);
}
//-----------------------------------------------------------------------------
void DebuggerCallstackView::OnFrameSelected(gsgs::Event& e)
{
    e.Skip();
    gsgs::IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && dbgr->IsRunning() && gsgs_Manager()->DbgCanInteract()) 
    {
        // set the frame
        dbgr->QueryFileLine();
    }
}
//-----------------------------------------------------------------------------
void DebuggerCallstackView::EnsureRowVisible(int row)
{
    wxDataViewItem item = m_dvListCtrl->RowToItem(row);
    if(item.IsOk()) {
        m_dvListCtrl->Select(item);
        m_dvListCtrl->EnsureVisible(item);
    }
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// EditorFrame
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
wxDEFINE_EVENT(wxEVT_DETACHED_EDITOR_CLOSED, gsgs::Event);
//---------------------------------------------------------------------------------
EditorFrame::EditorFrame(wxWindow* parent, gsgs::EditorScrolledPanel * esp, size_t notebookStyle)
    : EditorFrameBase(parent)
    , m_editor(esp)
{
    m_editor->Reparent(m_mainPanel);
    m_mainPanel->GetSizer()->Add(esp, 1, wxEXPAND);
    gsgs_Style().RegisterWindow(m_mainPanel);
    
    // Notebook::RemovePage hides the detached tab
    if(!esp->IsShown()) { esp->Show(); }

    // Set a find control for this editor
    m_findBar = new gsgs::QuickFindBar(m_mainPanel);
    m_findBar->SetEditor(esp->getEditor());
    m_mainPanel->GetSizer()->Add(m_findBar, 0, wxEXPAND | wxALL, 0);
    m_findBar->Hide();
    m_toolbar->SetDropdownMenu(XRCID("toggle_bookmark"), gsgs_Bookmark().CreateBookmarksSubmenu(NULL));
    
    m_toolbar->SetMiniToolBar(false);
    m_toolbar->AddTool(XRCID("file-save"), _("Save"), gsgs_Image().LoadBitmap("file_save"));
    m_toolbar->AddTool(XRCID("file-close"), _("Close"), gsgs_Image().LoadBitmap("file_close"));
    m_toolbar->AddTool(XRCID("reload_file"), _("Reload"), gsgs_Image().LoadBitmap("file_reload"));
    m_toolbar->AddTool(XRCID("show-find-bar"), _("Find"), gsgs_Image().LoadBitmap("cscope"));
    m_toolbar->AddSpacer();
    m_toolbar->AddTool(wxID_UNDO, _("Undo"), gsgs_Image().LoadBitmap("undo"));
    m_toolbar->AddTool(wxID_REDO, _("Redo"), gsgs_Image().LoadBitmap("redo"));
    m_toolbar->Realize();
    m_toolbar->Bind(wxEVT_TOOL, &EditorFrame::OnSave, this, XRCID("file-save"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &EditorFrame::OnSaveUI, this, XRCID("file-save"));
    m_toolbar->Bind(wxEVT_TOOL, &EditorFrame::OnClose, this, XRCID("file-close"));
    m_toolbar->Bind(wxEVT_TOOL, &EditorFrame::OnEdit, this, wxID_UNDO);
    m_toolbar->Bind(wxEVT_TOOL, &EditorFrame::OnEdit, this, wxID_REDO);
    m_toolbar->Bind(wxEVT_TOOL, &EditorFrame::OnFind, this, XRCID("show-find-bar"));
    m_toolbar->Bind(wxEVT_TOOL, &EditorFrame::OnReload, this, XRCID("reload_file"));
    
    m_mainPanel->Layout();
    SetTitle(esp->getEditor()->GetFileName().GetFullPath());
    SetSize(600, 600);
    CentreOnScreen();
}
//---------------------------------------------------------------------------------
EditorFrame::~EditorFrame() { DoCloseEditor(m_editor); }
//---------------------------------------------------------------------------------
void EditorFrame::OnClose(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Destroy();
}
//---------------------------------------------------------------------------------
void EditorFrame::OnCloseUI(wxUpdateUIEvent& event) { event.Enable(true); }
//---------------------------------------------------------------------------------
void EditorFrame::OnFind(wxCommandEvent& event)
{
    if(!m_findBar->IsShown()) {
        m_findBar->Show();
        m_mainPanel->GetSizer()->Layout();
    }
}
//---------------------------------------------------------------------------------
void EditorFrame::OnFindUI(wxUpdateUIEvent& event) { event.Enable(true); }
//---------------------------------------------------------------------------------
void EditorFrame::OnCloseWindow(wxCloseEvent& event) { event.Skip(); }
//---------------------------------------------------------------------------------
void EditorFrame::DoCloseEditor(gsgs::EditorScrolledPanel * esp)
{
    gsgs::Event evntInternalClosed(wxEVT_DETACHED_EDITOR_CLOSED);
    evntInternalClosed.SetClientData(esp);
    evntInternalClosed.SetFileName(esp->getEditor()->GetFileName().GetFullPath());
    gsgs_Event().ProcessEvent(evntInternalClosed);

    // Send the traditional plugin event notifying that this editor is about to be destroyed
    wxCommandEvent eventClose(wxEVT_EDITOR_CLOSING);
    eventClose.SetClientData(esp->getEditor());
    gsgs_Event().ProcessEvent(eventClose);
}
//---------------------------------------------------------------------------------
void EditorFrame::OnEdit(wxCommandEvent& event) { m_editor->getEditor()->OnMenuCommand(event); }
//---------------------------------------------------------------------------------
void EditorFrame::OnSave(wxCommandEvent& event)
{
    m_editor->getEditor()->SaveFile();
    SetTitle(m_editor->getEditor()->GetFileName().GetFullPath());
}
//---------------------------------------------------------------------------------
void EditorFrame::OnSaveUI(wxUpdateUIEvent& event) { event.Enable(m_editor && m_editor->getEditor()->GetModify()); }
//---------------------------------------------------------------------------------
void EditorFrame::OnUndo(wxCommandEvent& event) { m_editor->getEditor()->GetCtrl()->Undo(); }
//---------------------------------------------------------------------------------
void EditorFrame::OnRedo(wxCommandEvent& event) { m_editor->getEditor()->GetCtrl()->Redo(); }
//---------------------------------------------------------------------------------
void EditorFrame::OnUndoUI(wxUpdateUIEvent& event) { event.Enable(m_editor && m_editor->getEditor()->CanUndo()); }
//---------------------------------------------------------------------------------
void EditorFrame::OnRedoUI(wxUpdateUIEvent& event) { event.Enable(m_editor && m_editor->getEditor()->CanRedo()); }
//---------------------------------------------------------------------------------
void EditorFrame::OnReload(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_editor->getEditor()->ReloadFromDisk();
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// ImplParentVirtualFunctionsData
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
class ImplParentVirtualFunctionsData : public ObjectData
{
	size_t m_flags;
public:
	enum {
		InsertDoxygenComment   = 0x00000001,
		FormatText             = 0x00000002,
		PrependVirtual         = 0x00000004,
		ImplParentVFuncDefault = InsertDoxygenComment | FormatText
	};

public:
	ImplParentVirtualFunctionsData()
			: m_flags(ImplParentVFuncDefault) {
	}

	virtual ~ImplParentVirtualFunctionsData() {
	}

public:
	virtual void DeSerialize(Serializer &arch) {
		arch.Read(wxT("m_flags"), m_flags);
	}

	virtual void Serialize(Serializer &arch) {
		arch.Write(wxT("m_flags"), m_flags);
	}

	void SetFlags(const size_t& flags) {
		this->m_flags = flags;
	}

	const size_t& GetFlags() const {
		return m_flags;
	}
};
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// ImplementParentVirtualFunctionsDialog
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
ImplementParentVirtualFunctionsDialog::ImplementParentVirtualFunctionsDialog(wxWindow* parent,
                                                                             const wxString& scopeName,
                                                                             const std::vector<TagEntryPtr>& tags,
                                                                             wxChar doxyPrefix, gsgs::ContextCpp* contextCpp, wxWindowID id,
                                                                         const wxString& title, const wxPoint& pos,
                                                                         const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
    , m_tags(tags)
    , m_doxyPrefix(doxyPrefix)
    , m_contextCpp(contextCpp)
    , m_scope(scopeName)
{
wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(mainSizer);

    m_banner4 = new wxBannerWindow(this, wxID_ANY, wxBOTTOM, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
                                   wxBORDER_THEME);
    m_banner4->SetBitmap(wxNullBitmap);
    m_banner4->SetText(_("Implement inherited virtual functions"),
                       _("Select from the list below the functions that you want to override in your class"));
    m_banner4->SetGradient(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION),
                           wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
    m_banner4->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));

    mainSizer->Add(m_banner4, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* bSizer4 = new wxBoxSizer(wxHORIZONTAL);

    mainSizer->Add(bSizer4, 0, wxTOP | wxBOTTOM | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText2 =
        gsgs_Style().createStaticText(this, wxID_ANY, _("File:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    bSizer4->Add(m_staticText2, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textCtrlImplFile =
        gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_textCtrlImplFile->SetToolTip(_("Generate the functions in this filename"));

    bSizer4->Add(m_textCtrlImplFile, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* boxSizer6 = new wxBoxSizer(wxHORIZONTAL);

    mainSizer->Add(boxSizer6, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_dvListCtrl = new gsgs::ThemedListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
                                        wxDV_ROW_LINES | wxDV_SINGLE);

    boxSizer6->Add(m_dvListCtrl, 1, wxEXPAND, WXC_FROM_DIP(5));

    m_dvListCtrl->AppendTextColumn(_("Function"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                   wxDATAVIEW_COL_RESIZABLE);
    m_dvListCtrl->AppendTextColumn(_("Visibility"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                   wxDATAVIEW_COL_RESIZABLE);
    m_dvListCtrl->AppendTextColumn(_("Virtual"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                   wxDATAVIEW_COL_RESIZABLE);
    m_dvListCtrl->AppendTextColumn(_("Document"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                   wxDATAVIEW_COL_RESIZABLE);
    wxBoxSizer* boxSizer8 = new wxBoxSizer(wxVERTICAL);

    boxSizer6->Add(boxSizer8, 0, wxEXPAND, WXC_FROM_DIP(5));

    m_button10 = new Button(this, wxID_ANY, _("Check all"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    boxSizer8->Add(m_button10, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, WXC_FROM_DIP(5));

    m_button12 = new Button(this, wxID_ANY, _("Uncheck all"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    boxSizer8->Add(m_button12, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxStaticBoxSizer* sbSizer1 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Options:")), wxVERTICAL);

    mainSizer->Add(sbSizer1, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_checkBoxFormat = new wxCheckBox(this, wxID_ANY, _("Format text after insertion"), wxDefaultPosition,
                                      wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_checkBoxFormat->SetValue(false);

    sbSizer1->Add(m_checkBoxFormat, 0, wxALL, WXC_FROM_DIP(5));

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    mainSizer->Add(buttonSizer, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

    m_buttonOk = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonOk->SetDefault();

    buttonSizer->Add(m_buttonOk, 0, wxALL, WXC_FROM_DIP(5));

    m_buttonCancel =
        new Button(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    buttonSizer->Add(m_buttonCancel, 0, wxALL, WXC_FROM_DIP(5));

    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }

    // Connect events
    m_button10->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                        wxCommandEventHandler(ImplementParentVirtualFunctionsDialog::OnCheckAll), NULL, this);
    m_button12->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                        wxCommandEventHandler(ImplementParentVirtualFunctionsDialog::OnUnCheckAll), NULL, this);

    ImplParentVirtualFunctionsData data;
    gsgs_EditorConfig().ReadObject(wxT("ImplParentVirtualFunctionsData"), &data);
    m_checkBoxFormat->SetValue(data.GetFlags() & ImplParentVirtualFunctionsData::FormatText);
    DoInitialize(false);
}
// -----------------------------------------------------------------------------------------
ImplementParentVirtualFunctionsDialog::~ImplementParentVirtualFunctionsDialog()
{
    m_button10->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
                           wxCommandEventHandler(ImplementParentVirtualFunctionsDialog::OnCheckAll), NULL, this);
    m_button12->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
                           wxCommandEventHandler(ImplementParentVirtualFunctionsDialog::OnUnCheckAll), NULL, this);
                           
    ImplParentVirtualFunctionsData data;
    size_t flags(0);
    if(m_checkBoxFormat->IsChecked()) { flags |= ImplParentVirtualFunctionsData::FormatText; }
    data.SetFlags(flags);
    gsgs_EditorConfig().WriteObject(wxT("ImplParentVirtualFunctionsData"), &data);
    Clear();
}
// -----------------------------------------------------------------------------------------
void ImplementParentVirtualFunctionsDialog::DoInitialize(bool updateDoxyOnly)
{
    Clear();
    wxVector<wxVariant> cols;

    // Add declration
    //////////////////////////////////////////////////////
    for(size_t i = 0; i < m_tags.size(); ++i) {
        cols.clear();
        cols.push_back(::MakeCheckboxVariant(m_tags.at(i)->GetDisplayName(), true, wxNOT_FOUND)); // generate it, 0
        cols.push_back("public");                                                                 // visibility, 1
        cols.push_back(true);                                                                     // virtual, 2
        cols.push_back(false);                                                                    // document, 3
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr)i);
    }
}
// -----------------------------------------------------------------------------------------
wxString ImplementParentVirtualFunctionsDialog::GetDecl(const wxString& visibility)
{
    wxString decl;

    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        FunctionImplDetails details;
        UpdateDetailsForRow(details, i); // Update the values according to the user choices
        if(details.GetVisibility() == visibility) { decl << details.GetDecl(this); }
    }
    return decl;
}
// -----------------------------------------------------------------------------------------
wxString ImplementParentVirtualFunctionsDialog::GetImpl()
{
    wxString impl;
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        FunctionImplDetails details;
        UpdateDetailsForRow(details, i); // Update the values according to the user choices
        impl << details.GetImpl(this);
    }
    return impl;
}
// -----------------------------------------------------------------------------------------
wxString ImplementParentVirtualFunctionsDialog::DoMakeCommentForTag(TagEntryPtr tag) const
{
    // Add doxygen comment
    CommentConfigData data;
    gsgs_EditorConfig().ReadObject(wxT("CommentConfigData"), &data);

    CppCommentCreator commentCreator(tag, m_doxyPrefix);
    DoxygenComment dc;
    dc.comment = commentCreator.CreateComment();
    dc.name = tag->GetName();
    m_contextCpp->DoMakeDoxyCommentString(dc, data.GetCommentBlockPrefix(), m_doxyPrefix);

    // Format the comment
    wxString textComment = dc.comment;
    textComment.Replace("\r", "\n");
    wxArrayString lines = wxStringTokenize(textComment, "\n", wxTOKEN_STRTOK);
    textComment.Clear();

    for(size_t i = 0; i < lines.GetCount(); ++i)
        textComment << lines.Item(i) << wxT("\n");
    return textComment;
}
// -----------------------------------------------------------------------------------------
void ImplementParentVirtualFunctionsDialog::SetTargetFile(const wxString& file)
{
    m_textCtrlImplFile->ChangeValue(file);
}
// -----------------------------------------------------------------------------------------
void ImplementParentVirtualFunctionsDialog::UpdateDetailsForRow(FunctionImplDetails& details, size_t row)
{
    wxDataViewItem item = m_dvListCtrl->RowToItem(row);
    details.SetSelected(m_dvListCtrl->IsItemChecked(item, 0));
    details.SetVisibility(m_dvListCtrl->GetItemText(item, 1));
    details.SetPrependVirtualKeyword(m_dvListCtrl->IsItemChecked(item, 2));
    details.SetDoxygen(m_dvListCtrl->IsItemChecked(item, 3));
    details.SetTagIndex(m_dvListCtrl->GetItemData(item));
}
// -----------------------------------------------------------------------------------------
void ImplementParentVirtualFunctionsDialog::OnCheckAll(wxCommandEvent& event) { DoCheckAll(true); }
// -----------------------------------------------------------------------------------------
void ImplementParentVirtualFunctionsDialog::OnUnCheckAll(wxCommandEvent& event) { DoCheckAll(false); }
// -----------------------------------------------------------------------------------------
void ImplementParentVirtualFunctionsDialog::Clear() { m_dvListCtrl->DeleteAllItems(); }
// -----------------------------------------------------------------------------------------
TagEntryPtr ImplementParentVirtualFunctionsDialog::GetAssociatedTag(const wxDataViewItem& item) const
{
    int index = (int)m_dvListCtrl->GetItemData(item);
    return GetTag(index);
}
// -----------------------------------------------------------------------------------------
TagEntryPtr ImplementParentVirtualFunctionsDialog::GetAssociatedTag(size_t row) const
{
    return GetAssociatedTag(m_dvListCtrl->RowToItem(row));
}
// -----------------------------------------------------------------------------------------
void ImplementParentVirtualFunctionsDialog::DoCheckAll(bool check)
{
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        m_dvListCtrl->SetItemChecked(m_dvListCtrl->RowToItem(i), check, 0);
    }
}
// -----------------------------------------------------------------------------------------
TagEntryPtr ImplementParentVirtualFunctionsDialog::GetTag(size_t index) const
{
    if(index >= m_tags.size()) { return TagEntryPtr(NULL); }
    return m_tags[index];
}
// -----------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------
// FunctionImplDetails
// -----------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------
wxString FunctionImplDetails::GetImpl(ImplementParentVirtualFunctionsDialog* dlg) const
{
    if(!IsSelected()) { return ""; };

    TagEntryPtr tag = dlg->GetTag(m_tagIndex);
    if(!tag) { return ""; }

    wxString impl;
    tag->SetScope(dlg->m_scope);
    impl << TagsManagerST::Get()->FormatFunction(tag, FunctionFormat_Impl) << wxT("\n");
    impl.Trim().Trim(false);
    impl << "\n\n";

    return impl;
}
// -----------------------------------------------------------------------------------------
wxString FunctionImplDetails::GetDecl(ImplementParentVirtualFunctionsDialog* dlg) const
{
    if(!IsSelected()) { return ""; };

    TagEntryPtr tag = dlg->GetTag(m_tagIndex);
    if(!tag) { return ""; }

    wxString decl;
    if(IsDoxygen()) { decl << dlg->DoMakeCommentForTag(tag); }

    tag->SetScope(dlg->m_scope);
    decl << TagsManagerST::Get()->FormatFunction(tag, IsPrependVirtualKeyword() ? FunctionFormat_WithVirtual : 0);
    decl.Trim().Trim(false);
    decl << "\n";
    return decl;
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// TagsParserSearchPathsDlg
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
TagsParserSearchPathsDlg::TagsParserSearchPathsDlg(wxWindow* parent, const wxArrayString& paths, const wxArrayString& excludePaths, wxWindowID id,
    const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : 
    wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText2 = gsgs_Style().createStaticText( this, wxID_ANY, _("The following include paths were detected on your system and will be added to your parser search path.\nYou may remove a path by unchecking it.\n\nYou can always add/remove paths to the parser from the main menu:\nSettings > Tags Settings > Parser"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	sbSizer1->Add( m_staticText2, 0, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	wxArrayString m_checkListPathsChoices;
	m_checkListPaths = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListPathsChoices, 0 );
	mainSizer->Add( m_checkListPaths, 1, wxALL|wxEXPAND, 5 );
	
	wxArrayString m_checkListExcludePathsChoices;
	m_checkListExcludePaths = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListExcludePathsChoices, 0 );
	mainSizer->Add( m_checkListExcludePaths, 1, wxALL|wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new Button( this, wxID_OK, _("&Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	buttonSizer->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new Button( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxALL, 5 );
	
	mainSizer->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->Centre( wxBOTH );
    
    m_checkListPaths->Append(paths);
    for(unsigned int i = 0; i < m_checkListPaths->GetCount(); i++) {
        m_checkListPaths->Check(i, true);
    }

    m_checkListExcludePaths->Append(excludePaths);
    for(unsigned int i = 0; i < m_checkListExcludePaths->GetCount(); i++) {
        m_checkListExcludePaths->Check(i, true);
    }
}
//---------------------------------------------------------------------------------
wxArrayString TagsParserSearchPathsDlg::GetSearchPaths() const
{
    wxArrayString paths;
    for(unsigned int i = 0; i < m_checkListPaths->GetCount(); i++) {
        if(m_checkListPaths->IsChecked(i)) {
            paths.Add(m_checkListPaths->GetString(i));
        }
    }
    return paths;
}
//---------------------------------------------------------------------------------
TagsParserSearchPathsDlg::~TagsParserSearchPathsDlg() {}
//---------------------------------------------------------------------------------
wxArrayString TagsParserSearchPathsDlg::GetExcludePath() const
{
    wxArrayString paths;
    for(unsigned int i = 0; i < m_checkListExcludePaths->GetCount(); i++) {
        if(m_checkListExcludePaths->IsChecked(i)) {
            paths.Add(m_checkListExcludePaths->GetString(i));
        }
    }
    return paths;
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// TasksFindWhatDlg
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
class TaskData : public wxClientData
{
public:
    wxString m_regex;

public:
    TaskData(const wxString& regex)
        : m_regex(regex)
    {
    }
    virtual ~TaskData() {}
};
//---------------------------------------------------------------------------------
TasksFindWhatDlg::TasksFindWhatDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_list = new gsgs::ListCtrlAdvance(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES|wxLC_VRULES);
	bSizer3->Add( m_list, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonNew = new Button( this, wxID_ANY, _("New"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonNew, 0, wxALL, 5 );
	
	m_buttonEdit = new Button( this, wxID_ANY, _("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonEdit, 0, wxALL, 5 );
	
	m_buttonDelete = new Button( this, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonDelete, 0, wxALL, 5 );
	
	bSizer3->Add( bSizer4, 0, wxEXPAND, 5 );
	
	mainSizer->Add( bSizer3, 1, wxEXPAND|wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new Button( this, wxID_OK, _("&Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	buttonSizer->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new Button( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxALL, 5 );
	
	mainSizer->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_buttonNew->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TasksFindWhatDlg::OnNewTask ), NULL, this );
	m_buttonEdit->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TasksFindWhatDlg::OnEditTask ), NULL, this );
	m_buttonEdit->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TasksFindWhatDlg::OnEditTaskUI ), NULL, this );
	m_buttonDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TasksFindWhatDlg::OnDeleteTask ), NULL, this );
	m_buttonDelete->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TasksFindWhatDlg::OnDeleteTaskUI ), NULL, this );
	m_buttonOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TasksFindWhatDlg::OnButtonOk ), NULL, this );
    
    m_list->InsertColumn(0, _("Enabled"));
    m_list->InsertColumn(1, _("Task"));
    m_list->SetColumnWidth(1, 200);

    // Load all info from disk
    TasksPanelData data;
    gsgs_EditorConfig().ReadObject(wxT("TasksPanelData"), &data);

    gsgsStringMap::const_iterator iter = data.GetTasks().begin();
    for(; iter != data.GetTasks().end(); iter++) {
        DoAddLine(iter->first, iter->second, data.GetEnabledItems().Index(iter->first) != wxNOT_FOUND);
    }
}
//---------------------------------------------------------------------------------
TasksFindWhatDlg::~TasksFindWhatDlg() 
{
	// Disconnect Events
	m_buttonNew->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TasksFindWhatDlg::OnNewTask ), NULL, this );
	m_buttonEdit->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TasksFindWhatDlg::OnEditTask ), NULL, this );
	m_buttonEdit->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TasksFindWhatDlg::OnEditTaskUI ), NULL, this );
	m_buttonDelete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TasksFindWhatDlg::OnDeleteTask ), NULL, this );
	m_buttonDelete->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TasksFindWhatDlg::OnDeleteTaskUI ), NULL, this );
	m_buttonOk->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TasksFindWhatDlg::OnButtonOk ), NULL, this );
}
//---------------------------------------------------------------------------------
void TasksFindWhatDlg::OnNewTask(wxCommandEvent& event)
{
    NewTaskDialog dlg(this);
    gsgs::WindowManager::Load(this);

    dlg.SetLabel(_("New Task"));
    if(dlg.ShowModal() == wxID_OK) {
        wxRegEx re(dlg.m_regex->GetValue());
        if(re.IsValid() == false) {
            wxMessageBox(wxString::Format(_("'%s' is not a valid regular expression"), dlg.m_regex->GetValue().c_str()),
                         _("光栅故事"),
                         wxICON_WARNING | wxOK);
            return;
        }
        DoAddLine(dlg.m_name->GetValue(), dlg.m_regex->GetValue(), true);
    }
}
//---------------------------------------------------------------------------------
void TasksFindWhatDlg::OnDeleteTask(wxCommandEvent& event)
{
    int selection = m_list->GetSelection();
    if(selection == wxNOT_FOUND) return;

    int answer =
        wxMessageBox(_("Are you sure you want to delete this entry?"), _("Confirm"), wxICON_QUESTION | wxYES_NO);
    if(answer == wxYES) {
        m_list->DeleteItem(selection);
    }
}
//---------------------------------------------------------------------------------
void TasksFindWhatDlg::OnDeleteTaskUI(wxUpdateUIEvent& event)
{
    event.Enable(m_list->GetItemCount() && m_list->GetSelection() != wxNOT_FOUND);
}
//---------------------------------------------------------------------------------
void TasksFindWhatDlg::OnEditTask(wxCommandEvent& event)
{
    int selection = m_list->GetSelection();
    if(selection != wxNOT_FOUND) {

        NewTaskDialog dlg(this);
        gsgs::WindowManager::Load(this);

        dlg.SetLabel(_("Edit Task"));
        dlg.m_name->SetValue(m_list->GetText(selection, 1));

        TaskData* data = (TaskData*)m_list->GetItemData(selection);
        dlg.m_regex->SetValue(data->m_regex);
        if(dlg.ShowModal() == wxID_OK) {
            m_list->SetTextColumn(selection, 1, dlg.m_name->GetValue());
            m_list->SetItemClientData(selection, new TaskData(dlg.m_regex->GetValue()));
        }
    }
}
//---------------------------------------------------------------------------------
void TasksFindWhatDlg::OnEditTaskUI(wxUpdateUIEvent& event)
{
    event.Enable(m_list->GetItemCount() && m_list->GetSelection() != wxNOT_FOUND);
}
//---------------------------------------------------------------------------------
void TasksFindWhatDlg::DoAddLine(const wxString& name, const wxString& regex, bool enabled)
{
    long item = m_list->AppendRow();
    m_list->SetCheckboxRow(item, enabled);
    m_list->SetTextColumn(item, 1, name);
    m_list->SetItemClientData(item, new TaskData(regex));
}
//---------------------------------------------------------------------------------
void TasksFindWhatDlg::DoSaveList()
{
    // Save all items
    TasksPanelData data;
    gsgsStringMap items;
    wxArrayString enabledItems;

    for(int i = 0; i < m_list->GetItemCount(); i++) {
        wxString name = m_list->GetText(i, 1);
        TaskData* clientData = (TaskData*)m_list->GetItemData(i);
        items[name] = clientData->m_regex;

        if(m_list->IsChecked(i)) enabledItems.Add(name);
    }

    data.SetEnabledItems(enabledItems);
    data.SetTasks(items);
    gsgs_EditorConfig().WriteObject(wxT("TasksPanelData"), &data);
}
//---------------------------------------------------------------------------------
void TasksFindWhatDlg::OnButtonOk(wxCommandEvent& event)
{
    DoSaveList();
    event.Skip();
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// ManagePerspectivesDlg
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
ManagePerspectivesDlg::ManagePerspectivesDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
    : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBox = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer3->Add( m_listBox, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_button3 = new Button( this, wxID_DELETE, _("&Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button3, 0, wxALL|wxEXPAND, 5 );
	
	m_button4 = new Button( this, wxID_RENAME, _("&Rename"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button4, 0, wxALL|wxEXPAND, 5 );
	
	bSizer3->Add( bSizer4, 0, wxEXPAND, 5 );
	
	mainSizer->Add( bSizer3, 1, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new Button( this, wxID_OK, _("&Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	buttonSizer->Add( m_buttonOk, 0, wxRIGHT|wxLEFT, 5 );
	
	m_buttonCancel = new Button( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxRIGHT|wxLEFT, 5 );
	
	mainSizer->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( mainSizer );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ManagePerspectivesDlg::OnDeletePerspective ), NULL, this );
	m_button3->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ManagePerspectivesDlg::OnDeletePerspectiveUI ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ManagePerspectivesDlg::OnRenamePerspective ), NULL, this );
	m_button4->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ManagePerspectivesDlg::OnRenamePerspectiveUI ), NULL, this );
    m_listBox->Clear();
    m_listBox->Append(gsgs_Manager()->GetPerspectiveManager().GetAllPerspectives());
}
//---------------------------------------------------------------------------------
ManagePerspectivesDlg::~ManagePerspectivesDlg() 
{
	// Disconnect Events
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ManagePerspectivesDlg::OnDeletePerspective ), NULL, this );
	m_button3->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ManagePerspectivesDlg::OnDeletePerspectiveUI ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ManagePerspectivesDlg::OnRenamePerspective ), NULL, this );
	m_button4->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ManagePerspectivesDlg::OnRenamePerspectiveUI ), NULL, this );
}
//---------------------------------------------------------------------------------
void ManagePerspectivesDlg::OnDeletePerspective(wxCommandEvent& event)
{
    wxString sel = m_listBox->GetStringSelection();
    if(sel.IsEmpty()) return;

    if(::wxMessageBox(wxString::Format(_("Are you sure you want to delete perspective '%s'?"), sel.c_str()),
                      wxT("gsgs"),
                      wxYES_NO | wxICON_QUESTION) == wxYES) {
        gsgs_Manager()->GetPerspectiveManager().Delete(sel);
        DoPopulateList();
    }
}
//---------------------------------------------------------------------------------
void ManagePerspectivesDlg::OnDeletePerspectiveUI(wxUpdateUIEvent& event)
{
    event.Enable(m_listBox->GetSelection() != wxNOT_FOUND);
}
//---------------------------------------------------------------------------------
void ManagePerspectivesDlg::OnRenamePerspective(wxCommandEvent& event)
{
    wxString sel = m_listBox->GetStringSelection();
    if(sel.IsEmpty()) return;

    wxString newName = ::wxGetTextFromUser(_("Enter new name:"), _("Rename perspective"));
    if(newName.IsEmpty()) return;

    gsgs_Manager()->GetPerspectiveManager().Rename(sel, newName);
    DoPopulateList();
}
//---------------------------------------------------------------------------------
void ManagePerspectivesDlg::OnRenamePerspectiveUI(wxUpdateUIEvent& event)
{
    event.Enable(m_listBox->GetSelection() != wxNOT_FOUND);
}
//---------------------------------------------------------------------------------
void ManagePerspectivesDlg::DoPopulateList()
{
    // Refresh the list
    m_listBox->Clear();
    m_listBox->Append(gsgs_Manager()->GetPerspectiveManager().GetAllPerspectives());
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// NewVersionDlg
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
NewVersionDlg::NewVersionDlg( wxWindow* parent , wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
		:wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticText = gsgs_Style().createStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText->Wrap( -1 );
	mainSizer->Add( m_staticText, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_hyperlink1 = new wxHyperlinkCtrl( this, wxID_ANY, _("Show me whats new !"), wxT("http://www.wxformbuilder.org"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	mainSizer->Add( m_hyperlink1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new Button( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	bSizer2->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new Button( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonCancel, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_buttonOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewVersionDlg::OnOk ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewVersionDlg::OnCancel ), NULL, this );
	m_buttonOk->SetFocus();
}
//---------------------------------------------------------------------------------
NewVersionDlg::~NewVersionDlg()
{
    // Disconnect Events
	m_buttonOk->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewVersionDlg::OnOk ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewVersionDlg::OnCancel ), NULL, this );
}
//---------------------------------------------------------------------------------
void NewVersionDlg::OnOk( wxCommandEvent& event )
{
	wxUnusedVar( event );
	EndModal(wxID_OK);
}
//---------------------------------------------------------------------------------
void NewVersionDlg::OnCancel( wxCommandEvent& event )
{
	wxUnusedVar( event );
	EndModal(wxID_CANCEL);
}
//---------------------------------------------------------------------------------
void NewVersionDlg::SetMessage(const wxString& message)
{
	m_staticText->SetLabel(message);
	GetSizer()->Fit(this);
	GetSizer()->Layout();
}
//---------------------------------------------------------------------------------
void NewVersionDlg::SetReleaseNotesURL(const wxString& url)
{
	m_hyperlink1->SetURL(url);
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// SavePerspectiveDlg
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
SavePerspectiveDlg::SavePerspectiveDlg( wxWindow* parent, wxWindowID id, const wxString& title, 
    const wxPoint& pos, const wxSize& size, long style )
	: wxDialog( parent, id, title, pos, size, style )
{
    	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = gsgs_Style().createStaticText( this, wxID_ANY, _("Save the current layout as:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer5->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_comboBox = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer5->Add( m_comboBox, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer5->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button5 = new Button( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button5->SetDefault(); 
	bSizer6->Add( m_button5, 0, wxALL, 5 );
	
	m_button6 = new Button( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_button6, 0, wxALL, 5 );
	
	bSizer5->Add( bSizer6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
	bSizer5->Fit( this );
	
	this->Centre( wxBOTH );
	m_comboBox->SetFocus();
	
	m_comboBox->Append(gsgs_Manager()->GetPerspectiveManager().GetAllPerspectives());
	m_comboBox->SetValue(gsgs_Manager()->GetPerspectiveManager().GetActive());
}

wxString SavePerspectiveDlg::GetPerspectiveName() const
{
	return m_comboBox->GetValue();
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------