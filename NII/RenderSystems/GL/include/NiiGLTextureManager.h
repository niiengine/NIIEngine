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

#ifndef _NII_GLTextureManager_H_
#define _NII_GLTextureManager_H_

#include "NiiGLPreInclude.h"
#include "NiiTextureManager.h"
#include "NiiGLTexture.h"

namespace NII
{
    class _EngineGLAPI GL3PlusSampler : public TextureSampleUnit
    {
    public:
        GL3PlusSampler(GL3PlusRenderSystem* rs);
        ~GL3PlusSampler();
        void bind(uint32 unit);
        static GLint getCombinedMinMipFilter(TextureFilterOP min, TextureFilterOP mip);
        static GLint getTextureAddressingMode(TextureAddressingMode tam);
    private:
        uint32 mSamplerId;
    };
    
    /// TextureManager的GL特定实现
    class _EngineGLAPI GLTextureManager : public TextureManager
    {
        friend class GLRenderSystem;
    public:
        GLTextureManager(GBufferManager * mag, GLRenderSystem * rsys, const GL3PlusSupport &support );
        virtual ~GLTextureManager();

        /** Creates a special GL3PlusTextureGpuWindow pointer, to be used by Ogre::Window.
            The pointer can be freed by a regular N_delete. We do not track this pointer.
            If caller doesnt' delete it, it will leak.
        */
        TextureGpu * createTextureGpuWindow( GL3PlusContext * context, Window * window );

        GLuint getBlankTextureGlName(Texture::Type type) const;

        /// fboIdx must be in range [0; 1]
        GLuint getTemporaryFbo( uint32 fboIdx ) const           { return mTmpFbo[fboIdx]; }

        const GL3PlusSupport & getGlSupport(void) const         { return mSupport; }

		/// @copydetails TextureManager::getFormat
		PixelFormat getFormat(Texture::Type type, PixelFormat pf, Nmark mark);

        /// @copydetails TextureManager::isSupport
        bool isSupport(Texture::Type type, PixelFormat pf, Nmark usage, bool preciseFormatOnly = false);
        
        /// @copydoc TextureManager::getNativeFormat
        PixelFormat getNativeFormat(TextureType ttype, PixelFormat format, int usage);
    protected:
        /// @copydoc ResourceManager::createImpl
		Resource * createImpl(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs, const PropertyData * params);

        virtual TextureGpu * createTextureImpl( RecoverType rtype, IdString name, uint32 textureFlags, Texture::Type initialType);
                                               
        virtual MappedTexture * createMappedImpl(const PixelVolume & vol );
                                                          
        virtual void destroyMappedImpl(MappedTexture * stagingTexture );

        virtual TextureAsync * createAsyncImpl (uint32 width, uint32 height, uint32 depthOrSlices,
            Texture::Type textureType, PixelFormat familypf );
    protected:
        /// 4x4 texture for when we have nothing to display.
        GLuint  mBlankTexture[Texture::T_3D + 1u];
        GLuint  mTmpFbo[2];

        const GL3PlusSupport &mSupport;
    };
}
#endif