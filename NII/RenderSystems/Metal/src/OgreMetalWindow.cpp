/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2016 Torus Knot Software Ltd

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

#include "OgreMetalWindow.h"
#include "OgreMetalMappings.h"
#include "OgreMetalTextureGpuWindow.h"
#include "OgreMetalTextureGpuManager.h"
#include "OgreMetalDevice.h"
#include "OgrePixelFormatGpuUtils.h"
#include "OgreRenderSystem.h"
#include "OgreDepthBuffer.h"
#include "OgreViewport.h"
#include "OgreStringConverter.h"

namespace Ogre
{
    MetalWindow::MetalWindow( const String &title, uint32 width, uint32 height, bool fullscreenMode,
                              const PropertyData *miscParams, MetalDevice *ownerDevice ) :
        Window( title, width, height, fullscreenMode ),
        mClosed( false ),
        mHwGamma( true ),
        mMetalLayer( 0 ),
        mCurrentDrawable( 0 ),
        mDevice( ownerDevice )
    {
        create( fullscreenMode, miscParams );
    }
    //-------------------------------------------------------------------------
    MetalWindow::~MetalWindow()
    {
        destroy();
    }
    //-------------------------------------------------------------------------
    float MetalWindow::getViewRatio() const
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        return mMetalLayer.contentsScale;
#else
        NSScreen* screen = mMetalView.window.screen ?: [NSScreen mainScreen];
        return screen.backingScaleFactor;
#endif
    }
    //-------------------------------------------------------------------------
    inline void MetalWindow::checkLayerSizeChanges(void)
    {
        // Handle display changes here
        if( mMetalView.layerSizeDidUpdate )
        {
            // Resize anything if needed
            this->notify();

            mMetalView.layerSizeDidUpdate = NO;
        }
    }
    //-------------------------------------------------------------------------
    void MetalWindow::setResolutionFromView(void)
    {
        // update drawable size to match layer and thus view size
        float scale = getViewRatio();
        CGSize sizePt = mMetalLayer.frame.size;
        const uint32 widthPx = std::max(1u, (uint32)floor(sizePt.width * scale + 0.5));
        const uint32 heightPx = std::max(1u, (uint32)floor(sizePt.height * scale + 0.5));
        mMetalLayer.drawableSize = CGSizeMake( widthPx, heightPx );

        if( mTexture )
        {
            N_assert( dynamic_cast<MetalTextureGpuWindow*>( mTexture ) );
            MetalTextureGpuWindow *texWindow = static_cast<MetalTextureGpuWindow*>( mTexture );
            texWindow->_setMsaaBackbuffer( 0 );

            if( isMultiSample() )
            {
                MTLTextureDescriptor* desc = [MTLTextureDescriptor
                                             texture2DDescriptorWithPixelFormat:
                                             MetalMappings::get( mTexture->getFormat() )
                                             width: widthPx height: heightPx mipmapped: NO];
                desc.textureType = MTLTextureType2DMultisample;
                desc.sampleCount = mSrcSample.getMultiSampling();
                desc.usage       = MTLTextureUsageRenderTarget;
                desc.storageMode = MTLStorageModePrivate;

                id<MTLTexture> msaaTex = [mDevice->mDevice newTextureWithDescriptor: desc];
                if( !msaaTex )
                {
                    N_EXCEPT( RenderingAPI,
                                 "Out of GPU memory or driver refused.",
                                 "MetalWindow::setResolutionFromView" );
                }
                texWindow->_setMsaaBackbuffer( msaaTex );
            }

            bool wasResident = false;
            if( mDepthBuffer && mDepthBuffer->getResidencyStatus() != StorageType::ST_Disk )
            {
                mDepthBuffer->load( StorageType::ST_Disk, (uint8*)0 );
                wasResident = true;
            }

            setSize( widthPx, heightPx );

            if( mDepthBuffer && wasResident )
                mDepthBuffer->load( StorageType::Resident, (uint8*)0 );
        }
    }
    //-------------------------------------------------------------------------
    void MetalWindow::swap(void)
    {
        if( !mDevice->mFrameAborted )
        {
            // Schedule a present once rendering to the framebuffer is complete
            const CFTimeInterval presentationTime = mMetalView.presentationTime;

            if( presentationTime < 0 )
            {
                [mDevice->mCurrentCommandBuffer presentDrawable:mCurrentDrawable];
            }
            else
            {
                [mDevice->mCurrentCommandBuffer presentDrawable:mCurrentDrawable
                                                         atTime:presentationTime];
            }
        }
        mCurrentDrawable = 0;
    }
    //-------------------------------------------------------------------------
    void MetalWindow::notify(void)
    {
        CGSize sizePt = mMetalLayer.frame.size;
        if( mViewWidth != sizePt.width || mViewHeight != sizePt.height )
        {
            mViewWidth  = sizePt.width;
            mViewHeight = sizePt.height;

            setResolutionFromView();
        }
    }
    //-------------------------------------------------------------------------
    bool MetalWindow::nextDrawable(void)
    {
        bool isSuccess = true;

        @autoreleasepool
        {
            if( !mCurrentDrawable )
            {
                if( mMetalView.layerSizeDidUpdate )
                    checkLayerSizeChanges();
    
                // do not retain current drawable beyond the frame.
                // There should be no strong references to this object outside of this view class
                mCurrentDrawable = [mMetalLayer nextDrawable];
                if( !mCurrentDrawable )
                {
                    LogManager::getSingleton().logMessage( "Metal ERROR: Failed to get a drawable!",
                                                           LML_CRITICAL );
                    //We're unable to render. Skip frame.
                    //dispatch_semaphore_signal( _inflight_semaphore );
    
                    mDevice->mFrameAborted |= true;
                    isSuccess = false;
                }
                else
                {
                    N_assert( dynamic_cast<MetalTextureGpuWindow*>( mTexture ) );
                    MetalTextureGpuWindow *texWindow = static_cast<MetalTextureGpuWindow*>( mTexture );

                    texWindow->_setBackbuffer( mCurrentDrawable.texture );
                }
            }
        }

        return isSuccess;
    }
    //-------------------------------------------------------------------------
    void MetalWindow::create( bool fullScreen, const PropertyData *miscParams )
    {
        destroy();

        mClosed = false;
        mHwGamma = true;
        NSObject *externalWindowHandle; // OgreMetalView, NSView or NSWindow

        if( miscParams )
        {
            PropertyData::const_iterator opt;
            PropertyData::const_iterator end = miscParams->end();

            opt = miscParams->find("FSAA");
            if( opt != end )
                mSample.parseString( opt->second );

            opt = miscParams->find("gamma");
            if( opt != end )
                mHwGamma = StringConverter::parseBool( opt->second );
            
            opt = miscParams->find("externalWindowHandle");
            if( opt != end )
                externalWindowHandle = (__bridge NSObject*)(void*)StringConverter::parseSizeT(opt->second);

            if( !externalWindowHandle )
            {
                opt = miscParams->find("parentWindowHandle");
                if( opt != end )
                    externalWindowHandle = (__bridge NSObject*)(void*)StringConverter::parseSizeT(opt->second);
            }
        }

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        if( externalWindowHandle && [externalWindowHandle isKindOfClass:[OgreMetalView class]] )
        {
            mMetalView = (OgreMetalView*)externalWindowHandle;
        }
        else
        {
            CGRect frame = CGRectMake(0.0, 0.0, mViewWidth, mViewHeight);
            mMetalView = [[OgreMetalView alloc] initWithFrame:frame];
        }
#else
        // create window if nothing was provided
        if( !externalWindowHandle )
        {
            NSRect frame = NSMakeRect(0.0, 0.0, mViewWidth, mViewHeight);
            NSWindowStyleMask style = NSWindowStyleMaskResizable | NSWindowStyleMaskTitled;
            if( mNextFullMode )
            {
                frame.size = NSScreen.mainScreen.visibleFrame.size;
                style = NSWindowStyleMaskBorderless;
            }
            NSWindow* window = [[NSWindow alloc] initWithContentRect:frame
                                                           styleMask:style
                                                             backing:NSBackingStoreBuffered
                                                               defer:YES];
            window.title = @(mTitle.c_str());
            window.contentView = [[OgreMetalView alloc] initWithFrame:frame];
            
            externalWindowHandle = window;
        }

        NSView* externalView;
        if( [externalWindowHandle isKindOfClass:[NSWindow class]] )
        {
            mWindow = (NSWindow*)externalWindowHandle;
            externalView = mWindow.contentView;
        }
        else
        {
            N_assert( [externalWindowHandle isKindOfClass:[NSView class]] );
            externalView = (NSView*)externalWindowHandle;
            mWindow = externalView.window;
        }

        if( [externalView isKindOfClass:[OgreMetalView class]] )
            mMetalView = (OgreMetalView*)externalView;
        else
        {
            NSRect frame = externalView.bounds;
            mMetalView = [[OgreMetalView alloc] initWithFrame:frame];
            mMetalView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
            [externalView addSubview:mMetalView];
        }
#endif

        mMetalLayer = (CAMetalLayer*)mMetalView.layer;
        mMetalLayer.device      = mDevice->mDevice;
        mMetalLayer.pf = MetalMappings::get( mHwGamma ? PF_B8G8R8A8UN_SRGB :
                                                                 PFG_BGRA8_UNORM );

        //This is the default but if we wanted to perform compute
        //on the final rendering layer we could set this to no
        mMetalLayer.framebufferOnly = YES;

        checkLayerSizeChanges();
        setResolutionFromView();

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        //We'll need to refresh as mMetalView.layer.contentsScale is usually out of date.
        mMetalView.layerSizeDidUpdate = YES;
#endif
    }
    //-------------------------------------------------------------------------
    void MetalWindow::destroy()
    {
        mClosed = true;

        N_delete mTexture;
        mTexture = 0;

        N_delete mDepthBuffer;
        mDepthBuffer = 0;
        mStencilBuffer = 0;

        mMetalLayer = 0;
        mCurrentDrawable = 0;
        mMetalView = 0;
    }
    //-----------------------------------------------------------------------------------
    void MetalWindow::_init(/* TextureManager *textureGpuManager */)
    {
        MetalTextureGpuManager *textureManager = static_cast<MetalTextureGpuManager*>( textureGpuManager );

        mTexture        = textureManager->createTextureGpuWindow( this );
        mDepthBuffer    = textureManager->createWindowDepthBuffer();

        mTexture->setFormat( mHwGamma ? PF_B8G8R8A8UN_SRGB : PFG_BGRA8_UNORM );
        mDepthBuffer->setFormat( Texture::DepthFormat );

        if( PixelUtil::isStencil( mDepthBuffer->getFormat() ) )
            mStencilBuffer = mDepthBuffer;

        if( mDepthBuffer )
        {
            mTexture->_setDepthBufferDefaults( DepthBuffer::GroupSeparate,
                                               false, mDepthBuffer->getFormat() );
        }
        else
        {
            mTexture->_setDepthBufferDefaults( DepthBuffer::GroupUnknow, false, PFG_NULL );
        }

        mSrcSample = textureManager->getRenderSystem()->checkSample(
            mSample, mTexture->getFormat() );
        mTexture->setSample( mSample, mSrcSample );
        mDepthBuffer->setSample( mSample, mSrcSample );

        setResolutionFromView();
        mTexture->load( StorageType::Resident, (uint8*)0 );
        mDepthBuffer->load( StorageType::Resident, (uint8*)0 );
    }
    //-------------------------------------------------------------------------
    void MetalWindow::pos( int32 left, int32 top )
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        CGRect frame = mMetalView.frame;
        frame.origin.x = left;
        frame.origin.y = top;
        mMetalView.frame = frame;
