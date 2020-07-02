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

#ifndef _NII_RELPOS_H_
#define _NII_RELPOS_H_

#include "NiiPreInclude.h"
#include "NiiRect.h"

namespace NII
{
    /** 相对坐标中的位置
    @version NIIEngine 3.0.0
    */
    template <typename T> class RelPos
    {
    public:
        RelPos(){}

        RelPos(T scale, T oft):
            mScale(scale),
            mOffset(oft)
        {
        }

        RelPos(const RelPos & o):
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
            return val == (T)0 ? RelPos((T)0, (T)0) : RelPos( mScale / val, mOffset / val);
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
    typedef TRect<RelPosi> RelRecti;
    typedef TRect<RelPosf> RelRectf;
}
#endif