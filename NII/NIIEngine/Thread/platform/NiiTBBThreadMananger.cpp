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
    /// 线程方法
    class _EngineInner TBBThread : public Thread
    {
    public:
        TBBThread(ThreadManager * q):
            mQueue(q) {}

        /// tbb专用
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
        // 当前线程不需要被注册
        mRegisteredThreads.insert(tbb::this_tbb_thread::get_id());
        while(mRegisteredThreads.size() < mFireCount)
        {
            // 孵化任务,知道所有工作线程把他们注册到渲染系统中
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
            // 获取当前线程
            tbb::tbb_thread::id cur = tbb::this_tbb_thread::get_id();
            // 不重复地把线程注册进去
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

        // 等待所有的任务完成
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
        // 任务主函数.处理一个单一请求.
        FireThreadPrc();
    }
    //------------------------------------------------------------------------
    void TBBThreadMananger::notifyFire()
    {
        // 创建一个新的任务
        mTaskGroup.run(*mWorkerFunc);
    }
    //------------------------------------------------------------------------
}