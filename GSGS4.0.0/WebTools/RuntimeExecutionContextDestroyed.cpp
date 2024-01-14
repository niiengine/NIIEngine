#include "NodeJSEvents.h"
#include "RuntimeExecutionContextDestroyed.h"
#include "gsgsEvent.h"
#include "gsgsEvent.h"

RuntimeExecutionContextDestroyed::RuntimeExecutionContextDestroyed()
    : NodeMessageBase("Runtime.executionContextDestroyed")
{
}

RuntimeExecutionContextDestroyed::~RuntimeExecutionContextDestroyed() {}

NodeMessageBase::Ptr_t RuntimeExecutionContextDestroyed::Clone()
{
    return NodeMessageBase::Ptr_t(new RuntimeExecutionContextDestroyed());
}

void RuntimeExecutionContextDestroyed::Process(gsgs::WebSocketClient& socket, const JSONObject& json)
{
    gsgs::DebugEvent eventFinish(wxEVT_NODEJS_DEBUGGER_FINISHED);
    gsgs_Event().AddPendingEvent(eventFinish);
}
