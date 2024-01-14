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

#ifndef __GLES2HardwareBuffer_H__
#define __GLES2HardwareBuffer_H__

#include "OgreGLES2Prerequisites.h"
#include "OgreHardwareBuffer.h"

namespace Ogre 
{
    class GLES2RenderSystem;
    class GLES2BufferOp;
    /** GL3+ doesn't support mapping the same buffer twice even if the regions
        don't overlap. For performance we keep many buffers as one big buffer,
        but for compatibility reasons (with GL3/DX10 HW) we treat them as
        separate buffers.
    @par
        This class takes care of mapping buffers once while allowing
        BufferOp to map subregions of it as if they were separate
        buffers.
    @remarks
        This is a very thin/lightweight wrapper around OpenGL glMapBufferRange, thus:
            Caller is responsible for flushing regions before unmapping.
            Caller is responsible for proper synchronization.
            No check is performed to see if two map calls overlap.
    */
    class _OgreGLES2Export GLES2Buffer
    {
    public:
        GLES2Buffer(GLuint vboid, GLuint size, GLES2GBufferManager * mag, BufferModeMark mode);
        ~GLES2Buffer();

        inline GLuint getVboName() const               { return mVboName; }

        /// Assumes mVboName is already bound to GL_COPY_WRITE_BUFFER!!!
        void * map( NCount start, NCount count, NCount & outTicket );

        /// Flushes the region of the given ticket. start is 0-based.
        void flush( NCount ticket, NCount start, NCount count );

        /// Unmaps given ticket (got from @see map).
        /// Assumes mVboName is already bound to GL_COPY_WRITE_BUFFER!!!
        /// The ticket becomes invalid after this.
        void unmap( NCount ticket );
    protected:
        NCount addMappedRange(NCount start, NCount count);

        struct MappedRange
        {
            NCount start;
            NCount count;

            MappedRange( NCount _start, NCount _count ) : start( _start ), count( _count ) {}
        };

        typedef vector<MappedRange>::type MappedRangeVec;

        GLuint mVboName;
        GLuint mVboSize;
        void * mMappedPtr;

        MappedRangeVec mMappedRanges;
        vector<size_t>::type mFreeRanges;

        BufferModeMark mPersistentMethod;
    };
    
    /** For GL3+, most (if not all) buffers, can be treated with the same code.
        Hence most equivalent functionality is encapsulated here.
    */
    class _OgreGLES2Export GLES2BufferOp : public BufferOp
    {
    public:
        GLES2BufferOp(size_t vboidx, GLuint vboid, GLES2Buffer * buf);
        
        ~GLES2BufferOp();

        inline size_t getVboPoolIndex() const           { return mGroupIndex; }
        
        inline GLuint getVboName() const                { return mVboName; }

        /// Only use this function for the first upload
        virtual void initImpl( void *data, NCount oft, NCount size );

        virtual void * lockImpl(MuteMark m, NCount oft, NCount size, NCount multiIdx = 0);
        
        virtual void unlockImpl(UnlockType type, NCount oft, NCount size);

        virtual bool reserveImpl(NCount size, BufferModeMark newMode = -1, bool oldData = true);
        
        virtual void memcpyImpl(void * data, NCount oft, NCount size, bool write = true);
        virtual void memcpyImpl(Buffer * src, NCount srcOft, NCount oft, NCount size); //gles3.0
        
        virtual void waitImpl();
    protected:
        size_t mGroupIndex;
        GLuint mVboName;
        void * mMappedPtr;
        size_t mUnmapTicket;
        
        GLES2Buffer *mGLBuffer;
    };
    
    class GLES2HardwareBuffer
    {
    public:
        GLES2HardwareBuffer(GLenum target, size_t sizeInBytes, GLenum usage);
        ~GLES2HardwareBuffer();

        void readData(size_t offset, size_t length, void* pDest);

        void writeData(size_t offset, size_t length, const void * pSource, bool discardWholeBuffer = false);

        void copyData(GLuint srcBufferId, size_t srcOffset, size_t dstOffset, size_t length, bool discardWholeBuffer);

        inline GLuint getGLBufferId(void) const            { return mBufferId; }
        
        void createBuffer();

        void destroyBuffer();

        void* lockImpl(size_t offset, size_t length, Buffer::LockOptions options);

        void unlockImpl();
    private:
        GLenum mTarget;
        GLenum mUsage;

        GLuint mBufferId;
        GLES2RenderSystem * mRenderSys;
        size_t mSizeInBytes;

        /// Utility function to get the correct GL usage based on HBU's
        static GLenum getGLUsage(uint32 usage);
    };
    
    /// Specialisation of VertexBuffer for OpenGL ES
    class _OgreGLES2Export GLES2VertexBuffer : public VertexBuffer MANAGED_RESOURCE
    {  
    public:
        GLES2VertexBuffer(BufferManager * mag, NCount size, NCount count, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);
    };
    
    class _OgreGLES2Export GLES2IndexBuffer : public IndexBuffer MANAGED_RESOURCE
    {
    public:
        GLES2IndexBuffer(BufferManager * mag, NCount size, NCount count, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);
    };
    
    class _OgreGLES2Export GLES2StructBuffer : public StructBuffer
    {
    public:
        GLES2StructBuffer(BufferManager * mag, NCount size, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0 );
        virtual ~GLES2StructBuffer();

