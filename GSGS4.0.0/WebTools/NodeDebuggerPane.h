#ifndef NODEJSCLIDEBUGGERPANE_H
#define NODEJSCLIDEBUGGERPANE_H

#include "WebToolsBase.h"
#include "clDebugCallFramesEvent.h"
#include "clDebugRemoteObjectEvent.h"
#include "gsgsEvent.h"

class CallFrame;
class NodeDebuggerTooltip;
class wxTerminal;
class NodeDebuggerPane : public NodeJSCliDebuggerPaneBase
{
    wxTerminal* m_terminal = nullptr;
    wxTerminal* m_node_console = nullptr;
    NodeDebuggerTooltip* m_debuggerTooltip = nullptr;
    std::unordered_map<wxString, wxTreeItemId> m_localsPendingItems;
    nSerializableObject::Vec_t m_frames;

protected:
    virtual void OnStackContextMenu(wxDataViewEvent& event);
    virtual void OnStackEntryActivated(wxDataViewEvent& event);
    virtual void OnLocalExpanding(wxTreeEvent& event);
    void DoDestroyTip();
    void DoPrintStartupMessages();
    void DoUpdateLocalsView(CallFrame* callFrame);
    wxString GetLocalObjectItem(const wxTreeItemId& item) const;
    CallFrame* GetFrameById(const wxString& frameId) const;
    void SelectTab(const wxString& label);

protected:
    void OnUpdateBacktrace(clDebugCallFramesEvent& event);
    void OnDebuggerStopped(gsgs::DebugEvent& event);
    void OnMarkLine(gsgs::DebugEvent& event);
    void OnInteract(gsgs::DebugEvent& event);
    void OnUpdateBreakpoints(gsgs::DebugEvent& event);
    void OnRunTerminalCommand(gsgs::Event& event);
    void OnEval(gsgs::Event& event);
    void OnConsoleOutput(gsgs::DebugEvent& event);
    void OnDebugSessionStarted(gsgs::DebugEvent& event);
    void OnLocalProperties(gsgs::DebugEvent& event);
    void OnEvalResult(clDebugRemoteObjectEvent& event);
    void OnCreateObject(clDebugRemoteObjectEvent& event);
    void OnDestroyTip(gsgs::Event& event);
    void OnClearAllBreakpoints(wxCommandEvent& event);
    void OnClearAllBreakpointsUI(wxUpdateUIEvent& event);
    void OnDeleteBreakpoint(wxCommandEvent& event);
    void OnDeleteBreakpointUI(wxUpdateUIEvent& event);

public:
    NodeDebuggerPane(wxWindow* parent);
    virtual ~NodeDebuggerPane();
};
#endif // NODEJSCLIDEBUGGERPANE_H
