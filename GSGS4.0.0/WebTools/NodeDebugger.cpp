#include "NodeDebugger.h"
#include "NodeFileManager.h"
#include "NodeJSDebuggerDlg.h"
#include "NodeJSEvents.h"
#include "NoteJSWorkspace.h"
#include "SocketAPI/gsgsSocketBase.h"
#include "SocketAPI/gsgsWebSocketClient.h"
#include "gsgsBookmarkManager.h"
#include "gsgsEvent.h"
#include "gsgsEvent.h"
#include "gsgsLogManager.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "process_impl.h"

#define NODE_CLI_DEBUGGER_NAME "Node.js - CLI"

#define CHECK_RUNNING() \
    if(!IsRunning()) { return; }

#define CHECK_SHOULD_HANDLE(evt)                      \
    evt.Skip();                                       \
    if(!IsRunning()) { return; }                      \
    if(!NodeJSWorkspace::Get()->IsOpen()) { return; } \
    evt.Skip(false);

NodeDebugger::NodeDebugger()
    : m_socket(this)
{
    gsgs_Event().Bind(wxEVT_DBG_UI_START, &NodeDebugger::OnDebugStart, this);
    gsgs_Event().Bind(wxEVT_DBG_UI_CONTINUE, &NodeDebugger::OnDebugContinue, this);
    gsgs_Event().Bind(wxEVT_DBG_UI_STOP, &NodeDebugger::OnStopDebugger, this);
    gsgs_Event().Bind(wxEVT_DBG_UI_NEXT, &NodeDebugger::OnDebugNext, this);
    gsgs_Event().Bind(wxEVT_DBG_UI_STEP_IN, &NodeDebugger::OnDebugStepIn, this);
    gsgs_Event().Bind(wxEVT_DBG_UI_STEP_OUT, &NodeDebugger::OnDebugStepOut, this);
    gsgs_Event().Bind(wxEVT_DBG_IS_RUNNING, &NodeDebugger::OnDebugIsRunning, this);
    gsgs_Event().Bind(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, &NodeDebugger::OnToggleBreakpoint, this);
    gsgs_Event().Bind(wxEVT_WORKSPACE_CLOSED, &NodeDebugger::OnWorkspaceClosed, this);
    gsgs_Event().Bind(wxEVT_DBG_EXPR_TOOLTIP, &NodeDebugger::OnTooltip, this);

    gsgs_Event().Bind(wxEVT_NODEJS_DEBUGGER_INTERACT, &NodeDebugger::OnInteract, this);
    gsgs_Event().Bind(wxEVT_NODEJS_DEBUGGER_FINISHED, &NodeDebugger::OnStopDebugger, this);

    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &NodeDebugger::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &NodeDebugger::OnProcessTerminated, this);

    Bind(wxEVT_WEBSOCKET_CONNECTED, &NodeDebugger::OnWebSocketConnected, this);
    Bind(wxEVT_WEBSOCKET_ERROR, &NodeDebugger::OnWebSocketError, this);
    Bind(wxEVT_WEBSOCKET_ONMESSAGE, &NodeDebugger::OnWebSocketOnMessage, this);
    Bind(wxEVT_WEBSOCKET_DISCONNECTED, &NodeDebugger::OnWebSocketDisconnected, this);
    NodeJSDevToolsProtocol::Get().SetDebugger(this);
}

NodeDebugger::~NodeDebugger()
{
    gsgs_Event().Unbind(wxEVT_DBG_UI_START, &NodeDebugger::OnDebugStart, this);
    gsgs_Event().Unbind(wxEVT_DBG_UI_CONTINUE, &NodeDebugger::OnDebugContinue, this);
    gsgs_Event().Unbind(wxEVT_DBG_UI_STOP, &NodeDebugger::OnStopDebugger, this);
    gsgs_Event().Unbind(wxEVT_DBG_UI_NEXT, &NodeDebugger::OnDebugNext, this);
    gsgs_Event().Unbind(wxEVT_DBG_UI_STEP_IN, &NodeDebugger::OnDebugStepIn, this);
    gsgs_Event().Unbind(wxEVT_DBG_UI_STEP_OUT, &NodeDebugger::OnDebugStepOut, this);
    gsgs_Event().Unbind(wxEVT_DBG_IS_RUNNING, &NodeDebugger::OnDebugIsRunning, this);
    gsgs_Event().Unbind(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, &NodeDebugger::OnToggleBreakpoint, this);
    gsgs_Event().Unbind(wxEVT_DBG_EXPR_TOOLTIP, &NodeDebugger::OnTooltip, this);
    gsgs_Event().Unbind(wxEVT_WORKSPACE_CLOSED, &NodeDebugger::OnWorkspaceClosed, this);

    gsgs_Event().Unbind(wxEVT_NODEJS_DEBUGGER_INTERACT, &NodeDebugger::OnInteract, this);
    gsgs_Event().Unbind(wxEVT_NODEJS_DEBUGGER_FINISHED, &NodeDebugger::OnStopDebugger, this);

    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &NodeDebugger::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &NodeDebugger::OnProcessTerminated, this);
    Unbind(wxEVT_WEBSOCKET_CONNECTED, &NodeDebugger::OnWebSocketConnected, this);
    Unbind(wxEVT_WEBSOCKET_ERROR, &NodeDebugger::OnWebSocketError, this);
    Unbind(wxEVT_WEBSOCKET_ONMESSAGE, &NodeDebugger::OnWebSocketOnMessage, this);
    NodeJSDevToolsProtocol::Get().SetDebugger(nullptr);
}

