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

#include "SFTPBrowserDlg.h"
#include "SFTPSettingsDialog.h"
#include "SFTPStatusPage.h"
#include "SFTPTreeView.h"
#include "SSHAccountManagerDlg.h"
#include "gsgsEvent.h"
#include "gsgsWindowManager.h"
#include "gsgsLogManager.h"
#include "gsgsFileUtil.h"
#include "gsgsJSON.h"
#include "sftp.h"
#include "sftp_settings.h"
#include "sftp_worker_thread.h"
#include <algorithm>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>

SFTPWorkspaceSettings::SFTPWorkspaceSettings()
    : gsgs::ObjectData("sftp-workspace-settings")
{
}

SFTPWorkspaceSettings::~SFTPWorkspaceSettings()
{
}

void SFTPWorkspaceSettings::FromJSON(const JSONObject& json)
{
    m_remoteWorkspacePath = json.getObject("m_remoteWorkspacePath").getString();
    m_account = json.getObject("m_account").getString();
}

JSONObject SFTPWorkspaceSettings::ToJSON() const
{
    JSONObject element = JSONObject::createObject(GetName());
    element.addProperty("m_account", m_account);
    element.addProperty("m_remoteWorkspacePath", m_remoteWorkspacePath);
    return element;
}

void SFTPWorkspaceSettings::Load(SFTPWorkspaceSettings& settings, const wxFileName& workspaceFile)
{
    wxFileName fn(workspaceFile);
    fn.SetName("sftp-workspace-settings");
    fn.SetExt("conf");
    fn.AppendDir(".gsgsworkspace");
    gsgs::Config config( fn.GetFullPath() );
    config.ReadItem( &settings );
}

void SFTPWorkspaceSettings::Save(const SFTPWorkspaceSettings& settings, const wxFileName& workspaceFile)
{
    wxFileName fn(workspaceFile);
    fn.SetName("sftp-workspace-settings");
    fn.SetExt("conf");
    fn.AppendDir(".gsgsworkspace");
    gsgs::Config config( fn.GetFullPath() );
    config.WriteItem( &settings );
}

void SFTPWorkspaceSettings::Clear()
{
    m_account.Clear();
    m_remoteWorkspacePath.Clear();
}

static SFTPPlugin* thePlugin = NULL;
const wxEventType wxEVT_SFTP_OPEN_SSH_ACCOUNT_MANAGER = ::wxNewEventType();
const wxEventType wxEVT_SFTP_SETTINGS = ::wxNewEventType();
const wxEventType wxEVT_SFTP_SETUP_WORKSPACE_MIRRORING = ::wxNewEventType();
const wxEventType wxEVT_SFTP_DISABLE_WORKSPACE_MIRRORING = ::wxNewEventType();

// Exposed API (via events)
// SFTP plugin provides SFTP functionality for codelite based on events
// It uses the event type gsgs::Event to accept requests from codelite's code
// the SFTP uses the event GetString() method to read a string in the form of JSON format
// For example, to instruct the plugin to connect over SSH to a remote server and save a remote file:
// the GetString() should retrun this JSON string:
//  {
//      account : "account-name-to-use",
//      local_file : "/path/to/local/file",
//      remote_file : "/path/to/remote/file",
//  }

