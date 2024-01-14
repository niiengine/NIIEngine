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

#ifndef _NII_UNIT_KEY_FRAME_H_
#define _NII_UNIT_KEY_FRAME_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrame.h"
#include "NiiFusionData.h"

namespace NII
{
    /** 数值型的专用关键帧.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UnitKeyFrame : public KeyFrame
    {
    public:
        /** 帧过渡模式
        @version NIIEngine 3.0.0
        */
        enum Mode
        {
            M_Constant,     ///< 基本插值渡
            M_Point,        ///< 帧点过渡
            M_Square,       ///< 加速形式过渡
            M_SquareRoot    ///< 减速形式过渡
        };

        UnitKeyFrame(const KeyFrameTrack * o, TimeDurMS pos);
        UnitKeyFrame(const KeyFrameTrack * o, TimeDurMS pos, Mode mode, 
            const FusionData & value);
        virtual ~UnitKeyFrame();

        /** 设置插值函子模式
        @version NIIEngine 3.0.0
        */
        void setMode(Mode p);

        /** 获取插值函子模式
        @version NIIEngine 3.0.0
        */
        Mode getMode() const;

        /** 设置这个关键帧的数值
        @remark 所有的关键帧值必须有一致的数据类型
        @version NIIEngine 3.0.0
        */
        void setValue(const FusionData & val);

        /** 获取这个关键帧的数值
        @remark 所有的关键帧值必须有一致的数据类型
        @version NIIEngine 3.0.0
        */
        const FusionData & getValue() const;

        /** 获取帧过渡模式参子
        @param[in] pos[0, 1]
        @return 函子模式结果[0, 1]
        @version NIIEngine 3.0.0
        */
        NIIf getFactor(NIIf pos);
    protected:
        /// @copydetails KeyFrame::clone
        virtual KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        Mode mMode;
        FusionData mValue;
    };
}

#endif