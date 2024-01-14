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

#include "NiiEngine.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"
#include "NiiWin32GLSupport.h"
#include "NiiGLTexture.h"
#include "NiiWin32Window.h"
#include "NiiWin32TextureFrame.h"
#include <algorithm>

namespace NII
{
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = 0;
    //-----------------------------------------------------------------------
    GLSupport* getGLSupport(int profile)
    {
        return N_new Win32GLSupport();
    }
    //-----------------------------------------------------------------------
	template<class C> void removeDuplicates(C& c)
	{
		std::sort(c.begin(), c.end());
		typename C::iterator p = std::unique(c.begin(), c.end());
		c.erase(p, c.end());
	}
    //-----------------------------------------------------------------------
	Win32GLSupport::Win32GLSupport() : 
		mInitialWindow(0),
        mWglChoosePixelFormat(0),
		mHasPixelFormatARB(false),
        mHasMultisample(false), 
		mHasHardwareGamma(false)
    {
		// immediately test WGL_ARB_pixel_format and FSAA support
		// so we can set configuration options appropriately
		initialiseWGL();
        mStereoMode = SMT_NONE;
    }
    //-----------------------------------------------------------------------
	void Win32GLSupport::setupConfig(SysOptionList & out, PropertyData & out2)
	{
		//TODO: EnumDisplayDevices http://msdn.microsoft.com/library/en-us/gdi/devcons_2303.asp
		/*vector<string> DisplayDevices;
		DISPLAY_DEVICE DisplayDevice;
		DisplayDevice.cb = sizeof(DISPLAY_DEVICE);
		DWORD i=0;
		while (EnumDisplayDevices(NULL, i++, &DisplayDevice, 0) {
			DisplayDevices.push_back(DisplayDevice.DeviceName);
		}*/

		// Video mode possiblities
        DEVMODE DevMode;
        DevMode.dmSize = sizeof(DEVMODE);
        for (DWORD i = 0; EnumDisplaySettings(NULL, i, &DevMode); ++i)
        {
            if (DevMode.dmBitsPerPel < 16 || DevMode.dmPelsHeight < 480)
                continue;
            //mDevModes.push_back(DevMode);
            mVideoModes.push_back({ DevMode.dmPelsWidth, DevMode.dmPelsHeight,
                Nui16(DevMode.dmDisplayFrequency), Nui8(DevMode.dmBitsPerPel) });
        }

        SysOption optColourDepth;
        optColourDepth.mID = N_PropertyWindow_ColourDepth;
		optColourDepth.mName = _T("Colour Depth");
		optColourDepth.mReserveEnable = true;

		SysOption optVSyncInterval;
        optVSyncInterval.mID = N_PropertyWindow_VsyncInterval;
		optVSyncInterval.mName = _T("VSync Interval");
		optVSyncInterval.mReserveEnable = true;
		optVSyncInterval.mReserve.push_back(_T("1") );
		optVSyncInterval.mReserve.push_back(_T("2") );
		optVSyncInterval.mReserve.push_back(_T("3") );
		optVSyncInterval.mReserve.push_back(_T("4") );
		optVSyncInterval.mValue = _T("1");

        out[optColourDepth.mID] = optColourDepth;
        out[optVSyncInterval.mID] = optVSyncInterval;
        //out2.add(optColourDepth.mID, _T(""));
        out2.add(optVSyncInterval.mID, optVSyncInterval.mValue);
	}
    //-----------------------------------------------------------------------
	BOOL CALLBACK Win32GLSupport::sCreateMonitorsInfoEnumProc(
		HMONITOR hMonitor,  // handle to display monitor
		HDC hdcMonitor,     // handle to monitor DC
		LPRECT lprcMonitor, // monitor intersection rectangle
		LPARAM dwData       // data
		)
	{
		DisplayMonitorInfoList * pArrMonitorsInfo = (DisplayMonitorInfoList*)dwData;

		// Get monitor info
		DisplayMonitorInfo displayMonitorInfo;

		displayMonitorInfo.hMonitor = hMonitor;

		memset(&displayMonitorInfo.monitorInfoEx, 0, sizeof(MONITORINFOEX));
		displayMonitorInfo.monitorInfoEx.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(hMonitor, &displayMonitorInfo.monitorInfoEx);

		pArrMonitorsInfo->push_back(displayMonitorInfo);

		return TRUE;
	}
    //-----------------------------------------------------------------------
	ViewWindow * Win32GLSupport::newWindow(const String & name, NCount width, NCount height, 
        bool full, const PropertyData * param)
	{
		Win32Window * window = N_new Win32Window(name, width, height, full, PF_UNKNOWN, param, *this);
		PropertyData newParams;

		if (param != NULL)
		{
			newParams = *param;
			//param = &newParams;
			PropertyData::PropertyList::const_iterator monitorIndexIt = param->getList().find(N_PropertyWindow_MonitorIndex);
			HMONITOR hMonitor = NULL;
			NIIi monitorIndex = -1;

			// If monitor index found, try to assign the monitor handle based on it.
			if (monitorIndexIt != param->getList().end())
			{
				if (mMonitorInfoList.empty())
					EnumDisplayMonitors(NULL, NULL, sCreateMonitorsInfoEnumProc, (LPARAM)&mMonitorInfoList);

				StrConv::conv(monitorIndexIt->second, monitorIndex);
				if (monitorIndex < (NIIi)mMonitorInfoList.size())
				{
					hMonitor = mMonitorInfoList[monitorIndex].hMonitor;
				}
			}
			// If we didn't specified the monitor index, or if it didn't find it
			if (hMonitor == NULL)
			{
				POINT windowAnchorPoint;

				PropertyData::PropertyList::const_iterator opt;
				NIIi left = -1;
				NIIi top  = -1;

				if ((opt = newParams.getList().find(N_PropertyWindow_Left)) != newParams.getList().end())
					StrConv::conv(opt->second, left);

				if ((opt = newParams.getList().find(N_PropertyWindow_Top)) != newParams.getList().end())
					StrConv::conv(opt->second, top);

				// Fill in anchor point.
				windowAnchorPoint.x = left;
				windowAnchorPoint.y = top;

				// Get the nearest monitor to this window.
				hMonitor = MonitorFromPoint(windowAnchorPoint, MONITOR_DEFAULTTOPRIMARY);
			}
			newParams.add(N_PropertyWindow_MonitorHandle, N_conv((NCount)hMonitor));
		}

        if (!mInitialWindow)
            mInitialWindow = window;
		return window;
	}
    //-----------------------------------------------------------------------
	void Win32GLSupport::start()
	{
		LogManager::getOnly().log(_I18n("*** Starting Win32GL Subsystem ***"));
	}
    //-----------------------------------------------------------------------
	void Win32GLSupport::stop()
	{
		LogManager::getOnly().log(_I18n("*** Stopping Win32GL Subsystem ***"));
		mInitialWindow = 0;
	}
    //-----------------------------------------------------------------------
	void* Win32GLSupport::getProcAddress(const VString & procname)
	{
        return (void *)wglGetProcAddress(procname.c_str());
	}
    //-----------------------------------------------------------------------
	void Win32GLSupport::initialiseWGL()
	{
		// wglGetProcAddress does not work without an active OpenGL context,
		// but we need wglChoosePixelFormatARB's address before we can
		// create our main window.  Thank you very much, Microsoft!
		//
		// The solution is to create a dummy OpenGL window first, and then
		// test for WGL_ARB_pixel_format support.  If it is not supported,
		// we make sure to never call the ARB pixel format functions.
		//
		// If is is supported, we call the pixel format functions at least once
		// to init them (pointers are stored by glprocs.h).  We can also
		// take this opportunity to enumerate the valid FSAA modes.

		LPCTSTR dummyText = _T("NiiWGLDummy");
#ifdef __MINGW32_
    #ifdef NIIEngine_Static
		HINSTANCE hinst = GetModuleHandle(NULL);
    #else
        #if NII_DEBUG_MODE == 1
		HINSTANCE hinst = GetModuleHandle(_T("RenderSystem_GL_d.dll"));
        #else
		HINSTANCE hinst = GetModuleHandle(_T("RenderSystem_GL.dll"));
    #endif
#else
        static const Ntchar staticVar;
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, &staticVar, &hinst);
#endif

