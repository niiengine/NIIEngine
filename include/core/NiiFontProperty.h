/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-7-4

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_FONTPROPERTY_H_
#define _NII_FONTPROPERTY_H_

#include "NiiPreInclude.h"

namespace NII
{
    class _EngineAPI FontProperty
    {
    public:
        static const PropertyID FontIDProperty;
        static const PropertyID FontSrcProperty;
        static const PropertyID FontTypeProperty;
        static const PropertyID SizeProperty;
        static const PropertyID AutoResProperty;
        static const PropertyID NativeResProperty;
        static const PropertyID LineSpaceProperty;
        static const PropertyID AntiAliasProperty;

        static const PropertyID DefineProperty;
        static const PropertyID TextureIdxProperty;
        static const PropertyID CodeValueProperty;
        static const PropertyID FontPosPorperty;
        static const PropertyID FontSizeProperty;
        static const PropertyID FontPosOftPorperty;
        static const PropertyID FontHOftProperty;
        static const PropertyID PropertyCount;
    };
}

#endif