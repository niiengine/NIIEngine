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
#include "NiiSpaceManager.h"
#include "NiiMeshManager.h"
#include "NiiEquationManager.h"
#include "NiiMaterialManager.h"
#include "NiiOverlayManager.h"
#include "NiiGeometrySpace.h"
#include "NiiLight.h"
#include "NiiStrConv.h"
#include "NiiRenderQueue.h"
#include "NiiRenderObjListener.h"
#include "NiiParticleCtrl.h"
#include "NiiLogManager.h"
#include "NiiGBufferManager.h"
#include "NiiEngine.h"
#include "NiiGpuProgramManager.h"
#include "NiiAnimationManager.h"
#include "NiiInstanceSpace.h"
#include "NiiInstanceMesh.h"
#include "NiiDataStream.h"
#include "NiiSysSyncParam.h"
#include "NiiStaticSpace.h"
#include "NiiGpuBuffer.h"
#include "NiiCustomSpaceObj.h"
#include "NiiPixelChain.h"
#include "NiiPixelOrbit.h"
#include "NiiParticleSpaceManager.h"
#include "NiiFusionManager.h"
#include "NiiProfiler.h"
#include "NiiFrameFusion.h"
#include "NiiInstanceBatch.h"
#include "NiiInstanceGeo.h"
#include "NiiRenderPattern.h"
#include "NiiRenderQueue.h"
#include "NiiNullShadowTechniquePattern.h"
#include "NiiAreaSpaceQuery.h"
#include "NiiShadowRenderQueue.h"
#include "NiiSky.h"

#include <cstdio>

namespace NII
{
    class NullShadowTechniquePattern : public RenderPattern
    {
    public:
        NullShadowTechniquePattern(SpaceManager * sm, RenderSys * rsys, SysSyncParam * param):
            RenderPattern(sm, rsys, param)
        {
        }

        /// @copydetails RenderPattern::prepare
        virtual void prepare(RenderQueue * queue)
        {
            ShadowRenderQueue * temp = static_cast<ShadowRenderQueue *>(queue);
            temp->setCastersNotReceivers(true);

            temp->setSplitLightingType(false);

            temp->setSplitNoShadow(false);
        }

        /// @copydetails RenderPattern::prepare
        virtual void prepare(RenderGroup * group)
        {
            ShadowRenderGroup * temp = static_cast<ShadowRenderGroup *>(group);
            temp->setCastersNotReceivers(true);

            temp->setSplitLightingType(false);

            temp->setSplitNoShadow(false);
        }

        /// @copydetails Pattern::getQuality
        Nui8 getQuality() const             { return 0; }

        /// @copydetails Pattern::getPerformance
        Nui8 getPerformance() const         { return 0; }
    protected:
        /// @copydetails RenderPattern::renderImpl
        void renderImpl(RenderGroup * rg)   { renderBasic(rg); }

        /// @copydetails Pattern::_init
        bool _init()                        { return true; }

        /// @copydetails Pattern::_update
        void _update()                      {}

        /// @copydetails Pattern::_end
        bool _end()                         { return true; }
        
