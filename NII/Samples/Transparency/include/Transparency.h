#ifndef __Transparency_H__
#define __Transparency_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

/* NOTE: This sample simply displays an object with a transparent material. The really relevant stuff
is all in the material script itself. You won't find anything even vaguely related to transparency in
this source code. Check out the Examples/WaterStream material in Examples.material. */

class _OgreSampleClassExport Sample_Transparency : public SdkSample
{
public:
	Sample_Transparency()
	{
		mInfo["Title"] = "Transparency";
		mInfo["Description"] = "Demonstrates the use of transparent materials (or scene blending).";
		mInfo["Thumbnail"] = "thumb_trans.png";
		mInfo["Category"] = "Lighting";
	}

	bool onRenderCore(const RenderFrameArgs * evt)
	{
		NIIf theta = mRoot->getTimer()->getMS() / 1000.0f;

		// this is the equation for a PQ torus knot
		Ogre::NIIf r = 28 * (2 + Math::Sin(theta * 3 / 2 + 0.2));
		Ogre::NIIf x = r * Math::Cos(theta);
		Ogre::NIIf y = r * Math::Sin(theta);
		Ogre::NIIf z = 60 * Math::Cos(theta * 3 / 2 + 0.2);

		Vector3 lastPos = mFishNode->getPos();   // save fishy's last position
		mFishNode->setPos(Vector3f(x, y, z));              // set fishy's new position

		// set fishy's direction based on the change in position
		mFishNode->setDirection(mFishNode->getPos() - lastPos, PosNode::SM_Env, Vector3::INV_X);

		mFishSwim->addTime(evt->mCurrent * 5);   // update fishy's swimming animation

		return SdkSample::onRenderCore(evt);   // don't forget the parent class updates!
	}
protected:
	void setupContent()
	{     
        BoxSky * mBoxSky = N_new BoxSky(mSceneMgr);
        mBoxSky->setMaterial();
        mBoxSky->setRenderGroup(RQG_Sky);
		mSceneMgr->setSkyBox(true, "Examples/TrippySkyBox");

		mCamera->setPos(Vector3f(0, 0, 300));   // set camera's starting position

        mSceneMgr->createLight(0)->setPos(20, 80, 50);   // add basic point light

		// create a torus knot model, give it the translucent texture, and attach it to the origin
		GeometrySpace* ent = mSceneMgr->createGeo("Knot", "knot.mesh");
        ent->setMaterialName("Examples/WaterStream");
		mSceneMgr->getOriginNode()->attachObject(ent);

		// create a fishy and enable its swimming animation
		ent = mSceneMgr->createGeo("Fish", "fish.mesh");
		mFishSwim = ent->getFusion("swim");
		mFishSwim->setEnabled(true);
		
		// create a scene node, attach fishy to it, and scale it by a factor of 2
		mFishNode = mSceneMgr->getOriginNode()->create();
		mFishNode->attachObject(ent);
		mFishNode->setScale(Vector3f(2, 2, 2));
	}

	SpaceNode* mFishNode;
	AnimationFusion* mFishSwim;
};

#endif
