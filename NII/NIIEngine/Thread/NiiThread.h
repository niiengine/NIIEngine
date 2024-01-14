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

    /** �߳�������
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
    
        /** ����������ǰ����
        @version NIIEngine 3.0.0
        */
        virtual void onPrcBegin(void *) {}
        
        /** ���������б��жϺ󴥷�
        @remark Begin-> Interrupt -> End
        @version NIIEngine 3.0.0
        */
        virtual void onPrcInterrupt(void *) {}
        
        /** ���������б���ͣ�󴥷�
        @remark Begin-> Suspend -> (Go on) or (Stop -> End)
        @version NIIEngine 3.0.0
        */
        virtual void onPrcSuspend(void *) {}
        
        /** ���������б�ֹͣ�󴥷�
        @remark Begin-> Stop -> End
        @version NIIEngine 3.0.0
        */
        virtual void onPrcStop(void *) {}
        
        /** �����������󴥷�
        @version NIIEngine 3.0.0
        */
        virtual void onPrcEnd(void *)   {}
    };

    /** �߳�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Thread : public JobAlloc
    {
    public:
        Thread(const String & name, ThreadMain * task, void * arg = 0, bool autoDestroyMain = true);
        virtual ~Thread();

        /** ����ʵ��
        @version NIIEngine 3.0.0
        */
        void create(bool autoDestroy = false);

        /** ����
        @version NIIEngine 3.0.0
        */
        void sleep(TimeDurMS ms);

        /** �ȴ�ִ�����
        @version NIIEngine 3.0.0
        */
        bool join(TimeDurMS ms);

        /** ֹͣ
        @version NIIEngine 3.0.0
        */
        virtual void stop(bool bWait = true);

        /** ��ǰ�����߳�
        @version NIIEngine 3.0.0
        */
        bool isCurrent() const;

        /** �Զ�ɾ��
        @version NIIEngine 3.0.0
        */
        bool isAutoDestroy() const      {return mAutoDestroy;}

        /** ��ȡ�Ƿ���������
        @version NIIEngine 3.0.0
        */
        bool isRun() const;

        /** ��ȡ���ʹ����
        @version NIIEngine 3.0.0
        */
        NIIf getRelUsage();

        /** ��ȡ����ʹ����
        @version NIIEngine 3.0.0
        */
        Nui64 getAbsUsage();

        /** �������ȼ�
        @version NIIEngine 3.0.0
        */
        bool setPriority(NIIi level);

        /** ��ȡ���ȼ�
        @version NIIEngine 3.0.0
        */
        NIIi getPriority() const;
        
        /** ��ȡ�߳�������
        @version NIIEngine 3.0.0
        */
        ThreadMain * getThreadMain() const      {return mMain;}

        /** ��ȡ���ȼ�(��С)
        @version NIIEngine 3.0.0
        */
        static NIIi getPriorityMin();

        /** ��ȡ���ȼ�(���)
        @version NIIEngine 3.0.0
        */
        static NIIi getPriorityMax();

        /** ��ȡ���ȼ�(��ͨ)
        @version NIIEngine 3.0.0
        */
        static NIIi getPriorityNormal();

        /** ��ȡ��ǰ�߳�
        @version NIIEngine 3.0.0
        */
        static Thread * getCurrent();

        /** ��ȡ��ǰ�߳�ID
        @version NIIEngine 3.0.0
        */
        static std::thread::id getCurrentId();

        /** ��ȡ��ǰ�߳�ID
        @version NIIEngine 3.0.0
        */
        static uint64_t getCurrentNativeId();

        /** ��ȡ��ǰ�߳̾��
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
    
    /** �߳�������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GroupThreadMain : public ThreadMain
    {
    public:
		GroupThreadMain();
        virtual ~GroupThreadMain();
        
        /** �������
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
    
    /** �̳߳�
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