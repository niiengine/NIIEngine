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
#ifndef _NII_PLANE_SIZE_H_
#define _NII_PLANE_SIZE_H_

#include "NiiPreInclude.h"
#include "NiiCommon.h"
#include "NiiException.h"
#include "NiiVector2.h"

namespace NII
{
    /** 描述在平面中存在的大小
    @version NIIEngine 3.0.0
    */
    template<typename T> class PlaneSize
    {
    public:
        PlaneSize(){}

        PlaneSize(const T & w, const T & h) : mWidth(w), mHeight(h){}

        PlaneSize(const PlaneSize<T> & o) : mWidth(o.mWidth), mHeight(o.mHeight){}

        inline bool operator==(const PlaneSize<T> & o) const
        {
            return mWidth == o.mWidth && mHeight == o.mHeight;
        }

        inline bool operator!=(const PlaneSize<T> & o) const
        {
            return !operator==(o);
        }

        inline PlaneSize<T> operator+(const T & c) const
        {
            return PlaneSize<T>(mWidth + c, mHeight + c);
        }

        inline PlaneSize<T> operator-(const T & c) const
        {
            return PlaneSize<T>(mWidth - c, mHeight - c);
        }

        inline PlaneSize<T> operator*(const T & c) const
        {
            return PlaneSize<T>(mWidth * c, mHeight * c);
        }

        inline PlaneSize<T> operator/(const T & c) const
        {
            return PlaneSize<T>(mWidth / c, mHeight / c);
        }

        inline PlaneSize<T> operator+(const Vector2<T> & c) const
        {
            return PlaneSize<T>(mWidth + c.x, mHeight + c.y);
        }

        inline PlaneSize<T> operator-(const Vector2<T> & c) const
        {
            return PlaneSize<T>(mWidth - c.x, mHeight - c.y);
        }

        inline PlaneSize<T> operator*(const Vector2<T> & c) const
        {
            return PlaneSize<T>(mWidth * c.x, mHeight * c.y);
        }

        inline PlaneSize<T> operator/(const Vector2<T> & c) const
        {
            return PlaneSize<T>(c.x ? mWidth / c.x : 0, c.y ? mHeight / c.y : 0);
        }

        inline PlaneSize<T> operator+(const PlaneSize<T> & s) const
        {
            return PlaneSize<T>(mWidth + s.mWidth, mHeight + s.mHeight);
        }

        inline PlaneSize<T> operator-(const PlaneSize<T> & s) const
        {
            return PlaneSize<T>(mWidth - s.mWidth, mHeight - s.mHeight);
        }

        inline PlaneSize<T> operator*(const PlaneSize<T> & s) const
        {
            return PlaneSize<T>(mWidth * s.mWidth, mHeight * s.mHeight);
        }

        inline PlaneSize<T> operator/(const PlaneSize<T> & o) const
        {
            return PlaneSize<T>(o.mWidth == 0 ? mWidth : mWidth / o.mWidth,
                o.mHeight == 0 ? mHeight : mHeight / o.mHeight);
        }

        friend inline PlaneSize<T> operator*(const T & c, const PlaneSize<T> & o)
        {
            return PlaneSize<T>(c * o.mWidth, c * o.mHeight);
        }

        friend inline PlaneSize<T> operator/(const T & c, const PlaneSize<T> & o)
        {
            return PlaneSize<T>(o.mWidth == 0 ? c : c / o.mWidth,
                o.mHeight == 0 ? c : c / o.mHeight);
        }

        inline void clamp(const PlaneSize<T> & min, const PlaneSize<T> & max)
        {
            N_assert(min.mWidth <= max.mWidth, "error");
            N_assert(min.mHeight <= max.mHeight, "error");

            if(mWidth < min.mWidth)
                mWidth = min.mWidth;
            else if(mWidth > max.mWidth)
                mWidth = max.mWidth;

            if(mHeight < min.mHeight)
                mHeight = min.mHeight;
            else if(mHeight > max.mHeight)
                mHeight = max.mHeight;
        }

        void scale(AspectMode mode, NIIf ratio)
        {
            N_assert(ratio > 0, "error");

            if(mode == AM_None || (mWidth <= 0 && mHeight <= 0))
                return;

            T factor;
            switch(mode)
            {
            case AM_Shrink:
            {
                factor = mHeight * ratio;
                if (factor <= mWidth)
                {
                    mWidth = factor;
                }
                else
                {
                    mHeight = mWidth / ratio;
                }
                break;
            }
            case AM_Stretch:
            {
                factor = mWidth * ratio;
                if (factor <= mHeight)
                {
                    mHeight = factor;
                }
                else
                {
                    mWidth = mHeight / ratio;
                }
                break;
            }
            default:
            {
                factor = mHeight * ratio;
                if (factor >= mWidth)
                {
                    mWidth = factor;
                }
                else
                {
                    mHeight = mWidth / ratio;
                }
                break;
            }
            }
        }
    public:
        T mWidth;
        T mHeight;
    };
    
