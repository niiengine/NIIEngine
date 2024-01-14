/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef _OgreConstBufferPool_H_
#define _OgreConstBufferPool_H_

#include "OgrePrerequisites.h"
#include "Vao/OgreBufferPacked.h"

#include "ogrestd/map.h"
#include "ogrestd/vector.h"

#include "OgreHeaderPrefix.h"

namespace Ogre
{
    /** \addtogroup Core
    *  @{
    */
    /** \addtogroup Resources
    *  @{
    */

    class ConstBufferUnit;

    /** Maintains a pool of buffers grouped by hash ID. Keeping them separated
        by hash prevents mixing materials in the same buffer (which has limited
        space) that could never be batched together in the same draw call.

        When a buffer is full and has used all of its free slots, a new buffer
        is allocated.
    */
    class _OgreExport ConstBufferPool
    {
    public:
        struct BufferPool
        {
            uint32 hash;
            vector<uint32>::type freeSlots;
            StructBuffer * mBuffer;
            GpuBuffer         * mExtBuffer;

            BufferPool(uint32 _hash, uint32 slotsPerPool, StructBuffer *_materialBuffer, GpuBuffer * _extraBuffer );
        };

        struct _OgreExport ExtraBufferParams
        {
            size_t mSlotSize;
            BufferModeMark bufferType;
            bool useTextureBuffers;

            ExtraBufferParams( size_t _bytesPerSlot = 0, BufferModeMark _bufferType = M_DEVNORMAL, bool _useTextureBuffers = true );
        };

        enum OptimizationStrategy
        {
            /// Optimize for low CPU overhead. Use this on slow CPUs (i.e. older AMD CPU
            /// models) or if CPU is the bottleneck. This strategy is optimized for having
            /// a lot of different materials.
            /// Default on Desktop.
            LowerCpuOverhead,
            /// Optimize for low GPU overhead. Use this if GPU is the bottleneck.
            /// This strategy performs best when you have few materials.
            /// Do not enable this mode blindly unless you've profiled and
            /// it gives you important benefits.
            /// Default on mobile.
            LowerGpuOverhead
        };

        enum DirtyFlags
        {
            DirtyNone           = 0u,
            DirtyConstBuffer    = 1u << 0u,
            DirtyTextures       = 1u << 1u,
            DirtySamplers       = 1u << 2u
        };
    public:
        ConstBufferPool( uint32 bytesPerSlot, const ExtraBufferParams &params );
        virtual ~ConstBufferPool();

        /// Requests a slot and fills 'user'. Automatically schedules for update
        void requestSlot( uint32 hash, ConstBufferUnit *user, bool wantsExtraBuffer );

        /// Releases a slot requested with requestSlot.
        void releaseSlot( ConstBufferUnit *user );

        void scheduleForUpdate( ConstBufferUnit *dirtyUser, uint8 dirtyFlags = DirtyConstBuffer );

        /// Gets an ID corresponding to the pool this user was assigned to, unique per hash.
        size_t getPoolIndex( ConstBufferUnit *user ) const;

        /** Sets the optimization strategy. See OptimizationStrategy.
        @remarks
            Changing the optimization strategy on the fly
            will trigger a massive shader recompilation.
        @par
            Implementations that don't support different strategies can overload this function.
        */
        virtual void setOptimizationStrategy( OptimizationStrategy ostrategy );

        OptimizationStrategy getOptimizationStrategy() const;

        virtual void _changeRenderSystem( RenderSystem *newRs );
    protected:
        void destroyAllPool(void);

        void uploadDirtyDatablocks(void);
        void uploadDirtyDatablocksImpl(void);
    protected:
        typedef vector<BufferPool*>::type           BufferPoolVec;
        typedef map<uint32, BufferPoolVec>::type    BufferPoolVecMap;

        typedef vector<ConstBufferUnit*>::type  ConstBufferPoolUserVec;

        BufferPoolVecMap    mPools;
        uint32              mSlotSize;
        uint32              mSlotCount;
        size_t              mBufferSize;
        ExtraBufferParams   mExtraBufferParams;
    protected:
        ConstBufferPoolUserVec mDirtyUsers;
        ConstBufferPoolUserVec mDirtyUsersTmp;
        ConstBufferPoolUserVec mUsers;

        OptimizationStrategy    mOptimizationStrategy;
    private:
        GBufferManager * _mVaoManager;
    };

    class _OgreExport ConstBufferUnit
    {
        friend class ConstBufferPool;
    public:
        ConstBufferUnit();

        uint32 getAssignedSlot(void) const                              { return mAssignedSlot; }
        const ConstBufferPool::BufferPool* getAssignedPool(void) const  { return mAssignedPool; }

        uint8 getDirtyFlags(void) const                                 { return mDirtyFlags; }
    protected:
        /// Derived class must fill dstPtr. Amount of bytes written can't
        /// exceed the value passed to ConstBufferPool::uploadDirtyDatablocks
        virtual void uploadToConstBuffer( char *dstPtr, uint8 dirtyFlags ) = 0;
        virtual void uploadToExtraBuffer( char *dstPtr ) {}

        virtual void notifyOptimizationStrategyChanged(void) {}
        
        bool OrderConstBufferAUnitByPoolThenSlot( const ConstBufferUnit *_l, const ConstBufferUnit *_r );

        uint32                      mAssignedSlot;
        ConstBufferPool::BufferPool *mAssignedPool;
        size_t                      mGlobalIndex;
        uint8                       mDirtyFlags;
    };

    inline bool OrderConstBufferAUnitByPoolThenSlot( const ConstBufferUnit *_l, const ConstBufferUnit *_r )
    {
        return _l->mAssignedPool < _r->mAssignedPool && _l->mAssignedSlot < _r->mAssignedSlot;
    }

    /** @} */
    /** @} */

}

#include "OgreHeaderSuffix.h"

#endif
