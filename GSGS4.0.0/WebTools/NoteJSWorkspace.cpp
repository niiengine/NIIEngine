#include "NodeDebugger.h"
#include "NodeJSDebuggerDlg.h"
#include "NodeJSExecutable.h"
#include "NodeJSNewWorkspaceDlg.h"
#include "NodeJSWorkspaceConfiguration.h"
#include "NodeJSWorkspaceView.h"
#include "NoteJSWorkspace.h"
#include "gsgsProcess.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsEvent.h"
#include "gsgsTagManager.h"
#include "gsgsLogManager.h"
#include "globals.h"
#include "imanager.h"
#include "process_impl.h"
#include <wx/dir.h>
#include <wx/dirdlg.h>

NodeJSWorkspace* NodeJSWorkspace::ms_workspace = NULL;

NodeJSWorkspace::NodeJSWorkspace(bool dummy)
{
    m_dummy = true;
    SetWorkspaceType("Node.js");
}

NodeJSWorkspace::NodeJSWorkspace()
    : m_clangOldFlag(false)
    , m_showWelcomePage(false)
{
    SetWorkspaceType("Node.js");

    m_view = new NodeJSWorkspaceView(GetManager()->GetWorkspaceView()->GetBook(), GetWorkspaceType());
    GetManager()->GetWorkspaceView()->AddPage(m_view, GetWorkspaceType());

    gsgs_Event().Bind(wxEVT_CMD_CLOSE_WORKSPACE, &NodeJSWorkspace::OnCloseWorkspace, this);
    gsgs_Event().Bind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &NodeJSWorkspace::OnNewWorkspace, this);
    gsgs_Event().Bind(wxEVT_CMD_OPEN_WORKSPACE, &NodeJSWorkspace::OnOpenWorkspace, this);
    gsgs_Event().Bind(wxEVT_ALL_EDITORS_CLOSED, &NodeJSWorkspace::OnAllEditorsClosed, this);
    gsgs_Event().Bind(wxEVT_SAVE_SESSION_NEEDED, &NodeJSWorkspace::OnSaveSession, this);
    gsgs_Event().Bind(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT, &NodeJSWorkspace::OnExecute, this);
    gsgs_Event().Bind(wxEVT_CMD_STOP_EXECUTED_PROGRAM, &NodeJSWorkspace::OnStopExecute, this);
    gsgs_Event().Bind(wxEVT_CMD_IS_PROGRAM_RUNNING, &NodeJSWorkspace::OnIsExecuteInProgress, this);
    gsgs_Event().Bind(wxEVT_DBG_UI_START, &NodeJSWorkspace::OnDebugStart, this);

    m_terminal.Bind(wxEVT_TERMINAL_COMMAND_EXIT, &NodeJSWorkspace::OnProcessTerminated, this);
    m_terminal.Bind(wxEVT_TERMINAL_COMMAND_OUTPUT, &NodeJSWorkspace::OnProcessOutput, this);
}

NodeJSWorkspace::~NodeJSWorkspace()
{
    if(!m_dummy) {
        gsgs_Event().Unbind(wxEVT_CMD_CLOSE_WORKSPACE, &NodeJSWorkspace::OnCloseWorkspace, this);
        gsgs_Event().Unbind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &NodeJSWorkspace::OnNewWorkspace, this);
        gsgs_Event().Unbind(wxEVT_CMD_OPEN_WORKSPACE, &NodeJSWorkspace::OnOpenWorkspace, this);
        gsgs_Event().Unbind(wxEVT_ALL_EDITORS_CLOSED, &NodeJSWorkspace::OnAllEditorsClosed, this);
        gsgs_Event().Unbind(wxEVT_SAVE_SESSION_NEEDED, &NodeJSWorkspace::OnSaveSession, this);
        gsgs_Event().Unbind(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT, &NodeJSWorkspace::OnExecute, this);
        gsgs_Event().Unbind(wxEVT_CMD_STOP_EXECUTED_PROGRAM, &NodeJSWorkspace::OnStopExecute, this);
        gsgs_Event().Unbind(wxEVT_CMD_IS_PROGRAM_RUNNING, &NodeJSWorkspace::OnIsExecuteInProgress, this);
        gsgs_Event().Unbind(wxEVT_DBG_UI_START, &NodeJSWorkspace::OnDebugStart, this);
        m_debugger.reset(NULL);
        m_terminal.Unbind(wxEVT_TERMINAL_COMMAND_EXIT, &NodeJSWorkspace::OnProcessTerminated, this);
        m_terminal.Unbind(wxEVT_TERMINAL_COMMAND_OUTPUT, &NodeJSWorkspace::OnProcessOutput, this);

        m_terminal.Terminate();
    }
}

bool NodeJSWorkspace::IsBuildSupported() const { return false; }
bool NodeJSWorkspace::IsProjectSupported() const { return false; }

void NodeJSWorkspace::Free()
{
    if(ms_workspace) { delete ms_workspace; }
    ms_workspace = NULL;
}

