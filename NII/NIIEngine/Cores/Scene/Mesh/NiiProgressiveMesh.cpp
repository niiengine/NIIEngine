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
#include "NiiProgressiveMesh.h"

// The algorithm in this file is based heavily on:
/*
*  Progressive Mesh type Polygon Reduction Algorithm
*  by Stan Melax (c) 1998
*/
#include "NiiLodSchemeManager.h"
#include "NiiMeshManager.h"
#include "NiiString.h"
#include "NiiGBufferManager.h"
#include "NiiLogManager.h"
#include "NiiMeshLodScheme.h"

#if N_DEBUG
#define LOG_PROGRESSIVE_MESH_GENERATION 1
#else
#define LOG_PROGRESSIVE_MESH_GENERATION 0
#endif

#if LOG_PROGRESSIVE_MESH_GENERATION
#include <iostream>
std::ofstream ofdebug;
#endif

namespace NII
{
    //----------------------------------------------------------------------------
    const Nui8 BitArray::bit_count[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };
    const Nui8 BitArray::bit_mask[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
    //----------------------------------------------------------------------------
    #define NEVER_COLLAPSE_COST 99999.9f
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // VertexDataVariant
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class VertexDataVariant
    {
    private:
        Nui8 * mBaseDataPointer;
        Nui8 * mCurDataPointer;
        NCount mOffsetSize;

        // source dependent part, only first buffer of several with shared mSource holds lock on hardware vertex buffer
        NIIi mSource;
        VertexBuffer * mBuffer;
        std::auto_ptr<VertexBufferLockGuard> mLockGuard;    // only first by source takes lock

        friend class VertexDataVariantList;

        VertexDataVariant(const VertexData * vertexData, const VertexUnit * vertexElement, VertexDataVariant* bufferOwner)
            : mOffsetSize(0), mSource(-1)
        {
            static NIIf fakeDataBuffer[3] = { 0.0f, 0.0f, 0.0f }; // 12 bytes, can be safely used with zero mOffsetSize
            mBaseDataPointer = mCurDataPointer = (Nui8*)fakeDataBuffer;

            if(NULL != vertexElement)
            {
                mSource = vertexElement->mSource;
                mBuffer = vertexData->getAttach(mSource);

                // only first VertexDataVariant really locks buffer and store pointer to raw data
                if(NULL == bufferOwner)
                {
                    // buffer is not locked yet, so lock it and became buffer owner
                    mLockGuard.reset(new VertexBufferLockGuard(mBuffer, Buffer::MM_READ));
                    bufferOwner = this;
                }

                // adjust whole vertex pointer to vertex part pointer
                vertexElement->getData(bufferOwner->mLockGuard->pData, mBaseDataPointer);
                mCurDataPointer = mBaseDataPointer;
                mOffsetSize = mBuffer->getUnitSize();
            }
        }

    public:
        bool isValid() const { return mOffsetSize != 0; }

        NIIi getSource() const { return mSource; }
        Nui8 * getBaseDataPointer() const { return mBaseDataPointer; }
        Nui8 * getCurDataPointer() const { return mCurDataPointer; }
        NCount getOffsetSize() const { return mOffsetSize; }

        void reset() { mCurDataPointer = mBaseDataPointer; }
        void offset() { mCurDataPointer += mOffsetSize; }
        void offsetToElement(NIIi itemIndex) { mCurDataPointer = mBaseDataPointer + itemIndex * mOffsetSize;    }

        Vector3f getNextVector3()
        {
            NIIf * v = (NIIf *)mCurDataPointer;
            mCurDataPointer += mOffsetSize;
            return Vector3f(v[0], v[1], v[2]);
        }

        Vector2f getNextVector2()
        {
            NIIf * v = (NIIf *)mCurDataPointer;
            mCurDataPointer += mOffsetSize;
            return Vector2f(v[0], v[1]);
        }
    };
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    // VertexDataVariantList
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    class VertexDataVariantList
    {
    public:
        VertexDataVariant * create(const VertexData * vertexData, VertexType sem)
        {
            const VertexUnit * vertexElement = vertexData->get(sem);
            VertexDataVariant * bufferOwner = vertexElement ? getBySource(vertexElement->mSource) : NULL;
            mVdList.push_back(VertexDataVariantSharedPtr(new VertexDataVariant(vertexData, vertexElement, bufferOwner)));
            return mVdList.back().get();
        }

    private:
        VertexDataVariant* getBySource(NIIi source)
        {
            for (vd_list_t::const_iterator it = mVdList.begin(); it != mVdList.end(); ++it)
            {
                if ((*it)->getSource() == source)
                    return it->get();
            }
            return NULL;
        }

    private:
        typedef SharedPtrST<VertexDataVariant> VertexDataVariantSharedPtr;
        typedef vector<VertexDataVariantSharedPtr>::type vd_list_t;
        vd_list_t mVdList;
    };
    //-----------------------------------------------------------------------------
    class IndexDataVariant;

    typedef SharedPtrST<IndexDataVariant> IndexDataVariantSharedPtr;
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // IndexDataVariant
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    class IndexDataVariant
    {
    private:
        IndexBuffer * mBuffer;
        Nui8 * mBaseDataPointer;
        Nui8 * mCurDataPointer;
        NCount mIndexCount;
        NCount mOffsetSize;
        bool mUse32bitIndexes;
        std::auto_ptr<IndexBufferLockGuard> mLockGuard;

        IndexDataVariant(const IndexData * indexData, Buffer::MuteMode lockOpt) :
            mIndexCount(0),
            mOffsetSize(0),
            mUse32bitIndexes(false)
        {
            static NIIi fakeIndexBuffer = 0;    // 4 bytes, can be safely used with zero mOffsetSize
            mBaseDataPointer = mCurDataPointer = (Nui8 *)&fakeIndexBuffer;

            if(NULL == indexData)
                return;

            mBuffer = indexData->mBuffer;
            if(mBuffer == 0)
                return;

            mIndexCount = indexData->mCount;
            if(0 == mIndexCount)
                return;

            mUse32bitIndexes = (mBuffer->getUnitSize() == 32);
            mOffsetSize = mUse32bitIndexes ? sizeof(Nui32) : sizeof(Nui16);

            mLockGuard.reset(new IndexBufferLockGuard(mBuffer, lockOpt));
            mBaseDataPointer = (Nui8*)mLockGuard->pData;

            reset();
        }

        bool isValid() const { return mOffsetSize != 0; }
    public:

        static IndexDataVariantSharedPtr create(const IndexData * indexData, Buffer::MuteMode lockOpt = Buffer::MM_READ)
        {
            IndexDataVariantSharedPtr p(new IndexDataVariant(indexData, lockOpt));
            return p->isValid() ? p : IndexDataVariantSharedPtr();
        }

        Nui8 * getBaseDataPointer() const { return mBaseDataPointer; }
        Nui8 * getCurDataPointer() const { return mCurDataPointer; }
        NCount getOffsetSize() const { return mOffsetSize; }
        NCount getIndexCount() const { return mIndexCount; }
        bool is32bitIndexes() const { return mUse32bitIndexes; }

        void reset() { mCurDataPointer = mBaseDataPointer; }
        void offsetToElement(NIIi itemIndex) { mCurDataPointer = getBaseDataPointer() + itemIndex * getOffsetSize();    }
        unsigned getNextIndex() { unsigned idx = mUse32bitIndexes ? *(Nui32*)mCurDataPointer : *(Nui16*)mCurDataPointer; mCurDataPointer += mOffsetSize; return idx; }

        void markUsedVertices(BitArray & bitmask) const
        {
            if(mUse32bitIndexes)
            {
                for(const Nui32 * ptr = (const Nui32 *)mBaseDataPointer, *end_ptr = ptr + mIndexCount; ptr < end_ptr; ++ptr)
                    bitmask.setBit(*ptr);
            }
            else
            {
                for(const Nui16 * ptr = (const Nui16 *)mBaseDataPointer, *end_ptr = ptr + mIndexCount; ptr < end_ptr; ++ptr)
                    bitmask.setBit(*ptr);
            }
        }

        void createIndexData(IndexData * pIndexData, bool use16bitIndexes, vector<unsigned>::type* indexMap)
        {
            NCount indexCount = getIndexCount();
            reset();

            pIndexData->mOffset = 0;
            pIndexData->mCount = indexCount;
            N_Only(GBuffer).create(pIndexData->mBuffer, use16bitIndexes ? 16 : 32, indexCount, Buffer::M_NORMAL );

            IndexBufferLockGuard outIdataLock(pIndexData->mBuffer, Buffer::MM_WHOLE);

            Nui16 * pShortOut = use16bitIndexes ? (Nui16*)outIdataLock.pData : NULL;
            Nui32 * pIntOut = use16bitIndexes ? NULL : (Nui32*)outIdataLock.pData;

            if(use16bitIndexes)
            {
                for(NCount n = 0; n < indexCount; ++n)
                {
                    unsigned idx = getNextIndex();
                    *pShortOut++ = indexMap ? (*indexMap)[idx] : idx;
                }
            }
            else
            {
                for(NCount n = 0; n < indexCount; ++n)
                {
                    unsigned idx = getNextIndex();
                    *pIntOut++ = indexMap ? (*indexMap)[idx] : idx;
                }
            }
        }
    };
    //---------------------------------------------------------------------
    ProgressiveMesh::ProgressiveMesh(SubMesh * pSubMesh) :
        mSubMesh(pSubMesh),
        mCurrNumIndexes(0),
        mVertexComponentFlags(0)
    {
        // ignore un-indexed submeshes
        if(pSubMesh->getIndexData(0)->mCount == 0)
        {
            mSubMesh = NULL;
            return;
        }

        Mesh * pMesh = pSubMesh->getMesh();
        NIIf sqrDiag = pMesh->getAABB().getSize().lengthSquared();
        mInvSquaredBoundBoxDiagonal = (0.0f != sqrDiag) ? 1.0f / sqrDiag : 0.0f;

        mNextWorstCostHint = 0;
        mInvalidCostCount = 0;
        mRemovedVertexDuplicatesCount = 0;

        mVertexData = pSubMesh->isSelfVertex() ? pSubMesh->getVertexData() : pMesh->getVertexData();
        mIndexData = pSubMesh->getIndexData(0);

        mInvalidCostMask.resize(mVertexData->mCount);
        addWorkingData(mVertexData, mIndexData);
    }
    //---------------------------------------------------------------------
    ProgressiveMesh::~ProgressiveMesh()
    {
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::addExtraVertexPositionBuffer(const VertexData * vertexData)
    {
        addWorkingData(vertexData, mIndexData);
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::initializeProgressiveMeshList(ProgressiveMeshList & pmList,
        Mesh * pMesh)
    {
        NCount subMeshCount = pMesh->getSubCount();
        pmList.reserve(subMeshCount);
        for(NCount i = 0; i < subMeshCount; ++i)
        {
            SubMesh * pSubMesh = pMesh->getSub(i);
            pmList.push_back(N_new ProgressiveMesh(pSubMesh));
        }
    }
    //---------------------------------------------------------------------
    bool ProgressiveMesh::generateLodLevels(Mesh * pMesh, const LodValueList & lodValues,
        VertexReductionQuota reductionMethod, NIIf reductionValue)
    {
        pMesh->removeAllLod();

        N_Only(Log).stream() << "Generating " << lodValues.size() << " lower LODs for mesh " << pMesh->getOriginID();

        // Set up data for reduction
        ProgressiveMeshList pmList;
        initializeProgressiveMeshList(pmList, pMesh);
        MeshLodScheme * scheme = pMesh->getLodScheme();

        bool generated = build(pmList, scheme, lodValues, reductionMethod, reductionValue);
        if(generated)
        {
            // transfer all LODs from ProgressiveMesh to the real one
            IndexDataList::iterator smid, smidend;
            NCount subMeshCount = pMesh->getSubCount();
            for(NCount i = 0; i < subMeshCount; ++i)
            {
                smidend = pmList[i]->mLodFaceList.end();
                for(smid = pmList[i]->mLodFaceList.begin(); smid != smidend; ++smid)
                {
                    pMesh->getSub(i)->addIndexData(*smid);
                }
            }
            // register them
            //MeshLodScheme * scheme = static_cast<MeshLodScheme *>(N_Only(LodScheme).get(N_LOD_Mesh, scheme->getTypeID()));
            bakeLodUsage(pMesh, scheme, lodValues, false);
        }

        freeProgressiveMeshList(&pmList);

        return generated;
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::freeProgressiveMeshList(ProgressiveMeshList * pmList)
    {
        ProgressiveMeshList::iterator i, iend = pmList->end();
        for(i = pmList->begin(); i != iend; ++i)
        {
            N_delete *i;
            *i = NULL;
        }
    }
    //---------------------------------------------------------------------
    Mesh * ProgressiveMesh::generateSimplifiedMesh(ResourceID rid, GroupID gid, 
        Mesh * inMesh, bool dropOriginalGeometry, const LodValueList & lodValues, 
            VertexReductionQuota reductionMethod, NIIf reductionValue, 
                NCount * removedVertexDuplicatesCount)
    {
        N_Only(Log).stream() << "Generating simplified mesh " << rid << " for mesh " << inMesh->getOriginID();

        // Set up data for reduction
        ProgressiveMeshList pmList;
        initializeProgressiveMeshList(pmList, inMesh);
        MeshLodScheme * scheme = inMesh->getLodScheme();
        // Perform reduction
        build(pmList, scheme, lodValues, reductionMethod, reductionValue);

        // Bake new simplified mesh
        Mesh * simplifiedMesh = N_Only(Mesh).createManual(rid, gid);
        bakeSimplifiedMesh(simplifiedMesh, inMesh, pmList, dropOriginalGeometry);
        scheme = static_cast<MeshLodScheme *>(N_Only(LodScheme).get(N_LOD_Mesh, scheme->getTypeID()));
        bakeLodUsage(simplifiedMesh, scheme, lodValues, dropOriginalGeometry);

        // Return some statistic
        if(removedVertexDuplicatesCount)
        {
            NCount duplicatesCount = 0;
            for(ProgressiveMeshList::iterator it = pmList.begin(); it != pmList.end(); ++it)
                duplicatesCount += (*it)->mRemovedVertexDuplicatesCount;
            *removedVertexDuplicatesCount = duplicatesCount;
        }

        freeProgressiveMeshList(&pmList);
        return simplifiedMesh;
    }
    //---------------------------------------------------------------------
    bool ProgressiveMesh::build(ProgressiveMeshList & pmInList, const MeshLodScheme * lodStrategy,
        const LodValueList & lodValues, VertexReductionQuota quota, NIIf reductionValue)
    {
        N_assert(!pmInList.empty(), "error");
        bool generated = false;

        NCount numVerts = 0;

        ProgressiveMeshList pmBuildList;

        for(ProgressiveMeshList::iterator i = pmInList.begin(); i != pmInList.end(); ++i)
        {
            ProgressiveMesh * p = *i;
            if(NULL == p->mSubMesh)
                continue; // dummy, skip it

            p->computeAllCosts();

            // Init
            p->mCurrNumIndexes = (GeometryRaw::OT_TRIANGLE_LIST == p->mSubMesh->getGeometryType()) ?
                p->mIndexData->mCount : (p->mIndexData->mCount - 2) * 3;

#if LOG_PROGRESSIVE_MESH_GENERATION
            StringStream logname;
            logname << "pm_before_" << std::distance(pmInList.begin(), i) << ".log";
            (*i)->dumpContents(logname.str());
#endif
            numVerts += p->mWorstCostsSize;

            pmBuildList.push_back(p);
        }

        ProgressiveMeshList pmList(pmBuildList);

        // if any one of this two checks is failed - we complete one LOD generation
        NCount numCollapses = numVerts;            // unlimited
        NIIf costLimit = NEVER_COLLAPSE_COST;    // unlimited

        bool abandon = false;

        for (LodValueList::const_iterator lod = lodValues.begin(); lod != lodValues.end(); ++lod)
        {
            NIIi level = std::distance(lodValues.begin(), lod);

            // adjust LOD target limits
            switch(quota)
            {
            case VRQ_CONSTANT:
                numCollapses = (NCount)reductionValue;
                break;
            case VRQ_PROPORTIONAL:
                numCollapses = (NCount)(numVerts * reductionValue);
                numVerts -= numCollapses;
                break;
            case VRQ_ERROR_COST:
                // we must increase cost limit with each next lod level proportionally to squared distance ratio or inverted pixel area ratio
                NIIf reductionValueMultiplier = lodStrategy->getBias((NIIf)lodStrategy->getDest(lodValues[0]) / lodStrategy->getDest(lodValues[level]));
                N_assert(level == 0 || reductionValueMultiplier > 1.0, "error");
                costLimit = reductionValue * reductionValueMultiplier;
                break;
            }

            // NB if 'abandon' is set, we stop reducing
            // However, we still bake the number of LODs requested, even if it
            // means they are the same
            while(numCollapses > 0 && !abandon)
            {
                ProgressiveMesh * pmCur;            //out
                CostIndexPair * collapseTarget;    // out
                getNextCollapser(pmList, pmCur, collapseTarget);

                // we found collapse target, but may be we must not collapse it
                if(collapseTarget != NULL)
                {
                    N_assert(collapseTarget->first != NEVER_COLLAPSE_COST, "error");
                    if(VRQ_ERROR_COST == quota)
                    {
                        NIIf cost = collapseTarget->first;
                        if(cost > costLimit)
                            collapseTarget = NULL;
                    }
                    else // VRQ_CONSTANT, VRQ_PROPORTIONAL
                    {
                        if(getInvalidCostCount(pmList) >= numCollapses)
                            collapseTarget = NULL; // need recalc
                    }
                }

                // if we have not collapse target but have invalid costs - recalc them
                if(collapseTarget == NULL)
                {
                    if(recomputeInvalidCosts(pmList))
                    {
                        // a some invalid costs was recomputed and we should try to continue collapsing
                        // because the recomputed best cost can be less than level limit;
                        continue;
                    }
                    else
                    {
                        abandon = pmList.empty();
                        break; // an invalid costs is not found and we complete collapsing for the current LOD
                    }
                }

                // OK, we decide to collapse this target
                N_assert(collapseTarget, "error");
                N_assert(pmCur, "error");
                N_assert(numCollapses > 0, "error");

                // Collapse on every buffer
                WorkingDataList::iterator idata, idataend;
                idataend = pmCur->mWorkingData.end();

                for (idata = pmCur->mWorkingData.begin(); idata != idataend; ++idata)
                {
                    PMVertex * collapser = &(idata->mVertList[collapseTarget->second]);

                    if(collapser->face.size() == pmCur->mCurrNumIndexes / 3
                    || collapser->collapseTo == NULL)
                    {
                        // Must have run out of valid collapsables
                        pmList.erase(std::remove(pmList.begin(), pmList.end(), pmCur), pmList.end());
                        abandon = pmList.empty();
                        break;
                    }

#if LOG_PROGRESSIVE_MESH_GENERATION
                    ofdebug << "Collapsing index " << (Nui32)collapser->index <<    "(border: "<< (collapser->mBorderStatus == PMVertex::BS_BORDER ? "yes" : "no") <<
                    ") to " << (Nui32)collapser->collapseTo->index << "(border: "<< (collapser->collapseTo->mBorderStatus == PMVertex::BS_BORDER ? "yes" : "no") <<
                    ")" << std::endl;
#endif
                    N_assert(collapser->collapseTo->removed == false, "error");
                    N_assert(pmCur->mCurrNumIndexes > 0, "error");

                    pmCur->collapse(collapser);

                    N_assert(pmCur->mCurrNumIndexes > 0, "error");
                }

                // we must never return to it
                collapseTarget->first = NEVER_COLLAPSE_COST;
                --numCollapses;
            }
            // end of one LOD collapsing loop

            // Bake a new LOD and add it to the list
            for(ProgressiveMeshList::iterator i = pmBuildList.begin(); i != pmBuildList.end(); ++i)
            {
                ProgressiveMesh * p = *i;
                N_assert(NULL != p->mSubMesh, "error"); //dummy can't happen here

#if LOG_PROGRESSIVE_MESH_GENERATION
                StringStream logname;
                ProgressiveMeshList::iterator t = std::find(pmInList.begin(), pmInList.end(), p);
                N_assert(t != pmInList.end(), "error");
                logname << "pm_" << std::distance(pmInList.begin(), t) << "__level_" << level << ".log";
                (*i)->dumpContents(logname.str());
#endif
                IndexData * lodData = NULL;

                if(p->mCurrNumIndexes != p->mIndexData->mCount)
                {
                    N_assert(p->mCurrNumIndexes > 0, "error");

                    lodData = N_new IndexData();
                    p->bakeNewLOD(lodData);
                    generated = true;
                }
                else
                {
                    p->mRemovedVertexDuplicatesCount = 0;
                    lodData = p->mSubMesh->getIndexData(0)->clone();
                }

                N_assert(NULL != lodData, "error");

                p->mLodFaceList.push_back(lodData);
            }
        }
        return generated;
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::addWorkingData(const VertexData * vertexData,
        const IndexData * indexData)
    {
        if(0 == vertexData->mCount || 0 == indexData->mCount)
            return;

        // Insert blank working data, then fill
        mWorkingData.push_back(PMWorkingData());
        PMWorkingData& work = mWorkingData.back();

        // Build vertex list
        // Resize face list (this will always be this big)
        work.mFaceVertList.resize(vertexData->mCount);
        // Also resize common vert list to max, to avoid reallocations
        work.mVertList.resize(vertexData->mCount);

        VertexDataVariantList vdVariantList;

        VertexDataVariant* vertexDataBuffer = vdVariantList.create(vertexData, VT_Pos);
        VertexDataVariant* normalDataBuffer = vdVariantList.create(vertexData, VT_Normals);
        VertexDataVariant* uvDataBuffer = vdVariantList.create(vertexData, VT_Tex_Coord);

        mVertexComponentFlags |= (1 << VT_Pos);
        mVertexComponentFlags |= (1 << VT_Normals);
        mVertexComponentFlags |= (1 << VT_Tex_Coord);

        IndexDataVariantSharedPtr indexDataVar(IndexDataVariant::create(indexData));

        if(indexDataVar.isNull())
            return;

        //make used index list
        BitArray usedVertices(vertexData->mCount);
        indexDataVar->markUsedVertices(usedVertices);

        mWorstCostsSize = usedVertices.getBitsCount();
        mWorstCosts.resize(mWorstCostsSize);

        PMVertex * vBase = &work.mVertList.front();
        WorstCostList::iterator it = mWorstCosts.begin();
        bool someVerticesWasSkipped = false;
        for(unsigned idx = 0, end_idx = vertexData->mCount; idx < end_idx; ++idx)
        {
            // skip unused vertices
            if(!usedVertices.getBit(idx))
            {
                someVerticesWasSkipped = true;
                continue;
            }

            // resync vertex data offset if some vertices was skipped
            if(someVerticesWasSkipped)
            {
                someVerticesWasSkipped = false;

                vertexDataBuffer->offsetToElement(idx);
                normalDataBuffer->offsetToElement(idx);
                uvDataBuffer->offsetToElement(idx);
            }

            // store reference to used vertex
            it->first = 0.0f;
            it->second = idx;
            ++it;

            // read vertex data
            PMVertex * commonVert = vBase + idx;
            commonVert->setDetails(idx, vertexDataBuffer->getNextVector3(), normalDataBuffer->getNextVector3(), uvDataBuffer->getNextVector2());
        }

        // Build tri list
        NCount numTris = (GeometryRaw::OT_TRIANGLE_LIST == mSubMesh->getGeometryType()) ?
            mIndexData->mCount / 3 : mIndexData->mCount - 2;

        work.mTriList.reserve(numTris); // reserved tri list

        PMFaceVertex* fvBase = &work.mFaceVertList.front();
        if(GeometryRaw::OT_TRIANGLE_LIST == mSubMesh->getGeometryType())
        {
            for (NCount i = 0, vi = 0; i < numTris; ++i)
            {
                Nui32 vindex;

                PMFaceVertex *v0, *v1, *v2;

                vindex = indexDataVar->getNextIndex();

                v0 = fvBase + vindex;
                v0->commonVertex = vBase + vindex;
                v0->realIndex = vindex;

                vindex = indexDataVar->getNextIndex();

                v1 = fvBase + vindex;
                v1->commonVertex = vBase + vindex;
                v1->realIndex = vindex;

                vindex = indexDataVar->getNextIndex();

                v2 = fvBase + vindex;
                v2->commonVertex = vBase + vindex;
                v2->realIndex = vindex;

                if(v0!=v1 && v1!=v2 && v2!=v0) // see assertion: OgreProgressiveMesh.cpp, line: 723, condition: v0!=v1 && v1!=v2 && v2!=v0
                {
                    work.mTriList.push_back(PMTriangle());
                    work.mTriList.back().setDetails(vi++, v0, v1, v2);
                }
            }
        }
        else if(GeometryRaw::OT_TRIANGLE_STRIP == mSubMesh->getGeometryType())
        {
            bool tSign = true;
            for (NCount i = 0, vi = 0; i < numTris; ++i)
            {
                Nui32 vindex;

                PMFaceVertex *v0, *v1, *v2;

                indexDataVar->offsetToElement(i);
                vindex = indexDataVar->getNextIndex();

                v0 = fvBase + vindex;
                v0->commonVertex = vBase + vindex;
                v0->realIndex = vindex;

                vindex = indexDataVar->getNextIndex();

                v1 = fvBase + vindex;
                v1->commonVertex = vBase + vindex;
                v1->realIndex = vindex;

                vindex = indexDataVar->getNextIndex();

                v2 = fvBase + vindex;
                v2->commonVertex = vBase + vindex;
                v2->realIndex = vindex;

                if(v0!=v1 && v1!=v2 && v2!=v0) // see assertion: OgreProgressiveMesh.cpp, line: 723, condition: v0!=v1 && v1!=v2 && v2!=v0
                {
                    work.mTriList.push_back(PMTriangle());
                    work.mTriList.back().setDetails(vi++, tSign ? v0 : v1, tSign ? v1 : v0, v2);
                }

                tSign = !tSign;
            }
        }
        else //FAN
        {
            for (NCount i = 0, vi = 0; i < numTris; ++i)
            {
                Nui32 vindex;

                PMFaceVertex *v0, *v1, *v2;

                indexDataVar->offsetToElement(0);
                vindex = indexDataVar->getNextIndex();

                v0 = fvBase + vindex;
                v0->commonVertex = vBase + vindex;
                v0->realIndex = vindex;

                indexDataVar->offsetToElement(i);
                vindex = indexDataVar->getNextIndex();

                v1 = fvBase + vindex;
                v1->commonVertex = vBase + vindex;
                v1->realIndex = vindex;

                vindex = indexDataVar->getNextIndex();

                v2 = fvBase + vindex;
                v2->commonVertex = vBase + vindex;
                v2->realIndex = vindex;

                if(v0!=v1 && v1!=v2 && v2!=v0) // see assertion: OgreProgressiveMesh.cpp, line: 723, condition: v0!=v1 && v1!=v2 && v2!=v0
                {
                    work.mTriList.push_back(PMTriangle());
                    work.mTriList.back().setDetails(vi++, v0, v1, v2);
                }
            }
        }
        indexDataVar.setNull();

        // try to merge borders, to increase algorithm effectiveness
        mergeWorkingDataBorders();
    }

    /** Comparator for unique vertex list
     */
    struct ProgressiveMesh::vertexLess
    {
        bool operator()(PMVertex* v1, PMVertex* v2) const
        {
            if (v1->position.x < v2->position.x) return true;
            else if(v1->position.x > v2->position.x) return false;

            if (v1->position.y < v2->position.y) return true;
            else if(v1->position.y > v2->position.y) return false;

            if (v1->position.z < v2->position.z) return true;
            else if(v1->position.z > v2->position.z) return false;


            if (v1->normal.x < v2->normal.x) return true;
            else if(v1->normal.x > v2->normal.x) return false;

            if (v1->normal.y < v2->normal.y) return true;
            else if(v1->normal.y > v2->normal.y) return false;

            if (v1->normal.z < v2->normal.z) return true;
            else if(v1->normal.z > v2->normal.z) return false;


            if (v1->uv.x < v2->uv.x) return true;
            else if(v1->uv.x > v2->uv.x) return false;

            if (v1->uv.y < v2->uv.y) return true;
            else if(v1->uv.y > v2->uv.y) return false;

            return false;
        }
    };

    void ProgressiveMesh::mergeWorkingDataBorders()
    {
        IndexDataVariantSharedPtr indexDataVar(IndexDataVariant::create(mIndexData));

        if(indexDataVar.isNull())
            return;

        PMWorkingData& work = mWorkingData.back();

        typedef std::map<PMVertex*, NCount, vertexLess> CommonVertexMap;
        CommonVertexMap commonBorderVertexMap;

        std::map<Nui32 /* original index */, Nui32 /* new index (after merge) */> mappedIndexes;

        // try to merge borders to borders
        WorstCostList newUniqueIndexes;

        // we will use border status from first buffer only - is this right?
        PMVertex* vBase = &work.mVertList.front();

        // iterate over used vertices
        for(WorstCostList::iterator it = mWorstCosts.begin(), end_it = mWorstCosts.end(); it != end_it; ++it)
        {
            unsigned idx = it->second;

            PMVertex* v = vBase + idx;
            v->initBorderStatus();

            CommonVertexMap::iterator iCommonBorderVertex = commonBorderVertexMap.end();
            if(PMVertex::BS_BORDER == v->mBorderStatus)
            {
                //vertex is a border, try to find it in the common border vertex map at first
                iCommonBorderVertex = commonBorderVertexMap.find(v);

                // if found but not near enough - ignore it
                if(iCommonBorderVertex != commonBorderVertexMap.end() && !iCommonBorderVertex->first->isNearEnough(v))
                    iCommonBorderVertex = commonBorderVertexMap.end();

            }

            //if vertex is not found in the common border vertex map
            if(iCommonBorderVertex == commonBorderVertexMap.end())
            {
                if(PMVertex::BS_BORDER == v->mBorderStatus)
                    commonBorderVertexMap.insert(CommonVertexMap::value_type(v, idx));
                mappedIndexes[idx] = idx;       // old and new indexes should be equal
                newUniqueIndexes.push_back(*it); // update common unique index list
            }
            else // merge vertices
            {
                // vertex is found in the common border vertex map
                // the triangles will use [iCommonBorderVertex->second] instead of [*idx]
                mappedIndexes[idx] = iCommonBorderVertex->second;

                // set border status for the founded vertex as BF_UNKNOWN
                // (the border status will reinitialized at the end of function since we mark it as not defined)
                iCommonBorderVertex->first->mBorderStatus = PMVertex::BS_UNKNOWN;
                ++mRemovedVertexDuplicatesCount;
            }

            //the neighbor & face must be cleaned now (it will be recreated after merging)
            v->neighbor.clear();
            v->face.clear();
        }

        // Rebuild tri list by mapped indexes
        NCount numTris = (GeometryRaw::OT_TRIANGLE_LIST == mSubMesh->getGeometryType()) ?
            mIndexData->mCount / 3 : mIndexData->mCount - 2;

        work.mTriList.clear();
        work.mTriList.reserve(numTris); // reserved tri list

        PMFaceVertex* fvBase = &work.mFaceVertList.front();
        if(GeometryRaw::OT_TRIANGLE_LIST == mSubMesh->getGeometryType())
        {
            for (NCount i = 0, vi = 0; i < numTris; ++i)
            {
                Nui32 vindex;

                PMFaceVertex *v0, *v1, *v2;

                vindex = indexDataVar->getNextIndex(); //get original index from the index buffer
                vindex = mappedIndexes[vindex]; //vindex will replaced if needed

                v0 = fvBase + vindex;
                v0->commonVertex = vBase + vindex;
                v0->realIndex = vindex;

                vindex = indexDataVar->getNextIndex(); //get original index from the index buffer
                vindex = mappedIndexes[vindex]; //vindex will replaced if needed

                v1 = fvBase + vindex;
                v1->commonVertex = vBase + vindex;
                v1->realIndex = vindex;

                vindex = indexDataVar->getNextIndex(); //get original index from the index buffer
                vindex = mappedIndexes[vindex]; //vindex will replaced if needed

                v2 = fvBase + vindex;
                v2->commonVertex = vBase + vindex;
                v2->realIndex = vindex;

                if(v0!=v1 && v1!=v2 && v2!=v0) // see assertion: OgreProgressiveMesh.cpp, line: 723, condition: v0!=v1 && v1!=v2 && v2!=v0
                {
                    work.mTriList.push_back(PMTriangle());
                    work.mTriList.back().setDetails(vi++, v0, v1, v2);
                }
            }
        }
        else if(GeometryRaw::OT_TRIANGLE_STRIP == mSubMesh->getGeometryType())
        {
            bool tSign = true;
            for (NCount i = 0, vi = 0; i < numTris; ++i)
            {
                Nui32 vindex;

                PMFaceVertex *v0, *v1, *v2;

                indexDataVar->offsetToElement(i);
                vindex = indexDataVar->getNextIndex(); //get original index from the index buffer
                vindex = mappedIndexes[vindex]; //vindex will replaced if needed

                v0 = fvBase + vindex;
                v0->commonVertex = vBase + vindex;
                v0->realIndex = vindex;

                vindex = indexDataVar->getNextIndex(); //get original index from the index buffer
                vindex = mappedIndexes[vindex]; //vindex will replaced if needed

                v1 = fvBase + vindex;
                v1->commonVertex = vBase + vindex;
                v1->realIndex = vindex;

                vindex = indexDataVar->getNextIndex(); //get original index from the index buffer
                vindex = mappedIndexes[vindex]; //vindex will replaced if needed

                v2 = fvBase + vindex;
                v2->commonVertex = vBase + vindex;
                v2->realIndex = vindex;

                if(v0!=v1 && v1!=v2 && v2!=v0) // see assertion: OgreProgressiveMesh.cpp, line: 723, condition: v0!=v1 && v1!=v2 && v2!=v0
                {
                    work.mTriList.push_back(PMTriangle());
                    work.mTriList.back().setDetails(vi++, tSign ? v0 : v1, tSign ? v1 : v0, v2);
                }

                tSign = !tSign;
            }
        }
        else //FAN
        {
            for (NCount i = 0, vi = 0; i < numTris; ++i)
            {
                Nui32 vindex;

                PMFaceVertex *v0, *v1, *v2;

                indexDataVar->offsetToElement(0);
                vindex = indexDataVar->getNextIndex(); //get original index from the index buffer
                vindex = mappedIndexes[vindex]; //vindex will replaced if needed

                v0 = fvBase + vindex;
                v0->commonVertex = vBase + vindex;
                v0->realIndex = vindex;

                indexDataVar->offsetToElement(i);
                vindex = indexDataVar->getNextIndex(); //get original index from the index buffer
                vindex = mappedIndexes[vindex]; //vindex will replaced if needed

                v1 = fvBase + vindex;
                v1->commonVertex = vBase + vindex;
                v1->realIndex = vindex;

                vindex = indexDataVar->getNextIndex(); //get original index from the index buffer
                vindex = mappedIndexes[vindex]; //vindex will replaced if needed

                v2 = fvBase + vindex;
                v2->commonVertex = vBase + vindex;
                v2->realIndex = vindex;

                if(v0!=v1 && v1!=v2 && v2!=v0) // see assertion: OgreProgressiveMesh.cpp, line: 723, condition: v0!=v1 && v1!=v2 && v2!=v0
                {
                    work.mTriList.push_back(PMTriangle());
                    work.mTriList.back().setDetails(vi++, v0, v1, v2);
                }
            }
        }

        //update the common unique index list (it should be less than before merge)
        mWorstCosts = newUniqueIndexes;
        mWorstCostsSize = mWorstCosts.size();

        for(WorstCostList::iterator it = mWorstCosts.begin(), end_it = mWorstCosts.end(); it != end_it; ++it)
        {
            PMVertex* v = vBase + it->second;
            if(PMVertex::BS_UNKNOWN == v->mBorderStatus)
                v->initBorderStatus();

            if(0 == (mVertexComponentFlags & (1 << VT_Normals)))
                v->calculateNormal();
#ifdef CHECK_CALCULATED_NORMALS
            else
            {
                Vector3f savedNormal = v->normal;
                Vector3f norl;
                v->calculateNormal();
                savedNormal.normalise(norl);
                N_assert(v->normal.dotProduct(norl) > 0.5f, "error");
                v->normal = savedNormal;
            }
#endif
        }
    }
    //---------------------------------------------------------------------
    bool ProgressiveMesh::collapseInvertsNormals(PMVertex *src, PMVertex *dest) const
    {
        // Degenerate case check
        // Are we going to invert a face normal of one of the neighbouring faces?
        // Can occur when we have a very small remaining edge and collapse crosses it
        // Look for a face normal changing by > 90 degrees
        for(PMVertex::FaceList::iterator srcface = src->face.begin(), srcfaceend = src->face.end(); srcface != srcfaceend; ++srcface)
        {
            // Ignore the deleted faces (those including src & dest)
            if( !(*srcface)->hasCommonVertex(dest) )
            {
                // Test the new face normal
                PMVertex *v0, *v1, *v2;
                // Replace src with dest wherever it is
                v0 = ( (*srcface)->vertex[0]->commonVertex == src) ? dest : (*srcface)->vertex[0]->commonVertex;
                v1 = ( (*srcface)->vertex[1]->commonVertex == src) ? dest : (*srcface)->vertex[1]->commonVertex;
                v2 = ( (*srcface)->vertex[2]->commonVertex == src) ? dest : (*srcface)->vertex[2]->commonVertex;

                // Cross-product 2 edges
                Vector3f e1 = v1->position - v0->position;
                Vector3f e2 = v2->position - v1->position;

                Vector3f newNormal = e1.crossProduct(e2);

                // dot old and new face normal
                // If < 0 then more than 90 degree difference
                if (newNormal.dotProduct( (*srcface)->normal ) < 0.0f )
                    return true;
            }
        }
        return false;
    }
    //---------------------------------------------------------------------
    NIIf ProgressiveMesh::computeEdgeCollapseCost(PMVertex * src, PMVertex * dest) const
    {
        // if we collapse edge uv by moving src to dest then how
        // much different will the model change, i.e. how much "error".
        // The method of determining cost was designed in order
        // to exploit small and coplanar regions for
        // effective polygon reduction.
        Vector3f edgeVector = src->position - dest->position;

        NIIf cost;

        PMVertex::FaceList::iterator srcfacebegin = src->face.begin();
        PMVertex::FaceList::iterator srcfaceend = src->face.end();

        // find the "sides" triangles that are on the edge uv
        PMVertex::FaceList& sides = mEdgeAdjacentSides;
        sides.clear();

        // Iterate over src's faces and find 'sides' of the shared edge which is being collapsed
        PMVertex::FaceList::iterator sidesEnd = sides.end();
        for(PMVertex::FaceList::iterator it = srcfacebegin; it != srcfaceend; ++it)
        {
            // Check if this tri also has dest in it (shared edge)
            PMTriangle* srcface = *it;
            if(srcface->hasCommonVertex(dest) && sidesEnd == std::find(sides.begin(), sidesEnd, srcface))
            {
                sides.push_back(srcface);
                sidesEnd = sides.end();
            }
        }

        // Special cases
        // If we're looking at a border vertex
        if(PMVertex::BS_BORDER == src->mBorderStatus)
        {
            // Check for singular triangle destruction
            if (src->face.size() == 1 && dest->face.size() == 1)
            {
                // If src and dest both only have 1 triangle (and it must be a shared one)
                // then this would destroy the shape, so don't do this
                return NEVER_COLLAPSE_COST;
            }
            else if(collapseInvertsNormals(src, dest))
            {
                // border edge collapse must not invert normals of neighbor faces
                return NEVER_COLLAPSE_COST;
            }
            else if (sides.size() > 1)
            {
                // src is on a border, but the src-dest edge has more than one tri on it
                // So it must be collapsing inwards
                // Mark as very high-value cost
                cost = 1.0f + sides.size() * sides.size() * edgeVector.lengthSquared() * mInvSquaredBoundBoxDiagonal;
            }
            else
            {
                // Collapsing ALONG a border
                // We can't use curvature to measure the effect on the model
                // Instead, see what effect it has on 'pulling' the other border edges
                // The more colinear, the less effect it will have
                // So measure the delta triangle area
                // Normally there can be at most 1 other border edge attached to this
                // However in weird cases there may be more, so find the worst
                Vector3f otherBorderEdge;
                NIIf kinkiness, maxKinkiness; // holds squared parallelogram area
                maxKinkiness = 0.0f;

                for (PMVertex::NeighborList::iterator n = src->neighbor.begin(), nend = src->neighbor.end(); n != nend; ++n)
                {
                    PMVertex* third = *n;
                    if (third != dest && src->isManifoldEdgeWith(third))
                    {
                        otherBorderEdge = src->position - third->position;
                        kinkiness = otherBorderEdge.crossProduct(edgeVector).lengthSquared();    // doubled triangle area
                        maxKinkiness = std::max(kinkiness, maxKinkiness);
                    }
                }

                cost = 0.5f * Math::sqrt(maxKinkiness) * mInvSquaredBoundBoxDiagonal; // cost is equal to normalized triangle area
            }
        }
        else // not a border
        {
            if(collapseInvertsNormals(src, dest))
            {
                // border edge collapse must not invert normals of neighbor faces
                return NEVER_COLLAPSE_COST;
            }

            // each neighbour face sweep some tetrahedron duaring collapsing, we will try to minimize sum of their side areas
            // benefit of this metric is that it`s zero on any ridge, if collapsing along it
            // V = H * S / 3
            NIIf tripleVolumeSum = 0.0;
            NIIf areaSum = 0.0;
            for(PMVertex::FaceList::iterator srcface = srcfacebegin; srcface != srcfaceend; ++srcface)
            {
                PMTriangle* t = *srcface;
                tripleVolumeSum += Math::Abs(t->normal.dotProduct(edgeVector)) * t->area;
                areaSum += t->area;
            }

            // visible error is tetrahedron side area, so taking A = sqrt(S) we use such approximation
            // E = H * sqrt(S) / 2            (side triangle area)
            // and since H = 3 * V / S        (pyramid volume)
            // E = 3 * V / (2 * sqrt(S))
            cost = areaSum > 1e-06f ? tripleVolumeSum / (2.0f * Math::sqrt(areaSum)) * mInvSquaredBoundBoxDiagonal : 0.0f;
        }

        // take into consideration texture coordinates and normal direction
        if(mVertexComponentFlags & (1 << VT_Normals))
        {
            NIIf uvCost = 1.0f, normalCost = 1.0f;

            for(PMVertex::NeighborList::iterator ni = src->neighbor.begin(), nend = src->neighbor.end(); ni != nend; ++ni)
            {
                PMVertex* v0 = *ni;
                PMVertex::NeighborList::iterator inext = ni + 1;
                PMVertex* v1 = (inext != nend) ? *inext : src->neighbor.front(); // next or first

                Vector3f en(v0->position - dest->position);
                Vector3f en1(v1->position - dest->position);
                if(en == Vector3f::ZERO || en1 == Vector3f::ZERO || v0 == v1)
                    continue; //skip collapser

                // get first plane
                Plane p(src->normal.crossProduct(en), dest->position);

                // get second plane
                Plane p1(src->normal.crossProduct(en1), dest->position);

                NIIf pDistance = p.distance(src->position);
                NIIf p1Distance = p1.distance(src->position);

                if((pDistance > 0.0f && p1Distance > 0.0f) || (pDistance < 0.0f && p1Distance < 0.0f))
                    continue; //collapser NOT between two planes

                //calculate barycentric coordinates (b1, b2, b3)

                NIIf & h1 = pDistance;
                NIIf H1 = p.distance(v1->position);
                NIIf b1 = h1 / H1;

                NIIf & h2 = p1Distance;
                NIIf H2 = p1.distance(v0->position);
                NIIf b2 = h2 / H2;

                NIIf b3 = 1.0f - b1 - b2;

                if(b1 < 0.0f || b2 < 0.0f || b3 < 0.0f) continue;

                //three points: dest, *i, v1
                if(mVertexComponentFlags & (1 << VT_Tex_Coord))
                {
                    Vector2f newUV = v1->uv * b1 + v0->uv * b2 + dest->uv * b3;
                    uvCost = std::max(NIIf(1.0f), (newUV - src->uv).lengthSquared());
                }

                Vector3f newNormal;
                (b1 * v1->normal + b2 * v0->normal + b3 * dest->normal).normalise(newNormal);
                normalCost = std::max(NIIf(0.0f), 0.5f - 0.5f * newNormal.dotProduct(src->normal));

                N_assert(cost >= 0.0f && uvCost >= 0.0f && normalCost >= 0.0f, "error");
                break;
            }

#ifdef IGNORE_UV_AND_NORMAL_COSTS
            uvCost = normalCost = 0.0f;
#endif

            const static NIIf posWeight = 10.0f;
            const static NIIf normWeight = 1.0f;
            const static NIIf texWeight = 1.0f;
            const static NIIf invSumWeight = 1.0f / (posWeight + normWeight + texWeight);


            cost = invSumWeight * (posWeight * cost + texWeight * uvCost + normWeight * normalCost);

        }

        N_assert (cost >= 0, "error");

        return cost;
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::initialiseEdgeCollapseCosts(void)
    {
        for(WorkingDataList::iterator i = mWorkingData.begin(), iend = mWorkingData.end(); i != iend; ++i)
        {
            PMVertex* vFront = &i->mVertList.front();
            for(WorstCostList::iterator it = mWorstCosts.begin(), end_it = mWorstCosts.end(); it != end_it; ++it)
            {
                PMVertex* v = vFront + it->second;
                v->collapseTo = NULL;
                v->collapseCost = NEVER_COLLAPSE_COST;
            }
        }
    }
    //---------------------------------------------------------------------
    NIIf ProgressiveMesh::computeEdgeCostAtVertexForBuffer(PMVertex * v)
    {
        // compute the edge collapse cost for all edges that start
        // from vertex v.  Since we are only interested in reducing
        // the object by selecting the min cost edge at each step, we
        // only cache the cost of the least cost edge at this vertex
        // (in member variable collapse) as well as the value of the
        // cost (in member variable objdist).

        if(v->neighbor.empty())
        {
            // v doesn't have neighbors so nothing to collapse
            v->notifyRemoved();
            return v->collapseCost;
        }

        // Init metrics
        NIIf collapseCost = NEVER_COLLAPSE_COST;
        PMVertex * collapseTo = NULL;

        // search all neighboring edges for "least cost" edge
        for(PMVertex **n = &v->neighbor.front(), **nend = n + v->neighbor.size(); n != nend; ++n)
        {
            PMVertex * candidate = *n;
            NIIf cost = computeEdgeCollapseCost(v, candidate);
            if( (!collapseTo) || cost < collapseCost)
            {
                collapseTo = candidate;  // candidate for edge collapse
                collapseCost = cost;     // cost of the collapse
            }
        }

        v->collapseCost = collapseCost;
        v->collapseTo = collapseTo;
        return collapseCost;
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::computeAllCosts(void)
    {
        initialiseEdgeCollapseCosts();

        N_assert(!mWorstCosts.empty(), "error");
        for(WorstCostList::iterator it = mWorstCosts.begin(), end_it = mWorstCosts.end(); it != end_it; ++it)
            it->first = computeEdgeCostAtVertex(it->second);

        sortIndexesByCost();
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::collapse(ProgressiveMesh::PMVertex *src)
    {
        PMVertex *dest = src->collapseTo;

        if(PMVertex::BS_BORDER == src->mBorderStatus)
            dest->mBorderStatus = PMVertex::BS_BORDER;

        // Abort if we're never supposed to collapse
        if (src->collapseCost == NEVER_COLLAPSE_COST)
            return;

        // Remove this vertex from the running for the next check
        src->collapseTo = NULL;
        src->collapseCost = NEVER_COLLAPSE_COST;

        // Collapse the edge uv by moving vertex u onto v
        // Actually remove tris on uv, then update tris that
        // have u to have v, and then remove u.
        if(!dest) {
            // src is a vertex all by itself
#if LOG_PROGRESSIVE_MESH_GENERATION
            ofdebug << "Aborting collapse, orphan vertex. " << std::endl;
#endif
            return;
        }

        // Add dest and all the neighbours of source and dest to recompute list
        typedef vector<PMVertex*>::type RecomputeSet;
        RecomputeSet recomputeSet;
        recomputeSet.reserve(1 + src->neighbor.size() + dest->neighbor.size());
        recomputeSet.push_back(dest);
        recomputeSet.insert(recomputeSet.end(), src->neighbor.begin(), src->neighbor.end());
        recomputeSet.insert(recomputeSet.end(), dest->neighbor.begin(), dest->neighbor.end());

        // delete triangles on edge src-dest
        // Notify others to replace src with dest
        // Queue of faces for removal / replacement
        // prevents us screwing up the iterators while we parse
        PMVertex::FaceList faceRemovalList, faceReplacementList;

        for(PMVertex::FaceList::iterator f = src->face.begin(), fend = src->face.end(); f != fend; ++f)
        {
            if((*f)->hasCommonVertex(dest))
            {
                // Tri is on src-dest therefore is gone
                if(faceRemovalList.end() == std::find(faceRemovalList.begin(), faceRemovalList.end(), *f))
                    faceRemovalList.push_back(*f);
                // Reduce index count by 3 (useful for quick allocation later)

                mCurrNumIndexes -= 3;
            }
            else
            {
                // Only src involved, replace with dest
                if(faceReplacementList.end() == std::find(faceReplacementList.begin(), faceReplacementList.end(), *f))
                    faceReplacementList.push_back(*f);
            }
        }

        src->toBeRemoved = true;
        // Replace all the faces queued for replacement

        for(PMVertex::FaceList::iterator f = faceReplacementList.begin(), fend = faceReplacementList.end(); f != fend; ++f)
        {
            /* Locate the face vertex which corresponds with the common 'dest' vertex
            To to this, find a removed face which has the FACE vertex corresponding with
            src, and use it's FACE vertex version of dest.
            */
            PMFaceVertex* srcFaceVert = (*f)->getFaceVertexFromCommon(src);
            PMFaceVertex* destFaceVert = NULL;

            PMVertex::FaceList::iterator frlend = faceRemovalList.end();

            for(PMVertex::FaceList::iterator iremoved = faceRemovalList.begin(); iremoved != frlend; ++iremoved)
            {
                //if ( (*iremoved)->hasFaceVertex(srcFaceVert) )
                //{
                    destFaceVert = (*iremoved)->getFaceVertexFromCommon(dest);
                //}
            }

            N_assert(destFaceVert, "error");

#if LOG_PROGRESSIVE_MESH_GENERATION
            ofdebug << "Replacing vertex on face " << (Nui32)(*f)->index << std::endl;
#endif
            (*f)->replaceVertex(srcFaceVert, destFaceVert);
        }

        // Remove all the faces queued for removal
        for(PMVertex::FaceList::iterator f = faceRemovalList.begin(), fend = faceRemovalList.end(); f != fend; ++f)
        {
#if LOG_PROGRESSIVE_MESH_GENERATION
            ofdebug << "Removing face " << (Nui32)(*f)->index << std::endl;
#endif
            (*f)->notifyRemoved();
        }

        // Notify the vertex that it is gone
        src->notifyRemoved();

        // invalidate neighbour`s costs
        for(RecomputeSet::iterator it = recomputeSet.begin(), it_end = recomputeSet.end(); it != it_end; ++it)
        {
            PMVertex* v = *it;
            if(!mInvalidCostMask.getBit(v->index))
            {
                ++mInvalidCostCount;
                mInvalidCostMask.setBit(v->index);
            }
        }
    }
    //---------------------------------------------------------------------
    NIIf ProgressiveMesh::computeEdgeCostAtVertex(NCount vertIndex)
    {
        // Call computer for each buffer on this vertex
        NIIf worstCost = -0.01f;
        WorkingDataList::iterator i, iend = mWorkingData.end();
        for(i = mWorkingData.begin(); i != iend; ++i)
        {
            worstCost = std::max(worstCost,
                computeEdgeCostAtVertexForBuffer(&(*i).mVertList[vertIndex]));
        }

        // Return the worst cost
        return worstCost;
    }
    //---------------------------------------------------------------------
    NCount ProgressiveMesh::getInvalidCostCount(ProgressiveMesh::ProgressiveMeshList & pmList)
    {
        NCount invalidCostCount = 0;

        for(ProgressiveMeshList::iterator pmItr = pmList.begin(); pmItr != pmList.end(); ++pmItr)
            invalidCostCount += (*pmItr)->mInvalidCostCount;

        return invalidCostCount;
    }
    //---------------------------------------------------------------------
    bool ProgressiveMesh::recomputeInvalidCosts(ProgressiveMeshList& pmList)
    {
        bool isRecomputed = false;
        for(ProgressiveMeshList::iterator n = pmList.begin(); n != pmList.end(); ++n)
        {
            if((*n)->mInvalidCostCount != 0)
            {
                (*n)->recomputeInvalidCosts();
                isRecomputed = true;
            }
        }
        return isRecomputed;
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::recomputeInvalidCosts()
    {
        if(mInvalidCostCount > 0)
        {
            for(WorstCostList::iterator it = mWorstCosts.begin(), end_it = mWorstCosts.end(); it != end_it; ++it)
            {
                unsigned idx = it->second;
                if(mInvalidCostMask.getBit(idx))
                    it->first = computeEdgeCostAtVertex(idx);
            }
            sortIndexesByCost();
            mInvalidCostMask.clearAllBits();
            mInvalidCostCount = 0;
        }
        mNextWorstCostHint = 0;
    }
    //---------------------------------------------------------------------
    NIIi ProgressiveMesh::cmpByCost(const void * p1, const void * p2)
    {
        NIIf c1 = ((CostIndexPair *)p1)->first;
        NIIf c2 = ((CostIndexPair *)p2)->first;
        return (c1 < c2) ? -1 : c1 > c2;
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::sortIndexesByCost()
    {
        // half of total collapsing time is spended in this function for 700 000 triangles mesh, other
        // half in computeEdgeCostAtVertex. qsort is used instead of std::sort due to performance reasons
        qsort(&mWorstCosts.front(), mWorstCostsSize, sizeof(CostIndexPair), cmpByCost);

        // remove all vertices with NEVER_COLLAPSE_COST to reduce active vertex set
        while(mWorstCostsSize > 0 && mWorstCosts.back().first == NEVER_COLLAPSE_COST)
        {
            mWorstCosts.pop_back();
            --mWorstCostsSize;
        }
    }
    //---------------------------------------------------------------------
    ProgressiveMesh::CostIndexPair* ProgressiveMesh::getNextCollapser()
    {
        // as array is sorted by cost and only partially invalidated  - return first valid cost, it would be the best
        for(CostIndexPair* ptr = &mWorstCosts.front() + mNextWorstCostHint; mNextWorstCostHint  < mWorstCostsSize; ++ptr, ++mNextWorstCostHint)
            if(!mInvalidCostMask.getBit(ptr->second))
                return ptr;

        // no valid costs
        static CostIndexPair dontCollapse(NEVER_COLLAPSE_COST, 0);
        return &dontCollapse;
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::getNextCollapser(ProgressiveMesh::ProgressiveMeshList & pmList,
        ProgressiveMesh *& pm, CostIndexPair *& bestCollapser)
    {
        pm = NULL;
        bestCollapser = NULL;
        NIIf bestCost = NEVER_COLLAPSE_COST;

        for(ProgressiveMeshList::iterator pmItr = pmList.begin(); pmItr != pmList.end(); )
        {
            ProgressiveMesh* pmCur = *pmItr;

            CostIndexPair* collapser = pmCur->getNextCollapser();
            NIIf cost = collapser->first;

            if(NEVER_COLLAPSE_COST == cost)
            {
                if(pmCur->mInvalidCostCount != 0)
                {
                    ++pmItr;
                }
                else
                {
                    pmItr = pmList.erase(pmItr);
                }
            }
            else
            {
                if(cost < bestCost)
                {
                    bestCost = cost;
                    bestCollapser = collapser;
                    pm = pmCur;
                }

                ++pmItr;
            }
        }
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::bakeNewLOD(IndexData * pData)
    {
        N_assert(mCurrNumIndexes > 0, "No triangles to bake!");
        // Zip through the tri list of any working data copy and bake
        pData->mCount = mCurrNumIndexes;
        pData->mOffset = 0;
        // Base size of indexes on original
        bool use32bitindexes = (mIndexData->mBuffer->getUnitSize() == 32);

        // Create index buffer, we don't need to read it back or modify it a lot
        N_Only(GBuffer).create(pData->mBuffer, use32bitindexes? 32 : 16, pData->mCount, Buffer::M_NORMAL );

        IndexBufferLockGuard lockGuard(pData->mBuffer, 0, pData->mBuffer->getSize(),
            Buffer::MM_WHOLE);

        TriangleList::iterator tri, triend;
        // Use the first working data buffer, they are all the same index-wise
        WorkingDataList::iterator pWork = mWorkingData.begin();
        triend = pWork->mTriList.end();

        if (use32bitindexes)
        {
            Nui32* pInt = (Nui32*)lockGuard.pData;
            for (tri = pWork->mTriList.begin(); tri != triend; ++tri)
            {
                if (!tri->removed)
                {
                    *pInt++ = (Nui32)tri->vertex[0]->realIndex;
                    *pInt++ = (Nui32)tri->vertex[1]->realIndex;
                    *pInt++ = (Nui32)tri->vertex[2]->realIndex;
                }
            }
        }
        else
        {
            Nui16* pShort = (Nui16*)lockGuard.pData;
            for (tri = pWork->mTriList.begin(); tri != triend; ++tri)
            {
                if (!tri->removed)
                {
                    *pShort++ = (Nui16)tri->vertex[0]->realIndex;
                    *pShort++ = (Nui16)tri->vertex[1]->realIndex;
                    *pShort++ = (Nui16)tri->vertex[2]->realIndex;
                }
            }
        }
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::bakeLodUsage(Mesh * pMesh, MeshLodScheme * scheme,
        const LodValueList & lodValues, bool skipFirstLodLevel)
    {
        // Iterate over the lods and record usage
        LodValueList::const_iterator i = lodValues.begin();
        LodValueList::const_iterator iend = lodValues.end();
        if(skipFirstLodLevel && i != iend)
            ++i;

        pMesh->setLodCount(1 + lodValues.size());

        Nidx lodLevel = 1;
        for(; i != iend; ++i)
        {
            // Record usage
            pMesh->getLodScheme()->add(*i, 0, 0, 0, true);
        }
        pMesh->setLodScheme(scheme);
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::createSimplifiedVertexData(vector<IndexVertexPair>::type & usedVertices,
        VertexData * inVData, VertexData *& outVData, AABox & aabox)
    {
        outVData = NULL;

        VertexDataVariantList vdInVariantList;

        VertexDataVariant * inVertexDataBuffer = vdInVariantList.create(inVData, VT_Pos);
        N_assert(inVertexDataBuffer->isValid(), "error");
        if(!inVertexDataBuffer->isValid())
            return;
        VertexDataVariant * inNormalDataBuffer = vdInVariantList.create(inVData, VT_Normals);
        VertexDataVariant * inUvDataBuffer = vdInVariantList.create(inVData, VT_Tex_Coord);

        static const Nui16 source = 0;

        N_Only(GBuffer).create(outVData);
        outVData->mOffset = 0;
        outVData->mCount = usedVertices.size();

        NCount offset = 0;
        offset += outVData->add(DT_Float3x, VT_Pos, source, offset)->getSize();
        if(inNormalDataBuffer->isValid())
            offset += outVData->add(DT_Float3x, VT_Normals, source, offset)->getSize();
        if(inUvDataBuffer->isValid())
            offset += outVData->add(DT_Float2x, VT_Tex_Coord, source, offset)->getSize();

        N_assert(0 != offset, "error");

        VertexBuffer * outVbuffer;
        N_Only(GBuffer).create(outVbuffer, outVData->getUnitSize(source), outVData->mCount, Buffer::M_NORMAL );

        VertexBufferLockGuard outVdataLock(outVbuffer, Buffer::MM_WHOLE);
        NIIf* p = (NIIf*)outVdataLock.pData;

        N_assert(outVData->mCount == usedVertices.size(), "error");

        for (vector<IndexVertexPair>::type::iterator it = usedVertices.begin(); it != usedVertices.end(); ++it)
        {
            unsigned idx = it->first;

            Vector3f v3;
            Vector2f uv;

            inVertexDataBuffer->offsetToElement(idx);
            v3 = inVertexDataBuffer->getNextVector3();
            *p++ = v3.x;
            *p++ = v3.y;
            *p++ = v3.z;
            aabox.merge(v3);

#ifndef CHECK_CALCULATED_NORMALS
            if(inNormalDataBuffer->isValid())
            {
                //store original normals
                inNormalDataBuffer->offsetToElement(idx);
                v3 = inNormalDataBuffer->getNextVector3();
                *p++ = v3.x;
                *p++ = v3.y;
                *p++ = v3.z;
            }
            else
            {
                //store calculated normals
                PMVertex* v = it->second;
                *p++ = v->normal.x;
                *p++ = v->normal.y;
                *p++ = v->normal.z;
            }
#else
            N_assert(inNormalDataBuffer->isValid(), "error"); //test is possible only if normals is present in input data
            if(inNormalDataBuffer->isValid())
            {
                //store original normals
                inNormalDataBuffer->offsetToElement(idx);
                v3 = inNormalDataBuffer->getNextVector3();
            }

            //store calculated normals
            PMVertex * v = it->second;
            Vector3f norl;
            v->normal.normalise(norl);
            N_assert(v3.dotProduct(norl) > 0.5f, "error");
            *p++ = v->normal.x;
            *p++ = v->normal.y;
            *p++ = v->normal.z;
#endif
            if(inUvDataBuffer->isValid())
            {
                inUvDataBuffer->offsetToElement(idx);
                uv = inUvDataBuffer->getNextVector2();
                *p++ = uv.x;
                *p++ = uv.y;
            }
        }

        outVData->attach(source, outVbuffer);
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::createIndexMap(vector<IndexVertexPair>::type& usedVertices,
        unsigned allVertexCount, vector<unsigned>::type& indexMap)
    {
        // we re-index used vertices and store old-to-new mapping in index map
        indexMap.resize(allVertexCount);
        memset(&indexMap[0], 0, allVertexCount * sizeof(unsigned));

        NCount n = 0;
        for(vector<ProgressiveMesh::IndexVertexPair>::type::iterator it = usedVertices.begin(); it != usedVertices.end(); ++it)
            indexMap[it->first] = n++;
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::bakeSimplifiedMesh(Mesh * outMesh, Mesh * inMesh, ProgressiveMeshList& pmList, bool dropFirstLodLevel)
    {
        N_assert(inMesh && outMesh, "error");

        AABox outAabox;

        vector<IndexVertexPair>::type inCommonIndexes;
        BitArray usedVertices, usedBySubMesh;
        if(inMesh->getVertexData())
        {
            usedVertices.resize(inMesh->getVertexData()->mCount);
            usedBySubMesh.resize(inMesh->getVertexData()->mCount);
        }

        for(NCount i = 0; i < inMesh->getSubCount(); ++i)
        {
            SubMesh* inSubMesh = inMesh->getSub(i);
            if(inSubMesh->isSelfVertex())
                continue;

            ProgressiveMesh* pm = pmList[i];
            if(NULL == pm->mSubMesh)
                continue; // dummy, skip it

            IndexDataVariantSharedPtr inSubMeshIndexDataVar(IndexDataVariant::create(dropFirstLodLevel ? pm->mLodFaceList.front() : inSubMesh->getIndexData(0)));
            N_assert(!inSubMeshIndexDataVar.isNull(), "error");

            usedBySubMesh.clearAllBits();
            inSubMeshIndexDataVar->markUsedVertices(usedBySubMesh);

            for(unsigned idx = 0, end_idx = inMesh->getVertexData()->mCount; idx < end_idx; ++idx)
            {
                if(usedBySubMesh.getBit(idx) && !usedVertices.getBit(idx))
                {
                    usedVertices.setBit(idx);
                    inCommonIndexes.push_back(Npair(idx, &pm->mWorkingData.front().mVertList.front() + idx));
                }
            }
        }

        // note - we don`t preserve initial vertex order, but group vertices by submesh instead, improving locality
        vector<unsigned>::type sharedIndexMap; // between submeshes

        if(!inCommonIndexes.empty())
        {
            VertexData * outvet;
            createSimplifiedVertexData(inCommonIndexes,
                inMesh->getVertexData(), outvet, outAabox);
            outMesh->setVertexData(outvet);
            createIndexMap(inCommonIndexes, inMesh->getVertexData()->mCount,
                sharedIndexMap);
        }

        for(NCount i = 0; i < inMesh->getSubCount(); ++i)
        {
            SubMesh * inSubMesh = inMesh->getSub(i);

            SubMesh * outSubMesh = outMesh->createSub();
            outSubMesh->setMaterial(inSubMesh->getMaterial());

            outSubMesh->setSelfVertex(inSubMesh->isSelfVertex());

            ProgressiveMesh * pm = pmList[i];
            if(NULL == pm->mSubMesh)
                continue; // dummy, skip it

            IndexDataVariantSharedPtr inSubMeshIndexDataVar(IndexDataVariant::create(dropFirstLodLevel ? pm->mLodFaceList.front() : inSubMesh->getIndexData(0)));
            N_assert(!inSubMeshIndexDataVar.isNull(), "error");

            vector<unsigned>::type localIndexMap; // to submesh
            vector<unsigned>::type * indexMap = &sharedIndexMap;

            if(outSubMesh->isSelfVertex())
            {
                usedVertices.resize(inSubMesh->getVertexData()->mCount);
                inSubMeshIndexDataVar->markUsedVertices(usedVertices);

                vector<IndexVertexPair>::type inSubMeshIndexes;
                for(unsigned idx = 0, end_idx = inSubMesh->getVertexData()->mCount; idx < end_idx; ++idx)
                    if(usedVertices.getBit(idx))
                        inSubMeshIndexes.push_back(Npair(idx, &pm->mWorkingData.front().mVertList.front() + idx));
                VertexData * osmvd;
                createSimplifiedVertexData(inSubMeshIndexes, inSubMesh->getVertexData(),
                    osmvd, outAabox);
                outSubMesh->setVertexData(osmvd);
                indexMap = &localIndexMap;
                createIndexMap(inSubMeshIndexes, inSubMesh->getVertexData()->mCount, localIndexMap);
            }

            if(outSubMesh->isSelfVertex() && !outSubMesh->getVertexData())
            {
                N_EXCEPT(InvalidState,
                    _I18n("Submesh does not have any vertex data"));
            }

            bool outUse16bitIndexes = (outSubMesh->isSelfVertex() ? outSubMesh->getVertexData() : outMesh->getVertexData())->mCount <= 0xFFFF;

            inSubMeshIndexDataVar->createIndexData(outSubMesh->getIndexData(0), outUse16bitIndexes, indexMap);
            inSubMeshIndexDataVar.setNull();

            // clone all LODs, may be without first
            vector<IndexData *>::type::iterator n = pm->mLodFaceList.begin();
            if(dropFirstLodLevel && n != pm->mLodFaceList.end())
                ++n;
            for(; n != pm->mLodFaceList.end(); ++n)
            {
                IndexDataVariantSharedPtr lodIndexDataVar(IndexDataVariant::create(*n, Buffer::MM_WHOLE));

                IndexData * lod = N_new IndexData;
                lodIndexDataVar->createIndexData(lod, outUse16bitIndexes, indexMap);
                outSubMesh->addIndexData(lod);
            }
        }

        // Store bbox and sphere
        outMesh->setAABB(outAabox);
        outMesh->setCenterRange((outAabox.getMax() - outAabox.getMin()).length() / 2.0f);
    }
    //---------------------------------------------------------------------
    ProgressiveMesh::PMTriangle::PMTriangle() :
        removed(false)
    {
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::PMTriangle::setDetails(NCount newindex,
        ProgressiveMesh::PMFaceVertex * v0, ProgressiveMesh::PMFaceVertex * v1,
        ProgressiveMesh::PMFaceVertex * v2)
    {
        N_assert(v0!=v1 && v1!=v2 && v2!=v0, "error");

        removed = false;

        index = newindex;
        vertex[0]=v0;
        vertex[1]=v1;
        vertex[2]=v2;

        computeNormal();

        // Add tri to vertices
        // Also tell vertices they are neighbours
        for(NIIi i=0;i<3;i++)
        {
            PMVertex* vv = vertex[i]->commonVertex;

            if(vv->face.end() == std::find(vv->face.begin(), vv->face.end(), this))
                vv->face.push_back(this);

            for(NIIi j=0;j<3;j++) if(i!=j) {
                if(vv->neighbor.end() == std::find(vv->neighbor.begin(), vv->neighbor.end(), vertex[j]->commonVertex))
                    vv->neighbor.push_back(vertex[j]->commonVertex);
            }
        }
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::PMTriangle::notifyRemoved(void)
    {
        NIIi i;
        for(i=0; i<3; i++) 
        {
            // remove this tri from the vertices
            if(vertex[i])
                vertex[i]->commonVertex->face.erase(
                    std::remove(vertex[i]->commonVertex->face.begin(), vertex[i]->commonVertex->face.end(), this),
                    vertex[i]->commonVertex->face.end()
                                                    );
        }
        for(i=0; i<3; i++) {
            NIIi i2 = (i+1)%3;
            if(!vertex[i] || !vertex[i2]) continue;
            // Check remaining vertices and remove if not neighbours anymore
            // NB May remain neighbours if other tris link them
            vertex[i ]->commonVertex->removeIfNonNeighbor(vertex[i2]->commonVertex);
            vertex[i2]->commonVertex->removeIfNonNeighbor(vertex[i ]->commonVertex);
        }

        removed = true;
    }
    //---------------------------------------------------------------------
    bool ProgressiveMesh::PMTriangle::hasCommonVertex(ProgressiveMesh::PMVertex * v) const
    {
        return (v == vertex[0]->commonVertex || v == vertex[1]->commonVertex ||
            v == vertex[2]->commonVertex);
    }
    //---------------------------------------------------------------------
    bool ProgressiveMesh::PMTriangle::hasFaceVertex(ProgressiveMesh::PMFaceVertex * v) const
    {
        return (v == vertex[0] || v == vertex[1] || v == vertex[2]);
    }
    //---------------------------------------------------------------------
    ProgressiveMesh::PMFaceVertex *
    ProgressiveMesh::PMTriangle::getFaceVertexFromCommon(ProgressiveMesh::PMVertex * commonVert)
    {
        if (vertex[0]->commonVertex == commonVert) return vertex[0];
        if (vertex[1]->commonVertex == commonVert) return vertex[1];
        if (vertex[2]->commonVertex == commonVert) return vertex[2];

        return NULL;
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::PMTriangle::computeNormal()
    {
        Vector3f v0=vertex[0]->commonVertex->position;
        Vector3f v1=vertex[1]->commonVertex->position;
        Vector3f v2=vertex[2]->commonVertex->position;
        // Cross-product 2 edges
        Vector3f e1 = v1 - v0;
        Vector3f e2 = v2 - v1;

        normal = e1.crossProduct(e2);
        area = normal.normalise();
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::PMTriangle::replaceVertex(
        ProgressiveMesh::PMFaceVertex *vold, ProgressiveMesh::PMFaceVertex * vnew)
    {
        N_assert(vold && vnew, "error");
        N_assert(vold==vertex[0] || vold==vertex[1] || vold==vertex[2], "error");
        N_assert(vnew!=vertex[0] && vnew!=vertex[1] && vnew!=vertex[2], "error");
        if(vold==vertex[0]){
            vertex[0]=vnew;
        }
        else if(vold==vertex[1]){
            vertex[1]=vnew;
        }
        else {
            N_assert(vold==vertex[2], "error");
            vertex[2]=vnew;
        }
        NIIi i;
        vold->commonVertex->face.erase(std::remove(vold->commonVertex->face.begin(), vold->commonVertex->face.end(), this), vold->commonVertex->face.end());
        if(vnew->commonVertex->face.end() == std::find(vnew->commonVertex->face.begin(), vnew->commonVertex->face.end(), this))
            vnew->commonVertex->face.push_back(this);

        for(i=0;i<3;i++)
        {
            vold->commonVertex->removeIfNonNeighbor(vertex[i]->commonVertex);
            vertex[i]->commonVertex->removeIfNonNeighbor(vold->commonVertex);
        }

        for(i=0;i<3;i++) {
            N_assert(std::find(vertex[i]->commonVertex->face.begin(), vertex[i]->commonVertex->face.end(), this) != vertex[i]->commonVertex->face.end(), "error");
            for(NIIi j=0;j<3;j++) if(i!=j) {
#if LOG_PROGRESSIVE_MESH_GENERATION
                ofdebug << "Adding vertex " << (Nui32)vertex[j]->commonVertex->index << " to the neighbor list "
                    "of vertex " << (Nui32)vertex[i]->commonVertex->index << std::endl;
#endif
                if(vertex[i]->commonVertex->neighbor.end() ==
                   std::find(vertex[i]->commonVertex->neighbor.begin(), vertex[i]->commonVertex->neighbor.end(), vertex[j]->commonVertex))
                    vertex[i]->commonVertex->neighbor.push_back(vertex[j]->commonVertex);
            }
        }
        computeNormal();
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::PMVertex::setDetails(NCount newindex, const Vector3f & pos,
        const Vector3f & n, const Vector2f & texUV)
    {
        position = pos;
        normal = n;
        uv = texUV;
        index = newindex;

        removed = false;
        toBeRemoved = false;
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::PMVertex::calculateNormal()
    {
        Vector3f weightedSum(Vector3f::ZERO);    // sum of neighbour face normals weighted with neighbour area

        for(PMVertex::FaceList::iterator i = face.begin(), iend = face.end(); i != iend; ++i)
        {
            PMTriangle* t = *i;
            weightedSum += t->normal * t->area;    // accumulate neighbour face normals weighted with neighbour area
        }

        // there is small possibility, that weigtedSum is very close to zero, use arbitrary normal value in such case
        normal = weightedSum.normalise() > 1e-08 ? weightedSum : Vector3f::Z;
    }
    //---------------------------------------------------------------------
    bool ProgressiveMesh::PMVertex::isNearEnough(PMVertex * other) const
    {
        return position == other->position && normal.dotProduct(other->normal) > 0.8f &&
            (uv - other->uv).lengthSquared() < 0.01f;
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::PMVertex::notifyRemoved(void)
    {
        // Removes possible self listing as neighbor first
        neighbor.erase(std::remove(neighbor.begin(), neighbor.end(), this), neighbor.end());
        for (PMVertex::NeighborList::iterator n = neighbor.begin(); n != neighbor.end(); ++n)
        {
            // Remove me from neighbor
            (*n)->neighbor.erase(std::remove((*n)->neighbor.begin(), (*n)->neighbor.end(), this), (*n)->neighbor.end());
        }
        removed = true;
        collapseTo = NULL;
        collapseCost = NEVER_COLLAPSE_COST;
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::PMVertex::initBorderStatus()
    {
        N_assert(mBorderStatus == BS_UNKNOWN, "error logic");

        // Look for edges which only have one tri attached, this is a border

        NeighborList::iterator nend = neighbor.end();

        // Loop for each neighbor
        for(NeighborList::iterator n = neighbor.begin(); n != nend; ++n)
        {
            // Count of tris shared between the edge between this and neighbor
            Nui16 count = 0;
            // Loop over each face, looking for shared ones
            FaceList::iterator fend = face.end();
            for(FaceList::iterator j = face.begin(); j != fend; ++j)
            {
                if((*j)->hasCommonVertex(*n))
                {
                    // Shared tri
                    ++count;
                }
            }
            //N_assert(count>0, "error"); // Must be at least one!
            // This edge has only 1 tri on it, it's a border
            if(count == 1)
            {
                mBorderStatus = BS_BORDER;
                return;
            }
        }

        mBorderStatus = BS_NOT_BORDER;
    }
    //---------------------------------------------------------------------
    bool ProgressiveMesh::PMVertex::isManifoldEdgeWith(ProgressiveMesh::PMVertex * v)
    {
        // Check the sides involving both these verts
        // If there is only 1 this is a manifold edge
       Nui16 sidesCount = 0;

        FaceList::iterator fend = face.end();
        for (FaceList::iterator i = face.begin(); i != fend; ++i)
        {
            if ((*i)->hasCommonVertex(v))
            {
                sidesCount++;
            }
        }

        return (sidesCount == 1);
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::PMVertex::removeIfNonNeighbor(ProgressiveMesh::PMVertex * n)
    {
        // removes n from neighbor list if n isn't a neighbor.
        NeighborList::iterator i = std::find(neighbor.begin(), neighbor.end(), n);
        if (i == neighbor.end())
            return; // Not in neighbor list anyway

        FaceList::iterator f, fend;
        fend = face.end();
        for(f = face.begin(); f != fend; ++f)
        {
            if((*f)->hasCommonVertex(n)) return; // Still a neighbor
        }

#if LOG_PROGRESSIVE_MESH_GENERATION
        ofdebug << "Vertex " << (Nui32)n->index << " is no longer a neighbour of vertex " << (Nui32)this->index <<
            " so has been removed from the latter's neighbor list." << std::endl;
#endif

        neighbor.erase(std::remove(neighbor.begin(), neighbor.end(), n), neighbor.end());

        if (neighbor.empty() && !toBeRemoved)
        {
            // This vertex has been removed through isolation (collapsing around it)
            this->notifyRemoved();
        }
    }
    //---------------------------------------------------------------------
    void ProgressiveMesh::dumpContents(const String & log)
    {
        Nofstream ofdump(log.c_str());

        // Just dump 1st working data for now
        WorkingDataList::iterator worki = mWorkingData.begin();

        CommonVertexList::iterator vi, vend;
        vend = worki->mVertList.end();
        ofdump << "-------== VERTEX LIST ==-----------------" << std::endl;

        for(WorstCostList::iterator it = mWorstCosts.begin(), end_it = mWorstCosts.end(); it != end_it; ++it)
        {
            PMVertex* vert = &worki->mVertList[it->second];

            const NIIb* isBorder = (vert->mBorderStatus == PMVertex::BS_BORDER) ? "yes" : "no";

            ofdump << "Vertex " << (Nui32)vert->index << " pos: " << vert->position << " removed: "
                << vert->removed << " isborder: " << isBorder << std::endl;
            ofdump << "    Faces:" << std::endl;

            PMVertex::FaceList::iterator fend = vert->face.end();
            for(PMVertex::FaceList::iterator f = vert->face.begin(); f != fend; ++f)
            {
                ofdump << "    Triangle index " << (Nui32)(*f)->index << std::endl;
            }

            ofdump << "    Neighbours:" << std::endl;
            PMVertex::NeighborList::iterator nend = vert->neighbor.end();
            for (PMVertex::NeighborList::iterator n = vert->neighbor.begin(); n != nend; ++n)
            {
                ofdump << "    Vertex index " << (Nui32)(*n)->index << std::endl;
            }
        }

        TriangleList::iterator ti, tend;
        tend = worki->mTriList.end();
        ofdump << "-------== TRIANGLE LIST ==-----------------" << std::endl;
        for(ti = worki->mTriList.begin(); ti != tend; ++ti)
        {
            ofdump << "Triangle " << (Nui32)ti->index << " norm: " << ti->normal << " removed: " << ti->removed << std::endl;
            ofdump << "    Vertex 0: " << (Nui32)ti->vertex[0]->realIndex << std::endl;
            ofdump << "    Vertex 1: " << (Nui32)ti->vertex[1]->realIndex << std::endl;
            ofdump << "    Vertex 2: " << (Nui32)ti->vertex[2]->realIndex << std::endl;
        }

        ofdump << "-------== COLLAPSE COST LIST ==-----------------" << std::endl;

        for(WorstCostList::iterator it = mWorstCosts.begin(), end_it = mWorstCosts.end(); it != end_it; ++it)
        {
            PMVertex * vert = &worki->mVertList[it->second];

            const NIIb * isBorder = (vert->mBorderStatus == PMVertex::BS_BORDER) ? "yes" : "no";

            ofdump << "Vertex " << (Nui32)vert->index << ", pos: " << vert->position << ", cost: " << vert->collapseCost << ", removed: "
            << vert->removed << ", isborder: " << isBorder << std::endl;
        }
        ofdump.close();
    }
    //----------------------------------------------------------------------
}