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

#ifndef _NII_Win32Window_H_
#define _NII_Win32Window_H_

#include "NiiWin32PreInclude.h"
#include "NiiViewWindow.h"

namespace NII 
{
    class _EngineGLAPI Win32Window : public ViewWindow
    {
    public:
        Win32Window( const String &title, Nui32 width, Nui32 height, bool full,
                     PixelFormat dspf, const PropertyData * param, Win32GLSupport &glsupport );

        virtual ~Win32Window();

        virtual void _init(/* TextureManager *textureGpuManager */);
        
        /// @copydetails ViewWindow::destroy
        virtual void destroy(void);

		/// @copydetails ViewWindow::pos
        virtual void pos(NIIi left, NIIi top);
        
		/// @copydetails ViewWindow::resize
        virtual void resize(NCount width, NCount height);
        
		/// @copydetails ViewWindow::setFullMode
        virtual void setFullMode(bool full, Nui32 viewWidth, Nui32 viewHeight, Nui32 frequency, Nui32 denominator, bool borderless, Nui32 monitorIdx);

        /// @copydetails ViewWindow::isClosed
        bool isClosed(void) const;
        
        /// @copydetails ViewWindow::setVisible
        virtual void setVisible(bool set);
        
        /// @copydetails ViewWindow::isVisible
        virtual bool isVisible() const;
        
        /// @copydetails ViewWindow::setHidden
        virtual void setHidden(bool set);
        
        /// @copydetails ViewWindow::isHidden
        virtual bool isHidden() const;
        
		/// @copydetails ViewWindow::setVSync
        void setVSync(bool b);
        
        virtual void swap(void);
        
		/// @copydetails ViewWindow::setActive
        virtual void setActive(bool set);
        
		/// @copydetails ViewWindow::isActive
		virtual bool isActive() const;
        
        inline HWND getWindowHandle() const         { return mHWnd; }
        inline HDC getHDC() const                   { return mHDC; }

        GLContext * getContext() const              { return mContext; }

        bool getPlatformData(const String & name, void * data) const;
    protected:
        void create( PixelFormat depthStencilFormat, const PropertyData * params );

        void updateWindowRect(void);
        
        /// 调整窗体
        void adjustWindow(Nui32 clientWidth, Nui32 clientHeight, NIIi * width, NIIi * height);
        
        /// Return the target window style depending on the fullscreen parameter.
        DWORD getWindowStyle(bool fullScreen) const;

        void notifyResolutionChanged(void);
    protected:
		/// @copydetails ViewWindow::setup
        //void setup(const String & name, NCount wpixel, NCount hpixel, bool full, const PropertyData * params);

		/// @copydetails ViewWindow::setSwapInterval
        void setSwapInterval(NCount interval);

		/// @copydetails ViewWindow::notify
		virtual void notify();

		/// @copydetails ViewWindow::swap
        void swap(bool vsync);

		/// @copydetails FrameObj::fill
		//virtual void fill(FaceType type, const Box & src, PixelBlock & out);
	protected:
        /// 更新窗体区域
        void updateWindowRect();

        /// 获取窗体类型
        inline DWORD getWindowStyle(bool full) const
        {
            return full ? mFullscreenWinStyle : mWindowedWinStyle;
        }
	protected:
        Win32GLSupport & mGLSupport;
		HWND mHWnd;
		HDC mHDC;                           ///< 设备接口
		HGLRC mGlrc;                        ///< GL设备
		Ntchar * mDeviceName;				///< 所使用的显示器的设备名称
        DWORD mKBMark;                      ///< 键盘设置 DISCL_BACKGROUND DISCL_EXCLUSIVE DISCL_FOREGROUND DISCL_NONEXCLUSIVE DISCL_NOWINKEY
        DWORD mMSMark;                      ///< 鼠标设置 DISCL_BACKGROUND DISCL_EXCLUSIVE DISCL_FOREGROUND DISCL_NONEXCLUSIVE DISCL_NOWINKEY
        DWORD mJDMark;                      ///< 游戏设备设置 DISCL_BACKGROUND DISCL_EXCLUSIVE DISCL_FOREGROUND DISCL_NONEXCLUSIVE DISCL_NOWINKEY
        Nui32 mVSyncInterval;
        //NIIi mDisplayFrequency;				// fullscreen only, to restore display
        Win32Context * mContext;
        DWORD mWindowedWinStyle;			// Windowed mode window style flags.
        DWORD mFullscreenWinStyle;			// Fullscreen mode window style flags.
		bool mIsExternalGLControl;
        bool mOwnsGLContext;
		bool mSizing;
		bool mClosed;
		bool mHidden;
		bool mVSync;
		bool mIsExternal;
        bool mVisible;
        bool mHwGamma;

        static bool mClassRegistered;
    };
}

#endif
