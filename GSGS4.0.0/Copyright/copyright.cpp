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
#include "copyright.h"
#include "copyrights_options_dlg.h"
#include "copyrights_proj_sel_dlg.h"
#include "cpptoken.h"
#include "cppwordscanner.h"
#include "globals.h"
#include "gsgsProgressDialog.h"
#include "project.h"
#include <wx/app.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/progdlg.h>
#include <wx/tokenzr.h>
#include <wx/xrc/xmlres.h>

#include "copyrightsconfigdata.h"
#include "gsgsEvent.h"
#include "gsgsLogManager.h"
#include "globals.h"

static Copyright* thePlugin = NULL;

// Internal events used by this plugin
const wxEventType CR_copyrights_options = wxNewEventType();
const wxEventType CR_insert_copyrights = wxNewEventType();
const wxEventType CR_insert_prj_copyrights = wxNewEventType();

// Define the plugin entry point
CL_PLUGIN_API Plugin * CreatePlugin(IManager * manager)
{
    if(thePlugin == 0) 
    { 
        thePlugin = new Copyright(manager); 
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo * GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT(""));
    info.SetName(wxT("Copyright mark"));
    info.SetDescription( _("Copyright Mark Plugin"));
    info.SetVersion(wxT("v2.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() 
{
    return PLUGIN_INTERFACE_VERSION; 
}

Copyright::Copyright(IManager* manager)
    : Plugin(manager)
    , m_projectSepItem(NULL)
    , m_workspaceSepItem(NULL)
{
    m_longName = _("Copyright Mark Plugin");
    m_shortName = wxT("Copyright Mark");

    // connect events
    wxTheApp->Bind(wxEVT_MENU, &Copyright::OnOptions, this, XRCID("CR_copyrights_options"));
    wxTheApp->Bind(wxEVT_MENU, &Copyright::OnInsertCopyrights, this, XRCID("CR_insert_copyrights"));
    wxTheApp->Bind(wxEVT_MENU, &Copyright::OnBatchInsertCopyrights, this, XRCID("CR_batch_insert_copyrights"));
    wxTheApp->Bind(wxEVT_MENU, &Copyright::OnProjectInsertCopyrights, this, XRCID("CR_insert_prj_copyrights"));
    gsgs_Event().Bind(wxEVT_CONTEXT_MENU_EDITOR, &Copyright::OnEditorContextMenu, this);
}

Copyright::~Copyright() {}

void Copyright::CreateToolBar(gsgs::ToolBar* toolbar) 
{ 
    wxUnusedVar(toolbar); 
}

void Copyright::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("CR_insert_copyrights"), _("Insert Copyright Block"), _("Insert Copyright Block"),
                          wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("CR_batch_insert_copyrights"), _("Batch Insert of Copyright Block"),
                          _("Batch Insert of Copyright Block"), wxITEM_NORMAL);
    menu->Append(item);

    menu->AppendSeparator();
    item = new wxMenuItem(menu, XRCID("CR_copyrights_options"), _("Settings..."), wxEmptyString, wxITEM_NORMAL);

    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("Copyrights"), menu);
}

void Copyright::UnPlug()
{
    wxTheApp->Unbind(wxEVT_MENU, &Copyright::OnOptions, this, XRCID("CR_copyrights_options"));
    wxTheApp->Unbind(wxEVT_MENU, &Copyright::OnInsertCopyrights, this, XRCID("CR_insert_copyrights"));
    wxTheApp->Unbind(wxEVT_MENU, &Copyright::OnBatchInsertCopyrights, this, XRCID("CR_batch_insert_copyrights"));
    wxTheApp->Unbind(wxEVT_MENU, &Copyright::OnProjectInsertCopyrights, this, XRCID("CR_insert_prj_copyrights"));
    gsgs_Event().Unbind(wxEVT_CONTEXT_MENU_EDITOR, &Copyright::OnEditorContextMenu, this);
}

