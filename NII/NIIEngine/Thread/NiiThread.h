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

#ifndef _NII_THREAD_H_
#define _NII_THREAD_H_

#include "NiiPreInclude.h"
#include "NiiThreadLock.h"
#include <initializer_list>

#define TWait_Interrupt     -1
#define TWait_Signaled      0 
#define TWait_Timeout       1

#ifdef _WIN32
#define Nsleep(ms) Sleep(ms);
#else
#define Nsleep(ms) usleep(ms * 1000);
#endif

namespace NII
{
	class _EngineAPI ThreadEvent
	{
		friend class ThreadEventGroup;
	public:
		ThreadEvent();
		ThreadEvent(bool autoreset, bool signal_);

		inline void reset() { ScopeLock lock(mMutex); mSignal = false; }

		void set();

		inline bool isSignal() const { ScopeLock lock(mMutex); return mSignal; }

		bool wait(Nui64 ms);

		bool wait();

		inline NCount getWaitCount() const { ScopeLock lock(mMutex); return mCount; }
	protected:
		ThreadEvent(const ThreadEvent &) : mConditionEvent(mCondition, mSignal) {}
		ThreadEvent & operator=(const ThreadEvent &) { return  *this; }

		void addGroup(ThreadEventGroup * group);

		void removeGroup(ThreadEventGroup * group);
	protected:
		ThreadEventGroupList mGroupList;
		mutable ThreadMutex mMutex;
		ThreadMutex mGroupMutex;
		NCount mCount;
		ThreadCondition mCondition;
		ThreadConditionEvent mConditionEvent;
		volatile bool mSignal;
		bool mAutoReset;
	};

	class _EngineAPI ThreadEventGroup
	{
	public:
		ThreadEventGroup(std::initializer_list<ThreadEvent *> events);

		~ThreadEventGroup();

		inline void set(ThreadEvent * e) { ScopeLock l(mMutex); mEvent = e; mSignal = true; mConditionEvent.notifyAll(); }

		ThreadEvent * wait();

		ThreadEvent * wait(Nui64 ms);

		inline NCount getWaitCount() const { ScopeLock lock(mMutex); return mCount; }
	protected:
		ThreadEventGroup(const ThreadEventGroup &) : mConditionEvent(mCondition, mSignal) {}
		ThreadEventGroup & operator=(const ThreadEventGroup &) { return *this; }
	protected:
		ThreadEventList mEventList;
		mutable ThreadMutex mMutex;
		NCount mCount;
		ThreadEvent * mEvent;
		ThreadCondition mCondition;
		ThreadConditionEvent mConditionEvent;
		volatile bool mSignal;
	};

    /** 线程主函数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ThreadMain : virtual public JobAlloc
    {
        friend class Thread;
		friend class GroupThreadMain;
    public:
        virtual ~ThreadMain();
        virtual void cancel()           {}
        virtual bool isLoop() const     { return false; }
    protected:
        virtual void run(void * arg) = 0;
    
        /** 主函数运行前触发
        @version NIIEngine 3.0.0
        */
        virtual void onPrcBegin(void *) {}
        
        /** 主函数运行被中断后触发
        @remark Begin-> Interrupt -> End
        @version NIIEngine 3.0.0
        */
        virtual void onPrcInterrupt(void *) {}
        
        /** 主函数运行被暂停后触发
        @remark Begin-> Suspend -> (Go on) or (Stop -> End)
        @version NIIEngine 3.0.0
        */
        virtual void onPrcSuspend(void *) {}
        
        /** 主函数运行被停止后触发
        @remark Begin-> Stop -> End
        @version NIIEngine 3.0.0
        */
        virtual void onPrcStop(void *) {}
        
