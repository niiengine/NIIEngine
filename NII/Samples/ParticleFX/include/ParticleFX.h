#ifndef __ParticleFX_H__
#define __ParticleFX_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_ParticleFX : public SdkSample
{
public:
	Sample_ParticleFX()
	{
		mInfo["Title"] = "Particle Effects";
		mInfo["Description"] = "Demonstrates the creation and usage of particle effects.";
		mInfo["Thumbnail"] = "thumb_particles.png";
		mInfo["Category"] = "Effects";
		mInfo["Help"] = "Use the checkboxes to toggle visibility of the individual particle systems.";
	}

    bool onRenderCore(const RenderFrameArgs * evt)
    {
		mFountainPivot->yaw(Angle(evt->mCurrent * 30));   // spin the fountains around

		return SdkSample::onRenderCore(evt);   // don't forget the parent class updates!
    }

	void checkBoxToggled(CheckBox* box)
	{
		// show or hide the particle system with the same name as the check box
		mSceneMgr->getParticleSpace(box->getName())->setVisible(box->isChecked());
	}

protected:

	void setupContent()
	{
		// setup some basic lighting for our scene
		mSceneMgr->setAmbient(Colour(0.3, 0.3, 0.3));
		mSceneMgr->createLight(0)->setPos(20, 80, 50);

		// set our camera to orbit around the origin and show cursor
		mCameraMan->setStyle(CS_ORBIT);
		mCameraMan->setYawPitchDist(Angle(0), Angle(15), 250);
		mTrayMgr->showCursor();

		// create an ogre head entity and place it at the origin
        GeometrySpace* ent = mSceneMgr->createGeo("Head", "ogrehead.mesh");
        mSceneMgr->getOriginNode()->attachObject(ent);
		
		setupParticles();   // setup particles
		setupTogglers();    // setup particle togglers
	}

	void setupParticles()
	{
		ParticleSpace::setDefaultNonVisibleUpdateTimeout(5);  // set nonvisible timeout

		ParticleSpace* ps;
        Quaternion temp;
        // create some nice fireworks and place it at the origin
        ps = mSceneMgr->createParticleSpace("Fireworks", "Examples/Fireworks");
		mSceneMgr->getOriginNode()->attachObject(ps);

        // create a green nimbus around the ogre head
        ps = mSceneMgr->createParticleSpace("Nimbus", "Examples/GreenyNimbus");
		mSceneMgr->getOriginNode()->attachObject(ps);
       
        ps = mSceneMgr->createParticleSpace("Rain", "Examples/Rain");  // create a rainstorm
        ps->_forward(5);   // fast-forward the rain so it looks more natural
        static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create(Vector3(0, 1000, 0)))->attachObject(ps);

        // create aureola around ogre head perpendicular to the ground
        ps = mSceneMgr->createParticleSpace("Aureola", "Examples/Aureola");
        mSceneMgr->getOriginNode()->attachObject(ps);

        // create shared pivot node for spinning the fountains
        mFountainPivot = mSceneMgr->getOriginNode()->create();

        ps = mSceneMgr->createParticleSpace("Fountain1", "Examples/PurpleFountain");  // create fountain 1
        temp->from(Angle(20), Vector3::Z);
        // attach the fountain to a child node of the pivot at a distance and angle
		static_cast<SpaceNode *>(mFountainPivot->create(Vector3(200, -100, 0), temp))->attachObject(ps);
        temp->from(Angle(-20), Vector3::Z);
        ps = mSceneMgr->createParticleSpace("Fountain2", "Examples/PurpleFountain");  // create fountain 2
        // attach the fountain to a child node of the pivot at a distance and angle
		static_cast<SpaceNode *>(mFountainPivot->create(Vector3(-200, -100, 0), temp))->attachObject(ps);
	}

	void setupTogglers()
	{
		// create check boxes to toggle the visibility of our particle systems
		mTrayMgr->createLabel(TL_TOPLEFT, "VisLabel", "Particles");
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Fireworks", "Fireworks", 130)->setChecked(true);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Fountain1", "Fountain A", 130)->setChecked(true);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Fountain2", "Fountain B", 130)->setChecked(true);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Aureola", "Aureola", 130)->setChecked(false);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Nimbus", "Nimbus", 130)->setChecked(false);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Rain", "Rain", 130)->setChecked(false);
	}

	SpaceNode* mFountainPivot;
};

#endif
