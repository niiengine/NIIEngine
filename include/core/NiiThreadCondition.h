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

#ifndef _NII_Thread_Condition_h_
#define _NII_Thread_Condition_h_

#include "NiiPreInclude.h"
#include <condition_variable>
#include <atomic>

namespace NII
{
    /** �߳�����������
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

    /** �߳��¼�����������
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