		WNDCLASS dummyClass;
		memset(&dummyClass, 0, sizeof(WNDCLASS));
		dummyClass.style = CS_OWNDC;
		dummyClass.hInstance = hinst;
		dummyClass.lpfnWndProc = dummyWndProc;
		dummyClass.lpszClassName = dummyText;
		RegisterClass(&dummyClass);

		HWND hwnd = CreateWindow(dummyText, dummyText,
			WS_POPUP | WS_CLIPCHILDREN, 0, 0, 32, 32, 0, 0, hinst, 0);

		// if a simple CreateWindow fails, then boy are we in trouble...
		if (hwnd == NULL)
			N_EXCEPT(RenderingAPI, _I18n("CreateWindow() failed"));


		// no chance of failure and no need to release thanks to CS_OWNDC
		HDC hdc = GetDC(hwnd);

		// assign a simple OpenGL pixel format that everyone supports
		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.cColorBits = 16;
		pfd.cDepthBits = 15;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;

		// if these fail, wglCreateContext will also quietly fail
		NIIi format;
		if ((format = ChoosePixelFormat(hdc, &pfd)) != 0)
			SetPixelFormat(hdc, format, &pfd);

		HGLRC hrc = wglCreateContext(hdc);
		if (hrc)
		{
			HGLRC oldrc = wglGetCurrentContext();
			HDC oldhdc = wglGetCurrentDC();
			// if wglMakeCurrent fails, wglGetProcAddress will return null
			wglMakeCurrent(hdc, hrc);
            
            wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)
                                            wglGetProcAddress("wglCreateContextAttribsARB");

