#include "CallFrame.h"
#include "DebuggerPaused.h"
#include "NodeJSEvents.h"
#include "gsgsEvent.h"
#include <vector>
#include "gsgsLogManager.h"

DebuggerPaused::DebuggerPaused()
    : NodeMessageBase("Debugger.paused")
{
}

DebuggerPaused::~DebuggerPaused() {}

void DebuggerPaused::Process(gsgs::WebSocketClient& socket, const JSONObject& json)
{
    json.getObject("reason").getValue(m_stopReason);
    JSONObject frames = json.getObject("callFrames");
    nSerializableObject::Vec_t V;
    int size = frames.getArraySize();
    for(int i = 0; i < size; ++i) {
        JSONObject frame = frames.getArray(i);
        nSerializableObject::Ptr_t f(new CallFrame());
        f->FromJSON(frame);
        V.push_back(f);
    }
    
    wxString extraMessage;
    if(json.isObjectExist("data")) {
        JSONObject data = json.getObject("data");
        if(data.isObjectExist("description")) {
            data.getObject("description").getValue(extraMessage);
        }
    }
    // Notify the UI that the debugger paused
    gsgs::DebugEvent pauseEvent(wxEVT_NODEJS_DEBUGGER_INTERACT);
    pauseEvent.SetString(m_stopReason);
    pauseEvent.SetArguments(extraMessage);
    pauseEvent.SetAnswer(true);
    gsgs_Event().ProcessEvent(pauseEvent);

    // Notify the UI to update the view
    clDebugCallFramesEvent callFrameEvent(wxEVT_NODEJS_DEBUGGER_UPDATE_CALLSTACK);
    callFrameEvent.SetCallFrames(V);
    gsgs_Event().ProcessEvent(callFrameEvent);
}

NodeMessageBase::Ptr_t DebuggerPaused::Clone() { return NodeMessageBase::Ptr_t(new DebuggerPaused()); }
