/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-29

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
    public:
        enum ProcessStage
        {
            PS_Request,
            PS_Process,
            PS_Response
        };
    public:
        Job(RequestID id, void * data = 0, NCount retry = 0);
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
        bool isStop() const;

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        inline RequestID getID() const { return mID; }

        /** 获取内部数据
        @version NIIEngine 3.0.0
        */
        inline void * getData() const { return mData; }

        /** 设置重试次数
        @version NIIEngine 3.0.0
        */
        void setRetryCount(NCount c);

        /** 获取重试次数
        @version NIIEngine 3.0.0
        */
        NCount getRetryCount() const;

        /** 设置任务超时
        @remark 任务从开始历经的时间超过参数的将自动结束任务
        @param[in] ms 默认为0,表示没有时间限制
        @version nIIEngine 3.0.0
        */
        void setTimeOut(TimeDurMS ms);

        /** 获取任务超时
        @remark 任务从开始历经的时间超过参数的将自动结束任务
        @return 默认为0,表示没有时间限制.单位:毫秒
        @version NIIEngine 3.0.0
        */
        TimeDurMS getTimeOut() const;

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
        virtual NIIf getRelUsage();

        /** 获取绝对使用率
        @version NIIEngine 3.0.0
        */
        virtual Nui64 getAbsUsage();
    protected:
        Job();
        
        /** 周期断点到达时发生
        @version NIIEngine 3.0.0
        */
        virtual void onProcess();

        /** 属性/状态改变时发生
        @version NIIEngine 3.0.0
        */
        virtual void onNotify();
    protected:
        RequestID mID;
        TimeDurMS mTimeOut;
        PrcLevel mLevel;
        NCount mRetryCount;
        volatile ProcessStage mStage;
        void * mData;
        bool mAborted;
        bool mAlone;
    };
    
    /// 反馈结构.
    class _EngineAPI JobResult : public JobAlloc
    {
        friend class ThreadManager;
    public:
        JobResult(Job * src, void * data, bool complete, const String & msg = StrUtil::WBLANK);
        virtual ~JobResult();

        /** 是否完成任务
        @version NIIEngine 3.0.0
        */
        inline bool isComplete() const { return mSuccess; }

        /** 所处理的任务
        @version NIIEngine 3.0.0
        */
        inline const Job * getJob() const { return mJob; }

        /** 返回"回馈"数据
        @version NIIEngine 3.0.0 高级api
        */
        inline void * getData() const { return mData; }

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
        void * mData;
        String mMessages;
        bool mSuccess;
        bool mAbort;
        bool mRetry;
    };
    
    /** 任务处理
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JobPrc
    {
        friend class ThreadManager;
    public:
        JobPrc(bool autodestroy = false) : mAutoDestroy(autodestroy) {}
        virtual ~JobPrc() {}

        /** 处理任务
        @version NIIEngine 3.0.0
        */
        virtual JobResult * handle(Job * jop) = 0;

        /** 回馈处理
        @version NIIEngine 3.0.0
        */
        virtual void handle(JobResult * result) = 0;
    protected:
        bool mAutoDestroy;
    };
}
#endif