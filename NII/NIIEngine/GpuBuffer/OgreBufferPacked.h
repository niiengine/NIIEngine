/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

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

#ifndef _Ogre_BufferPacked_H_
#define _Ogre_BufferPacked_H_

#include "OgreResourceTransition.h"

namespace Ogre
{
    class _OgreExport GpuBuffer : public Buffer
    {
        friend class BufferOp;
    public:
        enum Buffer::BufferModeMark
        {
            /// Read access from GPU.
            /// i.e. Textures, most meshes.
            M_DEVREAD,

            /** Read and write access from GPU. No access for CPU at all.
                i.e. RenderTextures, vertex buffers that will be used for R2VB
            @remarks
                Ogre will use staging buffers to upload and download contents,
                but don't do this frequently (except where needed,
                i.e. live video capture)
            */
            M_DEVNORMAL,

            /// Read access from GPU. Write access for CPU.
            /// i.e. Particles, dynamic textures. Dynamic buffers don't put a
            /// hidden buffer behind the scenes. You get what you ask, therefore it's
            /// your responsability to ensure you don't lock a region that is currently
            /// in use by the GPU (or else stall).
            M_Normal,

            /// Same as BT_DYNAMIC, but mapping will be persistent.
            M_Normal | M_PersistentMap,

            /// Same as M_Normal | M_PersistentMap, but mapping will be persistent and cache coherent.
            M_NormalPersistentCoherent,
        };
        
        enum UnitType
        {
            UT_Pre,
            UT_Next,
            UT_Current
        }

        /** Generic constructor.
        @param initialData
            Initial data to populate. If bufferType == M_DEVREAD, can't be null.
        @param keepAsShadow
            Keeps "intialData" as a shadow copy for reading from CPU without querying the GPU
            (can be useful for reconstructing buffers on device/context loss or for efficient
            reading of the data without streaming back from GPU.)

            If keepAsShadow is false, caller is responsible for freeing the data

            If keepAsShadow is true, we're responsible for freeing the pointer. We will free the
            pointer using OGRE_FREE_SIMD( MEMCATEGORY_GEOMETRY ), in which case the pointer
            *must* have been allocated using OGRE_MALLOC_SIMD( MEMCATEGORY_GEOMETRY )

            If the constructor throws, then data will NOT be freed, and caller will have to do it.
            @see ScopeBuffer to help you with exceptions and correctly freeing the data.

            Must be false if bufferType >= BT_DYNAMIC
        */
        GpuBuffer( size_t byteoft, size_t unitCount, uint32 unitSize, uint32 eunitCount, 
            BufferModeMark bufferType, void * initialData, bool keepAsShadow, BufferOp * interface);

        virtual ~GpuBuffer();

        inline BufferOp * getGpuData() const                                { return mBufferInterface; }
        inline BufferModeMark getBufferType() const                        { return mBufferType; }
        inline BufferPackedTypes getBufferPackedType() const;               { return mType;}

        /// Async data read request. A ticket will be returned. Once the async transfer finishes,
        /// you can use the ticket to read the data from CPU. @See GpuBufferAsync
        GpuBufferAsyncPtr readRequest(size_t unitBegin, size_t unitCnt);

        /** Sends the provided data to the GPU
        @param data
            The data to transfer to the GPU. Caller is responsible for freeing the pointer.
            "data" starts at offset zero. i.e.
            dst[unitBegin * mUnitSize] = data[0];
        @param unitBegin
            The start region, usually zero.
        @param unitCnt
            Size, in number of elements, of data. Must be less than @getUnitCount - unitBegin
        */
        virtual void upload( const void * data, size_t unitBegin, size_t unitCnt );

        /** Maps the specified region to a pointer the CPU can access. Only dynamic buffers
            can use this function. The region [unitBegin; unitBegin + unitCnt)
            will be mapped.
        @remarks
            You can only map once per frame, regardless of parameters (except for advanceFrame).
            map( 0, 1 ) followed by map( 1, 1 ); is invalid.
            If you plan modifying elements 0 and 1; you should call map( 0, 2 )
        @par
            Note that even if you use persistent mapping, you still need to call @see unlock.
        @param unitBegin
            Start of the region to be mapped, in elements. Normally you want this to be 0.
        @param unitCnt
            Length of the region to map, in elements. @see getUnitCount to map the whole range.
            Can't be 0.
        @param bAdvanceFrame
            When true, the Buffer will be usable after unmapping it (or earlier if persistent mapped).
            However you won't be able to call map() again until the next frame.
            Calling this with false allows to call map multiple times. However ater calling unlock,
            you must call advanceFrame. THIS IS ONLY FOR VERY ADVANCED USERS.
        */
        void* RESTRICT_ALIAS_RETURN map( size_t unitBegin, size_t unitCnt, bool bAdvanceFrame=true );

