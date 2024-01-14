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



#include "Ogre.h"
#include "OgreRenderSystemCapabilitiesSerializer.h"
#include <iostream>
#include <sys/stat.h>

using namespace std;
using namespace Ogre;


void help(void)
{
    // Print help message
    cout << endl << "rcapsdump: Queries GPU capabilities and dumps them into .gpufeature files" << endl;

    cout << endl << "Usage: rcapsdump" << endl;
}



void setUpGLRenderSystemOptions(Ogre::RenderSystem* rs)
{
	using namespace Ogre;
	SysOptionList options = rs->getConfig();
	// set default options
	// this should work on every semi-normal system
	rs->setConfig(String("Colour Depth"), String("32"));
	rs->setConfig(String("FSAA"), String("0"));
	rs->setConfig(String("Full Screen"), String("No"));
	rs->setConfig(String("VSync"), String("No"));
	rs->setConfig(String("Video Mode"), String("800 x 600"));
	
	// use the best RTT
	SysOption optionRTT = options["RenderTarget Mode"];
	
	if(find(optionRTT.mReserve.begin(), optionRTT.mReserve.end(), "FBO") != optionRTT.mReserve.end())
	{
		rs->setConfig(String("RenderTarget Mode"), String("FBO"));
	}
	else if(find(optionRTT.mReserve.begin(), optionRTT.mReserve.end(), "PBuffer") != optionRTT.mReserve.end())
	{
		rs->setConfig(String("RenderTarget Mode"), String("PBuffer"));
	}
	else
		rs->setConfig(String("RenderTarget Mode"), String("Copy"));
}


void setUpD3D9RenderSystemOptions(Ogre::RenderSystem* rs)
{
	using namespace Ogre;
	SysOptionList options = rs->getConfig();
	// set default options
	// this should work on every semi-normal system
	rs->setConfig(String("Anti aliasing"), String("None"));
	rs->setConfig(String("Full Screen"), String("No"));
	rs->setConfig(String("VSync"), String("No"));
	rs->setConfig(String("Video Mode"), String("800 x 600 @ 32-bit colour"));
	
	// pick first available device
	SysOption optionDevice = options["Rendering Device"];

	rs->setConfig(optionDevice.name, optionDevice.mValue);
}


int main(int numargs, char** args)
{
    if (numargs != 1)
    {
        help();
        return -1;
    }
    
    RenderSysFeature* glCaps = 0;
    RenderSysFeature* d3d9Caps = 0;
    
    RenderSystemCapabilitiesSerializer serializer;
    
    // query openGL for caps if available
    Root* root = new Root("plugins.cfg");
    RenderSystem* rsGL = root->getRenderSystemByName("OpenGL Rendering Subsystem");
    if(rsGL)
    {
        setUpGLRenderSystemOptions(rsGL);
		root->set(rsGL);
		root->init(true, "OGRE rcapsdump GL Window");
		glCaps = const_cast<RenderSysFeature *>(rsGL->getFeature());	
    }
    if(glCaps)
    {
        serializer.writeScript(glCaps, glCaps->getGpuInfo()->getName(), "rcapsdump_gl.gpufeature");
    }
    
    delete root;
    
    // query D3D9 for caps if available
    root = new Root("plugins.cfg");
    RenderSystem* rsD3D9 = root->getRenderSystemByName("Direct3D9 Rendering Subsystem");
    if(rsD3D9)
    {
        setUpD3D9RenderSystemOptions(rsD3D9);
		root->set(rsD3D9);
		root->init(true, "OGRE rcapsdump D3D9 Window");
		d3d9Caps = const_cast<RenderSysFeature *>(rsD3D9->getFeature());
    }
    if(d3d9Caps)
    {
        serializer.writeScript(d3d9Caps, d3d9Caps->getGpuInfo()->getName(), "rcapsdump_d3d9.gpufeature");
    }

    delete root;
  
    return 0;

}

