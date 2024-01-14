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

#include "NiiPreProcess.h"
#include "NiiRadian.h"
#include "NiiMath.h"

namespace NII
{
    const NIIf Rad_Ang = 180.0f / Math::PI;
    //------------------------------------------------------------------------
    Radian::Radian(NIIf f) : 
        mValue(f)    
    {
    }
    //------------------------------------------------------------------------
    Radian::Radian(const Angle & a):
        mValue(a.getRadian())
    {
    }
    //------------------------------------------------------------------------
    Radian & Radian::operator = (const Angle & a)
    { 
        mValue = a.getRadian();
        return *this; 
    }
    //------------------------------------------------------------------------
    NIIf Radian::getAngle() const   
    { 
        return mValue * Rad_Ang;
    }
    //------------------------------------------------------------------------
    Radian Radian::operator + (const Angle * a) const
    { 
        return Radian(mValue + a->getRadian()); 
    }
    //------------------------------------------------------------------------
    Radian Radian::operator - (const Angle * a) const 
    { 
        return Radian(mValue - a->getRadian()); 
    }
    //------------------------------------------------------------------------
    Radian & Radian::operator += (const Angle * a) 
    { 
        mValue += a->getRadian(); 
        return *this; 
    }
    //------------------------------------------------------------------------
    Radian & Radian::operator -= (const Angle * a) 
    { 
        mValue -= a->getRadian(); 
        return *this; 
    }
    //------------------------------------------------------------------------
}