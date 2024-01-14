#ifndef NODESTDINTHREAD_H
#define NODESTDINTHREAD_H

#include <gsgs::Thread.h>
#include <gsgsEvent.h>
#include <gsgsPreInclude.h>

class NodeStdinThread : public gsgs::Thread
{
    wxEvtHandler* m_owner = nullptr;

public:
    NodeStdinThread(wxEvtHandler* owner);
    virtual ~NodeStdinThread();

    void* Entry();
    void Write(const wxString& command);
};
wxDECLARE_EVENT(wxEVT_STDIN_MESSAGE, gsgs::Event);
wxDECLARE_EVENT(wxEVT_STDIN_EXIT, gsgs::Event);
#endif // NODESTDINTHREAD_H
