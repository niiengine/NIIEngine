#include "NewPHPProjectWizard.h"
#include "PHPDebugPane.h"
#include "PHPXDebugSetupWizard.h"
#include "XDebugTester.h"
#include "gsgsBookmarkManager.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsZip.h"
#include "gsgsPathManager.h"
#include "gsgsEditorConfig.h"
#include "evalpane.h"
#include "globals.h"
#include "localsview.h"
#include "new_php_workspace_dlg.h"
#include "php.h"
#include "php_code_completion.h"
#include "php_configuration_data.h"
#include "php_editor_context_menu.h"
#include "php_open_resource_dlg.h"
#include "php_parser_thread.h"
#include "php_settings_dlg.h"
#include "php_strings.h"
#include "php_utils.h"
#include "php_workspace.h"
#include "php_workspace_view.h"
#include "quick_outline_dlg.h"
#include "ssh_workspace_settings.h"
#include "xdebugevent.h"
#include <gsgsConfig.h>
#include <gsgsPathManager.h>
#include <gsgsTagManager.h>
#include <gsgsWindowManager.h>
#include <gsgsEvent.h>
#include <gsgsLogManager.h>
#include <plugin.h>
#include <wx/app.h>
#include <wx/filedlg.h>
#include <wx/regex.h>
#include <wx/richmsgdlg.h>
#include <wx/xrc/xmlres.h>

