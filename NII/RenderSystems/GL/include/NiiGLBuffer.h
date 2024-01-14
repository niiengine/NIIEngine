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

#ifndef _NII_GLHARDWAREVERTEXBUFFER_H_
#define _NII_GLHARDWAREVERTEXBUFFER_H_

#include "NiiGLPreInclude.h"
#include "NiiGpuBuffer.h"
#include "NiiGLHardwarePixelBufferBase.h"

namespace NII
{
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
    class _EngineGLAPI GLBuffer
    {
    public:
        GLBuffer(GLuint vboid, GLuint size, GLGBufferManager * mag, BufferModeMark mode);
        ~GLBuffer();

        inline GLuint getVboName() const            { return mVboName; }

        /// Assumes mVboName is already bound to GL_COPY_WRITE_BUFFER!!!
        void * map(NCount start, NCount count, NCount & outTicket);

        /// Flushes the region of the given ticket. start is 0-based.
        void flush(NCount ticket, NCount start, NCount count);

        /// Unmaps given ticket (got from @see map).
        /// Assumes mVboName is already bound to GL_COPY_WRITE_BUFFER!!!
        /// The ticket becomes invalid after this.
        void unmap(NCount ticket);
    protected:
        struct MappedRange
        {
            NCount start;
            NCount count;

            MappedRange(NCount _start, NCount _count) : start(_start), count(_count) {}
        };

        typedef vector<MappedRange>::type MappedRangeVec;
    protected:
        NCount addMappedRange(NCount start, NCount count);

        GLuint mVboName;
        GLuint mVboSize;
        void * mMappedPtr;

        MappedRangeVec mMappedRanges;
        vector<NCount>::type mFreeRanges;

        BufferModeMark mPersistentMethod;
    };
    
    /** For GL3+, most (if not all) buffers, can be treated with the same code.
        Hence most equivalent functionality is encapsulated here.
    */
    class _EngineGLAPI GLBufferOp : public BufferOp
    {
    public:
        GLBufferOp(NCount vboidx, GLuint vboid, GLBuffer * dbuf);
        ~GLBufferOp();

        inline void _setVboPoolIndex(NCount idx)    { mGroupIndex = idx; }
        inline NCount getVboPoolIndex()             { return mGroupIndex; }

        inline GLuint getVboName() const            { return mVboName; }

        /// Only use this function for the first upload
        virtual void initImpl(void * data, NCount oft, NCount size);

        virtual void * lockImpl(MuteMark m, NCount oft, NCount size);
        virtual void unlockImpl(UnlockType type, NCount oft, NCount size);

        virtual bool reserveImpl(NCount size, BufferModeMark newMode = -1, bool oldData = true);
        virtual void memcpyImpl(void * data, NCount oft, NCount size, bool write = true);
        virtual void memcpyImpl(Buffer * src, NCount srcOft, NCount oft, NCount size);
        virtual void waitImpl();
    protected:
        NCount mGroupIndex;
        GLuint mVboName;
        void * mMappedPtr;

        NCount mUnmapTicket;
        GLBuffer * mGLBuffer;
    };
    
    class _EngineGLAPI GLIndexBuffer : public IndexBuffer
    {
    public:
        GLIndexBuffer(BufferManager * mag, NCount size, NCount count, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);
    };

    class _EngineGLAPI GLVertexBuffer : public VertexBuffer
    {
    public:
        GLVertexBuffer(BufferManager * mag, NCount size, NCount count, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);
    };
    
    class _EngineGLAPI GLStructBuffer : public StructBuffer
    {
    public:
        GLStructBuffer(BufferManager * mag, NCount size, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);
        virtual ~GLStructBuffer();
        
        void bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft, NCount size);
        
        void setGLBufferBinding(GLint binding);
        
