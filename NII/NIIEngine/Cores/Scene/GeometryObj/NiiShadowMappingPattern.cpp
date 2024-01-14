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
                /*  ���������ҵ��ĵƹ�,�������Թ����������,ע��ƹ�����������ڷ�������ӰͶ��
                    �ƹ⵽�ƹ��б�ʼ��,���,���ǲ���Ҫ�ٴ���Ǳ�ڵĲ�ƥ��ƹ�<->��Ӱ�����б�
                */
                LightList::const_iterator i, iend = lights.end();
                TextureList::iterator si, siend = mTextures.end();
                CameraList::iterator ci = mCameras.begin();
                NCount shadowTextureIndex = 0;

                for(i = lights.begin(), si = mTextures.begin(); i != iend && si != siend; ++i)
                {
                    Light * light = *i;

                    // �����Ӱ����,�����ƹ�
                    if(!light->isCastShadow())
                        continue;

                    if(mShadowLights.empty())
                        mShadowLights.push_back(light);
                    else
                        mShadowLights[0] = light;

                    // ����ÿ��������
                    NCount textureCountPerLight = mTextureCount[light->getType()];
                    for(NCount j = 0; j < textureCountPerLight && si != siend; ++j)
                    {
                        Texture * stex = *si;
                        FrameObj * texframe = stex->getBuffer()->getBuffer();
                        Viewport * shadowView = texframe->getViewport(0);
                        Camera * texCam = *ci;
                        // ���°������,������һ��SM��ʹ����,ת�䵽���������
                        shadowView->setCamera(texCam);

                        // �������ӿ������,��ΪLOD�����
                        texCam->setExtLod(cam);
                        // ���û�����
                        if(light->getType() != LT_POINT)
                            texCam->setDirection(light->getSpaceDirection());
                        if(light->getType() != LT_DIRECTIONAL)
                            texCam->setPos(light->getSpacePos());

                        // ʹ�����ӿڵĲ��ʷ���,��Ҫ����shadow_caster_material�����Ƶ�����
                        shadowView->setMaterial(vp->getMaterial());

                        CameraLightList::iterator camLightIt = mCamLightList.find(texCam);
                        N_assert(camLightIt != mCamLightList.end(), "error");
                        camLightIt->second = light;

                        if(light->getCameraSetup() == 0)
                            mShaderData->getCameraSetup()->setupShadow(texCam, mParent, cam, light, j);
                        else
                            light->getCameraSetup()->setupShadow(texCam, mParent, cam, light, j);

                        // ���ñ�����ɫ
                        shadowView->setBgColour(Colour::White);

                        // ������ӰͶ�����,�����ÿ��Ըı����������
                        if(mListener)
                            mListener->onCast(light, texCam);

                        // ��Ⱦһ��Ŀ��
                        texframe->flush();

                        ++si;
                        ++ci;
                    }
                    // Ϊ����ƹ����õ�һ����Ӱ��������
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
        // ȷ����ʹ���Զ������Ӱ,�������Ǵ�һ���Զ�����Ӱӳ�����͵�һ�����Զ����(ͳһ��Ӱӳ��)
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
        /*  ע��,�������ʹ��������Ӱ,����ʵ��ʹ��ƽ��ͷ�б�ǰ�治�ı�ĵƹ�,ƥ����
            �����ɵ�������Ӱ�����,���ǽ��������б��ʣ����Զ���������
        */
        NCount count = mTexConfList.size();
        if(dest.size() > count)
        {
            LightList::iterator start = dest.begin();
            std::advance(start, count);
            std::stable_sort(start, dest.end(), Light::LightDistanceLess());
        }

        // ���ڷ����������б���,������Ҫ���
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
                // �����ʵ�����Ȼ���
                texframe->setRasterGroup(mTexConfList[index].mGroup);

                Camera * cam = mParent->createCamera(0);
                cam->setAspectRatio((NIIf)stex->getWidth() / (NIIf)stex->getHeight());
                mCameras.push_back(cam);

                // ������ﲻ�����ӿ�,�򴴽�һ���ӿ�
                if(texframe->getViewportCount() == 0)
                {
                    // �����ڶ��SMʱ,ע������������Ƕ��ݵ�
                    Viewport * v = texframe->createViewport(N_Shadow_Viewport, cam);
                    v->setAutoClear(true);
                    v->setOverlayEnable(false);
                }

                // ����Ҫ�Զ�����-������Ҫʱ����
                texframe->setAutoFlush(false);

                Material * mat = static_cast<Material *>(N_Only(Material).create(0, GroupInner));
                ShaderCh * p = mat->get(0)->get(0);
                ShaderChTextureUnit * texUnit = p->setTexture(ShaderCh::SOT_Set)->create(stex);
                // �������������ͶӰ
                texUnit->setProjTexture(!p->getProgram().isExist(ShaderChProgram::ST_VS));
                texUnit->setProjFrustum(cam);

                texUnit->getSample()->setMode(TAM_BORDER, TAM_BORDER, TAM_BORDER);
                texUnit->getSample()->setBorder(Colour::White);

                mat->ref();
                mMaterials.push_back(mat);

                // ���������������
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
        // ��ȡ���е���Ӱ����ƹ�����
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
                /*  ��Ҫ����ȷ����Ӱ����,���ڿ�ʼ�ƹ�,���ܵƹ��б���Ըı�ÿ������,����Ϊ,
                    ��������Ӱ����ʱ,��������Ӱ֮�ϵĵƹ⽫�̶�������ƥ������Ѿ����ɵ�����
                    ��Ӱ�Ķ���, SpaceObjListener::query ע��ƹ�����׳�����,����
                    ����������в���,���ƹ������ render �����ǲŰ�
                */
                Texture * stex;
                if(stexindex < mTextures.size())
                {
                    stex = getTexture(stexindex);
                    Camera * cam = stex->getBuffer()->getBuffer()->getViewport(0)->getCamera();
                    // �ڹ̶���������������ͶӰ����,����ɫ�����������ȷҪ������
                    tunit->setProjTexture(!ch->getProgram().isExist(ShaderChProgram::ST_VS));
                    tunit->setProjFrustum(cam);

                    mSyncParams->setTextureProjector(cam, shdunit);
                }
                else
                {
                    // ʹ�ú󱸵�"null"��Ӱ����û��ͶӰ,���ͳһ��ɫ
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
                // �ֶ���������ͶӰ,���Ŀǰû���������ط�����,����Ҫ�ֶ�����
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
                //�ƹ�����ָ���� ����ƹ� ��ƹ� �۵ƹ�
                stexindex += mTextureCount[light->getType()];
                continue;
            }
            app.push_back(light);

            NCount j, jend = mTextureCount[light->getType()];
            for(j = 0; j < jend && stexindex < mTextures.size(); ++j)
            {
                // ����numShadowTextureLights��Ӱ����Ԫ
                ShaderChTextureUnit * tu = ch->getTexture().get(ShaderChTexture::T_SHADOW, stexcount);

                if(tu != 0)
                    break;
                // ������ǿ�ʼ��Ⱦ,��Ӧ�������ʱ�������
                Texture * stex = mTextures[stexindex];
                tu->setTexture(stex);
                Camera * cam = stex->getBuffer()->getBuffer()->getViewport(0)->getCamera();
                tu->setProjTexture(!ch->getProgram().isExist(ShaderChProgram::ST_VS));
                tu->setProjFrustum(cam);

                mSyncParams->setTextureProjector(cam, stexcount);
                ++stexcount;
                ++stexindex;
                // ������Ҫ����������Ⱦϵͳ,�Զ������������
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
        /* ������ǲ��Ǵ��������Ӱ����,�����������Ⱦ���Ͷ����Ӱ,����
            ������Ⱦ����ͨ���Ͳ���������Ӱ,ͨ�� > 0Ҳ������
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
        /* ������Ǵ���һ��������Ӱ��Ⱦ�����ͨ��,�����ͨ���ڵ�һ������(��Ӱ������Ⱦֻ
            ��Ҫ1��ͨ��,���Ƽ�������Ӱ�������Ҳ��Ҫ1��ͨ��)�ƹ���,���ͨ���ڵ�һ��ͨ��
            ֮��,�����Ⱦ״̬��ֹ�ı�,��Ϊ����ʵ��û��ʹ�����ͨ��������,Ҳ�ƹ���
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

        // ������� ͸���Ȼ��ͨ��
        if((src->getBlend().mSrcFactor == CF_SRC_ALPHA &&
            src->getBlend().mDstFactor == CF_INV_SRC_ALPHA) ||
                src->getColour().getAlphaReject() != CPM_ALWAYS_PASS)
        {
            // ͸���Ȼ��ͨ�����뱣�����ǵ�͸����
            re->setColour(ShaderCh::SOT_Set)->setAlphaReject(src->getColour().getAlphaReject());
            re->setColour(ShaderCh::SOT_Set)->setValue(src->getColour().getValue());
            re->setColour(ShaderCh::SOT_Set)->setAlphaCoverage(false);
            re->setBlend(ShaderCh::SOT_Set)->setBlend(src->getBlend().mSrcFactor, src->getBlend().mDstFactor);
            re->getParent()->getParent()->setAlphaShadowCast(true);

            // ��������Ԫ,��������ɫ����ȥ��������״̬,����һ����Ϊ0����Ӱ����
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
                // ���ƻ���״̬
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
            // Ҳ��Ҫ����ƹ��Զ�����,�ٵ���
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
        // ������������Ӱ��Ե
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

            // Ҳ����ƹ��Զ�����,��һ��
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
            // �����һ����,�����κκϲ���Ƭ����ɫ����
            if(re != mSTCRP)
            {
                re->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS, 0, 0);
            }
        }

        // ��Ҫ���־۹����ɫ��
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