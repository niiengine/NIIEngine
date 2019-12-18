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
#ifndef _NII_ColourArea_H_
#define _NII_ColourArea_H_

#include "NiiPreInclude.h"
#include "NiiColour.h"

namespace NII
{
    /** 图形专用颜色区域
    @remark
        四角颜色定义,中间颜色过度
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ColourArea : public UIAlloc
    {
    public:
        ColourArea();
        ColourArea(const Colour & c);
        ColourArea(const Colour & tl, const Colour & tr, const Colour & bl, const Colour & br);

        /** 赋值
        @version NIIEngine 3.0.0
        */
        void set(const Colour & c);

        /** 获取
        @version NIIEngine 3.0.0
        */
        void get(Colour & c);

        /** 赋值
        @note 会比副本对象快点点?
        @version NIIEngine 3.0.0
        */
        void set(const Colour & tl, const Colour & tr, const Colour & bl, const Colour & br);

        /** 获取
        @note 会比副本对象快点点?
        @version NIIEngine 3.0.0
        */
        void get(Colour & tl, Colour & tr, Colour & bl, Colour & br);

        /** 获取区域色区
        @version NIIEngine 3.0.0
        */
        ColourArea get(const Rectf & rect) const;

        /** 算术 +
        @version nIIEngine 3.0.0
        */
        ColourArea operator+(NIIf factor) const;

        /** 算术 - 
        @version nIIEngine 3.0.0
        */
        ColourArea operator-(NIIf factor) const;

        /** 算术 *
        @version nIIEngine 3.0.0
        */
        ColourArea operator*(NIIf factor) const;

        /** 算术 /
        @version nIIEngine 3.0.0
        */
        ColourArea operator/(NIIf factor) const;

        /** 算术 *=
        @version NIIEngine 3.0.0
        */
        ColourArea & operator*=(const ColourArea & o);

        /** 算术 =
        @version NIIEngine 3.0.0
        */
        ColourArea & operator=(const ColourArea & o);
        
        /** 算术 +
        @version NIIEngine 3.0.0
        */
        ColourArea operator+(const ColourArea & o) const;

        /** 算术 -
        @version NIIEngine 3.0.0
        */
        ColourArea operator-(const ColourArea & o) const;

        /** 算术 *
        @version NIIEngine 3.0.0
        */
        ColourArea operator*(const ColourArea & o) const;

        /** 算术 /
        @version NIIEngine 3.0.0
        */
        ColourArea operator/(const ColourArea & o) const;

        /** 设置透明度
        @version NIIEngine 3.0.0
        */
        void setAlpha(NIIf a);

        /** 设置透明度因子
        @version NIIEngine 3.0.0
        */
        void setAlphaFactor(NIIf a);

        /** 设置透明度
        @version NIIEngine 3.0.0
        */
        void setLeftAlpha(NIIf a);

        /** 设置透明度
        @version NIIEngine 3.0.0
        */
        void setTopAlpha(NIIf a);

        /** 设置透明度
        @version NIIEngine 3.0.0
        */
        void setRightAlpha(NIIf a);

        /** 设置透明度
        @version NIIEngine 3.0.0
        */
        void setBottomAlpha(NIIf a);

        /** 是否只有一种颜色
        @version NIIEngine 3.0.0
        */
        bool isOneColour() const;

        /** 获取区位颜色
        @version NIIEngine 3.0.0
        */
        Colour get(NIIf x, NIIf y) const;
    public:
        Nid mID;
        Colour mTL;
        Colour mTR;
        Colour mBL;
        Colour mBR;
    };
}
#endif