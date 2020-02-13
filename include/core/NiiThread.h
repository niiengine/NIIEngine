/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-1-7

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_THREAD_H_
#define _NII_THREAD_H_

#include "NiiPreInclude"
#include "NiiThreadEvent.h"

#define TWait_Interrupt     -1
#define TWait_Signaled      0 
#define TWait_Timeout       1

namespace NII
{
    inline void Nsleep(TimeDurMS ms)
    {
    #ifdef _WIN32
        Sleep(ms);
    #else
        usleep(ms * 1000);
    #endif
    }

    /** �߳�������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ThreadMain : public JobAlloc
    {
        friend class Thread;
    public:
        virtual ~ThreadMain() {}
        virtual void cancel() {}
        virtual bool isLoop() const { return false; }
    protected:
        virtual void run(void * arg) = 0;
    
        /** ����������ǰ����
        @version NIIEngine 3.0.0
        */
        virtual void onPrcBegin(void *){}
        
        /** �����������󴥷�
        @version NIIEngine 3.0.0
        */
        virtual void onPrcEnd(void *){}
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
        bool isAutoDestroy() const;

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
        ThreadMain * getThreadMain() const;

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
        virtual NIIi abort(ThreadEvent & event, TimeDurMst timeout = 0);

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
    protected:
        String mName;
        std::thread * mThread;
        ThreadMain * mMain;
        void * mMainArg;
        ThreadMutex mMutex;
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
    class _EngineAPI GroupThreadMain : public JobAlloc
    {
    public:
        virtual ~GroupThreadMain();
        
        /** �������
        @version NIIEngine 3.0.0
        */
        void addMain(ThreadMain * task, void * arg = 0, bool autoDestroy = true);
        
        /// @copydetails ThreadMain::run
        virtual void run();
        
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
        typedef vector<Thread *>::type ThreadList;
        ThreadList mThreadList;
    };
}

#endif