// Define the plugin entry point
CL_PLUGIN_API Plugin* CreatePlugin(gsgs::IManager* manager)
{
    if(thePlugin == 0) { thePlugin = new SFTPPlugin(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT(""));
    info.SetName(wxT("SFTP"));
    info.SetDescription(_("SFTP plugin for gsgs ide"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

SFTPPlugin::SFTPPlugin(IManager* manager)
    : gsgs::Plugin(manager)
{
    m_longName = _("SFTP plugin for codelite IDE");
    m_shortName = wxT("SFTP");

    wxTheApp->Connect(wxEVT_SFTP_OPEN_SSH_ACCOUNT_MANAGER, wxEVT_MENU, wxCommandEventHandler(SFTPPlugin::OnAccountManager),
                      NULL, this);
    wxTheApp->Connect(wxEVT_SFTP_SETTINGS, wxEVT_MENU, wxCommandEventHandler(SFTPPlugin::OnSettings), NULL, this);
    wxTheApp->Connect(wxEVT_SFTP_SETUP_WORKSPACE_MIRRORING, wxEVT_MENU,
                      wxCommandEventHandler(SFTPPlugin::OnSetupWorkspaceMirroring), NULL, this);
    wxTheApp->Connect(wxEVT_SFTP_DISABLE_WORKSPACE_MIRRORING, wxEVT_MENU,
                      wxCommandEventHandler(SFTPPlugin::OnDisableWorkspaceMirroring), NULL, this);
    wxTheApp->Connect(wxEVT_SFTP_DISABLE_WORKSPACE_MIRRORING, wxEVT_UPDATE_UI,
                      wxUpdateUIEventHandler(SFTPPlugin::OnDisableWorkspaceMirroringUI), NULL, this);

    gsgs_Event().Bind(wxEVT_WORKSPACE_LOADED, &SFTPPlugin::OnWorkspaceOpened, this);
    gsgs_Event().Bind(wxEVT_WORKSPACE_CLOSED, &SFTPPlugin::OnWorkspaceClosed, this);
    gsgs_Event().Bind(wxEVT_FILE_SAVED, &SFTPPlugin::OnFileSaved, this);
    gsgs_Event().Bind(wxEVT_FILE_RENAMED, &SFTPPlugin::OnFileRenamed, this);
    gsgs_Event().Bind(wxEVT_FILE_DELETED, &SFTPPlugin::OnFileDeleted, this);
    gsgs_Event().Bind(wxEVT_FILES_MODIFIED_REPLACE_IN_FILES, &SFTPPlugin::OnReplaceInFiles, this);
    gsgs_Event().Bind(wxEVT_EDITOR_CLOSING, &SFTPPlugin::OnEditorClosed, this);

    // API support
    gsgs_Event().Bind(wxEVT_SFTP_SAVE_FILE, &SFTPPlugin::OnSaveFile, this);
    gsgs_Event().Bind(wxEVT_SFTP_RENAME_FILE, &SFTPPlugin::OnRenameFile, this);
    gsgs_Event().Bind(wxEVT_SFTP_DELETE_FILE, &SFTPPlugin::OnDeleteFile, this);

    // Add the "SFTP" page to the workspace pane
    Notebook* book = m_mgr->GetWorkspacePaneNotebook();
    if(IsPaneDetached(_("SFTP"))) {
        // Make the window child of the main panel (which is the grand parent of the notebook)
        DockablePane* cp =
            new DockablePane(book->GetParent()->GetParent(), book, _("SFTP"), false, wxNullBitmap, wxSize(200, 200));
        m_treeView = new SFTPTreeView(cp, this);
        cp->SetChildNoReparent(m_treeView);

    } else {
        m_treeView = new SFTPTreeView(book, this);
        book->AddPage(m_treeView, _("SFTP"), false);
    }

    // Add the "SFTP Log" page to the output pane
    book = m_mgr->GetOutputPaneNotebook();
    wxBitmap bmp = m_mgr->GetStdIcons()->LoadBitmap("upload");
    if(IsPaneDetached(_("SFTP Log"))) {
        // Make the window child of the main panel (which is the grand parent of the notebook)
        DockablePane* cp =
            new DockablePane(book->GetParent()->GetParent(), book, _("SFTP Log"), false, bmp, wxSize(200, 200));
        m_outputPane = new SFTPStatusPage(cp, this);
        cp->SetChildNoReparent(m_outputPane);

    } else {
        m_outputPane = new SFTPStatusPage(book, this);
        book->AddPage(m_outputPane, _("SFTP Log"), false, bmp);
    }

    // Create the helper for adding our tabs in the "more" menu
    m_tabToggler.reset(new gsgs::TabTogglerHelper(_("SFTP Log"), m_outputPane, _("SFTP"), m_treeView));
    m_tabToggler->SetOutputTabBmp(m_mgr->GetStdIcons()->LoadBitmap("remote-folder"));

    SFTPWorkerThread::Instance()->SetNotifyWindow(m_outputPane);
    SFTPWorkerThread::Instance()->SetSftpPlugin(this);
    SFTPWorkerThread::Instance()->Start();
}

SFTPPlugin::~SFTPPlugin() {}

void SFTPPlugin::CreateToolBar(gsgs::ToolBar* toolbar) { wxUnusedVar(toolbar); }

void SFTPPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, wxEVT_SFTP_OPEN_SSH_ACCOUNT_MANAGER, _("Open SSH Account Manager"),
                          _("Open SSH Account Manager"), wxITEM_NORMAL);
    menu->Append(item);
    menu->AppendSeparator();
    item = new wxMenuItem(menu, wxEVT_SFTP_SETTINGS, _("Settings..."), _("Settings..."), wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("SFTPPlugin"), menu);
}

void SFTPPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    if(type == MenuTypeFileView_Workspace) {
        // Create the popup menu for the virtual folders
        wxMenuItem* item(NULL);

        wxMenu* sftpMenu = new wxMenu();
        item = new wxMenuItem(sftpMenu, wxEVT_SFTP_SETUP_WORKSPACE_MIRRORING, _("&Setup..."), wxEmptyString,
                              wxITEM_NORMAL);
        sftpMenu->Append(item);

        item = new wxMenuItem(sftpMenu, wxEVT_SFTP_DISABLE_WORKSPACE_MIRRORING, _("&Disable"), wxEmptyString,
                              wxITEM_NORMAL);
        sftpMenu->Append(item);

        item = new wxMenuItem(menu, wxID_SEPARATOR);
        menu->Prepend(item);
        menu->Prepend(wxID_ANY, _("Workspace Mirroring"), sftpMenu);
    }
}

bool SFTPPlugin::IsPaneDetached(const wxString& name) const
{
    DetachedPanesInfo dpi;
    m_mgr->GetConfigTool()->ReadObject(wxT("DetachedPanesList"), &dpi);
    const wxArrayString& detachedPanes = dpi.GetPanes();
    return detachedPanes.Index(name) != wxNOT_FOUND;
}

void SFTPPlugin::UnPlug()
{
    // Find our page and release it
    // before this plugin is un-plugged we must remove the tab we added
    for(size_t i = 0; i < m_mgr->GetOutputPaneNotebook()->GetPageCount(); ++i) {
        if(m_outputPane == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
            m_mgr->GetOutputPaneNotebook()->RemovePage(i);
            break;
        }
    }
    m_outputPane->Destroy();

    for(size_t i = 0; i < m_mgr->GetWorkspacePaneNotebook()->GetPageCount(); ++i) {
        if(m_treeView == m_mgr->GetWorkspacePaneNotebook()->GetPage(i)) {
            m_mgr->GetWorkspacePaneNotebook()->RemovePage(i);
            break;
        }
    }
    m_treeView->Destroy();

    SFTPWorkerThread::Release();
    wxTheApp->Disconnect(wxEVT_SFTP_OPEN_SSH_ACCOUNT_MANAGER, wxEVT_MENU, wxCommandEventHandler(SFTPPlugin::OnAccountManager),
                         NULL, this);
    wxTheApp->Disconnect(wxEVT_SFTP_SETTINGS, wxEVT_MENU, wxCommandEventHandler(SFTPPlugin::OnSettings), NULL, this);
    wxTheApp->Disconnect(wxEVT_SFTP_SETUP_WORKSPACE_MIRRORING, wxEVT_MENU,
                         wxCommandEventHandler(SFTPPlugin::OnSetupWorkspaceMirroring), NULL, this);
    wxTheApp->Disconnect(wxEVT_SFTP_DISABLE_WORKSPACE_MIRRORING, wxEVT_MENU,
                         wxCommandEventHandler(SFTPPlugin::OnDisableWorkspaceMirroring), NULL, this);
    wxTheApp->Disconnect(wxEVT_SFTP_DISABLE_WORKSPACE_MIRRORING, wxEVT_UPDATE_UI,
                         wxUpdateUIEventHandler(SFTPPlugin::OnDisableWorkspaceMirroringUI), NULL, this);

    gsgs_Event().Unbind(wxEVT_WORKSPACE_LOADED, &SFTPPlugin::OnWorkspaceOpened, this);
    gsgs_Event().Unbind(wxEVT_WORKSPACE_CLOSED, &SFTPPlugin::OnWorkspaceClosed, this);
    gsgs_Event().Unbind(wxEVT_FILE_SAVED, &SFTPPlugin::OnFileSaved, this);
    gsgs_Event().Unbind(wxEVT_FILE_RENAMED, &SFTPPlugin::OnFileRenamed, this);
    gsgs_Event().Unbind(wxEVT_FILE_DELETED, &SFTPPlugin::OnFileDeleted, this);
    gsgs_Event().Unbind(wxEVT_FILES_MODIFIED_REPLACE_IN_FILES, &SFTPPlugin::OnReplaceInFiles, this);
    gsgs_Event().Unbind(wxEVT_EDITOR_CLOSING, &SFTPPlugin::OnEditorClosed, this);

    gsgs_Event().Unbind(wxEVT_SFTP_SAVE_FILE, &SFTPPlugin::OnSaveFile, this);
    gsgs_Event().Unbind(wxEVT_SFTP_RENAME_FILE, &SFTPPlugin::OnRenameFile, this);
    gsgs_Event().Unbind(wxEVT_SFTP_DELETE_FILE, &SFTPPlugin::OnDeleteFile, this);

    m_tabToggler.reset(NULL);

    // Delete the temporary files
    wxFileName::Rmdir(gsgs::SFTP::GetDefaultDownloadFolder(), wxPATH_RMDIR_RECURSIVE);
}

void SFTPPlugin::OnAccountManager(wxCommandEvent& e)
{
    wxUnusedVar(e);
    SSHAccountManagerDlg dlg(wxTheApp->GetTopWindow());
    if(dlg.ShowModal() == wxID_OK) {

        SFTPSettings settings;
        settings.Load();
        settings.SetAccounts(dlg.GetAccounts());
        settings.Save();
    }
}

void SFTPPlugin::OnSetupWorkspaceMirroring(wxCommandEvent& e)
{
    SFTPBrowserDlg dlg(wxTheApp->GetTopWindow(), _("Select the remote workspace"), "*.workspace");
    dlg.Initialize(m_workspaceSettings.GetAccount(), m_workspaceSettings.GetRemoteWorkspacePath());
    if(dlg.ShowModal() == wxID_OK) {
        m_workspaceSettings.SetRemoteWorkspacePath(dlg.GetPath());
        m_workspaceSettings.SetAccount(dlg.GetAccount());
        SFTPWorkspaceSettings::Save(m_workspaceSettings, m_workspaceFile);
    }
}

void SFTPPlugin::OnWorkspaceOpened(wxCommandEvent& e)
{
    e.Skip();
    m_workspaceFile = e.GetString();
    SFTPWorkspaceSettings::Load(m_workspaceSettings, m_workspaceFile);
}

void SFTPPlugin::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    m_workspaceFile.Clear();
    m_workspaceSettings.Clear();
}