        void bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft, NCount size);
    private:
        inline void bindBuffer( Nui16 slot );
    };
    
    class _OgreGLES2Export GLES2TextureBufferEmu : public TextureBuffer
    {
    public:
        GLES2TextureBufferEmu(BufferManager * mag, NCount size, PixelFormat pf, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);
        virtual ~GLES2TextureBufferEmu();

        void bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft = 0, NCount size = 0);
    protected:
        GLuint mTexName;
        GLenum mInternalFormat;

        GLenum mOriginFormat;
        GLenum mOriginDataType;
        size_t mMaxTexSize;
        size_t mInternalNumElemBytes;
        size_t mInternalNumElements;

        inline void bindBuffer( Nui16 slot, size_t offset, size_t sizeBytes );
    };
    
    /** A staging buffer is a buffer that resides on the GPU and be written to/from both CPU & GPU
        However the access in both cases is limited. GPUs can only copy (i.e. memcpy) to another
        real buffer (can't be used directly as i.e. texture or vertex buffer) and CPUs can only
        map it.
        In other words, a staging buffer is an intermediate buffer to transfer data between
        CPU & GPU
    */
    class _OgreGLES2Export GLES2MappedBuffer : public MappedBuffer
    {
    public:
        GLES2MappedBuffer(GBufferManager * mag, NCount offset, NCount size, BufferModeMark mode, GLuint vboName);
        virtual ~GLES2MappedBuffer();

        virtual BlockType request( NCount sizeBytes, bool upload = true);

        void cleanUnfencedHazards(void);

        virtual NCount async( OperationList & oplist, bool upload);

        GLuint getBufferName(void) const           { return mVboName; }
    protected:
        /** Adds a fence to the region [from; to]. The region may actually be kept in
            mUnfencedHazards until the threshold is achieved or we're forced to fence
            it (i.e. when the internal buffer wraps around)
        @param forceFence
            When true, the region is merged with all the other unfenced hazards, and
            put behind a fence
        */
        void addFence( size_t from, size_t to, bool forceFence );

        void deleteFences( GLFenceVec::iterator itor, GLFenceVec::iterator end );

        /// Waits undefinitely on the given GLSync object. Can throw if failed to wait.
        void wait( GLsync syncObj );

        /// Checks all the fences and compares them against current state (i.e. mMappingStart &
        /// mMappingCount), and stalls if needed (synchronize); also book-keeps mFences and
        /// mUnfencedHazards.
        /// May modify mMappingStart.
        void waitIfNeeded(void);

        virtual void* writeMapImpl( NCount sizeBytes );
        virtual void unmapImpl(  );

        virtual const void* readMapImpl( NCount offset, NCount sizeBytes );

        /// mVboName is not deleted by us (the GBufferManager does) as we may have
        /// only been assigned a chunk of the buffer, not the whole thing.
        GLuint mVboName;
        void * mMappedPtr;

        /** How many bytes between the last fence and our current offset do we need to let
            through before we place another fence?
        @remarks
            When the ring buffer wraps around, a fence is always put.
            A threshold of zero means to put a fence after every unmap operation.
        */
        size_t  mFenceThreshold;

        struct GLFence : Block
        {
            GLsync  fenceName;

            GLFence( size_t _start, size_t _end ) :
                Block( _start , _end ), fenceName( 0 )
            {
            }
        };

        //------------------------------------
        // Begin used for uploads
        //------------------------------------
        typedef vector<GLFence>::type GLFenceVec;
        GLFenceVec mFences;

        /// Regions of memory that were unmapped but haven't
        /// been fenced due to not passing the threshold yet.
        GLFenceVec mUnfencedHazards;
        size_t mUnfencedBytes;
        //------------------------------------
        // End used for uploads
        //------------------------------------
    };
    
    struct _OgreGLES2Export GLES2VertexArrayObject : public GeometryRaw
    {
        GLint   mPrimType[2];

        GLES2VertexArrayObject( GLuint vaoName, uint32 renderQueueId, const VertexBufferList &vertexBuffers,
            IndexBufferPacked *indexBuffer, OperationType opType ) :
                GeometryRaw( vaoName, renderQueueId, 0, vertexBuffers, indexBuffer, opType )
        {
            switch( opType )
            {
            case OT_POINT_LIST:
                mPrimType[0] = GL_POINTS;
                mPrimType[1] = GL_POINTS;
                break;
            case OT_LINE_LIST:
                mPrimType[0] = GL_LINES;
                    mPrimType[1] = GL_LINES;//_ADJACENCY;
                break;
            case OT_LINE_STRIP:
                mPrimType[0] = GL_LINE_STRIP;
                mPrimType[1] = GL_LINE_STRIP;//_ADJACENCY;
                break;
            default:
            case OT_TRIANGLE_LIST:
                mPrimType[0] = GL_TRIANGLES;
                mPrimType[1] = GL_TRIANGLES;//_ADJACENCY;
                break;
            case OT_TRIANGLE_STRIP:
                mPrimType[0] = GL_TRIANGLE_STRIP;
                mPrimType[1] = GL_TRIANGLE_STRIP;//_ADJACENCY;
                break;
            case OT_TRIANGLE_FAN:
                mPrimType[0] = GL_TRIANGLE_FAN;
                mPrimType[1] = GL_TRIANGLE_FAN;
                break;
            }
        }
    };
}

#endif
