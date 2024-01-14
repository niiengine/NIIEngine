#ifndef __Fresnel_H__
#define __Fresnel_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_Fresnel : public SdkSample, public FrameObjListener
{
public:
	Sample_Fresnel() : NUM_FISH(30), NUM_FISH_WAYPOINTS(10), FISH_PATH_LENGTH(200), FISH_SCALE(2)
	{
		mInfo["Title"] = "Fresnel";
		mInfo["Description"] = "Shows how to create reflections and refractions using render-to-texture and shaders.";
		mInfo["Thumbnail"] = "thumb_fresnel.png";
		mInfo["Category"] = "Unsorted";
        mBoxSky = 0;
	}

    ~Sample_Fresnel()
    {
        if(mBoxSky)
        {
            mSceneMgr->removeSky(mBoxSky);
            N_delete mBoxSky;
            mBoxSky = 0;
        }
    }
    
	StringList getRequiredPlugins()
	{
		StringList names;
        if (!GpuProgramManager::getSingleton().isSyntaxSupported("glsles"))
            names.push_back("Cg Program Manager");
		return names;
	}

	void testCapabilities(const RenderSysFeature* caps)
	{
		if (!caps->hasCapability(GF_Program_Vertex) || !caps->hasCapability(GF_Program_Fragment))
        {
			N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your graphics card does not support vertex and fragment"
				" programs, so you cannot run this sample. Sorry!", "FresnelSample::testCapabilities");
        }

        if (!GpuProgramManager::getSingleton().isSyntaxSupported("arbfp1") &&
            !GpuProgramManager::getSingleton().isSyntaxSupported("ps_4_0") &&
            !GpuProgramManager::getSingleton().isSyntaxSupported("ps_2_0") &&
			!GpuProgramManager::getSingleton().isSyntaxSupported("ps_1_4") &&
            !GpuProgramManager::getSingleton().isSyntaxSupported("glsles"))
        {
			N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your graphics card does not support advanced fragment"
				" programs, so you cannot run this sample. Sorry!", "FresnelSample::testCapabilities");
        }
	}

    bool onRenderCore(const RenderFrameArgs * evt)
    {
		// update the fish spline path animations and loop as needed
        mFishAnimTime += evt->mCurrent;
        while (mFishAnimTime >= FISH_PATH_LENGTH) mFishAnimTime -= FISH_PATH_LENGTH;

        for (unsigned int i = 0; i < NUM_FISH; i++)
        {
            mFishAnimStates[i]->addTime(evt->mCurrent * 2);  // update fish swim animation

			// set the new position based on the spline path and set the direction based on displacement
			Vector3 lastPos = mFishNodes[i]->getPos();
            mFishNodes[i]->setPos(mFishSplines[i].interpolate(mFishAnimTime / FISH_PATH_LENGTH));
			mFishNodes[i]->setDirection(mFishNodes[i]->getPos() - lastPos, PosNode::SM_Env, Vector3::INV_X);
			mFishNodes[i]->setCustomYaw(true);
        }
        return SdkSample::onRenderCore(evt);
    }

    void onPreFlush(const FrameObjArgs * evt)
    {
        mWater->setVisible(false);  // hide the water

        if(evt->mFrame == mReflectionTarget)  // for reflection, turn on camera reflection and hide submerged entities
		{
			mCamera->setReflection(true, mWaterPlane);
			for(std::vector<GeometrySpace *>::iterator i = mSubmergedEnts.begin(); i != mSubmergedEnts.end(); i++)
				(*i)->setVisible(false);
		}
		else  // for refraction, hide surface entities
		{
			for(std::vector<GeometrySpace *>::iterator i = mSurfaceEnts.begin(); i != mSurfaceEnts.end(); i++)
				(*i)->setVisible(false);
		}
    }

    void onEndFlush(const FrameObjArgs * evt)
    {
        mWater->setVisible(true);  // unhide the water

        if(evt->mFrame == mReflectionTarget)  // for reflection, turn off camera reflection and unhide submerged entities
		{
			mCamera->setReflection(false);
			for (std::vector<GeometrySpace*>::iterator i = mSubmergedEnts.begin(); i != mSubmergedEnts.end(); i++)
				(*i)->setVisible(true);
		}
		else  // for refraction, unhide surface entities
		{
			for (std::vector<GeometrySpace*>::iterator i = mSurfaceEnts.begin(); i != mSurfaceEnts.end(); i++)
				(*i)->setVisible(true);
		}
    }