void SFTPPlugin::OnFileSaved(gsgs::Event& e)
{
    e.Skip();

    // --------------------------------------
    // Sanity
    // --------------------------------------
    wxString local_file = e.GetString();
    local_file.Trim().Trim(false);
    DoFileSaved(local_file);
}

void SFTPPlugin::OnFileWriteOK(const wxString& message) { gsgsLogMessage(message); }

void SFTPPlugin::OnFileWriteError(const wxString& errorMessage) { gsgsLogMessage(errorMessage); }

void SFTPPlugin::OnDisableWorkspaceMirroring(wxCommandEvent& e)
{
    m_workspaceSettings.Clear();
    SFTPWorkspaceSettings::Save(m_workspaceSettings, m_workspaceFile);
}

void SFTPPlugin::OnDisableWorkspaceMirroringUI(wxUpdateUIEvent& e)
{
    e.Enable(m_workspaceFile.IsOk() && m_workspaceSettings.IsOk());
}

void SFTPPlugin::OnSaveFile(gsgs::SFTPEvent& e)
{
    SFTPSettings settings;
    settings.Load();

    wxString accName = e.GetAccount();
    wxString localFile = e.GetLocalFile();
    wxString remoteFile = e.GetRemoteFile();

    SSHAccountInfo account;
    if(settings.GetAccount(accName, account)) {
        SFTPWorkerThread::Instance()->Add(new SFTPThreadRequet(account, remoteFile, localFile, 0));

    } else {
        wxString msg;
        msg << _("Failed to synchronize file '") << localFile << "'\n"
            << _("with remote server\n") << _("Could not locate account: ") << accName;
        ::wxMessageBox(msg, _("SFTPPlugin"), wxOK | wxICON_ERROR);
    }
}

