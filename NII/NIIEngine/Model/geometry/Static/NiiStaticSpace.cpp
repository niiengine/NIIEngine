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
#include "NiiStaticSpace.h"
#include "NiiGeometrySpace.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiLogManager.h"
#include "NiiGBufferManager.h"
#include "NiiLight.h"
#include "NiiMeshLodScheme.h"
#include "NiiMaterialLodScheme.h"
#include "NiiMaterialManager.h"
#include "NiiEngine.h"
#include "NiiEdgeListBuilder.h"
#include "NiiRenderPattern.h"
#include "NiiGeometryObj.h"

#define packIndex(x, y, z) (x + (y << 10) + (z << 20))

namespace NII
{
    typedef map<NCount, NCount>::type IndexRemap;
    //--------------------------------------------------------------------------
    template<typename T>
    void copyIndexes(const T * src, T * dst, NCount count, NCount indexOffset)
    {
        if (indexOffset == 0)
        {
            memcpy(dst, src, sizeof(T) * count);
        }
        else
        {
            while (count--)
            {
                *dst++ = static_cast<T>(*src++ + indexOffset);
            }
        }
    }

    /// 计算出一个索引缓存使用的顶点和计算一个重新映射找出一个仅包含那些顶点的顶点缓存的方法
    template <typename T>
    void buildIndexRemap(T* pBuffer, NCount numIndexes, IndexRemap & remap)
    {
        remap.clear();
        for (NCount i = 0; i < numIndexes; ++i)
        {
            // use insert since duplicates are silently discarded
            remap.insert(IndexRemap::value_type(*pBuffer++, remap.size()));
            // this will have mapped oldindex -> new index IF oldindex
            // wasn't already there
        }
    }

    /// 基于一个重新映射改变索引的方法
    template <typename T>
    void remapIndexes(T * src, T * dst, const IndexRemap & remap, NCount numIndexes)
    {
        for (NCount i = 0; i < numIndexes; ++i)
        {
            // look up original and map to target
            IndexRemap::const_iterator ix = remap.find(*src++);
            N_assert(ix != remap.end(), "error");
            *dst++ = static_cast<T>(ix->second);
        }
    }
    //-----------------------------------------------------------------------
    class _EngineAPI LODBucketShadowVolume : public ShadowVolume
    {
    public:
        LODBucketShadowVolume(StaticSpace::LODBatch * parent, IndexBuffer * idxbuf,
            const VertexData * vertexData, bool createSeparateLightCap, bool isLightCap = false) : 
                mParent(parent)
        {
            // Initialise render op
            mRenderOp.mIndex = N_new IndexData();
            mRenderOp.mIndex->mBuffer = idxbuf;
            mRenderOp.mIndex->mOffset = 0;
            // index start and count are sorted out later

            // Create vertex data which just references position component (and 2 component)
            N_Only(GBuffer).create(mRenderOp.mVertex);
            // Map in position data
            mRenderOp.mVertex->add(DT_Float3x, VT_Pos, 0, 0);
            Nui16 origPosBind = vertexData->get(VT_Pos)->mSource;
            mPositionBuffer = vertexData->getAttach(origPosBind);
            mRenderOp.mVertex->attach(0, mPositionBuffer);
            // Map in w-coord buffer (if present)
            if (vertexData->getExt() != 0)
            {
                mRenderOp.mVertex->add(DT_Float1x, VT_Tex_Coord, 1, 0, 0);
                mWBuffer = vertexData->getExt();
                mRenderOp.mVertex->attach(1, mWBuffer);
            }
            // Use same vertex start as input
            mRenderOp.mVertex->mOffset = vertexData->mOffset;

            if (isLightCap)
            {
                // Use original vertex count, no extrusion
                mRenderOp.mVertex->mCount = vertexData->mCount;
            }
            else
            {
                // Vertex count must take into account the doubling of the buffer,
                // because second half of the buffer is the extruded copy
                mRenderOp.mVertex->mCount = vertexData->mCount * 2;
                if (createSeparateLightCap)
                {
                    mLightCap = N_new LODBucketShadowVolume(parent, idxbuf, vertexData, false, true);
                }
            }
        };

        ~LODBucketShadowVolume()
        {
            N_delete mRenderOp.mIndex;
            N_delete mRenderOp.mVertex;
        }

        inline VertexBuffer * getWBuffer() const { return mWBuffer; }

        inline VertexBuffer * getPositionBuffer() const { return mPositionBuffer; }

        /// @copydetails GeometryObj::getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const
        {
            *out = mParent->getBatchObj()->getSpaceMatrix();
            count = 1;
        }
    protected:
        StaticSpace::LODBatch * mParent;
        VertexBuffer * mPositionBuffer;
        VertexBuffer * mWBuffer;
    };
    //--------------------------------------------------------------------------
    class _EngineInner StaticSpace::OptMeshGeo : public BatchObjectAlloc
    {
    public:
        OptMeshGeo() :
            mVertex(0),
            mIndex(0) {}
        OptMeshGeo(VertexData * vdata, IndexData * idata) :
            mVertex(vdata),
            mIndex(idata) {}
        ~OptMeshGeo()
        {
            if (mVertex)
            {
                N_delete mVertex;
                mVertex = 0;
            }
            if (mIndex)
            {
                N_delete mIndex;
                mIndex = 0;
            }
        }
    public:
        VertexData * mVertex;
        IndexData * mIndex;
    };

    struct StaticSpace::OptMeshGeoRef
    {
        VertexData * mVertex;
        IndexData * mIndex;
    };

    struct StaticSpace::Geometry : public BatchObjectAlloc
    {
        OptMeshGeoRef * geometry;
        Vector3f position;
        Quaternion orientation;
        Vector3f scale;
    };

