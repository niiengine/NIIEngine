#include "NewDockerWorkspaceDlg.h"
#include "clDockerWorkspace.h"
#include "clDockerWorkspaceView.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsEvent.h"
#include "gsgsTagManager.h"
#include "docker.h"
#include "globals.h"
#include "gsgsTag.h"
#include <imanager.h>

#define CHECK_EVENT(e)        \
    e.Skip();                 \
    if(!IsOpen()) { return; } \
    e.Skip(false);

clDockerWorkspace::clDockerWorkspace(bool bindEvents, Docker* plugin, clDockerDriver::Ptr_t driver)
    : m_bindEvents(bindEvents)
    , m_driver(driver)
    , m_plugin(plugin)
{
    SetWorkspaceType("Docker");
    if(m_bindEvents) {
        gsgs_Event().Bind(wxEVT_CMD_OPEN_WORKSPACE, &clDockerWorkspace::OnOpenWorkspace, this);
        gsgs_Event().Bind(wxEVT_CMD_CLOSE_WORKSPACE, &clDockerWorkspace::OnCloseWorkspace, this);
        gsgs_Event().Bind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &clDockerWorkspace::OnNewWorkspace, this);
        gsgs_Event().Bind(wxEVT_SAVE_SESSION_NEEDED, &clDockerWorkspace::OnSaveSession, this);
        gsgs_Event().Bind(wxEVT_GET_IS_BUILD_IN_PROGRESS, &clDockerWorkspace::OnIsBuildInProgress, this);
        gsgs_Event().Bind(wxEVT_BUILD_STARTING, &clDockerWorkspace::OnBuildStarting, this);
        gsgs_Event().Bind(wxEVT_STOP_BUILD, &clDockerWorkspace::OnStopBuild, this);
        gsgs_Event().Bind(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT, &clDockerWorkspace::OnRun, this);
        gsgs_Event().Bind(wxEVT_CMD_STOP_EXECUTED_PROGRAM, &clDockerWorkspace::OnStop, this);
        m_view = new clDockerWorkspaceView(GetManager()->GetWorkspaceView()->GetBook());
        GetManager()->GetWorkspaceView()->AddPage(m_view, GetWorkspaceType());
    }
}

clDockerWorkspace::~clDockerWorkspace()
{
    if(m_bindEvents) {
        gsgs_Event().Unbind(wxEVT_CMD_OPEN_WORKSPACE, &clDockerWorkspace::OnOpenWorkspace, this);
        gsgs_Event().Unbind(wxEVT_CMD_CLOSE_WORKSPACE, &clDockerWorkspace::OnCloseWorkspace, this);
        gsgs_Event().Unbind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &clDockerWorkspace::OnNewWorkspace, this);
        gsgs_Event().Unbind(wxEVT_SAVE_SESSION_NEEDED, &clDockerWorkspace::OnSaveSession, this);
        gsgs_Event().Unbind(wxEVT_GET_IS_BUILD_IN_PROGRESS, &clDockerWorkspace::OnIsBuildInProgress, this);
        gsgs_Event().Unbind(wxEVT_BUILD_STARTING, &clDockerWorkspace::OnBuildStarting, this);
        gsgs_Event().Unbind(wxEVT_STOP_BUILD, &clDockerWorkspace::OnStopBuild, this);
        gsgs_Event().Unbind(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT, &clDockerWorkspace::OnRun, this);
        gsgs_Event().Unbind(wxEVT_CMD_STOP_EXECUTED_PROGRAM, &clDockerWorkspace::OnStop, this);
    }
}

wxString clDockerWorkspace::GetActiveProjectName() const { return ""; }

wxFileName clDockerWorkspace::GetFileName() const { return m_filename; }

wxString clDockerWorkspace::GetFilesMask() const { return "Dockerfile;docker-compose.yml;*.txt"; }

wxFileName clDockerWorkspace::GetProjectFileName(const wxString& projectName) const { return wxFileName(); }

void clDockerWorkspace::GetProjectFiles(const wxString& projectName, wxArrayString& files) const
{
    wxUnusedVar(projectName);
}

wxString clDockerWorkspace::GetProjectFromFile(const wxFileName& filename) const
{
    wxUnusedVar(filename);
    return "";
}

void clDockerWorkspace::GetWorkspaceFiles(wxArrayString& files) const { wxUnusedVar(files); }

wxArrayString clDockerWorkspace::GetWorkspaceProjects() const { return wxArrayString(); }

bool clDockerWorkspace::IsBuildSupported() const { return true; }

bool clDockerWorkspace::IsProjectSupported() const { return false; }

static clDockerWorkspace* g_workspace = nullptr;

clDockerWorkspace* clDockerWorkspace::Get() { return g_workspace; }

void clDockerWorkspace::Initialise(Docker* plugin)
{
    if(!g_workspace) { g_workspace = new clDockerWorkspace(true, plugin, plugin->GetDriver()); }
}

void clDockerWorkspace::Shutdown() { wxDELETE(g_workspace); }

void clDockerWorkspace::OnOpenWorkspace(gsgs::Event& event)
{
    event.Skip();
    wxFileName workspaceFile(event.GetFileName());

    // Test that this is our workspace
    clDockerWorkspaceSettings conf;
    conf.Load(workspaceFile);
    if(!conf.IsOk()) { return; }

    // This is a Docker workspace, stop event processing by calling
    // event.Skip(false)
    event.Skip(false);

    // Check if this is a PHP workspace
    if(IsOpen()) { Close(); }
    Open(workspaceFile);
}

void clDockerWorkspace::OnCloseWorkspace(gsgs::Event& event)
{
    event.Skip();
    if(IsOpen()) {
        event.Skip(false);
        Close();
    }
}

