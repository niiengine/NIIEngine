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

#ifndef _NII_UI_COMMON_H_
#define _NII_UI_COMMON_H_

#include "NiiUIPreInclude.h"
#include "NiiColour.h"
#include "NiiRect.h"

namespace NII
{
namespace UI
{
    /** 更新模式
    @version NIIEngine 3.0.0
    */
    enum RefreshMode
    {
        RM_Alway,           ///< 总是更新
        RM_Never,           ///< 不更新
        RM_Visible          ///< 可见时更新
    };

    /**
    @version NIIEngine 3.0.0
    */
    enum SizeType
    {
        ST_Unknow,          ///<
        ST_LeftPadding,     ///<
        ST_TopPadding,      ///<
        ST_RightPadding,    ///<
        ST_BottomPadding,   ///<
        ST_Width,           ///<
        ST_Height,          ///<
        ST_X,               ///<
        ST_Y,               ///<
        ST_XOft,            ///<
        ST_YOft             ///<
    };

    /**
    @version NIIEngine 3.0.0
    */
    enum SizeOpType
    {
        SOT_Unknow,
        SOT_Add,
        SOT_Subtract,
        SOT_Multiply,
        SOT_Divide
    };

    /** 垂直排版模式
    @version NIIEngine 3.0.0
    */
    enum VLayout
    {
        VL_Top,             ///< 
        VL_Centre,          ///<
        VL_Bottom,          ///<
        VL_Expand,          ///<
        VL_Tile             ///< 
    };

    /** 水平排版模式
    @version NIIEngine 3.0.0
    */
    enum HLayout
    {
        HL_Left,            ///<
        HL_Centre,          ///<
        HL_Right,           ///<
        HL_Expand,          ///<
        HL_Tile             ///< 
    };

    /** 文字垂直排版模式
    @version NIIEngine 3.0.0
    */
    enum VTextLayout
    {
        VTL_Top,            ///<
        VTL_Centre,         ///<
        VTL_Bottom          ///<
    };

    /** 文字水平排版模式
    @version NIIEngine 3.0.0
    */
    enum HTextLayout
    {
        HTL_Left,           ///< 
        HTL_Right,          ///< 
        HTL_Centre,         ///< 
        HTL_Normal,         ///< 
        HTL_Wrap_Expand,    ///< 
        HTL_Wrap_Left,      ///< 
        HTL_Wrap_Right,     ///< 
        HTL_Wrap_Centre     ///< 
    };

    /**
    @version NIIEngine 3.0.0
    */
    enum FontSizeType
    {
        FST_LineSpace,      ///<
        FST_BaseLine,       ///<
        FST_TextEntent      ///<
    };

    typedef Vector2<RelPosi> RelVector2i;
    typedef Vector2<RelPosf> RelVector2f;

    typedef PlaneSize<RelPosi> RelPlaneSizei;
    typedef PlaneSize<RelPosf> RelPlaneSizef;

    template <typename T, typename T2> inline Vector2<T2> abs(const Vector2<T> & pos, const PlaneSize<T2> & base, bool pixelAlign = true)
    {
        return Vector2<T2>(pos.x.abs(base.mWidth, pixelAlign), pos.y.abs(base.mHeight, pixelAlign));
    }

    template <typename T, typename T2> inline PlaneSize<T2> abs(const PlaneSize<T> & pos, const PlaneSize<T2> & base, bool pixelAlign = true)
    {
        return PlaneSize<T2>(pos.mWidth.abs(base.mWidth, pixelAlign), pos.mHeight.abs(base.mHeight, pixelAlign));
    }

    template <typename T, typename T2> inline TRect<T2> abs(const TRect<T> & pos, const PlaneSize<T2> & base, bool pixelAlign = true)
    {
        return TRect<T2>(
            pos.mLeft.abs(base.mWidth, pixelAlign),
            pos.mTop.abs(base.mHeight, pixelAlign),
            pos.mRight.abs(base.mWidth, pixelAlign),
            pos.mBottom.abs(base.mHeight, pixelAlign));
    }

}
}
#endif