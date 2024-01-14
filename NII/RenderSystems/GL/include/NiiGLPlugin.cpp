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

#include "NiiGLPlugin.h"
#include "NiiEngine.h"
#include "NiiGLRenderSystem.h"

namespace NII
{
	//---------------------------------------------------------------------
	GLPlugin::GLPlugin() : Plugin(1, T_Render_System), mRSys(0){}
	//---------------------------------------------------------------------
	String GLPlugin::getName() const
	{
		return _T("GL Render version 3-4");
	}
	//---------------------------------------------------------------------
	void GLPlugin::install()
	{
		mRSys = N_new GLRenderSystem();
		Engine::getOnly().addRender(mRSys);
	}
	//---------------------------------------------------------------------
	void GLPlugin::init()
	{
		// nothing to do
	}
	//---------------------------------------------------------------------
	void GLPlugin::shutdown()
	{
		// nothing to do
	}
	//---------------------------------------------------------------------
	void GLPlugin::uninstall()
	{
		N_delete mRSys;
		mRSys = 0;
	}
    //-----------------------------------------------------------------------
}