/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-9-22

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

#ifndef _NII_PROPERTYID_KEYFRAMETRACK_H_
#define _NII_PROPERTYID_KEYFRAMETRACK_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrameTrack.h"

namespace NII
{
    /** 属性值关键帧轨迹
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyKeyFrameTrack : public KeyFrameTrack
    {
    public:
        /** 运算模式
        @version NIIEngine 3.0.0
        */
        enum Mode
        {
            M_ABS,        ///< 绝对
            M_REL,        ///< 相对
            M_MulREL    ///< 叠加型相对
        };
    public:
        PropertyKeyFrameTrack(const Animation * o, KeyFrameTrackID id);

        PropertyKeyFrameTrack(const Animation * o, KeyFrameTrackID id,
            PropertyID target, Mode mode = M_ABS);

        /** 设置运算模式
        @version NIIEngine 3.0.0
        */
        void setMode(Mode mode);

        /** 获取混算模式
        @version NIIEngine 3.0.0
        */
        Mode getMode() const;

        /** 设置属性
        @note 独立轨迹使用
        @version NIIEngine 3.0.0
        */
        void setTarget(PropertyID pid);

        /** 获取属性
        @note 独立轨迹使用
        @version NIIEngine 3.0.0
        */
        PropertyID getTarget() const;

        /** 设置运算值
        @note 从对象中设置值
        @version NIIEngine 3.0.0
        */
        void setValue(PropertyFusion * obj);

        /** 应用混合
        @version NIIEngine 3.0.0
        */
        void apply(AnimationFusion * dst, PropertyID target, TrackTime * tt, NIIf weight = 1.0, NIIf scale = 1.0f) const;

        /// @copydetails KeyFrameTrack::apply
        virtual void apply(AnimationFusion * dst, TrackTime * tt, NIIf weight = 1.0, NIIf scale = 1.0f) const;

        /// @copydetails KeyFrameTrack::getFusion
        virtual void getFusion(AnimationFusion * dst, TrackTime * tt, KeyFrame * kf) const;

        /// @copydetails KeyFrameTrack::createFusion
        virtual AnimationFusion * createFusion(AnimationObj * obj, Animation * ani, NIIf weight, bool enable) const;

        /// @copydetails KeyFrameTrack::clone
        KeyFrameTrack * clone(const Animation * o) const;
    protected:
        /// @copydetails KeyFrameTrack::createImpl
        KeyFrame * createImpl(TimeDurMS pos);
    protected:
        Mode mMode;
        PropertyID mTarget; /// PropertyCmd
    };
}

#endif