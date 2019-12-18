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
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_RECT_H_
#define _NII_RECT_H_

#include "NiiPreInclude.h"
#include "NiiPlaneSize.h"

namespace NII
{
    /** 矩形组合
    @version NIIEngine 3.0.0
    */
    template <typename T> class TRect
    {
    public:
        TRect(){}

        inline TRect(const T & t) :
            mLeft(t),
            mTop(t),
            mRight(t),
            mBottom(t){}

        inline TRect(const TRect<T> & o):
            mLeft(o.mLeft),
            mTop(o.mTop),
            mRight(o.mRight),
            mBottom(o.mBottom){}

        inline TRect(const T & l, const T & t, const T & r, const T & b) :
            mLeft(l),
            mTop(t),
            mRight(r),
            mBottom(b){}

        inline TRect(const Vector2<T> & min, const Vector2<T> & max) :
            mLeft(min.x),
            mTop(min.y),
            mRight(max.x),
            mBottom(max.y){}

        inline TRect(const Vector2<T> & pos, const PlaneSize<T> & size) :
            mLeft(pos.x),
            mTop(pos.y)
        {
            mRight = pos.x + size.mWidth;
            mBottom = pos.y + size.mHeight;
        }

        TRect<T> & operator= (const TRect<T> & o)
        {
            mLeft = o.mLeft;
            mTop = o.mTop;
            mRight = o.mRight;
            mBottom = o.mBottom;
            return *this;
        }

        Vector2<T> min() const
        {
            return Vector2<T>(mLeft, mTop);
        }

        Vector2<T> max() const
        {
            return Vector2<T>(mRight, mBottom);
        }

        inline bool operator== (const TRect<T> & o) const
        {
            return (mLeft == o.mLeft) && (mTop == o.mTop) &&
                (mRight == o.mRight) && (mBottom == o.mBottom);
        }

        inline bool operator!= (const TRect<T> & o) const
        {
            return (mLeft != o.mLeft) || (mTop != o.mTop) ||
                (mRight != o.mRight) || (mBottom != o.mBottom);
        }

        inline TRect<T> operator+ (NIIf factor) const
        {
            return TRect<T>(mLeft + factor, mTop + factor, mRight + factor, mBottom + factor);
        }

        inline TRect<T> operator- (NIIf factor) const
        {
            return TRect<T>(mLeft - factor, mTop - factor, mRight - factor, mBottom - factor);
        }

        inline TRect<T> operator/ (NIIf factor) const
        {
            return TRect<T>(mLeft / factor, mTop / factor, mRight / factor, mBottom / factor);
        }

        inline TRect<T> operator* (const T & factor) const
        {
            return TRect<T>(mLeft * factor, mTop * factor, mRight * factor, mBottom * factor);
        }

        const TRect<T> & operator*=(const T & factor)
        {
            mLeft *= factor;
            mTop *= factor;
            mRight *= factor;
            mBottom *= factor;
            return *this;
        }

        TRect<T> operator*(const TRect<T> & o) const
        {
            return TRect<T>(mLeft * o.mLeft, mTop * o.mTop, mRight * o.mRight, mBottom * o.mBottom);
        }

        TRect<T> operator/(const TRect<T> & o) const
        {
            return TRect<T>(mLeft, mTop, mRight, mBottom);
        }

        TRect<T> operator-(const TRect<T> & o) const
        {
            return TRect<T>(mLeft - o.mLeft, mTop - o.mTop, mRight - o.mRight, mBottom - o.mBottom);
        }

        TRect<T> operator+(const TRect<T> & o) const
        {
            return TRect<T>(mLeft + o.mLeft, mTop + o.mTop, mRight + o.mRight, mBottom + o.mBottom);
        }

        TRect<T> operator+=(const TRect<T> & o)
        {
            mLeft += o.mLeft;
            mTop += o.mTop;
            mRight += o.mRight;
            mBottom += o.mBottom;
            return *this;
        }

        TRect<T> operator+(const Vector2<T> & o) const
        {
            return TRect<T>(mLeft + o.x, mTop + o.y, mRight + o.x, mBottom + o.y);
        }

        TRect<T> operator-(const Vector2<T> & o) const
        {
            return TRect<T>(mLeft - o.x, mTop - o.y, mRight - o.x, mBottom - o.y);
        }

        TRect<T> operator+=(const Vector2<T> & o)
        {
            mLeft += o.x;
            mTop += o.y;
            mRight += o.x;
            mBottom += o.y;
            return *this;
        }

        TRect<T> operator-=(const Vector2<T> & o)
        {
            mLeft -= o.x;
            mTop -= o.y;
            mRight -= o.x;
            mBottom -= o.y;
            return *this;
        }

        inline void setNull()
        {
            mLeft = mRight = mTop = mBottom = 0;
        }

        inline bool isNull() const
        {
            return (mRight - mLeft == 0) || (mBottom - mTop == 0);
        }

        inline void setLeft(const T & v)
        {
            mLeft = v;
        }

        inline const T & left() const
        {
            return mLeft;
        }

        inline void setTop(const T & v)
        {
            mTop = v;
        }

        inline const T & top() const
        {
            return mTop;
        }

        inline void setRight(const T & v)
        {
            mRight = v;
        }

        inline const T & right() const
        {
            return mRight;
        }

        inline void setBottom(const T & v)
        {
            mBottom = v;
        }

        inline const T & bottom() const
        {
            return mBottom;
        }

        inline void setWidth(const T & w)
        {
            mRight = mLeft + w;
        }

		inline T getWidth() const
		{
			return mRight - mLeft;
		}

        inline T width() const
        {
            return mRight - mLeft;
        }

        inline void setHeight(const T & h)
        {
            mBottom = mTop + h;
        }

		inline T getHeight() const
		{
			return mBottom - mTop;
		}

        inline T height() const
        {
            return mBottom - mTop;
        }

        inline TRect<T> & offset(const Vector2<NIIf> & o)
        {
            mLeft += o.x;
            mTop += o.y;
            mRight += o.x;
            mBottom += o.y;

            return *this;
        }

        void setPos(const Vector2<T> & pos)
        {
            T width = mRight - mLeft;
            T height = mBottom - mTop;
            mLeft = pos.x;
            mTop = pos.y;
            mRight = mLeft + width;
            mBottom = mTop + height;
        }

		Vector2<T> getPos() const
        {
            return Vector2<T>(mLeft, mTop);
        }

        void setSize(const PlaneSize<T> & size)
        {
            mRight = mLeft + size.mWidth;
            mBottom = mTop + size.mHeight;
        }

        inline PlaneSize<T> getSize() const
        {
            return PlaneSize<T>(mRight - mLeft, mBottom - mTop);
        }

        /** 相交
        @version NIIEngine 3.0.0
        */
        inline bool intersects(T x, T y) const
        {
            return x >= mLeft && x <= mRight && y >= mTop && y <= mBottom;
        }

        /** 交集
        @version NIIEngine 3.0.0
        */
        TRect<T> intersects(const TRect<T> & o) const
        {
            TRect<T> re;
            if(isNull() || o.isNull())
            {
                return re;
            }
            else
            {
                re.mLeft   = mLeft   > o.mLeft   ? mLeft   : o.mLeft;
                re.mTop    = mTop    > o.mTop    ? mTop    : o.mTop;
                re.mRight  = mRight  < o.mRight  ? mRight  : o.mRight;
                re.mBottom = mBottom < o.mBottom ? mBottom : o.mBottom;
            }

            if(re.mLeft > re.mRight || re.mTop > re.mBottom)
            {
                re.mLeft = re.mTop = re.mRight = re.mBottom = 0;
            }

            return re;
        }

        /** 并集
        @version NIIEngine 3.0.0
        */
        TRect<T> collection(const TRect<T> & o)
        {
            TRect<T> re;
            if(isNull() || o.isNull())
            {
                return re;
            }
            else
            {
                re.mLeft   = mLeft   > o.mLeft   ? o.mLeft   : mLeft;
                re.mTop    = mTop    > o.mTop    ? o.mTop    : mTop;
                re.mRight  = mRight  < o.mRight  ? o.mRight  : mRight;
                re.mBottom = mBottom < o.mBottom ? o.mBottom : mBottom;
            }
            return re;
        }

        static const TRect<T> ZERO;
    public:
        T mLeft;
        T mTop;
        T mRight;
        T mBottom;
    };
    template <typename T>const TRect<T> TRect<T>::ZERO = TRect<T>(0, 0, 0, 0);

    template<typename T> std::ostream & operator << (std::ostream & o, const TRect<T> & r)
    {
        o << "Rect(" << r.mLeft << "," << r.mTop << "," << r.mRight << "," << r.mBottom << ")";
        return o;
    }
    typedef TRect<NIIf> Rectf;
    typedef TRect<NIIl> Rect;
}
#endif