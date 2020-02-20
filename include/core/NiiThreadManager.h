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
    class _EngineAPI ThreadManager : public Singleton<ThreadManager>, public JobAlloc
    {
        friend class Engine;
    public:
        ThreadManager();
        ~ThreadManager();
        
        /** 交换后台处理数据到主线程(主处理机制)
        @version NIIEngine 4.0.0
        */
        void update(Nui32 type);
        
        /** 是否启动
        @version NIIEngine 4.0.0
        */
        bool isRun(Nui32 type) const;
        
        /** 是否处于工作状态
        @version NIIEngine 4.0.0
        */
        bool isWorking(Nui32 type) const;
        
        /** 启动
        @version NIIEngine 4.0.0
        */
        bool startup(Nui32 type, int threadCnt, ThreadMain * task, void * arg = 0);

        /** 关闭
        @version NIIEngine 4.0.0
        */
        void shutdown(Nui32 type);

        /** 暂停处理
        @version NIIEngine 4.0.0
        */
        bool pause(Nui32 type);

        /** 恢复处理
        @version NIIEngine 4.0.0
        */
        void resume(Nui32 type);
        
        /** 设置更新时限
        @version NIIEngine 4.0.0
        */
        void setUpdateTimeOut(TimeDurMS ms);
        
        /** 获取更新时限
        @version NIIEngine 4.0.0
        */
        TimeDurMS getUpdateTimeOut(Nui32 type) const;
        
        /** 添加任务
        @version NIIEngine 4.0.0
        */
        RequestID add(Nui32 type, Job * job, JobPrc * prc, bool sync);
        
        /** 移去输入
        @version NIIEngine 4.0.0
        */
        void remove(Nui32 type, const Job::Identifier & id);
        
        /** 获取输入数量
        @version NIIEngine 4.0.0
        */
        NCount getInputSize(Nui32 type) const;
        
        /** 获取输出数量
        @version NIIEngine 4.0.0
        */
        NCount getOutputSize(Nui32 type) const;

        /** 获取所有输出
        @version NIIEngine 4.0.0
        */
        void removeAllInput(Nui32 type = 0);
        
        /** 获取所有输出
        @version NIIEngine 4.0.0
        */
        void removeAllOutput(Nui32 type = 0);

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
        void abort(Nui32 type);

        /** 获取工作中的线程数量
        @version NIIEngine 4.0.0
        */
        NCount getWorkingCount(Nui32 type) const;
        
        /// @copydetails Singleton::getOnly
        static ThreadManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static ThreadManager * getOnlyPtr();
    protected:
        /** 处理任务
        @version NIIEngine 3.0.0 inner
        */
        void process(Job * obj, JobPrc * jp, bool syn);
    protected:
        typedef list<std::pair<Job *, JobPrc *> >::type PrcMap;
        typedef list<std::pair<JobResult *, JobPrc *> >::type ResponseMap;

        class ThreadMainGroup : public ThreadMain
        {
        public:
            ThreadMainGroup():
                
            /// @copydetails ThreadMain::run
            void run(void * arg);
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
        
        typedef map<Nui32, ThreadMainGroup *>::type GroupList;
    protected:
        ThreadMutex mGroupMutex;
        GroupList mGroupList;
    };
}
#endif