void NodeDebugger::SetDebuggerMarker(IEditor* editor, int lineno)
{
    wxStyledTextCtrl* stc = editor->GetCtrl();
    stc->MarkerDeleteAll(smt_indicator);
    stc->MarkerAdd(lineno, smt_indicator);
    int caretPos = stc->PositionFromLine(lineno);
    stc->SetSelection(caretPos, caretPos);
    stc->SetCurrentPos(caretPos);
    stc->EnsureCaretVisible();
    editor->CenterLine(lineno);
#ifdef __WXOSX__
    stc->Refresh();
#endif
}

void NodeDebugger::ClearDebuggerMarker()
{
    IEditor::List_t editors;
    GetManager()->GetAllEditors(editors);
    std::for_each(editors.begin(), editors.end(),
                  [&](IEditor* editor) { editor->GetCtrl()->MarkerDeleteAll(smt_indicator); });
}

void NodeDebugger::DoHighlightLine(const wxString& filename, int lineNo)
{
    IEditor* activeEditor = GetManager()->OpenFile(filename, "", lineNo - 1);
    if(activeEditor) { SetDebuggerMarker(activeEditor, lineNo - 1); }
}

void NodeDebugger::SetDebuggerMarker(const wxString& path, int lineno) { DoHighlightLine(path, lineno); }

void NodeDebugger::OnDebugStart(gsgs::DebugEvent& event)
{
    event.Skip();
    CHECK_COND_RET(NodeJSWorkspace::Get()->IsOpen());

    event.SetFeatures(0); // No special features by the NodeJS debugger

    // Ours to handle
    event.Skip(false);

    if(!IsRunning()) {
        NodeJSDebuggerDlg dlg(gsgs_Event().TopFrame(), NodeJSDebuggerDlg::kDebugCLI);
        if(dlg.ShowModal() != wxID_OK) { return; }

        wxString command;
        wxString command_args;
        dlg.GetCommand(command, command_args);
        StartDebugger(command, command_args, dlg.GetWorkingDirectory());

    } else {
        OnDebugContinue(event);
    }
}

void NodeDebugger::OnDebugContinue(gsgs::DebugEvent& event)
{
    CHECK_SHOULD_HANDLE(event);
    NodeJSDevToolsProtocol::Get().Continue(m_socket);
}

void NodeDebugger::StopDebugger()
{
    // Terminate the process
    DoCleanup();
}

void NodeDebugger::OnStopDebugger(gsgs::DebugEvent& event)
{
    CHECK_SHOULD_HANDLE(event);
    // Terminate the process
    m_process->Terminate();
    m_socket.Close();
}

void NodeDebugger::OnDebugNext(gsgs::DebugEvent& event)
{
    CHECK_SHOULD_HANDLE(event);
    NodeJSDevToolsProtocol::Get().Next(m_socket);
}

bool NodeDebugger::IsRunning() const { return m_process != NULL; }

void NodeDebugger::DoCleanup()
{
    gsgsDEBUG() << "Cleaning Nodejs debugger...";
    m_canInteract = false;
    m_workingDirectory.Clear();
    if(m_process) { m_process->Terminate(); }
    m_socket.Close();
    NodeFileManager::Get().Clear();
    NodeJSDevToolsProtocol::Get().Clear();

    // Serialise the breakpoints
    m_bptManager.Save();
    m_activeFrame.Clear();
}

void NodeDebugger::OnDebugIsRunning(gsgs::DebugEvent& event)
{
    if(IsRunning()) {
        event.SetAnswer(true);

    } else {
        event.Skip();
    }
}

void NodeDebugger::OnProcessTerminated(gsgs::ProcessEvent& event)
{
    gsgsDEBUG() << "Nodejs process terminated";
    wxUnusedVar(event);
    wxDELETE(m_process);

    {
        gsgs::DebugEvent e(wxEVT_NODEJS_DEBUGGER_STOPPED);
        e.SetDebuggerName(NODE_CLI_DEBUGGER_NAME);
        gsgs_Event().AddPendingEvent(e);
    }

    {
        gsgs::DebugEvent e(wxEVT_DEBUG_ENDED);
        e.SetDebuggerName(NODE_CLI_DEBUGGER_NAME);
        gsgs_Event().AddPendingEvent(e);
    }

    DoCleanup();
}

