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
#include "NiiInstanceBatch.h"
#include "NiiGeometryRaw.h"
#include "NiiInstanceGeo.h"
#include "NiiSpaceNode.h"
#include "NiiCamera.h"
#include "NiiLodScheme.h"
#include "NiiException.h"
#include "NiiGeometryObj.h"
#include "NiiMaterial.h"
#include "NiiMaterialLodScheme.h"

namespace NII
{
    //----------------------------------------------------------------------------
    InstanceBatch::InstanceBatch(InstanceMesh * creator, Mesh * mesh, Material * material, 
        NCount instancesPerBatch, const BoneIndexList * bonemap, Nid bid) :
            GeometryObj(),
            SpaceObj(),
            mInstancesPerBatch(instancesPerBatch),
            mCreator(creator),
            mMaterial(material),
            mMeshReference(mesh),
            mIndexToBoneMap(bonemap),
            mBoundingRadius(0),
            mCurrentCamera(0),
            mMaterialLodIndex(0),
            mCachedCamera(0),
            mTransformSharingDirty(true),
            mTechnSupportsSkeletal(true),
            mRemoveOwnVertexData(false),
            mRemoveOwnIndexData(false),
            mBoundsUpdated(false),
            mBoundsDirty(false)
    {
        N_assert(mInstancesPerBatch, "error logic");

        //Force batch visibility to be always visible. The instanced entities
        //have individual visibility flags. If none matches the scene's current,
        //then this batch won't rendered.
        mVisibleMark = N_TYPE_MAX(Nui32);

        if(bonemap)
        {
            N_assert(!(mesh->isSkeletonFusion() && bonemap->empty()), "error");
        }
        mFullBoundingBox.setMin(-Vector3f::ZERO);
        mFullBoundingBox.setMax(Vector3f::ZERO);
        if(bid != 0)
            mID = bid;
    }
    //----------------------------------------------------------------------------
    InstanceBatch::~InstanceBatch()
    {
        deleteAllInstancedEntities();

        //Remove the parent scene node automatically
        SpaceNode * sceneNode = getSceneNode();
        if(sceneNode)
        {
            sceneNode->detachAll();
            static_cast<SpaceNode *>(sceneNode->getParent())->destroy(sceneNode->getID());
        }

        if(mRemoveOwnVertexData)
            N_delete mRenderOperation.mVertex;
        if(mRemoveOwnIndexData)
            N_delete mRenderOperation.mIndex;
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::_setInstancesPerBatch(NCount instancesPerBatch)
    {
        if(!mInstancedEntities.empty())
        {
            N_EXCEPT(InvalidState, _I18n("Instances per batch can only be changed before building the batch."));
        }
        mInstancesPerBatch = instancesPerBatch;
    }
    //-----------------------------------------------------------------------
    bool InstanceBatch::checkSubMeshCompatibility(const SubMesh * obj)
    {
        if(obj->getGeometryType() != GeometryRaw::OT_TRIANGLE_LIST)
        {
            N_EXCEPT(UnImpl, _I18n("Only meshes with OT_TRIANGLE_LIST are supported"));
        }
        return true;
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::updateSpaceAABB(void)
    {
        mFullBoundingBox.setNull();

        InstanceGeoVec::const_iterator itor = mInstancedEntities.begin();
        InstanceGeoVec::const_iterator end  = mInstancedEntities.end();

        NIIf maxScale = 0;
        while(itor != end)
        {
            InstanceGeo * ent = (*itor);
            //Only increase the bounding box for those objects we know are in the scene
            if(ent->isActive())
            {
                maxScale = std::max(maxScale, ent->getMaxScaleCoef());
                mFullBoundingBox.merge(ent->setSpacePos());
            }

            ++itor;
        }

        NIIf addToBound = maxScale * _getMeshReference()->getCenterRange();
        mFullBoundingBox.setMax(mFullBoundingBox.getMax() + addToBound);
        mFullBoundingBox.setMin(mFullBoundingBox.getMin() - addToBound);

        mBoundingRadius = mFullBoundingBox.calcRadius();

        //Tell the SpaceManager our bounds have changed
        getSceneNode()->notify(true);

        mBoundsDirty    = false;
        mBoundsUpdated    = true;
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::updateVisibility(void)
    {
        mVisible = false;

        InstanceGeoVec::const_iterator itor = mInstancedEntities.begin();
        InstanceGeoVec::const_iterator end  = mInstancedEntities.end();

        while(itor != end && !mVisible)
        {
            //Trick to force not to render us if none of our instances is visible
            //Because we do Camera::isVisible(), it is better if the SpaceNode from the
            //InstanceGeo is not part of the scene graph (i.e. ultimate parent is root node)
            //to avoid unnecessary wasteful calculations
            mVisible |= (*itor)->findVisible(mCurrentCamera);
            ++itor;
        }
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::createAllInstancedEntities()
    {
        mInstancedEntities.reserve(mInstancesPerBatch);
        mUnusedEntities.reserve(mInstancesPerBatch);

        for(NCount i=0; i<mInstancesPerBatch; ++i)
        {
            InstanceGeo * instance = generateInstanceGeo(i);
            mInstancedEntities.push_back(instance);
            mUnusedEntities.push_back(instance);
        }
    }
    //-----------------------------------------------------------------------
    InstanceGeo * InstanceBatch::generateInstanceGeo(NCount num)
    {
        return N_new InstanceGeo(this, num);
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::deleteAllInstancedEntities()
    {
        InstanceGeoVec::const_iterator itor = mInstancedEntities.begin();
        InstanceGeoVec::const_iterator end  = mInstancedEntities.end();

        while(itor != end)
        {
            if((*itor)->getSceneNode())
                (*itor)->getSceneNode()->detach((*itor));

            N_delete *itor++;
        }
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::deleteUnusedInstancedEntities()
    {
        InstanceGeoVec::const_iterator itor = mUnusedEntities.begin();
        InstanceGeoVec::const_iterator end  = mUnusedEntities.end();

        while( itor != end )
            N_delete *itor++;

        mUnusedEntities.clear();
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::makeMatrixCameraRelative3x4(NIIf * mat3x4, NCount numFloats)
    {
        const Vector3f & cameraRelativePosition = mCurrentCamera->getSpacePos();

        for(NCount i = 0; i<numFloats >> 2; i += 3)
        {
            const Vector3f worldTrans(mat3x4[(i + 0) * 4 + 3], mat3x4[(i + 1) * 4 + 3],
                mat3x4[(i + 2) * 4 + 3]);
            const Vector3f newPos(worldTrans - cameraRelativePosition);

            mat3x4[(i + 0) * 4 + 3] = (NIIf)newPos.x;
            mat3x4[(i + 1) * 4 + 3] = (NIIf)newPos.y;
            mat3x4[(i + 2) * 4 + 3] = (NIIf)newPos.z;
        }
    }
    //-----------------------------------------------------------------------
    GeometryRaw InstanceBatch::build(const SubMesh * baseSubMesh)
    {
        if(checkSubMeshCompatibility(baseSubMesh))
        {
            //Only triangle list at the moment
            mRenderOperation.mType    = GeometryRaw::OT_TRIANGLE_LIST;
            mRenderOperation.add(GRT_Index);
            setupVertices(baseSubMesh);
            setupIndices(baseSubMesh);

            createAllInstancedEntities();
        }
        return mRenderOperation;
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::buildFrom(const SubMesh * baseSubMesh, const GeometryRaw & geo)
    {
        mRenderOperation = geo;
        createAllInstancedEntities();
    }
    //-----------------------------------------------------------------------
    InstanceGeo * InstanceBatch::createInstanceGeo()
    {
        InstanceGeo * retVal = 0;

        if(!mUnusedEntities.empty())
        {
            retVal = mUnusedEntities.back();
            mUnusedEntities.pop_back();

            retVal->setInUse(true);
        }

        return retVal;
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::removeInstanceGeo(InstanceGeo * instancedEntity)
    {
        if(instancedEntity->mBatchOwner != this)
        {
            N_EXCEPT(InvalidParam, _I18n("Trying to remove an InstanceGeo from scene"
                "created with a different InstanceBatch"));
        }

        if(instancedEntity->getSceneNode())
            instancedEntity->getSceneNode()->detach(instancedEntity);

        instancedEntity->setInUse(false);
        instancedEntity->stopSharingTransform();

        //Put it back into the queue
        mUnusedEntities.push_back(instancedEntity);
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::getInstancedEntitiesInUse(InstanceGeoVec & outEntities)
    {
        InstanceGeoVec::const_iterator itor = mInstancedEntities.begin();
        InstanceGeoVec::const_iterator end  = mInstancedEntities.end();

        while(itor != end)
        {
            if((*itor)->isInUse())
                outEntities.push_back(*itor);
            ++itor;
        }
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::defragmentBatchNoCull(InstanceGeoVec & usedEntities)
    {
        const NCount maxInstancesToCopy = std::min(mInstancesPerBatch, usedEntities.size());
        InstanceGeoVec::iterator first = usedEntities.end() - maxInstancesToCopy;

        //Copy from the back to front, into m_instancedEntities
        mInstancedEntities.insert(mInstancedEntities.begin(), first, usedEntities.end());
        //Remove them from the array
        usedEntities.resize(usedEntities.size() - maxInstancesToCopy);
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::defragmentBatchDoCull(InstanceGeoVec & usedEntities)
    {
        //Get the the entity closest to the minimum bbox edge and put into "first"
        InstanceGeoVec::const_iterator itor   = usedEntities.begin();
        InstanceGeoVec::const_iterator end   = usedEntities.end();

        Vector3f vMinPos = Vector3f::ZERO, firstPos = Vector3f::ZERO;
        InstanceGeo * first = 0;

        if(!usedEntities.empty())
        {
            first = *usedEntities.begin();
            firstPos = first->setSpacePos();
            vMinPos = first->setSpacePos();
        }

        while(itor != end)
        {
            const Vector3f & vPos = (*itor)->setSpacePos();

            vMinPos.x = std::min(vMinPos.x, vPos.x);
            vMinPos.y = std::min(vMinPos.y, vPos.y);
            vMinPos.z = std::min(vMinPos.z, vPos.z);

            if( vMinPos.distanceSquared(vPos) < vMinPos.distanceSquared(firstPos))
            {
                firstPos   = vPos;
            }

            ++itor;
        }

        //Now collect entities closest to 'first'
        while(!usedEntities.empty() && mInstancedEntities.size() < mInstancesPerBatch)
        {
            InstanceGeoVec::iterator closest = usedEntities.begin();
            InstanceGeoVec::iterator it = usedEntities.begin();
            InstanceGeoVec::iterator e  = usedEntities.end();

            Vector3f closestPos;
            closestPos = (*closest)->setSpacePos();

            while(it != e)
            {
                const Vector3f & vPos = (*it)->setSpacePos();

                if(firstPos.distanceSquared(vPos) < firstPos.distanceSquared(closestPos))
                {
                    closest = it;
                    closestPos = vPos;
                }
                ++it;
            }
            mInstancedEntities.push_back(*closest);

            //Remove 'closest' from usedEntities using swap and pop_back trick
            *closest = *(usedEntities.end() - 1);
            usedEntities.pop_back();
        }
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::_defragmentBatch(bool optimizeCulling, InstanceGeoVec & usedEntities)
    {
        //Remove and clear what we don't need
        mInstancedEntities.clear();
        deleteUnusedInstancedEntities();

        if(!optimizeCulling)
            defragmentBatchNoCull(usedEntities);
        else
            defragmentBatchDoCull(usedEntities);

        //Reassign instance IDList and tell we're the new parent
        Nui32 instanceId = 0;
        InstanceGeoVec::const_iterator itor = mInstancedEntities.begin();
        InstanceGeoVec::const_iterator end  = mInstancedEntities.end();

        while(itor != end)
        {
            (*itor)->mInstanceId = instanceId++;
            (*itor)->mBatchOwner = this;
            ++itor;
        }

        //Recreate unused entities, if there's left space in our container
        N_assert((signed)(mInstancesPerBatch) - (signed)(mInstancedEntities.size()) >= 0, "error");
        mInstancedEntities.reserve(mInstancesPerBatch);
        mUnusedEntities.reserve(mInstancesPerBatch);
        for(NCount i = mInstancedEntities.size(); i < mInstancesPerBatch; ++i)
        {
            InstanceGeo *instance = generateInstanceGeo(i);
            mInstancedEntities.push_back(instance);
            mUnusedEntities.push_back(instance);
        }

        //We've potentially changed our bounds
        if( !isBatchUnused() )
            _boundsDirty();
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::_defragmentBatchDiscard(void)
    {
        //Remove and clear what we don't need
        mInstancedEntities.clear();
        deleteUnusedInstancedEntities();
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::_boundsDirty(void)
    {
        if(mCreator && !mBoundsDirty)
            mCreator->_addDirtyBatch(this);
        mBoundsDirty = true;
    }
    //-----------------------------------------------------------------------
    FactoryID InstanceBatch::getFactoryID() const
    {
        return N_FACTORY_InstanceBatch;
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::_notify(Camera * cam)
    {
        mCurrentCamera = cam;

        //See CameraLodScheme::getValueImpl()
        //We use our own because our SpaceNode is just filled with zeroes, and updating it
        //with real values is expensive, plus we would need to make sure it doesn't get to
        //the shader
        NIIf squaredDepth = distanceSquared(cam) - Math::Sqr(mMeshReference->getCenterRange());
        squaredDepth = std::max(squaredDepth, NIIf(0));
        NIIf lodValue = squaredDepth * (1.0f / cam->getLodBias());

        //Now calculate Material LOD
        /*LodScheme * materialStrategy;
        m_material->get(materialStrategy);

        //Calculate lod value for given strategy
        NIIf lodValue = materialStrategy->getValue( this, cam );*/
        MaterialLodScheme * scheme = mMaterial->getLodScheme();
        //Get the index at this depth
        Nidx idx = scheme->get(scheme->getDest(lodValue));

        //TODO: Replace subEntity for SpaceObj
        // Construct event object
        /*MaterialLodEvent subEntEvt;
        subEntEvt.subEntity = this;
        subEntEvt.camera = cam;
        subEntEvt.lodValue = lodValue;
        subEntEvt.previousLodIndex = m_materialLodIndex;
        subEntEvt.newLodIndex = idx;

        //Notify lod event listeners
        cam->getManager()->onLodEvent(subEntEvt);*/

        //Change lod index
        mMaterialLodIndex = idx;

        SpaceObj::_notify(cam);
    }
    //-----------------------------------------------------------------------
    const AABox & InstanceBatch::getAABB() const
    {
        return mFullBoundingBox;
    }
    //-----------------------------------------------------------------------
    NIIf InstanceBatch::getCenterRange() const
    {
        return mBoundingRadius;
    }
    //-----------------------------------------------------------------------
    NIIf InstanceBatch::distanceSquared(const Camera * cam) const
    {
        if(mCachedCamera != cam)
        {
            mCachedCameraDist = N_TYPE_INFINITY(NIIf);

            InstanceGeoVec::const_iterator itor = mInstancedEntities.begin();
            InstanceGeoVec::const_iterator end  = mInstancedEntities.end();

            while(itor != end)
            {
                mCachedCameraDist = std::min(mCachedCameraDist, (*itor)->distanceSquared(cam));
                ++itor;
            }

            mCachedCamera = cam;
        }

        return mCachedCameraDist;
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::getLight(LightList & ll) const
    {
        const_cast<InstanceBatch *>(this)->query(ll);
    }
    //-----------------------------------------------------------------------
    ShaderFusion * InstanceBatch::getShaderFusion() const
    {
        return mMaterial->getLOD(this, mMaterialLodIndex);
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::queue(RenderQueue * queue)
    {
        /*if( m_boundsDirty )
            updateSpaceAABB();*/

        mDirtyAnimation = false;

        //Is at least one object in the scene?
        updateVisibility();

        if(mVisible)
        {
            if(mMeshReference->isSkeletonFusion())
            {
                InstanceGeoVec::const_iterator itor = mInstancedEntities.begin();
                InstanceGeoVec::const_iterator end  = mInstancedEntities.end();

                while(itor != end)
                {
                    mDirtyAnimation |= (*itor)->_updateAnimation();
                    ++itor;
                }
            }

            queue->add(this);
        }

        //Reset visibility once we skipped add (which saves GPU time), because NII for some
        //reason stops updating our render queue afterwards, preventing us to recalculate visibility
        mVisible = true;
    }
    //-----------------------------------------------------------------------
    void InstanceBatch::query(GeometryQuery * query)
    {
        query->query(this);
    }
    //-----------------------------------------------------------------------
}