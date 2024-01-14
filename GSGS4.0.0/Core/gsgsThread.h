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
#ifndef _GSGS_THREAD_H_
#define _GSGS_THREAD_H_

#include "gsgsPreInclude.h"
#include <wx/thread.h>
#include <wx/process.h>
#include <wx/msgqueue.h>
#include <thread>

namespace gsgs
{
    class _gsgsAPI Thread : public wxThread
    {
    public:
        Thread();
        virtual ~Thread();

        void Start(int priority = WXTHREAD_DEFAULT_PRIORITY);
    protected:
        void Stop();
    };

    #ifndef __WXMSW__

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CL_PROCESS_TERMINATED, wxProcessEvent);

    class _gsgsAPI ZombieReaperPOSIX : public wxThread
    {
    public:
        virtual void* Entry();
        ZombieReaperPOSIX();
        virtual ~ZombieReaperPOSIX();

        void Start() {
            Create();
            Run();
        }

        void Stop();
    };
    #endif 

    /**
     * Base class for thread requests,
     */
    class _gsgsAPI ThreadRequest
    {
    public:
        ThreadRequest(){};
        virtual ~ThreadRequest(){};
    };

    /**
     * Worker Thread class
     * usually user should define the ProcessRequest method
     */
    class _gsgsAPI WorkerThread : public wxThread
    {
    public:
        WorkerThread();
        virtual ~WorkerThread();

        bool IsStop() const;
        /**
         * Thread execution point.
         */
        virtual void* Entry();

        /**
         * Called when the thread exits
         * whether it terminates normally or is stopped with Delete() (but not when it is Kill()'ed!)
         */
        virtual void OnExit(){};

        /**
         * Add a request to the worker thread
         * \param request request to execute.
         */
        void Add(ThreadRequest* request);

        /**
         * Set the window to be notified when a change was done
         * between current source file tree and the actual tree.
         * \param evtHandler
         */
        inline void SetNotifyWindow(wxEvtHandler* prc)          { m_notifiedWindow = prc; }

        inline wxEvtHandler* GetNotifiedWindow() const          { return m_notifiedWindow; }
        /**
         * Stops the thread
         * This function returns only when the thread is terminated.
         * \note This call must be called from the context of other thread (e.g. main thread)
         */
        void Stop();

        /**
         * Start the thread as joinable thread.
         * \note This call must be called from the context of other thread (e.g. main thread)
         */
        void Start(int priority = WXTHREAD_DEFAULT_PRIORITY);

        /**
         * Process request from the other thread
         * \param request ThreadRequest object to process
         */
        virtual void ProcessRequest(ThreadRequest* request) = 0;
    protected:
        wxEvtHandler* m_notifiedWindow;
        wxMessageQueue<ThreadRequest*> m_queue;
        bool mStop;
    };
}
#endif
