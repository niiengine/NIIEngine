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

#ifndef _NII_FRAMEOBJ_LISTENER_H_
#define _NII_FRAMEOBJ_LISTENER_H_

#include "NiiPreInclude.h"
#include "NiiEventArgs.h"

namespace NII
{
    class _EngineAPI FrameObjArgs : public EventArgs
    {
    public:
        FrameObjArgs(){}
        FrameObj * mFrame;
    };

    class FrameViewArgs : public FrameObjArgs
    {
    public:
        FrameViewArgs(){}
        Viewport * mView;
    };
    
    /** 帧对象监听
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameObjListener
    {
    public:
        virtual ~FrameObjListener();

        /** 帧对象执行冲刷前
        @version NIIEngine 3.0.0
        */
        virtual void onPreFlush(const FrameObjArgs * args);

        /** 帧对象执行冲刷后
        @version NIIEngine 3.0.0
        */
        virtual void onEndFlush(const FrameObjArgs * args);

        /** 帧对象的实际视口执行冲刷前
        @version NIIEngine 3.0.0
        */
        virtual void onPreViewFlush(const FrameViewArgs * args);

        /** 帧对象的实际视口执行冲刷后
        @version NIIEngine 3.0.0
        */
        virtual void onEndViewFlush(const FrameViewArgs * args);
    };
    
    /** 帧每秒
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI FramesPerSecond : public SysAlloc
    {
    public:
        FramesPerSecond(Timer * time);
        
        /** 重至
        @version NIIEngine 3.0.0
        */
        void reset();
        
        /** 渡过
        @version NIIEngine 3.0.0
        */
        void cost();
    public:
        NIIf mAvg;
        NIIf mBest;
        NIIf mWorst;
        NIIf mLast;
        TimeDurMS mBestFrame;
        TimeDurMS mWorstFrame;
    private:
        Timer * mTimer;
        NCount mSecFrame;
        Nul mLastTime;   
        Nul mTempTime;
    };
}
#endif
