#ifndef NODEJSDEVTOOLSPROTOCOL_H
#define NODEJSDEVTOOLSPROTOCOL_H

#include "MessageManager.h"
#include "NodeJSDebuggerBreakpoint.h"
#include "SocketAPI/gsgsWebSocketClient.h"
#include <unordered_map>
#include <wx/arrstr.h>

class NodeDebugger;
typedef std::function<void(const JSONObject&)> ResultFunc_t;
class CommandHandler
{
public:
    ResultFunc_t action = nullptr;
    long m_commandID = wxNOT_FOUND;
    CommandHandler() {}
    CommandHandler(long commandID, const ResultFunc_t& func)
        : action(func)
        , m_commandID(commandID)
    {
    }
};

class NodeJSDevToolsProtocol
{
    NodeDebugger* m_debugger = nullptr;
    long message_id = 0;
    MessageManager m_handlers;
    std::unordered_map<long, CommandHandler> m_waitingReplyCommands;

protected:
    void SendSimpleCommand(gsgs::WebSocketClient& socket, const wxString& command,
                           const JSONObject& params = JSONObject(NULL));

    NodeJSDevToolsProtocol();

public:
    static NodeJSDevToolsProtocol& Get();
    virtual ~NodeJSDevToolsProtocol();
    void Clear();

    void SetDebugger(NodeDebugger* debugger) { this->m_debugger = debugger; }
    NodeDebugger* GetDebugger() { return m_debugger; }
    void SendStartCommands(gsgs::WebSocketClient& socket);
    void ProcessMessage(const wxString& msg, gsgs::WebSocketClient& socket);
    void Next(gsgs::WebSocketClient& socket);
    void StepIn(gsgs::WebSocketClient& socket);
    void StepOut(gsgs::WebSocketClient& socket);
    void Continue(gsgs::WebSocketClient& socket);
    void SetBreakpoint(gsgs::WebSocketClient& socket, const NodeJSBreakpoint& bp);
    void DeleteBreakpoint(gsgs::WebSocketClient& socket, const NodeJSBreakpoint& bp);
    void DeleteBreakpointByID(gsgs::WebSocketClient& socket, const wxString& bpid);
    void GetScriptSource(gsgs::WebSocketClient& socket, const wxString& scriptId);
    void Eval(gsgs::WebSocketClient& socket, const wxString& expression, const wxString& frameId);
    void GetObjectProperties(gsgs::WebSocketClient& socket, const wxString& objectId, wxEventType eventType);
    void CreateObject(gsgs::WebSocketClient& socket, const wxString& expression, const wxString& frameId);
};

#endif // NODEJSDEVTOOLSPROTOCOL_H
