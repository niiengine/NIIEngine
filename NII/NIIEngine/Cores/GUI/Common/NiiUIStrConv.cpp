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

#include "NiiPreProcess.h"
#include "NiiUIStrConv.h"

#if N_PLAT == N_PLAT_WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <iconv.h>
    #include <errno.h>
    #include <locale>
#endif

namespace NII
{
namespace UI
{
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, PlaneSizef & out, const PlaneSizef & init)
    {
        out = init;
        Nsscanf(in.c_str(), _T("%g %g"), &out.mWidth, &out.mHeight);

    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, RelVector2f & out, const RelVector2f & init)
    {
        out = init;
        Nsscanf(in.c_str(), _T("%g %g %g %g"), &out.x.mScale, &out.x.mOffset, &out.y.mScale, &out.y.mOffset);
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, RelPlaneSizef & out, const RelPlaneSizef & init)
    {
        out = init;
        Nsscanf(in.c_str(), _T("%g %g %g %g"), &out.mWidth.mScale, &out.mWidth.mOffset, &out.mHeight.mScale, &out.mHeight.mOffset);
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, AspectMode & out, const AspectMode & o)
    {
        if (in == _T("Shrink"))
        {
            out = AM_Shrink;
        }
        else if (in == _T("Expand"))
        {
            out = AM_Expand;
        }
        else
        {
            out = o;
        }
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, TabControl::TabType & out, const TabControl::TabType & o)
    {
        if (in == _T("Bottom"))
        {
            out = TabControl::Bottom;
        }
        else if(in == _T("Top"))
        {
            out = TabControl::Top;
        }
        else
        {
            out = o;
        }
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, ListHeaderItem::SortMode & out, const ListHeaderItem::SortMode & o)
    {
        if (in == _T("Ascend"))
        {
            out = ListHeaderItem::SM_Ascend;
        }
        else if (in == _T("Descend"))
        {
            out = ListHeaderItem::SM_Descend;
        }
        else
        {
            out = o;
        }
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, MultiColumnList::SelectMode & out, const MultiColumnList::SelectMode & o)
    {
        if (in == _T("SM_MultiRow"))
        {
            out = MultiColumnList::SM_MultiRow;
        }
        else if (in == _T("SM_SingleColumn"))
        {
            out = MultiColumnList::SM_SingleColumn;
        }
        else if (in == _T("SM_MultiColumn"))
        {
            out = MultiColumnList::SM_MultiColumn;
        }
        else if (in == _T("SM_SingleCell"))
        {
            out = MultiColumnList::SM_SingleCell;
        }
        else if (in == _T("SM_MultiCell"))
        {
            out = MultiColumnList::SM_MultiCell;
        }
        else if (in == _T("SM_ReserveSingleColumn"))
        {
            out = MultiColumnList::SM_ReserveSingleColumn;
        }
        else if (in == _T("SM_ReserveMultiColumn"))
        {
            out = MultiColumnList::SM_ReserveMultiColumn;
        }
        else if (in == _T("SM_ReserveSingleRow"))
        {
            out = MultiColumnList::SM_ReserveSingleRow;
        }
        else if (in == _T("SM_ReserveMultiRow"))
        {
            out = MultiColumnList::SM_ReserveMultiRow;
        }
        else
        {
            out = o;
        }
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, SizeType & out, const SizeType & o)
    {
        if (in == _T("LeftEdge"))
            out = ST_LeftPadding;
        else if (in == _T("XPosition"))
            out = ST_X;
        else if (in == _T("TopEdge"))
            out = ST_TopPadding;
        else if (in == _T("YPosition"))
            out = ST_Y;
        else if (in == _T("RightEdge"))
            out = ST_RightPadding;
        else if (in == _T("BottomEdge"))
            out = ST_BottomPadding;
        else if (in == _T("Width"))
            out = ST_Width;
        else if (in == _T("Height"))
            out = ST_Height;
        else if (in == _T("XOffset"))
            out = ST_XOft;
        else if (in == _T("YOffset"))
            out = ST_YOft;
        else
            out = o;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, FontSizeType & out, const FontSizeType & o)
    {
        if (in == _T("LineSpacing"))
            out = FST_LineSpace;
        else if (in == _T("Baseline"))
            out = FST_BaseLine;
        else
            out = o;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, SizeOpType & out, const SizeOpType & o)
    {
        if (in == _T("Add"))
            out = SOT_Add;
        else if (in == _T("Subtract"))
            out = SOT_Subtract;
        else if (in == _T("Multiply"))
            out = SOT_Multiply;
        else if (in == _T("Divide"))
            out = SOT_Divide;
        else
            out = o;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, FrameComType & out, const FrameComType & o)
    {
        if (in == _T("TopLeftCorner"))
            out = FCT_TL;
        if (in == _T("TopRightCorner"))
            out = FCT_TR;
        if (in == _T("BottomLeftCorner"))
            out = FCT_BL;
        if (in == _T("BottomRightCorner"))
            out = FCT_BR;
        if (in == _T("LeftEdge"))
            out = FCT_Left;
        if (in == _T("RightEdge"))
            out = FCT_Right;
        if (in == _T("TopEdge"))
            out = FCT_Top;
        if (in == _T("BottomEdge"))
            out = FCT_Bottom;
        else
            out = o;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, HAlign & out, const HAlign & o)
    {
        if (in == _T("centre"))
            out = HA_CENTRE;
        else if (in == _T("right"))
            out = HA_RIGHT;
        else
            out = o;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, VAlign & out, const VAlign & o)
    {
        if (in == _T("centre"))
            out = VA_CENTRE;
        else if (in == _T("bottom"))
            out = VA_BOTTOM;
        else
            out = o;
    }

    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, VLayout & out, const VLayout & o)
    {
        if (in == _T("centre"))
            out = VL_Centre;
        else if (in == _T("bottom"))
            out = VL_Bottom;
        else if (in == _T("tile"))
            out = VL_Tile;
        else if (in == _T("stretch"))
            out = VL_Expand;
        else
            out = o;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, HTextLayout & out, const HTextLayout & o)
    {
        if (in == _T("centre"))
            out = HTL_Centre;
        else if (in == _T("right"))
            out = HTL_Right;
        else if (in == _T("Justified"))
            out = HTL_Normal;
        else if (in == _T("WordWrapLeftAligned"))
            out = HTL_Wrap_Left;
        else if (in == _T("WordWrapCentreAligned"))
            out = HTL_Wrap_Centre;
        else if (in == _T("WordWrapRightAligned"))
            out = HTL_Wrap_Right;
        else if (in == _T("WordWrapJustified"))
            out = HTL_Wrap_Expand;
        else
            out = o;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, HLayout & out, const HLayout & o)
    {
        if (in == _T("centre"))
            out = HL_Centre;
        else if (in == _T("right"))
            out = HL_Right;
        else if (in == _T("tile"))
            out = HL_Tile;
        else if (in == _T("stretch"))
            out = HL_Expand;
        else
            out = o;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, VTextLayout & out, const VTextLayout & o)
    {
        if (in == _T("centre"))
            out = VTL_Centre;
        else if (in == _T("bottom"))
            out = VTL_Bottom;
        else
            out = o;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, ColourArea & out, const ColourArea & o)
    {
        if (in.length() == 8)
        {
            Nui32 all = 0xFF000000;
            Nsscanf(in.c_str(), _T("%8X"), &all);
            out.mTL = Colour(ARGB(all));
            out.mTR = Colour(ARGB(all));
            out.mBL = Colour(ARGB(all));
            out.mBR = Colour(ARGB(all));
        }
        else if (in.length() == 0)
        {
            out = o;
        }
        else
        {
            Nui32 tl = o.mTL.getARGB(), tr = o.mTR.getARGB(), bl = o.mBL.getARGB(), br = o.mBR.getARGB();
            Nsscanf(in.c_str(), _T("%8X %8X %8X %8X"), &tl, &tr, &bl, &br);

            out.mTL = Colour(ARGB(tl));
            out.mTR = Colour(ARGB(tr));
            out.mBL = Colour(ARGB(bl));
            out.mBR = Colour(ARGB(br));
        }
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const PlaneSizef & in, String & out)
    {
        out.resize(64);
        NIIi n = Nsntprintf(&out[0], 64, _T("%g %g"), in.mWidth, in.mHeight);
        if (n > 0)
            out.resize(n);
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const RelVector2f & in, String & out)
    {
        out.resize(128);
        NIIi n = Nsntprintf(&out[0], 128, _T("%g %g %g %g"), in.x.mScale, in.x.mOffset, in.y.mScale, in.y.mOffset);
        if (n > 0)
            out.resize(n);
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const RelPlaneSizef & in, String & out)
    {
        out.resize(128);
        NIIi n = Nsntprintf(&out[0], 128, _T("%g %g %g %g"), in.mWidth.mScale, in.mWidth.mOffset, in.mHeight.mScale, in.mHeight.mOffset);
        if (n > 0)
            out.resize(n);
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const AspectMode & in, String & out)
    {
        switch (in)
        {
        case AM_None :
            out = _T("Ignore");
        case AM_Shrink :
            out = _T("Shrink");
        case AM_Expand :
            out = _T("Expand");
        default:
            N_assert(0, "Invalid aspect mode");
            out = _T("Ignore");
        }
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const TabControl::TabType & in, String & out)
    {
        switch (in)
        {
        case TabControl::Top :
            out = _T("Top");
        case TabControl::Bottom :
            out = _T("Bottom");
        default:
            out = _T("Top");
        }
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const ListHeaderItem::SortMode & in, String & out)
    {
        switch (in)
        {
        case ListHeaderItem::SM_None:
            out = _T("None");
        case ListHeaderItem::SM_Ascend:
            out = _T("Ascend");
        case ListHeaderItem::SM_Descend:
            out = _T("Descend");
        default:
            out = _T("Ascend");
        }
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const MultiColumnList::SelectMode & in, String & out)
    {
        switch (in)
        {
        case MultiColumnList::SM_MultiRow:
            out = _T("SM_MultiRow");
        case MultiColumnList::SM_SingleColumn:
            out = _T("SM_SingleColumn");
        case MultiColumnList::SM_MultiColumn:
            out = _T("SM_MultiColumn");
        case MultiColumnList::SM_SingleCell:
            out = _T("SM_SingleCell");
        case MultiColumnList::SM_MultiCell:
            out = _T("SM_MultiCell");
        case MultiColumnList::SM_ReserveSingleColumn:
            out = _T("SM_ReserveSingleColumn");
        case MultiColumnList::SM_ReserveMultiColumn:
            out = _T("SM_ReserveMultiColumn");
        case MultiColumnList::SM_ReserveSingleRow:
            out = _T("SM_ReserveSingleRow");
        case MultiColumnList::SM_ReserveMultiRow:
            out = _T("SM_ReserveMultiRow");
        default:
            out = _T("SM_SingleRow");
        }
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const SizeType & in, String & out)
    {
        switch (in)
        {
        case ST_LeftPadding:
            out = _T("LeftEdge");
        case ST_X:
            out = _T("XPosition");
        case ST_TopPadding:
            out = _T("TopEdge");
        case ST_Y:
            out = _T("YPosition");
        case ST_RightPadding:
            out = _T("RightEdge");
        case ST_BottomPadding:
            out = _T("BottomEdge");
        case ST_Width:
            out = _T("Width");
        case ST_Height:
            out = _T("Height");
        case ST_XOft:
            out = _T("XOffset");
        case ST_YOft:
            out = _T("YOffset");
        default:
            out = _T("Invalid");
        }
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const FontSizeType & in, String & out)
    {
        switch (in)
        {
        case FST_BaseLine:
            out = _T("Baseline");
        case FST_TextEntent:
            out = _T("HorzExtent");
        default:
            out = _T("LineSpacing");
        }
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const SizeOpType & in, String & out)
    {
        switch (in)
        {
        case SOT_Add:
            out = _T("Add");
        case SOT_Subtract:
            out = _T("Subtract");
        case SOT_Multiply:
            out = _T("Multiply");
        case SOT_Divide:
            out = _T("Divide");
        default:
            out = _T("Noop");
        }
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const FrameComType & in, String & out)
    {
        switch (in)
        {
        case FCT_TL:
            out = _T("TopLeftCorner");
        case FCT_TR:
            out = _T("TopRightCorner");
        case FCT_BL:
            out = _T("BottomLeftCorner");
        case FCT_BR:
            out = _T("BottomRightCorner");
        case FCT_Left:
            out = _T("LeftEdge");
        case FCT_Right:
            out = _T("RightEdge");
        case FCT_Top:
            out = _T("TopEdge");
        case FCT_Bottom:
            out = _T("BottomEdge");
        default:
            out = _T("Bg");
        }
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const HAlign & in, String & out)
    {
        switch (in)
        {
        case HA_RIGHT:
            out = _T("right");
        case HA_CENTRE:
            out = _T("centre");
        default:
            out = _T("left");
        }
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const VAlign & in, String & out)
    {
        switch (in)
        {
        case VA_BOTTOM:
            out = _T("bottom");
        case VA_CENTRE:
            out = _T("centre");
        default:
            out = _T("top");
        }
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const VLayout & in, String & out)
    {
        switch (in)
        {
        case VL_Bottom:
            out = _T("bottom");
        case VL_Centre:
            out = _T("centre");
        case VL_Tile:
            out = _T("tile");
        case VL_Expand:
            out = _T("stretch");
        default:
            out = _T("top");
        }
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const HTextLayout & in, String & out)
    {
        switch (in)
        {
        case HTL_Right:
            out = _T("right");
        case HTL_Centre:
            out = _T("centre");
        case HTL_Normal:
            out = _T("Justified");
        case HTL_Wrap_Left:
            out = _T("WordWrapLeftAligned");
        case HTL_Wrap_Right:
            out = _T("WordWrapRightAligned");
        case HTL_Wrap_Centre:
            out = _T("WordWrapCentreAligned");
        case HTL_Wrap_Expand:
            out = _T("WordWrapJustified");
        default:
            out = _T("left");
        }
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const HLayout & in, String & out)
    {
        switch (in)
        {
        case HL_Right:
            out = _T("right");
        case HL_Centre:
            out = _T("centre");
        case HL_Tile:
            out = _T("tile");
        case HL_Expand:
            out = _T("stretch");
        default:
            out = _T("left");
        }
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const VTextLayout & in, String & out)
    {
        switch (in)
        {
        case VTL_Bottom:
            out = _T("bottom");
        case VTL_Centre:
            out = _T("centre");
        default:
            out = _T("top");
        }
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const ColourArea & in, String & out)
    {
        out.resize(128);
        NIIi n = Nsntprintf(&out[0], 128, _T("%.8X %.8X %.8X %.8X"), in.mTL.getARGB(), in.mTR.getARGB(), in.mBL.getARGB(), in.mBR.getARGB());
        if (n > 0)
            out.resize(n);
        return out;
    }
    //-----------------------------------------------------------------------
}
}