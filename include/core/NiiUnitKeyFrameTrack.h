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