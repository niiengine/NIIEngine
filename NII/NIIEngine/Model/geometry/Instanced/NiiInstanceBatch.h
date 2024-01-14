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

#ifndef _NII_InstanceBatch_H__
#define _NII_InstanceBatch_H__

#include "NiiPreInclude.h"
#include "NiiGeometryObj.h"
#include "NiiSpaceObj.h"
#include "NiiMesh.h"

namespace NII
{
    /** InstanceBatch forms part of the new Instancing system
        This is an abstract class that must be derived to implement different instancing techniques
        (@see InstanceMeshType)
        NII wasn't truly thought for instancing. NII assumes that either:
            a. One SpaceObj -> No GeometryObj
            b. One SpaceObj -> One GeometryObj
            c. One SpaceObj -> Many GeometryObj.
        However, instances work on reverse: Many SpaceObj have the same GeometryObj.
        <b>Instancing is already difficult to cull by a CPU</b>, but the main drawback from this assumption
        is that it makes it even harder to take advantage from NII's culling capabilities
        (i.e. @see OctreeSceneManager)
    @par
        To workaround this problem, InstanceBatch updates on almost every frame,
        growing the bounding box to fit all instances that are not being culled individually.
        This helps by avoiding a huge bbox that may cover the whole scene, which decreases shadow
        quality considerably (as it is seen as large shadow receiver)
        Furthermore, if no individual instance is visible, the InstanceBatch switches it's visibility
        (@see SpaceObj::setVisible) to avoid sending this GeometryObj to the GPU. This happens because
        even when no individual instance is visible, their merged bounding box may cause NII to think
        the batch is visible (i.e. the camera is looking between object A & B, but A & B aren't visible)
    @par
        <b>As it happens with instancing in general, all instanced entities from the same batch will share
        the same textures and materials</b>
    @par
        Each InstanceBatch preallocates a fixed amount of mInstancesPerBatch instances once it's been
        built (@see build, @see buildFrom).
    @see createInstanceGeo and @see removeInstanceGeo on how to retrieve those instances
        remove them from scene.
        Note that, on GPU side, removing an instance from scene doesn't save GPU cycles on what
        respects vertex shaders, but saves a little fillrate and pixel shaders; unless all instances
        are removed, which saves GPU.
    @remarks Design discussion webpage
    @author Matias N. Goldberg ("dark_sylinc")
    @version 1.0
     */
    class _EngineAPI InstanceBatch : public GeometryObj, public SpaceObj
    {
    public:
        typedef vector<InstanceGeo *>::type InstanceGeoVec;
    public:
        InstanceBatch(InstanceMesh * creator, Mesh * mesh, Material * material,
            NCount count, const BoneIndexList * bonemap, Nid bid);
        virtual ~InstanceBatch();

        Mesh * _getMeshRef() { return mMeshReference; }

        /** Raises an exception if trying to change it after being built
        */
        void _setInstancesPerBatch(NCount instancesPerBatch);

        const BoneIndexList * _getIndexToBoneMap() const { return mIndexToBoneMap; }

        /** Returns true if this technique supports skeletal animation
        @remarks
            A virtual function could have been used, but using a simple variable overriden
            by the derived class is faster than virtual call overhead. And both are clean
            ways of implementing it.
        */
        bool _supportsSkeletalAnimation() const { return mTechnSupportsSkeletal; }

        /** @see InstanceMesh::updateDirtyBatches 
        */
        void updateSpaceAABB(void);

        /** Some techniques have a limit on how many instances can be done.
            Sometimes even depends on the material being used.
            @par
            Note this is a helper function, as such it takes a submesh base to compute
            the parameters, instead of using the object's own. This allows
            querying for a technique without requiering to actually build it.
            @param[in] baseSubMesh The base submesh that will be using to build it.
            @param[in] flags @see InstanceManagerFlags
            @return The max instances limit
        */
        virtual NCount calculateMaxNumInstances(const SubMesh * baseSubMesh, Nui16 flags) const = 0;

        /** Constructs all the data needed to use this batch, as well as the
            InstanceEntities. Placed here because in the constructor virtual
            tables may not have been yet filled.
            @param[in] baseSubMesh A sub mesh which the instances will be based upon from.
            @remarks
                Call this only ONCE. This is done automatically by InstanceMesh
                Caller is responsable for freeing buffers in this GeometryRaw
                Buffers inside the RenderOp may be null if the built failed.
            @return
                A render operation which is very useful to pass to other InstanceBatches
                (@see buildFrom) so that they share the same vertex buffers and indices,
                when possible
        */
        virtual GeometryRaw build(const SubMesh * baseSubMesh);

