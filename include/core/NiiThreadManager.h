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

#ifndef _NII_THREAD_MANAGER_H_
#define _NII_THREAD_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiThread.h"
#include "NiiJob.h"
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
        typedef list<std::pair<Job *, JobPrc *> >::type PrcMap;
        typedef list<std::pair<JobResult *, JobPrc *> >::type ResponseMap;

        /** 任务组
        @version NIIEngine 4.0.0
        */
        class TaskGroup : public ThreadMain
        {
        public:
			TaskGroup();
			~TaskGroup();

            /** 启动
            @version NIIEngine 4.0.0
            */
            bool startup(NCount threadCnt, ThreadMain * task, void * arg);

            /** 关闭
            @version NIIEngine 4.0.0
            */
            bool shutdown();

            /** 交换后台处理数据到主线程(主处理机制)
            @version NIIEngine 4.0.0
            */
            void update();

            /** 暂停处理
            @version NIIEngine 4.0.0
            */
            bool pause();

            /** 恢复处理
            @version NIIEngine 4.0.0
            */
            void resume();

            /** 是否启动
            @version NIIEngine 4.0.0
            */
            bool isRun() const;

            /** 是否处于工作状态
            @version NIIEngine 4.0.0
            */
			bool isWorking() const;

            /** 设置更新时限
            @version NIIEngine 4.0.0
            */
            inline void setUpdateTimeOut(TimeDurMS ms) { mUpdateTimeOut = ms; }

            /** 获取更新时限
            @version NIIEngine 4.0.0
            */
            inline TimeDurMS getUpdateTimeOut() const { return mUpdateTimeOut; }

            /** 添加任务
            @version NIIEngine 4.0.0
            */
            RequestID add(Job * job, JobPrc * prc, bool sync);

			/** 移去任务
			@version NIIEngine 4.0.0
			*/
			void remove(const Job::Identifier & id);

            /** 终止请求
            @version NIIEngine 4.0.0
            */
            void abort(RequestID id);

            /** 终止请求
            @version NIIEngine 4.0.0
            */
            void abort(JobPrc * prc);

            /** 终止请求
            @version NIIEngine 4.0.0
            */
            void abort();

            /** 获取输出数量
            @version NIIEngine 4.0.0
            */
            NCount getOutputSize() const;

            /** 获取输入数量
            @version NIIEngine 4.0.0
            */
            NCount getInputSize() const;

            /** 获取所有输出
            @version NIIEngine 4.0.0
            */
            void removeAllInput();

            /** 获取所有输出
            @version NIIEngine 4.0.0
            */
            void removeAllOutput();

            /** 获取工作中的线程数量
            @version NIIEngine 4.0.0
            */
            NCount getWorkingCount() const;
        protected:
            /// @copydetails ThreadMain::run
            void run(void * arg);

            /** 处理任务
            @version NIIEngine 3.0.0 inner
            */
            void process(Job * obj, JobPrc * jp, bool syn);

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
    public:
        ThreadManager();
        ~ThreadManager();

        /** 交换后台处理数据到主线程(主处理机制)
        @version NIIEngine 4.0.0
        */
        void update(Nui32 type = 0);

        /** 是否启动
        @version NIIEngine 4.0.0
        */
        bool isRun(Nui32 type = 0) const;

        /** 是否处于工作状态
        @version NIIEngine 4.0.0
        */
        bool isWorking(Nui32 type = 0) const;

        /** 启动
        @version NIIEngine 4.0.0
        */
        bool startup(Nui32 type, NCount threadCnt, ThreadMain * task, void * arg = 0);

        /** 关闭
        @version NIIEngine 4.0.0
        */
		bool shutdown(Nui32 type);

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
        void setUpdateTimeOut(Nui32 type, TimeDurMS ms);

        /** 获取更新时限
        @version NIIEngine 4.0.0
        */
        TimeDurMS getUpdateTimeOut(Nui32 type) const;

        /** 添加任务
        @version NIIEngine 4.0.0
        */
        RequestID add(Nui32 type, Job * job, JobPrc * prc, bool sync = false);

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
        void abort(Nui32 type, RequestID id);

        /** 终止请求
        @version NIIEngine 3.0.0
        */
        void abort(Nui32 type, JobPrc * prc);

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
        typedef map<Nui32, TaskGroup *>::type GroupList;

		mutable ThreadRWMutex mGroupMutex;
        GroupList mGroupList;
    };
}
#endif