void Copyright::OnInsertCopyrights(wxCommandEvent& e)
{
    wxUnusedVar(e);

    // read configuration
    CopyrightsConfigData data;
    m_mgr->GetConfigTool()->ReadObject(wxT("CopyrightsConfig"), &data);

    // make sure that the template file exists
    if(!wxFileName::FileExists(data.GetTemplateFilename())) {
        wxMessageBox(
            wxString::Format(_("Template file name '%s', does not exist!"), data.GetTemplateFilename().GetData()),
            _("光栅故事"), wxICON_WARNING | wxOK);
        return;
    }

    // read the copyrights file
    wxString content;
    if(!ReadFileWithConversion(data.GetTemplateFilename(), content)) {
        wxMessageBox(wxString::Format(_("Failed to read template file '%s'"), data.GetTemplateFilename().c_str()),
                     _("光栅故事"), wxICON_WARNING | wxOK);
        return;
    }

    gsgs::IEditor* editor = m_mgr->GetActiveEditor();
    if(!editor) {
        wxMessageBox(wxString::Format(_("There is no active editor\n")), _("光栅故事"), wxICON_WARNING | wxOK);
        return;
    }

    // verify that the file consist only with comment code
    CppWordScanner scanner(data.GetTemplateFilename().mb_str().data());

    CppTokensMap l;
    scanner.FindAll(l);

    if(!l.is_empty()) {
        if(wxMessageBox(_("Template file contains text which is not comment, continue anyway?"), _("光栅故事"),
                        wxICON_QUESTION | wxYES_NO) == wxNO) {
            return;
        }
    }

    // expand constants
    wxString _content = ExpandAllVariables(content, m_mgr->GetWorkspace(), wxEmptyString, wxEmptyString,
                                           editor->GetFileName().GetFullPath());

    // we are good to go :)
    wxString ignoreString = data.GetIgnoreString();
    ignoreString = ignoreString.Trim().Trim(false);

    if(ignoreString.IsEmpty() == false) {
        if(editor->GetEditorText().Find(data.GetIgnoreString()) != wxNOT_FOUND) {
            gsgsLogMessage(_("File contains ignore string, skipping it"));
            return;
        }
    }
    editor->InsertText(0, _content);
}

void Copyright::OnOptions(wxCommandEvent& e)
{
    // pop up the options dialog
    CopyrightsOptionsDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr->GetConfigTool());
    dlg.ShowModal();
}

void Copyright::OnBatchInsertCopyrights(wxCommandEvent& e)
{
    // pop up the projects selection dialog
    if(m_mgr->IsOpen() == false) 
    {
        wxMessageBox(_("Batch insert requires a workspace to be opened"), _("光栅故事"), wxICON_WARNING | wxOK);
        return;
    }

    if(!m_mgr->SaveAll()) return;

    // read configuration
    CopyrightsConfigData data;
    m_mgr->GetConfigTool()->ReadObject(wxT("CopyrightsConfig"), &data);

    wxString content;
    if(!Validate(content)) { return; }

    CopyrightsProjectSelDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr->GetWorkspace());
    if(dlg.ShowModal() == wxID_OK) {
        wxArrayString projects;
        dlg.GetProjects(projects);

        // expand constants
        std::vector<wxFileName> files;
        std::vector<wxFileName> filtered_files;
        // loop over the project and collect list of files to work with
        for(size_t i = 0; i < projects.size(); i++) {
            ProjectPtr p = m_mgr->GetWorkspace()->GetProject(projects.Item(i));
            if(p) { p->GetFilesAsVectorOfFileName(files); }
        }

        wxString mask(data.GetFileMasking());
        mask.Replace(wxT("*."), wxEmptyString);
        mask = mask.Trim().Trim(false);

        wxArrayString exts = ::wxStringTokenize(mask, wxT(";"));

        // filter out non-matching files (according to masking)
        for(size_t i = 0; i < files.size(); i++) {
            if(exts.Index(files.at(i).GetExt(), false) != wxNOT_FOUND) {
                // valid file
                filtered_files.push_back(files.at(i));
            }
        }

        if(filtered_files.empty() == false) { MassUpdate(filtered_files, content); }
    }
}

