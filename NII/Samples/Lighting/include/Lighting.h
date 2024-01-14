#ifndef __Lighting_H__
#define __Lighting_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

const uint8 cPriorityMain = 50;
const uint8 cPriorityQuery = 51;
const uint8 cPriorityLights = 55;

class _OgreSampleClassExport Sample_Lighting : public SdkSample, public GeometryObjListener
{
public:
	Sample_Lighting() :
	  	mGreenLightAnimState(NULL),
		mYellowLightAnimState(NULL),
		mLight1BBFlare(NULL),
		mLight1BBQueryArea(NULL),
		mLight1BBQueryVisible(NULL),
		mLight2BBFlare(NULL),
		mLight2BBQueryArea(NULL),
		mLight2BBQueryVisible(NULL),
		mLight1QueryArea(NULL),
		mLight1QueryVisible(NULL),
		mLight2QueryArea(NULL),
		mLight2QueryVisible(NULL),
        mActiveQuery(NULL),
		mUseOcclusionQuery(false),
		mDoOcclusionQuery(false)

	{
		mInfo["Title"] = "Lighting";
		mInfo["Description"] = "Shows OGRE's lighting support. Also demonstrates "
			"usage of occlusion queries and automatic time-relative behaviour "
			"using billboards and controllers.";
		mInfo["Thumbnail"] = "thumb_lighting.png";
		mInfo["Category"] = "Lighting";
	}

    bool onRenderCore(const RenderFrameArgs * evt)
    {
		// Move the lights along their paths
		mGreenLightAnimState->addTime(evt->mCurrent);
		mYellowLightAnimState->addTime(evt->mCurrent);

		// Modulate the light flare according to performed occlusion queries
		if (mUseOcclusionQuery)
		{
			// Stop occlusion queries until we get their information
			// (may not happen on the same frame they are requested in)
			mDoOcclusionQuery = false;

			// Check if all query information available
			if ((mLight1QueryArea->isRun() == false) &&
				(mLight1QueryVisible->isRun() == false) &&
				(mLight2QueryArea->isRun() == false) &&
				(mLight2QueryVisible->isRun() == false))
			{
				// Modulate the lights according to the query data
				unsigned int lightAreaCount;
				unsigned int lightVisibleCount;
				float ratio;

				mLight1QueryArea->query(lightAreaCount);
				mLight1QueryVisible->query(lightVisibleCount);
				ratio = float(lightVisibleCount) / float(lightAreaCount);
				mLight1BBFlare->setColour(mTrail->getColour(0) * ratio);

				mLight2QueryArea->query(lightAreaCount);
				mLight2QueryVisible->query(lightVisibleCount);
				ratio = float(lightVisibleCount) / float(lightAreaCount);
				mLight2BBFlare->setColour(mTrail->getColour(1) * ratio);

				// Request new query data
				mDoOcclusionQuery = true;
			}
		}
		return SdkSample::onRenderCore(evt);   // don't forget the parent class updates!
    }
protected:

