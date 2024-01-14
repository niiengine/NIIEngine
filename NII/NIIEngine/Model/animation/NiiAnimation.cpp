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
#include "NiiAnimation.h"
#include "NiiException.h"
#include "NiiSkeleton.h"
#include "NiiGeometrySpace.h"
#include "NiiStrConv.h"
#include "NiiAnimationFusion.h"
#include "NiiScriptProperty.h"

using namespace NII::NII_COMMAND;

namespace NII
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	// AnimationProperty
	//------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_PROPERTY(ScriptProperty, AnimationProperty, KeyFrameTrackProperty,              0); // Track
    N_PROPERTY(ScriptProperty, AnimationProperty, KeyFrameProperty,                   1); // KeyFrame
    N_PROPERTY(ScriptProperty, AnimationProperty, AnimationEventProperty,             2); // Event

    N_PROPERTY(ScriptProperty, AnimationProperty, DurationProperty,                   3); // duration
    N_PROPERTY(ScriptProperty, AnimationProperty, ReplayModeAttribute,                4); // play_mode
    N_PROPERTY(ScriptProperty, AnimationProperty, ReplayModeOnce,                     5); // once
    N_PROPERTY(ScriptProperty, AnimationProperty, ReplayModeLoop,                     6); // loop
    N_PROPERTY(ScriptProperty, AnimationProperty, ReplayModeBounce,                   7); // bounce
    N_PROPERTY(ScriptProperty, AnimationProperty, AutoStartProperty,                  8); // auto_start
    N_PROPERTY(ScriptProperty, AnimationProperty, DataTypeProperty,                   9); // data_type
    N_PROPERTY(ScriptProperty, AnimationProperty, TargetPropertyAttribute,            10);// property
    N_PROPERTY(ScriptProperty, AnimationProperty, CalcModeProperty,                   11);// calc_mode
    N_PROPERTY(ScriptProperty, AnimationProperty, ValueMethodAbsProperty,             12);// absolute
    N_PROPERTY(ScriptProperty, AnimationProperty, ValueMethodRelProperty,             13);// relative
    N_PROPERTY(ScriptProperty, AnimationProperty, ValueMethodMulRelProperty,          14);// relative multiply
    N_PROPERTY(ScriptProperty, AnimationProperty, FactorConstantProperty,             15);// constant
    N_PROPERTY(ScriptProperty, AnimationProperty, FactorPointProperty,                16);// point
    N_PROPERTY(ScriptProperty, AnimationProperty, FactorSquareProperty,               17);// square
    N_PROPERTY(ScriptProperty, AnimationProperty, FactorSquareRootProperty,           18);// square_root
    N_PROPERTY(ScriptProperty, AnimationProperty, PositionProperty,                   19);// position
    N_PROPERTY(ScriptProperty, AnimationProperty, ValuePropertySrcProperty,           20);// property_value
    N_PROPERTY(ScriptProperty, AnimationProperty, EventAttribute,                     21);// event
    N_PROPERTY(ScriptProperty, AnimationProperty, ActionAttribute,                    22);// action
    N_PROPERTY(ScriptProperty, AnimationProperty, PropertyCount,                      23);
    //------------------------------------------------------------------------
    //---------------------------------------------------------------------
    Animation::Animation(AnimationID id, TimeDurMS dur, AnimationMode mode) :
        mID(id),
        mLength(dur),
        mContainer(0),
        mBaseID(0),
        mBaseTime(0),
        mMode(mode),
        mPlayMode(RM_Once),
        mAutoStart(false)
    {
    }
    //---------------------------------------------------------------------
    Animation::~Animation()
    {
        TrackList::iterator i, iend = mTracks.end();
        for(i = mTracks.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
        mTracks.clear();
    }
    //---------------------------------------------------------------------
    void Animation::setDur(TimeDurMS dur)
    {
        mLength = dur;
    }
    //---------------------------------------------------------------------
    TimeDurMS Animation::getDur() const
    {
        return mLength;
    }
    //---------------------------------------------------------------------
    void Animation::add(KeyFrameTrack * obj)
    {
        N_assert(!isExist(obj->getID()), "动画轨道已存在 ID:");

        mTracks.insert(Npair(obj->getID(), obj));
    }
    //---------------------------------------------------------------------
    void Animation::remove(KeyFrameTrackID id)
    {
        TrackList::iterator i = mTracks.find(id);
        if(i != mTracks.end())
        {
            mTracks.erase(i);
        }
    }
    //---------------------------------------------------------------------
    void Animation::destroy(KeyFrameTrackID id)
    {
        TrackList::iterator i = mTracks.find(id);
        if(i != mTracks.end())
        {
            N_delete i->second;
            mTracks.erase(i);
        }
    }
    //---------------------------------------------------------------------
    KeyFrameTrack * Animation::get(KeyFrameTrackID id) const
    {
        TrackList::const_iterator i = mTracks.find(id);
        if(i != mTracks.end())
        {
            i->second;
        }
        return 0;
    }
    //---------------------------------------------------------------------
    bool Animation::isExist(KeyFrameTrackID id) const
    {
        return (mTracks.find(id) != mTracks.end());
    }
    //---------------------------------------------------------------------
    NCount Animation::getCount() const
    {
        return mTracks.size();
    }
    //------------------------------------------------------------------------
    void Animation::add(EventID eid, ActionType action)
    {
        Actions::iterator it = mEvents.find(eid);
        while(it != mEvents.end() && it->first == eid)
        {
            if(it->second == action)
            {
                N_EXCEPT(InvalidRequest, _I18n("已经存在"));
            }
            ++it;
        }
        mEvents.insert(Npair(eid, action));
    }
    //------------------------------------------------------------------------
    void Animation::remove(EventID eid, ActionType action)
    {
        Actions::iterator it = mEvents.find(eid);
        while(it != mEvents.end() && it->first == eid)
        {
            if(it->second == action)
            {
                mEvents.erase(it);
                return;
            }
            ++it;
        }
    }
    //------------------------------------------------------------------------
    void Animation::connect(AnimationFusion * obj)
    {
        Actions::const_iterator it, itend = mEvents.end();
        for(it = mEvents.begin(); it != itend; ++it)
        {
            EventID e = it->first;
            switch(it->second)
            {
            case AT_Start :
                obj->addSignal(obj->bind(e, obj, (CommandObjFunctor)&AnimationFusion::startCB));
                break;
            case AT_Stop :
                obj->addSignal(obj->bind(e, obj, (CommandObjFunctor)&AnimationFusion::stopCB));
                break;
            case AT_Pause :
                obj->addSignal(obj->bind(e, obj, (CommandObjFunctor)&AnimationFusion::pauseCB));
                break;
            case AT_Resume :
                obj->addSignal(obj->bind(e, obj, (CommandObjFunctor)&AnimationFusion::resumeCB));
                break;
            default:
                N_EXCEPT(InvalidRequest, _I18n("未知事件类型 '") + it->second);
            }
        }
    }
    //------------------------------------------------------------------------
    void Animation::disconnect(AnimationFusion * obj)
    {
        if(this == obj->getAnimation())
            obj->removeSignal();
    }
    //------------------------------------------------------------------------
    AnimationID Animation::getID() const
    {
        return mID;
    }
    //------------------------------------------------------------------------
    AnimationMode Animation::getMode() const
    {
        return mMode;
    }
    //------------------------------------------------------------------------
    void Animation::setPlayMode(PlayMode mode)
    {
        mPlayMode = mode;
    }
    //------------------------------------------------------------------------
    Animation::PlayMode Animation::getPlayMode() const
    {
        return mPlayMode;
    }
    //------------------------------------------------------------------------
    void Animation::setAutoStart(bool b)
    {
        mAutoStart = b;
    }
    //------------------------------------------------------------------------
    bool Animation::isAutoStart() const
    {
        return mAutoStart;
    }
    //---------------------------------------------------------------------
    void Animation::setBase(AnimationID id, TimeDurMS ms)
    {
        mBaseID = id;
        mBaseTime = ms;

        if(mBaseID && mContainer != 0)
        {
            Animation * baseAni = mContainer->getAnimation(mBaseID);
            if(baseAni)
            {
                KeyFrameTrack * baseTrack;
                TrackList::iterator i, iend = mTracks.end();
                for(i = mTracks.begin(); i != iend; ++i)
                {
                    KeyFrameTrack * track = i->second;

                    if(baseAni == this)
                        baseTrack = track;
                    else
                        baseTrack = baseAni->get(track->getID());

                    TrackTime tt = baseAni->getTime(mBaseTime);
                    track->setOrigin(baseTrack, &tt);
                }
            }
        }
    }
    //---------------------------------------------------------------------
    AnimationID Animation::getBaseID() const
    {
        return mBaseID;
    }
    //---------------------------------------------------------------------
    TimeDurMS Animation::getBaseTime() const
    {
        return mBaseTime;
    }
    //---------------------------------------------------------------------
    void Animation::apply(AnimationFusion * fusion, TimeDurMS time, NIIf weight, NIIf scale)
    {
        TrackList::iterator i, iend = mTracks.end();
        for(i = mTracks.begin(); i != iend; ++i)
        {
            i->second->apply(fusion, &getTime(time), weight, scale);
        }
    }
    //------------------------------------------------------------------------
    const Animation::TrackList & Animation::getTrack() const
    {
        return mTracks;
    }
    //------------------------------------------------------------------------
    const Animation::Actions & Animation::getEvent() const
    {
        return mEvents;
    }
    //------------------------------------------------------------------------
    TrackTime Animation::getTime(TimeDurMS pos) const
    {
        return TrackTime(pos, 0, 0);
    }
    //------------------------------------------------------------------------
    void Animation::optimise(bool non)
    {
        // Iterate over the node tracks and identify those with no useful keyframes
        list<KeyFrameTrack *>::type discard;

        TrackList::iterator i, iend = mTracks.end();
        for(i = mTracks.begin(); i != iend; ++i)
        {
            KeyFrameTrack * track = i->second;
            if(non && !track->isValid())
            {
                // mark the entire track for destruction
                discard.push_back(i->second);
            }
            else
            {
                track->optimise();
            }
        }

        list<KeyFrameTrack *>::type::iterator j, jend = discard.end();
        for(j = discard.begin(); j != jend; ++j)
        {
            destroy((*j)->getID());
        }
    }
    //-----------------------------------------------------------------------
    AnimationFusion * Animation::createFusion(AnimationObj * obj, NIIf weight, bool enable)
    {
        N_assert(mTracks.size(), "logic error");
        return mTracks.begin()->second->createFusion(obj, this, weight, enable);
    }
    //-----------------------------------------------------------------------
    Animation * Animation::clone(AnimationID id) const
    {
        Animation * re = N_new Animation(id, mLength, mMode);

        TrackList::const_iterator i, iend = mTracks.end();
        for(i = mTracks.begin(); i != iend; ++i)
        {
            i->second->clone(re);
        }
        return re;
    }
    //------------------------------------------------------------------------
}