void SFTPPlugin::DoSaveRemoteFile(const RemoteFileInfo& remoteFile)
{
    SFTPWorkerThread::Instance()->Add(new SFTPThreadRequet(remoteFile.GetAccount(), remoteFile.GetRemoteFile(),
                                                           remoteFile.GetLocalFile(), remoteFile.GetPremissions()));
}

void SFTPPlugin::FileDownloadedSuccessfully(const SFTPClientData& cd)
{
    wxString tooltip;
    tooltip << "Local: " << cd.GetLocalPath() << "\n"
            << "Remote: " << cd.GetRemotePath();

    const wxBitmap & bmp = m_mgr->GetStdIcons()->LoadBitmap("download");
    IEditor* editor = m_mgr->OpenFile(cd.GetLocalPath(), bmp, tooltip);
    if(editor) {
        // Tag this editor as a remote file
        SFTPClientData* pcd = new SFTPClientData(cd);
        editor->SetClientData("sftp", pcd);
        // set the line number
        if(cd.GetLineNumber() != wxNOT_FOUND) { editor->GetCtrl()->GotoLine(cd.GetLineNumber()); }
    }

    // Now that the file was downloaded, update the file permissions
    if(m_remoteFiles.count(cd.GetLocalPath())) {
        RemoteFileInfo& info = m_remoteFiles[cd.GetLocalPath()];
        info.SetPremissions(cd.GetPermissions());
    }
}

