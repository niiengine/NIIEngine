/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-2-1

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

#ifndef _NII_MaterialMatchListener_H_
#define _NII_MaterialMatchListener_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ����ƥ�����
    @version NIIEngine 3.0.0 �߼�api
    */
    class MaterialMatchListener
    {
    public:
        virtual ~MaterialMatchListener();

        /** ƥ��
        @version NIIEngine 3.0.0
        */
        virtual ShaderFusion * find(SchemeID sid, const Material * mat, 
            const GeometryObj * geo, Nidx lod);
            
        /** ����
        @version NIIEngine 3.0.0
        */
        virtual ShaderFusion * miss(SchemeID sid, const Material * mat, 
            const GeometryObj * geo, Nidx lod);
    };
}

#endif