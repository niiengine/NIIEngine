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
#include "NiiVertexUnitFusion.h"
#include "NiiVertexKeyFrameTrack.h"
#include "NiiGeometrySpace.h"

namespace NII
{
    //------------------------------------------------------------------------
    VertexUnitFusion::VertexUnitFusion(AnimationObj * obj, Animation * ani, TimeDurMS pos, 
        TimeDurMS dur, NIIf weight, bool enable) :
            VertexFusion(obj, ani, pos, dur, weight, enable)
    {
    }
    //------------------------------------------------------------------------
    VertexUnitFusion::VertexUnitFusion(AnimationFusionObj * own, const VertexUnitFusion & o) :
        VertexFusion(own, o)
    {
    }
    //------------------------------------------------------------------------
    AnimationFusion * VertexUnitFusion::clone(AnimationFusionObj * own) const
    {
        VertexUnitFusion * re = N_new VertexUnitFusion(own, *this);
        return re;
    }
    //------------------------------------------------------------------------
    void VertexUnitFusion::apply(GeometrySpace * dst, bool sw, bool hw)
    {
        if(mAnimation)
        {
            TrackTime tt = mAnimation->getTime(mTimePos);

            Animation::TrackList::const_iterator i, iend = mAnimation->getTrack().end();
            for(i = mAnimation->getTrack().begin(); i != iend; ++i)
            {
                KeyFrameTrackID idx = i->first;
                VertexKeyFrameTrack * track = static_cast<VertexKeyFrameTrack *>(i->second);

                VertexData * swdata;
                VertexData * hwdata;
                if(idx == 0)
                {   // 公共顶点
                    swdata = dst->getSwFusionVertex();
                    hwdata = dst->getHwFusionVertex();
                    dst->setAnimation(true);
                }
                else
                {   // 子空间顶点 (-1)
                    GeometryCom * s = dst->getCom(idx - 1);
                    if(!s->isVisible())
                        continue;

                    swdata = s->getSwFusionVertex();
                    hwdata = s->getHwFusionVertex();
                    s->setAnimation(true);
                }
                // 如果需要的话,应用硬件动画和软件动画
                if(sw)
                {
                    track->setMode(KeyFrameTrack::KeyFrameTrack::M_HOST);
                    track->apply(this, swdata, &tt, mWeight);
                }
                if(hw)
                {
                    track->setMode(KeyFrameTrack::KeyFrameTrack::M_DEV);
                    track->apply(this, hwdata, &tt, mWeight);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void VertexUnitFusion::apply(VertexData * data)
    {
        if(mAnimation)
        {
            TrackTime tt = mAnimation->getTime(mTimePos);

            Animation::TrackList::const_iterator i, iend = mAnimation->getTrack().end();
            for(i = mAnimation->getTrack().begin(); i != iend; ++i)
            {
                KeyFrameTrackID idx = i->first;
                VertexKeyFrameTrack * track = static_cast<VertexKeyFrameTrack *>(i->second);

                track->setMode(KeyFrameTrack::KeyFrameTrack::M_HOST);
                track->apply(this, &data[idx], &tt, mWeight);
            }
        }
    }
    //------------------------------------------------------------------------
}