void Copyright::OnProjectInsertCopyrights(wxCommandEvent& e)
{
    // pop up the projects selection dialog
    if(m_mgr->IsOpen() == false) {
        wxMessageBox(_("Batch insert requires a workspace to be opened"), _("光栅故事"), wxICON_WARNING | wxOK);
        return;
    }

    if(!m_mgr->SaveAll()) return;

    // read configuration
    CopyrightsConfigData data;
    m_mgr->GetConfigTool()->ReadObject(wxT("CopyrightsConfig"), &data);

    wxString content;
    if(!Validate(content)) { return; }

    // get the project to work on
    TreeItemInfo info = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
    wxString project_name = info.m_text;

    wxString err_msg;
    std::vector<wxFileName> files;
    std::vector<wxFileName> filtered_files;
    // loop over the project and collect list of files to work with
    ProjectPtr p = m_mgr->GetWorkspace()->GetProject(project_name);
    if(!p) { return; }

    p->GetFilesAsVectorOfFileName(files);

    // filter non matched files
    wxString mask(data.GetFileMasking());
    mask.Replace(wxT("*."), wxEmptyString);
    mask = mask.Trim().Trim(false);

    wxArrayString exts = wxStringTokenize(mask, wxT(";"));

    // filter out non-matching files (according to masking)
    for(size_t i = 0; i < files.size(); i++) {
        if(exts.Index(files.at(i).GetExt(), false) != wxNOT_FOUND) {
            // valid file
            filtered_files.push_back(files.at(i));
        }
    }

    // update files
    if(filtered_files.empty() == false) 
    { 
        MassUpdate(filtered_files, content); 
    }
}

void Copyright::MassUpdate(const std::vector<wxFileName>& filtered_files, const wxString& content)
{
    // last confirmation from the user
    if(wxMessageBox(
           wxString::Format(_("You are about to modify %u files. Continue?"), (unsigned int)filtered_files.size()),
           _("光栅故事"), wxYES_NO | wxICON_QUESTION) == wxNO) {
        return;
    }

    gsgs::ProgressDlg* prgDlg = NULL;
    prgDlg = new gsgs::ProgressDlg(NULL, _("Processing file ..."), wxT(""), (int)filtered_files.size());

    CopyrightsConfigData data;
    m_mgr->GetConfigTool()->ReadObject(wxT("CopyrightsConfig"), &data);

    // now loop over the files and add copyrights block
    for(size_t i = 0; i < filtered_files.size(); i++) {
        wxFileName fn = filtered_files.at(i);

        wxString file_content;
        wxString _content = ExpandAllVariables(content, m_mgr->GetWorkspace(), wxEmptyString, wxEmptyString, fn.GetFullPath());
        if(ReadFileWithConversion(fn.GetFullPath(), file_content)) {

            wxString msg;

            // if the file contains the ignore string, skip this file
            wxString ignoreString = data.GetIgnoreString();
            ignoreString = ignoreString.Trim().Trim(false);

            if(ignoreString.IsEmpty() == false && file_content.Find(data.GetIgnoreString()) != wxNOT_FOUND) {
                msg << _("File contains ignore string, skipping it: ") << fn.GetFullName();
                if(!prgDlg->Update(i, msg)) {
                    prgDlg->Destroy();
                    return;
                }
            } else {

                msg << _("Inserting comment to file: ") << fn.GetFullName();
                if(!prgDlg->Update(i, msg)) {
                    prgDlg->Destroy();
                    return;
                }

                file_content.Prepend(_content);
                WriteFileWithBackup(fn.GetFullPath(), file_content, data.GetBackupFiles());
            }
        }
    }
    prgDlg->Destroy();
}

bool Copyright::Validate(wxString& content)
{
    CopyrightsConfigData data;
    m_mgr->GetConfigTool()->ReadObject(wxT("CopyrightsConfig"), &data);

    // make sure that the template file exists
    if(!wxFileName::FileExists(data.GetTemplateFilename())) {
        wxMessageBox(
            wxString::Format(_("Template file name '%s', does not exist!"), data.GetTemplateFilename().GetData()),
            _("光栅故事"), wxICON_WARNING | wxOK);
        return false;
    }

    // read the copyrights file
    if(!ReadFileWithConversion(data.GetTemplateFilename(), content)) {
        wxMessageBox(wxString::Format(_("Failed to read template file '%s'"), data.GetTemplateFilename().c_str()),
                     _("光栅故事"), wxICON_WARNING | wxOK);
        return false;
    }

    // verify that the file consist only with comment code
    CppWordScanner scanner(data.GetTemplateFilename().mb_str().data());

    CppTokensMap l;
    scanner.FindAll(l);

    if(!l.is_empty()) {
        if(wxMessageBox(_("Template file contains text which is not comment, continue anyways?"), _("光栅故事"),
                        wxICON_QUESTION | wxYES_NO) == wxNO) {
            return false;
        }
    }
    content.Replace(wxT("`"), wxT("'"));
    return true;
}

void Copyright::OnEditorContextMenu(ContextMenuEvent& event)
{
    event.Skip();
    event.GetMenu()->Append(XRCID("CR_insert_copyrights"), _("Insert Copyrights Block"));
}
