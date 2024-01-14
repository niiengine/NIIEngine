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
#include "NiiBoostThreadManager.h"
#include "NiiLogManager.h"
#include "NiiEngine.h"
#include "NiiRenderSys.h"

namespace NII
{
    /// 线程方法
    class _EngineInner BoostThread : public Thread
    {
    public
        BoostThread(ThreadManager * q):
            mQueue(q) {}

        /// boost poco 专用
        void operator()()
        {
            mQueue->main();
        }

        ThreadManager * mQueue;
    };
    //---------------------------------------------------------------------
    BoostThreadManager::BoostThreadManager(const String & name) :
        ThreadManager(name)
    {
    }
    //---------------------------------------------------------------------
    BoostThreadManager::~BoostThreadManager()
    {
        shutdown();
    }
    //---------------------------------------------------------------------
    void BoostThreadManager::startup(bool force)
    {
        if(mRun)
        {
            if(force)
                shutdown();
            else
                return;
        }

        mShutDown = false;
        // 创建工作处理函数
        mWorkerFunc = N_new_t(BoostThread(this));

        N_Only(Log).stream() <<
            _T("BoostThreadManager('") << mName << _T("') initialising on thread ") <<
#if NII_THREAD_SUPPORT
            NII_THREAD_CURRENT_ID
#else
            "main"
#endif
            << ".";

#if NII_THREAD_SUPPORT
        mNumThreadsRegisteredWithRS = 0;
        // 限定线程不多于8个
        for(Nui8 i = 0; i < mFireCount; ++i)
        {   // 创建工作线程
            NII_THREAD_CREATE(t, *mWorkerFunc);
            mWorkers.push_back(t);
        }

        NII_LOCK_MUTEX_NAMED(mInitMutex, initLock)
        // 等待所有线程注册到渲染系统,然后再设置图形设备上下文,在此之前所有线程都听留在这里
        while(mNumThreadsRegisteredWithRS < mFireCount)
            NII_THREAD_WAIT(mInitSync, mInitMutex, initLock);
#endif
        mRun = true;
    }
    //---------------------------------------------------------------------
    void BoostThreadManager::notifyThreadRegistered()
    {
        N_lock_mutex(mInitMutex)

        ++mNumThreadsRegisteredWithRS;

        // 唤醒主线程
        NII_THREAD_NOTIFY_ALL(mInitSync);
    }
    //---------------------------------------------------------------------
    void BoostThreadManager::shutdown()
    {
        if(!mRun)
            return;

        N_Only(Log).stream() <<
            _T("BoostThreadManager('") << mName << _T("') shutting down on thread ") <<
#if NII_THREAD_SUPPORT
            NII_THREAD_CURRENT_ID
#else
            "main"
#endif
            << ".";

        mShutDown = true;
        abort();
#if NII_THREAD_SUPPORT
        // wake all threads (they should check shutting down as first thing after wait)
        NII_THREAD_NOTIFY_ALL(mRequestCondition)

        // all our threads should have been woken now, so join
        WorkerThreadList::iterator i, iend = mWorkers.end();
        for(i = mWorkers.begin(); i != iend; ++i)
        {
            (*i)->join();
            NII_THREAD_DESTROY(*i);
        }
        mWorkers.clear();
#endif
        if(mWorkerFunc)
        {   // 删除工作处理函数
            N_delete_t(mWorkerFunc, BoostThread);
            mWorkerFunc = 0;
        }

        mRun = false;
    }
    //---------------------------------------------------------------------
    void BoostThreadManager::notifyFire()
    {
        // 唤起等待工作的线程
        NII_THREAD_NOTIFY_ONE(mRequestCondition)
    }
    //---------------------------------------------------------------------
    void BoostThreadManager::waitForNextRequest()
    {
#if NII_THREAD_SUPPORT
        // Lock; note that NII_THREAD_WAIT will free the lock
        NII_LOCK_MUTEX_NAMED(mRequestMutex, queueLock);
        if(mRequestList.empty())
        {
            // frees lock and suspends the thread
            NII_THREAD_WAIT(mRequestCondition, mRequestMutex, queueLock);
        }
        // When we get back here, it's because we've been notified
        // and thus the thread has been woken up. Lock has also been
        // re-acquired, but we won't use it. It's safe to try processing and fail
        // if another thread has got in first and grabbed the request
#endif
    }
    //---------------------------------------------------------------------
    void BoostThreadManager::main()
    {
        // default worker thread
#if NII_THREAD_SUPPORT
        N_Only(Log).stream() << _T("BoostThreadManager('") << getName() <<
            _T("')::BoostThread - thread ") << NII_THREAD_CURRENT_ID << _T(" starting.");

        // Initialise the thread for RS if necessary
        if()
        {
            RenderSys * rsys;
            N_Engine().get(rsys);
            rsys->registerThread();   // 注册线程
            notifyThreadRegistered(); // 通知注册了
        }

        // Spin forever until we're told to shut down
        while(!mShutDown)
        {
            waitForNextRequest();   // 等待任务
            FireThreadPrc();  // 处理任务
        }

        N_Only(Log).stream() << _T("BoostThreadManager('") << getName() <<
            _T("')::BoostThread - thread ") << NII_THREAD_CURRENT_ID << _T(" stopped.");
#endif
    }
    //---------------------------------------------------------------------
    /*  join就是阻塞调用线程，直到这个线程结束后，调用线程才能继续执行。该影响只存
        在于执行join方法的线程和调用这个线程的线程之间如在t1线程中调用t2.join(),
        则需要t2线程执行完后t1方能继续执行

        sleep()是让某个线程暂停运行一段时间,其控制范围是由当前线程决定,也就是说,
        在线程里面决定.好比如说,我要做的事情是 "点火->烧水->煮面",而当我点完火之
        后我不立即烧水,我要休息一段时间再烧.对于运行的主动权是由我的流程来控制.

        而wait(),首先,这是由某个确定的对象来调用的,将这个对象理解成一个传话的人,
        当这个人在某个线程里面说"暂停!",也是 thisOBJ.wait(),这里的暂停是阻塞,还
        是"点火->烧水->煮饭",thisOBJ就好比一个监督我的人站在我旁边,本来该线 程应
        该执行1后执行2,再执行3,而在2处被那个对象喊暂停,那么我就会一直等在这里而不
        执行3,但正个流程并没有结束,我一直想去煮饭,但还没被允许, 直到那个对象在某
        个地方说"通知暂停的线程启动!",也就是thisOBJ.notify()的时候,那么我就可以
        煮饭了,这个被暂停的线程就会从暂停处继续执行.

        notify_one用于唤醒一个等待该condition发生的线程，notify_all则用于唤醒所
        有等待该condition发生的线程。
    */
}