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
#include "NiiKeyFrameTrack.h"
#include "NiiKeyFrame.h"
#include "NiiAnimation.h"
#include "NiiException.h"

namespace NII
{
    //---------------------------------------------------------------------
    struct KeyFrameTimeLess
    {
        bool operator() (const KeyFrame * kf, const KeyFrame * kf2) const
        {
            return kf->getTime() < kf2->getTime();
        }
    };

    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // KeyFrameTrack
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    KeyFrameTrack::KeyFrameTrack(const Animation * own, KeyFrameTrackID id) :
        mParent(const_cast<Animation *>(own)),
        mID(id),
        mMode(KeyFrameTrack::M_HOST)
    {
    }
    //---------------------------------------------------------------------
    KeyFrameTrack::~KeyFrameTrack()
    {
        removeAll();
    }
    //---------------------------------------------------------------------
    KeyFrameTrackID KeyFrameTrack::getID() const
    {
        return mID;
    }
    //---------------------------------------------------------------------
    Animation * KeyFrameTrack::getParent() const
    {
        return mParent;
    }
    //---------------------------------------------------------------------
    KeyFrame * KeyFrameTrack::get(Nidx index) const
    {
        N_assert(index < mKeyFrames.size(), "error");
        return mKeyFrames[index];
    }
    //---------------------------------------------------------------------
    KeyFrame * KeyFrameTrack::create(TimeDurMS pos)
    {
        KeyFrame * kf = createImpl(pos);
        KeyFrames::iterator i = std::upper_bound(mKeyFrames.begin(), mKeyFrames.end(), kf, KeyFrameTimeLess());
        mKeyFrames.insert(i, kf);

        notifyImpl();

        return kf;
    }
    //---------------------------------------------------------------------
    void KeyFrameTrack::modify(KeyFrame * obj, TimeDurMS pos)
    {
        KeyFrames::iterator i, iend = mKeyFrames.end();
        for(i = mKeyFrames.begin(); i != iend; ++i)
        {
            if(*i == obj)
            {
                mKeyFrames.erase(i);
                i = std::upper_bound(mKeyFrames.begin(), mKeyFrames.end(), obj, KeyFrameTimeLess());
                mKeyFrames.insert(i, obj);
                notifyImpl();
                break;
            }
        }
    }
    //---------------------------------------------------------------------
    void KeyFrameTrack::destroy(Nidx index)
    {
        // If you hit this N_assert, then the keyframe index is out of bounds
        N_assert(index < mKeyFrames.size(), "error");

        KeyFrames::iterator i = mKeyFrames.begin();
        std::advance(i, index);
        N_delete *i;
        mKeyFrames.erase(i);

        notifyImpl();
    }
    //---------------------------------------------------------------------
    void KeyFrameTrack::removeAll()
    {
        KeyFrames::iterator i, iend = mKeyFrames.end();
        for(i = mKeyFrames.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mKeyFrames.clear();

        notifyImpl();
    }
    //---------------------------------------------------------------------
    NCount KeyFrameTrack::getCount() const
    {
        return mKeyFrames.size();
    }
    //---------------------------------------------------------------------
    const KeyFrameTrack::KeyFrames & KeyFrameTrack::getList() const
    {
        return mKeyFrames;
    }
    //---------------------------------------------------------------------
    void KeyFrameTrack::notifyImpl() const
    {
    }
    //---------------------------------------------------------------------
    bool KeyFrameTrack::isValid() const
    {
        return true;
    }
    //---------------------------------------------------------------------
    void KeyFrameTrack::optimise()
    {
    }
    //---------------------------------------------------------------------
    NIIf KeyFrameTrack::get(TrackTime * tt, KeyFrame *& kf1, KeyFrame *& kf2) const
    {
        TimeDurMS t1, t2;
        TimeDurMS pos = tt->getTimePos();
        TimeDurMS total = mParent->getDur();
        N_assert(total > 0.0f, "Invalid animation length!");

        if(pos > total && total > 0)
            pos = pos % total;

        KeyFrames::const_iterator i, iend = mKeyFrames.end();
        for(i = mKeyFrames.begin(); i != iend; ++i)
        {
            if((*i)->getTime() >= pos)
                break;
        }

        if(i == mKeyFrames.end())
        {
            kf2 = mKeyFrames.front();
            t2 = mParent->getDur() + kf2->getTime();
            --i;
        }
        else
        {
            kf2 = *i;
            t2 = kf2->getTime();

            // 查找最后一个关键帧之前的或当前时间
            if(i != mKeyFrames.begin() && pos < (*i)->getTime())
            {
                --i;
            }
        }

        tt->setLocalKeyIndex(std::distance(mKeyFrames.begin(), i));

        kf1 = *i;

        t1 = kf1->getTime();

        if(t1 == t2)
        {
            return 0.0f;
        }
        else
        {
            return (NIIf)(pos - t1) / (t2 - t1);
        }
    }
    //---------------------------------------------------------------------
    void KeyFrameTrack::setOrigin(const KeyFrameTrack *, TrackTime *)
    {
    }
    //---------------------------------------------------------------------
    void KeyFrameTrack::copyImpl(const KeyFrameTrack * in)
    {
        KeyFrames::const_iterator i, iend = in->mKeyFrames.end();
        for(i = in->mKeyFrames.begin(); i != iend; ++i)
        {
            KeyFrame * temp = (*i)->clone(this);
            mKeyFrames.push_back(temp);
        }
    }
    //---------------------------------------------------------------------
}