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
#include "NiiUnitKeyFrameTrack.h"
#include "NiiUnitKeyFrame.h"
#include "NiiUnitFusion.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // UnitKeyFrameTrack
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    UnitKeyFrameTrack::UnitKeyFrameTrack(const Animation * parent, KeyFrameTrackID id) :
        KeyFrameTrack(parent, id),
        mTarget(0)
    {
    }
    //------------------------------------------------------------------------
    UnitKeyFrameTrack::UnitKeyFrameTrack(const Animation * parent, KeyFrameTrackID id,
        FusionData * target) :
            KeyFrameTrack(parent, id),
            mTarget(target)
    {
    }
    //------------------------------------------------------------------------
    void UnitKeyFrameTrack::setTarget(const FusionData * dst)
    {
        mTarget = const_cast<FusionData *>(dst);
    }
    //------------------------------------------------------------------------
    const FusionData * UnitKeyFrameTrack::getTarget() const
    {
        return mTarget;
    }
    //------------------------------------------------------------------------
    void UnitKeyFrameTrack::getFusion(AnimationFusion * dst, TrackTime * tt, KeyFrame * kf) const
    {
        KeyFrame *tk1, *tk2;
        NIIf t = get(tt, tk1, tk2);
        UnitKeyFrame * out = static_cast<UnitKeyFrame *>(kf);
        UnitKeyFrame *k1 = static_cast<UnitKeyFrame *>(tk1);
        UnitKeyFrame *k2 = static_cast<UnitKeyFrame *>(tk2);
        if(t == 0.0)
        {
            // Just use k1
            out->setValue(k1->getValue());
        }
        else
        {
            // Interpolate by t
            FusionData diff = k2->getValue() - k1->getValue();
            out->setValue(k1->getValue() + diff * t);
        }
    }
    //------------------------------------------------------------------------
    void UnitKeyFrameTrack::apply(AnimationFusion * dst, TrackTime * tt,
        NIIf weight, NIIf scale) const
    {
        apply(dst, mTarget, tt, weight, scale);
    }
    //------------------------------------------------------------------------
    void UnitKeyFrameTrack::apply(AnimationFusion * obj, FusionData * dst,
        TrackTime * tt, NIIf weight, NIIf scale) const
    {
        if(mKeyFrames.empty() || !weight || !scale)
            return;

        UnitKeyFrame kf(0, tt->getTimePos());
        getFusion(obj, tt, &kf);

        FusionData val = kf.getValue() * (weight * scale);
        dst->setValue(*val.getBaseValue());
    }
    //------------------------------------------------------------------------
    AnimationFusion * UnitKeyFrameTrack::createFusion(AnimationObj * obj, 
        Animation * ani, NIIf weight, bool enable) const
    {
        return N_new UnitFusion(0,  obj, ani, 0, ani->getDur(), weight, enable);
    }
    //------------------------------------------------------------------------
    KeyFrameTrack * UnitKeyFrameTrack::clone(const Animation * o) const
    {
        UnitKeyFrameTrack * re = N_new UnitKeyFrameTrack(o, mID, mTarget);
        re->copyImpl(this);
        return re;
    }
    //------------------------------------------------------------------------
    KeyFrame * UnitKeyFrameTrack::createImpl(TimeDurMS time)
    {
        return N_new UnitKeyFrame(this, time);
    }
    //------------------------------------------------------------------------
}