#else
        mMetalView.frame = [mWindow.contentView bounds];
        checkLayerSizeChanges();
#endif
    }
    //-------------------------------------------------------------------------
    void MetalWindow::resize( uint32 width, uint32 height )
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        CGRect frame = mMetalView.frame;
        frame.size.width    = width;
        frame.size.height   = height;
        mMetalView.frame = frame;
#else
        mMetalView.frame = [mWindow.contentView bounds];
#endif
        checkLayerSizeChanges();
    }
    //-------------------------------------------------------------------------
    bool MetalWindow::isClosed(void) const
    {
        return mClosed;
    }
    //-------------------------------------------------------------------------
    void MetalWindow::setVisible( bool visible )
    {
    }
    //-------------------------------------------------------------------------
    bool MetalWindow::isVisible(void) const
    {
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
        return mWindow.isVisible;
#else
        return mMetalView && mMetalView.window;
#endif
    }
    //-------------------------------------------------------------------------
    void MetalWindow::setHidden( bool hidden )
    {
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
        [mWindow setIsMiniaturized:hidden];
#endif
    }
    //-------------------------------------------------------------------------
    bool MetalWindow::isHidden(void) const
    {
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
        return mWindow.isMiniaturized;
#else
        return false;
#endif
    }
    //-------------------------------------------------------------------------
    void MetalWindow::getPlatformData( IdString name, void* pData )
    {
        if( name == "MetalDevice" )
        {
            *static_cast<MetalDevice**>(pData) = mDevice;
        }
        else if( name == "UIView" )
        {
            *static_cast<void**>(pData) = (void*)CFBridgingRetain( mMetalView );
        }
        else
        {
            Window::getPlatformData( name, pData );
        }
    }
}

