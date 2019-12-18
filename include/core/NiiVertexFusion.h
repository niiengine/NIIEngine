/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-4-2

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
#ifndef _NII_VERTEXFUSION_H_
#define _NII_VERTEXFUSION_H_

#include "NiiPreInclude.h"
#include "NiiAnimationFusion.h"

namespace NII
{
    /** ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexFusion : public AnimationFusion
    {
    public:
        VertexFusion(AnimationObj * obj, Animation * ani, TimeDurMS pos, TimeDurMS dur,
            NIIf weight = 1.0, bool enable = false);
            
        VertexFusion(AnimationFusionObj * own, const VertexFusion & o);
        
        virtual ~VertexFusion();

        /** Ӧ�õ�Ŀ�����
        @remark Ӳ����ϲ��ܻ�ȡ��ǰ״̬,���ֲ����������Ӱ�����Ҫ������
        @param[in] sw ִ�������������
        @param[in] hw ִ��Ӳ����������
        @version NIIEngine 3.0.0
        */
        virtual void apply(GeometrySpace * dst, bool sw, bool hw) = 0;
        
        /** Ӧ�õ���������
        @version NIIEngine 3.0.0
        */
        virtual void apply(VertexData * data) = 0;
    };
}
#endif