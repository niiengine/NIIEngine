/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-6-6

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

#ifndef _NII_THREAD_MANAGER_H_
#define _NII_THREAD_MANAGER_H_

#include "NiiPreInclude.h"
//#include "NiiThread.h"
#include "NiiAtomic.h"
#include "NiiSingleton.h"

namespace NII
{
    /** 线程管理器
    @remark
    @note 一般是2个体系 Win32 和 Posix
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ThreadManager : public ThreadMain, public Singleton<ThreadManager>, public JobAlloc
    {
        friend class Engine;
    public:
        ThreadManager();
        ~ThreadManager();
        
        /** 交换后台处理数据到主线程(主处理机制)
        @version NIIEngine 4.0.0
        */
        void update();
        
        /** 是否启动
        @version NIIEngine 4.0.0
        */
        bool isRun() const;
        
        /** 是否处于工作状态
        @version NIIEngine 4.0.0
        */
        bool isWorking() const;
        
        /** 启动
        @version NIIEngine 4.0.0
        */
        bool startup(int threadCnt);

        /** 关闭
        @version NIIEngine 4.0.0
        */
        void shutdown();

        /** 暂停处理
        @version NIIEngine 4.0.0
        */
        bool pause();

        /** 恢复处理
        @version NIIEngine 4.0.0
        */
        void resume();
        
        /** 设置更新时限
        @version NIIEngine 4.0.0
        */
        void setUpdateTimeOut(TimeDurMS ms);
        
        /** 获取更新时限
        @version NIIEngine 4.0.0
        */
        TimeDurMS getUpdateTimeOut() const;
        
        /** 添加任务
        @version NIIEngine 4.0.0
        */
        RequestID add(Job * job, JobPrc * prc, bool sync);

        /** 
        @version NIIEngine 4.0.0
        */
        void add(JobResult * result);

        /**
        @version NIIEngine 4.0.0
        */
        NCount getOutputSize() const { ScopeLock temp(mOutMutex); return mResponseList.size();}

        /**
        @version NIIEngine 4.0.0
        */
        NCount getInputSize() const { ScopeLock temp(mInMutex); return mRequestList.size(); }

        /** 获取所有输出
        @version NIIEngine 4.0.0
        */
        void removeAllInput();
        
        /** 获取所有输出
        @version NIIEngine 4.0.0
        */
        void removeAllOutput();
        
        /** 获取输入
        @version NIIEngine 4.0.0
        */
        Job * getInput(Nui32 index){ ScopeLock temp(mInMutex); return mRequestList[index].first; }

        /** 移去输入
        @version NIIEngine 4.0.0
        */
        void removeInput(Nui32 index);

        /** 获取输出
        @version NIIEngine 4.0.0
        */
        JobResult * getOutput(Nui32 index){ScopeLock temp(mOutMutex); return mResponseList[index].first;}

        /** 移去输出
        @version NIIEngine 4.0.0
        */
        void removeOutput(Nui32 index);
        
        /** 终止请求
        @version NIIEngine 3.0.0
        */
        void abort(RequestID id);

        /** 终止请求
        @version NIIEngine 3.0.0
        */
        void abort(JobPrc * prc);
        
        /** 终止请求
        @version NIIEngine 3.0.0
        */
        void abort();

        /** 获取工作中的线程数量
        @version NIIEngine 4.0.0
        */
        NCount getWorkingCount() const;
        
        /// @copydetails Singleton::getOnly
        static ThreadManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static ThreadManager * getOnlyPtr();
    protected:
        /// @coypdetals ThreadMain::run
        void run(void * arg);
        
        /** 处理任务
        @version NIIEngine 3.0.0 inner
        */
        void process(Job * obj, JobPrc * jp, bool syn);
    protected:
        typedef list<std::pair<Job *, JobPrc *> >::type PrcMap;
        typedef list<std::pair<JobResult *, JobPrc *> >::type ResponseMap;
    protected:
        ThreadList mThreadList;
        RequestID mValidJobID;
        PrcMap mRequestList;
        ResponseMap mResponseList;      
        mutable ThreadMutex mInMutex;
        mutable ThreadMutex mOutMutex;
        mutable ThreadMutex mExistMutex;
        mutable ThreadMutex mQuitMutex;
        ThreadCondition mPauseCondition;
        ThreadCondition mExistCondition;
        ThreadCondition mQuitCondition;
        TimeDurMS mUpdateTimeOut;
        volatile NCount mThreadCount;
        volatile NCount mRunThreadCount;
        volatile bool mRun;
        volatile bool mPause;        
    };
}
#endif