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

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0502
#endif

#include "NiiWin32Window.h"
#include "NiiGLPixelFormat.h"
#include "NiiWin32GLSupport.h"
#include "NiiWin32Context.h"
#include "NiiEngine.h"
#include "NiiLogManager.h"
#include "NiiImageCodec.h"
#include "NiiStrConv.h"
#include "NiiSysOption.h"
#include "NiiExtFrameBuffer.h"

#include <sstream>

#define TODO_notify_listeners

#define DISCL_EXCLUSIVE     0x00000001
#define DISCL_NONEXCLUSIVE  0x00000002
#define DISCL_FOREGROUND    0x00000004
#define DISCL_BACKGROUND    0x00000008
#define DISCL_NOWINKEY      0x00000010

namespace Ogre
{
    #define _MAX_CLASS_NAME_ 128
    bool Win32Window::mClassRegistered = false;

    Win32Window::Win32Window(const String & name, Nui32 width, Nui32 height, bool full,
                              PixelFormat dspf, const PropertyData * param,
                              Win32GLSupport & glsupport ) :
        ViewWindow(width, height, full, name),
        mGLSupport( glsupport ),
        mHWnd( 0 ),
        mHDC( 0 ),
        mGlrc( 0 ),
        mKBMark(0),
        mMSMark(0),
        mJDMark(0),
        mColourDepth( 32 ),
        mIsExternal( false ),
        mDeviceName( 0 ),
        mIsExternalGLControl( false ),
        mOwnsGLContext( true ),
        mSizing( false ),
        mClosed( false ),
        mHidden( false ),
        mVisible( true ),
        mHwGamma( false ),
        mActive(false),
        mContext(0),
        mWindowedWinStyle(0),
        mFullscreenWinStyle(0)
    {
        create(dspf, param);
    }
    //-----------------------------------------------------------------------------------
    Win32Window::~Win32Window()
    {
        destroy();
    }
    //-----------------------------------------------------------------------
    void Win32Window::updateWindowRect()
    {
		RECT rc;
        BOOL result;

        // Update top left parameters
        result = GetWindowRect(mHWnd, &rc);
        if(result == FALSE)
        {
            mTop = 0;
            mLeft = 0;
            setSize( 0, 0 );
            return;
        }
		mTop = rc.top;
		mLeft = rc.left;
        // width and height represent drawable area only
        result = GetClientRect(mHWnd, &rc);
        if(result == FALSE)
        {
            mTop = 0;
            mLeft = 0;
            setSize( 0, 0 );
			return;
        }
        unsigned int width = rc.right - rc.left;
        unsigned int height = rc.bottom - rc.top;

        // Case window resized.
        if(width != getWidth() || height != getHeight())
        {
            mViewWidth  = rc.right - rc.left;
            mViewHeight = rc.bottom - rc.top;
            setSize( mViewWidth, mViewHeight );
            notifyResolutionChanged();
        }
	}
    //-----------------------------------------------------------------------
	void Win32Window::adjustWindow(Nui32 clientWidth, Nui32 clientHeight, NIIi * winWidth, NIIi * winHeight)
	{
		RECT rc;
		SetRect(&rc, 0, 0, clientWidth, clientHeight);
		AdjustWindowRect(&rc, getWindowStyle(mNextFullMode), false);
		*winWidth = rc.right - rc.left;
		*winHeight = rc.bottom - rc.top;

		// adjust to monitor
		HMONITOR hMonitor = MonitorFromWindow(mHWnd, MONITOR_DEFAULTTONEAREST);

		// Get monitor info
		MONITORINFO monitorInfo;

		memset(&monitorInfo, 0, sizeof(MONITORINFO));
		monitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &monitorInfo);

		LONG maxW = monitorInfo.rcWork.right  - monitorInfo.rcWork.left;
		LONG maxH = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