static PhpPlugin* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API Plugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) { thePlugin = new PhpPlugin(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("PHP"));
    info.SetDescription(_("Enable PHP support for codelite IDE"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

PhpPlugin::PhpPlugin(IManager* manager)
    : Plugin(manager)
    , m_clangOldFlag(false)
    , m_browser(NULL)
    , m_debuggerPane(NULL)
    , m_xdebugLocalsView(NULL)
    , m_xdebugEvalPane(NULL)
    , m_showWelcomePage(false)
{
    // Add new workspace type
    gsgs_Workspace().RegisterWorkspace(new PHPWorkspace());

    m_longName = _("PHP Plugin for the codelite IDE");
    m_shortName = wxT("PHP");

    // Instantiate the bitmaps, we do this so they will be populated in wxXmlResource
    // Sigleton class
    PHPWorkspace::Get()->SetPluginManager(m_mgr);
    XDebugManager::Initialize(this);

    // Add our UI
    // create tab (possibly detached)
    m_workspaceView = new PHPWorkspaceView(m_mgr->GetWorkspaceView()->GetBook(), m_mgr);
    m_mgr->GetWorkspaceView()->AddPage(m_workspaceView, PHPStrings::PHP_WORKSPACE_VIEW_LABEL);

    PHPCodeCompletion::Instance()->SetManager(m_mgr);
    PHPEditorContextMenu::Instance()->ConnectEvents();
    PHPParserThread::Instance()->Start();

    // Pass the manager class to the context menu manager
    PHPEditorContextMenu::Instance()->SetManager(m_mgr);

    // Connect events
    gsgs_Event().Connect(wxEVT_CC_SHOW_QUICK_OUTLINE,
                                  clCodeCompletionEventHandler(PhpPlugin::OnShowQuickOutline), NULL, this);
    gsgs_Event().Connect(wxEVT_DBG_UI_DELETE_ALL_BREAKPOINTS,
                                  gsgsDebugEventHandler(PhpPlugin::OnXDebugDeleteAllBreakpoints), NULL, this);
    gsgs_Event().Connect(wxEVT_CMD_CREATE_NEW_WORKSPACE, gsgsEventHandler(PhpPlugin::OnNewWorkspace),
                                  NULL, this);
    gsgs_Event().Connect(wxEVT_NEW_PROJECT_WIZARD_SHOWING, gsgsNewProjectEventHandler(PhpPlugin::OnNewProject),
                                  NULL, this);
    gsgs_Event().Connect(wxEVT_NEW_PROJECT_WIZARD_FINISHED,
                                  gsgsNewProjectEventHandler(PhpPlugin::OnNewProjectFinish), NULL, this);
    gsgs_Event().Connect(wxEVT_CMD_IS_WORKSPACE_OPEN, gsgsEventHandler(PhpPlugin::OnIsWorkspaceOpen),
                                  NULL, this);
    gsgs_Event().Connect(wxEVT_CMD_CLOSE_WORKSPACE, gsgsEventHandler(PhpPlugin::OnCloseWorkspace), NULL,
                                  this);
    gsgs_Event().Connect(wxEVT_CMD_OPEN_WORKSPACE, gsgsEventHandler(PhpPlugin::OnOpenWorkspace), NULL,
                                  this);
    gsgs_Event().Connect(wxEVT_CMD_RELOAD_WORKSPACE, gsgsEventHandler(PhpPlugin::OnReloadWorkspace), NULL,
                                  this);
    gsgs_Event().Connect(wxEVT_CMD_OPEN_RESOURCE, wxCommandEventHandler(PhpPlugin::OnOpenResource), NULL,
                                  this);
    gsgs_Event().Connect(wxEVT_CMD_GET_WORKSPACE_FILES, wxCommandEventHandler(PhpPlugin::OnGetWorkspaceFiles),
                                  NULL, this);
    gsgs_Event().Connect(wxEVT_CMD_GET_CURRENT_FILE_PROJECT_FILES,
                                  wxCommandEventHandler(PhpPlugin::OnGetCurrentFileProjectFiles), NULL, this);
    gsgs_Event().Connect(wxEVT_CMD_GET_ACTIVE_PROJECT_FILES,
                                  wxCommandEventHandler(PhpPlugin::OnGetActiveProjectFiles), NULL, this);
    gsgs_Event().Connect(wxEVT_PHP_LOAD_URL, PHPEventHandler(PhpPlugin::OnLoadURL), NULL, this);
    gsgs_Event().Connect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(PhpPlugin::OnAllEditorsClosed), NULL,
                                  this);

    gsgs_Event().Bind(wxEVT_XDEBUG_SESSION_STARTED, &PhpPlugin::OnDebugStarted, this);
    gsgs_Event().Bind(wxEVT_XDEBUG_SESSION_ENDED, &PhpPlugin::OnDebugEnded, this);

    gsgs_Event().Connect(wxEVT_GOING_DOWN, gsgsEventHandler(PhpPlugin::OnGoingDown), NULL, this);
    gsgs_Event().Bind(wxEVT_FILE_SYSTEM_UPDATED, &PhpPlugin::OnFileSysetmUpdated, this);
    gsgs_Event().Bind(wxEVT_SAVE_SESSION_NEEDED, &PhpPlugin::OnSaveSession, this);

    // Menu bar actions
    wxTheApp->Bind(wxEVT_MENU, &PhpPlugin::OnRunXDebugDiagnostics, this, wxID_PHP_RUN_XDEBUG_DIAGNOSTICS);
    wxTheApp->Bind(wxEVT_MENU, &PhpPlugin::OnMenuCommand, this, wxID_PHP_SETTINGS);

    CallAfter(&PhpPlugin::FinalizeStartup);

    // Extract all CC files from PHP.zip into the folder ~/.codelite/php-plugin/cc
    wxFileName phpResources(gsgs_Path().GetDataDir(), "PHP.zip");
    if(phpResources.Exists()) {

        gsgs::ZipReader zipReader(phpResources);
        wxFileName targetDir(gsgs_Path().GetUserDataDir(), "");
        targetDir.AppendDir("php-plugin");

        // Don't extract the zip if one of the files on disk is newer or equal to the zip timestamp
        wxFileName fnSampleFile(targetDir.GetPath(), "basic.php");
        fnSampleFile.AppendDir("cc");
        PHPConfigurationData config;
        if(!fnSampleFile.Exists() || // the sample file does not exists
                                     // Or the resource file (PHP.zip) is newer than the sample file
           (phpResources.GetModificationTime().GetTicks() > fnSampleFile.GetModificationTime().GetTicks())) {

            targetDir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            zipReader.Extract("*", targetDir.GetPath());

            // Make sure we add this path to the general PHP settings
            targetDir.AppendDir("cc"); // the CC files are located under an internal folder named "cc" (lowercase)

            if(config.Load().GetCcIncludePath().Index(targetDir.GetPath()) == wxNOT_FOUND) {
                config.Load().GetCcIncludePath().Add(targetDir.GetPath());
                config.Save();
            }
        } else if(fnSampleFile.Exists()) {
            // Ensure that we have the core PHP code completion methods
            if(config.Load().GetCcIncludePath().Index(fnSampleFile.GetPath()) == wxNOT_FOUND) {
                config.Load().GetCcIncludePath().Add(fnSampleFile.GetPath());
                config.Save();
            }
        }
    } else {
        gsgsWARNING() << "PHP: Could not locate PHP resources 'PHP.zip' =>" << phpResources.GetFullPath();
    }

#if USE_SFTP
    // Allocate SFTP handler
    m_sftpHandler.reset(new PhpSFTPHandler());
#endif // USE_SFTP
}

PhpPlugin::~PhpPlugin() {}

bool PhpPlugin::IsWorkspaceViewDetached()
{
    DetachedPanesInfo dpi;
    m_mgr->GetConfigTool()->ReadObject(wxT("DetachedPanesList"), &dpi);
    wxArrayString detachedPanes = dpi.GetPanes();
    return detachedPanes.Index(PHPStrings::PHP_WORKSPACE_VIEW_TITLE) != wxNOT_FOUND;
}

void PhpPlugin::CreateToolBar(gsgs::ToolBar* toolbar) { wxUnusedVar(toolbar); }

void PhpPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    if(pluginsMenu->GetMenuBar()) { DoPlaceMenuBar(pluginsMenu->GetMenuBar()); }
}

void PhpPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void PhpPlugin::UnPlug()
{
#if USE_SFTP
    m_sftpHandler.reset(nullptr);
#endif // USE_SFTP
    XDebugManager::Free();
    gsgs_Event().Disconnect(wxEVT_DBG_UI_DELETE_ALL_BREAKPOINTS,
                                     gsgsDebugEventHandler(PhpPlugin::OnXDebugDeleteAllBreakpoints), NULL, this);
    gsgs_Event().Disconnect(wxEVT_CC_SHOW_QUICK_OUTLINE,
                                     clCodeCompletionEventHandler(PhpPlugin::OnShowQuickOutline), NULL, this);
    gsgs_Event().Disconnect(wxEVT_CMD_CREATE_NEW_WORKSPACE, gsgsEventHandler(PhpPlugin::OnNewWorkspace),
                                     NULL, this);
    gsgs_Event().Disconnect(wxEVT_NEW_PROJECT_WIZARD_SHOWING,
                                     gsgsNewProjectEventHandler(PhpPlugin::OnNewProject), NULL, this);
    gsgs_Event().Disconnect(wxEVT_NEW_PROJECT_WIZARD_FINISHED,
                                     gsgsNewProjectEventHandler(PhpPlugin::OnNewProjectFinish), NULL, this);
    gsgs_Event().Disconnect(wxEVT_CMD_IS_WORKSPACE_OPEN, gsgsEventHandler(PhpPlugin::OnIsWorkspaceOpen),
                                     NULL, this);
    gsgs_Event().Disconnect(wxEVT_CMD_CLOSE_WORKSPACE, gsgsEventHandler(PhpPlugin::OnCloseWorkspace),
                                     NULL, this);
    gsgs_Event().Disconnect(wxEVT_CMD_OPEN_WORKSPACE, gsgsEventHandler(PhpPlugin::OnOpenWorkspace), NULL,
                                     this);
    gsgs_Event().Disconnect(wxEVT_CMD_RELOAD_WORKSPACE, gsgsEventHandler(PhpPlugin::OnReloadWorkspace),
                                     NULL, this);
    gsgs_Event().Disconnect(wxEVT_CMD_OPEN_RESOURCE, wxCommandEventHandler(PhpPlugin::OnOpenResource), NULL,
                                     this);
    gsgs_Event().Disconnect(wxEVT_CMD_GET_WORKSPACE_FILES,
                                     wxCommandEventHandler(PhpPlugin::OnGetWorkspaceFiles), NULL, this);
    gsgs_Event().Disconnect(wxEVT_CMD_GET_CURRENT_FILE_PROJECT_FILES,
                                     wxCommandEventHandler(PhpPlugin::OnGetCurrentFileProjectFiles), NULL, this);
    gsgs_Event().Disconnect(wxEVT_CMD_GET_ACTIVE_PROJECT_FILES,
                                     wxCommandEventHandler(PhpPlugin::OnGetActiveProjectFiles), NULL, this);
    gsgs_Event().Disconnect(wxEVT_PHP_LOAD_URL, PHPEventHandler(PhpPlugin::OnLoadURL), NULL, this);
    gsgs_Event().Disconnect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(PhpPlugin::OnAllEditorsClosed),
                                     NULL, this);

    gsgs_Event().Unbind(wxEVT_XDEBUG_SESSION_STARTED, &PhpPlugin::OnDebugStarted, this);
    gsgs_Event().Unbind(wxEVT_XDEBUG_SESSION_ENDED, &PhpPlugin::OnDebugEnded, this);
    gsgs_Event().Disconnect(wxEVT_GOING_DOWN, gsgsEventHandler(PhpPlugin::OnGoingDown), NULL, this);
    gsgs_Event().Unbind(wxEVT_FILE_SYSTEM_UPDATED, &PhpPlugin::OnFileSysetmUpdated, this);
    gsgs_Event().Unbind(wxEVT_SAVE_SESSION_NEEDED, &PhpPlugin::OnSaveSession, this);

    // Menu bar actions
    wxTheApp->Unbind(wxEVT_MENU, &PhpPlugin::OnRunXDebugDiagnostics, this, wxID_PHP_RUN_XDEBUG_DIAGNOSTICS);
    wxTheApp->Unbind(wxEVT_MENU, &PhpPlugin::OnMenuCommand, this, wxID_PHP_SETTINGS);

    SafelyDetachAndDestroyPane(m_debuggerPane, "XDebug");
    SafelyDetachAndDestroyPane(m_xdebugLocalsView, "XDebugLocals");
    SafelyDetachAndDestroyPane(m_xdebugEvalPane, "XDebugEval");

    // Remove the PHP tab
    m_mgr->GetWorkspaceView()->RemovePage(PHPStrings::PHP_WORKSPACE_VIEW_LABEL);

    // Close any open workspace
    if(PHPWorkspace::Get()->IsOpen()) {
        PHPWorkspace::Get()->Close(true, false);
        m_workspaceView->UnLoadWorkspaceView();
    }

    m_workspaceView->Destroy();
    m_workspaceView = NULL;

    PHPParserThread::Release();
    PHPWorkspace::Release();
    PHPCodeCompletion::Release();
    PHPEditorContextMenu::Release();
}

