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

#ifndef _NII_GLTEXTURE_H_
#define _NII_GLTEXTURE_H_

#include "NiiGLPreInclude.h"
#include "NiiGLTextureBase.h"

namespace NII
{
    class _EngineGLAPI GLTexture : public GLTextureBase
    {
    public:
        GLTexture(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs, GLRenderSystem * rsys);
        
        GLTexture( RecoverType rtype, GBufferManager * mag, IdString name, uint32 textureFlags,
            Texture::Type type, TextureManager * texmag );
            
        virtual ~GLTexture();

        virtual void setTextureType(Texture::Type type );
        
		/// GL的纹理类型 (1d/2d/3d/cube) 
		GLenum getGLTextureTarget() const;

        virtual void read(TextureGpu * dst, const PixelBlock & dstBox, uint8 dstMipLevel,
            const PixelBlock & srcBox, uint8 srcMipLevel, bool keepResolvedTexSynced = true );

        virtual void mipmap(void);

        virtual void getMultiSampleCoord(Vector2List & locations );
        
        void createShaderAccessPoint(uint bindPoint, TextureAccess access = TA_READ_WRITE,
            int mipmapLevel = 0, int textureArrayIndex = 0, PixelFormat format = PF_UNKNOWN);

        virtual void notifyGpuData(void);
        virtual bool isValidImpl(void) const;

        virtual void _setToDisplayDummyTexture(void);
        virtual void poolImpl( const TexturePool *newPool, uint16 slice );

        /// Returns the GLuid of the texture that is being displayed. While the texture is
        /// being loaded (i.e. data is not ready), we will display a dummy white texture.
        /// Once notifyGpuData, getDisplayTextureName should be the same as
        /// getFinalTextureName. In other words, getDisplayTextureName may change its
        /// returned value based on the texture's status
        inline GLuint getDisplayTextureName(void) const    { return mDisplayTextureName; }

        /// Always returns the internal handle that belongs to this texture.
        /// Note that for Texture::MM_POOLSLICE textures, this will be the
        /// handle of a 2D Array texture pool.
        ///
        /// If the texture has MSAA enabled, this returns the handle to the resolve
        /// texture, not the MSAA one.
        ///
        /// If Texture::MM_ManualMSAA is set, it returns the handle
        /// to the MSAA texture, since there is no resolve texture.
        inline GLuint getFinalTextureName(void) const      { return mFinalTextureName; }

        /// If MSAA > 1u and Texture::MM_ManualMSAA is not set, this
        /// returns the handle to the temporary MSAA renderbuffer used for rendering,
        /// which will later be resolved into the resolve texture.
        ///
        /// Otherwise it returns null.
        inline GLuint getMsaaFramebufferName(void) const   { return mMsaaFramebufferName; }

        /// Returns GL_TEXTURE_2D / GL_TEXTURE_2D_ARRAY / etc
        inline GLenum getGlTextureTarget(void) const       { return mGlTextureTarget; }

        void getPlatformData( IdString name, void *pData );
    protected:
		/// @copydoc Texture::createInternalImpl
		void createInternalImpl();

        /// @copydoc Texture::freeInternalImpl
        void freeInternalImpl();

        /// @copydoc Texture::createViewImpl
        TextureBuffer * createViewImpl(PixelFormat pf);

		/** internal method, create GLHardwarePixelBuffers for every face and
			 mipmap level. This method must be called after the GL texture object was created,
			the number of mipmaps was set (GL_TEXTURE_MAX_LEVEL) and glTexImageXD was called to
			actually allocate the buffer
		*/
		void _createSurfaceList();

        bool isRenderbuffer(void) const;

        void bindTextureToFrameBuffer( GLenum target, uint8 mipLevel, uint32 depthOrSlice );

        void bindTextureToFrameBuffer( GLenum target, GLuint textureName, uint8 mipLevel,
            uint32 depthOrSlice, bool bindMsaaColourRenderbuffer );

        void copyViaFramebuffer( TextureGpu *dst, const PixelBlock &dstBox, uint8 dstMipLevel,
            const PixelBlock &srcBox, uint8 srcMipLevel, bool keepResolvedTexSynced );
    protected:
        GLRenderSystem * mRenderSys;
        /// This will not be owned by us if isPoolSlice is true.
        /// It will also not be owned by us if we're not in StorageType::ST_Gpu
        /// This will always point to:
        ///     * A GL texture owned by us.
        ///     * A 4x4 dummy texture (now owned by us).
        ///     * A 64x64 mipmapped texture of us (but now owned by us).
        ///     * A GL texture not owned by us, but contains the final information.
        GLuint  mDisplayTextureName;
        GLenum  mGlTextureTarget;

        /// When we're transitioning to StorageType::ST_Gpu but we're not there yet,
        /// we will be either displaying a 4x4 dummy texture or a 64x64 one. However
        /// we reserve a spot to a final place will already be there for us once the
        /// texture data is fully uploaded. This variable contains that texture.
        /// Async upload operations should stack to this variable.
        /// May contain:
        ///     1. 0, if not resident or resident but not yet reached main thread.
        ///     2. The texture
        ///     3. An msaa texture (isManualMSAA == true)
        ///     4. The msaa resolved texture (isManualMSAA==false)
        /// This value may be a renderbuffer instead of a texture if isRenderbuffer() returns true.
        GLuint  mFinalTextureName;
        /// Only used when isManualMSAA() == false.
        /// This value is always an FBO.
        GLuint  mMsaaFramebufferName;
    };
    
    class _EngineGLAPI GL3PlusTextureGpuRenderTarget : public GLTexture
    {
    public:
        GL3PlusTextureGpuRenderTarget( RecoverType rtype, GBufferManager *vaoManager, 
            IdString name, uint32 textureFlags, Texture::Type initialType, TextureManager *textureManager );

        virtual void _setDepthBufferDefaults( uint16 depthBufferPoolId, bool preferDepthTexture,
                                              PixelFormat desiredDepthBufferFormat );
        virtual uint16 getDepthBufferPoolId(void) const;
        virtual bool getPreferDepthTexture(void) const;
        virtual PixelFormat getDesiredDepthBufferFormat(void) const;
    protected:
        uint16 mDSPool;
        bool mDSTexture;
        PixelFormat mDSFormat;

        virtual void createInternalImpl(void);
        virtual void freeInternalImpl(void);
    };
}
#endif