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
#include "NiiPropertyFusion.h"
#include "NiiPropertyKeyFrameTrack.h"
#include "NiiFusionData.h"

namespace NII
{
    //------------------------------------------------------------------------
    PropertyFusion::PropertyFusion(PropertyCmdObj * dst, AnimationObj * obj, Animation * ani,
        NIIf weight, bool enable) :
            AnimationFusion(obj, ani, 0, ani->getDur(), weight, enable),
            mDst(dst)
    {
    }
    //------------------------------------------------------------------------
    PropertyFusion::PropertyFusion(PropertyCmdObj * dst, AnimationObj * obj, Animation * ani,
        TimeDurMS pos, TimeDurMS dur, NIIf weight, bool enable):
            AnimationFusion(obj, ani, pos, dur, weight, enable),
            mDst(dst)
    {
    }
    //------------------------------------------------------------------------
    PropertyFusion::PropertyFusion(AnimationFusionObj * own, const PropertyFusion & o):
        AnimationFusion(own, o),
        mDst(o.mDst)
    {
    }
    //------------------------------------------------------------------------
    PropertyFusion::~PropertyFusion()
    {
        mBaseValue.clear();
    }
    //------------------------------------------------------------------------
    void PropertyFusion::apply(NIIf factor)
    {
        if(mDst)
        {
            TrackTime tt = mAnimation->getTime(mTimePos);

            Animation::TrackList::const_iterator i, iend = mAnimation->getTrack().end();
            for(i = mAnimation->getTrack().begin(); i != iend; ++i)
            {
                static_cast<PropertyKeyFrameTrack *>(i->second)->apply(this, &tt);
            }
        }
    }
    //------------------------------------------------------------------------
    void PropertyFusion::setBaseValue(PropertyID pid)
    {
        N_assert(mDst, "error");
        PropertyValueMap::iterator it = mBaseValue.find(pid);
        if(it == mBaseValue.end())
        {
            FusionNum * temp = mDst->createNum(pid);
            it = mBaseValue.insert(Npair(pid, FusionData(temp))).first;
 
        }
        mDst->get(pid, const_cast<void *>(it->second.getBaseValue()->getDataPtr()));
    }
    //------------------------------------------------------------------------
    const FusionData & PropertyFusion::getBaseValue(PropertyID pid)
    {
        PropertyValueMap::iterator it = mBaseValue.find(pid);
        if(it == mBaseValue.end())
        {
            FusionNum * temp = mDst->createNum(pid);
            it = mBaseValue.insert(Npair(pid, FusionData(temp))).first;

            mDst->get(pid, const_cast<void *>(it->second.getBaseValue()->getDataPtr()));
        }
        return it->second;
    }
    //------------------------------------------------------------------------
    void PropertyFusion::setTarget(PropertyCmdObj * dst)
    {
        mDst = dst;

        mBaseValue.clear();

        if(mAutoStart && !mEnable)
        {
            start();
        }
    }
    //------------------------------------------------------------------------
    PropertyCmdObj * PropertyFusion::getTarget() const
    {
        return mDst;
    }
    //------------------------------------------------------------------------
    AnimationFusion * PropertyFusion::clone(AnimationFusionObj * own) const
    {
        PropertyFusion * re = N_new PropertyFusion(own, *this);
        return re;
    }
    //------------------------------------------------------------------------
    void PropertyFusion::onStart()
    {
        mBaseValue.clear();

        Animation::TrackList::const_iterator i, iend = mAnimation->getTrack().end();
        for(i = mAnimation->getTrack().begin(); i != iend; ++i)
        {
            static_cast<PropertyKeyFrameTrack *>(i->second)->setValue(this);
        }

        AnimationFusion::onStart();
    }
    //------------------------------------------------------------------------
}