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
#include "NiiStencilRenderPattern.h"
#include "NiiGBufferManager.h"
#include "NiiMeshManager.h"
#include "NiiMaterialManager.h"
#include "NiiLogManager.h"
#include "NiiSpaceManager.h"
#include "NiiShadowRenderQueue.h"
#include "NiiShadowVolumeExtrudeProgram.h"
#include "NiiAreaSpaceQuery.h"
#include "NiiSpaceQueryCmd.h"
#include "NiiShadowObj.h"
#include "NiiSysSyncParam.h"
#include "NiiViewRectGeo.h"
#include "NiiLight.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ShadowCasterSpaceQueryCmd
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineAPI ShadowCasterSpaceQueryCmd : public SpaceQueryCmd, public SceneAlloc
    {
    public:
        ShadowCasterSpaceQueryCmd(SpaceManager * sm) :
            SpaceQueryCmd(sm),
            mCasterList(0),
            mIsLightInFrustum(false),
            mLightClipVolumeList(0),
            mCamera(0){}

        void prepare(bool lightInFrustum, const PlaneSpaceList * lightClip,
            const Light * light, const Camera * cam, StencilObjList * casterList,
                NIIf farDistSquared)
        {
            mCasterList = casterList;
            mIsLightInFrustum = lightInFrustum;
            mLightClipVolumeList = lightClip;
            mCamera = cam;
            mLight = light;
            mFarDistSquared = farDistSquared;
        }

        /// @copydetails SpaceQueryCmd::execute
        void execute()
        {
        }

        /// @copydetails SpaceQueryCmd::onQuery
        bool onQuery(const QueryDetail & result);
    protected:
        SpaceManager * mSceneMgr;
        StencilObjList * mCasterList;
        const PlaneSpaceList * mLightClipVolumeList;
        const Camera * mCamera;
        const Light * mLight;
        NIIf mFarDistSquared;
        bool mIsLightInFrustum;
    };
    //---------------------------------------------------------------------
    bool ShadowCasterSpaceQueryCmd::onQuery(const QueryDetail & result)
    {
        if(result.mType != SpaceQuery::PT_SpaceObj)
            return true;
        SpaceObj * object = static_cast<const SpaceObjQueryDetail &>(result).mSpaceObj;
        ShadowObj * svg = dynamic_cast<ShadowObj *>(object);
        if(svg)
        {
            if(svg->isEdgeExist() != true)
                return true;
        }

        if(object->isCastShadow() && object->isVisible() &&
            !mArea->isExcludeRenderExist(object->getRenderGroup()))
        {
            if(mFarDistSquared)
            {
                // 检测对象在阴影远距离内
                Vector3f toObj = object->getSpaceNode()->getSpacePos() - mCamera->getSpacePos();
                NIIf radius = object->getSpaceBS().getRadius();
                NIIf dist =  toObj.lengthSquared();
                if(dist - (radius * radius) > mFarDistSquared)
                {
                    // 跳过,超出最大范围
                    return true;
                }
            }

            // 如果对象在平截头中,我门总是能看到阴影
            if(mCamera->isIntersect(svg->getSpaceAABB()))
            {
                mCasterList->push_back(svg);
                return true;
            }

            /* 否则,如果灯光在平截头外(或是方向灯光,总是在外的)对象只能投射一个阴影到我们
                的视口,对象与体积成形在平截头和灯光间有相交
            */
            if(!mIsLightInFrustum || mLight->getType() == LT_DIRECTIONAL)
            {
                PlaneSpaceList::const_iterator i, iend = mLightClipVolumeList->end();
                for(i = mLightClipVolumeList->begin(); i != iend; ++i)
                {
                    if(i->intersects(svg->getSpaceAABB()))
                    {
                        mCasterList->push_back(svg);
                        return true;
                    }
                }
            }
        }
        return true;
    }
    class StencilShaderData : public ShadowAlloc
    {
    public:
        StencilShaderData(SpaceManager * sm)
        {
            mQueryCmd = N_new ShadowCasterSpaceQueryCmd(sm);
            mCasterSphereQuery = sm->createSphereQuery();
            mCasterAABBQuery = sm->createAABBQuery();
        }
        ~StencilShaderData()
        {
            if (mQueryCmd)
            {
                N_delete mQueryCmd;
                mQueryCmd = 0;
            }
            if (mCasterAABBQuery)
            {
                N_delete mCasterAABBQuery;
                mCasterAABBQuery = 0;
            }
            if (mCasterSphereQuery)
            {
                N_delete mCasterSphereQuery;
                mCasterSphereQuery = 0;
            }
        }
        GpuProgramParam * mInfiniteParam;
        GpuProgramParam * mFiniteParam;
        AABSpaceQuery * mCasterAABBQuery;
        SphereSpaceQuery * mCasterSphereQuery;
        ShadowCasterSpaceQueryCmd * mQueryCmd;
    };

    ShaderChStencil stencilRender0(false);

    //------------------------------------------------------------------------
    StencilRenderPattern::StencilRenderPattern(SpaceManager * sm, RenderSys * rsys,
        SysSyncParam * param):
        RenderPattern(sm, rsys, param),
        mScreen(0),
        mIndexBuffer(0),
        mIndexBufferSize(51200),
        mDebugCh(0),
        mStencilCh(0),
        mModulateCh(0),
        mDebugShadows(false),
        mBothSideStencil(false)
    {
        mDebugChRID = N_Only(Material).genValidID();
        mStencilChRID = N_Only(Material).genValidID();
        mModulationChRID = N_Only(Material).genValidID();
        mShaderData = N_new StencilShaderData(mParent);
    }
    //------------------------------------------------------------------------
    StencilRenderPattern::~StencilRenderPattern()
    {
        if (mScreen)
        {
            N_delete mScreen;
            mScreen = 0;
        }
        if (mIndexBuffer)
        {
            N_delete mIndexBuffer;
            mIndexBuffer = 0;
        }
        N_delete mShaderData;
    }
    //------------------------------------------------------------------------
    bool StencilRenderPattern::isValid()
    {
        if(mRenderSys->getFeature()->isSupport(GF_HW_Stencil))
        {
            if(mIndexBuffer == 0)
            {
                N_Only(GBuffer).create(mIndexBuffer, 16, mIndexBufferSize, Buffer::M_WRITE | Buffer::M_WHOLE);

                mParent->setShadowVolume(mRenderQueue, true);
            }
        }
        else
        {
            N_Only(Log).log(_I18n("警告: 硬件需要支持模版操作 "));
            return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    void StencilRenderPattern::renderShadow(Camera * cam, const Viewport * vp, const LightList & lights)
    {
        RenderPattern::renderShadow(cam, vp, lights);
        if(cam->getProjType() == Frustum::T_Perspective &&
            cam->getFarClipDistance() != 0 && mInfiniteFar &&
            mRenderSys->getFeature()->isSupport(GF_Infinite_FarPlane))
        {
            // 设置无限远面
            cam->setFarClipDistance(0);
        }
    }
    //------------------------------------------------------------------------
    void StencilRenderPattern::sortLight(LightList & dest)
    {
        NCount index;
        // 排序(稳定的保证方向灯光顺序)
        std::stable_sort(dest.begin(), dest.end(), Light::LightDistanceLess());
        LightList::iterator i, iend = dest.end();
        for(index = 0, i = dest.begin(); i != iend; ++i, ++index)
        {
            (*i)->setIndex(index);
        }
    }
    //------------------------------------------------------------------------
    void StencilRenderPattern::init()
    {
        if(mInit)
            return;

        RenderPattern::init();

        if(!mDebugCh)
        {
            Material * matDebug = static_cast<Material *>(N_Only(Material).get(mDebugChRID));
            if(matDebug == 0)
            {
                matDebug = static_cast<Material *>(N_Only(Material).create(mDebugChRID, GroupInner));

                mDebugCh = matDebug->get(0)->get(0);
                mDebugCh->setBlend(ShaderCh::SOT_Set)->setBlend(FBM_ADD);
                mDebugCh->setLight(false);
                mDebugCh->setStencil(ShaderCh::SOT_Set)->setDepthWrite(false);
                ShaderChTextureUnit * t = mDebugCh->setTexture(ShaderCh::SOT_Set)->create();
                t->getColourBlend()->setBlend(TBO_MODULATE, Colour(0.7f, 0.0, 0.2f), TBS_PRE_TEXTURE);
                mDebugCh->setCullingMode(CM_None);

                if(mRenderSys->getFeature()->isSupport(GF_Program_Vertex))
                {
                    ShadowVolumeExtrudeProgram::init();

                    // 现在启用(无限)点灯光挤出,仅为获得和设置一些参数
                    //设置顶点着色程序/片段着色程序
                    mDebugCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS,
                        ShadowVolumeExtrudeProgram::programNames[ShadowVolumeExtrudeProgram::POINT_LIGHT]);
                    mDebugCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS,
                        ShadowVolumeExtrudeProgram::frgProgramName);
                    mDebugCh->getProgram().getParam(ShaderChProgram::ST_VS, mShaderData->mInfiniteParam);
                    //设置阴影着色程序使用的自动参数
                    mShaderData->mInfiniteParam->set(0, GSP_Matrix_Proj_X_View_X_Space);
                    mShaderData->mInfiniteParam->set(4, GSP_Light_Pos_ChSpace_ParamIndex);
                    // 注意忽略额外的参数-为兼容有限挤出顶点着色程序
                    mShaderData->mInfiniteParam->set(5, GSP_Shadow_VolExtent);
                }
                matDebug->check();
            }
            else
            {
                mDebugCh = matDebug->get(0)->get(0);

                if(mRenderSys->getFeature()->isSupport(GF_Program_Vertex))
                {
                    mDebugCh->getProgram().getParam(ShaderChProgram::ST_VS, mShaderData->mInfiniteParam);
                }
            }
        }

        if(!mStencilCh)
        {
            Material * matStencil = static_cast<Material *>(N_Only(Material).get(mStencilChRID));
            if(matStencil == 0)
            {
                matStencil = static_cast<Material *>(N_Only(Material).create(mStencilChRID, GroupInner));

                mStencilCh = matStencil->get(0)->get(0);

                if(mRenderSys->getFeature()->isSupport(GF_Program_Vertex))
                {
                    // 现在启用有限点灯光挤出,仅为获得一些参数
                    mStencilCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS, ShadowVolumeExtrudeProgram::programNames[ShadowVolumeExtrudeProgram::POINT_LIGHT_FINITE]);
                    mStencilCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS, ShadowVolumeExtrudeProgram::frgProgramName);
                    mStencilCh->getProgram().getParam(ShaderChProgram::ST_VS, mShaderData->mFiniteParam);
                    // 设置自动参数
                    mShaderData->mFiniteParam->set(0, GSP_Matrix_Proj_X_View_X_Space);
                    mShaderData->mFiniteParam->set(4, GSP_Light_Pos_ChSpace_ParamIndex);
                    // 注意额外的参数
                    mShaderData->mFiniteParam->set(5, GSP_Shadow_VolExtent);
                }
                matStencil->check();
                // 没有其他的,我们不使用像一个"真正"通道那样,这是一个占位符
            }
            else
            {
                mStencilCh = matStencil->get(0)->get(0);

                if(mRenderSys->getFeature()->isSupport(GF_Program_Vertex))
                {
                    mStencilCh->getProgram().getParam(ShaderChProgram::ST_VS, mShaderData->mFiniteParam);
                }
            }
        }

        if(!mModulateCh)
        {
            Material * matModStencil = static_cast<Material *>(N_Only(Material).get(mModulationChRID));
            if(matModStencil == 0)
            {
                // 初始化
                matModStencil = static_cast<Material *>(N_Only(Material).create(mModulationChRID, GroupInner));

                mModulateCh = matModStencil->get(0)->get(0);
                mModulateCh->setBlend(ShaderCh::SOT_Set)->setBlend(CF_DST, CF_ZERO);
                mModulateCh->setLight(false);
                mModulateCh->setStencil(ShaderCh::SOT_Set)->setDepthWrite(false);
                mModulateCh->setStencil(ShaderCh::SOT_Set)->setDepthEnable(false);
                ShaderChTextureUnit * t = mModulateCh->setTexture(ShaderCh::SOT_Set)->create();
                t->getColourBlend()->setBlend(TBO_MODULATE, mShadowColour, TBS_PRE_TEXTURE);
                mModulateCh->setCullingMode(CM_None);
            }
            else
            {
                mModulateCh = matModStencil->get(0)->get(0);
            }
        }

        // 也初始化全屏四方
        if(!mScreen)
        {
            mScreen = N_new RectGeo();
            mScreen->setRect(-1, 1, 1, -1);
        }

        if(mRenderSys->getFeature()->isSupport(GF_Stencil_BothSide) &&
            mRenderSys->getFeature()->isSupport(GF_Stencil_Wrap))
        {
            mBothSideStencil = true;
        }
        else
        {
            mBothSideStencil = false;
        }

        mSyncParams->setShadowDirLightExtrusionDistance(mShadowExtent);

        mInit = true;
    }
    //------------------------------------------------------------------------
    void StencilRenderPattern::setShadowColour(const Colour & c)
    {
        mShadowColour = c;

        // 仅在它准备时,改变阴影纹理设置,否则,它会建立准备时的阴影材质
        if(mModulateCh)
        {
            mModulateCh->getTexture().get(0)->
                getColourBlend()->setBlend(TBO_MODULATE, c, TBS_PRE_TEXTURE);
        }
    }
    //------------------------------------------------------------------------
    RenderQueue * StencilRenderPattern::createQueue() const
    {
        RenderQueue * re = N_new ShadowRenderQueue();
        re->init();
        return re;
    }
    //------------------------------------------------------------------------
    void StencilRenderPattern::applyDarkCap(const Light * l, const Camera * camera,
        ShadowObj * caster, NIIf extent, Nmark & flags)
    {
        // 如下,在zpass中需要一个暗罩
        /*  1:点/聚灯光在调制阴影模式中无限挤压,否则在没有深度(skybox)的区
            域,无限投影阴影将流一个暗带
            2:在任何灯光资源有限挤出中,因为查看角度检测结束和阴影对象后面是
            不正确的
        */
        if(!(flags & ShadowObj::VT_Infinity) &&
            camera->isIntersect(caster->getBackCapAABB(*l, extent)))
        {
            flags |= ShadowObj::VT_BackCap;
        }
    }
    //------------------------------------------------------------------------
    void StencilRenderPattern::setIndexSize(NCount size)
    {
        if(mIndexBuffer != 0 && size != mIndexBufferSize)
        {
            N_delete mIndexBuffer;
            N_Only(GBuffer).create(mIndexBuffer, 16, size, Buffer::M_NORMAL | Buffer::M_WHOLE);
        }
        mIndexBufferSize = size;
    }
    //------------------------------------------------------------------------
    const PlaneSpace & StencilRenderPattern::getNearClip(const Light * light, const Camera * cam) const
    {
        // First check if the light is close to the near plane, since
        // in this case we have to build a degenerate clip volume
        mNearClipSpace.mPlanes.clear();
        mNearClipSpace.mOuterType = Plane::PT_Negative;

        NIIf n = cam->getNearClipDistance();
        // Homogenous position
        Vector4f lightPos = light->getSpace4();
        // 3D version (not the same as getSpacePos, is -direction for
        // directional lights)
        Vector3f lightPos3 = Vector3f(lightPos.x, lightPos.y, lightPos.z);

        // Get eye-space light position
        // use 4D vector so directional lights still work
        Vector4f eyeSpaceLight = cam->getViewMatrix() * lightPos;
        // Find distance to light, project onto -Z axis
        NIIf d = eyeSpaceLight.dotProduct(Vector4f(0, 0, -1, -n) );
        #define THRESHOLD 1e-6
        if(d > THRESHOLD || d < -THRESHOLD)
        {
            // light is not too close to the near plane
            // First find the worldspace positions of the corners of the viewport
            const Vector3f * corner = cam->getClipPlaneIntersect();
            NIIi winding = (d < 0) ^ cam->isReflection() ? +1 : -1;
            // Iterate over world points and form side planes
            Vector3f normal;
            Vector3f lightDir;
            for(NCount i = 0; i < 4; ++i)
            {
                // Figure out light dir
                lightDir = lightPos3 - (corner[i] * lightPos.w);
                // Cross with anticlockwise corner, therefore normal points in
                normal = (corner[i] - corner[(i + winding) % 4]).crossProduct(lightDir);
                normal.normalise();
                mNearClipSpace.mPlanes.push_back(Plane(normal, corner[i]));
            }

            // Now do the near plane plane
            normal = cam->getClipPlane(Frustum::PT_Near).mNormal;
            if(d < 0)
            {
                // Behind near plane
                normal = -normal;
            }
            const Vector3f & cameraPos = cam->getSpacePos();
            mNearClipSpace.mPlanes.push_back(Plane(normal, cameraPos));

            // Finally, for a point/spot light we can add a sixth plane
            // This prevents false positives from behind the light
            if(light->getType() != LT_DIRECTIONAL)
            {
                // Direction from light perpendicular to near plane
                mNearClipSpace.mPlanes.push_back(Plane(-normal, lightPos3));
            }
        }
        else
        {
            // light is close to being on the near plane
            // degenerate volume including the entire scene
            // we will always require light / dark caps
            mNearClipSpace.mPlanes.push_back(Plane(Vector3f::Z, -n));
            mNearClipSpace.mPlanes.push_back(Plane(-Vector3f::Z, n));
        }
        return mNearClipSpace;
    }
    //-----------------------------------------------------------------------
    const PlaneSpaceList & StencilRenderPattern::getFrustumClip(const Light * light, const Camera * cam) const
    {
        // Homogenous light position
        Vector4f lightPos = light->getSpace4();
        // 3D version (not the same as getSpacePos, is -direction for
        // directional lights)
        Vector3f lightPos3 = Vector3f(lightPos.x, lightPos.y, lightPos.z);

        const Vector3f * clockwiseVerts[4];

        // Get worldspace frustum corners
        const Vector3f * corners = cam->getClipPlaneIntersect();
        NIIi windingPt0 = cam->isReflection() ? 1 : 0;
        NIIi windingPt1 = cam->isReflection() ? 0 : 1;

        bool infiniteViewDistance = (cam->getFarClipDistance() == 0);

        Vector3f notSoFarCorners[4];
        if(infiniteViewDistance)
        {
            Vector3f camPosition = cam->getLocalPos();
            notSoFarCorners[0] = corners[0] + corners[0] - camPosition;
            notSoFarCorners[1] = corners[1] + corners[1] - camPosition;
            notSoFarCorners[2] = corners[2] + corners[2] - camPosition;
            notSoFarCorners[3] = corners[3] + corners[3] - camPosition;
        }

        mFrustumClipSpace.clear();
        for(NCount n = 0; n < 6; ++n)
        {
            // Skip far plane if infinite view frustum
            if (infiniteViewDistance && n == Frustum::PT_Far)
                continue;

            const Plane & plane = cam->getClipPlane((Frustum::PlaneType)n);
            Vector4f planeVec(plane.mNormal.x, plane.mNormal.y, plane.mNormal.z, plane.mD);
            // planes face inwards, we need to know if light is on negative side
            NIIf d = planeVec.dotProduct(lightPos);
            if (d < -1e-06)
            {
                // Ok, this is a valid one
                // clockwise verts mean we can cross-product and always get normals
                // facing into the volume we create

                mFrustumClipSpace.push_back(PlaneSpace());
                PlaneSpace & vol = mFrustumClipSpace.back();
                switch(n)
                {
                case(Frustum::PT_Near):
                    clockwiseVerts[0] = corners + 3;
                    clockwiseVerts[1] = corners + 2;
                    clockwiseVerts[2] = corners + 1;
                    clockwiseVerts[3] = corners + 0;
                    break;
                case(Frustum::PT_Far):
                    clockwiseVerts[0] = corners + 7;
                    clockwiseVerts[1] = corners + 6;
                    clockwiseVerts[2] = corners + 5;
                    clockwiseVerts[3] = corners + 4;
                    break;
                case(Frustum::PT_Left):
                    clockwiseVerts[0] = infiniteViewDistance ? notSoFarCorners + 1 : corners + 5;
                    clockwiseVerts[1] = corners + 1;
                    clockwiseVerts[2] = corners + 2;
                    clockwiseVerts[3] = infiniteViewDistance ? notSoFarCorners + 2 : corners + 6;
                    break;
                case(Frustum::PT_Right):
                    clockwiseVerts[0] = infiniteViewDistance ? notSoFarCorners + 3 : corners + 7;
                    clockwiseVerts[1] = corners + 3;
                    clockwiseVerts[2] = corners + 0;
                    clockwiseVerts[3] = infiniteViewDistance ? notSoFarCorners + 0 : corners + 4;
                    break;
                case(Frustum::PT_Top):
                    clockwiseVerts[0] = infiniteViewDistance ? notSoFarCorners + 0 : corners + 4;
                    clockwiseVerts[1] = corners + 0;
                    clockwiseVerts[2] = corners + 1;
                    clockwiseVerts[3] = infiniteViewDistance ? notSoFarCorners + 1 : corners + 5;
                    break;
                case(Frustum::PT_Bottom):
                    clockwiseVerts[0] = infiniteViewDistance ? notSoFarCorners + 2 : corners + 6;
                    clockwiseVerts[1] = corners + 2;
                    clockwiseVerts[2] = corners + 3;
                    clockwiseVerts[3] = infiniteViewDistance ? notSoFarCorners + 3 : corners + 7;
                    break;
                };

                // Build a volume
                // Iterate over world points and form side planes
                Vector3f normal;
                Vector3f lightDir;
                NCount infiniteViewDistanceInt = infiniteViewDistance ? 1 : 0;
                for(NCount i = 0; i < 4 - infiniteViewDistanceInt; ++i)
                {
                    // Figure out light dir
                    lightDir = lightPos3 - (*(clockwiseVerts[i]) * lightPos.w);
                    Vector3f edgeDir = *(clockwiseVerts[(i+windingPt1)%4]) - *(clockwiseVerts[(i+windingPt0)%4]);
                    // Cross with anticlockwise corner, therefore normal points in
                    normal = edgeDir.crossProduct(lightDir);
                    normal.normalise();
                    vol.mPlanes.push_back(Plane(normal, *(clockwiseVerts[i])));
                }

                // Now do the near plane (this is the plane of the side we're
                // talking about, with the normal inverted (d is already interpreted as -ve)
                vol.mPlanes.push_back(Plane(-plane.mNormal, plane.mD));

                // Finally, for a point/spot light we can add a sixth plane
                // This prevents false positives from behind the light
                if(light->getType() != LT_DIRECTIONAL)
                {
                    // re-use our own plane normal
                    vol.mPlanes.push_back(Plane(plane.mNormal, lightPos3));
                }
            }
        }

        return mFrustumClipSpace;
    }
    //------------------------------------------------------------------------
    void StencilRenderPattern::renderStencil(const Light * light, const Camera * camera, bool clip)
    {
        // 获取阴影投射列表
        const StencilObjList & casters = findCastList(light, camera);
        if(casters.empty())
            return;

        LightList lightList;
        // const_cast在这里,因为我们输入的是常量
        lightList.push_back(const_cast<Light *>(light));

        // 建立裁剪测试(仅点&聚光灯)
        SpaceManager::ClipType scissored = SpaceManager::CT_None;
        if(clip)
        {
            scissored = mParent->applyClip(lightList, camera);
            if(scissored == SpaceManager::CT_All)
                return;
        }

        mRenderSys->off(GpuProgram::ST_FS);

        bool extrudeInSoftware = true;
        // 是否使用顶点着色程序做挤压?
        bool finiteExtrude = !mInfiniteFar ||
            !mRenderSys->getFeature()->isSupport(GF_Infinite_FarPlane);
        if(mRenderSys->getFeature()->isSupport(GF_Program_Vertex))
        {
            extrudeInSoftware = false;
            mStencilCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS, ShadowVolumeExtrudeProgram::frgProgramName);
            // 设置参数
            if(finiteExtrude)
            {
                // 附加相应的挤压顶点着色程序,注意我们从来没有消除它,因为顶点着色程序是常量
                mStencilCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS, 
                    ShadowVolumeExtrudeProgram::getProgramName(light->getType(), finiteExtrude, false),
                    mShaderData->mFiniteParam);
            }
            else
            {
                // 附加相应的挤压顶点着色程序,注意我们从来没有消除它,因为顶点着色程序是常量
                mStencilCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS,
                    ShadowVolumeExtrudeProgram::getProgramName(light->getType(), finiteExtrude, false),
                    mShaderData->mInfiniteParam);
            }
            if(mDebugShadows)
            {
                mDebugCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS,
                    ShadowVolumeExtrudeProgram::frgProgramName);

                // 设置参数
                if(finiteExtrude)
                {
                    mDebugCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS,
                        ShadowVolumeExtrudeProgram::getProgramName(light->getType(), finiteExtrude, true),
                        mShaderData->mFiniteParam);
                }
                else
                {
                    mDebugCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS,
                        ShadowVolumeExtrudeProgram::getProgramName(light->getType(), finiteExtrude, true),
                        mShaderData->mInfiniteParam);
                }
            }

            mParent->applyGpuProgram(&mStencilCh->getProgram(), GPT_VS);
            if(ShadowVolumeExtrudeProgram::frgProgramName)
            {
                mParent->applyGpuProgram(&mStencilCh->getProgram(), GPT_FS);
            }
        }
        else
        {
            mRenderSys->off(GpuProgram::ST_VS);
        }

        // 关闭颜色写入和深度写入
        mRenderSys->setColourWrite(ColourMark::CM_ZERO);
        mRenderSys->_enableTexture(0);
        mRenderSys->setStencil(ShaderChStencil(true, CPM_LESS, false), SB_Depth);
        mRenderSys->setStencilTest(true);

        // 计算挤压距离
        // 现在使用方向灯光挤压距离, 从优化算法中生成一点,点/射灯光随后更新
        NIIf extrudeDist = mShadowExtent;

        const PlaneSpace & nearClipVol = getNearClip(light, camera);

        // 现在遍历投射和渲染
        StencilObjList::const_iterator si, siend = casters.end();
        for(si = casters.begin(); si != siend; ++si)
        {
            ShadowObj * caster = *si;
            bool zfailAlgo = camera->isCustomNearClip();
            Nmark flags = 0;

            if(light->getType() != LT_DIRECTIONAL)
            {
                 extrudeDist = caster->getExtrudeDistance(light);
            }

            NIIf darkCapExtrudeDist = extrudeDist;
            if(!extrudeInSoftware && !finiteExtrude)
            {
                flags |= ShadowObj::VT_Infinity;
                darkCapExtrudeDist = mShadowExtent;
            }

            // 确定是否需要zfail
            if(zfailAlgo || nearClipVol.intersects(caster->getSpaceAABB()))
            {
                // 为这个对象,我们使用zfail,仅因为zfail合适zpass算法
                zfailAlgo = true;
                // 我们需要包含灯亮罩或暗罩,但仅在他们可见时
                if(camera->isIntersect(caster->getFrontCapAABB()))
                {
                    flags |= ShadowObj::VT_FrontCap;
                }
                // zfail 需要暗罩
                // 除非方向灯光使用硬件挤出为无限远,因为挤出为单一点
                if(!((flags & ShadowObj::VT_Infinity) && light->getType() == LT_DIRECTIONAL) &&
                    camera->isIntersect(caster->getBackCapAABB(*light, darkCapExtrudeDist)))
                {
                    flags |= ShadowObj::VT_BackCap;
                }
            }
            else
            {
                applyDarkCap(light, camera, caster, darkCapExtrudeDist, flags);
            }

            if(extrudeInSoftware)
            {
                flags |= ShadowObj::VT_Software;
            }
            // 获取阴影可渲染物
            const ShadowVolumeList & iShadowRenderables =
                caster->getVolumeList(light, extrudeDist, flags, mIndexBuffer);

            // 渲染一个阴影体积到这
            //  - 如果我们支持2边蒙板功能,使用无拣选渲染
            //  - 否则,渲染2次,遍历每个拣选方法和反转操作
            applyStencil(false, zfailAlgo);

            renderShadows(iShadowRenderables, mStencilCh, &lightList, false, zfailAlgo,
                mBothSideStencil, flags);

            if(!mBothSideStencil)
            {
                // 第二个通道
                applyStencil(true, zfailAlgo);

                renderShadows(iShadowRenderables, mStencilCh, &lightList, true, zfailAlgo,
                    false, flags);
            }

            // 我们需要渲染一个调试阴影光标?
            if(mDebugShadows)
            {
                // 重置蒙板&颜色操作
                mRenderSys->setStencil(stencilRender0, SB_Stencil);
                mDebugCh->getTexture().get(0)->getColourBlend()->setBlend(TBO_MODULATE,
                    zfailAlgo ? Colour(0.7f, 0.0f, 0.2f) : Colour(0.0f, 0.7f, 0.2f), TBS_PRE_TEXTURE);
                mParent->applyCh(mDebugCh, false, true);

                renderShadows(iShadowRenderables, mDebugCh, &lightList, true, false, false, flags);

                mRenderSys->setColourWrite(ColourMark::CM_ZERO);
                mRenderSys->_setDepthOpFunc(CPM_LESS);
            }
        }

        mRenderSys->setColourWrite(ColourMark::CM_RGBA);
        mRenderSys->setStencil(ShaderChStencil(), SB_Depth);
        mRenderSys->setStencilTest(false);
        mRenderSys->off(GpuProgram::ST_VS);

        if(scissored == SpaceManager::CT_Some)
        {
            mParent->resetClip();
        }
    }
    //------------------------------------------------------------------------
    const StencilObjList & StencilRenderPattern::findCastList(const Light * light, const Camera * camera)
    {
        mCasterList.clear();
        NIIf temp;
        if(light->getType() == LT_DIRECTIONAL)
        {
            AABox aabb;
            const Vector3f * corners = camera->getClipPlaneIntersect();
            Vector3f min, max;
            Vector3f extrude = light->getSpaceDirection() * -mShadowExtent;
            // 处理第一个角落
            min = max = corners[0];
            min = min.floor(corners[0] + extrude);
            max = max.ceil(corners[0] + extrude);
            for(NCount c = 1; c < 8; ++c)
            {
                min = min.floor(corners[c]);
                max = max.ceil(corners[c]);
                min = min.floor(corners[c] + extrude);
                max = max.ceil(corners[c] + extrude);
            }
            aabb.setMin(min);
            aabb.setMax(max);
            mShaderData->mCasterAABBQuery->setBox(aabb);
            temp = light->getShadowExtent();
            // 执行,使用回调
            mShaderData->mQueryCmd->prepare(false, &(getFrustumClip(light, camera)),
                light, camera, &mCasterList, temp * temp);
            mShaderData->mCasterAABBQuery->execute(mShaderData->mQueryCmd);
        }
        else
        {
            Sphere s(light->getSpacePos(), light->getRange());
            if(camera->isIntersect(s))
            {
                mShaderData->mCasterSphereQuery->setSphere(s);

                // 确定灯光在平截头的内外
                bool lightInFrustum = camera->isIntersect(light->getSpacePos());
                const PlaneSpaceList * volList = 0;
                if(!lightInFrustum)
                {
                    // 如果灯光在平截头外,仅建立一个额外体积列表
                    volList = &(getFrustumClip(light, camera));
                }
                temp = light->getShadowExtent();
                // 执行,使用回调
                mShaderData->mQueryCmd->prepare(lightInFrustum, volList,
                    light, camera, &mCasterList, temp * temp);
                mShaderData->mCasterSphereQuery->execute(mShaderData->mQueryCmd);
            }
        }
        return mCasterList;
    }
    //------------------------------------------------------------------------
    void StencilRenderPattern::applyStencil(bool secondrender, bool zfail)
    {
        StencilOpType incrOp, decrOp;
        if(mRenderSys->getFeature()->isSupport(GF_Stencil_Wrap))
        {
            incrOp = SOT_INCR_WRAP;
            decrOp = SOT_DECR_WRAP;
        }
        else
        {
            incrOp = SOT_INCR;
            decrOp = SOT_DECR;
        }

        // 如果zpass为真时,第一通道处理正面
        // 如果zpass为真时,第二通道处理正面
        // 如果zfail反转
        // 当2边模版时,这样去确保我们在递减前总是递增,总是输入正面模版操作参数和当发生背面时翻转他们
        if(!mBothSideStencil && ((secondrender || zfail) && !(secondrender && zfail)))
        {
            mCullingMode = mBothSideStencil ? CM_None : CM_Anticlockwise;
            mRenderSys->setStencil(ShaderChStencil(
                mBothSideStencil,
                0xFFFFFFFF,                 // 没有 mask
                0xFFFFFFFF,                 // 没有 cmpmask
                CPM_ALWAYS_PASS,             // 总是通过蒙板检查
                0,                          // 没有ref值(无比较)
                SOT_KEEP,                   // 蒙板测试总是通过
                zfail ? incrOp : SOT_KEEP,  // 背面深度不通过时
                zfail ? SOT_KEEP : decrOp   // 背面通过时
                ), SB_Stencil);
        }
        else
        {
            mCullingMode = mBothSideStencil ? CM_None : CM_Clockwise;
            mRenderSys->setStencil(ShaderChStencil(
                mBothSideStencil,
                0xFFFFFFFF,                 // 没有 mask
                0xFFFFFFFF,                 // 没有 cmpmask
                CPM_ALWAYS_PASS,             // 总是通过蒙板检查
                0,                          // 没有ref值(无比较)
                SOT_KEEP,                   // 蒙板测试总是通过
                zfail ? decrOp : SOT_KEEP,  // 正面深度不通过时
                zfail ? SOT_KEEP : incrOp  // 正面通过时
                ), SB_Stencil);
        }
        mRenderSys->_setCullingMode(mCullingMode);
    }
    //------------------------------------------------------------------------
    bool StencilRenderPattern::isStencilShadow() const
    {
        return true;
    }
    //------------------------------------------------------------------------
    void StencilRenderPattern::renderShadows(const ShadowVolumeList & src,
        ShaderCh * pass, const LightList * lights, bool secondpass, bool zfail, 
        bool twosided, Nmark flags)
    {
        // ----- SHADOW VOLUME LOOP -----
        // 使用相同的蒙板操作,渲染所有阴影可渲染物
        ShadowVolumeList::const_iterator i, iend = src.end();
        for(i = src.begin(); i != iend; ++i)
        {
            ShadowVolume * sr = *i;
            // 忽略隐藏的可渲染物
            if(sr->isVisible())
            {
                // 渲染体积,包含暗罩和灯亮罩
                mParent->render(sr, pass, *lights, false);

                // 可选的渲染分割灯光照
                if((sr->getLightCap() != 0) && (flags & ShadowObj::VT_FrontCap))
                {
                    ShadowVolume * lightCap = sr->getLightCap();
                    N_assert(lightCap, "Shadow renderable is missing a separate light cap renderable!");

                    /* 我们必须小心使用灯亮罩,那样可以直接"看到"背面三角形
                        1. 前面对灯亮罩渲染时必须总是fail depth 检测去避免"深度混乱"
                        2. 背面对灯亮罩必须使用正常深度函数,避免打破标准深度测试
                     // TODO:
                        1. 分开灯亮罩渲染,不需要`封闭`网格,从不需要接触近平面,因为在这个
                            情况下,我们不能直接`看到`任何背面三角形。这个`封闭`网格必须
                            由边缘列表生成器确定
                        2. 在共面三角形的相反面向中,这个仍然存在`深度混乱`,在使用2个面材
                            质时,在模型工具和模型导出工具导出NIId三角形描述的模型时通常
                            会发生。这个错误不能仅从通道GPU中修复,这必须在边缘列表生成器
                            中,还有阴影体积生成器中有额外工作解决它
                    */
                    if(twosided)
                    {
                        // 选择背面对灯亮罩去渲染
                        mRenderSys->_setCullingMode(CM_Anticlockwise);
                        mCullingMode = CM_Anticlockwise;
                        // 为背面对灯亮罩,使用正常深度函数
                        mParent->render(lightCap, pass, *lights, false);

                        // 选择正面对灯亮罩去渲染
                        mRenderSys->_setCullingMode(CM_Clockwise);
                        mCullingMode = CM_Clockwise;
                        // 前面对灯光招,必须始终fail depth检测
                        mRenderSys->_setDepthOpFunc(CPM_ALWAYS_FAIL);
                        mParent->render(lightCap, pass, *lights, false);

                        // 重置深度函数
                        mRenderSys->_setDepthOpFunc(CPM_LESS);
                        // 重置拣选模式
                        mRenderSys->_setCullingMode(CM_None);
                        mCullingMode = CM_None;
                    }
                    else if((secondpass || zfail) && !(secondpass && zfail))
                    {
                        // 为背面对灯亮罩使用正常的深度函数
                        mParent->render(lightCap, pass, *lights, false);
                    }
                    else
                    {
                        // 为前面对灯亮罩,必须总是不通过深度测试
                        mRenderSys->_setDepthOpFunc(CPM_ALWAYS_FAIL);
                        mParent->render(lightCap, pass, *lights, false);
                        // 重置深度函数
                        mRenderSys->_setDepthOpFunc(CPM_LESS);
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
}