NodeJSWorkspace* NodeJSWorkspace::Get()
{
    if(!ms_workspace) { ms_workspace = new NodeJSWorkspace(); }
    return ms_workspace;
}

bool NodeJSWorkspace::IsOpen() const { return m_filename.IsOk() && m_filename.Exists(); }

bool NodeJSWorkspace::Create(const wxFileName& filename)
{
    if(IsOpen()) return false;
    if(filename.Exists()) return false;
    DoClear();
    m_filename = filename;

    // By default add the workspace path
    m_folders.Add(m_filename.GetPath());
    Save();

    // We dont load the workspace
    DoClear();
    return true;
}

bool NodeJSWorkspace::Open(const wxFileName& filename)
{
    if(IsOpen()) return false;
    m_filename = filename;
    return DoOpen(m_filename);
}

void NodeJSWorkspace::Close()
{
    if(!IsOpen()) return;

    // Store the session
    GetManager()->StoreWorkspaceSession(m_filename);

    Save();
    DoClear();

    // disable clang for NodeJS
    GetManager()->EnableClangCodeCompletion(m_clangOldFlag);

    // Clear the UI
    GetView()->Clear();

    // Notify workspace closed event
    wxCommandEvent event(wxEVT_WORKSPACE_CLOSED);
    gsgs_Event().ProcessEvent(event);

    m_debugger.reset(NULL);

    // notify codelite to close the currently opened workspace
    wxCommandEvent eventClose(wxEVT_MENU, wxID_CLOSE_ALL);
    eventClose.SetEventObject(gsgs_Event().TopFrame());
    gsgs_Event().TopFrame()->GetEventHandler()->ProcessEvent(eventClose);
    m_showWelcomePage = true;
}

void NodeJSWorkspace::DoClear()
{
    m_filename.Clear();
    m_folders.Clear();
}

void NodeJSWorkspace::Save()
{
    NodeJSWorkspaceConfiguration conf(GetFileName());
    conf.SetFolders(m_folders);
    conf.Save();
}

void NodeJSWorkspace::OnCloseWorkspace(gsgs::Event& e)
{
    e.Skip();
    if(IsOpen()) {
        e.Skip(false);
        Close();
    }
}

