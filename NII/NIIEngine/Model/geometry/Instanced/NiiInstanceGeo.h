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

#ifndef _NII_INSTANCEDENTITY_H_
#define _NII_INSTANCEDENTITY_H_

#include "NiiPreInclude.h"
#include "NiiSpaceObj.h"
#include "NiiPosNode.h"

namespace NII
{
    /** @see InstanceBatch to understand how instancing works.

        Instanced entities work in a very similar way as how an GeometrySpace works, as this interface
        tries to mimic it as most as possible to make the transition between GeometrySpace and InstanceGeo
        as straightforward and simple as possible.
        There are a couple inherent limitations, for example setRenderGroup only works on
        the InstanceBatch level, not the individual instance. This limits Z sorting for alpha blending
        quite significantly
        An InstanceGeo won't appear in scene until a SpaceNode is attached to them. Once the
        InstanceGeo is no longer needed, call InstanceBatch::removeInstanceGeo to put them
        back into a queue so the batch can return it back again when a new instance is requested.
    @par
        Internally, instanced entities that are not visible (i.e. by setting setVisible(false) or
        when they don't have a SpaceNode attached to them) a Zero world matrix is sent to the vertex shader
        which in turn causes a zero area triangle.
        This obviously means no Vertex shader benefit, but saves a bit of pixel shader processing power.
        Also this means this object won't be considered when sizing the InstanceBatch's bounding box.
    @par
        Each instance has an instance ID unique within the batch, which matches the ID sent to the vertex
        shader by some techniques (like InstanceBatchShader).
    @par
        Differences between GeometrySpace and InstanceGeo:
        * Setting RenderGroup and other GeometryObj stuff works at InstanceBatch level, not
        InstanceGeo. This is obviously a limitation from instancing in general, not this particular
        implementation
    @remarks Design discussion webpage
    @author Matias N. Goldberg ("dark_sylinc")
    @version 1.0
     */
    class _EngineAPI InstanceGeo : public SpaceObj
    {
        friend class InstanceBatch;
        friend class InstanceBatchShader;
        friend class InstanceBatchHW;
        friend class InstanceBatchHW_VTF;
        friend class BaseInstanceBatchVTF;
    public:
        InstanceGeo(InstanceBatch * batchOwner, Nui32 instanceID, InstanceGeo * sharedTransformEntity = NULL);
        virtual ~InstanceGeo();

        /** Shares the entire transformation with another InstanceGeo. This is useful when a mesh
            has more than one submeshes, therefore creating multiple InstanceMeshs (one for each
            submesh). With this function, sharing makes the skeleton to be shared (less memory) and
            updated once (performance optimization).
            Note that one InstanceGeo (i.e. submesh 0) must be chosen as "master" which will share
            with the other instanced entities (i.e. submeshes 1-N) which are called "slaves"
            @par
            Requirements to share trasnformations:
                * Both InstancedEntities must have use the same skeleton
                * An InstanceGeo can't be both "master" and "slave" at the same time
            @remarks
            Sharing does nothing if the original mesh doesn't have a skeleton
            When an InstanceGeo is removed (@see InstanceBatch::removeInstanceGeo), it stops
            sharing the transform. If the instanced entity was the master one, all it's slaves stop
            sharing and start having their own transform too.
            @param[in] slave The InstanceGeo that should share with us and become our slave
            @return true if successfully shared (may fail if they aren't skeletally animated)
        */
        bool shareTransformWith(InstanceGeo * slave);

        /** @see shareTransformWith
            Stops sharing the transform if this is a slave, and notifies the master we're no longer
            a slave.
            If this is a master, tells all it's slave to stop sharing
            @remarks
            This function is automatically called in InstanceBatch::removeInstanceGeo
        */
        void stopSharingTransform();

        InstanceBatch * _getOwner() const { return mBatchOwner; }

        /// @copydetails SpaceObj::getFactoryID
        FactoryID getFactoryID() const;

        /// @copydetails SpaceObj::getAABB
        const AABox & getAABB() const;

        /// @copydetails SpaceObj::getCenterRange
        NIIf getCenterRange() const;

        /** This is used by our batch owner to get the closest entity's depth, returns infinity
            when not attached to a scene node */
        NIIf distanceSquared(const Camera * cam) const;

        /// @copydetails SpaceObj::attach
        void attach(PosNode * parent, bool tag = false);

        /// @copydetails SpaceObj::_notify
        void _notify();

        /// @copydetails SpaceObj::_upload
        void queue(RenderQueue * queue){}

        /// @copydetails SpaceObj::query
        void query(GeometryQuery * query) {}

        /// @see GeometrySpace::hasSkeleton
        bool hasSkeleton() const { return mSkeleton != 0; }

        /// @see GeometrySpace::getSkeleton
        SkeletonFusion * getSkeleton(void) const { return mSkeleton; }

        /// @see GeometrySpace::getFusion
        AnimationFusion * getFusion(AnimationID id) const;

