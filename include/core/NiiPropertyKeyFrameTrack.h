/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-9-22

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

#ifndef _NII_PROPERTYID_KEYFRAMETRACK_H_
#define _NII_PROPERTYID_KEYFRAMETRACK_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrameTrack.h"

namespace NII
{
    /** ����ֵ�ؼ�֡�켣
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyKeyFrameTrack : public KeyFrameTrack
    {
    public:
        /** ����ģʽ
        @version NIIEngine 3.0.0
        */
        enum Mode
        {
            M_ABS,        ///< ����
            M_REL,        ///< ���
            M_MulREL    ///< ���������
        };
    public:
        PropertyKeyFrameTrack(const Animation * o, KeyFrameTrackID id);

        PropertyKeyFrameTrack(const Animation * o, KeyFrameTrackID id,
            PropertyID target, Mode mode = M_ABS);

        /** ��������ģʽ
        @version NIIEngine 3.0.0
        */
        void setMode(Mode mode);

        /** ��ȡ����ģʽ
        @version NIIEngine 3.0.0
        */
        Mode getMode() const;

        /** ��������
        @note �����켣ʹ��
        @version NIIEngine 3.0.0
        */
        void setTarget(PropertyID pid);

        /** ��ȡ����
        @note �����켣ʹ��
        @version NIIEngine 3.0.0
        */
        PropertyID getTarget() const;

        /** ��������ֵ
        @note �Ӷ���������ֵ
        @version NIIEngine 3.0.0
        */
        void setValue(PropertyFusion * obj);

        /** Ӧ�û��
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