void NodeJSWorkspace::OnNewWorkspace(gsgs::Event& e)
{
    e.Skip();
    if(e.GetString() == GetWorkspaceType()) {
        e.Skip(false);
        // Create a new NodeJS workspace
        NodeJSNewWorkspaceDlg dlg(NULL);
        if(dlg.ShowModal() != wxID_OK) return;

        wxFileName workspaceFile = dlg.GetWorkspaceFilename();
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

bool NodeJSWorkspace::DoOpen(const wxFileName& filename)
{
    NodeJSWorkspaceConfiguration conf(filename);
    conf.Load();
    if(!conf.IsOk()) {
        DoClear();
        return false;
    }

    m_folders = conf.GetFolders();
    GetView()->Clear();
    GetView()->ShowHiddenFiles(conf.IsShowHiddenFiles());

    const wxArrayString& folders = GetFolders();
    for(size_t i = 0; i < folders.size(); ++i) {
        GetView()->AddFolder(folders.Item(i));
    }

    // Notify codelite that NodeJS workspace is opened
    GetManager()->GetWorkspaceView()->SelectPage(GetWorkspaceType());
    gsgs_Workspace().SetWorkspace(this);

    // Keep the old clang state before we disable it
    const TagsOptionsData& options = TagsManagerST::Get()->GetCtagsOptions();
    m_clangOldFlag = (options.GetClangOptions() & CC_CLANG_ENABLED);

    GetManager()->EnableClangCodeCompletion(false);

    // Notify that the a new workspace is loaded
    wxCommandEvent event(wxEVT_WORKSPACE_LOADED);
    event.SetString(filename.GetFullPath());
    gsgs_Event().AddPendingEvent(event);

    // and finally, request codelite to keep this workspace in the recently opened workspace list
    GetManager()->AddWorkspaceToRecentlyUsedList(m_filename);

    // Load the workspace session (if any)
    CallAfter(&NodeJSWorkspace::RestoreSession);

    // Create new debugger for this workspace
    DoAllocateDebugger();
    return true;
}

void NodeJSWorkspace::OnOpenWorkspace(gsgs::Event& event)
{
    event.Skip();
    wxFileName workspaceFile(event.GetFileName());

    // Test that this is our workspace
    NodeJSWorkspaceConfiguration conf(workspaceFile);
    conf.Load();
    if(!conf.IsOk()) { return; }
    // This is a NodeJS workspace, stop event processing by calling
    // event.Skip(false)
    event.Skip(false);

    // Check if this is a PHP workspace
    if(IsOpen()) { Close(); }
    Open(workspaceFile);
}

void NodeJSWorkspace::OnAllEditorsClosed(wxCommandEvent& event)
{
    event.Skip();
    if(m_showWelcomePage) {
        m_showWelcomePage = false;
        // Show the 'Welcome Page'
        wxFrame* frame = gsgs_Event().TopFrame();
        wxCommandEvent eventShowWelcomePage(wxEVT_MENU, XRCID("view_welcome_page"));
        eventShowWelcomePage.SetEventObject(frame);
        frame->GetEventHandler()->AddPendingEvent(eventShowWelcomePage);
    }
}

void NodeJSWorkspace::RestoreSession()
{
    if(IsOpen()) { GetManager()->LoadWorkspaceSession(m_filename); }
}

void NodeJSWorkspace::OnSaveSession(gsgs::Event& event)
{
    event.Skip();
    if(IsOpen()) {
        // Call event.Skip(false) so no other session are kept beside ours
        event.Skip(false);
        GetManager()->StoreWorkspaceSession(m_filename);
    }
}

wxString NodeJSWorkspace::GetFilesMask() const
{
    return "*.js;*.html;*.css;*.scss;*.json;*.xml;*.ini;*.md;*.txt;*.text;*.javascript";
}

void NodeJSWorkspace::OnExecute(gsgs::ExecuteEvent& event)
{
    event.Skip();
    if(IsOpen()) {
        if(m_terminal.IsRunning()) {
            ::wxMessageBox(_("Another instance is already running. Please stop it before executing another one"),
                           "gsgs", wxICON_WARNING | wxCENTER | wxOK);
            return;
        }
        event.Skip(false);
        NodeJSDebuggerDlg dlg(gsgs_Event().TopFrame(), NodeJSDebuggerDlg::kExecute);
        if(dlg.ShowModal() != wxID_OK) { return; }

        wxString command;
        wxString command_args;
        dlg.GetCommand(command, command_args);
        m_terminal.ExecuteConsole(command, true, command_args, dlg.GetWorkingDirectory(), command + " " + command_args);
    }
}

void NodeJSWorkspace::OnProcessOutput(gsgs::Event& event)
{
    GetManager()->AppendOutputTabText(kOutputTab_Output, event.GetString());
}

void NodeJSWorkspace::OnProcessTerminated(gsgs::Event& event)
{
    wxUnusedVar(event);
    gsgs_Event().TopFrame()->Raise();
}

void NodeJSWorkspace::OnIsExecuteInProgress(gsgs::ExecuteEvent& event)
{
    event.Skip();
    if(IsOpen()) {
        event.Skip(false);
        event.SetAnswer(m_terminal.IsRunning());
    }
}

void NodeJSWorkspace::OnStopExecute(gsgs::ExecuteEvent& event)
{
    event.Skip();
    if(IsOpen()) {
        event.Skip(false);
        m_terminal.Terminate();
    }
}

wxString NodeJSWorkspace::GetProjectFromFile(const wxFileName& filename) const
{
    // projects are not supported in NodeJS
    return "";
}
void NodeJSWorkspace::GetProjectFiles(const wxString& projectName, wxArrayString& files) const
{
    wxUnusedVar(files);
    wxUnusedVar(projectName);
}

void NodeJSWorkspace::GetWorkspaceFiles(wxArrayString& files) const
{
    // Return all the files
    wxDir::GetAllFiles(GetFileName().GetPath(), &files);
}
wxString NodeJSWorkspace::GetActiveProjectName() const { return wxEmptyString; }

wxFileName NodeJSWorkspace::GetProjectFileName(const wxString& projectName) const
{
    wxUnusedVar(projectName);
    return wxFileName();
}

wxArrayString NodeJSWorkspace::GetWorkspaceProjects() const { return wxArrayString(); }

int NodeJSWorkspace::GetNodeJSMajorVersion() const
{
    NodeJSExecutable nodeJS;
    int nodeVersion = nodeJS.GetMajorVersion();
    gsgsDEBUG() << "NodeJS major version is:" << nodeVersion;
    return nodeVersion;
}

void NodeJSWorkspace::DoAllocateDebugger()
{
    if((GetNodeJSMajorVersion() >= 8)) {
        gsgsDEBUG() << "Successfully allocated new JS debugger";
        m_debugger.reset(new NodeDebugger());
    } else {
        m_debugger.reset();
        gsgsWARNING() << "Your Nodejs version is lower than v8, unable to allocate debugger";
    }
}

void NodeJSWorkspace::OnDebugStart(gsgs::DebugEvent& event)
{
    if(IsOpen()) {
        if(m_debugger) {
            // let our debugger handle it
            event.Skip();
        } else {
            // We don't have a proper debugger, however, we dont want gsgs to start the wrong debugger
            ::wxMessageBox(_("Could not instantiate a debugger for your NodeJS version!"), "gsgs", wxICON_WARNING);
            event.Skip(false);
        }
    } else {
        event.Skip();
    }
}

NodeDebugger::Ptr_t NodeJSWorkspace::GetDebugger() { return m_debugger; }

void NodeJSWorkspace::AllocateDebugger() { DoAllocateDebugger(); }
