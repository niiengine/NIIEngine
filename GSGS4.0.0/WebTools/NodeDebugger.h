#ifndef NODEJSCLIDEBUGGER_H
#define NODEJSCLIDEBUGGER_H

#include "NodeJSDebuggerBreakpoint.h"
#include "NodeJSDebuggerBreakpointManager.h"
#include "NodeJSDevToolsProtocol.h"
#include "SocketAPI/gsgsWebSocketClient.h"
#include "gsgsProcess.h"
#include "gsgsEvent.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <wx/event.h>

class NodeDebugger : public wxEvtHandler
{
    IProcess* m_process = nullptr;
    bool m_canInteract = false;
    wxString m_workingDirectory;
    gsgs::WebSocketClient m_socket;
    NodeJSBptManager m_bptManager;
    wxString m_activeFrame;

public:
    typedef wxSharedPtr<NodeDebugger> Ptr_t;

public:
    NodeDebugger();
    virtual ~NodeDebugger();
    bool IsRunning() const;
    bool IsCanInteract() const;
    void StartDebugger(const wxString& command, const wxString& command_args, const wxString& workingDirectory);
    void StopDebugger();
    void ListBreakpoints();
    void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }
    void SendToDebuggee(const wxString& command);
    void Eval(const wxString& command, const wxString& frameId);
    void GetObjectProperties(const wxString& objectId, wxEventType eventType);
    NodeJSBptManager* GetBreakpointsMgr() { return &m_bptManager; }

    void SetDebuggerMarker(IEditor* editor, int lineno);
    void SetDebuggerMarker(const wxString& path, int lineno);
    void ClearDebuggerMarker();

    void SetBreakpoint(const wxFileName& file, int lineNumber);
    void DeleteBreakpointByID(const wxString& bpid);
    void DeleteBreakpoint(const NodeJSBreakpoint& bp);
    void DeleteAllBreakpoints();

    void SetActiveFrame(const wxString& activeFrame) { this->m_activeFrame = activeFrame; }
    const wxString& GetActiveFrame() const { return m_activeFrame; }

protected:
    void OnDebugStart(gsgs::DebugEvent& event);
    void OnStopDebugger(gsgs::DebugEvent& event);
    void OnDebugNext(gsgs::DebugEvent& event);
    void OnDebugStepIn(gsgs::DebugEvent& event);
    void OnDebugStepOut(gsgs::DebugEvent& event);
    void OnDebugContinue(gsgs::DebugEvent& event);
    void OnToggleBreakpoint(gsgs::DebugEvent& event);
    void OnDebugIsRunning(gsgs::DebugEvent& event);
    void OnProcessTerminated(gsgs::ProcessEvent& event);
    void OnProcessOutput(gsgs::ProcessEvent& event);
    void OnWebSocketConnected(gsgs::Event& event);
    void OnWebSocketError(gsgs::Event& event);
    void OnWebSocketOnMessage(gsgs::Event& event);
    void OnWebSocketDisconnected(gsgs::Event& event);
    void OnWorkspaceClosed(wxCommandEvent& event);
    void OnInteract(gsgs::DebugEvent& event);
    void OnTooltip(gsgs::DebugEvent& event);
    // Helpers
    void DoCleanup();
    wxString GetBpRelativeFilePath(const NodeJSBreakpoint& bp) const;
    void ApplyAllBerakpoints();
    void DoHighlightLine(const wxString& filename, int lineNo);
};

#endif // NODEJSCLIDEBUGGER_H
