#include "clDebugCallFramesEvent.h"

// ------------------------------------------------
// clDebugCallFramesEvent
// ------------------------------------------------

clDebugCallFramesEvent::clDebugCallFramesEvent(const clDebugCallFramesEvent& event) { *this = event; }

clDebugCallFramesEvent::clDebugCallFramesEvent(wxEventType commandType, int winid)
    : gsgs::DebugEvent(commandType, winid)
{
}

clDebugCallFramesEvent::~clDebugCallFramesEvent() {}

clDebugCallFramesEvent& clDebugCallFramesEvent::operator=(const clDebugCallFramesEvent& src)
{
    // Call parent operator =
    gsgs::DebugEvent::operator=(src);
    m_callFrames = src.m_callFrames;
    return *this;
}