		if (*winWidth > maxW)
			*winWidth = maxW;
		if (*winHeight > maxH)
			*winHeight = maxH;
	}
    //-----------------------------------------------------------------------------------
    void Win32Window::notifyResolutionChanged(void)
    {
        TODO_notify_listeners;
        
        //Views::iterator it = mViews.begin();
        //while(it != mViews.end())
            //(*it++).second->sync();
    }
    //-----------------------------------------------------------------------------------
    void Win32Window::create( PixelFormat dspf, const PropertyData *params )
    {
        // destroy current window, if any
        if( mHWnd )
            destroy();

        mClosed = false;
        mRasterGroup = GroupInner;
        mColourDepth = mNextFullMode ? 32 : GetDeviceCaps( GetDC(0), BITSPIXEL );
        int left = -1; // Defaults to screen center
        int top = -1; // Defaults to screen center
        HWND parentHwnd = 0;
        WNDPROC windowProc = DefWindowProc;
        bool hidden = false;
        String border;
        bool outerSize = false;
        mHwGamma = false;
        bool enableDoubleClick = false;
        int monitorIndex = -1;
        HMONITOR hMonitor = NULL;
        
        map<String, DWORD>::type temp;
        temp[_T("DISCL_BACKGROUND")] = DISCL_BACKGROUND;
        temp[_T("DISCL_EXCLUSIVE")] = DISCL_EXCLUSIVE;
        temp[_T("DISCL_FOREGROUND")] = DISCL_FOREGROUND;
        temp[_T("DISCL_NONEXCLUSIVE")] = DISCL_NONEXCLUSIVE;
        temp[_T("DISCL_NOWINKEY")] = DISCL_NOWINKEY;
        
        if( params )
        {
			// Get variable-length params
			PropertyData::PropertyList::const_iterator opt, optend = params->getList().end();
            for(opt = params->getList().begin(); opt != optend; ++opt)
            {
                switch (opt->first)
                {
                case N_PropertyWindow_Width:
                    if (width == 0)
                        StrConv::conv(opt->second, width);
                    break;
                case N_PropertyWindow_Height:
                    if (height == 0)
                        StrConv::conv(opt->second, height);
                    break;
                case N_PropertyWindow_FullWindow:
                    StrConv::conv(opt->second, full);
                    break;
                /*case N_PropertyWindow_DepthBuffer:
                {
                    Nui32 tempi;
                    StrConv::conv(opt->second, tempi);
                    mRasterGroup = tempi ? GroupInner : GroupUnknow;
                }
                break;*/
                case N_PropertyWindow_Title:
                    mName = opt->second;
                    break;
                case N_PropertyWindow_Left:
                    StrConv::conv(opt->second, left);
                    break;
                case N_PropertyWindow_Top:
                    StrConv::conv(opt->second, top);
                    break;
                case N_PropertyWindow_Vsync:
                    StrConv::conv(opt->second, mVSync);
                    break;
                case N_PropertyWindow_Hide:
                    StrConv::conv(opt->second, hidden);
                    break;
                case N_PropertyWindow_VsyncInterval:
                    StrConv::conv(opt->second, mVSyncInterval);
                    break;
                case N_PropertyWindow_FSAA:
                    StrConv::conv(opt->second, mFSAA);
                    break;
                case N_PropertyWindow_FSAAHint:
                    mFSAAExt = opt->second;
                    mSample.parseString(mFSAAExt);
                    break;
                case N_PropertyWindow_Gamma:
                    StrConv::conv(opt->second, mHwGamma);
                    break;

                case N_PropertyWindow_Stereo:
                {
                    Nui32 stereoMode;
                    StrConv::conv(opt->second, stereoMode);
                    if (SMT_NONE != stereoMode)
                        mStereo = true;
                }
                break;
                case N_PropertyWindow_Handle:
                {
                    Ni64 tempdata;
                    StrConv::conv(opt->second, tempdata);
                    mHWnd = (HWND)tempdata;
                    if (IsWindow(mHWnd) && mHWnd)
                    {
                        mIsExternal = true;
                        mNextFullMode = false;
                    }

                    if ((opt = params->getList().find(N_PropertyWindow_ExtGLContext)) != optend)
                    {
                        StrConv::conv(opt->second, mIsExternalGLControl);
                    }
                }
                break;
                case N_PropertyWindow_CurrentGLContext:
                {
                    bool temp;
                    StrConv::conv(opt->second, temp);
                    if (temp)
                    {
                        mGlrc = wglGetCurrentContext();

                        if (mGlrc)
                        {
                            mOwnsGLContext = false;
                        }
                        else
                        {
                            N_EXCEPT(RenderingAPI, "currentGLContext was specified with no current GL context");
                        }
                    }
                }
                break;

                case N_PropertyWindow_GLContext:
                {
                    Nui64 tempdata;
                    StrConv::conv(opt->second, tempdata);
                    mGlrc = (HGLRC)tempdata;
                    if (mGlrc)
                    {
                        mOwnsGLContext = false;
                    }
                    else
                    {
                        N_EXCEPT(RenderingAPI, "parsing the value of 'externalGLContext' failed: " + translateWGLError());
                    }
                }
                break;

            // window border style
                case N_PropertyWindow_Border:
                    border = opt->second;
                    break;
                case N_PropertyWindow_AdjustSize:
                    StrConv::conv(opt->second, outerSize);
                    break;

            // only available with fullscreen
                case N_PropertyWindow_Frequency:
                    StrConv::conv(opt->second, mDisplayFrequency);
                    break;
                case N_PropertyWindow_ColourDepth:
                {
                    StrConv::conv(opt->second, mColourDepth);
                    if (!mNextFullMode)
                    {
                        // make sure we don't exceed desktop colour depth
                        if ((NIIi)mColourDepth > GetDeviceCaps(GetDC(0), BITSPIXEL))
                            mColourDepth = GetDeviceCaps(GetDC(0), BITSPIXEL);
                    }
                }
                break;
                case N_PropertyWindow_WindowPrc:
                {
                    Nui64 tempdata;
                    StrConv::conv(opt->second, tempdata);
                    windowProc = reinterpret_cast<WNDPROC>(tempdata);
                }
                break;
                
                // incompatible with fullscreen
                case N_PropertyWindow_ParentHandle:
                {
                    Ni32 tempdata;
                    StrConv::conv(opt->second, tempdata);
                    parentHwnd = (HWND)tempdata;
                }
                break;

            // monitor index
                case N_PropertyWindow_MonitorIndex:
                {
                    StrConv::conv(opt->second, monitorIndex);
                }
                break;

            // monitor handle
                case N_PropertyWindow_MonitorHandle:
                {
                    Ni32 tempdata;
                    StrConv::conv(opt->second, tempdata);
                    hMonitor = (HMONITOR)tempdata;
                }
                break;

            // enable double click messages
                case N_PropertyWindow_EnableDoubleClick:
                    StrConv::conv(opt->second, enableDoubleClick);
                    break;
                case N_PropertyWindow_KeyBoard:
                    mKBMark |= temp[opt->second];
                    break;
                case N_PropertyWindow_Mouse:
                    mMSMark |= temp[opt->second];
                    break;
                case N_PropertyWindow_Joystick:
                    mJDMark |= temp[opt->second];
                    break;
                }
            }
        }
        
        if(mKBMark == 0)
            mKBMark = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY;

        if(mMSMark == 0)
            mMSMark = DISCL_FOREGROUND | DISCL_EXCLUSIVE;

        if(mJDMark == 0)
            mJDMark = DISCL_FOREGROUND | DISCL_EXCLUSIVE;

        if( !mIsExternal )
        {           
            DWORD         dwStyleEx = 0;                    
            MONITORINFOEX monitorInfoEx;
            
            // If we didn't specified the adapter index, or if it didn't find it
            if( hMonitor == NULL )
            {
                POINT windowAnchorPoint;

                // Fill in anchor point.
                windowAnchorPoint.x = left;
                windowAnchorPoint.y = top;

                // Get the nearest monitor to this window.
                hMonitor = MonitorFromPoint( windowAnchorPoint, MONITOR_DEFAULTTOPRIMARY );
            }

            // Get the target monitor info      
            memset( &monitorInfoEx, 0, sizeof(MONITORINFOEX) );
            monitorInfoEx.cbSize = sizeof(MONITORINFOEX);
            GetMonitorInfo( hMonitor, &monitorInfoEx );

            NCount devNameLen = Nstrlen( monitorInfoEx.szDevice );
            mDeviceName = N_alloc_t(Ntchar, devNameLen + 1];

            Nstrcpy( mDeviceName, monitorInfoEx.szDevice );

            // Update window style flags.
            mFullscreenWinStyle = (hidden ? 0 : WS_VISIBLE) | WS_CLIPCHILDREN | WS_POPUP;
            mWindowedWinStyle   = (hidden ? 0 : WS_VISIBLE) | WS_CLIPCHILDREN;
            
            if( parentHwnd )
            {
                mWindowedWinStyle |= WS_CHILD;
            }
            else
            {
                if (border == "none")
                    mWindowedWinStyle |= WS_POPUP;
                else if (border == "fixed")
                    mWindowedWinStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
                else
                    mWindowedWinStyle |= WS_OVERLAPPEDWINDOW;
            }

            uint32 winWidth = mViewWidth;
            uint32 winHeight = mViewHeight;
            {
                //Center window horizontally and/or vertically, on the right monitor.
                uint32 screenw = monitorInfoEx.rcWork.right  - monitorInfoEx.rcWork.left;
                uint32 screenh = monitorInfoEx.rcWork.bottom - monitorInfoEx.rcWork.top;
                uint32 outerw = (winWidth < screenw) ? winWidth : screenw;
                uint32 outerh = (winHeight < screenh) ? winHeight : screenh;
                if( left == INT_MAX )
                    left = monitorInfoEx.rcWork.left + (screenw - outerw) / 2;
                else if( monitorIndex != -1 )
                    left += monitorInfoEx.rcWork.left;
                if( top == INT_MAX )
                    top = monitorInfoEx.rcWork.top + (screenh - outerh) / 2;
                else if( monitorIndex != -1 )
                    top += monitorInfoEx.rcWork.top;
            }

            mTop = top;
            mLeft = left;

            if( mNextFullMode )
            {
                dwStyleEx |= WS_EX_TOPMOST;
                mTop = monitorInfoEx.rcMonitor.top;
                mLeft = monitorInfoEx.rcMonitor.left;
            }
            else
            {
                RECT rc;
                SetRect( &rc, mLeft, mTop, mViewWidth, mViewHeight );
                if( !outerSize )
                {
                    //User requested "client resolution", we need to grow the rect
                    //for the window's resolution (which is always bigger).
                    AdjustWindowRect( &rc, getWindowStyle(mNextFullMode), false );
                }

                //Clamp to current monitor's size
                if( rc.left < monitorInfoEx.rcWork.left )
                {
                    rc.right    += monitorInfoEx.rcWork.left - rc.left;
                    rc.left     = monitorInfoEx.rcWork.left;
                }
                if( rc.top < monitorInfoEx.rcWork.top )
                {
                    rc.bottom   += monitorInfoEx.rcWork.top - rc.top;
                    rc.top      = monitorInfoEx.rcWork.top;
                }
                if( rc.right > monitorInfoEx.rcWork.right )
                    rc.right = monitorInfoEx.rcWork.right;
                if( rc.bottom > monitorInfoEx.rcWork.bottom )
                    rc.bottom = monitorInfoEx.rcWork.bottom;

                mLeft           = rc.left;
                mTop            = rc.top;
                mViewWidth = rc.right - rc.left;
                mViewHeight= rc.bottom - rc.top;
            }

            //Grab the HINSTANCE by asking the OS what's the hinstance at an address in this process
            HINSTANCE hInstance = NULL;
            #ifdef __MINGW32__
                #ifdef NIIEngine_Static
                    hInstance = GetModuleHandle( NULL );
                #else
                    #if NII_DEBUG_MODE == 1
                        hInstance = GetModuleHandle("NIIEngineOGL.dll");
                    #else
                        hInstance = GetModuleHandle("NIIEngineOGL.dll");
                    #endif
                #endif
            #else
                static const TCHAR staticVar;
                GetModuleHandleEx( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                   GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                   &staticVar, &hInstance );
            #endif
 
            // register class and create window
            WNDCLASSEX wcex;
            wcex.cbSize         = sizeof( WNDCLASSEX );
            wcex.style          = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
            wcex.lpfnWndProc    = WindowUtil::proc;
            wcex.cbClsExtra     = 0;
            wcex.cbWndExtra     = 0;
            wcex.hInstance      = hInstance;
            wcex.hIcon          = LoadIcon( (HINSTANCE)0, (LPCTSTR)IDI_APPLICATION );
            wcex.hCursor        = LoadCursor( (HINSTANCE)0, IDC_ARROW );
            wcex.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
            wcex.lpszMenuName   = 0;
            wcex.lpszClassName  = _T("NIIEngine");
            wcex.hIconSm        = 0;

            if( enableDoubleClick )
                wcex.style |= CS_DBLCLKS;

            if( !mClassRegistered )
            {
                if( !RegisterClassEx( &wcex ) )
                {
                    N_EXCEPT(RenderingAPI, "RegisterClassEx failed! Cannot create window");
                }
                mClassRegistered = true;
            }

            if( mNextFullMode )
            {
                DEVMODE displayDeviceMode;

                memset(&displayDeviceMode, 0, sizeof(displayDeviceMode));
                displayDeviceMode.dmSize = sizeof(DEVMODE);
                displayDeviceMode.dmBitsPerPel = mColourDepth;
                displayDeviceMode.dmPelsWidth = mViewWidth;
                displayDeviceMode.dmPelsHeight = mViewHeight;
                displayDeviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

                if( mDisplayFrequency )
                {
                    displayDeviceMode.dmDisplayFrequency = mDisplayFrequency;
                    displayDeviceMode.dmFields |= DM_DISPLAYFREQUENCY;

                    if( ChangeDisplaySettingsEx( mDeviceName, &displayDeviceMode, NULL, CDS_FULLSCREEN | CDS_TEST,
                                                                        NULL ) != DISP_CHANGE_SUCCESSFUL )
                    {
                        LogManager::getOnly().log(LL_Info, "ChangeDisplaySettings with user display frequency failed" );
                        displayDeviceMode.dmFields ^= DM_DISPLAYFREQUENCY;
                    }
                }

                if( ChangeDisplaySettingsEx( mDeviceName, &displayDeviceMode, NULL, CDS_FULLSCREEN, NULL ) != DISP_CHANGE_SUCCESSFUL )
                {
                    LogManager::getOnly().log(LL_Info,  LML_CRITICAL, "ChangeDisplaySettings failed" );
                    mNextFullMode = false;
                }
            }

            // Pass pointer to self as WM_CREATE parameter
            mHWnd = CreateWindowEx( dwStyleEx, _T("NIIEngine"), mName.c_str(),
                                    getWindowStyle(mNextFullMode), mLeft, mTop,
                                    mViewWidth, mViewHeight, parentHwnd, 0, hInstance, this );

            WindowUtil::add(this);

            LogManager::getSingleton().stream() << "Created Win32Window '" << mName << "' : " << mViewWidth << "x" << mViewHeight
                << ", " << mColourDepth << "bpp";
        }

        HDC oldHdc = wglGetCurrentDC();
        HGLRC oldContext = wglGetCurrentContext();

        RECT rc;
        // top and left represent outer window coordinates
        GetWindowRect( mHWnd, &rc );
        mTop  = rc.top;
        mLeft = rc.left;
        // width and height represent interior drawable area
        GetClientRect( mHWnd, &rc );
        setSize( rc.right - rc.left, rc.bottom - rc.top );

        mHDC = GetDC( mHWnd );

        if( !mIsExternalGLControl )
        {
            // TODO: move into GL3PlusRenderSystem::checkSample
            unsigned msaaCount = mSample.getMultiSampling();
            int testFsaa = msaaCount > 1u ? msaaCount : 0;
            bool testHwGamma = mHwGamma;
            bool formatOk = mGLSupport.selectPixelFormat( mHDC, mColourDepth, testFsaa,
                                                          dspf, testHwGamma );
            if( !formatOk )
            {
                if( msaaCount > 1u )
                {
                    // try without FSAA
                    testFsaa = 0;
                    formatOk = mGLSupport.selectPixelFormat( mHDC, mColourDepth, testFsaa,
                                                             dspf, testHwGamma );
                }

                if( !formatOk && mHwGamma )
                {
                    // try without sRGB
                    testHwGamma = false;
                    testFsaa = msaaCount > 1u ? msaaCount : 0;
                    formatOk = mGLSupport.selectPixelFormat( mHDC, mColourDepth, testFsaa,
                                                             dspf, testHwGamma );
                }

                if( !formatOk && mHwGamma && msaaCount > 1u )
                {
                    // try without both
                    testHwGamma = false;
                    testFsaa = 0;
                    formatOk = mGLSupport.selectPixelFormat( mHDC, mColourDepth, testFsaa,
                                                             dspf, testHwGamma );
                }

                if( !formatOk )
                {
                    N_EXCEPT(RenderingAPI, "selectPixelFormat failed");
                }
            }

            // record what gamma option we used in the end
            // this will control enabling of sRGB state flags when used
			if(testHwGamma)
                mMark |= M_SRGB;
            else
                mMark &= ~M_SRGB;
            mSrcSample = SampleType( testFsaa ? testFsaa : 1u );
            mFSAA = testFsaa ? testFsaa : 1u
        }

        GLint contextMajor = 4;
        GLint contextMinor = 5;

        if( mOwnsGLContext )
        {
            int attribList[] =
            {
                WGL_CONTEXT_MAJOR_VERSION_ARB, contextMajor,
                WGL_CONTEXT_MINOR_VERSION_ARB, contextMinor,
            #if N_DEBUG
				WGL_CONTEXT_FLAGS_ARB,  WGL_CONTEXT_DEBUG_BIT_ARB,
            #endif
                WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                0, 0
            };

            while( !mGlrc &&
                   (attribList[1] > 3 ||
                    (attribList[1] == 3 && attribList[3] >= 3)) )
            {
                // New context is shared with previous one
                mGlrc = wglCreateContextAttribsARB( mHDC, oldContext, attribList );

                if( !mGlrc )
                {
                    if( attribList[3] == 0 )
                    {
                        contextMajor -= 1;
                        contextMinor = 5;
                        attribList[1] = contextMajor;
                        attribList[3] = contextMinor;
                    }
                    else
                    {
                        contextMinor -= 1;
                        attribList[3] = contextMinor;
                    }
                }
            }

            if( !mGlrc )
            {
                N_EXCEPT(RenderingAPI, "wglCreateContextAttribsARB failed: " + translateWGLError());
            }

            LogManager::getOnly().log(LL_Info, 
                        "Created GL " + N_conv(contextMajor) + "." +
                        N_conv(contextMinor) + " context" );
        }
        
        /*if (oldContext && oldContext != mGlrc)
        {
            // Share lists with old context
            if (!wglShareLists(oldContext, mGlrc))
                N_EXCEPT(RenderingAPI, _I18n("wglShareLists() failed"));
        }*/

        if( !wglMakeCurrent( mHDC, mGlrc ) )
        {
            N_EXCEPT(RenderingAPI, "wglMakeCurrent failed: " + translateWGLError());
        }

        // Do not change vsync if the external window has the OpenGL control
        if( !mIsExternalGLControl )
        {
            // Don't use wglew as if this is the first window, we won't have initialised yet
            PFNWGLSWAPINTERVALEXTPROC _wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
            if( _wglSwapIntervalEXT )
                _wglSwapIntervalEXT( mVSync? mVSyncInterval : 0 );
        }

        if( oldContext && oldContext != mGlrc )
        {
            // Restore old context
            if( !wglMakeCurrent( oldHdc, oldContext ) )
            {
                N_EXCEPT(RenderingAPI, "wglMakeCurrent() failed");
            }
        }

        // Create RenderSystem context
        mContext = new Win32Context( mHDC, mGlrc, static_cast<uint32>( contextMajor ), static_cast<uint32>( contextMinor ) );

        mFullMode = mNextFullMode;
    }
    //-----------------------------------------------------------------------------------
    void Win32Window::_init(/* TextureManager * mag*/ )
    {
        GL3PlusTextureGpuManager *textureManager = static_cast<GL3PlusTextureGpuManager*>( mag );

        mTexture        = textureManager->createTextureGpuWindow( mContext, this );
        mDepthBuffer    = textureManager->createTextureGpuWindow( mContext, this );

        if( mColourDepth == 16u )
            mTexture->setFormat( PF_B5G5R5A1UN );
        else
            mTexture->setFormat( mHwGamma ? PF_R8G8B8A8UN_SRGB : PF_R8G8B8A8UN );
        mDepthBuffer->setFormat( Texture::DepthFormat );
        if( PixelUtil::isStencil( mDepthBuffer->getFormat() ) )
            mStencilBuffer = mDepthBuffer;

        mTexture->setSampleDescription( mSrcSample );
        mDepthBuffer->setSampleDescription( mSrcSample );

        setSize( mViewWidth, mViewHeight );

        if( mDepthBuffer )
        {
            mTexture->_setDepthBufferDefaults( DepthBuffer::GroupSeparate, false, mDepthBuffer->getFormat() );
        }
        else
        {
            mTexture->_setDepthBufferDefaults( DepthBuffer::GroupUnknow, false, PFG_NULL );
        }

        mTexture->load( StorageType::ST_Gpu, (uint8*)0 );
        if( mDepthBuffer )
            mDepthBuffer->load( StorageType::ST_Gpu, (uint8*)0 );

        setHidden( mHidden );
    }
    //-----------------------------------------------------------------------------------
    void Win32Window::destroy(void)
    {
        if( mTexture )
        {
            mTexture->process( Texture::Process::Deleted );
            N_delete mTexture;
            mTexture = 0;
        }
        if( mDepthBuffer )
        {
            mDepthBuffer->process( Texture::Process::Deleted );
            N_delete mDepthBuffer;
            mDepthBuffer = 0;
        }

        //Depth & Stencil buffers are the same pointer
        //N_delete mStencilBuffer;
        mStencilBuffer = 0;

        if( !mHWnd )
            return;

        // Unregister and destroy OGRE GL3PlusContext
        if (mContext)
        {
            N_delete mContext;
            mContext = 0;
        }

        if( mOwnsGLContext )
        {
            wglDeleteContext( mGlrc );
            mGlrc = 0;
        }
        if( !mIsExternal )
        {
            WindowUtil::remove(this);

            if( mFullMode )
                ChangeDisplaySettingsEx( mDeviceName, NULL, NULL, 0, NULL );
            DestroyWindow( mHWnd );
        }
        else
        {
            // just release the DC
            ReleaseDC( mHWnd, mHDC );
        }

        mActive = false;
        mClosed = true;
        mHDC = 0; // no release thanks to CS_OWNDC wndclass style
        mHWnd = 0;

        if (mDeviceName != NULL)
        {
            delete[] mDeviceName;
            mDeviceName = NULL;
        }
    }
    //-----------------------------------------------------------------------
	void Win32Window::pos(NIIi left, NIIi top)
	{
		if (mHWnd && !mNextFullMode)
		{
			SetWindowPos(mHWnd, 0, left, top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}
    //-----------------------------------------------------------------------------------
    void Win32Window::resize( NCount width, NCount height )
    {
        if(mIsExternal )
        {
            updateWindowRect();
        }
        else
        {
            if( mHWnd && !mNextFullMode )
            {
                uint32 winWidth, winHeight;
                adjustWindow( width, height, &winWidth, &winHeight );
                SetWindowPos( mHWnd, 0, 0, 0, winWidth, winHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );
            }
        }
    }
    //-----------------------------------------------------------------------
	void Win32Window::setFullMode(bool full, Nui32 width, Nui32 height, Nui32 frequency, Nui32 denominator, bool borderless, Nui32 monitorIdx)
	{
        if (mIsExternal)
            return;
        
		if (mNextFullMode != full || width != mViewWidth || height != mViewHeight)
		{
            mNextFullMode = full;
            mDisplayFrequency = frequency;

			if (mNextFullMode)
			{
				DEVMODE displayDeviceMode;

				memset(&displayDeviceMode, 0, sizeof(displayDeviceMode));
				displayDeviceMode.dmSize = sizeof(DEVMODE);
				displayDeviceMode.dmBitsPerPel = mColourDepth;
				displayDeviceMode.dmPelsWidth = width;
				displayDeviceMode.dmPelsHeight = height;
				displayDeviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
				if (mDisplayFrequency)
				{
					displayDeviceMode.dmDisplayFrequency = mDisplayFrequency;
					displayDeviceMode.dmFields |= DM_DISPLAYFREQUENCY;

					if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL,
						CDS_FULLSCREEN | CDS_TEST, NULL) != DISP_CHANGE_SUCCESSFUL)
					{
						LogManager::getOnly().log(LL_Info, _I18n("ChangeDisplaySettings with user display frequency failed"));
						displayDeviceMode.dmFields ^= DM_DISPLAYFREQUENCY;
					}
				}
				else
				{
					// try a few
					displayDeviceMode.dmDisplayFrequency = 100;
					displayDeviceMode.dmFields |= DM_DISPLAYFREQUENCY;
					if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL,
						CDS_FULLSCREEN | CDS_TEST, NULL) != DISP_CHANGE_SUCCESSFUL)
					{
						displayDeviceMode.dmDisplayFrequency = 75;
						if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL,
							CDS_FULLSCREEN | CDS_TEST, NULL) != DISP_CHANGE_SUCCESSFUL)
						{
							displayDeviceMode.dmFields ^= DM_DISPLAYFREQUENCY;
						}
					}

				}
				// move window to 0,0 before display switch
				SetWindowPos(mHWnd, HWND_TOPMOST, 0, 0, mViewWidth, mViewHeight, SWP_NOACTIVATE);

				if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL)
                {
					LogManager::getOnly().log(LL_Fatal, _I18n("ChangeDisplaySettings failed"));
                    mNextFullMode = false;
                }

				// Get the nearest monitor to this window.
				HMONITOR hMonitor = MonitorFromWindow(mHWnd, MONITOR_DEFAULTTONEAREST);

				// Get monitor info
				MONITORINFO monitorInfo;

				memset(&monitorInfo, 0, sizeof(MONITORINFO));
				monitorInfo.cbSize = sizeof(MONITORINFO);
				GetMonitorInfo(hMonitor, &monitorInfo);

				mTop = monitorInfo.rcMonitor.top;
				mLeft = monitorInfo.rcMonitor.left;

				SetWindowLong(mHWnd, GWL_STYLE, getWindowStyle(mNextFullMode));
				SetWindowPos(mHWnd, HWND_TOPMOST, mLeft, mTop, width, height, SWP_NOACTIVATE);
                setSize( width, height );
			}
			else
			{
				// drop out of fullscreen
				ChangeDisplaySettingsEx(mDeviceName, NULL, NULL, 0, NULL);

				// calculate overall dimensions for requested client area
			    NIIi winWidth, winHeight;
				adjustWindow(width, height, &winWidth, &winHeight);

				// deal with centreing when switching down to smaller resolution

				HMONITOR hMonitor = MonitorFromWindow(mHWnd, MONITOR_DEFAULTTONEAREST);
				MONITORINFO monitorInfo;
				memset(&monitorInfo, 0, sizeof(MONITORINFO));
				monitorInfo.cbSize = sizeof(MONITORINFO);
				GetMonitorInfo(hMonitor, &monitorInfo);

				LONG screenw = monitorInfo.rcWork.right  - monitorInfo.rcWork.left;
				LONG screenh = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;


				NIIi left = screenw > winWidth ? ((screenw - winWidth) / 2) : 0;
				NIIi top = screenh > winHeight ? ((screenh - winHeight) / 2) : 0;

				SetWindowLong(mHWnd, GWL_STYLE, getWindowStyle(mNextFullMode));
				SetWindowPos(mHWnd, HWND_NOTOPMOST, left, top, winWidth, winHeight,
					SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE);
                mLeft = left;
                mTop = top;
                setSize( width, height );

				notify();
			}
            mFullMode = mNextFullMode;
		}
	}
    //-----------------------------------------------------------------------------------
    void Win32Window::notify()
    {
        if( !mHWnd || IsIconic(mHWnd) )
            return;

        updateWindowRect();
    }
    //-----------------------------------------------------------------------------------
    bool Win32Window::isClosed() const
    {
        return mClosed;
    }
    //-----------------------------------------------------------------------------------
    void Win32Window::setVisible( bool set )
    {
        mVisible = set;
    }
    //-----------------------------------------------------------------------------------
    bool Win32Window::isVisible() const
    {
        bool visible = mVisible && !mHidden;

        //Window minimized or fully obscured (we got notified via setVisible/WM_PAINT messages)
        if( !visible )
            return visible;

        {
            HWND cHWnd = mHWnd;
            while( (visible = (IsIconic(cHWnd) == false)) && (GetWindowLong(cHWnd, GWL_STYLE) & WS_CHILD) != 0)
            {
                cHWnd = GetParent( cHWnd );
            }

            //Window is minimized
            if( !visible )
                return visible;
        }

        /*
         *  Poll code to see if we're fully obscured.
         * Not needed since we do it via WM_PAINT messages.
         *
        HDC hdc = GetDC( mHWnd );
        if( hdc )
        {
            RECT rcClip, rcClient;
            switch( GetClipBox( hdc, &rcClip ) )
            {
            case NULLREGION:
                // Completely covered
                visible = false;
                break;
            case SIMPLEREGION:
                GetClientRect(hwnd, &rcClient);
                if( EqualRect( &rcClient, &rcClip ) )
                {
                    // Completely uncovered
                    visible = true;
                }
                else
                {
                    // Partially covered
                    visible = true;
                }
                break;
            case COMPLEXREGION:
                // Partially covered
                visible = true;
                break;
            default:
                // Error
                visible = true;
                break;
            }

            // If we wanted, we could also use RectVisible
            // or PtVisible - or go totally overboard by
            // using GetClipRgn
            ReleaseDC( mHWnd, hdc );
        }*/

        return visible;
    }
    //-----------------------------------------------------------------------------------
    void Win32Window::setHidden( bool set )
    {
        mHidden = set;
        if( !mIsExternal )
        {
            if( set )
                ShowWindow( mHWnd, SW_HIDE );
            else
                ShowWindow( mHWnd, SW_SHOWNORMAL );
        }
    }
    //-----------------------------------------------------------------------------------
    bool Win32Window::isHidden() const
    {
        return mHidden;
    }
    //-----------------------------------------------------------------------
    void Win32Window::setVSync(bool vsync)
    {
        mVSync = vsync;
        HDC oldHdc = wglGetCurrentDC();
        HGLRC oldContext = wglGetCurrentContext();
        if(!wglMakeCurrent(mHDC, mGlrc))
            N_EXCEPT(RenderingAPI, _T("wglMakeCurrent"));

        // Do not change vsync if the external window has the OpenGL control
        if(!mIsExternalGLControl) 
        {
            // Don't use wglew as if this is the first window, we won't have initialised yet
            PFNWGLSWAPINTERVALEXTPROC _wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
            if (_wglSwapIntervalEXT)
                _wglSwapIntervalEXT(mVSync? mVSyncInterval : 0);
        }

        if (oldContext && oldContext != mGlrc)
        {
            // Restore old context
            if (!wglMakeCurrent(oldHdc, oldContext))
                N_EXCEPT(RenderingAPI, _I18n("wglMakeCurrent() failed"));
        }
    }
    //-----------------------------------------------------------------------------------
    void Win32Window::swap()
    {
        if( !mIsExternalGLControl )
        {
            //OgreProfileBeginDynamic( ("SwapBuffers: " + mName).c_str() );
            //OgreProfileGpuBeginDynamic( "SwapBuffers: " + mName );
            SwapBuffers(mHDC);
            //OgreProfileEnd( "SwapBuffers: " + mName );
           // OgreProfileGpuEnd( "SwapBuffers: " + mName );
        }
    }
    //-----------------------------------------------------------------------
	void Win32Window::setActive(bool state)
	{
		if (mDeviceName != NULL && state == false)
		{
			HWND hActiveWindow = GetActiveWindow();
			TCHAR classNameSrc[_MAX_CLASS_NAME_ + 1];
			TCHAR classNameDst[_MAX_CLASS_NAME_ + 1];

			GetClassName(mHWnd, classNameSrc, _MAX_CLASS_NAME_);
			GetClassName(hActiveWindow, classNameDst, _MAX_CLASS_NAME_);

			if (Nstrcmp(classNameDst, classNameSrc) == 0)
			{
				state = true;
			}
		}

		ViewWindow::setActive( state );

		if(mNextFullMode)
		{
			if(state == false)
			{
				ChangeDisplaySettingsEx(mDeviceName, NULL, NULL, 0, NULL);
				ShowWindow(mHWnd, SW_SHOWMINNOACTIVE);
			}
			else
			{
				ShowWindow(mHWnd, SW_SHOWNORMAL);

                mNextFullMode = false;
                setFullMode(true, mViewWidth, mViewHeight, mDisplayFrequency, mDisplayDenominator, mBorderless, -1);
			}
		}
	}
    //-----------------------------------------------------------------------
	bool Win32Window::getPlatformData(const String & name, void * pData) const
	{
		if(name == _T("GLCONTEXT"))
        {
			*static_cast<GLContext**>(pData) = mContext;
			return true;
		}
        else if(name == _T("WINDOW"))
		{
			HWND * pHwnd = (HWND*)pData;
			*pHwnd = getWindowHandle();
			return true;
		}
        else if(name == _T("KBD"))
        {
            DWORD * n = reinterpret_cast<DWORD *>(pData);
            *n = mKBMark;
			return true;
        }
        else if(name == _T("MS"))
        {
            DWORD * n = reinterpret_cast<DWORD *>(pData);
            *n = mMSMark;
			return true;
        }
        else if(name == _T("JD"))
        {
            DWORD * n = reinterpret_cast<DWORD *>(pData);
            *n = mJDMark;
			return true;
        }

        return false;
	}
    //-----------------------------------------------------------------------
	bool Win32Window::isActive() const
	{
		if(isFullMode())
			return isVisible();

		return mActive && isVisible();
	}
    //-----------------------------------------------------------------------
    void Win32Window::setSwapInterval(NCount interval)
    {
        mVSyncInterval = interval;
        if (mVSync)
            setVSync(true);
    }
    //-----------------------------------------------------------------------
	/*void Win32Window::fill(FaceType buffer, const Box & src, PixelBlock & dst)
	{
        if (src.mRight > mWidth || src.mBottom > mHeight || src.mFront != 0 || src.mBack != 1 || 
            dst.getWidth() != src.getWidth() || dst.getHeight() != src.getHeight() || dst.getDepth() != 1)
        {
            N_EXCEPT(InvalidParam, _T("Invalid box."));
        }

        if (buffer == FT_Unknow)
        {
            buffer = mFullMode ? FT_Front : FT_Back;
        }

        static_cast<GLRenderSystemBase *>(N_Engine().getRender())->_copyContentsToMemory(getViewport(0), src, dst, buffer);
	}*/
    //-----------------------------------------------------------------------
    
    
}
