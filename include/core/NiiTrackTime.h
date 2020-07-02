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

#ifndef _NII_TRACK_TIME_H_
#define _NII_TRACK_TIME_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 关键帧时间索引
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TrackTime
    {
    public:
        TrackTime(TimeDurMS pos);
        TrackTime(TimeDurMS pos, Nidx index, Nidx lindex);

        /** 是否有效
        @version NIIEngine 3.0.0
        */
        bool isValid() const;

        /** 获取时间
        @version NIIEngine 3.0.0
        */
        TimeDurMS getTimePos() const;

        /** 获取动画开始关键帧
        @version NIIEngine 3.0.0
        */
        Nidx getKeyIndex() const;
        
        /** 设置当前时间关键帧
        @version NIIEngine 3.0.0
        */
        void setLocalKeyIndex(Nidx idx);
        
        /** 获取当前时间关键帧
        @version NIIEngine 3.0.0
        */
        Nidx getLocalKeyIndex() const;
    protected:
        TimeDurMS mTimePos;
        Nidx mKeyIndex;
        Nidx mLocalKeyIndex;
    };
}

#endif