void PhpPlugin::OnShowQuickOutline(CodeCompleteEvent& e)
{
    gsgs::IEditor* editor = dynamic_cast<gsgs::IEditor*>(e.GetEditor());
    if(editor) {
        // we handle only .php files
        if(!IsPHPFile(editor)) {
            // get the position
            e.Skip();
            return;
        }
        PHPQuickOutlineDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), editor, m_mgr);
        dlg.ShowModal();
        CallAfter(&PhpPlugin::SetEditorActive, editor);
    }
}

void PhpPlugin::OnNewWorkspace(gsgs::Event& e)
{
    e.Skip();
    if(e.GetString() == PHPWorkspace::Get()->GetWorkspaceType()) {
        e.Skip(false);
        // Create a PHP workspace
        NewPHPWorkspaceDlg newWspDlg(m_mgr->GetTheApp()->GetTopWindow());
        if(newWspDlg.ShowModal() == wxID_OK) {
            // Ensure that the workspace path exists
            wxFileName workspaceFile(newWspDlg.GetWorkspacePath());
            if(!workspaceFile.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) {
                ::wxMessageBox(wxString::Format(_("Could not create workspace folder:\n%s"), workspaceFile.GetPath()),
                               "gsgs", wxICON_ERROR | wxOK | wxCENTER);
                return;
            }
            PHPWorkspace::Get()->Create(newWspDlg.GetWorkspacePath());
            DoOpenWorkspace(newWspDlg.GetWorkspacePath(), false /* create if missing */, false);
        }
    }
}

