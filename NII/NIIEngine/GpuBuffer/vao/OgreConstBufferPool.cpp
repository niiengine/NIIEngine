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

#include "OgreStableHeaders.h"

#include "OgreConstBufferPool.h"

#include "Vao/OgreVaoManager.h"
#include "Vao/OgreStagingBuffer.h"
#include "Vao/OgreConstBufferPacked.h"
#include "Vao/OgreTexBufferPacked.h"
#include "OgreRenderSystem.h"
#include "OgreProfiler.h"

namespace Ogre
{
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // ConstBufferPool
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    ConstBufferPool::ConstBufferPool( uint32 size, const ExtraBufferParams & param ) :
        mSlotSize( size ),
        mSlotCount( 0 ),
        mBufferSize( 0 ),
        mExtraBufferParams( param ),
        _mVaoManager( 0 ),
    #if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS && OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
        mOptimizationStrategy( LowerCpuOverhead )
    #else
        mOptimizationStrategy( LowerGpuOverhead )
    #endif
    {
    }
    //-----------------------------------------------------------------------------------
    ConstBufferPool::~ConstBufferPool()
    {
        destroyAllPool();
    }
    //-----------------------------------------------------------------------------------
    void ConstBufferPool::destroyAllPool(void)
    {
        {
            BufferPoolVecMap::const_iterator itor = mPools.begin();
            BufferPoolVecMap::const_iterator end  = mPools.end();

            while( itor != end )
            {
                BufferPoolVec::const_iterator it = itor->second.begin();
                BufferPoolVec::const_iterator en = itor->second.end();

                while( it != en )
                {
                    _mVaoManager->destroyConstBuffer( (*it)->mBuffer );

                    if( (*it)->mExtBuffer )
                    {
                        if( mExtraBufferParams.useTextureBuffers )
                        {
                            assert( dynamic_cast<TextureBuffer*>( (*it)->mExtBuffer ) );
                            _mVaoManager->destroyTexBuffer( static_cast<TextureBuffer*>((*it)->mExtBuffer ) );
                        }
                        else
                        {
                            assert( dynamic_cast<StructBuffer*>( (*it)->mExtBuffer ) );
                            _mVaoManager->destroyConstBuffer( static_cast<StructBuffer*>((*it)->mExtBuffer ) );
                        }
                    }

                    delete *it;

                    ++it;
                }

                ++itor;
            }

            mPools.clear();
        }

        {
            ConstBufferPoolUserVec::const_iterator itor = mUsers.begin();
            ConstBufferPoolUserVec::const_iterator end  = mUsers.end();

            while( itor != end )
            {
                (*itor)->mAssignedSlot  = 0;
                (*itor)->mAssignedPool  = 0;
                (*itor)->mGlobalIndex   = -1;
                (*itor)->mDirtyFlags    = DirtyNone;
                ++itor;
            }

            mUsers.clear();
        }
    }
    //-----------------------------------------------------------------------------------
    void ConstBufferPool::uploadDirtyDatablocks(void)
    {
        while( !mDirtyUsers.empty() )
        {
            //While inside ConstBufferPool::uploadToConstBuffer, the pool user may tag
            //itself dirty again, in which case we need to loop again. Move users
            //to a temporary array to avoid iterator invalidation from screwing us.
            mDirtyUsersTmp.swap( mDirtyUsers );
            uploadDirtyDatablocksImpl();
        }
    }
    //-----------------------------------------------------------------------------------
    void ConstBufferPool::uploadDirtyDatablocksImpl(void)
    {
        //assert( !mDirtyUsersTmp.empty() );

        OgreProfileExhaustive( "ConstBufferPool::uploadDirtyDatablocks" );

        const size_t materialSizeInGpu = mSlotSize;
        const size_t extraBufferSizeInGpu = mExtraBufferParams.mSlotSize;

        std::sort( mDirtyUsersTmp.begin(), mDirtyUsersTmp.end(), OrderConstBufferAUnitByPoolThenSlot );

        const size_t uploadSize = (materialSizeInGpu + extraBufferSizeInGpu) * mDirtyUsersTmp.size();
        MappedBuffer *stagingBuffer = _mVaoManager->getStagingBuffer( uploadSize, true );

        MappedBuffer::OperationList destinations;
        MappedBuffer::OperationList extraDestinations;

        destinations.reserve( mDirtyUsersTmp.size() );
        extraDestinations.reserve( mDirtyUsersTmp.size() );

        ConstBufferPoolUserVec::const_iterator itor = mDirtyUsersTmp.begin();
        ConstBufferPoolUserVec::const_iterator end  = mDirtyUsersTmp.end();

        char *bufferStart = reinterpret_cast<char*>( stagingBuffer->map( uploadSize ) );
        char *data      = bufferStart;
        char *extraData = bufferStart + materialSizeInGpu * mDirtyUsersTmp.size();

        while( itor != end )
        {
            const size_t srcOffset = static_cast<size_t>( data - bufferStart );
            const size_t dstOffset = (*itor)->getAssignedSlot() * materialSizeInGpu;

            uint8 dirtyFlags = (*itor)->mDirtyFlags;
            (*itor)->mDirtyFlags = DirtyNone;
            (*itor)->uploadToConstBuffer( data, dirtyFlags );
            data += materialSizeInGpu;

            const BufferPool *usersPool = (*itor)->getAssignedPool();

            MappedBuffer::Operation dst( usersPool->mBuffer, dstOffset, srcOffset, materialSizeInGpu );

            if( !destinations.empty() )
            {
                MappedBuffer::Operation &lastElement = destinations.back();

                if( lastElement.destination == dst.destination &&
                    (lastElement.dstOffset + lastElement.length == dst.dstOffset) )
                {
                    lastElement.length += dst.length;
                }
                else
                {
                    destinations.push_back( dst );
                }
            }
            else
            {
                destinations.push_back( dst );
            }

            if( usersPool->mExtBuffer )
            {
                (*itor)->uploadToExtraBuffer( extraData );

                const size_t extraSrcOffset = static_cast<size_t>( extraData - bufferStart );
                const size_t extraDstOffset = (*itor)->getAssignedSlot() * extraBufferSizeInGpu;

                extraData += extraBufferSizeInGpu;

                MappedBuffer::Operation extraDst( usersPool->mExtBuffer, extraDstOffset, extraSrcOffset, extraBufferSizeInGpu );

                if( !extraDestinations.empty() )
                {
                    MappedBuffer::Operation &lastElement = extraDestinations.back();

                    if( lastElement.destination == dst.destination &&
                        (lastElement.dstOffset + lastElement.length == dst.dstOffset) )
                    {
                        lastElement.length += dst.length;
                    }
                    else
                    {
                        extraDestinations.push_back( extraDst );
                    }
                }
                else
                {
                    extraDestinations.push_back( extraDst );
                }
            }

            ++itor;
        }

        destinations.insert( destinations.end(), extraDestinations.begin(), extraDestinations.end() );

        stagingBuffer->unmap( destinations );
        stagingBuffer->removeRef();

        mDirtyUsersTmp.clear();
    }
    //-----------------------------------------------------------------------------------
    void ConstBufferPool::requestSlot( uint32 hash, ConstBufferUnit *user, bool wantsExtraBuffer )
    {
        uint8 oldDirtyFlags = 0;
        if( user->mAssignedPool )
        {
            oldDirtyFlags = user->mDirtyFlags;
            releaseSlot( user );
        }

        BufferPoolVecMap::iterator it = mPools.find( hash );

        if( it == mPools.end() )
        {
            mPools[hash] = BufferPoolVec();
            it = mPools.find( hash );
        }

        BufferPoolVec &bufferPool = it->second;

        BufferPoolVec::iterator itor = bufferPool.begin();
        BufferPoolVec::iterator end  = bufferPool.end();

        while( itor != end && (*itor)->freeSlots.empty() )
            ++itor;

        if( itor == end )
        {
            StructBuffer * materialBuffer = _mVaoManager->createConstBuffer( mBufferSize, M_DEVNORMAL, 0, false );
            GpuBuffer * extraBuffer = 0;

            if( mExtraBufferParams.mSlotSize && wantsExtraBuffer )
            {
                if( mExtraBufferParams.useTextureBuffers )
                {
                    extraBuffer = _mVaoManager->createTexBuffer( PFG_RGBA32_FLOAT,
                                                                 mExtraBufferParams.mSlotSize * mSlotCount,
                                                                 mExtraBufferParams.bufferType,
                                                                 0, false );
                }
                else
                {
                    extraBuffer = _mVaoManager->createConstBuffer( mExtraBufferParams.mSlotSize * mSlotCount,
                                                                   mExtraBufferParams.bufferType,
                                                                   0, false );
                }
            }

            BufferPool *newPool = new BufferPool( hash, mSlotCount, materialBuffer, extraBuffer );
            bufferPool.push_back( newPool );
            itor = bufferPool.end() - 1;
        }

        BufferPool *pool = *itor;
        user->mAssignedSlot = pool->freeSlots.back();
        user->mAssignedPool = pool;
        user->mGlobalIndex  = mUsers.size();
        //user->mPoolOwner    = this;

        //If this assert triggers, you need to be consistent between your hashes
        //and the wantsExtraBuffer flag so that you land in the right pool.
        assert( (pool->mExtBuffer != 0) == wantsExtraBuffer &&
                "The pool was first requested with/without an extra buffer "
                "but now the opposite is being requested" );

        mUsers.push_back( user );

        pool->freeSlots.pop_back();

        scheduleForUpdate( user, DirtyConstBuffer|oldDirtyFlags );
    }
    //-----------------------------------------------------------------------------------
    void ConstBufferPool::releaseSlot( ConstBufferUnit *user )
    {
        BufferPool *pool = user->mAssignedPool;

        if( user->mDirtyFlags != DirtyNone )
        {
            ConstBufferPoolUserVec::iterator it = std::find( mDirtyUsers.begin(), mDirtyUsers.end(), user );

            if( it != mDirtyUsers.end() )
                remove( mDirtyUsers, it );
        }

        assert( user->mAssignedSlot < mSlotCount );
        assert( std::find( pool->freeSlots.begin(),
                           pool->freeSlots.end(),
                           user->mAssignedSlot ) == pool->freeSlots.end() );

        pool->freeSlots.push_back( user->mAssignedSlot );
        user->mAssignedSlot = 0;
        user->mAssignedPool = 0;
        //user->mPoolOwner    = 0;
        user->mDirtyFlags   = DirtyNone;

        assert( user->mGlobalIndex < mUsers.size() && user == *(mUsers.begin() + user->mGlobalIndex) &&
                "mGlobalIndex out of date or argument doesn't belong to this pool manager" );
        ConstBufferPoolUserVec::iterator itor = mUsers.begin() + user->mGlobalIndex;
        itor = remove( mUsers, itor );

        //The node that was at the end got swapped and has now a different index
        if( itor != mUsers.end() )
            (*itor)->mGlobalIndex = itor - mUsers.begin();
    }
    //-----------------------------------------------------------------------------------
    void ConstBufferPool::scheduleForUpdate( ConstBufferUnit *dirtyUser, uint8 dirtyFlags )
    {
        assert( dirtyFlags != DirtyNone );

        if( dirtyUser->mDirtyFlags == DirtyNone )
            mDirtyUsers.push_back( dirtyUser );
        dirtyUser->mDirtyFlags |= dirtyFlags;
    }
    //-----------------------------------------------------------------------------------
    size_t ConstBufferPool::getPoolIndex(ConstBufferUnit * user) const
    {
        BufferPool *pool = user->mAssignedPool;

        assert( user->mAssignedSlot < mSlotCount );
        assert( std::find( pool->freeSlots.begin(), pool->freeSlots.end(), user->mAssignedSlot ) == pool->freeSlots.end() );

        BufferPoolVecMap::const_iterator itor = mPools.find( pool->hash );
        assert( itor != mPools.end() && "Error or argument doesn't belong to this pool manager." );

        const BufferPoolVec &poolVec = itor->second;
        BufferPoolVec::const_iterator it = std::find( poolVec.begin(), poolVec.end(), pool );

        return it - poolVec.begin();
    }
    //-----------------------------------------------------------------------------------
    struct OldUserRecord
    {
        ConstBufferUnit *user;
        uint32              hash;
        bool                wantsExtraBuffer;
        OldUserRecord( ConstBufferUnit *_user, uint32 _hash, bool _wantsExtraBuffer ) :
            user( _user ), hash( _hash ), wantsExtraBuffer( _wantsExtraBuffer ) {}
    };
    typedef std::vector<OldUserRecord> OldUserRecordVec;
    void ConstBufferPool::setOptimizationStrategy( OptimizationStrategy ostrategy )
    {
        if( mOptimizationStrategy != ostrategy )
        {
            mDirtyUsers.clear();

            OldUserRecordVec oldUserRecords;
            {
                //Save all the data we need before we destroy the pools.
                ConstBufferPoolUserVec::const_iterator itor = mUsers.begin();
                ConstBufferPoolUserVec::const_iterator end  = mUsers.end();

                while( itor != end )
                {
                    OldUserRecord record( *itor, (*itor)->mAssignedPool->hash,
                                          (*itor)->mAssignedPool->mExtBuffer != 0 );
                    oldUserRecords.push_back( record );
                    ++itor;
                }
            }

            destroyAllPool();
            mOptimizationStrategy = ostrategy;
            if( _mVaoManager )
            {
                if( mOptimizationStrategy == LowerCpuOverhead )
                    mBufferSize = std::min<size_t>( _mVaoManager->getMaxSize(GBT_Struct), 64 * 1024 );
                else
                    mBufferSize = mSlotSize;

                mSlotCount = mBufferSize / mSlotSize;
            }

            {
                //Recreate the pools.
                OldUserRecordVec::const_iterator itor = oldUserRecords.begin();
                OldUserRecordVec::const_iterator end  = oldUserRecords.end();

                while( itor != end )
                {
                    this->requestSlot( itor->hash, itor->user, itor->wantsExtraBuffer );
                    itor->user->notifyOptimizationStrategyChanged();
                    ++itor;
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    ConstBufferPool::OptimizationStrategy ConstBufferPool::getOptimizationStrategy() const
    {
        return mOptimizationStrategy;
    }
    //-----------------------------------------------------------------------------------
    void ConstBufferPool::_changeRenderSystem( RenderSystem *newRs )
    {
        if( _mVaoManager )
        {
            destroyAllPool();
            mDirtyUsers.clear();
            _mVaoManager = 0;
        }

        if( newRs )
        {
            _mVaoManager = newRs->getBufferManager();

            if( mOptimizationStrategy == LowerCpuOverhead )
                mBufferSize = std::min<size_t>( _mVaoManager->getMaxSize(GBT_Struct), 64 * 1024 );
            else
                mBufferSize = mSlotSize;

            mSlotCount = mBufferSize / mSlotSize;
        }
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    ConstBufferPool::BufferPool::BufferPool( uint32 _hash, uint32 slotsPerPool,
                                             StructBuffer *_materialBuffer,
                                             GpuBuffer *_extraBuffer ) :
        hash( _hash ),
        mBuffer( _materialBuffer ),
        mExtBuffer( _extraBuffer )
    {
        freeSlots.reserve( slotsPerPool );
        for( uint32 i=0; i<slotsPerPool; ++i )
            freeSlots.push_back( (slotsPerPool - i) - 1 );
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    ConstBufferPool::ExtraBufferParams::ExtraBufferParams( size_t _bytesPerSlot, Buffer::ModeMark _bufferType,
                                                           bool _useTextureBuffers ) :
        mSlotSize( _bytesPerSlot ),
        bufferType( _bufferType ),
        useTextureBuffers( _useTextureBuffers )
    {
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    ConstBufferUnit::ConstBufferUnit() :
        mAssignedSlot( 0 ),
        mAssignedPool( 0 ),
        //mPoolOwner( 0 ),
        mGlobalIndex( -1 ),
        mDirtyFlags( ConstBufferPool::DirtyNone )
    {
    }
}
