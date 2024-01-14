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
#include "NiiAnimationObj.h"
#include "NiiAnimationFusion.h"
#include "NiiAnimationManager.h"

namespace NII
{
    AnimationFusionObj::AnimationFusions AnimationFusionObj::Empty;
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // AnimationFusionObj
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    AnimationFusionObj::AnimationFusionObj() :
        mFrameMark(N_TYPE_MAX(Nmark))
    {
    }
    //---------------------------------------------------------------------
    AnimationFusionObj::AnimationFusionObj(const AnimationFusionObj & o) :
        mFrameMark(N_TYPE_MAX(Nmark))
    {
        N_lock_mutex(o.N_mutex1_name)
            ObjFusions::iterator temp;
        ObjFusions::const_iterator i, iend = o.mFusions.end();
        for (i = o.mFusions.begin(); i != iend; ++i)
        {
            temp = mFusions.insert(Npair(i->first, AnimationFusions())).first;
            AnimationFusions::const_iterator j, jend = i->second.end();
            for (j = i->second.begin(); j != jend; ++j)
            {
                const AnimationFusion * src = j->second;
                AnimationFusion * dst = src->clone(this);
                temp->second.insert(Npair(src->getAnimation()->getID(), dst));
                if (dst->isEnable())
                {
                    mValidFusions.push_back(dst);
                    N_Only(Animation).addFusion(dst);
                }
            }
        }
    }
    //---------------------------------------------------------------------
    AnimationFusionObj::~AnimationFusionObj()
    {
        removeAllFusion();
    }
    //---------------------------------------------------------------------
    bool AnimationFusionObj::isValid() const
    {
        return !mValidFusions.empty();
    }
    //---------------------------------------------------------------------
    void AnimationFusionObj::addFusion(AnimationFusion * fusion, Nid id, bool enable)
    {
        N_mutex1_lock

            ObjFusions::iterator i = mFusions.find(id);
        if (i == mFusions.end())
        {
            i = mFusions.insert(Npair(id, AnimationFusions())).first;
            i->second.insert(Npair(fusion->getAnimation()->getID(), fusion));
            fusion->mParent = this;
            if (enable)
            {
                fusion->setEnable(enable);
                N_Only(Animation).addFusion(fusion);
            }
            _notifyFrame();
        }
        else
        {
            AnimationFusions::iterator j = i->second.find(fusion->getAnimation()->getID());
            if (j == i->second.end())
            {
                i->second.insert(Npair(fusion->getAnimation()->getID(), fusion));
                fusion->mParent = this;
                if (enable)
                {
                    fusion->setEnable(enable);
                    N_Only(Animation).addFusion(fusion);
                }
                _notifyFrame();
            }
        }
    }
    //---------------------------------------------------------------------
    AnimationFusion * AnimationFusionObj::getFusion(AnimationID aid, Nid id) const
    {
        N_mutex1_lock

            ObjFusions::const_iterator i = mFusions.find(id);
        if (i != mFusions.end())
        {
            AnimationFusions::const_iterator j = i->second.find(aid);
            if (j != i->second.end())
            {
                return j->second;
            }
        }
        return 0;
    }
    //---------------------------------------------------------------------
    bool AnimationFusionObj::isFusionExist(AnimationID aid, Nid id) const
    {
        N_mutex1_lock

            return mFusions.find(aid) != mFusions.end();
    }
    //---------------------------------------------------------------------
    void AnimationFusionObj::removeFusion(AnimationID aid, Nid id)
    {
        N_mutex1_lock

            ObjFusions::iterator i = mFusions.find(id);
        if (i != mFusions.end())
        {
            AnimationFusions::iterator j = i->second.find(aid);
            if (j != i->second.end())
            {
                if (j->second->isEnable())
                {
                    FusionList::iterator z, zend = mValidFusions.end();
                    for (z = mValidFusions.begin(); z != zend; ++z)
                    {
                        if (*z == j->second)
                        {
                            mValidFusions.erase(z);
                            N_Only(Animation).removeFusion(*z);
                            break;
                        }
                    }
                }
                N_delete j->second;
                i->second.erase(j);

                if (i->second.empty())
                {
                    mFusions.erase(i);
                }
            }
        }
    }
    //---------------------------------------------------------------------
    void AnimationFusionObj::removeAllFusion(Nid id)
    {
        N_mutex1_lock
            ObjFusions::iterator i = mFusions.find(id);
        if (i != mFusions.end())
        {
            AnimationFusions::iterator j, jend = i->second.end();
            for (j = i->second.begin(); j != jend; ++j)
            {
                if (j->second->isEnable())
                {
                    FusionList::iterator z, zend = mValidFusions.end();
                    for (z = mValidFusions.begin(); z != zend; ++z)
                    {
                        if (*z == j->second)
                        {
                            mValidFusions.erase(z);
                            N_Only(Animation).removeFusion(*z);
                            break;
                        }
                    }
                }
                N_delete j->second;
            }
            mFusions.erase(i);
        }
    }
    //---------------------------------------------------------------------
    void AnimationFusionObj::removeAllFusion()
    {
        N_mutex1_lock
            ObjFusions::iterator i, iend = mFusions.end();
        for (i = mFusions.begin(); i != iend; ++i)
        {
            AnimationFusions::iterator j, jend = i->second.end();
            for (j = i->second.begin(); j != jend; ++j)
            {
                N_delete j->second;
            }
        }
        mFusions.clear();
        mValidFusions.clear();
        N_Only(Animation).removeAllFusion(this);
    }
    //---------------------------------------------------------------------
    void AnimationFusionObj::_notifyDisable()
    {
        N_mutex1_lock

            FusionList::iterator i, iend = mValidFusions.end();
        for (i = mValidFusions.begin(); i != iend; ++i)
        {
            (*i)->mEnable = false;
        }
        mValidFusions.clear();
        ++mFrameMark;
    }
    //---------------------------------------------------------------------
    void AnimationFusionObj::_notifyEnable(AnimationFusion * obj)
    {
        N_mutex1_lock

            FusionList::iterator j, jend = mValidFusions.end();
        for (j = mValidFusions.begin(); j != jend; ++j)
        {
            if (*j == obj)
            {
                if (!obj->mEnable)
                {
                    mValidFusions.erase(j);
                    N_Only(Animation).removeFusion(obj);
                }

                ++mFrameMark;
                return;
            }
        }
        if (obj->mEnable)
        {
            mValidFusions.push_back(obj);
            N_Only(Animation).addFusion(obj);
        }
        ++mFrameMark;
    }
    //---------------------------------------------------------------------
    void AnimationFusionObj::_notifyDisable(AnimationObjID aid)
    {
        N_mutex1_lock

            FusionList::iterator j = mValidFusions.begin();
        for (; j != mValidFusions.end(); ++j)
        {
            if ((*j)->getAnimationObj()->getAniObjID() == aid && (*j)->mEnable)
            {
                (*j)->mEnable = false;
                mValidFusions.erase(j);
                N_Only(Animation).removeFusion(*j);
            }
        }
        ++mFrameMark;
    }
    //---------------------------------------------------------------------
    void AnimationFusionObj::_notifyFrame()
    {
        N_mutex1_lock
            ++mFrameMark;
    }
    //---------------------------------------------------------------------
    AnimationFusionObj::AnimationFusions & AnimationFusionObj::getFusions(Nid id) const
    {
        N_mutex1_lock
            ObjFusions::iterator i = mFusions.find(id);
        if (i != mFusions.end())
            return i->second;
        return Empty;
    }
    //---------------------------------------------------------------------
    void AnimationFusionObj::copyState(const AnimationFusionObj * src)
    {
        N_lock_mutex(src->N_mutex1_name)
            N_mutex1_lock

            mValidFusions.clear();

        ObjFusions::iterator i, iend = mFusions.end();
        for (i = mFusions.begin(); i != iend; ++i)
        {
            ObjFusions::const_iterator j = src->mFusions.find(i->first);
            if (j == src->mFusions.end())
            {
                N_EXCEPT(NotExist, _I18n("没有找到动画混合组 ") + N_conv(i->first));
            }
            else
            {
                AnimationFusions::const_iterator t, temp = i->second.end();
                for (t = i->second.begin(); t != temp; ++t)
                {
                    AnimationFusions::const_iterator z = j->second.find(t->first);
                    if (z != j->second.end())
                    {
                        N_EXCEPT(NotExist, _I18n("没有找到动画混合 ") + N_conv(t->first));
                    }
                    else
                    {
                        t->second->setState(*(z->second));
                        if (z->second->isEnable())
                        {
                            mValidFusions.push_back(t->second);
                            N_Only(Animation).addFusion(t->second);
                        }
                    }
                }
            }
        }

        mFrameMark = src->mFrameMark;
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // AnimationObj
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    Nid AnimationObj::mValidID = N_Max_Valid_ID;
    //------------------------------------------------------------------------
    AnimationObj::AnimationObj()
    {
        mAniObjId = ++mValidID;
    }
    //------------------------------------------------------------------------
    AnimationObj::AnimationObj(AnimationObjID aoid) :
        mAniObjId(aoid)
    {

    }
    //------------------------------------------------------------------------
    AnimationObj::~AnimationObj()
    {
        removeAllAnimations();
    }
    //------------------------------------------------------------------------
    AnimationObjID AnimationObj::getAniObjID() const
    {
        return mAniObjId;
    }
    //------------------------------------------------------------------------
    Animation * AnimationObj::createAnimation(AnimationID id, TimeDurMS dur)
    {
        if(mAnimations.find(id) != mAnimations.end())
        {
            N_EXCEPT(UniqueRepeat, _I18n("动画已经存在,动画ID: ") + N_conv(id));
        }
        Animation * re = N_new Animation(id, dur, ANIM_None);
        mAnimations[id] = re;
        return re;
    }
    //------------------------------------------------------------------------
    void AnimationObj::removeAnimation(AnimationID id)
    {
        AnimationList::iterator i = mAnimations.find(id);
        if(i != mAnimations.end())
        {
            N_delete i->second;
            mAnimations.erase(i);
        }
    }
    //------------------------------------------------------------------------
    void AnimationObj::removeAllAnimations()
    {
        AnimationList::iterator i, iend = mAnimations.end();
        for(i = mAnimations.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
        mAnimations.clear();
    }
    //------------------------------------------------------------------------
    Animation * AnimationObj::getAnimation(Nidx index) const
    {
        N_assert(index < mAnimations.size(), "error");
        AnimationList::const_iterator i = mAnimations.begin();
        std::advance(i, index);
        return i->second;
    }
    //------------------------------------------------------------------------
    Animation * AnimationObj::getAnimation(AnimationID id) const
    {
        Animation * re = 0;
        AnimationList::const_iterator i = mAnimations.find(id);
        if(i != mAnimations.end())
        {
            re = i->second;
        }
        return re;
    }
    //------------------------------------------------------------------------
    Animation * AnimationObj::getAnimationPop(AnimationID id)
    {
        Animation * re = 0;
        AnimationList::const_iterator i = mAnimations.find(id);
        if (i != mAnimations.end())
        {
            re = i->second;
            mAnimations.erase(i);
        }
        return re;
    }
    //------------------------------------------------------------------------
    bool AnimationObj::isAnimationExist(AnimationID id) const
    {
        return getAnimation(id) != 0;
    }
    //------------------------------------------------------------------------
    bool AnimationObj::isVertexFusion() const
    {
        return false;
    }
    //------------------------------------------------------------------------
    bool AnimationObj::isNormlFusion() const
    {
        return false;
    }
    //------------------------------------------------------------------------
    bool AnimationObj::isSkeletonFusion() const
    {
        return false;
    }
    //------------------------------------------------------------------------
    void AnimationObj::_init(AnimationFusionObj * obj)
    {
        (void)obj;
    }
    //------------------------------------------------------------------------
    void AnimationObj::_append(AnimationFusionObj * obj)
    {
        (void)obj;
    }
    //------------------------------------------------------------------------
    void AnimationObj::_refresh(AnimationFusionObj * obj)
    {
        (void)obj;
    }
    //------------------------------------------------------------------------
    void AnimationObj::_update(AnimationFusionObj * obj)
    {
        (void)obj;
    }
    //------------------------------------------------------------------------
    void AnimationObj::optimise()
    {
    }
    //------------------------------------------------------------------------
    NCount AnimationObj::getAnimationCount() const
    {
        return mAnimations.size();
    }
    //------------------------------------------------------------------------
}