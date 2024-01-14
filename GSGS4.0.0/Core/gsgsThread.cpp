#include "gsgsThread.h"
#include "gsgsEvent.h"

namespace gsgs
{
    //--------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------
    // Thread
    //--------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------
    Thread::Thread()
        : wxThread(wxTHREAD_JOINABLE)
    {
    }
    //--------------------------------------------------------------------------------------------------
    Thread::~Thread() { Stop(); }
    //--------------------------------------------------------------------------------------------------
    void Thread::Stop()
    {
        // Notify the thread to exit and
        // wait for it
        if(IsAlive()) {
            Delete(NULL, wxTHREAD_WAIT_BLOCK);

        } else {
            Wait(wxTHREAD_WAIT_BLOCK);
        }
    }
    //--------------------------------------------------------------------------------------------------
    void Thread::Start(int priority)
    {
        Create();
        SetPriority(priority);
        Run();
    }
    //--------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------
    // WorkerThread
    //--------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------
    WorkerThread::WorkerThread()
        : wxThread(wxTHREAD_JOINABLE)
        , m_notifiedWindow(NULL)
        , mStop(true)
    {
    }
    //--------------------------------------------------------------------------------------------------
    WorkerThread::~WorkerThread() { Stop(); }
    //--------------------------------------------------------------------------------------------------
    bool WorkerThread::IsStop() const {
        return mStop;
    }
    //--------------------------------------------------------------------------------------------------
    void* WorkerThread::Entry()
    {
        while (true) {
            // Did we get a request to terminate?
            if (TestDestroy()) break;
            ThreadRequest* request = NULL;
            if (m_queue.ReceiveTimeout(50, request) == wxMSGQUEUE_NO_ERROR) {
                // Call user's implementation for processing request
                ProcessRequest(request);
                wxDELETE(request);
            }
        }
        return NULL;
    }
    //--------------------------------------------------------------------------------------------------
    void WorkerThread::Add(ThreadRequest* request)
    {
        if (!request) { return; }
        m_queue.Post(request);
    }
    //--------------------------------------------------------------------------------------------------
    void WorkerThread::Stop()
    {
        mStop = true;
        // Notify the thread to exit and
        // wait for it
        if (IsAlive()) {
            Delete(NULL, wxTHREAD_WAIT_BLOCK);

        }
        else {
            Wait(wxTHREAD_WAIT_BLOCK);
        }
    }
    //--------------------------------------------------------------------------------------------------
    void WorkerThread::Start(int priority)
    {
        Create();
        SetPriority(priority);
        mStop = false;
        Run();
    }
    //--------------------------------------------------------------------------------------------------
    #ifndef __WXMSW__
    #include <sys/wait.h>
    #include "gsgsLogManager.h"
    //--------------------------------------------------------------------------------------------------
    wxDEFINE_EVENT(wxEVT_CL_PROCESS_TERMINATED, wxProcessEvent);
    //--------------------------------------------------------------------------------------------------
    ZombieReaperPOSIX::ZombieReaperPOSIX()
        : wxThread(wxTHREAD_JOINABLE)
    {
        gsgs_Debug("ZombieReaperPOSIX: started");
    }
    //--------------------------------------------------------------------------------------------------
    ZombieReaperPOSIX::~ZombieReaperPOSIX()
    {

    }
    //--------------------------------------------------------------------------------------------------
    void* ZombieReaperPOSIX::Entry()
    {
        while (!TestDestroy()) {
            int status(0);
            pid_t pid = ::waitpid((pid_t)-1, &status, WNOHANG);
            if (pid > 0) {
                // Notify about this process termination
                wxProcessEvent event(0, pid, status);
                event.SetEventType(wxEVT_CL_PROCESS_TERMINATED);
                gsgs_Event().AddPendingEvent(event);
                gsgs_Debug("ZombieReaperPOSIX: process %d exited with status code %d", (int)pid, status);
            }
            wxThread::Sleep(50);
        }

        gsgs_Debug("ZombieReaperPOSIX: going down");
        return NULL;
    }
    //--------------------------------------------------------------------------------------------------
    void ZombieReaperPOSIX::Stop()
    {
        if (IsAlive()) {
            Delete(NULL, wxTHREAD_WAIT_BLOCK);
        }
        else {
            Wait(wxTHREAD_WAIT_BLOCK);
        }
    }
    //--------------------------------------------------------------------------------------------------
    #endif
}