void SFTPPlugin::OpenWithDefaultApp(const wxString& localFileName) { ::wxLaunchDefaultApplication(localFileName); }

void SFTPPlugin::AddRemoteFile(const RemoteFileInfo& remoteFile)
{
    if(m_remoteFiles.count(remoteFile.GetLocalFile())) { m_remoteFiles.erase(remoteFile.GetLocalFile()); }
    m_remoteFiles.insert(std::make_pair(remoteFile.GetLocalFile(), remoteFile));
}

void SFTPPlugin::OnEditorClosed(wxCommandEvent& e)
{
    e.Skip();
    IEditor* editor = (IEditor*)e.GetClientData();
    if(editor) {
        wxString localFile = editor->GetFileName().GetFullPath();
        if(m_remoteFiles.count(localFile)) {

            wxLogNull noLog;

            // Remove the file from our cache
            gsgsRemoveFile(localFile);
            m_remoteFiles.erase(localFile);
        }
    }
}

void SFTPPlugin::MSWInitiateConnection()
{
#ifdef __WXMSW__
    // Under Windows, there seems to be a small problem with the connection establishment
    // only the first connection seems to be unstable (i.e. it takes up to 30 seconds to create it)
    // to workaround this, we initiate a dummy connection to the first connection we can find
    SFTPSettings settings;
    settings.Load();
    const SSHAccountInfo::Vect_t& accounts = settings.GetAccounts();
    if(accounts.empty()) return;
    const SSHAccountInfo& account = accounts.at(0);
    SFTPWorkerThread::Instance()->Add(new SFTPThreadRequet(account));
#endif
}

