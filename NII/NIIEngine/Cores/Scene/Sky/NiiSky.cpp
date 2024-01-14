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
#include "NiiSky.h"
#include "NiiEngine.h"
#include "NiiCustomSpaceObj.h"
#include "NiiSpaceManager.h"
#include "NiiMaterialManager.h"
#include "NiiLogManager.h"
#include "NiiMeshManager.h"
#include "NiiGeometrySpace.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Sky
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    Sky::Sky() :
        mEnable(true),
        mNode(0),
        mResourceGroup(GroupDefault),
        mRenderGroup(RQG_NearEnd)
    {
        mParent = N_Engine().getProcessSpace();
    }
    //------------------------------------------------------------------------
    Sky::Sky(const SpaceManager * gm) :
        mParent(const_cast<SpaceManager *>(gm)),
        mNode(0),
        mEnable(true),
        mResourceGroup(GroupDefault),
        mRenderGroup(RQG_NearEnd)
    {
    }
    //------------------------------------------------------------------------
    Sky::~Sky()
    {
    }
    //------------------------------------------------------------------------
    void Sky::render(Camera * obj)
    {
        if(mNode)
        {
            mNode->setPos(obj->getSpacePos());
        }
        if(mEnable)
            renderImpl(mParent->getRenderQueue());
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // BoxSky
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    BoxSky::BoxSky() :
        Sky(),
        mGeo(0),
        mOriOft(Quaternion::IDENTITY)
    {
        mMaterial[0] = 0;
        mMaterial[1] = 0;
        mMaterial[2] = 0;
        mMaterial[3] = 0;
        mMaterial[4] = 0;
        mMaterial[5] = 0;
        mMaterial[6] = 0;
    }
    //------------------------------------------------------------------------
    BoxSky::BoxSky(const SpaceManager * gm):
        Sky(gm),
        mGeo(0),
        mOriOft(Quaternion::IDENTITY)
    {
        mMaterial[0] = 0;
        mMaterial[1] = 0;
        mMaterial[2] = 0;
        mMaterial[3] = 0;
        mMaterial[4] = 0;
        mMaterial[5] = 0;
        mMaterial[6] = 0;
    }
    //------------------------------------------------------------------------
    BoxSky::~BoxSky()
    {
        if(mNode)
        {
            if(mGeo)
                mNode->detach(mGeo);
            mParent->destroySpaceNode(mNode->getID());
            mNode = 0;
        }
        if(mGeo)
        {
            N_delete mGeo;
            mGeo = 0;
        }
        for(NCount i = 0; i < 7; ++i)
        {
            if(mMaterial[i] != 0)
            {
                N_Only(Material).destroy(mMaterial[i]);
                mMaterial[i] = 0;
            }
        }
    }
    //------------------------------------------------------------------------
    void BoxSky::setMaterial(ResourceID rid)
    {
        Material * m = static_cast<Material *>(N_Only(Material).get(rid, mResourceGroup));
        if(m == 0)
        {
            N_EXCEPT(InvalidParam, _I18n("盒天空材质找不到: '") + rid);
        }
        m->load();

        if(!m->getLOD() || !m->getLOD()->getCount())
        {
            N_Only(Log).log(_I18n("Warning, skybox material ") + N_conv(rid) + 
                _I18n(" is not supported, defaulting."));
            m = N_Only(Material).getNull();
        }
        mMaterial[0] = rid;
    }
    //-----------------------------------------------------------------------
    void BoxSky::buildGeo(NIIf distance, const Quaternion & ori_oft)
    {
        if(mEnable)
        {
            bool cubetex = false;
            Material * m = static_cast<Material *>(N_Only(Material).get(mMaterial[0], mResourceGroup));
            if(m == 0)
            {
                N_EXCEPT(InvalidParam, _I18n("盒天空材质找不到: '") + mMaterial[0]);
            }

            ShaderCh * ch = m->getLOD()->get(0);
            if(ch->getTexture().getCount() > 0 &&
                ch->getTexture().get(0)->getType() == Texture::T_CUBE)
                cubetex = true;

            // 创建节点
            if(!mNode)
            {
                 mNode = mParent->createSpaceNode(0);
            }

            // 创建对象
            if(!mGeo)
            {
                mGeo = N_new CustomSpaceObj();
                mGeo->setCastShadow(false);
                mNode->attach(mGeo);
            }
            else
            {
                if(!mGeo->isAttach())
                {
                    mNode->attach(mGeo);
                }
                mGeo->clear();
            }

            mGeo->setRenderGroup(mRenderGroup);

            if(cubetex)
            {
                mGeo->begin(GeometryRaw::OT_TRIANGLE_LIST, mMaterial[0]);
            }

            // 建立盒子 (6 个平面面)
            for(NCount i = 0; i < 6; ++i)
            {
                Plane plane;
                String meshName;
                Vector3f middle;
                Vector3f up, right;

                switch(i)
                {
                case PT_Front:
                    middle = Vector3f(0, 0, -distance);
                    up = Vector3f::Y * distance;
                    right = Vector3f::X * distance;
                    break;
                case PT_Back:
                    middle = Vector3f(0, 0, distance);
                    up = Vector3f::Y * distance;
                    right = Vector3f::INV_X * distance;
                    break;
                case PT_Left:
                    middle = Vector3f(-distance, 0, 0);
                    up = Vector3f::Y * distance;
                    right = Vector3f::INV_Z * distance;
                    break;
                case PT_Right:
                    middle = Vector3f(distance, 0, 0);
                    up = Vector3f::Y * distance;
                    right = Vector3f::Z * distance;
                    break;
                case PT_Up:
                    middle = Vector3f(0, distance, 0);
                    up = Vector3f::Z * distance;
                    right = Vector3f::X * distance;
                    break;
                case PT_Down:
                    middle = Vector3f(0, -distance, 0);
                    up = Vector3f::INV_Z * distance;
                    right = Vector3f::X * distance;
                    break;
                }
                // 通过方向改变
                middle = ori_oft * middle;
                up = ori_oft * up;
                right = ori_oft * right;

                if(cubetex)
                {
                    // 三维立方体纹理
                    // Note UVs mirrored front/back
                    // I could save a few vertices here by sharing the corners
                    // since 3D coords will function correctly but it's really not worth
                    // making the code more complicated for the sake of 16 verts
                    // top left
                    Vector3f pos, cpos, norl, temp;
                    pos = middle + up - right;
                    mGeo->position(pos.x, pos.y, pos.z);
                    pos.normalise(norl);
                    temp = norl * Vector3f(1, 1, -1);
                    mGeo->textureCoord(temp.x, temp.y, temp.z);
                    // bottom left
                    pos = middle - up - right;
                    mGeo->position(pos.x, pos.y, pos.z);
                    pos.normalise(norl);
                    temp = norl * Vector3f(1, 1, -1);
                    mGeo->textureCoord(temp.x, temp.y, temp.z);
                    // bottom right
                    pos = middle - up + right;
                    mGeo->position(pos.x, pos.y, pos.z);
                    pos.normalise(norl);
                    temp = norl * Vector3f(1, 1, -1);
                    mGeo->textureCoord(temp.x, temp.y, temp.z);
                    // top right
                    pos = middle + up + right;
                    mGeo->position(pos.x, pos.y, pos.z);
                    pos.normalise(norl);
                    temp = norl * Vector3f(1, 1, -1);
                    mGeo->textureCoord(temp.x, temp.y, temp.z);

                    Nui32 base = i * 4;
                    mGeo->quad(base, base+1, base+2, base+3);

                }
                else // !cubetex
                {
                    Vector3f temp;
                    // If we're using 6 separate images, have to create 6 materials, one for each frame
                    // Used to use combined material but now we're using queue we can't split to change frame
                    // This doesn't use much memory because textures aren't duplicated
                    mMaterial[i + 1] = N_Only(Material).genValidID();
                    ResourceID matName = mMaterial[i + 1];
                    Material * boxMat = static_cast<Material *>(N_Only(Material).get(matName, mResourceGroup));
                    if(boxMat == 0)
                    {
                        // 通过复制创建新的
                        boxMat = m->clone(matName);
                        boxMat->load();
                    }
                    else
                    {
                        // 复制现有的
                        *boxMat = *m;
                        boxMat->load();
                    }
                    // 确保材料不更新深度缓存
                    N_Only(Material).setDepthWrite(boxMat, false);
                    // 设置活动框架
                    ShaderFusionList::const_iterator j, jend = boxMat->getValidList().end();
                    for(j = boxMat->getValidList().begin(); j != jend; ++j)
                    {
                        ShaderFusion * tech = *j;
                        if(tech->get(0)->getTexture().getCount() > 0)
                        {
                            ShaderChTextureUnit * t = tech->get(0)->getTexture().get(0);
                            t->getSample()->setMode(TAM_CLAMP, TAM_CLAMP, TAM_CLAMP);
                            t->setCurrentFrame(i);
                        }
                    }

                    // section per material
                    mGeo->begin(GeometryRaw::OT_TRIANGLE_LIST, matName, mResourceGroup);
                    temp = middle + up - right;
                    // top left
                    mGeo->position(temp.x, temp.y, temp.z);
                    mGeo->textureCoord(0,0);
                    temp = middle - up - right;
                    // bottom left
                    mGeo->position(temp.x, temp.y, temp.z);
                    mGeo->textureCoord(0,1);
                    temp = middle - up + right;
                    // bottom right
                    mGeo->position(temp.x, temp.y, temp.z);
                    mGeo->textureCoord(1,1);
                    temp = middle + up + right;
                    // top right
                    mGeo->position(temp.x, temp.y, temp.z);
                    mGeo->textureCoord(1,0);

                    mGeo->quad(0, 1, 2, 3);

                    mGeo->end();
                }
            } // 每个面

            if(cubetex)
            {
                mGeo->end();
            }
        }
        mRange = distance;
        mOriOft = ori_oft;
    }
    //------------------------------------------------------------------------
    void BoxSky::renderImpl(RenderQueue * rq)
    {
        mGeo->queue(rq);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // DomeSky
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    DomeSky::DomeSky()
    {
        mGeo[0] = 0;
        mGeo[1] = 0;
        mGeo[2] = 0;
        mGeo[3] = 0;
        mGeo[4] = 0;
    }
    //------------------------------------------------------------------------
    DomeSky::DomeSky(const SpaceManager * gm) :
        Sky(gm)
    {
        mGeo[0] = 0;
        mGeo[1] = 0;
        mGeo[2] = 0;
        mGeo[3] = 0;
        mGeo[4] = 0;
    }
    //------------------------------------------------------------------------
    DomeSky::~DomeSky()
    {
        NCount i;
        if(mNode)
        {
            for(i = 0; i < 5; ++i)
            {
                if(mGeo[i])
                    mNode->detach(mGeo[i]);
            }
            mParent->destroySpaceNode(mNode->getID());
            mNode = 0;
        }

        for(i = 0; i < 5; ++i)
        {
            if(mGeo[i])
            {
                mParent->destroyGeo(mGeo[i]->getID());
                mGeo[i] = NULL;
            }
        }

        for(i = 0; i < 5; ++i)
        {
            if(mResource[i] != 0)
            {
                N_Only(Mesh).destroy(mResource[i]);
                mResource[i] = 0;
            }
        }
    }
    //------------------------------------------------------------------------
    void DomeSky::setMaterial(ResourceID rid)
    {
        Material * m = static_cast<Material *>(N_Only(Material).get(rid, mResourceGroup));
        if(m == 0)
        {
            N_EXCEPT(InvalidParam, _I18n("穹天空材质不能找到: '") + rid);
        }
        // 确保材料不更新深度缓存
        N_Only(Material).setDepthWrite(m, false);
        m->load();
        rid = mMaterial;
    }
    //------------------------------------------------------------------------
    void DomeSky::buildGeo(NIIf curvature, NIIf distance, NIIf xtiling, NIIf ytiling,
         NIIi xsegments, NIIi ysegments, const Quaternion & ori_oft)
    {
        if(mEnable)
        {
            if(!mNode)
            {
                 mNode = mParent->createSpaceNode(0);
            }
            else
            {
                 mNode->detachAll();
            }

            // 设置圆顶(5个面)
            for(NCount i = 0; i < 5; ++i)
            {
                if(mResource[i] != 0)
                {
                    N_Only(Mesh).destroy(mResource[i]);
                }

                Mesh * pm = createPlaneImpl(mResource[i], mResourceGroup, (PlaneType)i,
                    curvature, distance, ori_oft, xtiling, ytiling, xsegments, ysegments);

                if(mGeo[i])
                {
                     mParent->destroyGeo(mGeo[i]->getID());
                     mGeo[i] = 0;
                }

                SpaceObjFactory * factory = N_Engine().getFactory(N_FACTORY_GeoSpace);
                PropertyData params;
                params.add(N_PropertyMesh_ID, N_conv(pm->getOriginID()));
                mGeo[i] = static_cast<GeometrySpace *>(factory->create(0, mParent, &params));
                mGeo[i]->setMaterial(mMaterial, mResourceGroup);
                mGeo[i]->setCastShadow(false);

                mParent->addSpace(mGeo[i], N_FACTORY_GeoSpace);

                // 连接到节点
                mNode->attach(mGeo[i]);
            } // 每个面

        }
        mCurveFactor = curvature;
        mRange = distance;
        mXTiling = xtiling;
        mYTiling = ytiling;
        mXSegment = xsegments;
        mYSegment = ysegments;
    }
    //-----------------------------------------------------------------------
    Mesh * DomeSky::createPlaneImpl(ResourceID & rid, GroupID gid, PlaneType bp,
        NIIf curvature, NIIf distance, const Quaternion & ori_oft, NIIf xtiling,
            NIIf ytiling, NIIi xsegments, NIIi ysegments) const
    {
        Plane plane;
        Vector3f up;

        if(rid == 0)
        {
            rid = N_Only(Mesh).genValidID();
        }

        // 设置平面方程
        plane.mD = distance;
        switch(bp)
        {
        case PT_Front:
            plane.mNormal = Vector3f::Z;
            up = Vector3f::Y;
            break;
        case PT_Back:
            plane.mNormal = -Vector3f::Z;
            up = Vector3f::Y;
            break;
        case PT_Left:
            plane.mNormal = Vector3f::X;
            up = Vector3f::Y;
            break;
        case PT_Right:
            plane.mNormal = -Vector3f::X;
            up = Vector3f::Y;
            break;
        case PT_Up:
            plane.mNormal = -Vector3f::Y;
            up = Vector3f::Z;
            break;
        case PT_Down:
            return 0;
        }
        plane.mNormal = ori_oft * plane.mNormal;
        up = ori_oft * up;

        Mesh * pm = static_cast<Mesh *>(N_Only(Mesh).get(rid, gid));
        if(pm != 0)
        {
            N_Only(Mesh).destroy(pm->getPrc());
        }
        // 创建新的
        NIIf planeSize = distance * 2;
        pm = N_Only(Mesh).createCurvedIllusion(rid, gid, plane, planeSize, planeSize,
            curvature, xsegments, ysegments, -1, false, 1, xtiling, ytiling, up,
                ori_oft, Buffer::M_WRITE, Buffer::M_DEV | Buffer::M_WRITE);
        return pm;
    }
    //------------------------------------------------------------------------
    void DomeSky::renderImpl(RenderQueue * rq)
    {
        rq->add(mGeo[0]->getCom(0), mRenderGroup, N_Geo_Default_Level);
        rq->add(mGeo[1]->getCom(0), mRenderGroup, N_Geo_Default_Level);
        rq->add(mGeo[2]->getCom(0), mRenderGroup, N_Geo_Default_Level);
        rq->add(mGeo[3]->getCom(0), mRenderGroup, N_Geo_Default_Level);
        rq->add(mGeo[4]->getCom(0), mRenderGroup, N_Geo_Default_Level);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // PlaneSky
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    PlaneSky::PlaneSky() :
        mGeo(0)
    {
        mMesh = N_Only(Mesh).genValidID();
    }
    //------------------------------------------------------------------------
    PlaneSky::PlaneSky(const SpaceManager * gm) :
        Sky(gm),
        mGeo(0)
    {
        mMesh = N_Only(Mesh).genValidID();
    }
    //------------------------------------------------------------------------
    PlaneSky::~PlaneSky()
    {
        if(mNode != 0)
        {
            if(mGeo)
                mNode->detach(mGeo);
            mParent->destroySpaceNode(mNode->getID());
        }
        
        if(mGeo)
        {
            mParent->destroyGeo(mMesh);
            mGeo = 0;
        }
    }
    //------------------------------------------------------------------------
    void PlaneSky::setMaterial(ResourceID rid)
    {
        Material * m = static_cast<Material *>(N_Only(Material).get(rid, mResourceGroup));
        if(m == 0)
        {
            N_EXCEPT(InvalidParam, _I18n("Sky plane material '") + N_conv(rid) + _I18n("' not found."));
        }
        // 确保材料不更新深度缓存
        N_Only(Material).setDepthWrite(m, false);
        m->load();
        mMaterial = rid;
    }
    //-----------------------------------------------------------------------
    void PlaneSky::buildGeo(const Plane & plane, NIIf gscale, NIIf xtiling, NIIf ytiling,
        NIIi xsegments, NIIi ysegments, NIIf bow)
    {
        if(mEnable)
        {
            mPlane = plane;

            Mesh * planeMesh = static_cast<Mesh *>(N_Only(Mesh).get(mMesh));
            if(planeMesh != 0)
            {
                N_Only(Mesh).destroy(planeMesh->getPrc());
            }

            // 创建向上向量
            Vector3f up = plane.mNormal.crossProduct(Vector3f::X);
            if(up == Vector3f::ZERO)
                up = plane.mNormal.crossProduct(-Vector3f::Z);

            if(bow > 0)
            {
                // 建立一个平面弯曲的天空
                planeMesh = N_Only(Mesh).createCurved(mMesh, mResourceGroup,
                    plane, gscale * 100, gscale * 100, gscale * bow * 100,
                        xsegments, ysegments, false, 1, xtiling, ytiling, up);
            }
            else
            {
                planeMesh = N_Only(Mesh).createPlane(mMesh, mResourceGroup,
                    plane, gscale * 100, gscale * 100, xsegments, ysegments, false,
                        1, xtiling, ytiling, up);
            }

            // 创建实体
            if(mGeo)
            {
                 // destroy old one, do it by name for speed
                 mParent->destroyGeo(mMesh);
                 mGeo = 0;
            }
            // Create, use the same name for mesh and entity
            // manually construct as we don't want this to be destroyed on destroyAllSpace
            SpaceObjFactory * factory = N_Engine().getFactory(N_FACTORY_GeoSpace);
            PropertyData params;
            params.add(N_PropertyMesh_ID, N_conv(mMesh));
            mGeo = static_cast<GeometrySpace *>(factory->create(0, mParent, &params));
            mGeo->setMaterial(mMaterial, mResourceGroup);
            mGeo->setCastShadow(false);

            mParent->addSpace(mGeo, N_FACTORY_GeoSpace);

            // 创建节点和附加上
            if(!mNode)
            {
                 mNode = mParent->createSpaceNode(0);
            }
            else
            {
                 mNode->detachAll();
            }
            mNode->attach(mGeo);
        }

        mBow = bow;
        mScale = gscale;
        mXTiling = xtiling;
        mYTiling = ytiling;
        mXSegment = xsegments;
        mYSegment = ysegments;
    }
    //------------------------------------------------------------------------
    void PlaneSky::renderImpl(RenderQueue * rq)
    {
        rq->add(mGeo->getCom(0), mRenderGroup, N_Geo_Default_Level);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // VideoSky
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    VideoSky::VideoSky():
        mVideoRID(0),
        mTimeFactor(1.0),
        mGeo(0)
    {
    }
    //------------------------------------------------------------------------
    VideoSky::VideoSky(const SpaceManager * gm):
        Sky(gm),
        mTimeFactor(1.0),
        mGeo(0)
    {
    }
    //------------------------------------------------------------------------
    VideoSky::~VideoSky()
    {
        if(mGeo)
        {
        }
    }
    //------------------------------------------------------------------------
    void VideoSky::buildGeo(const Plane & plane, NIIf scale, NIIf width, NIIf height)
    {
        mPlane = plane;
    }
    //------------------------------------------------------------------------
    void VideoSky::renderImpl(RenderQueue * rq)
    {
    
    }
    //------------------------------------------------------------------------ 
}