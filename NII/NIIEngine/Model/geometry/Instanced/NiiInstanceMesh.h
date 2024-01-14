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

#ifndef _NII_InstanceMesh_H_
#define _NII_InstanceMesh_H_

#include "NiiPreInclude.h"
#include "NiiInstanceBatch.h"

namespace NII
{
    enum InstanceMeshType
    {
        ShaderBased,            //Any SM 2.0+ @See InstanceBatchShader
        TextureVTF,                //Needs Vertex Texture Fetch & SM 3.0+ @See InstanceBatchVTF
        HWInstancingBasic,        //Needs SM 3.0+ and HW instancing support @See InstanceBatchHW
        HWInstancingVTF,        //Needs SM 3.0+, HW instancing support & VTF @See InstanceBatchHW_VTF
        InstancingTechniquesCount
    };
        
    /** Flags for the Instance Manager when calculating ideal number of instances per batch */
    enum InstanceManagerFlags
    {
        /** Forces an amount of instances per batch low enough so that vertices * numInst < 65535
            since usually improves performance. In HW instanced techniques, this flag is ignored
        */
        IM_USE16BIT = 0x0001,

        /** The num. of instances is adjusted so that as few pixels as possible are wasted
            in the vertex texture */
        IM_VTFBESTFIT = 0x0002,

        /** Use a limited number of skeleton animations shared among all instances.
        Update only that limited amount of animations in the vertex texture.*/
        IM_VTFBONEMATRIXLOOKUP = 0x0004,

        IM_USEBONEDUALQUATERNIONS = 0x0008,

        /** Use one weight per vertex when recommended (i.e. VTF). */
        IM_USEONEWEIGHT = 0x0010,

        /** All techniques are forced to one weight per vertex. */
        IM_FORCEONEWEIGHT = 0x0020,

        IM_USEALL = IM_USE16BIT|IM_VTFBESTFIT|IM_USEONEWEIGHT
    };
    
    /** 
    @version 1.0
     */
    class _EngineAPI InstanceMesh : public FactoryAlloc
    {
    public:
        /** Values to be used in setSetting() & BatchSettings::setting */
        enum BatchSettingId
        {
            /// Makes all batches from same material cast shadows
            CAST_SHADOWS = 0,
            /// Makes each batch to display it's bounding box. Useful for debugging or profiling
            SHOW_BOUNDINGBOX,

            NUM_SETTINGS
        };
        typedef vector<InstanceBatch *>::type InstanceBatchVec;
        typedef map<ResourceID, InstanceBatchVec>::type InstanceBatchMap;
    public:
        InstanceMesh(InstanceMeshID imid, SpaceManager * sceneManager, ResourceID rid,
            GroupID gid, InstanceMeshType instancingTechnique, Nui16 instancingFlags,
                NCount instancesPerBatch, Nui16 subMeshIdx, bool useBoneMatrixLookup = false);
        virtual ~InstanceMesh();

        InstanceMeshID getID() const { return mName; }

        /** Raises an exception if trying to change it after creating the first InstanceGeo
        @remarks The actual value may be less if the technique doesn't support having so much
        @see getMaxOrBestNumInstancesPerBatches for the usefulness of this function
        @param[in] instancesPerBatch New instances per batch number
        @version NIIEngine 3.0.0
        */
        void setInstancesPerBatch(NCount instancesPerBatch);

        /** Sets the size of the lookup table for techniques supporting bone lookup table.
            Raises an exception if trying to change it after creating the first InstanceGeo.
            Setting this value below the number of unique (non-sharing) entity instance animations
            will produce a crash during runtime. Setting this value above will increase memory
            consumption and reduce framerate.
        @remarks The value should be as close but not below the actual value.
        @param[in] maxLookupTableInstances New size of the lookup table
        @version NIIEngine 3.0.0
        */
        void setMaxLookupTableInstances(NCount maxLookupTableInstances);

        /**    Calculates the maximum (or the best amount, depending on flags) of instances
            per batch given the suggested size for the technique this manager was created for.
        @remarks
            This is done automatically when creating an instanced entity, but this function in conjunction
            with @see setInstancesPerBatch allows more flexible control over the amount of instances
            per batch
        @param[in] materialName Name of the material to base on
        @param[in] suggestedSize Suggested amount of instances per batch
        @param[in] flags @see InstanceManagerFlags
        @return The max/best amount of instances per batch given the suggested size and flags
        @version NIIEngine 3.0.0
        */
        NCount getMaxOrBestNumInstancesPerBatch(ResourceID materialName, NCount suggestedSize, Nui16 flags);

        /** @copydoc SpaceManager::createInstanceGeo 
        */
        InstanceGeo * createInstanceGeo(SpaceManager * sm, ResourceID materialName);

        /** This function can be useful to improve CPU speed after having too many instances
            created, which where now removed, thus freeing many batches with zero used Instanced Entities
            However the batches aren't automatically removed from memory until the InstanceMesh is
            destroyed, or this function is called. This function removes those batches which are completely
            unused (only wasting memory).
        @version NIIEngine 3.0.0
        */
        void cleanupEmptyBatches(void);

        /** After creating many entities (which turns in many batches) and then removing entities that
            are in the middle of these batches, there might be many batches with many free entities.
            Worst case scenario, there could be left one batch per entity. Imagine there can be
            80 entities per batch, there are 80 batches, making a total of 6400 entities. Then
            6320 of those entities are removed in a very specific way, which leads to having
            80 batches, 80 entities, and GPU vertex shader still needs to process 6400!
            This is called fragmentation. This function reparents the InstancedEntities
            to fewer batches, in this case leaving only one batch with 80 entities
        @remarks
            This function takes time. Make sure to call this only when you're sure there's
            too much of fragmentation and you won't be creating more InstancedEntities soon
            Also in many cases cleanupEmptyBatches() ought to be enough
            Defragmentation is done per material
            Static batches won't be defragmented. If you want to degragment them, set them
            to dynamic again, and switch back to static after calling this function.
        @param[in] optimizeCulling When true, entities close together will be reorganized
            in the same batch for more efficient CPU culling. This can take more CPU
            time. You want this to be false if you now you're entities are moving very
            randomly which tends them to get separated and spread all over the scene
            (which nullifies any CPU culling)
        @version NIIEngine 3.0.0
        */
        void defragmentBatches(bool optimizeCulling);