void clDockerWorkspace::Open(const wxFileName& path)
{
    m_filename = path;
    m_settings.Load(m_filename);
    m_isOpen = m_settings.Load(m_filename).IsOk();
    if(!IsOpen()) {
        m_filename.Clear();
        m_settings.Clear();
        GetView()->Clear();
    } else {

        //===------------------------------------------
        // Finalize the workspace open process:
        //===------------------------------------------

        // Notify codelite that NodeJS workspace is opened
        GetManager()->GetWorkspaceView()->SelectPage(GetWorkspaceType());
        gsgs_Workspace().SetWorkspace(this);

        // Keep the old clang state before we disable it
        const TagsOptionsData& options = TagsManagerST::Get()->GetCtagsOptions();
        m_clangOldFlag = (options.GetClangOptions() & CC_CLANG_ENABLED);

        GetManager()->EnableClangCodeCompletion(false);

        // Notify that the a new workspace is loaded
        wxCommandEvent event(wxEVT_WORKSPACE_LOADED);
        event.SetString(m_filename.GetFullPath());
        gsgs_Event().AddPendingEvent(event);

        // and finally, request codelite to keep this workspace in the recently opened workspace list
        GetManager()->AddWorkspaceToRecentlyUsedList(m_filename);

        // Load the workspace session (if any)
        CallAfter(&clDockerWorkspace::RestoreSession);
    }
}

void clDockerWorkspace::Close()
{
    if(IsOpen()) {
        // Store the session
        GetManager()->StoreWorkspaceSession(m_filename);

        // disable clang for NodeJS
        GetManager()->EnableClangCodeCompletion(m_clangOldFlag);

        // Clear the UI
        GetView()->Clear();

        // Notify workspace closed event
        wxCommandEvent event(wxEVT_WORKSPACE_CLOSED);
        gsgs_Event().ProcessEvent(event);

        // notify codelite to close the currently opened workspace
        wxCommandEvent eventClose(wxEVT_MENU, wxID_CLOSE_ALL);
        eventClose.SetEventObject(gsgs_Event().TopFrame());
        gsgs_Event().TopFrame()->GetEventHandler()->ProcessEvent(eventClose);

        m_filename.Clear();
        m_settings.Clear();
        m_isOpen = false;
    }
}

bool clDockerWorkspace::IsOpen() const { return m_isOpen; }

void clDockerWorkspace::OnNewWorkspace(gsgs::Event& event)
{
    event.Skip();
    if(event.GetString() == GetWorkspaceType()) {
        event.Skip(false);

        // Create a new NodeJS workspace
        NewDockerWorkspaceDlg dlg(gsgs_Event().TopFrame());
        if(dlg.ShowModal() != wxID_OK) return;

        wxFileName workspaceFile = dlg.GetWorkspaceFile();
        if(!workspaceFile.GetDirCount()) {
            ::wxMessageBox(_("Can not create workspace in the root folder"), _("New Workspace"),
                           wxICON_ERROR | wxOK | wxCENTER);
            return;
        }

        // Ensure that the path the workspace exists
        workspaceFile.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        if(!Create(workspaceFile)) {
            ::wxMessageBox(_("Failed to create workspace\nWorkspace already exists"), _("New Workspace"),
                           wxICON_ERROR | wxOK | wxCENTER);
            return;
        }
        Open(workspaceFile);
    }
}

bool clDockerWorkspace::Create(const wxFileName& filename)
{
    // Already exists
    if(filename.FileExists()) { return false; }
    return m_settings.Save(filename).Load(filename).IsOk();
}

void clDockerWorkspace::RestoreSession()
{
    if(IsOpen()) { GetManager()->LoadWorkspaceSession(m_filename); }
}

void clDockerWorkspace::OnSaveSession(gsgs::Event& event)
{
    event.Skip();
    if(IsOpen()) {
        event.Skip(false);
        GetManager()->StoreWorkspaceSession(m_filename);
    }
}

void clDockerWorkspace::OnIsBuildInProgress(gsgs::BuildEvent& event)
{
    CHECK_EVENT(event);
    event.SetIsRunning(m_driver->IsRunning());
}

void clDockerWorkspace::OnBuildStarting(gsgs::BuildEvent& event)
{
    CHECK_EVENT(event);
    gsgs::IEditor* editor = GetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);
    if(editor->GetFileName().GetFullName() == "Dockerfile") {
        if(event.GetKind() == "build") { BuildDockerfile(editor->GetFileName()); }
    }
}

void clDockerWorkspace::OnStopBuild(gsgs::BuildEvent& event)
{
    CHECK_EVENT(event);
    if(m_driver->IsRunning()) { m_driver->Stop(); }
}

void clDockerWorkspace::OnRun(gsgs::ExecuteEvent& event)
{
    CHECK_EVENT(event);
    gsgs::IEditor* editor = GetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);
    if(editor->GetFileName().GetFullName() == "Dockerfile") { RunDockerfile(editor->GetFileName()); }
}

void clDockerWorkspace::OnStop(gsgs::ExecuteEvent& event)
{
    CHECK_EVENT(event);
    if(m_driver->IsRunning()) { m_driver->Stop(); }
}

void clDockerWorkspace::BuildDockerfile(const wxFileName& dockerfile) { m_driver->Build(dockerfile, m_settings); }

void clDockerWorkspace::RunDockerfile(const wxFileName& dockerfile) { m_driver->Run(dockerfile, m_settings); }

void clDockerWorkspace::BuildDockerCompose(const wxFileName& docker_compose)
{
    m_driver->Build(docker_compose, m_settings);
}

void clDockerWorkspace::RunDockerCompose(const wxFileName& docker_compose)
{
    m_driver->Run(docker_compose, m_settings);
}