			PFNWGLGETEXTENSIONSSTRINGARBPROC _wglGetExtensionsStringARB =
				(PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");

			// check for pixel format and multisampling support
			if (_wglGetExtensionsStringARB)
			{
				std::istringstream wglexts(_wglGetExtensionsStringARB(hdc));
				std::string ext;
				while (wglexts >> ext)
				{
					if (ext == "WGL_ARB_pixel_format")
						mHasPixelFormatARB = true;
					else if (ext == "WGL_ARB_multisample")
						mHasMultisample = true;
					else if (ext == "WGL_EXT_framebuffer_sRGB" || ext == "WGL_ARB_framebuffer_sRGB")
						mHasHardwareGamma = true;
				}
			}

			if (mHasPixelFormatARB && mHasMultisample)
			{
				// enumerate all formats w/ multisampling
				static const int iattr[] = {
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
                    /* We are no matter about the colour, depth and stencil buffers here
					WGL_COLOR_BITS_ARB, 24,
					WGL_ALPHA_BITS_ARB, 8,
					WGL_DEPTH_BITS_ARB, 24,
					WGL_STENCIL_BITS_ARB, 8,
                    */
					WGL_SAMPLES_ARB, 2,
					0
				};
				int formats[256];
				unsigned int count;
                // cheating here.  wglChoosePixelFormatARB procc address needed later on
                // when a valid GL context does not exist and glew is not initialized yet.
                PFNWGLGETPIXELFORMATATTRIBIVARBPROC _wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribivARB");
                mWglChoosePixelFormat = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
                if (mWglChoosePixelFormat(hdc, iattr, 0, 256, formats, &count))
                {
                    // determine what multisampling levels are offered
                    NIIi query = WGL_SAMPLES_ARB, samples;
                    for (unsigned int i = 0; i < count; ++i)
                    {
                        if (_wglGetPixelFormatAttribivARB(hdc, formats[i], 0, 1, &query, &samples))
                        {
                            mFSAALevels.push_back(samples);
                        }
                    }
                    removeDuplicates(mFSAALevels);
                }
			}

			wglMakeCurrent(oldhdc, oldrc);
			wglDeleteContext(hrc);
		}

		// clean up our dummy window and class
		DestroyWindow(hwnd);
		UnregisterClass(dummyText, hinst);
        
