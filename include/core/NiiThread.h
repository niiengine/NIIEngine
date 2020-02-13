/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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

    /** 线程主函数
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
    
        /** 主函数运行前触发
        @version NIIEngine 3.0.0
        */
        virtual void onPrcBegin(void *){}
        
        /** 主函数结束后触发
        @version NIIEngine 3.0.0
        */
        virtual void onPrcEnd(void *){}
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
        bool isAutoDestroy() const;

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
        ThreadMain * getThreadMain() const;

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
    
    /** 线程主函数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GroupThreadMain : public JobAlloc
    {
    public:
        virtual ~GroupThreadMain();
        
        /** 添加任务
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
        typedef vector<Thread *>::type ThreadList;
        ThreadList mThreadList;
    };
}

#endif