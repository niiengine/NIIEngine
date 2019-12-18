/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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

#ifndef _NII_COLOUR_H_
#define _NII_COLOUR_H_

#include "NiiPreInclude.h"
#include "NiiString.h"

namespace NII
{
    /** 描述颜色的类
    @remark 颜色表示为四部分,从0.0到1.0浮点.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Colour
    {
    public:
        Colour();
        Colour(RGBA rgba);
        explicit Colour(NIIf val[4]);
        explicit Colour(NIIf val[3], NIIf a);
        explicit Colour(NIIf r, NIIf g, NIIf b, NIIf a = 1.0f);

		Colour & operator= (const Colour & o);
        bool operator== (const Colour & o) const;
        bool operator!= (const Colour & o) const;
        NIIf operator [](NCount i) const;

        Colour operator + (const Colour & o) const;
        Colour operator - (const Colour & o) const;
        Colour operator * (const Colour & o) const;
        Colour operator / (const Colour & o) const;
        
        Colour operator + (NIIf f) const;
        Colour operator - (NIIf f) const;
        Colour operator * (NIIf f) const;
        Colour operator / (NIIf f) const;
        
        Colour & operator += (const Colour & o);
        Colour & operator -= (const Colour & o);
		Colour & operator *= (const Colour & o);
		Colour & operator /= (const Colour & o);
        Colour & operator *= (const NIIf f);
        Colour & operator /= (const NIIf f);

        friend Colour operator * (NIIf f, const Colour & o);
        friend Colour operator / (NIIf f, const Colour & o);

        /** 设置多个成分
        @param[in] r 红色
        @param[in] g 绿色
        @param[in] b 蓝色
        @param[in] a 透明
        @version NIIEngine 3.0.0
        */
        void set(NIIf r, NIIf g, NIIf b, NIIf a = 1.0f);

        /** 设置红色成分
        @version NIIEngine 3.0.0
        */
        void setRed(NIIf f);

        /** 获取红色成分
        @version NIIEngine 3.0.0
        */
        NIIf getRed() const;

        /** 设置绿色成分
        @version NIIEngine 3.0.0
        */
        void setGreen(NIIf f);

        /** 获取绿色成分
        @version NIIEngine 3.0.0
        */
        NIIf getGreen() const;

        /** 设置蓝色成分
        @version NIIEngine 3.0.0
        */
        void setBlue(NIIf f);

        /** 获取蓝色成分
        @version NIIEngine 3.0.0
        */
        NIIf getBlue() const;

        /** 设置透明成分
        @version NIIEngine 3.0.0
        */
        void setAlpha(NIIf f);

        /** 获取透明成分
        @version NIIEngine 3.0.0
        */
        NIIf getAlpha() const;

        /** 翻转
        @version NIIEngine 3.0.0
        */
        void invert();

        /** 限制颜色值在[0, 1].
        @version NIIEngine 3.0.0
        */
        void clamp();

        /** 设置RGBA颜色
        @version NIIEngine 3.0.0
        */
        void setRGBA(RGBA val);

        /** 获取RGBA颜色
        @version NIIEngine 3.0.0
        */
        RGBA getRGBA() const;

        /** 设置ARGB颜色
        @version NIIEngine 3.0.0
        */
        void setARGB(ARGB val);

        /** 获取ARGB颜色
        @version NIIEngine 3.0.0
        */
        ARGB getARGB() const;

        /** 设置BGRA颜色
        @version NIIEngine 3.0.0
        */
        void setBGRA(BGRA val);

        /** 获取BGRA颜色
        @version NIIEngine 3.0.0
        */
        BGRA getBGRA() const;

        /** 设置ABGR颜色
        @version NIIEngine 3.0.0
        */
        void setABGR(ABGR val);

        /** 获取ABGR颜色
        @version NIIEngine 3.0.0
        */
        ABGR getABGR() const;

        /** 色调
        @version NIIEngine 3.0.0
        */
        NIIf getHue() const;

        /** 饱和
        @version NIIEngine 3.0.0
        */
        NIIf getSatura() const;

        /** 亮度
        @version NIIEngine 3.0.0
        */
        NIIf getLumen() const;

        /** 从色调,饱和度和亮度中设置出一个颜色
        @param[in] hue 色调值,缩放到[0,1]
        @param[in] satura 饱和程度[0,1]
        @param[in] lumen 亮度级别[0,1]
        @version NIIEngine 3.0.0
        */
        void setHSL(NIIf hue, NIIf satura, NIIf lumen);

        /** 转换当前颜色到色相,饱和度和亮度值
        @param[in] hue 色调值,缩放到[0,1]
        @param[in] satura 饱和程度[0,1]
        @param[in] lumen 亮度级别[0,1]
        @version NIIEngine 3.0.0
        */
        void getHSL(NIIf * hue, NIIf * satura, NIIf * lumen) const;

        /** 文本描述
        @version NIIEngine 3.0.0
        */
        String toStr() const;
        
        static const Colour ZERO;
        static const Colour Black;
        static const Colour White;
        static const Colour Red;
        static const Colour Green;
        static const Colour Magenta;
        static const Colour Blue;
        static const Colour CyanBlue;
        static const Colour Yellow;
    public:
        NIIf r;
        NIIf g;
        NIIf b;
        NIIf a;
    };

    inline _EngineAPI std::ostream & operator << (std::ostream & o, const Colour & c)
    {
        o << "Colour(" << c.r << "," << c.g << "," << c.b << "," << c.a << ")";
        return o;
    }
}
#endif