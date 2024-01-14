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
#include "NiiUnitFusion.h"
#include "NiiFusionData.h"
#include "NiiUnitKeyFrameTrack.h"

namespace NII
{
    //------------------------------------------------------------------------
    UnitFusion::UnitFusion(FusionData * dst, AnimationObj * obj, Animation * ani, 
        NIIf weight, bool enable) :
            AnimationFusion(obj, ani, 0, ani->getDur(), weight, enable),
            mDst(dst)
    {
    }
    //------------------------------------------------------------------------
    UnitFusion::UnitFusion(FusionData * dst, AnimationObj * obj,
        Animation * ani, TimeDurMS pos, TimeDurMS dur, NIIf weight, bool enable) :
            AnimationFusion(obj, ani, pos, dur, weight, enable),
            mDst(dst)
    {
    }
    //------------------------------------------------------------------------
    UnitFusion::UnitFusion(AnimationFusionObj * own, const UnitFusion & o) :
        AnimationFusion(own, o)
    {
        mDst = N_new FusionData(*o.mDst);
    }
    //------------------------------------------------------------------------
    UnitFusion::~UnitFusion()
    {
        if(mDst)
        {
            N_delete mDst;
            mDst = 0;
        }
    }
    //------------------------------------------------------------------------
    void UnitFusion::reset()
    {
        if(mDst != 0)
            mDst->setBaseAsCurrent();
    }
    //------------------------------------------------------------------------
    AnimationFusion * UnitFusion::clone(AnimationFusionObj * own) const
    {
        UnitFusion * re = N_new UnitFusion(own, *this);
        return re;
    }
    //------------------------------------------------------------------------
    void UnitFusion::apply(NIIf scale)
    {
        // Calculate time index for fast keyframe search
        TrackTime tt = mAnimation->getTime(mTimePos);

        Animation::TrackList::const_iterator j, jend = mAnimation->getTrack().end();
        for(j = mAnimation->getTrack().begin(); j != jend; ++j)
        {
            static_cast<UnitKeyFrameTrack *>(j->second)->apply(this, mDst, &tt, mWeight, scale);
        }
    }
    //------------------------------------------------------------------------
}