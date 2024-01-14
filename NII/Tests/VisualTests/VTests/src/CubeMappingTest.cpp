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

#include "CubeMappingTest.h"

CubeMappingTest::CubeMappingTest()
{
    mInfo["Title"] = "VTests_CubeMapping";
    mInfo["Description"] = "Tests basic fixed-function cube mapping.";
    addScreenshotFrame(10);
}
//---------------------------------------------------------------------------
void CubeMappingTest::setupContent()
{
    BoxSky * mBoxSky = N_new BoxSky(mSceneMgr);
    mBoxSky->setMaterial();
    mBoxSky->setRenderGroup(RQG_Sky);
    mSceneMgr->setSkyBox(true, "Examples/MorningSkyBox");
    Ogre::SpaceNode* node = mSceneMgr->getOriginNode()->create();
    Ogre::GeometrySpace* head = mSceneMgr->createGeo(0, "ogrehead.mesh");
    node->attachObject(head);
    head->setMaterialName("Examples/MorningCubeMap");
    mCamera->setPos(NII::Vector3f(0,0,100));
}
//-----------------------------------------------------------------------