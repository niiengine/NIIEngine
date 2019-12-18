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

#ifndef _NII_Thread_Condition_h_
#define _NII_Thread_Condition_h_

#include "NiiPreInclude.h"
#include <condition_variable>
#include <atomic>

namespace NII
{
    /** 线程条件触发器
    @version NIIEngine 3.0.0
    */
    class ThreadCondition
    {
    public:
        ThreadCondition() = default;

        void wait(ThreadMutex & mutex);

        bool wait(ThreadMutex & mutex, Nui64 ms);

        inline void wait(ScopeLock & lock) { wait(lock.getMutex()); }

        inline bool wait(ScopeLock & lock, Nui64 ms) { return wait(lock.getMutex(), ms); }

        inline void notify()            { mCondition.notify_one(); }

        inline void notifyAll()         { mCondition.notify_all(); }
    private:
        ThreadCondition(const ThreadCondition & o)              {}
        ThreadCondition & operator=(const ThreadCondition & o)  {}
    private:
        std::condition_variable_any mCondition;
    };

    /** 线程事件条件触发器
    @version NIIEngine 3.0.0
    */
    class ThreadConditionEvent
    {
    public:
        ThreadConditionEvent(ThreadCondition & cond, atomic_bool & signal) : mCondition(cond), mSignal(signal) {}

        void wait(ThreadMutex & mutex);
        bool wait(ThreadMutex & mutex, Nui64 ms);

        inline void notifyAll() { mCondition.notifyAll(); }
        inline void notify() { mCondition.notify(); }
    private:
        atomic_bool & mSignal;
        ThreadCondition & mCondition;
    };
}
#endif