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
#ifndef _NII_RADIAN_H_
#define _NII_RADIAN_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** »¡¶È
    @remark [0, 2pai]
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Radian
    {
    public:
        Radian() {}
        explicit Radian(NIIf f);
        Radian(const Angle & a);

        inline Radian & operator = (const NIIf & f)    { mValue = f; return *this; }
        inline Radian & operator = (const Radian & r)  { mValue = r.mValue; return *this; }
        inline Radian & operator = (const Angle & a);

        NIIf getAngle() const;

        inline bool operator <  (const Radian & r) const { return mValue <  r.mValue; }
        inline bool operator <= (const Radian & r) const { return mValue <= r.mValue; }
        inline bool operator == (const Radian & r) const { return mValue == r.mValue; }
        inline bool operator != (const Radian & r) const { return mValue != r.mValue; }
        inline bool operator >= (const Radian & r) const { return mValue >= r.mValue; }
        inline bool operator >  (const Radian & r) const { return mValue >  r.mValue; }

        inline Radian operator + () const                { return Radian(mValue); }
        inline Radian operator - () const                { return Radian(-mValue); }

        inline Radian operator + (const Radian & r) const{ return Radian(mValue + r.mValue); }
        inline Radian operator - (const Radian & r) const{ return Radian(mValue - r.mValue); }

        Radian operator + (const Angle * a) const;
        Radian operator - (const Angle * a) const;

        inline Radian operator * (NIIf f) const          { return Radian(mValue * f); }
        inline Radian operator / (NIIf f) const          { return Radian (mValue / f); }
        inline Radian & operator *= (NIIf f)             { mValue *= f; return *this; }
        inline Radian & operator /= (NIIf f)             { mValue /= f; return *this; }

        _EngineAPI friend Radian operator * (NIIf f, const Radian & r){ return Radian(f * r.mValue);}
        _EngineAPI friend Radian operator / (NIIf f, const Radian & r){ return Radian(f / r.mValue); }

        inline Radian operator * (const Radian & r) const{ return Radian(mValue * r.mValue); }
        inline Radian operator / (const Radian & r) const{ return Radian(mValue / r.mValue); }

        inline Radian & operator +=(const Radian & r)    { mValue += r.mValue; return *this; }
        inline Radian & operator -= (const Radian & r)   { mValue -= r.mValue; return *this; }
        Radian & operator += (const Angle * a);
        Radian & operator -= (const Angle * a);

        inline NIIf getValue() const { return mValue; }
    public:
        NIIf mValue;
    };

    inline _EngineAPI Nostream & operator << (Nostream & o, const Radian & v)
    {
        o << "Radian(" << v.getValue() << ")";
        return o;
    }
}
#endif