void NodeDebugger::OnProcessOutput(gsgs::ProcessEvent& event)
{
    gsgsDEBUG1() << event.GetOutput();
    {
        gsgs::DebugEvent e(wxEVT_NODEJS_DEBUGGER_UPDATE_CONSOLE);
        e.SetString(event.GetOutput());
        gsgs_Event().AddPendingEvent(e);
    }

    wxString lcOutput = event.GetOutput().Lower();
    const wxString& processOutput = event.GetOutput();
    int where = processOutput.Find("ws://");
    if(where != wxNOT_FOUND) {
        wxString websocketAddress = processOutput.Mid(where);
        websocketAddress = websocketAddress.BeforeFirst('\n');
        websocketAddress.Trim().Trim(false);

        try {
            gsgsDEBUG() << "Attempting to connect debugger on" << websocketAddress;

            // Initialise the socket
            m_socket.Initialise();
            // Start a helper thread to listen on the this socket
            m_socket.StartLoop(websocketAddress);
        } catch(gsgs::SocketException& e) {
            gsgsWARNING() << e.what();
        }
    } else if(lcOutput.Contains("address already in use")) {

        // Anothe process is already using our port
        ::wxMessageBox(processOutput, "gsgs", wxICON_WARNING | wxCENTER);

        // Terminate the process
        if(m_process) { m_process->Terminate(); }
    }
}

void NodeDebugger::StartDebugger(const wxString& command, const wxString& command_args,
                                 const wxString& workingDirectory)
{
#if 0
    if(::wxMessageBox(_("The old debugger protocol is not supported by your current Node.js version.\nWould you "
                        "gsgs to a CLI debug session for you in a terminal?"),
                      _("光栅故事"), wxICON_QUESTION | wxYES_NO | wxCANCEL | wxYES_DEFAULT,
                      gsgs_Event().TopFrame()) == wxYES) {
        gsgs::ConsoleBase::Ptr_t console = gsgs::ConsoleBase::GetTerminal();
        console->SetWorkingDirectory(workingDirectory);
        console->SetCommand(command, command_args);
        console->SetTerminalNeeded(true);
        console->Start();
    }
#else
    if(m_socket.IsConnected()) {
        gsgsDEBUG() << "An instance of the debugger is already running";
        return;
    }
    size_t createFlags = IProcessCreateDefault;
    wxString one_liner = command;
    if(!command_args.IsEmpty()) { one_liner << " " << command_args; }
    m_process = ::CreateAsyncProcess(this, one_liner, createFlags, workingDirectory);
    if(!m_process) {
        ::wxMessageBox(wxString() << _("Failed to launch NodeJS: ") << command);
        DoCleanup();
        return;
    }

    // Keep the working directory
    m_workingDirectory = workingDirectory;

    gsgs::DebugEvent eventStart(wxEVT_NODEJS_DEBUGGER_STARTED);
    eventStart.SetDebuggerName(NODE_CLI_DEBUGGER_NAME);
    gsgs_Event().AddPendingEvent(eventStart);

    if(one_liner.Contains("inspect")) {
        // Fire CodeLite IDE event indicating that a debug session started
        gsgs::DebugEvent cl_event(wxEVT_DEBUG_STARTED);
        gsgs_Event().AddPendingEvent(cl_event);
    }
#endif
}

bool NodeDebugger::IsCanInteract() const { return m_process && m_canInteract; }

void NodeDebugger::OnToggleBreakpoint(gsgs::DebugEvent& event)
{
    event.Skip();
    if(!NodeJSWorkspace::Get()->IsOpen()) { return; }
    event.Skip(false);
    IEditor* editor = GetManager()->GetActiveEditor();
    if(IsRunning()) {
        if(editor && (editor->GetFileName().GetFullPath() == event.GetFileName())) {
            // Correct editor
            // add marker
            NodeJSBreakpoint bp = m_bptManager.GetBreakpoint(event.GetFileName(), event.GetInt());
            if(bp.IsOk()) {
                DeleteBreakpoint(bp);

            } else {
                SetBreakpoint(event.GetFileName(), event.GetInt());
            }
        }
    } else {

        NodeJSBreakpoint bp = m_bptManager.GetBreakpoint(event.GetFileName(), event.GetInt());
        if(bp.IsOk()) {
            m_bptManager.DeleteBreakpoint(event.GetFileName(), event.GetInt());
        } else {
            m_bptManager.AddBreakpoint(event.GetFileName(), event.GetInt());
        }
    }
    if(editor) { m_bptManager.SetBreakpoints(editor); }
}

