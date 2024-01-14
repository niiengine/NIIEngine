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
#include "NiiModulativeStencilRenderPattern.h"
#include "NiiShadowRenderQueue.h"
#include "NiiShadowRenderTest.h"
#include "NiiShadowObj.h"
#include "NiiViewRectGeo.h"
#include "NiiLight.h"
#include "NiiViewPort.h"
#include "NiiSpaceManager.h"

namespace NII
{
    //------------------------------------------------------------------------
    ShaderChStencil additiveStencil1(false, 0xFFFFFFFF, 0xFFFFFFFF, CPM_EQUAL, 0);
    ShaderChStencil additiveStencil2(false);
    ShaderChStencil modulativeStencil1(false, 0xFFFFFFFF, 0xFFFFFFFF, CPM_NOT_EQUAL, 0);
    ShaderChStencil modulativeStencil2(false);
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // AdditiveStencilRenderPattern
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    AdditiveStencilRenderPattern::AdditiveStencilRenderPattern(SpaceManager * sm, 
        RenderSys * rsys, SysSyncParam * sparam) :
        StencilRenderPattern(sm, rsys, sparam)
    {
    }
    //------------------------------------------------------------------------
    void AdditiveStencilRenderPattern::prepare(RenderQueue * queue)
    {
        ShadowRenderQueue * temp = static_cast<ShadowRenderQueue *>(queue);
        temp->setCastersNotReceivers(false);

        if(mViewPort->isShadowEnable())
        {
            temp->setSplitLightingType(true);
            temp->setSplitNoShadow(true);
        }
        else
        {
            temp->setSplitLightingType(false);
            temp->setSplitNoShadow(false);
        }
    }
    //------------------------------------------------------------------------
    void AdditiveStencilRenderPattern::prepare(RenderGroup * group)
    {
        ShadowRenderGroup * temp = static_cast<ShadowRenderGroup *>(group);
        temp->setCastersNotReceivers(false);

        if(mShadowEnable && mViewPort->isShadowEnable())
        {
            temp->setSplitLightingType(true);
            temp->setSplitNoShadow(true);
        }
        else
        {
            temp->setSplitLightingType(false);
            temp->setSplitNoShadow(false);
        }
    }
    //------------------------------------------------------------------------
    void AdditiveStencilRenderPattern::renderImpl(RenderGroup * pGroup)
    {
        if(pGroup->isShadowEnable())
        {
            LightList lightList;
            RenderGroup::LevelList::const_iterator z, zend = pGroup->getLevelList().end();
            for(z = pGroup->getLevelList().begin(); z != zend; ++z)
            {
                ShadowRenderLevelGroup * rpg = static_cast<ShadowRenderLevelGroup *>(z->second);

                rpg->sort(mCamera);

                lightList.clear();

                mTest->setParam(false, false, false, &mNullLight);
                rpg->render(ST_BasicMaterial, mTest);

                mTest->setParam(false, true, true, &mNullLight);
                rpg->getSolidsNoShadowReceive().render(mTest, RSM_Material);

                LightList::const_iterator li, liend = mParent->getLightList().end();
                for(li = mParent->getLightList().begin(); li != liend; ++li)
                {
                    Light * l = *li;
                    
                    if(lightList.empty())
                        lightList.push_back(l);
                    else
                        lightList[0] = l;

                    // 建立裁剪,覆盖阴影体积和常规灯光渲染
                    SpaceManager::ClipType scissored = mParent->applyClip(lightList, mCamera);
                    SpaceManager::ClipType clipped = SpaceManager::CT_None;
                    if(mLightClipPlane)
                        clipped = mParent->applyPlaneClip(lightList);

                    // 如果整个 裁减/修剪 ,跳过灯光
                    if(scissored == SpaceManager::CT_All || clipped == SpaceManager::CT_All)
                        continue;

                    if(l->isCastShadow())
                    {
                        mRenderSys->clearBuffer(FBT_STENCIL);
                        renderStencil(l, mCamera, false);

                        mRenderSys->setStencilTest(true);
                        // 注意,我们渲染的灯光区域的模版等于零
                        mRenderSys->setStencil(additiveStencil1, SB_Stencil);
                    }

                    // 为这个灯光渲染照明通道
                    mTest->setParam(false, false, false, &lightList);
                    rpg->getSolidsDiffuseSpecular().render(mTest, RSM_Material);

                    // 重置蒙板参数
                    mRenderSys->setStencilTest(false);
                    mRenderSys->setStencil(additiveStencil2, SB_Stencil);
                    mRenderSys->setStencil(ShaderChStencil(), SB_Depth);

                    if(scissored == SpaceManager::CT_Some)
                        mParent->resetClip();
                    if(clipped == SpaceManager::CT_Some)
                        mParent->resetPlaneClip();

                }// 每个灯光

                // 现在渲染贴花通道, 不需要设置灯光,因为禁止照明
                mTest->setParam(false, false, false, &mNullLight);
                rpg->getSolidsDecal().render(mTest, RSM_Material);
            }

            zend = pGroup->getLevelList().end();
            for(z = pGroup->getLevelList().begin(); z != zend; ++z)
            {
                RenderLevelGroup * rpg = z->second;

                mTest->setParam(false, true, true, &mNullLight);
                rpg->render(ST_AlphaMaterial, mTest);

                mTest->setParam(false, true, true, &mNullLight);
                rpg->render(ST_AlphaViewDescend, mTest);
            }
        }
        else
        {
            renderBasic(pGroup);
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ModulativeStencilRenderPattern
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ModulativeStencilRenderPattern::ModulativeStencilRenderPattern(SpaceManager * sm,
        RenderSys * rsys, SysSyncParam * param):
            StencilRenderPattern(sm, rsys, param)
    {
    }
    //------------------------------------------------------------------------
    void ModulativeStencilRenderPattern::prepare(RenderQueue * queue)
    {
        ShadowRenderQueue * temp = static_cast<ShadowRenderQueue *>(queue);
        temp->setCastersNotReceivers(false);

        temp->setSplitLightingType(false);

        if(mViewPort->isShadowEnable())
        {
            temp->setSplitNoShadow(true);
        }
        else
        {
            temp->setSplitNoShadow(false);
        }
    }
    //------------------------------------------------------------------------
    void ModulativeStencilRenderPattern::prepare(RenderGroup * group)
    {
        ShadowRenderGroup * temp = static_cast<ShadowRenderGroup *>(group);
        temp->setCastersNotReceivers(false);

        temp->setSplitLightingType(false);

        if(mShadowEnable && mViewPort->isShadowEnable())
        {
            temp->setSplitNoShadow(true);
        }
        else
        {
            temp->setSplitNoShadow(false);
        }
    }
    //------------------------------------------------------------------------
    void ModulativeStencilRenderPattern::renderImpl(RenderGroup * pGroup)
    {
        if(pGroup->isShadowEnable())
        {
            /* 为每个灯光,我们需要渲染来自每个组的固体,然后处理调制阴影,之后渲染每个组的
                透明物。现在,意味着我们将要物体的排序,但如果阴影需要看上去正确的话。为了
                保留整体的排序,仅是所有透明物在尾部,而不是交错在正常渲染循环中
            */
            // 遍历优先级
            RenderGroup::LevelList::const_iterator z, zend = pGroup->getLevelList().end();
            for(z = pGroup->getLevelList().begin(); z != zend; ++z)
            {
                RenderLevelGroup * rpg = z->second;

                rpg->sort(mCamera);

                mTest->setParam(false, true, true, &mNullLight);
                rpg->render(ST_BasicMaterial, mTest);
            }

            // 渲染所有体积
            LightList::const_iterator li, liend = mParent->getLightList().end();
            for(li = mParent->getLightList().begin(); li != liend; ++li)
            {
                Light * l = *li;
                if(l->isCastShadow())
                {
                     mRenderSys->clearBuffer(FBT_STENCIL);
                     renderStencil(l, mCamera, true);

                     mParent->applyCh(mModulateCh, false, true);

                     mRenderSys->setStencilTest(true);
                     mRenderSys->setStencil(modulativeStencil1, SB_Stencil);
                     mParent->render(mScreen, mModulateCh, LightList(), false);
                     
                     mRenderSys->setStencilTest(false);
                     mRenderSys->setStencil(modulativeStencil2, SB_Stencil);
                     mRenderSys->setStencil(ShaderChStencil(), SB_Depth);
                }
            }

            zend = pGroup->getLevelList().end();
            for(z = pGroup->getLevelList().begin(); z != zend; ++z)
            {
                ShadowRenderLevelGroup * rpg = static_cast<ShadowRenderLevelGroup *>(z->second);

                mTest->setParam(false, true, true, &mNullLight);
                rpg->getSolidsNoShadowReceive().render(mTest, RSM_Material);
            }

            z, zend = pGroup->getLevelList().end();
            for(z = pGroup->getLevelList().begin(); z != zend; ++z)
            {
                RenderLevelGroup * rpg = z->second;

                mTest->setParam(false, true, true, &mNullLight);
                rpg->render(ST_AlphaMaterial, mTest);

                mTest->setParam(false, true, true, &mNullLight);
                rpg->render(ST_AlphaViewDescend, mTest);
            }
        }
        else
        {
            renderBasic(pGroup);
        }
    }
    //---------------------------------------------------------------------------------
    void ModulativeStencilRenderPattern::applyDarkCap(const Light * lig, const Camera * camera,
        ShadowObj * caster, NIIf extrude, Nmark & flags)
    {
        // 如下,在zpass中需要一个暗罩
        /*  1:点/聚灯光在调制阴影模式中无限挤压,否则在没有深度(skybox)的区
            域,无限投影阴影将流一个暗带
            2:在任何灯光资源有限挤出中,因为查看角度检测结束和阴影对象后面是
            不正确的
        */
        if((flags & ShadowObj::VT_Infinity) && lig->getType() != LT_DIRECTIONAL &&
            camera->isIntersect(caster->getBackCapAABB(*lig, extrude)))
        {
            flags |= ShadowObj::VT_BackCap;
        }
        else if (!(flags & ShadowObj::VT_Infinity) &&
            camera->isIntersect(caster->getBackCapAABB(*lig, extrude)))
        {
            flags |= ShadowObj::VT_BackCap;
        }
    }
    //---------------------------------------------------------------------------------
}