/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-5-7

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

#ifndef _NII_GPU_PROGRAM_PROPERTY_
#define _NII_GPU_PROGRAM_PROPERTY_

#include "NiiPreInclude.h"

namespace NII
{
    class _EngineAPI GpuProgramProperty
    {
    public:
        static const PropertyID GpuProgramSrcProperty;
        static const PropertyID GpuProgramSyntaxProperty;
        static const PropertyID GpuProgramParamsProperty;
        static const PropertyID GpuProgramParamIndexProperty;
        static const PropertyID GpuProgramParamNameProperty;
        static const PropertyID GpuProgramSyncParamIndexProperty;
        static const PropertyID GpuProgramSyncParamNameProperty;
        static const PropertyID GpuProgramParamLinkProperty;
        static const PropertyID PropertyCount;
    };
}

#endif