        /** 主函数结束后触发
        @version NIIEngine 3.0.0
        */
        virtual void onPrcEnd(void *)   {}
    };

    /** 线程
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Thread : public JobAlloc
    {
    public:
        Thread(const String & name, ThreadMain * task, void * arg = 0, bool autoDestroyMain = true);
        virtual ~Thread();

        /** 创建实例
        @version NIIEngine 3.0.0
        */
        void create(bool autoDestroy = false);

        /** 挂起
        @version NIIEngine 3.0.0
        */
        void sleep(TimeDurMS ms);

        /** 等待执行完成
        @version NIIEngine 3.0.0
        */
        bool join(TimeDurMS ms);

        /** 停止
        @version NIIEngine 3.0.0
        */
        virtual void stop(bool bWait = true);

        /** 当前调用线程
        @version NIIEngine 3.0.0
        */
        bool isCurrent() const;

        /** 自动删除
        @version NIIEngine 3.0.0
        */
        bool isAutoDestroy() const      {return mAutoDestroy;}

        /** 获取是否正在运行
        @version NIIEngine 3.0.0
        */
        bool isRun() const;

        /** 获取相对使用率
        @version NIIEngine 3.0.0
        */
        NIIf getRelUsage();

        /** 获取绝对使用率
        @version NIIEngine 3.0.0
        */
        Nui64 getAbsUsage();

        /** 设置优先级
        @version NIIEngine 3.0.0
        */
        bool setPriority(NIIi level);

        /** 获取优先级
        @version NIIEngine 3.0.0
        */
        NIIi getPriority() const;
        
        /** 获取线程主函数
        @version NIIEngine 3.0.0
        */
        ThreadMain * getThreadMain() const      {return mMain;}

        /** 获取优先级(最小)
        @version NIIEngine 3.0.0
        */
        static NIIi getPriorityMin();

        /** 获取优先级(最大)
        @version NIIEngine 3.0.0
        */
        static NIIi getPriorityMax();

        /** 获取优先级(普通)
        @version NIIEngine 3.0.0
        */
        static NIIi getPriorityNormal();

        /** 获取当前线程
        @version NIIEngine 3.0.0
        */
        static Thread * getCurrent();

        /** 获取当前线程ID
        @version NIIEngine 3.0.0
        */
        static std::thread::id getCurrentId();

        /** 获取当前线程ID
        @version NIIEngine 3.0.0
        */
        static uint64_t getCurrentNativeId();

        /** 获取当前线程句柄
        @version NIIEngine 3.0.0
        */
        static std::uintptr_t getCurrentNativeHandle();
    protected:
        Thread(const String & name);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void process();

        /**
        @version NIIEngine 3.0.0
        */
        virtual NIIi abort(ThreadEvent & event, TimeDurMS timeout = 0);

        /** 
        @version NIIEngine 3.0.0
        */
        virtual void onStart() {}

        /** 
        @version NIIEngine 3.0.0
        */
        virtual void onException() {}

        /** 
        @version NIIEngine 3.0.0
        */
        virtual void onExit() {}
    protected:
        void action();

		virtual void actionImpl();
    protected:
        String mName;
        std::thread * mThread;
        ThreadMain * mMain;
        void * mMainArg;
        mutable ThreadMutex mMutex;
        ThreadId mID;
        std::future<bool> mRun;
        Nui64 mLastUsage;
        Nui64 mLastUsageTime;
        NIIf mLastRelUsage;
        ThreadEvent mStopEvent;
        ThreadEvent mStartEvent;
        STbool mStop;
        bool mAutoDestroy;
        bool mMainAutoDestroy;
    };
    
    /** 线程主函数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GroupThreadMain : public ThreadMain
    {
    public:
		GroupThreadMain();
        virtual ~GroupThreadMain();
        
        /** 添加任务
        @version NIIEngine 3.0.0
        */
        void addMain(ThreadMain * task, void * arg = 0, bool autoDestroy = true);
        
        /// @copydetails ThreadMain::run
        virtual void run(void * arg);
        
        /// @copydetails ThreadMain::cancel
        virtual void cancel();
    protected:
        class _EngineAPI Task : public JobAlloc
        {
        public:
            Task(ThreadMain * task, void * arg = 0, bool autoDestroy = true) :
                mTask(task),
                mArg(arg),
                mAutoDestroy(autoDestroy){}
        public:  
            ThreadMain * mTask;
            void * mArg;
            bool mAutoDestroy;
        };
        typedef list<Task *>::type ThreadMainList;
    protected:
        ThreadMainList mThreadMainList;
        ThreadCondition mThreadMainCondition;
        ThreadMutex mThreadMainMutex;
        bool mRun;
    };
    
    /** 线程池
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ThreadPool
    {
    public:
        ThreadPool(){}
        ~ThreadPool();
        
        /**
        @version NIIEngine 3.0.0
        */
        int setup(NCount threadcnt);
        
        /**
        @version NIIEngine 3.0.0
        */
        void addTask(ThreadMain * task, void * arg = 0, bool autoDestroy = true);
        
        /**
        @version NIIEngine 3.0.0
        */
        void shutdown();
    protected:
        ThreadList mThreadList;
    };
}

#endif