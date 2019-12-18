/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2017-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_UISTRCONV_H_
#define _NII_UISTRCONV_H_

#include "NiiUIPreInclude.h"
#include "NiiStrConv.h"
#include "NiiUICommon.h"
#include "NiiUIStyleSectionFrameUnit.h"
#include "NiiUIListHeaderSegment.h"
#include "NiiUIMultiColumnList.h"
#include "NiiUITabControl.h"
#include "NiiColourArea.h"
#include "NiiPlaneSize.h"

namespace NII
{
namespace UI
{
    /** 把单位数据 转为/换成 字符串类型的类(UI级)
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UIStrConv : public StrConv
    {
    public:
        using StrConv::conv;

        /** 从字符串中解析出 PlaneSizef
        @remark 格式"width hegiht", 2个NIIf
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, PlaneSizef & out, const PlaneSizef & o = PlaneSizef(0,0));

        /** 从字符串中解析出 RelVector2
        @remark 格式"x.mScale x.mOffset y.mScale y.mOffset", 4个NIIf
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, RelVector2 & out, const RelVector2 & o = RelVector2(RelPos(0, 0), RelPos(0, 0)));

        /** 从字符串中解析出 RelPlaneSize
        @remark 格式"mWidth.mScale mWidth.mOffset mHeight.mScale mHeight.mOffset", 4个NIIf
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, RelPlaneSize & out, const RelPlaneSize & o = RelPlaneSize(RelPos(0, 0), RelPos(0, 0)));
        
        /** 从字符串中解析出 AspectMode
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, AspectMode & out, const AspectMode & o = AM_None);

        /** 从字符串中解析出 TabControl::TabPanePosition
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, TabControl::TabPanePosition & out, const TabControl::TabPanePosition & o = TabControl::Top);

        /** 从字符串中解析出 ListHeaderSegment::SortDirection
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, ListHeaderSegment::SortDirection & out, const ListHeaderSegment::SortDirection & o = ListHeaderSegment::None);

        /** 从字符串中解析出 MultiColumnList::SelectionMode
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, MultiColumnList::SelectionMode & out, const MultiColumnList::SelectionMode & o = MultiColumnList::RowSingle);

        /** 从字符串中解析出 SizeType
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, SizeType & out, const SizeType & o = ST_Unknow);

        /** 从字符串中解析出 FontSizeType
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, FontSizeType & out, const FontSizeType & o = FST_TextEntent);

        /** 从字符串中解析出 SizeOpType
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, SizeOpType &out, const SizeOpType & o = SOT_Unknow);

        /** 从字符串中解析出 FrameComType
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, FrameComType & out, const FrameComType & o = FCT_Bg);

        /** 从字符串中解析出 HAlign
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, HAlign & out, const HAlign & o = HA_LEFT);

        /** 从字符串中解析出 VAlign
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, VAlign & out, const VAlign & o = VA_TOP);

        /** 从字符串中解析出 VLayout
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, VLayout & out, const VLayout & o = VL_Top);

        /** 从字符串中解析出 HTextLayout
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, HTextLayout & out, const HTextLayout & o = HTL_Left);

        /** 从字符串中解析出 HLayout
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, HLayout & out, const HLayout & o = HL_Left);

        /** 从字符串中解析出 VTextLayout
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, VTextLayout & out, const VTextLayout & o = VTL_Top);

        /** 从字符串中解析出ColourArea
        @remark 格式"r g b a" ,4个NIIf 或 "r g b" a值为1.0
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, ColourArea & out, const ColourArea & o = Colour::Black);

        /** 转换 PlaneSizef 为字符串.
        @version NIIEngine 3.0.0
        */
        static const String & conv(const PlaneSizef & in, String & out);

        /** 转换 RelVector2 为字符串.
        @version NIIEngine 3.0.0
        */
        static const String & conv(const RelVector2 & in, String & out);

        /** 转换 RelPlaneSize 为字符串.
        @version NIIEngine 3.0.0
        */
        static const String & conv(const RelPlaneSize & in, String & out);

        /** 转换 AspectMode 为字符串.
        @version NIIEngine 3.0.0
        */
        static const String & conv(const AspectMode & in, String & out);

        /** 转换 TabControl::TabPanePosition 为字符串.
        @version NIIEngine 3.0.0
        */
        static const String & conv(const TabControl::TabPanePosition & in, String & out);

        /** 转换 ListHeaderSegment::SortDirection 为字符串.
        @version NIIEngine 3.0.0
        */
        static const String & conv(const ListHeaderSegment::SortDirection & in, String & out);

        /** 转换 MultiColumnList::SelectionMode 为字符串.
        @version NIIEngine 3.0.0
        */
        static const String & conv(const MultiColumnList::SelectionMode & in, String & out);

        /** 转换 SizeType 为字符串.
        @version NIIEngine 3.0.0
        */
        static const String & conv(const SizeType & in, String & out);

        /** 转换 FontSizeType 为字符串.
        @version NIIEngine 3.0.0
        */
        static const String & conv(const FontSizeType & in, String & out);

        /** 转换 SizeOpType 为字符串.
        @version NIIEngine 3.0.0
        */
        static const String & conv(const SizeOpType & in, String & out);

        /** 转换 FrameComType 为字符串.
        @version NIIEngine 3.0.0
        */
        static const String & conv(const FrameComType & in, String & out);

        /*
        @version NIIEngine 3.0.0
        */
        static const String & conv(const HAlign & in, String & out);

        /*
        @version NIIEngine 3.0.0
        */
        static const String & conv(const VAlign & in, String & out);

        /*
        @version NIIEngine 3.0.0
        */
        static const String & conv(const VLayout & in, String & out);

        /*
        @version NIIEngine 3.0.0
        */
        static const String & conv(const HTextLayout & in, String & out);

        /*
        @version NIIEngine 3.0.0
        */
        static const String & conv(const HLayout & in, String & out);

        /*
        @version NIIEngine 3.0.0
        */
        static const String & conv(const VTextLayout & in, String & out);

        /** 转换ColourArea为字符串.
        @remark 格式"r g b a",4个NIIf
        @version NIIEngine 3.0.0
        */
        static const String & conv(const ColourArea & in, String & out);
    };

    template<typename T> N_FORCEINLINE String NUI_conv(T in)
    {
        String temp;
        return UIStrConv::conv(in, temp);
    }
}
}
#endif