void PhpPlugin::OnIsWorkspaceOpen(gsgs::Event& e)
{
    e.Skip();
    bool isOpen = PHPWorkspace::Get()->IsOpen();
    e.SetAnswer(isOpen);
    if(isOpen) {
        e.SetFileName(PHPWorkspace::Get()->GetFilename().GetFullPath());
        e.SetString(e.GetFileName());
    }
}

void PhpPlugin::OnCloseWorkspace(gsgs::Event& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {

        m_mgr->EnableClangCodeCompletion(m_clangOldFlag);
        PHPWorkspace::Get()->Close(true, true);
        m_workspaceView->UnLoadWorkspaceView();

        // notify codelite to close the currently opened workspace
        wxCommandEvent eventClose(wxEVT_COMMAND_MENU_SELECTED, wxID_CLOSE_ALL);
        eventClose.SetEventObject(FRAME);
        FRAME->GetEventHandler()->ProcessEvent(eventClose);

        wxCommandEvent eventCloseWsp(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_workspace"));
        eventCloseWsp.SetEventObject(FRAME);
        FRAME->GetEventHandler()->ProcessEvent(eventCloseWsp);

        /// The 'wxID_CLOSE_ALL' is done async (i.e. it will take place in the next event loop)
        /// So we mark ourself that we should display the welcome page next time we capture
        /// the 'All Editors Closed' event
        m_showWelcomePage = true;

    } else {
        e.Skip();
    }
}

void PhpPlugin::OnOpenWorkspace(gsgs::Event& e)
{
    e.Skip();
    wxFileName workspaceFile(e.GetFileName());
    JSON root(workspaceFile);
    if(!root.isValid()) return;

    wxString type;
    root.getObject().getObject("metadata").getObject("type").getValue(type);
    bool hasProjects = root.getObject().isObjectExist("projects");
    if(type == "php" || hasProjects) {
        // this is our to handle
        e.Skip(false);
    } else {
        return;
    }

    // Check if this is a PHP workspace
    if(PHPWorkspace::Get()->IsOpen()) { PHPWorkspace::Get()->Close(true, true); }
    DoOpenWorkspace(workspaceFile.GetFullPath());
}

void PhpPlugin::DoOpenWorkspace(const wxString& filename, bool createIfMissing, bool createProjectFromSources)
{
    // notify codelite to close the currently opened workspace
    wxCommandEvent eventClose(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_workspace"));
    eventClose.SetEventObject(FRAME);
    FRAME->GetEventHandler()->ProcessEvent(eventClose);

    // Open the PHP workspace
    if(!PHPWorkspace::Get()->Open(filename, m_workspaceView, createIfMissing)) {
        wxMessageBox(_("Failed to open workspace: corrupted workspace file"), wxT("gsgs"),
                     wxOK | wxICON_WARNING | wxCENTER, FRAME);
        return;
    }

    // Keep the old clang state before we disable it
    const TagsOptionsData& options = TagsManagerST::Get()->GetCtagsOptions();
    m_clangOldFlag = (options.GetClangOptions() & CC_CLANG_ENABLED);

    m_mgr->EnableClangCodeCompletion(false);
    m_workspaceView->LoadWorkspaceView();

    // Select the 'PHP' tab
    m_mgr->GetWorkspaceView()->SelectPage(PHPStrings::PHP_WORKSPACE_VIEW_LABEL);

    if(createProjectFromSources) {
        PHPConfigurationData conf;
        PHPProject::CreateData cd;
        conf.Load();
        cd.importFilesUnderPath = true;
        cd.name = PHPWorkspace::Get()->GetWorkspaceName();
        cd.phpExe = conf.GetPhpExe();
        cd.path = PHPWorkspace::Get()->GetFilename().GetPath();
        cd.projectType = PHPProjectSettingsData::kRunAsCLI;
        m_workspaceView->CallAfter(&PHPWorkspaceView::CreateNewProject, cd);
    }
}

void PhpPlugin::OnOpenResource(wxCommandEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        OpenResourceDlg dlg(FRAME, m_mgr);
        if(dlg.ShowModal() == wxID_OK) {
            ResourceItem* itemData = dlg.GetSelectedItem();
            if(itemData) {
                gsgs::IEditor* editor = m_mgr->OpenFile(itemData->filename.GetFullPath());
                if(editor) {
                    if(itemData->line != wxNOT_FOUND) {
                        if(!editor->FindAndSelect(itemData->displayName, itemData->displayName,
                                                  editor->PosFromLine(itemData->line), NULL)) {
                            editor->CenterLine(itemData->line);
                        }
                    }
                }
            }
        }
    } else {
        e.Skip();
    }
}

void PhpPlugin::OnGetActiveProjectFiles(wxCommandEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        wxArrayString* pfiles = (wxArrayString*)e.GetClientData();
        if(pfiles) {
            wxString activeProjectName = PHPWorkspace::Get()->GetActiveProjectName();
            PHPProject::Ptr_t proj = PHPWorkspace::Get()->GetProject(activeProjectName);
            CHECK_PTR_RET(proj);
            const wxArrayString& projfiles = proj->GetFiles(NULL);
            pfiles->insert(pfiles->end(), projfiles.begin(), projfiles.end());
        }
    } else {
        e.Skip();
    }
}

