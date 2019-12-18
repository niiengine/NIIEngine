/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-5-12

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_VERTEX_OFFSET_KEYFRAME_TRACK_H_
#define _NII_VERTEX_OFFSET_KEYFRAME_TRACK_H_

#include "NiiPreInclude.h"
#include "NiiVertexKeyFrameTrack.h"
#include "NiiVertexOffset.h"

namespace NII
{
    /** ����ƫ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexOffsetKeyFrameTrack : public VertexKeyFrameTrack
    {
    public:
        VertexOffsetKeyFrameTrack(const Animation * own, KeyFrameTrackID id);

        VertexOffsetKeyFrameTrack(const Animation * own, KeyFrameTrackID id,
            VertexData * target, CalcMode mode = M_CPU);

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

        /// @copydetails KeyFrameTrack::setOrigin
        void setOrigin(const KeyFrameTrack * org, TrackTime * time);

        /// @copydetails KeyFrameTrack::createFusion
        virtual AnimationFusion * createFusion(AnimationObj * obj, Animation * ani, NIIf weight, bool enable) const;

        /// @copydetails KeyFrameTrack::clone
        KeyFrameTrack * clone(const Animation * o) const;

        /** ��ϲ���
        @param[in] dest ��������
        @param[in] pos λ��ƫ��
        @param[in] normal ����ƫ��
        @param[in] weight Ȩ��
        */
        static void fusion(VertexData * dest, const VertexOffset::OffsetMap & pos,
            const VertexOffset::NormalsMap & normal, NIIf weight);

        /** ��������
        @param[in] src Դ
        @param[in] dest Ŀ��
        @param[in] normal �Ƿ������������
        */
        static void memcpy(const VertexData * src, VertexData * dest, bool normal);
    protected:
        /// @copydetails KeyFrameTrack::createImpl
        KeyFrame * createImpl(TimeDurMS time);
    };
}
#endif