        if( !wglCreateContextAttribsARB )
        {
            N_EXCEPT(RenderingAPI,
                "WGL_ARB_create_context extension required for OpenGL 3.3");
        }
	}
    //-----------------------------------------------------------------------
	LRESULT Win32GLSupport::dummyWndProc(HWND hwnd, UINT umsg, WPARAM wp, LPARAM lp)
	{
		return DefWindowProc(hwnd, umsg, wp, lp);
	}
    //-----------------------------------------------------------------------
	bool Win32GLSupport::selectPixelFormat(HDC hdc, NIIi colourDepth, NIIi multisample, PixelFormat dspf, bool hwGamma)
	{
		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = (colourDepth > 16)? 24 : colourDepth;
		pfd.cAlphaBits = (colourDepth > 16)? 8 : 0;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
        
        if( dspf == PF_Depth24UN_Stencil8ui || dspf == PF_Depth24UN || dspf == PF_UNKNOWN )
        {
            pfd.cDepthBits = 24;
        }
        else if( dspf == PF_Depth32 || dspf == PF_Depth32f_Stencil8ui_64 )
        {
            pfd.cDepthBits = 32;
        }
        else if( dspf == PFG_NULL )
        {
            pfd.cDepthBits = 0;
        }

        if( PixelUtil::isStencil( dspf ) || dspf == PF_UNKNOWN )
        {
            pfd.cStencilBits = 8;
        }
        else
        {
            pfd.cStencilBits = 0;
        }

        if (SMT_Stereo == mStereoMode)
            pfd.dwFlags |= PFD_STEREO;

		NIIi format = 0;

		NIIi useHwGamma = hwGamma;

		if (multisample && (!mHasMultisample || !mHasPixelFormatARB))
			return false;

		if (hwGamma && !mHasHardwareGamma)
			return false;

		if ((multisample || hwGamma) && mWglChoosePixelFormat)
		{

			// Use WGL to test extended caps (multisample, sRGB)
			vector<NIIi>::type attribList;
			attribList.push_back(WGL_DRAW_TO_WINDOW_ARB); attribList.push_back(GL_TRUE);
			attribList.push_back(WGL_SUPPORT_OPENGL_ARB); attribList.push_back(GL_TRUE);
			attribList.push_back(WGL_DOUBLE_BUFFER_ARB); attribList.push_back(GL_TRUE);
			attribList.push_back(WGL_SAMPLE_BUFFERS_ARB); attribList.push_back(GL_TRUE);
			attribList.push_back(WGL_ACCELERATION_ARB); attribList.push_back(WGL_FULL_ACCELERATION_ARB);
			attribList.push_back(WGL_COLOR_BITS_ARB); attribList.push_back(pfd.cColorBits);
			attribList.push_back(WGL_ALPHA_BITS_ARB); attribList.push_back(pfd.cAlphaBits);
			attribList.push_back(WGL_DEPTH_BITS_ARB); attribList.push_back(pfd.cDepthBits);
			attribList.push_back(WGL_STENCIL_BITS_ARB); attribList.push_back(pfd.cStencilBits);
			attribList.push_back(WGL_SAMPLES_ARB); attribList.push_back(multisample);

            if (SMT_Stereo == mStereoMode)
            {
                attribList.push_back(WGL_STEREO_ARB); attribList.push_back(GL_TRUE);
            }
			if (useHwGamma && mHasHardwareGamma)
			{
				attribList.push_back(WGL_FRAMEBUFFER_SRGB_CAPABLE_EXT); attribList.push_back(GL_TRUE);
			}
			// terminator
			attribList.push_back(0);


			UINT nformats;
			// ChoosePixelFormatARB proc address was obtained when setting up a dummy GL context in initialiseWGL()
			// since glew hasn't been initialized yet, we have to cheat and use the previously obtained address
			if (!mWglChoosePixelFormat(hdc, &(attribList[0]), NULL, 1, &format, &nformats) || nformats <= 0)
				return false;
		}
		else
		{
			format = ChoosePixelFormat(hdc, &pfd);
		}


		return (format && SetPixelFormat(hdc, format, &pfd));
	}
    //-----------------------------------------------------------------------
    GLPBuffer * Win32GLSupport::createPBuffer(PixelDataType format, NCount w, NCount h)
	{
		return N_new Win32PBuffer(format, w, h);
	}
    //-----------------------------------------------------------------------
	NCount Win32GLSupport::getDisplayMonitorCount() const
	{
		if(mMonitorInfoList.empty())
			EnumDisplayMonitors(NULL, NULL, sCreateMonitorsInfoEnumProc, (LPARAM)&mMonitorInfoList);

		return mMonitorInfoList.size();
	}
    //-----------------------------------------------------------------------
    void Win32GLSupport::initialiseExtensions()
    {
        assert(mInitialWindow);
        // First, initialise the normal extensions
        GLSupport::initialiseExtensions();

        // Check for W32 specific extensions probe function
        PFNWGLGETEXTENSIONSSTRINGARBPROC _wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
        if (!_wglGetExtensionsStringARB)
            return mInitialWindow;
        const char * wgl_extensions = _wglGetExtensionsStringARB(mInitialWindow->getHDC());
        String tempext;
        StrConv::conv(wgl_extensions, tempext);
        LogManager::getOnly().stream() << _I18n("Supported WGL extensions: ") << tempext;
        // Parse them, and add them to the main list
        VStringStream ext;
        VString instr;
        ext << wgl_extensions;
        while (ext >> instr)
        {
            extensionList.insert(instr);
        }
    }
    //-----------------------------------------------------------------------
	String translateWGLError()
	{
		NIIi winError = GetLastError();
		NIIi i;
		Ntchar errDesc[255];

		// Try windows errors first
		i = FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			winError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) errDesc,
			255,
			NULL
			);

		return String(errDesc);
	}
    //-----------------------------------------------------------------------
}