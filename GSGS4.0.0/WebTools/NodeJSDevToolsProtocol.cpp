#include "NodeDebugger.h"
#include "NodeFileManager.h"
#include "NodeJSDevToolsProtocol.h"
#include "NodeJSEvents.h"
#include "RemoteObject.h"
#include "SocketAPI/gsgsSocketBase.h"
#include "clDebugRemoteObjectEvent.h"
#include "gsgsEvent.h"
#include "gsgsLogManager.h"
#include "gsgsJSON.h"

class RemoteObject;
NodeJSDevToolsProtocol::NodeJSDevToolsProtocol() {}

NodeJSDevToolsProtocol::~NodeJSDevToolsProtocol() {}

NodeJSDevToolsProtocol& NodeJSDevToolsProtocol::Get()
{
    static NodeJSDevToolsProtocol protocol;
    return protocol;
}

void NodeJSDevToolsProtocol::SendSimpleCommand(gsgs::WebSocketClient& socket, const wxString& command,
                                               const JSONObject& params)
{
    try {
        JSON root(cJSON_Object);
        JSONObject e = root.getObject();
        e.addProperty("id", ++message_id);
        e.addProperty("method", command);
        if(params.isValid()) { e.arrayAppend(params); }
        wxString command = e.getString(false);
        gsgsDEBUG() << "-->" << command;
        socket.Send(command);
    } catch(gsgs::SocketException& e) {
        gsgsWARNING() << e.what();
    }
}

void NodeJSDevToolsProtocol::ProcessMessage(const wxString& msg, gsgs::WebSocketClient& socket)
{
    JSON root(msg);
    if(!root.isValid()) { return; }
    JSONObject rootElement = root.getObject();
    wxString method;
    rootElement.getObject("method").getValue(method);
    long replyId;
    rootElement.getObject("id").getValue(replyId, (long)wxNOT_FOUND);
    if(!method.IsEmpty()) {
        // Debugger event arrived
        NodeMessageBase::Ptr_t handler = m_handlers.GetHandler(method);
        if(handler) {
            gsgsDEBUG() << "Invoking handler:" << handler->GetEventName();
            handler->Process(socket, rootElement.getObject("params"));
        }
    } else if(replyId != wxNOT_FOUND) {
        if(rootElement.isObjectExist("result")) {
            // Invoke the action for this reply
            if(m_waitingReplyCommands.count(replyId)) {
                CommandHandler& handler = m_waitingReplyCommands[replyId];
                if(handler.action) { handler.action(rootElement.getObject("result")); }
                // Delete the handler
                m_waitingReplyCommands.erase(replyId);
            }
        }
    }
}

void NodeJSDevToolsProtocol::SendStartCommands(gsgs::WebSocketClient& socket)
{
    SendSimpleCommand(socket, "Runtime.enable");
    SendSimpleCommand(socket, "Debugger.enable");
    {
        JSONObject params = JSONObject::createObject("params");
        params.addProperty("state", "uncaught");
        SendSimpleCommand(socket, "Debugger.setPauseOnExceptions", params);
    }
    SendSimpleCommand(socket, "Runtime.runIfWaitingForDebugger");
}

void NodeJSDevToolsProtocol::Next(gsgs::WebSocketClient& socket) { SendSimpleCommand(socket, "Debugger.stepOver"); }

void NodeJSDevToolsProtocol::StepIn(gsgs::WebSocketClient& socket) { SendSimpleCommand(socket, "Debugger.stepInto"); }

void NodeJSDevToolsProtocol::StepOut(gsgs::WebSocketClient& socket) { SendSimpleCommand(socket, "Debugger.stepOut"); }

void NodeJSDevToolsProtocol::Continue(gsgs::WebSocketClient& socket) { SendSimpleCommand(socket, "Debugger.resume"); }

void NodeJSDevToolsProtocol::Clear() { m_waitingReplyCommands.clear(); }

void NodeJSDevToolsProtocol::SetBreakpoint(gsgs::WebSocketClient& socket, const NodeJSBreakpoint& bp)
{
    try {
        JSONObject params = bp.ToJSON("params");
        // Nodejs is using 0 based line numbers, while the editor starts from 1
        params.removeProperty("lineNumber");
        params.addProperty("lineNumber", bp.GetLine() - 1);
        // Send the command
        SendSimpleCommand(socket, "Debugger.setBreakpointByUrl", params);

        // Register a handler to handle this command when it returns
        CommandHandler handler(message_id, [=](const JSONObject& result) {
            wxString breakpointId;
            result.getObject("breakpointId").getValue(breakpointId);
            NodeJSBreakpoint& b = m_debugger->GetBreakpointsMgr()->GetBreakpoint(bp.GetFilename(), bp.GetLine());
            if(b.IsOk()) {
                b.SetNodeBpID(breakpointId);
                gsgs::DebugEvent bpEvent(wxEVT_NODEJS_DEBUGGER_UPDATE_BREAKPOINTS_VIEW);
                gsgs_Event().AddPendingEvent(bpEvent);
            }
        });
        m_waitingReplyCommands.insert({ handler.m_commandID, handler });
    } catch(gsgs::SocketException& e) {
        gsgsWARNING() << e.what();
    }
}