    template<typename T> Nostream & operator << (Nostream & st, const PlaneSize<T> & s)
    {
        st << "PlaneSize(" << s.mWidth << "," << s.mHeight << ")";
        return st;
    }

    typedef PlaneSize<NIIf> PlaneSizef;
    typedef PlaneSize<Nui32> PlaneSizei;

    /** 相对坐标中的位置
    @version NIIEngine 3.0.0
    */
    template <typename T> class RelPos
    {
    public:
        RelPos() {}

        RelPos(T scale, T oft) :
            mScale(scale),
            mOffset(oft)
        {
        }

        RelPos(const RelPos & o) :
            mScale(o.mScale),
            mOffset(o.mOffset)
        {
        }

        inline bool operator ==(T num) const
        {
            return mScale == num && mOffset == num;
        }

        inline bool operator ==(const RelPos & o) const
        {
            return mScale == o.mScale && mOffset == o.mOffset;
        }

        inline bool operator !=(const RelPos & o) const
        {
            return mScale != o.mScale || mOffset != o.mOffset;
        }

        inline RelPos operator +(const RelPos & o) const
        {
            return RelPos(mScale + o.mScale, mOffset + o.mOffset);
        }

        inline RelPos operator -(const RelPos & o) const
        {
            return RelPos(mScale - o.mScale, mOffset - o.mOffset);
        }

        inline RelPos operator *(const RelPos & o) const
        {
            return RelPos(mScale * o.mScale, mOffset * o.mOffset);
        }

        inline RelPos operator /(const RelPos & o) const
        {
            return RelPos(o.mScale == (T)0 ? (T)0 : mScale / o.mScale,
                o.mOffset == (T)0 ? (T)0 : mOffset / o.mOffset);
        }

        inline const RelPos & operator +=(const RelPos & o)
        {
            mScale += o.mScale;
            mOffset += o.mOffset;
            return *this;
        }

        inline const RelPos & operator -=(const RelPos & o)
        {
            mScale -= o.mScale;
            mOffset -= o.mOffset;
            return *this;
        }

        inline const RelPos & operator *=(const RelPos & o)
        {
            mScale *= o.mScale;
            mOffset *= o.mOffset;
            return *this;
        }

        inline const RelPos & operator /=(const RelPos & o)
        {
            mScale = (o.mScale == (T)0 ? (T)0 : mScale / o.mScale);
            mOffset = (o.mOffset == (T)0 ? (T)0 : mOffset / o.mOffset);
            return *this;
        }

        inline RelPos operator +(T val) const
        {
            return RelPos(mScale, mOffset + val);
        }

        friend inline RelPos operator +(T val, const RelPos & o)
        {
            return RelPos(o.mScale, val + o.mOffset);
        }

        inline RelPos operator -(T val) const
        {
            return RelPos(mScale, mOffset - val);
        }

        friend inline RelPos operator -(T val, const RelPos & o)
        {
            return RelPos(o.mScale, val - o.mOffset);
        }

        inline RelPos operator *(T val) const
        {
            return RelPos(mScale * val, mOffset * val);
        }

        friend inline RelPos operator *(T val, const RelPos & o)
        {
            return RelPos(val * o.mScale, val * o.mOffset);
        }

        inline RelPos operator /(T val) const
        {
            return val == (T)0 ? RelPos((T)0, (T)0) : RelPos(mScale / val, mOffset / val);
        }

        friend inline RelPos operator /(T val, const RelPos & u)
        {
            return RelPos(u.mScale == (T)0 ? (T)0 : val / u.mScale, u.mOffset == (T)0 ? (T)0 : val / u.mOffset);
        }

        inline T abs(T base, bool pixel = true) const
        {
            return pixel ? N_ToPixel(base * mScale + mOffset) : base * mScale + mOffset;
        }
    public:
        T mScale;
        T mOffset;
    };
    typedef RelPos<Nui32> RelPosi;
    typedef RelPos<NIIf> RelPosf;
    template <typename T> inline Nostream & operator << (Nostream & s, const RelPos<T> & v)
    {
        s << "RelPos(" << v.mScale << "," << v.mOffset << ")";
        return s;
    }
}
#endif