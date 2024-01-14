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
/*
The nVidia occlusion query extension is defined in glext.h so you don't
need anything else. You do need to look up the function, we provide a
GLSupport class to do this, which has platform implementations for
getProcAddress. Check the way that extensions like glActiveTextureARB are
initialised and used in glRenderSystem and copy what is done there.



  To do: fix so dx7 and DX9 checks and flags if HW Occlusion is supported
  See the openGl dito for ideas what to do.

*/

//GL_ActiveTextureARB_Func* glActiveTextureARB_ptr = (GL_ActiveTextureARB_Func)mGLSupport->getProcAddress("glActiveTextureARB");
#ifndef _NII_GL_PixelCountQuery_H_
#define _NII_GL_PixelCountQuery_H_

#include "NiiGLPreInclude.h"
#include "NiiPixelCountQuery.h"

namespace NII 
{
	/**
	@version NIIEngine 3.0.0
	*/
	class _EngineGLAPI GLPixelCountQuery : public PixelCountQuery
	{
	public:
		GLPixelCountQuery();
		~GLPixelCountQuery();

		/// @copydetails PixelCountQuery::beginRender
		void beginRender();

		/// @copydetails PixelCountQuery::endRender
		void endRender();

		/// @copydetails PixelCountQuery::query
		bool query(NCount & count); 

		/// @copydetails PixelCountQuery::isRun
		bool isRun();
	private:
		GLuint mQueryID;
        GLuint mLast;
	};
}
#endif 

