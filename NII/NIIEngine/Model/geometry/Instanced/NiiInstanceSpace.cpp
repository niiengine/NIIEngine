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
#include "NiiInstanceSpace.h"
#include "NiiGeometrySpace.h"
#include "NiiException.h"
#include "NiiSkeleton.h"
#include "NiiMesh.h"
#include "NiiMeshLodScheme.h"
#include "NiiMaterialLodScheme.h"
#include "NiiMaterialManager.h"
#include "NiiLogManager.h"
#include "NiiEdgeListBuilder.h"
#include "NiiGBufferManager.h"
#include "NiiGeometryObj.h"
#include "NiiNodeFusion.h"
#include "NiiEngine.h"
#include "NiiStrConv.h"

namespace NII
{
    //--------------------------------------------------------------------------
    #define packIndex(x, y, z) (x + (y << 10) + (z << 20))
    //--------------------------------------------------------------------------
    typedef map<NCount, NCount>::type IndexRemap;
    //--------------------------------------------------------------------------
    /// 计算出一个索引缓存使用的顶点和计算一个重新映射找出一个仅包含那些顶点的顶点缓存的方法
    template <typename T> void
        buildIndexRemap(T * pBuffer, NCount numIndexes, IndexRemap & remap)
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
    //--------------------------------------------------------------------------
    /// 基于一个重新映射改变索引的方法
    template <typename T> void remapIndexes(T * src, T * dst, const IndexRemap & remap,
        NCount numIndexes)
    {
        for (NCount i = 0; i < numIndexes; ++i)
        {
            // look up original and map to target
            IndexRemap::const_iterator ix = remap.find(*src++);
            N_assert(ix != remap.end(), "error");
            *dst++ = static_cast<T>(ix->second);
        }
    }
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
    //--------------------------------------------------------------------------
    class _EngineInner InstanceSpace::OptMeshGeo : public BatchObjectAlloc
    {
    public:
        OptMeshGeo(VertexData * vdata, IndexData * idata) :
            mVertex(vdata), mIndex(idata) {}
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

        VertexData * mVertex;
        IndexData * mIndex;
    };
    //--------------------------------------------------------------------------
    struct InstanceSpace::OptMeshGeoRef
    {
        VertexData * mVertex;
        IndexData * mIndex;
    };
    //--------------------------------------------------------------------------
    struct InstanceSpace::MeshRef : public BatchObjectAlloc
    {
        SubMesh * submesh;
        OptMeshGeoRefList * mLodList;
        ResourceID materialName;
        Vector3f position;
        Quaternion orientation;
        Vector3f scale;
        AABox worldBounds;
        Nui32 ID;
    };
    //--------------------------------------------------------------------------
    struct InstanceSpace::Geometry : public BatchObjectAlloc
    {
        OptMeshGeoRef * geometry;
        Vector3f position;
        Quaternion orientation;
        Vector3f scale;
        Nui32 ID;
    };
    //--------------------------------------------------------------------------
    String getGeometryFormatString2(InstanceSpace::OptMeshGeoRef * geom)
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
        const VertexUnitList & elemList = geom->mVertex->getUnits();
        VertexUnitList::const_iterator ei, eiend = elemList.end();
        for (ei = elemList.begin(); ei != eiend; ++ei)
        {
            const VertexUnit & elem = *ei;
            str << elem.mSource << "|";
            str << elem.mVType << "|";
            str << elem.mType << "|";
        }

