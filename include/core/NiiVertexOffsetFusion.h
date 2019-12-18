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

#ifndef _NII_VERTEX_OFFSET_FUSION_H_
#define _NII_VERTEX_OFFSET_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiVertexFusion.h"

namespace NII
{
    /** ����ƫ��(��)���
    @remark һ��ָʵ��ģ���еĶ���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexOffsetFusion : public VertexFusion
    {
    public:
        VertexOffsetFusion(AnimationObj * obj, Animation * ani, TimeDurMS pos, TimeDurMS dur,
            NIIf weight = 1.0, bool enable = false);

        VertexOffsetFusion(AnimationFusionObj * own, const VertexOffsetFusion & o);

        /** ��ȡ����ƫ���б�
        @version NIIEngine 3.0.0
        */
        const VertexOffsetList & getOffsetList() const;
        
        /// @copydetails VertexFusion::clone
        AnimationFusion * clone(AnimationFusionObj * own) const;
        
        /// @copydetails VertexFusion::apply
        void apply(GeometrySpace * dst, bool sw, bool hw);
        
        /// @copydetails VertexFusion::apply
        void apply(VertexData * data);
    protected:
        Mesh * mMesh;
    };
}
#endif