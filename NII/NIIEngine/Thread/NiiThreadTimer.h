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
    /** 线程计时器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ThreadTimer : public Thread
    {
    public:
        ThreadTimer(ThreadMain * _main);
        ~ThreadTimer();

        /** 开始计时器
        @version NIIEngine 3.0.0
        */
        bool start(TimeDurMS timeout, bool loop = false);

        /** 结束计时器
        @version NIIEngine 3.0.0
        */
        bool stop(bool wait = false);

        /** 重启计时器
        @version NIIEngine 3.0.0
        */
        bool restart(TimeDurMS timeout = 0);

        /** 获取启动后渡过的时间(毫秒)
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