        /** Instancing consumes significantly more GPU memory than regular rendering
            methods. However, multiple batches can share most, if not all, of the
            vertex & index buffers to save memory.
            Derived classes are free to overload this method to manipulate what to
            reference from Render Op.
            For example, Hardware based instancing uses it's own vertex buffer for the
            last source binding, but shares the other sources.
            @param[in] op The RenderOp to reference.
            @remarks
                Caller is responsable for freeing buffers passed as input arguments
                This function replaces the need to call build()
        */
        virtual void buildFrom(const SubMesh * baseSubMesh, const GeometryRaw & op);

        const Mesh * _getMeshReference(void) const { return mMeshReference; }

        /** @return true if it can not create more InstancedEntities
            (Num InstancedEntities == mInstancesPerBatch)
        */
        bool isBatchFull(void) const { return mUnusedEntities.empty(); }

        /** Returns true if it no instanced entity has been requested or all of them have been removed
        */
        bool isBatchUnused(void) const { return mUnusedEntities.size() == mInstancedEntities.size(); }

        /** Fills the input vector with the instances that are currently being used or were requested
            Used for defragmentation, @see InstanceMesh::defragmentBatches
        */
        void getInstancedEntitiesInUse( InstanceGeoVec &outEntities );

        /** @see InstanceMesh::defragmentBatches
            This function takes InstancedEntities and pushes back all entities it can fit here
            Extra entities in mUnusedEntities are destroyed
            (so that used + unused = mInstancedEntities.size())
            @param[in] optimizeCulling true will call the DoCull version, false the NoCull
            @param[in] Array of InstancedEntities to parent with this batch. Those reparented
            are removed from this input vector
            @remarks:
            This function assumes caller holds data to mInstancedEntities! Otherwise
            you can get memory leaks. Don't call this directly if you don't know what you're doing!
        */
        void _defragmentBatch( bool optimizeCulling, InstanceGeoVec &usedEntities );

        /** @see InstanceMesh::_defragmentBatchDiscard
            Destroys unused entities and clears the mInstanceGeo container which avoids leaving
            dangling pointers from reparented InstancedEntities
            Usually called before deleting this pointer. Don't call directly!
        */
        void _defragmentBatchDiscard(void);

        /** Called by InstanceGeo(s) to tell us we need to update the bounds
            (we touch the SpaceNode so the SpaceManager aknowledges such change)
        */
        virtual void _boundsDirty(void);

        /** Tells this batch to stop updating animations, positions, rotations, and display
            all it's active instances. Currently only InstanceBatchHW & InstanceBatchHW_VTF support it.
            This option makes the batch behave pretty much like Static Geometry, but with the GPU RAM
            memory advantages (less VRAM, less bandwidth) and not LOD support. Very useful for
            billboards of trees, repeating vegetation, etc.
            @remarks
                This function moves a lot of processing time from the CPU to the GPU. If the GPU
                is already a bottleneck, you may see a decrease in performance instead!
                Call this function again (with bStatic=true) if you've made a change to an
                InstanceGeo and wish this change to take effect.
                Be sure to call this after you've set all your instances
        @see InstanceBatchHW::setStaticAndUpdate
        */
        virtual void setStaticAndUpdate(bool bStatic) {}

        /** Returns true if this batch was set as static. @see setStaticAndUpdate
        */
        virtual bool isStatic() const { return false; }

        /** Returns a pointer to a new InstanceGeo ready to use
            Note it's actually preallocated, so no memory allocation happens at
            this point.
            @remarks
                Returns NULL if all instances are being used
        */
        InstanceGeo * createInstanceGeo();

        /** Removes an InstanceGeo from the scene retrieved with
            getNewInstanceGeo, putting back into a queue
        @remarks Throws an exception if the instanced entity wasn't created by this batch
            Removed instanced entities save little CPU time, but _not_ GPU
        */
        void removeInstanceGeo(InstanceGeo * instancedEntity);

