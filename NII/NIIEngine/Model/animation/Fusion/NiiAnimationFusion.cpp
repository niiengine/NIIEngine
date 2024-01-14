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
#include "NiiAnimationFusion.h"
#include "NiiAnimationObj.h"
#include "NiiException.h"
#include "NiiStrConv.h"
#include "NiiAnimationEventArgs.h"
#include "NiiIDManager.h"

namespace NII
{
    //---------------------------------------------------------------------
    N_EVENT(CommandObj, AnimationFusion, AnimationStartEvent,      0);
    N_EVENT(CommandObj, AnimationFusion, AnimationStopEvent,       1);
    N_EVENT(CommandObj, AnimationFusion, AnimationPauseEvent,      2);
    N_EVENT(CommandObj, AnimationFusion, AnimationResumeEvent,     3);
    N_EVENT(CommandObj, AnimationFusion, AnimationEndEvent,        4);
    N_EVENT(CommandObj, AnimationFusion, AnimationLoopEvent,       5);
    N_EVENT(CommandObj, AnimationFusion, EventCount,               6);
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // AnimationDataValue
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    AnimationDataValue::AnimationDataValue(AnimationFusion * target) :
        DataValue<NIIf>(false),
        mTargetFusion(target)
    {
    }
    //---------------------------------------------------------------------
    AnimationDataValue::~AnimationDataValue()
    {
    }
    //---------------------------------------------------------------------
    const NIIf AnimationDataValue::getValue() const
    {
        return (NIIf)mTargetFusion->tell() / mTargetFusion->getDur();
    }
    //---------------------------------------------------------------------
    void AnimationDataValue::setValue(const NIIf & value)
    {
        mTargetFusion->seek((TimeDurMS)(mTargetFusion->getDur() * value));
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // AnimationFusion
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    AnimationFusion::AnimationFusion(AnimationObj * obj, Animation * ani, TimeDurMS pos, 
        TimeDurMS dur, NIIf weight, bool enable):
            mParent(0),
            mAnimationObj(obj),
            mAnimation(ani),
            mBegin(pos),
            mTimePos(pos),
            mLength(dur),
            mWeight(weight),
            mMaxCostLimit(0),
            mMinCostLimit(0),
            mTimeFactor(1.0f),
            mAutoUpdate(true),
            mSkipStep(false),
            mEnable(enable),
            mReplayback(false)
    {
        notifySignal(AnimationFusion::EventCount);
        if(mAnimation)
        {
            mAnimation->connect(this);
            mPlayMode = mAnimation->getPlayMode(),
            mAutoStart = mAnimation->isAutoStart();
        }
        else
        {
            mPlayMode = Animation::RM_Once;
            mAutoStart = false;
        }
    }
    //---------------------------------------------------------------------
    AnimationFusion::AnimationFusion(AnimationFusionObj * own, const AnimationFusion & o):
        mParent(own),
        mAnimation(o.mAnimation),
        mTimePos(o.mTimePos),
        mLength(o.mLength),
        mWeight(o.mWeight),
        mEnable(o.mEnable),
        mPlayMode(o.mPlayMode),
        mTimeFactor(o.mTimeFactor),
        mAutoUpdate(o.mAutoUpdate),
        mBegin(o.mBegin),
        mMaxCostLimit(0),
        mMinCostLimit(0),
        mSkipStep(false),
        mAutoStart(false),
        mReplayback(false)
    {
        if(mAnimation)
        {
            mAnimation->connect(this);
        }
    }
    //---------------------------------------------------------------------
    AnimationFusion::~AnimationFusion()
    {
        if(mAnimation)
        {
            mAnimation->disconnect(this);
        }
    }
    //---------------------------------------------------------------------
    void AnimationFusion::setWeight(NIIf weight)
    {
        mWeight = weight;

        if(mEnable)
            mParent->_notifyFrame();
    }
    //---------------------------------------------------------------------
    void AnimationFusion::seek(TimeDurMS pos)
    {
        if(pos != mTimePos)
        {
            switch (mPlayMode)
            {
            case Animation::RM_Loop:
            {
                mTimePos = pos;
                // Wrap
                if (mTimePos > mLength || mTimePos < mBegin)
                {
                    mTimePos = mBegin;
                    onLoop();
                }
            }
                break;
            case Animation::RM_Once:
            {
                mTimePos = pos;
                // Clamp
                if (mTimePos < mBegin)
                {
                    mTimePos = mBegin;
                }
                else if (mTimePos > mLength)
                {
                    mTimePos = mLength;
                    setEnable(false);
                    onStop();
                    onEnd();
                }
            }
                break;
            case Animation::RM_Bounce:
            {
                TimeDurMS newtime;
                if (mReplayback)
                {
                    newtime = mTimePos + mTimePos - pos;
                }
                else
                {
                    newtime = mTimePos + pos - mTimePos;
                }

                while (newtime < mBegin || newtime > mLength)
                {
                    if (newtime < mBegin)
                    {
                        mReplayback = false;
                        newtime = 2 * mBegin - newtime;

                        onLoop();
                    }

                    if (newtime > mLength)
                    {
                        mReplayback = true;
                        newtime = 2 * mLength - newtime;

                        onLoop();
                    }
                }
            }
                break;
            default:
            {
                mTimePos = pos;
                // Clamp
                if (mTimePos < 0)
                    mTimePos = 0;
                else if (mTimePos > mLength)
                    mTimePos = mLength;
            }
                break;
            }
            if(mEnable)
                mParent->_notifyFrame();
        }
    }
    //---------------------------------------------------------------------
    void AnimationFusion::skip(TimeDurMS offset)
    {
        seek(mTimePos + offset);
    }
    //---------------------------------------------------------------------
    TimeDurMS AnimationFusion::tell() const
    {
        return mTimePos;
    }
    //---------------------------------------------------------------------
    bool AnimationFusion::isEnd() const
    {
        return (mTimePos >= mLength && mPlayMode != Animation::RM_Loop);
    }
    //---------------------------------------------------------------------
    void AnimationFusion::setBeginTime(TimeDurMS ms)
    {
        mBegin = ms;
    }
    //---------------------------------------------------------------------
    TimeDurMS AnimationFusion::getBeginTime() const
    {
        return mBegin;
    }
    //---------------------------------------------------------------------
    void AnimationFusion::reset()
    {
    }
    //---------------------------------------------------------------------
    void AnimationFusion::update(TimeDurMS cost)
    {
        if(!mEnable)
        {
            return;
        }

        if(mSkipStep)
        {
            mSkipStep = false;
            return;
        }

        if(cost > mMaxCostLimit && mMaxCostLimit > 0)
        {
            cost = std::max(cost, mMaxCostLimit);
        }

        if(cost < mMinCostLimit && mMinCostLimit > 0)
        {
            cost = std::min(cost, mMinCostLimit);
        }

        cost = (TimeDurMS)((NIIf)cost * mTimeFactor);

        seek(mTimePos + cost);
    }
    //------------------------------------------------------------------------
    void AnimationFusion::start(bool skip)
    {
        seek(0);
        mSkipStep = skip;

        if(mAnimation && mLength > 0)
        {
            setEnable(true);
            onStart();
        }
        else
        {
            onStart();
            onEnd();
        }
    }
    //------------------------------------------------------------------------
    void AnimationFusion::stop()
    {
        seek(0);
        setEnable(false);
        onStop();
    }
    //------------------------------------------------------------------------
    void AnimationFusion::pause()
    {
        setEnable(false);
        onPause();
    }
    //------------------------------------------------------------------------
    void AnimationFusion::resume(bool skip)
    {
        mSkipStep = skip;

        if(mAnimation && mLength > 0)
        {
            setEnable(true);
            onResume();
        }
        else
        {
            onResume();
            onEnd();
        }
    }
    //---------------------------------------------------------------------
    void AnimationFusion::addSignal(SignalPtr conn)
    {
        mEvents.push_back(conn);
    }
    //---------------------------------------------------------------------
    void AnimationFusion::removeSignal()
    {
        SignalPtrs::iterator i, iend = mEvents.end();
        for(i = mEvents.begin(); i != iend; ++i)
        {
            (*i)->disconnect();
        }

        mEvents.clear();
    }
    //---------------------------------------------------------------------
    void AnimationFusion::onStart()
    {
        AnimationEventArgs arg(this);
        signal(AnimationStartEvent, &arg);
    }
    //------------------------------------------------------------------------
    void AnimationFusion::onStop()
    {
        AnimationEventArgs arg(this);
        signal(AnimationStopEvent, &arg);
    }
    //------------------------------------------------------------------------
    void AnimationFusion::onPause()
    {
        AnimationEventArgs arg(this);
        signal(AnimationPauseEvent, &arg);
    }
    //------------------------------------------------------------------------
    void AnimationFusion::onResume()
    {
        AnimationEventArgs arg(this);
        signal(AnimationResumeEvent, &arg);
    }
    //------------------------------------------------------------------------
    void AnimationFusion::onEnd()
    {
        AnimationEventArgs arg(this);
        signal(AnimationEndEvent, &arg);
    }
    //------------------------------------------------------------------------
    void AnimationFusion::onLoop()
    {
        AnimationEventArgs arg(this);
        signal(AnimationLoopEvent, &arg);
    }
    //------------------------------------------------------------------------
    void AnimationFusion::startCB(const EventArgs *)
    {
        start();
    }
    //------------------------------------------------------------------------
    void AnimationFusion::stopCB(const EventArgs *)
    {
        stop();
    }
    //------------------------------------------------------------------------
    void AnimationFusion::pauseCB(const EventArgs *)
    {
        pause();
    }
    //------------------------------------------------------------------------
    void AnimationFusion::resumeCB(const EventArgs *)
    {
        resume();
    }
    //---------------------------------------------------------------------
    void AnimationFusion::setEnable(bool b)
    {
        if (mEnable != b)
        {
            mEnable = b;
            mParent->_notifyEnable(this);
        }
    }
    //---------------------------------------------------------------------
    void AnimationFusion::setState(const AnimationFusion & o)
    {
        mTimePos = o.mTimePos;
        mLength = o.mLength;
        mWeight = o.mWeight;
        mEnable = o.mEnable;
        mPlayMode = o.mPlayMode;
        mParent->_notifyFrame();
    }
    //---------------------------------------------------------------------
}