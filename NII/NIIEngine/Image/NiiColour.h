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

#ifndef _NII_COLOUR_H_
#define _NII_COLOUR_H_

#include "NiiPreInclude.h"
#include "NiiRect.h"
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
        inline Colour() {}
        inline Colour(RGBA rgba) { setRGBA(rgba); }
        explicit Colour(NIIf val[4]);
        explicit Colour(NIIf val[3], NIIf a);
        explicit Colour(NIIf r, NIIf g, NIIf b, NIIf a = 1.0f);

        inline Colour& operator= (const Colour & o) { r = o.r; g = o.g; b = o.b; a = o.a; return *this; }
        inline bool operator== (const Colour & o) const { return (r == o.r && g == o.g && b == o.b && a == o.a); }
        inline bool operator!= (const Colour & o) const { return (r != o.r || g != o.g || b != o.b || a != o.a); }
        inline bool operator< (const Colour & o) const { return r + g + b + a < o.r + o.g + o.b + o.a; }
        inline NIIf operator [](NCount i) const { return *(&r + i); }

        inline Colour operator + (const Colour & o) const { return Colour(r + o.r, g + o.g, b + o.b, a + o.a); }
        inline Colour operator - (const Colour & o) const { return Colour(r - o.r, g - o.g, b - o.b, a - o.a); }
        inline Colour operator * (const Colour & o) const { return Colour(o.r * r, o.g * g, o.b * b, o.a * a); }
        inline Colour operator / (const Colour & o) const 
        { return Colour(o.r == 0 ? r : r / o.r, o.g == 0 ? g : g / o.g, o.b == 0 ? b : b / o.b, o.a == 0 ? a : a / o.a); }
        
        inline Colour operator + (NIIf f) const { return Colour(f + r, f + g, f + b, f + a); }
        inline Colour operator - (NIIf f) const { return Colour(r - f, g - f, b - f, a - f); }
        inline Colour operator * (NIIf f) const { return Colour(f * r, f * g, f * b, f * a);  }
        Colour operator / (NIIf f) const;
        
        inline Colour & operator += (const Colour & o) { r += o.r; g += o.g; b += o.b; a += o.a; return *this; }
        inline Colour & operator -= (const Colour & o) { r -= o.r; g -= o.g; b -= o.b; a -= o.a; return *this; }
        inline Colour & operator *= (const Colour & o) { r *= o.r; g *= o.g; b *= o.b; a *= o.a; return *this; }
        Colour & operator /= (const Colour & o);
        inline Colour & operator *= (const NIIf f) { r *= f; g *= f; b *= f; a *= f; return *this; }
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
        inline void set(NIIf r, NIIf g, NIIf b, NIIf a = 1.0f) { r = r; g = g; b = b; a = a; }

        /** 获取RGBA颜色
        @version NIIEngine 3.0.0
        */
        inline const NIIf * ptr() const { return &r; }

        /** 获取RGBA颜色
        @version NIIEngine 3.0.0
        */
        inline NIIf * ptr() { return &r; }

        /** 获取RGBA颜色
        @version NIIEngine 3.0.0
        */
        inline void getRGBA(NIIf * out) const { out[0] = r; out[1] = g; out[2] = b; out[3] = a; }

        /** 获取BGRA颜色
        @version NIIEngine 3.0.0
        */
        inline void getBGRA(NIIf * out) const { out[0] = b; out[1] = g; out[2] = r; out[3] = a; }

        /** 设置红色成分
        @version NIIEngine 3.0.0
        */
        inline void setRed(NIIf v) { r = v; }

        /** 获取红色成分
        @version NIIEngine 3.0.0
        */
        inline NIIf getRed() const { return r; }

        /** 设置绿色成分
        @version NIIEngine 3.0.0
        */
        inline void setGreen(NIIf v) { g = v; }

        /** 获取绿色成分
        @version NIIEngine 3.0.0
        */
        inline NIIf getGreen() const { return g; }

        /** 设置蓝色成分
        @version NIIEngine 3.0.0
        */
        inline void setBlue(NIIf v) { b = v; }

        /** 获取蓝色成分
        @version NIIEngine 3.0.0
        */
        inline NIIf getBlue() const { return b; }

        /** 设置透明成分
        @version NIIEngine 3.0.0
        */
        inline void setAlpha(NIIf value) { a = value; }

        /** 获取透明成分
        @version NIIEngine 3.0.0
        */
        inline NIIf getAlpha() const { return a; }

		/** 调制亮度
		@version NIIEngine 3.0.0
		*/
		inline void modulate(NIIf alpha) { setAlpha(a * alpha); }

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

    inline _EngineAPI Nostream & operator << (Nostream & o, const Colour & c)
    {
        o << "Colour(" << c.r << "," << c.g << "," << c.b << "," << c.a << ")";
        return o;
    }
    
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
        inline void set(const Colour & c) { mTL = mTR = mBL = mBR = c; }

        /** 获取
        @version NIIEngine 3.0.0
        */
        inline void get(Colour & c) { c = mTL;  }

        /** 赋值
        @note 会比副本对象快点点?
        @version NIIEngine 3.0.0
        */
        inline void set(const Colour & tl, const Colour & tr, const Colour & bl, const Colour & br)
        { mTL = tl;mTR = tr; mBL = bl; mBR = br; }

        /** 获取
        @note 会比副本对象快点点?
        @version NIIEngine 3.0.0
        */
        inline void get(Colour & tl, Colour & tr, Colour & bl, Colour & br)
        { tl = mTL; tr = mTR; bl = mBL; br = mBR; }

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

        /** 调制透明度
        @version NIIEngine 3.0.0
        */
		inline void modulate(NIIf a) { mTL.modulate(a); mTR.modulate(a); mBL.modulate(a); mBR.modulate(a); }

        /** 设置透明度
        @version NIIEngine 3.0.0
        */
        inline void setAlpha(NIIf a) { mTL.setAlpha(a); mTR.setAlpha(a); mBL.setAlpha(a); mBR.setAlpha(a); }

        /** 设置透明度因子
        @version NIIEngine 3.0.0
        */
        void setAlphaFactor(NIIf a);

        /** 设置透明度
        @version NIIEngine 3.0.0
        */
        inline void setLeftAlpha(NIIf a) { mTL.setAlpha(a); mBL.setAlpha(a); }

        /** 设置透明度
        @version NIIEngine 3.0.0
        */
        inline void setTopAlpha(NIIf a) { mTL.setAlpha(a); mTR.setAlpha(a); }

        /** 设置透明度
        @version NIIEngine 3.0.0
        */
        inline void setRightAlpha(NIIf a) { mTR.setAlpha(a); mBR.setAlpha(a); }

        /** 设置透明度
        @version NIIEngine 3.0.0
        */
        inline void setBottomAlpha(NIIf a) { mBL.setAlpha(a); mBR.setAlpha(a); }

        /** 是否只有一种颜色
        @version NIIEngine 3.0.0
        */
        inline bool isOneColour() const { return mTL == mTR && mTL == mBL && mTL == mBR;  }

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