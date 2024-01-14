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

                    // �����ü�,������Ӱ����ͳ���ƹ���Ⱦ
                    SpaceManager::ClipType scissored = mParent->applyClip(lightList, mCamera);
                    SpaceManager::ClipType clipped = SpaceManager::CT_None;
                    if(mLightClipPlane)
                        clipped = mParent->applyPlaneClip(lightList);

                    // ������� �ü�/�޼� ,�����ƹ�
                    if(scissored == SpaceManager::CT_All || clipped == SpaceManager::CT_All)
                        continue;

                    if(l->isCastShadow())
                    {
                        mRenderSys->clearBuffer(FBT_STENCIL);
                        renderStencil(l, mCamera, false);

                        mRenderSys->setStencilTest(true);
                        // ע��,������Ⱦ�ĵƹ������ģ�������
                        mRenderSys->setStencil(additiveStencil1, SB_Stencil);
                    }

                    // Ϊ����ƹ���Ⱦ����ͨ��
                    mTest->setParam(false, false, false, &lightList);
                    rpg->getSolidsDiffuseSpecular().render(mTest, RSM_Material);

                    // �����ɰ����
                    mRenderSys->setStencilTest(false);
                    mRenderSys->setStencil(additiveStencil2, SB_Stencil);
                    mRenderSys->setStencil(ShaderChStencil(), SB_Depth);

                    if(scissored == SpaceManager::CT_Some)
                        mParent->resetClip();
                    if(clipped == SpaceManager::CT_Some)
                        mParent->resetPlaneClip();

                }// ÿ���ƹ�

                // ������Ⱦ����ͨ��, ����Ҫ���õƹ�,��Ϊ��ֹ����
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
            /* Ϊÿ���ƹ�,������Ҫ��Ⱦ����ÿ����Ĺ���,Ȼ���������Ӱ,֮����Ⱦÿ�����
                ͸�������,��ζ�����ǽ�Ҫ���������,�������Ӱ��Ҫ����ȥ��ȷ�Ļ���Ϊ��
                �������������,��������͸������β��,�����ǽ�����������Ⱦѭ����
            */
            // �������ȼ�
            RenderGroup::LevelList::const_iterator z, zend = pGroup->getLevelList().end();
            for(z = pGroup->getLevelList().begin(); z != zend; ++z)
            {
                RenderLevelGroup * rpg = z->second;

                rpg->sort(mCamera);

                mTest->setParam(false, true, true, &mNullLight);
                rpg->render(ST_BasicMaterial, mTest);
            }

            // ��Ⱦ�������
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
        // ����,��zpass����Ҫһ������
        /*  1:��/�۵ƹ��ڵ�����Ӱģʽ�����޼�ѹ,������û�����(skybox)����
            ��,����ͶӰ��Ӱ����һ������
            2:���κεƹ���Դ���޼�����,��Ϊ�鿴�Ƕȼ���������Ӱ���������
            ����ȷ��
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