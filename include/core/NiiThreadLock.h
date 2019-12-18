/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-1-7

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_THREADLOCK_H_
#define _NII_THREADLOCK_H_

#include "NiiPreInclude.h"
#include <mutex>

namespace NII
{
    /** �̻߳���
    @version NIIEngine 3.0.0
    */
    class ThreadMutex
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

    /** �̻߳�����
    @version NIIEngine 3.0.0
    */
    class ScopeLock
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

    /** �̻߳������
    @version NIIEngine 3.0.0
    */
    class ScopeUnlock
    {
    public:
        explicit ScopeUnlock(ThreadMutex & mutex) : mMutex(mutex) { mCount = mMutex.unlock(0); }
        ~ScopeUnlock() { mMutex.lock(mCount); }
    protected:
        ThreadMutex & mMutex;
        NCount mCount;
    };

    /** �̶߳�д����
    @version NIIEngine 3.0.0
    */
    class ThreadRWMutex
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
        atomic_bool mSignal;
        NCount mCount;
    };

    /** �̶߳�д������
    @version NIIEngine 3.0.0
    */
    class ScopeRWLock
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
}
#endif