        /** Applies a setting for all batches using the same material_ existing ones and
            those that will be created in the future.
        @par
            For example setSetting( BatchSetting::CAST_SHADOWS, false ) disables shadow
            casting for all instanced entities (@see SpaceObj::setCastShadow)
        @par
            For example setSetting( BatchSetting::SHOW_BOUNDINGBOX, true, "MyMat" )
            will display the bounding box of the batch (not individual InstancedEntities)
            from all batches using material "MyMat"
        @note If the material name hasn't been used, the settings are still stored
        This allows setting up batches before they get even created.
        @param[in] id Setting Id to setup, @see BatchSettings::BatchSettingId
        @param[in] enabled Boolean value. It's meaning depends on the id.
        @param[in] materialName When Blank, the setting is applied to all existing materials
        @version NIIEngine 3.0.0
        */
        void setSetting(BatchSettingId id, bool value, ResourceID mid = 0);

        /** If settings for the given material didn't exist, default value is returned
        @version NIIEngine 3.0.0
        */
        bool getSetting(BatchSettingId id, ResourceID mid) const;

        /** Returns true if settings were already created for the given material name.
            If false is returned, it means getSetting will return default settings.
        @version NIIEngine 3.0.0
        */
        bool hasSettings(ResourceID mid) const
        {
            return mBatchSettings.find(mid) != mBatchSettings.end();
        }

        /**    @copydoc InstanceBatch::setStaticAndUpdate 
        */
        void setBatchesAsStaticAndUpdate(bool bStatic);

        /** Called by an InstanceBatch when it requests their bounds to be updated for proper culling
        @param[in] dirtyBatch The batch which is dirty, usually same as caller.
        @version NIIEngine 3.0.0
        */
        void _addDirtyBatch(InstanceBatch * dirtyBatch);

        /** Called by SpaceManager when we told it we have at least one dirty batch 
        @version NIIEngine 3.0.0
        */
        void _updateDirtyBatches();

        typedef ConstMapIterator<InstanceBatchMap> InstanceBatchMapIterator;
        typedef ConstVectorIterator<InstanceBatchVec> InstanceBatchIterator;

        /// Get non-updateable iterator over instance batches per material
        InstanceBatchMapIterator getInstanceBatchMapIterator(void) const
        { return InstanceBatchMapIterator(mInstanceBatches.begin(), mInstanceBatches.end()); }

        /** Get non-updateable iterator over instance batches for given material
        @remarks
            Each InstanceBatch pointer may be modified for low level usage (i.e.
            setCustonParam), but there's no synchronization mechanism when
            multithreading or creating more instances, that's up to the user.
        */
        InstanceBatchIterator getInstanceBatchIterator(ResourceID mid) const
        {
            InstanceBatchMap::const_iterator it = mInstanceBatches.find(mid);
            return InstanceBatchIterator(it->second.begin(), it->second.end());
        }
    private:
        /** Finds a batch with at least one free instanced entity we can use. If none found, 
            creates one.
        @version NIIEngine 3.0.0
        */
        inline InstanceBatch * getFreeBatch(SpaceManager * sm, ResourceID materialName);

        /** Called when batches are fully exhausted (can't return more instances) so a new batch
            is created.
            For the first time use, it can take big build time.
            It takes care of getting the render operation which will be shared by further batches,
            which decreases their build time, and prevents GPU RAM from skyrocketing.
        @param[in] materialName The material name, to know where to put this batch in the map
        @param[in] firstTime True if this is the first time it is called
        @return The created InstancedManager for convenience
        @version NIIEngine 3.0.0
        */
        InstanceBatch * buildNewBatch(SpaceManager * sm, ResourceID materialName, bool firstTime);

        /** @see defragmentBatches overload, this takes care of an array of batches
            for a specific material
        @version NIIEngine 3.0.0
        */
        void defragmentBatches(bool optimizeCull, InstanceBatch::InstanceGeoVec & entities,
            InstanceMesh::InstanceBatchVec & fragmentedBatches);

        /** @see setSetting. This function helps it by setting the given parameter to all batches
            in container.
        @version NIIEngine 3.0.0
        */
        void applySettingToBatches(BatchSettingId id, bool value, const InstanceBatchVec & container);
    private:
        struct BatchSettings
        {
            //These are all per material
            bool setting[NUM_SETTINGS];

            BatchSettings()
            {
                setting[CAST_SHADOWS] = true;
                setting[SHOW_BOUNDINGBOX] = false;
            }
        };

        typedef map<ResourceID, BatchSettings>::type BatchSettingsMap;
    private:
        InstanceMeshID mName;
        Mesh * mMeshReference;
        InstanceBatchMap mInstanceBatches;
        NCount mIdCount;

        InstanceBatchVec mDirtyBatches;
        GeometryRaw mSharedRenderOperation;

        NCount mInstancesPerBatch;
        InstanceMeshType mInstancingTechnique;
        Nui16 mInstancingFlags;        //@see InstanceManagerFlags
        Nui16 mSubMeshIdx;

        BatchSettingsMap mBatchSettings;
        SpaceManager * mSceneManager;

        NCount mMaxLookupTableInstances;
    };
}
#endif