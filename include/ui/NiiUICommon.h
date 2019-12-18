/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-7

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

#ifndef _NII_UI_COMMON_H_
#define _NII_UI_COMMON_H_

#include "NiiUIPreInclude.h"
#include "NiiColourArea.h"
#include "NiiRect.h"

namespace NII
{
    static const StateID StateCount;
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

    typedef Vector2<RelPos> RelVector2;
    typedef PlaneSize<RelPos> RelPlaneSize;

    inline Vector2f abs(const RelVector2 & pos, const PlaneSizef & base, bool pixelAlign = true)
    {
        return Vector2f(pos.x.abs(base.mWidth, pixelAlign), pos.y.abs(base.mHeight, pixelAlign));
    }

    inline PlaneSizef abs(const RelPlaneSize & pos, const PlaneSizef & base, bool pixelAlign = true)
    {
        return PlaneSizef(pos.mWidth.abs(base.mWidth, pixelAlign), pos.mHeight.abs(base.mHeight, pixelAlign));
    }

    inline Rectf abs(const RelRect & pos, const PlaneSizef & base, bool pixelAlign = true)
    {
        return Rectf(
            pos.mLeft.abs(base.mWidth, pixelAlign),
            pos.mTop.abs(base.mHeight, pixelAlign),
            pos.mRight.abs(base.mWidth, pixelAlign),
            pos.mBottom.abs(base.mHeight, pixelAlign));
    }

}
}
#endif