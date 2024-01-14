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

#include "TextureEffectsTest.h"

TextureEffectsTest::TextureEffectsTest()
{
    mInfo["Title"] = "VTests_TextureEffects";
    mInfo["Description"] = "Tests texture effects.";
    addScreenshotFrame(50);
}
//---------------------------------------------------------------------------

void TextureEffectsTest::setupContent()
{
    mViewport->setBgColour(Colour(0.8,0.8,0.8));

    // the names of the four materials we will use
    String matNames[] = {"Examples/OgreDance", "Examples/OgreParade", "Examples/OgreSpin", "Examples/OgreWobble"};

    for (unsigned int i = 0; i < 4; i++)
    {
        // create a standard plane entity
        GeometrySpace* ent = mSceneMgr->createGeo("Plane" + StrConv::conv(i + 1), GSP_Plane);

        // attach it to a node, scale it, and position appropriately
        SpaceNode * node = mSceneMgr->getOriginNode()->create();
        node->setPos(Vecotr3f(i % 2 ? 25 : -25, i / 2 ? -25 : 25, 0));
        node->setScale(Vector3f(0.25, 0.25, 0.25));
        node->attachObject(ent);

        ent->setMaterialName(matNames[i]);  // give it the material we prepared
    }

    mCamera->setPos(NII::Vector3f(0, 0, 125));
    mCamera->setDirection(NII::Vector3f(0, 0, -1));
}
//-----------------------------------------------------------------------


