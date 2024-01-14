#include "DebuggerResumed.h"
#include "NodeJSEvents.h"
#include "gsgsEvent.h"
#include "gsgsEvent.h"

DebuggerResumed::DebuggerResumed()
    : NodeMessageBase("Debugger.resumed")
{
}

DebuggerResumed::~DebuggerResumed() {}

void DebuggerResumed::Process(gsgs::WebSocketClient& socket, const JSONObject& json)
{
    // Notify the UI that the debugger paused
    gsgs::DebugEvent resumeEvent(wxEVT_NODEJS_DEBUGGER_INTERACT);
    resumeEvent.SetString("");
    resumeEvent.SetAnswer(false);
    gsgs_Event().ProcessEvent(resumeEvent);
}

NodeMessageBase::Ptr_t DebuggerResumed::Clone() { return NodeMessageBase::Ptr_t(new DebuggerResumed()); }
