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

#include "NiiPreProcess.h"
#include "NiiTBBThreadManager.h"
#include "NiiLogManager.h"
#include "NiiEngine.h"
#include "NiiRenderSys.h"

namespace NII
{
    /// �̷߳���
    class _EngineInner TBBThread : public Thread
    {
    public:
        TBBThread(ThreadManager * q):
            mQueue(q) {}

        /// tbbר��
        void run()
        {
            mQueue->main();
        }

        ThreadManager * mQueue;
    };
    //------------------------------------------------------------------------
    struct RegisterRSWorker
    {
        RegisterRSWorker(TBBThreadMananger * queue) :
            mQueue(queue) {}

        void operator() ()
        {
            mQueue->_registerThreadWithRenderSystem();
        }

        TBBThreadMananger * mQueue;
    };
    //------------------------------------------------------------------------
    TBBThreadMananger::TBBThreadMananger(const String & name) :
        ThreadManager(name),
        mTaskScheduler(tbb::task_scheduler_init::deferred){}
    //------------------------------------------------------------------------
    void TBBThreadMananger::startup(bool force)
    {
        if(mRun)
        {
            if(force)
                shutdown();
            else
                return;
        }

        mShutDown = false;

        mWorkerFunc = N_new_t(TBBThread(this));

        N_Only(Log).stream() << _T("TBBThreadMananger('") << mName <<
            _T("') initialising.");

        mTaskScheduler.initialize(mFireCount);

        RegisterRSWorker worker(this);
        // ��ǰ�̲߳���Ҫ��ע��
        mRegisteredThreads.insert(tbb::this_tbb_thread::get_id());
        while(mRegisteredThreads.size() < mFireCount)
        {
            // ��������,֪�����й����̰߳�����ע�ᵽ��Ⱦϵͳ��
            for(NCount i = 0; i < mFireCount * 3; ++i)
            {
                mTaskGroup.run(worker);
            }
            mTaskGroup.wait();
        }

        mRun = true;
    }
    //------------------------------------------------------------------------
    void TBBThreadMananger::_registerThreadWithRenderSystem()
    {
        {
            N_lock_mutex(mRegisterRSMutex);
            // ��ȡ��ǰ�߳�
            tbb::tbb_thread::id cur = tbb::this_tbb_thread::get_id();
            // ���ظ��ذ��߳�ע���ȥ
            if(mRegisteredThreads.find(cur) == mRegisteredThreads.end())
            {
                RenderSys * rsys;
                N_Engine().get(rsys);
                rsys->registerThread();
                mRegisteredThreads.insert(cur);
            }
        }

        tbb::this_tbb_thread::yield();
    }
    //------------------------------------------------------------------------
    TBBThreadMananger::~TBBThreadMananger()
    {
        shutdown();
    }
    //------------------------------------------------------------------------
    void TBBThreadMananger::shutdown()
    {
        N_Only(Log).stream() << _T("TBBThreadMananger('") << mName <<
            _T("') shutting down.");

        mShutDown = true;
        mTaskGroup.cancel();
        abort();

        // �ȴ����е��������
        mTaskGroup.wait();

        if(mTaskScheduler.is_active())
            mTaskScheduler.terminate();

        if(mWorkerFunc)
        {
            N_delete_t(mWorkerFunc, TBBThread);
            mWorkerFunc = 0;
        }
        mRun = false;
    }
    //------------------------------------------------------------------------
    void TBBThreadMananger::main()
    {
        // ����������.����һ����һ����.
        FireThreadPrc();
    }
    //------------------------------------------------------------------------
    void TBBThreadMananger::notifyFire()
    {
        // ����һ���µ�����
        mTaskGroup.run(*mWorkerFunc);
    }
    //------------------------------------------------------------------------
}