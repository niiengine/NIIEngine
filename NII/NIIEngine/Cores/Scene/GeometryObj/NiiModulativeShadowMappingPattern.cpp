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
        // ʹ�õ����ɰ���Ӱ
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

                // ������Ⱦ���л���ͨ��,���ƹ����,�޵ƹ�
                mTest->setParam(false, false, false, &mNullLight);
                rpg->render(ST_BasicMaterial, mTest);

                // Ҳ��Ⱦ��ֹ������Ӱ�Ķ���
                mTest->setParam(false, true, true, &mNullLight);
                rpg->getSolidsNoShadowReceive().render(mTest, RSM_Material);

                // ִ����һ����,���������"����"��Ⱦ�׶εĻ�,��������Ⱦ�������ڼ䴦�����
                if(mRenderStage == ST_Normal)
                {
                    LightList::const_iterator li, liend = mParent->getLightList().end();
                    TextureList::iterator si = mTextures.begin(), siend = mTextures.end();

                    for(li = mParent->getLightList().begin(); li != liend; ++li)
                    {
                        Light * l = *li;

                        if(l->isCastShadow() && si != siend)
                        {
                            // �洢��ǰ��Ӱ����
                            mCurrentTexture = *si;
                            // Ϊ��ǰ��Ӱ����,��ȡ�����
                            Camera * cam = mCurrentTexture->getBuffer()->getBuffer()->
                                getViewport(0)->getCamera();
                            // ��ͨ��������
                            ShaderCh * targetPass = mSTCRP ? mSTCRP : mReceive;
                            ResourceIDList rlist;
                            rlist.push_back(mCurrentTexture->getOriginID());
                            targetPass->setTexture(ShaderCh::SOT_Set)->get(0)->setSrc(rlist, Texture::T_2D);
                            // ����ǹ̶���������,����ͶӰƽ��ͷ,����ɫ�������ʱҪ��ȷ����
                            ShaderChTextureUnit * texUnit = targetPass->setTexture(ShaderCh::SOT_Set)->get(0);
                            texUnit->setProjTexture(!targetPass->getProgram().isExist(ShaderChProgram::ST_VS));
                            texUnit->setProjFrustum(cam);
                            // ��ʹ����һ���Զ������,��ס�߿���ɫ
                            texUnit->getSample()->setMode(TAM_BORDER, TAM_BORDER, TAM_BORDER);
                            texUnit->getSample()->setBorder(Colour::White);

                            mSyncParams->setTextureProjector(cam, 0);

                            // ��ȥ�κΰߵ�˥����
                            if(targetPass->getTexture().getCount() > 1 &&
                                targetPass->getTexture().get(1)->getResourceID() == N_SpotShadowFade)
                            {
                                // ��ȥ�ߵ�˥����(��������ǰһ ��ʹ���˵�����Ӱ,��������)
                                targetPass->setTexture(ShaderCh::SOT_Set)->remove(1);
                            }
                            // ���� ����/���ģʽ
                            targetPass->setBlend(ShaderCh::SOT_Set)->setBlend(CF_ONE, CF_ONE);
                            targetPass->setLight(true);
                            targetPass->load();

                            // ��Ϊʹ��,������Ӱ����
                            ++si;
                            // �ı���Ⱦ״̬(������Ӱ)
                            mRenderStage = ShadowType::ST_Receive;
                        }
                        else
                        {   // �����Ⱦ״̬(��ͨ)
                            mRenderStage = ShadowType::ST_Normal;
                        }

                        // Ϊ����ƹ���Ⱦ����ͨ��
                        if(lightList.empty())
                            lightList.push_back(l);
                        else
                            lightList[0] = l;

                        // �����ƹ�ü�,�������ģʽ�������õ�
                        SpaceManager::ClipType scissored = mParent->applyClip(lightList, mCamera);
                        SpaceManager::ClipType clipped = SpaceManager::CT_None;
                        if(mLightClipPlane)
                            clipped = mParent->applyPlaneClip(lightList);
                        // ��������޼�,������
                        if(scissored == SpaceManager::CT_All || clipped == SpaceManager::CT_All)
                            continue;

                        mTest->setParam(false, false, false, &lightList);
                        rpg->getSolidsDiffuseSpecular().render(mTest, RSM_Material);

                        if(scissored == SpaceManager::CT_Some)
                            mParent->resetClip();
                        if(clipped == SpaceManager::CT_Some)
                            mParent->resetPlaneClip();

                    }// ÿ���ƹ�

                    mRenderStage = ST_Normal;

                    mTest->setParam(false, false, false, &mNullLight);
                    rpg->getSolidsDecal().render(mTest, RSM_Material);
                    // ������Ⱦ����ͨ��,����Ҫ���õƹ�,��Ϊ��������
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
        // �����͵ƹⲻӦ��ʹ����,��Ϊ����̫������,ʹ��border clamp ��ȷ�����
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

        // �����ͨ����һ���Զ�����Ӱ���ճ���?
        GpuProgramParam * param;
        GpuProgram * prg;

        src->getProgram().get(ShaderChProgram::ST_VS_SR, prg, param);
        if(prg != 0)
        {
            re->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS, prg->getOriginID(), param);
            //re->getProgram().get(ShaderChProgram::ST_VS, prg);
            if(prg->getState() != LS_LOADED)
                prg->load();
            // Ҳ����ƹ��Զ�����,��һ��
        }
        else
        {
            if(re != mSTCRP)
            {
                // ��׼��Ӱ����ͨ��,����Ϊ�� vp
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
        // ��������״̬,����һ����Ϊretpass��0����Ӱ����
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
            // ����ǿɱ�̵�,���������������,D3D����������������ڿɱ��ģʽ��ƥ������Ԫ
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

            // ����һ����Ӱ������ɫ����?
            if(src->getProgram().isExist(ShaderChProgram::ST_VS) && !re->getProgram().isExist(ShaderChProgram::ST_VS))
            {
                // ���󶨽���ָ����ɫ����, ��˰�ԭʼ��
                src->getProgram().get(ShaderChProgram::ST_VS, prg, param);
                re->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS, prg->getOriginID(), param);
                //re->getProgram().get(ShaderChProgram::ST_VS, prg);
                if(prg->getState() != LS_LOADED)
                    prg->load();
            }
        }
        else
        {
            // �����һ����,�����κκϲ���Ƭ����ɫ����
            if(re != mSTCRP)
            {
                // ��׼��Ӱ����ͨ��,��fs
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
        /* ������Ǵ���һ��������Ӱ��Ⱦ�����ͨ��,�����ͨ���ڵ�һ������(��Ӱ������Ⱦֻ
            ��Ҫ1��ͨ��,���Ƽ�������Ӱ�������Ҳ��Ҫ1��ͨ��)�ƹ���,���ͨ���ڵ�һ��ͨ��
            ֮��,�����Ⱦ״̬��ֹ�ı�,��Ϊ����ʵ��û��ʹ�����ͨ��������,Ҳ�ƹ���
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
        /* ������ǲ��Ǵ��������Ӱ����,�����������Ⱦ���Ͷ����Ӱ,����
            ������Ⱦ����ͨ���Ͳ���������Ӱ,ͨ��>0Ҳ������
        */
        if(mShadowEnable && mViewPort->isShadowEnable())
        {
            // Ϊ͸����,test��һЩ�ظ�ʹ��
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
        // ʹ�õ����ɰ���Ӱ
        if(mRenderStage == ShadowType::ST_Cast)
        {
            renderTexture(pGroup, RSM_Material);
        }
        else if(pGroup->isShadowEnable())
        {
            /* Ϊÿ���ƹ�,������Ҫ��Ⱦ����ÿ����Ĺ���,Ȼ���������Ӱ,֮����Ⱦÿ�����͸
                �������,��ζ�����ǽ�Ҫ���������,�������Ӱ��Ҫ����ȥ��ȷ�Ļ���Ϊ�˱���
                ���������,��������͸������β��,�����ǽ�����������Ⱦѭ����
            */
            // �������ȼ�
            RenderGroup::LevelList::const_iterator z, zend = pGroup->getLevelList().end();
            for(z = pGroup->getLevelList().begin(); z != zend; ++z)
            {
                ShadowRenderLevelGroup * rpg = static_cast<ShadowRenderLevelGroup *>(z->second);

                rpg->sort(mCamera);

                // �������
                mTest->setParam(false, true, true, &mNullLight);
                rpg->render(ST_BasicMaterial, mTest);

                mTest->setParam(false, true, true, &mNullLight);
                rpg->getSolidsNoShadowReceive().render(mTest, RSM_Material);
            }

            // �����ƹ�,���������һ��"����"��Ⱦ�׶β�ִ�н�����Ӱ,����Ⱦ������׶��б��⴦�����
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
                    //////////////////////////////////////�ⲿ�ֺ͸����͵�һ��
                    // �洢��ǰ��Ӱ����
                    mCurrentTexture = *si;
                    // Ϊ��ǰ��Ӱ����,��ȡ�����
                    Camera * cam = mCurrentTexture->getBuffer()->getBuffer()->getViewport(0)->getCamera();
                     // ���ӽ�������
                    ShaderCh * targetPass = mSTCRP ? mSTCRP : mReceive;
                    ResourceIDList rlist;
                    rlist.push_back(mCurrentTexture->getOriginID());
                    targetPass->setTexture(ShaderCh::SOT_Set)->get(0)->setSrc(rlist, Texture::T_2D);
                    // ����ǹ̶���������,����ͶӰƽ��ͷ,����ɫ�������Ҫ��ȷ����
                    ShaderChTextureUnit* texUnit = targetPass->getTexture().get(0);
                    texUnit->setProjTexture(!targetPass->getProgram().isExist(ShaderChProgram::ST_VS));
                    texUnit->setProjFrustum(cam);
                    // ��ʹ����һ���Զ������,��ס�߿���ɫ
                    texUnit->getSample()->setMode(TAM_BORDER, TAM_BORDER, TAM_BORDER);
                    texUnit->getSample()->setBorder(Colour::White);

                    mSyncParams->setTextureProjector(cam, 0);
                    /////////////////////////////////////////////////////////////////
                    /* �������ƹ���һ���۹��,������Ҫ��Ӱߵ�˥����,����һ
                        ���Զ���ͶӰ�����в�ʹ��,��Ϊ�����ʵ�Ԥ���е���״
                    */
                    if(l->getType() == LT_SPOT && !cam->isCustomProjMatrix())
                    {
                        // ����0 & 1�� ��ȥ��������
                        // (����Ѿ�ʹ�õ��Ǹ�������Ӱ,��һ������)
                        while(targetPass->getTexture().getCount() > 2)
                            targetPass->setTexture(ShaderCh::SOT_Set)->remove(2);

                         // ���Ŀǰû��,���һ���ߵ�˥��
                        if (targetPass->getTexture().getCount() == 2 &&
                            targetPass->getTexture().get(1)->getResourceID() ==
                                N_SpotShadowFade)
                        {
                            // ������
                            ShaderChTextureUnit * t = targetPass->getTexture().get(1);
                            t->setProjTexture(!targetPass->getProgram().isExist(ShaderChProgram::ST_VS));
                            t->setProjFrustum(cam);
                        }
                        else
                        {
                            // ��ȥ�κβ����ϵİߵ��
                            while(targetPass->getTexture().getCount() > 1)
                                targetPass->setTexture(ShaderCh::SOT_Set)->remove(1);
                            // �����µ�
                            ShaderChTextureUnit* t = targetPass->setTexture(ShaderCh::SOT_Set)->create(N_SpotShadowFade);
                            t->setProjTexture(!targetPass->getProgram().isExist(ShaderChProgram::ST_VS));
                            t->setProjFrustum(cam);
                            t->getColourBlend()->setColourBlend(TBM_Src_Add_Dst);
                            t->getSample()->setMode(TAM_CLAMP, TAM_CLAMP, TAM_CLAMP);
                        }
                    }
                    else
                    {
                        // ���˰����ߵ� 0 ��,��ȥ��������
                        while(targetPass->getTexture().getCount() > 1)
                            targetPass->setTexture(ShaderCh::SOT_Set)->remove(1);

                    }
                    // ���� ����/���ģʽ
                    targetPass->setBlend(ShaderCh::SOT_Set)->setBlend(CF_DST, CF_ZERO);
                    targetPass->setLight(false);

                    targetPass->load();

                    renderSolidsBasic(pGroup);

                    ++si;

                }// ÿ�ƹ�
                mRenderStage = ST_Normal;
            }

            // �ٴε��� - ����gcc,Ҫ�ı��������
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