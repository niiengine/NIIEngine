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

#include "OgreGLES2HardwareBuffer.h"
#include "OgreRoot.h"
#include "OgreGLES2RenderSystem.h"
#include "OgreGLES2StateCacheManager.h"

namespace Ogre 
{
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLES2Buffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GLES2Buffer::GLES2Buffer( GLuint vboid, GLuint size, GLES2GBufferManager * mag, BufferModeMark mode ) :
        mVboName( vboid ),
        mVboSize( size ),
        mMappedPtr( 0 ),
        mPersistentMethod( mode )
    {
        if( !N_Engine().getFeature()->isSupport(CF_PersistentMapping))
            mPersistentMethod = M_NORMAL;
    }
    //-----------------------------------------------------------------------------------
    GLES2Buffer::~GLES2Buffer()
    {
    }
    //-----------------------------------------------------------------------------------
    NCount GLES2Buffer::addMappedRange(NCount start, NCount count)
    {
        NCount ticket;

        if(!mFreeRanges.empty())
        {
            ticket = mFreeRanges.back();
            mMappedRanges[ticket] = MappedRange(start, count);
            mFreeRanges.pop_back();
        }
        else
        {
            ticket = mMappedRanges.size();
            mMappedRanges.push_back(MappedRange(start, count));
        }

        return ticket;
    }
    //-----------------------------------------------------------------------------------
    void* GLES2Buffer::map(NCount start, NCount count, NCount & outTicket)
    {
        N_assert( start <= mVboSize && start + count <= mVboSize );

        if( mMappedRanges.size() == mFreeRanges.size() )
        {
            GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;

//            if( N_MarkTrue(mPersistentMethod, M_PersistentMap) )
//            {
//                flags |= GL_MAP_PERSISTENT_BIT;
//
//                if(N_MarkTrue(mPersistentMethod, M_CoherentMap))
//                    flags |= GL_MAP_COHERENT_BIT;
//            }
//            else
            {
                flags |= GL_MAP_UNSYNCHRONIZED_BIT;
            }

            GLCheck( mMappedPtr = glMapBufferRange( GL_COPY_WRITE_BUFFER, 0, mVboSize, flags ) );
        }

        outTicket = addMappedRange( start, count );

        return static_cast<uint8 *>(mMappedPtr) + start;
    }
    //-----------------------------------------------------------------------------------
    void GLES2Buffer::flush( NCount ticket, NCount start, NCount count )
    {
        N_assert( start <= mMappedRanges[ticket].count && start + count <= mMappedRanges[ticket].count );

        GLCheck( glFlushMappedBufferRange( GL_COPY_WRITE_BUFFER, mMappedRanges[ticket].start + start, count ) );
    }
    //-----------------------------------------------------------------------------------
    void GLES2Buffer::unmap(NCount ticket)
    {
        N_assert(ticket < mMappedRanges.size() && "Invalid unmap ticket!" );
        N_assert(mMappedRanges.size() != mFreeRanges.size() && "Unmapping an already unmapped buffer! Did you call unmap with the same ticket twice?");

        mFreeRanges.push_back(ticket);

        if(mMappedRanges.size() == mFreeRanges.size())
        {
            GLCheck(glUnmapBuffer( GL_COPY_WRITE_BUFFER));
            mMappedPtr = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLES2BufferOp
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GLES2BufferOp::GLES2BufferOp( size_t vboidx, GLuint vboid, GLES2Buffer * glbuf) :
        mGroupIndex( vboidx ),
        mVboName( vboid ),
        mMappedPtr( 0 ),
        mUnmapTicket( (size_t)~0 ),
        mGLBuffer( glbuf )
    {
    }
    //-----------------------------------------------------------------------------------
    GLES2BufferOp::~GLES2BufferOp()
    {
    }
    //-----------------------------------------------------------------------------------
    void GLES2BufferOp::initImpl(void * data, NCount oft, NCount size)
    {
        //In OpenGL; immutable buffers are a charade. They're mostly there to satisfy D3D11's needs.
        //However we emulate the behavior and trying to upload to an immutable buffer will result
        //in an exception or an N_assert, thus we temporarily change the type.
        BufferModeMark otype = mBuffer->mMark;
        if(N_MarkOnlyTrue(mBuffer->mMark, M_USAGEMARK, M_DEVREAD))
            mBuffer->mMark = M_DEVREAD | M_DEVWRITE;

        mBuffer->write(data, oft, size);

        mBuffer->mMark = otype;
    }
    //-----------------------------------------------------------------------------------
    void * GLES2BufferOp::lockImpl(MuteMark m, NCount oft, NCount size, NCount multiIdx)
    {
        //size_t bytesPerElement = mBuffer->mUnitSize;

        GLES2GBufferManager * mag = static_cast<GLES2GBufferManager*>( mBuffer->mMag );
        bool persistent = N_Engine().getFeature()->isSupport(CF_PersistentMapping);

        mag->wait();

        //size_t dFrame = advanceFrame( bAdvanceFrame );

        if( !mBuffer->mMapping || !persistent )
        {
            //Non-persistent buffers just map the small region they'll need.
            size_t offset;
            size_t length;

            if( N_MarkTrue(mBuffer->mMark, M_PersistentMap) && N_MarkTrue(m, MM_Persistent) && persistent )
            {
                //Persistent buffers map the *whole* assigned buffer,
                //we later care for the offsets and lengths
                offset = mBuffer->mOffset;
                length = mBuffer->getReserveSize() * mBuffer->getMutilCount();
            }
            else
            {
                offset = mBuffer->mOffset + oft + mBuffer->getReserveSize() * multiIdx;
                length = size;
            }

            GLCheck( glBindBuffer( GL_COPY_WRITE_BUFFER, mVboName ) );
            mMappedPtr = mGLBuffer->map( offset, length, mUnmapTicket );
            mBuffer->mMapping = true;
        }

        //For regular maps, mMuteOft is 0. So that we can later flush correctly.
        //mBuffer->mMuteSize = size;

        char * retVal = (char*)mMappedPtr;

        if( N_MarkTrue(mBuffer->mMark, M_PersistentMap) && N_MarkTrue(m, MM_Persistent) && persistent )
        {
            //For persistent maps, we've mapped the whole 3x size of the buffer. mMuteOft
            //points to the right offset so that we can later flush correctly.
            mBuffer->mMuteOft = oft + mBuffer->getReserveSize() * multiIdx;
            retVal += mBuffer->mMuteOft;
        }
        else
        {
            mBuffer->mMuteOft = 0;
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void GLES2BufferOp::unlockImpl(UnlockType type, NCount oft, NCount size)
    {
        N_assert( oft <= mBuffer->mMuteSize && "Flush starts after the end of the mapped region!" );
        N_assert( oft + size <= mBuffer->mMuteSize && "Flush region out of bounds!" );

        bool persistent = N_Engine().getFeature()->isSupport(CF_PersistentMapping);

        if( N_MarkFalse(mBuffer->mMark, M_CoherentMap) || type == UT_UnlockMap || !persistent )
        {
            if(!size)
                size = mBuffer->mMuteSize - oft;

            GLCheck(glBindBuffer(GL_COPY_WRITE_BUFFER, mVboName));
            mGLBuffer->flush( mUnmapTicket, mBuffer->mMuteOft + oft, size);

            if( type == UT_UnlockMap || !persistent || N_MarkOnlyTrue(mBuffer->mMark, M_USAGEMARK, M_NORMAL) )
            {
                mGLBuffer->unmap(mUnmapTicket);
                mMappedPtr = 0;
                mBuffer->mMapping = false;
            }
        }
    }
    //-----------------------------------------------------------------------------------
    /*void GLES2BufferOp::advanceFrame(void)
    {
        advanceFrame( true );
    }*/
    //-----------------------------------------------------------------------------------
    /*size_t GLES2BufferOp::advanceFrame( bool bAdvanceFrame )
    {
        GLES2GBufferManager * mag = static_cast<GLES2GBufferManager*>( mBuffer->mMag );
        size_t dFrame = mBuffer->mCurrentOffset - mBuffer->mOffset;
        dFrame /= mBuffer->getReserveSize();

        dFrame = (dFrame + 1) % mag->getMutilCount();

        if( bAdvanceFrame )
        {
            mBuffer->mCurrentOffset = mBuffer->mOffset + dFrame * mBuffer->getReserveSize();
        }

        return dFrame;
    }
    //-----------------------------------------------------------------------------------
    void GLES2BufferOp::regressFrame(void)
    {
        GLES2GBufferManager *mag = static_cast<GLES2GBufferManager*>( mBuffer->mMag );
        size_t dFrame = mBuffer->mCurrentOffset - mBuffer->mOffset;
        dFrame /= mBuffer->getReserveSize();

        dFrame = (dFrame + mag->getMutilCount() - 1) % mag->getMutilCount();

        mBuffer->mCurrentOffset = mBuffer->mOffset + dFrame * mBuffer->getReserveSize();
    }*/
    //-----------------------------------------------------------------------------------
    bool GLES2BufferOp::reserveImpl(NCount size, BufferModeMark newMode, bool oldData)
    {
        
    }
    //-----------------------------------------------------------------------------------
    GLenum getGLUsage(uint32 usage)
    {
        if (N_MarkTrue(usage, M_WHOLE))
            return GL_STREAM_DRAW;
        else if(N_MarkFalse(usage, M_HOST))
            return GL_STATIC_DRAW;
        
        return GL_DYNAMIC_DRAW;
    }
    //-----------------------------------------------------------------------------------
    void GLES2BufferOp::memcpyImpl(void * data, NCount oft, NCount size, bool write)
    {
        if(write)
        {
            GLCheck( glBindBuffer( GL_COPY_WRITE_BUFFER, mVboName ) );
            if (oft == 0 && size == mSize)
            {
                GLCheck(glBufferData(GL_COPY_WRITE_BUFFER, mSize, data, getGLUsage(mBuffer->mMark)));
            }
            else
            {
                if(oft == 0 && size == mSize)
                {
                    GLCheck(glBufferData(GL_COPY_WRITE_BUFFER, mSize, NULL, getGLUsage(mBuffer->mMark)));
                }

                GLCheck(glBufferSubData(GL_COPY_WRITE_BUFFER, oft, size, data));
            }
        }
        else
        {
            GLCheck( glBindBuffer( GL_COPY_READ_BUFFER, mVboName ) );
            GLCheck(glGetBufferSubData(GL_COPY_READ_BUFFER, oft, size, data));
        }
    }
    //-----------------------------------------------------------------------------------
    void GLES2BufferOp::memcpyImpl(Buffer * src, NCount srcOft, NCount oft, NCount size )
    {
        N_assert( dynamic_cast<GLES2BufferOp *>( src->mOp ) );
        GLES2BufferOp * srcbuf = static_cast<GLES2BufferOp*>( src->mOp );

        GLCheck( glBindBuffer( GL_COPY_WRITE_BUFFER, mVboName ) );
        GLCheck( glBindBuffer( GL_COPY_READ_BUFFER, srcbuf->getVboName() ) );

        GLCheck( glCopyBufferSubData( GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, srcOft, oft, size ) );
    }
    //-----------------------------------------------------------------------------------
    void GLES2BufferOp::waitImpl()
    {
        
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLES2HardwareBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GLES2HardwareBuffer::GLES2HardwareBuffer(GLenum target, size_t sizeInBytes, GLenum usage)
        : mTarget(target), mSizeInBytes(sizeInBytes), mUsage(usage)
    {
        mRenderSys = static_cast<GLES2RenderSystem*>(Root::getSingleton().getRenderSystem());
        createBuffer();
    }
    //-----------------------------------------------------------------------------------
    GLES2HardwareBuffer::~GLES2HardwareBuffer()
    {
        destroyBuffer();
    }
    //-----------------------------------------------------------------------------------
    void GLES2HardwareBuffer::createBuffer()
    {
        GLCheck(glGenBuffers(1, &mBufferId));

        if (!mBufferId)
        {
            N_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Cannot create GL ES buffer");
        }
        
        mRenderSys->getStateManager()->bindGLBuffer(mTarget, mBufferId);

        if(mRenderSys->getCapabilities()->hasCapability(RSC_DEBUG))
        {
            GLCheck(glLabelObjectEXT(GL_BUFFER_OBJECT_EXT, mBufferId, 0, ("Buffer #" + StrConv::conv(mBufferId)).c_str()));
        }

        GLCheck(glBufferData(mTarget, mSizeInBytes, NULL, getGLUsage(mUsage)));
    }
    //-----------------------------------------------------------------------------------
    void GLES2HardwareBuffer::destroyBuffer()
    {
        // Delete the cached value
        if(GLES2StateCacheManager* stateCacheManager = mRenderSys->getStateManager())
            stateCacheManager->deleteGLBuffer(mTarget, mBufferId);
    }
    //-----------------------------------------------------------------------------------
    void * GLES2HardwareBuffer::lockImpl(size_t offset, size_t length, Buffer::LockOptions options)
    {
        GLenum access = 0;

        // Use glMapBuffer
        mRenderSys->getStateManager()->bindGLBuffer(mTarget, mBufferId);

        bool writeOnly =
            options == Buffer::HBL_WRITE_ONLY ||
            ((mUsage & Buffer::HBU_WRITE_ONLY) &&
             options != Buffer::HBL_READ_ONLY && options != Buffer::HBL_NORMAL);

        void* pBuffer = NULL;
        if(mRenderSys->getCapabilities()->hasCapability(RSC_MAPBUFFER))
        {
            if (writeOnly)
            {
                access = GL_MAP_WRITE_BIT_EXT;
                if (options == Buffer::HBL_DISCARD ||
                    options == Buffer::HBL_NO_OVERWRITE)
                {
                    // Discard the buffer
                    access |= GL_MAP_INVALIDATE_RANGE_BIT_EXT;
                }
            }
            else if (options == Buffer::HBL_READ_ONLY)
                access = GL_MAP_READ_BIT_EXT;
            else
                access = GL_MAP_READ_BIT_EXT | GL_MAP_WRITE_BIT_EXT;

            GLCheck(pBuffer = glMapBufferRangeEXT(mTarget, offset, length, access));
        }

        if (!pBuffer)
        {
            N_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Buffer: Out of memory");
        }

        // pBuffer is already offsetted in glMapBufferRange
        return static_cast<uint8*>(pBuffer);
    }
    //-----------------------------------------------------------------------------------
    void GLES2HardwareBuffer::unlockImpl()
    {
        mRenderSys->getStateManager()->bindGLBuffer(mTarget, mBufferId);

        if(mRenderSys->getCapabilities()->hasCapability(RSC_MAPBUFFER)) {
            GLboolean mapped;
            GLCheck(mapped = glUnmapBufferOES(mTarget));
            if(!mapped)
            {
                N_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Buffer data corrupted, please reload");
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void GLES2HardwareBuffer::readData(size_t offset, size_t length, void * pDest)
    {
        if(!OGRE_NO_GLES3_SUPPORT || mRenderSys->checkGpuApi("GL_EXT_map_buffer_range"))
        {
            // Map the buffer range then copy out of it into our destination buffer
            void* srcData;
            GLCheck(srcData = glMapBufferRangeEXT(mTarget, offset, length, GL_MAP_READ_BIT_EXT));
            memcpy(pDest, srcData, length);

            // Unmap the buffer since we are done.
            GLboolean mapped;
            GLCheck(mapped = glUnmapBufferOES(mTarget));
            if(!mapped)
            {
                N_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Buffer data corrupted, please reload");
            }
        }
        else
        {
            N_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Read hardware buffer is not supported");
        }
    }
    //-----------------------------------------------------------------------------------
    void GLES2HardwareBuffer::writeData(size_t offset, size_t length, const void* pSource, bool discardWholeBuffer)
    {
        mRenderSys->getStateManager()->bindGLBuffer(mTarget, mBufferId);

        if (offset == 0 && length == mSizeInBytes)
        {
            GLCheck(glBufferData(mTarget, mSizeInBytes, pSource, getGLUsage(mUsage)));
        }
        else
        {
            if(discardWholeBuffer)
            {
                GLCheck(glBufferData(mTarget, mSizeInBytes, NULL, getGLUsage(mUsage)));
            }

            GLCheck(glBufferSubData(mTarget, offset, length, pSource));
        }
    }
    //-----------------------------------------------------------------------------------
    void GLES2HardwareBuffer::copyData(GLuint srcBufferId, size_t srcOffset, size_t dstOffset, size_t length, bool discardWholeBuffer)
    {
#if OGRE_NO_GLES3_SUPPORT == 0
        // Zero out this(destination) buffer
        GLCheck(glBindBuffer(mTarget, mBufferId));
        GLCheck(glBufferData(mTarget, length, 0, getGLUsage(mUsage)));
        GLCheck(glBindBuffer(mTarget, 0));

        // Do it the fast way.
        GLCheck(glBindBuffer(GL_COPY_READ_BUFFER, srcBufferId));
        GLCheck(glBindBuffer(GL_COPY_WRITE_BUFFER, mBufferId));

        GLCheck(glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, srcOffset, dstOffset, length));

        GLCheck(glBindBuffer(GL_COPY_READ_BUFFER, 0));
        GLCheck(glBindBuffer(GL_COPY_WRITE_BUFFER, 0));
#else
        OgreAssert(false, "GLES3 needed");
#endif
    }
    //-----------------------------------------------------------------------------------
    GLenum GLES2HardwareBuffer::getGLUsage(unsigned int usage)
    {
        if (N_MarkTrue(usage, M_WHOLE))
            return GL_STREAM_DRAW;
        else if(N_MarkFalse(usage, M_HOST))
            return GL_STATIC_DRAW;
        
        return GL_DYNAMIC_DRAW;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLES2VertexBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GLES2VertexBuffer::GLES2VertexBuffer(BufferManager * mag, NCount size, NCount count, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) : 
        VertexBuffer(mag, size, count, multi, mode, initData, gid, goft, padSize)
    {
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLES2IndexBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GLES2IndexBuffer::GLES2IndexBuffer(BufferManager * mag, NCount size, NCount count, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) : 
        IndexBuffer(mag, size, count, multi, mode, initData, gid, goft, padSize)
    {
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLES2StructBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GLES2StructBuffer::GLES2StructBuffer(BufferManager * mag, NCount size, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) :
        StructBuffer(mag, size, multi, mode, initData, gid, goft, padSize)
    {
    }
    //-----------------------------------------------------------------------------------
    GLES2StructBuffer::~GLES2StructBuffer()
    {
    }
    //-----------------------------------------------------------------------------------
    inline void GLES2StructBuffer::bindBuffer(Nui16 slot)
    {
        N_assert(dynamic_cast<GLES2BufferOp *>(mOp));

        GLES2BufferOp * op = static_cast<GLES2BufferOp *>(mOp);

        GLCheck(glBindBufferRange(GL_UNIFORM_BUFFER, slot, op->getVboName(), mCurrentMulti * mReserveSize, mUnitCount * mUnitSize));
    }
    //-----------------------------------------------------------------------------------
    void GLES2StructBuffer::bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft, NCount size)
    {
        N_assert(dynamic_cast<GLES2BufferOp *>(mOp));

        GLES2BufferOp *op = static_cast<GLES2BufferOp *>(mOp);
        
        if(!size)
            size = mUnitCount * mUnitSize;
        GLCheck(glBindBufferRange(GL_UNIFORM_BUFFER, slot, op->getVboName(), mCurrentMulti * mReserveSize + oft, size));
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLES2TextureBufferEmu
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GLES2TextureBufferEmu::GLES2TextureBufferEmu(BufferManager * mag, NCount size, PixelFormat pf, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) :
        TextureBuffer(mag, size, pf, multi, mode, initData, gid, goft, padSize)
    {
        GLCheck(glGenTextures(1, &mTexName));
        
        mInternalFormat = GLES2PixelUtil::getGLImageInternalFormat(pf);

        GLCheck(glBindTexture(GL_TEXTURE_2D, mTexName));

        mMaxTexSize = 2048;

        mOriginFormat = GLES2PixelUtil::getGLOriginFormat(pf, false);
        mOriginDataType = GLES2PixelUtil::getGLOriginDataType(pf);

        mInternalNumElemBytes = PixelUtil::getNumElemBytes(pf);

        mInternalNumElements = numElements / mInternalNumElemBytes;

        size_t width = std::min( mMaxTexSize,  mInternalNumElements);

        size_t texHeight = (mInternalNumElements + mMaxTexSize - 1) / mMaxTexSize;

        GLCheck( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 ) );
        GLCheck( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 ) );
        GLCheck( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ) );
        GLCheck( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ) );
        GLCheck( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ) );
        GLCheck( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ) );
        GLCheck( glTexImage2D( GL_TEXTURE_2D, 0, mInternalFormat, width, texHeight,
                            0, mOriginFormat, mOriginDataType, NULL ) );
    }
    //-----------------------------------------------------------------------------------
    GLES2TextureBufferEmu::~GLES2TextureBufferEmu()
    {
        GLCheck(glDeleteTextures(1, &mTexName));
    }
    //-----------------------------------------------------------------------------------
    void GLES2TextureBufferEmu::bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft, NCount size)
    {
        N_assert(dynamic_cast<GLES2BufferOp *>(mOp));
        N_assert( oft < (mUnitCount * mUnitSize - 1) );
        N_assert( (oft + size) <= mUnitCount * mUnitSize );

        size = !size ? (mUnitCount * mUnitSize - oft) : size;

        GLES2BufferOp * op = static_cast<GLES2BufferOp *>(mOp);

        size_t numModifiedElements = size / mInternalNumElemBytes;
        N_assert( size % mInternalNumElemBytes == 0 );
        size_t texWidth = std::min( numModifiedElements, std::min( mMaxTexSize, mInternalNumElements ) );
        size_t texHeight = ( numModifiedElements + mMaxTexSize - 1 ) / mMaxTexSize;

        if( (mUnitSize & 4) != 4 )
        {
            // Standard alignment of 4 is not right for some formats.
            GLCheck( glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ) );
        }

        GLCheck( glBindBuffer( GL_PIXEL_UNPACK_BUFFER, op->getVboName() ) );
        GLCheck( glActiveTexture( GL_TEXTURE0 + slot ) );
        GLCheck( glBindTexture( GL_TEXTURE_2D, mTexName ) );
        GLCheck( glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, texWidth, texHeight, mOriginFormat, mOriginDataType, reinterpret_cast<void*>(
            mCurrentMulti * mReserveSize + oft ) ) );

        // Restore alignment.
        if ((mUnitSize & 4) != 4)
        {
            GLCheck(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
        }

        //TODO: Get rid of this nonsense of restoring the active texture.
        //RenderSystem is always restores to 0 after using,
        //plus activateGLTextureUnit won't see our changes otherwise.
        GLCheck(glActiveTexture(GL_TEXTURE0));
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLES2MappedBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    extern const GLuint64 kOneSecondInNanoSeconds;
    //-----------------------------------------------------------------------------------
    GLES2MappedBuffer::GLES2MappedBuffer(GBufferManager * mag, NCount oft, NCount size, BufferModeMark mode, GLuint vboName) :
        MappedBuffer(mag, mode, oft, size),
        mVboName(vboName),
        mMappedPtr(0),
        mFenceThreshold(size / 4),
        mUnfencedBytes(0)
    {
    }
    //-----------------------------------------------------------------------------------
    GLES2MappedBuffer::~GLES2MappedBuffer()
    {
        if(!mFences.empty())
            wait(mFences.back().fenceName);

        deleteFences(mFences.begin(), mFences.end());
    }
    //-----------------------------------------------------------------------------------
    void GLES2MappedBuffer::addFence(size_t from, size_t to, bool forceFence)
    {
        N_assert( to <= mSizeBytes );

        GLFence unfencedHazard(from, to);

        mUnfencedHazards.push_back(unfencedHazard);

        N_assert(from <= to);

        mUnfencedBytes += to - from;

        if(mUnfencedBytes >= mFenceThreshold || forceFence)
        {
            GLFenceVec::const_iterator itor = mUnfencedHazards.begin();
            GLFenceVec::const_iterator end  = mUnfencedHazards.end();

            size_t startRange = itor->start;
            size_t endRange   = itor->end;

            while( itor != end )
            {
                if( endRange <= itor->end )
                {
                    //Keep growing (merging) the fences into one fence
                    endRange = itor->end;
                }
                else
                {
                    //We wrapped back to 0. Can't keep merging. Make a fence.
                    GLFence fence( startRange, endRange );
                    GLCheck( fence.fenceName = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 ) );
                    mFences.push_back( fence );

                    startRange  = itor->start;
                    endRange    = itor->end;
                }

                ++itor;
            }

            //Make the last fence.
            GLFence fence(startRange, endRange);
            GLCheck(fence.fenceName = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));
            mFences.push_back(fence);

            mUnfencedHazards.clear();
            mUnfencedBytes = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    void GLES2MappedBuffer::wait(GLsync syncObj)
    {
        GLbitfield waitFlags    = 0;
        GLuint64 waitDuration   = 0;
        while( true )
        {
            GLenum waitRet = glClientWaitSync(syncObj, waitFlags, waitDuration);
            if(waitRet == GL_ALREADY_SIGNALED || waitRet == GL_CONDITION_SATISFIED)
            {
                return;
            }

            if(waitRet == GL_WAIT_FAILED)
            {
                N_EXCEPT( RenderingAPI,
                             "Failure while waiting for a GL GLFence. Could be out of GPU memory. "
                             "Update your video card drivers. If that doesn't help, contact the developers." );
                return;
            }

            // After the first time, need to start flushing, and wait for a looong time.
            waitFlags = GL_SYNC_FLUSH_COMMANDS_BIT;
            waitDuration = kOneSecondInNanoSeconds;
        }
    }
    //-----------------------------------------------------------------------------------
    void GLES2MappedBuffer::deleteFences(GLFenceVec::iterator itor, GLFenceVec::iterator end)
    {
        while(itor != end)
        {
            if(itor->fenceName)
                glDeleteSync(itor->fenceName);
            itor->fenceName = 0;
            ++itor;
        }
    }
    //-----------------------------------------------------------------------------------
    void GLES2MappedBuffer::waitIfNeeded(void)
    {
        //N_assert( mLastOp );

        size_t mappingStart = mMappingStart;
        size_t sizeBytes    = mMappingCount;

        if(mappingStart + sizeBytes > mSizeBytes)
        {
            if(!mUnfencedHazards.empty())
            {
                //mUnfencedHazards will be cleared in addFence
                addFence( mUnfencedHazards.front().start, mSizeBytes - 1, true );
            }

            //Wraps around the ring buffer. Sadly we can't do advanced virtual memory
            //manipulation to keep the virtual space contiguous, so we'll have to reset to 0
            mappingStart = 0;
        }

        GLFence regionToMap( mappingStart, mappingStart + sizeBytes );

        GLFenceVec::iterator itor = mFences.begin();
        GLFenceVec::iterator end  = mFences.end();

        GLFenceVec::iterator lastWaitableFence = end;

        while( itor != end )
        {
            if( regionToMap.overlaps( *itor ) )
                lastWaitableFence = itor;

            ++itor;
        }

        if( lastWaitableFence != end )
        {
            wait( lastWaitableFence->fenceName );
            deleteFences( mFences.begin(), lastWaitableFence + 1 );
            mFences.erase( mFences.begin(), lastWaitableFence + 1 );
        }

        mMappingStart = mappingStart;
    }
    //-----------------------------------------------------------------------------------
    void* GLES2MappedBuffer::writeMapImpl( NCount sizeBytes )
    {
        //N_assert( mLastOp );

        GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT |
                           GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;

        mMappingCount = sizeBytes;

        waitIfNeeded(); //Will fill mMappingStart

        GLCheck(glBindBuffer(GL_COPY_WRITE_BUFFER, mVboName));
        GLCheck(mMappedPtr = glMapBufferRange(GL_COPY_WRITE_BUFFER, mOffset + mMappingStart, mMappingCount, flags));

        return mMappedPtr;
    }
    //-----------------------------------------------------------------------------------
    void GLES2MappedBuffer::unmapImpl()
    {
        GLenum target = (mLastOp == BOT_Write) ? GL_COPY_WRITE_BUFFER : GL_COPY_READ_BUFFER;
        GLCheck(glBindBuffer(target, mVboName));

        if(mLastOp == BOT_Write)
        {
            GLCheck(glFlushMappedBufferRange(target,
                                            0 /*mOffset + mMappingStart*/,
                                            mMappingCount));
        }

        GLCheck(glUnmapBuffer(target));
        mMappedPtr = 0;
        
        if(mLastOp == BOT_Read)
        {
            recover(mMappingStart, mMappingCount);
        }
    }
    //-----------------------------------------------------------------------------------
    BlockType GLES2MappedBuffer::request( NCount sizeBytes, bool upload )
    {
        if(upload)
        {
            //N_assert( mLastOp );

            size_t mappingStart = mMappingStart;

            BlockType retVal = BT_None;

            if( mappingStart + sizeBytes > mSizeBytes )
            {
                if( !mUnfencedHazards.empty() )
                {
                    GLFence regionToMap( 0, sizeBytes );
                    GLFence hazardousRegion( mUnfencedHazards.front().start, mSizeBytes - 1 );

                    if( hazardousRegion.overlaps( regionToMap ) )
                    {
                        retVal = BT_All;
                        return retVal;
                    }
                }

                mappingStart = 0;
            }

            GLFence regionToMap( mappingStart, mappingStart + sizeBytes );

            GLFenceVec::const_iterator itor = mFences.begin();
            GLFenceVec::const_iterator end  = mFences.end();

            GLFenceVec::const_iterator lastWaitableFence = end;

            while( itor != end )
            {
                if( regionToMap.overlaps( *itor ) )
                    lastWaitableFence = itor;

                ++itor;
            }

            if( lastWaitableFence != end )
            {
                //Ask GL API to return immediately and tells us about the fence
                GLenum waitRet = glClientWaitSync( lastWaitableFence->fenceName, 0, 0 );
                if( waitRet != GL_ALREADY_SIGNALED && waitRet != GL_CONDITION_SATISFIED )
                    retVal = BT_Part;
            }

            return retVal;
        }
        else
        {
            return MappedBuffer::request(sizeBytes, upload);
        }
        return BT_All;
    }
    //-----------------------------------------------------------------------------------
    void GLES2MappedBuffer::cleanUnfencedHazards(void)
    {
        if( !mUnfencedHazards.empty() )
            addFence( mUnfencedHazards.front().start, mUnfencedHazards.back().end, true );
    }
    //-----------------------------------------------------------------------------------
    size_t GLES2MappedBuffer::async(OperationList & oplist, bool upload)
    {
        if(upload)
        {
            GLCheck(glBindBuffer(GL_COPY_READ_BUFFER, mVboName));

            OperationList::const_iterator i, iend = oplist.end();
            for( i = oplist.begin(); i != iend; ++i)
            {
                const Operation & dst = *i;
                const GpuBuffer * dest = dst.mDest;
                const GLES2BufferOp * op = static_cast<const GLES2BufferOp*>(dest->getOperation() );

                N_assert( dest->mMark == M_DEVNORMAL );

                GLintptr dstOffset = dst.mDestOffset + dest->getOffset() * dest->getUnitSize();

                GLCheck( glBindBuffer( GL_COPY_WRITE_BUFFER, op->getVboName() ) );
                GLCheck( glCopyBufferSubData( GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
                                            mOffset + mLastUploadOffset + dst.mOffset, dstOffset, dst.mSize ) );
                                            
                addFence( mLastUploadOffset + dst.mOffset, mLastUploadOffset + dst.mSize - 1, false );
            }
            mLastUploadOffset = mMappingStart;
            //Add fence to this region (or at least, track the hazard).
            //addFence( mMappingStart, mMappingStart + mMappingCount - 1, false );
        }
        else
        {
            GLCheck( glBindBuffer( GL_COPY_WRITE_BUFFER, mVboName ) );
            OperationList::iterator i, iend = oplist.end();
            for(i = oplist.begin(); i != iend; ++i)
            {
                Operation & src = *i;
                GpuBuffer * source = dst.mDest;
                
                size_t freeRegionOffset = requestImpl( src.mSize );

                if( freeRegionOffset == (size_t)(-1) )
                {
                    N_EXCEPT( Exception::ERR_INVALIDPARAMS,
                                 "Cannot download the request amount of " +
                                 StrConv::conv( src.mSize ) + " bytes to this staging buffer. "
                                 "Try another one (we're full of requests that haven't been read by CPU yet)" );
                }

                //N_assert( !mLastOp );
                N_assert(dynamic_cast<GLES2BufferOp*>( source->getOperation()));
                N_assert((src.mDestOffset + src.mSize) <= source->getSize());

                GLES2BufferOp * op = static_cast<GLES2BufferOp*>(source->getOperation());

                GLCheck(glBindBuffer(GL_COPY_READ_BUFFER, op->getVboName()));

                GLCheck(glCopyBufferSubData( GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
                                           source->getCurrentOffset() * source->getUnitSize() + src.mDestOffset,
                                           mOffset + freeRegionOffset, src.mSize));

                src.mOffset = freeRegionOffset;
            }
        }
        return oplist.size();
    }
    //-----------------------------------------------------------------------------------
    const void* GLES2MappedBuffer::readMapImpl(NCount offset, NCount sizeBytes)
    {
        //N_assert( !mLastOp );
        GLbitfield flags;

        //TODO: Reading + Persistency is supported, unsynchronized is not.
        flags = GL_MAP_READ_BIT;

        mMappingStart = offset;
        mMappingCount = sizeBytes;

        GLCheck(glBindBuffer(GL_COPY_READ_BUFFER, mVboName));
        GLCheck(mMappedPtr = glMapBufferRange(GL_COPY_READ_BUFFER, mOffset + mMappingStart, mMappingCount, flags));

        //Put the mapped region back to our records as "available" for subsequent async
        //recover( offset, sizeBytes );

        return mMappedPtr;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLES2OldVertexArrayObject
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    GLES2OldVertexArrayObject::GLES2OldVertexArrayObject() :
        mVAO(0),
        mInitialised(false)
    {
#if OGRE_NO_GLES2_VAO_SUPPORT == 0
#   if defined(GL_OES_vertex_array_object) || (OGRE_NO_GLES3_SUPPORT == 0)
        GLCheck(glGenVertexArraysOES(1, &mVAO));
//        LogManager::getSingleton().logMessage("Created VAO " + StrConv::conv(mVAO));

        if (!mVAO)
        {
            N_EXCEPT(Exception::ERR_INTERNAL_ERROR,
                    "Cannot create GL ES Vertex Array Object");
        }
#   endif
#endif
    }

    //-----------------------------------------------------------------------
    GLES2OldVertexArrayObject::~GLES2OldVertexArrayObject()
    {
#if OGRE_NO_GLES2_VAO_SUPPORT == 0
#   if defined(GL_OES_vertex_array_object) || (OGRE_NO_GLES3_SUPPORT == 0)
//        LogManager::getSingleton().logMessage("Deleting VAO " + StrConv::conv(mVAO));
        GLCheck(glDeleteVertexArraysOES(1, &mVAO));
#   endif
#endif
    }

    //-----------------------------------------------------------------------
    void GLES2OldVertexArrayObject::bind(void)
    {
#if OGRE_NO_GLES2_VAO_SUPPORT == 0
#   if defined(GL_OES_vertex_array_object) || (OGRE_NO_GLES3_SUPPORT == 0)
//        LogManager::getSingleton().logMessage("Binding VAO " + StrConv::conv(mVAO));
        GLCheck(glBindVertexArrayOES(mVAO));
#   endif
#endif
    }
    //-----------------------------------------------------------------------
}
