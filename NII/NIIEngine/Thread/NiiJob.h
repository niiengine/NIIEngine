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

#ifndef _NII_JOB_H_
#define _NII_JOB_H_

#include "NiiPreInclude.h"
#include "NiiCommon.h"

namespace NII
{
    /** 工作类,可以理解为一个具体任务的工作线程
    @remark
        这个类和引擎里使用的 Pattern 很像,而 Pattern 更像是轮询概念,而这个类运行起
        来并不能完整的将操作控制在准确的某个点上再或者说是超出了引擎的控制范围,所以有
        实质性的区别.
    @note
        还有一点就是这个类不应该理解成 ThreadMain 概念体,在这款引擎中 ThreadMain 被
        理解成一个具体的线程任务,而这个类或许可以同时由多个相同或不同的具体线程控制
    */
    class _EngineAPI Job : public JobAlloc
    {
        friend class ThreadManager;
		friend class JobResult;
    public:
		enum ProcessStage
		{
			PS_Request,
			PS_Process,
			PS_Response
		};

        class Identifier : public JobAlloc
        {
        public:
            virtual ~Identifier(){}
            
            virtual bool isOnly() const { return true; }
        };
    public:
        Job(NCount retry, RequestID id = 0);
        virtual ~Job();

        /** 执行任务
        @remark 运行任务的执行核心
        @version NIIEngine 3.0.0
        */
        virtual void process();

        /** 暂停任务
        @version NIIEngine 3.0.0
        */
        virtual void pause();

        /** 停止任务
        @version NIIEngine 3.0.0
        */
        virtual void stop();

        /** 任务是否被停止
        @version NIIEngine 3.0.0
        */
        bool isStop() const             {return mAborted;}

        /** 获取ID
        @note 如果返回0则代表任务没有启动
        @version NIIEngine 3.0.0
        */
        inline RequestID getID() const  { return mID; }

        /** 设置重试次数
        @version NIIEngine 3.0.0
        */
        void setRetryCount(NCount c)    { mRetryCount = c; }

        /** 获取重试次数
        @version NIIEngine 3.0.0
        */
        NCount getRetryCount() const    { return mRetryCount; }

        /** 设置任务超时
        @remark 任务从开始历经的时间超过参数的将自动结束任务
        @param[in] ms 默认为0,表示没有时间限制
        @version nIIEngine 3.0.0
        */
        void setTimeOut(TimeDurMS ms)   { mTimeOut = ms; }

        /** 获取任务超时
        @remark 任务从开始历经的时间超过参数的将自动结束任务
        @return 默认为0,表示没有时间限制.单位:毫秒
        @version NIIEngine 3.0.0
        */
        TimeDurMS getTimeOut() const    { return mTimeOut; }

        /** 设置是否使用独立的线程
        @note 仅在任务未处理前设置有效
        @version NIIEngine 3.0.0
        */
        virtual void setAlone(bool s);

        /** 返回是否使用独立的线程
        @note 仅在任务未处理前设置有效
        @version NIIEngine 3.0.0
        */
        virtual bool isAlone() const;

        /** 设置任务的优先级
        @version NIIEngine 3.0.0
        */
        virtual void setLevel(PrcLevel l);

        /** 获取任务的优先级
        @version NIIEngine 3.0.0
        */
        virtual PrcLevel getLevel() const;

        /** 获取相对使用率
        @version NIIEngine 3.0.0
        */
        virtual NIIf getRelUsage() const;

        /** 获取绝对使用率
        @version NIIEngine 3.0.0
        */
        virtual Nui64 getAbsUsage() const;
        
        /** 是否相等概念
        @version NIIEngine 4.0.0
        */
        virtual bool equal(const Identifier & id);
    protected:
        Job();
        
        /** 周期断点到达时触发
        @version NIIEngine 3.0.0
        */
        virtual void onProcess();

        /** 属性/状态改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onNotify();
        
        /** 完成时触发
        @version NIIEngine 4.0.0
        */
        virtual void onComplete();
        
        /** 触发
        @version NIIEngine 4.0.0
        */
        virtual void onFail();
        
        /** 重试时触发
        @version NIIEngine 4.0.0
        */
        virtual void onRetry();
    protected:
        RequestID mID;
		Nui32 mTypeID;
        TimeDurMS mTimeOut;
        PrcLevel mLevel;
        NCount mRetryCount;
        volatile ProcessStage mStage;
        bool mAborted;
        bool mAlone;
    };
    
    /// 工作反馈结构
    class _EngineAPI JobResult : public JobAlloc
    {
        friend class ThreadManager;
    public:
        JobResult(Job * src, bool complete = true, const String & tips = StrUtil::WBLANK);
        virtual ~JobResult();

        /** 是否完成任务
        @version NIIEngine 3.0.0
        */
        inline bool isComplete() const { return mSuccess; }

        /** 所处理的任务
        @version NIIEngine 3.0.0
        */
        inline const Job * getJob() const { return mJob; }

        /** 获取回馈信息
        @version NIIEngine 3.0.0
        */
        inline const String & getMessages() const { return mMessages; }

        /** 终止请求
        @version NIIEngine 3.0.0
        */
        void abort();

        /** 尝试再次执行任务
        @version NIIEngine 3.0.0
        */
        void retry(JobPrc * prc);
    private:
        Job * mJob;
        String mMessages;
        bool mSuccess;
        bool mAbort;
        bool mRetry;
    };
    
    /** 工作处理结构
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JobPrc
    {
        friend class ThreadManager;
    public:
        JobPrc(bool autodestroy) : mAutoDestroy(autodestroy) {}
        virtual ~JobPrc() {}
        
        /** 是否自动删除
        @version NIIEngine 3.0.0
        */
        inline bool isAutoDestroy() const { return mAutoDestroy; }
    protected:
        JobPrc() : mAutoDestroy(true) {}
	protected:
        /** 处理任务
        @return 如果没有需要主线程处理的事务则返回0
        @version NIIEngine 3.0.0
        */
        virtual JobResult * handle(Job * jop) = 0;

        /** 处理回馈
        @version NIIEngine 3.0.0
        */
        virtual void handle(JobResult * result){}

        /** 处理任务前
        @version NIIEngine 3.0.0
        */
        virtual void onPrcBegin(Job * jop){}

        /** 处理任务后
        @version NIIEngine 3.0.0
        */
        virtual void onPrcEnd(Job * jop){}
    protected:
        bool mAutoDestroy;
    };
}
#endif