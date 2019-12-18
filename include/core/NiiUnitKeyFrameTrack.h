/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-9-21

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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