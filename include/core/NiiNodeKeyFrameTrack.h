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

#ifndef _NII_NODE_KEY_FRAME_TRACK_H_
#define _NII_NODE_KEY_FRAME_TRACK_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrameTrack.h"

namespace NII
{
    struct NodeKeyFrameSpline;
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
        static void fusion(const Vector3f * src, Vector3f * dst, const Matrix4f * const * space, NCount count);

        /** 顶点数据混合
        @param[in] src
        @param[out] dst
        @param[in] space
        @param[in] count
        @version NIIEngine 3.0.0 高级api
        */
        static void fusion(const Vector3f * src, const Vector3f * srcnormal, Vector3f * dst, Vector3f * dstnormal, 
            const Matrix4f * const * space, NCount count);

        /** 顶点数据混合
        @param[in] src 来源数据
        @param[in] dest 目标数据
        @param[in] space 空间变换
        @param[in] count 数据数量
        @param[in] normal 包含法线的计算
        */
        static void fusion(const VertexData * src, VertexData * dest, const Matrix4f * const * space, NCount count, bool normal);
    protected:
        /// @copydetails KeyFrameTrack::createImpl
        KeyFrame * createImpl(TimeDurMS time);

        /// @copydetails KeyFrameTrack::notifyImpl
        void notifyImpl() const;
    protected:
        PosNode * mTarget;
        NodeKeyFrameSpline * mSplines;
        bool mShortPath;
    };
}
#endif