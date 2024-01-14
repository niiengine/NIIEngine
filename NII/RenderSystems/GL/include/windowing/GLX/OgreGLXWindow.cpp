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

#include "OgreGLXWindow.h"
#include "OgreRoot.h"
#include "OgreGL3PlusRenderSystem.h"
#include "OgreGL3PlusTextureGpuWindow.h"
#include "OgreException.h"
#include "OgreLogManager.h"
#include "OgreStringConverter.h"
#include "OgreGLXUtils.h"
#include "OgreGLXGLSupport.h"
#include "OgreGL3PlusTextureGpuManager.h"
#include "OgreTextureGpuListener.h"
#include "OgreWindowEventUtilities.h"
#include "OgrePixelFormatGpuUtils.h"
#include "OgreDepthBuffer.h"

#include "OgreProfiler.h"
#include "OgreString.h"

#include <iostream>
#include <algorithm>
#include <sys/time.h>
#include <climits>

#include <X11/Xlib.h>
#include <X11/keysym.h>

extern "C"
{
    int
    safeXErrorHandler (Display *display, XErrorEvent *event)
    {
        // Ignore all XErrorEvents
        return 0;
    }
    int (*oldXErrorHandler)(Display *, XErrorEvent*);
}

namespace Ogre
{
    GLXWindow::GLXWindow( const String &title, uint32 width, uint32 height, bool fullscreenMode,
                          PixelFormat depthStencilFormat, const PropertyData *miscParams,
                          GLXGLSupport* glsupport ) :
        Window( title, width, height, fullscreenMode ),
        mClosed( false ),
        mVisible( true ),
        mHidden( false ),
        mIsTopLevel( false ),
        mIsExternal( false ),
        mIsExternalGLControl( false ),
        mGLSupport( glsupport ),
        mWindow( 0 ),
        mContext( 0 )
    {
        assert( depthStencilFormat == PFG_NULL || depthStencilFormat == PF_UNKNOWN ||
                PixelUtil::isDepth( depthStencilFormat ) );

        create( depthStencilFormat, miscParams );
    }
    //-----------------------------------------------------------------------------------
    GLXWindow::~GLXWindow()
    {
        Display* xDisplay = mGLSupport->getXDisplay();

        destroy();

        // Ignore fatal XErrorEvents from stale handles.
        oldXErrorHandler = XSetErrorHandler(safeXErrorHandler);

        if (mWindow)
        {
            XDestroyWindow(xDisplay, mWindow);
            XSync(xDisplay, false);
        }

        if (mContext)
        {
            delete mContext;
        }

        if( mTexture )
        {
            mTexture->process( Texture::Process::Deleted );
            OGRE_DELETE mTexture;
            mTexture = 0;
        }
        if( mDepthBuffer )
        {
            mDepthBuffer->process( Texture::Process::Deleted );
            OGRE_DELETE mDepthBuffer;
            mDepthBuffer = 0;
        }
        //Depth & Stencil buffers are the same pointer
        //OGRE_DELETE mStencilBuffer;
        mStencilBuffer = 0;

        XSetErrorHandler(oldXErrorHandler);

        mContext = 0;
        mWindow = 0;
    }
    //-----------------------------------------------------------------------------------
    void GLXWindow::create( PixelFormat depthStencilFormat, const PropertyData *miscParams )
    {
        Display *xDisplay = mGLSupport->getXDisplay();
        uint8 samples = 0;
        bool hidden = false;
        bool vSync = false;
        uint32 vSyncInterval = 1u;
        bool gamma = 0;
        ::GLXContext glxContext     = 0;
        ::GLXDrawable glxDrawable   = 0;
        ::Window externalWindow     = 0;
        ::Window parentWindow       = DefaultRootWindow(xDisplay);
        int left = DisplayWidth( xDisplay, DefaultScreen(xDisplay) ) / 2 - mViewWidth / 2u;
        int top  = DisplayHeight( xDisplay, DefaultScreen(xDisplay) ) / 2 - mViewHeight / 2u;
        String border;

        if( miscParams )
        {
            PropertyData::const_iterator opt;
            PropertyData::const_iterator end = miscParams->end();

            // NB: Do not try to implement the externalGLContext option.
            //
            //   Accepting a non-current context would expose us to the
            //   risk of segfaults when we made it current. Since the
            //   application programmers would be responsible for these
            //   segfaults, they are better discovering them in their code.

            if( (opt = miscParams->find("currentGLContext")) != end &&
                StringConverter::parseBool(opt->second) )
            {
                if( !glXGetCurrentContext() )
                {
                    N_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR,
                                 "currentGLContext was specified with no current GL context",
                                 "GLXWindow::create" );
                }

                glxContext = glXGetCurrentContext();
                // Trying to reuse the drawable here will actually
                // cause the context sharing to break so it is hidden
                // behind this extra option
                if( (opt = miscParams->find( "currentGLDrawable" )) != end &&
                    StringConverter::parseBool( opt->second ) )
                {
                    glxDrawable = glXGetCurrentDrawable();
                }
            }

            // Note: Some platforms support AA inside ordinary windows
            if( (opt = miscParams->find("FSAA")) != end )
                samples = StringConverter::parseUnsignedInt(opt->second);
            if( (opt = miscParams->find("MSAA")) != end )
                samples = StringConverter::parseUnsignedInt(opt->second);

            if( (opt = miscParams->find("displayFrequency")) != end && opt->second != "N/A" )
                mDisplayFrequency = StringConverter::parseUnsignedInt( opt->second );

            if((opt = miscParams->find("vsync")) != end)
                vSync = StringConverter::parseBool(opt->second);

            if((opt = miscParams->find("hidden")) != end)
                hidden = StringConverter::parseBool(opt->second);

            if((opt = miscParams->find("vsyncInterval")) != end)
                vSyncInterval = StringConverter::parseUnsignedInt(opt->second);

            if ((opt = miscParams->find("gamma")) != end)
                gamma = StringConverter::parseBool(opt->second);

            if((opt = miscParams->find("left")) != end)
                left = StringConverter::parseInt(opt->second);

            if((opt = miscParams->find("top")) != end)
                top = StringConverter::parseInt(opt->second);

            if ((opt = miscParams->find("externalGLControl")) != end)
                mIsExternalGLControl = StringConverter::parseBool(opt->second);
            
#if OGRE_NO_QUAD_BUFFER_STEREO == 0
			if ((opt = miscParams->find("stereoMode")) != end)
			{
				StereoModeType stereoMode = StringConverter::parseStereoMode(opt->second);
				if (SMT_NONE != stereoMode)
					mStereoEnabled = true;
			}
#endif

            if( (opt = miscParams->find("parentWindowHandle")) != end )
            {
                vector<String>::type tokens = StringUtil::split(opt->second, " :");

                if (tokens.size() == 3)
                {
                    // deprecated display:screen:xid format
                    parentWindow = StringConverter::parseUnsignedLong(tokens[2]);
                }
                else
                {
                    // xid format
                    parentWindow = StringConverter::parseUnsignedLong(tokens[0]);
                }
            }
            else if((opt = miscParams->find("externalWindowHandle")) != end)
            {
                vector<String>::type tokens = StringUtil::split(opt->second, " :");

                LogManager::getSingleton().logMessage(
                    "GLXWindow::create: The externalWindowHandle parameter is deprecated.\n"
                    "Use the parentWindowHandle or currentGLContext parameter instead.");

                if (tokens.size() == 3)
                {
                    // Old display:screen:xid format
                    // The old GLX code always created a "parent" window in this case:
                    parentWindow = StringConverter::parseUnsignedLong(tokens[2]);
                }
                else if (tokens.size() == 4)
                {
                    // Old display:screen:xid:visualinfo format
                    externalWindow = StringConverter::parseUnsignedLong(tokens[2]);
                }
                else
                {
                    // xid format
                    externalWindow = StringConverter::parseUnsignedLong(tokens[0]);
                }
            }

            if( (opt = miscParams->find("border")) != end )
                border = opt->second;
        }