        /** Tells whether world bone matrices need to be calculated.
            This does not include bone matrices which are calculated regardless
        */
        virtual bool useBoneWorldMatrices() const { return true; }

        /** Tells that the list of entity instances with shared transforms has changed */
        void _markTransformSharingDirty() { mTransformSharingDirty = true; }
        
        using SpaceObj::query;

        /// @copydetails GeometryObj::getMaterial.
        const Material * getMaterial() const { return mMaterial; }

        /// @copydetails GeometryObj::getGeometry.
        void getGeometry(GeometryRaw & op) const { op = mRenderOperation; }

        /// @copydetails GeometryObj::distanceSquared.
        NIIf distanceSquared(const Camera * cam) const;

        /// @copydetails GeometryObj::getLight.
        void getLight(LightList & ll) const;

        /// @copydetails GeometryObj::getShaderFusion.
        ShaderFusion * getShaderFusion() const;

        /// @copydetails SpaceObj::getFactoryID.
        FactoryID getFactoryID() const;

        /// @copydetails SpaceObj::_notify
        void _notify(Camera * cam);

        /// @copydetails SpaceObj::getAABB
        const AABox & getAABB() const;

        /// @copydetails SpaceObj::getCenterRange.
        NIIf getCenterRange() const;

        /// @copydetails SpaceObj::_upload.
        virtual void queue(RenderQueue * queue);

        /// @copydetails SpaceObj::query
        void query(GeometryQuery * query);
    protected:
        virtual void setupVertices(const SubMesh * baseSubMesh) = 0;
        virtual void setupIndices(const SubMesh * baseSubMesh) = 0;
        virtual void createAllInstancedEntities(void);
        virtual void deleteAllInstancedEntities(void);
        virtual void deleteUnusedInstancedEntities(void);
        /// Creates a new InstanceGeo instance
        virtual InstanceGeo* generateInstanceGeo(NCount num);

        /** Takes an array of 3x4 matrices and makes it camera relative. Note the second argument
            takes number of floats in the array, not number of matrices. Assumes mCachedCamera
            contains the camera which is about to be rendered to.
        */
        void makeMatrixCameraRelative3x4(NIIf * mat3x4, NCount numFloats);

        /// Returns false on errors that would prevent building this batch from the given submesh
        virtual bool checkSubMeshCompatibility(const SubMesh * baseSubMesh);

        void updateVisibility(void);

        /** @see _defragmentBatch */
        void defragmentBatchNoCull(InstanceGeoVec & usedEntities);

        /** @see _defragmentBatch
            This one takes the entity closest to the minimum corner of the bbox, then starts
            gathering entities closest to this entity. There might be much better algorithms (i.e.
            involving space partition), but this one is simple and works well enough
        */
        void defragmentBatchDoCull(InstanceGeoVec & usedEntities);
    protected:
        GeometryRaw mRenderOperation;
        NCount mInstancesPerBatch;

        InstanceMesh * mCreator;

        Material * mMaterial;
        Mesh * mMeshReference;

        BoneIndexList const * mIndexToBoneMap;

        //InstancedEntities are all allocated at build time and kept as "unused"
        //when they're requested, they're removed from there when requested,
        //and put back again when they're no longer needed
        //Note each InstanceGeo has a unique ID ranging from [0; mInstancesPerBatch)
        InstanceGeoVec mInstancedEntities;
        InstanceGeoVec mUnusedEntities;

        /// This bbox contains all (visible) instanced entities
        AABox mFullBoundingBox;
        NIIf mBoundingRadius;
        Camera * mCurrentCamera;

        Nidx mMaterialLodIndex;

        /// Cached distance to last camera for distanceSquared
        mutable NIIf mCachedCameraDist;
        /// The camera for which the cached distance is valid
        mutable const Camera * mCachedCamera;

        /// Tells that the list of entity instances with shared transforms has changed
        bool mTransformSharingDirty;
        /// Set to false at start of each _update
        bool mDirtyAnimation;
        bool mBoundsDirty;
        /// Set to false by derived classes that need it
        bool mBoundsUpdated;
        /// False if a technique doesn't support skeletal animation
        bool mTechnSupportsSkeletal;
        /// When true remove the memory of the VertexData we've created because no one else will
        bool mRemoveOwnVertexData;
        /// When true remove the memory of the IndexData we've created because no one else will
        bool mRemoveOwnIndexData;
    };
}
#endif