void NodeJSDevToolsProtocol::DeleteBreakpoint(gsgs::WebSocketClient& socket, const NodeJSBreakpoint& bp)
{
    try {
        JSONObject params = JSONObject::createObject("params");
        params.addProperty("breakpointId", bp.GetNodeBpID());
        // Send the command
        SendSimpleCommand(socket, "Debugger.removeBreakpoint", params);

        // Register a handler to handle this command when it returns
        CommandHandler handler(message_id, [=](const JSONObject& result) {
            gsgs::DebugEvent bpEvent(wxEVT_NODEJS_DEBUGGER_UPDATE_BREAKPOINTS_VIEW);
            gsgs_Event().AddPendingEvent(bpEvent);
        });
        m_waitingReplyCommands.insert({ handler.m_commandID, handler });
    } catch(gsgs::SocketException& e) {
        gsgsWARNING() << e.what();
    }
}

void NodeJSDevToolsProtocol::GetScriptSource(gsgs::WebSocketClient& socket, const wxString& scriptId)
{
    JSONObject params = JSONObject::createObject("params");
    params.addProperty("scriptId", scriptId);
    // Send the command
    SendSimpleCommand(socket, "Debugger.getScriptSource", params);
    // Register a handler to handle this command when it returns
    CommandHandler handler(message_id, [=](const JSONObject& result) {
        wxString fileContent;
        result.getObject("scriptSource").getValue(fileContent);
        NodeFileManager::Get().CacheRemoteCopy(scriptId, fileContent);
    });
    m_waitingReplyCommands.insert({ handler.m_commandID, handler });
}

void NodeJSDevToolsProtocol::Eval(gsgs::WebSocketClient& socket, const wxString& expression, const wxString& frameId)
{
    JSONObject params = JSONObject::createObject("params");
    params.addProperty("callFrameId", frameId);
    params.addProperty("expression", expression);
    params.addProperty("generatePreview", true);
    SendSimpleCommand(socket, "Debugger.evaluateOnCallFrame", params);
    // Register a handler to handle this command when it returns
    CommandHandler handler(message_id, [=](const JSONObject& result) {
        if(result.isObjectExist("result")) {
            nSerializableObject::Ptr_t ro(new RemoteObject());
            ro->To<RemoteObject>()->SetExpression(expression);
            ro->To<RemoteObject>()->FromJSON(result.getObject("result"));
            clDebugRemoteObjectEvent evt(wxEVT_NODEJS_DEBUGGER_EVAL_RESULT);
            evt.SetRemoteObject(ro);
            gsgs_Event().AddPendingEvent(evt);
        }
    });
    m_waitingReplyCommands.insert({ handler.m_commandID, handler });
}

void NodeJSDevToolsProtocol::GetObjectProperties(gsgs::WebSocketClient& socket, const wxString& objectId,
                                                 wxEventType eventType)
{
    JSONObject params = JSONObject::createObject("params");
    params.addProperty("objectId", objectId);
    SendSimpleCommand(socket, "Runtime.getProperties", params);
    // Register a handler to handle this command when it returns
    CommandHandler handler(message_id, [=](const JSONObject& result) {
        if(result.isObjectExist("result")) {
            gsgs::DebugEvent evt(eventType);
            evt.SetString(result.getObject("result").getString(false));
            evt.SetStartupCommands(objectId);
            gsgs_Event().AddPendingEvent(evt);
        }
    });
    m_waitingReplyCommands.insert({ handler.m_commandID, handler });
}

void NodeJSDevToolsProtocol::CreateObject(gsgs::WebSocketClient& socket, const wxString& expression,
                                          const wxString& frameId)
{
    // Same as eval but without the preview
    JSONObject params = JSONObject::createObject("params");
    params.addProperty("callFrameId", frameId);
    params.addProperty("expression", expression);
    params.addProperty("generatePreview", false);
    SendSimpleCommand(socket, "Debugger.evaluateOnCallFrame", params);
    // Register a handler to handle this command when it returns
    CommandHandler handler(message_id, [=](const JSONObject& result) {
        if(result.isObjectExist("result")) {
            nSerializableObject::Ptr_t ro(new RemoteObject());
            ro->To<RemoteObject>()->SetExpression(expression);
            ro->To<RemoteObject>()->FromJSON(result.getObject("result"));
            clDebugRemoteObjectEvent evt(wxEVT_NODEJS_DEBUGGER_CREATE_OBJECT);
            evt.SetRemoteObject(ro);
            gsgs_Event().AddPendingEvent(evt);
        }
    });
    m_waitingReplyCommands.insert({ handler.m_commandID, handler });
}

void NodeJSDevToolsProtocol::DeleteBreakpointByID(gsgs::WebSocketClient& socket, const wxString& bpid)
{
    try {
        JSONObject params = JSONObject::createObject("params");
        params.addProperty("breakpointId", bpid);
        // Send the command
        SendSimpleCommand(socket, "Debugger.removeBreakpoint", params);
    } catch(gsgs::SocketException& e) {
        gsgsWARNING() << e.what();
    }
}
