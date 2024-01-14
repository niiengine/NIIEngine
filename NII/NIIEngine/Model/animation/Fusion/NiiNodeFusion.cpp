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
#include "NiiNodeFusion.h"
#include "NiiAnimation.h"
#include "NiiSkeleton.h"
#include "NiiNodeKeyFrameTrack.h"

namespace NII
{
    //------------------------------------------------------------------------
    NodeFusion::NodeFusion(AnimationObj * obj, Animation * ani, 
        TimeDurMS pos, TimeDurMS dur, NIIf scale, NIIf weight, bool enable) :
            AnimationFusion(obj, ani, pos, dur, weight, enable),
            mMode(NodeFusion::M_LINEAR),
            mRotationMode(NodeFusion::RM_LINEAR),
            mDst(static_cast<SkeletonFusion *>(obj)),
            mScale(scale),
            mBlendMask(0)
    {
        N_assert(dynamic_cast<Skeleton *>(obj), "logic error");
    }
    //------------------------------------------------------------------------
    NodeFusion::NodeFusion(AnimationFusionObj * own, const NodeFusion & o) :
        AnimationFusion(own, o)
    {
        mScale = o.mScale;
        mMode = o.mMode;
        mRotationMode = o.mRotationMode;
        mDst = o.mDst;
        if(o.mBlendMask != 0)
        {
            NCount count = mAnimation->getCount();
            NIIf * temp = N_alloc_t(NIIf, count);
            memcpy(temp, o.mBlendMask, count);
            mBlendMask = temp;
        }
        else
        {
            mBlendMask = 0;
        }
    }
    //------------------------------------------------------------------------
    NodeFusion::~NodeFusion()
    {
        if(mBlendMask != 0)
        {
            N_free(mBlendMask);
            mBlendMask = 0;
        }
    }
    //------------------------------------------------------------------------
    void NodeFusion::apply(NIIf factor)
    {
        factor *= mWeight;
        if(mAnimation)
        {
            TrackTime tt = mAnimation->getTime(mTimePos);

            Animation::TrackList::const_iterator i, iend = mAnimation->getTrack().end();
            if(mBlendMask == 0)
            {
                for(i = mAnimation->getTrack().begin(); i != iend; ++i)
                {
                    Bone * b = mDst->get(i->first);
                    static_cast<NodeKeyFrameTrack *>(i->second)->apply(this, b, &tt, factor, mScale);
                }
            }
            else
            {
                for(i = mAnimation->getTrack().begin(); i != iend; ++i)
                {
                    Bone * b = mDst->get(i->first);
                    static_cast<NodeKeyFrameTrack *>(i->second)->apply(this, b, &tt, mBlendMask[b->getID()] * factor, mScale);
                }
            }
        }
    }
    //---------------------------------------------------------------------
    void NodeFusion::setScale(NIIf scale)
    {
        mScale = scale;
    }
    //---------------------------------------------------------------------
    NIIf NodeFusion::getScale() const
    {
        return mScale;
    }
    //---------------------------------------------------------------------
    void NodeFusion::setMode(Mode im)
    {
        mMode = im;
    }
    //---------------------------------------------------------------------
    NodeFusion::Mode NodeFusion::getMode() const
    {
        return mMode;
    }
    //---------------------------------------------------------------------
    void NodeFusion::setRotationMode(RotationMode im)
    {
        mRotationMode = im;
    }
    //---------------------------------------------------------------------
    NodeFusion::RotationMode NodeFusion::getRotationMode() const
    {
        return mRotationMode;
    }
    //------------------------------------------------------------------------
    void NodeFusion::setFactor(NIIf * in)
    {
        if(mBlendMask != 0)
        {
            N_free(mBlendMask);
        }

        if(in != 0)
        {
            mBlendMask = in;
            return;
        }
        else
        {
            mBlendMask = 0;
        }

        if(mEnable)
            mParent->_notifyFrame();
    }
    //------------------------------------------------------------------------
    void NodeFusion::getFactor(NIIf *& out) const
    {
        out = mBlendMask;
    }
    //------------------------------------------------------------------------
    void NodeFusion::setFactor(Nidx boneidx, NIIf weight)
    {
        N_assert(mBlendMask, "error");
        mBlendMask[boneidx] = weight;
        if(mEnable)
            mParent->_notifyFrame();
    }
    //------------------------------------------------------------------------
    NIIf NodeFusion::getFactor(Nidx boneidx) const
    {
        N_assert(mBlendMask, "error");
        return mBlendMask[boneidx];
    }
    //------------------------------------------------------------------------
    void NodeFusion::reset()
    {
        mDst->reset();
    }
    //------------------------------------------------------------------------
    AnimationFusion * NodeFusion::clone(AnimationFusionObj * own) const
    {
        NodeFusion * re = N_new NodeFusion(own, *this);
        return re;
    }
    //------------------------------------------------------------------------
}