void SFTPPlugin::OnSettings(wxCommandEvent& e)
{
    // Show the SFTPPlugin settings dialog
    SFTPSettingsDialog dlg(gsgs_Event().TopFrame());
    dlg.ShowModal();
}

void SFTPPlugin::DoFileSaved(const wxString& filename)
{
    if(filename.IsEmpty()) return;

    // Check to see if this file is part of a remote files managed by our plugin
    if(m_remoteFiles.count(filename)) {
        // ----------------------------------------------------------------------------------------------
        // this file was opened by the SFTPPlugin explorer
        // ----------------------------------------------------------------------------------------------
        DoSaveRemoteFile(m_remoteFiles.find(filename)->second);

    } else {
        // ----------------------------------------------------------------------------------------------
        // Not a remote file, see if have a sychronization setup between this workspace and a remote one
        // ----------------------------------------------------------------------------------------------

        wxString remoteFile = GetRemotePath(filename);
        if(remoteFile.IsEmpty()) { return; }

        SFTPSettings settings;
        settings.Load();

        SSHAccountInfo account;
        if(settings.GetAccount(m_workspaceSettings.GetAccount(), account)) {
            SFTPWorkerThread::Instance()->Add(new SFTPThreadRequet(account, remoteFile, filename, 0));

        } else {

            wxString msg;
            msg << _("Failed to synchronize file '") << filename << "'\n"
                << _("with remote server\n") << _("Could not locate account: ") << m_workspaceSettings.GetAccount();
            ::wxMessageBox(msg, _("SFTPPlugin"), wxOK | wxICON_ERROR);

            // Disable the workspace mirroring for this workspace
            m_workspaceSettings.Clear();
            SFTPWorkspaceSettings::Save(m_workspaceSettings, m_workspaceFile);
        }
    }
}

void SFTPPlugin::OnReplaceInFiles(gsgs::FileEvent& e)
{
    e.Skip();
    const wxArrayString& files = e.GetStrings();
    for(size_t i = 0; i < files.size(); ++i) {
        DoFileSaved(files.Item(i));
    }
}

void SFTPPlugin::OpenContainingFolder(const wxString& localFileName) { FileUtil::OpenFileExplorerAndSelect(localFileName); }

void SFTPPlugin::OnFileRenamed(gsgs::FileEvent& e)
{
    e.Skip();

    // Convert local paths to remote paths
    wxString remoteFile = GetRemotePath(e.GetPath());
    wxString remoteNew = GetRemotePath(e.GetNewpath());
    if(remoteFile.IsEmpty() || remoteNew.IsEmpty()) { return; }

    SFTPSettings settings;
    settings.Load();

    SSHAccountInfo account;
    if(settings.GetAccount(m_workspaceSettings.GetAccount(), account)) {
        gsgsDEBUG() << "SFTPPlugin: Renaming remote file:" << remoteFile << "->" << remoteNew;
        SFTPWorkerThread::Instance()->Add(new SFTPThreadRequet(account, remoteFile, remoteNew));

    } else {

        wxString msg;
        msg << _("Failed to rename file '") << e.GetPath() << "'\n"
            << _("with remote server\n") << _("Could not locate account: ") << m_workspaceSettings.GetAccount();
        ::wxMessageBox(msg, _("SFTPPlugin"), wxOK | wxICON_ERROR);

        // Disable the workspace mirroring for this workspace
        m_workspaceSettings.Clear();
        SFTPWorkspaceSettings::Save(m_workspaceSettings, m_workspaceFile);
    }
}

void SFTPPlugin::OnFileDeleted(gsgs::FileEvent& e)
{
    e.Skip();
    const wxArrayString& files = e.GetPaths();
    for(size_t i = 0; i < files.size(); ++i) {
        DoFileDeleted(files.Item(i));
    }
}

