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

#ifndef _NII_THREAD_EVENT_H_
#define _NII_THREAD_EVENT_H_

#include "NiiPreInclude.h"
#include <initializer_list>
#include <atomic>

namespace NII
{
    class ThreadEvent
    {
        friend class ThreadEventGroup;
    public:
        ThreadEvent(bool auto_ = true, bool signal_ = false);

        inline void reset() { mSignal = false; }

        void set();

        inline bool isSignal() const { return mSignal; }

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
        atomic_bool mSignal;
        NCount mCount;
        ThreadCondition mCondition;
        ThreadConditionEvent mConditionEvent;
        bool mAutoReset;
    };

    class ThreadEventGroup
    {
    public:
        ThreadEventGroup(std::initializer_list<ThreadEvent *> eventlist);

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
        NCount mCount;
        ThreadEvent * mEvent;
        atomic_bool mSignal;
        ThreadCondition mCondition;
        ThreadConditionEvent mConditionEvent;
        mutable ThreadMutex mMutex;
    };
}

#endif
