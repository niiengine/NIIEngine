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
#ifndef _NII_ANGLE_H_
#define _NII_ANGLE_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ½Ç¶È
    @remark [0, 360]
    @version NIIEngine 3.0.0
    */
    class Angle
    {
    public:
        explicit Angle(NIIf f);
        Angle(const Radian & r);

        inline Angle & operator = (const NIIf & f)     { mValue = f; return *this; }
        inline Angle & operator = (const Angle & a)    { mValue = a.mValue; return *this; }
        Angle & operator = (const Radian & r);

        NIIf getRadian() const;

        inline bool operator <  (const Angle & a) const { return mValue <  a.mValue; }
        inline bool operator <= (const Angle & a) const { return mValue <= a.mValue; }
        inline bool operator == (const Angle & a) const { return mValue == a.mValue; }
        inline bool operator != (const Angle & a) const { return mValue != a.mValue; }
        inline bool operator >= (const Angle & a) const { return mValue >= a.mValue; }
        inline bool operator >  (const Angle & a) const { return mValue >  a.mValue; }

        inline Angle operator + () const                { return Angle(mValue); }
        inline Angle operator - () const                { return Angle(-mValue); }

        inline Angle operator + (const Angle & a) const { return Angle (mValue + a.mValue); }
        Angle operator + (const Radian * r) const;
        inline Angle operator - (const Angle & a) const { return Angle(mValue - a.mValue); }
        Angle operator - (const Radian * r) const;

        _EngineAPI friend Angle operator * (NIIf a, const Angle & b){ return Angle (a * b.mValue); }
        _EngineAPI friend Angle operator / (NIIf a, const Angle & b){ return Angle (a / b.mValue); }

        inline Angle operator * (NIIf f) const          { return Angle(mValue * f); }
        inline Angle operator / (NIIf f) const          { return Angle(mValue / f); }

        inline Angle & operator += (const Angle & a)    { mValue += a.mValue; return *this; }
        Angle & operator += (const Radian * r);
        inline Angle & operator -= (const Angle & a)    { mValue -= a.mValue; return *this; }
        Angle & operator -= (const Radian * r);
        inline Angle & operator *= (NIIf f)             { mValue *= f; return *this; }
        inline Angle & operator /= (NIIf f)             { mValue /= f; return *this; }

        inline Angle operator * (const Angle & a) const{ return Angle(mValue * a.mValue); }
        inline Angle operator / (const Angle & a) const{ return Angle(mValue / a.mValue); }

        inline NIIf getValue() const { return mValue; }
    public:
        NIIf mValue;
    };

    inline _EngineAPI Nostream & operator << (Nostream & o, const Angle & v)
    {
        o << "Angle(" << v.getValue() << ")";
        return o;
    }
}

#endif