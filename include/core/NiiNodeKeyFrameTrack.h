/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-7-1

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

#ifndef _NII_NODE_KEY_FRAME_TRACK_H_
#define _NII_NODE_KEY_FRAME_TRACK_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrameTrack.h"

namespace NII
{
    struct KeyFrameSpline;
    /** 节点变换的专用KeyFrameTrack
    @version NIIEngine 3.0.0
    */
    class _EngineAPI NodeKeyFrameTrack : public KeyFrameTrack
    {
    public:
        NodeKeyFrameTrack(const Animation * own, KeyFrameTrackID id);
        NodeKeyFrameTrack(const Animation * own, KeyFrameTrackID id, PosNode * target);

        virtual ~NodeKeyFrameTrack();

        /** 设置目标对象
        @param node
        @version NIIEngine 3.0.0
        */
        virtual void setTarget(PosNode * node);

        /** 获取目标对象
        @version NIIEngine 3.0.0
        */
        virtual PosNode * getTarget() const;

        /** 设置旋转算法的方法
        @version NIIEngine 3.0.0
        */
        void setShortPath(bool b);

        /** 获取旋转算法的方法
        @version NIIEngine 3.0.0
        */
        bool getShortPath() const;

        /** 构建样条
        @version NIIEngine 3.0.0
        */
        virtual void buildSpline() const;

        /** 应用混合
        @version NIIEngine 3.0.0
        */
        virtual void apply(AnimationFusion * dst, PosNode * com, TrackTime * tt, NIIf weight = 1.0, NIIf scale = 1.0f) const;

        /// @copydetails KeyFrameTrack::getFusion
        virtual void getFusion(AnimationFusion * dst, TrackTime * tt, KeyFrame * kf) const;

        /// @copydetails KeyFrameTrack::apply
        virtual void apply(AnimationFusion * dst, TrackTime * tt, NIIf weight = 1.0, NIIf scale = 1.0f) const;

        /// @copydetails KeyFrameTrack::isValid
        virtual bool isValid() const;

        /// @copydetails KeyFrameTrack::optimise
        virtual void optimise();

        /// @copydetails KeyFrameTrack::setOrigin
        void setOrigin(const KeyFrameTrack * org, TrackTime * time);

        /// @copydetails KeyFrameTrack::createFusion
        virtual AnimationFusion * createFusion(AnimationObj * obj, Animation * ani,  NIIf weight, bool enable) const;

        /// @copydetails KeyFrameTrack::clone
        KeyFrameTrack * clone(const Animation * o) const;
    public:
        /** 顶点数据混合
        @param[in] src
        @param[out] dst
        @param[in] space
        @param[in] count
        @version NIIEngine 3.0.0 高级api
        */
        static void fusion(const Vector3f * src, Vector3f * dst,
            const Matrix4f * const * space, NCount count);

        /** 顶点数据混合
        @param[in] src
        @param[out] dst
        @param[in] space
        @param[in] count
        @version NIIEngine 3.0.0 高级api
        */
        static void fusion(const Vector3f * src, const Vector3f * srcnormal,
            Vector3f * dst, Vector3f * dstnormal, const Matrix4f * const * space,
                NCount count);

        /** 顶点数据混合
        @param[in] src 来源数据
        @param[in] dest 目标数据
        @param[in] space 空间变换
        @param[in] count 数据数量
        @param[in] normal 包含法线的计算
        */
        static void fusion(const VertexData * src, VertexData * dest,
            const Matrix4f * const * space, NCount count, bool normal);
    protected:
        /// @copydetails KeyFrameTrack::createImpl
        KeyFrame * createImpl(TimeDurMS time);

        /// @copydetails KeyFrameTrack::notifyImpl
        void notifyImpl() const;
    protected:
        PosNode * mTarget;
        KeyFrameSpline * mSplines;
        bool mShortPath;
    };
}
#endif