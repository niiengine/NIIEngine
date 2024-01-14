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
#include "NiiSoundKeyFrameTrack.h"
#include "NiiSoundFusion.h"
#include "NiiSoundKeyFrame.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // SoundKeyFrameTrack
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    SoundKeyFrameTrack::SoundKeyFrameTrack(const Animation * o, KeyFrameTrackID id) :
        KeyFrameTrack(o, id),
        mTarget(0)
    {
    }
    //------------------------------------------------------------------------
    SoundKeyFrameTrack::SoundKeyFrameTrack(const Animation * o, KeyFrameTrackID id, SpaceNode * target) :
        KeyFrameTrack(o, id),
        mTarget(target)
    {
    }
    //------------------------------------------------------------------------
    SoundKeyFrameTrack::~SoundKeyFrameTrack()
    {
    }
    //------------------------------------------------------------------------
    void SoundKeyFrameTrack::getFusion(AnimationFusion * dst, TrackTime * tt, KeyFrame * kf) const
    {
        SoundKeyFrame * re = static_cast<SoundKeyFrame *>(kf);
        KeyFrame * tk1, * tk2;

        Nidx leftkf = tt->getLocalKeyIndex();
        NIIf t = get(tt, tk1, tk2);
        SoundKeyFrame * k1 = static_cast<SoundKeyFrame *>(tk1);
        SoundKeyFrame * k2 = static_cast<SoundKeyFrame *>(tk2);

        if(t == 1.0)
        {
            re = k2;
        }
        else
        {
            re = k1;
        }
    }
    //------------------------------------------------------------------------
    void SoundKeyFrameTrack::setTarget(SpaceNode * node)
    {
        mTarget = node;
    }
    //------------------------------------------------------------------------
    SpaceNode * SoundKeyFrameTrack::getTarget() const
    {
        return mTarget;
    }
    //------------------------------------------------------------------------
    void SoundKeyFrameTrack::apply(AnimationFusion * dst, TrackTime * tt, NIIf weight, NIIf scale) const
    {
        apply(dst, mTarget, tt, weight, scale);
    }
    //------------------------------------------------------------------------
    void SoundKeyFrameTrack::apply(AnimationFusion * dst, SpaceNode * com, TrackTime * tt, NIIf weight, NIIf) const
    {
        // Nothing to do if no keyframes or zero weight or no node
        if(mKeyFrames.empty() || !weight || !com)
            return;

        SoundKeyFrame kf(0, tt->getTimePos());
        getFusion(dst, tt, &kf);

        kf.play(com);
    }
    //------------------------------------------------------------------------
    void SoundKeyFrameTrack::notifyImpl() const
    {
    }
    //------------------------------------------------------------------------
    bool SoundKeyFrameTrack::isValid() const
    {
        KeyFrames::const_iterator i, iend = mKeyFrames.end();
        for(i = mKeyFrames.begin(); i != iend; ++i)
        {
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void SoundKeyFrameTrack::optimise()
    {
    }
    //--------------------------------------------------------------------------
    KeyFrame * SoundKeyFrameTrack::createImpl(TimeDurMS time)
    {
        return N_new SoundKeyFrame(this, time);
    }
    //--------------------------------------------------------------------------
    AnimationFusion * SoundKeyFrameTrack::createFusion(AnimationObj * obj, 
        Animation * ani, NIIf weight, bool enable) const
    {
        return N_new SoundFusion(obj, ani, 0, ani->getDur());
    }
    //--------------------------------------------------------------------------
    KeyFrameTrack * SoundKeyFrameTrack::clone(const Animation * o) const
    {
        SoundKeyFrameTrack * re = N_new SoundKeyFrameTrack(o, mID, mTarget);
        re->copyImpl(this);
        return re;
    }
    //--------------------------------------------------------------------------
}