void PhpPlugin::OnGetCurrentFileProjectFiles(wxCommandEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        gsgs::IEditor* editor = m_mgr->GetActiveEditor();
        wxArrayString* pfiles = (wxArrayString*)e.GetClientData();
        if(editor && pfiles) { ::wxMessageBox("Not implemented for PHP!"); }
    } else {
        e.Skip();
    }
}

void PhpPlugin::OnGetWorkspaceFiles(wxCommandEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        wxArrayString* pfiles = (wxArrayString*)e.GetClientData();
        if(pfiles) {
            gsgsStringSet files;
            PHPWorkspace::Get()->GetWorkspaceFiles(files);
            gsgsStringSet::iterator iter = files.begin();
            for(; iter != files.end(); iter++) {
                pfiles->Add(*iter);
            }
        }

    } else
        e.Skip();
}

void PhpPlugin::OnNewProject(gsgs::NewProjectEvent& e)
{
    if(!PHPWorkspace::Get()->IsOpen()) {
        e.Skip();
    } else {
        // we have a PHP workspace opened - handle it ourself
        NewPHPProjectWizard wiz(gsgs_Event().TopFrame());
        if(wiz.RunWizard(wiz.GetFirstPage())) {
            m_workspaceView->CallAfter(&PHPWorkspaceView::CreateNewProject, wiz.GetCreateData());
        }
    }
}

