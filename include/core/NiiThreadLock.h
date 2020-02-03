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

#ifndef _NII_THREADLOCK_H_
#define _NII_THREADLOCK_H_

#include "NiiPreInclude.h"
#include <mutex>
#include <atomic>

using namespace std;

namespace NII
{
    typedef STbool              STbool;
    typedef atomic_int16_t      STNi16;
    typedef atomic_uint16_t     STNui16;
    typedef atomic_int32_t      STNi32;
    typedef atomic_uint32_t     STNui32;
    typedef std::atomic<NCount> STNCount;
    typedef std::atomic<NIIf>   STNIIf;
    
    /** 线程互斥
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ThreadMutex
    {
        friend class ThreadCondition;
    public:
        ThreadMutex() : mCount(0){}

        inline void lock() { mMutex.lock(); mCount++; }

        void lock(NCount cnt);

        inline bool tryLock() { return mMutex.tryLock() ? mCount++, true : false; }

        inline void unlock() { mCount--; mMutex.unlock(); }

        NCount unlock(NCount cnt);

        inline std::recursive_mutex & getMutex() { return mMutex; }
    protected:
        std::recursive_mutex mMutex;
        NCount mCount;
    };

    /** 线程互斥锁
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScopeLock
    {
    public:
        explicit ScopeLock(ThreadMutex & mutex) : mMutex(mutex), mCurrent(true) { mMutex.lock(); }

        ~ScopeLock() { if (mCurrent) mMutex.unlock(); }
    public:
        inline bool isCurrent() const { return mCurrent; }

        inline void lock() { mMutex.lock(); mCurrent = true; }

        inline bool tryLock() { return (mCurrent = mMutex.tryLock()); }

        inline void unlock() { if (mCurrent) { mMutex.unlock(); mCurrent = false; } }

        inline ThreadMutex & getMutex() { return mMutex; }
    protected:
        ScopeLock(const ScopeLock & o) {}
        ScopeLock & operator=(const ScopeLock & o) {}
    private:
        ThreadMutex & mMutex;
        bool mCurrent;
    };

    /** 线程互斥解锁
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScopeUnlock
    {
    public:
        explicit ScopeUnlock(ThreadMutex & mutex) : mMutex(mutex) { mCount = mMutex.unlock(0); }
        ~ScopeUnlock() { mMutex.lock(mCount); }
    protected:
        ThreadMutex & mMutex;
        NCount mCount;
    };

    /** 线程读写互斥
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ThreadRWMutex
    {
    public:
        inline ThreadRWMutex() : mCount(0), mSignal(true), mConditionEvent(mCondition, mSignal) {}

        inline void lockW() { ScopeLock smutex(mMutex); while (mCount) mConditionEvent.wait(mMutex); mMutex.lock(); }
        inline bool tryLockW() { return (mMutex.tryLock() ? ((mCount == 0) ? true : (mMutex.unlock(), false)) : false); }
        inline void unlockW() { mMutex.unlock(); }

        inline void lockR() { ScopeLock smutex(mMutex); mCount++; mSignal = false; }
        inline bool tryLockR() { return (mMutex.tryLock() ? mCount++, mSignal = false, mMutex.unlock(), true : false); }
        inline void unlockR() { ScopeLock smutex(mMutex); mCount--; if (!mCount) { mSignal = true; mConditionEvent.notifyAll(); } }
    protected:
        ThreadMutex mMutex;
        ThreadCondition mCondition;
        ThreadConditionEvent mConditionEvent;
        STbool mSignal;
        NCount mCount;
    };

    /** 线程读写互斥锁
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScopeRWLock
    {
    public:
        explicit ScopeRWLock(ThreadRWMutex & mutex, bool write = false) : mMutex(mutex), mCurrent(true), mWrite(write)
        {
            if (mWrite)
                mMutex.lockR();
            else
                mMutex.lockW();
        }

        ~ScopeRWLock()
        {
            if (mCurrent)
            {
                if (mWrite)
                    mMutex.unlockR();
                else
                    mMutex.unlockW();
            }
        }

        inline void lock()
        {
            if (mWrite)
                mMutex.lockR();
            else
                mMutex.lockW();
            mCurrent = true;
        }

        inline bool tryLock()
        {
            if (mWrite)
                mCurrent = mMutex.tryLockR();
            else
                mCurrent = mMutex.tryLockW();
            return mCurrent;
        }

        inline void unlock()
        {
            if (mCurrent)
            {
                if (mWrite)
                    mMutex.unlockR();
                else
                    mMutex.unlockW();
                mCurrent = false;
            }
        }

        inline bool isCurrent() const { return mCurrent; }

        inline ThreadRWMutex & getMutex() { return mMutex; }
    protected:
        ScopeRWLock(const ScopeRWLock & o) {}
        ScopeRWLock & operator=(const ScopeRWLock & o) {}
    protected:
        ThreadRWMutex & mMutex;
        bool mCurrent;
        bool mWrite;
    };
    
    class _EngineAPI ScopeSpinLock
    {
    public:
        explicit ScopeSpinLock(std::atomic_flag & lock);
        ~ScopeSpinLock();
    private:
        std::atomic_flag & mLock;
    };
}
#endif