        inline GLint getGLBufferBinding(void) const { return mBinding; }
    protected:
        GLint mBinding;
    };
    
    class _EngineGLAPI GLCounterBuffer : public CounterBuffer
    {
    public:
        GLCounterBuffer(BufferManager * mag, NCount size, NCount count, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);
        
        void bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft, NCount size);
        
        void setGLBufferBinding(GLint binding);
        
        inline GLint getGLBufferBinding(void) const { return mBinding; }
    protected:
        GLint mBinding;
    };
    
	class _EngineGLAPI GLFrameBuffer : public GLFrameBufferBase
	{
	public:
		GLFrameBuffer(NCount w, NCount h, NCount d, PixelFormat pf, Nmark mode);

		using FrameBuffer::write;

		/// @copydetails FrameBuffer::write
		void write(const PixelBlock & src, const Box & target);

		/// @copydetails FrameBuffer::read
		void read(const Box & src, PixelBlock & target);

		/// @copydetails FrameBuffer::clone
		Buffer * clone(Nmark m = Buffer::M_WRITE | Buffer::M_WHOLE | Buffer::M_HOST) const;
	};

    /// 纹理面缓存
    class _EngineGLAPI GLTextureBuffer1: public GLFrameBuffer
	{
    public:
		GLTextureBuffer1(GLRenderSystem * sys, GLTexture* parent, GLint face, GLint level,
            NCount width, NCount height, NCount depth);

        ~GLTextureBuffer1();

        /// @copydetails FrameBuffer::getBuffer
        TextureFrame * getBuffer(Nidx index = 0);

		/// @copydetails FrameBuffer::write
        virtual void write(const PixelBlock & src, const Box & target);

		/// @copydoc FrameBuffer::write
		void write(const FrameBuffer * target, const Box & src, const Box & dst);

		/// @copydetails GLFrameBuffer::bindToFramebuffer
		virtual void bindToFramebuffer(Nui32 attachment, NCount zoft);

        /// 从帧缓存中复制数据
        void copyFromFramebuffer(NCount zoft);

        /// 从纹理中复制数据
        void blitFromTexture(const GLTextureBuffer1 * src, const Box & srcBox, const Box & dstBox);
        
        void _bindToFramebuffer(GLenum attachment, uint32 zoffset, GLenum which);
	protected:
		/// @copydetails FrameBuffer::destroyImpl
		void destroyImpl(TextureFrame * obj, Nidx index) { mSliceTRT[index] = 0; }

		/// @copydetails GLFrameBuffer::upload
		virtual void upload(const PixelBlock & data, const Box & dest);

		/// @copydetails GLFrameBuffer::download
		virtual void download(const PixelBlock & data);

        /// @copydetails GLFrameBuffer::reserveImpl
        virtual bool reserveImpl(NCount size, Nmark newMode = -1, bool oldData = true);
	protected:
		typedef vector<TextureFrame *>::type SliceTRT;
		GLenum mTarget;
		GLenum mFaceTarget;		///< 像 mTarget 如 GL_TEXTURE_xD, 但存在 cubemap
		GLuint mTextureID;
		GLint mFace;
		GLint mLevel;
        bool mHwGamma;
        SliceTRT mSliceTRT;		///< 一般用于渲染纹理,如果是非正方纹理数量一般为1
        GLRenderSystem* mRenderSys;
    };

    /** 渲染到缓存面.需要FBO扩展
    @version NIIEngine 3.0.0 内部api
    */
    class _EngineGLAPI GLRenderBuffer: public GLFrameBuffer
	{
    public:
        GLRenderBuffer(GLenum format, NCount width, NCount height, GLsizei numSamples);

        ~GLRenderBuffer();

        /// @copydetails GLFrameBuffer::bindToFramebuffer
        virtual void bindToFramebuffer(Nui32 attachment, NCount zoft);

        /// @copydetails GLFrameBuffer::reserveImpl
        virtual bool reserveImpl(NCount size, Nmark newMode = -1, bool oldData = true);
    protected:
        GLuint mRenderbufferID;
        GLStateCacheManager * mState;
    };
    
    class _EngineGLAPI GLTextureBuffer : public TextureBuffer
    {
    public:
        GLTextureBuffer( BufferManager * mag, NCount size, PixelFormat pf, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);
        virtual ~GLTextureBuffer();

        void bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft = 0, NCount size = 0);

        virtual void _bind( NCount offset, NCount size );
    protected:
        GLuint mTexName;
        GLenum mInternalFormat;
    };
    
    class _EngineGLAPI GLTextureBufferEmu : public TextureBuffer
    {
    public:
        GLTextureBufferEmu(BufferManager * mag, NCount size, PixelFormat pf, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);

        virtual ~GLTextureBufferEmu();

        virtual void _bind( NCount offset, NCount size );
        
        void bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft = 0, NCount size = 0);
    protected:
        void bindBuffer(NCount offset, NCount size);
    protected:
        GLuint mTexName;
        GLenum mInternalFormat;

        GLenum mOriginFormat;
        GLenum mOriginDataType;
        size_t mMaxTexSize;
        size_t mInternalNumElemBytes;
        size_t mInternalNumElements;
    };
    
    class _EngineGLAPI GLStorageBuffer : public StorageBuffer
    {
    public:
        GLStorageBuffer(BufferManager * mag, NCount usize, NCount ucount, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);
        virtual ~GLStorageBuffer();
        
        void bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft = 0, NCount size = 0);

        virtual void bindBufferCS(Nui16 slot, NCount oft = 0, NCount size = 0);
        
        void setGLBufferBinding(GLint binding);
        
        inline GLint getGLBufferBinding() const     { return mBinding; }
    protected:
        virtual TextureBuffer * createViewImpl( PixelFormat pf );
        
        GLint mBinding;
    };
    
    /**
    */
    class _EngineGLAPI GLMappedBuffer : public MappedBuffer
    {
    public:
        GLMappedBuffer(GBufferManager * mag, NCount offset, NCount size, BufferModeMark mode, GLuint vboName );
        virtual ~GLMappedBuffer();
        
        inline GLuint getBufferName() const         { return mVboName; }

        virtual BlockType request(NCount sizeBytes, bool upload = true);

        void cleanUnfencedHazards();

        virtual NCount async(OperationList & oplist, bool upload);
    protected:
        /** Adds a fence to the region [from; to]. The region may actually be kept in
            mUnfencedHazards until the threshold is achieved or we're forced to fence
            it (i.e. when the internal buffer wraps around)
        @param forceFence
            When true, the region is merged with all the other unfenced hazards, and
            put behind a fence
        */
        void addFence(NCount from, NCount to, bool forceFence);

        void deleteFences(GLFenceVec::iterator itor, GLFenceVec::iterator end);

        /// Waits undefinitely on the given GLSync object. Can throw if failed to wait.
        void wait(GLsync syncObj);

        /// Checks all the fences and compares them against current state (i.e. mMappingStart &
        /// mMappingCount), and stalls if needed (synchronize); also book-keeps mFences and
        /// mUnfencedHazards.
        /// May modify mMappingStart.
        void waitIfNeeded(void);

        virtual void * writeMapImpl(NCount sizeBytes);

        virtual const void * readMapImpl(NCount offset, NCount sizeBytes);
        
        virtual void unmapImpl();
    protected:
        struct GLFence : Block
        {
            GLsync  fenceName;

            GLFence( NCount _start, NCount _end ) :
                Block( _start , _end ), fenceName( 0 )
            {
            }
        };
        typedef vector<GLFence>::type GLFenceVec;
        
        void * mMappedPtr;
        /// mVboName is not deleted by us (the GLGBufferManager does) as we may have
        /// only been assigned a chunk of the buffer, not the whole thing.
        GLuint  mVboName;
        GLFenceVec mFences;
        /// Regions of memory that were unmapped but haven't
        /// been fenced due to not passing the threshold yet.
        GLFenceVec mUnfencedHazards;
        /** How many bytes between the last fence and our current offset do we need to let
            through before we place another fence?
        @remarks
            When the ring buffer wraps around, a fence is always put.
            A threshold of zero means to put a fence after every unmap operation.
        */
        NCount mFenceThreshold;
        NCount mUnfencedBytes;
    };
    
    class _EngineGLAPI GLMappedTexture : public MappedTexture
    {
    public:
        GLMappedTexture( GLGBufferManager *mag, PixelFormat pf, NCount size, NCount oft, NCount vboPoolIdx, GLBuffer * buf );
        virtual ~GLMappedTexture();

        void _unmapBuffer(void);

        virtual void unmap(void);

        virtual void read(TextureGpu * dst, NCount dstmipmap, const PixelBlock * dstBox, const PixelBlock & box, bool host = false );
        
        virtual void write(TextureGpu * src, NCount srcmipmap, const PixelBlock * srcBox, const PixelBlock & box, bool host = false );

        inline GLBuffer * _getDynamicBuffer()       { return mBuffer; }

        inline void _resetDynamicBuffer()           { mBuffer = 0; }
    protected:
        virtual bool belongsToUs(const PixelBlock & box);

        void uploadCubemap(const PixelBlock &srcBox, PixelFormat pf, uint8 mipLevel, GLenum format, GLenum type,
            GLint xPos, GLint yPos, GLint slicePos, GLsizei width, GLsizei height, GLsizei mArray);
                            
        PixelBlock mapImpl(const PixelVolume & vol);
    protected:
        GLBuffer * mBuffer;
        size_t mUnmapTicket;
        void * mLastMappedPtr;
    };
}
#endif