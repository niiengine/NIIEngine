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
#include "NiiAnimationManager.h"
#include "NIIAnimationObj.h"
#include "NiiPredefinedControllers.h"
#include "NiiEngine.h"
/*
public:
    struct IDLess
    {
        bool operator()(const Animation * s1, const Animation * s2) const
        {
            return s1->getID() < s2->getID();
        }
    }; */
namespace NII
{
    /** 预定义控制器值,获取最后的帧时间
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameTimeControllerValue : public DataValue<TimeDurMS>,
        public RenderFrameListener
    {
    public:
        FrameTimeControllerValue(bool autoDsy) :
            DataValue<TimeDurMS>(autoDsy)
        {
            N_Engine().add(this);
            mFrameTime = 0;
            mTimeFactor = 1;
            mFrameDelay = 0;
            mElapsedTime = 0;
        }
        ~FrameTimeControllerValue()
        {
            N_Engine().remove(this);
        }

        /// @copydetails RenderFrameListener::onEndRender
        bool onEndRender(const RenderFrameArgs * evt)
        {
            return true;
        }

        /// @copydetails RenderFrameListener::onPreRender
        bool onPreRender(const RenderFrameArgs * evt)
        {
            if (mFrameDelay)
            {
                mFrameTime = mFrameDelay;
                mTimeFactor = (NIIf)mFrameDelay / evt->mCurrent;
            }
            else
            {
                // Save the time value after applying time factor
                mFrameTime = mTimeFactor * evt->mCurrent;
            }

            mElapsedTime += mFrameTime;
            return true;
        }

        /// @copydetails DataValue::getValue
        const TimeDurMS getValue() const
        {
            return mFrameTime;
        }

        /// @copydetails DataValue::setValue
        void setValue(const TimeDurMS & value)
        {
            // 从帧监听器中设置
        }

        void setTimeFactor(NIIf f)
        {
            if (f >= 0)
            {
                mTimeFactor = f;
                mFrameDelay = 0;
            }
        }

        inline NIIf getTimeFactor() const
        {
            return mTimeFactor;
        }

        inline void setFixFrame(TimeDurMS fd)
        {
            mTimeFactor = 0;
            mFrameDelay = fd;
        }

        inline TimeDurMS getFixFrame() const
        {
            return mFrameDelay;
        }

        inline void setCostTime(TimeDurMS cost)
        {
            mElapsedTime = cost;
        }

        inline TimeDurMS getCostTime() const
        {
            return mElapsedTime;
        }
    protected:
        TimeDurMS mFrameTime;
        TimeDurMS mElapsedTime;
        TimeDurMS mFrameDelay;
        NIIf mTimeFactor;
    };
    /** 预定义控制器函数,源资源不做处理直接为目标资源
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PassthroughControllerFunction : public DataFunc<TimeDurMS, TimeDurMS>
    {
    public:
        /**
        @param[in] sequenceTime The amount of time in seconds it takes to loop through the whole animation sequence.
        @param[in] timeOffset The offset in seconds at which to start (default is start at 0)
        */
        PassthroughControllerFunction(bool autoDsy) :
            DataFunc<TimeDurMS, TimeDurMS>(autoDsy) {}

