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
#include "NiiInstanceMesh.h"
#include "NiiInstanceBatchHW.h"
#include "NiiInstanceBatchHW_VTF.h"
#include "NiiInstanceBatchShader.h"
#include "NiiInstanceBatchVTF.h"
#include "NiiMeshManager.h"
#include "NiiMaterialManager.h"
#include "NiiEngine.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // InstanceMesh
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    InstanceMesh::InstanceMesh(InstanceMeshID imid, SpaceManager * sceneManager,
        ResourceID rid, GroupID gid, InstanceMeshType instancingTechnique, Nui16 instancingFlags,
        NCount instancesPerBatch, Nui16 subMeshIdx, bool useBoneMatrixLookup) :
            mName(imid),
            mIdCount(0),
            mInstancesPerBatch(instancesPerBatch),
            mInstancingTechnique(instancingTechnique),
            mInstancingFlags(instancingFlags),
            mSubMeshIdx(subMeshIdx),
            mSceneManager(sceneManager),/// Ó¦¸ÃÒÆÈ¥
            mMaxLookupTableInstances(16)
    {
        mMeshReference = N_Only(Mesh).load(rid, gid);
        mMeshReference->ref(false);
        if(mMeshReference->isSkeletonFusion() && mMeshReference->getSkeleton())
            mMeshReference->getSub(mSubMeshIdx)->buildSkeleton();
    }
    //-----------------------------------------------------------------------
    InstanceMesh::~InstanceMesh()
    {
        mMeshReference->unref();
        //Remove all batches from all materials we created
        InstanceBatchMap::const_iterator itor = mInstanceBatches.begin();
        InstanceBatchMap::const_iterator end  = mInstanceBatches.end();

        while(itor != end)
        {
            InstanceBatchVec::const_iterator it = itor->second.begin();
            InstanceBatchVec::const_iterator en = itor->second.end();

            while(it != en)
                N_delete *it++;

            ++itor;
        }
    }
    //----------------------------------------------------------------------
    void InstanceMesh::setInstancesPerBatch(NCount instancesPerBatch)
    {
        if(!mInstanceBatches.empty())
        {
            N_EXCEPT(InvalidState, _I18n("Instances per batch can only be changed before building the batch."));
        }

        mInstancesPerBatch = instancesPerBatch;
    }
    //----------------------------------------------------------------------
    void InstanceMesh::setMaxLookupTableInstances(NCount maxLookupTableInstances)
    {
        if(!mInstanceBatches.empty())
        {
            N_EXCEPT(InvalidState, _I18n("Instances per batch can only be changed before"));
        }

        mMaxLookupTableInstances = maxLookupTableInstances;
    }
    //----------------------------------------------------------------------
    NCount InstanceMesh::getMaxOrBestNumInstancesPerBatch(ResourceID rid,
        NCount suggestedSize, Nui16 flags)
    {
        //Get the material
        Material * mat = static_cast<Material *>(N_Only(Material).get(rid, mMeshReference->getGroup()));
        InstanceBatch * batch = 0;

        //Base material couldn't be found
        if(mat == 0)
            return 0;

        switch(mInstancingTechnique)
        {
        case ShaderBased:
            batch = N_new InstanceBatchShader(this, mMeshReference, mat, suggestedSize, 0, 0);
            break;
        case TextureVTF:
            batch = N_new InstanceBatchVTF(this, mMeshReference, mat, suggestedSize, 0, 0);
            static_cast<InstanceBatchVTF *>(batch)->setBoneDualQuaternions((mInstancingFlags & IM_USEBONEDUALQUATERNIONS) != 0);
            static_cast<InstanceBatchVTF *>(batch)->setUseOneWeight((mInstancingFlags & IM_USEONEWEIGHT) != 0);
            static_cast<InstanceBatchVTF *>(batch)->setForceOneWeight((mInstancingFlags & IM_FORCEONEWEIGHT) != 0);
            break;
        case HWInstancingBasic:
            batch = N_new InstanceBatchHW(this, mMeshReference, mat, suggestedSize, 0, 0);
            break;
        case HWInstancingVTF:
            batch = N_new InstanceBatchHW_VTF(this, mMeshReference, mat, suggestedSize, 0, 0);
            static_cast<InstanceBatchHW_VTF *>(batch)->setBoneMatrixLookup((mInstancingFlags & IM_VTFBONEMATRIXLOOKUP) != 0, mMaxLookupTableInstances);
            static_cast<InstanceBatchHW_VTF *>(batch)->setBoneDualQuaternions((mInstancingFlags & IM_USEBONEDUALQUATERNIONS) != 0);
            static_cast<InstanceBatchHW_VTF *>(batch)->setUseOneWeight((mInstancingFlags & IM_USEONEWEIGHT) != 0);
            static_cast<InstanceBatchHW_VTF *>(batch)->setForceOneWeight((mInstancingFlags & IM_FORCEONEWEIGHT) != 0);
            break;
        default:
            N_EXCEPT(UnImpl, _I18n("Unimplemented instancing technique: ") + N_conv(mInstancingTechnique));
        }

        const NCount retVal = batch->calculateMaxNumInstances(mMeshReference->getSub(mSubMeshIdx), flags);

        N_delete batch;
        return retVal;
    }
    //----------------------------------------------------------------------
    InstanceGeo * InstanceMesh::createInstanceGeo(SpaceManager * sm, ResourceID mrid)
    {
        InstanceBatch * instanceBatch;

        if(mInstanceBatches.empty())
            instanceBatch = buildNewBatch(sm, mrid, true);
        else
            instanceBatch = getFreeBatch(sm, mrid);

        return instanceBatch->createInstanceGeo();
    }
    //-----------------------------------------------------------------------
    inline InstanceBatch * InstanceMesh::getFreeBatch(SpaceManager * sm, ResourceID mid)
    {
        InstanceBatchVec & batchVec = mInstanceBatches[mid];

        InstanceBatchVec::const_reverse_iterator itor = batchVec.rbegin();
        InstanceBatchVec::const_reverse_iterator end  = batchVec.rend();

        while(itor != end)
        {
            if(!(*itor)->isBatchFull())
                return *itor;
            ++itor;
        }

        //None found, or they're all full
        return buildNewBatch(sm, mid, false);
    }
    //-----------------------------------------------------------------------
    InstanceBatch * InstanceMesh::buildNewBatch(SpaceManager * sm, ResourceID mid,
        bool firstTime)
    {
        //Get the bone to index map for the batches
        const BoneIndexList * idxMap = &mMeshReference->getSub(mSubMeshIdx)->getMatrixList();
        idxMap = idxMap->empty() ? &mMeshReference->getMatrixList() : idxMap;

        //Get the material
        Material * mat = static_cast<Material *>(N_Only(Material).get(mid, mMeshReference->getGroup()));

        //Get the array of batches grouped by this material
        InstanceBatchVec &materialInstanceBatch = mInstanceBatches[mid];

        InstanceBatch * batch = 0;

        switch(mInstancingTechnique)
        {
        case ShaderBased:
            batch = N_new InstanceBatchShader(this, mMeshReference, mat, mInstancesPerBatch, idxMap, 0);
            break;
        case TextureVTF:
            batch = N_new InstanceBatchVTF(this, mMeshReference, mat, mInstancesPerBatch, idxMap, 0);
            static_cast<InstanceBatchVTF *>(batch)->setBoneDualQuaternions((mInstancingFlags & IM_USEBONEDUALQUATERNIONS) != 0);
            static_cast<InstanceBatchVTF *>(batch)->setUseOneWeight((mInstancingFlags & IM_USEONEWEIGHT) != 0);
            static_cast<InstanceBatchVTF *>(batch)->setForceOneWeight((mInstancingFlags & IM_FORCEONEWEIGHT) != 0);
            break;
        case HWInstancingBasic:
            batch = N_new InstanceBatchHW(this, mMeshReference, mat, mInstancesPerBatch, idxMap, 0);
            break;
        case HWInstancingVTF:
            batch = N_new InstanceBatchHW_VTF(this, mMeshReference, mat, mInstancesPerBatch, idxMap, 0);
            static_cast<InstanceBatchHW_VTF *>(batch)->setBoneMatrixLookup((mInstancingFlags & IM_VTFBONEMATRIXLOOKUP) != 0, mMaxLookupTableInstances);
            static_cast<InstanceBatchHW_VTF *>(batch)->setBoneDualQuaternions((mInstancingFlags & IM_USEBONEDUALQUATERNIONS) != 0);
            static_cast<InstanceBatchHW_VTF *>(batch)->setUseOneWeight((mInstancingFlags & IM_USEONEWEIGHT) != 0);
            static_cast<InstanceBatchHW_VTF *>(batch)->setForceOneWeight((mInstancingFlags & IM_FORCEONEWEIGHT) != 0);
            break;
        default:
            N_EXCEPT(UnImpl, _I18n("Unimplemented instancing technique: ") + N_conv(mInstancingTechnique));
        }

        batch->setManager(sm);

        if(!firstTime)
        {
            //TODO: Check different materials have the same mInstancesPerBatch upper limit
            //otherwise we can't share
            batch->buildFrom(mMeshReference->getSub(mSubMeshIdx), mSharedRenderOperation);
        }
        else
        {
            //Ensure we don't request more than we can
            NCount maxInstPerBatch = batch->calculateMaxNumInstances(mMeshReference->getSub(mSubMeshIdx), mInstancingFlags);
            mInstancesPerBatch = std::min(maxInstPerBatch, mInstancesPerBatch);
            batch->_setInstancesPerBatch(mInstancesPerBatch);

            //TODO: Create a "merge" function that merges all submeshes into one big submesh
            //instead of just sending submesh #0

            //Get the GeometryRaw to be shared with further instances.
            mSharedRenderOperation = batch->build(mMeshReference->getSub(mSubMeshIdx));
        }

        const BatchSettings & batchSettings = mBatchSettings[mid];
        batch->setCastShadow(batchSettings.setting[CAST_SHADOWS]);

        //Batches need to be part of a scene node so that their renderable can be rendered
        SpaceNode * sceneNode = static_cast<SpaceNode *>(sm->getOriginNode()->create());
        sceneNode->attach(batch);
        sceneNode->setAABBView(batchSettings.setting[SHOW_BOUNDINGBOX]);

        materialInstanceBatch.push_back(batch);

        return batch;
    }
    //-----------------------------------------------------------------------
    void InstanceMesh::cleanupEmptyBatches(void)
    {
        //Do this now to avoid any dangling pointer inside mDirtyBatches
        _updateDirtyBatches();

        InstanceBatchMap::iterator itor = mInstanceBatches.begin();
        InstanceBatchMap::iterator end  = mInstanceBatches.end();

        while(itor != end)
        {
            InstanceBatchVec::iterator it = itor->second.begin();
            InstanceBatchVec::iterator en = itor->second.end();

            while(it != en)
            {
                if((*it)->isBatchUnused())
                {
                    N_delete *it;
                    //Remove it from the list swapping with the last element and popping back
                    NCount idx = it - itor->second.begin();
                    *it = itor->second.back();
                    itor->second.pop_back();

                    //Restore invalidated iterators
                    it = itor->second.begin() + idx;
                    en = itor->second.end();
                }
                else
                    ++it;
            }
            ++itor;
        }
        //By this point it may happen that all mInstanceBatches' objects are also empty
        //however if we call mInstanceBatches.clear(), next time we'll create an InstancedObject
        //we'll end up calling buildFirstTime() instead of buildNewBatch(), which is not the idea
        //(takes more time and will leak the shared render operation)
    }
    //-----------------------------------------------------------------------
    void InstanceMesh::defragmentBatches(bool optimizeCull,
        InstanceBatch::InstanceGeoVec & usedEntities, InstanceMesh::InstanceBatchVec & fragmentedBatches)
    {
        InstanceBatchVec::iterator itor = fragmentedBatches.begin();
        InstanceBatchVec::iterator end  = fragmentedBatches.end();

        while(itor != end && !usedEntities.empty())
        {
            if(!(*itor)->isStatic())
                (*itor)->_defragmentBatch(optimizeCull, usedEntities);
            ++itor;
        }

        InstanceBatchVec::iterator lastImportantBatch = itor;

        while(itor != end)
        {
            if(!(*itor)->isStatic())
            {
                //If we get here, this means we hit remaining batches which will be unused.
                //Destroy them
                //Call this to avoid freeing InstancedEntities that were just reparented
                (*itor)->_defragmentBatchDiscard();
                N_delete *itor;
            }
            else
            {
                //This isn't a meaningless batch, move it forward so it doesn't get wipe
                //when we resize the container (faster than removing element by element)
                *lastImportantBatch++ = *itor;
            }
            ++itor;
        }
        //Remove remaining batches all at once from the vector
        const NCount remainingBatches = end - lastImportantBatch;
        fragmentedBatches.resize(fragmentedBatches.size() - remainingBatches);
    }
    //-----------------------------------------------------------------------
    void InstanceMesh::defragmentBatches(bool optimizeCulling)
    {
        //Do this now to avoid any dangling pointer inside mDirtyBatches
        _updateDirtyBatches();

        //Do this for every material
        InstanceBatchMap::iterator itor = mInstanceBatches.begin();
        InstanceBatchMap::iterator end  = mInstanceBatches.end();

        while(itor != end)
        {
            InstanceBatch::InstanceGeoVec usedEntities;
            usedEntities.reserve(itor->second.size() * mInstancesPerBatch);

            //Collect all Instanced Entities being used by _all_ batches from this material
            InstanceBatchVec::iterator it = itor->second.begin();
            InstanceBatchVec::iterator en = itor->second.end();

            while(it != en)
            {
                //Don't collect instances from static batches, we assume they're correctly set
                //Plus, we don't want to put InstancedEntities from non-static into static batches
                if(!(*it)->isStatic())
                    (*it)->getInstancedEntitiesInUse(usedEntities);
                ++it;
            }

            defragmentBatches(optimizeCulling, usedEntities, itor->second);

            ++itor;
        }
    }
    //-----------------------------------------------------------------------
    void InstanceMesh::setSetting(BatchSettingId id, bool value, ResourceID materialName)
    {
        N_assert( id < NUM_SETTINGS, "error");

        if(materialName == 0)
        {
            //Setup all existing materials
            InstanceBatchMap::iterator itor = mInstanceBatches.begin();
            InstanceBatchMap::iterator end  = mInstanceBatches.end();

            while(itor != end)
            {
                mBatchSettings[itor->first].setting[id] = value;
                applySettingToBatches( id, value, itor->second );

                ++itor;
            }
        }
        else
        {
            //Setup a given material
            mBatchSettings[materialName].setting[id] = value;

            InstanceBatchMap::const_iterator itor = mInstanceBatches.find(materialName);
            //Don't crash or throw if the batch with that material hasn't been created yet
            if(itor != mInstanceBatches.end() )
                applySettingToBatches(id, value, itor->second);
        }
    }
    //-----------------------------------------------------------------------
    bool InstanceMesh::getSetting(BatchSettingId id, ResourceID mid) const
    {
        N_assert(id < NUM_SETTINGS, "error");

        BatchSettingsMap::const_iterator itor = mBatchSettings.find(mid);
        if(itor != mBatchSettings.end())
            return itor->second.setting[id]; //Return current setting

        //Return default
        return BatchSettings().setting[id];
    }
    //-----------------------------------------------------------------------
    void InstanceMesh::applySettingToBatches(BatchSettingId id, bool value,
        const InstanceBatchVec & container)
    {
        InstanceBatchVec::const_iterator itor = container.begin();
        InstanceBatchVec::const_iterator end  = container.end();

        while(itor != end)
        {
            switch(id)
            {
            case CAST_SHADOWS:
                (*itor)->setCastShadow(value);
                break;
            case SHOW_BOUNDINGBOX:
                (*itor)->getSceneNode()->setAABBView(value);
                break;
            default:
                break;
            }
            ++itor;
        }
    }
    //-----------------------------------------------------------------------
    void InstanceMesh::setBatchesAsStaticAndUpdate(bool b)
    {
        InstanceBatchMap::iterator itor = mInstanceBatches.begin();
        InstanceBatchMap::iterator end = mInstanceBatches.end();

        while(itor != end)
        {
            InstanceBatchVec::iterator it = itor->second.begin();
            InstanceBatchVec::iterator en = itor->second.end();

            while(it != en)
            {
                (*it)->setStaticAndUpdate(b);
                ++it;
            }

            ++itor;
        }
    }
    //-----------------------------------------------------------------------
    void InstanceMesh::_addDirtyBatch(InstanceBatch * dirtyBatch)
    {
        if(mDirtyBatches.empty())
            N_Engine().updateInstanceMesh(this);

        mDirtyBatches.push_back(dirtyBatch);
    }
    //-----------------------------------------------------------------------
    void InstanceMesh::_updateDirtyBatches()
    {
        InstanceBatchVec::const_iterator itor = mDirtyBatches.begin();
        InstanceBatchVec::const_iterator end  = mDirtyBatches.end();

        while(itor != end)
        {
            (*itor)->updateSpaceAABB();
            ++itor;
        }

        mDirtyBatches.clear();
    }
    //-----------------------------------------------------------------------
}
