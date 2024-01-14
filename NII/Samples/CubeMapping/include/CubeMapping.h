#ifndef __CubeMapping_H__
#define __CubeMapping_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_CubeMapping : public SdkSample, public FrameObjListener
{
public:
	Sample_CubeMapping()
	{
		mInfo["Title"] = "Cube Mapping";
		mInfo["Description"] = "Demonstrates the cube mapping feature where a wrap-around environment is reflected "
			"off of an object. Uses render-to-texture to create dynamic cubemaps.";
		mInfo["Thumbnail"] = "thumb_cubemap.png";
		mInfo["Category"] = "Unsorted";
        mDomeSky = 0;
	}

    ~Sample_CubeMapping()
    {
        if(mDomeSky)
        {
            mSceneMgr->removeSky(mDomeSky);
            N_delete mDomeSky;
            mDomeSky = 0;
        }
    }

	void testCapabilities(const RenderSysFeature* caps)
	{
        if (!caps->hasCapability(GF_Tex_Cube))
        {
			N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your graphics card does not support cube mapping, "
				"so you cannot run this sample. Sorry!", "CubeMappingSample::testCapabilities");
        }
	}

    bool onRenderCore(const RenderFrameArgs * evt)
    {
		mPivot->yaw(Radian(evt->mCurrent));      // spin the fishy around the cube mapped one
		mFishSwim->addTime(evt->mCurrent * 3);   // make the fishy swim
		return SdkSample::onRenderCore(evt);      // don't forget the parent updates!
    }

    void onPreFlush(const FrameObjArgs * evt)
    {
        mHead->setVisible(false);  // hide the head

		// point the camera in the right direction based on which face of the cubemap this is
		mCubeCamera->setOri(Quaternion::IDENTITY);
		if(evt->mFrame == mTargets[0])
            mCubeCamera->yaw(Angle(-90));
		else if(evt->mFrame == mTargets[1])
            mCubeCamera->yaw(Angle(90));
		else if(evt->mFrame == mTargets[2])
            mCubeCamera->pitch(Angle(90));
		else if(evt->mFrame == mTargets[3])
            mCubeCamera->pitch(Angle(-90));
		else if(evt->mFrame == mTargets[5])
            mCubeCamera->yaw(Angle(180));
    }

    void onEndFlush(const FrameObjArgs * evt)
    {
        mHead->setVisible(true);  // unhide the head
    }

protected:
	void setupContent()
	{
        mDomeSky = N_new DomeSky(mSceneMgr);
        mDomeSky->setMaterial("Examples/CloudySky");
        mDomeSky->setRenderGroup(RQG_Sky);
		mDomeSky->buildGeo();
        mSceneMgr->addSky(mDomeSky);
		// setup some basic lighting for our scene
        mSceneMgr->setAmbient(Colour(0.3, 0.3, 0.3));
        mSceneMgr->createLight(0)->setPos(20, 80, 50);

		createCubeMap();

		// create an ogre head, give it the dynamic cube map material, and place it at the origin
		mHead = mSceneMgr->createGeo("CubeMappedHead", "ogrehead.mesh");
		mHead->setMaterialName("Examples/DynamicCubeMap");
		mSceneMgr->getOriginNode()->attachObject(mHead);

		mPivot = mSceneMgr->getOriginNode()->create();  // create a pivot node

		GeometrySpace* fish = mSceneMgr->createGeo("Fish", "fish.mesh");
		mFishSwim = fish->getFusion("swim");
		mFishSwim->setEnabled(true);

		// create a child node at an offset and attach a regular ogre head and a nimbus to it
		SpaceNode* node = mPivot->create(Vector3(-60, 10, 0));
		node->setScale(Vector3f(7, 7, 7));
		node->yaw(Angle(90));
		node->attachObject(fish);

		// create a floor mesh resource
		MeshManager::getSingleton().createPlane("floor", ResourceSchemeManager::GroupDefault,
			Plane(Vector3::Y, -30), 1000, 1000, 10, 10, true, 1, 8, 8, Vector3::Z);

		// create a floor entity, give it a material, and place it at the origin
        GeometrySpace* floor = mSceneMgr->createGeo("Floor", "floor");
        floor->setMaterialName("Examples/BumpyMetal");
        mSceneMgr->getOriginNode()->attachObject(floor);

		// set our camera to orbit around the head and show cursor
		mCameraMan->setStyle(CS_ORBIT);
		mTrayMgr->showCursor();
	}

	void createCubeMap()
	{
		// create the camera used to render to our cubemap
		mCubeCamera = mSceneMgr->createCamera("CubeMapCamera");
		mCubeCamera->setFOV(Angle(90));
		mCubeCamera->setAspectRatio(1);
		mCubeCamera->setCustomYaw(false);
		mCubeCamera->setNearClipDistance(5);

		// create our dynamic cube map texture
		Texture * tex = N_Only(TextureManager).createTexture("dyncubemap", GroupDefault,
            TEX_TYPE_CUBE_MAP, 128, 128, 0, PF_R8G8B8, Texture::MM_FRAME_RGB);

		// assign our camera to all 6 render targets of the texture (1 for each direction)
		for(unsigned int i = 0; i < 6; i++)
		{
			mTargets[i] = tex->getCubeBuffer(i)->getBuffer();
			mTargets[i]->createViewport(mCubeCamera)->setOverlayEnable(false);
			mTargets[i]->add(this);
		}
	}

	void cleanupContent()
	{
        mSceneMgr->destroyCamera(mCubeCamera->getID());
		MeshManager::getSingleton().remove("floor");
		TextureManager::getSingleton().remove("dyncubemap");
	}

	GeometrySpace* mHead;
	Camera* mCubeCamera;
	FrameObj* mTargets[6];
	SpaceNode* mPivot;
	AnimationFusion* mFishSwim;
    DomeSky * mDomeSky;
};

#endif
