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
#include "NiiPropertyKeyFrameTrack.h"
#include "NiiPropertyFusion.h"
#include "NiiPropertyKeyFrame.h"
#include "NiiFusionData.h"

namespace NII
{
    //------------------------------------------------------------------------
    PropertyKeyFrameTrack::PropertyKeyFrameTrack(const Animation * o, KeyFrameTrackID id) :
        KeyFrameTrack(o, id),
        mTarget(0),
        mMode(M_ABS)
    {
    }
    //------------------------------------------------------------------------
    PropertyKeyFrameTrack::PropertyKeyFrameTrack(const Animation * o, KeyFrameTrackID id,
        PropertyID target, Mode mode) :
        KeyFrameTrack(o, id),
        mTarget(target),
        mMode(mode)
    {
    }
    //------------------------------------------------------------------------
    void PropertyKeyFrameTrack::setMode(PropertyKeyFrameTrack::Mode mode)
    {
        mMode = mode;
    }
    //------------------------------------------------------------------------
    PropertyKeyFrameTrack::Mode PropertyKeyFrameTrack::getMode() const
    {
        return mMode;
    }
    //------------------------------------------------------------------------
    void PropertyKeyFrameTrack::setTarget(PropertyID pid)
    {
        mTarget = pid;
    }
    //------------------------------------------------------------------------
    PropertyID PropertyKeyFrameTrack::getTarget() const
    {
        return mTarget;
    }
    //------------------------------------------------------------------------
    void PropertyKeyFrameTrack::apply(AnimationFusion * dst, PropertyID target,
        TrackTime * tt, NIIf weight, NIIf scale) const
    {
        if(mKeyFrames.empty() || !weight || !scale)
            return;

        PropertyFusion * fusion = static_cast<PropertyFusion *>(dst);
        PropertyKeyFrame kf(0, tt->getTimePos());
        getFusion(dst, tt, &kf);

        FusionData val = kf.getValue() * (weight * scale);
        fusion->getTarget()->set(target, val.getBaseValue()->getDataPtr());
    }
    //------------------------------------------------------------------------
    void PropertyKeyFrameTrack::apply(AnimationFusion * dst, TrackTime * tt,
        NIIf weight, NIIf scale) const
    {
        apply(dst, mTarget, tt, weight, scale);
    }
    //------------------------------------------------------------------------
    void PropertyKeyFrameTrack::getFusion(AnimationFusion * dst, TrackTime * tt, KeyFrame * kf) const
    {
        KeyFrame *tk1, *tk2;
        NIIf t = get(tt, tk1, tk2);

        PropertyKeyFrame *out = static_cast<PropertyKeyFrame *>(kf);
        PropertyKeyFrame *k1 = static_cast<PropertyKeyFrame *>(tk1);
        PropertyKeyFrame *k2 = static_cast<PropertyKeyFrame *>(tk2);
        t = k2->getFactor(t);
        PropertyFusion * fusion = static_cast<PropertyFusion *>(dst);
        if(t == 0.0)
        {
            // Just use k1
            out->setValue(k1->getPropertyValue(fusion));
        }
        else if(mMode == M_ABS)
        {
            // Interpolate by t
            const FusionData & kd1 = k1->getPropertyValue(fusion);
            const FusionData & kd2 = k2->getPropertyValue(fusion);
            FusionData re;
            re.FusionValue(kd1, kd2, t);
            out->setValue(re);
        }
        else if(mMode == M_REL)
        {
            const FusionData & kd1 = k1->getPropertyValue(fusion);
            const FusionData & kd2 = k2->getPropertyValue(fusion);
            const FusionData & base = fusion->getBaseValue(mTarget);
            FusionData re;
            re.RelFusionValue(base, kd1, kd2, t);
            out->setValue(re);
        }
        else if(mMode == M_MulREL)
        {
            const FusionData & kd1 = k1->getPropertyValue(fusion);
            const FusionData & kd2 = k2->getPropertyValue(fusion);
            const FusionData & base = fusion->getBaseValue(mTarget);
            FusionData re;
            re.RelMulFusionValue(base, kd1, kd2, t);
            out->setValue(re);
        }
    }
    //------------------------------------------------------------------------
    void PropertyKeyFrameTrack::setValue(PropertyFusion * obj)
    {
        switch(mMode)
        {
        case M_REL:
        case M_MulREL:
            obj->setBaseValue(mTarget);
            break;
        default:
            break;
        }

        KeyFrames::const_iterator it, itend = mKeyFrames.end();
        for(it = mKeyFrames.begin(); it != itend; ++it)
        {
            static_cast<PropertyKeyFrame *>(*it)->setPropertyValue(obj);
        }
    }
    //------------------------------------------------------------------------
    AnimationFusion * PropertyKeyFrameTrack::createFusion(AnimationObj * obj, 
        Animation * ani, NIIf weight, bool enable) const
    {
        return N_new PropertyFusion(0, obj, ani, weight, enable);
    }
    //------------------------------------------------------------------------
    KeyFrameTrack * PropertyKeyFrameTrack::clone(const Animation * o) const
    {
        PropertyKeyFrameTrack * re = N_new PropertyKeyFrameTrack(o, mID,
            mTarget, mMode);
        re->copyImpl(this);
        return re;
    }
    //------------------------------------------------------------------------
    KeyFrame * PropertyKeyFrameTrack::createImpl(TimeDurMS pos)
    {
        return N_new PropertyKeyFrame(this, pos);
    }
    //------------------------------------------------------------------------
}