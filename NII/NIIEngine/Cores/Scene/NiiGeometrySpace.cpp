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
#include "NiiGeometrySpace.h"
#include "NiiSpaceManager.h"
#include "NiiLogManager.h"
#include "NiiMesh.h"
#include "NiiException.h"
#include "NiiCamera.h"
#include "NiiMaterialManager.h"
#include "NiiGBufferManager.h"
#include "NiiResourceSchemeManager.h"
#include "NiiTagBone.h"
#include "NiiEngine.h"
#include "NiiSkeleton.h"
#include "NiiSimdUtil.h"
#include "NiiRenderPattern.h"
#include "NiiVertexFusion.h"
#include "NiiMeshManager.h"
#include "NiiMeshLodScheme.h"
#include "NiiMaterialLodScheme.h"
#include "NiiNodeKeyFrameTrack.h"
#include "NiiVertexOffsetKeyFrameTrack.h"
#include "NiiVertexUnitKeyFrameTrack.h"
#include "NiiGeometryObj.h"
#include "NiiLight.h"

namespace NII
{
    /** 临时顶点缓存
    @version NIIEngine 3.0.0
    */
    class _EngineInner AniTempBuffer : public TempBufferCtl, public BufferAlloc
    {
    public:
        AniTempBuffer(bool autoDsy = true);
        ~AniTempBuffer();

        /** 设置映射源
        @param[in] src 源缓存
        @version NIIEngine 3.0.0
        */
        void map(const VertexData * src);

        /** 分配副本
        @version NIIEngine 3.0.0
        */
        void alloc(bool pos = true, bool normal = true);

        /** 续定临时缓存
        @version NIIEngine 3.0.0
        */
        bool refill(bool pos = true, bool normal = true) const;

        /** 绑定副本到指定顶点数据中
        @param[in] dst 指定顶点数据
        @param[in] syn 是否同步前后缓存
        @version NIIEngine 3.0.0
        */
        void bind(VertexData * dst, bool syn);

