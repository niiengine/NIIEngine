/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-6-8

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

#ifndef _NII_UNIT_FUSION_H_
#define _NII_UNIT_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiAnimationFusion.h"

namespace NII
{
    /** ��Ԫ���ʵ��
    @remark
        �������ڻ�� Vector2 Vector3 �ȼ򵥵�������(����)����,��UI����/��Ԫ�Ӵ�С,��С
        ��������Ź��̶���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UnitFusion : public AnimationFusion
    {
    public:
        UnitFusion(FusionData * dst, AnimationObj * obj, Animation * ani,
            NIIf weight = 1.0, bool enable = false);

        UnitFusion(FusionData * dst, AnimationObj * obj, Animation * ani, TimeDurMS pos,
            TimeDurMS dur, NIIf weight = 1.0, bool enable = false);

        UnitFusion(AnimationFusionObj * own, const UnitFusion & o);

        ~UnitFusion();

        /** Ӧ��
        @remark
        @param[in] factor ���Ӱ��Ȩ��
        @version NIIEngine 3.0.0
        */
        void apply(NIIf factor = 1.0f);
    public:
        /// @copydetails AnimationFusion::reset
        void reset();
        
        /// @copydetails AnimationFusion::
        AnimationFusion * clone(AnimationFusionObj * own) const;
    protected:
        FusionData * mDst;
    };
}
#endif