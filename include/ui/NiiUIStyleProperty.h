/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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