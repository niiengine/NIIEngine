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
    /** �ڵ�任��ר��KeyFrameTrack
    @version NIIEngine 3.0.0
    */
    class _EngineAPI NodeKeyFrameTrack : public KeyFrameTrack
    {
    public:
        NodeKeyFrameTrack(const Animation * own, KeyFrameTrackID id);
        NodeKeyFrameTrack(const Animation * own, KeyFrameTrackID id, PosNode * target);

        virtual ~NodeKeyFrameTrack();

        /** ����Ŀ�����
        @param node
        @version NIIEngine 3.0.0
        */
        virtual void setTarget(PosNode * node);

        /** ��ȡĿ�����
        @version NIIEngine 3.0.0
        */
        virtual PosNode * getTarget() const;

        /** ������ת�㷨�ķ���
        @version NIIEngine 3.0.0
        */
        void setShortPath(bool b);

        /** ��ȡ��ת�㷨�ķ���
        @version NIIEngine 3.0.0
        */
        bool getShortPath() const;

        /** ��������
        @version NIIEngine 3.0.0
        */
        virtual void buildSpline() const;

        /** Ӧ�û��
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
        /** �������ݻ��
        @param[in] src
        @param[out] dst
        @param[in] space
        @param[in] count
        @version NIIEngine 3.0.0 �߼�api
        */
        static void fusion(const Vector3f * src, Vector3f * dst, const Matrix4f * const * space, NCount count);

        /** �������ݻ��
        @param[in] src
        @param[out] dst
        @param[in] space
        @param[in] count
        @version NIIEngine 3.0.0 �߼�api
        */
        static void fusion(const Vector3f * src, const Vector3f * srcnormal, Vector3f * dst, Vector3f * dstnormal, 
            const Matrix4f * const * space, NCount count);

        /** �������ݻ��
        @param[in] src ��Դ����
        @param[in] dest Ŀ������
        @param[in] space �ռ�任
        @param[in] count ��������
        @param[in] normal �������ߵļ���
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