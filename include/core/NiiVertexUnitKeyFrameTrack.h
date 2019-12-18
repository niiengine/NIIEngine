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

#ifndef _NII_VERTEXUNIT_KEYFRAME_TRACK_H_
#define _NII_VERTEXUNIT_KEYFRAME_TRACK_H_

#include "NiiPreInclude.h"
#include "NiiVertexKeyFrameTrack.h"

namespace NII
{
    /** 处理改变顶点位置信息的专用 KeyFrameTrack.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexUnitKeyFrameTrack : public VertexKeyFrameTrack
    {
    public:
        VertexUnitKeyFrameTrack(const Animation * own, KeyFrameTrackID id);
        VertexUnitKeyFrameTrack(const Animation * own, KeyFrameTrackID id,
            VertexData * target, CalcMode mode = M_CPU);

        /** 顶点插值
        @param[in] kf1 开始位置
        @param[in] kf2 结束位置
        @param[in] t 插值系数
        @param[in] out 插值结果
        */
        static void fusion(const VertexBuffer * kf1, const VertexBuffer * kf2, NIIf t, VertexData * out);

        /// @copydetails VertexKeyFrameTrack::apply
        void apply(AnimationFusion * dst, VertexData * target, TrackTime * tt, NIIf weight = 1.0) const;

        /// @copydetails VertexKeyFrameTrack::isNromalsFusion
        bool isNromalsFusion() const;

        /// @copydetails KeyFrameTrack::apply
        virtual void apply(AnimationFusion * dst, TrackTime * tt, NIIf weight = 1.0, NIIf scale = 1.0f) const;

        /// @copydoc KeyFrameTrack::getFusion
        virtual void getFusion(AnimationFusion * dst, TrackTime * tt, KeyFrame * kf) const;

        /// @copydetails KeyFrameTrack::isValid
        virtual bool isValid() const;

        /// @copydetails KeyFrameTrack::reset
        virtual void reset();

        /// @copydetails KeyFrameTrack::optimise
        virtual void optimise();

        /// @copydetails KeyFrameTrack::createFusion
        virtual AnimationFusion * createFusion(AnimationObj * obj, Animation * ani, NIIf weight, bool enable) const;

        /// @copydetails KeyFrameTrack::clone
        KeyFrameTrack * clone(const Animation * o) const;
    protected:
        /// @copydetails KeyFrameTrack::createImpl
        KeyFrame * createImpl(TimeDurMS time);
    };
}
#endif