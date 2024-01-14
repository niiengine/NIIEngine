/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/

#include "LLDBNetworkListenerThread.h"
#include "LLDBReply.h"
#include "LLDBEvent.h"
#include "gsgsLogManager.h"

LLDBNetworkListenerThread::LLDBNetworkListenerThread(wxEvtHandler* owner, const LLDBPivot& pivot, int fd)
    : wxThread(wxTHREAD_JOINABLE)
    , m_owner(owner)
{
    m_socket.reset(new gsgs::SocketBase(fd));
    m_pivot = pivot;
}

LLDBNetworkListenerThread::~LLDBNetworkListenerThread() { Stop(); }

void* LLDBNetworkListenerThread::Entry()
{
    while(!TestDestroy()) {
        wxString msg;
        try {
            if(m_socket->ReadMessage(msg, 1) == gsgs::SocketBase::kSuccess) {
                LLDBReply reply(msg);
                reply.UpdatePaths(m_pivot);
                switch(reply.GetReplyType()) {
                case kReplyTypeInterperterReply: {
                    LLDBEvent event(wxEVT_LLDB_INTERPERTER_REPLY);
                    event.SetString(reply.GetText());
                    m_owner->AddPendingEvent(event);
                    break;
                }
                case kReplyTypeDebuggerStartedSuccessfully: {
                    // notify debugger started successfully
                    LLDBEvent event(wxEVT_LLDB_STARTED);
                    event.SetSessionType(reply.GetDebugSessionType());
                    m_owner->AddPendingEvent(event);
                    break;
                }

                case kReplyTypeDebuggerExited: {
                    // notify debugger exited
                    LLDBEvent event(wxEVT_LLDB_EXITED);
                    m_owner->AddPendingEvent(event);
                    break;
                }

                case kReplyTypeDebuggerStopped: {
                    // notify debugger exited
                    LLDBEvent event(wxEVT_LLDB_STOPPED);
                    event.SetFileName(reply.GetFilename());
                    event.SetLinenumber(reply.GetLine());
                    event.SetInterruptReason(reply.GetInterruptResaon());
                    event.SetBacktrace(reply.GetBacktrace());
                    event.SetThreads(reply.GetThreads());
                    m_owner->AddPendingEvent(event);
                    break;
                }

                case kReplyTypeDebuggerRunning: {
                    // notify debugger exited
                    LLDBEvent event(wxEVT_LLDB_RUNNING);
                    m_owner->AddPendingEvent(event);
                    break;
                }
                
                case kReplyTypeLaunchSuccess: {
                    // notify debugger exited
                    LLDBEvent event(wxEVT_LLDB_LAUNCH_SUCCESS);
                    m_owner->AddPendingEvent(event);
                    break;
                }
                
                case kReplyTypeDebuggerStoppedOnFirstEntry: {
                    // notify debugger exited
                    LLDBEvent event(wxEVT_LLDB_STOPPED_ON_FIRST_ENTRY);
                    m_owner->AddPendingEvent(event);
                    break;
                }

                case kReplyTypeAllBreakpointsDeleted: {
                    LLDBEvent event(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL);
                    m_owner->AddPendingEvent(event);
                    break;
                }

                case kReplyTypeBreakpointsUpdated: {
                    LLDBEvent event(wxEVT_LLDB_BREAKPOINTS_UPDATED);
                    event.SetBreakpoints(reply.GetBreakpoints());
                    m_owner->AddPendingEvent(event);
                    break;
                }

                case kReplyTypeLocalsUpdated: {
                    LLDBEvent event(wxEVT_LLDB_LOCALS_UPDATED);
                    event.SetVariables(reply.GetVariables());
                    m_owner->AddPendingEvent(event);
                    break;
                }

                case kReplyTypeVariableExpanded: {
                    LLDBEvent event(wxEVT_LLDB_VARIABLE_EXPANDED);
                    event.SetVariables(reply.GetVariables());
                    event.SetVariableId(reply.GetLldbId());
                    m_owner->AddPendingEvent(event);
                    break;
                }

                case kReplyTypeExprEvaluated: {
                    LLDBEvent event(wxEVT_LLDB_EXPRESSION_EVALUATED);
                    event.SetVariables(reply.GetVariables());
                    event.SetExpression(reply.GetExpression());
                    m_owner->AddPendingEvent(event);
                    break;
                }
                }
            }
        } catch(gsgs::SocketException& e) {
            CL_WARNING("Seems like we lost connection to codelite-lldb (probably crashed): %s", e.what().c_str());
            LLDBEvent event(wxEVT_LLDB_CRASHED);
            m_owner->AddPendingEvent(event);

            // exit the main loop
            break;
        }
    }
    return NULL;
}