        /** Unmaps or flushes the region mapped with @see map. Alternatively, you can flush a smaller region
            (i.e. you didn't know which regions you were to update when mapping, but now that you're done,
            you know).
            The region being flushed is [flushStart; flushStart + flushSize)
        @param unmapOption
            When using persistent mapping, UT_PersistentMap will keep the map alive; but you will
            have to call map again to use it. This requirement allows Ogre to:
                1. Synchronize if needed (avoid mapping a region that is still in use)
                2. Emulate persistent mapping on Hardware/Drivers that don't support it.
        @param unitBegin
            In elements, 0-based index (based on the mapped region) on where to start flushing from.
            Default is 0.
        @param unitCnt
            The length of the flushing region, which can't be bigger than 'unitCnt' passed
            to @see map. When this value is 0, we flush until the end of the buffer starting
            from unitBegin
        */
        void unlock( UnmapOptions option, size_t unitBegin = 0, size_t unitCnt = 0 );

        void advanceFrame(void);

        void regressFrame(void);

        /** Copies the contents of this buffer to another, using GPU -> GPU transfers.

            In simple terms it is similar to doing:
                memcpy( dstBuffer + dstElemStart, this + srcElemStart, srcNumElems );
        @remarks
            When both src and dst have different values for GpuBuffer::getUnitSize()
            then srcNumElems * this->getUnitSize() must be divisible
            by dstBuffer->getUnitSize()

            If dst has a shadow buffer, then src must have it too.
        @param dstBuffer
            Buffer to copy to. Must be of type M_DEVNORMAL
        @param dstElemStart
            The offset for dstBuffer. It must be in the unit of measure of dstBuffer.
            e.g. actual offset in bytes is dstElemStart * dstBuffer->getUnitSize()
        @param srcElemStart
            The offset of this buffer to start from
        */
        void read(GpuBuffer * dst, size_t destUnitBegin = 0, size_t srcUnitBegin = 0, size_t cnt = std::numeric_limits<size_t>::max() );

        /// Returns the mapping state. Note that if you call map with MS_PERSISTENT_INCOHERENT or
        /// MS_PERSISTENT_COHERENT, then call unlock( UT_PersistentMap ); the returned value will
        /// still be MS_PERSISTENT_INCOHERENT/_COHERENT when persistent mapping is supported.
        /// This differs from isMapping
        inline bool isMapping() const                    { return mMappingState; }

        /// Returns whether the buffer is currently mapped. If you've persistently mapped the buffer
        /// and then called unlock( UT_PersistentMap ); this function will return false; which
        /// differs from isMapping's behavior.
        inline bool isLock() const                          { return ( mMappingState == 0 || mLastMappingCount == 0 ) ? false : true;}

        inline size_t getUnitCount() const                  { return mUnitCount; }
        inline size_t getUnitSize() const                   { return mUnitSize; }
        inline size_t getSize() const                       { return mUnitCount * mUnitSize; }

        inline size_t getOffset() const                     { return mOffset; }
        inline size_t getCurrentOffset() const              { return mCurrentOffset; }
        inline size_t getAllocSize() const                  { return (mUnitCount + mExtUnitCount) * mUnitSize; }
        inline size_t getAllocUnitCount() const             { return mUnitCount + mExtUnitCount; }

        inline const void * getHostData() const             { return mShadowData; }
    protected:
        size_t mOffset;  
        size_t mCurrentOffset;
        size_t mUnitCount;
        size_t mUnitSize;
        size_t mExtUnitCount;
        size_t mMutexBegin;
        size_t mLastMappingCount;

        BufferModeMark mBufferType;
        BufferPackedTypes mType;
        void * mShadowData;

        bool mMappingState;

        BufferOp * mBufferInterface;

#if OGRE_DEBUG_MODE
        /// Used by Dynamic buffers only
        uint32 mLastFrameMapped;
        uint32 mLastFrameMappedAndAdvanced;
#endif
    };

    typedef StdVector<GpuBuffer *> BufferPackedVec;
    typedef StdUnorderedSet<GpuBuffer *> BufferPackedSet;
}

#endif