        return str.str();
    }
    //--------------------------------------------------------------------------
    #define BatchInstance_RANGE 1024
    #define BatchInstance_HALF_RANGE 512
    #define BatchInstance_MAX_INDEX 511
    #define BatchInstance_MIN_INDEX -512
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // InstanceSpace
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    InstanceSpace::InstanceSpace(SpaceManager * owner, SpaceID sid):
        mManager(owner),
        mID(sid),
        mVisibleDistance(0.0f),
        mBatchSize(Vector3f(1000, 1000, 1000)),
        mOrigin(Vector3f(0, 0, 0)),
        mRenderGroup(RQG_Unknow),
        mObjectCount(0),
        mSkeletonFusion(0),
        mCastShadow(false),
        mVisible(true),
        mBuilt(false)
    {
        mSkeleton = 0;
    }
    //--------------------------------------------------------------------------
    InstanceSpace::~InstanceSpace()
    {
        removeAll();
        if(mSkeletonFusion)
            N_delete mSkeletonFusion;
    }
    //--------------------------------------------------------------------------
    InstanceSpace::BatchObj * InstanceSpace::getBatchObj(const AABox & bounds)
    {
        if (bounds.isNull())
            return 0;

        const Vector3f min = bounds.getMin();
        const Vector3f max = bounds.getMax();

        // Get the min and max BatchObj indexes
        Nui16 minx, miny, minz;
        Nui16 maxx, maxy, maxz;
        getBatchIndex(min, minx, miny, minz);
        getBatchIndex(max, maxx, maxy, maxz);
        NIIf maxVolume = 0.0f;
        Nui16 finalx =0 , finaly = 0, finalz = 0;
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
    NIIf InstanceSpace::getVolume(const AABox & box, Nui16 x, Nui16 y, Nui16 z)
    {
        // Get bounds of indexed BatchObj
        AABox BatchInstanceBounds = getBatchAABB(x, y, z);
        AABox intersectBox = BatchInstanceBounds.intersection(box);
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
    AABox InstanceSpace::getBatchAABB(Nui16 x, Nui16 y, Nui16 z)
    {
        Vector3f min(
            ((NIIf)x - BatchInstance_HALF_RANGE) * mBatchSize.x + mOrigin.x,
            ((NIIf)y - BatchInstance_HALF_RANGE) * mBatchSize.y + mOrigin.y,
            ((NIIf)z - BatchInstance_HALF_RANGE) * mBatchSize.z + mOrigin.z
            );
        Vector3f max = min + mBatchSize;
        return AABox(min, max);
    }
    //--------------------------------------------------------------------------
    Vector3f InstanceSpace::getBatchCentre(Nui16 x, Nui16 y, Nui16 z)
    {
        Vector3f temp = mBatchSize * 0.5;
        return Vector3f(
            ((NIIf)x - BatchInstance_HALF_RANGE) * mBatchSize.x + mOrigin.x + temp.x,
            ((NIIf)y - BatchInstance_HALF_RANGE) * mBatchSize.y + mOrigin.y + temp.y,
            ((NIIf)z - BatchInstance_HALF_RANGE) * mBatchSize.z + mOrigin.z + temp.z
            );
    }
    //--------------------------------------------------------------------------
    InstanceSpace::BatchObj * InstanceSpace::getBatchObj(Nui16 x, Nui16 y, Nui16 z)
    {
        Nui32 index = packIndex(x, y, z);
        BatchObj * ret = 0;
        BatchObjectList::iterator i = mBatchObjectList.find(index);
        if (i != mBatchObjectList.end())
        {
            ret = i->second;
        }
        if (!ret)
        {
            // Make a name
            //StringStream str;
            //str << mName << ":" << index;
            // Calculate the BatchObj centre
            Vector3f centre(0,0,0);// = getBatchCentre(x, y, z);
            ret = N_new BatchObj(this, index, mManager, index/*, centre*/);
            mManager->addSpace(ret);
            ret->setVisible(mVisible);
            ret->setCastShadow(mCastShadow);
            if(mRenderGroup != RQG_Unknow)
            {
                ret->setRenderGroup(mRenderGroup);
            }
            mBatchObjectList[index] = ret;
        }
        return ret;
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::getBatchIndex(const Vector3f& point, Nui16 & x, Nui16 & y, Nui16 & z)
    {
        // Scale the point into multiples of BatchObj and adjust for origin
        Vector3f scaledPoint = (point - mOrigin) / mBatchSize;

        // Round down to 'bottom left' point which represents the cell index
        NIIi ix = Math::IFloor(scaledPoint.x);
        NIIi iy = Math::IFloor(scaledPoint.y);
        NIIi iz = Math::IFloor(scaledPoint.z);

        // Check bounds
        if (ix < BatchInstance_MIN_INDEX || ix > BatchInstance_MAX_INDEX
            || iy < BatchInstance_MIN_INDEX || iy > BatchInstance_MAX_INDEX
            || iz < BatchInstance_MIN_INDEX || iz > BatchInstance_MAX_INDEX)
        {
            N_EXCEPT(InvalidParam, _I18n("Point out of bounds"));
        }
        // Adjust for the fact that we use unsigned values for simplicity
        // (requires less faffing about for negatives give 10-bit packing
        x = static_cast<Nui16>(ix + BatchInstance_HALF_RANGE);
        y = static_cast<Nui16>(iy + BatchInstance_HALF_RANGE);
        z = static_cast<Nui16>(iz + BatchInstance_HALF_RANGE);
    }
    //--------------------------------------------------------------------------
    InstanceSpace::BatchObj * InstanceSpace::getBatchObj(const Vector3f & point)
    {
        Nui16 x, y, z;
        getBatchIndex(point, x, y, z);
        return getBatchObj(x, y, z);
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::add(GeometrySpace * obj, const Vector3f & position,
        const Quaternion & orientation, const Vector3f & scale)
    {
        const Mesh * msh = obj->getMesh();
        if(!msh->getLodScheme()->isRelation(0))
        {
            N_Only(Log).log(_I18n("WARNING (InstanceSpace): Manual LOD is not supported. ")
                _I18n("Using only highest LOD level for mesh ") + msh->getOriginID());
        }

        if(obj->getMesh()->getSkeleton() != 0 && mSkeleton == 0)
        {
            mSkeleton = obj->getMesh()->getSkeleton();
            mSkeletonFusion = N_new SkeletonFusion(mSkeleton);
            mSkeletonFusion->load();
            mAnimationCtrl = obj->getAnimationCtrl();
        }

        // queue this entities submeshes and choice of material
        // also build the lists of geometry to be used for the source of lods
        for(NCount i = 0; i < obj->getUnitCount(); ++i)
        {
            GeometryCom * se = obj->getCom(i);
            MeshRef * q = N_new MeshRef();

            q->submesh = se->getMesh();
            q->mLodList = createRefList(q->submesh);
            q->materialName = se->getMaterialID();
            q->orientation = orientation;
            q->position = position;
            q->scale = scale;
            q->ID = mObjectCount;
            // Determine the bounds based on the highest LOD
            (*q->mLodList)[0].mVertex->calcAABB(position, orientation, scale, q->worldBounds);

            mMeshRefList.push_back(q);
        }
        mObjectCount++;
    }
    //--------------------------------------------------------------------------
    InstanceSpace::OptMeshGeoRefList * InstanceSpace::createRefList(SubMesh * sm)
    {
        MeshGeoRefList::iterator i = mMeshGeoRefList.find(sm);
        if (i != mMeshGeoRefList.end())
        {
            return i->second;
        }

        OptMeshGeoRefList * lodList = N_new_t(OptMeshGeoRefList)();
        mMeshGeoRefList[sm] = lodList;

        //Nui16 numLods = sm->getMesh()->getLodScheme()->isRelation() ?
            //sm->getMesh()->getLodCount() : 1;

        //lodList->resize(numLods);
        NCount lod, lodend = sm->getMesh()->getLodScheme()->getCount();
        for (lod = 0; lod < lodend; ++lod)
        {
            if (sm->getMesh()->getLodScheme()->isRelation(lod))
                lodList->push_back(OptMeshGeoRef());
            else
                continue;
            OptMeshGeoRef & geomLink = (*lodList)[lod];
            IndexData * lodIndexData;
            if (lod == 0)
            {
                lodIndexData = sm->getIndexData(0);
            }
            else
            {
                lodIndexData = sm->getIndexData(lod);
            }
            // Can use the original mesh geometry?
            if(sm->isSelfVertex())
            {
                if (lod == 0)
                {
                    geomLink.mVertex = sm->getVertexData();
                    geomLink.mIndex = sm->getIndexData(0);
                }
                else
                {
                    createRef(&geomLink, sm->getVertexData(), lodIndexData);
                }
            }
            else
            {
                if(sm->getMesh()->getSubCount() == 1)
                {
                    geomLink.mVertex = sm->getMesh()->getVertexData();
                    geomLink.mIndex = lodIndexData;
                }
                else
                {
                    createRef(&geomLink, sm->getMesh()->getVertexData(), lodIndexData);
                }
            }
            N_assert (geomLink.mVertex->mOffset == 0,
                "Cannot use mOffset > 0 on indexed geometry due to "
                "rendersystem incompatibilities - see the docs!");
        }
        return lodList;
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::createRef(InstanceSpace::OptMeshGeoRef * ref, 
        VertexData * vd, IndexData * id)
    {
        // Firstly we need to scan to see how many vertices are being used
        // and while we're at it, build the remap we can use later
        bool use32bitIndexes = id->mBuffer->getUnitSize() == 32;
        IndexRemap indexRemap;
        if (use32bitIndexes)
        {
            Nui32 *p32 = (Nui32 *)id->mBuffer->lock(id->mOffset,
                id->mCount * id->mBuffer->getUnitSize(), Buffer::MM_READ);
            buildIndexRemap(p32, id->mCount, indexRemap);
            id->mBuffer->unlock();
        }
        else
        {
            Nui16 *p16 = (Nui16 *)id->mBuffer->lock(id->mOffset,
                id->mCount * id->mBuffer->getUnitSize(), Buffer::MM_READ);
            buildIndexRemap(p16, id->mCount, indexRemap);
            id->mBuffer->unlock();
        }
        if(indexRemap.size() == vd->mCount)
        {
            // ha, complete usage after all
            ref->mVertex = vd;
            ref->mIndex = id;
            return;
        }

        // Create the new vertex data records
        ref->mVertex = vd->clone(false);
        // Convenience
        VertexData * newvd = ref->mVertex;
        //IndexData* newid = ref->mIndex;
        // Update the vertex count
        newvd->mCount = indexRemap.size();

        NCount numvbufs = vd->getAttachCount();
        // Copy buffers from old to new
        for(Nui16 b = 0; b < numvbufs; ++b)
        {
            // Lock old buffer
            VertexBuffer * oldBuf = vd->getAttach(b);
            // Create new buffer
            VertexBuffer * newBuf;
            N_Only(GBuffer).create(newBuf, oldBuf->getUnitSize(), indexRemap.size(), Buffer::M_NORMAL );
            // rebind
            newvd->attach(b, newBuf);

            // Copy all the elements of the buffer across, by iterating over
            // the IndexRemap which describes how to move the old vertices
            // to the new ones. By nature of the map the remap is in order of
            // indexes in the old buffer, but note that we're not guaranteed to
            // address every vertex (which is kinda why we're here)
            Nui8* pSrcBase = (Nui8*)oldBuf->lock(Buffer::MM_READ);
            Nui8* pDstBase = (Nui8*)newBuf->lock(Buffer::MM_WHOLE);
            NCount vertexSize = oldBuf->getUnitSize();
            // Buffers should be the same size
            N_assert (vertexSize == newBuf->getUnitSize(), "error");

            for (IndexRemap::iterator r = indexRemap.begin(); r != indexRemap.end(); ++r)
            {
                N_assert (r->first < oldBuf->getUnitCount(), "error");
                N_assert (r->second < newBuf->getUnitCount(), "error");

                Nui8 * pSrc = pSrcBase + r->first * vertexSize;
                Nui8 * pDst = pDstBase + r->second * vertexSize;
                memcpy(pDst, pSrc, vertexSize);
            }
            // unlock
            oldBuf->unlock();
            newBuf->unlock();
        }

        // Now create a new index buffer
        IndexBuffer * ibuf;
        N_Only(GBuffer).create(ibuf, id->mBuffer->getUnitSize(), id->mCount, Buffer::M_DEV);

        if(use32bitIndexes)
        {
            Nui32 *pSrc32 = (Nui32*)id->mBuffer->lock(id->mOffset,
                id->mCount * id->mBuffer->getUnitSize(), Buffer::MM_READ);

            Nui32 *pDst32 = (Nui32*)ibuf->lock(Buffer::MM_WHOLE);

            remapIndexes(pSrc32, pDst32, indexRemap, id->mCount);
            id->mBuffer->unlock();
            ibuf->unlock();
        }
        else
        {
            Nui16 * pSrc16 = (Nui16 *)id->mBuffer->lock(id->mOffset,
                id->mCount * id->mBuffer->getUnitSize(), Buffer::MM_READ);
            Nui16 * pDst16 = (Nui16 *)ibuf->lock(Buffer::MM_WHOLE);

            remapIndexes(pSrc16, pDst16, indexRemap, id->mCount);
            id->mBuffer->unlock();
            ibuf->unlock();
        }

        ref->mIndex = N_new IndexData();
        ref->mIndex->mOffset = 0;
        ref->mIndex->mCount = id->mCount;
        ref->mIndex->mBuffer = ibuf;

        // Store optimised geometry for deallocation later
        OptMeshGeo * optGeom = N_new OptMeshGeo(ref->mVertex, ref->mIndex);
        mOptMeshGeoList.push_back(optGeom);
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::add(const SpaceNode * node)
    {
        SpaceNode::AttachList::const_iterator obji, objiend = node->getAttachList().end();
        for(obji = node->getAttachList().begin(); obji != objiend; ++obji)
        {
            SpaceObj * mobj = obji->second;
            if (mobj->getFactoryID() == N_FACTORY_GeoSpace)
            {
                add(static_cast<GeometrySpace *>(mobj), node->getSpacePos(),
                    node->getSpaceOri(), node->getSpaceScale());
            }
        }

        PosNode::ChildList::const_iterator i, iend = node->getChildList().end();
        for(i = node->getChildList().begin(); i != iend; ++i)
        {
            const SpaceNode * newNode = static_cast<const SpaceNode *>(i->second);
            add(newNode);
        }
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::build()
    {
        destroy();

        if (!mBatchObj)
        {
            Nui32 index = 0;
            // Make a name
            //StringStream str;
            //str << mName << ":" << index;

            mBatchObj = N_new BatchObj(this, index, mManager, index);
            mManager->addSpace(mBatchObj);
            mBatchObj->setVisible(mVisible);
            mBatchObj->setCastShadow(mCastShadow);
            if (mRenderGroup != RQG_Unknow)
            {
                mBatchObj->setRenderGroup(mRenderGroup);
            }
            mBatchObjectList[index] = mBatchObj;
        }

        MeshRefList::iterator qi, qiend = mMeshRefList.end();
        for (qi = mMeshRefList.begin(); qi != qiend; ++qi)
        {
            MeshRef * ref = *qi;
            mBatchObj->add(*qi);
        }

        BatchObjectList::iterator ri, riend = mBatchObjectList.end();
        for (ri = mBatchObjectList.begin(); ri != riend; ++ri)
        {
            ri->second->build();
        }
    }
    //--------------------------------------------------------------------------
    /*void InstanceSpace::addBatchInstance()
    {
        if (!mBatchObjectList.size())
            return;

        BatchObj * lastobj = mBatchObjectList.rbegin()->second;
        if(!lastobj)
            N_EXCEPT(NotExist, _I18n("No batch instance found"));
        Nui32 index = (lastobj) ? lastobj->getID() + 1 : 0;

        BatchObj * ret = N_new BatchObj(this, 0, mManager, index);

        ret->mNode = static_cast<SpaceNode *>(ret->mManager->getOriginNode()->create(mID));
        ret->mNode->attach(this);

        mManager->addSpace(ret);
        ret->setVisible(mVisible);
        ret->setCastShadow(mCastShadow);
        mBatchObjectList[index] = ret;

        if(mRenderGroup != RQG_Unknow)
        {
            ret->setRenderGroup(mRenderGroup);
        }

        // update bounds
        AABox box(lastobj->mAABB.getMin(),lastobj->mAABB.getMax());
        ret->mAABB.merge(box);

        ret->mBoundRange = lastobj->mBoundRange ;
        //now create  news instanced objects
        BatchObj::ObjectList::const_iterator objIt, objItend = lastobj->getObjectList().end();
        for(objIt = lastobj->getObjectList().begin();objIt!= objItend; ++objIt)
        {
            InstancedObject * instancedObject = ret->isObjectExist(objIt->first);
            if(instancedObject == NULL)
            {
                if(mSkeleton == 0)
                {
                    instancedObject= N_new InstancedObject(objIt->first);
                }
                else
                {
                    instancedObject= N_new InstancedObject(objIt->first, mSkeletonFusion, mAnimationCtrl);
                }
                ret->addObject(objIt->first,instancedObject);
            }
        }

        const LODBatchList & lodIterator = lastobj->getLODList();
        LODBatchList::const_iterator j, jend = lodIterator.end();
        for(j = lodIterator.begin(); j != jend; ++j)
        {
            LODBatch * lod = *j;
            LODBatch * lodBucket = N_new LODBatch(ret, lod->getLod());

            //add the lod bucket to the BatchObj list
            //ret->updateContainers(lodBucket);
            ret->mLODBatchList.push_back(lodBucket);
            const LODBatch::MaterialBatchList & matIt = lod->getMaterialList();
            LODBatch::MaterialBatchList::const_iterator cc, ccend = matIt.end();
            for(cc = matIt.begin(); cc != ccend; ++cc)
            {
                MaterialBatch * mat = cc->second;
                //create a new material bucket
                ResourceID materialName = mat->getMaterialID();
                MaterialBatch * matBucket = N_new MaterialBatch(lodBucket, materialName);

                //add the material bucket to the lod buckets list and map
                //lodBucket->updateContainers(matBucket, materialName);
                lodBucket->mMaterialBatchList[materialName] = matBucket;
                const GeometryBatchList & geomIt = mat->getGeometryList();
                GeometryBatchList::const_iterator cc, ccend = geomIt.end();
                for(cc = geomIt.begin(); cc != ccend; ++cc)
                {
                    //get the source geometry bucket
                    GeometryBatch * geom = *cc;
                    //create a new geometry bucket
                    GeometryBatch * geomBucket = N_new GeometryBatch(matBucket, geom->getFormatString(),geom);

                    //update the material bucket map of the material bucket
                    //matBucket->updateContainers(geomBucket, geomBucket->getFormatString() );
                    matBucket->mGeoBatchMarkList[geomBucket->getFormatString()] = geomBucket;
                    matBucket->mGeometryBatchList.push_back(geomBucket);
                    //copy bounding informations
                    geomBucket->getAABB() = geom->getAABB();
                    geomBucket->setAABB(geom->getAABB());
                    //now setups the news InstancedObjects.
                    for(objIt=ret->getObjectList().begin();objIt!=ret->getObjectList().end();objIt++)
                    {
                        //get the destination IntanciedObject
                        InstancedObject * obj = objIt->second;
                        GeometryBatchList::const_iterator findIt;
                        //check if the bucket is not already in the list
                        findIt = std::find(obj->mGeometryBatchList.begin(),obj->mGeometryBatchList.end(),geomBucket);
                        if(findIt == obj->mGeometryBatchList.end())
                            obj->mGeometryBatchList.push_back(geomBucket);
                    }
                }
            }
        }
    }*/
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // BatchObj
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    /*void InstanceSpace::BatchObj::updateContainers(LODBatch * bucket)
    {
        mLODBatchList.push_back(bucket);
    }*/
    //--------------------------------------------------------------------------
    /*void InstanceSpace::LODBatch::updateContainers(MaterialBatch * bucket, ResourceID name)
    {
        mMaterialBatchList[name] = bucket;
    }*/
    //--------------------------------------------------------------------------
    /*void InstanceSpace::MaterialBatch::updateContainers(InstanceSpace::GeometryBatch * bucket, const String & format)
    {
        mGeoBatchMarkList[format] = bucket;
        mGeometryBatchList.push_back(bucket);
    }*/
    //--------------------------------------------------------------------------
    void InstanceSpace::destroy(void)
    {
        GeometryRawList::iterator it, itend = mGeometryRawList.end();
        for(it = mGeometryRawList.begin(); it != itend; ++it)
        {
            N_Only(GBuffer).destroy((*it)->mVertex);
            N_Only(GBuffer).destroy((*it)->mIndex);
        }
        mGeometryRawList.clear();

        BatchObjectList::iterator i, iend = mBatchObjectList.end();
        for (i = mBatchObjectList.begin(); i != iend; ++i)
        {
            mManager->removeSpace(i->second->getID(), i->second->getFactoryID());
            N_delete i->second;
        }
        mBatchObjectList.clear();
        mBatchObj = NULL;
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::removeAll(void)
    {
        destroy();

        MeshRefList::iterator i, iend = mMeshRefList.end();
        for (i = mMeshRefList.begin(); i != iend; ++i)
        {
            N_delete *i;

        }
        mMeshRefList.clear();

        MeshGeoRefList::iterator l, lend = mMeshGeoRefList.end();
        for (l = mMeshGeoRefList.begin(); l != lend; ++l)
        {
            N_delete_t(l->second, OptMeshGeoRefList);

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
    void InstanceSpace::setVisibleDistance(NIIf dist)
    {
        mVisibleDistance = dist;
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::setVisible(bool set)
    {
        mVisible = set;
        BatchObjectList::iterator i, iend = mBatchObjectList.end();
        for(i = mBatchObjectList.begin(); iend != iend; ++i)
        {
            i->second->setVisible(set);
        }
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::setCastShadow(bool set)
    {
        mCastShadow = set;
        BatchObjectList::iterator i, iend = mBatchObjectList.end();
        for(i = mBatchObjectList.begin(); i != iend; ++i)
        {
            i->second->setCastShadow(set);
        }
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::setBatchSize(const Vector3f & size)
    {
        mBatchSize = size;
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::setOrigin(const Vector3f & origin) 
    { 
        mOrigin = origin; 
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::setRenderGroup(Nui16 queueID)
    {
        N_assert(queueID <= RQG_Count, "Render queue out of range!");
        mRenderGroup = queueID;
        BatchObjectList::iterator i, iend = mBatchObjectList.end();
        for(i = mBatchObjectList.begin(); i != iend; ++i)
        {
            i->second->setRenderGroup(queueID);
        }
    }
    //--------------------------------------------------------------------------
    Nui16 InstanceSpace::getRenderGroup() const
    {
        return mRenderGroup;
    }
    //--------------------------------------------------------------------------
    /*void InstanceSpace::dump(const String& filename) const
    {
        Nofstream of(filename.c_str());
        of << _I18n("Static Geometry Report for ") << mName << std::endl;
        of << "-------------------------------------------------" << std::endl;
        of << _I18n("Number of queued submeshes: ") << mMeshRefList.size() << std::endl;
        of << _I18n("Number of BatchInstances: ") << mBatchObjectList.size() << std::endl;
        of << _I18n("BatchObj dimensions: ") << mBatchSize << std::endl;
        of << _I18n("Origin: ") << mOrigin << std::endl;
        of << _I18n("Max distance: ") << mVisibleDistance << std::endl;
        of << _I18n("Casts shadows?: ") << mCastShadow << std::endl;
        of << std::endl;
        for (BatchObjectList::const_iterator ri = mBatchObjectList.begin();
            ri != mBatchObjectList.end(); ++ri)
        {
            ri->second->dump(of);
        }
        of << "-------------------------------------------------" << std::endl;
    }*/
    //---------------------------------------------------------------------
    void InstanceSpace::query(GeometryQuery * query)
    {
        BatchObjectList::const_iterator ri, iend = mBatchObjectList.end();
        for(ri = mBatchObjectList.begin(); ri != iend; ++ri)
        {
            ri->second->query(query);
        }
    }
    //--------------------------------------------------------------------------
    InstanceSpace::InstancedObject::InstancedObject(Nui16 index,
        SkeletonFusion * skeleton, AnimationFusionObj * ctrl) :
            mIndex(index),
            mMatrix(Matrix4f::ZERO),
            mOrientation(Quaternion::IDENTITY),
            mScale(Vector3f::UNIT),
            mPosition(Vector3f::ZERO),
            mSkeletonFusion(skeleton),
            mBoneMatrix(NULL)
    {
        mSkeletonFusion->load();

        mBoneCount = mSkeletonFusion->getCount();
        AnimationFusionObj::AnimationFusions::iterator i, iend = ctrl->getFusions().end();
        for(i = ctrl->getFusions().begin(); i != iend; ++i)
        {
            NodeFusion * fusion = static_cast<NodeFusion *>(i->second);
            addFusion(N_new NodeFusion(mSkeletonFusion, fusion->getAnimation(),
                fusion->tell(), fusion->getDur(), fusion->getScale(), fusion->getWeight(), fusion->isEnable()));
        }
    }
    //--------------------------------------------------------------------------
    InstanceSpace::InstancedObject::InstancedObject(Nui16 index) :
        mIndex(index),
        mMatrix(Matrix4f::ZERO),
        mOrientation(Quaternion::IDENTITY),
        mScale(Vector3f::UNIT),
        mPosition(Vector3f::ZERO),
        mSkeletonFusion(0),
        mBoneMatrix(0),
        mBoneCount(0)
    {
    }
    //--------------------------------------------------------------------------
    InstanceSpace::InstancedObject::~InstancedObject()
    {
        mGeometryBatchList.clear();
        N_free(mBoneMatrix);
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::InstancedObject::setPos(Vector3f position)
    {
        mPosition = position;
        mMatrix.set(mPosition, mScale, mOrientation);
        BatchObj * batch = (*(mGeometryBatchList.begin()))->getMaterialBatch()->getLODBatch()->getBatchObj();
        batch->updateAABB();
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::InstancedObject::translate(const Vector3f & d)
    {
        mPosition += d;
        mMatrix.set(mPosition, mScale, mOrientation);
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::InstancedObject::translate(const Matrix3f & axes,const Vector3f & move)
    {
        translate(axes * move);
    }
    //--------------------------------------------------------------------------
    /*Matrix3f InstanceSpace::InstancedObject::getLocalAxes() const
    {
        Vector3f axisX = Vector3f::X;
        Vector3f axisY = Vector3f::Y;
        Vector3f axisZ = Vector3f::Z;

        axisX = mOrientation * axisX;
        axisY = mOrientation * axisY;
        axisZ = mOrientation * axisZ;

        return Matrix3f(axisX.x, axisY.x, axisZ.x,
            axisX.y, axisY.y, axisZ.y,
            axisX.z, axisY.z, axisZ.z);
    }*/
    //--------------------------------------------------------------------------
    const Vector3f & InstanceSpace::InstancedObject::getPos() const
    {
        return mPosition;
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::InstancedObject::yaw(const Radian & angle)
    {
        Quaternion q;
        q.from(angle, Vector3f::Y);
        rotate(q);
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::InstancedObject::pitch(const Radian & angle)
    {
        Quaternion q;
        q.from(angle, Vector3f::X);
        rotate(q);
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::InstancedObject::roll(const Radian&angle)
    {
        Quaternion q;
        q.from(angle, Vector3f::Z);
        rotate(q);
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::InstancedObject::setScale(const Vector3f & scale)
    {
        mScale = scale;
        mMatrix.set(mPosition, mScale, mOrientation);
    }
    //--------------------------------------------------------------------------
    const Vector3f& InstanceSpace::InstancedObject::getScale() const
    {
        return mScale;
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::InstancedObject::rotate(const Quaternion& q)
    {
        mOrientation = mOrientation * q;
        mMatrix.set(mPosition, mScale, mOrientation);
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::InstancedObject::setOri(const Quaternion & q)
    {
        mOrientation = q;
        mMatrix.set(mPosition, mScale, mOrientation);
    }
    //--------------------------------------------------------------------------
    /*void InstanceSpace::InstancedObject::setPositionAndOrientation(Vector3f p, const Quaternion & q)
    {
        mPosition = p;
        mOrientation = q;
        mMatrix.set(mPosition, mScale, mOrientation);
        BatchObj* parentBatchInstance=(*(mGeometryBatchList.begin()))->getMaterialBatch()->getLODBatch()->getBatchObj();
        parentBatchInstance->updateAABB();
    }*/
    //--------------------------------------------------------------------------
    const Quaternion & InstanceSpace::InstancedObject::getOri()
    {
        return mOrientation;
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::InstancedObject::updateAnimation()
    {
        if(mSkeletonFusion)
        {
            mSkeletonFusion->_refresh(this);
            mSkeletonFusion->update();
            const Matrix4f * src = mSkeletonFusion->getMatrix();
             // Allocate bone world matrices on demand, for better memory footprint
            // when using software animation.
            if(!mBoneMatrix)
            {
                mBoneMatrix = N_alloc_t(Matrix4f, mBoneCount);
            }

            for(NCount i = 0; i < mBoneCount; ++i)
            {
                mBoneMatrix[i] =  mMatrix * src[i];
            }
        }
    }
    //--------------------------------------------------------------------------
    const InstanceSpace::BatchObjectList & InstanceSpace::getBatchObjList() const
    {
        return mBatchObjectList;
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // BatchObj
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    InstanceSpace::BatchObj::BatchObj(InstanceSpace * parent, SpaceID sid, 
        SpaceManager * mgr, Nid bid) :
            SpaceObj(sid),
            mParent(parent),
            mManager(mgr),
            mNode(0),
            mBatchID(bid),
            mBoundRange(0.0f),
            mLodIdx(0),
            mLodScheme(0)
    {
    }
    //--------------------------------------------------------------------------
    InstanceSpace::BatchObj::~BatchObj()
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

        ObjectList::iterator o, oend = mObjectList.end();
        for(o = mObjectList.begin();o != oend; ++o)
        {
            N_delete o->second;
        }
        mObjectList.clear();
        // no need to delete queued meshes, these are managed in InstanceSpace
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::BatchObj::add(MeshRef * ref)
    {
        mMeshRefList.push_back(ref);

        MeshLodScheme * scheme = ref->submesh->getMesh()->getLodScheme();
        if(mLodScheme == 0)
        {
            mLodScheme = scheme;
        }
        else
        {
            if(mLodScheme != scheme)
                N_EXCEPT(InvalidParam, _I18n("Lod strategies do not match"));
        }

        // update lod values
        NCount lodLevels = scheme->getCount();
        N_assert(ref->mLodList->size() == lodLevels, "error");

        // update bounds
        // Transform world bounds relative to our centre
        AABox localBounds(ref->worldBounds.getMin(), ref->worldBounds.getMax());
        mAABB.merge(localBounds);
        mBoundRange = mAABB.calcRadius();
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::BatchObj::build()
    {
        mNode = static_cast<SpaceNode *>(mManager->getOriginNode()->create(mID));
        mNode->attach(this);
        // We need to create enough LOD buckets to deal with the highest LOD
        // we encountered in all the meshes queued
        for(Nui16 lod = 0; lod < mLodScheme->getCount(); ++lod)
        {
            LODBatch * batch = N_new LODBatch(this, lod);
            mLODBatchList.push_back(batch);
            // Now iterate over the meshes and assign to LODs
            // LOD bucket will pick the right LOD to use
            MeshRefList::iterator i, iend = mMeshRefList.end();
            for(i = mMeshRefList.begin(); i != iend; ++i)
            {
                batch->add(*i, lod);
            }
            batch->build();
        }
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::BatchObj::updateAABB()
    {
        AABox aabb;
        const LODBatchList & lodIterator = getLODList();
        LODBatchList::const_iterator c, cend = getLODList().end();
        for(c = getLODList().begin(); c != cend; ++c)
        {
            LODBatch * lod = *c;
            const LODBatch::MaterialBatchList & matIt = lod->getMaterialList();
            LODBatch::MaterialBatchList::const_iterator j, jend = matIt.end();
            for(j = matIt.begin(); j !=jend; ++j)
            {
                const GeometryBatchList & geomIt = j->second->getGeometryList();
                GeometryBatchList::const_iterator cc = geomIt.begin();
                if(cc != geomIt.end())
                {
                    aabb = (*cc)->getAABB();
                }
            }
        }

        ObjectList::iterator objIt = mObjectList.begin();
        ObjectList::iterator objItend = mObjectList.end();
        Vector3f vMin;
        Vector3f vMax;
        if(objIt != objItend)
        {
            vMin = objIt->second->getPos() + aabb.getMin();
            vMax = objIt->second->getPos() + aabb.getMax();

            for(; objIt!=mObjectList.end(); objIt++ )
            {
                const Vector3f &position = objIt->second->getPos();
                const Vector3f &scale    = objIt->second->getScale();

                vMin.x = std::min( vMin.x, position.x + aabb.getMin().x * scale.x );
                vMin.y = std::min( vMin.y, position.y + aabb.getMin().y * scale.y );
                vMin.z = std::min( vMin.z, position.z + aabb.getMin().z * scale.z );

                vMax.x = std::max( vMax.x, position.x + aabb.getMax().x * scale.x );
                vMax.y = std::max( vMax.y, position.y + aabb.getMax().y * scale.y );
                vMax.z = std::max( vMax.z, position.z + aabb.getMax().z * scale.z );
            }
        }
        aabb.setMin(vMin);
        aabb.setMax(vMax);
        //Now apply the bounding box
        for (c = getLODList().begin(); c != cend; ++c)
        {
            LODBatch * lod = *c;
            const LODBatch::MaterialBatchList & matIt = lod->getMaterialList();
            LODBatch::MaterialBatchList::const_iterator k, kend = matIt.end();
            for (k = matIt.begin(); k != kend; ++k)
            {
                const GeometryBatchList& geomIt = k->second->getGeometryList();
                GeometryBatchList::const_iterator cc, ccend = geomIt.end();
                for(cc = geomIt.begin(); cc != ccend; ++cc)
                {
                    (*cc)->setAABB(aabb);
                    this->mNode->updateSpaceAABB();
                    mAABB = aabb;
                }
            }
        }
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::BatchObj::addObject(Nui16 index, InstancedObject * object)
    {
        mObjectList[index] = object;
    }
    //--------------------------------------------------------------------------
    InstanceSpace::InstancedObject* InstanceSpace::BatchObj::isObjectExist(Nui16 index)
    {
        if (mObjectList.find(index)!=mObjectList.end())
            return mObjectList[index];
        return NULL;
    }
    //--------------------------------------------------------------------------
    FactoryID InstanceSpace::BatchObj::getFactoryID() const
    {
        FactoryID sType = N_FACTORY_InstanceSpace;
        return sType;
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::BatchObj::_notify(Camera * cam)
    {
        mCamera = cam;
        // Cache squared view depth for use by GeometryBatch
        mSquaredViewDepth = mSpaceNode->distanceSquared(cam->getExtLod());
        // No lod strategy set yet, skip (this indicates that there are no submeshes)
        if (mLodScheme)
        {
            mLodValue = mLodScheme->getValue(this, cam);
            mLodIdx = mLodScheme->get(mLodScheme->getDest(mLodValue));
        }
    }
    //--------------------------------------------------------------------------
    const AABox& InstanceSpace::BatchObj::getAABB(void) const
    {
        return mAABB;
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::BatchObj::setAABB(AABox & box)
    {
        mAABB = box;
    }
    //--------------------------------------------------------------------------
    NIIf InstanceSpace::BatchObj::getCenterRange(void) const
    {
        return mBoundRange;
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::BatchObj::queue(RenderQueue * queue)
    {
        ObjectList::iterator it, itend = mObjectList.end();
        for(it = mObjectList.begin(); it != itend; ++it)
        {
            it->second->updateAnimation();
        }
        mLODBatchList[mLodIdx]->queue(queue, mRenderGroup, mLodValue);
    }
    //---------------------------------------------------------------------
    void InstanceSpace::BatchObj::query(GeometryQuery * query)
    {
        LODBatchList::iterator i, iend = mLODBatchList.end();
        for(i = mLODBatchList.begin(); i != iend; ++i)
        {
            (*i)->query(query);
        }
    }
    //--------------------------------------------------------------------------
    bool InstanceSpace::BatchObj::isVisible() const
    {
        return mVisible && !mInvisible;
    }
    //--------------------------------------------------------------------------
    const InstanceSpace::LODBatchList & InstanceSpace::BatchObj::getLODList() const
    {
        return mLODBatchList;
    }
    //--------------------------------------------------------------------------
    /*void InstanceSpace::BatchObj::dump(Nofstream & of) const
    {
        of << "BatchObj " << mBatchID << std::endl;
        of << "--------------------------" << std::endl;
        of << "Local AABB: " << mAABB << std::endl;
        of << "Bounding radius: " << mBoundRange << std::endl;
        of << "Number of LODs: " << mLODBatchList.size() << std::endl;

        LODBatchList::const_iterator i, iend = mLODBatchList.end();
        for (i = mLODBatchList.begin(); i != iend; ++i)
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
    InstanceSpace::LODBatch::LODBatch(BatchObj * obj, Nui16 lod) :
        mBatchObj(obj),
        mLod(lod)
    {
    }
    //--------------------------------------------------------------------------
    InstanceSpace::LODBatch::~LODBatch()
    {
        MaterialBatchList::iterator i, iend = mMaterialBatchList.end();
        for(i = mMaterialBatchList.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
        mMaterialBatchList.clear();

        GeometryList::iterator j, jend = mGeometryList.end();
        for(j = mGeometryList.begin(); j != jend; ++j)
        {
            N_delete *j;
        }
        mGeometryList.clear();
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::LODBatch::add(MeshRef * ref, Nui16 atLod)
    {
        Geometry* q = N_new Geometry();
        mGeometryList.push_back(q);
        q->position = ref->position;
        q->orientation = ref->orientation;
        q->scale = ref->scale;
        q->ID = ref->ID;
        if (ref->mLodList->size() > atLod)
        {
            // This submesh has enough lods, use the right one
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
    void InstanceSpace::LODBatch::build()
    {
        MaterialBatchList::iterator i, iend = mMaterialBatchList.end();
        for(i = mMaterialBatchList.begin(); i != iend; ++i)
        {
            i->second->build();
        }
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::LODBatch::queue(RenderQueue * queue, GroupID group, NIIf lodValue)
    {
        MaterialBatchList::iterator i, iend =  mMaterialBatchList.end();
        for(i = mMaterialBatchList.begin(); i != iend; ++i)
        {
            i->second->queue(queue, group, lodValue);
        }
    }
    //---------------------------------------------------------------------
    void InstanceSpace::LODBatch::query(GeometryQuery * query)
    {
        MaterialBatchList::iterator i, iend =  mMaterialBatchList.end();
        for(i = mMaterialBatchList.begin(); i != iend; ++i)
        {
            i->second->query(query);
        }
    }
    //--------------------------------------------------------------------------
    const InstanceSpace::LODBatch::MaterialBatchList & InstanceSpace::LODBatch::getMaterialList() const
    {
        return mMaterialBatchList;
    }
    //--------------------------------------------------------------------------
    /*void InstanceSpace::LODBatch::dump(Nofstream & of) const
    {
        of << _I18n("LOD Bucket ") << mLod << std::endl;
        of << "------------------" << std::endl;
        of << _I18n("Lod Value: ") << mLodValue << std::endl;
        of << _I18n("Number of Materials: ") << mMaterialBatchList.size() << std::endl;
        for (MaterialBatchList::const_iterator i = mMaterialBatchList.begin();
            i != mMaterialBatchList.end(); ++i)
        {
            i->second->dump(of);
        }
        of << "------------------" << std::endl;
    }*/
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // MaterialBatch
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    InstanceSpace::MaterialBatch::MaterialBatch(LODBatch * batch, ResourceID mid) :
        mLODBatch(batch),
        mMaterialID(mid),
        mShaderFusion(0),
        mLastIndex(0)
    {
    }
    //--------------------------------------------------------------------------
    InstanceSpace::MaterialBatch::~MaterialBatch()
    {
        GeometryBatchList::iterator i, iend = mGeometryBatchList.end();
        for(i = mGeometryBatchList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mGeometryBatchList.clear();
        if(mMaterial)
        {
            mMaterial->ref(false);
            mMaterial = 0;
        }
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::MaterialBatch::add(Geometry * geo)
    {
        // Look up any current geometry
        String formatString = getGeometryFormatString2(geo->geometry);
        GeoBatchMarkList::iterator gi = mGeoBatchMarkList.find(formatString);
        bool newBucket = true;
        if (gi != mGeoBatchMarkList.end())
        {
            // Found existing geometry, try to assign
            newBucket = !gi->second->add(geo);
            // Note that this bucket will be replaced as the 'current'
            // for this format string below since it's out of space
        }
        // Do we need to create a new one?
        if (newBucket)
        {
            GeometryBatch * gbucket = N_new GeometryBatch(this, geo->geometry->mVertex, geo->geometry->mIndex);

            mGeoBatchMarkList[formatString] = gbucket;
            mGeometryBatchList.push_back(gbucket);

            if(!gbucket->add(geo))
            {
                N_EXCEPT(Internal, _I18n("Somehow we couldn't fit the requested geometry even in a brand new GeometryBatch!! Must be a bug, please report."));
            }
        }
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::MaterialBatch::build()
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
            (*i)->build();
        }
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::MaterialBatch::queue(RenderQueue * queue, GroupID group, NIIf lodValue)
    {
        // Get batch instance
        BatchObj * batchInstance = mLODBatch->getBatchObj();

        // Get material lod strategy
        MaterialLodScheme * scheme = mMaterial->getLodScheme();

        // If material strategy doesn't match, recompute lod value with correct strategy
        //if (scheme != batchInstance->mLodScheme)
            lodValue = scheme->getValue(batchInstance, batchInstance->mCamera);

        // Determine the current material technique
        mShaderFusion = mMaterial->getLOD(0, scheme->get(scheme->getDest(lodValue)));

        GeometryBatchList::iterator i, iend =  mGeometryBatchList.end();
        for(i = mGeometryBatchList.begin(); i != iend; ++i)
        {
            queue->add(*i, group);
        }
    }
    //---------------------------------------------------------------------
    void InstanceSpace::MaterialBatch::query(GeometryQuery * query)
    {
        GeometryBatchList::iterator i, iend =  mGeometryBatchList.end();
        for(i = mGeometryBatchList.begin(); i != iend; ++i)
        {
            (*i)->query(query);
        }
    }
    //--------------------------------------------------------------------------
    const InstanceSpace::GeometryBatchList & InstanceSpace::MaterialBatch::getGeometryList() const
    {
        return mGeometryBatchList;
    }
    //--------------------------------------------------------------------------
    /*void InstanceSpace::MaterialBatch::dump(Nofstream & of) const
    {
        of << _I18n("Material Bucket ") << mMaterialName << std::endl;
        of << "--------------------------------------------------" << std::endl;
        of << _I18n("Geometry buckets: ") << mGeometryBatchList.size() << std::endl;
        GeometryBatchList::const_iterator i, iend = mGeometryBatchList.end();
        for(i = mGeometryBatchList.begin(); i != iend; ++i)
        {
            (*i)->dump(of);
        }
        of << "--------------------------------------------------" << std::endl;
    }*/
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // InstanceSpace::GeometryBatch
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    InstanceSpace::GeometryBatch::GeometryBatch(MaterialBatch * parent, const VertexData * vdata,
        const IndexData * idata):
            mMaterialBatch(parent),
            mVertexData(0),
            mIndexData(0)
    {
        setupData(vdata, idata);
    }
    //--------------------------------------------------------------------------
    InstanceSpace::GeometryBatch::GeometryBatch(SpaceID sid, MaterialBatch * batch, 
        const VertexData * vdata, const IndexData * idata) :
            RenderObj(sid),
            mMaterialBatch(batch),
            mVertexData(0),
            mIndexData(0)
    {
        setupData(vdata, idata);
    }
    //--------------------------------------------------------------------------
    InstanceSpace::GeometryBatch::GeometryBatch(MaterialBatch * batch, GeometryBatch * bucket) :
        mMaterialBatch(batch),
        mVertexData(0),
        mIndexData(0)
    {
        setupData(bucket);
    }
    //--------------------------------------------------------------------------
    InstanceSpace::GeometryBatch::GeometryBatch(SpaceID sid, MaterialBatch * batch, GeometryBatch * bucket) :
        RenderObj(sid),
        mMaterialBatch(batch),
        mVertexData(0),
        mIndexData(0)
    {
        setupData(bucket);
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::GeometryBatch::setupData(const VertexData * vData, const IndexData * iData)
    {
        mManager = mMaterialBatch->getLODBatch()->getBatchObj()->getParent();
        if(mManager->mSkeleton != 0)
            setCustonParam(0, Vector4f(mManager->mSkeleton->getCount(), 0.0f, 0.0f, 0.0f));

        mVertexData = vData->clone(false);
        IndexData * nidata;
        VertexData * nvdata;
        N_Only(GBuffer).create(nidata);
        nidata->mCount = 0;
        nidata->mOffset = 0;

        N_Only(GBuffer).create(nvdata);
        nvdata->mCount = 0;
        nvdata->copy(vData);

        mRenderOp.add(GRT_Index);
        mRenderOp.mIndex = nidata;
        mRenderOp.mVertex = nvdata;

        // Derive the max vertices
        if(iData->mBuffer->getUnitSize() == 32)
        {
            mIdx32 = true;
        }
        else
        {
            mIdx32 = false;
        }

        NCount offset = 0;
        Nui16 texCoordOffset = 0;
        Nui16 texCoordSource = 0;

        const VertexUnit * elem = nvdata->get(VT_Tex_Coord);
        if(elem)
        {
            texCoordSource = elem->mSource;
        }

        for(Nui16 i = 0; i < nvdata->getCount(); ++i)
        {
            if(nvdata->get(i)->mVType == VT_Tex_Coord)
            {
                texCoordOffset++;
            }
            if(texCoordSource == nvdata->get(i)->mSource)
            {
                offset += DataTypeUtil::getSize(nvdata->get(i)->mType);
            }
        }

        nvdata->add(DT_Float1x, VT_Tex_Coord, texCoordSource, offset, texCoordOffset);
        mTexCoordIndex = texCoordOffset;
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::GeometryBatch::setupData(GeometryBatch * bucket)
    {
        mManager = mMaterialBatch->getLODBatch()->getBatchObj()->getParent();
        if(mManager->mSkeleton)
            setCustonParam(0, Vector4f(mManager->mSkeleton->getCount(),0.0f, 0.0f, 0.0f));
        bucket->getGeometry(mRenderOp);
        mVertexData = mRenderOp.mVertex;
        mIndexData = mRenderOp.mIndex;
        setAABB(AABox(-10000,-10000,-10000,10000,10000,10000));
    }
    //--------------------------------------------------------------------------
    InstanceSpace::GeometryBatch::~GeometryBatch(){}
    //--------------------------------------------------------------------------
    NIIf InstanceSpace::GeometryBatch::getCenterRange() const
    {
        return 1;
    }
    //--------------------------------------------------------------------------
    const Material * InstanceSpace::GeometryBatch::getMaterial() const
    {
        return mMaterialBatch->getMaterial();
    }
    //--------------------------------------------------------------------------
    ShaderFusion * InstanceSpace::GeometryBatch::getShaderFusion() const
    {
        return mMaterialBatch->getShaderFusion();
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::GeometryBatch::getMatrix(Matrix4f * xform, NCount & count) const
    {
        bool bSendInverseXfrm = false;//mMaterialBatch->getLODBatch()->getBatchObj()->getParent()->getProvideWorldInverses();
        // Should be the identity transform, but lets allow transformation of the
        // nodes the BatchInstances are attached to for kicks
        BatchObj * batch = mMaterialBatch->getLODBatch()->getBatchObj();
        if(mManager->mSkeleton == 0)
        {
            BatchObj::ObjectList::const_iterator it;
            BatchObj::ObjectList::const_iterator itbegin = batch->getObjectList().begin();
            BatchObj::ObjectList::const_iterator itend = batch->getObjectList().end();

            /*if(batch->getParent()->getProvideWorldInverses())
            {
                // For shaders that use normal maps on instanced geometry objects,
                // we can pass the world transform inverse matrices alongwith with
                // the world matrices. This reduces our usable geometry limit by
                // half in each instance.
                for(it = itbegin; it != itend; ++it, xform += 2)
                {
                    *xform = it->second->mMatrix;
                    *(xform+1) = xform->inverse();
                }
            }
            else*/
            {
                for(it = itbegin; it != itend; ++it, xform++)
                {
                    *xform = it->second->mMatrix;
                }
            }

            count = batch->getObjectList().size() * (bSendInverseXfrm ? 2 : 1);
        }
        else
        {
            BatchObj::ObjectList::const_iterator it;
            BatchObj::ObjectList::const_iterator itbegin = batch->getObjectList().begin();
            BatchObj::ObjectList::const_iterator itend = batch->getObjectList().end();

            for(it = itbegin; it != itend; ++it)
            {
                /*if(mMaterialBatch->getLODBatch()->getBatchObj()->getParent()->getProvideWorldInverses() )
                {
                    for(NIIi i = 0; i < it->second->mBoneCount; ++i, xform += 2)
                    {
                        *xform = it->second->mBoneMatrix[i];
                        *(xform+1) = xform->inverse();
                    }
                }
                else*/
                {
                    for(NCount i = 0; i < it->second->mBoneCount; ++i, xform++)
                    {
                        *xform = it->second->mBoneMatrix[i];
                    }
                }
            }
            count = mManager->mSkeleton->getCount()*batch->getObjectList().size() * (bSendInverseXfrm? 2 : 1);
        }
    }
    //--------------------------------------------------------------------------
    NCount InstanceSpace::GeometryBatch::getMatrixCount() const
    {
        bool bSendInverseXfrm = false;// mMaterialBatch->getLODBatch()->getBatchObj()->getParent()->getProvideWorldInverses();
        BatchObj * batch = mMaterialBatch->getLODBatch()->getBatchObj();
        if(mManager->mSkeleton)
        {
            return mManager->mSkeleton->getCount()*batch->getObjectList().size() * (bSendInverseXfrm ? 2 : 1);
        }
        else
        {
            return batch->getObjectList().size() * (bSendInverseXfrm ? 2 : 1);
        }
    }
    //--------------------------------------------------------------------------
    NIIf InstanceSpace::GeometryBatch::distanceSquared(const Camera * cam) const
    {
        const BatchObj * batchInstance = mMaterialBatch->getLODBatch()->getBatchObj();
        if (cam == batchInstance->mCamera)
            return batchInstance->mSquaredViewDepth;
        else
            return batchInstance->getSpaceNode()->distanceSquared(cam->getExtLod());
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::GeometryBatch::getLight(LightList & ll) const
    {
        return mMaterialBatch->getLODBatch()->getBatchObj()->query(ll);
    }
    //--------------------------------------------------------------------------
    bool InstanceSpace::GeometryBatch::isCastShadow() const
    {
        return mMaterialBatch->getLODBatch()->getBatchObj()->isCastShadow();
    }
    //--------------------------------------------------------------------------
    bool InstanceSpace::GeometryBatch::add(Geometry * geo)
    {
        if(mRenderOp.mVertex->mCount + geo->geometry->mVertex->mCount > (mIdx32 ? 0xFFFFFFFF : 0xFFFF))
        {
            return false;
        }

        mGeometryList.push_back(geo);
        mRenderOp.mVertex->mCount += geo->geometry->mVertex->mCount;
        mRenderOp.mIndex->mCount += geo->geometry->mIndex->mCount;
        return true;
    }
    //--------------------------------------------------------------------------
    void InstanceSpace::GeometryBatch::build()
    {
        // Ok, here's where we transfer the vertices and indexes to the shared
        // buffers
        // Shortcuts
        Nui32 * p32Dest = 0;
        Nui16 * p16Dest = 0;
        IndexBuffer * srcibuf = mRenderOp.mIndex->mBuffer;
        if(mIdx32)
        {
            N_Only(GBuffer).create(srcibuf, 32, mRenderOp.mIndex->mCount, Buffer::M_NORMAL );
            p32Dest = (Nui32 *)srcibuf->lock(Buffer::MM_WHOLE);
        }
        else
        {
            N_Only(GBuffer).create(srcibuf, 16, mRenderOp.mIndex->mCount, Buffer::M_NORMAL );
            p16Dest = static_cast<Nui16 *>(srcibuf->lock(Buffer::MM_WHOLE));
        }

        vector<Nui8 *>::type destBufferLocks;
        vector<VertexUnitList>::type bufferElements;
        Nui16 b;
        VertexData * dcl = mRenderOp.mVertex;
        for(b = 0; b < mVertexData->getAttachCount(); ++b)
        {
            NCount vertexCount = mRenderOp.mVertex->mCount;

            VertexBuffer * vbuf;
            N_Only(GBuffer).create(vbuf, dcl->getUnitSize(b), vertexCount, Buffer::M_NORMAL );
            mVertexData->attach(b, vbuf);
            destBufferLocks.push_back((Nui8 *)vbuf->lock(Buffer::MM_WHOLE));

            VertexUnitList temp;
            dcl->getUnits(b, temp);
            bufferElements.push_back(temp);
            mRenderOp.mVertex->attach(b, vbuf);
        }

        // Iterate over the geometry items
        NCount indexOffset = 0;

        // to generate the boundingBox
        NIIf Xmin = 0,Ymin = 0,Zmin = 0,Xmax = 0,Ymax = 0,Zmax = 0;
        Geometry * precGeom = *(mGeometryList.begin());
        Nui16 index = 0;
        if(mMaterialBatch->mLastIndex != 0)
            index = mMaterialBatch->mLastIndex + 1;

        GeometryList::iterator gi, giend = mGeometryList.end();
        for(gi = mGeometryList.begin(); gi != giend; ++gi)
        {
            Geometry * geom = *gi;
            if(precGeom->ID != geom->ID)
                index++;

            InstancedObject * instancedObject = mMaterialBatch->getLODBatch()->getBatchObj()->isObjectExist(index);
            if(instancedObject == NULL)
            {
                if(mManager->mSkeleton)
                {
                    instancedObject = N_new InstancedObject(index, mManager->mSkeletonFusion, mManager->mAnimationCtrl);
                }
                else
                {
                    instancedObject = N_new InstancedObject(index);
                }
                mMaterialBatch->getLODBatch()->getBatchObj()->addObject(index, instancedObject);
            }
            instancedObject->mGeometryBatchList.push_back(this);

            // Copy indexes across with offset
            IndexData * srcIdxData = geom->geometry->mIndex;
            if(mIdx32)
            {
                Nui32 * pSrc = (Nui32 *)srcIdxData->mBuffer->lock(srcIdxData->mOffset, 
                    srcIdxData->mCount * srcIdxData->mBuffer->getUnitSize(), Buffer::MM_READ);

                copyIndexes(pSrc, p32Dest, srcIdxData->mCount, indexOffset);
                p32Dest += srcIdxData->mCount;
                srcIdxData->mBuffer->unlock();
            }
            else
            {
                Nui16 * pSrc = static_cast<Nui16 *>(srcIdxData->mBuffer->lock(
                    srcIdxData->mOffset, srcIdxData->mCount * srcIdxData->mBuffer->getUnitSize(),
                        Buffer::MM_READ));

                copyIndexes(pSrc, p16Dest, srcIdxData->mCount, indexOffset);
                p16Dest += srcIdxData->mCount;
                srcIdxData->mBuffer->unlock();
            }

            // Now deal with vertex buffers
            // we can rely on buffer counts / formats being the same
            VertexData * srcVData = geom->geometry->mVertex;

            for (b = 0; b < mVertexData->getAttachCount(); ++b)
            {
                // lock source
                VertexBuffer * srcBuf = srcVData->getAttach(b);
                Nui8 * pSrcBase = static_cast<Nui8 *>(srcBuf->lock(Buffer::MM_READ));
                Nui8 * pDstBase = destBufferLocks[b];
                NCount bufInc = srcBuf->getUnitSize();

                NIIf *pSrcReal, *pDstReal;
                Vector3f tmp;

                for(NCount v = 0; v < srcVData->mCount; ++v)
                {
                    //to know if the current buffer is the one with the buffer or not
                    bool isTheBufferWithIndex=false;
                    // Iterate over vertex elements
                    VertexUnitList & elems = bufferElements[b];
                    VertexUnitList::iterator ei;
                    for (ei = elems.begin(); ei != elems.end(); ++ei)
                    {
                        VertexUnit & elem = *ei;
                        elem.getData(pSrcBase, pSrcReal);
                        elem.getData(pDstBase, pDstReal);
                        if(elem.mVType == VT_Tex_Coord && elem.mIndex == mTexCoordIndex)
                        {
                            isTheBufferWithIndex = true;
                            *pDstReal++ = static_cast<NIIf>(index);
                        }
                        else
                        {
                            switch(elem.mVType)
                            {
                            case VT_Pos:
                                tmp.x = pSrcReal[0];
                                tmp.y = pSrcReal[1];
                                tmp.z = pSrcReal[2];
                                if(tmp.x<Xmin)
                                    Xmin = tmp.x;
                                if(tmp.y<Ymin)
                                    Ymin = tmp.y;
                                if(tmp.z<Zmin)
                                    Zmin = tmp.z;
                                if(tmp.x>Xmax)
                                    Xmax = tmp.x;
                                if(tmp.y>Ymax)
                                    Ymax = tmp.y;
                                if(tmp.z>Zmax)
                                    Zmax = tmp.z;
                            default:
                                memcpy(pDstReal, pSrcReal, DataTypeUtil::getSize(elem.mType));
                            break;
                            };
                        }
                    }
                    if (isTheBufferWithIndex)
                        pDstBase += bufInc+4;
                    else
                        pDstBase += bufInc;
                        pSrcBase += bufInc;
                }
                // Update pointer
                destBufferLocks[b] = pDstBase;
                srcBuf->unlock();
            }
            indexOffset += geom->geometry->mVertex->mCount;
            precGeom = geom;
        }
        mMaterialBatch->mLastIndex = index;

        srcibuf->unlock();
        for (b = 0; b < mVertexData->getAttachCount(); ++b)
        {
            mVertexData->getAttach(b)->unlock();
        }

        N_delete mVertexData;
        N_delete mIndexData;

        mVertexData = mRenderOp.mVertex;
        mIndexData = mRenderOp.mIndex;
        mManager->mGeometryRawList.push_back(&mRenderOp);
        setAABB(AABox(Xmin,Ymin,Zmin,Xmax,Ymax,Zmax));
        mAABB = AABox(Xmin,Ymin,Zmin,Xmax,Ymax,Zmax);
    }
    //--------------------------------------------------------------------------
    /*void InstanceSpace::GeometryBatch::dump(Nofstream & of) const
    {
        of << _I18n("Geometry Bucket") << std::endl;
        of << "---------------" << std::endl;
        of << _I18n("Format string: ") << mFormatString << std::endl;
        of << _I18n("Geometry items: ") << mGeometryList.size() << std::endl;
        of << "---------------" << std::endl;
    }*/
    //--------------------------------------------------------------------------
    void InstanceSpace::GeometryBatch::query(GeometryQuery * query)
    {
        this->setGeometryLod(mMaterialBatch->getLODBatch()->getLod());
        query->query(this);
    }
    //---------------------------------------------------------------------
}