void NodeDebugger::SetBreakpoint(const wxFileName& file, int lineNumber)
{
    // We have no breakpoint on this file/line (yet)
    m_bptManager.AddBreakpoint(file, lineNumber);
    const NodeJSBreakpoint& bp = m_bptManager.GetBreakpoint(file, lineNumber);
    if(!bp.IsOk()) { return; }
    NodeJSDevToolsProtocol::Get().SetBreakpoint(m_socket, bp);
    m_bptManager.AddBreakpoint(file.GetFullPath(), lineNumber);
}

void NodeDebugger::DeleteAllBreakpoints()
{
    wxArrayString arr = m_bptManager.GetAllAppliedBreakpoints();
    for(size_t i = 0; i < arr.size(); ++i) {
        DeleteBreakpointByID(arr.Item(i));
    }
    // Update the UI
    m_bptManager.DeleteAll();
}

void NodeDebugger::DeleteBreakpointByID(const wxString& bpid)
{
    NodeJSDevToolsProtocol::Get().DeleteBreakpointByID(m_socket, bpid);

    // Update the UI
    m_bptManager.DeleteByID(bpid);
}

void NodeDebugger::DeleteBreakpoint(const NodeJSBreakpoint& bp)
{
    if(!bp.IsOk()) { return; }
    m_bptManager.DeleteBreakpoint(bp.GetFilename(), bp.GetLine());
    NodeJSDevToolsProtocol::Get().DeleteBreakpoint(m_socket, bp);
}

void NodeDebugger::ListBreakpoints() {}

wxString NodeDebugger::GetBpRelativeFilePath(const NodeJSBreakpoint& bp) const
{
    wxFileName fn(bp.GetFilename());
    fn.MakeRelativeTo(GetWorkingDirectory());
    wxString file_path = fn.GetFullPath();

    // We need to escapte backslashes, otherwise, it wont work...
    file_path.Replace("\\", "\\\\");
    return file_path;
}

void NodeDebugger::ApplyAllBerakpoints()
{
    const NodeJSBreakpoint::Vec_t& breakpoints = m_bptManager.GetBreakpoints();
    std::for_each(breakpoints.begin(), breakpoints.end(),
                  [&](const NodeJSBreakpoint& bp) { SetBreakpoint(bp.GetFilename(), bp.GetLine()); });
    gsgs::DebugEvent bpEvent(wxEVT_NODEJS_DEBUGGER_UPDATE_BREAKPOINTS_VIEW);
    gsgs_Event().AddPendingEvent(bpEvent);
}

void NodeDebugger::OnWebSocketConnected(gsgs::Event& event)
{
    // Now that the connection has been established, we can send the startup commands
    NodeJSDevToolsProtocol::Get().SendStartCommands(m_socket);

    // Apply all breakpoints
    ApplyAllBerakpoints();
}

void NodeDebugger::OnWebSocketError(gsgs::Event& event)
{
    // an error occured!, terminate the debug session
    if(m_process) { m_process->Terminate(); }
}

void NodeDebugger::OnWebSocketOnMessage(gsgs::Event& event)
{
    // We got a message from the websocket
    gsgsDEBUG() << "<--" << event.GetString();
    NodeJSDevToolsProtocol::Get().ProcessMessage(event.GetString(), m_socket);
}

void NodeDebugger::OnWebSocketDisconnected(gsgs::Event& event) {}

void NodeDebugger::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
    DoCleanup();
}

void NodeDebugger::OnInteract(gsgs::DebugEvent& event)
{
    event.Skip();
    m_canInteract = event.IsAnswer();
}

void NodeDebugger::SendToDebuggee(const wxString& command)
{
    if(m_process) { m_process->Write(command); }
}

void NodeDebugger::OnDebugStepIn(gsgs::DebugEvent& event)
{
    CHECK_SHOULD_HANDLE(event);
    NodeJSDevToolsProtocol::Get().StepIn(m_socket);
}

void NodeDebugger::OnDebugStepOut(gsgs::DebugEvent& event)
{
    CHECK_SHOULD_HANDLE(event);
    NodeJSDevToolsProtocol::Get().StepOut(m_socket);
}

void NodeDebugger::Eval(const wxString& command, const wxString& frameId)
{
    NodeJSDevToolsProtocol::Get().Eval(m_socket, command, frameId);
}

void NodeDebugger::OnTooltip(gsgs::DebugEvent& event)
{
    CHECK_SHOULD_HANDLE(event);
    if(m_activeFrame.IsEmpty()) { return; }

    wxString expression = event.GetString();
    NodeJSDevToolsProtocol::Get().CreateObject(m_socket, expression, m_activeFrame);
}

void NodeDebugger::GetObjectProperties(const wxString& objectId, wxEventType eventType)
{
    NodeJSDevToolsProtocol::Get().GetObjectProperties(m_socket, objectId, eventType);
}
