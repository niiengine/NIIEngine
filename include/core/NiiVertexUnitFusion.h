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

#ifndef _NII_VERTEX_UNIT_FUSION_H_
#define _NII_VERTEX_UNIT_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiVertexFusion.h"

namespace NII
{
    /** ���㵥Ԫ���ʵ��(��Ԫ����е�һ���ر�����)
    @remark һ��ָʵ��ģ���еĶ���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexUnitFusion : public VertexFusion
    {
    public:
        VertexUnitFusion(AnimationObj * obj, Animation * ani, TimeDurMS pos, TimeDurMS dur,
            NIIf weight = 1.0, bool enable = false);

        VertexUnitFusion(AnimationFusionObj * own, const VertexUnitFusion & o);

        /// @copydetails AnimationFusion::clone
        AnimationFusion * clone(AnimationFusionObj * own) const;
        
        /// @copydetails VertexFusion::apply
        void apply(GeometrySpace * dst, bool sw, bool hw);
        
        /// @copydetails VertexFusion::apply
        void apply(VertexData * data);
    };
}
#endif