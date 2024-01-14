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

#ifndef _NII_GLUTIL_H_
#define _NII_GLUTIL_H_

#include "NiiGLPreInclude.h"
#include "NiiExtFrameBuffer.h"
#include "NiiGpuBuffer.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiPixelFormat.h"
#include "NiiTexture.h"
#include "NiiFrameObj.h"
#include "NiiBitwise.h"

namespace NII 
{
    class GLContext;
    class GLSLProgramBase;
    class GLRenderSystemBase;
    class GLFrameBufferObjectBase;
    
    /** Specialisation of VertexDeclaration for OpenGL Vertex Array Object usage */
    class GLVertexArrayObject
    {
    public:
        GLVertexArrayObject();
        ~GLVertexArrayObject();
        
        void notifyContextDestroyed(GLContext* ctx)             { if(mCreatorContext == ctx) { mCreatorContext = 0; mVAO = 0; } }
        void bind(GLRenderSystemBase * rs) const;
        //bool needsUpdate(VertexData * bind, size_t vertexOft);
        //void bindToGpu(GLRenderSystemBase * rs, VertexData * bind, size_t vertexOft);
    protected:
        void notifyChanged()                                    { mNeedsUpdate = true; }
        
        std::vector<std::pair<NIIi, VertexBuffer *> > mAttribsBound;
        std::vector<Nui32> mInstanceAttribsBound;
        
        /// Context that was used to create VAO. It could already be destroyed, so do not dereference this field blindly
        mutable GLContext* mCreatorContext;
        NCount mVertexStart;
        mutable Nui32 mVAO;
        mutable bool mNeedsUpdate;
    };

    class _EngineGLAPI GLRenderTarget
    {
    public:
        virtual ~GLRenderTarget() {}

        virtual GLContext * getContext() const = 0;

        virtual GLFrameBufferObjectBase * getFBO() const { return NULL; }
    };
    
    /** 离屏渲染上下文
    @version NIIEngine 3.0.0
    */
    class _EngineGLAPI GLPBuffer : public GLRenderTarget
    {
    public:
        GLPBuffer(PixelDataType format, NCount w, NCount h): mFormat(format), mWidth(w), mHeight(h){}
        virtual ~GLPBuffer() {}
        
        /** 像素格式
        @version NIIEngine 3.0.0
        */
        inline PixelDataType getFormat()       { return mFormat; }

        /** 宽
        @version NIIEngine 3.0.0
        */
        inline NCount getWidth()               { return mWidth; }

        /** 高
        @version NIIEngine 3.0.0
        */
        inline NCount getHeight()              { return mHeight; }
    protected:
        PixelDataType mFormat;
        NCount mWidth, mHeight;
    };
    
    class _EngineGLAPI GLTextureBase  : public Texture
    {
    public:
        GLTextureBase(ResourceID rid, GroupID gid, ResLoadScheme * ls = 0, ResResultScheme * rs = 0, LangID lid = N_PrimaryLang) : 
            Texture(rid, gid, ls, rs, lid), 
            mTextureID(0){}

        inline Nui32 getGLID() const                    { return mTextureID; }

        /// @copydetails RenderSysData::getPlatformData
        bool getPlatformData(const String & name, void * data) const;
    protected:
        /** Returns the maximum number of Mipmaps that can be generated until we reach
            the mininum format possible. This does not count the base level.

            @return how many times we can divide this texture in 2 until we reach 1x1.
        */
        inline Nui32 getMaxMipmaps() const {        // see ARB_texture_non_power_of_two
            return Bitwise::mostSignificantBitSet(std::max(mWidth, std::max(mHeight, mDepth)));}
    protected:
        Nui32 mTextureID;
    };
    
	/**
		@copydoc ExtFrameBuffer

		OpenGL supports 3 different methods: FBO, pbuffer & Copy.
		Each one has it's own limitations. Non-FBO methods are solved using "dummy" DepthBuffers.
		That is, a ExtFrameBuffer pointer is attached to the FrameObj (for the sake of consistency)
		but it doesn't actually contain a Depth surface/renderbuffer (mDepthBuffer & mStencilBuffer are
		null pointers all the time) Those dummy DepthBuffers are identified thanks to their GL context.
		Note that FBOs don't allow sharing with the main window's depth buffer. Therefore even
		when FBO is enabled, a dummy ExtFrameBuffer is still used to manage the windows.
	*/
	class GLDepthBufferBase : public ExtFrameBuffer
	{
	public:
        GLDepthBufferBase(GroupID poolId, GLRenderSystemBase * rsys, GLContext * glcontext, 
            GLFrameBufferBase * depth, GLFrameBufferBase * stencil, 
                NCount width, NCount height, Nui32 fsaa,  bool isManual);
		~GLDepthBufferBase();

		/// @copydoc ExtFrameBuffer::isSuitable
		virtual bool isSuitable(FrameObj * fo) const;

		inline GLContext * getGLContext() const                 { return mCreatorContext; }
        inline GLFrameBufferBase * getDepthBuffer() const       { return mDepthBuffer; }
        inline GLFrameBufferBase * getStencilBuffer() const     { return mStencilBuffer; }
	protected:
		GLContext * mCreatorContext;
        GLFrameBufferBase * mDepthBuffer;
        GLFrameBufferBase * mStencilBuffer;
        GLRenderSystemBase * mRenderSys;
	};
    
	class _EngineGLAPI GLFrameBufferBase : public FrameBuffer
	{
	public:
        GLFrameBufferBase(NCount w, NCount h, NCount d, PixelFormat pf, Nmark mode);
	    virtual ~GLFrameBufferBase();

		/// 获取GL格式
		Nui32 getGLFormat() { return mGLInternalFormat; }

		/// 绑定面到帧缓存中,需要FBO扩展.
		virtual void bindToFramebuffer(Nui32 attachment, NCount zoft);
	protected:
		/// @copydetails FrameBuffer::lockImpl
		PixelBlock lockImpl(Nmark m, const Box & src);

		/// @copydetails Buffer::unlockImpl
		void unlockImpl();

        /// @copydetails Buffer::clone
        Buffer * clone(Nmark m) const;

		/// 分配像素缓存
		void allocateBuffer();

		/// 释放像素缓存
		void freeBuffer();

		/// 上传指定像素块盒到这个缓存中
		virtual void upload(const PixelBlock & data, const Box & dest);

		/// 从显示卡中下载像素盒
		virtual void download(const PixelBlock & data);
	protected:
		PixelBlock mBuffer;
        Nui32 mGLInternalFormat;
		Nmark mCurrentLockOptions;
	};
    
    /** An in memory cache of the OpenGL uniforms. */
    class _EngineGLAPI GLUniformCache : public GPUAlloc
    {
    public:
        /** Clears all cached values
        */
        void clearCache();

        /** Update a uniform
         @return A boolean value indicating whether this uniform needs to be updated in the GL.
         */
        bool updateUniform(NIIi location, const void * value, NCount length);
    protected:
        typedef std::unordered_map<NIIi, Nui32> UniformMap;

        /// A map of uniform names and a hash of their values
        UniformMap mUniformValueMap;
    };
}

#endif