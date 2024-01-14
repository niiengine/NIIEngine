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

#include <QuickTime/QuickTime.h>
#include "OgreLogManager.h"
#include "OgreConfigDialog.h"

namespace NII 
{
	ConfigDialog* dlg = NULL;
	ConfigDialog::ConfigDialog()
	{
		dlg = this;
	}

	ConfigDialog::~ConfigDialog()
	{
	}

	void ConfigDialog::init()
	{
	}

	bool ConfigDialog::display()
	{
		// TODO: Fix OS X Config dialog
		const RenderSysList & renderers = N_Engine().getRenderList();
		RenderSys * renderer = renderers.front();

		// WARNING: restoreConfig() should not be invoked here as Engine calls
		// it before this method anyway, and invoking restoreConfig() here
		// forces the client application to use Ogre.cfg, while it may have
		// different plans.
		if(!N_Engine().restoreConfig())
		{
			// Set some defaults
			renderer->setConfig("Video Mode", "800 x 600");
			renderer->setConfig("Colour Depth", "32");
			renderer->setConfig("FSAA", "0");
			renderer->setConfig("Full Screen", "No");
			renderer->setConfig("RenderTarget Mode", "FBO");
			// Set the rendersystem and save the config.
			N_Engine().set(renderer);
		}
		return true;
	}

};