void PhpPlugin::DoPlaceMenuBar(wxMenuBar* menuBar)
{
    // Add our menu bar
    wxMenu* phpMenuBarMenu = new wxMenu();
    phpMenuBarMenu->Append(wxID_PHP_SETTINGS, _("PHP Settings..."), _("PHP Settings..."));
    phpMenuBarMenu->Append(wxID_PHP_RUN_XDEBUG_DIAGNOSTICS, _("Run XDebug Setup Wizard..."),
                           _("Run XDebug Setup Wizard..."));

    int helpLoc = menuBar->FindMenu(_("Help"));
    if(helpLoc != wxNOT_FOUND) { menuBar->Insert(helpLoc, phpMenuBarMenu, _("P&HP")); }
}

void PhpPlugin::OnMenuCommand(wxCommandEvent& e)
{
    switch(e.GetId()) {
    case wxID_PHP_SETTINGS: {
        PHPSettingsDlg dlg(FRAME);
        dlg.ShowModal();
    } break;
    default:
        e.Skip();
        break;
    }
}

void PhpPlugin::OnReloadWorkspace(gsgs::Event& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        m_workspaceView->CallAfter(&PHPWorkspaceView::ReloadWorkspace, false);

    } else {
        e.Skip();
    }
}

void PhpPlugin::OnLoadURL(PHPEvent& e)
{
    e.Skip();
    gsgs_Debug("Loading URL: " + e.GetUrl());
    ::wxLaunchDefaultBrowser(e.GetUrl());
}

