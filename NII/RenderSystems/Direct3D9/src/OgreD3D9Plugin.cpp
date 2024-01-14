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

#include "OgreD3D9Plugin.h"
#include "OgreRoot.h"

#ifdef __MINGW32__
extern "C" {   
#include "OgreMinGWSupport.h"
void _chkstk();
}   
#endif

namespace Ogre 
{
    const String sPluginName = "D3D9 RenderSystem";
    //---------------------------------------------------------------------
    D3D9Plugin::D3D9Plugin()
        : mRenderSys(0)
    {

    }
    //---------------------------------------------------------------------
    const String& D3D9Plugin::getName() const
    {
        return sPluginName;
    }
    
    //---------------------------------------------------------------------
    void D3D9Plugin::install()
    {
        // When building with MinGW, we need to call dummy implementations
        // of missing MinGW DirectX functions to make sure they are carried over
        // in dynamic AND static builds
#ifdef __MINGW32__
    _chkstk();
    __security_check_cookie((intptr_t)NULL);    
#endif
    
        // Create the DirectX 9 rendering api
		HINSTANCE hInst = NULL;
		#ifdef __MINGW32__
			#ifdef OGRE_STATIC_LIB
        		hInst = GetModuleHandle( NULL );
			#else
				#if OGRE_DEBUG_MODE == 1
					hInst = GetModuleHandle("RenderSystem_Direct3D9_d.dll");
				#else
					hInst = GetModuleHandle("RenderSystem_Direct3D9.dll");
				#endif
			#endif
		#else
			static const TCHAR staticVar;
			GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, &staticVar, &hInst);
		#endif

        mRenderSys = N_new D3D9RenderSystem( hInst );
        // Register the render system
        Root::getSingleton().addRenderSystem( mRenderSys );
    }
    //---------------------------------------------------------------------
    void D3D9Plugin::initialise()
    {
        // nothing to do
    }
    //---------------------------------------------------------------------
    void D3D9Plugin::shutdown()
    {
        // nothing to do
    }
    //---------------------------------------------------------------------
    void D3D9Plugin::uninstall()
    {
        if (mRenderSys != NULL)
        {
            N_delete mRenderSys;
            mRenderSys = NULL;
        }               
    }


}