        ///@copydetails DataFunc::func
        TimeDurMS func(const TimeDurMS & src)
        {
            return src;
        }
    };
    //------------------------------------------------------------------------
    template<> AnimationManager * Singleton<AnimationManager>::mOnly  = 0;
    //------------------------------------------------------------------------
    AnimationManager::AnimationManager():
        mLastFrameNumber(0),
        mFrameTimeValue(N_new FrameTimeControllerValue(false)),
        mNormalTimeFunc(N_new PassthroughControllerFunction(false)),
        mValidID(N_Max_Valid_ID)
    {
    }
    //------------------------------------------------------------------------
    AnimationManager::~AnimationManager()
    {
        removeAllFusion();
        AniObjList::iterator i, iend = mAniObjList.end();
        for(i = mAniObjList.begin(); i != iend; ++i)
        {
            N_delete i->first;
        }
        mAniObjList.clear();
    }
    //------------------------------------------------------------------------
    Animation * AnimationManager::createAnimation(AnimationObj * obj, AnimationID id, 
        TimeDurMS time, AnimationMode mode)
    {
        N_lock_mutex(mAnimationsMutex)

        if(obj->isAnimationExist(id))
        {
            N_EXCEPT(UniqueRepeat, _I18n("UI动画已经存在,名字: '") + id);
        }

        AnimationID fid;
        if(id == 0)
        {
            fid = genValidID();
        }
        else
        {
            fid = id;
        }

        AniObjList::iterator i = mAniObjList.find(obj);
        if(i == mAniObjList.end())
        {
            i = mAniObjList.insert(Npair(obj, AnimationList())).first;
        }
        Animation * ret = obj->createAnimation(fid, time);

        i->second.push_back(fid);

        return ret;
    }
    //------------------------------------------------------------------------
    Animation * AnimationManager::getAnimation(AnimationObj * obj, AnimationID id) const
    {
        return obj->getAnimation(id);
    }
    //------------------------------------------------------------------------
    Animation * AnimationManager::getAnimation(AnimationObj * obj, Nidx index) const
    {
        return obj->getAnimation(index);
    }
    //------------------------------------------------------------------------
    bool AnimationManager::isAnimationExist(AnimationObj * obj, AnimationID id) const
    {
        return obj->isAnimationExist(id);;
    }
    //------------------------------------------------------------------------
    NCount AnimationManager::getCount(AnimationObj * obj) const
    {
        N_lock_mutex(mAnimationsMutex)
        AniObjList::const_iterator i = mAniObjList.find(obj);
        if(i != mAniObjList.end())
        {
            return i->second.size();
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void AnimationManager::destroyAnimation(AnimationObj * obj, AnimationID id)
    {
        N_lock_mutex(mAnimationsMutex)

        AniObjList::iterator i = mAniObjList.find(obj);
        if(i != mAniObjList.end())
        {
            AnimationList & al = i->second;
            AnimationList::iterator j, jend = al.end();
            for(j = al.begin(); j != jend; ++i)
            {
                if(id == *j)
                {                    
                    Animation * temp = obj->getAnimationPop(id);
                    if (temp)
                    {
                        ObjFusionList::iterator k, kend = mObjFusionList.end();
                        for (k = mObjFusionList.begin(); k != kend; ++k)
                        {
                            std::pair<AniFusionList::iterator, AniFusionList::iterator> f =
                                k->second.equal_range(obj->getAniObjID());
                            AniFusionList::iterator t, temp, tend = f.second;
                            for (AniFusionList::iterator t = f.first; t != tend;)
                            {
                                if (id == t->second->getAnimation()->getID())
                                {
                                    temp = t++;
                                    k->second.erase(temp);
                                }
                                else
                                {
                                    ++t;
                                }
                            }
                            if (k->second.empty())
                                mObjFusionList.erase(k);
                        }
                        N_delete temp;
                    }
                    al.erase(j);
                    if (al.empty())
                    {
                        mAniObjList.erase(i);
                    }
                    break;
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void AnimationManager::destroyAllAnimations()
    {
        N_lock_mutex(mAnimationsMutex)
        // we have to destroy all instances to avoid dangling pointers
        // destroying all instances now is also faster than doing that for each
        // animation that is being destroyed
        removeAllFusion();

        AniObjList::iterator i, iend = mAniObjList.end();
        for(i = mAniObjList.begin(); i != iend; ++i)
        {
            N_delete i->first;
        }

        mAniObjList.clear();
    }
    //------------------------------------------------------------------------
    void AnimationManager::addFusion(AnimationFusion * fusion)
    {
        AnimationID tempid = fusion->getAnimationObj()->getAniObjID();
        ObjFusionList::iterator i = mObjFusionList.find(fusion->getParent());
        if (i == mObjFusionList.end())
        {
            i = mObjFusionList.insert(Npair(fusion->getParent(), AniFusionList())).first;
            i->second.insert(Npair(tempid, fusion));
        }
        else
        {
            /*
            std::pair<AniFusionList::iterator, AniFusionList::iterator> f = 
                i->second.equal_range(tempid);

            if (f.first != f.second)
            {
                for (AniFusionList::iterator temp = f.first; temp != f.second; ++temp)
                {
                    if (temp->second == fusion)
                        return;
                }
            }
            */
            ///假设只有一个
            i->second.insert(Npair(tempid, fusion));
        }
    }
    //------------------------------------------------------------------------
    void AnimationManager::removeFusion(AnimationFusion * fusion)
    {
        ObjFusionList::iterator i = mObjFusionList.find(fusion->getParent());
        if(i != mObjFusionList.end())
        {
            std::pair<AniFusionList::iterator, AniFusionList::iterator> f =
                i->second.equal_range(fusion->getAnimationObj()->getAniObjID());
            for(AniFusionList::iterator temp = f.first; temp != f.second; ++temp)
            {
                if (temp->second == fusion)
                {
                    ///假设只有一个
                    i->second.erase(temp);
                    break;
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void AnimationManager::removeAllFusion(AnimationFusionObj * obj)
    {
        ObjFusionList::iterator i = mObjFusionList.find(obj);
        if (i != mObjFusionList.end())
        {
            i->first->_notifyDisable();
            mObjFusionList.erase(i);
        }
    }
    //------------------------------------------------------------------------
    void AnimationManager::removeAllFusion()
    {
        ObjFusionList::iterator i, iend = mObjFusionList.end();
        for (i = mObjFusionList.begin(); i != iend; ++i)
        {
            i->first->_notifyDisable();
        }

        mAniObjList.clear();
    }
    //------------------------------------------------------------------------
    NCount AnimationManager::getFusionCount(AnimationFusionObj * obj) const
    {
        ObjFusionList::const_iterator i = mObjFusionList.find(obj);
        if (i != mObjFusionList.end())
        {
            return i->second.size();
        }
        return 0;
    }
    //-----------------------------------------------------------------------
    NIIf AnimationManager::getTimeFactor() const
    {
        return static_cast<const FrameTimeControllerValue *>(mFrameTimeValue)->getTimeFactor();
    }
    //-----------------------------------------------------------------------
    void AnimationManager::setTimeFactor(NIIf tf)
    {
        static_cast<FrameTimeControllerValue *>(mFrameTimeValue)->setTimeFactor(tf);
    }
    //-----------------------------------------------------------------------
    TimeDurMS AnimationManager::getFixFrame() const
    {
        return static_cast<const FrameTimeControllerValue *>(mFrameTimeValue)->getFixFrame();
    }
    //-----------------------------------------------------------------------
    void AnimationManager::setFixFrame(TimeDurMS fd)
    {
        static_cast<FrameTimeControllerValue *>(mFrameTimeValue)->setFixFrame(fd);
    }
    //-----------------------------------------------------------------------
    void AnimationManager::setCostTime(TimeDurMS cost)
    {
        static_cast<FrameTimeControllerValue *>(mFrameTimeValue)->setCostTime(cost);
    }
    //-----------------------------------------------------------------------
    TimeDurMS AnimationManager::getCostTime() const
    {
        return static_cast<const FrameTimeControllerValue *>(mFrameTimeValue)->getCostTime();
    }
    //------------------------------------------------------------------------
    void AnimationManager::update(TimeDurMS cost)
    {
        ObjFusionList::iterator i, iend = mObjFusionList.end();
        for(i = mObjFusionList.begin(); i != iend; ++i)
        {
            AniFusionList::iterator j, jend = i->second.end();
            for (j = i->second.begin(); j != jend; ++j)
            {
                if (j->second->isAutoUpdate())
                {
                    j->second->update(cost);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    Nid AnimationManager::genValidID()
    {
        N_lock_mutex(mValidIDMutex)
        Nid temp = mValidID;
        ++mValidID;

        return temp;
    }
    //------------------------------------------------------------------------
}