        /// @copydetails Pattern::getPID
        PatternID getPID() const            { return 0; }
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // SpaceManager
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Nmark SpaceManager::SpaceGeoMark        = 0x80000000;
    Nmark SpaceManager::SelfGeoMark         = 0x40000000;
    Nmark SpaceManager::ParticleSpaceMark   = 0x20000000;
    Nmark SpaceManager::StaticGeoMark       = 0x10000000;
    Nmark SpaceManager::InstanceGeoMark     = 0x08000000;
    Nmark SpaceManager::LightSpaceMark      = 0x04000000;
    Nmark SpaceManager::CameraSpaceMark     = 0x02000000;
    Nmark SpaceManager::ViewSpaceMark       = 0x01000000;
    Nmark SpaceManager::CustomSpaceMark     = SpaceManager::ViewSpaceMark;
    RenderPattern * SpaceManager::mNoShadowPattern = 0;
    SpaceManager::QueueFusionList SpaceManager::mQueueFusionList;
    //-----------------------------------------------------------------------
    SpaceManager::SpaceManager(const String & name, TypeID tid) :
        mName(name),
        mType(tid),
        mView(0),
        mLightDirtyMark(0),
        mSceneGeoRenderGroup(RQG_Geo_1),
        mFrameResult(0),
        mLightClipFrame(999),
        mVisibleMark(0xFFFFFFFF),
        mLastLightHash(0),
        mLastLightLimit(0),
        mLastProgramLightMark(0),
        mLastFrame(0),
        mGpuParamMark(GPT_Unknow),
        mShadowVolume(false),
        mOptCh(false),
        mLockShaderCh(false),
        mNodeView(false),
        mAABBView(false),
        mAutoVisible(true),
        mIdentityViewMode(false),
        mIdentityProjMode(false),
        mLastCustomRender(false),
        mUnitNormals(true),
        mCameraOrigin(false),
        mNegScaleFlipCull(true)
    {
        Engine * root = Engine::getOnlyPtr();
        if(root)
        {
            mRenderSys = root->getRender();
        }
        mSyncParams = N_new SysSyncParam();
        mRenderPattern = mNoShadowPattern = N_new NullShadowTechniquePattern(this, mRenderSys, mSyncParams);
        mOriginNode = createNodeImpl(0);
        mOriginNode->setEnable(true);

        mMainQueue = mNoShadowPattern->createQueue();

        mMainQueue->getGroup(RQG_Bg)->setShadowEnable(false);
        mMainQueue->getGroup(RQG_Overlay)->setShadowEnable(false);
        mMainQueue->getGroup(RQG_Sky)->setShadowEnable(false);
        mMainQueue->getGroup(RQG_NearEnd)->setShadowEnable(false);

        mFog.setMode(FM_NONE);
    }
    //-----------------------------------------------------------------------
    SpaceManager::~SpaceManager()
    {
        onDestroy();
        destroyAllCreateObj();
        destroyAllCamera();

        mSpaceMapList.clear();

        N_delete mNoShadowPattern;
        N_delete mOriginNode;
        N_delete mMainQueue;
        N_delete mSyncParams;
    }
    //-----------------------------------------------------------------------
    void SpaceManager::setShadowVolume(RenderQueue * rq, bool b)
    {
        mShadowVolume = b;
    }
    //-----------------------------------------------------------------------
    bool SpaceManager::isShadowVolume(RenderQueue * rq) const
    {
        return mShadowVolume;
    }
    //-----------------------------------------------------------------------
    void SpaceManager::addExcludeRender(GroupID qid)
    {
        if(qid < RQG_Max)
            mExcludeList.insert(qid);
    }
    //-----------------------------------------------------------------------
    void SpaceManager::removeExcludeRender(GroupID qid)
    {
        if (qid < RQG_Max)
        {
            mExcludeList.erase(qid);
        }
    }
    //-----------------------------------------------------------------------
    void SpaceManager::removeAllExcludeRender()
    {
        mExcludeList.clear();
    }
    //-----------------------------------------------------------------------
    bool SpaceManager::isExcludeRenderExist(GroupID qid)
    {
        return mExcludeList.find(qid) != mExcludeList.end();
    }
    //-----------------------------------------------------------------------
    void SpaceManager::setSceneGeoRenderGroup(GroupID qid)
    {
        if(qid < RQG_Max)
            mSceneGeoRenderGroup = qid;
    }
    //-----------------------------------------------------------------------
    GroupID SpaceManager::getSceneGeoRenderGroup()
    {
        return mSceneGeoRenderGroup;
    }
    //-----------------------------------------------------------------------
    bool SpaceManager::setProperty(PropertyID pid, const void * value)
    {
        (void)pid;
        (void)value;
        return false;
    }
    //-----------------------------------------------------------------------
    bool SpaceManager::getProperty(PropertyID pid, void * value) const
    {
        (void)pid;
        (void)value;
        return false;
    }
    //-----------------------------------------------------------------------
    bool SpaceManager::isPropertyExist(PropertyID pid) const
    {
        (void)pid;
        return false;
    }
    //-----------------------------------------------------------------------
    bool SpaceManager::getPropertyList(PropertyArray * pids) const
    {
        (void)pids;
        return false;
    }
    //-----------------------------------------------------------------------
    Camera * SpaceManager::createCamera(SpaceID sid)
    {
        if(mCameraList.find(sid) != mCameraList.end())
        {
            N_EXCEPT(UniqueRepeat, _I18n("摄像机: ") + N_conv(sid) + _I18n(" 已经存在!"));
        }

        Camera * re = N_new Camera(sid, this);
        mCameraList.insert(CameraMap::value_type(sid, re));

        return re;
    }
    //-----------------------------------------------------------------------
    void SpaceManager::destroyCamera(SpaceID sid)
    {
        CameraMap::iterator i = mCameraList.find(sid);
        if(i != mCameraList.end())
        {
            mRenderSys->_notifyDestroy(i->second);
            N_delete i->second;
            mCameraList.erase(i);
        }
    }
    //-----------------------------------------------------------------------
    Camera * SpaceManager::getCamera(SpaceID sid) const
    {
        CameraMap::const_iterator i = mCameraList.find(sid);
        if(i == mCameraList.end())
        {
            N_EXCEPT(NotExist, _I18n("不能找到摄像机: ") + N_conv(sid));
            return 0;
        }

        return i->second;
    }
    //-----------------------------------------------------------------------
    bool SpaceManager::isCameraExist(SpaceID sid) const
    {
        return (mCameraList.find(sid) != mCameraList.end());
    }
    //-----------------------------------------------------------------------
    void SpaceManager::destroyAllCamera()
    {
        CameraMap::iterator i, iend = mCameraList.end();
        for(i = mCameraList.begin(); i != iend; ++i)
        {
            mRenderSys->_notifyDestroy(i->second);
            N_delete i->second;
        }
        mCameraList.clear();
    }
    //-----------------------------------------------------------------------
    Light * SpaceManager::createLight(SpaceID sid)
    {
        return static_cast<Light *>(createSpace(sid, N_FACTORY_Light));
    }
    //-----------------------------------------------------------------------
    Light * SpaceManager::getLight(SpaceID sid) const
    {
        return static_cast<Light *>(getSpace(sid, N_FACTORY_Light));
    }
    //-----------------------------------------------------------------------
    bool SpaceManager::isLightExist(SpaceID sid) const
    {
        return isSpaceExist(sid, N_FACTORY_Light);
    }
    //-----------------------------------------------------------------------
    void SpaceManager::destroyLight(SpaceID sid)
    {
        destroySpace(sid, N_FACTORY_Light);
    }
    //-----------------------------------------------------------------------
    void SpaceManager::destroyAllLight()
    {
        destroyAllSpace(N_FACTORY_Light);
    }
    //-----------------------------------------------------------------------
    void SpaceManager::findLight(LightList & dst, const Vector3f & pos, NIIf radius, Nmark mark)
    {
        dst.clear();
        dst.reserve(mValidLights.size());

        LightList::const_iterator it, itend = mValidLights.end();
        for(it = mValidLights.begin(); it != itend; ++it)
        {
            Light * l = *it;
            if((l->getLightMark() & mark) == 0)
                continue;

            l->setSortBase(pos);

            if(l->getType() == LT_DIRECTIONAL)
            {
                 dst.push_back(l);
            }
            else
            {
                 NIIf maxDist = l->getRange() + radius;
                 if(Light::ValueDistanceLess()(l, maxDist))
                 {
                     dst.push_back(l);
                 }
            }
        }
        mRenderPattern->sortLight(dst);
    }
    //-----------------------------------------------------------------------
    GeometrySpace * SpaceManager::createGeo(SpaceID sid, GeometryPrefab ptype, GroupID gid)
    {
        PropertyData params;
        params.add(N_PropertyMesh_PrefabType, N_conv(ptype));
        params.add(N_PropertyResource_Group, N_conv(gid));
        return static_cast<GeometrySpace *>(createSpace(sid, N_FACTORY_GeoSpace, &params));
    }
    //-----------------------------------------------------------------------
    GeometrySpace * SpaceManager::createGeo(SpaceID sid, ResourceID rid, GroupID gid)
    {
        PropertyData params;
        params.add(N_PropertyMesh_ID, N_conv(rid));
        params.add(N_PropertyResource_Group, N_conv(gid));
        return static_cast<GeometrySpace *>(createSpace(sid, N_FACTORY_GeoSpace, &params));
    }
    //-----------------------------------------------------------------------
    GeometrySpace * SpaceManager::getGeo(SpaceID sid) const
    {
        return static_cast<GeometrySpace *>(getSpace(sid, N_FACTORY_GeoSpace));
    }
    //-----------------------------------------------------------------------
    CustomSpaceObj * SpaceManager::createCustomGeo(SpaceID sid)
    {
        return static_cast<CustomSpaceObj *>(createSpace(sid, N_FACTORY_CustomSpace));
    }
    //-----------------------------------------------------------------------
    CustomSpaceObj * SpaceManager::getCustomGeo(SpaceID sid) const     
    { 
        return static_cast<CustomSpaceObj *>(getSpace(sid, N_FACTORY_CustomSpace)); 
    }
    //-----------------------------------------------------------------------
    ParticleSpace * SpaceManager::createParticleSpace(SpaceID sid, SpaceID templateName)
    {
        PropertyData params;
        params.add(N_PropertyParticleSpace_Template, N_conv(templateName));

        return static_cast<ParticleSpace *>(createSpace(sid, N_FACTORY_ParticleSpace, &params));
    }
    //-----------------------------------------------------------------------
    ParticleSpace * SpaceManager::createParticleSpace(SpaceID sid, GroupID gid, NCount quota)
    {
        PropertyData params;
        params.add(N_PropertyParticleSpace_PoolSize, N_conv(quota));
        params.add(N_PropertyResource_Group, N_conv(gid));

        return static_cast<ParticleSpace *>(createSpace(sid, N_FACTORY_ParticleSpace, &params));
    }
    //-----------------------------------------------------------------------
    ParticleSpace * SpaceManager::getParticleSpace(SpaceID sid) const
    {
        return static_cast<ParticleSpace *>(getSpace(sid, N_FACTORY_ParticleSpace));
    }
    //-----------------------------------------------------------------------
    void SpaceManager::destroyAllCreateObj()
    {
        destroyAllStaticSpace();
        destroyAllSpace();

        // 清除根节点所有子对象
        getOriginNode()->removeAll();
        getOriginNode()->detachAll();

        // 删除所有SceneNodes,除了根节点外
        SceneNodeList::iterator i, iend = mSceneNodes.end();
        for(i = mSceneNodes.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
        mSceneNodes.clear();
        mAutoFocusList.clear();

        // 清除所有动画
        N_Only(Animation).destroyAllAnimations();

        // 清除渲染队列,完全空的
        if (mMainQueue)
        {
            mMainQueue->setDestroyAllOnClear(true);
            mMainQueue->clear();
        }
    }
    //-----------------------------------------------------------------------
    SpaceNode * SpaceManager::createSpaceNode(SpaceID sid)
    {
        if(mSceneNodes.find(sid) != mSceneNodes.end())
        {
            N_EXCEPT(UniqueRepeat, _I18n("A scene node with the name ") + 
                N_conv(sid) + _I18n(" already exists"));
        }

        SpaceNode * sn = createNodeImpl(sid);
        mSceneNodes[sn->getID()] = sn;
        return sn;
    }
    //-----------------------------------------------------------------------
    void SpaceManager::destroySpaceNode(SpaceID sid)
    {
        SceneNodeList::iterator i = mSceneNodes.find(sid);
        if(i == mSceneNodes.end())
        {
            N_EXCEPT(NotExist, _I18n("SpaceNode '") + N_conv(sid) + _I18n("' not found."));
        }

        // 查找任何跟踪这个节点场景节点,关闭它们
        AutoFocusList::iterator ai = mAutoFocusList.begin();
        for(; ai != mAutoFocusList.end(); )
        {
            AutoFocusList::iterator curri = ai++;
            SpaceNode * sn = *curri;
            // 检测跟踪节点
            if(sn->getAutoFocus() == i->second)
            {
                sn->setAutoFocus(false);
            }
            // 节点本身就是一个跟踪器
            else if(sn == i->second)
            {
                 mAutoFocusList.erase(curri);
            }
        }

        // 从父对象中分离 (不要在析构函数中做这个,因为绝大部分析构函数行为有所不同)
        PosNode * parentNode = i->second->getParent();
        if(parentNode)
        {
            parentNode->remove(i->second);
        }
        N_delete i->second;
        mSceneNodes.erase(i);
    }
    //-----------------------------------------------------------------------
    SpaceNode * SpaceManager::getSpaceNode(SpaceID sid) const
    {
        SceneNodeList::const_iterator i = mSceneNodes.find(sid);
        if(i == mSceneNodes.end())
        {
            N_EXCEPT(NotExist, _I18n("SpaceNode '") + N_conv(sid) + _I18n("' not found."));
        }

        return i->second;
    }
    //-----------------------------------------------------------------------
    bool SpaceManager::isSpaceNodeExist(SpaceID sid) const
    {
        return (mSceneNodes.find(sid) != mSceneNodes.end());
    }
    //-----------------------------------------------------------------------
    const ShaderCh * SpaceManager::applyCh(const ShaderCh * ch, bool force, bool shadow)
    {
        if(mOptCh)
        {
            ShaderFusion * lod = ch->getParent()->getParent()->getLOD();
            if(lod->getCount() > ch->getIndex())
            {
                ch = lod->get(ch->getIndex());
            }
        }

        if(!mLockShaderCh || force)
        {
            if (shadow)
            {
                mRenderPattern->deriveShadow(ch, ch);
            }

            mSyncParams->setCurrentShaderCh(ch);

            bool envcolour = true;
            bool envlight = true;
            bool fsfog = true;

            GpuProgram * prog = 0;
            const ShaderChProgram & schprog = ch->getProgram();
            schprog.get(ShaderChProgram::ST_VS, prog);
            if(prog)
            {
                mLastProgramLightMark = 1;
                mGpuParamMark = GPT_Unknow;
                mRenderSys->on(prog->getMain());
            
                envcolour = prog->isEnvColourEnable();
                envlight = prog->isEnvLightEnable();
            }
            else
            {
                if(mRenderSys->isOn(GpuProgram::ST_VS))
                    mRenderSys->off(GpuProgram::ST_VS);
                // 设置固定函数管线的顶点参数
            }

            schprog.get(ShaderChProgram::ST_GS, prog);
            if(prog)
            {
                mLastProgramLightMark = 1;
                mGpuParamMark = GPT_Unknow;
                mRenderSys->on(prog->getMain());
            }
            else
            {
                if(mRenderSys->isOn(GpuProgram::ST_GS))
                    mRenderSys->off(GpuProgram::ST_GS);
                // 设置固定函数管线的顶点参数
            }

            if(envlight)
            {
                if(ch->isLight() && envcolour)
                {
                    mRenderSys->setColour(ch->getColour());
                }
                mRenderSys->setLight(ch->isLight());
            }

            schprog.get(ShaderChProgram::ST_FS, prog);
            if(prog)
            {
                mLastProgramLightMark = 1;
                mGpuParamMark = GPT_Unknow;
                mRenderSys->on(prog->getMain());

                fsfog = prog->isEnvFogEnable();
            }
            else
            {
                if(mRenderSys->isOn(GpuProgram::ST_FS))
                    mRenderSys->off(GpuProgram::ST_FS);
                // 设置固定函数管线的片段设置
            }

            FogMode fmode;
            if(fsfog)
            {
                /*  在D3D中,它适用于着色以vs_3_0和ps_3_0前的版本.而在OGL,它适用于"ARB_fog_XXX"
                    的片断着色程序,在其他方式中,它可通过gpu 着色程序的"state.fog.XXX"访问.
                */
                if(ch->isFogValid())
                {
                    mSyncParams->setFog(ch->getFog());
                    fmode = ch->getFog().mMode;
                }
                else
                {
                    mSyncParams->setFog(mFog);
                    fmode = mFog.mMode;
                }
            }

            if (mRenderSys->getFeature()->isSupport(GF_Fixed_Pipeline) && envlight)
            {
                mRenderSys->getFFPParam()->setExtData(3, ch->getColour().getColourTracking());
                mRenderSys->getFFPParam()->setExtData(9, fmode);
            }

            mRenderSys->setBlend(ch->getBlend());
            mRenderSys->setPoint(ch->getPoint());

            mSyncParams->setPoint(ch->getPoint());

            mRenderPattern->applyTexture(ch);

            mRenderSys->setStencil(ch->getStencil(), SB_Depth);
            mRenderSys->setColour(ch->getColour());
            mRenderSys->setColourWrite(ch->getColourWrite());

            mRenderPattern->setCullingMode(ch->getCullingMode());
            mCullingMode = mRenderPattern->getCullingMode();

            mRenderSys->setShadingType(ch->getShaderMode());
            mRenderSys->_setPolygonMode(ch->getPolygonMode());

            mSyncParams->setChIndex(ch->getIndex());

            // 标志全局参数改变
            mGpuParamMark |= GPT_Render;
        }
        return ch;
    }
    //-----------------------------------------------------------------------
    void SpaceManager::prepareQueue(const RenderQueueFusion * rqfusion)
    {
        NiiProfileGroup("prepareQueue", NIIPROF_GENERAL);
        mMainQueue->clear();

        if(rqfusion)
        {
            // 遍历所有创建/重置 一次
            RenderGroupFusionList::const_iterator i, iend = rqfusion->getList().end();
            for(i = rqfusion->getList().begin(); i != iend; ++i)
            {
                RenderGroup * group = mMainQueue->getGroup((*i)->getRenderGroup());
                group->removeSort(RSM_All);
            }

            // 再次迭代,建立选项(可能多选)
            bool oldset = mRenderPattern->isShadowEnable();
            for(i = rqfusion->getList().begin(); i != iend; ++i)
            {
                RenderGroupFusion * cq = *i;
                RenderGroup * group = mMainQueue->getGroup(cq->getRenderGroup());
                mRenderPattern->setShadowEnable(cq->isShadowEnable());
                mRenderPattern->prepare(group);
            }
            mRenderPattern->setShadowEnable(oldset);
            mLastCustomRender = true;
        }
        else
        {
            if(mLastCustomRender)
            {
                /*  如果走出渲染队列序列,需要在这里重置,但这样做,会重置任何指定的设置,当需要时
                    候,设置每渲染队列的全局,在事例中可让我们设置组织模式。
                */
                // 默认所有队列在都在这里,新的也将以默认形式创建
                RenderQueue::GroupList::const_iterator i, iend = mMainQueue->getGroupList().end();
                for(i = mMainQueue->getGroupList().begin(); i != iend; ++i)
                {
                    if(i->second != 0)
                        i->second->resetSort();
                }
            }

            mRenderPattern->prepare(mMainQueue);

            mLastCustomRender = false;
        }
    }
    //-----------------------------------------------------------------------
    void SpaceManager::renderSpace(Camera * camera, Viewport * vp, const RenderQueueFusion * rqfusion, ShadowType type)
    {
        NiiProfileGroup("renderSpace", NIIPROF_GENERAL);

        N_Engine().processBegin(this);

        mSyncParams->setCurrentSpaceManager(this);

        mView = vp;
        mCamera[type] = camera;

        if(0 != mLastLightHash || 0 != mLastLightLimit)
        {
            mRenderSys->_setLight(0);
            mLastLightHash = 0;
            mLastLightLimit = 0;
        }

        mLastFrame = N_Engine().getCurrentFrame();
        
        N_Only(Equation).update(mLastFrame);

        {
            N_lock_mutex(mSpaceMutex)

            if(mAutoVisible)
            {
                const SpaceMap & lights = getSpace(N_FACTORY_Light);
                {
                    mValidLights.clear();

                    SpaceManager::SpaceMap::const_iterator i, iend = lights.end();
                    for(i = lights.begin(); i != iend; ++i)
                    {
                        Light * l = static_cast<Light *>(i->second);

                        if(l->isVisible())
                        {
                            if (mCameraOrigin)
                                l->setRel(mCamera);
                            else
                                l->setRel(0);

                            if(l->getType() == LT_DIRECTIONAL)
                            {
                                mValidLights.push_back(l);
                            }
                            else
                            {
                                Sphere sphere(l->getSpacePos(), l->getRange());
                                if(camera->isIntersect(sphere))
                                {
                                    mValidLights.push_back(l);
                                }
                            }
                        }
                    }
                }
                mRenderPattern->findLight(mValidLights);
            }

            mRenderPattern->renderShadow(camera, vp, mValidLights);

            {
                updateSpace(camera);
            }

            if(camera->isReflection())
            {
                mRenderSys->setInvertVertexWinding(true);
            }
            else
            {
                mRenderSys->setInvertVertexWinding(false);
            }

            mSyncParams->setCurrentViewport(vp);

            // 设置视口,这个故意在阴影纹理更新之后
            //mView = vp;
            mRenderSys->_setViewport(vp);
            N_Only(Material).setCurrent(vp->getMaterial());

            // 设置关于摄象机参数
            mSyncParams->setCurrentCamera(camera, mCameraOrigin);
            // 设置当前环境灯光的参数
            mSyncParams->setAmbient(mAmbient);
            // 设置关于渲染目标的参数
            mSyncParams->setCurrentFBO(mRenderSys->getFBO());

            // 设置摄象机窗口裁剪
            if(mRenderSys->getFeature()->isSupport(GF_ClipPlanes))
            {
                mRenderSys->resetClipPlanes();
                if(camera->isSubMode())
                    mRenderSys->setClipPlanes(camera->getSubClip());
            }

            // 准备渲染队列
            {
                prepareQueue(rqfusion);
            }

            // 寻找物体
            if(mAutoVisible)
            {
                NiiProfileGroup("findSpace", NIIPROF_CULLING);
                camera->resetBound();
                mRenderPattern->onFindGeometry(vp, ShadowType::ST_Normal);
                findSpace(camera, mRenderPattern->isRenderShadow());
                mSyncParams->setMainCameraBound(camera);
            }

            // 渲染外围物体
            if(!mRenderPattern->isRenderShadow())
            {
                if(vp->isOverlayEnable())
                    N_Only(Overlay).render(vp, camera, mMainQueue);

                if(vp->isSkyEnable() && mAutoVisible)
                {
                    SkyList::iterator i, iend = mSkyList.end();
                    for(i = mSkyList.begin(); i != iend; ++iend)
                    {
                        (*i)->render(camera);
                    }
                }
            }
        }

        if(mView->isAutoClear())
        {
            mRenderSys->clearBuffer(mView->getClearMark(), mView->getBgColour(), mView->getDepthClear());
        }

        mRenderSys->_resetGeoCount();
        mRenderSys->_beginFrame();
        mRenderSys->_setPolygonMode(camera->getPolygonMode());
        mRenderSys->_setProj(mCamera->getSysProjMatrix());

        //mTempViewMatrix = mCamera->getViewMatrix(false);

        //if(mCameraOrigin)
        //{
        //    mTempViewMatrix.setTrans(Vector3f::ZERO);
        //    mCameraRelPos = mCamera->getSpacePos();
        //}
        mRenderSys->_setTextureProjRel(mCameraOrigin, camera->getSpacePos());

        //mRenderSys->_setView(mTempViewMatrix);
        // 如果已经设置灯光,重置灯光hash
        mLastLightHash = 0;

        // 渲染场景内容, 包含 ui层 天空层
        {
            renderQueue(rqfusion);
        }

        mRenderSys->_endFrame();
        camera->setTriangle(mRenderSys->getTriangleCount());
        camera->setBatch(mRenderSys->getBatchCount());
        N_Engine().processEnd(this);
    }
    //-----------------------------------------------------------------------
    void SpaceManager::setRender(RenderSys * sys)
    {
        mRenderSys = sys;
        mRenderPattern->setRender(mRenderSys);
    }
    //-----------------------------------------------------------------------
    void SpaceManager::setSceneGeo(const String &)
    {

    }
    //-----------------------------------------------------------------------
    void SpaceManager::setSceneGeo(DataStream *, const String &)
    {
    }
    //-----------------------------------------------------------------------
    void SpaceManager::updateSpace(Camera * cam)
    {
        NiiProfileGroup("updateSpace", NIIPROF_GENERAL);
        N_Engine().updateNode();
        getOriginNode()->update(false, true);
        
        AutoFocusList::iterator af, afend = mAutoFocusList.end();
        for(af = mAutoFocusList.begin(); af != afend; ++af)
        {
            (*af)->updateFocus();
        }

        cam->updateFocus();
    }
    //-----------------------------------------------------------------------
    void SpaceManager::findSpace(Camera * cam, bool onlyCast)
    {
        getOriginNode()->setNodeView(mNodeView);
        cam->setExpandMode(true);
        getOriginNode()->queue(mMainQueue, cam, true, onlyCast);
    }
    //-----------------------------------------------------------------------
    void SpaceManager::renderQueue(RenderQueueFusion * rqfusion)
    {
        NiiProfileGroup("render_space", NIIPROF_RENDERING);

        RenderListenerList::iterator r, rend = mRenderListenerList.end();
        for(r = mRenderListenerList.begin(); r != rend; ++r)
        {
            (*r)->renderBegin();
        }

        if(rqfusion && !mRenderPattern->isRenderShadow()) //包含 ui层 天空层
        {
            RenderGroupFusionList::const_iterator i, iend = rqfusion->getList().end();
            for(i = rqfusion->getList().begin(); i != iend; ++i)
            {
                RenderGroupFusion * rgfusion = *i;
                RenderGroupType rgt = rgfusion->getRenderGroup();

                if(isExcludeRenderExist(rgt))
                    continue;

                RenderGroup * group = mMainQueue->getGroup(rgt);
                bool repeatQueue = false;
                do
                {
                    if(onPreRenderQueue(rgt))
                    {
                        break;
                    }

                    rgfusion->render(group, mRenderPattern, this);

                    repeatQueue = onEndRenderQueue(rgt);
                } while(repeatQueue);
            }
        
        }
        else
        {
            RenderQueue::GroupList::const_iterator i, iend = mMainQueue->getGroupList().end();
            for(i = mMainQueue->getGroupList().begin(); i != iend; ++i)
            {
                RenderGroupType rgt = i->first;
                RenderGroup * rg = i->second;
                if(rg == 0 || isExcludeRenderExist(rgt))
                    continue;

                bool repeatQueue = false;
                do
                {
                    if(onPreRenderQueue(rgt))
                    {
                        break;
                    }

                    mRenderPattern->render(rg);

                    repeatQueue = onEndRenderQueue(rgt);
                } while(repeatQueue);
            }
        }

        rend = mRenderListenerList.end();
        for(r = mRenderListenerList.begin(); r !=rend; ++r)
        {
            (*r)->renderEnd();
        }
    }
    //-----------------------------------------------------------------------
    void SpaceManager::render(GeometryObj * geo, const ShaderCh * ch, bool lightClip)
    {
        NCount matxcount;
        GeometryRaw ro;

        const_cast<GeometryObj *>(geo)->getGeometry(ro);

        GpuProgram * vprog = 0;
        ch->getProgram().get(ShaderChProgram::ST_VS, vprog);

        bool envspace = true;
        bool envlight = true;

        if(vprog)
        {
            envspace = vprog->isEnvSpaceEnable();
            envlight = vprog->isEnvLightEnable();
        }

        // 获取可渲染物的世界变换数量
        geo->getMatrix(mTempSpace, matxcount);

        if(geo->isOn(GeometryObj::M_IdentityView))
        {
            //if(envspace)
            //{
                //mRenderSys->_setView(Matrix4f::IDENTITY);
            //    mLastLightHash = 0;
            //}
            mGpuParamMark |= GPT_Render;
            mIdentityViewMode = true;
        }
        if(geo->isOn(GeometryObj::M_IdentityProj))
        {
            // 使用统一投影矩阵,需要把RS 深度考虑进来
            //if(envspace)
            //{
            //    Matrix4f mat;
            //    mRenderSys->getSysProj(Matrix4f::IDENTITY, mat);
            //    mRenderSys->_setProj(mat);
            //}
            mGpuParamMark |= GPT_Render;
            mIdentityProjMode = true;
        }
        // 标志每个对象参数的改变
        mGpuParamMark |= GPT_Space_Param;

        if(!mLockShaderCh)
        {
            if(envspace)
                mSyncParams->setCurrentRenderable(geo);

            // 重新发行任何依赖于视图矩阵纹理生成设置
            Nidx unit = 0;
            ShaderChTexture::Textures::const_iterator i, iend = ch->getTexture().getTextures().end();
            for(i = ch->getTexture().getTextures().begin(); i != iend; ++i, ++unit)
            {
                ShaderChTextureUnit * pTex = *i;
                if(pTex->isCameraRelation())
                {
                    mRenderSys->_enableTexture(unit, pTex);
                }
            }

            if((ch->isUnitNormals() || mUnitNormals) && mTempSpace[0].isScale())
                mRenderSys->setUnitNormals(true);
            else
                mRenderSys->setUnitNormals(false);

            //假设第一个世界矩阵为代表
            if(mNegScaleFlipCull)
            {
                CullingMode cullMode = mCullingMode = mRenderPattern->getCullingMode();

                if(mTempSpace[0].isNegativeScale())
                {
                    switch(cullMode)
                    {
                    case CM_Clockwise:
                        cullMode = CM_Anticlockwise;
                        break;
                    case CM_Anticlockwise:
                        cullMode = CM_Clockwise;
                        break;
                    case CM_None:
                        break;
                    };
                }
                // 这个也从相同通道的前一个渲染操作中的负缩放中复制返回
                if(cullMode != mRenderSys->_getCullingMode())
                    mRenderSys->_setCullingMode(cullMode);
            }

            PolygonMode reqMode = ch->getPolygonMode();
            if(ch->getAutoReduce())
            {
                PolygonMode cpm = mCamera->getPolygonMode();
                if(reqMode > cpm)
                {
                    reqMode = cpm;
                }
            }
            mRenderSys->_setPolygonMode(reqMode);

            /* 这里我们发布渲染操作到渲染系统,注意我们每灯光做一次,因此,
                它在一个循环,灯光参数在这个循环的每个遍历中更新一次
            */
            LightList vlights;
            geo->getLight(vlights);
            bool eachlight = ch->isEachLight();

            // 故意是无符号以防,开始灯光超出灯光数量,这个通道将会跳过它们
            NCount lightsLeft = 1;
            NCount start = ch->getLightBegin();
            if(eachlight)
            {
                lightsLeft = vlights.size() - start;
                if(lightsLeft > ch->getLightMax())
                {
                    lightsLeft = ch->getLightMax();
                }
            }

            NCount depthInc = 0;
            LightList dlights;
            while(lightsLeft > 0)
            {
                dlights.clear();
                if(eachlight)
                {
                    mRenderPattern->applyLight(ch, vlights, start, dlights);
                    if(dlights.size() != 0)
                    {
                        lightsLeft -= dlights.size();
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    // 使用完整灯光列表,受过开始灯光调的整过
                    if(ch->getLightBegin() || ch->getLightMax() != NII_MAX_LIGHT ||
                        ch->getLightMark() != 0xFFFFFFFF)
                    {
                        if(ch->getLightBegin() > 0 && ch->getLightBegin() >= vlights.size())
                        {
                            break;
                        }
                        else
                        {
                            LightList::const_iterator copyStart = vlights.begin();
                            std::advance(copyStart, ch->getLightBegin());
                            NCount lightsCopied = 0, lightsToCopy = std::min(
                                ch->getLightMax(), (Nui16)(vlights.size() - ch->getLightBegin()));

                            LightList::const_iterator iter, iterend = vlights.end();
                            for(iter = copyStart; iter != iterend && lightsCopied < lightsToCopy; ++iter)
                            {
                                if((ch->getLightMark() & (*iter)->getLightMark()) != 0)
                                {
                                    dlights.push_back(*iter);
                                    lightsCopied++;
                                }
                            }
                        }
                    }
                    else
                    {
                        dlights = vlights;
                    }
                    lightsLeft = 0;
                }

                onRender(geo, ch, dlights);

                const ShaderChProgram & p = ch->getProgram();
                if(p.isExist())
                {
                    if(dlights.getHash() != mLastProgramLightMark)
                    {
                        mSyncParams->setCurrentLightList(dlights);

                        mGpuParamMark |= GPT_Light_Param;
                        mLastProgramLightMark = dlights.getHash();
                    }
                }
                // 需要更新灯光状态吗?
                // 仅在固定管线顶点灯光应用时,这样做
                if(ch->isLight() && envlight)
                {
                    if(dlights.getHash() != mLastLightHash || ch->getLightMax() != mLastLightLimit)
                    {
                        mRenderSys->_setLight(N_MIN(Nui16, dlights.size(), ch->getLightMax()));
                        mLastLightHash = dlights.getHash();
                        mLastLightLimit = ch->getLightMax();
                    }
                }

                ClipType scissored = CT_None;
                ClipType clipped = CT_None;
                if(lightClip && (ch->isLightClip() || ch->isLightClipPlane()))
                {
                    if(dlights.empty())
                        continue;

                    if(ch->isLightClip())
                        scissored = applyClip(dlights, mCamera);

                    if(ch->isLightClipPlane())
                        clipped = applyPlaneClip(dlights);

                    if(scissored == CT_All || clipped == CT_All)
                        continue;
                }

                mRenderSys->setRenderCount(ch->getRenderCount());
                const ShaderChStencil & scd = ch->getStencil();

                if(scd.getBiasConstantFactor() != 0.0f)
                {
                    NIIf depthBiasBase = scd.getBiasConstant() + scd.getBiasConstantFactor() * depthInc;

                    mRenderSys->_setDepthBias(true, depthBiasBase, scd.getBiasConstantFactor(), scd.getBiasSlopeScale());
                }
                else
                {
                    mRenderSys->_setDepthBias(false);
                }
                depthInc += ch->getRenderCount();

                applyGpuProgram(&ch->getProgram(), GPT_TypeParamMark);

                mRenderSys->_render(ro);

                if(scissored == CT_Some)
                    resetClip();
                if(clipped == CT_Some)
                    resetPlaneClip();
            }
        }
        else
        {
            onRender(geo, ch, LightList());
            mRenderSys->setRenderCount(1);
            mRenderSys->_render(ro);
        }

        if(mIdentityViewMode)
        {
            // 从统一视图中返回正常的
            //if(envspace)
            //{
                //mRenderSys->_setView(mTempViewMatrix);
                // 如果已经设置灯光,重置灯光hash
                //mLastLightHash = 0;
            //}
            mGpuParamMark |= GPT_Render;

            mIdentityViewMode = false;
        }

        if(mIdentityProjMode)
        {
            // 从单调着色投影中返回来
            //if (envspace)
                //mRenderSys->_setProj(mCamera->getSysProjMatrix());
            mGpuParamMark |= GPT_Render;

            mIdentityProjMode = false;
        }
    }
    //-----------------------------------------------------------------------
    void SpaceManager::render(GeometryObj * geo, const ShaderCh * ch, const LightList & lights, bool lightClip)
    {
        NCount matxcount;
        GeometryRaw ro;

        /* 建立我们知道的渲染状态。const_cast比较恶心,否则的话它需要Renderable的所有
            内部状态分配到rop上是可变的
        */
        const_cast<GeometryObj *>(geo)->getGeometry(ro);

        //GpuProgram * vprog = 0;
        //ch->getProgram().get(ShaderChProgram::ST_VS, vprog);

        //bool envspace = true;
        //bool envlight = true;

        //if(vprog)
        //{
        //    envspace = vprog->isEnvSpaceEnable();
        //    envlight = vprog->isEnvLightEnable();
        //}

        // 获取可渲染物的世界变换数量
        geo->getMatrix(mTempSpace, matxcount);

        if(geo->isOn(GeometryObj::M_IdentityView))
        {
            //if(envspace)
            //{
                //mRenderSys->_setView(Matrix4f::IDENTITY);
                // 如果已经设置灯光,重置灯光hash
            //    mLastLightHash = 0;
            //}
            mGpuParamMark |= GPT_Render;
            mIdentityViewMode = true;
        }

        if(geo->isOn(GeometryObj::M_IdentityProj))
        {
            //if(envspace)
            //{
                //Matrix4f mat;
                ///mRenderSys->getSysProj(Matrix4f::IDENTITY, mat);
                //mRenderSys->_setProj(mat);
            //}
            mGpuParamMark |= GPT_Render;
            mIdentityProjMode = true;
        }
        // 标志每个对象参数的改变
        mGpuParamMark |= GPT_Space_Param;

        if(!mLockShaderCh)
        {
            //if(envspace)
                mSyncParams->setCurrentRenderable(geo);

            // 重新发行任何依赖于视图矩阵纹理生成设置
            Nidx unit = 0;
            ShaderChTexture::Textures::const_iterator i, iend = ch->getTexture().getTextures().end();
            for(i = ch->getTexture().getTextures().begin(); i != iend; ++i, ++unit)
            {
                ShaderChTextureUnit * pTex = *i;
                if(pTex->isCameraRelation())
                {
                    mRenderSys->_enableTexture(unit, pTex);
                }
            }

            if((ch->isUnitNormals() || mUnitNormals) && mTempSpace[0].isScale())
                mRenderSys->setUnitNormals(true);
            else
                mRenderSys->setUnitNormals(false);

            //假设第一个世界矩阵为代表
            if(mNegScaleFlipCull)
            {
                CullingMode cullMode = mCullingMode = mRenderPattern->getCullingMode();

                if(mTempSpace[0].isNegativeScale())
                {
                    switch(cullMode)
                    {
                    case CM_Clockwise:
                        cullMode = CM_Anticlockwise;
                        break;
                    case CM_Anticlockwise:
                        cullMode = CM_Clockwise;
                        break;
                    case CM_None:
                        break;
                    };
                }
                // 这个也从相同通道的前一个渲染操作中的负缩放中复制返回
                if(cullMode != mRenderSys->_getCullingMode())
                    mRenderSys->_setCullingMode(cullMode);
            }

            PolygonMode reqMode = ch->getPolygonMode();
            if(ch->getAutoReduce())
            {
                PolygonMode cpm = mCamera->getPolygonMode();
                if(reqMode > cpm)
                {
                    reqMode = cpm;
                }
            }
            mRenderSys->_setPolygonMode(reqMode);

            // 即使手动驱动灯光,也检查灯光类型通道
            bool skip = false;
            if(ch->getLightTypeLimit() != LT_NONE)
            {
                if(lights.size() == 0 || (lights.size() == 1 && lights.at(0)->getType() != ch->getLightTypeLimit()))
                {
                    skip = true;
                }
            }

            if(!skip)
            {
                onRender(geo, ch, lights);
                // 我们需要更新GPU着色程序的参数吗?
                const ShaderChProgram & p = ch->getProgram();
                if(p.isExist())
                {
                    if(lights.size())
                    {
                        if(lights.getHash() != mLastProgramLightMark)
                        {
                            // 更新自动GPU参数,其他位信息也可以找到
                            mSyncParams->setCurrentLightList(lights);
                            mGpuParamMark |= GPT_Light_Param;
                            mLastProgramLightMark = lights.getHash();
                        }
                    }
                }

                // 如果存在,使用手动灯光,不使用顶点着色程序,不使用固定管道
                if(lights.size() && ch->isLight() /*&& envlight*/)
                {
                    if(lights.getHash() != mLastLightHash || ch->getLightMax() != mLastLightLimit)
                    {
                        mRenderSys->_setLight(N_MIN(Nui16, lights.size(), ch->getLightMax()));
                        mLastLightHash = lights.getHash();
                        mLastLightLimit = ch->getLightMax();
                    }
                }

                // 可选灯光裁剪
                ClipType scissored = CT_None;
                ClipType clipped = CT_None;
                if(lightClip && lights.size() && ch->isLightClip())
                {
                    scissored = applyClip(lights, mCamera);
                }
                if(lightClip && lights.size() && ch->isLightClipPlane())
                {
                    clipped = applyPlaneClip(lights);
                }

                if(scissored != CT_All && clipped != CT_All)
                {
                    mRenderSys->setRenderCount(ch->getRenderCount());
                    applyGpuProgram(&ch->getProgram(), GPT_TypeParamMark);
                    mRenderSys->_render(ro);
                }

                if(scissored == CT_Some)
                    resetClip();
                if(clipped == CT_Some)
                    resetPlaneClip();
            }
        }
        else
        {
            onRender(geo, ch, LightList());
            mRenderSys->setRenderCount(1);
            mRenderSys->_render(ro);
        }

        if(mIdentityViewMode)
        {
            // 从统一视图中返回正常的
            //if(envspace)
            //{
                //mRenderSys->_setView(mTempViewMatrix);
                // 如果已经设置灯光,重置灯光hash
                //mLastLightHash = 0;
            //}
            mGpuParamMark |= GPT_Render;
            mIdentityViewMode = false;
        }

        if(mIdentityProjMode)
        {
            // 从单调着色投影中返回来
            //if(envspace)
            //    mRenderSys->_setProj(mCamera->getSysProjMatrix());
            mGpuParamMark |= GPT_Render;
            mIdentityProjMode = false;
        }
    }
    //---------------------------------------------------------------------
    void SpaceManager::render(Viewport * vp, 
        const Matrix4f & projMatrix, const Matrix4f & viewMatrix, const Matrix4f & worldMatrix, 
            GeometryRaw * rend, ShaderCh * ch, bool aloneframe)
    {
        if(vp)
            mRenderSys->_setViewport(vp);

        if(aloneframe)
            mRenderSys->_beginFrame();

        mLastLightHash = 0;

        applyCh(ch, false, true);

        //const ShaderChProgram & p = ch->getProgram();
        //if(p.isExist())
        //{
            if(vp)
            {
                mSyncParams->setCurrentViewport(vp);
                mSyncParams->setCurrentFBO(mRenderSys->getFBO());
            }
            mSyncParams->setCurrentSpaceManager(this);
            mSyncParams->setWorldMatrices(&worldMatrix, 1);
            Camera dummyCam(0, 0);
            dummyCam.setCustomViewMatrix(true);
            dummyCam.setCustomViewMatrix(viewMatrix);
            dummyCam.setCustomProjMatrix(true);
            dummyCam.setCustomProjMatrix(projMatrix);

            mSyncParams->setCurrentCamera(&dummyCam, false);
            applyGpuProgram(&ch->getProgram(), GPT_TypeParamMark);
        //}
        mRenderSys->_render(*rend);

        if(aloneframe)
            mRenderSys->_endFrame();
    }
    //---------------------------------------------------------------------
    void SpaceManager::render(Viewport * vp, const Matrix4f & projMatrix, const Matrix4f & viewMatrix, 
        GeometryObj * rend, const ShaderCh * ch, bool lightClip, bool aloneframe)
    {
        if(vp)
            mRenderSys->_setViewport(vp);

        if(aloneframe)
            mRenderSys->_beginFrame();

        mLastLightHash = 0;

        applyCh(ch, false, true);

        //const ShaderChProgram & p = ch->getProgram();
        //if(p.isExist())
        //{
            if(vp)
            {
                mView = vp;
                mSyncParams->setCurrentViewport(vp);
                mSyncParams->setCurrentFBO(mRenderSys->getFBO());
            }
            mSyncParams->setCurrentSpaceManager(this);

            Camera dummyCam(0, 0);
            dummyCam.setCustomViewMatrix(true);
            dummyCam.setCustomViewMatrix(viewMatrix);
            dummyCam.setCustomProjMatrix(true);
            dummyCam.setCustomProjMatrix(projMatrix);

            mSyncParams->setCurrentCamera(&dummyCam, false);

            applyGpuProgram(&ch->getProgram(), GPT_TypeParamMark);
        //}

        render(rend, ch, lightClip);

        if(aloneframe)
            mRenderSys->_endFrame();
    }
    //---------------------------------------------------------------------
    void SpaceManager::render(Viewport * vp, const Matrix4f & projMatrix, const Matrix4f & viewMatrix, 
        GeometryObj * rend, const ShaderCh * ch, const LightList & lights, bool lightClip, bool aloneframe)
    {
        if(vp)
            mRenderSys->_setViewport(vp);

        if(aloneframe)
            mRenderSys->_beginFrame();

        mLastLightHash = 0;

        applyCh(ch, false, true);

        //const ShaderChProgram & p = ch->getProgram();
        //if(p.isExist())
        //{
            if(vp)
            {
                mView = vp;
                mSyncParams->setCurrentViewport(vp);
                mSyncParams->setCurrentFBO(mRenderSys->getFBO());
            }
            mSyncParams->setCurrentSpaceManager(this);

            Camera dummyCam(0, 0);
            dummyCam.setCustomViewMatrix(true);
            dummyCam.setCustomViewMatrix(viewMatrix);
            dummyCam.setCustomProjMatrix(true);
            dummyCam.setCustomProjMatrix(projMatrix);

            mSyncParams->setCurrentCamera(&dummyCam, false);
            applyGpuProgram(&ch->getProgram(), GPT_TypeParamMark);
        //}

        render(rend, ch, lights, lightClip);

        if(aloneframe)
            mRenderSys->_endFrame();
    }
    //-----------------------------------------------------------------------
    ParticleCtrl * SpaceManager::createParticleCtrl(SpaceID sid, NCount poolSize)
    {
        PropertyData params;
        params.add(N_PropertyParticleCtrl_PoolSize, N_conv(poolSize));
        return static_cast<ParticleCtrl *>(createSpace(sid, N_FACTORY_ParticleCtrl, &params));
    }
    //-----------------------------------------------------------------------
    ParticleCtrl* SpaceManager::getParticleCtrl(SpaceID sid) const
    {
        return static_cast<ParticleCtrl *>(getSpace(sid, N_FACTORY_ParticleCtrl));
    }
    //-----------------------------------------------------------------------
    void SpaceManager::setNodeView(bool b)
    {
        mNodeView = b;
    }
    //-----------------------------------------------------------------------
    bool SpaceManager::isNodeView() const
    {
        return mNodeView;
    }
    //---------------------------------------------------------------------
    void SpaceManager::add(RenderListener * obj)
    {
        mRenderListenerList.push_back(obj);
    }
    //---------------------------------------------------------------------
    void SpaceManager::remove(RenderListener * obj)
    {
        RenderListenerList::iterator i, iend = mRenderListenerList.end();
        for(i = mRenderListenerList.begin(); i != iend; ++i)
        {
            if(*i == obj)
            {
                 mRenderListenerList.erase(i);
                 break;
            }
        }
    }
    //---------------------------------------------------------------------
    void SpaceManager::add(GeometryObjListener * obj)
    {
        mGeoRenderLisList.push_back(obj);
    }
    //---------------------------------------------------------------------
    void SpaceManager::remove(GeometryObjListener * obj)
    {
        GeoRenderListenerList::iterator i, iend = mGeoRenderLisList.end();
        for(i = mGeoRenderLisList.begin(); i != iend; ++i)
        {
            if(*i == obj)
            {
                mGeoRenderLisList.erase(i);
                break;
            }
        }
    }
    //---------------------------------------------------------------------
    bool SpaceManager::onPreRenderQueue(GroupID qid)
    {
        bool skip = false;
        RenderListenerList::iterator q, qend = mRenderListenerList.end();
        for(q = mRenderListenerList.begin(); q != qend; ++q)
        {
            if((*q)->renderBegin(qid) != true)
                skip = true;
        }
        return skip;
    }
    //---------------------------------------------------------------------
    bool SpaceManager::onEndRenderQueue(GroupID qid)
    {
        bool repeat = false;
        RenderListenerList::iterator i, iend = mRenderListenerList.end();
        for(i = mRenderListenerList.begin(); i != iend; ++i)
        {
            if((*i)->renderEnd(qid) == true)
                repeat = true;
        }
        return repeat;
    }
    //---------------------------------------------------------------------
    void SpaceManager::onRender(GeometryObj * obj, const ShaderCh * ch, const LightList & lights)
    {
        GeoRenderListenerList::iterator i, iend = mGeoRenderLisList.end();
        for(i = mGeoRenderLisList.begin(); i != iend; ++i)
        {
            (*i)->notifyRenderSingleObject(obj, ch, mSyncParams, lights, mLockShaderCh);
        }
    }
    //---------------------------------------------------------------------
    void SpaceManager::setAABBView(bool b)
    {
        mAABBView = b;
    }
    //---------------------------------------------------------------------
    bool SpaceManager::isAABBView() const
    {
        return mAABBView;
    }
    //---------------------------------------------------------------------
    void SpaceManager::setAutoFocus(SpaceNode * node, bool b)
    {
        if(b)
        {
            mAutoFocusList.insert(node);
        }
        else
        {
            mAutoFocusList.erase(node);
        }
    }
    //---------------------------------------------------------------------
    void SpaceManager::addSky(Sky * obj)
    {
        SkyList::iterator i, iend = mSkyList.end();
        for(i = mSkyList.begin(); i != iend; ++iend)
        {
            if(*i == obj)
                return;
        }
        obj->setSpace(this);
        obj->setEnable(true);
        mSkyList.push_back(obj);
    }
    //---------------------------------------------------------------------
    void SpaceManager::removeSky(Sky * obj)
    {
        SkyList::iterator i, iend = mSkyList.end();
        for(i = mSkyList.begin(); i != iend; ++iend)
        {
            if(*i == obj)
            {
                obj->setSpace(0);
                obj->setEnable(false);
                mSkyList.erase(i);
                break;
            }
        }
    }
    //---------------------------------------------------------------------
    PixelOrbit * SpaceManager::createOrbit(SpaceID sid)    
    { 
        return static_cast<PixelOrbit *>(createSpace(sid, N_FACTORY_PixelOrbit)); 
    }
    //---------------------------------------------------------------------
    PixelOrbit * SpaceManager::getOrbit(SpaceID sid) const 
    { 
        return static_cast<PixelOrbit *>(getSpace(sid, N_FACTORY_PixelOrbit)); 
    }
    //---------------------------------------------------------------------
    void SpaceManager::setRenderPattern(RenderPattern * pattern)
    {
        mRenderPattern = pattern;

        if(mRenderPattern->isValid())
        {
            mRenderPattern->init();
            if (mMainQueue != 0)
            {
                N_delete mMainQueue;
            }
            mMainQueue = mRenderPattern->createQueue();
        }
        else
        {
            mRenderPattern = mNoShadowPattern;
            if (mMainQueue != 0)
            {
                N_delete mMainQueue;
            }
            mMainQueue = mRenderPattern->createQueue();
        }
    }
    //---------------------------------------------------------------------
    SpaceManager::ClipType SpaceManager::applyClip(const LightList & ll, const Camera * cam)
    {
        if(!mRenderSys->getFeature()->isSupport(GF_ClipRect))
            return CT_None;

        Rectf fr;
        // 初始化,反向的,因为想从零增长
        fr.mLeft = fr.mBottom = 1.0f;
        fr.mRight = fr.mTop = -1.0f;

        LightList::const_iterator i, iend = ll.end();
        for(i = ll.begin(); i != iend; ++i)
        {
            Light * l = *i;
            // 一个方向灯光正在使用,所以做到不剪裁,周期的
            if(l->getType() == LT_DIRECTIONAL)
                return CT_None;

            const Rectf & clipRect = getClipRect(l, cam);

            // 最后合并这个灯光列表的
            fr.mLeft = std::min(fr.mLeft, clipRect.mLeft);
            fr.mBottom = std::min(fr.mBottom, clipRect.mBottom);
            fr.mRight= std::max(fr.mRight, clipRect.mRight);
            fr.mTop = std::max(fr.mTop, clipRect.mTop);
        }

        if(fr.mLeft >= 1.0f || fr.mRight <= -1.0f || fr.mTop <= -1.0f || fr.mBottom >= 1.0f)
        {
            // 矩形是屏外的
            return CT_All;
        }

        // 一些裁剪?
        if(fr.mLeft > -1.0f || fr.mRight < 1.0f || fr.mBottom > -1.0f || fr.mTop < 1.0f)
        {
            // 打开标准设备坐标到像素
            NIIi iLeft, iTop, iWidth, iHeight;
            mView->getPixelArea(iLeft, iTop, iWidth, iHeight);
            NCount szLeft, szRight, szTop, szBottom;

            szLeft = (NCount)(iLeft + ((fr.mLeft + 1) * 0.5 * iWidth));
            szRight = (NCount)(iLeft + ((fr.mRight + 1) * 0.5 * iWidth));
            szTop = (NCount)(iTop + ((-fr.mTop + 1) * 0.5 * iHeight));
            szBottom = (NCount)(iTop + ((-fr.mBottom + 1) * 0.5 * iHeight));

            mRenderSys->setScissorTest(true, szLeft, szTop, szRight, szBottom);

            return CT_Some;
        }
        else
        {
            return CT_None;
        }
    }
    //---------------------------------------------------------------------
    void SpaceManager::resetClip()
    {
        if(!mRenderSys->getFeature()->isSupport(GF_ClipRect))
            return;

        mRenderSys->setScissorTest(false);
    }
    //---------------------------------------------------------------------
    const PlaneList & SpaceManager::getClipPlane(Light * l)
    {
        static PlaneList stemp;
        if(!mRenderSys->getFeature()->isSupport(GF_ClipPlanes))
            return stemp;

        Nmark frame = N_Engine().getCurrentFrame();
        if(frame != mLightClipFrame)
        {
            mLightPlaneClipList.clear();
            mLightClipFrame = frame;
        }

        LightPlaneClipList::iterator ci = mLightPlaneClipList.find(l);
        if(ci == mLightPlaneClipList.end())
        {
            ci = mLightPlaneClipList.insert(LightPlaneClipList::value_type(l, PlaneList())).first;
            l->getClip(ci->second);
        }

        return ci->second;
    }
    //---------------------------------------------------------------------
    const Rectf & SpaceManager::getClipRect(Light * l, const Camera * cam)
    {
        Nmark frame = N_Engine().getCurrentFrame();
        if(frame != mLightClipFrame)
        {
            mLightClipList.clear();
            mLightClipFrame = frame;
        }

        LightClipList::iterator ci = mLightClipList.find(l);
        if(ci == mLightClipList.end())
        {
            Rectf rect;
            Sphere sphere(l->getSpacePos(), l->getRange());
            cam->project(sphere, rect);
            ci = mLightClipList.insert(LightClipList::value_type(l, rect)).first;
        }

        return ci->second;
    }
    //---------------------------------------------------------------------
    SpaceManager::ClipType SpaceManager::applyPlaneClip(const LightList & ll)
    {
        if(!mRenderSys->getFeature()->isSupport(GF_ClipPlanes))
            return CT_None;

        Light * temp = 0;
        LightList::const_iterator i, iend = ll.end();
        for(i = ll.begin(); i != iend; ++i)
        {
            if((*i)->getType() == LT_DIRECTIONAL)
                return CT_None;

            if(temp)
            {
                return CT_None;
            }
            temp = *i;
        }

        if(temp)
        {
            const PlaneList & clip = getClipPlane(temp);

            mRenderSys->setClipPlanes(clip);
            return CT_Some;
        }
        else
        {
            return CT_All;
        }
    }
    //---------------------------------------------------------------------
    void SpaceManager::notifyLight()
    {
        ++mLightDirtyMark;
    }
    //---------------------------------------------------------------------
    void SpaceManager::resetPlaneClip()
    {
        if(!mRenderSys->getFeature()->isSupport(GF_ClipPlanes))
            return;

        mRenderSys->resetClipPlanes();
    }
    //---------------------------------------------------------------------
    void SpaceManager::pauseRender(SpaceManager::RenderBase * out)
    {
        out->mQueue = mMainQueue;
        out->mView = mView;
        out->mCamera = mCamera;
        out->mFusion = getFrameResult();
        out->mFrame = mRenderSys->_pauseFrame();
        mMainQueue = 0;
    }
    //---------------------------------------------------------------------
    void SpaceManager::resumeRender(SpaceManager::RenderBase * in)
    {
        if(mMainQueue != 0)
        {
            N_delete mMainQueue;
        }
        mMainQueue = in->mQueue;
        setFrameResult(in->mFusion);
        mView = in->mView;
        mCamera = in->mCamera;

        mSyncParams->setCurrentViewport(mView);

        // 设置视口-故意在阴影纹理更新后
        mRenderSys->_setViewport(mView);
        N_Only(Material).setCurrent(mView->getMaterial());

        mSyncParams->setCurrentCamera(mCamera, mCameraOrigin);
        mSyncParams->setAmbient(mAmbient);
        mSyncParams->setCurrentFBO(mRenderSys->getFBO());

        // 设置摄象机窗体修剪面(如果有的话)
        if(mRenderSys->getFeature()->isSupport(GF_ClipPlanes))
        {
            mRenderSys->resetClipPlanes();
            if(mCamera->isSubMode())
            {
                mRenderSys->setClipPlanes(mCamera->getSubClip());
            }
        }

        mRenderSys->_resumeFrame(in->mFrame);

        mRenderSys->_setPolygonMode(mCamera->getPolygonMode());
        mRenderSys->_setProj(mCamera->getSysProjMatrix());

        //mTempViewMatrix = mCamera->getViewMatrix(false);

        //if(mCameraOrigin)
        //{
        //    mTempViewMatrix.setTrans(Vector3f::ZERO);
        //    mCameraRelPos = mCamera->getSpacePos();
        //}
        mRenderSys->_setTextureProjRel(mCameraOrigin, mCamera->getSpacePos());

        //mRenderSys->_setView(mTempViewMatrix);
        // 如果已经设置灯光,重置灯光hash
        mLastLightHash = 0;
        N_delete in;
    }
    //---------------------------------------------------------------------
    StaticSpace * SpaceManager::createStaticSpace(SpaceID sid)
    {
        if(mStatics.find(sid) != mStatics.end())
        {
            N_EXCEPT(UniqueRepeat, _I18n("StaticSpace with id '") + N_conv(sid) + _I18n("' already exists!"));
        }
        StaticSpace * ret = N_new StaticSpace(this, sid);
        mStatics[sid] = ret;
        return ret;
    }
    //---------------------------------------------------------------------
    StaticSpace * SpaceManager::getStaticSpace(SpaceID sid) const
    {
        StaticSpaceList::const_iterator i = mStatics.find(sid);
        if(i == mStatics.end())
        {
            N_EXCEPT(NotExist, _I18n("StaticSpace with id '") + N_conv(sid) + _I18n("' not found"));
        }
        return i->second;
    }
    //-----------------------------------------------------------------------
    bool SpaceManager::isStaticSpaceExist(SpaceID sid) const
    {
        return (mStatics.find(sid) != mStatics.end());
    }
    //---------------------------------------------------------------------
    void SpaceManager::destroyStaticSpace(SpaceID sid)
    {
        StaticSpaceList::iterator i = mStatics.find(sid);
        if(i != mStatics.end())
        {
            N_delete i->second;
            mStatics.erase(i);
        }
    }
    //---------------------------------------------------------------------
    void SpaceManager::destroyAllStaticSpace()
    {
        StaticSpaceList::iterator i, iend = mStatics.end();
        for(i = mStatics.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
        mStatics.clear();
    }
    //---------------------------------------------------------------------
    InstanceSpace * SpaceManager::createInstanceSpace(SpaceID sid)
    {
        if(mInstances.find(sid) != mInstances.end())
        {
            N_EXCEPT(UniqueRepeat, _I18n("InstanceSpace with name '") + N_conv(sid) + _I18n("' already exists!"));
        }
        InstanceSpace * ret = N_new InstanceSpace(this, sid);
        mInstances[sid] = ret;
        return ret;
    }
    //---------------------------------------------------------------------
    InstanceSpace * SpaceManager::getInstanceSpace(SpaceID sid) const
    {
        InstanceSpaceList::const_iterator i = mInstances.find(sid);
        if(i == mInstances.end())
        {
            N_EXCEPT(NotExist, _I18n("InstanceSpace with name '") + N_conv(sid) + _I18n("' not found"));
        }
        return i->second;
    }
    //---------------------------------------------------------------------
    void SpaceManager::destroyInstanceSpace(SpaceID sid)
    {
        InstanceSpaceList::iterator i = mInstances.find(sid);
        if (i != mInstances.end())
        {
            N_delete i->second;
            mInstances.erase(i);
        }
    }
    //---------------------------------------------------------------------
    void SpaceManager::destroyAllInstanceSpace()
    {
        InstanceSpaceList::iterator i, iend = mInstances.end();
        for(i = mInstances.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
        mInstances.clear();
    }
    //---------------------------------------------------------------------
    AABSpaceQuery * SpaceManager::createAABBQuery(const AABox & box, Nmark mask)
    {
        AABSpaceQuery * q = N_new AABSpaceQuery(this);
        q->setBox(box);
        q->setQueryMask(mask);
        return q;
    }
    //---------------------------------------------------------------------
    AABSpaceQuery * SpaceManager::createAABBQuery(Nmark mask)
    {
        AABSpaceQuery * q = N_new AABSpaceQuery(this);
        q->setQueryMask(mask);
        return q;
    }
    //---------------------------------------------------------------------
    SphereSpaceQuery * SpaceManager::createSphereQuery(const Sphere & sphere, Nmark mask)
    {
        SphereSpaceQuery * q = N_new SphereSpaceQuery(this);
        q->setSphere(sphere);
        q->setQueryMask(mask);
        return q;
    }
    //---------------------------------------------------------------------
    SphereSpaceQuery * SpaceManager::createSphereQuery(Nmark mask)
    {
        SphereSpaceQuery * q = N_new SphereSpaceQuery(this);
        q->setQueryMask(mask);
        return q;
    }
    //---------------------------------------------------------------------
    PlaneSpaceQuery * SpaceManager::createPlaneSpaceQuery(const PlaneSpaceList & volumes, Nmark mask)
    {
        PlaneSpaceQuery * q = N_new PlaneSpaceQuery(this);
        q->setSpaceList(volumes);
        q->setQueryMask(mask);
        return q;
    }
    //---------------------------------------------------------------------
    PlaneSpaceQuery * SpaceManager::createPlaneSpaceQuery(Nmark mask)
    {
        PlaneSpaceQuery * q = N_new PlaneSpaceQuery(this);
        q->setQueryMask(mask);
        return q;
    }
    //---------------------------------------------------------------------
    RaySpaceQuery * SpaceManager::createRayQuery(const Ray & ray, Nmark mask)
    {
        RaySpaceQuery * q = N_new RaySpaceQuery(this);
        q->setRay(ray);
        q->setQueryMask(mask);
        return q;
    }
    //---------------------------------------------------------------------
    RaySpaceQuery * SpaceManager::createRayQuery(Nmark mask)
    {
        RaySpaceQuery * q = N_new RaySpaceQuery(this);
        q->setQueryMask(mask);
        return q;
    }
    //---------------------------------------------------------------------
    AreaSpaceQuery * SpaceManager::createIntersectQuery(Nmark mask)
    {
        AreaSpaceQuery * q = N_new AreaSpaceQuery(this);
        q->setQueryMask(mask);
        return q;
    }
    //---------------------------------------------------------------------
    void SpaceManager::destroyQuery(SpaceQuery * query)
    {
        N_delete query;
    }
    //---------------------------------------------------------------------
    RenderQueueFusion * SpaceManager::createQueueFusion(Nid id)
    {
        QueueFusionList::iterator i = mQueueFusionList.find(id);
        if (i == mQueueFusionList.end())
        {
            RenderQueueFusion * re = N_new RenderQueueFusion(id);
            mQueueFusionList[id] = re;
            return re;
        }

        return i->second;
    }
    //---------------------------------------------------------------------
    RenderQueueFusion * SpaceManager::getQueueFusion(Nid id)
    {
        QueueFusionList::iterator i = mQueueFusionList.find(id);
        if (i != mQueueFusionList.end())
        {
            return i->second;
        }
        return 0;
    }
    //---------------------------------------------------------------------
    void SpaceManager::destroyQueueFusion(Nid id)
    {
        QueueFusionList::iterator i = mQueueFusionList.find(id);
        if (i != mQueueFusionList.end())
        {
            N_delete i->second;
            mQueueFusionList.erase(i);
        }
    }
    //---------------------------------------------------------------------
    void SpaceManager::destroyAllQueueFusion()
    {
        QueueFusionList::iterator i, iend = mQueueFusionList.end();
        for (i = mQueueFusionList.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
        mQueueFusionList.clear();
    }
    //---------------------------------------------------------------------
    SpaceObj * SpaceManager::createSpace(SpaceID sid, FactoryID fid, const PropertyData * params)
    {
        if(fid == N_FACTORY_Camera)
        {
            return createCamera(sid);
        }
        SpaceObjFactory * factory = N_Engine().getFactory(fid);

        SpaceMap & objectMap = getSpace(fid);
        {
            if(objectMap.find(sid) != objectMap.end())
            {
                N_EXCEPT(UniqueRepeat, _I18n("An object of type '") + N_conv(fid) +
                    _I18n("' with name '") + N_conv(sid) + _I18n("' already exists."));
            }

            SpaceObj * newObj = factory->create(sid, this, params);
            objectMap.insert(SpaceMap::value_type(newObj->getID(), newObj));
            return newObj;
        }
    }
    //---------------------------------------------------------------------
    void SpaceManager::destroySpace(SpaceID sid, FactoryID fid)
    {
        if(fid == N_FACTORY_Camera)
        {
            destroyCamera(sid);
            return;
        }
        SpaceMap & objectMap = getSpace(fid);
        SpaceObjFactory * factory = N_Engine().getFactory(fid);

        {
            SpaceMap::iterator mi = objectMap.find(sid);
            if (mi != objectMap.end())
            {
                factory->destroy(mi->second);
                objectMap.erase(mi);
            }
        }
    }
    //---------------------------------------------------------------------
    void SpaceManager::destroyAllSpace(FactoryID fid)
    {
        if(fid == N_FACTORY_Camera)
        {
            destroyAllCamera();
            return;
        }
        SpaceMap & objectMap = getSpace(fid);
        SpaceObjFactory * factory = N_Engine().getFactory(fid);

        {
            SpaceMap::iterator i = objectMap.begin();
            for(; i != objectMap.end(); ++i)
            {
                if (i->second->getManager() == this)
                {
                    factory->destroy(i->second);
                }
            }
            objectMap.clear();
        }
    }
    //---------------------------------------------------------------------
    void SpaceManager::destroyAllSpace()
    {
        SpaceMapList::iterator ci, ciend = mSpaceMapList.end();
        for(ci = mSpaceMapList.begin();ci != ciend; ++ci)
        {
            SpaceMap & coll = ci->second;
            if(N_Engine().isFactoryExist(ci->first))
            {
                SpaceObjFactory * factory = N_Engine().getFactory(ci->first);
                SpaceMap::iterator i = coll.begin();
                for (; i != coll.end(); ++i)
                {
                    if (i->second->getManager() == this)
                    {
                        factory->destroy(i->second);
                    }
                }
            }
            coll.clear();
        }

    }
    //---------------------------------------------------------------------
    SpaceObj * SpaceManager::getSpace(SpaceID sid, FactoryID fid) const
    {
        // 直接生成摄象机工作,而没有打破 添加到SMs
        if(fid == N_FACTORY_Camera)
        {
            return getCamera(sid);
        }

        const SpaceMap & objectMap = getSpace(fid);
        {
            SpaceMap::const_iterator mi = objectMap.find(sid);
            if(mi == objectMap.end())
            {
                N_EXCEPT(NotExist, _I18n("Object named '") + N_conv(sid) + _I18n("' does not exist."));
            }
            return mi->second;
        }
    }
    //-----------------------------------------------------------------------
    bool SpaceManager::isSpaceExist(SpaceID sid, FactoryID fid) const
    {
        if(fid == N_FACTORY_Camera)
        {
            return isCameraExist(sid);
        }

        SpaceMapList::const_iterator i = mSpaceMapList.find(fid);
        if(i == mSpaceMapList.end())
            return false;

        return (i->second.find(sid) != i->second.end());
    }
    //---------------------------------------------------------------------
    SpaceManager::SpaceMap & SpaceManager::getSpace(FactoryID fid)
    {
        SpaceMapList::iterator i = mSpaceMapList.find(fid);
        if(i == mSpaceMapList.end())
        {
            return mSpaceMapList.insert(Npair(fid, SpaceMap())).first->second;
        }

        return i->second;
    }
    //---------------------------------------------------------------------
    void SpaceManager::addSpace(SpaceObj * m, FactoryID fid)
    {
        if (fid == 0)
        {
            SpaceMap & objectMap = getSpace(m->getFactoryID());
            objectMap.insert(Npair(m->getID(), m));
        }
        else
        {
            SpaceMap & objectMap = getSpace(fid);
            objectMap.insert(Npair(m->getID(), m));
        }
    }
    //---------------------------------------------------------------------
    void SpaceManager::removeSpace(SpaceID sid, FactoryID fid)
    {
        SpaceMap & objectMap = getSpace(fid);
        {
            SpaceMap::iterator mi = objectMap.find(sid);
            if(mi != objectMap.end())
            {
                objectMap.erase(mi);
            }
        }
    }
    //---------------------------------------------------------------------
    void SpaceManager::removeSpace(FactoryID fid)
    {
        SpaceMap & objectMap = getSpace(fid);
        {
            objectMap.clear();
        }
    }
    //---------------------------------------------------------------------
    Nmark SpaceManager::getEnvVisibleMask() const
    {
        return mView ? mView->getVisableMark() & mVisibleMark : mVisibleMark;
    }
    //---------------------------------------------------------------------
    void SpaceManager::add(LodListener * obj)
    {
        LodListenerSet::iterator it = mLodListeners.find(obj);
        if(it != mLodListeners.end())
            mLodListeners.insert(obj);
    }
    //---------------------------------------------------------------------
    void SpaceManager::remove(LodListener * obj)
    {
        LodListenerSet::iterator it = mLodListeners.find(obj);
        if (it != mLodListeners.end())
        {
            mLodListeners.erase(it);
        }
    }
    //---------------------------------------------------------------------
    void SpaceManager::onResEvent(const ResResultEvent & arg)
    {

    }
    //---------------------------------------------------------------------
    void SpaceManager::onResEvent(const ResLoadEvent & arg)
    {

    }
    //---------------------------------------------------------------------
    void SpaceManager::onLodEvent(const SpaceLodEvent & evt)
    {
        bool queueEvent = false;
        LodListenerSet::iterator i, iend = mLodListeners.end();
        for(i = mLodListeners.begin(); i != iend; ++i)
        {
            if((*i)->preLodChange(evt))
                 queueEvent = true;
        }

        if(queueEvent)
            mSpaceLodEventList.push_back(evt);
    }
    //---------------------------------------------------------------------
    void SpaceManager::onLodEvent(const MeshLodEvent & evt)
    {
        bool queueEvent = false;
        LodListenerSet::iterator i, iend = mLodListeners.end();
        for(i = mLodListeners.begin(); i != iend; ++i)
        {
            if((*i)->preLodChange(evt))
                 queueEvent = true;
        }

        if(queueEvent)
            mMeshLodEventList.push_back(evt);
    }
    //---------------------------------------------------------------------
    void SpaceManager::onLodEvent(const MaterialLodEvent & evt)
    {
        bool queueEvent = false;
        LodListenerSet::iterator i, iend = mLodListeners.end();
        for(i = mLodListeners.begin(); i != iend; ++i)
        {
            if((*i)->preLodChange(evt))
                 queueEvent = true;
        }

        if(queueEvent)
            mMatLodEventList.push_back(evt);
    }
    //---------------------------------------------------------------------
    void SpaceManager::prcResEvent()
    {
    }
    //---------------------------------------------------------------------
    void SpaceManager::prcLodEvent()
    {
        // 用每个监听器,处理事件
        LodListenerSet::iterator i, iend = mLodListeners.end();
        for(i = mLodListeners.begin(); i != iend; ++i)
        {
            SpaceLodEventList::const_iterator mobegin, moend = mSpaceLodEventList.end();
            for(mobegin = mSpaceLodEventList.begin(); mobegin != moend; ++mobegin)
                (*i)->postLodChange(*mobegin);

            MeshLodEventList::const_iterator embegin, emend = mMeshLodEventList.end();
            for(embegin = mMeshLodEventList.begin(); embegin != emend; ++embegin)
                (*i)->postLodChange(*embegin);

            MaterialLodEventList::const_iterator emcbegin, emcend = mMatLodEventList.end();
            for(emcbegin = mMatLodEventList.begin(); emcbegin != emcend; ++emcbegin)
                (*i)->postLodChange(*emcbegin);
        }

        // 清除事件队列
        mSpaceLodEventList.clear();
        mMeshLodEventList.clear();
        mMatLodEventList.clear();
    }
    //-----------------------------------------------------------------------
    SpaceNode * SpaceManager::createNodeImpl(SpaceID sid)
    {
        return N_new SpaceNode(this, sid);
    }
    //---------------------------------------------------------------------
    void SpaceManager::applyGpuProgram(const ShaderChProgram * cpg, Nmark mark)
    {
        if(GPT_ProgramMark & mark)
        {
            mLastProgramLightMark = 1;
            mGpuParamMark = GPT_Unknow;
            GpuProgram * gp = 0;
            if(GPT_VS & mark)
            {
                cpg->get(ShaderChProgram::ST_VS, gp);
                mRenderSys->on(gp->getMain());
            }
            if(GPT_TS & mark)
            {
                cpg->get(ShaderChProgram::ST_TS, gp);
                mRenderSys->on(gp->getMain());
            }
            if(GPT_DS & mark)
            {
                cpg->get(ShaderChProgram::ST_DS, gp);
                mRenderSys->on(gp->getMain());
            }
            if(GPT_GS & mark)
            {
                cpg->get(ShaderChProgram::ST_GS, gp);
                mRenderSys->on(gp->getMain());
            }
            if(GPT_FS & mark)
            {
                cpg->get(ShaderChProgram::ST_FS, gp);
                mRenderSys->on(gp->getMain());
            }
            if(GPT_CS & mark)
            {
                cpg->get(ShaderChProgram::ST_CS, gp);
                mRenderSys->on(gp->getMain());
            }
        }
        
        if (mGpuParamMark & mark)
        {
            const_cast<ShaderChProgram *>(cpg)->_update(mSyncParams, mGpuParamMark);
            GpuProgramParam * gpp;
            if((GPT_VS_Param & mark) && cpg->isExist(ShaderChProgram::ST_VS))
            {
                cpg->getParam(ShaderChProgram::ST_VS, gpp);
                mRenderSys->on(GpuProgram::ST_VS, gpp, mGpuParamMark);
            }

            if((GPT_GS_Param & mark) && cpg->isExist(ShaderChProgram::ST_GS))
            {
                cpg->getParam(ShaderChProgram::ST_GS, gpp);
                mRenderSys->on(GpuProgram::ST_GS, gpp, mGpuParamMark);
            }

            if((GPT_FS_Param & mark) && cpg->isExist(ShaderChProgram::ST_FS))
            {
                cpg->getParam(ShaderChProgram::ST_FS, gpp);
                mRenderSys->on(GpuProgram::ST_FS, gpp, mGpuParamMark);
            }
                
            if((GPT_TS_Param & mark) && cpg->isExist(ShaderChProgram::ST_TS))
            {
                cpg->getParam(ShaderChProgram::ST_TS, gpp);
                mRenderSys->on(GpuProgram::ST_TS, gpp, mGpuParamMark);
            }
                
            if((GPT_DS_Param & mark) && cpg->isExist(ShaderChProgram::ST_DS))
            {
                cpg->getParam(ShaderChProgram::ST_DS, gpp);
                mRenderSys->on(GpuProgram::ST_DS, gpp, mGpuParamMark);
            }
                
            if((GPT_CS_Param & mark) && cpg->isExist(ShaderChProgram::ST_CS))
            {
                cpg->getParam(ShaderChProgram::ST_CS, gpp);
                mRenderSys->on(GpuProgram::ST_CS, gpp, mGpuParamMark);
            }

            mRenderSys->getFFPParam()->sync(mSyncParams, mGpuParamMark);
            mRenderSys->setFFPParam(*mRenderSys->getFFPParam(), mGpuParamMark);
            mGpuParamMark = 0;
        }
    }
    //---------------------------------------------------------------------
    void SpaceManager::onDestroy()
    {
        /*
        Listeners::iterator i, iend = mListeners.end();
        for(i = mListeners.begin(); i != iend; ++i)
        {
            (*i)->onDestroy(this);
        }*/
    }
    //---------------------------------------------------------------------
}