        // Ignore fatal XErrorEvents during parameter validation:
        oldXErrorHandler = XSetErrorHandler( safeXErrorHandler );
        // Validate parentWindowHandle

        if( parentWindow != DefaultRootWindow(xDisplay) )
        {
            XWindowAttributes windowAttrib;

            if( !XGetWindowAttributes( xDisplay, parentWindow, &windowAttrib ) ||
                windowAttrib.root != DefaultRootWindow(xDisplay) )
            {
                N_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR,
                             "Invalid parentWindowHandle (wrong server or screen)",
                             "GLXWindow::create" );
            }
        }

        // Validate externalWindowHandle

        if( externalWindow != 0 )
        {
            XWindowAttributes windowAttrib;

            if( !XGetWindowAttributes(xDisplay, externalWindow, &windowAttrib) ||
                windowAttrib.root != DefaultRootWindow(xDisplay) )
            {
                N_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR,
                             "Invalid externalWindowHandle (wrong server or screen)",
                             "GLXWindow::create");
            }
            glxDrawable = externalWindow;
        }

        // Derive fbConfig

        ::GLXFBConfig fbConfig = 0;

        if( glxDrawable )
        {
            unsigned int width = mViewWidth;
            unsigned int height = mViewHeight;
            fbConfig = mGLSupport->getFBConfigFromDrawable( glxDrawable, &width, &height );
            setSize( width, height );
        }

        if( !fbConfig && glxContext )
            fbConfig = mGLSupport->getFBConfigFromContext( glxContext );

        mIsExternal = (glxDrawable != 0);

        XSetErrorHandler( oldXErrorHandler );

        if( !fbConfig )
        {
            const int MAX_ATTRIB_COUNT = 64;
            int minAttribs[MAX_ATTRIB_COUNT];
            memset( minAttribs, 0, sizeof( minAttribs ) );
            int attrib = 0;
            minAttribs[attrib++] = GLX_DRAWABLE_TYPE;   minAttribs[attrib++] = GLX_WINDOW_BIT;
            minAttribs[attrib++] = GLX_RENDER_TYPE;     minAttribs[attrib++] = GLX_RGBA_BIT;
            minAttribs[attrib++] = GLX_RED_SIZE;        minAttribs[attrib++] = 1;
            minAttribs[attrib++] = GLX_GREEN_SIZE;      minAttribs[attrib++] = 1;
            minAttribs[attrib++] = GLX_BLUE_SIZE;       minAttribs[attrib++] = 1;
#if OGRE_NO_QUAD_BUFFER_STEREO == 0
            minAttribs[attrib++] = GLX_STEREO;          minAttribs[attrib++] = mStereoEnabled ? True :
                                                                                                False;
#endif
            if( depthStencilFormat == PFG_NULL )
            {
                minAttribs[attrib++] = GLX_DEPTH_SIZE;
                minAttribs[attrib++] = 0;
            }
            else
            {
                minAttribs[attrib++] = GLX_DEPTH_SIZE;
                minAttribs[attrib++] = 24;
            }
            minAttribs[attrib++] = GLX_FRAMEBUFFER_SRGB_CAPABLE_EXT;
            minAttribs[attrib++] = gamma == true;
            minAttribs[attrib++] = None;

            attrib = 0;

            int maxAttribs[MAX_ATTRIB_COUNT];
            memset( maxAttribs, 0, sizeof( maxAttribs ) );
            maxAttribs[attrib++] = GLX_SAMPLES;         maxAttribs[attrib++] = static_cast<int>(samples);
            maxAttribs[attrib++] = GLX_DOUBLEBUFFER;    maxAttribs[attrib++] = 1;
            if( depthStencilFormat == PF_Depth24UN_Stencil8ui || depthStencilFormat == PF_Depth24UN )
            {
                //If user wants a 24-bit format, we'll restrict it.
                maxAttribs[attrib++] = GLX_DEPTH_SIZE;
                maxAttribs[attrib++] = 24;
            }
            else if( depthStencilFormat == PFG_NULL )
            {
                //User wants no depth buffer, restrict it even more.
                maxAttribs[attrib++] = GLX_DEPTH_SIZE;
                maxAttribs[attrib++] = 0;
            }
            if( PixelUtil::isStencil( depthStencilFormat ) ||
                depthStencilFormat == PF_UNKNOWN )
            {
                maxAttribs[attrib++] = GLX_STENCIL_SIZE;
                maxAttribs[attrib++] = INT_MAX;
            }
            else
            {
                maxAttribs[attrib++] = GLX_STENCIL_SIZE;
                maxAttribs[attrib++] = 0;
            }
            maxAttribs[attrib++] = GLX_FRAMEBUFFER_SRGB_CAPABLE_EXT;
            maxAttribs[attrib++] = gamma == true;
            maxAttribs[attrib++] = None;

            assert( attrib < MAX_ATTRIB_COUNT );

            fbConfig = mGLSupport->selectFBConfig( minAttribs, maxAttribs );
        }

        if( !fbConfig )
        {
            // This should never happen.
            N_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR,
                         "Unexpected failure to determine a GLXFBConfig",
                         "GLXWindow::create" );
        }

        mIsTopLevel = !mIsExternal && parentWindow == DefaultRootWindow(xDisplay);

        if( !mIsTopLevel )
        {
            mFullMode = false;
            mNextFullMode = false;
            left = top = 0;
        }

        if( mNextFullMode )
        {
            mGLSupport->switchMode( mViewWidth, mViewHeight,
                                    static_cast<short>( mDisplayFrequency ) );
            mFullMode = true;
        }

        if( !mIsExternal )
        {
            XSetWindowAttributes attr;
            ulong mask;
            XVisualInfo *visualInfo = mGLSupport->getVisualFromFBConfig( fbConfig );

            attr.background_pixel = 0;
            attr.border_pixel = 0;
            attr.colormap = XCreateColormap( xDisplay, DefaultRootWindow(xDisplay),
                                             visualInfo->visual, AllocNone );
            attr.event_mask = StructureNotifyMask | VisibilityChangeMask | FocusChangeMask;
            mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

            if( mFullMode && mGLSupport->mAtomFullScreen == None )
            {
                mNextFullMode = false;
                mFullMode = false;
                LogManager::getSingleton().logMessage(
                            "GLXWindow::switchFullScreen: Your WM has no fullscreen support" );

                // A second best approach for outdated window managers
                attr.backing_store = NotUseful;
                attr.save_under = False;
                attr.override_redirect = True;
                mask |= CWSaveUnder | CWBackingStore | CWOverrideRedirect;
                left = top = 0;
            }

            // Create window on server
            mWindow = XCreateWindow( xDisplay, parentWindow, left, top,
                                     mViewWidth, mViewHeight, 0,
                                     visualInfo->depth, InputOutput, visualInfo->visual, mask, &attr );

            XFree( visualInfo );

            if( !mWindow )
            {
                N_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR,
                             "Unable to create an X Window",
                             "GLXWindow::create" );
            }

            if( mIsTopLevel )
            {
                XWMHints *wmHints;
                XSizeHints *sizeHints;

                if( (wmHints = XAllocWMHints()) != NULL )
                {
                    wmHints->initial_state = NormalState;
                    wmHints->input = True;
                    wmHints->flags = StateHint | InputHint;

                    int depth = DisplayPlanes( xDisplay, DefaultScreen(xDisplay) );

                    // Check if we can give it an icon
                    if( depth == 24 || depth == 32 )
                    {
                        if( mGLSupport->loadIcon( "GLX_icon.png", &wmHints->icon_pixmap,
                                                  &wmHints->icon_mask ) )
                        {
                            wmHints->flags |= IconPixmapHint | IconMaskHint;
                        }
                    }
                }

                // Is this really necessary ? Which broken WM might need it?
                if( (sizeHints = XAllocSizeHints()) != NULL )
                {
                    // Is this really necessary ? Which broken WM might need it?
                    sizeHints->flags = USPosition;

                    if( !mNextFullMode && border == "fixed" )
                    {
                        sizeHints->min_width = sizeHints->max_width = mViewWidth;
                        sizeHints->min_height = sizeHints->max_height = mViewHeight;
                        sizeHints->flags |= PMaxSize | PMinSize;
                    }
                }

                XTextProperty titleprop;
                char *lst = const_cast<char*>( mTitle.c_str() );
                XStringListToTextProperty( (char**)&lst, 1, &titleprop );
                XSetWMProperties( xDisplay, mWindow, &titleprop, NULL, NULL,
                                  0, sizeHints, wmHints, NULL );

                XFree( titleprop.value );
                XFree( wmHints );
                XFree( sizeHints );

                XSetWMProtocols( xDisplay, mWindow, &mGLSupport->mAtomDeleteWindow, 1 );

                XWindowAttributes windowAttrib;

                XGetWindowAttributes(xDisplay, mWindow, &windowAttrib);

                left = windowAttrib.x;
                top = windowAttrib.y;
                setSize( windowAttrib.width, windowAttrib.height );
            }

            glxDrawable = mWindow;

            // setHidden takes care of mapping or unmapping the window
            // and also calls setFullScreen if appropriate.
            setHidden(hidden);
            XSync(xDisplay, False);

            WindowUtil::add(this);
        }

        mContext = new GLXContext( mGLSupport, fbConfig, glxDrawable, glxContext );

        setVSync( vSync, vSyncInterval );

        int fbConfigID;

        mGLSupport->getFBConfigAttrib( fbConfig, GLX_FBCONFIG_ID, &fbConfigID );

        LogManager::getSingleton().logMessage( "GLXWindow::create used FBConfigID = " +
                                               StringConverter::toString(fbConfigID) );

        mLeft = left;
        mTop = top;
        mActive = true;
        mClosed = false;
    }
    //-----------------------------------------------------------------------------------
    void GLXWindow::_init(/* TextureManager *_textureManager */)
    {
        GL3PlusTextureGpuManager *textureManager =
                static_cast<GL3PlusTextureGpuManager*>( _textureManager );

        mTexture = textureManager->createTextureGpuWindow( mContext, this );

        mTexture->setFormat( PF_R8G8B8A8UN );

        ::GLXFBConfig fbConfig = mContext->_getFbConfig();

        // Now check the actual supported fsaa value
        GLint maxSamples;
        mGLSupport->getFBConfigAttrib( fbConfig, GLX_SAMPLES, &maxSamples );
        SampleType sampleDesc( std::max<uint8>( 1u, static_cast<uint8>( maxSamples ) ) );
        mTexture->setSampleDescription( sampleDesc );

        GLint depthSupport = 0, stencilSupport = 0;
        mGLSupport->getFBConfigAttrib( fbConfig, GLX_DEPTH_SIZE, &depthSupport );
        mGLSupport->getFBConfigAttrib( fbConfig, GLX_STENCIL_SIZE, &stencilSupport );
        if( depthSupport != 0 )
        {
            mDepthBuffer = textureManager->createTextureGpuWindow( mContext, this );
            mDepthBuffer->setSampleDescription( sampleDesc );

            if( depthSupport == 24 )
            {
                mDepthBuffer->setFormat( stencilSupport == 0 ? PF_Depth24UN :
                                                                    PF_Depth24UN_Stencil8ui );
            }
            else
            {
                mDepthBuffer->setFormat( stencilSupport == 0 ? PF_Depth32 :
                                                                    PF_Depth32f_Stencil8ui_64 );
            }

            if( stencilSupport != 0 )
                mStencilBuffer = mDepthBuffer;
        }

        GLint gammaSupport = 0;
        int result = mGLSupport->getFBConfigAttrib( fbConfig, GLX_FRAMEBUFFER_SRGB_CAPABLE_EXT,
                                                    &gammaSupport );
        if( result != Success )
            gammaSupport = 0;

        if( gammaSupport )
            mTexture->setFormat( PF_R8G8B8A8UN_SRGB );

        setSize( mViewWidth, mViewHeight );

        if( mDepthBuffer )
        {
            mTexture->_setDepthBufferDefaults( DepthBuffer::GroupSeparate,
                                               false, mDepthBuffer->getFormat() );
        }
        else
        {
            mTexture->_setDepthBufferDefaults( DepthBuffer::GroupUnknow, false, PFG_NULL );
        }

        mTexture->load( StorageType::ST_Gpu, (uint8*)0 );
        if( mDepthBuffer )
            mDepthBuffer->load( StorageType::ST_Gpu, (uint8*)0 );
    }
    //-----------------------------------------------------------------------------------
    void GLXWindow::destroy(void)
    {
        if( mClosed )
            return;

        mClosed = true;
        mActive = false;

        if( !mIsExternal )
            WindowUtil::remove(this);

        if( mFullMode )
        {
            mGLSupport->switchMode();
            switchFullScreen( false );
            mNextFullMode = false;
        }
    }

    //-----------------------------------------------------------------------------------
    void GLXWindow::setFullMode( bool goFullscreen, bool borderless, uint32 monitorIdx,
                                             uint32 width, uint32 height,
                                             uint32 frequencyNumerator, uint32 frequencyDenominator )
    {
        if( mClosed || !mIsTopLevel )
            return;

        if( goFullscreen == mFullMode &&
            width == mViewWidth && height == mViewHeight &&
            mTexture->getWidth() == width && mTexture->getHeight() == height &&
            mDisplayFrequency == frequencyNumerator )
        {
            mNextFullMode = mFullMode;
            return;
        }

        if( goFullscreen && mGLSupport->mAtomFullScreen == None )
        {
            // Without WM support it is best to give up.
            LogManager::getSingleton().logMessage(
                        "GLXWindow::switchFullScreen: Your WM has no fullscreen support" );
            mNextFullMode = false;
            mFullMode = false;
            return;
        }

        Window::setFullMode( goFullscreen, borderless, monitorIdx,
            width, height, frequencyNumerator, frequencyDenominator );

        if( goFullscreen )
        {
            mGLSupport->switchMode( width, height, frequencyNumerator );
        }
        else
        {
            mGLSupport->switchMode();
        }

        if( mFullMode != goFullscreen )
            switchFullScreen( goFullscreen );

        if( !mFullMode )
        {
            resize( width, height );
            pos( mLeft, mTop );
        }
    }

    //-----------------------------------------------------------------------------------
    bool GLXWindow::isClosed() const
    {
        return mClosed;
    }
    //-----------------------------------------------------------------------------------
    bool GLXWindow::isVisible() const
    {
        return mVisible;
    }
    //-----------------------------------------------------------------------------------
    void GLXWindow::setVisible(bool visible)
    {
        mVisible = visible;
    }
    //-----------------------------------------------------------------------------------
    void GLXWindow::setHidden(bool hidden)
    {
        mHidden = hidden;
        // ignore for external windows as these should handle
        // this externally
        if (mIsExternal)
            return;

        if( hidden )
        {
            XUnmapWindow( mGLSupport->getXDisplay(), mWindow );
        }
        else
        {
            XMapWindow( mGLSupport->getXDisplay(), mWindow );
            if( mFullMode )
                switchFullScreen( true );
        }
    }
    //-----------------------------------------------------------------------------------
    void GLXWindow::setVSync( bool vSync, uint32 vSyncInterval )
    {
        Window::setVSync( vSync, vSyncInterval );

        // we need to make our context current to set vsync
        // store previous context to restore when finished.
        ::GLXDrawable oldDrawable = glXGetCurrentDrawable();
        ::GLXContext  oldContext  = glXGetCurrentContext();

        mContext->setCurrent();

        PFNGLXSWAPINTERVALEXTPROC _glXSwapIntervalEXT;
        _glXSwapIntervalEXT =
                (PFNGLXSWAPINTERVALEXTPROC)mGLSupport->getProcAddress("glXSwapIntervalEXT");
        PFNGLXSWAPINTERVALMESAPROC _glXSwapIntervalMESA;
        _glXSwapIntervalMESA =
                (PFNGLXSWAPINTERVALMESAPROC)mGLSupport->getProcAddress("glXSwapIntervalMESA");
        PFNGLXSWAPINTERVALSGIPROC _glXSwapIntervalSGI;
        _glXSwapIntervalSGI =
                (PFNGLXSWAPINTERVALSGIPROC)mGLSupport->getProcAddress("glXSwapIntervalSGI");

        if( !mIsExternalGLControl )
        {
            if( _glXSwapIntervalMESA )
                _glXSwapIntervalMESA( mVSync ? mVSyncInterval : 0 );
            else if( _glXSwapIntervalEXT )
            {
                _glXSwapIntervalEXT( mGLSupport->getGLDisplay(), mContext->mDrawable,
                                     mVSync ? mVSyncInterval : 0 );
            }
            else
                _glXSwapIntervalSGI( mVSync ? mVSyncInterval : 0 );
        }

        mContext->endCurrent();

        glXMakeCurrent( mGLSupport->getGLDisplay(), oldDrawable, oldContext );
    }
    //-----------------------------------------------------------------------------------
    void GLXWindow::pos( int32 left, int32 top )
    {
        if( mClosed || ! mIsTopLevel )
            return;

        XMoveWindow( mGLSupport->getXDisplay(), mWindow, left, top );
    }
    //-----------------------------------------------------------------------------------
    void GLXWindow::resize( uint32 width, uint32 height )
    {
        if( mClosed )
            return;

        if( mTexture && mTexture->getWidth() == width && mTexture->getHeight() == height )
            return;

        Window::resize( width, height );

        if( width != 0 && height != 0 )
        {
            if( !mIsExternal )
                XResizeWindow( mGLSupport->getXDisplay(), mWindow, width, height );
            else
                setSize( width, height );
        }
    }
    //-----------------------------------------------------------------------------------
    void GLXWindow::notify()
    {
        if( mClosed || !mWindow )
            return;

        Display* xDisplay = mGLSupport->getXDisplay();
        XWindowAttributes windowAttrib;

        if( mIsTopLevel && !mFullMode )
        {
            ::Window parent, root, *children;
            uint nChildren;

            XQueryTree( xDisplay, mWindow, &root, &parent, &children, &nChildren );

            if (children)
                XFree(children);

            XGetWindowAttributes(xDisplay, parent, &windowAttrib);

            mLeft = windowAttrib.x;
            mTop  = windowAttrib.y;
        }

        XGetWindowAttributes( xDisplay, mWindow, &windowAttrib );

        setSize( windowAttrib.width, windowAttrib.height );
    }
    //-----------------------------------------------------------------------------------
    void GLXWindow::swap()
    {
        if( mClosed || mIsExternalGLControl )
            return;

        OgreProfileBeginDynamic( ("SwapBuffers: " + mTitle).c_str() );
        OgreProfileGpuBeginDynamic( "SwapBuffers: " + mTitle );

        glXSwapBuffers( mGLSupport->getGLDisplay(), mContext->mDrawable );

        OgreProfileEnd( "SwapBuffers: " + mTitle );
        OgreProfileGpuEnd( "SwapBuffers: " + mTitle );
    }
    //-----------------------------------------------------------------------------------
    void GLXWindow::getPlatformData( IdString name, void *pData )
    {
        if( name == "DISPLAY NAME" )
        {
            *static_cast<String*>(pData) = mGLSupport->getDisplayName();
            return;
        }
        else if( name == "DISPLAY" )
        {
            *static_cast<Display**>(pData) = mGLSupport->getGLDisplay();
            return;
        }
        else if( name == "GLCONTEXT" )
        {
            *static_cast<GLXContext**>(pData) = mContext;
            return;
        }
        else if( name == "XDISPLAY" )
        {
            *static_cast<Display**>(pData) = mGLSupport->getXDisplay();
            return;
        }
        else if( name == "ATOM" )
        {
            *static_cast< ::Atom* >(pData) = mGLSupport->mAtomDeleteWindow;
            return;
        }
        else if( name == "WINDOW" )
        {
            *static_cast< ::Window* >(pData) = mWindow;
            return;
        }
    }
    //-----------------------------------------------------------------------------------
    void GLXWindow::switchFullScreen( bool fullscreen )
    {
        if( mGLSupport->mAtomFullScreen != None )
        {
            Display* xDisplay = mGLSupport->getXDisplay();
            XClientMessageEvent xMessage;

            xMessage.type = ClientMessage;
            xMessage.serial = 0;
            xMessage.send_event = True;
            xMessage.window = mWindow;
            xMessage.message_type = mGLSupport->mAtomState;
            xMessage.format = 32;
            xMessage.data.l[0] = (fullscreen ? 1 : 0);
            xMessage.data.l[1] = mGLSupport->mAtomFullScreen;
            xMessage.data.l[2] = 0;

            XSendEvent( xDisplay, DefaultRootWindow(xDisplay), False,
                        SubstructureRedirectMask | SubstructureNotifyMask, (XEvent*)&xMessage );

            mFullMode = fullscreen;
        }
    }
}
