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

#ifndef _NII_W32Context_H_
#define _NII_W32Context_H_

#include "NiiWin32PreInclude.h"
#include "NiiGLContext.h"

namespace NII
{
    /// win32平台中的gl设备上下文
    class _EngineGLAPI Win32Context : public GLContext, public GPUAlloc
    {
    public:
        Win32Context(HDC HDC, HGLRC Glrc);

        virtual ~Win32Context();

        /// @copydetails GLContext::setCurrent
        virtual void setCurrent();

		/// @copydetails GLContext::endCurrent
		virtual void endCurrent();

		/// @copydetails GLContext::releaseContext
		virtual void releaseContext();

		/// @copydoc GLContext::clone
		GLContext * clone() const;
	protected:
        HDC     mHDC;   ///< 设备器
        HGLRC   mGlrc;  ///< 资源器
    };
}
#endif