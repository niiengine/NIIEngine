/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-9-21

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

#ifndef _NII_UNIT_KEY_FRAME_TRACK_H_
#define _NII_UNIT_KEY_FRAME_TRACK_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrameTrack.h"
#include "NiiFusionData.h"

namespace NII
{
    /** ��ֵ�͵�ר�ùؼ�֡�켣
    @remark
        �� FusionData ��Ҫ��д����� ��/��/��/�� ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UnitKeyFrameTrack : public KeyFrameTrack
    {
    public:
        UnitKeyFrameTrack(const Animation * parent, KeyFrameTrackID id);
        UnitKeyFrameTrack(const Animation * parent, KeyFrameTrackID id, FusionData * target);

        /** Ӧ��
        @param[in] obj ���ʵ��
        @param[in] dst Ӧ�õ�����������
        @param[in] tt ����Ӧ�õ�ʱ��λ��
        @param[in] weight �������`���`��Ӱ��,1.0ΪȫӰ��,����������������
        @param[in] scale Ӧ�õ�ƽ�ƺ����ŵĹ�ģ,����Ӧһ������ʹ�õ���ͬ��С��Ŀ��
        @version NIIEngine 3.0.0
        */
        void apply(AnimationFusion * obj, FusionData * dst, TrackTime * tt,
            NIIf weight = 1.0, NIIf scale = 1.0f) const;

        /** ���ù����������
        @note �����켣ʹ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void setTarget(const FusionData * dst);

        /** ��ȡ�����������
        @note �����켣ʹ��
        @version NIIEngine 3.0.0 �߼�api
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