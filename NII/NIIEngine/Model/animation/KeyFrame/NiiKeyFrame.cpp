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
#include "NiiKeyFrame.h"
#include "NiiKeyFrameTrack.h"

namespace NII
{
    //---------------------------------------------------------------------
    KeyFrame::KeyFrame(const KeyFrameTrack * own, TimeDurMS time) :
        mTime(time),
        mTrack(const_cast<KeyFrameTrack *>(own))
    {

    }
    //---------------------------------------------------------------------
    KeyFrame::~KeyFrame()
    {
    }
    //---------------------------------------------------------------------
    void KeyFrame::setTime(TimeDurMS time)
    {
        mTime = time;
        mTrack->modify(this, time);
    }
    //---------------------------------------------------------------------
    TimeDurMS KeyFrame::getTime() const
    {
        return mTime;
    }
    //---------------------------------------------------------------------
    KeyFrame * KeyFrame::clone(const KeyFrameTrack * o) const
    {
        return N_new KeyFrame(o, mTime);
    }
    //---------------------------------------------------------------------
}