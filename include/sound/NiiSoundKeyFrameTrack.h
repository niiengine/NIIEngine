/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-7-1

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

#ifndef _NII_SOUND_KEY_FRAME_TRACK_H_
#define _NII_SOUND_KEY_FRAME_TRACK_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrameTrack.h"

namespace NII
{
    /** ����ר��KeyFrameTrack
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SoundKeyFrameTrack : public KeyFrameTrack
    {
    public:
        SoundKeyFrameTrack(const Animation * own, KeyFrameTrackID id);
        SoundKeyFrameTrack(const Animation * own, KeyFrameTrackID id, SpaceNode * target);

        virtual ~SoundKeyFrameTrack();

        /** ����Ŀ�����
        @param node
        @version NIIEngine 4.0.0
        */
        virtual void setTarget(SpaceNode * node);

        /** ��ȡĿ�����
        @version NIIEngine 4.0.0
        */
        virtual SpaceNode * getTarget() const;

        /**
        @version NIIEngine 4.0.0
        */
        inline void setPos(const Vector3f & pos) { mPos = pos; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline const Vector3f & getPos() const { return mPos; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline void setRelativeGain(bool set) { mRelativeGain = set; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline bool getRelativeGain() const { return mRelativeGain; }
        
        /** Ӧ�û��
        @version NIIEngine 4.0.0
        */
        virtual void apply(AnimationFusion * dst, SpaceNode * com, TrackTime * tt, NIIf weight = 1.0, NIIf scale = 1.0f) const;

        /// @copydetails KeyFrameTrack::getFusion
        virtual void getFusion(AnimationFusion * dst, TrackTime * tt, KeyFrame * kf) const;

        /// @copydetails KeyFrameTrack::apply
        virtual void apply(AnimationFusion * dst, TrackTime * tt, NIIf weight = 1.0, NIIf scale = 1.0f) const;

        /// @copydetails KeyFrameTrack::isValid
        virtual bool isValid() const;

        /// @copydetails KeyFrameTrack::optimise
        virtual void optimise();

        /// @copydetails KeyFrameTrack::createFusion
        virtual AnimationFusion * createFusion(AnimationObj * obj, Animation * ani,  NIIf weight, bool enable) const;

        /// @copydetails KeyFrameTrack::clone
        KeyFrameTrack * clone(const Animation * o) const;
    protected:
        /// @copydetails KeyFrameTrack::createImpl
        KeyFrame * createImpl(TimeDurMS time);

        /// @copydetails KeyFrameTrack::notifyImpl
        void notifyImpl() const;
    protected:
        SpaceNode * mTarget;
        Vector3f mPos;
        bool mRelativeGain;
    };
}
#endif