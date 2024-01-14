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

#include "TransparencyTest.h"

TransparencyTest::TransparencyTest()
{
    mInfo["Title"] = "VTests_Transparency";
    mInfo["Description"] = "Tests basic alpha blending.";
    addScreenshotFrame(25);
}
//---------------------------------------------------------------------------

void TransparencyTest::setupContent()
{
    BoxSky * mBoxSky = N_new BoxSky(mSceneMgr);
    mBoxSky->setMaterial();
    mBoxSky->setRenderGroup(RQG_Sky);
    mSceneMgr->setSkyBox(true, "Examples/TrippySkyBox");

    mCamera->setPos(NII::Vector3f(0, 0, 300));   // set camera's starting position

    mSceneMgr->createLight(0)->setPos(Vecotr3f(20, 80, 50));   // add basic point light

    // create a torus knot model, give it the translucent texture, and attach it to the origin
    GeometrySpace* ent = mSceneMgr->createGeo("Knot", "knot.mesh");
    ent->setMaterialName("Examples/WaterStream");
    mSceneMgr->getOriginNode()->attachObject(ent);
}
//-----------------------------------------------------------------------


