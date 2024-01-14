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
#ifndef __jobqueue__
#define __jobqueue__

#include "gsgsPreInclude.h"
#include <wx/thread.h>
#include <wx/msgqueue.h>

extern _gsgsAPI const wxEventType wxEVT_CMD_JOB_STATUS;
extern _gsgsAPI const wxEventType wxEVT_CMD_JOB_STATUS_VOID_PTR;

namespace gsgs
{
    class _gsgsAPI Job
    {
    public:
        /**
         * @brief Construct Job object with optional parent event handler.
         * @param parent event handler class which would like to receive notifications of progress
         */
        Job(wxEvtHandler *parent = NULL);
        virtual ~Job();

        /**
         * @brief post string and int values to parent in a form of wxCommandEvent of type wxEVT_CMD_JOB_STATUS. the string can be accessed by using event.GetString() and the int
         * by calling event.GetInt().
         * this function has no affect if parnet is NULL
         * @param i
         * @param message
         */
        void Post(int i, const wxString &message);
        /**
         * @brief post void* to parent in a form of wxCommandEvent of type wxEVT_CMD_JOB_STATUS_VOID_PTR. the void* can be accessed by using event.GetClientData() method.
         * NB: User must free the void*
         * this function has no affect if parnet is NULL
         * @param i integer to send to parent
         */
        virtual void Post(void *ptr);
        /**
         * @brief overridable Process() method. If the Process() method is performs a long computations, it is advised to
         * to call thread->TestDestroy() to allow the thread to exit when requested
         * @param thread the thread that is currently running the job.
         */
        virtual void Process(wxThread *thread) = 0;
    protected:
        wxEvtHandler *m_parent;
    };

    class _gsgsAPI JobQueueWorker : public wxThread
    {
        wxMessageQueue<Job*>* m_queue;

    public:
        /// default ctor/dtor
        JobQueueWorker(wxMessageQueue<Job*>* queue);
        virtual ~JobQueueWorker();

    private:
        //declare this class as non copyable
        JobQueueWorker(const JobQueueWorker& rhs);
        JobQueueWorker& operator=(const JobQueueWorker& rhs);

    public:
        /**
         * @brief stop the running thread. usually called before calling JobQueue::Release()
         */
        void Stop();

        /**
         * @brief start the thread with given priority
         * @param priority thread priority (defaulted to WXTHREAD_DEFAULT_PRIORITY)
         */
        void Start(int priority = WXTHREAD_DEFAULT_PRIORITY);


        /**
         * @brief implementation of the wxThread pure virtual function Entry()
         * @sa wxThread
         */
        virtual void* Entry();

        /**
         * @brief the main code that processes the job
         * @param job job to process
         * @sa Job
         */
        virtual void ProcessJob(Job *job);
    };

    /**
     * @class JobQueue
     * @author Eran
     * @date 05/09/08
     * @file gsgsJob.h
     * @brief this class provides a convenient way of handling background tasks using Job objects
     *
     * @code
     * // somewhere in your application initialization
     * // start the job queue with 5 workers and default thread prioritization
     * m_jobQueue = new JobQueue();
     * m_jobQueue->Start(5);
     *
     * // whenever you need to add new job to be processed
     * // just add it
     * m_jobQueue->PushJob( new MyJob() );
     *
     * // Shutdown code:
     * m_jobQueue->Stop();
     * // free the resources allocated by the JobQueue and empty the queue
     * delete m_jobQueue;
     * @endcode
     *
     */
    class JobQueue
    {
        wxMessageQueue<Job*>         m_queue;
        std::vector<JobQueueWorker*> m_threads;

    public:
        JobQueue();
        virtual ~JobQueue();

        /**
         * @brief add job to the queue. all jobs must be constructed on the heap.
         * note that the job will be freed when processing is done by the job queue
         * @param job job to process allocated on the heap
         */
        virtual void PushJob(Job *job);

        /**
         * @brief
         * @param poolSize
         * @param priority
         */
        virtual void Start(size_t poolSize = 1, int priority = WXTHREAD_DEFAULT_PRIORITY);

        /**
         * @brief stop all workers threads
         */
        virtual void Stop();
    };

    /**
     * @class JobQueueSingleton
     * @author Eran
     * @date 05/09/08
     * @file gsgsJob.h
     * @brief a wrapper around the JobQueue class that provides the functionality of singleton
     */
    class _gsgsAPI JobQueueSingleton
    {
    public:
        static JobQueue *Instance();
        static void Release();

    private:
        JobQueueSingleton();
        virtual ~JobQueueSingleton();

    private:
        static JobQueue* gInstance;
    };
}
#endif // __jobqueue__
