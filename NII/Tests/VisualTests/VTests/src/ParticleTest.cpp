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

#include "ParticleTest.h"

String VisualTest::TRANSIENT_RESOURCE_GROUP = "VisualTestTransient";

ParticleTest::ParticleTest()
{
    mInfo["Title"] = "VTests_Particles";
    mInfo["Description"] = "Tests basic particle system functionality.";
    // take screenshot early, when emitters are just beginning
    addScreenshotFrame(35);
    // and another after particles have died, extra emitters emitted, etc
    addScreenshotFrame(500);
}
//---------------------------------------------------------------------------

void ParticleTest::setupContent()
{
    // create a bunch of random particle systems
    Ogre::ParticleSpace* ps = mSceneMgr->createParticleSpace("Fireworks", "Examples/Fireworks");
    mSceneMgr->getOriginNode()->attachObject(ps);
    
    mCamera->setPos(NII::Vector3f(0,150,500));
}
//-----------------------------------------------------------------------


