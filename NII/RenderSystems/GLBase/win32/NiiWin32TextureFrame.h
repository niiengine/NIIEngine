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

#ifndef _NII_Win32RT_H_
#define _NII_Win32RT_H_

#include "NiiWin32PreInclude.h"
#include "NiiWin32Context.h"
#include "NiiGLPBuffer.h"

namespace NII 
{
    class _EngineGLAPI Win32PBuffer : public GLPBuffer, public GPUAlloc
    {
	public:
		Win32PBuffer(PixelDataType format, NCount w, NCount h);
		~Win32PBuffer();
	    
		/// @copydetails GLPBuffer::getContext
        GLContext * getContext() const { return mContext; }
    protected:
        void createPBuffer();
		void destroyPBuffer();
	protected:
		HDC		mHDC;
		HGLRC	mGlrc;
		HPBUFFERARB mPBuffer;
        Win32Context * mContext;
    };
}
#endif