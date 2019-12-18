/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-1-7

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

#ifndef _NII_UISTYLE_PROPERTY_H_
#define _NII_UISTYLE_PROPERTY_H_

namespace NII
{
namespace UI
{
    class _EngineAPI StyleProperty
    {
    public:
        static const PropertyID StyleUnitProperty;
        static const PropertyID WidgetStyleUnitProperty;
        static const PropertyID StyleWidgetUnitProperty;
        static const PropertyID StyleSectionUnitProperty;
        static const PropertyID StyleLayoutUnitProperty;
        static const PropertyID LayerUnitProperty;
        static const PropertyID SectionUnitProperty;
        static const PropertyID SectionImageUnitProperty;
        static const PropertyID SectionTextUnitProperty;
        static const PropertyID SectionFrameUnitProperty;
        static const PropertyID ComAreaUnitProperty;
        static const PropertyID ComPixelUnitProperty;
        static const PropertyID ColoursUnitProperty;
        static const PropertyID VLayoutUnitProperty;
        static const PropertyID HLayoutUnitProperty;
        static const PropertyID VAlignUnitProperty;
        static const PropertyID HAlignUnitProperty;
        static const PropertyID PropertyUnitProperty;
		static const PropertyID SizeDefineProperty;
		static const PropertyID RelSizeUnitProperty;
        static const PropertyID AbsSizeUnitProperty;
        static const PropertyID PixelSizeUnitProperty;
        static const PropertyID PixelSizeIDUnitProperty;
        static const PropertyID WidgetSizeUnitProperty;
        static const PropertyID FontSizeUnitProperty;
        static const PropertyID SizeIDUnitProperty;
        static const PropertyID TextUnitProperty;
        static const PropertyID ColourUnitProperty;
        static const PropertyID ColoursIDUnitProperty;
        static const PropertyID StyleSpecialUnitProperty;
        static const PropertyID StylePropertyUnitProperty;
        static const PropertyID ListenerPropertyUnitProperty;
        static const PropertyID ListenerTargetUnitProperty;
        static const PropertyID SizeOpUnitProperty;
        static const PropertyID VLayoutIDUnitProperty;
        static const PropertyID HLayoutIDUnitProperty;
        static const PropertyID ComAreaIDUnitProperty;
        static const PropertyID PixelIDUnitProperty;
        static const PropertyID TextIDUnitProperty;
        static const PropertyID FontIDUnitProperty;
        static const PropertyID ListenerEventUnitProperty;
        static const PropertyID EventTargetUnitProperty;
        static const PropertyID StyleSpecialIDUnitProperty;

        static const PropertyID TopLeftAttribute;
        static const PropertyID TopRightAttribute;
        static const PropertyID BottomLeftAttribute;
        static const PropertyID BottomRightAttribute;
        static const PropertyID PriorityAttribute;
        static const PropertyID WidgetStyleAttribute;
        static const PropertyID ScaleAttribute;
        static const PropertyID OffsetAttribute;
        static const PropertyID WidgetAttribute;
        static const PropertyID StringAttribute;
        static const PropertyID FontAttribute;
        static const PropertyID InitialValueAttribute;
        static const PropertyID ClippedAttribute;
        static const PropertyID PaddingAttribute;
        static const PropertyID LayoutOnWriteAttribute;
        static const PropertyID RedrawOnWriteAttribute;
        static const PropertyID TargetPropertyAttribute;
        static const PropertyID ControlPropertyAttribute;
        static const PropertyID PropertyAttribute;
        static const PropertyID ControlValueAttribute;
        static const PropertyID ControlWidgetAttribute;
        static const PropertyID HelpStringAttribute;
        static const PropertyID EventAttribute;
        static const PropertyID InheritsAttribute;
        static const PropertyID AutoWindowAttribute;
        static const PropertyID FireEventAttribute;
        static const PropertyID PixelIDProperty;
        static const PropertyID PropertyCount;
    };
}
}
#endif