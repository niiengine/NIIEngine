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
                // ����������ӰԶ������
                Vector3f toObj = object->getSpaceNode()->getSpacePos() - mCamera->getSpacePos();
                NIIf radius = object->getSpaceBS().getRadius();
                NIIf dist =  toObj.lengthSquared();
                if(dist - (radius * radius) > mFarDistSquared)
                {
                    // ����,�������Χ
                    return true;
                }
            }

            // ���������ƽ��ͷ��,���������ܿ�����Ӱ
            if(mCamera->isIntersect(svg->getSpaceAABB()))
            {
                mCasterList->push_back(svg);
                return true;
            }

            /* ����,����ƹ���ƽ��ͷ��(���Ƿ���ƹ�,���������)����ֻ��Ͷ��һ����Ӱ������
                ���ӿ�,���������������ƽ��ͷ�͵ƹ�����ཻ
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
            N_Only(Log).log(_I18n("����: Ӳ����Ҫ֧��ģ����� "));
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
            // ��������Զ��
            cam->setFarClipDistance(0);
        }
    }
    //------------------------------------------------------------------------
    void StencilRenderPattern::sortLight(LightList & dest)
    {
        NCount index;
        // ����(�ȶ��ı�֤����ƹ�˳��)
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

                    // ��������(����)��ƹ⼷��,��Ϊ��ú�����һЩ����
                    //���ö�����ɫ����/Ƭ����ɫ����
                    mDebugCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS,
                        ShadowVolumeExtrudeProgram::programNames[ShadowVolumeExtrudeProgram::POINT_LIGHT]);
                    mDebugCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS,
                        ShadowVolumeExtrudeProgram::frgProgramName);
                    mDebugCh->getProgram().getParam(ShaderChProgram::ST_VS, mShaderData->mInfiniteParam);
                    //������Ӱ��ɫ����ʹ�õ��Զ�����
                    mShaderData->mInfiniteParam->set(0, GSP_Matrix_Proj_X_View_X_Space);
                    mShaderData->mInfiniteParam->set(4, GSP_Light_Pos_ChSpace_ParamIndex);
                    // ע����Զ���Ĳ���-Ϊ�������޼���������ɫ����
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
                    // �����������޵�ƹ⼷��,��Ϊ���һЩ����
                    mStencilCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS, ShadowVolumeExtrudeProgram::programNames[ShadowVolumeExtrudeProgram::POINT_LIGHT_FINITE]);
                    mStencilCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS, ShadowVolumeExtrudeProgram::frgProgramName);
                    mStencilCh->getProgram().getParam(ShaderChProgram::ST_VS, mShaderData->mFiniteParam);
                    // �����Զ�����
                    mShaderData->mFiniteParam->set(0, GSP_Matrix_Proj_X_View_X_Space);
                    mShaderData->mFiniteParam->set(4, GSP_Light_Pos_ChSpace_ParamIndex);
                    // ע�����Ĳ���
                    mShaderData->mFiniteParam->set(5, GSP_Shadow_VolExtent);
                }
                matStencil->check();
                // û��������,���ǲ�ʹ����һ��"����"ͨ������,����һ��ռλ��
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
                // ��ʼ��
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

        // Ҳ��ʼ��ȫ���ķ�
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

        // ������׼��ʱ,�ı���Ӱ��������,����,���Ὠ��׼��ʱ����Ӱ����
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
        // ����,��zpass����Ҫһ������
        /*  1:��/�۵ƹ��ڵ�����Ӱģʽ�����޼�ѹ,������û�����(skybox)����
            ��,����ͶӰ��Ӱ����һ������
            2:���κεƹ���Դ���޼�����,��Ϊ�鿴�Ƕȼ���������Ӱ���������
            ����ȷ��
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
        // ��ȡ��ӰͶ���б�
        const StencilObjList & casters = findCastList(light, camera);
        if(casters.empty())
            return;

        LightList lightList;
        // const_cast������,��Ϊ����������ǳ���
        lightList.push_back(const_cast<Light *>(light));

        // �����ü�����(����&�۹��)
        SpaceManager::ClipType scissored = SpaceManager::CT_None;
        if(clip)
        {
            scissored = mParent->applyClip(lightList, camera);
            if(scissored == SpaceManager::CT_All)
                return;
        }

        mRenderSys->off(GpuProgram::ST_FS);

        bool extrudeInSoftware = true;
        // �Ƿ�ʹ�ö�����ɫ��������ѹ?
        bool finiteExtrude = !mInfiniteFar ||
            !mRenderSys->getFeature()->isSupport(GF_Infinite_FarPlane);
        if(mRenderSys->getFeature()->isSupport(GF_Program_Vertex))
        {
            extrudeInSoftware = false;
            mStencilCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS, ShadowVolumeExtrudeProgram::frgProgramName);
            // ���ò���
            if(finiteExtrude)
            {
                // ������Ӧ�ļ�ѹ������ɫ����,ע�����Ǵ���û��������,��Ϊ������ɫ�����ǳ���
                mStencilCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS, 
                    ShadowVolumeExtrudeProgram::getProgramName(light->getType(), finiteExtrude, false),
                    mShaderData->mFiniteParam);
            }
            else
            {
                // ������Ӧ�ļ�ѹ������ɫ����,ע�����Ǵ���û��������,��Ϊ������ɫ�����ǳ���
                mStencilCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS,
                    ShadowVolumeExtrudeProgram::getProgramName(light->getType(), finiteExtrude, false),
                    mShaderData->mInfiniteParam);
            }
            if(mDebugShadows)
            {
                mDebugCh->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS,
                    ShadowVolumeExtrudeProgram::frgProgramName);

                // ���ò���
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

        // �ر���ɫд������д��
        mRenderSys->setColourWrite(ColourMark::CM_ZERO);
        mRenderSys->_enableTexture(0);
        mRenderSys->setStencil(ShaderChStencil(true, CPM_LESS, false), SB_Depth);
        mRenderSys->setStencilTest(true);

        // ���㼷ѹ����
        // ����ʹ�÷���ƹ⼷ѹ����, ���Ż��㷨������һ��,��/��ƹ�������
        NIIf extrudeDist = mShadowExtent;

        const PlaneSpace & nearClipVol = getNearClip(light, camera);

        // ���ڱ���Ͷ�����Ⱦ
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

            // ȷ���Ƿ���Ҫzfail
            if(zfailAlgo || nearClipVol.intersects(caster->getSpaceAABB()))
            {
                // Ϊ�������,����ʹ��zfail,����Ϊzfail����zpass�㷨
                zfailAlgo = true;
                // ������Ҫ���������ֻ���,���������ǿɼ�ʱ
                if(camera->isIntersect(caster->getFrontCapAABB()))
                {
                    flags |= ShadowObj::VT_FrontCap;
                }
                // zfail ��Ҫ����
                // ���Ƿ���ƹ�ʹ��Ӳ������Ϊ����Զ,��Ϊ����Ϊ��һ��
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
            // ��ȡ��Ӱ����Ⱦ��
            const ShadowVolumeList & iShadowRenderables =
                caster->getVolumeList(light, extrudeDist, flags, mIndexBuffer);

            // ��Ⱦһ����Ӱ�������
            //  - �������֧��2���ɰ幦��,ʹ���޼�ѡ��Ⱦ
            //  - ����,��Ⱦ2��,����ÿ����ѡ�����ͷ�ת����
            applyStencil(false, zfailAlgo);

            renderShadows(iShadowRenderables, mStencilCh, &lightList, false, zfailAlgo,
                mBothSideStencil, flags);

            if(!mBothSideStencil)
            {
                // �ڶ���ͨ��
                applyStencil(true, zfailAlgo);

                renderShadows(iShadowRenderables, mStencilCh, &lightList, true, zfailAlgo,
                    false, flags);
            }

            // ������Ҫ��Ⱦһ��������Ӱ���?
            if(mDebugShadows)
            {
                // �����ɰ�&��ɫ����
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
            // �����һ������
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
            // ִ��,ʹ�ûص�
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

                // ȷ���ƹ���ƽ��ͷ������
                bool lightInFrustum = camera->isIntersect(light->getSpacePos());
                const PlaneSpaceList * volList = 0;
                if(!lightInFrustum)
                {
                    // ����ƹ���ƽ��ͷ��,������һ����������б�
                    volList = &(getFrustumClip(light, camera));
                }
                temp = light->getShadowExtent();
                // ִ��,ʹ�ûص�
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

        // ���zpassΪ��ʱ,��һͨ����������
        // ���zpassΪ��ʱ,�ڶ�ͨ����������
        // ���zfail��ת
        // ��2��ģ��ʱ,����ȥȷ�������ڵݼ�ǰ���ǵ���,������������ģ����������͵���������ʱ��ת����
        if(!mBothSideStencil && ((secondrender || zfail) && !(secondrender && zfail)))
        {
            mCullingMode = mBothSideStencil ? CM_None : CM_Anticlockwise;
            mRenderSys->setStencil(ShaderChStencil(
                mBothSideStencil,
                0xFFFFFFFF,                 // û�� mask
                0xFFFFFFFF,                 // û�� cmpmask
                CPM_ALWAYS_PASS,             // ����ͨ���ɰ���
                0,                          // û��refֵ(�ޱȽ�)
                SOT_KEEP,                   // �ɰ��������ͨ��
                zfail ? incrOp : SOT_KEEP,  // ������Ȳ�ͨ��ʱ
                zfail ? SOT_KEEP : decrOp   // ����ͨ��ʱ
                ), SB_Stencil);
        }
        else
        {
            mCullingMode = mBothSideStencil ? CM_None : CM_Clockwise;
            mRenderSys->setStencil(ShaderChStencil(
                mBothSideStencil,
                0xFFFFFFFF,                 // û�� mask
                0xFFFFFFFF,                 // û�� cmpmask
                CPM_ALWAYS_PASS,             // ����ͨ���ɰ���
                0,                          // û��refֵ(�ޱȽ�)
                SOT_KEEP,                   // �ɰ��������ͨ��
                zfail ? decrOp : SOT_KEEP,  // ������Ȳ�ͨ��ʱ
                zfail ? SOT_KEEP : incrOp  // ����ͨ��ʱ
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
        // ʹ����ͬ���ɰ����,��Ⱦ������Ӱ����Ⱦ��
        ShadowVolumeList::const_iterator i, iend = src.end();
        for(i = src.begin(); i != iend; ++i)
        {
            ShadowVolume * sr = *i;
            // �������صĿ���Ⱦ��
            if(sr->isVisible())
            {
                // ��Ⱦ���,�������ֺ͵�����
                mParent->render(sr, pass, *lights, false);

                // ��ѡ����Ⱦ�ָ�ƹ���
                if((sr->getLightCap() != 0) && (flags & ShadowObj::VT_FrontCap))
                {
                    ShadowVolume * lightCap = sr->getLightCap();
                    N_assert(lightCap, "Shadow renderable is missing a separate light cap renderable!");

                    /* ���Ǳ���С��ʹ�õ�����,��������ֱ��"����"����������
                        1. ǰ��Ե�������Ⱦʱ��������fail depth ���ȥ����"��Ȼ���"
                        2. ����Ե����ֱ���ʹ��������Ⱥ���,������Ʊ�׼��Ȳ���
                     // TODO:
                        1. �ֿ���������Ⱦ,����Ҫ`���`����,�Ӳ���Ҫ�Ӵ���ƽ��,��Ϊ�����
                            �����,���ǲ���ֱ��`����`�κα��������Ρ����`���`�������
                            �ɱ�Ե�б�������ȷ��
                        2. �ڹ��������ε��෴������,�����Ȼ����`��Ȼ���`,��ʹ��2�����
                            ��ʱ,��ģ�͹��ߺ�ģ�͵������ߵ���NIId������������ģ��ʱͨ��
                            �ᷢ������������ܽ���ͨ��GPU���޸�,������ڱ�Ե�б�������
                            ��,������Ӱ������������ж��⹤�������
                    */
                    if(twosided)
                    {
                        // ѡ����Ե�����ȥ��Ⱦ
                        mRenderSys->_setCullingMode(CM_Anticlockwise);
                        mCullingMode = CM_Anticlockwise;
                        // Ϊ����Ե�����,ʹ��������Ⱥ���
                        mParent->render(lightCap, pass, *lights, false);

                        // ѡ������Ե�����ȥ��Ⱦ
                        mRenderSys->_setCullingMode(CM_Clockwise);
                        mCullingMode = CM_Clockwise;
                        // ǰ��Եƹ���,����ʼ��fail depth���
                        mRenderSys->_setDepthOpFunc(CPM_ALWAYS_FAIL);
                        mParent->render(lightCap, pass, *lights, false);

                        // ������Ⱥ���
                        mRenderSys->_setDepthOpFunc(CPM_LESS);
                        // ���ü�ѡģʽ
                        mRenderSys->_setCullingMode(CM_None);
                        mCullingMode = CM_None;
                    }
                    else if((secondpass || zfail) && !(secondpass && zfail))
                    {
                        // Ϊ����Ե�����ʹ����������Ⱥ���
                        mParent->render(lightCap, pass, *lights, false);
                    }
                    else
                    {
                        // Ϊǰ��Ե�����,�������ǲ�ͨ����Ȳ���
                        mRenderSys->_setDepthOpFunc(CPM_ALWAYS_FAIL);
                        mParent->render(lightCap, pass, *lights, false);
                        // ������Ⱥ���
                        mRenderSys->_setDepthOpFunc(CPM_LESS);
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
}