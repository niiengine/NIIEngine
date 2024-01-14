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

#include "OgreCgPlugin.h"
#include "OgreCgProgram.h"
#include "OgreRoot.h"

namespace Ogre {

    CgPlugin* cgPlugin;
#ifndef OGRE_STATIC_LIB
    //-----------------------------------------------------------------------
    extern "C" void _OgreCgPluginExport dllStartPlugin(void)
    {
        // Create new plugin
        cgPlugin = N_new CgPlugin();

        // Register
        Root::getSingleton().installPlugin(cgPlugin);


    }
    extern "C" void _OgreCgPluginExport dllStopPlugin(void)
    {
		Root::getSingleton().uninstallPlugin(cgPlugin);
		N_delete cgPlugin;
    }
#endif

    void checkForCgError(const String& ogreMethod, const String& errorTextPrefix, CGcontext context)
    {
        CGerror error = cgGetError();
        if (error != CG_NO_ERROR)
        {
            String msg = errorTextPrefix + cgGetErrorString(error); 

            if (error == CG_COMPILER_ERROR)
            {
                // Get listing with full compile errors
                msg = msg + "\n" + cgGetLastListing(context);
            }
            N_EXCEPT(Exception::ERR_INTERNAL_ERROR, msg, ogreMethod);
        }
    }

}
