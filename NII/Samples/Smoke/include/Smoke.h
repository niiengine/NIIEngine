#ifndef __Smoke_H__
#define __Smoke_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_Smoke : public SdkSample
{
public:
	Sample_Smoke()
	{
		mInfo["Title"] = "Smoke";
		mInfo["Description"] = "Demonstrates depth-sorting of particles in particle systems.";
		mInfo["Thumbnail"] = "thumb_smoke.png";
		mInfo["Category"] = "Effects";
        mBoxSky = 0;
	}
    ~Sample_Smoke()
    {
        if(mBoxSky)
        {
            mSceneMgr->removeSky(mBoxSky);
            N_delete mBoxSky;
            mBoxSky = 0;
        }
    }
	bool onRenderCore(const RenderFrameArgs * evt)
	{
		// spin the head around and make it float up and down
		mPivot->setPos(Vector3f(0, Math::Sin(mRoot->getTimer()->getMS() / 150.0) * 10, 0));
		mPivot->yaw(Radian(-evt->mCurrent * 1.5));
		return SdkSample::onRenderCore(evt);
	}
protected:
	void setupContent()
	{     
        BoxSky * mBoxSky = N_new BoxSky(mSceneMgr);
        mBoxSky->setMaterial("Examples/EveningSkyBox");
        mBoxSky->setRenderGroup(RQG_Sky);
		mBoxSky->buildGeo(5000);
        mSceneMgr->addSky(mBoxSky);
        
		// dim orange ambient and two bright orange lights to match the skybox
		mSceneMgr->setAmbient(Colour(0.3, 0.2, 0));
		Light* light = mSceneMgr->createLight(0);
		light->setPos(2000, 1000, -1000);
		light->setDiffuse(1, 0.5, 0);
		light = mSceneMgr->createLight(0);
        light->setPos(-2000, 1000, 1000);
		light->setDiffuse(1, 0.5, 0);

		mPivot = mSceneMgr->getOriginNode()->create();  // create a pivot node

		// create a child node and attach an ogre head and some smoke to it
		SpaceNode* headNode = mPivot->create(Vector3(100, 0, 0));
		headNode->attachObject(mSceneMgr->createGeo("Head", "ogrehead.mesh"));
        headNode->attachObject(mSceneMgr->createParticleSpace("Smoke", "Examples/Smoke"));

		mCamera->setPos(Vector3f(0, 30, 350));
	}

	SpaceNode * mPivot;
    BoxSky * mBoxSky;
};

#endif
