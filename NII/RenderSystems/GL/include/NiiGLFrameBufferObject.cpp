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

#include "OgreStableHeaders.h"

#include "OgreGL3PlusRenderPassDescriptor.h"

#include "OgreGL3PlusTextureGpu.h"
#include "OgreGL3PlusRenderSystem.h"

#include "OgreHlmsDatablock.h"
#include "OgrePixelFormatGpuUtils.h"

namespace Ogre
{
    GLFrameBufferObject::GLFrameBufferObject( GL3PlusRenderSystem * sys ) :
        mFboName( 0 ),
        mFboMsaaResolve( 0 ),
        mAllClearColoursSetAndIdentical( false ),
        mAnyColourLoadActionsSetToClear( false ),
        mHasRenderWindow( false ),
        mSharedFboItor( sys->_getFrameBufferDescMap().end() ),
        mRenderSys( sys )
    {
    }
    //-----------------------------------------------------------------------------------
    GLFrameBufferObject::~GLFrameBufferObject()
    {
        if( mFboMsaaResolve )
        {
            GLCheck( glDeleteFramebuffers( 1, &mFboMsaaResolve ) );
            mFboMsaaResolve = 0;
        }

        GL3PlusFrameBufferDescMap & frameBufferDescMap = mRenderSys->_getFrameBufferDescMap();
        if( mSharedFboItor != frameBufferDescMap.end() )
        {
            --mSharedFboItor->second.refCount;
            if( !mSharedFboItor->second.refCount )
            {
                GLCheck( glDeleteFramebuffers( 1, &mSharedFboItor->second.fboName ) );
                frameBufferDescMap.erase( mSharedFboItor );
            }
            mSharedFboItor = frameBufferDescMap.end();
            mFboName = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    void GLFrameBufferObject::checkRenderWindowStatus(void)
    {
        if( (mColourCount > 0 && mColour[0].texture->isWindow()) ||
            (mDepth.texture && mDepth.texture->isWindow()) ||
            (mStencil.texture && mStencil.texture->isWindow()) )
        {
            if( mColourCount > 1u )
            {
                N_EXCEPT( Exception::ERR_INVALIDPARAMS,
                             "Cannot use RenderWindow as MRT with other colour textures",
                             "GLFrameBufferObject::colourEntriesModified" );
            }

            if( (mColourCount > 0 && !mColour[0].texture->isWindow()) ||
                (mDepth.texture && !mDepth.texture->isWindow()) ||
                (mStencil.texture && !mStencil.texture->isWindow()) )
            {
                N_EXCEPT( Exception::ERR_INVALIDPARAMS,
                             "Cannot mix RenderWindow colour texture with depth or stencil buffer "
                             "that aren't for RenderWindows, or viceversa",
                             "GLFrameBufferObject::checkRenderWindowStatus" );
            }

            switchToRenderWindow();
        }
        else
        {
            switchToFBO();
        }
    }
    //-----------------------------------------------------------------------------------
    void GLFrameBufferObject::switchToRenderWindow(void)
    {
        GL3PlusFrameBufferDescMap & frameBufferDescMap = mRenderSys->_getFrameBufferDescMap();
        if( mSharedFboItor != frameBufferDescMap.end() )
        {
            --mSharedFboItor->second.refCount;
            if( !mSharedFboItor->second.refCount )
            {
                GLCheck( glDeleteFramebuffers( 1, &mSharedFboItor->second.fboName ) );
                frameBufferDescMap.erase( mSharedFboItor );
            }
            mSharedFboItor = frameBufferDescMap.end();
            mFboName = 0;
        }

        mHasRenderWindow = true;
    }
    //-----------------------------------------------------------------------------------
    void GLFrameBufferObject::switchToFBO(void)
    {
        FboKey key( *this );
        GL3PlusFrameBufferDescMap & frameBufferDescMap = mRenderSys->_getFrameBufferDescMap();
        GL3PlusFrameBufferDescMap::iterator newItor = frameBufferDescMap.find( key );

        if( newItor == frameBufferDescMap.end() )
        {
            GL3PlusFrameBufferDescValue value;
            value.refCount = 0;

            GLCheck( glGenFramebuffers( 1, &value.fboName ) );

            GLCheck( glBindFramebuffer( GL_FRAMEBUFFER, value.fboName ) );

            //Disable target independent rasterization to let the driver warn us
            //of wrong behavior during regular rendering.
            GLCheck( glFramebufferParameteri( GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, 0 ) );
            GLCheck( glFramebufferParameteri( GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, 0 ) );
            frameBufferDescMap[key] = value;
            newItor = frameBufferDescMap.find( key );
        }

        ++newItor->second.refCount;

        if( mSharedFboItor != frameBufferDescMap.end() )
        {
            --mSharedFboItor->second.refCount;
            if( !mSharedFboItor->second.refCount )
            {
                GLCheck( glDeleteFramebuffers( 1, &mSharedFboItor->second.fboName ) );
                frameBufferDescMap.erase( mSharedFboItor );
            }
        }

        mSharedFboItor = newItor;
        mFboName = mSharedFboItor->second.fboName;
        mHasRenderWindow = false;
    }
    //-----------------------------------------------------------------------------------
    void GLFrameBufferObject::analyzeClearColour(void)
    {
        if( !mColourCount )
        {
            mAllClearColoursSetAndIdentical = false;
            mAnyColourLoadActionsSetToClear = false;
            return;
        }

        const RenderFeature *capabilities = mRenderSys->getFeature();
        const bool isTiler = capabilities->hasCapability( RSC_IS_TILER );

        mAllClearColoursSetAndIdentical = true;
        Colour lastClearColour = mColour[0].clearColour;

        for( size_t i=0u; i<mColourCount; ++i )
        {
            bool performsClear = mColour[i].mInitType == FrameTarget::IT_Clear ||
                                 (isTiler && (mColour[i].mInitType == FrameTarget::IT_None ||
                                              mColour[i].mInitType == FrameTarget::IT_SubClear ));
            if( performsClear )
                mAnyColourLoadActionsSetToClear = true;
            if( !performsClear || lastClearColour != mColour[i].clearColour )
                mAllClearColoursSetAndIdentical = false;
        }
    }
    //-----------------------------------------------------------------------------------
    void GLFrameBufferObject::updateColourFbo( uint8 lastNumColourEntries )
    {
        if( mColourCount < lastNumColourEntries && !mHasRenderWindow )
        {
            for( size_t i=mColourCount; i<lastNumColourEntries; ++i )
            {
                //Detach removed colour entries
                GLCheck( glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                                 GL_RENDERBUFFER, 0 ) );
            }
        }

        if( !mHasRenderWindow )
        {
            GLCheck( glFramebufferParameteri( GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, 0 ) );
            GLCheck( glFramebufferParameteri( GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, 0 ) );
            GLCheck( glFramebufferParameteri( GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_SAMPLES, 0 ) );
        }

        bool needsMsaaResolveFbo = false;

        //Attach colour entries
        for( size_t i=0; i<mColourCount; ++i )
        {
            if( mColour[i].texture->getCurrentStorage() != StorageType::ST_Gpu )
            {
                N_EXCEPT( Exception::ERR_INVALIDPARAMS, "RenderTexture '" + mColour[i].texture->getName() + "' must be resident!");
            }

            if( !mHasRenderWindow && mColour[i].texture->getFormat() != PFG_NULL )
            {
                assert( dynamic_cast<GL3PlusTextureGpu*>( mColour[i].texture ) );
                GL3PlusTextureGpu *texture = static_cast<GL3PlusTextureGpu*>( mColour[i].texture );

                if( texture->isWindow() )
                {
                    N_EXCEPT( Exception::ERR_INVALIDPARAMS, "Cannot use RenderWindow as MRT with other colour textures");
                }

                Texture::Texture::Type textureType = mColour[i].texture->getTextureType();
                const bool hasLayers = textureType != Texture::Type1D && textureType != Texture::Type2D;

                if( mColour[i].mAllArray || !hasLayers )
                {
                    if( texture->isMultiSample() && (!texture->isManualMSAA() || !texture->isTexture()) )
                    {
                        GLCheck( glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER,
                                                         texture->getMsaaFramebufferName() ) );
                    }
                    else
                    {
                        GLCheck( glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                                    texture->getFinalTextureName(),
                                                    mColour[i].mMip ) );
                    }
                }
                else
                {
                    if( texture->isMultiSample() && (!texture->isManualMSAA() || !texture->isTexture()) )
                    {
                        GLCheck( glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, texture->getMsaaFramebufferName() ) );
                    }
                    else
                    {
                        GLCheck( glFramebufferTextureLayer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, texture->getFinalTextureName(),
                            mColour[i].mMip, mColour[i].mArrayIndex ) );
                    }
                }
            }
            else if( mColour[i].texture->getFormat() == PFG_NULL )
            {
                GLCheck( glFramebufferParameteri( GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, mColour[i].texture->getWidth() ) );
                GLCheck( glFramebufferParameteri( GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, mColour[i].texture->getHeight() ) );
                GLCheck( glFramebufferParameteri(
                    GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_SAMPLES,
                    mColour[i].texture->isMultiSample()
                        ? mColour[i].texture->getOriginSample().getMultiSampling()
                        : 0 ) );
            }

            if( (mColour[i].mStoreType == FrameTarget::ST_Multisample ||
                 mColour[i].mStoreType == FrameTarget::ST_MultisampleStore) &&
                mColour[i].mTarget )
            {
                needsMsaaResolveFbo = true;
            }
        }

        analyzeClearColour();

        if( needsMsaaResolveFbo && !mFboMsaaResolve )
        {
            GLCheck( glGenFramebuffers( 1, &mFboMsaaResolve ) );
        }
        else if( !needsMsaaResolveFbo && mFboMsaaResolve )
        {
            GLCheck( glDeleteFramebuffers( 1, &mFboMsaaResolve ) );
            mFboMsaaResolve = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    void GLFrameBufferObject::updateDepthFbo(void)
    {
        if( mHasRenderWindow )
            return;

        if( !mDepth.texture )
        {
            GLCheck( glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0 ) );
            return;
        }

        if( mDepth.texture->getCurrentStorage() != StorageType::ST_Gpu )
        {
            N_EXCEPT( Exception::ERR_INVALIDPARAMS, "RenderTexture '" +
                         mDepth.texture->getName() + "' must be resident!",
                         "GLFrameBufferObject::updateDepthFbo" );
        }

        assert( dynamic_cast<GL3PlusTextureGpu*>( mDepth.texture ) );
        GL3PlusTextureGpu *texture = static_cast<GL3PlusTextureGpu*>( mDepth.texture );

        if( texture->isTexture() )
        {
            GLCheck( glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture->getFinalTextureName(), 0 ) );
        }
        else
        {
            GLCheck( glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, texture->getFinalTextureName() ) );
        }
    }
    //-----------------------------------------------------------------------------------
    void GLFrameBufferObject::updateStencilFbo(void)
    {
        if( mHasRenderWindow )
            return;

        if( !mStencil.texture )
        {
            GLCheck( glFramebufferTexture( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, 0, 0 ) );
            return;
        }

        if( mStencil.texture->getCurrentStorage() != StorageType::ST_Gpu )
        {
            N_EXCEPT( Exception::ERR_INVALIDPARAMS, "RenderTexture '" +
                         mStencil.texture->getName() + "' must be resident!",
                         "GLFrameBufferObject::updateStencilFbo" );
        }

        assert( dynamic_cast<GL3PlusTextureGpu*>( mStencil.texture ) );
        GL3PlusTextureGpu *texture = static_cast<GL3PlusTextureGpu*>( mStencil.texture );

        if( texture->isTexture() )
        {
            GLCheck( glFramebufferTexture( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, texture->getFinalTextureName(), 0 ) );
        }
        else
        {
            GLCheck( glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, texture->getFinalTextureName() ) );
        }
    }
    //-----------------------------------------------------------------------------------
    void GLFrameBufferObject::syncType( uint32 entryTypes )
    {
        uint8 lastNumColourEntries = mColourCount;
        FrameBufferObject::syncType( entryTypes );

        checkRenderWindowStatus();

        GLCheck( glBindFramebuffer( GL_FRAMEBUFFER, mFboName ) );

        if( entryTypes & FrameBufferObject::TT_Colour )
            updateColourFbo( lastNumColourEntries );

        if( entryTypes & FrameBufferObject::TT_Depth )
            updateDepthFbo();

        if( entryTypes & FrameBufferObject::TT_Stencil )
            updateStencilFbo();
    }
    //-----------------------------------------------------------------------------------
    void GLFrameBufferObject::setClearColour( uint8 idx, const Colour &clearColour )
    {
        FrameBufferObject::setClearColour( idx, clearColour );
        analyzeClearColour();
    }
    //-----------------------------------------------------------------------------------
    void GLFrameBufferObject::setClearColour( const Colour &clearColour )
    {
        for( uint8 i=0; i<mColourCount; ++i )
            FrameBufferObject::setClearColour( i, clearColour );

        analyzeClearColour();
    }
    //-----------------------------------------------------------------------------------
    uint32 GLFrameBufferObject::checkForClearActions( GLFrameBufferObject *other ) const
    {
        uint32 entriesToFlush = 0;

        assert( this->mFboName == other->mFboName );
        assert( this->mColourCount == other->mColourCount );

        const RenderFeature *capabilities = mRenderSys->getFeature();
        const bool isTiler = capabilities->hasCapability( RSC_IS_TILER );

        for( size_t i=0; i<mColourCount; ++i )
        {
            //this->mColour[i].mAllArray doesn't need to be analyzed
            //because it requires a different FBO.
            if( other->mColour[i].mInitType == FrameTarget::IT_Clear ||
                (isTiler && mColour[i].mInitType == FrameTarget::IT_SubClear) )
            {
                entriesToFlush |= FrameBufferObject::TT_Colour0 << i;
            }
        }

        if( other->mDepth.mInitType == FrameTarget::IT_Clear ||
            (isTiler && mDepth.mInitType == FrameTarget::IT_SubClear) )
        {
            entriesToFlush |= FrameBufferObject::TT_Depth;
        }

        if( other->mStencil.mInitType == FrameTarget::IT_Clear ||
            (isTiler && mStencil.mInitType == FrameTarget::IT_SubClear) )
        {
            entriesToFlush |= FrameBufferObject::TT_Stencil;
        }

        return entriesToFlush;
    }
    //-----------------------------------------------------------------------------------
    uint32 GLFrameBufferObject::willSwitchTo( GLFrameBufferObject *newDesc,
                                                      bool warnIfRtvWasFlushed ) const
    {
        uint32 entriesToFlush = 0;

        if( !newDesc ||
            this->mFboName != newDesc->mFboName ||
            this->mStepping || newDesc->mStepping )
        {
            entriesToFlush = FrameBufferObject::TT_All;
        }
        else
            entriesToFlush |= checkForClearActions( newDesc );

        if( warnIfRtvWasFlushed )
            newDesc->checkWarnIfRtvWasFlushed( entriesToFlush );

        return entriesToFlush;
    }
    //-----------------------------------------------------------------------------------
    void GLFrameBufferObject::performLoadActions( uint8 blendChannelMask,
                                                          bool depthWrite,
                                                          uint32 stencilWriteMask,
                                                          uint32 entriesToFlush )
    {
        if( mStepping )
            return;

        GLCheck( glBindFramebuffer( GL_FRAMEBUFFER, mFboName ) );

        if( mHasRenderWindow )
        {
            if( !mColourCount )
            {
                //Do not render to colour Render Windows.
                GLCheck( glDrawBuffer( GL_NONE ) );
            }
            else
            {
                //Make sure colour writes are enabled for RenderWindows.
                GLCheck( glDrawBuffer( GL_BACK ) );
            }
        }
        else
        {
            GLenum colourBuffs[NII_MAX_RenderTarget];
            for( int i=0; i<mColourCount; ++i )
                colourBuffs[i] = GL_COLOR_ATTACHMENT0 + i;
            GLCheck( glDrawBuffers( mColourCount, colourBuffs ) );
        }

        GLCheck( glEnable( GL_FRAMEBUFFER_SRGB ) );

        const RenderFeature *capabilities = mRenderSys->getFeature();
        const bool isTiler = capabilities->hasCapability( RSC_IS_TILER );

        bool colourMask = blendChannelMask != ShaderChBlend::BlendChannelAll;

        GLbitfield flags = 0;

        uint32 fullColourMask = (1u << mColourCount) - 1u;
        if( entriesToFlush & FrameBufferObject::TT_Colour )
        {
            if( mColourCount > 0 && mAllClearColoursSetAndIdentical &&
                (entriesToFlush & fullColourMask) == fullColourMask )
            {
                //All at the same time
                flags |= GL_COLOR_BUFFER_BIT;

                if( colourMask )
                {
                    //Enable buffer for writing if it isn't
                    GLCheck( glColorMask( true, true, true, true ) );
                }
                const Colour &clearColour = mColour[0].clearColour;
                GLCheck( glClearColor( clearColour.r, clearColour.g, clearColour.b, clearColour.a ) );
            }
            else if( mColourCount > 0 && mAnyColourLoadActionsSetToClear )
            {
                //Clear one at a time
                if( colourMask )
                {
                    //Enable buffer for writing if it isn't
                    GLCheck( glColorMask( true, true, true, true ) );
                }

                GLCheck( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, mFboName ) );

                for( size_t i=0; i<mColourCount; ++i )
                {
                    if( (entriesToFlush & (FrameBufferObject::TT_Colour0 << i)) &&
                        (mColour[i].mInitType == FrameTarget::IT_Clear ||
                         (isTiler && (mColour[i].mInitType == FrameTarget::IT_None ||
                                      mColour[i].mInitType == FrameTarget::IT_SubClear))) )
                    {
                        GLfloat clearColour[4];
                        clearColour[0] = mColour[i].clearColour.r;
                        clearColour[1] = mColour[i].clearColour.g;
                        clearColour[2] = mColour[i].clearColour.b;
                        clearColour[3] = mColour[i].clearColour.a;
                        GLCheck( glClearBufferfv( GL_COLOR, i, clearColour ) );
                    }
                }
            }
        }

        if( (entriesToFlush & FrameBufferObject::TT_Depth) &&
            mDepth.texture &&
            (mDepth.mInitType == FrameTarget::IT_Clear ||
             (isTiler && (mDepth.mInitType == FrameTarget::IT_None ||
                          mDepth.mInitType == FrameTarget::IT_SubClear))) )
        {
            flags |= GL_DEPTH_BUFFER_BIT;
            if( !depthWrite )
            {
                //Enable buffer for writing if it isn't
                GLCheck( glDepthMask( GL_TRUE ) );
            }

            if( !mRenderSys->isReverseDepth() )
            {
                GLCheck( glClearDepth( mDepth.clearDepth ) );
            }
            else
            {
                GLCheck( glClearDepth( Real(1.0f) - mDepth.clearDepth ) );
            }
        }

        if( (entriesToFlush & FrameBufferObject::TT_Stencil) &&
            mStencil.texture &&
            (mStencil.mInitType == FrameTarget::IT_Clear ||
             (isTiler && (mStencil.mInitType == FrameTarget::IT_None ||
                          mStencil.mInitType == FrameTarget::IT_SubClear))) )
        {
            flags |= GL_STENCIL_BUFFER_BIT;
            if( stencilWriteMask != 0xFFFFFFFF )
            {
                //Enable buffer for writing if it isn't
                GLCheck( glStencilMask( 0xFFFFFFFF ) );
            }
            GLCheck( glClearStencil( mStencil.clearStencil ) );
        }

        if( flags != 0 )
        {
            //Issue the clear (except for glClearBufferfv which was immediate)
            GLCheck( glClear( flags ) );
        }

        //Restore state
        if( (entriesToFlush & fullColourMask) == fullColourMask &&
            colourMask && mColourCount > 0 &&
            (mAllClearColoursSetAndIdentical || mAnyColourLoadActionsSetToClear) )
        {
            GLboolean r = (blendChannelMask & ShaderChBlend::BlendChannelRed) != 0;
            GLboolean g = (blendChannelMask & ShaderChBlend::BlendChannelGreen) != 0;
            GLboolean b = (blendChannelMask & ShaderChBlend::BlendChannelBlue) != 0;
            GLboolean a = (blendChannelMask & ShaderChBlend::BlendChannelAlpha) != 0;
            GLCheck( glColorMask( r, g, b, a ) );
        }

        if( !depthWrite && (flags & GL_DEPTH_BUFFER_BIT) )
        {
            GLCheck( glDepthMask( GL_FALSE ) );
        }

        if( stencilWriteMask != 0xFFFFFFFF && (flags & GL_STENCIL_BUFFER_BIT) )
        {
            GLCheck( glStencilMask( stencilWriteMask ) );
        }
    }
    //-----------------------------------------------------------------------------------
    void GLFrameBufferObject::performStoreActions( bool hasArbInvalidateSubdata,
                                                           uint32 entriesToFlush )
    {
        if( mStepping )
            return;

        GLsizei numAttachments = 0;
        GLenum attachments[NII_MAX_RenderTarget+2u];

        bool unbindReadDrawFramebuffers = false;

        GLenum invalidateTarget = GL_DRAW_FRAMEBUFFER;

        if( entriesToFlush & FrameBufferObject::TT_Colour && !mHasRenderWindow )
        {
            for( size_t i=0; i<mColourCount; ++i )
            {
                if( entriesToFlush & (FrameBufferObject::TT_Colour0 << i) )
                {
                    if( (mColour[i].mStoreType == FrameTarget::ST_Multisample ||
                         mColour[i].mStoreType == FrameTarget::ST_MultisampleStore) &&
                        mColour[i].mTarget )
                    {
                        assert( mColour[i].mTarget->getCurrentStorage() == StorageType::ST_Gpu );
                        assert( dynamic_cast<GL3PlusTextureGpu*>( mColour[i].mTarget ) );
                        GL3PlusTextureGpu *resolveTexture = static_cast<GL3PlusTextureGpu*>( mColour[i].mTarget );

                        const Texture::Texture::Type resolveTextureType = mColour[i].mTarget->getTextureType();
                        const bool hasLayers = resolveTextureType != Texture::Type1D && resolveTextureType != Texture::Type2D;

                        invalidateTarget = GL_READ_FRAMEBUFFER;

                        // Blit from multisample buffer to final buffer, triggers resolve
                        GLCheck( glBindFramebuffer( GL_READ_FRAMEBUFFER, mFboName ) );
                        GLCheck( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, mFboMsaaResolve ) );
                        if( !hasLayers )
                        {
                            GLCheck( glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                                        resolveTexture->getFinalTextureName(),
                                                        mColour[i].mMip ) );
                        }
                        else
                        {
                            GLCheck( glFramebufferTextureLayer( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                                             resolveTexture->getFinalTextureName(),
                                                             mColour[i].mTargetMip,
                                                             mColour[i].mTargetArrayIndex ) );
                        }

                        const uint32 width  = resolveTexture->getWidth();
                        const uint32 height = resolveTexture->getHeight();

                        GLCheck( glReadBuffer( GL_COLOR_ATTACHMENT0 + i ) );
                        GLCheck( glDrawBuffer( GL_COLOR_ATTACHMENT0 + 0 ) );
                        GLCheck( glBlitFramebuffer( 0, 0, width, height, 0, 0, width, height,
                                                 GL_COLOR_BUFFER_BIT, GL_NEAREST ) );

                        if( mColour[i].mStoreType == FrameTarget::ST_Multisample )
                        {
                            attachments[numAttachments] = mHasRenderWindow ? GL_COLOR :
                                                                             GL_COLOR_ATTACHMENT0 + i;
                        }

                        unbindReadDrawFramebuffers = true;
                    }

                    if( mColour[i].mStoreType == FrameTarget::ST_None ||
                        mColour[i].mStoreType == FrameTarget::ST_Multisample )
                    {
                        attachments[numAttachments] = GL_COLOR_ATTACHMENT0 + i;
                        ++numAttachments;
                    }
                }
            }
        }

        if( (entriesToFlush & FrameBufferObject::TT_Depth) &&
            mDepth.texture &&
            mDepth.mStoreType == FrameTarget::ST_None )
        {
            attachments[numAttachments] = mHasRenderWindow ? GL_DEPTH : GL_DEPTH_ATTACHMENT;
            ++numAttachments;
        }

        if( (entriesToFlush & FrameBufferObject::TT_Stencil) &&
            mStencil.texture &&
            mStencil.mStoreType == FrameTarget::ST_None )
        {
            attachments[numAttachments] = mHasRenderWindow ? GL_STENCIL : GL_STENCIL_ATTACHMENT;
            ++numAttachments;
        }

        if( numAttachments > 0 && hasArbInvalidateSubdata )
        {
            GLCheck( glInvalidateFramebuffer( invalidateTarget, numAttachments, attachments ) );
        }

        if( unbindReadDrawFramebuffers )
        {
            GLCheck( glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 ) );
            GLCheck( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 ) );
        }
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GL3PlusFrameBufferDescValue::GL3PlusFrameBufferDescValue() : fboName( 0 ), refCount( 0 ) {}
}
