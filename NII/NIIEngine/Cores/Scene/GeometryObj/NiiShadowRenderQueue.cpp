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
#include "NiiShadowRenderQueue.h"
#include "NiiMaterial.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ShadowRenderLevelGroup
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ShadowRenderLevelGroup::ShadowRenderLevelGroup(RenderGroup * parent, RenderCommandGroup * dcg,
        bool splitLightingType, bool splitNoShadow, bool castNoReceive) :
            RenderLevelGroup(parent, dcg),
            mSplitLightingType(splitLightingType),
            mSplitNoShadow(splitNoShadow),
            mCastersNotReceivers(castNoReceive)
    {
    }
    //-----------------------------------------------------------------------
    void ShadowRenderLevelGroup::addSort(RenderSortMode om)
    {
        RenderLevelGroup::addSort(om);
        mSolidsDecal.addSort(om);
        mSolidsDiffuseSpecular.addSort(om);
        mSolidsNoShadowReceive.addSort(om);
    }
    //-----------------------------------------------------------------------
    void ShadowRenderLevelGroup::removeSort(RenderSortMode om)
    {
        RenderLevelGroup::removeSort(om);
        mSolidsDecal.removeSort(om);
        mSolidsDiffuseSpecular.removeSort(om);
        mSolidsNoShadowReceive.removeSort(om);
    }
    //-----------------------------------------------------------------------
    void ShadowRenderLevelGroup::add(SpaceObj * sobj, GeometryObj * gobj, ShaderFusion * sf)
    {
        if(sf->isForceDepthSort() || (sf->isTransparent() &&
            (!sf->isDepthWrite() || !sf->isDepthEnable() || (sf->getColourWrite() != 0))))
        {
            mAlpha.add(sobj, gobj, sf);
        }
        else
        {
            if(mSplitNoShadow && mParent->isShadowEnable() &&
                ((!sf->getParent()->isReceiveShadow() ||
                    gobj->isCastShadow()) && mCastersNotReceivers))
            {
                mSolidsNoShadowReceive.add(sobj, gobj, sf);
            }
            else
            {
                if(mSplitLightingType && mParent->isShadowEnable())
                {
                    ShaderFusion::FusionShaderList::iterator i, iend = sf->getFusionShaderList().end();
                    for(i = sf->getFusionShaderList().begin(); i != iend; ++i)
                    {
                        ShaderFusion::Fusion * fusion = *i;
                        switch(fusion->mStep)
                        {
                        case SS_Global:
                            mBasic.add(sobj, gobj, fusion->mDest);
                            break;
                        case SS_Detail_Light:
                            mSolidsDiffuseSpecular.add(sobj, gobj, fusion->mDest);
                            break;
                        case SS_Decal:
                            mSolidsDecal.add(sobj, gobj, fusion->mDest);
                            break;
                        default:
                            N_assert(false, "error");
                        };
                    }
                }
                else
                {
                    mBasic.add(sobj, gobj, sf);
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ShadowRenderLevelGroup::remove(ShaderCh * p)
    {
        RenderLevelGroup::remove(p);
        mSolidsDecal.remove(p);
        mSolidsDiffuseSpecular.remove(p);
        mSolidsNoShadowReceive.remove(p);
    }
    //-----------------------------------------------------------------------
    void ShadowRenderLevelGroup::clear()
    {
        RenderLevelGroup::clear();
        mSolidsDecal.clear();
        mSolidsDiffuseSpecular.clear();
        mSolidsNoShadowReceive.clear();
    }
    //-----------------------------------------------------------------------
    void ShadowRenderLevelGroup::sort(const Camera * cam)
    {
        RenderLevelGroup::sort(cam);
        mSolidsDecal.sort(cam);
        mSolidsDiffuseSpecular.sort(cam);
        mSolidsNoShadowReceive.sort(cam);
    }
    //-----------------------------------------------------------------------
    void ShadowRenderLevelGroup::merge(const RenderLevelGroup * o)
    {
        const ShadowRenderLevelGroup * temp = static_cast<const ShadowRenderLevelGroup *>(o);
        RenderLevelGroup::merge(temp);
        mSolidsDecal.merge(temp->mSolidsDecal);
        mSolidsDiffuseSpecular.merge(temp->mSolidsDiffuseSpecular);
        mSolidsNoShadowReceive.merge(temp->mSolidsNoShadowReceive);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ShadowRenderGroup
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ShadowRenderGroup::ShadowRenderGroup(RenderQueue * parent, bool lightingType, 
        bool splitNoShadow, bool castNoReceive) :
            RenderGroup(parent),
            mSplitLightingType(lightingType),
            mSplitNoShadow(splitNoShadow),
            mCastersNotReceivers(castNoReceive)
    {
    }
    //-----------------------------------------------------------------------
    void ShadowRenderGroup::setSplitLightingType(bool b)
    {
        mSplitLightingType = b;
        LevelList::iterator i, iend = mLevelList.end();
        for (i = mLevelList.begin(); i != iend; ++i)
        {
            static_cast<ShadowRenderLevelGroup *>(i->second)->setSplitLightingType(b);
        }
    }
    //-----------------------------------------------------------------------
    void ShadowRenderGroup::setSplitNoShadow(bool b)
    {
        mSplitNoShadow = b;
        LevelList::iterator i, iend = mLevelList.end();
        for(i = mLevelList.begin(); i != iend; ++i)
        {
            static_cast<ShadowRenderLevelGroup *>(i->second)->setSplitNoShadow(b);
        }
    }
    //-----------------------------------------------------------------------
    void ShadowRenderGroup::setCastersNotReceivers(bool b)
    {
        mCastersNotReceivers = b;
        LevelList::iterator i, iend = mLevelList.end();
        for(i = mLevelList.begin(); i != iend; ++i)
        {
            static_cast<ShadowRenderLevelGroup *>(i->second)->setCastersNotReceivers(b);
        }
    }
    //------------------------------------------------------------------------
    void ShadowRenderGroup::createImpl(RenderLevelGroup *& out)
    {
        out = N_new ShadowRenderLevelGroup(this, mSplitLightingType,
            mSplitNoShadow, mCastersNotReceivers);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ShadowRenderQueue
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ShadowRenderQueue::ShadowRenderQueue(RenderCommandGroup * dcg):
        RenderQueue(dcg),
        mSplitNoShadow(false),
        mSplitLightingType(false),
        mCastersNotReceivers(false)
    {
    }
    //-----------------------------------------------------------------------
    void ShadowRenderQueue::setSplitLightingType(bool b)
    {
        mSplitLightingType = b;

        GroupList::iterator i, iend = mGroups.end();
        for(i = mGroups.begin(); i != iend; ++i)
        {
            if(i->second != 0)
                static_cast<ShadowRenderGroup *>(i->second)->setSplitLightingType(b);
        }
    }
    //-----------------------------------------------------------------------
    void ShadowRenderQueue::setSplitNoShadow(bool b)
    {
        mSplitNoShadow = b;

        GroupList::iterator i, iend = mGroups.end();
        for(i = mGroups.begin(); i != iend; ++i)
        {
            if(i->second != 0)
                static_cast<ShadowRenderGroup *>(i->second)->setSplitNoShadow(b);
        }
    }
    //-----------------------------------------------------------------------
    void ShadowRenderQueue::setCastersNotReceivers(bool b)
    {
        mCastersNotReceivers = b;

        GroupList::iterator i, iend = mGroups.end();
        for(i = mGroups.begin(); i != iend; ++i)
        {
            if(i->second != 0)
                static_cast<ShadowRenderGroup *>(i->second)->setCastersNotReceivers(b);
        }
    }
    //------------------------------------------------------------------------
    void ShadowRenderQueue::createImpl(RenderGroup *& out)
    {
        out = N_new ShadowRenderGroup(this, mSplitLightingType, mSplitNoShadow, mCastersNotReceivers);
    }
    //------------------------------------------------------------------------
    void ShadowRenderQueue::init()
    {
        if(mGroups[RQG_User].second == 0)
        {
            mGroups[RQG_User].second = N_new ShadowRenderGroup(this,
                mSplitLightingType, mSplitNoShadow, mCastersNotReceivers);
            mGroups[RQG_User].setLevel(mSceneManager->getLevelCount());
            mGroups[RQG_User].setRenderType(RT_Queue);
        }
    }
    //------------------------------------------------------------------------
}