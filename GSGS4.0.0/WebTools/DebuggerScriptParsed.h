#ifndef DEBUGGERSCRIPTPARSED_H
#define DEBUGGERSCRIPTPARSED_H

#include "NodeMessageBase.h" // Base class: NodeMessageBase

class DebuggerScriptParsed : public NodeMessageBase
{
    wxString m_stopReason;

public:
    DebuggerScriptParsed();
    virtual ~DebuggerScriptParsed();

public:
    void Process(gsgs::WebSocketClient& socket, const JSONObject& json);
    NodeMessageBase::Ptr_t Clone();
};

#endif // DEBUGGERSCRIPTPARSED_H
