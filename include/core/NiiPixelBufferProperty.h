/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-7-1

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

#ifndef _NII_VIEW_PIXEL_PROPERTY_H_
#define _NII_VIEW_PIXEL_PROPERTY_H_

#include "NiiPreInclude.h"

namespace NII
{
    class _EngineAPI PixelBufferProperty
    {
    public:
        static const PropertyID PixelBufferProperty;
        static const PropertyID TextureSrcProperty;
        static const PropertyID PosXProperty;
        static const PropertyID PosYProperty;
        static const PropertyID WidthProperty;
        static const PropertyID HeightProperty;
        static const PropertyID PosXOffsetPorperty;
        static const PropertyID PosYOffsetPorperty;
        static const PropertyID NativeResXProperty;
        static const PropertyID NativeResYProperty;
        static const PropertyID AutoResProperty;
        static const PropertyID PropertyCount;
    };
}

#endif