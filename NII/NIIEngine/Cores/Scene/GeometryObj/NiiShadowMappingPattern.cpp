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
#include "NiiShadowMappingPattern.h"
#include "NiiTextureManager.h"
#include "NiiProfiler.h"
#include "NiiFusionManager.h"
#include "NiiFrameFusion.h"
#include "NiiSpotShadowFadePng.h"
#include "NiiShadowRenderQueue.h"
#include "NiiShadowRenderTest.h"
#include "NiiTextureFrame.h"
#include "NiiGpuBuffer.h"
#include "NiiMaterialManager.h"
#include "NiiSpaceManager.h"
#include "NiiSysSyncParam.h"
#include "NiiDefaultStream.h"
#include "NiiLight.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ShadowMappingData
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class ShadowMappingData : public ShadowAlloc
    {
    public:
        ShadowMappingData():
            mSTCCR(0),
            mSTCRR(0),
            mNullTexture(0)
        {
            mCamera = N_new CameraSetup();
        }
        ~ShadowMappingData()
        {
            N_delete mCamera;
        }
        
        inline CameraSetup * getCameraSetup() const { return mCamera; }
        
        Material * mSTCCR;
        Material * mSTCRR;
        CameraSetup * mCamera;
        Texture * mNullTexture;
    };
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ShadowMappingPattern
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ShadowMappingPattern::ShadowMappingPattern(SpaceManager * sm, RenderSys * rsys, SysSyncParam * param):
        RenderPattern(sm, rsys, param),
        mRenderStage(ST_Normal),
        mSTCRP(0),
        mSTCCP(0),
        mListener(0),
        mFadeEnd(0.9f),
        mFadeBegin(0.7f),
        mExtentOffsetFactor(0.6f),
        mTextureValid(false),
        mSelfShadow(false)
    {
        mTextureCount[LT_POINT] = 1;
        mTextureCount[LT_DIRECTIONAL] = 1;
        mTextureCount[LT_SPOT] = 1;

        mShaderData = N_new ShadowMappingData();

        setTextureCount(1);
    }
    //------------------------------------------------------------------------
    ShadowMappingPattern::~ShadowMappingPattern()
    {
        CameraList::iterator it, itend = mCameras.end();
        for(it= mCameras.begin(); it != itend; it++)
        {
            N_delete *it;
        }
        mCameras.clear();
        mCamLightList.clear();
        N_delete mShaderData;
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::renderShadow(Camera * cam, const Viewport * vp, const LightList & lights)
    {
        RenderPattern::renderShadow(cam, vp, lights);

        if(mRenderStage == ShadowType::ST_Cast)
            return;

        if(vp->isShadowEnable())
        {
            NiiProfileGroup("createTexture", NIIPROF_GENERAL);
            createTexture();

            ShadowType savedStage = mRenderStage;
            mRenderStage = ShadowType::ST_Cast;

            try
            {
                NIIf shadowDist = mShadowExtentMax < 0.000001f ? cam->getNearClipDistance() * 300 : mShadowExtentMax;
                NIIf shadowEnd = shadowDist * (1 + mExtentOffsetFactor);
                NIIf fadeStart = shadowEnd * mFadeBegin;
                NIIf fadeEnd = shadowEnd * mFadeEnd;

                setTextureFade(fadeStart, fadeEnd);

                mTextureIndexList.clear();
                /*  遍历我们找到的灯光,最大输出以光的纹理限制,注意灯光排序必须现在放置在阴影投射
                    灯光到灯光列表开始处,因此,我们不需要再处理潜在的不匹配灯光<->阴影纹理列表
                */
                LightList::const_iterator i, iend = lights.end();
                TextureList::iterator si, siend = mTextures.end();
                CameraList::iterator ci = mCameras.begin();
                NCount shadowTextureIndex = 0;

                for(i = lights.begin(), si = mTextures.begin(); i != iend && si != siend; ++i)
                {
                    Light * light = *i;

                    // 如果阴影禁用,跳过灯光
                    if(!light->isCastShadow())
                        continue;

                    if(mShadowLights.empty())
                        mShadowLights.push_back(light);
                    else
                        mShadowLights[0] = light;

                    // 迭代每纹理数量
                    NCount textureCountPerLight = mTextureCount[light->getType()];
                    for(NCount j = 0; j < textureCountPerLight && si != siend; ++j)
                    {
                        Texture * stex = *si;
                        FrameObj * texframe = stex->getBuffer()->getBuffer();
                        Viewport * shadowView = texframe->getViewport(0);
                        Camera * texCam = *ci;
                        // 重新绑定摄象机,假如另一个SM在使用中,转变到它的摄象机
                        shadowView->setCamera(texCam);

                        // 关联主视口摄象机,作为LOD摄象机
                        texCam->setExtLod(cam);
                        // 设置基本的
                        if(light->getType() != LT_POINT)
                            texCam->setDirection(light->getSpaceDirection());
                        if(light->getType() != LT_DIRECTIONAL)
                            texCam->setPos(light->getSpacePos());

                        // 使用主视口的材质方案,需要接收shadow_caster_material或类似的属性
                        shadowView->setMaterial(vp->getMaterial());

                        CameraLightList::iterator camLightIt = mCamLightList.find(texCam);
                        N_assert(camLightIt != mCamLightList.end(), "error");
                        camLightIt->second = light;

                        if(light->getCameraSetup() == 0)
                            mShaderData->getCameraSetup()->setupShadow(texCam, mParent, cam, light, j);
                        else
                            light->getCameraSetup()->setupShadow(texCam, mParent, cam, light, j);

                        // 设置背景颜色
                        shadowView->setBgColour(Colour::White);

                        // 引发阴影投射更新,被调用可以改变摄象机设置
                        if(mListener)
                            mListener->onCast(light, texCam);

                        // 渲染一遍目标
                        texframe->flush();

                        ++si;
                        ++ci;
                    }
                    // 为这个灯光设置第一个阴影纹理索引
                    mTextureIndexList.push_back(shadowTextureIndex);
                    shadowTextureIndex += textureCountPerLight;
                }
            }
            catch(Exception & e)
            {
                mRenderStage = savedStage;
                throw e;
            }
            mRenderStage = savedStage;

            N_Only(Texture).recoverShadow();
        }
    }
    //------------------------------------------------------------------------
    bool ShadowMappingPattern::isValid()
    {
        // 确保不使用自定义的阴影,假设我们从一个自定义阴影映射类型到一个非自定义的(统一阴影映射)
        for(NCount i = 0; i < mCameras.size(); ++i)
        {
            Camera * texCam = mCameras[i];
            texCam->setCustomViewMatrix(false);
            texCam->setCustomProjMatrix(false);
        }
        return true;
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::sortLight(LightList & dest)
    {
        /*  注意,如果我们使用纹理阴影,我们实际使用平截头列表前面不改变的灯光,匹配我
            们生成的纹理阴影。因此,我们仅允许在列表的剩余相对对象中排序
        */
        NCount count = mTexConfList.size();
        if(dest.size() > count)
        {
            LightList::iterator start = dest.begin();
            std::advance(start, count);
            std::stable_sort(start, dest.end(), Light::LightDistanceLess());
        }

        // 现在分配索引到列表中,根据需要检查
        NCount index = 0;
        LightList::iterator i, iend = dest.end();
        for(i = dest.begin(); i != iend; ++i, ++index)
        {
            (*i)->setIndex(index);
        }
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::destroyTexture()
    {
        MaterialList::iterator m, mend = mMaterials.end();
        for(m = mMaterials.begin(); m != mend; ++m)
        {
            Material * mat = *m;
            mat->get(0)->get(0)->setTexture(ShaderCh::SOT_Set)->removeAll();
            N_Only(Material).destroy(mat->getPrc());
        }
        mMaterials.clear();

        CameraList::iterator ci, ciend = mCameras.end();
        for (ci = mCameras.begin(); ci != ciend; ++ci)
        {
            N_delete *ci;
        }
        mCameras.clear();

        TextureList::iterator i, iend = mTextures.end();
        for(i = mTextures.begin(); i != iend; ++i)
        {
            (*i)->unref();
        }
        mTextures.clear();

        N_Only(Texture).recoverShadow();

        mTextureValid = false;
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::createTexture()
    {
        if(!mTextureValid)
        {
            destroyTexture();
            mCamLightList.clear();

            N_Only(Texture).getTexture(mTexConfList, mTextures);

            NCount index = 0;
            TextureList::iterator i, iend = mTextures.end();
            for(i = mTextures.begin(); i != iend; ++i, ++index)
            {
                Texture * stex = *i;

                TextureFrame * texframe = stex->getBuffer()->getBuffer();
                // 设置适当的深度缓存
                texframe->setRasterGroup(mTexConfList[index].mGroup);

                Camera * cam = mParent->createCamera(0);
                cam->setAspectRatio((NIIf)stex->getWidth() / (NIIf)stex->getHeight());
                mCameras.push_back(cam);

                // 如果这里不存在视口,则创建一个视口
                if(texframe->getViewportCount() == 0)
                {
                    // 当存在多个SM时,注意摄象机分配是短暂的
                    Viewport * v = texframe->createViewport(N_Shadow_Viewport, cam);
                    v->setAutoClear(true);
                    v->setOverlayEnable(false);
                }

                // 不需要自动更新-仅在需要时处理
                texframe->setAutoFlush(false);

                Material * mat = static_cast<Material *>(N_Only(Material).create(0, GroupInner));
                ShaderCh * p = mat->get(0)->get(0);
                ShaderChTextureUnit * texUnit = p->setTexture(ShaderCh::SOT_Set)->create(stex);
                // 基于摄象机设置投影
                texUnit->setProjTexture(!p->getProgram().isExist(ShaderChProgram::ST_VS));
                texUnit->setProjFrustum(cam);

                texUnit->getSample()->setMode(TAM_BORDER, TAM_BORDER, TAM_BORDER);
                texUnit->getSample()->setBorder(Colour::White);

                mat->ref();
                mMaterials.push_back(mat);

                // 添加虚拟摄象机组合
                mCamLightList[cam] = 0;
                stex->ref(false);
            }

            if(mTexConfList.empty())
            {
                if(mShaderData->mNullTexture)
                {
                    mShaderData->mNullTexture->unref();
                    mShaderData->mNullTexture = 0;
                }
            }
            else
            {
                if(mShaderData->mNullTexture)
                {
                    mShaderData->mNullTexture->unref();
                }
                mShaderData->mNullTexture = N_Only(Texture).getDummy(mTexConfList[0].mFormat);
                mShaderData->mNullTexture->ref(false);
            }
            mTextureValid = true;
        }
    }
    //------------------------------------------------------------------------
    Camera * ShadowMappingPattern::createCamera(SpaceID sid)
    {
        Camera * c = N_new Camera(sid, mParent);
        return c;
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::renderTexture(RenderGroup * rg, RenderSortMode om)
    {
        mSyncParams->setAmbient(mShadowColour);
        mRenderSys->setAmbient(mShadowColour);

        RenderGroup::LevelList::const_iterator z, zend = rg->getLevelList().end();
        for(z = rg->getLevelList().begin(); z != zend; ++z)
        {
            ShadowRenderLevelGroup * temp = static_cast<ShadowRenderLevelGroup *>(z->second);

            temp->sort(mCamera);

            mTest->setParam(false, false, false, &mShadowLights);
            temp->render(ST_BasicMaterial, mTest, om);

            mTest->setParam(false, false, false, &mShadowLights);
            temp->getSolidsNoShadowReceive().render(mTest, om);

            mTest->setParam(false, false, false, &mShadowLights);
            temp->render(ST_AlphaMaterial, mTest, om);

            mTest->setParam(true, false, false, &mShadowLights);
            temp->render(ST_AlphaViewDescend, mTest);
        }

        const Colour & ambient = mParent->getAmbient();

        mSyncParams->setAmbient(ambient);

        mRenderSys->setAmbient(ambient);
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::ShadowMappingPattern::applyTexture(const ShaderCh * ch)
    {
        NCount startLightIndex = ch->getLightBegin();
        NCount stexindex = mTextures.size();
        // 获取现有的阴影纹理灯光索引
        if(mTextureIndexList.size() > startLightIndex)
            stexindex = mTextureIndexList[startLightIndex];

        Nidx unit = 0;
        NCount shdunit = 0;
        ShaderChTexture::Textures::const_iterator i, iend = ch->getTexture().getTextures().end();
        for( i = ch->getTexture().getTextures().begin(); i != iend; ++i, ++unit)
        {
            ShaderChTextureUnit * tunit = *i;
            if(!ch->isEachLight() && tunit->getContentType() == ShaderChTexture::T_SHADOW)
            {
                /*  需要绑定正确的阴影纹理,基于开始灯光,尽管灯光列表可以改变每个对象,表现为,
                    当纹理阴影启用时,在纹理阴影之上的灯光将固定在所有匹配这个已经生成的纹理
                    阴影的对象, SpaceObjListener::query 注意灯光迭代抛出索引,我们
                    在这个事例中不绑定,当灯光迭代在 render 中我们才绑定
                */
                Texture * stex;
                if(stexindex < mTextures.size())
                {
                    stex = getTexture(stexindex);
                    Camera * cam = stex->getBuffer()->getBuffer()->getViewport(0)->getCamera();
                    // 在固定函数管线中启用投影纹理,在着色程序管线中明确要禁用它
                    tunit->setProjTexture(!ch->getProgram().isExist(ShaderChProgram::ST_VS));
                    tunit->setProjFrustum(cam);

                    mSyncParams->setTextureProjector(cam, shdunit);
                }
                else
                {
                    // 使用后备的"null"阴影纹理没有投影,因此统一颜色
                    stex = mShaderData->mNullTexture;
                    tunit->setProjTexture(false);

                    mSyncParams->setTextureProjector(0, shdunit);
                }
                tunit->setTexture(stex);

                ++stexindex;
                ++shdunit;
            }
            else if(mRenderStage == ST_Normal && ch->getProgram().isExist(ShaderChProgram::ST_VS))
            {
                // 手动设置纹理投影,如果目前没有在其他地方设置,或需要手动设置
                if(tunit->isProjTexture())
                {
                    mSyncParams->setTextureProjector(tunit->getProjFrustum(), unit);
                }
            }

            if(tunit->getContentType() == ShaderChTexture::T_FUSION)
            {
                FusionInstance * currentChain = mParent->getFrameResult();
                if(!currentChain)
                {
                    N_EXCEPT(InvalidState, _I18n("A ch that wishes to reference a compositor texutre ")
                        _I18n("attempted to render in a pipeline without a compositor"));
                }
                FrameFusion * ff = currentChain->get(tunit->getFrameFusion());
                if(ff == 0)
                {
                    N_EXCEPT(NotExist, _I18n("Invalid compositor content_type compositor name"));
                }
                Texture * rtex = ff->getTexture(tunit->getFusionTexture(), tunit->getMultiIndex());
                if(rtex == 0)
                {
                    N_EXCEPT(NotExist, _I18n("Invalid compositor content_type texture name"));
                }
                tunit->setTexture(rtex);
            }
            mRenderSys->_enableTexture(unit, tunit);
        }
        mRenderSys->_enableTexture(ch->getTexture().getCount());
    }
    //------------------------------------------------------------------------
    RenderQueue * ShadowMappingPattern::createQueue() const
    {
        RenderQueue * re = N_new ShadowRenderQueue();
        re->init();
        return re;
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::applyLight(const ShaderCh * ch, const LightList & src,  NCount & start, LightList & app)
    {
        NCount stexindex;
        NCount stexcount = 0;
        if(mTextureIndexList.size() > start)
        {
            stexindex = mTextureIndexList[start];
        }
        else
        {
            stexindex = mTextures.size();
        }

        NCount i, iend = ch->getLightRenderLimit();
        for(i = 0; i < iend && start < src.size(); ++start)
        {
            Light * light = src[start];

            if(ch->getLightTypeLimit() == LT_NONE ||
                ch->getLightTypeLimit() == light->getType() ||
                    (ch->getLightMark() & light->getLightMark()) == 0)
            {
                //灯光类型指的是 方向灯光 点灯光 聚灯光
                stexindex += mTextureCount[light->getType()];
                continue;
            }
            app.push_back(light);

            NCount j, jend = mTextureCount[light->getType()];
            for(j = 0; j < jend && stexindex < mTextures.size(); ++j)
            {
                // 连接numShadowTextureLights阴影纹理单元
                ShaderChTextureUnit * tu = ch->getTexture().get(ShaderChTexture::T_SHADOW, stexcount);

                if(tu != 0)
                    break;
                // 如果这是开始渲染,不应该在这个时候才设置
                Texture * stex = mTextures[stexindex];
                tu->setTexture(stex);
                Camera * cam = stex->getBuffer()->getBuffer()->getViewport(0)->getCamera();
                tu->setProjTexture(!ch->getProgram().isExist(ShaderChProgram::ST_VS));
                tu->setProjFrustum(cam);

                mSyncParams->setTextureProjector(cam, stexcount);
                ++stexcount;
                ++stexindex;
                // 现在需要设置纹理到渲染系统,自动参数随后设置
                mRenderSys->_enableTexture(tu->getLocalIndex(ch), tu);
            }
        }
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::setCullingMode(CullingMode mode)
    {
        if(mRenderStage == ShadowType::ST_Cast && mBackFaceInCast && mode == CM_Clockwise)
        {
            mCullingMode = CM_Anticlockwise;
        }
        mRenderSys->_setCullingMode(mCullingMode);
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::init()
    {
        if(mInit)
            return;

        RenderPattern::init();

        Texture * spotFade = static_cast<Texture *>(N_Only(Texture).get(N_SpotShadowFade));
        if(spotFade == 0)
        {
            DataStream * stream = N_new MemStream(SPOT_SHADOW_FADE_PNG, SPOT_SHADOW_FADE_PNG_SIZE, false);
            Image img;
            img.load(stream, _T("png"));
            spotFade = N_Only(Texture).load(N_SpotShadowFade, GroupInner, img, Texture::T_2D);
            N_delete stream;
        }

        mInit = true;
    }
    //------------------------------------------------------------------------
    bool ShadowMappingPattern::isRenderShadow() const
    {
        return mRenderStage == ShadowType::ST_Cast ? true : false;
    }
    //------------------------------------------------------------------------
    bool ShadowMappingPattern::isTextureShadow() const
    {
        return true;
    }
    //------------------------------------------------------------------------
    bool ShadowMappingPattern::testRender(const ShaderCh * ch, const GeometryObj * obj)
    {
        /* 如果我们不是处理调制阴影纹理,跳过这个可渲染物。它投射阴影,我们
            处理渲染接收通道和不是自身阴影,通道 > 0也是这样
        */
        if(mShadowEnable && mViewPort->isShadowEnable())
        {
            if(mRenderStage == ShadowType::ST_Receive && obj->isCastShadow() && !mSelfShadow)
            {
                return false;
            }
            if((mRenderStage == ShadowType::ST_Cast || mParent->isLockShaderCh()) && ch->getIndex() > 0)
            {
                return false;
            }
        }
        return RenderPattern::testRender(ch, obj);
    }
    //------------------------------------------------------------------------
    bool ShadowMappingPattern::testRender(const ShaderCh * ch)
    {
        /* 如果我们处理一个纹理阴影渲染和这个通道,而这个通道在第一个后面(阴影纹理渲染只
            需要1个通道,调制技术的阴影纹理接收也需要1个通道)绕过它,如果通道在第一个通道
            之后,如果渲染状态禁止改变,因为我们实际没有使用这个通道的数据,也绕过它
        */
        if(mShadowEnable && mViewPort->isShadowEnable() &&
            (mRenderStage == ShadowType::ST_Cast || mParent->isLockShaderCh()) && ch->getIndex() > 0)
        {
            return false;
        }
        return RenderPattern::testRender(ch);
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::deriveShadow(const ShaderCh *& dst, const ShaderCh * src)
    {
        if(mRenderStage == ShadowType::ST_Cast)
        {
            dst = deriveCaster(src);
        }
        else if(mRenderStage == ShadowType::ST_Receive)
        {
            dst = deriveReceiver(src);
        }
        dst = src;
    }
    //------------------------------------------------------------------------
    ShaderCh * ShadowMappingPattern::deriveCaster(const ShaderCh * src)
    {
        ShaderCh * re;
        if(src->getParent()->getShadowCaster() != 0)
        {
            return src->getParent()->getShadowCaster()->getLOD()->get(0);
        }
        else
        {
            re = mSTCCP ? mSTCCP : mCast;
        }

        // 特殊情况 透明度混合通道
        if((src->getBlend().mSrcFactor == CF_SRC_ALPHA &&
            src->getBlend().mDstFactor == CF_INV_SRC_ALPHA) ||
                src->getColour().getAlphaReject() != CPM_ALWAYS_PASS)
        {
            // 透明度混合通道必须保留它们的透明性
            re->setColour(ShaderCh::SOT_Set)->setAlphaReject(src->getColour().getAlphaReject());
            re->setColour(ShaderCh::SOT_Set)->setValue(src->getColour().getValue());
            re->setColour(ShaderCh::SOT_Set)->setAlphaCoverage(false);
            re->setBlend(ShaderCh::SOT_Set)->setBlend(src->getBlend().mSrcFactor, src->getBlend().mDstFactor);
            re->getParent()->getParent()->setAlphaShadowCast(true);

            // 充许纹理单元,但覆盖颜色函数去复制纹理状态,上移一个因为0是阴影纹理
            NCount srctexcount = src->getTexture().getCount();
            for(NCount t = 0; t < srctexcount; ++t)
            {
                ShaderChTextureUnit * tex;
                if(re->getTexture().getCount() <= t)
                {
                    tex = re->setTexture(ShaderCh::SOT_Set)->create();
                }
                else
                {
                    tex = re->setTexture(ShaderCh::SOT_Set)->get(t);
                }
                // 复制基本状态
                (*tex) = *(src->getTexture().get(t));

                tex->getColourBlend()->setBlend(TBO_OPERAND1, mShadowColour, TBS_PRE_TEXTURE);
            }
            while(re->getTexture().getCount() > srctexcount)
            {
                re->setTexture(ShaderCh::SOT_Set)->remove(srctexcount);
            }
        }
        else
        {
            re->setBlend(ShaderCh::SOT_Set)->setBlend(FBM_REPLACE);
            re->setColour(ShaderCh::SOT_Set)->setAlphaReject(CPM_ALWAYS_PASS);
            re->setTexture(ShaderCh::SOT_Set)->removeAll();
        }

        re->setCullingMode(src->getCullingMode());
        re->setSysCullingMode(src->getSysCullingMode());

        GpuProgramParam * param;
        GpuProgram * prg;
        src->getProgram().get(ShaderChProgram::ST_VS_SC, prg, param);
        if(prg != 0)
        {
            re->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS, prg->getOriginID(), param);
            //re->getProgram().get(ShaderChProgram::ST_VS, prg);
            if(prg->getState() != LS_LOADED)
                prg->load();
            // 也需要处理灯光自动参数,迟点做
        }
        else
        {
            if(re != mSTCCP)
            {
                re->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS, 0, 0);
            }
        }

        src->getProgram().get(ShaderChProgram::ST_FS_SC, prg, param);
        if(prg != 0)
        {
            re->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS, prg->getOriginID(), param);
            //re->getProgram().get(ShaderChProgram::ST_FS, prg);
            if(prg->getState() != LS_LOADED)
                prg->load();
            // Also have to hack the light autoparams, that is done later
        }
        else
        {
            if(re != mSTCCP)
            {
                re->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS, 0, 0);
            }
        }

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
    void ShadowMappingPattern::setTextureFade(NIIf start, NIIf end)
    {
        // 设置雾隐藏阴影边缘
        ShaderChFog temp(FM_LINEAR, Colour::White, 0, start, end);
        mReceive->setFog(ShaderCh::SOT_Set, &temp);
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::findLight(LightList & dest)
    {
        if(dest.getHash() != mLightHash)
        {
            LightList::iterator j, jend = dest.end();
            for(j = dest.begin(); j != jend; ++j)
            {
                (*j)->setSortBase(mCamera->getSpacePos());
            }

            std::stable_sort(dest.begin(), dest.end(), LightShadowLess());

            mLightHash = dest.getHash();
            mParent->notifyLight();
        }
    }
    //------------------------------------------------------------------------
    ShaderCh * ShadowMappingPattern::deriveReceiver(const ShaderCh * src)
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

        GpuProgramParam * param;
        GpuProgram * prg;
        src->getProgram().get(ShaderChProgram::ST_VS_SR, prg, param);
        if(prg != 0)
        {
            re->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS, prg->getOriginID(), param);
            if(prg->getState() != LS_LOADED)
                prg->load();

            // 也处理灯光自动参数,迟一点
        }
        else
        {
            if(re != mSTCRP)
            {
                re->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS, 0, 0);
            }
        }

        src->getProgram().get(ShaderChProgram::ST_FS_SR, prg, param);
        if(prg != 0)
        {
            re->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS, prg->getOriginID(), param);
            if(prg->getState() != LS_LOADED)
                prg->load();

            src->getProgram().get(ShaderChProgram::ST_VS, prg, param);
            if(prg && !re->getProgram().isExist(ShaderChProgram::ST_VS))
            {
                re->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS, prg->getOriginID(), param);
                if(prg->getState() != LS_LOADED)
                    prg->load();

            }
        }
        else
        {
            // 从最后一次中,重置任何合并的片断着色程序
            if(re != mSTCRP)
            {
                re->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS, 0, 0);
            }
        }

        // 需要保持聚光灯褪色等
        NCount keepTUCount = re->getTexture().getCount();
        while(re->getTexture().getCount() > keepTUCount)
        {
            re->setTexture(ShaderCh::SOT_Set)->remove(keepTUCount);
        }

        re->load();

        re->getParent()->getParent()->check();
        ShaderFusion * best = re->getParent()->getParent()->getLOD();
        if(best)
        {
            re = best->get(0);
        }

        return re;
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::setSelfShadow(bool b)
    {
        mSelfShadow = b;
        static_cast<ShadowRenderQueue *>(mParent->getRenderQueue())->setCastersNotReceivers(!b);
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::setTexture(Nidx idx, NCount w, NCount h, PixelFormat pf, Nui16 fsaa, GroupID gid)
    {
        TextureDefine conf;
        conf.mWidth = w;
        conf.mHeight = h;
        conf.mFormat = pf;
        conf.mFSAA = fsaa;
        conf.mGroup = gid;

        setTexture(idx, conf);
    }
    //---------------------------------------------------------------------
    void ShadowMappingPattern::setTexture(Nidx idx, const TextureDefine & config)
    {
        if (idx >= mTexConfList.size())
        {
            N_EXCEPT(NotExist, _I18n("shadowIndex out of bounds"));
        }
        mTexConfList[idx] = config;

        mTextureValid = false;
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::setTextureConfig(NCount w, NCount h, NCount count,
        PixelFormat fmt, Nui16 fsaa, GroupID gid)
    {
        setTextureCount(count);
        TextureDefineList::iterator i, iend = mTexConfList.end();
        for(i = mTexConfList.begin(); i != iend; ++i)
        {
            if(i->mWidth != w || i->mHeight != h || i->mFormat != fmt || i->mFSAA != fsaa)
            {
                i->mWidth = w;
                i->mHeight = h;
                i->mFormat = fmt;
                i->mFSAA = fsaa;
                i->mGroup = gid;
                mTextureValid = false;
            }
        }
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::setTextureCount(NCount count)
    {
        if(count != mTexConfList.size())
        {
            if(mTexConfList.empty())
            {
                mTexConfList.resize(count);
            }
            else
            {
                mTexConfList.resize(count, *mTexConfList.rbegin());
            }
            mTextureValid = false;
        }
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::setTextureSize(NCount w, NCount h)
    {
        TextureDefineList::iterator i, iend = mTexConfList.end();
        for(i = mTexConfList.begin(); i != iend; ++i)
        {
            if(i->mWidth != w || i->mHeight != h)
            {
                i->mWidth = w;
                i->mHeight = h;
                mTextureValid = false;
            }
        }
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::setTexturePixelFormat(PixelFormat pf)
    {
        TextureDefineList::iterator i, iend = mTexConfList.end();
        for(i = mTexConfList.begin(); i != iend; ++i)
        {
            if(i->mFormat != pf)
            {
                i->mFormat = pf;
                mTextureValid = false;
            }
        }
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::setTextureFSAA(Nui16 fsaa)
    {
        TextureDefineList::iterator i, iend = mTexConfList.end();
        for(i = mTexConfList.begin(); i != iend; ++i)
        {
            if(i->mFSAA != fsaa)
            {
                i->mFSAA = fsaa;
                mTextureValid = false;
            }
        }
    }
    //------------------------------------------------------------------------
    Texture * ShadowMappingPattern::getTexture(Nidx index) const
    {
        if(index >= mTexConfList.size())
        {
            N_EXCEPT(NotExist, _I18n("shadowIndex out of bounds"));
        }
        const_cast<ShadowMappingPattern *>(this)->createTexture();

        return mTextures[index];
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::setTextureCasterMaterial(ResourceID rid)
    {
        if(mShaderData->mSTCCR != 0 && mShaderData->mSTCCR->getOriginID() != rid)
        {
            mShaderData->mSTCCR->unref();
            mShaderData->mSTCCR = 0;
        }

        if(rid == 0)
        {
            mSTCCP = 0;
        }
        else
        {
            mShaderData->mSTCCR = static_cast<Material *>(N_Only(Material).get(rid));
            if(mShaderData->mSTCCR == 0)
            {
                N_EXCEPT(NotExist, _I18n("Cannot locate material called '") + N_conv(rid) + _T("'"));
            }
            if(mShaderData->mSTCCR->getLOD())
            {
                mShaderData->mSTCCR->ref();
                mSTCCP = mShaderData->mSTCCR->getLOD()->get(0);
            }
            else
            {
                mSTCCP = 0;
                mShaderData->mSTCCR = 0;
            }
        }
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::setTextureReceiverMaterial(ResourceID rid)
    {
        if(mShaderData->mSTCRR != 0 && mShaderData->mSTCRR->getOriginID() != rid)
        {
            mShaderData->mSTCRR->unref();
            mShaderData->mSTCRR = 0;
        }

        if(rid == 0)
        {
            mSTCRP = 0;
        }
        else
        {
            mShaderData->mSTCRR = static_cast<Material *>(N_Only(Material).get(rid));
            if(mShaderData->mSTCRR == 0)
            {
                N_EXCEPT(NotExist, _I18n("Cannot locate material called '") + N_conv(rid) + _T("'"));
            }

            if(mShaderData->mSTCRR->getLOD())
            {
                mShaderData->mSTCRR->ref();
                mSTCRP = mShaderData->mSTCRR->getLOD()->get(0);
            }
            else
            {
                mSTCRP = 0;
                mShaderData->mSTCRR = 0;
            }
        }
    }
    //------------------------------------------------------------------------
    void ShadowMappingPattern::setCameraSetup(CameraSetup * cam)
    {
        if(mShaderData->mCamera)
            N_delete mShaderData->mCamera;
        mShaderData->mCamera = cam;
    }
    //------------------------------------------------------------------------
    const CameraSetup * ShadowMappingPattern::getCameraSetup() const
    {
        return mShaderData->mCamera;
    }
    //------------------------------------------------------------------------
    bool ShadowMappingPattern::LightShadowLess::operator ()(const Light * l1, const Light * l2) const
    {
        if(l1 == l2)
            return false;

        if(l1->isCastShadow() != l2->isCastShadow())
        {
            return l1->isCastShadow();
        }

        return Light::LightDistanceLess()(l1, l2);
    }
    //------------------------------------------------------------------------
}