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
#define _WIN32_WINNT 0x0500
#endif

#include "NiiWin32Context.h"
#include "NiiGLRenderSystem.h"
#include "NiiEngine.h"

namespace NII
{
    //-----------------------------------------------------------------------
    Win32Context::Win32Context(HDC HDC, HGLRC Glrc, uint32 major, uint32 minor) :
        mHDC(HDC),
        mGlrc(Glrc),
        mContexMajorVersion( major ),
        mContexMinorVersion( minor )
    {
    }
    //-----------------------------------------------------------------------
    Win32Context::~Win32Context()
    {
		// NB have to do this is subclass to ensure any methods called back
		// are on this subclass and not half-destructed superclass
		GLRenderSystem * rs = static_cast<GLRenderSystem *>(Engine::getOnly().getRender());
		rs->_unregisterContext(this);
    }
    //-----------------------------------------------------------------------
    void Win32Context::setCurrent()
    {
         wglMakeCurrent(mHDC, mGlrc);
    }
    //-----------------------------------------------------------------------
	void Win32Context::endCurrent()
	{
		wglMakeCurrent(NULL, NULL);
	}
    //-----------------------------------------------------------------------
	GLContext * Win32Context::clone() const
	{
        const int attribList[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, static_cast<int>( mContexMajorVersion ),
            WGL_CONTEXT_MINOR_VERSION_ARB, static_cast<int>( mContexMinorVersion ),
        #if N_DEBUG
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
        #endif
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0, 0
        };

        // Create new context based on own HDC (shared with ours)
        HGLRC newCtx = wglCreateContextAttribsARB( mHDC, mGlrc, attribList );
        
        if (!newCtx)
        {
            N_EXCEPT(Internal, "Error calling wglCreateContextAttribsARB: " + translateWGLError());
        }

        return new Win32Context( mHDC, newCtx, mContexMajorVersion, mContexMinorVersion );
	}
    //-----------------------------------------------------------------------
	void Win32Context::releaseContext()
	{
		if(mGlrc != NULL)
		{
			wglDeleteContext(mGlrc);
			mGlrc = NULL;
			mHDC  = NULL;
		}
	}
    //-----------------------------------------------------------------------
}
/*#if NII_THREAD_SUPPORT == 1
    //-----------------------------------------------------------------------
    // declared in NiiGLPreInclude.h
    WGLEWContext * wglewGetContext()
    {
        using namespace NII;
        static NII_THREAD_POINTER_VAR(WGLEWContext, WGLEWContextsPtr);

        WGLEWContext * currentWGLEWContextsPtr = N_Thread_Get_Obj(WGLEWContextsPtr);
        if (currentWGLEWContextsPtr == NULL)
        {
            currentWGLEWContextsPtr = N_new WGLEWContext();
            N_Thread_Set_Obj(WGLEWContextsPtr, currentWGLEWContextsPtr);
            ZeroMemory(currentWGLEWContextsPtr, sizeof(WGLEWContext));
            wglewInit();
        }
        return currentWGLEWContextsPtr;
    }
    //-----------------------------------------------------------------------
#endif*/