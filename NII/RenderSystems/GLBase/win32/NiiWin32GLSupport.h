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

#ifndef NII_Win32GLSupport_H
#define NII_Win32GLSupport_H

#include "NiiWin32PreInclude.h"
#include "NiiGLRenderSystem.h"
#include "NiiGLSupport.h"

namespace NII
{
	class _EngineGLAPI Win32GLSupport : public GLSupport, public GPUAlloc
	{
	public:
        Win32GLSupport();

		/// @copydetails GLSupport::setupConfig
		void setupConfig(SysOptionList & out, PropertyData & out2);

		/// @copydetails RenderSys::createWindow
		virtual ViewWindow * newWindow(const String & name, NCount width, NCount height, bool full, const PropertyData * custom = 0);

		/// @copydetails GLSupport::start
		void start();

		/// @copydetails GLSupport::stop
		void stop();

		/// @copydetails GLSupport::getProcAddress
		void * getProcAddress(const VString & procname);

		bool selectPixelFormat(HDC hdc, NIIi colourDepth, NIIi multisample, PixelFormat dspf, bool hwGamma);

		/// @copydetails GLSupport::createPBuffer
		virtual GLPBuffer * createPBuffer(PixelDataType format, NCount w, NCount h);

		/// @copydetails GLSupport::getDisplayMonitorCount
		virtual NCount getDisplayMonitorCount() const;
        
        /// @copydetails GLSupport::initialiseExtensions
        virtual void initialiseExtensions();
        
        void setStereoModeType(StereoModeType stereoMode)
        {
            mStereoMode = stereoMode;
        }
    protected:
        void initialiseWGL();
        static LRESULT CALLBACK dummyWndProc(HWND hwnd, UINT umsg, WPARAM wp, LPARAM lp);
        static BOOL CALLBACK sCreateMonitorsInfoEnumProc(HMONITOR hMonitor, HDC hdcMonitor,
            LPRECT lprcMonitor, LPARAM dwData);
	private:
        struct DisplayMonitorInfo
        {
            HMONITOR		hMonitor;
            MONITORINFOEX	monitorInfoEx;
        };

        typedef vector<DisplayMonitorInfo>::type DisplayMonitorInfoList;
        typedef DisplayMonitorInfoList::iterator DisplayMonitorInfoIterator;

        //vector<DEVMODE>::type mDevModes;
        Win32Window *mInitialWindow;
		DisplayMonitorInfoList mMonitorInfoList;
        StereoModeType mStereoMode;
        PFNWGLCHOOSEPIXELFORMATARBPROC mWglChoosePixelFormat;
        bool mHasPixelFormatARB;
        bool mHasMultisample;
        bool mHasHardwareGamma;
	};
    
    extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
}

#endif
