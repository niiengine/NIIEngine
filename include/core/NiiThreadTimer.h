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

#ifndef _NII_THREAD_TIMER_H_
#define _NII_THREAD_TIMER_H_

#include "NiiPreInclude.h"
#include "NiiThread.h"

namespace NII
{
    /** �̼߳�ʱ��
    @version NIIEngine 3.0.0
    */
    class ThreadTimer : public Thread
    {
    public:
        ThreadTimer(ThreadMain * _main);
        ~ThreadTimer();

        /** ��ʼ��ʱ��
        @version NIIEngine 3.0.0
        */
        bool start(TimeDurMS timeout, bool loop = false);

        /** ������ʱ��
        @version NIIEngine 3.0.0
        */
        bool stop(bool wait = false);

        /** ������ʱ��
        @version NIIEngine 3.0.0
        */
        bool restart(TimeDurMS timeout = 0);

        /** ��ȡ������ɹ���ʱ��(����)
        @version NIIEngine 3.0.0
        */
        float geCostMS() const;
    protected:
        /// @copydetails Thread::Thread
        void process();
    private:
        TimeDurMS mTimerOut;
        TimeDurMS mLastTime;
        ThreadEvent mTimeEvent;
        bool mLoop;
    };
}
#endif