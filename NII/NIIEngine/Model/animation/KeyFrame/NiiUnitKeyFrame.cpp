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
#include "NiiUnitKeyFrame.h"

namespace NII
{
    //------------------------------------------------------------------------
    UnitKeyFrame::UnitKeyFrame(const KeyFrameTrack * o, TimeDurMS pos) :
        KeyFrame(o, pos),
        mMode(M_Constant)
    {
    }
    //------------------------------------------------------------------------
    UnitKeyFrame::UnitKeyFrame(const KeyFrameTrack * o, TimeDurMS pos, Mode mode,
        const FusionData & value):
            KeyFrame(o, pos),
            mMode(mode),
            mValue(value)
    {
    }
    //------------------------------------------------------------------------
    UnitKeyFrame::~UnitKeyFrame()
    {
    }
    //------------------------------------------------------------------------
    NIIf UnitKeyFrame::getFactor(NIIf pos)
    {
        switch(mMode)
        {
        case M_Constant:
            return pos;
        case M_Point:
            return pos < 1.0f ? 0.0f : 1.0f;
        case M_Square:
            return pos * pos;
        case M_SquareRoot:
            return sqrtf(pos);
        }
        return pos;
    }
    //------------------------------------------------------------------------
    void UnitKeyFrame::setMode(UnitKeyFrame::Mode p)
    {
        mMode = p;
    }
    //------------------------------------------------------------------------
    UnitKeyFrame::Mode UnitKeyFrame::getMode() const
    {
        return mMode;
    }
    //------------------------------------------------------------------------
    void UnitKeyFrame::setValue(const FusionData & val)
    {
        mValue = val;
    }
    //------------------------------------------------------------------------
    const FusionData & UnitKeyFrame::getValue() const
    {
        return mValue;
    }
    //------------------------------------------------------------------------
    KeyFrame * UnitKeyFrame::clone(const KeyFrameTrack * o) const
    {
        UnitKeyFrame * re = N_new UnitKeyFrame(o, mTime, mMode, mValue);
        return re;
    }
    //------------------------------------------------------------------------
}