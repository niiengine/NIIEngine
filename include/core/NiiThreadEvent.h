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

#ifndef _NII_THREAD_EVENT_H_
#define _NII_THREAD_EVENT_H_

#include "NiiPreInclude.h"
#include <initializer_list>
#include <atomic>

namespace NII
{
    class _EngineAPI ThreadEvent
    {
        friend class ThreadEventGroup;
    public:
        ThreadEvent();
        ThreadEvent(bool autoreset = true, bool signal_);

        inline void reset() { ScopeLock lock(mMutex); mSignal = false; }

        void set();

        inline bool isSignal() const { ScopeLock lock(mMutex); return mSignal; }

        bool wait(Nui64 ms);

        bool wait();

        inline NCount getWaitCount() const { ScopeLock lock(mMutex); return mCount; }
    protected:
        ThreadEvent(const ThreadEvent & o) {}
        ThreadEvent & operator=(const ThreadEvent & o) {}

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
        ThreadEventGroup(const ThreadEventGroup & o) {}
        ThreadEventGroup & operator=(const ThreadEventGroup & o) {}
    protected:
        ThreadEventList mEventList;
        mutable ThreadMutex mMutex;        
        NCount mCount;
        ThreadEvent * mEvent;
        ThreadCondition mCondition;
        ThreadConditionEvent mConditionEvent;
        volatile bool mSignal;
    };
}

#endif
