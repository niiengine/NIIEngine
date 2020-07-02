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

#ifndef _NII_UNIT_KEY_FRAME_TRACK_H_
#define _NII_UNIT_KEY_FRAME_TRACK_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrameTrack.h"
#include "NiiFusionData.h"

namespace NII
{
    /** 数值型的专用关键帧轨迹
    @remark
        在 FusionData 中要重写基类的 加/减/乘/除 操作
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UnitKeyFrameTrack : public KeyFrameTrack
    {
    public:
        UnitKeyFrameTrack(const Animation * parent, KeyFrameTrackID id);
        UnitKeyFrameTrack(const Animation * parent, KeyFrameTrackID id, FusionData * target);

        /** 应用
        @param[in] obj 混合实例
        @param[in] dst 应用到动画的数据
        @param[in] tt 动画应用的时间位置
        @param[in] weight 给定这个`轨道`的影响,1.0为全影响,更少融入其他动画
        @param[in] scale 应用到平移和缩放的规模,以适应一个动画使用到不同大小的目标
        @version NIIEngine 3.0.0
        */
        void apply(AnimationFusion * obj, FusionData * dst, TrackTime * tt,
            NIIf weight = 1.0, NIIf scale = 1.0f) const;

        /** 设置关联混合数据
        @note 独立轨迹使用
        @version NIIEngine 3.0.0 高级api
        */
        virtual void setTarget(const FusionData * dst);

        /** 获取关联混合数据
        @note 独立轨迹使用
        @version NIIEngine 3.0.0 高级api
        */
        virtual const FusionData * getTarget() const;

        /// @copydetails KeyFrameTrack::getFusion
        virtual void getFusion(AnimationFusion * dst, TrackTime * tt, KeyFrame * kf) const;

        /// @copydetails KeyFrameTrack::apply
        virtual void apply(AnimationFusion * dst, TrackTime * tt, NIIf weight = 1.0, NIIf scale = 1.0f) const;

        /// @copydetails KeyFrameTrack::createFusion
        virtual AnimationFusion * createFusion(AnimationObj * obj, Animation * ani, NIIf weight, bool enable) const;

        /// @copydetails KeyFrameTrack::clone
        KeyFrameTrack * clone(const Animation * o) const;
    protected:
        /// @copydetails KeyFrameTrack::createImpl
        KeyFrame * createImpl(TimeDurMS pos);
    protected:
        FusionData * mTarget;
    };
}
#endif