// Debugger events
void PhpPlugin::OnDebugEnded(XDebugEvent& e)
{
    e.Skip();

    // Save the layout
    wxFileName fnConfig(gsgs_Path().GetUserDataDir(), "xdebug-perspective");
    fnConfig.AppendDir("config");

    wxFFile fp(fnConfig.GetFullPath(), "w+b");
    if(fp.IsOpened()) {
        fp.Write(m_mgr->GetAuiManager()->SavePerspective());
        fp.Close();
    }

    if(!m_savedPerspective.IsEmpty()) {
        m_mgr->GetAuiManager()->LoadPerspective(m_savedPerspective);
        m_savedPerspective.Clear();
    }
}

void PhpPlugin::OnDebugStarted(XDebugEvent& e)
{
    e.Skip();
    DoEnsureXDebugPanesVisible();
}

void PhpPlugin::OnXDebugDeleteAllBreakpoints(gsgs::DebugEvent& e)
{
    e.Skip();
    PHPEvent eventDelAllBP(wxEVT_PHP_DELETE_ALL_BREAKPOINTS);
    gsgs_Event().AddPendingEvent(eventDelAllBP);
}

void PhpPlugin::OnXDebugShowBreakpointsWindow(wxCommandEvent& e) { DoEnsureXDebugPanesVisible(_("Breakpoints")); }

void PhpPlugin::DoEnsureXDebugPanesVisible(const wxString& selectWindow)
{
    // Save the current layout to be the normal layout
    m_savedPerspective = m_mgr->GetAuiManager()->SavePerspective();
    m_debuggerPane->SelectTab(selectWindow);

    // If we have an old perspective, load it
    wxFileName fnConfig(gsgs_Path().GetUserDataDir(), "xdebug-perspective");
    fnConfig.AppendDir("config");

    if(fnConfig.Exists()) {
        wxFFile fp(fnConfig.GetFullPath(), "rb");
        if(fp.IsOpened()) {
            wxString perspective;
            fp.ReadAll(&perspective);

            m_mgr->GetAuiManager()->LoadPerspective(perspective, false);
        }
    }

    EnsureAuiPaneIsVisible("XDebug");
    EnsureAuiPaneIsVisible("XDebugEval");
    EnsureAuiPaneIsVisible("XDebugLocals", true);
}

void PhpPlugin::SafelyDetachAndDestroyPane(wxWindow* pane, const wxString& name)
{
    if(pane) {
        wxAuiPaneInfo& pi = m_mgr->GetAuiManager()->GetPane(name);
        if(pi.IsOk()) {
            m_mgr->GetAuiManager()->DetachPane(pane);
            pane->Destroy();
        }
    }
}

void PhpPlugin::EnsureAuiPaneIsVisible(const wxString& paneName, bool update)
{
    wxAuiPaneInfo& pi = m_mgr->GetAuiManager()->GetPane(paneName);
    if(pi.IsOk() && !pi.IsShown()) { pi.Show(); }
    if(update) { m_mgr->GetAuiManager()->Update(); }
}

void PhpPlugin::OnNewProjectFinish(gsgs::NewProjectEvent& e)
{
    if(e.GetTemplateName() != "PHP Project") {
        e.Skip();
        return;
    }

    if(m_mgr->IsOpen()) {
        ::wxMessageBox(_("Can't create PHP project. Close your current workspace first"), "PHP",
                       wxOK | wxICON_ERROR | wxCENTER);
        return;
    }

    if(!PHPWorkspace::Get()->IsOpen()) {
        // No PHP workspace is open, create a new one
        wxFileName workspacePath(e.GetProjectFolder(), e.GetProjectName());
        workspacePath.SetExt(PHPStrings::PHP_WORKSPACE_EXT);
        DoOpenWorkspace(workspacePath.GetFullPath(), true);
    }

    if(PHPWorkspace::Get()->IsOpen()) {
        PHPProject::CreateData cd;
        cd.importFilesUnderPath = true;
        cd.name = e.GetProjectName();
        cd.path = e.GetProjectFolder();
        m_workspaceView->CallAfter(&PHPWorkspaceView::CreateNewProject, cd);
    }
}

