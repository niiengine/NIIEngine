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
#include "NiiSoundKeyFrame.h"
#include "NiiKeyFrameTrack.h"

namespace NII
{
    //------------------------------------------------------------------------
    SoundKeyFrame::SoundKeyFrame(const KeyFrameTrack * own, TimeDurMS pos) :
        KeyFrame(own, pos),
        mResourceID(0),
    {
    }
    //------------------------------------------------------------------------
    SoundKeyFrame::~SoundKeyFrame()
    {
    }
    //------------------------------------------------------------------------
    SoundKeyFrame & SoundKeyFrame::operator = (const SoundKeyFrame & o)
    {
        mResourceList = o.mResourceList;
        return *this;
    }
    //------------------------------------------------------------------------
    void SoundKeyFrame::addResource(ResourceID rid)
    {
        ResourceIDList::iterator i, iend = mResourceList.end();
        for(i = mResourceList.begin(); i != iend; ++i)
        {
            if(*i == rid)
                return;
        }
        mResourceList.push_back(rid);
        if(mTrack)
            mTrack->notifyImpl();
    }
    //------------------------------------------------------------------------
    void SoundKeyFrame::removeResource(ResourceID rid)
    {
        ResourceIDList::iterator i, iend = mResourceList.end();
        for(i = mResourceList.begin(); i != iend; ++i)
        {
            if(*i == rid)
            {
                mResourceList.erase(i);
                break;
            }
        }
        if(mTrack)
            mTrack->notifyImpl();
    }
    //------------------------------------------------------------------------
    const ResourceIDList & SoundKeyFrame::getList() const
    {
        return mResourceList;
    }
    //------------------------------------------------------------------------
    void SoundKeyFrame::play(SpaceNode * node)
    {
        ResourceIDList::iterator first, last = mSoundList.end();
        for(first = mSoundList.begin(); first != last; ++first)
        {
            SoundUnit * source = N_Engine().getSoundSys()->createUnit((*first), true, NULL, NULL, node);
            if(source != NULL)
            {
                source->setRelativeGain(mRelativeGain);
                source->setPos(mPos);
                source->play();
            }
        }
    }
    //------------------------------------------------------------------------
    KeyFrame * SoundKeyFrame::clone(const KeyFrameTrack * o) const
    {
        SoundKeyFrame * re = N_new SoundKeyFrame(o, mTime);
        re->mResourceList = mResourceList;
        return re;
    }
    //------------------------------------------------------------------------
}