protected:
	void setupContent()
	{
        mCamera->setPos(NII::Vector3f(-50, 125, 760));
		mCameraMan->setTopSpeed(280);

        mSceneMgr->setAmbient(Colour(0.5, 0.5, 0.5));  // set ambient light
        mBoxSky = N_new BoxSky(mSceneMgr);
        mBoxSky->setMaterial("Examples/CloudyNoonSkyBox");
        mBoxSky->setRenderGroup(RQG_Sky);
        mBoxSky->buildGeo(5000);  // set a skybox
        mSceneMgr->addSky(mBoxSky);
        // make the scene's main light come from above
        Light * l = mSceneMgr->createLight(0);
        l->setType(LT_DIRECTIONAL);
        l->setDirection(Vector3::INV_Y);

		setupWater();
		setupProps();
		setupFish();
	}

	void setupWater()
	{
		// create our reflection & refraction render textures, and setup their render targets
		for(unsigned int i = 0; i < 2; i++)
		{
			Texture * tex = N_Only(TextureManager).createTexture(i == 0 ? "refraction" : "reflection",
				GroupDefault, TEX_TYPE_2D, 512, 512, 0, PF_R8G8B8, Texture::MM_FRAME_RGB);

			FrameObj * fo = tex->getBuffer()->getBuffer();
			fo->createViewport(mCamera)->setOverlayEnable(false);
			fo->add(this);

			if(i == 0) 
                mRefractionTarget = fo;
			else 
                mReflectionTarget = fo;
		}

		// create our water plane mesh
        mWaterPlane = Plane(Vector3::Y, 0);
        MeshManager::getSingleton().createPlane("water", GroupDefault,
            mWaterPlane, 700, 1300, 10, 10, true, 1, 3, 5, Vector3::Z);

		// create a water entity using our mesh, give it the shader material, and attach it to the origin
        mWater = mSceneMgr->createGeo("Water", "water");
        mWater->setMaterialName("Examples/FresnelReflectionRefraction");
        mSceneMgr->getOriginNode()->attachObject(mWater);
	}

    void windowUpdate()
    {
#if OGRE_PLATFORM != OGRE_PLATFORM_NACL
        mWindow->flush();
#endif
    }

	void setupProps()
	{
        GeometrySpace* ent;

		// setting up props might take a while, so create a progress bar for visual feedback
		ProgressBar* pb = mTrayMgr->createProgressBar(TL_CENTER, "FresnelBuildingBar", "Creating Props...", 280, 100);
		mTrayMgr->showBackdrop("SdkTrays/Shade");

		pb->setComment("Upper Bath");
		windowUpdate();
        ent = mSceneMgr->createGeo("UpperBath", "RomanBathUpper.mesh" );
		mSceneMgr->getOriginNode()->attachObject(ent);
        mSurfaceEnts.push_back(ent);
		pb->setProgress(0.4);

		pb->setComment("Columns");
		windowUpdate();
        ent = mSceneMgr->createGeo("Columns", "columns.mesh");
		mSceneMgr->getOriginNode()->attachObject(ent);
        mSurfaceEnts.push_back(ent);
		pb->setProgress(0.5);

		pb->setComment("Ogre Head");
		windowUpdate();
		ent = mSceneMgr->createGeo("Head", "ogrehead.mesh");
		ent->setMaterialName("RomanBath/OgreStone");
        mSurfaceEnts.push_back(ent);
		pb->setProgress(0.6);

		SpaceNode * headNode = mSceneMgr->getOriginNode()->create();
		headNode->setPos(Vector3f(-350, 55, 130));
		headNode->yaw(Angle(90));
        headNode->attachObject(ent);

		pb->setComment("Lower Bath");
		windowUpdate();
		ent = mSceneMgr->createGeo("LowerBath", "RomanBathLower.mesh");
        mSceneMgr->getOriginNode()->attachObject(ent);
        mSubmergedEnts.push_back(ent);
		pb->setProgress(1);
		windowUpdate();

		mTrayMgr->destroyWidget(pb);
		mTrayMgr->hideBackdrop();
	}

	void setupFish()
	{
		mFishNodes.resize(NUM_FISH);
		mFishAnimStates.resize(NUM_FISH);
		mFishSplines.resize(NUM_FISH);

		for (unsigned int i = 0; i < NUM_FISH; i++)
        {
			// create fish entity
            GeometrySpace* ent = mSceneMgr->createGeo("Fish" + StrConv::conv(i + 1), "fish.mesh");
            mSubmergedEnts.push_back(ent);

			// create an appropriately scaled node and attach the entity
			mFishNodes[i] = mSceneMgr->getOriginNode()->create();
			mFishNodes[i]->setScale(Vector3::UNIT * FISH_SCALE);
            mFishNodes[i]->attachObject(ent);

			// enable and save the swim animation state
            mFishAnimStates[i] = ent->getFusion("swim");
            mFishAnimStates[i]->setEnabled(true);

            // generate random waypoints for the fish to swim through
            for(unsigned int j = 0; j < NUM_FISH_WAYPOINTS; ++j)
            {
                Vector3 pos(Math::randomNegPos() * 270, -10, Math::randomNegPos() * 700);

                if(j > 0)  // make sure the waypoint isn't too far from the last, or our fish will be turbo-fish
                {
					const Vector3 & lastPos = mFishSplines[i].get(j - 1);
					Vector3 delta = pos - lastPos;
                    Vector3 norl;
                    delta.normalise(norl);
					if (delta.length() > 750) pos = lastPos + norl * 750;
                }

                mFishSplines[i].add(pos);
            }

			// close the spline and calculate all the tangents at once
            mFishSplines[i].add(mFishSplines[i].get(0));
            mFishSplines[i].rebuild();
        }

		mFishAnimTime = 0;
	}

	void cleanupContent()
	{
		mSurfaceEnts.clear();
		mSubmergedEnts.clear();
		mFishNodes.clear();
		mFishAnimStates.clear();
		mFishSplines.clear();

		MeshManager::getSingleton().remove("water");
		TextureManager::getSingleton().remove("refraction");
		TextureManager::getSingleton().remove("reflection");
	}

	const unsigned int NUM_FISH;
	const unsigned int NUM_FISH_WAYPOINTS;
	const unsigned int FISH_PATH_LENGTH;
	const NIIf FISH_SCALE;
	std::vector<GeometrySpace *> mSurfaceEnts;
	std::vector<GeometrySpace *> mSubmergedEnts;
	FrameObj * mRefractionTarget;
	FrameObj * mReflectionTarget;
	Plane mWaterPlane;
	GeometrySpace* mWater;
	std::vector<SpaceNode *> mFishNodes;
	std::vector<AnimationFusion *> mFishAnimStates;
	std::vector<LinearSpline> mFishSplines;
	NIIf mFishAnimTime;
    BoxSky * mBoxSky;
};

#endif
