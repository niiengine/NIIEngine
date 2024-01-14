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

#ifndef _NII_KEYFRAME_H_
#define _NII_KEYFRAME_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 到动画序列中的关键帧.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI KeyFrame : public AnimableAlloc
    {
        friend class KeyFrameTrack;
    public:
        /** 构造函数
        @param[in] own 所属动画轨道
        @param[in] pos 所属动画时间
        @version NIIEngine 3.0.0
        */
        KeyFrame(const KeyFrameTrack * own, TimeDurMS pos);

        virtual ~KeyFrame();

        /** 设置动画轨道中的关键帧时间
        @return 毫秒,时间是相对的
        @version NIIEngine 3.0.0
        */
        void setTime(TimeDurMS time);

        /** 获取动画轨道中的关键帧时间
        @return 毫秒,时间是相对的
        @version NIIEngine 3.0.0
        */
        TimeDurMS getTime() const;
    protected:
        virtual KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        TimeDurMS mTime;                ///< 关键帧的时间点
        mutable KeyFrameTrack * mTrack; ///< 关键帧所属的动画轨道
    };
}
#endif