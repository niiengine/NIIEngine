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

#include "StencilShadowTest.h"
//---------------------------------------------------------------------------
StencilShadowTest::StencilShadowTest()
{
    mInfo["Title"] = "VTests_StencilShadows";
    mInfo["Description"] = "Tests basic stencil shadow functionality.";

    // take screenshot almost immediately, since the scene is static
    addScreenshotFrame(10);
}
//---------------------------------------------------------------------------
void StencilShadowTest::setupContent()
{
    // turn ambient light off
    mSceneMgr->setAmbient(Colour(0.0, 0.0, 0.0));

    // turn on stencil shadows
    mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

    // add a couple lights
    Ogre::Light * light = mSceneMgr->createLight("Light1");
    light->setDiffuse(0.5f,0.4f,0.35f);
    light->setSpecular(0, 0, 0);
    light->setRange(8000);
    light->setAttenuationConstant(1);
    light->setAttenuationLinear(0.0005);
    light->setAttenuationQuadric(0);
    light->setPos(Vecotr3f(220, 100, 0));
    light->setCastShadow(true);
    light->setType(LT_POINT);

    light = mSceneMgr->createLight("Light2");
    light->setDiffuse(0.5f,0.4f,0.35f);
    light->setSpecular(0, 0, 0);
    light->setRange(8000);
    light->setAttenuationConstant(1);
    light->setAttenuationLinear(0.0005);
    light->setAttenuationQuadric(0);
    light->setPos(Vecotr3f(220, 100, -200));
    light->setCastShadow(true);
    light->setType(LT_POINT);

    // create a ground plane to recieve some shadows
    Plane pln = MovablePlane("plane");
    pln.normal = Vector3::Y;
    pln.d = 107;
    MeshManager::getSingleton().createPlane("ground_plane", GroupDefault, pln,
        1500, 1500, 50, 50, true, 1, 5, 5, Vector3::Z);
    Ogre::GeometrySpace* groundPlane = mSceneMgr->createGeo("plane", "ground_plane");
    groundPlane->setMaterialName("Examples/Rocky");
    groundPlane->setCastShadow(false);
    static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create())->attachObject(groundPlane);

    // and a couple objects to cast the shadows
    Ogre::GeometrySpace* bar = mSceneMgr->createGeo( "barrel", "Barrel.mesh" );
    bar->setCastShadow(true);
    Ogre::SpaceNode* barNode = mSceneMgr->getOriginNode()->create();
    barNode->attachObject(bar);
    barNode->setScale(Vector3f(7,7,7));
    barNode->setPos(Ogre::Vector3(0,-85,-320));
    Ogre::GeometrySpace* head = mSceneMgr->createGeo( "ogrehead", "ogrehead.mesh" );
    head->setCastShadow(true);
    Ogre::SpaceNode* headNode = mSceneMgr->getOriginNode()->create();
    headNode->attachObject(head);
    headNode->setPos(Ogre::Vector3(-100,-80,-320));
    Ogre::GeometrySpace* torus = mSceneMgr->createGeo( "torus", "knot.mesh" );
    torus->setCastShadow(true);
    torus->setMaterialName("Examples/RustySteel");
    Ogre::SpaceNode* torusNode = mSceneMgr->getOriginNode()->create();
    torusNode->setScale(Vector3f(0.5,0.5,0.5));
    torusNode->attachObject(torus);
    torusNode->setPos(Ogre::Vector3(100,-60,-320));

    // point the camera down a bit
    mCamera->pitch(Ogre::Angle(-20.f));
}
//-----------------------------------------------------------------------