void PhpPlugin::OnXDebugSettings(wxCommandEvent& e) {}

void PhpPlugin::OnAllEditorsClosed(wxCommandEvent& e)
{
    e.Skip();

    /// If all editors closed event was triggered due to workspace close
    /// show the welcome page

    if(m_showWelcomePage) {
        m_showWelcomePage = false;
        // Show the 'Welcome Page'
        wxCommandEvent eventShowWelcomePage(wxEVT_COMMAND_MENU_SELECTED, XRCID("view_welcome_page"));
        eventShowWelcomePage.SetEventObject(FRAME);
        FRAME->GetEventHandler()->AddPendingEvent(eventShowWelcomePage);
    }
}

void PhpPlugin::SetEditorActive(gsgs::IEditor* editor) { editor->SetActive(); }

void PhpPlugin::RunXDebugDiagnostics()
{
    PHPXDebugSetupWizard wiz(gsgs_Event().TopFrame());
    if(wiz.RunWizard(wiz.GetFirstPage())) {}
#if 0
    XDebugTester xdebugTester;
    if(xdebugTester.RunTest()) {
        // Display the result
        wxString html;
        html << "<html><body>";
        html << "<table>";

        html << "<tr valign=\"top\" align=\"left\"><th>What?</th><th>Result</th><th>Description</th></tr>";
        const XDebugTester::ResultMap_t& result = xdebugTester.GetResults();
        XDebugTester::ResultMap_t::const_iterator iter = result.begin();
        for(; iter != result.end(); ++iter) {
            html << "<tr valign=\"top\" align=\"left\">";
            html << "<td>" << iter->first << "</td>";
            html << "<td>" << iter->second.first << "</td>";
            html << "<td>" << iter->second.second << "</td>";
            html << "</tr>";
        }

        html << "</table></body></html>";

        XDebugDiagDlg dlg(gsgs_Event().TopFrame());
        dlg.Load(html);
        dlg.ShowModal();
    }
#endif
}

void PhpPlugin::OnRunXDebugDiagnostics(wxCommandEvent& e)
{
    wxUnusedVar(e);
    RunXDebugDiagnostics();
}

void PhpPlugin::FinalizeStartup()
{
    // Create the debugger windows (hidden)
    wxWindow* parent = m_mgr->GetAuiManager()->GetManagedWindow();
    m_debuggerPane = new PHPDebugPane(parent);
    m_mgr->GetAuiManager()->AddPane(m_debuggerPane, wxAuiPaneInfo()
                                                            .Name("XDebug")
                                                            .Caption("Call Stack & Breakpoints")
                                                            .Hide()
                                                            .CloseButton()
                                                            .MaximizeButton()
                                                            .Bottom()
                                                            .Position(3));

    m_xdebugLocalsView = new LocalsView(parent);
    m_mgr->GetAuiManager()->AddPane(
        m_xdebugLocalsView,
        wxAuiPaneInfo().Name("XDebugLocals").Caption("Locals").Hide().CloseButton().MaximizeButton().Bottom());

    m_xdebugEvalPane = new EvalPane(parent);
    m_mgr->GetAuiManager()->AddPane(
        m_xdebugEvalPane,
        wxAuiPaneInfo().Name("XDebugEval").Caption("PHP").Hide().CloseButton().MaximizeButton().Bottom().Position(2));

    // Check to see if the have a PHP executable setup
    // if not - update it
    PHPConfigurationData data;
    data.Load();
}

void PhpPlugin::OnGoingDown(gsgs::Event& event) { event.Skip(); }

void PhpPlugin::OnFileSysetmUpdated(gsgs::FileEvent& event)
{
    event.Skip();
    if(PHPWorkspace::Get()->IsOpen()) {
        // Sync the workspace view, notify the view when its done
        PHPWorkspace::Get()->SyncWithFileSystemAsync(m_workspaceView);
    }
}

void PhpPlugin::OnSaveSession(gsgs::Event& event)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        // gsgs requires us to store the session, do it
        m_mgr->StoreWorkspaceSession(PHPWorkspace::Get()->GetFilename());
    } else {
        event.Skip();
    }
}
