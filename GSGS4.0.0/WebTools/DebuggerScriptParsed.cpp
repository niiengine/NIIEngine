#include "DebuggerScriptParsed.h"
#include "NodeFileManager.h"
#include "NodeJSDevToolsProtocol.h"

DebuggerScriptParsed::DebuggerScriptParsed()
    : NodeMessageBase("Debugger.scriptParsed")
{
}

DebuggerScriptParsed::~DebuggerScriptParsed() {}

void DebuggerScriptParsed::Process(gsgs::WebSocketClient& socket, const JSONObject& json)
{
    wxString scriptId;
    wxString url;
    json.getObject("scriptId").getValue(scriptId);
    json.getObject("url").getValue(url);
    if(url.IsEmpty()) { return; }
    NodeFileManager::Get().AddFile(scriptId, url);
    // Check that this file exists locally
    if(!NodeFileManager::Get().IsFileExists(scriptId)) {
        // Request a copy of the file content
        NodeJSDevToolsProtocol::Get().GetScriptSource(socket, scriptId);
    }
}

NodeMessageBase::Ptr_t DebuggerScriptParsed::Clone() { return NodeMessageBase::Ptr_t(new DebuggerScriptParsed()); }
