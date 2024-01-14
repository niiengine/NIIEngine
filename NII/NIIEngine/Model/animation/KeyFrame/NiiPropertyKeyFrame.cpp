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
#include "NiiPropertyKeyFrame.h"
#include "NiiPropertyFusion.h"

namespace NII
{
    //------------------------------------------------------------------------
    PropertyKeyFrame::PropertyKeyFrame(const KeyFrameTrack * o, TimeDurMS pos):
        UnitKeyFrame(o, pos),
        mProperty(0)
    {
    }
    //------------------------------------------------------------------------
    PropertyKeyFrame::PropertyKeyFrame(const KeyFrameTrack * o, TimeDurMS pos,
        Mode mode, const FusionData & value, PropertyID pid):
            UnitKeyFrame(o, pos, mode, value),
            mProperty(pid)
    {
    }
    //------------------------------------------------------------------------
    PropertyKeyFrame::~PropertyKeyFrame()
    {
    }
    //------------------------------------------------------------------------
    void PropertyKeyFrame::setPropertyValue(PropertyFusion * obj)
    {
        if(mProperty != 0)
        {
            obj->setBaseValue(mProperty);
        }
    }
    //------------------------------------------------------------------------
    const FusionData & PropertyKeyFrame::getPropertyValue(PropertyFusion * obj) const
    {
        if(mProperty == 0)
        {
            return mValue;
        }
        else
        {
            return obj->getBaseValue(mProperty);
        }
    }
    //------------------------------------------------------------------------
    void PropertyKeyFrame::setProperty(PropertyID pid)
    {
        mProperty = pid;
    }
    //------------------------------------------------------------------------
    PropertyID PropertyKeyFrame::getProperty() const
    {
        return mProperty;
    }
    //------------------------------------------------------------------------
    KeyFrame * PropertyKeyFrame::clone(const KeyFrameTrack * o) const
    {
        PropertyKeyFrame * re = N_new PropertyKeyFrame(o, mTime, mMode,
            mValue, mProperty);
        return re;
    }
    //------------------------------------------------------------------------
}