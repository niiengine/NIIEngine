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
#include "OgreRoot.h"
#include "OgreHighLevelGpuProgramManager.h"
#include "OgreCgFxScriptLoader.h"

namespace Ogre 
{
	const String sPluginName = "Cg Program Manager";
	//---------------------------------------------------------------------
	CgPlugin::CgPlugin()
		:mCgProgramFactory(0)
	{

	}
	//---------------------------------------------------------------------
	const String& CgPlugin::getName() const
	{
		return sPluginName;
	}
	//---------------------------------------------------------------------
	void CgPlugin::install()
	{
	}
	//---------------------------------------------------------------------
	void CgPlugin::init()
	{
        // check for gles2 by the glsles factory (this plugin is not supported on embedded systems for now)
        if (HighLevelGpuProgramManager::getSingleton().isShaderSupport(SL_GLSLES) == false)
        {
            // Create new factory
            mCgProgramFactory = N_new CgProgramFactory();

            // Register
            HighLevelGpuProgramManager::getSingleton().add(mCgProgramFactory);

            N_new CgFxScriptLoader();
        }
	}
	//---------------------------------------------------------------------
	void CgPlugin::shutdown()
	{
		// nothing to do
	}
	//---------------------------------------------------------------------
	void CgPlugin::uninstall()
	{
        if (mCgProgramFactory)
        {
			N_delete CgFxScriptLoader::getSingletonPtr(); 

            // Remove from manager safely
            if (HighLevelGpuProgramManager::getSingletonPtr())
                HighLevelGpuProgramManager::getSingleton().remove(mCgProgramFactory);
		    N_delete mCgProgramFactory;
		    mCgProgramFactory = 0;
        }
	}
}