        /// @copydetails TempBufferCtl::expire
        void expire(Buffer * buffer);
    private:
        VertexBuffer * mSrcPos;     ///< 源头指针(位置)
        VertexBuffer * mSrcNrls;    ///< 源头指针(法线)
        VertexBuffer * mDstPos;     ///< 副本指针(位置)
        VertexBuffer * mDstNrls;    ///< 副本指针(法线)
        Nui16 mPosIndex;
        Nui16 mNrlsIndex;
        Nui8 mMark;
    };
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // AniTempBuffer
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    AniTempBuffer::AniTempBuffer(bool autoDsy) :
        TempBufferCtl(autoDsy)
    {
    }
    //------------------------------------------------------------------------
    AniTempBuffer::~AniTempBuffer()
    {
        // 检测临时缓存已经释放
        if(mDstPos != 0)
        {
            mDstPos->getManager()->freeTemp(mDstPos);
            mDstPos = 0;
        }
        if(mDstNrls != 0)
        {
            mDstNrls->getManager()->freeTemp(mDstNrls);
            mDstNrls = 0;
        }
    }
    //------------------------------------------------------------------------
    void AniTempBuffer::map(const VertexData * src)
    {
        // 先释放旧缓存副本
        if(mDstPos != 0)
        {
            mDstPos->getManager()->freeTemp(mDstPos);
            mDstPos = 0;
        }
        if(mDstNrls != 0)
        {
            mDstNrls->getManager()->freeTemp(mDstNrls);
            mDstNrls = 0;
        }

        // 索引为0
        const VertexUnit * pos = src->get(VT_Pos);
        const VertexUnit * nrls = src->get(VT_Normals);

        N_assert(pos, "Positions are required");

        mPosIndex = pos->mSource;
        mSrcPos = src->getAttach(mPosIndex);

        if(!nrls)
        {
            mMark &= T_N_SHARE_BUFFER;
            mSrcNrls = 0;
        }
        else
        {
            mNrlsIndex = nrls->mSource;
            if(mNrlsIndex == mPosIndex)
            {
                mMark |= T_SHARE_BUFFER;
                mSrcNrls = 0;
            }
            else
            {
                mMark &= T_N_SHARE_BUFFER;
                mSrcNrls = src->getAttach(mNrlsIndex);
            }
        }
    }
    //------------------------------------------------------------------------
    void AniTempBuffer::alloc(bool pos, bool nrls)
    {
        mMark = pos ? (mMark | T_POS) : (mMark & T_N_POS);
        mMark = nrls ? (mMark | T_NRLS) : (mMark & T_N_NRLS);

        if(pos && mDstPos == 0)
        {
            mDstPos = static_cast<VertexBuffer *>(N_Only(GBuffer).allocTemp(mSrcPos, this));
        }
        if(nrls && !(mMark & T_SHARE_BUFFER) && mSrcNrls != 0 && mDstNrls == 0)
        {
            mDstNrls = static_cast<VertexBuffer *>(N_Only(GBuffer).allocTemp(mSrcNrls, this));
        }
    }
    //------------------------------------------------------------------------
    bool AniTempBuffer::refill(bool pos, bool nrls) const
    {
        if(pos || (nrls && (mMark & T_SHARE_BUFFER)))
        {
            if(mDstPos == 0)
                return false;

            mDstPos->getManager()->refillTemp(mDstPos);
        }

        if(nrls && !(mMark & T_SHARE_BUFFER))
        {
            if(mDstNrls == 0)
                return false;

            mDstNrls->getManager()->refillTemp(mDstNrls);
        }

        return true;
    }
    //------------------------------------------------------------------------
    void AniTempBuffer::bind(VertexData * dst, bool syn)
    {
        mDstPos->setSync(syn);
        dst->attach(mPosIndex, mDstPos);

        if((mMark & T_NRLS) && !(mMark & T_SHARE_BUFFER) && mDstNrls != 0)
        {
            mDstNrls->setSync(syn);
            dst->attach(mNrlsIndex, mDstNrls);
        }
    }
    //------------------------------------------------------------------------
    void AniTempBuffer::expire(Buffer * src)
    {
        N_assert(src == mDstPos || src == mDstNrls, "error temp buffer");

        if(src == mDstPos)
        {
            mDstPos = 0;
        }
        if(src == mDstNrls)
        {
            mDstNrls = 0;
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // GeometrySpaceFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    GeometrySpaceFactory::GeometrySpaceFactory()
    {
    }
    //-----------------------------------------------------------------------
    GeometrySpaceFactory::~GeometrySpaceFactory()
    {
    }
    //-----------------------------------------------------------------------
    FactoryID GeometrySpaceFactory::getID() const
    {
        return N_FACTORY_GeoSpace;
    }
    //-----------------------------------------------------------------------
    SpaceObj * GeometrySpaceFactory::create(SpaceID sid, SpaceManager * mag, const PropertyData * params)
    {
        // must have mesh parameter
        Mesh * re = 0;
        if (params != 0)
        {
            Nui32 gid, meshid, prefabtype;

            params->get(N_PropertyMesh_ID, meshid);
            params->get(N_PropertyMesh_PrefabType, prefabtype, GSP_None);
            params->get(N_PropertyResource_Group, gid, GroupUnknow);

            if (meshid != 0)
            {
                // Get mesh (load if required)
                re = static_cast<Mesh *>(N_Only(Mesh).load(meshid, gid, (GeometryPrefab)prefabtype));
            }
        }
        if (re == 0)
        {
            N_EXCEPT(InvalidParam, _I18n("没有'mesh'参数构造空间对象."));
        }

        SpaceObj * ret = N_new GeometrySpace(sid, re);
        ret->setFactory(this);
        ret->setManager(mag);
        return ret;
    }
    //-----------------------------------------------------------------------
    void GeometrySpaceFactory::destroy(void * obj)
    {
        SpaceObj * temp = static_cast<SpaceObj *>(obj);
        N_delete temp;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // GeometryCom
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    GeometryCom::GeometryCom(GeometrySpace * parent, SubMesh * obj) :
        GeometryObj(),
        mParent(parent),
        mMesh(obj),
        mMaterialID(MaterialManager::WhiteLight->getOriginID()),
        mLastCamera(0),
        mNromalsFusion(false),
        mAnimationMode(ANIM_None),
        mRenderGroup(RQG_Unknow),
        mRenderLevel(0)
    {
        mAniData = N_alloc_t(VertexData *, 3);
        mAniData[2] = 0;
        mAniData[0] = 0;
        mAniData[1] = 0;

        mMaterial = static_cast<Material *>(N_Only(Material).get(mMaterialID, obj->getMesh()->getGroup()));
        mMaterial->ref();

        mLodIndex = 0;

        mVisible = true;
        mVertexFusionCount = 0;

        mAniPrc = N_alloc_t(AniTempBuffer, 2);
    }
    //-----------------------------------------------------------------------
    GeometryCom::~GeometryCom()
    {
        if (mAniData[2])
            N_delete mAniData[2];
        if (mAniData[1])
            N_delete mAniData[1];
        if (mAniData[0])
            N_delete mAniData[0];

        N_free(mAniData);
        N_free(mAniPrc);

        mMaterial->unref();
    }
    //-----------------------------------------------------------------------
    void GeometryCom::setMaterial(ResourceID rid, GroupID gid)
    {
        Material * material = static_cast<Material *>(N_Only(Material).get(rid, gid));

        if (material == 0)
        {
            N_Only(Log).log(_I18n("不能设置材质: ") + N_conv(rid) +
                _I18n(" 到子空间对象: ") + N_conv(mParent->getID()) +
                _I18n(" because this Material does not exist. Have you forgotten to define it in a .material script?"));

            material = const_cast<Material *>(MaterialManager::WhiteLight);

            if (material == 0)
            {
                N_EXCEPT(Internal, _I18n("不能设置默认材质: ")
                    _I18n(" 到子空间对象: ") + N_conv(mParent->getID()) +
                    _I18n(". Did you forget to call MaterialManager::init()?"));
            }
        }
        setMaterial(material);
    }
    //--------------------------------------------------------------------------------
    void GeometryCom::setMaterial(const Material * material)
    {
        if (material != mMaterial)
        {
            if (mMaterial != 0)
            {
                mMaterial->unref();
            }

            if (material == 0)
            {
                N_Only(Log).log(_I18n("Can't assign material to GeometryCom of ") + N_conv(mParent->getID()) +
                    _I18n(" because this Material does not exist. Have you forgotten to define it in a .material script?"));

                mMaterial = const_cast<Material *>(MaterialManager::WhiteLight);
            }
            else
            {
                mMaterial = const_cast<Material *>(material);
            }
            mMaterial->ref();
            mMaterialID = mMaterial->getOriginID();
        }
        // 告诉父对象重新考虑材质顶点处理选项
        mParent->checkHWAnimation();
    }
    //-----------------------------------------------------------------------
    const Material * GeometryCom::getMaterial() const
    {
        return mMaterial;
    }
    //-----------------------------------------------------------------------
    ShaderFusion * GeometryCom::getShaderFusion() const
    {
        return mMaterial->getLOD(this, mLodIndex);
    }
    //-----------------------------------------------------------------------
    void GeometryCom::getGeometry(GeometryRaw & op) const
    {
        mMesh->getGeometry(op, mParent->mMeshLod);
        // Deal with any vertex data overrides
        op.mVertex = getRenderVertex();
    }
    //-----------------------------------------------------------------------
    AnimationMode GeometryCom::checkAnimation()
    {
        mParent->checkAnimation();
        return mAnimationMode;
    }
    //-----------------------------------------------------------------------
    VertexData * GeometryCom::getRenderVertex() const
    {
        if (!mMesh->mSelfVertex)
        {
            return mParent->getRenderVertex();
        }
        else
        {
            if (mParent->getSkeleton() != 0)
            {
                if (!mParent->isHWAnimation())
                {
                    // all software skeletal binds same vertex data
                    // may be a 2-stage s/w transform including morph earlier though
                    return mAniData[2];
                }
                else if (mAnimationMode != ANIM_None)
                {
                    // hardware morph animation
                    return mAniData[1];
                }
                else
                {
                    // hardware skeletal, no morphing
                    return mMesh->getVertexData();
                }
            }
            else if (mAnimationMode != ANIM_None)
            {
                // morph only, no skeletal
                if (mParent->isHWAnimation())
                {
                    return mAniData[1];
                }
                else
                {
                    return mAniData[0];
                }
            }
            else
            {
                return mMesh->getVertexData();
            }
        }
        return mMesh->getVertexData();
    }
    //-----------------------------------------------------------------------
    void GeometryCom::getMatrix(Matrix4f * xform, NCount & count) const
    {
        if (!mParent->getSkeleton()->getCount() || !mParent->isHWAnimation())
        {
            // No skeletal animation, or software skinning
            *xform = mParent->getSpaceMatrix();
            count = 1;
        }
        else
        {
            // Hardware skinning, pass all actually used matrices
            const BoneIndexList & indexMap = mMesh->mSelfVertex ?
                mMesh->getMatrixList() : mMesh->getMesh()->getMatrixList();
            N_assert(indexMap.size() <= mParent->getSkeleton()->getCount(), "error");

            if (mParent->isSkeletonValid())
            {
                // Bones, use cached matrices built when GeometrySpace::_update was called
                N_assert(mParent->mBoneSpace, "error");

                BoneIndexList::const_iterator it, itend = indexMap.end();
                for (it = indexMap.begin(); it != itend; ++it, ++xform)
                {
                    *xform = mParent->mBoneSpace[*it];
                }
            }
            else
            {
                std::fill_n(xform, indexMap.size(), mParent->getSpaceMatrix());
            }
            count = indexMap.size();
        }
    }
    //-----------------------------------------------------------------------
    NCount GeometryCom::getMatrixCount() const
    {
        if (!mParent->getSkeleton()->getCount() || !mParent->isHWAnimation())
        {
            // No skeletal animation, or software skinning
            return 1;
        }
        else
        {
            // Hardware skinning, pass all actually used matrices
            const BoneIndexList & indexMap = mMesh->mSelfVertex ?
                mMesh->getMatrixList() : mMesh->getMesh()->getMatrixList();
            N_assert(indexMap.size() <= mParent->getSkeleton()->getCount(), "error");

            return indexMap.size();
        }
    }
    //-----------------------------------------------------------------------
    NIIf GeometryCom::distanceSquared(const Camera * cam) const
    {
        // First of all, check the cached value
        // NB this is manually invalidated by parent each _notify call
        // Done this here rather than there since we only need this for transparent objects
        if (mLastCamera == cam)
            return mViewDist;

        PosNode * n = mParent->getSpaceNode();
        N_assert(n, "error");
        NIIf dist;
        if (!mMesh->getEdgePoints().empty())
        {
            const Vector3f & cp = cam->getSpacePos();
            const Matrix4f & l2w = mParent->getSpaceMatrix();
            dist = N_TYPE_INFINITY(NIIf);

            vector<Vector3f >::type::const_iterator i, endi = mMesh->getEdgePoints().end();
            for (i = mMesh->getEdgePoints().begin(); i != endi; ++i)
            {
                Vector3f v = l2w * (*i);
                NIIf d = (v - cp).lengthSquared();

                dist = std::min(d, dist);
            }
        }
        else
        {
            dist = n->distanceSquared(cam);
        }
        mViewDist = dist;
        mLastCamera = const_cast<Camera *>(cam);

        return dist;
    }
    //-----------------------------------------------------------------------
    void GeometryCom::getLight(LightList & ll) const
    {
        mParent->query(ll);
    }
    //-----------------------------------------------------------------------
    bool GeometryCom::isCastShadow() const
    {
        return mParent->isCastShadow();
    }
    //-----------------------------------------------------------------------
    void GeometryCom::setupAnimationBuffer()
    {
        if (!mMesh->mSelfVertex)
            return;

        if (mAniData[0])
        {
            N_delete mAniData[0];
            mAniData[0] = 0;
        }
        if (mAniData[1])
        {
            N_delete mAniData[1];
            mAniData[1] = 0;
        }
        if (mAniData[2])
        {
            N_delete mAniData[2];
            mAniData[2] = 0;
        }

        if (mMesh->mSelfVertex)
        {
            if (mAnimationMode != ANIM_None)
            {
                // Create temporary vertex blend info
                // Prepare temp vertex data if needed
                // Clone without copying data, don't remove any blending info
                // (since if we skeletally animate too, we need it)
                mAniData[0] = mMesh->getVertexData()->clone(false);
                mAniPrc[0].map(mAniData[0]);

                // Also clone for hardware usage, don't remove blend info since we'll
                // need it if we also hardware skeletally animate
                mAniData[1] = mMesh->getVertexData()->clone(false);
            }

            if (mParent->getSkeleton() != 0)
            {
                mAniData[2] = mMesh->getVertexData()->cloneUnit(true);
                mAniPrc[1].map(mAniData[2]);
            }
        }
    }
    //-----------------------------------------------------------------------------
    void GeometryCom::finishAnimationBlend(bool hw)
    {
        if (mMesh->mSelfVertex)
        {
            // Rebind original positions if:
            // We didn't apply any animation and
            // We're morph animated (hardware binds keyframe, software is missing)
            // or we're pose animated and software (hardware is fine, still bound)
            if (mAnimationMode != ANIM_None && !mAnimationEnable &&
                (!hw || mAnimationMode == ANIM_VertexUnit))
            {
                // Note, VT_Pos is specified here but if normals are included in animation
                // then these will be re-bound too (buffers must be shared)
                const VertexUnit * srcpos = mMesh->getVertexData()->get(VT_Pos);
                VertexBuffer * buf = mMesh->getVertexData()->getAttach(srcpos->mSource);
                const VertexUnit * destpos = mAniData[0]->get(VT_Pos);

                mAniData[0]->attach(destpos->mSource, buf);
            }

            // rebind any missing hardware pose buffers
            // Caused by not having any animations enabled, or keyframes which reference
            // no poses
            if (hw && mAnimationMode == ANIM_VertexOffset)
            {
                // For hardware pose animation, also make sure we've bound buffers to all the elements
                // required - if there are missing bindings for elements in use,
                // some rendersystems can complain because elements refer
                // to an unbound source.
                // Get the original position source, we'll use this to fill gaps
                const VertexUnit * srcpos = mMesh->getVertexData()->get(VT_Pos);
                VertexBuffer * srcBuf = mMesh->getVertexData()->getAttach(srcpos->mSource);

                HardwareAnimation::FusionList::const_iterator i, end = mAniData[1]->mHWAni->mFusionData.end();
                for (i = mAniData[1]->mHWAni->mFusionData.begin(); i != end; ++i)
                {
                    const HardwareAnimation::Fusion & blend = *i;
                    if (!mAniData[1]->isAttach(blend.mBufferIndex))
                    {
                        // 绑定到一个安全的默认
                        mAniData[1]->attach(blend.mBufferIndex, srcBuf);
                    }
                }
            }
        }
    }
    //-------------------------------------------------------------------------------
    void GeometryCom::setRenderGroup(GroupID rqgid, Nui16 level)
    {
        mRenderGroup = rqgid;
        mRenderLevel = level;
    }
    //-------------------------------------------------------------------------------
    void GeometryCom::updateCustom(GpuProgramParam * params, const GpuParamBlock & bind) const
    {
        if (bind.mSyncParam == GSP_Ch_AniParam)
        {
            /* 设置4个值,或限制的硬件动画项基于常量数据索引偏移,包装到4元素常量
                如果这里大于4个项,将被限制
            */
            Vector4f val(0.0f, 0.0f, 0.0f, 0.0f);

            NCount animIndex = bind.mInputInt * 4;
            NCount count = mAniData[1]->mHWAni->mFusionData.size();
            for (NCount i = 0; i < 4 && animIndex < count; ++i, ++animIndex)
            {
                val[i] = mAniData[1]->mHWAni->mFusionData[animIndex].mFactor;
            }
            // set the parametric morph value
            params->_write(bind.mMemIndex, 0, val);
        }
        else
        {
            // default
            return GeometryObj::updateCustom(params, bind);
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // GeometrySpaceShadowVolume
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    class _EngineAPI GeometrySpaceShadowVolume : public ShadowVolume
    {
    public:
        GeometrySpaceShadowVolume(GeometrySpace * parent, IndexBuffer * index, const VertexData * vertex,
            bool cLightCap, GeometryCom * sub, bool isLightCap = false) :
                mParent(parent),
                mSubEntity(sub),
                mVertexData(vertex)
        {
            mRenderOp.mIndex = N_new IndexData();
            N_Only(GBuffer).create(mRenderOp.mVertex);

            mRenderOp.mIndex->mBuffer = index;
            mRenderOp.mIndex->mOffset = 0;
            mRenderOp.mVertex->add(DT_Float3x, VT_Pos, 0, 0);

            mPBufferSrc = mVertexData->get(VT_Pos)->mSource;
            mPBuffer = mVertexData->getAttach(mPBufferSrc);

            mRenderOp.mVertex->attach(0, mPBuffer);
            // Map in w-coord buffer (if present)
            if(mVertexData->getExt() != 0)
            {
                mRenderOp.mVertex->add(DT_Float1x, VT_Tex_Coord, 1, 0, 0);
                mWBuffer = mVertexData->getExt();
                mRenderOp.mVertex->attach(1, mWBuffer);
            }
            // Use same vertex start as input
            mRenderOp.mVertex->mOffset = mVertexData->mOffset;

            if(isLightCap)
            {
                // Use original vertex count, no extrusion
                mRenderOp.mVertex->mCount = mVertexData->mCount;
            }
            else
            {
                // Vertex count must take into account the doubling of the buffer,
                // because second half of the buffer is the extruded copy
                mRenderOp.mVertex->mCount = mVertexData->mCount * 2;
                if(cLightCap)
                {
                    createLightCap();
                }
            }
        }

        ~GeometrySpaceShadowVolume()
        {
            N_delete mRenderOp.mIndex;
            N_delete mRenderOp.mVertex;
        }

        /** Create the separate light cap if it doesn't already exists
        @version NIIEngine 3.0.0
        */
        void createLightCap()
        {
            if(mLightCap == NULL)
            {
                // Create child light cap
                mLightCap = N_new GeometrySpaceShadowVolume(mParent, mRenderOp.mIndex->mBuffer,
                    mVertexData, false, mSubEntity, true);
            }
        }

        /** 重新绑定资源位置(临时缓存用户使用)
        @version NIIEngine 3.0.0 内部api
        */
        void rebindPosBuffer(const VertexData * vd, bool force)
        {
            if(force || mVertexData != vd)
            {
                mVertexData = vd;
                mPBuffer = mVertexData->getAttach(mPBufferSrc);
                mRenderOp.mVertex->attach(0, mPBuffer);

                if(mLightCap)
                    static_cast<GeometrySpaceShadowVolume *>(mLightCap)->rebindPosBuffer(vd, force);
            }
        }

        /** 获取位置缓存
        @version NIIEngine 3.0.0 内部api
        */
        inline VertexBuffer * getPositionBuffer()
        {
            return mPBuffer;
        }

        /** 获取权重缓存
        @version NIIEngien 3.0.0 内部api
        */
        inline VertexBuffer * getWBuffer()
        {
            return mWBuffer;
        }

        /// @copydetails GeometryObj::getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const
        {
            *out = mParent->getSpaceMatrix();
            count = 1;
        }

        /// @copydetails ShadowVolume::isVisible
        bool isVisible() const
        {
            if(mSubEntity)
            {
                return mSubEntity->isVisible();
            }
            else
            {
                return ShadowVolume::isVisible();
            }
        }
    protected:
        GeometrySpace * mParent;
        GeometryCom * mSubEntity;
        VertexBuffer * mPBuffer;        ///< 位置缓存的共享连接
        VertexBuffer * mWBuffer;        ///< w-coord 缓存共享连接(可选)
        Nui16 mPBufferSrc;                ///< 原始位置缓存源的绑定
        const VertexData * mVertexData; ///< 当前绑定顶点数据的连接(可能会变化)
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // GeometrySpace
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    GeometrySpace::GeometrySpace() :
        mMesh(0),
        mLinkList(NULL),
        mBoneSpace(NULL),
        mAnimationCtrl(NULL),
        mSkeletonFrame(NULL),
        mAnimationFrame(N_TYPE_MAX(Nmark)),
        mMeshLod(0),
        mMinMeshLod(99),
        mMaxMeshLod(0),
        mMeshLodFactor(1000),
        mMaterialLodBias(1.0f),
        mMaterialLodFactor(1000),
        mMinMaterialLod(99),
        mMaxMaterialLod(0),
        mAABB(),
        mSkeleton(0),
        mMeshResState(0),
        mVertexFusionCount(0),
        mLastSpace(Matrix4f::ZERO),
        mAnimationMode(ANIM_None),
        mInit(false),
        mBoneView(false),
        mVPEffect(false),
        mAutoSkeleton(true),
        mHWAnimation(false),
        mNromalsFusion(false),
        mLODAutoSkeleton(false),
        mVertexShadowVolume(false)
    {
        mAniData = N_alloc_t(VertexData *, 3);
        mAniData[0] = 0;
        mAniData[1] = 0;
        mAniData[2] = 0;

        mAniPrc = N_alloc_t(AniTempBuffer, 2);

        mLods.push_back(this);
    }
    //-----------------------------------------------------------------------
    GeometrySpace::GeometrySpace(SpaceID sid, const Mesh * mesh) :
        ShadowObj(sid),
        mMesh(const_cast<Mesh *>(mesh)),
        mLinkList(NULL),
        mBoneSpace(NULL),
        mAnimationCtrl(NULL),
        mSkeletonFrame(NULL),
        mAnimationFrame(N_TYPE_MAX(Nmark)),
        mMeshLod(0),
        mMinMeshLod(99),
        mMaxMeshLod(0),
        mMeshLodFactor(1000),
        mMaterialLodBias(1.0f),
        mMaterialLodFactor(1000),
        mMinMaterialLod(99),
        mMaxMaterialLod(0),
        mAABB(),
        mSkeleton(0),
        mMeshResState(0),
        mVertexFusionCount(0),
        mLastSpace(Matrix4f::ZERO),
        mAnimationMode(ANIM_None),
        mInit(false),
        mVPEffect(false),
        mBoneView(false),
        mAutoSkeleton(true),
        mHWAnimation(false),
        mNromalsFusion(false),
        mLODAutoSkeleton(false),
        mVertexShadowVolume(false)
    {
        mAniData = N_alloc_t(VertexData *, 3);
        mAniData[0] = 0;
        mAniData[1] = 0;
        mAniData[2] = 0;

        mAniPrc = N_alloc_t(AniTempBuffer, 2);

        mLods.push_back(this);

        _init();
    }
    //-----------------------------------------------------------------------
    GeometrySpace::~GeometrySpace()
    {
        _destroy();
        mMesh->remove(this);
        N_free(mAniData);
        N_free(mAniPrc);
    }
    //-----------------------------------------------------------------------
    TagBone * GeometrySpace::attach(Nidx boneidx, SpaceObj * obj, const Vector3f & oftpos,
        const Quaternion & oftori)
    {
        if(mChilds.find(obj->getID()) != mChilds.end())
        {
            N_EXCEPT(UniqueRepeat, _I18n("已经附加了相同ID的标签节点对象, id:") + obj->getID());
        }
        if(obj->isAttach())
        {
            N_EXCEPT(InvalidParam, _I18n("标签节点对象已经附加到节点上, id:") + obj->getID());
        }
        if(mSkeleton == 0)
        {
            N_EXCEPT(InvalidParam, _I18n("本对象不存在骨骼,不能附加标签节点对象") + getID());
        }

        Bone * bone = mSkeleton->get(boneidx);
        N_assert(bone != 0, "不存在骨头");
        TagBone * tp = mSkeleton->attach(bone, oftpos, oftori);
        tp->setOwner(this);
        tp->setAttach(obj);

        attachImpl(obj, tp);

        // Trigger update of bounding box if necessary
        if(mSpaceNode)
            mSpaceNode->notify();

        return tp;
    }
    //-----------------------------------------------------------------------
    SpaceObj * GeometrySpace::detach(SpaceID sid)
    {
        Childs::iterator i = mChilds.find(sid);
        if(i != mChilds.end())
        {
            SpaceObj * obj = i->second;
            detachImpl(obj);
            mChilds.erase(i);

            if(mSpaceNode)
                mSpaceNode->notify();

            return obj;
        }
        N_assert(0, "error");
        return 0;
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::detach(SpaceObj * obj)
    {
        Childs::iterator i, iend = mChilds.end();
        for(i = mChilds.begin(); i != iend; ++i)
        {
            if(i->second == obj)
            {
                detachImpl(obj);
                mChilds.erase(i);

                if(mSpaceNode)
                    mSpaceNode->notify();
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::detachAll()
    {
        detachAllImpl();

        if(mSpaceNode)
            mSpaceNode->notify();
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::onLoadEnd(Resource * res)
    {
        if(res == mMesh)
        {
            _init();
        }
    }
    //-----------------------------------------------------------------------
    bool GeometrySpace::_init(bool forceRe)
    {
        if(forceRe)
            _destroy();

        if(mInit)
            return true;

        if(mMesh->getLoadScheme() && mMesh->getState() != LS_LOADED)
        {
            // register for a callback when mesh is finished loading
            // do this before asking for load to happen to avoid race
            mMesh->add(this);
        }
        mMesh->load();

        // If loading failed, or deferred loading isn't done yet, defer
        // Will get a callback in the case of deferred loading
        // Skeletons are cascade-loaded so no issues there
        if(mMesh->getState() != LS_LOADED)
            return false;

        // Is mesh skeletally animated?
        if(mMesh->isSkeletonFusion() && mMesh->getSkeleton() != 0)
        {
            mSkeleton = N_new SkeletonFusion(mMesh->getSkeleton());
            mSkeleton->load();
            mSkeletonFrame = N_new_t(Nmark)(N_TYPE_MAX(Nmark));
        }

        // Build main subentity list
        buildComImpl(mMesh);


        Nidx i, lodcnt = mMesh->getLodCount();
        for(i = 1; i < lodcnt; ++i)
        {
            if(!mMesh->getLodScheme()->isRelation(i))
            {
                Mesh * mesh;
                mMesh->getLodScheme()->get(i, mesh);
                GeometrySpace * lodEnt = N_new GeometrySpace(0, mesh);
                mLods.push_back(lodEnt);
            }
        }

        if(mSkeleton != 0 || mMesh->isVertexFusion())
        {
            mAnimationCtrl = N_new AnimationFusionObj();
            mMesh->_init(mAnimationCtrl);
            mAnimationCheck = true;
            setupAnimationBuffer();
        }

        checkHWAnimation();

        if(mSpaceNode)
        {
            getSceneNode()->notify();
        }

        mMeshResState = mMesh->getNotifyCount();
        mInit = true;
        return mInit;
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::_destroy()
    {
        if(!mInit)
            return;

        Coms::iterator i, iend = mComs.end();
        for(i = mComs.begin(); i != iend; ++i)
        {
            N_delete *i;
            *i = 0;
        }
        mComs.clear();

        Lods::iterator li, liend = mLods.end();
        for(li = mLods.begin() + 1; li != liend; ++li)
        {
            N_delete *li;
        }
        mLods.clear();
        mLods.push_back(this);

        ShadowVolumeList::iterator si, siend = mShadowVolumeList.end();
        for(si = mShadowVolumeList.begin(); si != siend; ++si)
        {
            N_delete *si;
            *si = 0;
        }
        mShadowVolumeList.clear();

        // Detach all child objects, do this manually to avoid notify() call
        // which can fail because of deleted items
        detachAllImpl();

        if(mSkeleton)
        {
            N_free_simd(mBoneSpace);
            mBoneSpace = 0;

            if(mLinkList)
            {
                mLinkList->erase(this);
                if(mLinkList->size() == 1)
                {
                    (*mLinkList->begin())->unlinkSkeleton();
                }
                // Should never occuring, just in case
                else if(mLinkList->empty())
                {
                    N_delete_t(mLinkList, GeoList);
                    mLinkList = 0;
                    // using N_free since Nmark is not a destructor
                    N_free(mSkeletonFrame);
                    mSkeletonFrame = 0;
                    N_delete mSkeleton;
                    mSkeleton = 0;
                    N_delete mAnimationCtrl;
                    mAnimationCtrl = 0;
                }
            }
            else
            {
                // using N_free since Nmark is not a destructor
                N_free(mSkeletonFrame);
                mSkeletonFrame = 0;
                N_delete mSkeleton;
                mSkeleton = 0;
                N_delete mAnimationCtrl;
                mAnimationCtrl = 0;
            }
        }
        else if(mMesh->isVertexFusion())
        {
            N_delete mAnimationCtrl;
            mAnimationCtrl = 0;
        }

        if(mAniData[0])
        {
            N_delete mAniData[0];
            mAniData[0] = 0;
        }

        if(mAniData[1])
        {
            N_delete mAniData[1];
            mAniData[1] = 0;
        }

        if(mAniData[2])
        {
            N_delete mAniData[2];
            mAniData[2] = 0;
        }
        mInit = false;
    }
    //-----------------------------------------------------------------------
    GeometrySpace * GeometrySpace::clone(SpaceID newsid) const
    {
        if(!mManager)
        {
            N_EXCEPT(NotExist, _I18n("Cannot clone an GeometrySpace that wasn't created through a SpaceManager"));
        }

        GeometrySpace * re = mManager->createGeo(newsid, getMesh()->getOriginID());

        if(mInit)
        {
            Nidx n = 0;
            Coms::const_iterator i, iend = mComs.end();
            for(i = mComs.begin(); i != iend; ++i, ++n)
            {
                re->getCom(n)->setMaterial((*i)->getMaterialID());
            }
            if(mAnimationCtrl)
            {
                N_delete re->mAnimationCtrl;
                re->mAnimationCtrl = N_new AnimationFusionObj(*mAnimationCtrl);
            }
        }                
        return re;
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::setMaterial(ResourceID rid, GroupID gid)
    {
        // Set for all subentities
        Coms::iterator i, iend = mComs.end();
        for(i = mComs.begin(); i != iend; ++i)
        {
            (*i)->setMaterial(rid, gid);
        }
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::setMaterial(const Material * mat)
    {
        // Set for all subentities
        Coms::iterator i, iend = mComs.end();
        for(i = mComs.begin(); i != iend; ++i)
        {
            (*i)->setMaterial(mat);
        }
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::_notify(Camera * cam)
    {
        SpaceObj::_notify(cam);
        // 计算LOD
        if(mSpaceNode)
        {
            // Get mesh lod strategy
            const LodScheme * meshscheme = mMesh->getLodScheme();
            // Get the appropriate lod value
            NIIf lodValue = meshscheme->getValue(this, cam);
            // Bias the lod value
            Nui32 meshdest = lodValue * mMeshLodFactor;

            // Get the index at this biased depth
            Nui16 meshindex = meshscheme->get(meshdest);
            // Apply maximum detail restriction (remember lower = higher detail)
            meshindex = N_MAX(Nidx, mMaxMeshLod, meshindex);
            // Apply minimum detail restriction (remember higher = lower detail)
            meshindex = N_MIN(Nidx, mMinMeshLod, meshindex);

            // Construct event object
            MeshLodEvent evt;
            evt.mGeometrySpace = this;
            evt.mCamera = cam;
            evt.mValue = meshdest;
            evt.mPreIndex = mMeshLod;
            evt.mIndex = meshindex;

            // 告诉所有的lod事件监听器
            cam->getManager()->onLodEvent(evt);
            // 更新lod索引
            mMeshLod = evt.mIndex;
            // 现在处理纹理的LOD
            lodValue *= mMaterialLodFactor;

            Coms::iterator i, iend = mComs.end();
            for(i = mComs.begin(); i != iend; ++i)
            {
                // Get sub-entity material lod strategy
                const MaterialLodScheme * materialscheme = (*i)->mMaterial->getLodScheme();

                // Recalculate lod value if strategies do not match
                Nui32 materialdest = materialscheme->getValue(this, cam) *
                    materialscheme->getBias(mMaterialLodBias);

                // Get the index at this biased depth
                Nidx materialindex = materialscheme->get(materialdest);
                // Apply maximum detail restriction (remember lower = higher detail)
                materialindex = N_MAX(Nidx, mMaxMaterialLod, materialindex);
                // Apply minimum detail restriction (remember higher = lower detail)
                materialindex = N_MIN(Nidx, mMinMaterialLod, materialindex);

                // Construct event object
                MaterialLodEvent evt;
                evt.mGeometryCom = (*i);
                evt.mCamera = cam;
                evt.mValue = materialdest;
                evt.mPreIndex = (*i)->mLodIndex;
                evt.mIndex = materialindex;

                // 告诉所有的lod事件监听器
                cam->getManager()->onLodEvent(evt);
                // 更新lod索引
                (*i)->mLodIndex = evt.mIndex;
                // 使任何临时存储摄象机无效
                (*i)->mLastCamera = 0;;
            }
        }
        // Notify any child objects
        Childs::iterator j, jend = mChilds.end();
        for(j = mChilds.begin(); j != jend; ++j)
        {
            (*j).second->_notify(cam);
        }
    }
    //-----------------------------------------------------------------------
    const AABox & GeometrySpace::getAABB() const
    {
        if(mMesh->getState() == LS_LOADED)
        {
            mAABB = mMesh->getAABB();
            mAABB.merge(getComAABB());
            // Don't scale here, this is taken into account when world BBox calculation is done
        }
        else
            mAABB.setNull();

        return mAABB;
    }
    //-----------------------------------------------------------------------
    AABox GeometrySpace::getComAABB() const
    {
        AABox aab;
        AABox caab;
        caab.setNull();

        Childs::const_iterator i, iend = mChilds.end();
        for(i = mChilds.begin(); i != iend; ++i)
        {
            aab = i->second->getAABB();
            aab.affine(i->second->getSpaceNode()->getSpaceMatrix());

            caab.merge(aab);
        }
        return caab;
    }
    //-----------------------------------------------------------------------
    const AABox & GeometrySpace::getSpaceAABB(bool derive) const
    {
        if(derive)
        {
            // derive child bounding boxes
            Childs::const_iterator i, iend = mChilds.end();
            for(i = mChilds.begin(); i != iend; ++i)
            {
                i->second->getSpaceAABB(true);
            }
        }
        return SpaceObj::getSpaceAABB(derive);
    }
    //-----------------------------------------------------------------------
    const Sphere & GeometrySpace::getSpaceBS(bool derive) const
    {
        if(derive)
        {
            // derive child bounding boxes
            Childs::const_iterator i, iend = mChilds.end();
            for(i = mChilds.begin(); i != iend; ++i)
            {
                i->second->getSpaceBS(true);
            }
        }
        return SpaceObj::getSpaceBS(derive);
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::queue(RenderQueue * queue)
    {
        if(!mInit)
            return;

        // Check mesh state count, will be incremented if reloaded
        if(mMesh->getNotifyCount() != mMeshResState)
        {
            _init(true);
        }

        GeometrySpace * displayEntity = this;
        // Check we're not using a manual LOD
        if(mMeshLod > 0 && !mMesh->getLodScheme()->isRelation(mMeshLod))
        {
            // Use alternate entity
            N_assert(static_cast<NCount>(mMeshLod) < mLods.size(),
                "No LOD EntityList - did you build the manual LODs after creating the entity?");
            // index - 1 as we skip index 0 (original lod)
            if(mSkeleton != 0 && mLods[mMeshLod]->mSkeleton != 0)
            {
                mLods[mMeshLod]->mAnimationCtrl->copyState(mAnimationCtrl);
            }
            displayEntity = mLods[mMeshLod];
        }

        // Add each visible GeometryCom to the queue
        Coms::iterator i, iend = displayEntity->mComs.end();
        for(i = displayEntity->mComs.begin(); i != iend; ++i)
        {
            if((*i)->isVisible())
            {
                // Order: first use subentity queue settings, if available
                //        if not then use entity queue settings, if available
                //        finally fall back on default queue settings
                if((*i)->getRenderLevel() != 0)
                {
                    N_assert(((*i)->getRenderGroup() != RQG_Unknow) == true, "error");
                    queue->add(*i, (*i)->getRenderGroup(), (*i)->getRenderLevel());
                }
                else if((*i)->getRenderGroup() != RQG_Unknow)
                {
                    queue->add(*i, (*i)->getRenderGroup());
                }
                else if(mRenderLevel != 0)
                {
                    queue->add(*i, mRenderGroup, mRenderLevel);
                }
                else if(mRenderGroup != RQG_Unknow)
                {
                    queue->add(*i, mRenderGroup);
                }
                else
                {
                    queue->add(*i);
                }
            }
        }

        if(mLODAutoSkeleton && mSkeleton != 0 && (mMeshLod > 0))
        {
            //check if an update was made
            if(updateSkelImpl())
            {
                mSkeleton->_update(mAnimationCtrl);
                mSkeleton->_notifyManualUpdate(0);
            }
        }

        // Since we know we're going to be rendered, take this opportunity to
        // update the animation
        if(displayEntity->mSkeleton != 0 || displayEntity->mMesh->isVertexFusion())
        {
            displayEntity->updateAniImpl();

            //--- pass this point,  we are sure that the transformation matrix of each bone and tagPoint have been updated
            Childs::iterator c, cend = mChilds.end();
            for(c = mChilds.begin(); c != cend; c++)
            {
                SpaceObj * child = c->second;
                bool isVisible = child->isVisible();
                if(isVisible && (displayEntity != this))
                {
                    //Check if the bone exists in the current LOD

                    //The child is connected to a tagpoint which is connected to a bone
                    Bone * bone = static_cast<Bone *>(child->getSpaceNode()->getParent());
                    if(!displayEntity->mSkeleton->get(bone->getID()))
                    {
                        //当前LOD的实体没有这个子对象的骨头,所以不显示它
                        isVisible = false;
                    }
                }
                if(isVisible)
                {
                    child->queue(queue);
                }
            }
        }

        // HACK to display bones
        // This won't work if the entity is not centered at the origin
        // TODO work out a way to allow bones to be rendered when GeometrySpace not centered
        if(mBoneView && mSkeleton != 0)
        {
            NCount bcount = mSkeleton->getCount();
            for(Nidx b = 0; b < bcount; ++b)
            {
                Bone * bone = mSkeleton->get(b);
                if(mRenderLevel != 0)
                {
                    queue->add(bone->getGeometry(1), mRenderGroup, mRenderLevel);
                }
                else if(mRenderGroup != RQG_Unknow)
                {
                     queue->add(bone->getGeometry(1), mRenderGroup);
                }
                else
                {
                     queue->add(bone->getGeometry(1));
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    bool GeometrySpace::isFusionExist(AnimationID id) const
    {
        return mAnimationCtrl && mAnimationCtrl->isFusionExist(id);
    }
    //-----------------------------------------------------------------------
    bool GeometrySpace::touchVertAniCopy() const
    {
        // Do we still have temp buffers for software vertex animation bound?
        bool ret = true;
        if(mMesh->getVertexData() && mAnimationMode != ANIM_None)
        {
            ret = ret && mAniPrc[0].refill(true, mNromalsFusion);
        }
        Coms::const_iterator i, iend = mComs.end();
        for(i = mComs.begin(); i != iend; ++i)
        {
            GeometryCom * sub = *i;
            if(sub->mMesh->isSelfVertex() && sub->mAnimationMode != ANIM_None)
            {
                ret = ret && sub->mAniPrc[0].refill(true, sub->mNromalsFusion);
            }
        }
        return ret;
    }
    //-----------------------------------------------------------------------
    bool GeometrySpace::touchSkelAniCopy(bool normal) const
    {
        // Do we still have temp buffers for software skeleton animation bound?
        if(mAniData[2])
        {
            if(!mAniPrc[1].refill(true, normal))
                return false;
        }
        Coms::const_iterator i, iend = mComs.end();
        for(i = mComs.begin(); i != iend; ++i)
        {
            GeometryCom * sub = *i;
            if(sub->isVisible() && sub->mAniData[2])
            {
                if(!sub->mAniPrc[1].refill(true, normal))
                    return false;
            }
        }
        return true;
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::updateAniImpl()
    {
        if(!mInit)
            return;

        Engine & root = N_Engine();
        bool hwani = isHWAnimation();
        bool lhwani = hwani && !mHWAnimation;
        bool stencilShadows = false;
        if(isCastShadow() && isEdgeExist() && root.getProcessSpace())
            stencilShadows = root.getProcessSpace()->getRenderPattern()->isStencilShadow();
        bool swani = !hwani || stencilShadows;
        // Blend normals in s/w only if we're not using h/w animation,
        // since shadows only require positions
        bool blendNormals = !hwani;
        // Animation dirty if animation state modified or manual bones modified
        bool animationDirty = (mAnimationFrame != mAnimationCtrl->getFrameMark()) ||
            (mSkeleton != 0 && !mSkeleton->isValidBone());

        //update the current hardware animation state
        mHWAnimation = hwani;

        // We only do these tasks if animation is dirty
        // Or, if we're using a skeleton and manual bones have been moved
        // Or, if we're using software animation and temp buffers are unbound
        if(animationDirty || (swani && mMesh->isVertexFusion() && !touchVertAniCopy()) ||
            (swani && mSkeleton != 0 && !touchSkelAniCopy(blendNormals)))
        {
            if(mMesh->isVertexFusion())
            {
                if(swani)
                {
                    // grab & bind temporary buffer for positions (& normals if they are included)
                    if(mAniData[0] && mAnimationMode != ANIM_None)
                    {
                        mAniPrc[0].alloc(true, mNromalsFusion);
                        // NB we suppress hardware upload while doing blend if we're
                        // hardware animation, because the only reason for doing this
                        // is for shadow, which need only be uploaded then
                        mAniPrc[0].bind(mAniData[0], !hwani);
                    }
                    Coms::iterator i, iend = mComs.end();
                    for(i = mComs.begin(); i != iend; ++i)
                    {
                        // Blend dedicated geometry
                        GeometryCom * se = *i;
                        if(se->isVisible() && se->mAniData[0] && se->mAnimationMode != ANIM_None)
                        {
                            se->mAniPrc[0].alloc(true, se->mNromalsFusion);
                            se->mAniPrc[0].bind(se->mAniData[0], !hwani);
                        }
                    }
                }
                updateVertImpl(hwani, swani);
            }

            if(mSkeleton != 0)
            {
                updateSkelImpl();

                // Software blend?
                if(swani)
                {
                    const Matrix4f * src = mSkeleton->getMatrix();
                    const Matrix4f * tm[256];

                    // Ok, we need to do a software blend
                    // Firstly, check out working vertex buffers
                    if(mAniData[2])
                    {
                        // Blend shared geometry
                        // NB we suppress hardware upload while doing blend if we're
                        // hardware animation, because the only reason for doing this
                        // is for shadow, which need only be uploaded then
                        mAniPrc[1].alloc(true, blendNormals);
                        mAniPrc[1].bind(mAniData[2], !hwani);

                        N_assert(mMesh->getMatrixList().size() <= 256, "error");
                        NCount k = 0;
                        BoneIndexList::const_iterator it, itend = mMesh->getMatrixList().end();
                        for(it = mMesh->getMatrixList().begin(); it != itend; ++it, ++k)
                        {
                            tm[k] = src + *it;
                        }
                        // Blend, taking source from either mesh data or morph data
                        NodeKeyFrameTrack::fusion((mAnimationMode != ANIM_None) ?
                            mAniData[0] : mMesh->getVertexData(), mAniData[2], tm,
                                mMesh->getMatrixList().size(), blendNormals);
                    }
                    Coms::iterator i, iend = mComs.end();
                    for(i = mComs.begin(); i != iend; ++i)
                    {
                        // Blend dedicated geometry
                        GeometryCom * se = *i;
                        if(se->isVisible() && se->mAniData[2])
                        {
                            se->mAniPrc[1].alloc(true, blendNormals);
                            se->mAniPrc[1].bind(se->mAniData[2], !hwani);
                            N_assert(se->mMesh->getMatrixList().size() <= 256, "error");
                            NCount k = 0;
                            BoneIndexList::const_iterator it, itend = se->mMesh->getMatrixList().end();
                            for(it = se->mMesh->getMatrixList().begin(); it != itend; ++it)
                            {
                                tm[k] = src + *it;
                            }
                            // Blend, taking source from either mesh data or morph data
                            NodeKeyFrameTrack::fusion((se->mAnimationMode != ANIM_None)?
                                se->mAniData[0] : se->mMesh->getVertexData(), se->mAniData[2], tm,
                                    se->mMesh->getMatrixList().size(), blendNormals);
                        }
                    }
                }
            }

            // Trigger update of bounding box if necessary
            if(!mChilds.empty())
                mSpaceNode->notify();

            mAnimationFrame = mAnimationCtrl->getFrameMark();
        }

        // Need to update the child object's transforms when animation dirty
        // or parent node transform has altered.
        if(mSkeleton != 0 && (lhwani || animationDirty || mLastSpace != getSpaceMatrix()))
        {
            // Cache last parent transform for next frame use too.
            mLastSpace = getSpaceMatrix();

            //--- Update the child object's transforms
            Childs::iterator c, cend = mChilds.end();
            for(c = mChilds.begin(); c != cend; ++c)
            {
                (*c).second->getSpaceNode()->update(true, true);
            }

            //
            if(hwani && isSkeletonValid())
            {
                NCount tb = mSkeleton->getCount();
                // Allocate bone world matrices on demand, for better memory footprint
                // when using software animation.
                if(!mBoneSpace)
                {
                    mBoneSpace = static_cast<Matrix4f *>(N_alloc_simd(sizeof(Matrix4f) * tb));
                }

                SIMDUtil::getImplementation()->affine(
                    mBoneSpace, mLastSpace, mSkeleton->getMatrix(), tb);
            }
        }
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::updateVertImpl(bool hw, bool sw)
    {
        // make sure we have enough hardware animation elements to play with
        if(hw)
        {
            if(mAniData[1] && mAnimationMode != ANIM_None)
            {
                Nui16 pcount = mAniData[1]->allocHWAnimate((mAnimationMode == ANIM_VertexOffset) ?
                    mVertexFusionCount : 1, mNromalsFusion);

                if(mAnimationMode == ANIM_VertexOffset && pcount < mVertexFusionCount)
                {
                    mVertexFusionCount = pcount;
                }
            }
            Coms::iterator si, siend = mComs.end();
            for(si = mComs.begin(); si != siend; ++si)
            {
                GeometryCom * sub = *si;
                if(sub->mAnimationMode != ANIM_None && sub->getMesh()->isSelfVertex())
                {
                    Nui16 pcount = sub->mAniData[1]->allocHWAnimate((sub->mAnimationMode == ANIM_VertexOffset) ?
                        sub->mVertexFusionCount : 1, sub->mNromalsFusion);

                    if(sub->mAnimationMode == ANIM_VertexOffset && pcount < sub->mVertexFusionCount)
                    {
                        sub->mVertexFusionCount = pcount;
                    }
                }
            }
        }
        else
        {
            // 可能软件混合多个姿态,禁止硬件上传缓存
            // Note, we query position buffer here but it may also include normals
            if(mAniData[0] && mAnimationMode == ANIM_VertexOffset)
            {
                const VertexUnit * elem = mAniData[0]->get(VT_Pos);
                VertexBuffer * buf = mAniData[0]->getAttach(elem->mSource);
                buf->setSync(false);
                VertexOffsetKeyFrameTrack::memcpy(mMesh->getVertexData(), mAniData[0], mNromalsFusion);
            }
            Coms::iterator si, siend = mComs.end();
            for(si = mComs.begin(); si != siend; ++si)
            {
                GeometryCom * sub = *si;
                if(sub->getMesh()->isSelfVertex() && sub->mAnimationMode == ANIM_VertexOffset)
                {
                    VertexData * data = sub->mAniData[0];
                    const VertexUnit * elem = data->get(VT_Pos);
                    VertexBuffer * buf = data->getAttach(elem->mSource);
                    buf->setSync(false);
                    // if we're animating normals, we need to start with zeros
                    VertexOffsetKeyFrameTrack::memcpy(sub->getMesh()->getVertexData(), data, sub->mNromalsFusion);
                }
            }
        }

        // Now apply the animation(s)
        // Note - you should only apply one morph animation to each set of vertex data
        // at once; if you do more, only the last one will actually apply
        setAnimation(false);
        AnimationFusionObj::FusionList::const_iterator i, iend = mAnimationCtrl->getValidFusions().end();
        for(i = mAnimationCtrl->getValidFusions().begin(); i != iend; ++i)
        {
            VertexFusion * fusion = dynamic_cast<VertexFusion *>(*i);
            if(fusion)
            {
                fusion->apply(this, sw, hw);
            }
        }
        // 处理没有应用动画的情况
        finishAnimationBlend(hw);

        // 软件更新动画一般是使用后台缓存操作,通过更新后台缓存,再更新期间
        // 是和实际GPU脱轨,直到更新完了,再把后台缓存上载到实际缓存中,达到
        // 更新目的
        if(!hw)
        {
            if(mAniData[0] && mAnimationMode == ANIM_VertexOffset)
            {
                // if we're animating normals, if pose mWeight < 1 need to use the base mesh
                if(mNromalsFusion)
                {
                    const VertexData * src = mMesh->getVertexData();
                    VertexData * dest = mAniData[0];
                    const VertexUnit * destNormElem = dest->get(VT_Normals);
                    const VertexUnit * srcNormElem = src->get(VT_Normals);

                    if(destNormElem && srcNormElem)
                    {
                        VertexBuffer * srcbuf = src->getAttach(srcNormElem->mSource);
                        VertexBuffer * dstbuf = dest->getAttach(destNormElem->mSource);
                        Nui8 * pSrcBase = (Nui8 *)srcbuf->lock(Buffer::MM_READ);
                        Nui8 * pDstBase = (Nui8 *)dstbuf->lock(Buffer::MM_READ | Buffer::MM_WRITE);
                        pSrcBase += src->mOffset * srcbuf->getUnitSize();
                        pDstBase += dest->mOffset * dstbuf->getUnitSize();

                        // The goal here is to detect the length of the vertices, and to apply
                        // the base mesh vertex normal at one minus that length; this deals with
                        // any individual vertices which were either not affected by any pose, or
                        // were not affected to a complete extent
                        // We also normalise every normal to deal with over-weighting
                        for(NCount v = 0; v < dest->mCount; ++v)
                        {
                            NIIf * pDstNorm;
                            destNormElem->getData(pDstBase, pDstNorm);
                            Vector3f norm(pDstNorm[0], pDstNorm[1], pDstNorm[2]);
                            NIIf len = norm.length();
                            if(len + 1e-4f < 1.0f)
                            {
                                // Poses did not completely fill in this normal
                                // Apply base mesh
                                NIIf baseWeight = 1.0f - (NIIf)len;
                                NIIf * pSrcNorm;
                                srcNormElem->getData(pSrcBase, pSrcNorm);
                                norm.x += *pSrcNorm++ * baseWeight;
                                norm.y += *pSrcNorm++ * baseWeight;
                                norm.z += *pSrcNorm++ * baseWeight;
                            }
                            norm.normalise();

                            *pDstNorm++ = norm.x;
                            *pDstNorm++ = norm.y;
                            *pDstNorm++ = norm.z;

                            pDstBase += dstbuf->getUnitSize();
                            pSrcBase += dstbuf->getUnitSize();
                        }
                        srcbuf->unlock();
                        dstbuf->unlock();
                    }
                }
                const VertexUnit * elem = mAniData[0]->get(VT_Pos);
                VertexBuffer * buf = mAniData[0]->getAttach(elem->mSource);
                buf->setSync(true);
            }
            Coms::iterator si, siend = mComs.end();
            for(si = mComs.begin(); si != siend; ++si)
            {
                GeometryCom * sub = *si;
                if(sub->getMesh()->isSelfVertex() && sub->mAnimationMode == ANIM_VertexOffset)
                {
                    VertexData * dest = sub->mAniData[0];
                    // if we're animating normals, if pose mWeight < 1 need to use the base mesh
                    if(sub->mNromalsFusion)
                    {
                        const VertexData * src = sub->getMesh()->getVertexData();

                        const VertexUnit * destNormElem = dest->get(VT_Normals);
                        const VertexUnit * srcNormElem = src->get(VT_Normals);

                        if(destNormElem && srcNormElem)
                        {
                            VertexBuffer * srcbuf = src->getAttach(srcNormElem->mSource);
                            VertexBuffer * dstbuf = dest->getAttach(destNormElem->mSource);
                            Nui8 * pSrcBase = (Nui8 *)srcbuf->lock(Buffer::MM_READ);
                            Nui8 * pDstBase = (Nui8 *)dstbuf->lock(Buffer::MM_READ | Buffer::MM_WRITE);
                            pSrcBase += src->mOffset * srcbuf->getUnitSize();
                            pDstBase += dest->mOffset * dstbuf->getUnitSize();

                            // The goal here is to detect the length of the vertices, and to apply
                            // the base mesh vertex normal at one minus that length; this deals with
                            // any individual vertices which were either not affected by any pose, or
                            // were not affected to a complete extent
                            // We also normalise every normal to deal with over-weighting
                            for(NCount v = 0; v < dest->mCount; ++v)
                            {
                                NIIf * pDstNorm;
                                destNormElem->getData(pDstBase, pDstNorm);
                                Vector3f norm(pDstNorm[0], pDstNorm[1], pDstNorm[2]);
                                NIIf len = norm.length();
                                if(len + 1e-4f < 1.0f)
                                {
                                    // Poses did not completely fill in this normal
                                    // Apply base mesh
                                    NIIf baseWeight = 1.0f - (NIIf)len;
                                    NIIf * pSrcNorm;
                                    srcNormElem->getData(pSrcBase, pSrcNorm);
                                    norm.x += *pSrcNorm++ * baseWeight;
                                    norm.y += *pSrcNorm++ * baseWeight;
                                    norm.z += *pSrcNorm++ * baseWeight;
                                }
                                norm.normalise();

                                *pDstNorm++ = norm.x;
                                *pDstNorm++ = norm.y;
                                *pDstNorm++ = norm.z;

                                pDstBase += dstbuf->getUnitSize();
                                pSrcBase += dstbuf->getUnitSize();
                            }
                            srcbuf->unlock();
                            dstbuf->unlock();
                        }
                    }
                    const VertexUnit * elem = dest->get(VT_Pos);
                    VertexBuffer * buf = dest->getAttach(elem->mSource);
                    buf->setSync(true);
                }
            }
        }
    }
    //-----------------------------------------------------------------------------
    void GeometrySpace::setAnimation(bool b)
    {
        mAnimationEnable = b;
        Coms::iterator i, iend = mComs.end();
        for(i = mComs.begin(); i != iend; ++i)
        {
            (*i)->setAnimation(b);
        }
    }
    //-----------------------------------------------------------------------------
    void GeometrySpace::finishAnimationBlend(bool hw)
    {
        if(mMesh->getVertexData())
        {
            // Rebind original positions if:
            //  We didn't apply any animation and
            //    We're morph animated (hardware binds keyframe, software is missing)
            //    or we're pose animated and software (hardware is fine, still bound)
            if(!mAnimationEnable && (!hw || mAnimationMode == ANIM_VertexUnit))
            {
                // Note, VT_Pos is specified here but if normals are included in animation
                // then these will be re-bound too (buffers must be shared)
                const VertexUnit * pos = mMesh->getVertexData()->get(VT_Pos);
                VertexBuffer * buf = mMesh->getVertexData()->getAttach(pos->mSource);
                pos = mAniData[0]->get(VT_Pos);

                mAniData[0]->attach(pos->mSource, buf);
            }

            // rebind any missing hardware pose buffers
            // Caused by not having any animations enabled, or keyframes which reference
            // no poses
            if(hw && mAnimationMode == ANIM_VertexOffset)
            {
                // For hardware pose animation, also make sure we've bound buffers to all the elements
                // required - if there are missing bindings for elements in use,
                // some rendersystems can complain because elements refer
                // to an unbound source.
                // Get the original position source, we'll use this to fill gaps
                const VertexUnit * srcPosElem = mMesh->getVertexData()->get(VT_Pos);
                VertexBuffer * srcBuf = mMesh->getVertexData()->getAttach(srcPosElem->mSource);

                HardwareAnimation::FusionList::const_iterator i, end = mAniData[1]->mHWAni->mFusionData.end();
                for(i = mAniData[1]->mHWAni->mFusionData.begin(); i != end ; ++i)
                {
                    const HardwareAnimation::Fusion & animData = *i;
                    if(!mAniData[1]->isAttach(animData.mBufferIndex))
                    {
                        // 绑定到一个安全的默认
                        mAniData[1]->attach(animData.mBufferIndex, srcBuf);
                    }
                }
            }
        }
        Coms::iterator i, iend = mComs.end();
        for(i = mComs.begin();i != iend; ++i)
        {
            (*i)->finishAnimationBlend(hw);
        }
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::_updateAnimation()
    {
        checkAnimation();
        if(mSkeleton != 0 || mMesh->isVertexFusion())
        {
            updateAniImpl();
        }
    }
    //-----------------------------------------------------------------------
    bool GeometrySpace::isAnimationValid() const
    {
        return (mAnimationCtrl && mAnimationCtrl->isValid()) || (mSkeleton && mSkeleton->isCustomBoneExist());
    }
    //-----------------------------------------------------------------------
    bool GeometrySpace::isSkeletonValid() const
    {
        return mSkeleton && (mAnimationCtrl->isValid() || mSkeleton->isCustomBoneExist());
    }
    //-----------------------------------------------------------------------
    AnimationMode GeometrySpace::checkAnimation()
    {
        if(mAnimationCheck)
        {
            // Don't check flag here; since detail checks on track changes are not
            // done, allow caller to force if they need to

            // Initialise all types to nothing
            mAnimationMode = ANIM_None;
            mNromalsFusion = false;
            bool posenrl = false;
            Coms::iterator i, iend = mComs.end();
            for(i = mComs.begin(); i != iend; ++i)
            {
                (*i)->mAnimationMode = ANIM_None;
                (*i)->mNromalsFusion = false;
            }

            VertexOffsetList::const_iterator j, jend = mMesh->mVertexOffsetList.end();
            for(j = mMesh->mVertexOffsetList.begin(); j != jend; ++j)
            {
                if(j == mMesh->mVertexOffsetList.begin())
                    posenrl = (*j)->isNormalsValid();
                else if(posenrl != (*j)->isNormalsValid())
                    // only support normals if consistently included
                    posenrl = posenrl && (*j)->isNormalsValid();
            }

            // Scan all animations and determine the type of animation tracks
            // relating to each vertex data
            AnimationObj::AnimationList::const_iterator ai, aiend = mMesh->mAnimations.end();
            for(ai = mMesh->mAnimations.begin(); ai != aiend; ++ai)
            {
                Animation * anim = ai->second;
                Animation::TrackList::const_iterator i, iend = anim->getTrack().end();
                for(i = anim->getTrack().begin(); i != iend; ++i)
                {
                    VertexKeyFrameTrack * track = static_cast<VertexKeyFrameTrack *>(i->second);
                    KeyFrameTrackID tid = track->getID();
                    AnimationMode animType;
                    if (typeid(*track) == typeid(VertexUnitKeyFrameTrack))
                        animType = ANIM_VertexUnit;
                    else
                        animType = ANIM_VertexOffset;

                    if(tid == 0)
                    {
                        // shared data
                        if(mAnimationMode != ANIM_None && mAnimationMode != animType)
                        {
                            N_EXCEPT(InvalidParam, _I18n("同个网格只能有一种类型的混合,几何空间") + getID());
                        }
                        mAnimationMode = animType;
                        if(animType == ANIM_VertexUnit)
                            mNromalsFusion = track->isNromalsFusion();
                        else
                            mNromalsFusion = posenrl;
                    }
                    else
                    {
                        // 子网格索引(-1)
                        GeometryCom * sm = mComs[tid - 1];
                        if(sm->mAnimationMode != ANIM_None && sm->mAnimationMode != animType)
                        {
                            // Mixing of morph and pose animation on same data is not allowed
                            N_EXCEPT(InvalidParam, _I18n("同个子网格只能有一种类型的混合 ")
                                + N_conv(tid - 1) + _I18n(" 几何空间") + N_conv(getID()));
                        }
                        sm->mAnimationMode = animType;
                        if(animType == ANIM_VertexUnit)
                            sm->mNromalsFusion = track->isNromalsFusion();
                        else
                            sm->mNromalsFusion = posenrl;
                    }
                }
            }
            mAnimationCheck = false;
        }
        return mAnimationMode;
    }
    //-----------------------------------------------------------------------
    bool GeometrySpace::updateSkelImpl()
    {
        Nmark cfm = N_Engine().getCurrentFrame();
        if((*mSkeletonFrame != cfm) || (mSkeleton != 0 && !mSkeleton->isValidBone()))
        {
            if(mAutoSkeleton && (*mSkeletonFrame != cfm))
                mSkeleton->_refresh(mAnimationCtrl);
            mSkeleton->update();
            *mSkeletonFrame  = cfm;

            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::setMeshLodBias(NIIf factor, Nui16 max, Nui16 min)
    {
        MeshLodScheme * scheme = mMesh->getLodScheme();
        mMeshLodFactor = scheme->getDest(factor);
        mMaxMeshLod = max;
        mMinMeshLod = min;
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::setMaterialLodBias(NIIf factor, Nui16 max, Nui16 min)
    {
        MeshLodScheme * scheme = mMesh->getLodScheme();
        mMaterialLodBias = factor;
        mMaterialLodFactor = scheme->getDest(factor);
        mMaxMaterialLod = max;
        mMinMaterialLod = min;
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::buildComImpl(const Mesh * mesh)
    {
        SubMesh * submesh;
        GeometryCom * com;

        NCount i, count = mesh->getSubCount();
        for(i = 0; i < count; ++i)
        {
            submesh = mesh->getSub(i);
            com = N_new GeometryCom(this, submesh);
            if(submesh->getMaterial() != 0)
            {
                com->setMaterial(submesh->getMaterial(), mesh->getGroup());
            }
            mComs.push_back(com);
        }
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::attachImpl(SpaceObj * obj, TagBone * node)
    {
        N_assert(mChilds.find(obj->getID()) == mChilds.end(), "不应该存在附加物");
        mChilds[obj->getID()] = obj;
        obj->attach(node, true);
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::detachImpl(SpaceObj * obj)
    {
        TagBone * tp = static_cast<TagBone *>(obj->getSpaceNode());
        mSkeleton->detach(tp);
        obj->attach((TagBone *)0);
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::detachAllImpl()
    {
        Childs::const_iterator i, iend = mChilds.end();
        for(i = mChilds.begin(); i != iend; ++i)
        {
            detachImpl(i->second);
        }
        mChilds.clear();
    }
    //-----------------------------------------------------------------------
    NIIf GeometrySpace::getCenterRange() const
    {
        return mMesh->getCenterRange();
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::setupAnimationBuffer()
    {
        if(mAniData[2])
        {
            N_delete mAniData[2];
            mAniData[2] = 0;
        }
        if(mAniData[0])
        {
            N_delete mAniData[0];
            mAniData[0] = 0;
        }
        if(mAniData[1])
        {
            N_delete mAniData[1];
            mAniData[1] = 0;
        }

        if(mMesh->isVertexFusion())
        {
            // Shared data
            if(mMesh->getVertexData() && mAnimationMode != ANIM_None)
            {
                // Create temporary vertex blend info
                // Prepare temp vertex data if needed
                // Clone without copying data, don't remove any blending info
                // (since if we skeletally animate too, we need it)
                mAniData[0] = mMesh->getVertexData()->clone(false);
                mAniPrc[0].map(mAniData[0]);

                // Also clone for hardware usage, don't remove blend info since we'll
                // need it if we also hardware skeletally animate
                mAniData[1] = mMesh->getVertexData()->clone(false);
            }
        }

        if(mSkeleton != 0)
        {
            // Shared data
            if(mMesh->getVertexData())
            {
                // Create temporary vertex blend info
                // Prepare temp vertex data if needed
                // Clone without copying data, remove blending info
                // (since blend is performed in software)
                mAniData[2] = mMesh->getVertexData()->cloneUnit(true);
                mAniPrc[1].map(mAniData[2]);
            }
        }

        // Do SubEntities
        Coms::iterator i, iend = mComs.end();
        for(i = mComs.begin(); i != iend; ++i)
        {
            GeometryCom * s = *i;
            s->setupAnimationBuffer();
        }

        // It's prepared for shadow volumes only if mesh has been prepared for shadow volumes.
        mVertexShadowVolume = mMesh->isExist(Mesh::M_ShadowVol);
    }
    //-----------------------------------------------------------------------
    bool GeometrySpace::isEdgeExist()
    {
        return (mMesh->getEdgeList(mMeshLod) != NULL);
    }
    //-----------------------------------------------------------------------
    GeoEdget * GeometrySpace::getEdgeList()
    {
        return mMesh->getEdgeList(mMeshLod);
    }
    //-----------------------------------------------------------------------
    const AABox & GeometrySpace::getFrontCapAABB() const
    {
        return getSpaceAABB();
    }
    //-----------------------------------------------------------------------
    bool GeometrySpace::isHWAnimation() const
    {
        //find whether the entity has hardware animation for the current active sceme
        SchemeID sid = N_Only(Material).getCurrent();
        HWAnimationList::iterator it = mHWAnimationList.find(sid);
        if(it == mHWAnimationList.end())
        {
            //evaluate the animation hardware value
            it = mHWAnimationList.insert(Npair(sid, 
                const_cast<GeometrySpace *>(this)->checkHWAniImpl())).first;
        }
        return it->second;
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::checkHWAnimation(void)
    {
        //clear the cache so that the values will be reevaluated
        mHWAnimationList.clear();
    }
    //-----------------------------------------------------------------------
    bool GeometrySpace::checkHWAniImpl()
    {
        bool hasHDAnim = false;
        bool firstPass = true;

        Coms::iterator i, iend = mComs.end();
        for(i = mComs.begin(); i != iend; ++i)
        {
            GeometryCom * sub = *i;

            Material * m = const_cast<Material *>(sub->getMaterial());
            m->load();
            ShaderFusion * t = m->getLOD(sub, 0);
            if(t == 0 || t->getCount() == 0)
                continue;

            ShaderCh * p = t->get(0);
            if(p->getProgram().isExist(ShaderChProgram::ST_VS))
            {
                if(mVPEffect == false)
                {
                    // If one material uses a vertex program, set this flag
                    // Causes some special processing like forcing a separate light cap
                    mVPEffect = true;

                    // If shadow renderables already created create their light caps
                    ShadowVolumeList::iterator si, siend = mShadowVolumeList.end();
                    for(si = mShadowVolumeList.begin(); si != siend; ++si)
                    {
                        static_cast<GeometrySpaceShadowVolume *>(*si)->createLightCap();
                    }
                }

                if(mSkeleton != 0)
                {
                    GpuProgram * prog;
                    p->getProgram().get(ShaderChProgram::ST_VS, prog);
                    // All materials must support skinning for us to consider using
                    // hardware animation - if one fails we use software
                    if (firstPass)
                    {
                        hasHDAnim = prog->isVertexMatrixFusion();
                        firstPass = false;
                    }
                    else
                    {
                        hasHDAnim = hasHDAnim && prog->isVertexMatrixFusion();
                    }
                }

                AnimationMode animType = ANIM_None;
                if(sub->getMesh()->isSelfVertex())
                {
                    animType = sub->mAnimationMode;
                }
                else
                {
                    animType = mAnimationMode;
                }

                if(animType == ANIM_VertexUnit)
                {
                    GpuProgram * prog;
                    p->getProgram().get(ShaderChProgram::ST_VS, prog);
                    // All materials must support morph animation for us to consider using
                    // hardware animation - if one fails we use software
                    if(firstPass)
                    {
                        hasHDAnim = prog->isVertexInterpolation();
                        firstPass = false;
                    }
                    else
                    {
                        hasHDAnim = hasHDAnim && prog->isVertexInterpolation();
                    }
                }
                else if(animType == ANIM_VertexOffset)
                {
                    GpuProgram * prog;
                    p->getProgram().get(ShaderChProgram::ST_VS, prog);
                    // All materials must support pose animation for us to consider using
                    // hardware animation - if one fails we use software
                    if(firstPass)
                    {
                        hasHDAnim = prog->getVertexOffsetCount() > 0;
                        if(sub->getMesh()->isSelfVertex())
                        {
                            sub->mVertexFusionCount = prog->getVertexOffsetCount();
                        }
                        else
                        {
                            mVertexFusionCount = prog->getVertexOffsetCount();
                        }
                        firstPass = false;
                    }
                    else
                    {
                        hasHDAnim = hasHDAnim && (prog->getVertexOffsetCount() > 0);
                        if(sub->getMesh()->isSelfVertex())
                        {
                            sub->mVertexFusionCount = N_MAX(NCount, sub->mVertexFusionCount,
                                prog->getVertexOffsetCount());
                        }
                        else
                        {
                            mVertexFusionCount = N_MAX(NCount, mVertexFusionCount,
                                prog->getVertexOffsetCount());
                        }
                    }
                }
            }
        }

        if(mAnimationCtrl)
        {
            mAnimationFrame = mAnimationCtrl->getFrameMark() - 1;
        }
        return hasHDAnim;
    }
    //-----------------------------------------------------------------------
    const ShadowVolumeList & GeometrySpace::getVolumeList(const Light * srclight,
         NIIf destdis, Nmark flags, IndexBuffer * destindex)
    {
        N_assert(destindex, "Only external index buffers are supported right now");
        N_assert(destindex->getUnitSize() == 16, "Only 16-bit indexes supported for now");

        if(!mMesh->getLodScheme()->isRelation(mMeshLod) && mMeshLod > 0)
        {
            N_assert(static_cast<NCount>(mMeshLod) < mLods.size(),
                "No LOD EntityList - did you build the manual LODs after creating the entity?");

            if(mSkeleton != 0 && mLods[mMeshLod]->mSkeleton != 0)
            {
                mLods[mMeshLod]->mAnimationCtrl->copyState(mAnimationCtrl);
            }
            return mLods[mMeshLod]->getVolumeList(srclight, destdis, flags, destindex);
        }

        if(!mVertexShadowVolume)
        {
            mMesh->build(Mesh::M_ShadowVol);
            // reset frame last updated to force update of animations if they exist
            if(mAnimationCtrl)
                mAnimationFrame = mAnimationCtrl->getFrameMark() - 1;
            // re-prepare buffers
            setupAnimationBuffer();
        }

        bool hasAnimation = (mSkeleton != 0 || mMesh->isVertexFusion());

        // Update any animation
        if(hasAnimation)
        {
            updateAniImpl();
        }

        // Calculate the object space light details
        Vector4f lightPos = srclight->getSpace4();
        Matrix4f world2Obj = mSpaceNode->getSpaceMatrix().inverseAffine();
        lightPos = world2Obj.affine(lightPos);

        // We need to search the edge list for silhouette edges
        GeoEdget * edgeList = getEdgeList();

        if(!edgeList)
        {
            // we can't get an edge list for some reason, return blank
            // really we shouldn't be able to get here, but this is a safeguard
            return mShadowVolumeList;
        }

        bool init = mShadowVolumeList.empty();

        if(init)
            mShadowVolumeList.resize(edgeList->mGeoList.size());

        bool isAnimated = hasAnimation;
        bool updatedSharedGeomNormals = false;
        GeometrySpaceShadowVolume * esr = 0;
        ShadowVolumeList::iterator si, siend = mShadowVolumeList.end();
        GeoEdget::EdgeGroupList::iterator egi = edgeList->mGeoList.begin();
        for(si = mShadowVolumeList.begin(); si != siend; ++si, ++egi)
        {
            const VertexData * pVertData;
            if(isAnimated)
            {
                pVertData = getSWFusionVertex(egi->mVertex);
            }
            else
            {
                pVertData = egi->mVertex;
            }

            if(init)
            {
                // Try to find corresponding GeometryCom; this allows the
                // linkage of visibility between ShadowVolume and GeometryCom
                GeometryCom * sub = 0;

                if(egi->mVertex == mMesh->getVertexData())
                {
                    sub = 0;
                }
                else
                {
                    Coms::iterator c, cend = mComs.end();
                    for(c = mComs.begin(); c != cend; ++c)
                    {
                        GeometryCom * se = *c;
                        if(egi->mVertex == se->getMesh()->getVertexData())
                        {
                            sub = se;
                        }
                    }
                }

                // Create a new renderable, create a separate light cap if
                // we're using a vertex program (either for this model, or
                // for extruding the shadow volume) since otherwise we can
                // get depth-fighting on the light cap

                *si = N_new GeometrySpaceShadowVolume(this, destindex, pVertData,
                    mVPEffect || ((flags & ShadowObj::VT_Software) == 0), sub);
            }
            else
            {
                // If we have animation, we have no guarantee that the position
                // buffer we used last frame is the same one we used last frame
                // since a temporary buffer is requested each frame
                // therefore, we need to update the GeometrySpaceShadowVolume
                // with the current position buffer
                static_cast<GeometrySpaceShadowVolume *>(*si)->rebindPosBuffer(pVertData, hasAnimation);

            }
            // Get shadow renderable
            esr = static_cast<GeometrySpaceShadowVolume *>(*si);
            VertexBuffer * svposbuf = esr->getPositionBuffer();
            // For animated entities we need to recalculate the face normals
            if(hasAnimation)
            {
                if(egi->mVertex != mMesh->getVertexData() || !updatedSharedGeomNormals)
                {
                    // recalculate face normals
                    edgeList->updateNormals(egi->mVertexDataIndex, svposbuf);
                    // If we're not extruding in software we still need to update
                    // the latter part of the buffer (the hardware extruded part)
                    // with the latest animated positions
                    if((flags & ShadowObj::VT_Software) == 0)
                    {
                        // Lock, we'll be locking the (suppressed hardware update) shadow buffer
                        NIIf * pSrc = static_cast<NIIf *>(svposbuf->lock(Buffer::MM_READ | Buffer::MM_WRITE));
                        NIIf * pDest = pSrc + (egi->mVertex->mCount * 3);
                        memcpy(pDest, pSrc, sizeof(NIIf) * 3 * egi->mVertex->mCount);
                        svposbuf->unlock();
                    }
                    if(egi->mVertex == mMesh->getVertexData())
                    {
                        //公共网格仅更新一次
                        updatedSharedGeomNormals = true;
                    }
                }
            }
            // Extrude vertices in software if required
            if(flags & ShadowObj::VT_Software)
                extrude(svposbuf, egi->mVertex->mCount, lightPos, destdis);

            // Stop suppressing hardware update now, if we were
            svposbuf->setSync(true);
        }
        // Calc triangle light facing
        edgeList->updateView(lightPos);
        // Generate indexes and update renderables
        generate(edgeList, destindex, srclight, flags, mShadowVolumeList);

        return mShadowVolumeList;
    }
    //-----------------------------------------------------------------------
    NIIf GeometrySpace::getExtrudeDistance(const Light * l) const
    {
        if(mSpaceNode)
        {
            Vector3f diff = mSpaceNode->getSpacePos() - const_cast<Light *>(l)->getSpacePos();
            return l->getRange() - diff.length();
        }

        return 0;
    }
    //-----------------------------------------------------------------------
    const VertexData * GeometrySpace::getSWFusionVertex(const VertexData * orig)
    {
        if(orig == mMesh->getVertexData())
        {
            return mSkeleton != 0 ? mAniData[2] : mAniData[0];
        }
        Coms::iterator i, iend = mComs.end();
        for(i = mComs.begin(); i != iend; ++i)
        {
            GeometryCom * se = *i;
            if(orig == se->getMesh()->getVertexData())
            {
                return mSkeleton != 0 ? se->mAniData[2] : se->mAniData[0];
            }
        }

        N_EXCEPT(NotExist, _I18n("Cannot find blended version of the vertex data specified."));
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::attach(PosNode * parent, bool tag)
    {
        SpaceObj::attach(parent, tag);
        Lods::iterator i, iend = mLods.end();
        for(i = mLods.begin() + 1; i != iend; ++i)
        {
            (*i)->attach(parent, tag);
        }
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::setRenderGroup(Nui16 qid, Nui16 level)
    {
        SpaceObj::setRenderGroup(qid, level);
        // Set render queue for all manual LOD entities
        Nidx i = 0;
        Lods::iterator li, liend = mLods.end();
        for(li = mLods.begin() + 1; li != liend; ++li, ++i)
        {
            if(!mMesh->getLodScheme()->isRelation(i))
            {
                (*li)->setRenderGroup(qid, level);
            }
        }
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::updateAnimation()
    {
        mMesh->_update(mAnimationCtrl);
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::linkSkeleton(GeometrySpace * obj)
    {
        if(mLinkList != obj->mLinkList)
        {
            if(obj->getMesh()->getSkeleton() != getMesh()->getSkeleton())
            {
                N_EXCEPT(Runtime, _I18n("The supplied entity has a different skeleton."));
            }
            if(!mSkeleton)
            {
                N_EXCEPT(Runtime, _I18n("This entity has no skeleton."));
            }
            if(mLinkList != NULL && obj->mLinkList != NULL)
            {
                N_EXCEPT(Runtime,
                    _I18n("Both entities already shares their SkeletonInstances! At least ")
                        _I18n("one of the instances must not share it's instance."));
            }

            if(mLinkList != NULL)
            {
                obj->linkSkeleton(this);
            }
            else
            {
                N_delete mSkeleton;
                N_delete mAnimationCtrl;
                N_free(mSkeletonFrame);
                mSkeleton = obj->mSkeleton;
                mAnimationCtrl = obj->mAnimationCtrl;
                mSkeletonFrame = obj->mSkeletonFrame;
                if(obj->mLinkList == NULL)
                {
                    obj->mLinkList = N_new_t(GeoList)();
                    obj->mLinkList->insert(obj);
                }
                mLinkList = obj->mLinkList;
                mLinkList->insert(this);
            }
        }
    }
    //-----------------------------------------------------------------------
    void GeometrySpace::unlinkSkeleton()
    {
        if(mLinkList == NULL)
        {
            N_EXCEPT(Runtime, _I18n("并有共享骨骼!"));
        }

        if(mLinkList->size() == 1)
        {
            //just reset
            N_delete_t(mLinkList, GeoList);
            mLinkList = 0;
        }
        else
        {
            mSkeleton = N_new SkeletonFusion(mMesh->getSkeleton());
            mSkeleton->load();
            mAnimationCtrl = N_new AnimationFusionObj();
            mMesh->_init(mAnimationCtrl);
            mSkeletonFrame = N_new_t(Nmark)(N_TYPE_MAX(Nmark));

            mLinkList->erase(this);
            if(mLinkList->size() == 1)
            {
                (*mLinkList->begin())->unlinkSkeleton();
            }
            mLinkList = 0;
        }
    }
    //-----------------------------------------------------------------------
    FactoryID GeometrySpace::getFactoryID() const
    {
        return N_FACTORY_GeoSpace;
    }
    //-----------------------------------------------------------------------
    Nui32 GeometrySpace::getTypeMark() const
    {
        return SpaceManager::SelfGeoMark;
    }
    //-----------------------------------------------------------------------
    VertexData * GeometrySpace::getRenderVertex() const
    {
        if(mSkeleton != 0)
        {
            if(!isHWAnimation())
            {
                // all software skeletal binds same vertex data
                // may be a 2-stage s/w transform including morph earlier though
                return mAniData[2];
            }
            else if(mAnimationMode != ANIM_None)
            {
                // hardware morph animation
                return mAniData[1];
            }
            else
            {
                // hardware skeletal, no morphing
                return mMesh->getVertexData();
            }
        }
        else if(mAnimationMode != ANIM_None)
        {
            // morph only, no skeletal
            if(isHWAnimation())
            {
                return mAniData[1];
            }
            else
            {
                return mAniData[0];
            }
        }
        else
        {
            return mMesh->getVertexData();
        }
        return mMesh->getVertexData();
    }
    //---------------------------------------------------------------------
    void GeometrySpace::query(GeometryQuery * query)
    {
        Coms::iterator i, iend = mComs.end();
        for(i = mComs.begin(); i != iend; ++i)
        {
            query->query(*i);
        }
        Nidx lodi = 1;
        GeometryCom * temp;
        Lods::iterator e, end = mLods.end();;
        for(e = mLods.begin() + 1; e != end; ++e, ++lodi)
        {
            NCount nsub = (*e)->getUnitCount();
            for(Nidx s = 0; s < nsub; ++s)
            {
                temp = (*e)->getCom(s);
                temp->setGeometryLod(lodi);
                query->query(temp);
            }
        }
    }
    //-----------------------------------------------------------------------
}