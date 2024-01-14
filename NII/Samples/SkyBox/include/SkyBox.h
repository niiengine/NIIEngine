#ifndef __SkyBox_H__
#define __SkyBox_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_SkyBox : public SdkSample
{
public:
	Sample_SkyBox()
	{
		mInfo["Title"] = "Sky Box";
		mInfo["Description"] = "Shows how to use skyboxes (fixed-distance cubes used for backgrounds).";
		mInfo["Thumbnail"] = "thumb_skybox.png";
		mInfo["Category"] = "Environment";
        mBoxSky = 0;
	}
    ~Sample_SkyBox()
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
		// setup some basic lighting for our scene
        mSceneMgr->setAmbient(Colour(0.3, 0.3, 0.3));
        mSceneMgr->createLight(0)->setPos(20, 80, 50);
        mBoxSky = N_new BoxSky(mSceneMgr);
        mBoxSky->setMaterial("Examples/SpaceSkyBox");
        mBoxSky->setRenderGroup(RQG_Sky);
        mBoxSky->buildGeo(5000);  // set our skybox
        mSceneMgr->addSky(mBoxSky);
		// create a spaceship model, and place it at the origin
        mSceneMgr->getOriginNode()->attachObject(mSceneMgr->createGeo("Razor", "razor.mesh"));

		// create a particle system with 200 quota, then set its material and dimensions
        ParticleSpace* thrusters = mSceneMgr->createParticleSpace(0,GroupDefault,25);
        thrusters->setMaterialName("Examples/Flare");
        thrusters->setParticleWidth(25);
        thrusters->setParticleHeight(25);
        
		// create two emitters for our thruster particle system
		for(unsigned int i = 0; i < 2; i++)
		{
			SpreadParticle * emitter = thrusters->addSpread("Point");  // add a point emitter

			// set the emitter properties
			emitter->setAngle(Angle(3));
            emitter->setMinTTL(0.5);
            emitter->setMaxTTL(0.5);
			emitter->setSpreadRate(25);
			emitter->setSpreadSpeed(25);
			emitter->setDirection(Vector3::INV_Z);
			emitter->setColour(Colour::White, Colour::Red);
			emitter->setPos(Vector3(i == 0 ? 5.7 : -18, 0, 0));
		}

		// attach our thruster particles to the rear of the ship
        static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create(Vector3(0, 6.5, -67)))->attachObject(thrusters);

		// set the camera's initial position and orientation
		mCamera->setPos(NII::Vector3f(0, 0, 150));
		mCamera->yaw(Angle(5));
	}
};

#endif