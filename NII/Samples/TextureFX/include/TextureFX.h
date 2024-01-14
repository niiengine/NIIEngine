#ifndef __TextureFX_H__
#define __TextureFX_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_TextureFX : public SdkSample
{
public:
	Sample_TextureFX()
	{
		mInfo["Title"] = "Texture Effects";
		mInfo["Description"] = "Demonstrates OGRE's many different animated texture effects.";
		mInfo["Thumbnail"] = "thumb_texfx.png";
		mInfo["Category"] = "Unsorted";
		mInfo["Help"] = "Top Left: Multi-frame\nTop Right: Scrolling\nBottom Left: Rotation\nBottom Right: Scaling";
        mBoxSky = 0;
    }
    ~Sample_TextureFX()
    {
        if(mBoxSky)
        {
            mSceneMgr->removeSky(mBoxSky);
            N_delete mBoxSky;
            mBoxSky = 0;
        }
    }
protected:
	void setupContent()
	{
        mBoxSky = N_new BoxSky(mSceneMgr);
        mBoxSky->setMaterial("Examples/TrippySkyBox");
        mBoxSky->setRenderGroup(RQG_Sky);
		mBoxSky->buildGeo(5000);
        mSceneMgr->addSky(mBoxSky);
		// set our camera to orbit around the origin and show cursor
		mCameraMan->setStyle(CS_ORBIT);
		mTrayMgr->showCursor();

		// the names of the four materials we will use
		String matNames[] = {"Examples/OgreDance", "Examples/OgreParade", "Examples/OgreSpin", "Examples/OgreWobble"};

		for(unsigned int i = 0; i < 4; i++)
		{
			// create a standard plane entity
			GeometrySpace * ent = mSceneMgr->createGeo("Plane" + StrConv::conv(i + 1), GSP_Plane);

			// attach it to a node, scale it, and position appropriately
			SpaceNode * node = mSceneMgr->getOriginNode()->create();
			node->setPos(Vector3f(i % 2 ? 25 : -25, i / 2 ? -25 : 25, 0));
			node->setScale(Vector3f(0.25, 0.25, 0.25));
			node->attachObject(ent);

			ent->setMaterialName(matNames[i]);  // give it the material we prepared
		}
	}
protected:
    BoxSky * mBoxSky;
};

#endif
