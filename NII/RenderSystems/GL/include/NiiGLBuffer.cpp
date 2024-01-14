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

#include "NiiGLGBufferManager.h"
#include "NiiGLBuffer.h"
#include "NiiException.h"
#include "NiiLogManager.h"
#include "NiiGLTexture.h"
#include "NiiTextureManager.h"
#include "NiiGLSupport.h"
#include "NiiGLPixelFormat.h"
#include "NiiGLFBORenderTexture.h"
#include "NiiGLRenderSystem.h"
#include "NiiStrConv.h"
#include "NiiBitwise.h"
#include "NiiDefaultStream.h"
#include "NiiEngine.h"

namespace NII
{
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GLBuffer::GLBuffer( GLuint vboid, GLuint size, GLGBufferManager * mag, BufferModeMark mode ) :
        mVboName( vboid ),
        mVboSize( size ),
        mMappedPtr( 0 ),
        mPersistentMethod( mode )
    {
        if(!N_Engine().getFeature()->isSupport(CF_PersistentMapping))
            mPersistentMethod = M_NORMAL;
    }
    //-----------------------------------------------------------------------------------
    GLBuffer::~GLBuffer()
    {
    }
    //-----------------------------------------------------------------------------------
    NCount GLBuffer::addMappedRange(NCount oft, NCount count)
    {
        NCount ticket;

        if(mFreeRanges.empty() )
        {
            ticket = mMappedRanges.size();
            mMappedRanges.push_back( MappedRange( oft, count ) );
        }
        else
        {
            ticket = mFreeRanges.back();
            mMappedRanges[ticket] = MappedRange( oft, count );
            mFreeRanges.pop_back();
        }

        return ticket;
    }
    //-----------------------------------------------------------------------------------
    void * GLBuffer::map( NCount start, NCount count, NCount & outTicket )
    {
        N_assert( start <= mVboSize && start + count <= mVboSize );

        if( mMappedRanges.size() == mFreeRanges.size() )
        {
            GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;

            if(N_MarkTrue(mPersistentMethod, M_PersistentMap))
            {
                flags |= GL_MAP_PERSISTENT_BIT;

                if(N_MarkTrue(mPersistentMethod, M_CoherentMap))
                    flags |= GL_MAP_COHERENT_BIT;
            }
            else
            {
                flags |= GL_MAP_UNSYNCHRONIZED_BIT;
            }

            GLCheck( mMappedPtr = glMapBufferRange( GL_COPY_WRITE_BUFFER, 0, mVboSize, flags ) );
        }

        outTicket = addMappedRange( start, count );

        return static_cast<Nui8*>(mMappedPtr) + start;
    }
    //-----------------------------------------------------------------------------------
    void GLBuffer::flush( NCount ticket, NCount oft, NCount count )
    {
        N_assert( oft <= mMappedRanges[ticket].count && oft + count <= mMappedRanges[ticket].count );

        GLCheck( glFlushMappedBufferRange( GL_COPY_WRITE_BUFFER, mMappedRanges[ticket].start + oft, count ) );
    }
    //-----------------------------------------------------------------------------------
    void GLBuffer::unmap( NCount ticket )
    {
        N_assert( ticket < mMappedRanges.size() && "Invalid unmap ticket!" );
        N_assert( mMappedRanges.size() != mFreeRanges.size() && "Unmapping an already unmapped buffer! Did you call unmap with the same ticket twice?" );

        mFreeRanges.push_back( ticket );

        if( mMappedRanges.size() == mFreeRanges.size() )
        {
            GLCheck( glUnmapBuffer( GL_COPY_WRITE_BUFFER ) );
            mMappedPtr = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLBufferOp
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GLBufferOp::GLBufferOp(NCount vboidx, GLuint vboid, GLBuffer * dbuf) :
        BufferOp(0),
        mGroupIndex( vboidx ),
        mVboName( vboid ),
        mMappedPtr( 0 ),
        mUnmapTicket( (NCount)~0 ),
        mGLBuffer( dbuf )
    {
    }
    //-----------------------------------------------------------------------------------
    GLBufferOp::~GLBufferOp()
    {
    }
    //-----------------------------------------------------------------------------------
    void GLBufferOp::initImpl(void * data, NCount oft, NCount size)
    {
        //In OpenGL; immutable buffers are a charade. They're mostly there to satisfy D3D11's needs.
        //However we emulate the behavior and trying to upload to an immutable buffer will result
        //in an exception or an N_assert, thus we temporarily change the type.
        BufferModeMark otype = mBuffer->mMark;
        if( N_MarkOnlyTrue(mBuffer->mMark, M_USAGEMARK, M_DEVREAD))
            mBuffer->mMark = M_DEVREAD | M_DEVWRITE;

        mBuffer->write(data, oft, size);

        mBuffer->mMark = otype;
    }
    //-----------------------------------------------------------------------------------
    void * GLBufferOp::lockImpl(MuteMark m, NCount oft, NCount size)
    {
        GLGBufferManager * mag = static_cast<GLGBufferManager*>( mBuffer->mMag );
        bool persistent = N_Engine().getFeature()->isSupport(CF_PersistentMapping);

        mag->wait();

        //size_t dFrame = advanceFrame(bAdvanceFrame);

        if(!mBuffer->mMapping || !persistent)
        {
            NCount offset, length;

            if(N_MarkTrue(mBuffer->mMark, M_PersistentMap) && N_MarkTrue(m, MM_Persistent) && persistent)
            {
                //Persistent buffers map the *whole* assigned buffer,
                offset = mBuffer->mOffset;
                length = mBuffer->getReserveSize() * mBuffer->getMutilCount();
            }
            else
            {
                //Non-persistent buffers just map the small region they'll need.
                offset = mBuffer->mOffset + oft /*+ mBuffer->getReserveSize() * multiIdx*/;
                length = size;
            }

            GLCheck(glBindBuffer(GL_COPY_WRITE_BUFFER, mVboName));
            mMappedPtr = mGLBuffer->map(offset, length, mUnmapTicket);
            mBuffer->mMapping = true;
        }

        //For regular maps, mMuteOft is 0. So that we can later flush correctly.

        mBuffer->mMuteSize = size;

        char * re = (char *)mMappedPtr;

        if(N_MarkTrue(mBuffer->mMark, M_PersistentMap) && N_MarkTrue(m, MM_Persistent) && persistent)
        {
            //For persistent maps, we've mapped the whole 3x size of the buffer. mMuteOft
            //points to the right offset so that we can later flush correctly.
            mBuffer->mMuteOft = oft /*+ mBuffer->getReserveSize() * multiIdx*/;
            re += mBuffer->mMuteOft;
        }
        else
            mBuffer->mMuteOft = 0;

        return re;
    }
    //-----------------------------------------------------------------------------------
    void GLBufferOp::unlockImpl(UnlockType type, NCount oft, NCount size)
    {
        N_assert( oft <= mBuffer->mMuteSize && "Flush starts after the end of the mapped region!" );
        N_assert( oft + size <= mBuffer->mMuteSize && "Flush region out of bounds!" );

        bool persistent = N_Engine().getFeature()->isSupport(CF_PersistentMapping);

        if(N_MarkFalse(mBuffer->mMark, M_CoherentMap) || type == UT_UnlockMap || !persistent)
        {
            if(!size)
                size = mBuffer->mMuteSize - oft;

            GLCheck(glBindBuffer( GL_COPY_WRITE_BUFFER, mVboName));
            mGLBuffer->flush(mUnmapTicket, mBuffer->mMuteOft + oft, size);

            if(type == UT_UnlockMap || !persistent || N_MarkOnlyTrue(mBuffer->mMark, M_USAGEMARK, M_NORMAL))
            {
                mGLBuffer->unmap( mUnmapTicket );
                mMappedPtr = 0;
                mBuffer->mMapping = false;
            }
        }
    }
    //-----------------------------------------------------------------------------------
    /*size_t GLBufferOp::advanceFrame(bool bAdvanceFrame)
    {
        //GLGBufferManager * mag = static_cast<GLGBufferManager *>(mBuffer->mMag);
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
    void GLBufferOp::regressFrame(void)
    {
        //GLGBufferManager * mag = static_cast<GLGBufferManager *>(mBuffer->mMag);
        size_t dFrame = mBuffer->mCurrentOffset - mBuffer->mOffset;
        dFrame /= mBuffer->getReserveSize();

        dFrame = (dFrame + mag->getMutilCount() - 1) % mag->getMutilCount();

        mBuffer->mCurrentOffset = mBuffer->mOffset + dFrame * mBuffer->getReserveSize();
    }*/
    //-----------------------------------------------------------------------------------
    bool GLBufferOp::reserveImpl(NCount size, BufferModeMark newMode, bool oldData)
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
    void GLBufferOp::memcpyImpl(void * data, NCount oft, NCount size, bool write)
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
    void GLBufferOp::memcpyImpl(Buffer * src, NCount srcOft, NCount oft, NCount size )
    {
        N_assert( dynamic_cast<GLBufferOp *>( src->mOp ) );
        GLBufferOp * srcbuf = static_cast<GLBufferOp*>( src->mOp );

        GLCheck( glBindBuffer( GL_COPY_WRITE_BUFFER, mVboName ) );
        GLCheck( glBindBuffer( GL_COPY_READ_BUFFER, srcbuf->getVboName() ) );

        GLCheck( glCopyBufferSubData( GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, srcOft, oft, size ) );
    }
    //-----------------------------------------------------------------------------------
    void GLBufferOp::waitImpl()
    {
        
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLIndexBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GLIndexBuffer::GLIndexBuffer(BufferManager * mag, NCount size, NCount count, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) :
        IndexBuffer(mag, size, count, multi, mode, initData, gid, goft, padSize),
    {
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLVertexBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GLVertexBuffer::GLVertexBuffer(BufferManager * mag, NCount size, NCount count, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) :
        VertexBuffer(mag, size, count, multi, mode, initData, gid, goft, padSize)
    {
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLCounterBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GLCounterBuffer::GLCounterBuffer(BufferManager * mag, NCount size, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) :
        CounterBuffer(mag, size, multi, mode, initData, gid, goft, padSize)
    {
    }
    //-----------------------------------------------------------------------------------
    void GLCounterBuffer::bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft, NCount size)
    {
        N_assert(dynamic_cast<GLBufferOp *>(mOp));

        GLBufferOp * op = static_cast<GLBufferOp *>(mOp);
        mBinding = slot;
        GLCheck(glBindBufferRange(GL_ATOMIC_COUNTER_BUFFER, slot, op->getVboName(), mCurrentMulti * mReserveSize + oft, size));
    }
    //-----------------------------------------------------------------------------------
    void GLCounterBuffer::setGLBufferBinding(GLint slot)
    {
        N_assert(dynamic_cast<GLBufferOp *>(mOp));

        GLBufferOp * op = static_cast<GLBufferOp *>(mOp);
        mBinding = slot;
        GLCheck(glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, mBinding, op->getVboName()));
    }

    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLStructBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GLStructBuffer::GLStructBuffer(BufferManager * mag, NCount size, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) :
        StructBuffer(mag, size, multi, mode, initData, gid, goft, padSize)
    {
    }
    //-----------------------------------------------------------------------------------
    void GLStructBuffer::bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft, NCount size)
    {
        N_assert(dynamic_cast<GLBufferOp *>(mOp));

        GLBufferOp * op = static_cast<GLBufferOp *>(mOp);
        mBinding = slot;
        GLCheck(glBindBufferRange(GL_UNIFORM_BUFFER, slot, op->getVboName(), mCurrentMulti * mReserveSize + oft, size));
    }
    //-----------------------------------------------------------------------------------
    void GLStructBuffer::setGLBufferBinding(GLint slot)
    {
        N_assert(dynamic_cast<GLBufferOp *>(mOp));

        GLBufferOp * op = static_cast<GLBufferOp *>(mOp);
        mBinding = slot;
        GLCheck(glBindBufferBase(GL_UNIFORM_BUFFER, mBinding, op->getVboName()));
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLTextureBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GLTextureBuffer::GLTextureBuffer(BufferManager * mag, NCount size, PixelFormat pf, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) :
        TextureBuffer( mag, size, pf, multi, mode, initData, gid, goft, padSize)
    {
        GLCheck( glGenTextures( 1, &mTexName ) );
        GLCheck( glBindTexture( GL_TEXTURE_BUFFER, mTexName ) );

        mInternalFormat = GL3PlusMappings::get( pf );
    }
    //-----------------------------------------------------------------------------------
    GLTextureBuffer::~GLTextureBuffer()
    {
        GLCheck( glDeleteTextures( 1, &mTexName ) );
    }
    //-----------------------------------------------------------------------------------
    void GLTextureBuffer::bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft, NCount size)
    {
        N_assert( dynamic_cast<GLBufferOp *>( mOp ) );
        N_assert( oft < (mUnitCount * mUnitSize - 1) );
        N_assert( (oft + size) <= mUnitCount * mUnitSize );

        size = !size ? (mUnitCount * mUnitSize - oft) : size;

        GLBufferOp * op = static_cast<GLBufferOp *>(mOp);

        GLCheck( glActiveTexture( GL_TEXTURE0 + slot ) );
        GLCheck( glBindTexture( GL_TEXTURE_BUFFER, mTexName ) );
        GLCheck( glTexBufferRange( GL_TEXTURE_BUFFER, mInternalFormat, op->getVboName(), mCurrentMulti * mReserveSize + oft, size ) );

        //TODO: Get rid of this nonsense of restoring the active texture.
        //RenderSystem is always restores to 0 after using,
        //plus activateGLTextureUnit won't see our changes otherwise.
        GLCheck( glActiveTexture( GL_TEXTURE0 ) );
    }
    //-----------------------------------------------------------------------------------
    void GLTextureBuffer::_bind(NCount offset, NCount size)
    {
        N_assert(dynamic_cast<GLBufferOp *>(mOp));
        N_assert(offset < (mUnitCount * mUnitSize - 1));
        N_assert((offset + size) <= mUnitCount * mUnitSize);

        size = !size ? (mUnitCount * mUnitSize - offset) : size;

        GLBufferOp * op = static_cast<GLBufferOp *>(mOp);
        
        GLCheck(glBindTexture(GL_TEXTURE_BUFFER, mTexName));
        GLCheck(glTexBufferRange(GL_TEXTURE_BUFFER, mInternalFormat, op->getVboName(), mCurrentMulti * mReserveSize + offset, size));
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLStorageBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GLStorageBuffer::GLStorageBuffer(BufferManager * mag, NCount usize, NCount ucount, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) :
        StorageBuffer(mag, usize, ucount, multi, mode, initData, gid, goft, padSize)
    {
    }
    //-----------------------------------------------------------------------------------
    GLStorageBuffer::~GLStorageBuffer()
    {
    }
    //-----------------------------------------------------------------------------------
    TextureBuffer * GLStorageBuffer::createViewImpl( PixelFormat pf )
    {
        N_assert( dynamic_cast<GLBufferOp *>( mOp ) );

        TextureBuffer * re = N_new GLTextureBuffer(mMag, mUnitCount * mUnitSize, mMark, pf, 0, mGroupID, mOffset);

        re->setOperation( mOp );

        mViewList.push_back( re );

        return re;
    }
    //-----------------------------------------------------------------------------------
    void GLStorageBuffer::bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft, NCount size)
    {
        N_assert( dynamic_cast<GLBufferOp *>( mOp ) );
        N_assert( oft < (mUnitCount * mUnitSize - 1) );
        N_assert( size < mUnitCount * mUnitSize );

        size = !size ? (mUnitCount * mUnitSize - oft) : size;

        GLBufferOp * op = static_cast<GLBufferOp*>(mOp );
        mBinding = slot;
        GLCheck(glBindBufferRange( GL_SHADER_STORAGE_BUFFER, slot, op->getVboName(), mCurrentMulti * mReserveSize + oft, size ) );
    }
    //-----------------------------------------------------------------------------------
    void GLStorageBuffer::bindBufferCS( Nui16 slot, NCount offset, NCount size )
    {
        bindProgram(GpuProgram::ST_CS, slot, offset, size );
    }
    //-----------------------------------------------------------------------------------
    void GLStorageBuffer::setGLBufferBinding(GLint slot)
    {
        N_assert( dynamic_cast<GLBufferOp *>( mOp ) );
        
        GLBufferOp * op = static_cast<GLBufferOp*>(mOp );
        mBinding = slot;
        GLCheck(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, mBinding, op->getVboName()));
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLTextureBufferEmu
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GLTextureBufferEmu::GLTextureBufferEmu(BufferManager * mag, NCount size, PixelFormat pf, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) :
        TextureBuffer( mag, size, pf, multi, mode, initData, gid, goft, padSize),
        mTexName( 0 ),
        mOriginFormat( 0 ),
        mOriginDataType( 0 )
        //mInternalNumElemBytes( 0 ),
        //mInternalNumElements( 0 )
    {
        GLCheck( glGenTextures( 1, &mTexName ) );
        
        mInternalFormat = GL3PlusMappings::get( pf );

        GLCheck( glBindTexture( GL_TEXTURE_2D, mTexName ) );

        mMaxTexSize = 2048;

        GL3PlusMappings::getFormatAndType( pf, mOriginFormat, mOriginDataType );

        mInternalNumElemBytes = PixelUtil::getUnitSize( pf );

        mInternalNumElements = numElements / mInternalNumElemBytes;

        size_t width = std::min( mMaxTexSize,  mInternalNumElements);

        size_t texHeight = (mInternalNumElements + mMaxTexSize - 1) / mMaxTexSize;

        GLCheck( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 ) );
        GLCheck( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 ) );
        GLCheck( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ) );
        GLCheck( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ) );
        GLCheck( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ) );
        GLCheck( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ) );
        GLCheck( glTexImage2D( GL_TEXTURE_2D, 0, mInternalFormat, width, texHeight, 0, mOriginFormat, mOriginDataType, NULL ) );
    }
    //-----------------------------------------------------------------------------------
    GLTextureBufferEmu::~GLTextureBufferEmu()
    {
        GLCheck( glDeleteTextures( 1, &mTexName ) );
    }
    //-----------------------------------------------------------------------------------
    void GLTextureBufferEmu::bindBuffer( NCount offset, NCount sizeBytes )
    {
        N_assert( dynamic_cast<GLBufferOp*>( mOp ) );
        N_assert( offset < (mUnitCount * mUnitSize - 1) );
        N_assert( (offset + sizeBytes) <= mUnitCount * mUnitSize );

        sizeBytes = !sizeBytes ? (mUnitCount * mUnitSize - offset) : sizeBytes;

        GLBufferOp *op = static_cast<GLBufferOp*>( mOp );

        size_t numModifiedElements = sizeBytes / mInternalNumElemBytes;
        N_assert( sizeBytes % mInternalNumElemBytes == 0 );
        size_t texWidth = std::min( numModifiedElements, std::min( mMaxTexSize, mInternalNumElements ) );
        size_t texHeight = ( numModifiedElements + mMaxTexSize - 1 ) / mMaxTexSize;

        if( (mUnitSize & 4) != 4 )
        {
            // Standard alignment of 4 is not right for some formats.
            GLCheck( glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ) );
        }

        GLCheck( glBindBuffer( GL_PIXEL_UNPACK_BUFFER, op->getVboName() ) );
        GLCheck( glBindTexture( GL_TEXTURE_2D, mTexName ) );
        GLCheck( glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, texWidth, texHeight, mOriginFormat, mOriginDataType, 
                               reinterpret_cast<void*>( mCurrentMulti * mReserveSize + offset ) ) );

        // Restore alignment.
        if ((mUnitSize & 4) != 4)
        {
            GLCheck( glPixelStorei( GL_UNPACK_ALIGNMENT, 4 ) );
        }
    }
    //-----------------------------------------------------------------------------------
    void GLTextureBufferEmu::bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft, NCount size)
    {
        GLCheck( glActiveTexture( GL_TEXTURE0 + slot ) );
        bindBuffer( oft, size );
        //TODO: Get rid of this nonsense of restoring the active texture.
        //RenderSystem is always restores to 0 after using,
        //plus activateGLTextureUnit won't see our changes otherwise.
        GLCheck( glActiveTexture( GL_TEXTURE0 ) );
    }
    //-----------------------------------------------------------------------------------
    void GLTextureBufferEmu::_bind( NCount offset, NCount size )
    {
        bindBuffer( offset, size );
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLMappedBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    extern const GLuint64 kOneSecondInNanoSeconds;
    //-----------------------------------------------------------------------------------
    GLMappedBuffer::GLMappedBuffer(GBufferManager *mag, NCount oft, NCount size, BufferModeMark mode, GLuint vboName ) :
        MappedBuffer(mag, mode, oft, size),
        mVboName(vboName),
        mMappedPtr(0),
        mFenceThreshold(size / 4),
        mUnfencedBytes(0)
    {
    }
    //-----------------------------------------------------------------------------------
    GLMappedBuffer::~GLMappedBuffer()
    {
        if( !mFences.empty() )
            wait( mFences.back().fenceName );

        deleteFences( mFences.begin(), mFences.end() );
    }
    //-----------------------------------------------------------------------------------
    void GLMappedBuffer::addFence( NCount from, NCount to, bool forceFence )
    {
        N_assert( to <= mSizeBytes );
        N_assert( from <= to );
                
        mUnfencedHazards.emplace_back( from, to );

        mUnfencedBytes += to - from;

        if( mUnfencedBytes >= mFenceThreshold || forceFence )
        {
            GLFenceVec::const_iterator itor = mUnfencedHazards.begin();
            GLFenceVec::const_iterator end  = mUnfencedHazards.end();

            NCount startRange = itor->start;
            NCount endRange   = itor->end;

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
            GLFence fence( startRange, endRange );
            GLCheck( fence.fenceName = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 ) );
            mFences.push_back( fence );

            mUnfencedHazards.clear();
            mUnfencedBytes = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    void GLMappedBuffer::wait( GLsync syncObj )
    {
        GLbitfield waitFlags    = 0;
        GLuint64 waitDuration   = 0;
        while( true )
        {
            GLenum waitRet = glClientWaitSync( syncObj, waitFlags, waitDuration );
            if( waitRet == GL_ALREADY_SIGNALED || waitRet == GL_CONDITION_SATISFIED )
            {
                return;
            }

            if( waitRet == GL_WAIT_FAILED )
            {
                N_EXCEPT( RenderingAPI,
                             "Failure while waiting for a GL GLFence. Could be out of GPU memory. "
                             "Update your video card drivers. If that doesn't help, contact the developers.");
                return;
            }

            // After the first time, need to start flushing, and wait for a looong time.
            waitFlags = GL_SYNC_FLUSH_COMMANDS_BIT;
            waitDuration = kOneSecondInNanoSeconds;
        }
    }
    //-----------------------------------------------------------------------------------
    void GLMappedBuffer::deleteFences( GLFenceVec::iterator itor, GLFenceVec::iterator end )
    {
        while( itor != end )
        {
            if( itor->fenceName )
                glDeleteSync( itor->fenceName );
            itor->fenceName = 0;
            ++itor;
        }
    }
    //-----------------------------------------------------------------------------------
    void GLMappedBuffer::waitIfNeeded(void)
    {
        //N_assert( mLastOp );

        NCount mappingStart = mMappingStart;
        NCount sizeBytes    = mMappingCount;

        if( mappingStart + sizeBytes > mSizeBytes )
        {
            if( !mUnfencedHazards.empty() )
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
    void * GLMappedBuffer::writeMapImpl( NCount sizeBytes )
    {
        GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;

        mMappingCount = sizeBytes;

        waitIfNeeded(); //Will fill mMappingStart

        GLCheck( glBindBuffer( GL_COPY_WRITE_BUFFER, mVboName ) );
        GLCheck( mMappedPtr = glMapBufferRange( GL_COPY_WRITE_BUFFER, mOffset + mMappingStart, mMappingCount, flags ) );

        return mMappedPtr;
    }
    //-----------------------------------------------------------------------------------
    void GLMappedBuffer::unmapImpl()
    {
        GLenum target = mLastOp ? GL_COPY_WRITE_BUFFER : GL_COPY_READ_BUFFER;
        GLCheck( glBindBuffer( target, mVboName ) );

        if( mLastOp )
        {
            GLCheck( glFlushMappedBufferRange( target,
                                            0 /*mOffset + mMappingStart*/,
                                            mMappingCount ) );
        }

        GLCheck( glUnmapBuffer( target ) );
        mMappedPtr = 0;

        if(!mLastOp)
        {
            recover(mMappingStart, mMappingCount);
        }
    }
    //-----------------------------------------------------------------------------------
    BlockType GLMappedBuffer::request( NCount sizeBytes, bool upload)
    {
        if(upload)
        {
            //N_assert( mLastOp );

            NCount mappingStart = mMappingStart;

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

        return MappedBuffer::request(sizeBytes, upload);
    }
    //-----------------------------------------------------------------------------------
    void GLMappedBuffer::cleanUnfencedHazards(void)
    {
        if( !mUnfencedHazards.empty() )
            addFence( mUnfencedHazards.front().start, mUnfencedHazards.back().end, true );
    }
    //-----------------------------------------------------------------------------------
    NCount GLMappedBuffer::async(OperationList & oplist, bool upload)
    {
        if(upload)
        {
            GLCheck( glBindBuffer( GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, mVboName ) );
            OperationList::const_iterator i, iend = oplist.end();
            for( i = oplist.begin(); i != iend; ++i)
            {
                const Operation & dst = *i;
                const GpuBuffer * dest = src.mDest;
                const GLBufferOp * op = static_cast<const GLBufferOp*>(dest->getOperation() );

                N_assert( dest->mMark == M_DEVNORMAL );

                GLintptr dstOffset = dst.mDestOffset + dest->getOffset() * dest->getUnitSize();

                GLCheck( glBindBuffer( GL_COPY_WRITE_BUFFER, op->getVboName() ) );
                GLCheck( glCopyBufferSubData( GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, mOffset + mLastUploadOffset + dst.mOffset, dstOffset, dst.mSize ) );
                
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
                GpuBuffer * source = src.mDest;
                size_t freeRegionOffset = requestImpl( src.mSize );

                if( freeRegionOffset == (size_t)(-1) )
                {
                    N_EXCEPT( Exception::ERR_INVALIDPARAMS,
                                 "Cannot download the request amount of " +
                                 StrConv::conv( srcLength ) + " bytes to this staging buffer. "
                                 "Try another one (we're full of requests that haven't been read by CPU yet)" );
                }

                //N_assert( !mLastOp );
                N_assert( dynamic_cast<GLBufferOp *>( source->getOperation() ) );
                N_assert( (srcOffset + srcLength) <= source->getSize() );

                GLBufferOp * op = static_cast<GLBufferOp *>( source->getOperation() );

                GLCheck( glBindBuffer( GL_COPY_READ_BUFFER, op->getVboName() ) );

                GLCheck( glCopyBufferSubData( GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
                                           source->getCurrentOffset() * source->getUnitSize() + src.mDestOffset,
                                           mOffset + freeRegionOffset, srcLength ) );

                src.mOffset = freeRegionOffset;
            }
        }
        return oplist.size();
    }
    //-----------------------------------------------------------------------------------
    const void * GLMappedBuffer::readMapImpl( NCount offset, NCount sizeBytes )
    {
        //N_assert( !mLastOp );
        GLbitfield flags;

        //TODO: Reading + Persistency is supported, unsynchronized is not.
        flags = GL_MAP_READ_BIT;

        mMappingStart = offset;
        mMappingCount = sizeBytes;

        GLCheck( glBindBuffer( GL_COPY_READ_BUFFER, mVboName ) );
        GLCheck( mMappedPtr = glMapBufferRange( GL_COPY_READ_BUFFER, mOffset + mMappingStart, mMappingCount, flags ) );

        //Put the mapped region back to our records as "available" for subsequent async
        //recover( offset, sizeBytes );

        return mMappedPtr;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLMappedTexture
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GLMappedTexture::GLMappedTexture( GLGBufferManager * mag, PixelFormat pf, NCount size, NCount oft, NCount vboPoolIdx, GLBuffer * buffer ) :
        MappedTexture( mag, pf, size, oft, vboPoolIdx ),
        mBuffer( buffer ),
        mLastMappedPtr( 0 )
    {
        const bool canPersistentMap = N_Engine().getFeature()->isSupport(CF_PersistentMapping);

        if( canPersistentMap )
        {
            GLCheck( glBindBuffer( GL_COPY_WRITE_BUFFER, mBuffer->getVboName() ) );
            mMappedPtr = mBuffer->map( mOffset, mSize, mUnmapTicket );
            mLastMappedPtr = mMappedPtr;
        }
        else
        {
            mMappedPtr = 0;
            mUnmapTicket = std::numeric_limits<size_t>::max();
        }
    }
    //-----------------------------------------------------------------------------------
    GLMappedTexture::~GLMappedTexture()
    {
        N_assert( mUnmapTicket == std::numeric_limits<size_t>::max() );
    }
    //-----------------------------------------------------------------------------------
    void GLMappedTexture::_unmapBuffer(void)
    {
        if( mUnmapTicket != std::numeric_limits<size_t>::max() )
        {
            GLCheck( glBindBuffer( GL_COPY_WRITE_BUFFER, mBuffer->getVboName() ) );
            mBuffer->unmap( mUnmapTicket );
            mUnmapTicket = std::numeric_limits<size_t>::max();
            mMappedPtr = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    bool GLMappedTexture::belongsToUs(const PixelBlock & box)
    {
        return box.data >= mLastMappedPtr && box.data <= static_cast<uint8 *>( mLastMappedPtr ) + mCurrentOffset;
    }
    //-----------------------------------------------------------------------------------
    void GLMappedTexture::unmap(void)
    {
        const bool canPersistentMap = N_Engine().getFeature()->isSupport(CF_PersistentMapping);

        GLCheck( glBindBuffer( GL_COPY_WRITE_BUFFER, mBuffer->getVboName() ) );
        mBuffer->flush( mUnmapTicket, 0, mCurrentOffset );

        if( !canPersistentMap )
        {
            mBuffer->unmap( mUnmapTicket );
            mUnmapTicket = std::numeric_limits<size_t>::max();
            mMappedPtr = 0; //Do not zero it so belongsToUs can work.
        }

        MappedTexture::unmap();
        mCurrentOffset = 0;
    }
    //-----------------------------------------------------------------------------------
    void GLMappedTexture::uploadCubemap(const PixelBlock &srcBox, PixelFormat pf, uint8 mipLevel, GLenum format, GLenum type,
        GLint xPos, GLint yPos, GLint slicePos, GLsizei width, GLsizei height, GLsizei mArray )
    {
        const size_t distToStart = static_cast<uint8*>( srcBox.data ) - static_cast<uint8*>( mMappedPtr );
        uint8 *offsetPtr = reinterpret_cast<uint8*>( mOffset + distToStart );

        const GLsizei sizeBytes = static_cast<GLsizei>(PixelUtil::getSize( srcBox.width, srcBox.height, 1u, 1u, pf ) );

        for( size_t i=0; i<(size_t)mArray; ++i )
        {
            const GLenum targetGl = static_cast<GLenum>( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i + slicePos );
            if( type != GL_NONE )
            {
                GLCheck( glTexSubImage2D( targetGl, mipLevel, xPos, yPos, width, height, format, type, offsetPtr ) );
            }
            else
            {
                GLCheck( glCompressedTexSubImage2D( targetGl, mipLevel, xPos, yPos, width, height, format, sizeBytes, offsetPtr ) );
            }
            offsetPtr += srcBox.mSliceSize;
        }
    }
    //-----------------------------------------------------------------------------------
    void GLMappedTexture::read( Texture * dstTexture, NCount mipLevel, const PixelBlock * dstBox, const PixelBlock &srcBox, bool host )
    {
        if(host)
        {
            MappedTexture::read( dstTexture, mipLevel, dstBox, srcBox, true );
            return;
        }

        size_t mPixelSize = PixelUtil::getUnitSize( dstTexture->getFormat() );

        N_assert( !dstTexture->isMultiSample() && "Cannot upload to an MSAA texture!" );

        const GLint rowLength   = mPixelSize > 0 ? (srcBox.mRowSize / mPixelSize) : 0;
        const GLint imageHeight = (srcBox.mRowSize > 0) ? (srcBox.mSliceSize / srcBox.mRowSize) : 0;

        GLCheck( glPixelStorei( GL_UNPACK_ALIGNMENT, 4 ) );
        GLCheck( glPixelStorei( GL_UNPACK_ROW_LENGTH, rowLength) );
        GLCheck( glPixelStorei( GL_UNPACK_IMAGE_HEIGHT, imageHeight ) );

        const Texture::Type::Texture::Type textureType = dstTexture->getActualType();
        const PixelFormat pf = dstTexture->getFormat();

        N_assert( dynamic_cast<GLTexture*>( dstTexture ) );
        GLTexture *dstTextureGl = static_cast<GLTexture*>( dstTexture );

        const GLenum targetGl   = dstTextureGl->getGlTextureTarget();
        const GLuint texName    = dstTextureGl->getFinalTextureName();

        GLCheck( glBindTexture( targetGl, texName ) );

        GLCheck( glBindBuffer( GL_PIXEL_UNPACK_BUFFER, mBuffer->getVboName() ) );

        GLint xPos      = static_cast<GLint>( dstBox ? dstBox->x : 0 );
        GLint yPos      = static_cast<GLint>( dstBox ? dstBox->y : 0 );
        GLint zPos      = static_cast<GLint>( dstBox ? dstBox->z : 0 );
        GLint slicePos  = static_cast<GLint>( (dstBox ? dstBox->mArrayIdx : 0) + dstTexture->getPoolArray());
        //Dst & src must have the same resolution & number of slices, so just pick src's dimensions.
        GLsizei width   = static_cast<GLsizei>( srcBox.width );
        GLsizei height  = static_cast<GLsizei>( srcBox.height  );
        GLsizei depth   = static_cast<GLsizei>( srcBox.depth );
        GLsizei mArray=static_cast<GLsizei>( srcBox.mArray );

        const size_t distToStart = static_cast<uint8*>( srcBox.data ) - static_cast<uint8*>( mMappedPtr );
        const void *offsetPtr = reinterpret_cast<void*>( mOffset + distToStart );

        if( !PixelUtil::isCompress( pf ) )
        {
            GLenum format, type;
            GL3PlusMappings::getFormatAndType( pf, format, type );

            switch( textureType )
            {
            case Texture::T_Unknow:
                N_EXCEPT(Internal, "Ogre should never hit this path" );
                break;
            case Texture::T_1D:
                GLCheck( glTexSubImage1D( targetGl, mipLevel, xPos, width, format, type, offsetPtr ) );
                break;
            case Texture::T_1DArray:
                GLCheck( glTexSubImage2D( targetGl, mipLevel, xPos, slicePos, width, mArray, format, type, offsetPtr ) );
                break;
            case Texture::T_2D:
                GLCheck( glTexSubImage2D( targetGl, mipLevel, xPos, yPos, width, height, format, type, offsetPtr ) );
                break;
            case Texture::T_CUBE:
                uploadCubemap( srcBox, pf, mipLevel, format, type, xPos, yPos, slicePos, width, height, mArray );
                break;
            case Texture::T_2DArray:
            case Texture::Type::T_CUBEArray:
                GLCheck( glTexSubImage3D( targetGl, mipLevel, xPos, yPos, slicePos, width, height, mArray, format, type, offsetPtr ) );
                break;
            case Texture::T_3D:
                GLCheck( glTexSubImage3D( targetGl, mipLevel, xPos, yPos, zPos, width, height, depth, format, type, offsetPtr ) );
                break;
            }
        }
        else
        {
            GLenum format = GL3PlusMappings::get( pf );

            const GLsizei sizeBytes = static_cast<GLsizei>(
                    PixelUtil::getSize( srcBox.width, srcBox.height, srcBox.depth, srcBox.mArray, pf ) );

            switch( textureType )
            {
            case Texture::T_Unknow:
                N_EXCEPT(Internal, "Ogre should never hit this path" );
                break;
            case Texture::T_1D:
                GLCheck( glCompressedTexSubImage1D( targetGl, mipLevel, xPos, width, format, sizeBytes, offsetPtr ) );
                break;
            case Texture::T_1DArray:
                GLCheck( glCompressedTexSubImage2D( targetGl, mipLevel, xPos, slicePos, width, mArray, format, sizeBytes, offsetPtr ) );
                break;
            case Texture::T_2D:
                GLCheck( glCompressedTexSubImage2D( targetGl, mipLevel, xPos, yPos, width, height, format, sizeBytes, offsetPtr ) );
                break;
            case Texture::T_CUBE:
                uploadCubemap( srcBox, pf, mipLevel, format, GL_NONE, xPos, yPos, slicePos, width, height, mArray );
                break;
            case Texture::T_2DArray:
            case Texture::Type::T_CUBEArray:
                GLCheck( glCompressedTexSubImage3D( targetGl, mipLevel, xPos, yPos, slicePos,
                                                 width, height, mArray, format, sizeBytes, offsetPtr ) );
                break;
            case Texture::T_3D:
                GLCheck( glCompressedTexSubImage3D( targetGl, mipLevel, xPos, yPos, zPos,
                                                 width, height, depth, format, sizeBytes, offsetPtr ) );
                break;
            }
        }

        GLCheck( glBindTexture( targetGl, 0 ) );
        GLCheck( glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 ) );
    }
    //-----------------------------------------------------------------------------------
    void GLMappedTexture::write(Texture * src, NCount srcmipmap, const PixelBlock * srcBox, const PixelBlock & box, bool host)
    {
        
    }
    //-----------------------------------------------------------------------------------
    PixelBlock GLMappedTexture::mapImpl(const PixelVolume & vol)
    {
        mMapping = true;

#if OGRE_DEBUG_MODE
        //We only warn if isBlock wasn't called. Because if you didn't wait
        //getDynamicBufferMultiplier frames and never called isBlock to check,
        //you're risking certain machines (with slower GPUs) to stall even if yours doesn't.
        if( mMag->getFrameCount() - mLastFrame < mMag->getDynamicBufferMultiplier() /*&&
            !mUserQueriedIfUploadWillStall*/ )
        {
            N_log(
                "PERFORMANCE WARNING: Calling MappedTexture::startMapRegion too soon, "
                "it will very likely stall depending on HW and OS setup. You need to wait"
                " GLGBufferManager::getDynamicBufferMultiplier frames after having called "
                "MappedTexture::upload before calling startMapRegion again.",
                LML_CRITICAL );
        }
#endif

        mMag->wait( mLastFrame );

        const bool canPersistentMap = N_Engine().getFeature()->isSupport(CF_PersistentMapping);

        if( !canPersistentMap )
        {
            GLCheck( glBindBuffer( GL_COPY_WRITE_BUFFER, mBuffer->getVboName() ) );
            mMappedPtr = mBuffer->map( mOffset, mSize, mUnmapTicket );
            mLastMappedPtr = mMappedPtr;
        }
        
        
        #define rowAlignment 4u
        const size_t sizeBytes = PixelUtil::getSize( vol.mWidth, vol.mHeight, vol.mDepth, vol.mArray, vol.mFormat );

        if( PixelUtil::isCompress( vol.mFormat ) )
        {
            //Ensure the offset is always aligned to the block size for compressed formats
            const size_t blockSizeBytes = PixelUtil::getUnitSize( pf );
            mCurrentOffset = multiCeil( mCurrentOffset, blockSizeBytes );
            mCurrentOffset = std::min( mCurrentOffset, mSize );
        }

        PixelBlock re;

        const size_t availableSize = mSize - mCurrentOffset;

        if( sizeBytes <= availableSize )
        {
            re.mWidth = vol.mWidth;
            re.mHeight = vol.mHeight;
            re.mDepth = vol.mDepth;
            re.mArray = vol.mArray;
            re.mPixelSize = PixelUtil::getUnitSize( vol.mFormat );
            re.mRowSize = PixelUtil::getSize( vol.mWidth, 1, 1, 1, vol.mFormat );
            re.mSliceSize = PixelUtil::getSize( vol.mWidth, vol.mHeight, 1, 1, vol.mFormat );
            re.mData = mMappedPtr + mCurrentOffset;;
            mCurrentOffset += sizeBytes;
        }

        return re;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GLFrameBuffer
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    GLFrameBuffer::GLFrameBuffer(NCount w, NCount h, NCount d, PixelFormat pf, Nmark mode):
        GLFrameBufferBase( w, h, d, pf, mode)
    {
    }

    //-----------------------------------------------------------------------------
    void GLFrameBuffer::write(const PixelBlock & src, const Box & dstSize)
    {
        if(!mBuffer.contains(dstSize))
            N_EXCEPT(InvalidParam, _I18n("destination box out of range"));

        PixelBlock scaled;
        bool needfree = true;
        if(src.getWidth() != dstSize.getWidth() || src.getHeight() != dstSize.getHeight() ||
            src.getDepth() != dstSize.getDepth())
        {
            allocateBuffer();
            scaled = mBuffer.getSub(dstSize);
            PixelBlock::scale(src, scaled, PixelBlock::F_Bilinear);
        }
        else if(GLPixelUtil::getGLInternalFormat(src.mFormat) == 0)
        {
            allocateBuffer();
            scaled = mBuffer.getSub(dstSize);
            PixelUtil::conv(src, scaled);
        }
        else
        {
            needfree = false;
            // 不需要转换
            scaled = src;
        }
        // 来源像素盒更新到指定范围盒
        upload(scaled, dstSize);
        if(needfree)
            freeBuffer();
    }
    //-----------------------------------------------------------------------------
    void GLFrameBuffer::read(const Box & srcSize, PixelBlock & dst)
    {
        if(!mBuffer.contains(srcSize))
            N_EXCEPT(InvalidParam, _I18n("source box out of range"));
        if(srcSize.mLeft == 0 && srcSize.mRight == getWidth() && srcSize.mTop == 0 && srcSize.mBottom == getHeight() && 
            srcSize.mFront == 0 && srcSize.mBack == getDepth() &&
            dst.getWidth() == getWidth() && dst.getHeight() == getHeight() && dst.getDepth() == getDepth() && 
            GLPixelUtil::getGLInternalFormat(dst.mFormat) != 0)
        {
            download(dst);
        }
        else
        {
            allocateBuffer();
            download(mBuffer);
            if(srcSize.getWidth() != dst.getWidth() || srcSize.getHeight() != dst.getHeight() ||
                srcSize.getDepth() != dst.getDepth())
            {
                PixelBlock::scale(mBuffer.getSub(srcSize), dst, PixelBlock::F_Bilinear);
            }
            else
            {
                PixelUtil::conv(mBuffer.getSub(srcSize), dst);
            }
            freeBuffer();
        }
    }
    //-----------------------------------------------------------------------------
    Buffer * GLFrameBuffer::clone(Nmark m) const
    {
        return 0;
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // GLCounterBuffer
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    GLCounterBuffer::GLCounterBuffer(BufferManager * mag, NCount size, NCount count, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0)
    {
        
    }
    //-----------------------------------------------------------------------------
    GLCounterBuffer::~GLCounterBuffer()
    {
        
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // GLTextureBuffer1
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    GLTextureBuffer1::GLTextureBuffer1(GLRenderSystem * sys, GLTexture * parent, GLint face,
        GLint level, NCount width, NCount height, NCount depth) : 
        GLFrameBuffer(width, height, depth, parent->getFormat(), parent->getModeMark()),
        mTarget(parent->getGLTextureTarget()),
        mFaceTarget(0),
        mTextureID(parent->getGLID()),
        mFace(face), 
        mLevel(level),
        mHwGamma(parent->isHardwareSRGB()),
        mSliceTRT(0),
        mRenderSys(sys)
    {
        // Get face identifier
        mFaceTarget = mTarget;
        if (mTarget == GL_TEXTURE_CUBE_MAP)
            mFaceTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;

        // Get format
        mGLInternalFormat = GLPixelUtil::getGLInternalFormat(mFormat, mHwGamma);

        // Default
        mRowSize = mWidth;
        mSliceSize = mHeight * mWidth;
        mSize = PixelUtil::getSize(mWidth, mHeight, mDepth, 1, mFormat);

        // 建立像素盒
        mBuffer = PixelBlock(mWidth, mHeight, mDepth, mFormat);

        if(mWidth==0 || mHeight==0 || mDepth==0)
            return;

        // Allocate buffer
        //if(mMark & Buffer::M_DEV)
        //    allocateBuffer();
        // 这是个渲染目标?
        if(mMark & Texture::MM_FRAME_RGB)
        {
            // Create render target for each slice
            mSliceTRT.reserve(mDepth);
            for(NCount zoffset=0; zoffset < mDepth; ++zoffset)
            {
                String name;
                name = _T("rtt/") + N_conv((NCount)this) + _T("/") + parent->getOriginID();
                GLSurfaceDesc surface;
                surface.buffer = this;
                surface.zoffset = zoffset;
                TextureFrame *trt = GLRTTManager::getOnly().createRenderTexture(name, surface, mHwGamma, parent->getFSAA());
                mSliceTRT.push_back(trt);
                Engine::getOnly().getRender()->add(mSliceTRT[zoffset]);
            }
        }
    }
    //----------------------------------------------------------------------------
    GLTextureBuffer1::~GLTextureBuffer1()
    {
        if(mMark & Texture::MM_FRAME_RGB)
        {
            // Delete all render targets that are not yet deleted via destroyImpl because the rendertarget
            // was deleted by the user.
            for (SliceTRT::const_iterator it = mSliceTRT.begin(); it != mSliceTRT.end(); ++it)
            {
                Engine::getOnly().getRender()->destroyFrame(*it);
            }
        }
    }
        //----------------------------------------------------------------------------
    void GLTextureBuffer1::_bindToFramebuffer(GLenum attachment, uint32 zoffset, GLenum which)
    {
        N_assert(zoffset < mDepth);
        N_assert(which == GL_READ_FRAMEBUFFER || which == GL_DRAW_FRAMEBUFFER || which == GL_FRAMEBUFFER);

        mRenderSys->getStateManager()->bindGLTexture( mTarget, mTextureID );
        switch(mTarget)
        {
        case GL_TEXTURE_1D:
        case GL_TEXTURE_2D:
        case GL_TEXTURE_RECTANGLE:
            GLCheck(glFramebufferTexture(which, attachment,
                                                     mTextureID, mLevel));
            break;
        case GL_TEXTURE_CUBE_MAP:
            GLCheck(glFramebufferTexture2D(which, GL_COLOR_ATTACHMENT0,
                                                       mFaceTarget, mTextureID, mLevel));
            break;
        case GL_TEXTURE_3D:
        case GL_TEXTURE_2D_ARRAY:
            GLCheck(glFramebufferTexture3D(which, attachment,
                                                       mFaceTarget, mTextureID, mLevel, zoffset));
            break;
        }
    }
    //-----------------------------------------------------------------------------
    void GLTextureBuffer1::upload(const PixelBlock & data, const Box & dest)
    {
        mRenderSys->getStateManager()->bindGLTexture(mTarget, mTextureID);
        
        // PBOs have no advantage with this usage pattern
        // see: https://www.khronos.org/opengl/wiki/Pixel_Buffer_Object
#ifdef USE_PBO
        // Calculate size for all mip levels of the texture.
        size_t dataSize = data.getWholeSliceSize();
        GL3PlusHardwareBuffer buffer(GL_PIXEL_UNPACK_BUFFER, dataSize, mUsage);
        buffer.writeData(0, dataSize, data.mData, false);

        PixelBox tmp(data.getWidth(), data.getHeight(), data.getHeight(), data.mFormat);
        tmp.mData = buffer.lockImpl(0, dataSize, Buffer::HBL_DISCARD);
        PixelUtil::bulkPixelConversion(data, tmp);
        buffer.unlockImpl(dataSize);

        // std::stringstream str;
        // str << "GL3PlusHardwarePixelBuffer::upload: " << mTextureID
        // << " pixel buffer: " << buffer.getGLBufferId()
        // << " bytes: " << mSizeInBytes
        // << " dest depth: " << dest.getDepth()
        // << " dest front: " << dest.front
        // << " datasize: " << dataSize
        // << " face: " << mFace << " level: " << mLevel
        // << " width: " << mWidth << " height: "<< mHeight << " depth: " << mDepth
        // << " format: " << PixelUtil::getFormatName(mFormat);
        // LogManager::getSingleton().logMessage(LML_NORMAL, str.str());

        void* pdata = NULL;
#else
        void* pdata = data.getData();
#endif
        
        if(PixelUtil::isCompress(data.mFormat))
        {
            if(data.mFormat != mFormat || !data.isWholeSlice())
                N_EXCEPT(InvalidParam, _I18n("Compressed images must be consecutive, in the source format"));
            GLenum format = GLPixelUtil::getGLInternalFormat(mFormat, mHwGamma);
            // Data must be consecutive and at beginning of buffer as PixelStorei not allowed
            // for compressed formats
            switch(mTarget) 
			{
                case GL_TEXTURE_1D:
                    // some systems (e.g. old Apple) don't like compressed subimage calls
                    // so prefer non-sub versions
                    if (dest.mLeft == 0)
                    {
                        GLCheck(glCompressedTexImage1D(GL_TEXTURE_1D, mLevel, format, dest.getWidth(), 0, data.getWholeSliceSize(),
                            pdata));
                    }
                    else
                    {
                        GLCheck(glCompressedTexSubImage1D(GL_TEXTURE_1D, mLevel, dest.mLeft, dest.getWidth(), format,
                            data.getWholeSliceSize(), pdata));
                    }
                    break;
                case GL_TEXTURE_2D:
                case GL_TEXTURE_CUBE_MAP:
                case GL_TEXTURE_RECTANGLE:
                    // some systems (e.g. old Apple) don't like compressed subimage calls
                    // so prefer non-sub versions
                    if (dest.mLeft == 0 && dest.mTop == 0)
                    {
                        GLCheck(glCompressedTexImage2D(mFaceTarget, mLevel, format, dest.getWidth(), dest.getHeight(), 0, 
							data.getWholeSliceSize(), pdata));
                    }
                    else
                    {
                        GLCheck(glCompressedTexSubImage2D(mFaceTarget, mLevel, dest.mLeft, dest.mTop, dest.getWidth(), dest.getHeight(),
                            format, data.getWholeSliceSize(), pdata));
                    }
                    break;
                case GL_TEXTURE_3D:
                case GL_TEXTURE_2D_ARRAY:
                    // some systems (e.g. old Apple) don't like compressed subimage calls
                    // so prefer non-sub versions
                    if (dest.mLeft == 0 && dest.mTop == 0 && dest.mFront == 0)
                    {
                        GLCheck(glCompressedTexImage3DARB(mTarget, mLevel, format, 
							dest.getWidth(), dest.getHeight(), dest.getDepth(), 
							0, data.getWholeSliceSize(), pdata));
                    }
                    else
                    {

                        GLCheck(glCompressedTexSubImage3D(mTarget, mLevel,
                            dest.mLeft, dest.mTop, dest.mFront,
                            dest.getWidth(), dest.getHeight(), dest.getDepth(),
                            format, data.getWholeSliceSize(),
                            pdata));
                    }
                    break;
            }

        }
        else
        {
#ifndef USE_PBO
            NCount sliceskip = data.mSliceSize / data.mPixelSize;
            NCount rowskip = data.mRowSize / data.mPixelSize;
            if (data.getWidth() != rowskip)
                glPixelStorei(GL_UNPACK_ROW_LENGTH, rowskip);
            if (data.getHeight() * data.getWidth() != sliceskip)
                glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, (sliceskip / data.getWidth()));
#endif
            if ((data.getWidth()*PixelUtil::getUnitSize(data.mFormat)) & 3) {
                // Standard alignment of 4 is not right
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            }

            GLenum type = GLPixelUtil::getGLOriginDataType(data.format);

            if (PixelUtil::isDepth(data.format))
            {
                switch (GLPixelUtil::getGLInternalFormat(data.format))
                {
                case GL_DEPTH_COMPONENT16:
                    type = GL_UNSIGNED_SHORT;
                    break;

                default:
                case GL_DEPTH_COMPONENT24:
                case GL_DEPTH_COMPONENT32:
                    type = GL_UNSIGNED_INT;
                    break;
                }
            }
            
            switch(mTarget)
            {
            case GL_TEXTURE_1D:
                glTexSubImage1D(GL_TEXTURE_1D, mLevel, dest.mLeft, dest.getWidth(),
                    GLPixelUtil::getGLOriginFormat(data.mFormat), type, pdata);
                break;
            case GL_TEXTURE_2D:
            case GL_TEXTURE_CUBE_MAP:
            case GL_TEXTURE_RECTANGLE:
                glTexSubImage2D(mFaceTarget, mLevel, dest.mLeft, dest.mTop, dest.getWidth(), dest.getHeight(), 
                    GLPixelUtil::getGLOriginFormat(data.mFormat), type, pdata);
                break;
            case GL_TEXTURE_3D:
            case GL_TEXTURE_2D_ARRAY:
                glTexSubImage3D(mTarget, mLevel, dest.mLeft, dest.mTop, dest.mFront, dest.getWidth(), dest.getHeight(), dest.getDepth(),
                    GLPixelUtil::getGLOriginFormat(data.mFormat), type, pdata);
                break;
            }
        }

        // TU_AUTOMIPMAP is only enabled when there are no custom mips
        // so we do not have to care about overwriting
        if ((mMark & Texture::MM_MIPMAP_AUTO) && (mLevel == 0))
        {
            GLCheck(glGenerateMipmap(mTarget));
        }

        // Restore defaults.
        GLCheck(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
        GLCheck(glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0));
        GLCheck(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
    }
    //-----------------------------------------------------------------------------
    void GLTextureBuffer1::download(const PixelBlock & data)
    {
        if(data.getWidth() != getWidth() || data.getHeight() != getHeight() || data.getDepth() != getDepth())
            N_EXCEPT(InvalidParam, _I18n("only download of entire buffer is supported by GL"));
        
        mRenderSys->getStateManager()->bindGLTexture(mTarget, mTextureID);
        
        if(PixelUtil::isCompress(data.mFormat))
        {
            if(data.mFormat != mFormat || !data.isWholeSlice())
                N_EXCEPT(InvalidParam, _I18n("Compressed images must be consecutive, in the source format"));
            // Data must be consecutive and at beginning of buffer as PixelStorei not allowed
            // for compressed formate
            GLCheck(glGetCompressedTexImage(mFaceTarget, mLevel, data.mData));
        }
        else
        {
            NCount sliceskip = data.mSliceSize / data.mPixelSize;
            NCount rowskip = data.mRowSize / data.mPixelSize;
            if (data.getWidth() != rowskip)
                glPixelStorei(GL_PACK_ROW_LENGTH, rowskip);
            if (data.getHeight() * data.getWidth() != sliceskip)
                glPixelStorei(GL_PACK_IMAGE_HEIGHT, (sliceskip / data.getWidth()));
            if ((data.getWidth() * PixelUtil::getUnitSize(data.mFormat)) & 3) {
                // Standard alignment of 4 is not right
                glPixelStorei(GL_PACK_ALIGNMENT, 1);
            }
            // We can only get the entire texture
            glGetTexImage(mFaceTarget, mLevel,
                GLPixelUtil::getGLOriginFormat(data.mFormat), GLPixelUtil::getGLOriginDataType(data.mFormat), data.getData());
            // Restore defaults
            glPixelStorei(GL_PACK_ROW_LENGTH, 0);
            glPixelStorei(GL_PACK_IMAGE_HEIGHT, 0);
            glPixelStorei(GL_PACK_ALIGNMENT, 4);
        }
    }
    //-----------------------------------------------------------------------------
    bool GLTextureBuffer1::reserveImpl(NCount size, Nmark newMode, bool oldData)
    {
        return false;
    }
    //-----------------------------------------------------------------------------
    void GLTextureBuffer1::bindToFramebuffer(Nui32 attachment, NCount zoft)
    {
        // Delegate the framebuffer binding to a more specific function
        // This call retains the original implementation using GL_FRAMEBUFFER (aka GL_DRAW_FRAMEBUFFER)
        _bindToFramebuffer(attachment, zoffset, GL_DRAW_FRAMEBUFFER);
    }
    //-----------------------------------------------------------------------------
    void GLTextureBuffer1::copyFromFramebuffer(NCount zoffset)
    {
        mRenderSys->getStateManager()->bindGLTexture(mTarget, mTextureID);
        switch(mTarget)
        {
        case GL_TEXTURE_1D:
            GLCheck(glCopyTexSubImage1D(mFaceTarget, mLevel, 0, 0, 0, mWidth));
            break;
        case GL_TEXTURE_2D:
        case GL_TEXTURE_CUBE_MAP:
        case GL_TEXTURE_RECTANGLE:
            GLCheck(glCopyTexSubImage2D(mFaceTarget, mLevel, 0, 0, 0, 0, mWidth, mHeight));
            break;
        case GL_TEXTURE_3D:
        case GL_TEXTURE_2D_ARRAY:
            GLCheck(glCopyTexSubImage3D(mFaceTarget, mLevel, 0, 0, zoffset, 0, 0, mWidth, mHeight));
            break;
        }
    }
    //-----------------------------------------------------------------------------
    void GLTextureBuffer1::write(const FrameBuffer * src, const Box & srcBox, const Box & dstBox)
    {
        const GLTextureBuffer1 * srct = static_cast<const GLTextureBuffer1 *>(src);
        // Check for FBO support first
        // Destination texture must be 1D, 2D, 3D, or Cube
        // Source texture must be 1D, 2D or 3D

        // This does not sem to work for RTTs after the first update
        // I have no idea why! For the moment, disable
        if((src->getModeMark() & Texture::MM_FRAME_RGB) == 0 &&
            (srct->mTarget == GL_TEXTURE_1D || srct->mTarget == GL_TEXTURE_2D
             || srct->mTarget == GL_TEXTURE_RECTANGLE || srct->mTarget == GL_TEXTURE_3D)
                && mTarget != GL_TEXTURE_2D_ARRAY)
        {
            blitFromTexture(srct, srcBox, dstBox);
        }
        else
        {
            GLFrameBuffer::write(src, srcBox, dstBox);
        }
    }
    //-----------------------------------------------------------------------------
    // Very fast texture-to-texture blitter and hardware bi/trilinear scaling implementation using FBO
    // Destination texture must be 1D, 2D, 3D, or Cube
    // Source texture must be 1D, 2D or 3D
    // Supports compressed formats as both source and destination format, it will use the hardware DXT compressor
    // if available.
    // @author W.J. van der Laan
    void GLTextureBuffer1::blitFromTexture(const GLTextureBuffer1 * src, const Box & srcBox, const Box & dstBox)
    {
        //        std::cerr << "GL3PlusTextureBuffer::blitFromTexture " <<
        //        src->mTextureID << ":" << srcBox.left << "," << srcBox.top << "," << srcBox.right << "," << srcBox.bottom << " " <<
        //        mTextureID << ":" << dstBox.left << "," << dstBox.top << "," << dstBox.right << "," << dstBox.bottom << std::endl;
        // Store reference to FBO manager
        GL3PlusFBOManager * fboMan = static_cast<GL3PlusFBOManager *>(GL3PlusRTTManager::getSingletonPtr());

        GLenum filtering = GL_LINEAR;

        // Set filtering modes depending on the dimensions and source
        if (srcBox.getWidth()==dstBox.getWidth() && srcBox.getHeight()==dstBox.getHeight() &&
            srcBox.getDepth()==dstBox.getDepth())
        {
            // Dimensions match -- use nearest filtering (fastest and pixel correct)
            filtering = GL_NEAREST;
        }

        // Store old binding so it can be restored later
        GLint oldfb;
        GLCheck(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldfb));

        // Set up temporary FBO
        GLuint tempFBO[2] = { 0, 0 };
        GLCheck(glGenFramebuffers(1, &tempFBO[0]));
        GLCheck(glGenFramebuffers(1, &tempFBO[1]));
        mRenderSys->getStateManager()->bindGLFrameBuffer( GL_DRAW_FRAMEBUFFER, tempFBO[0] );
        mRenderSys->getStateManager()->bindGLFrameBuffer( GL_READ_FRAMEBUFFER, tempFBO[1] );

        TexturePtr tempTex;
        if (fboMan->checkFormat(mFormat))
        {
            // We are going to bind directly, so set viewport to size and position of destination slice
            mRenderSys->getStateManager()->setViewport(dstBox.left, dstBox.top, dstBox.getWidth(), dstBox.getHeight());
        }
        else
        {
            // If target format not directly supported, create intermediate texture
            tempTex = N_Only(Texture).createTexture(0, GroupInner, T_2D, fboMan->getSupportedAlternative(mFormat),
                dstBox.getWidth(), dstBox.getHeight(), dstBox.getDepth(), 0);

            GLCheck(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, static_pointer_cast<GL3PlusTexture>(tempTex)->getGLID(), 0));

            GLCheck(glCheckFramebufferStatus(GL_FRAMEBUFFER));

            // Set viewport to size of destination slice
            mRenderSys->getStateManager()->setViewport(0, 0, dstBox.getWidth(), dstBox.getHeight());
        }

        bool isDepth = PixelUtil::isDepth(mFormat);

        // Process each destination slice
        for(uint32 slice = dstBox.front; slice < dstBox.back; ++slice)
        {
            if (!tempTex)
            {
                // Bind directly
                bindToFramebuffer(isDepth ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0, slice);
            }

            GLCheck(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER));

            GLbitfield mask = GL_ZERO;

            // Bind the appropriate source texture to the read framebuffer
            if (isDepth)
            {
                src->_bindToFramebuffer(GL_DEPTH_ATTACHMENT, slice, GL_READ_FRAMEBUFFER);

                GLCheck(glReadBuffer(GL_NONE));

                mask |= GL_DEPTH_BUFFER_BIT;

                // Depth framebuffer sources can only be blit with nearest filtering
                filtering = GL_NEAREST;
            }
            else
            {
                src->_bindToFramebuffer(GL_COLOR_ATTACHMENT0, slice, GL_READ_FRAMEBUFFER);

                GLCheck(glReadBuffer(GL_COLOR_ATTACHMENT0));

                mask |= GL_COLOR_BUFFER_BIT;
            }

            GLCheck(glCheckFramebufferStatus(GL_READ_FRAMEBUFFER));

            //N_assert(mask != GL_ZERO);

            // Perform blit from the source texture bound to read framebuffer to
            // this texture bound to draw framebuffer using the pixel coorinates.
            // Sampling ouside the source box is implicitly handled using GL_CLAMP_TO_EDGE.
            GLCheck(glBlitFramebuffer(srcBox.mLeft, srcBox.mTop, srcBox.mRight, srcBox.mBottom,
                dstBox.mLeft, dstBox.mTop, dstBox.mRight, dstBox.mBottom, mask, filtering));
                                                  
            if(tempTex)
            {
                mRenderSys->getStateManager()->bindGLTexture(mTarget, mTextureID);
                switch(mTarget)
                {
                case GL_TEXTURE_1D:
                    glCopyTexSubImage1D(mFaceTarget, mLevel, dstBox.mLeft, 0, 0, dstBox.getWidth());
                    break;
                case GL_TEXTURE_2D:
                case GL_TEXTURE_CUBE_MAP:
                case GL_TEXTURE_RECTANGLE:
                    glCopyTexSubImage2D(mFaceTarget, mLevel, dstBox.mLeft, dstBox.mTop, 0, 0, dstBox.getWidth(), dstBox.getHeight());
                    break;
                case GL_TEXTURE_3D:
                case GL_TEXTURE_2D_ARRAY:
                    glCopyTexSubImage3D(mFaceTarget, mLevel, dstBox.mLeft, dstBox.mTop, slice, 0, 0, dstBox.getWidth(), dstBox.getHeight());
                    break;
                }
            }
        }

        // Finish up
        if (!tempTex)
        {
            // Generate mipmaps
            if (mMark & MM_MIPMAP_AUTO)
            {
                mRenderSys->getStateManager()->bindGLTexture( mTarget, mTextureID );
                GLCheck(glGenerateMipmap(mTarget));
            }
        }

        // Reset source texture to sane state
        mRenderSys->getStateManager()->bindGLTexture( src->mTarget, src->mTextureID );

        GLCheck(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, isDepth ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0));

        // Reset read buffer/framebuffer
        GLCheck(glReadBuffer(GL_NONE));
        mRenderSys->getStateManager()->bindGLFrameBuffer( GL_READ_FRAMEBUFFER, 0 );

        // Restore old framebuffer
        mRenderSys->getStateManager()->bindGLFrameBuffer( GL_DRAW_FRAMEBUFFER, oldfb);
        
        mRenderSys->getStateManager()->deleteGLFrameBuffer(GL_FRAMEBUFFER, tempFBO[0]);
        mRenderSys->getStateManager()->deleteGLFrameBuffer(GL_FRAMEBUFFER, tempFBO[1]);
        
        if(tempTex)
            N_Only(Texture).remove(tempTex);
    }
    //-----------------------------------------------------------------------------
    // write doing hardware trilinear scaling
    void GLTextureBuffer1::write(const PixelBlock & src, const Box & dstBox)
    {
        // Fall back to normal GLFrameBuffer::write in case
        // - FBO is not supported
        // - Either source or target is luminance due doesn't looks like supported by hardware
        // - the source dimensions match the destination ones, in which case no scaling is needed
        if (src.getWidth() == dstBox.getWidth() && src.getHeight() == dstBox.getHeight() &&
            src.getDepth() == dstBox.getDepth())
        {
            GLFrameBuffer::write(src, dstBox);
            return;
        }
        if(!mBuffer.contains(dstBox))
            N_EXCEPT(InvalidParam, _I18n("destination box out of range"));
        // For scoped deletion of conversion buffer

        Texture::Type type = (src.getDepth() != 1) ? Texture::T_3D : Texture::T_2D;

        // Set automatic mipmap generation; nice for minimisation
        Texture * tex = N_Only(Texture).createTexture(0, GroupInner, type, src.mFormat,
            src.getWidth(), src.getHeight(), src.getDepth(), N_TEX_MIPMAP);

        // Upload data to 0,0,0 in temporary texture
        Box tempTarget(0, 0, 0, src.getWidth(), src.getHeight(), src.getDepth());
        tex->getBuffer()->write(src, tempTarget);

        // Blit from texture
        write(tex->getBuffer(), tempTarget, dstBox);

        // Delete temp texture
        N_Only(Texture).destroy(tex->getOriginID());
    }
    //-----------------------------------------------------------------------------
    TextureFrame * GLTextureBuffer1::getBuffer(Nidx zoffset)
    {
        N_assert(mMark & Texture::MM_FRAME_RGB);
        N_assert(zoffset < mDepth);
        return mSliceTRT[zoffset];
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    //GLRenderBuffer
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    GLRenderBuffer::GLRenderBuffer(GLenum format, NCount width, NCount height, GLsizei numSamples):
        GLFrameBuffer(width, height, 1, GLPixelUtil::getClosestNIIFormat(format), M_WRITE)
        //mRenderbufferID(0)
    {
        mGLInternalFormat = format;
        glGenRenderbuffersEXT(1, &mRenderbufferID);
        glBindRenderbufferEXT(GL_RENDERBUFFER, mRenderbufferID);
        if(numSamples > 0)
        {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, numSamples, format, width, height);
        }
        else
        {
            glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
        }
    }
    //-----------------------------------------------------------------------------
    GLRenderBuffer::~GLRenderBuffer()
    {
        glDeleteRenderbuffers(1, &mRenderbufferID);
    }
    //-----------------------------------------------------------------------------
    void GLRenderBuffer::bindToFramebuffer(Nui32 attachment, NCount zoft)
    {
        N_assert(zoft < mDepth);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, mRenderbufferID);
    }
    //-----------------------------------------------------------------------------
    bool GLRenderBuffer::reserveImpl(NCount size, Nmark newMode, bool oldData)
    {
        return false;
    }
    //-----------------------------------------------------------------------------
}