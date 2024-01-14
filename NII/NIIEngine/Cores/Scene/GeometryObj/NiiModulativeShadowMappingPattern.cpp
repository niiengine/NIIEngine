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
#include "NiiModulativeShadowMappingPattern.h"
#include "NiiShadowRenderQueue.h"
#include "NiiShadowRenderTest.h"
#include "NiiShaderChProgram.h"
#include "NiiViewPort.h"
#include "NiiTexture.h"
#include "NiiGpuBuffer.h"
#include "NiiTextureFrame.h"
#include "NiiSpaceManager.h"
#include "NiiSysSyncParam.h"
#include "NiiGBufferManager.h"
#include "NiiMaterial.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // IntegrateShadowMapping
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    IntegrateShadowMapping::IntegrateShadowMapping(SpaceManager * parent, RenderSys * rsys, SysSyncParam * param) :
        ShadowMappingPattern(parent, rsys, param)
    {
    }
    //------------------------------------------------------------------------
    void IntegrateShadowMapping::prepare(RenderQueue * queue)
    {
        ShadowRenderQueue * temp = static_cast<ShadowRenderQueue *>(queue);
        temp->setCastersNotReceivers(!mSelfShadow);

        temp->setSplitLightingType(false);

        temp->setSplitNoShadow(false);
    }
    //------------------------------------------------------------------------
    void IntegrateShadowMapping::prepare(RenderGroup * group)
    {
        ShadowRenderGroup * temp = static_cast<ShadowRenderGroup *>(group);
        temp->setCastersNotReceivers(!mSelfShadow);

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
    //-------------------------------------------------------------------------
    void IntegrateShadowMapping::renderImpl(RenderGroup * rg)
    {
        renderBasic(rg);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // AdditiveShadowMappingPattern
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    AdditiveShadowMappingPattern::AdditiveShadowMappingPattern(SpaceManager * parent, 
        RenderSys * rsys, SysSyncParam * param) :
        ShadowMappingPattern(parent, rsys, param),
        mShadowColour(Colour::Black)
    {
    }
    //------------------------------------------------------------------------
    void AdditiveShadowMappingPattern::prepare(RenderQueue * queue)
    {
        ShadowRenderQueue * temp = static_cast<ShadowRenderQueue *>(queue);
        temp->setCastersNotReceivers(!mSelfShadow);

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
    void AdditiveShadowMappingPattern::prepare(RenderGroup * group)
    {
        ShadowRenderGroup * temp = static_cast<ShadowRenderGroup *>(group);
        temp->setCastersNotReceivers(!mSelfShadow);

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
    void AdditiveShadowMappingPattern::renderImpl(RenderGroup * pGroup)
    {
        // 使用调制蒙板阴影
        if(mRenderStage == ShadowType::ST_Cast)
        {
            renderTexture(pGroup, RSM_Material);
        }
        else if(pGroup->isShadowEnable())
        {
            LightList lightList;
            RenderGroup::LevelList::const_iterator z, zend = pGroup->getLevelList().end();
            for(z = pGroup->getLevelList().begin(); z != zend; ++z)
            {
                ShadowRenderLevelGroup * rpg = static_cast<ShadowRenderLevelGroup *>(z->second);

                rpg->sort(mCamera);

                lightList.clear();

                // 首先渲染所有环境通道,不灯光迭代,无灯光
                mTest->setParam(false, false, false, &mNullLight);
                rpg->render(ST_BasicMaterial, mTest);

                // 也渲染禁止接收阴影的对象
                mTest->setParam(false, true, true, &mNullLight);
                rpg->getSolidsNoShadowReceive().render(mTest, RSM_Material);

                // 执行下一部分,如果我们在"正常"渲染阶段的话,避免在渲染到纹理期间处理这个
                if(mRenderStage == ST_Normal)
                {
                    LightList::const_iterator li, liend = mParent->getLightList().end();
                    TextureList::iterator si = mTextures.begin(), siend = mTextures.end();

                    for(li = mParent->getLightList().begin(); li != liend; ++li)
                    {
                        Light * l = *li;

                        if(l->isCastShadow() && si != siend)
                        {
                            // 存储当前阴影纹理
                            mCurrentTexture = *si;
                            // 为当前阴影纹理,获取摄象机
                            Camera * cam = mCurrentTexture->getBuffer()->getBuffer()->
                                getViewport(0)->getCamera();
                            // 接通接收纹理
                            ShaderCh * targetPass = mSTCRP ? mSTCRP : mReceive;
                            ResourceIDList rlist;
                            rlist.push_back(mCurrentTexture->getOriginID());
                            targetPass->setTexture(ShaderCh::SOT_Set)->get(0)->setSrc(rlist, Texture::T_2D);
                            // 如果是固定函数管线,连接投影平截头,当着色程序管线时要明确禁用
                            ShaderChTextureUnit * texUnit = targetPass->setTexture(ShaderCh::SOT_Set)->get(0);
                            texUnit->setProjTexture(!targetPass->getProgram().isExist(ShaderChProgram::ST_VS));
                            texUnit->setProjFrustum(cam);
                            // 假使这是一个自定义材质,夹住边框颜色
                            texUnit->getSample()->setMode(TAM_BORDER, TAM_BORDER, TAM_BORDER);
                            texUnit->getSample()->setBorder(Colour::White);

                            mSyncParams->setTextureProjector(cam, 0);

                            // 移去任何斑点衰减层
                            if(targetPass->getTexture().getCount() > 1 &&
                                targetPass->getTexture().get(1)->getResourceID() == N_SpotShadowFade)
                            {
                                // 移去斑点衰减层(仅当我们前一 个使用了调制阴影,才在这里)
                                targetPass->setTexture(ShaderCh::SOT_Set)->remove(1);
                            }
                            // 设置 照明/混合模式
                            targetPass->setBlend(ShaderCh::SOT_Set)->setBlend(CF_ONE, CF_ONE);
                            targetPass->setLight(true);
                            targetPass->load();

                            // 因为使用,增加阴影纹理
                            ++si;
                            // 改变渲染状态(接收阴影)
                            mRenderStage = ShadowType::ST_Receive;
                        }
                        else
                        {   // 完成渲染状态(普通)
                            mRenderStage = ShadowType::ST_Normal;
                        }

                        // 为这个灯光渲染照明通道
                        if(lightList.empty())
                            lightList.push_back(l);
                        else
                            lightList[0] = l;

                        // 建立灯光裁减,对添加型模式总是有用的
                        SpaceManager::ClipType scissored = mParent->applyClip(lightList, mCamera);
                        SpaceManager::ClipType clipped = SpaceManager::CT_None;
                        if(mLightClipPlane)
                            clipped = mParent->applyPlaneClip(lightList);
                        // 如果整个修剪,跳过它
                        if(scissored == SpaceManager::CT_All || clipped == SpaceManager::CT_All)
                            continue;

                        mTest->setParam(false, false, false, &lightList);
                        rpg->getSolidsDiffuseSpecular().render(mTest, RSM_Material);

                        if(scissored == SpaceManager::CT_Some)
                            mParent->resetClip();
                        if(clipped == SpaceManager::CT_Some)
                            mParent->resetPlaneClip();

                    }// 每个灯光

                    mRenderStage = ST_Normal;

                    mTest->setParam(false, false, false, &mNullLight);
                    rpg->getSolidsDecal().render(mTest, RSM_Material);
                    // 现在渲染贴花通道,不需要设置灯光,因为禁用照明
                }
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
    void AdditiveShadowMappingPattern::setShadowColour(const Colour & c)
    {
        mShadowColour = Colour::Black;
    }
    //------------------------------------------------------------------------
    void AdditiveShadowMappingPattern::setTextureFade(NIIf begin, NIIf end)
    {
        // 附加型灯光不应该使用雾,因为可能太过明亮,使用border clamp 明确雾禁用
        ShaderChFog temp(FM_NONE, Colour::White, 0.001f, 0.0, 1.0);
        mReceive->setFog(ShaderCh::SOT_Set, &temp);
    }
    //------------------------------------------------------------------------
    ShaderCh * AdditiveShadowMappingPattern::deriveReceiver(const ShaderCh * src)
    {
        ShaderCh * re;
        if(src->getParent()->getShadowReceiver() != 0)
        {
            return src->getParent()->getShadowReceiver()->getLOD()->get(0);
        }
        else
        {
            re = mSTCRP ? mSTCRP : mReceive;
        }

        // 引入的通道有一个自定义阴影接收程序?
        GpuProgramParam * param;
        GpuProgram * prg;

        src->getProgram().get(ShaderChProgram::ST_VS_SR, prg, param);
        if(prg != 0)
        {
            re->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS, prg->getOriginID(), param);
            //re->getProgram().get(ShaderChProgram::ST_VS, prg);
            if(prg->getState() != LS_LOADED)
                prg->load();
            // 也处理灯光自动参数,迟一点
        }
        else
        {
            if(re != mSTCRP)
            {
                // 标准阴影接收通道,重置为无 vp
                re->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS, 0, 0);
            }
        }

        *(re->setColour(ShaderCh::SOT_Set)) = src->getColour();

        re->setLight(true);
        re->setEachLight(src->isEachLight());
        re->setLightTypeLimit(src->getLightTypeLimit());
        re->setLightMark(src->getLightMark());

        re->setColour(ShaderCh::SOT_Set)->setAlphaReject(src->getColour().getAlphaReject());
        re->setColour(ShaderCh::SOT_Set)->setValue(src->getColour().getValue());
        re->setColour(ShaderCh::SOT_Set)->setAlphaCoverage(false);
        // 复制纹理状态,上移一个因为retpass的0是阴影纹理
        NCount origPassTUCount = src->getTexture().getCount();
        for(Nidx t = 0; t < origPassTUCount; ++t)
        {
            ShaderChTextureUnit * tex;
            Nidx16 targetIndex = t + 1;
            if(re->getTexture().getCount() <= targetIndex)
            {
                tex = re->setTexture(ShaderCh::SOT_Set)->create();
            }
            else
            {
                tex = re->setTexture(ShaderCh::SOT_Set)->get(targetIndex);
            }
            (*tex) = *(src->getTexture().get(t));
            // 如果是可编程的,必须调整纹理坐标,D3D坚持纹理坐标设置在可编程模式中匹配纹理单元
            if(re->getProgram().isExist(ShaderChProgram::ST_VS))
                tex->setCoordSet(targetIndex);
        }
        NCount keepTUCount = origPassTUCount + 1;
        while(re->getTexture().getCount() > keepTUCount)
        {
            re->setTexture(ShaderCh::SOT_Set)->remove(keepTUCount);
        }

        src->getProgram().get(ShaderChProgram::ST_FS_SR, prg, param);
        if(prg != 0)
        {
            re->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS, prg->getOriginID(), param);
            //re->getProgram().get(ShaderChProgram::ST_FS, prg);
            if(prg->getState() != LS_LOADED)
                prg->load();

            // 绑定了一个阴影顶点着色程序?
            if(src->getProgram().isExist(ShaderChProgram::ST_VS) && !re->getProgram().isExist(ShaderChProgram::ST_VS))
            {
                // 不绑定接收指定着色程序, 因此绑定原始的
                src->getProgram().get(ShaderChProgram::ST_VS, prg, param);
                re->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS, prg->getOriginID(), param);
                //re->getProgram().get(ShaderChProgram::ST_VS, prg);
                if(prg->getState() != LS_LOADED)
                    prg->load();
            }
        }
        else
        {
            // 从最后一次中,重置任何合并的片断着色程序
            if(re != mSTCRP)
            {
                // 标准阴影接收通道,无fs
                re->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS, 0, 0);
            }
        }

        re->load();
        // handle the case where there is no fixed pipeline support
        re->getParent()->getParent()->check();
        ShaderFusion * best = re->getParent()->getParent()->getLOD();
        if(best)
        {
            re = best->get(0);
        }

        return re;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ModulativeShadowMappingPattern
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ModulativeShadowMappingPattern::ModulativeShadowMappingPattern(SpaceManager * parent, 
        RenderSys * rsys, SysSyncParam * param) :
        ShadowMappingPattern(parent, rsys, param)
    {
    }
    //------------------------------------------------------------------------
    void ModulativeShadowMappingPattern::prepare(RenderQueue * queue)
    {
        ShadowRenderQueue * temp = static_cast<ShadowRenderQueue *>(queue);
        temp->setCastersNotReceivers(!mSelfShadow);

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
    void ModulativeShadowMappingPattern::prepare(RenderGroup * group)
    {
        ShadowRenderGroup * temp = static_cast<ShadowRenderGroup *>(group);
        temp->setCastersNotReceivers(!mSelfShadow);

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
    bool ModulativeShadowMappingPattern::testRender(const ShaderCh * ch)
    {
        /* 如果我们处理一个纹理阴影渲染和这个通道,而这个通道在第一个后面(阴影纹理渲染只
            需要1个通道,调制技术的阴影纹理接收也需要1个通道)绕过它,如果通道在第一个通道
            之后,如果渲染状态禁止改变,因为我们实际没有使用这个通道的数据,也绕过它
        */
        if(mShadowEnable && mViewPort->isShadowEnable() &&
            (mRenderStage == ShadowType::ST_Receive || mRenderStage == ShadowType::ST_Cast ||
            mParent->isLockShaderCh()) && ch->getIndex() > 0)
        {
            return false;
        }
        return ShadowMappingPattern::testRender(ch);
    }
    //------------------------------------------------------------------------
    bool ModulativeShadowMappingPattern::testRender(const ShaderCh * ch, const GeometryObj * obj)
    {
        /* 如果我们不是处理调制阴影纹理,跳过这个可渲染物。它投射阴影,我们
            处理渲染接收通道和不是自身阴影,通道>0也是这样
        */
        if(mShadowEnable && mViewPort->isShadowEnable())
        {
            // 为透明的,test的一些重复使用
            if((mRenderStage == ShadowType::ST_Receive || mRenderStage == ShadowType::ST_Cast ||
                mParent->isLockShaderCh()) && ch->getIndex() > 0)
            {
                return false;
            }
        }
        return ShadowMappingPattern::testRender(ch, obj);
    }
    //------------------------------------------------------------------------
    void ModulativeShadowMappingPattern::renderImpl(RenderGroup * pGroup)
    {
        // 使用调制蒙板阴影
        if(mRenderStage == ShadowType::ST_Cast)
        {
            renderTexture(pGroup, RSM_Material);
        }
        else if(pGroup->isShadowEnable())
        {
            /* 为每个灯光,我们需要渲染来自每个组的固体,然后处理调制阴影,之后渲染每个组的透
                明物。现在,意味着我们将要物体的排序,但如果阴影需要看上去真确的话。为了保留
                整体的排序,仅是所有透明物在尾部,而不是交错在正常渲染循环中
            */
            // 遍历优先级
            RenderGroup::LevelList::const_iterator z, zend = pGroup->getLevelList().end();
            for(z = pGroup->getLevelList().begin(); z != zend; ++z)
            {
                ShadowRenderLevelGroup * rpg = static_cast<ShadowRenderLevelGroup *>(z->second);

                rpg->sort(mCamera);

                // 处理固体
                mTest->setParam(false, true, true, &mNullLight);
                rpg->render(ST_BasicMaterial, mTest);

                mTest->setParam(false, true, true, &mNullLight);
                rpg->getSolidsNoShadowReceive().render(mTest, RSM_Material);
            }

            // 迭代灯光,如果我们在一个"正常"渲染阶段才执行接收阴影,在渲染到纹理阶段中避免处理这个
            if(mRenderStage == ST_Normal)
            {
                mRenderStage = ShadowType::ST_Receive;

                LightList::const_iterator i, iend = mParent->getLightList().end();
                TextureList::iterator si, siend = mTextures.end();
                for(i = mParent->getLightList().begin(), si = mTextures.begin();
                    i != iend && si != siend; ++i)
                {
                    Light * l = *i;

                    if(!l->isCastShadow())
                        continue;
                    //////////////////////////////////////这部分和附加型的一样
                    // 存储当前阴影纹理
                    mCurrentTexture = *si;
                    // 为当前阴影纹理,获取摄象机
                    Camera * cam = mCurrentTexture->getBuffer()->getBuffer()->getViewport(0)->getCamera();
                     // 连接接收纹理
                    ShaderCh * targetPass = mSTCRP ? mSTCRP : mReceive;
                    ResourceIDList rlist;
                    rlist.push_back(mCurrentTexture->getOriginID());
                    targetPass->setTexture(ShaderCh::SOT_Set)->get(0)->setSrc(rlist, Texture::T_2D);
                    // 如果是固定函数管线,连接投影平截头,当着色程序管线要明确禁用
                    ShaderChTextureUnit* texUnit = targetPass->getTexture().get(0);
                    texUnit->setProjTexture(!targetPass->getProgram().isExist(ShaderChProgram::ST_VS));
                    texUnit->setProjFrustum(cam);
                    // 假使这是一个自定义材质,夹住边框颜色
                    texUnit->getSample()->setMode(TAM_BORDER, TAM_BORDER, TAM_BORDER);
                    texUnit->getSample()->setBorder(Colour::White);

                    mSyncParams->setTextureProjector(cam, 0);
                    /////////////////////////////////////////////////////////////////
                    /* 如果这个灯光是一个聚光灯,我们需要添加斑点衰减层,但在一
                        个自定义投影矩阵中不使用,因为它不适当预料中的形状
                    */
                    if(l->getType() == LT_SPOT && !cam->isCustomProjMatrix())
                    {
                        // 除了0 & 1外 移去所有纹理
                        // (如果已经使用的是附加型阴影,有一个问题)
                        while(targetPass->getTexture().getCount() > 2)
                            targetPass->setTexture(ShaderCh::SOT_Set)->remove(2);

                         // 如果目前没有,添加一个斑点衰减
                        if (targetPass->getTexture().getCount() == 2 &&
                            targetPass->getTexture().get(1)->getResourceID() ==
                                N_SpotShadowFade)
                        {
                            // 仅设置
                            ShaderChTextureUnit * t = targetPass->getTexture().get(1);
                            t->setProjTexture(!targetPass->getProgram().isExist(ShaderChProgram::ST_VS));
                            t->setProjFrustum(cam);
                        }
                        else
                        {
                            // 移去任何不符合的斑点层
                            while(targetPass->getTexture().getCount() > 1)
                                targetPass->setTexture(ShaderCh::SOT_Set)->remove(1);
                            // 创建新的
                            ShaderChTextureUnit* t = targetPass->setTexture(ShaderCh::SOT_Set)->create(N_SpotShadowFade);
                            t->setProjTexture(!targetPass->getProgram().isExist(ShaderChProgram::ST_VS));
                            t->setProjFrustum(cam);
                            t->getColourBlend()->setColourBlend(TBM_Src_Add_Dst);
                            t->getSample()->setMode(TAM_CLAMP, TAM_CLAMP, TAM_CLAMP);
                        }
                    }
                    else
                    {
                        // 除了包含斑点 0 外,移去所有纹理
                        while(targetPass->getTexture().getCount() > 1)
                            targetPass->setTexture(ShaderCh::SOT_Set)->remove(1);

                    }
                    // 设置 照明/混合模式
                    targetPass->setBlend(ShaderCh::SOT_Set)->setBlend(CF_DST, CF_ZERO);
                    targetPass->setLight(false);

                    targetPass->load();

                    renderSolidsBasic(pGroup);

                    ++si;

                }// 每灯光
                mRenderStage = ST_Normal;
            }

            // 再次迭代 - 由于gcc,要改变变量名字
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
    void ModulativeShadowMappingPattern::renderSolidsBasic(RenderGroup * rg)
    {
        mSyncParams->setAmbient(Colour::White);
        mRenderSys->setAmbient(Colour::White);

        RenderGroup::LevelList::const_iterator z, zend = rg->getLevelList().end();
        for(z = rg->getLevelList().begin(); z != zend; ++z)
        {
            RenderLevelGroup * rpg = z->second;

            mTest->setParam(false, false, false, &mNullLight);
            rpg->render(ST_BasicMaterial, mTest);
        }

        const Colour & ambient = mParent->getAmbient();
        mSyncParams->setAmbient(ambient);
        mRenderSys->setAmbient(ambient);
    }
    //------------------------------------------------------------------------
}