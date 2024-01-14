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

#include "VTestPlugin.h"
#include "StencilShadowTest.h"
#include "ParticleTest.h"
#include "TransparencyTest.h"
#include "TextureEffectsTest.h"
#include "CubeMappingTest.h"
#include "OgreResourceLoadManager.h"

VTestPlugin::VTestPlugin()
    :SamplePlugin("VTestPlugin")
{
    // add the playpen tests
    addSample(new ParticleTest());
    addSample(new StencilShadowTest());
    addSample(new TransparencyTest());
    addSample(new CubeMappingTest());
    addSample(new TextureEffectsTest());
}
//---------------------------------------------------------------------

VTestPlugin::~VTestPlugin()
{
    for (OgreBites::SampleSet::iterator i = mSamples.begin(); i != mSamples.end(); ++i)
    {
        delete *i;
    }
    mSamples.clear();
}
//---------------------------------------------------------------------

#ifndef OGRE_STATIC_LIB

VTestPlugin* testPlugin = 0;

extern "C" _OgreSampleExport void dllStartPlugin()
{
    testPlugin = N_new VTestPlugin();
    Ogre::Root::getSingleton().installPlugin(testPlugin);
}

extern "C" _OgreSampleExport void dllStopPlugin()
{
    Ogre::Root::getSingleton().uninstallPlugin(testPlugin); 
    N_delete testPlugin;
}

#endif
