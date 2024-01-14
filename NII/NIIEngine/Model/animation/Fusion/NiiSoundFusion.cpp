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
#include "NiiSoundFusion.h"
#include "NiiAnimation.h"
#include "NiiSoundKeyFrameTrack.h"

namespace NII
{
    //------------------------------------------------------------------------
    SoundFusion::SoundFusion(AnimationObj * obj, Animation * ani, 
        TimeDurMS pos, TimeDurMS dur, NIIf scale, NIIf weight, bool enable) :
            AnimationFusion(obj, ani, pos, dur, weight, enable),
            mDst(static_cast<SpaceNode *>(obj)),
            mDirty(false)
    {
    }
    //------------------------------------------------------------------------
    SoundFusion::SoundFusion(AnimationFusionObj * own, const SoundFusion & o) :
        AnimationFusion(own, o)
    {
    }
    //------------------------------------------------------------------------
    SoundFusion::~SoundFusion()
    {
    }
    //------------------------------------------------------------------------
    void SoundFusion::apply(NIIf factor)
    {
        factor *= mWeight;
        if(mAnimation)
        {
            TrackTime tt = mAnimation->getTime(mTimePos);

            Animation::TrackList::const_iterator i, iend = mAnimation->getTrack().end();
            for(i = mAnimation->getTrack().begin(); i != iend; ++i)
            {
                static_cast<SoundKeyFrameTrack *>(i->second)->apply(this, mDst, &tt, factor, mScale);
            }
        }
    }
    //------------------------------------------------------------------------
    void SoundFusion::reset()
    {

    }
    //------------------------------------------------------------------------
    AnimationFusion * SoundFusion::clone(AnimationFusionObj * own) const
    {
        SoundFusion * re = N_new SoundFusion(own, *this);
        return re;
    }
    //------------------------------------------------------------------------
}