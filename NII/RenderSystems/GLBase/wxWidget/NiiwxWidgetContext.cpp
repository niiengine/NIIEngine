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

#include "NiiwxWidgetContext.h"
#include "NiiGLRenderSystem.h"
#include "NiiEngine.h"

namespace NII
{
    //-----------------------------------------------------------------------
    wxWidgetContext::wxWidgetContext(wxGLCanvas * dc) :
        mHDC(dc)
    {
        mContext = new wxGLContext(mHDC);
    }
    //-----------------------------------------------------------------------
    wxWidgetContext::~wxWidgetContext()
    {
		// NB have to do this is subclass to ensure any methods called back
		// are on this subclass and not half-destructed superclass
		GLRenderSystem * rs = static_cast<GLRenderSystem *>(Engine::getOnly().getRender());
		rs->_unregisterContext(this);
    }
    //-----------------------------------------------------------------------
    void wxWidgetContext::setCurrent()
    {
        mContext->SetCurrent(*mHDC);
    }
    //-----------------------------------------------------------------------
	void wxWidgetContext::endCurrent()
	{
        wglMakeCurrent(NULL, NULL);
	}
    //-----------------------------------------------------------------------
	GLContext * wxWidgetContext::clone() const
	{
		return N_new wxWidgetContext(mHDC);
	}
    //-----------------------------------------------------------------------
	void wxWidgetContext::releaseContext()
	{
		if(mContext != NULL)
		{
            delete mContext;
		}
	}
    //-----------------------------------------------------------------------
}