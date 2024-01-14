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
#include "NiiTrackTime.h"

namespace NII
{
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // TrackTime
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    TrackTime::TrackTime(TimeDurMS pos) :
        mTimePos(pos),
        mKeyIndex((Nidx)-1),
        mLocalKeyIndex(0)
    {
    }
    //---------------------------------------------------------------------
    TrackTime::TrackTime(TimeDurMS pos, Nidx index, Nidx lindex) :
        mTimePos(pos),
        mKeyIndex(index),
        mLocalKeyIndex(lindex)
    {
    }
    //---------------------------------------------------------------------
    bool TrackTime::isValid() const
    {
        return mKeyIndex != (Nidx)-1;
    }
    //---------------------------------------------------------------------
    TimeDurMS TrackTime::getTimePos() const
    {
        return mTimePos;
    }
    //---------------------------------------------------------------------
    Nidx TrackTime::getKeyIndex() const
    {
        return mKeyIndex;
    }
    //---------------------------------------------------------------------
    void TrackTime::setLocalKeyIndex(Nidx idx)
    {
        mLocalKeyIndex = idx;
    }
    //---------------------------------------------------------------------
    Nidx TrackTime::getLocalKeyIndex() const
    {
        return mLocalKeyIndex;
    }
    //---------------------------------------------------------------------
}