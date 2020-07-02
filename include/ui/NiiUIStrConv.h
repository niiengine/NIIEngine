/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/

#ifndef _NII_UISTRCONV_H_
#define _NII_UISTRCONV_H_

#include "NiiUIPreInclude.h"
#include "NiiStrConv.h"
#include "NiiUICommon.h"
#include "NiiUIStyleSectionFrameUnit.h"
#include "NiiUIListHeaderItem.h"
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
        static void conv(const String & in, RelVector2f & out, const RelVector2f & o = RelVector2f(RelPosf(0, 0), RelPosf(0, 0)));

        /** 从字符串中解析出 RelPlaneSize
        @remark 格式"mWidth.mScale mWidth.mOffset mHeight.mScale mHeight.mOffset", 4个NIIf
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, RelPlaneSizef & out, const RelPlaneSizef & o = RelPlaneSizef(RelPosf(0, 0), RelPosf(0, 0)));
        
        /** 从字符串中解析出 AspectMode
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, AspectMode & out, const AspectMode & o = AM_None);

        /** 从字符串中解析出 TabControl::TabType
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, TabControl::TabType & out, const TabControl::TabType & o = TabControl::Top);

        /** 从字符串中解析出 ListHeaderItem::SortMode
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, ListHeaderItem::SortMode & out, const ListHeaderItem::SortMode & o = ListHeaderItem::SM_None);

        /** 从字符串中解析出 MultiColumnList::SelectMode
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认值
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, MultiColumnList::SelectMode & out, const MultiColumnList::SelectMode & o = MultiColumnList::SM_SingleRow);

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
        static const String & conv(const RelVector2f & in, String & out);

        /** 转换 RelPlaneSize 为字符串.
        @version NIIEngine 3.0.0
        */
        static const String & conv(const RelPlaneSizef & in, String & out);

        /** 转换 AspectMode 为字符串.
        @version NIIEngine 3.0.0
        */
        static const String & conv(const AspectMode & in, String & out);

        /** 转换 TabControl::TabType 为字符串.
        @version NIIEngine 3.0.0
        */
        static const String & conv(const TabControl::TabType & in, String & out);

        /** 转换 ListHeaderItem::SortMode 为字符串.
        @version NIIEngine 3.0.0
        */
        static const String & conv(const ListHeaderItem::SortMode & in, String & out);

        /** 转换 MultiColumnList::SelectMode 为字符串.
        @version NIIEngine 3.0.0
        */
        static const String & conv(const MultiColumnList::SelectMode & in, String & out);

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

        /** 转换
        @version NIIEngine 3.0.0
        */
        static const String & conv(const HAlign & in, String & out);

        /** 转换
        @version NIIEngine 3.0.0
        */
        static const String & conv(const VAlign & in, String & out);

        /** 转换
        @version NIIEngine 3.0.0
        */
        static const String & conv(const VLayout & in, String & out);

        /** 转换
        @version NIIEngine 3.0.0
        */
        static const String & conv(const HTextLayout & in, String & out);

        /** 转换
        @version NIIEngine 3.0.0
        */
        static const String & conv(const HLayout & in, String & out);

        /** 转换
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