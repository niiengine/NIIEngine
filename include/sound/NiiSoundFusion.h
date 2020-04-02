/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-9-17

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

#ifndef _NII_SOUND_FUSION_H_
#define _NII_SOUND_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiAnimationFusion.h"

namespace NII
{
    /** �ڵ���ʵ��
    @remark ���ò�ֵ�������LOD����ʹ������Ч�Ŀ�������Ч��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SoundFusion : public AnimationFusion
    {
    public:
        SoundFusion(AnimationObj * obj, Animation * ani,  TimeDurMS pos, TimeDurMS dur, 
            NIIf scale = 1.0, NIIf weight = 1.0, bool enable = false);

        SoundFusion(AnimationFusionObj * own, const SoundFusion & o);

        ~SoundFusion();

        /** ִ��Ӧ��
        @param[in] factor ���Ӱ��Ȩ��(0.0 - 1.0]
        @version NIIEngine 3.0.0
        */
        void apply(NIIf factor = 1.0);

        /** 
        @version NIIEngine 4.0.0
        */
        inline void setDirty(bool b) { mDirty = b; }

        /** 
        @version NIIEngine 4.0.0
        */
        inline bool isDirty() const { return mDirty; }
    public:
        /// @copydetails AnimationFusion::reset
        void reset();

        /// @copydetails AnimationFusion::clone
        AnimationFusion * clone(AnimationFusionObj * own) const;
    protected:
        SpaceNode * mDst;
        bool mDirty;
    };
}
#endif