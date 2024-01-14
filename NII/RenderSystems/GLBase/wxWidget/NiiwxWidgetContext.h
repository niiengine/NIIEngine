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

#ifndef _NII_wxWidgetContext_H_
#define _NII_wxWidgetContext_H_

#include "NiiGLPreInclude.h"
#include "NiiGLContext.h"
#include "wx/glcanvas.h"

namespace NII
{
    /// wxWidget平台中的gl设备上下文
    class _EngineGLAPI wxWidgetContext : public GLContext, public GPUAlloc
    {
    public:
        wxWidgetContext(wxGLCanvas * dc);

        virtual ~wxWidgetContext();

        /// @copydetails GLContext::setCurrent
        virtual void setCurrent();

		/// @copydetails GLContext::endCurrent
		virtual void endCurrent();

		/// @copydetails GLContext::releaseContext
		virtual void releaseContext();

		/// @copydoc GLContext::clone
		GLContext * clone() const;
	protected:
        wxGLCanvas * mHDC;
        wxGLContext * mContext;
    };
}
#endif