    void testCapabilities(const RenderSysFeature * caps)
    {
        if (Ogre::Root::getSingletonPtr()->getRender()->getName().find("OpenGL ES 1") != String::npos)
        {
            N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "This sample uses 1D textures which are not supported in OpenGL ES 1.1, "
                "so you cannot run this sample. Sorry!");
        }
    }

	void setupContent()
	{
		// Set our camera to orbit around the origin at a suitable distance
		mCameraMan->setStyle(CS_ORBIT);
		mCameraMan->setYawPitchDist(Radian(0), Radian(0), 400);

		mTrayMgr->showCursor();

		// Create an ogre head and place it at the origin
		GeometrySpace * head = mSceneMgr->createGeo("Head", "ogrehead.mesh");
		head->setRenderGroup(cPriorityMain);
		mSceneMgr->getOriginNode()->attachObject(head);

		setupLights();
	}

	void setupLights()
	{

		mSceneMgr->setAmbient(Colour(0.1, 0.1, 0.1));  // Dim ambient lighting

		// Create a ribbon trail that our lights will leave behind
		PropertyData params;
		params["numberOfChains"] = "2";
		params["maxElements"] = "80";
		mTrail = (PixelOrbit*)mSceneMgr->createSpace(0, N_FACTORY_PIXELORBIT, &params);
		mSceneMgr->getOriginNode()->attachObject(mTrail);
		mTrail->setMaterialName("Examples/LightPixelOrbit");
		mTrail->setOrbitLength(400);
		mTrail->setRenderGroup(cPriorityLights);

		// Create the occlusion queries to be used in this sample
		try {
			RenderSystem* renderSystem = Ogre::Root::getSingleton().getRender();
			mLight1QueryArea = renderSystem->createHOQ();
			mLight1QueryVisible = renderSystem->createHOQ();
			mLight2QueryArea = renderSystem->createHOQ();
			mLight2QueryVisible = renderSystem->createHOQ();

			mUseOcclusionQuery = (mLight1QueryArea != NULL) &&
				(mLight1QueryVisible != NULL) &&
				(mLight2QueryArea != NULL) &&
				(mLight2QueryVisible != NULL);
		}
		catch (Ogre::Exception e)
		{
			mUseOcclusionQuery = false;
		}

		if (mUseOcclusionQuery == false)
		{
			LogManager::getSingleton().logMessage("Sample_Lighting - Error: failed to create hardware occlusion query");
		}

		// Create the materials to be used by the objects used fo the occlusion query
		Material * matBase = MaterialManager::WhiteColour;
		Material * matQueryArea = matBase->clone("QueryArea");
		matQueryArea->setColourWrite(ColourMark::CM_NONE);
        N_Only(Material).setDepthWrite(matQueryArea, false);
		N_Only(Material).setDepthCheck(matQueryArea, false); // Not occluded by objects

        Material * matQueryVisible = matBase->clone("QueryVisible");
		matQueryVisible->setColourWrite(ColourMark::CM_NONE);
        N_Only(Material).setDepthWrite(matQueryVisible, false);
		N_Only(Material).setDepthCheck(matQueryVisible, true); // Occluded by objects

		SpaceNode * node;
		Animation * anim;
		NodeKeyFrameTrack * track;
		Light * light;
		ParticleCtrl * bbs;

		// Create a light node
		node = mSceneMgr->getOriginNode()->create(Vector3(50, 30, 0));

		// Create a 14 second animation with spline interpolation
		anim = mSceneMgr->createAnimation("Path1", 14);
		anim->setInterpolationMode(NodeFusion::M_SPLINE);

        track = N_new NodeKeyFrameTrack(anim, 1);
        track->setAssociatedNode(node);
		// Enter keyframes for our track to define a path for the light to follow
		static_cast<NodeKeyFrame *>(track->create(0))->setTranslate(Vector3(50, 30, 0));
		static_cast<NodeKeyFrame *>(track->create(2))->setTranslate(Vector3(100, -30, 0));
		static_cast<NodeKeyFrame *>(track->create(4))->setTranslate(Vector3(120, -80, 150));
		static_cast<NodeKeyFrame *>(track->create(6))->setTranslate(Vector3(30, -80, 50));
		static_cast<NodeKeyFrame *>(track->create(8))->setTranslate(Vector3(-50, 30, -50));
		static_cast<NodeKeyFrame *>(track->create(10))->setTranslate(Vector3(-150, -20, -100));
		static_cast<NodeKeyFrame *>(track->create(12))->setTranslate(Vector3(-50, -30, 0));
		static_cast<NodeKeyFrame *>(track->create(14))->setTranslate(Vector3(50, 30, 0));
        track->buildSpline();
		// Create an animation state from the animation and enable it
		mYellowLightAnimState = mSceneMgr->createFusion("Path1");
		mYellowLightAnimState->setEnabled(true);

		// Set initial settings for the ribbon mTrail and add the light node
		mTrail->setColour(0, 1.0, 0.8, 0);
		mTrail->setColourFade(0, 0.5, 0.5, 0.5, 0.5);
		mTrail->setDim(0, 5);
		mTrail->attachNode(node);

		// Attach a light with the same colour to the light node
		light = mSceneMgr->createLight(0);
		light->setDiffuse(mTrail->getColour(0));
		node->attachObject(light);

		// Attach a flare with the same colour to the light node
		bbs = mSceneMgr->createParticleCtrl(0, 1);
		mLight1BBFlare = bbs->create(Vector3::ZERO, mTrail->getColour(0));
		bbs->setMaterialName("Examples/Flare");
		bbs->setRenderGroup(cPriorityLights);
		node->attachObject(bbs);

		if (mUseOcclusionQuery)
		{
			// Attach a billboard which will be used to get a relative area occupied by the light
			mLight1BBQueryArea = mSceneMgr->createParticleCtrl(0, 1);

            mLight1BBQueryArea->setParticleWidth(10);
            mLight1BBQueryArea->setParticleHeight(10);
			mLight1BBQueryArea->create(Vector3::ZERO);
			mLight1BBQueryArea->setMaterialName("QueryArea");
			mLight1BBQueryArea->setRenderGroup(cPriorityQuery);
			node->attachObject(mLight1BBQueryArea);

			// Attach a billboard which will be used to get the visible area occupied by the light
			mLight1BBQueryVisible = mSceneMgr->createParticleCtrl(0, 1);
            mLight1BBQueryVisible->setParticleWidth(10);
            mLight1BBQueryVisible->setParticleHeight(10);
			mLight1BBQueryVisible->create(Vector3::ZERO);
			mLight1BBQueryVisible->setMaterialName("QueryVisible");
			mLight1BBQueryVisible->setRenderGroup(cPriorityQuery);
			node->attachObject(mLight1BBQueryVisible);
		}

		// Create a second light node
		node = mSceneMgr->getOriginNode()->create(Vector3(-50, 100, 0));

		// Create a 10 second animation with spline interpolation
		anim = mSceneMgr->createAnimation("Path2", 10);
		anim->setInterpolationMode(NodeFusion::M_SPLINE);

        track = N_new NodeKeyFrameTrack(anim, 1);
        track->setAssociatedNode(node);
		// Enter keyframes for our track to define a path for the light to follow
		static_cast<NodeKeyFrame *>(track->create(0))->setTranslate(Vector3(-50, 100, 0));
		static_cast<NodeKeyFrame *>(track->create(2))->setTranslate(Vector3(-100, 150, -30));
		static_cast<NodeKeyFrame *>(track->create(4))->setTranslate(Vector3(-200, 0, 40));
		static_cast<NodeKeyFrame *>(track->create(6))->setTranslate(Vector3(0, -150, 70));
		static_cast<NodeKeyFrame *>(track->create(8))->setTranslate(Vector3(50, 0, 30));
		static_cast<NodeKeyFrame *>(track->create(10))->setTranslate(Vector3(-50, 100, 0));
        track->buildSpline();
		// Create an animation state from the animation and enable it
		mGreenLightAnimState = mSceneMgr->createFusion("Path2");
		mGreenLightAnimState->setEnabled(true);

		// Set initial settings for the ribbon mTrail and add the light node
		mTrail->setColour(1, 0.0, 1.0, 0.4);
		mTrail->setColourFade(1, 0.5, 0.5, 0.5, 0.5);
		mTrail->setDim(1, 5);
		mTrail->attachNode(node);

		// Attach a light with the same colour to the light node
		light = mSceneMgr->createLight(0);
		light->setDiffuse(mTrail->getColour(1));
		node->attachObject(light);

		// Attach a flare with the same colour to the light node
		bbs = mSceneMgr->createParticleCtrl(0, 1);
		mLight2BBFlare = bbs->create(Vector3::ZERO, mTrail->getColour(1));
		bbs->setMaterialName("Examples/Flare");
		bbs->setRenderGroup(cPriorityLights);
		node->attachObject(bbs);

		if(mUseOcclusionQuery)
		{
			// Attach a billboard which will be used to get a relative area occupied by the light
			mLight2BBQueryArea = mSceneMgr->createParticleCtrl(0, 1);
            mLight2BBQueryArea->setParticleWidth(10);
            mLight2BBQueryArea->setParticleHeight(10);
			mLight2BBQueryArea->create(Vector3::ZERO);
			mLight2BBQueryArea->setMaterialName("QueryArea");
			mLight2BBQueryArea->setRenderGroup(cPriorityQuery);
			node->attachObject(mLight2BBQueryArea);

			// Attach a billboard which will be used to get the visible area occupied by the light
			mLight2BBQueryVisible = mSceneMgr->createParticleCtrl(0, 1);
            mLight2BBQueryVisible->setParticleWidth(10);
            mLight2BBQueryVisible->setParticleHeight(10);
			mLight2BBQueryVisible->create(Vector3::ZERO);
			mLight2BBQueryVisible->setMaterialName("QueryVisible");
			mLight2BBQueryVisible->setRenderGroup(cPriorityQuery);
			node->attachObject(mLight2BBQueryVisible);
		}

		// Setup the listener for the occlusion query
		if(mUseOcclusionQuery)
		{
			mSceneMgr->add(this);
			mDoOcclusionQuery = true;
		}
	}

	// Event raised when render single object started.
	virtual void notifyRenderSingleObject(GeometryObj * rend, const Pass * pass,
        const SysSyncParam * source, const LightList * pLightList, bool lockch)
	{
		//
		// The following code activates and deactivates the occlusion queries
		// so that the queries only include the rendering of their intended targets
		//

		// Close the last occlusion query
		// Each occlusion query should only last a single rendering
		if (mActiveQuery != NULL)
		{
			mActiveQuery->endRender();
			mActiveQuery = NULL;
		}

		// Open a new occlusion query
		if (mDoOcclusionQuery == true)
		{
			// Check if a the object being rendered needs
			// to be occlusion queried, and by which query instance.
			if (rend == mLight1BBQueryArea)
				mActiveQuery = mLight1QueryArea;
			else if (rend == mLight1BBQueryVisible)
				mActiveQuery = mLight1QueryVisible;
			else if (rend == mLight2BBQueryArea)
				mActiveQuery = mLight2QueryArea;
			else if (rend == mLight2BBQueryVisible)
				mActiveQuery = mLight2QueryVisible;

			if (mActiveQuery != NULL)
			{
				mActiveQuery->beginRender();
			}
		}
	}

    void cleanupContent()
    {
        RenderSystem* renderSystem = Ogre::Root::getSingleton().getRender();
		if (mLight1QueryArea != NULL)
            renderSystem->destroyHOQ(mLight1QueryArea);
		if (mLight1QueryVisible != NULL)
            renderSystem->destroyHOQ(mLight1QueryVisible);
		if (mLight2QueryArea != NULL)
            renderSystem->destroyHOQ(mLight2QueryArea);
		if (mLight2QueryVisible != NULL)
            renderSystem->destroyHOQ(mLight2QueryVisible);
    }

	AnimationFusion* mGreenLightAnimState;
	AnimationFusion* mYellowLightAnimState;

	PixelOrbit* mTrail;

	SyncParticle* mLight1BBFlare;
	ParticleCtrl* mLight1BBQueryArea;
	ParticleCtrl* mLight1BBQueryVisible;
	SyncParticle* mLight2BBFlare;
	ParticleCtrl* mLight2BBQueryArea;
	ParticleCtrl* mLight2BBQueryVisible;

	PixelCountQuery* mLight1QueryArea;
	PixelCountQuery* mLight1QueryVisible;
	PixelCountQuery* mLight2QueryArea;
	PixelCountQuery* mLight2QueryVisible;
	PixelCountQuery* mActiveQuery;

	bool mUseOcclusionQuery;
	bool mDoOcclusionQuery;
};

#endif
