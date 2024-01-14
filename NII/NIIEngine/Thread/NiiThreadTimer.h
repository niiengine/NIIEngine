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

#ifndef _NII_THREAD_TIMER_H_
#define _NII_THREAD_TIMER_H_

#include "NiiPreInclude.h"
#include "NiiThread.h"

namespace NII
{
    /** �̼߳�ʱ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ThreadTimer : public Thread
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