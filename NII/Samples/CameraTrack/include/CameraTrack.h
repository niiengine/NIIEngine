#ifndef __CameraTrack_H__
#define __CameraTrack_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_CameraTrack : public SdkSample
{
public:
	Sample_CameraTrack()
	{
		mInfo["Title"] = "Camera Tracking";
		mInfo["Description"] = "An example of using KeyFrameTracks to make a node smoothly follow "
			"a predefined path with spline interpolation. Also uses the auto-tracking feature of the camera.";
		mInfo["Thumbnail"] = "thumb_camtrack.png";
		mInfo["Category"] = "Unsorted";
        mBoxSky = 0;
	}
	~Sample_CameraTrack()
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
        mAnimState->addTime(evt->mCurrent);   // increment animation time
		return SdkSample::onRenderCore(evt);
    }
protected:
	void setupContent()
	{
		// setup some basic lighting for our scene
        mSceneMgr->setAmbient(Colour(0.3, 0.3, 0.3));
        mSceneMgr->createLight(0)->setPos(20, 80, 50);

        mBoxSky = N_new BoxSky(mSceneMgr);
        mBoxSky->setMaterial("Examples/MorningSkyBox");
        mBoxSky->setRenderGroup(RQG_Sky);
        mBoxSky->buildGeo(5000);
		mSceneMgr->addSky(mBoxSky);

		// create an ogre head entity and attach it to a node
		GeometrySpace * head = mSceneMgr->createGeo("Head", "ogrehead.mesh");
        SpaceNode * headNode = mSceneMgr->getOriginNode()->create();
		headNode->attachObject(head);

		mCameraMan->setStyle(CS_MANUAL);   // we will be controlling the camera ourselves, so disable the camera man
        mCamera->setAutoFocus(true);   // make the camera face the head
        mCamera->setAutoFocus(headNode);
        // create a camera node and attach camera to it
        SpaceNode * camNode = mSceneMgr->getOriginNode()->create();
        camNode->attachObject(mCamera);

		// set up a 10 second animation for our camera, using spline interpolation for nice curves
        Animation * anim = mSceneMgr->createAnimation("CameraTrack", 10);
        anim->setInterpolationMode(NodeFusion::M_SPLINE);

		// create a track to animate the camera's node
        NodeKeyFrameTrack * track = N_new NodeKeyFrameTrack(anim, 0);
        track->setAssociatedNode(camNode);

        // create keyframes for our track
        static_cast<NodeKeyFrame *>(track->create(0))->setTranslate(Vector3(200, 0, 0));
        static_cast<NodeKeyFrame *>(track->create(2.5))->setTranslate(Vector3(0, -50, 100));
        static_cast<NodeKeyFrame *>(track->create(5))->setTranslate(Vector3(-500, 100, 0));
        static_cast<NodeKeyFrame *>(track->create(7.5))->setTranslate(Vector3(0, 200, -300));
        static_cast<NodeKeyFrame *>(track->create(10))->setTranslate(Vector3(200, 0, 0));

        track->buildSpline();
        // create a new animation state to track this
        mAnimState = mSceneMgr->createFusion("CameraTrack");
        mAnimState->setEnabled(true);
	}

	AnimationFusion* mAnimState;
    BoxSky * mBoxSky;
};

#endif