        /// @see GeometrySpace::getAnimationCtrl
        AnimationFusionObj * getAnimationCtrl() const;

        /** Called by InstanceBatch in <i>his</i> _update to tell us we need
            to calculate our bone matrices.
            @remarks Assumes it has a skeleton (mSkeleton != 0)
            @return true if something was actually updated
        */
        virtual bool _updateAnimation(void);

        /** Sets the transformation look up number */
        void setTransformLookupNumber(Nui16 num) { mTransformLookupNumber = num;}

        /** Retrieve the position */
        const Vector3f & getPos() const { return mPosition; }
        /** Set the position or the offset from the parent node if a parent node exists */
        void setPos(const Vector3f & position, bool doUpdate = true);

        /** Retrieve the orientation */
        const Quaternion & getOri() const { return mOrientation; }
        /** Set the orientation or the offset from the parent node if a parent node exists */
        void setOri(const Quaternion & orientation, bool doUpdate = true);

        /** Retrieve the local scale */
        const Vector3f & getScale() const { return mScale; }

        /** Set the  scale or the offset from the parent node if a parent node exists  */
        void setScale(const Vector3f & scale, bool doUpdate = true);

        /** Returns the maximum derived scale coefficient among the xyz values */
        NIIf getMaxScaleCoef() const;

        /** Update the world transform and derived values */
        void updateTransforms();

        /** Tells if the entity is in use. */
        bool isInUse() const { return mInUse; }

        /** Sets whether the entity is in use. */
        void setInUse(bool used);

        /// @copydetails SpaceObj::getSpaceMatrix
        virtual const Matrix4f & getSpaceMatrix() const
        {
            N_assert((!mNeedTransformUpdate || !mUseLocalTransform), "Transform data should be updated at this point");
            return mUseLocalTransform ? mFullLocalTransform :
                mSpaceNode ? mSpaceNode->getSpaceMatrix() : Matrix4f::IDENTITY;
        }

        /** Returns the derived position of the instanced entity including local transform */
        const Vector3f & setSpacePos() const
        {
            N_assert((!mNeedTransformUpdate || !mUseLocalTransform), "Transform data should be updated at this point");
            return mUseLocalTransform ? mDerivedLocalPosition :
                mSpaceNode ? mSpaceNode->getSpacePos() : Vector3f::ZERO;
        }

        /// @copydetails SpaceObj::isActive
        virtual bool isActive() const
        {
            //We assume that the instanced entity is in the scene if it is in use
            //It is in the scene whether it has a parent node or not
            return mInUse;
        }
    protected:
        /// Stores the master when we're the slave, store our slaves when we're the master
        typedef vector<InstanceGeo *>::type InstanceGeoVec;
    protected:
        Nui16 mInstanceId;                        //Note it may change after defragmenting!
        InstanceBatch * mBatchOwner;
        AnimationFusionObj * mAnimationState;
        SkeletonFusion * mSkeleton;
        Matrix4f * mBoneWorldMatrices;            //World space
        Nul mFrameAnimationLastUpdated;
        InstanceGeo * mSharedTransformEntity;    //When not null, another InstanceGeo controls the skeleton

        /** Used in conjunction with bone matrix lookup. Tells the number of the transform for
            as arranged in the vertex texture */
        Nui16 mTransformLookupNumber;
        InstanceGeoVec mSharingPartners;

        //////////////////////////////////////////////////////////////////////////
        // Parameters used for local transformation offset information
        // The
        //////////////////////////////////////////////////////////////////////////

        /// Object position
        Vector3f mPosition;
        Vector3f mDerivedLocalPosition;
        /// Object orientation
        Quaternion mOrientation;
        /// Object scale
        Vector3f mScale;
        /// The maximum absolute scale for all dimension
        NIIf mMaxScaleLocal;
        /// Full world transform
        Matrix4f mFullLocalTransform;
        /// Tells if mFullTransform needs an updated
        bool mNeedTransformUpdate;
        /// Tells if the animation world transform needs an update
        bool mNeedAnimTransformUpdate;
        /// Tells whether to use the local transform parameters
        bool mUseLocalTransform;

        bool mInUse;

        /// Returns number of matrices written to transform, assumes transform has enough space
        NCount getTransforms(Matrix4f * xform) const;

        /// Returns number of 32-bit values written
        NCount getTransforms3x4(NIIf * xform) const;

        /// Returns true if this InstancedObject is visible to the current camera
        bool findVisible(Camera * camera) const;

        /// Creates/destroys our own skeleton, also tells slaves to unlink if we're destroying
        void createSkeletonInstance();
        void destroySkeletonInstance();

        /// Just unlinks, and tells our master we're no longer sharing
        void unlinkTransform();

        /// Called when a slave has unlinked from us
        void notifyUnlink(const InstanceGeo * slave);

        /// Mark the transformation matrixes as dirty
        inline void markTransformDirty();

        /// Incremented count for next name extension
        static NameGenerator msNameGenerator;
    };
}
#endif