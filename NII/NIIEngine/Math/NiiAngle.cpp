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
#include "NiiAngle.h"
#include "NiiRadian.h"
#include "NiiMath.h"

namespace NII
{
    const NIIf Ang_Rad = Math::PI / 180.0f;
    //------------------------------------------------------------------------
    Angle::Angle(NIIf d = 0) :
        mValue(d)
    {
    }
    //------------------------------------------------------------------------
    Angle::Angle(const Radian & r) :
        mValue(r.getAngle())
    {
    }
    //------------------------------------------------------------------------
    NIIf Angle::getRadian() const
    {
        return mValue * Ang_Rad;
    }
    //------------------------------------------------------------------------
    Angle & Angle::operator = (const Radian & r)
    { 
        mValue = r.getAngle(); 
        return *this; 
    }
    //------------------------------------------------------------------------
    Angle Angle::operator + (const Radian * r) const
    { 
        return Angle(mValue + r->getAngle()); 
    }
    //------------------------------------------------------------------------
    Angle Angle::operator - (const Radian * r) const
    { 
        return Angle(mValue - r->getAngle()); 
    }
    //------------------------------------------------------------------------
    Angle & Angle::operator += (const Radian * r)
    { 
        mValue += r->getAngle();
        return *this; 
    }
    //------------------------------------------------------------------------
    Angle & Angle::operator -= (const Radian * r)
    { 
        mValue -= r->getAngle();
        return *this; 
    }
    //------------------------------------------------------------------------
}