    struct StaticSpace::MeshRef : public BatchObjectAlloc
    {
        SubMesh * mMesh;
        OptMeshGeoRefList * mLodList;
        ResourceID materialName;
        Vector3f position;
        Quaternion orientation;
        Vector3f scale;
        AABox worldBounds;
    };
    //--------------------------------------------------------------------------
    String getGeometryFormatString(StaticSpace::OptMeshGeoRef * geom)
    {
        // Formulate an identifying string for the geometry format
        // Must take into account the vertex declaration and the index type
        // Format is (all lines separated by '|'):
        // Index type
        // Vertex element (repeating)
        //   source
        //   semantic
        //   type
        StringStream str;

        str << geom->mIndex->mBuffer->getUnitSize() << "|";
        const VertexUnitList & ulist = geom->mVertex->getUnits();
        VertexUnitList::const_iterator i, iend = ulist.end();
        for (i = ulist.begin(); i != iend; ++i)
        {
            const VertexUnit & elem = *i;
            str << elem.mSource << "|";
            str << elem.mVType << "|";
            str << elem.mType << "|";
        }
        return str.str();
    }
    class StaticSpace::LODBatch::ShadowVolume : public BatchObjectAlloc
    {
    public:
        ShadowVolume(LODBatch * batch) :
            mLODBatch(batch),
            mEdgeList(0),
            mVertexProgram(false)
        {
        }
        ~ShadowVolume()
        {
            N_delete mEdgeList;
            ShadowVolumeList::iterator s, send = mShadowVolumeList.end();
            for (s = mShadowVolumeList.begin(); s != send; ++s)
            {
                N_delete *s;
            }
            mShadowVolumeList.clear();
        }
        void updateShadowVolume(const Vector4f & lightPos, IndexBuffer * index, NIIf extend, Nmark flags = 0);
    public:
        LODBatch * mLODBatch;
        bool mVertexProgram;
        GeoEdget * mEdgeList;
        ShadowVolumeList mShadowVolumeList;
    };
    //--------------------------------------------------------------------------
    #define REGION_RANGE 1024
    #define REGION_HALF_RANGE 512
    #define REGION_MAX_INDEX 511
    #define REGION_MIN_INDEX -512
    //--------------------------------------------------------------------------
    StaticSpace::StaticSpace(SpaceManager * owner, SpaceID sid):
        mManager(owner),
        mID(sid),
        mVisibleDistance(0.0f),
        mBatchSize(Vector3f(1000,1000,1000)),
        mOrigin(Vector3f(0,0,0)),
        mCastShadow(false),
        mVisible(true),
        mBuilt(false),
        mRenderGroup(RQG_Unknow),
        mVisibleMark(0xFFFFFFFF)
    {
    }
    //--------------------------------------------------------------------------
    StaticSpace::~StaticSpace()
    {
        removeAll();
    }
    //--------------------------------------------------------------------------
    StaticSpace::BatchObj * StaticSpace::getBatchObj(const AABox & bounds)
    {
        if (bounds.isNull())
            return 0;

        // Get the region which has the largest overlapping volume
        const Vector3f & min = bounds.getMin();
        const Vector3f & max = bounds.getMax();

        // Get the min and max region indexes
        Nui16 minx, miny, minz;
        Nui16 maxx, maxy, maxz;
        getBatchIndex(min, minx, miny, minz);
        getBatchIndex(max, maxx, maxy, maxz);
        NIIf maxVolume = 0.0f;
        Nui16 finalx = 0, finaly = 0, finalz = 0;
        for (Nui16 x = minx; x <= maxx; ++x)
        {
            for (Nui16 y = miny; y <= maxy; ++y)
            {
                for (Nui16 z = minz; z <= maxz; ++z)
                {
                    NIIf vol = getVolume(bounds, x, y, z);
                    if (vol > maxVolume)
                    {
                        maxVolume = vol;
                        finalx = x;
                        finaly = y;
                        finalz = z;
                    }

                }
            }
        }

        N_assert(maxVolume > 0.0f, "Static geometry: Problem determining closest volume match!");

        return getBatchObj(finalx, finaly, finalz);
    }
    //--------------------------------------------------------------------------
    NIIf StaticSpace::getVolume(const AABox & box, Nui16 x, Nui16 y, Nui16 z)
    {
        // Get bounds of indexed region
        AABox regionBounds = getBatchAABB(x, y, z);
        AABox intersectBox = regionBounds.intersection(box);
        // return a 'volume' which ignores zero dimensions
        // since we only use this for relative comparisons of the same bounds
        // this will still be internally consistent
        Vector3f boxdiff = box.getMax() - box.getMin();
        Vector3f intersectDiff = intersectBox.getMax() - intersectBox.getMin();

        return (boxdiff.x == 0 ? 1 : intersectDiff.x) *
            (boxdiff.y == 0 ? 1 : intersectDiff.y) *
            (boxdiff.z == 0 ? 1 : intersectDiff.z);

    }
    //--------------------------------------------------------------------------
    AABox StaticSpace::getBatchAABB(Nui16 x, Nui16 y, Nui16 z)
    {
        Vector3f min(
            ((NIIf)x - REGION_HALF_RANGE) * mBatchSize.x + mOrigin.x,
            ((NIIf)y - REGION_HALF_RANGE) * mBatchSize.y + mOrigin.y,
            ((NIIf)z - REGION_HALF_RANGE) * mBatchSize.z + mOrigin.z
            );
        Vector3f max = min + mBatchSize;
        return AABox(min, max);
    }
    //--------------------------------------------------------------------------
    Vector3f StaticSpace::getBatchCentre(Nui16 x, Nui16 y, Nui16 z)
    {
        Vector3f temp = mBatchSize * 0.5;
        return Vector3f(
            ((NIIf)x - REGION_HALF_RANGE) * mBatchSize.x + mOrigin.x + temp.x,
            ((NIIf)y - REGION_HALF_RANGE) * mBatchSize.y + mOrigin.y + temp.y,
            ((NIIf)z - REGION_HALF_RANGE) * mBatchSize.z + mOrigin.z + temp.z
            );
    }
    //--------------------------------------------------------------------------
    StaticSpace::BatchObj * StaticSpace::getBatchObj(Nui16 x, Nui16 y, Nui16 z)
    {
        Nui32 index = packIndex(x, y, z);
        BatchObj * ret = 0;
        BatchList::iterator i = mBatchList.find(index);
        if (i != mBatchList.end())
        {
            ret = i->second;
        }
        if(!ret)
        {
            // Make a name
            //StringStream str;
            //str << mName << ":" << index;
            // Calculate the region centre
            Vector3f centre = getBatchCentre(x, y, z);
            ret = N_new BatchObj(this, index, mManager, index, centre);
            mManager->addSpace(ret);
            ret->setVisible(mVisible);
            ret->setCastShadow(mCastShadow);
            if(mRenderGroup != RQG_Unknow)
            {
                ret->setRenderGroup(mRenderGroup);
            }
            mBatchList[index] = ret;
        }
        return ret;
    }
    //--------------------------------------------------------------------------
    void StaticSpace::getBatchIndex(const Vector3f & point, Nui16 & x, Nui16 & y, Nui16 & z)
    {
        // Scale the point into multiples of region and adjust for origin
        Vector3f scaledPoint = (point - mOrigin) / mBatchSize;

        // Round down to 'bottom left' point which represents the cell index
        NIIi ix = Math::IFloor(scaledPoint.x);
        NIIi iy = Math::IFloor(scaledPoint.y);
        NIIi iz = Math::IFloor(scaledPoint.z);

        // Check bounds
        if (ix < REGION_MIN_INDEX || ix > REGION_MAX_INDEX
            || iy < REGION_MIN_INDEX || iy > REGION_MAX_INDEX
            || iz < REGION_MIN_INDEX || iz > REGION_MAX_INDEX)
        {
            N_EXCEPT(InvalidParam, _I18n("Point out of bounds"));
        }
        // Adjust for the fact that we use unsigned values for simplicity
        // (requires less faffing about for negatives give 10-bit packing
        x = static_cast<Nui16>(ix + REGION_HALF_RANGE);
        y = static_cast<Nui16>(iy + REGION_HALF_RANGE);
        z = static_cast<Nui16>(iz + REGION_HALF_RANGE);
    }
    //--------------------------------------------------------------------------
    StaticSpace::BatchObj* StaticSpace::getBatchObj(const Vector3f& point)
    {
        Nui16 x, y, z;
        getBatchIndex(point, x, y, z);
        return getBatchObj(x, y, z);
    }
    //--------------------------------------------------------------------------
    void StaticSpace::add(GeometrySpace* ent, const Vector3f & pos,
        const Quaternion & orientation, const Vector3f & scale)
    {
        const Mesh * msh = ent->getMesh();
        if(!msh->getLodScheme()->isRelation(0))
        {
            N_Only(Log).log(
                _I18n("WARNING (StaticSpace): Manual LOD is not supported. ")
                _I18n("Using only highest LOD level for mesh ") + msh->getOriginID());
        }

        AABox sharedWorldBounds;
        // queue this entities submeshes and choice of material
        // also build the lists of geometry to be used for the source of lods
        for(NCount i = 0; i < ent->getUnitCount(); ++i)
        {
            GeometryCom * se = ent->getCom(i);
            MeshRef * q = N_new MeshRef();

            q->mMesh = se->getMesh();
            q->mLodList = createRefList(q->mMesh);
            q->materialName = se->getMaterialID();
            q->orientation = orientation;
            q->position = pos;
            q->scale = scale;
            (*q->mLodList)[0].mVertex->calcAABB(pos, orientation, scale, q->worldBounds);

            mMeshRefList.push_back(q);
        }
    }
    //--------------------------------------------------------------------------
    StaticSpace::OptMeshGeoRefList * StaticSpace::createRefList(SubMesh * sm)
    {
        MeshGeoRefList::iterator i = mMeshGeoRefList.find(sm);
        if(i != mMeshGeoRefList.end())
        {
            return i->second;
        }
        OptMeshGeoRefList * lodList = N_new_t(OptMeshGeoRefList)();
        mMeshGeoRefList[sm] = lodList;
        NCount numLods = sm->getMesh()->getLodScheme()->isRelation(0) ? sm->getMesh()->getLodCount() : 1;
        lodList->resize(numLods);
        for(NCount lod = 0; lod < numLods; ++lod)
        {
            OptMeshGeoRef & geomLink = (*lodList)[lod];
            IndexData * lodidata;
            if(lod == 0)
            {
                lodidata = sm->getIndexData(0);
            }
            else
            {
                lodidata = sm->getIndexData(lod);
            }
            if(sm->isSelfVertex())
            {
                if(lod == 0)
                {
                    geomLink.mVertex = sm->getVertexData();
                    geomLink.mIndex = sm->getIndexData(0);
                }
                else
                {
                    createRef(&geomLink, sm->getVertexData(), lodidata);
                }
            }
            else
            {
                if(sm->getMesh()->getSubCount() == 1)
                {
                    geomLink.mVertex = sm->getMesh()->getVertexData();
                    geomLink.mIndex = lodidata;
                }
                else
                {
                    createRef(&geomLink, sm->getMesh()->getVertexData(), lodidata);
                }
            }
            N_assert (geomLink.mVertex->mOffset == 0,
                "Cannot use mOffset > 0 on indexed geometry due to "
                "rendersystem incompatibilities - see the docs!");
        }

        return lodList;
    }
    //--------------------------------------------------------------------------
    void StaticSpace::createRef(StaticSpace::OptMeshGeoRef * ref, 
        VertexData * vdata, IndexData * idata)
    {
        // Firstly we need to scan to see how many vertices are being used
        // and while we're at it, build the remap we can use later
        bool idx32 = idata->mBuffer->getUnitSize() == 32;
        IndexRemap indexRemap;
        if (idx32)
        {
            Nui32 *p32 = static_cast<Nui32 *>(idata->mBuffer->lock(idata->mOffset,
                idata->mCount * idata->mBuffer->getUnitSize(), Buffer::MM_READ));
            buildIndexRemap(p32, idata->mCount, indexRemap);
            idata->mBuffer->unlock();
        }
        else
        {
            Nui16 *p16 = static_cast<Nui16 *>(idata->mBuffer->lock(idata->mOffset,
                idata->mCount * idata->mBuffer->getUnitSize(), Buffer::MM_READ));
            buildIndexRemap(p16, idata->mCount, indexRemap);
            idata->mBuffer->unlock();
        }
        if(indexRemap.size() == vdata->mCount)
        {
            // ha, complete usage after all
            ref->mVertex = vdata;
            ref->mIndex = idata;
            return;
        }
        // Create the new vertex data records
        ref->mVertex = vdata->clone(false);
        // Convenience
        VertexData * newvd = ref->mVertex;
        //IndexData* newid = ref->mIndex;
        // Update the vertex count
        newvd->mCount = indexRemap.size();

        NCount numvbufs = vdata->getAttachCount();
        // Copy buffers from old to new
        for(Nui16 b = 0; b < numvbufs; ++b)
        {
            VertexBuffer * oldBuf = vdata->getAttach(b);
            // Create new buffer
            VertexBuffer * newBuf;
            N_Only(GBuffer).create(newBuf, oldBuf->getUnitSize(), indexRemap.size(), Buffer::M_NORMAL);
            newvd->attach(b, newBuf);

            // Copy all the elements of the buffer across, by iterating over
            // the IndexRemap which describes how to move the old vertices
            // to the new ones. By nature of the map the remap is in order of
            // indexes in the old buffer, but note that we're not guaranteed to
            // address every vertex (which is kinda why we're here)
            Nui8 * pSrcBase = (Nui8*)(oldBuf->lock(Buffer::MM_READ));
            Nui8 * pDstBase = (Nui8*)(newBuf->lock(Buffer::MM_WHOLE));

            NCount vertexSize = oldBuf->getUnitSize();
            N_assert (vertexSize == newBuf->getUnitSize(), "error");

            for (IndexRemap::iterator r = indexRemap.begin(); r != indexRemap.end(); ++r)
            {
                N_assert (r->first < oldBuf->getUnitCount(), "error");
                N_assert (r->second < newBuf->getUnitCount(), "error");

                Nui8* pSrc = pSrcBase + r->first * vertexSize;
                Nui8* pDst = pDstBase + r->second * vertexSize;
                memcpy(pDst, pSrc, vertexSize);
            }

            oldBuf->unlock();
            newBuf->unlock();
        }

        IndexBuffer * ibuf;
        N_Only(GBuffer).create(ibuf, idata->mBuffer->getUnitSize(), idata->mCount, Buffer::M_NORMAL );

        if (idx32)
        {
            Nui32 * pSrc32 = static_cast<Nui32 *>(idata->mBuffer->lock(idata->mOffset,
                idata->mCount * idata->mBuffer->getUnitSize(), Buffer::MM_READ));
            Nui32 * pDst32 = static_cast<Nui32 *>(ibuf->lock(Buffer::MM_WHOLE));
            remapIndexes(pSrc32, pDst32, indexRemap, idata->mCount);
            idata->mBuffer->unlock();
            ibuf->unlock();
        }
        else
        {
            Nui16 *pSrc16 = static_cast<Nui16 *>(idata->mBuffer->lock(idata->mOffset,
                idata->mCount * idata->mBuffer->getUnitSize(), Buffer::MM_READ));
            Nui16 * pDst16 = static_cast<Nui16 *>(ibuf->lock(Buffer::MM_WHOLE));
            remapIndexes(pSrc16, pDst16, indexRemap, idata->mCount);
            idata->mBuffer->unlock();
            ibuf->unlock();
        }

        ref->mIndex = N_new IndexData();
        ref->mIndex->mOffset = 0;
        ref->mIndex->mCount = idata->mCount;
        ref->mIndex->mBuffer = ibuf;

        OptMeshGeo * optGeom = N_new OptMeshGeo(ref->mVertex, ref->mIndex);
        mOptMeshGeoList.push_back(optGeom);
    }
    //--------------------------------------------------------------------------
    void StaticSpace::add(const SpaceNode * node)
    {
        SpaceNode::AttachList::const_iterator j, jend = node->getAttachList().end();
        for(j = node->getAttachList().begin(); j != jend; ++j)
        {
            SpaceObj * mobj = j->second;
            if(mobj->getFactoryID() == N_FACTORY_GeoSpace)
            {
                add(static_cast<GeometrySpace *>(mobj), node->getSpacePos(),
                    node->getSpaceOri(), node->getSpaceScale());
            }
        }

        PosNode::ChildList::const_iterator i, iend = node->getChildList().end();
        for(i = node->getChildList().begin(); i != iend; ++i)
        {
            const SpaceNode * node = static_cast<const SpaceNode *>(i->second);
            add(node);
        }
    }
    //--------------------------------------------------------------------------
    void StaticSpace::build()
    {
        destroy();

        MeshRefList::iterator qi, qiend = mMeshRefList.end();
        for (qi = mMeshRefList.begin(); qi != qiend; ++qi)
        {
            MeshRef * ref = *qi;
            BatchObj* region = getBatchObj(ref->worldBounds);
            region->add(ref);
        }
        bool stencilShadow = false;
        if(mCastShadow && mManager->getRenderPattern()->isStencilShadow())
        {
            stencilShadow = true;
        }

        BatchList::iterator ri, riend = mBatchList.end();
        for (ri = mBatchList.begin(); ri!= riend; ++ri)
        {
            ri->second->build(stencilShadow);
            ri->second->setVisibleMark(mVisibleMark);
        }
    }
    //--------------------------------------------------------------------------
    void StaticSpace::setVisibleDistance(NIIf dist)
    {
        mVisibleDistance = dist;
    }
    //--------------------------------------------------------------------------
    void StaticSpace::destroy()
    {
        BatchList::iterator i, iend = mBatchList.end();
        for(i = mBatchList.begin(); i != iend; ++i)
        {
            mManager->removeSpace(i->second->getID(), i->second->getFactoryID());
            N_delete i->second;
        }
        mBatchList.clear();
    }
    //--------------------------------------------------------------------------
    void StaticSpace::removeAll()
    {
        destroy();
        MeshRefList::iterator i, iend = mMeshRefList.end();
        for(i = mMeshRefList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mMeshRefList.clear();
        MeshGeoRefList::iterator m, mend = mMeshGeoRefList.end();
        for(m = mMeshGeoRefList.begin(); m != mend; ++m)
        {
            N_delete_t(m->second, OptMeshGeoRefList);
        }
        mMeshGeoRefList.clear();

        OptMeshGeoList::iterator o, oend = mOptMeshGeoList.end();
        for (o = mOptMeshGeoList.begin(); o != oend; ++o)
        {
            N_delete *o;
        }
        mOptMeshGeoList.clear();
    }
    //--------------------------------------------------------------------------
    void StaticSpace::setVisible(bool set)
    {
        mVisible = set;
        for (BatchList::iterator ri = mBatchList.begin(); ri != mBatchList.end(); ++ri)
        {
            ri->second->setVisible(set);
        }
    }
    //--------------------------------------------------------------------------
    bool StaticSpace::isVisible() const 
    { 
        return mVisible; 
    }
    //--------------------------------------------------------------------------
    void StaticSpace::setCastShadow(bool set)
    {
        mCastShadow = set;
        BatchList::iterator i, iend = mBatchList.end();
        for(BatchList::iterator i = mBatchList.begin(); i != iend; ++i)
        {
            i->second->setCastShadow(set);
        }
    }
    //--------------------------------------------------------------------------
    bool StaticSpace::isCastShadow() const
    { 
        return mCastShadow; 
    }
    //--------------------------------------------------------------------------
    void StaticSpace::setOrigin(const Vector3f & origin) 
    { 
        mOrigin = origin; 
    }
    //--------------------------------------------------------------------------
    void StaticSpace::setBatchSize(const Vector3f & size)
    {
        mBatchSize = size;
    }
    //--------------------------------------------------------------------------
    void StaticSpace::setRenderGroup(Nui16 queueID)
    {
        N_assert(queueID <= RQG_Count, "Render queue out of range!");
        mRenderGroup = queueID;
        // tell any existing regions
        BatchList::iterator i, iend = mBatchList.end();
        for(i = mBatchList.begin(); i != iend; ++i)
        {
            i->second->setRenderGroup(queueID);
        }
    }
    //--------------------------------------------------------------------------
    Nui16 StaticSpace::getRenderGroup() const
    {
        return mRenderGroup;
    }
    //--------------------------------------------------------------------------
    void StaticSpace::setVisibleMark(Nmark mark)
    {
        mVisibleMark = mark;
        BatchList::const_iterator i, iend = mBatchList.end();
        for(i = mBatchList.begin(); i != iend; ++i)
        {
            i->second->setVisibleMark(mark);
        }
    }
    //--------------------------------------------------------------------------
    Nmark StaticSpace::getVisibleMark() const
    {
        if(mBatchList.empty())
            return 0xFFFFFFFF;

        BatchList::const_iterator ri = mBatchList.begin();
        return ri->second->getVisibleMark();
    }
    //--------------------------------------------------------------------------
    /*void StaticSpace::dump(const String& filename) const
    {
        Nofstream of(filename.c_str());
        of << _I18n("Static Geometry Report for ") << mName << std::endl;
        of << "-------------------------------------------------" << std::endl;
        of << _I18n("Number of queued submeshes: ") << mMeshRefList.size() << std::endl;
        of << _I18n("Number of regions: ") << mBatchList.size() << std::endl;
        of << _I18n("BatchObj dimensions: ") << mBatchSize << std::endl;
        of << _I18n("Origin: ") << mOrigin << std::endl;
        of << _I18n("Max distance: ") << mVisibleDistance << std::endl;
        of << _I18n("Casts shadows?: ") << mCastShadow << std::endl;
        of << std::endl;
        for (BatchList::const_iterator ri = mBatchList.begin(); ri != mBatchList.end(); ++ri)
        {
            ri->second->dump(of);
        }
        of << "-------------------------------------------------" << std::endl;
    }*/
    //---------------------------------------------------------------------
    void StaticSpace::query(GeometryQuery * query)
    {
        BatchList::const_iterator ri, riend = mBatchList.end();
        for(ri = mBatchList.begin(); ri != riend; ++ri)
        {
            ri->second->query(query);
        }
    }
    //--------------------------------------------------------------------------
    const StaticSpace::BatchList & StaticSpace::getBatchList() const
    {
        return mBatchList;
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //BatchObj
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    StaticSpace::BatchObj::BatchObj(StaticSpace * parent, SpaceID sid,
        SpaceManager * mgr, Nui32 regionID, const Vector3f & centre) :
            ShadowObj(sid),
            mParent(parent),
            mManager(mgr),
            mNode(0),
            mRegionID(regionID),
            mCentre(centre),
            mBoundRange(0.0f),
            mLodIdx(0),
            mLodScheme(0)
    {
    }
    //--------------------------------------------------------------------------
    StaticSpace::BatchObj::~BatchObj()
    {
        if(mNode)
        {
            static_cast<SpaceNode *>(mNode->getParent())->remove(mNode);
            mManager->destroySpaceNode(mNode->getID());
            mNode = 0;
        }
        LODBatchList::iterator i, iend = mLODBatchList.end();
        for(i = mLODBatchList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mLODBatchList.clear();
    }
    //--------------------------------------------------------------------------
    Nui32 StaticSpace::BatchObj::getTypeMark() const
    {
        return SpaceManager::StaticGeoMark;
    }
    //--------------------------------------------------------------------------
    void StaticSpace::BatchObj::add(MeshRef * ref)
    {
        mMeshRefList.push_back(ref);

        MeshLodScheme * scheme = ref->mMesh->getMesh()->getLodScheme();
        if(mLodScheme == 0)
        {
            mLodScheme = scheme;
        }
        else
        {
            if(mLodScheme != scheme)
                N_EXCEPT(InvalidParam, _I18n("Lod strategies do not match"));
        }

        NCount lodLevels = scheme->getCount();
        N_assert(ref->mLodList->size() == lodLevels, "error");

        AABox localBounds(ref->worldBounds.getMin() - mCentre, ref->worldBounds.getMax() - mCentre);

        mAABB.merge(localBounds);
        mBoundRange = mAABB.calcRadius();
    }
    //--------------------------------------------------------------------------
    void StaticSpace::BatchObj::build(bool stencilShadow)
    {
        mNode = static_cast<SpaceNode *>(mManager->getOriginNode()->create(mID, mCentre));
        mNode->attach(this);
        // We need to create enough LOD buckets to deal with the highest LOD
        // we encountered in all the meshes queued
        for (Nui16 lod = 0; lod < mLodScheme->getCount(); ++lod)
        {
            LODBatch* lodBucket = N_new LODBatch(this, lod);
            mLODBatchList.push_back(lodBucket);
            // Now iterate over the meshes and assign to LODs
            // LOD bucket will pick the right LOD to use
            MeshRefList::iterator qi, qiend = mMeshRefList.end();
            for (qi = mMeshRefList.begin(); qi != qiend; ++qi)
            {
                lodBucket->add(*qi, lod);
            }
            // now build
            lodBucket->build(stencilShadow);
        }
    }
    //--------------------------------------------------------------------------
    FactoryID StaticSpace::BatchObj::getFactoryID() const
    {
        return N_FACTORY_StaticSpace;
    }
    //--------------------------------------------------------------------------
    void StaticSpace::BatchObj::_notify(Camera * cam)
    {
        mCamera = cam;
        mSquaredViewDepth = mSpaceNode->distanceSquared(cam->getExtLod());
        if (mLodScheme)
        {
            mLodValue = mLodScheme->getValue(this, cam);
            mLodIdx = mLodScheme->get(mLodScheme->getDest(mLodValue));
        }
    }
    //--------------------------------------------------------------------------
    const AABox & StaticSpace::BatchObj::getAABB() const
    {
        return mAABB;
    }
    //--------------------------------------------------------------------------
    NIIf StaticSpace::BatchObj::getCenterRange() const
    {
        return mBoundRange;
    }
    //--------------------------------------------------------------------------
    void StaticSpace::BatchObj::queue(RenderQueue * queue)
    {
        mLODBatchList[mLodIdx]->queue(queue, mRenderGroup, mLodValue);
    }
    //---------------------------------------------------------------------
    void StaticSpace::BatchObj::query(GeometryQuery * query)
    {
        LODBatchList::iterator i, iend = mLODBatchList.end();
        for (i = mLODBatchList.begin(); i != iend; ++i)
        {
            (*i)->query(query);
        }
    }
    //--------------------------------------------------------------------------
    bool StaticSpace::BatchObj::isVisible() const
    {
        if(!mVisible || mInvisible)
            return false;

        SpaceManager * sm = N_Engine().getProcessSpace();
        if(sm && !(mVisibleMark & sm->getEnvVisibleMask()))
            return false;

        return true;
    }
    //--------------------------------------------------------------------------
    const StaticSpace::LODBatchList & StaticSpace::BatchObj::getLODList() const
    {
        return mLODBatchList;
    }
    //---------------------------------------------------------------------
    const ShadowVolumeList & StaticSpace::BatchObj::getVolumeList(
        const Light * srclight, NIIf destdis, Nmark flags, IndexBuffer * destindex)
    {
        Vector4f lightPos = srclight->getSpace4();
        Matrix4f world2Obj = mSpaceNode->getSpaceMatrix().inverseAffine();
        lightPos = world2Obj.affine(lightPos);

        LODBatch::ShadowVolume * sv = mLODBatchList[mLodIdx]->mShadowVolume;
        sv->updateShadowVolume(lightPos, destindex, destdis, flags);
        GeoEdget * edgeList = sv->mEdgeList;
        ShadowVolumeList & shadowRendList = sv->mShadowVolumeList;

        // Calc triangle light facing
        edgeList->updateView(lightPos);

        // Generate indexes and update renderables
        generate(edgeList, destindex, srclight, flags, shadowRendList);

        return shadowRendList;
    }
    //--------------------------------------------------------------------------
    NIIf StaticSpace::BatchObj::getExtrudeDistance(const Light * l) const
    {
        if(mSpaceNode)
        {
            Vector3f diff = mSpaceNode->getSpacePos() - const_cast<Light *>(l)->getSpacePos();
            return l->getRange() - diff.length();
        }

        return 0;
    }
    //--------------------------------------------------------------------------
    GeoEdget * StaticSpace::BatchObj::getEdgeList()
    {
        return mLODBatchList[mLodIdx]->mShadowVolume->mEdgeList;
    }
    //--------------------------------------------------------------------------
    bool StaticSpace::BatchObj::isEdgeExist(void)
    {
        return getEdgeList() != 0;
    }
    //--------------------------------------------------------------------------
    const AABox & StaticSpace::BatchObj::getFrontCapAABB() const
    {
        return getSpaceAABB();
    }
    //--------------------------------------------------------------------------
    /*void StaticSpace::BatchObj::dump(Nofstream & of) const
    {
        of << _I18n("BatchObj ") << mRegionID << std::endl;
        of << "--------------------------" << std::endl;
        of << _I18n("Centre: ") << mCentre << std::endl;
        of << _I18n("Local AABB: ") << mAABB << std::endl;
        of << _I18n("Bounding radius: ") << mBoundRange << std::endl;
        of << _I18n("Number of LODs: ") << mLODBatchList.size() << std::endl;

        for(LODBatchList::const_iterator i = mLODBatchList.begin(); i != mLODBatchList.end(); ++i)
        {
            (*i)->dump(of);
        }
        of << "--------------------------" << std::endl;
    }*/
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // LODBatch
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    StaticSpace::LODBatch::LODBatch(BatchObj * obj, Nui16 lod) :
        mBatchObj(obj),
        mLod(lod),
        mShadowVolume(0)
    {
    }
    //--------------------------------------------------------------------------
    StaticSpace::LODBatch::~LODBatch()
    {
        if(mShadowVolume)
            delete mShadowVolume;
        MaterialBatchList::iterator i, iend = mMaterialBatchList.end();
        for(i = mMaterialBatchList.begin();i != iend;++i)
        {
            N_delete i->second;
        }
        mMaterialBatchList.clear();
        GeometryList::iterator j, jend = mGeometryList.end();
        for(j = mGeometryList.begin();j != jend;++j)
        {
            N_delete *j;
        }
        mGeometryList.clear();
    }
    //--------------------------------------------------------------------------
    void StaticSpace::LODBatch::add(MeshRef * ref, Nui16 atLod)
    {
        Geometry* q = N_new Geometry();
        mGeometryList.push_back(q);
        q->position = ref->position;
        q->orientation = ref->orientation;
        q->scale = ref->scale;
        if (ref->mLodList->size() > atLod)
        {
            // This mMesh has enough lods, use the right one
            q->geometry = &(*ref->mLodList)[atLod];
        }
        else
        {
            // Not enough lods, use the lowest one we have
            q->geometry = &(*ref->mLodList)[ref->mLodList->size() - 1];
        }
        // Locate a material bucket
        MaterialBatch* mbucket = 0;
        MaterialBatchList::iterator m = mMaterialBatchList.find(ref->materialName);
        if (m != mMaterialBatchList.end())
        {
            mbucket = m->second;
        }
        else
        {
            mbucket = N_new MaterialBatch(this, ref->materialName);
            mMaterialBatchList[ref->materialName] = mbucket;
        }
        mbucket->add(q);
    }
    //--------------------------------------------------------------------------
    void StaticSpace::LODBatch::build(bool stencilShadow)
    {
        EdgeListBuilder eb;
        NCount vdidx = 0;
        if (stencilShadow)
        {
            mShadowVolume = new LODBatch::ShadowVolume(this);
        }
        MaterialBatchList::iterator i, iend = mMaterialBatchList.end();
        for(i = mMaterialBatchList.begin();i != iend; ++i)
        {
            MaterialBatch * mat = i->second;

            mat->build(stencilShadow);
            if(stencilShadow)
            {
                ShaderFusion * t = mat->getMaterial()->getLOD();
                if(t)
                {
                    ShaderCh * p = t->get(0);
                    if(p)
                    {
                        if(p->getProgram().isExist(ShaderChProgram::ST_VS))
                        {
                            mShadowVolume->mVertexProgram = true;
                        }
                    }
                }
                const GeometryBatchList & geomIt = mat->getGeometryList();
                GeometryBatchList::const_iterator z, zend = geomIt.end();
                for(z = geomIt.begin(); z != zend; ++z)
                {
                    GeometryBatch * geom = *z;

                    // Check we're dealing with 16-bit indexes here
                    // Since stencil shadows can only deal with 16-bit
                    // More than that and stencil is probably too CPU-heavy
                    // in any case
                    N_assert(geom->getIndexData()->mBuffer->getUnitSize() == 16,
                        "Only 16-bit indexes allowed when using stencil shadows");
                    eb.addVertex(geom->getVertexData());
                    eb.addIndex(geom->getIndexData(), vdidx++);
                }
            }
        }
        if(stencilShadow)
        {
            mShadowVolume->mEdgeList = eb.build();
        }
    }
    //--------------------------------------------------------------------------
    void StaticSpace::LODBatch::queue(RenderQueue * queue, Nui16 group, NIIf lodValue)
    {
        MaterialBatchList::iterator i, iend = mMaterialBatchList.end();
        for(i = mMaterialBatchList.begin(); i != iend; ++i)
        {
            i->second->queue(queue, group, lodValue);
        }
    }
    //--------------------------------------------------------------------------
    const StaticSpace::LODBatch::MaterialBatchList & StaticSpace::LODBatch::getMaterialList() const
    {
        return mMaterialBatchList;
    }
    //--------------------------------------------------------------------------
    /*void StaticSpace::LODBatch::dump(Nofstream & of) const
    {
        of << _I18n("LOD Bucket ") << mLod << std::endl;
        of << "------------------" << std::endl;
        of << _I18n("Lod Value: ") << mLodValue << std::endl;
        of << _I18n("Number of Materials: ") << mMaterialBatchList.size() << std::endl;
        MaterialBatchList::const_iterator i, iend = mMaterialBatchList.end();
        for(i = mMaterialBatchList.begin(); i != iend; ++i)
        {
            i->second->dump(of);
        }
        of << "------------------" << std::endl;
    }*/
    //---------------------------------------------------------------------
    void StaticSpace::LODBatch::query(GeometryQuery * query)
    {
        MaterialBatchList::const_iterator i, iend = mMaterialBatchList.end();
        for(i = mMaterialBatchList.begin(); i != iend; ++i)
        {
            i->second->query(query);
        }
    }
    //---------------------------------------------------------------------
    void StaticSpace::LODBatch::ShadowVolume::updateShadowVolume(const Vector4f & lightPos,
        IndexBuffer * idxbuf, NIIf extrudeDis, Nmark flags)
    {
        N_assert(idxbuf, "Only external index buffers are supported right now");
        N_assert(idxbuf->getUnitSize() == 16, "Only 16-bit indexes supported for now");
        N_assert(mEdgeList, "error");

        bool init = mShadowVolumeList.empty();

        if(init)
            mShadowVolumeList.resize(mEdgeList->mGeoList.size());

        GeoEdget::EdgeGroupList::iterator egi = mEdgeList->mGeoList.begin();
        ShadowVolumeList::iterator si, siend = mShadowVolumeList.end();
        for(si = mShadowVolumeList.begin(); si != siend; ++si, ++egi)
        {
            if(init)
            {
                // Create a new renderable, create a separate light cap if
                // we're using a vertex program (either for this model, or
                // for extruding the shadow volume) since otherwise we can
                // get depth-fighting on the light cap

                *si = N_new LODBucketShadowVolume(mLODBatch, idxbuf, egi->mVertex,
                    mVertexProgram || (flags & ShadowObj::VT_Software) == 0);
            }
            VertexBuffer * posbuf = static_cast<LODBucketShadowVolume *>(*si)->getPositionBuffer();
            if(flags & ShadowObj::VT_Software)
            {
                ShadowObj::extrude(posbuf, egi->mVertex->mCount, lightPos, extrudeDis);
            }
        }
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // MaterialBatch
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    StaticSpace::MaterialBatch::MaterialBatch(LODBatch * parent, ResourceID rid) :
        mLODBatch(parent),
        mMaterialID(rid),
        mShaderFusion(0)
    {
    }
    //--------------------------------------------------------------------------
    StaticSpace::MaterialBatch::~MaterialBatch()
    {
        GeometryBatchList::iterator i, iend = mGeometryBatchList.end();
        for(i = mGeometryBatchList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mGeometryBatchList.clear();
    }
    //--------------------------------------------------------------------------
    void StaticSpace::MaterialBatch::add(Geometry * geo)
    {
        bool newBucket = true;
        String formatString = getGeometryFormatString(geo->geometry);
        GeoBatchMarkList::iterator gi = mGeoBatchMarkList.find(formatString);
        if(gi != mGeoBatchMarkList.end())
        {
            // Found existing geometry, try to assign
            newBucket = !gi->second->add(geo);
            // Note that this bucket will be replaced as the 'current'
            // for this format string below since it's out of space
        }

        if(newBucket)
        {
            GeometryBatch * batch = N_new GeometryBatch(this, geo->geometry->mVertex, geo->geometry->mIndex);
            mGeometryBatchList.push_back(batch);
            mGeoBatchMarkList[formatString] = batch;
            if(!batch->add(geo))
            {
                N_EXCEPT(Internal, _I18n("Somehow we couldn't fit the requested geometry even in a brand new GeometryBatch!! Must be a bug, please report."));
            }
        }
    }
    //--------------------------------------------------------------------------
    void StaticSpace::MaterialBatch::build(bool stencilShadow)
    {
        mShaderFusion = 0;
        mMaterial = static_cast<Material *>(N_Only(Material).get(mMaterialID));
        if(mMaterial == 0)
        {
            N_EXCEPT(NotExist, _I18n("Material '") + N_conv(mMaterialID) + _I18n("' not found."));
        }
        mMaterial->load();
        mMaterial->ref(false);

        GeometryBatchList::iterator i, iend = mGeometryBatchList.end();
        for(i = mGeometryBatchList.begin(); i != iend; ++i)
        {
            (*i)->build(stencilShadow);
        }
    }
    //--------------------------------------------------------------------------
    void StaticSpace::MaterialBatch::queue(RenderQueue * queue, Nui16 group, NIIf lodValue)
    {
        BatchObj * region = mLODBatch->getBatchObj();
        const MaterialLodScheme * scheme = mMaterial->getLodScheme();

        // If material strategy doesn't match, recompute lod value with correct strategy
        //if(scheme != region->mLodScheme)
        lodValue = scheme->getValue(region, region->mCamera);

        // Determine the current material technique
        mShaderFusion = mMaterial->getLOD(0, scheme->get(scheme->getDest(lodValue)));
        GeometryBatchList::iterator i, iend =  mGeometryBatchList.end();
        for(i = mGeometryBatchList.begin(); i != iend; ++i)
        {
            queue->add(*i, group);
        }
    }
    //--------------------------------------------------------------------------
    const StaticSpace::GeometryBatchList & StaticSpace::MaterialBatch::getGeometryList() const
    {
        return mGeometryBatchList;
    }
    //--------------------------------------------------------------------------
    /*void StaticSpace::MaterialBatch::dump(Nofstream & of) const
    {
        of << _I18n("Material Bucket ") << mMaterialName << std::endl;
        of << "--------------------------------------------------" << std::endl;
        of << _I18n("Geometry buckets: ") << mGeometryBatchList.size() << std::endl;
        for (GeometryBatchList::const_iterator i = mGeometryBatchList.begin();
            i != mGeometryBatchList.end(); ++i)
        {
            (*i)->dump(of);
        }
        of << "--------------------------------------------------" << std::endl;
    }*/
    //---------------------------------------------------------------------
    void StaticSpace::MaterialBatch::query(GeometryQuery * query)
    {
        GeometryBatchList::const_iterator i, iend = mGeometryBatchList.end();
        for (i = mGeometryBatchList.begin(); i != iend; ++i)
        {
            (*i)->setGeometryLod(mLODBatch->getLod());
            query->query(*i);
        }
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // StaticSpace
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    StaticSpace::GeometryBatch::GeometryBatch(MaterialBatch * batch, const VertexData * vdata, const IndexData * idata) :
        mMaterialBatch(batch)
            //mFormatString(formatString)
    {
        // Clone the structure from the example
        mVertexData = vdata->clone(false);
        mIndexData = idata->clone(false);
        mVertexData->mCount = 0;
        mVertexData->mOffset = 0;
        mIndexData->mCount = 0;
        mIndexData->mOffset = 0;
        if(idata->mBuffer->getUnitSize() == 32)
        {
            mIdx32 = true;
        }
        else
        {
            mIdx32 = false;
        }

        // Check to see if we have blend indices / blend weights
        // remove them if so, they can try to blend non-existent bones!
        const VertexUnit * blendIndices = mVertexData->get(VT_Matrix_Index);
        const VertexUnit * blendWeights = mVertexData->get(VT_Matrix_Weight);
        if (blendIndices && blendWeights)
        {
            N_assert(blendIndices->mSource == blendWeights->mSource,
                "Blend indices and weights should be in the same buffer");
            // Get the source
            Nui16 source = blendIndices->mSource;
            N_assert(blendIndices->getSize() + blendWeights->getSize() ==
                mVertexData->getAttach(source)->getUnitSize(),
                    "Blend indices and blend buffers should have buffer to themselves!");
            // Unset the buffer
            mVertexData->detach(source);
            // Remove the elements
            mVertexData->remove(VT_Matrix_Index);
            mVertexData->remove(VT_Matrix_Weight);
            mVertexData->opt();
        }
    }
    //--------------------------------------------------------------------------
    StaticSpace::GeometryBatch::~GeometryBatch()
    {
        N_delete mVertexData;
        N_delete mIndexData;
    }
    //--------------------------------------------------------------------------
    const Material * StaticSpace::GeometryBatch::getMaterial() const
    {
        return mMaterialBatch->getMaterial();
    }
    //--------------------------------------------------------------------------
    ShaderFusion * StaticSpace::GeometryBatch::getShaderFusion() const
    {
        return mMaterialBatch->getShaderFusion();
    }
    //--------------------------------------------------------------------------
    void StaticSpace::GeometryBatch::getGeometry(GeometryRaw & op) const
    {
        op.mIndex = mIndexData;
        op.mVertex = mVertexData;
        op.mType = GeometryRaw::OT_TRIANGLE_LIST;
        op.add(GRT_Index);
    }
    //--------------------------------------------------------------------------
    void StaticSpace::GeometryBatch::getMatrix(Matrix4f * out, NCount & count) const
    {
        // Should be the identity transform, but lets allow transformation of the
        // nodes the regions are attached to for kicks
        *out = mMaterialBatch->getLODBatch()->getBatchObj()->getSpaceMatrix();
        count = 1;
    }
    //--------------------------------------------------------------------------
    NIIf StaticSpace::GeometryBatch::distanceSquared(const Camera * cam) const
    {
        const BatchObj *region = mMaterialBatch->getLODBatch()->getBatchObj();
        if (cam == region->mCamera)
            return region->mSquaredViewDepth;
        else
            return region->getSpaceNode()->distanceSquared(cam->getExtLod());
    }
    //--------------------------------------------------------------------------
    void StaticSpace::GeometryBatch::getLight(LightList & ll) const
    {
        return mMaterialBatch->getLODBatch()->getBatchObj()->query(ll);
    }
    //--------------------------------------------------------------------------
    bool StaticSpace::GeometryBatch::isCastShadow() const
    {
        return mMaterialBatch->getLODBatch()->getBatchObj()->isCastShadow();
    }
    //--------------------------------------------------------------------------
    bool StaticSpace::GeometryBatch::add(Geometry * geo)
    {
        // Do we have enough space?
        // -2 first to avoid overflow (-1 to adjust count to index, -1 to ensure
        // no overflow at 32 bits and use >= instead of >)
        if((mVertexData->mCount - 2 + geo->geometry->mVertex->mCount) >= (mIdx32 ? 0xFFFFFFFF : 0xFFFF))
        {
            return false;
        }

        mGeometryList.push_back(geo);
        mVertexData->mCount += geo->geometry->mVertex->mCount;
        mIndexData->mCount += geo->geometry->mIndex->mCount;
        return true;
    }
    //--------------------------------------------------------------------------
    void StaticSpace::GeometryBatch::build(bool stencilShadow)
    {
        NCount mtmp;
        if(mIdx32)
            mtmp = 32;
        else
            mtmp = 16;
        // create index buffer, and lock
        N_Only(GBuffer).create(mIndexData->mBuffer, mtmp, mIndexData->mCount, Buffer::M_NORMAL );
        Nui32 * p32Dest = 0;
        Nui16 * p16Dest = 0;
        if(mtmp == 32)
        {
            p32Dest = (Nui32 *)mIndexData->mBuffer->lock(Buffer::MM_WHOLE);
        }
        else
        {
            p16Dest = (Nui16 *)mIndexData->mBuffer->lock(Buffer::MM_WHOLE);
        }
        // create all vertex buffers, and lock
        Nui16 b;
        Nui16 posBufferIdx = mVertexData->get(VT_Pos)->mSource;

        vector<Nui8 *>::type destBufferLocks;
        vector<VertexUnitList>::type bufferElements;
        for(b = 0; b < mVertexData->getAttachCount(); ++b)
        {
            NCount vertexCount = mVertexData->mCount;
            // Need to NIId the vertex count for the position buffer
            // if we're doing stencil shadows
            if(stencilShadow && b == posBufferIdx)
            {
                vertexCount = vertexCount * 2;
                N_assert(vertexCount <= (mIdx32 ? 0xFFFFFFFF : 0xFFFF),
                    "Index range exceeded when using stencil shadows, consider "
                    "reducing your region size or reducing poly count");
            }
            VertexUnitList tunits;
            VertexBuffer * vbuf;
            N_Only(GBuffer).create(vbuf, mVertexData->getUnitSize(b), vertexCount, Buffer::M_NORMAL );
            mVertexData->attach(b, vbuf);
            Nui8 * pLock = (Nui8 *)vbuf->lock(Buffer::MM_WHOLE);
            destBufferLocks.push_back(pLock);
            mVertexData->getUnits(b, tunits);
            // Pre-cache vertex elements per buffer
            bufferElements.push_back(tunits);
        }

        // Iterate over the geometry items
        NCount indexOffset = 0;
        GeometryList::iterator gi, giend = mGeometryList.end();
        Vector3f regionCentre = mMaterialBatch->getLODBatch()->getBatchObj()->getCentre();
        for (gi = mGeometryList.begin(); gi != giend; ++gi)
        {
            Geometry * geom = *gi;
            // Copy indexes across with offset
            IndexData * srcIdxData = geom->geometry->mIndex;
            if (mIdx32)
            {
                // Lock source indexes
                Nui32 * pSrc = (Nui32 *)srcIdxData->mBuffer->lock(srcIdxData->mOffset, 
                    srcIdxData->mCount * srcIdxData->mBuffer->getUnitSize(), Buffer::MM_READ);

                copyIndexes(pSrc, p32Dest, srcIdxData->mCount, indexOffset);
                p32Dest += srcIdxData->mCount;
                srcIdxData->mBuffer->unlock();
            }
            else
            {
                // Lock source indexes
                Nui16 * pSrc = (Nui16 *)srcIdxData->mBuffer->lock(srcIdxData->mOffset, 
                    srcIdxData->mCount * srcIdxData->mBuffer->getUnitSize(), Buffer::MM_READ);

                copyIndexes(pSrc, p16Dest, srcIdxData->mCount, indexOffset);
                p16Dest += srcIdxData->mCount;
                srcIdxData->mBuffer->unlock();
            }

            // Now deal with vertex buffers
            // we can rely on buffer counts / formats being the same
            VertexData * srcVData = geom->geometry->mVertex;
            for(b = 0; b < mVertexData->getAttachCount(); ++b)
            {
                // lock source
                VertexBuffer * srcBuf = srcVData->getAttach(b);
                Nui8 * pSrcBase = (Nui8 *)srcBuf->lock(Buffer::MM_READ);
                // Get buffer lock pointer, we'll update this later
                Nui8 * pDstBase = destBufferLocks[b];
                NCount bufInc = srcBuf->getUnitSize();

                // Iterate over vertices
                NIIf *pSrcReal, *pDstReal;
                Vector3f tmp;
                for(NCount v = 0; v < srcVData->mCount; ++v)
                {
                    // Iterate over vertex elements
                    VertexUnitList & elems = bufferElements[b];
                    VertexUnitList::iterator ei, eiend = elems.end();
                    for (ei = elems.begin(); ei != eiend; ++ei)
                    {
                        VertexUnit & elem = *ei;
                        elem.getData(pSrcBase, pSrcReal);
                        elem.getData(pDstBase, pDstReal);
                        switch(elem.mVType)
                        {
                        case VT_Pos:
                            tmp.x = *pSrcReal++;
                            tmp.y = *pSrcReal++;
                            tmp.z = *pSrcReal++;
                            // transform
                            tmp = (geom->orientation * (tmp * geom->scale)) + geom->position;
                            // Adjust for region centre
                            tmp -= regionCentre;
                            *pDstReal++ = tmp.x;
                            *pDstReal++ = tmp.y;
                            *pDstReal++ = tmp.z;
                            break;
                        case VT_Normals:
                        case VT_Tangent:
                        case VT_Binormal:
                            tmp.x = *pSrcReal++;
                            tmp.y = *pSrcReal++;
                            tmp.z = *pSrcReal++;
                            // scale (invert)
                            tmp = tmp / geom->scale;
                            tmp.normalise();
                            // rotation
                            tmp = geom->orientation * tmp;
                            *pDstReal++ = tmp.x;
                            *pDstReal++ = tmp.y;
                            *pDstReal++ = tmp.z;
                            // copy parity for tangent.
                            if(elem.mType == DT_Float4x)
                                *pDstReal = *pSrcReal;
                            break;
                        default:
                            // just raw copy
                            memcpy(pDstReal, pSrcReal, DataTypeUtil::getSize(elem.mType));
                            break;
                        };
                    }
                    // Increment both pointers
                    pDstBase += bufInc;
                    pSrcBase += bufInc;
                }
                // Update pointer
                destBufferLocks[b] = pDstBase;
                srcBuf->unlock();
            }
            indexOffset += geom->geometry->mVertex->mCount;
        }
        // Unlock everything
        mIndexData->mBuffer->unlock();
        for(b = 0; b < mVertexData->getAttachCount(); ++b)
        {
            mVertexData->getAttach(b)->unlock();
        }

        // If we're dealing with stencil shadows, copy the position data from
        // the early half of the buffer to the latter part
        if(stencilShadow)
        {
            VertexBuffer * buf = mVertexData->getAttach(posBufferIdx);
            void* pSrc = buf->lock(Buffer::MM_READ | Buffer::MM_WRITE);
            // Point dest at second half (remember vertexcount is original count)
            void* pDest = static_cast<Nui8*>(pSrc) + buf->getUnitSize() * mVertexData->mCount;
            memcpy(pDest, pSrc, buf->getUnitSize() * mVertexData->mCount);
            buf->unlock();

            // Also set up hardware W buffer if appropriate
            RenderSys * rend = N_Engine().getRender();
            if(rend && rend->getFeature()->isSupport(GF_Program_Vertex))
            {
                N_Only(GBuffer).create(buf, sizeof(NIIf), mVertexData->mCount * 2, Buffer::M_NORMAL );
                // Fill the first half with 1.0, second half with 0.0
                NIIf * pW = static_cast<NIIf *>(buf->lock(Buffer::MM_WHOLE));
                NCount v;
                for(v = 0; v < mVertexData->mCount; ++v)
                {
                    *pW++ = 1.0f;
                }
                for(v = 0; v < mVertexData->mCount; ++v)
                {
                    *pW++ = 0.0f;
                }
                buf->unlock();
                mVertexData->setExt(buf);
            }
        }
    }
    //--------------------------------------------------------------------------
    /*void StaticSpace::GeometryBatch::dump(Nofstream & of) const
    {
        of << _I18n("Geometry Bucket") << std::endl;
        of << "---------------" << std::endl;
        of << _I18n("Format string: ") << mFormatString << std::endl;
        of << _I18n("Geometry items: ") << mGeometryList.size() << std::endl;
        of << _I18n("Vertex count: ") << mVertexData->mCount << std::endl;
        of << _I18n("Index count: ") << mIndexData->mCount << std::endl;
        of << "---------------" << std::endl;
    }*/
    //--------------------------------------------------------------------------
}