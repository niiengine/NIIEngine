/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-3-7

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

#ifndef _NII_RENDER_FILTER_H_
#define _NII_RENDER_FILTER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ��Ⱦ������
    @version NIIEngine 3.0.0 �߼�api
    */
    class _EngineAPI RenderFilter : public RenderAlloc
    {
    public:
        RenderFilter() {}
        virtual ~RenderFilter() {}

        /** ִ����Ⱦ
        @version NIIEngine 3.0.0
        */
        virtual void render(const ShaderCh * ch, GeometryObj * obj) = 0;

        /** ִ����Ⱦ
        @version NIIEngine 3.0.0
        */
        virtual void render(const ShaderCh * ch, const GeometryObjList & obj) = 0;
    };
}

#endif