bool SFTPPlugin::IsCxxWorkspaceMirrorEnabled() const { return m_workspaceFile.IsOk() && m_workspaceSettings.IsOk(); }

void SFTPPlugin::OnRenameFile(gsgs::SFTPEvent& e)
{
    SFTPSettings settings;
    settings.Load();

    wxString accName = e.GetAccount();
    wxString remoteOld = e.GetRemoteFile();
    wxString remoteNew = e.GetNewRemoteFile();

    SSHAccountInfo account;
    if(settings.GetAccount(accName, account)) {
        SFTPWorkerThread::Instance()->Add(new SFTPThreadRequet(account, remoteOld, remoteNew));

    } else {
        wxString msg;
        msg << _("Failed to rename file '") << remoteOld << "'\n"
            << _("with remote server\n") << _("Could not locate account: ") << accName;
        ::wxMessageBox(msg, _("SFTPPlugin"), wxOK | wxICON_ERROR);
    }
}

void SFTPPlugin::OnDeleteFile(gsgs::SFTPEvent& e)
{
    SFTPSettings settings;
    settings.Load();

    wxString accName = e.GetAccount();
    wxString path = e.GetRemoteFile();

    SSHAccountInfo account;
    if(settings.GetAccount(accName, account)) {
        SFTPWorkerThread::Instance()->Add(new SFTPThreadRequet(account, path));

    } else {
        wxString msg;
        msg << _("Failed to delete remote file '") << path << _("'\nCould not locate account: ") << accName;
        ::wxMessageBox(msg, _("SFTPPlugin"), wxOK | wxICON_ERROR);
    }
}

void SFTPPlugin::DoFileDeleted(const wxString& filepath)
{
    wxString remoteFile = GetRemotePath(filepath);
    if(remoteFile.IsEmpty()) { return; }

    SFTPSettings settings;
    settings.Load();

    SSHAccountInfo account;
    if(settings.GetAccount(m_workspaceSettings.GetAccount(), account)) {
        SFTPWorkerThread::Instance()->Add(new SFTPThreadRequet(account, remoteFile));

    } else {

        wxString msg;
        msg << _("Failed to delete remote file: ") << remoteFile << "'\n"
            << _("Could not locate account: ") << m_workspaceSettings.GetAccount();
        ::wxMessageBox(msg, _("SFTPPlugin"), wxOK | wxICON_ERROR);

        // Disable the workspace mirroring for this workspace
        m_workspaceSettings.Clear();
        SFTPWorkspaceSettings::Save(m_workspaceSettings, m_workspaceFile);
    }
}

wxString SFTPPlugin::GetRemotePath(const wxString& localpath) const
{
    if(!IsCxxWorkspaceMirrorEnabled()) { return ""; }
    wxFileName file(localpath);
    file.MakeRelativeTo(m_workspaceFile.GetPath());
    file.MakeAbsolute(wxFileName(m_workspaceSettings.GetRemoteWorkspacePath(), wxPATH_UNIX).GetPath());
    return file.GetFullPath(wxPATH_UNIX);
}

void SFTPPlugin::OpenFile(const wxString& remotePath, int lineNumber)
{
    RemoteFileInfo::Map_t::iterator iter =
        std::find_if(m_remoteFiles.begin(), m_remoteFiles.end(), [&](const RemoteFileInfo::Map_t::value_type& vt) {
            return vt.second.GetRemoteFile() == remotePath;
        });
    if(iter != m_remoteFiles.end()) {
        m_mgr->OpenFile(iter->second.GetLocalFile(), "", lineNumber);
    } else {
        RemoteFileInfo remoteFile;
        remoteFile.SetAccount(GetTreeView()->GetAccount());
        remoteFile.SetRemoteFile(remotePath);
        remoteFile.SetLineNumber(lineNumber);

        SFTPThreadRequet* req = new SFTPThreadRequet(remoteFile);
        SFTPWorkerThread::Instance()->Add(req);
        AddRemoteFile(remoteFile);
    }
}