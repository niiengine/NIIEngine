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

#ifndef _NII_THREADLOCK_H_
#define _NII_THREADLOCK_H_

#include "NiiPreInclude.h"
#include <condition_variable>
#include <atomic>
#include <mutex>

using namespace std;

namespace NII
{
    typedef atomic_bool         STbool;
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

        inline bool tryLock() { return mMutex.try_lock() ? mCount++, true : false; }

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
        explicit ScopeLock(ThreadMutex * mutex) : mMutex(*mutex), mCurrent(true) { mMutex.lock(); }
        
        ~ScopeLock() { if (mCurrent) mMutex.unlock(); }
    public:
        inline bool isCurrent() const { return mCurrent; }

        inline void lock() { mMutex.lock(); mCurrent = true; }

        inline bool tryLock() { return (mCurrent = mMutex.tryLock()); }

        inline void unlock() { if (mCurrent) { mMutex.unlock(); mCurrent = false; } }

        inline ThreadMutex & getMutex() { return mMutex; }
    protected:
        ScopeLock(const ScopeLock & o):mMutex(o.mMutex){}
		ScopeLock & operator=(const ScopeLock & o) { return *this; }
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

	/** 线程条件触发器
	@version NIIEngine 3.0.0
	*/
	class _EngineAPI ThreadCondition
	{
	public:
		ThreadCondition() = default;

		void wait(ThreadMutex & mutex);

		bool wait(ThreadMutex & mutex, Nui64 ms);

		inline void wait(ScopeLock & lock) { wait(lock.getMutex()); }

		inline bool wait(ScopeLock & lock, Nui64 ms) { return wait(lock.getMutex(), ms); }

		inline void notify() { mCondition.notify_one(); }

		inline void notifyAll() { mCondition.notify_all(); }
	protected:
		ThreadCondition(const ThreadCondition &){}
		ThreadCondition & operator=(const ThreadCondition &) { return *this; }
	private:
		std::condition_variable_any mCondition;
	};

	/** 线程事件条件触发器
	@version NIIEngine 3.0.0
	*/
	class _EngineAPI ThreadConditionEvent
	{
	public:
		ThreadConditionEvent(ThreadCondition & cond, volatile bool & signal) : mCondition(cond), mSignal(signal) {}

		void wait(ThreadMutex & mutex);
		bool wait(ThreadMutex & mutex, Nui64 ms);

		inline void notifyAll() { mCondition.notifyAll(); }
		inline void notify() { mCondition.notify(); }
	private:
		volatile bool & mSignal;
		ThreadCondition & mCondition;
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
		volatile bool mSignal;
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
			mWrite ? mMutex.lockR() : mMutex.lockW();
        }
        
        explicit ScopeRWLock(ThreadRWMutex * mutex, bool write = false) : mMutex(*mutex), mCurrent(true), mWrite(write)
        {
			mWrite ? mMutex.lockR() : mMutex.lockW();
        }
        
        ~ScopeRWLock()
        {
            if (mCurrent)
            {
                mWrite ? mMutex.unlockR() : mMutex.unlockW();
            }
        }

        inline void lock()
        {
			mWrite ? mMutex.lockR() : mMutex.lockW();
            mCurrent = true;
        }

        inline bool tryLock()
        {
            mWrite ? mCurrent = mMutex.tryLockR() :  mCurrent = mMutex.tryLockW();
            return mCurrent;
        }

        inline void unlock()
        {
            if (mCurrent)
            {
                mWrite ? mMutex.unlockR() : mMutex.unlockW();
                mCurrent = false;
            }
        }

        inline bool isCurrent() const { return mCurrent; }

        inline ThreadRWMutex & getMutex() { return mMutex; }
    protected:
        ScopeRWLock(const ScopeRWLock & o):mMutex(o.mMutex){}
		ScopeRWLock & operator=(const ScopeRWLock & o) { return *this; }
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