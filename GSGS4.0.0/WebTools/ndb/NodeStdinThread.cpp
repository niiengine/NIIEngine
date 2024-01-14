#include "NodeStdinThread.h"
#include <wx/crt.h>

wxDEFINE_EVENT(wxEVT_STDIN_MESSAGE, gsgs::Event);
wxDEFINE_EVENT(wxEVT_STDIN_EXIT, gsgs::Event);
NodeStdinThread::NodeStdinThread(wxEvtHandler* owner)
    : m_owner(owner)
{
}

NodeStdinThread::~NodeStdinThread() {}

void* NodeStdinThread::Entry()
{
    while(!TestDestroy()) {
        char command[1024];
        memset(command, 0, sizeof(command));
        if(fgets(command, sizeof(command), stdin)) {
            wxString buffer;
            buffer << command;
            if(buffer.Contains(".exit")) { break; }
            gsgs::Event evt(wxEVT_STDIN_MESSAGE);
            evt.SetString(command);
            m_owner->AddPendingEvent(evt);
        }
    }
    gsgs::Event evt_exit(wxEVT_STDIN_EXIT);
    m_owner->AddPendingEvent(evt_exit);
    return NULL;
}

void NodeStdinThread::Write(const wxString& command) { wxFprintf(stdin, "%s\n", command); }
