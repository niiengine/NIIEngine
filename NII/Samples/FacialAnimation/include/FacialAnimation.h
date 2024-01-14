#ifndef __FacialAnimation_H__
#define __FacialAnimation_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_FacialAnimation : public SdkSample
{
public:
	Sample_FacialAnimation():
		mSpeakAnimState(0), mManualAnimState(0), mManualKeyFrame(0), mPlayAnimation(false)
	{
		mInfo["Title"] = "Facial Animation";
		mInfo["Description"] = "A demonstration of the facial animation feature, using pose animation.";
		mInfo["Thumbnail"] = "thumb_facial.png";
		mInfo["Category"] = "Animation";
		mInfo["Help"] = "Use the checkbox to enable/disable manual animation. "
			"When manual animation is enabled, use the sliders to adjust each pose's weight.";
	}

	bool onRenderCore(const RenderFrameArgs * evt)
	{
		if(mPlayAnimation) mSpeakAnimState->addTime(evt->mCurrent);
		return SdkSample::onRenderCore(evt);  // don't forget the parent class updates!
	}

protected:
	void setupContent(void)
	{
		// setup some basic lighting for our scene
        mSceneMgr->setAmbient(Colour(0.5, 0.5, 0.5));
		mSceneMgr->createLight(0)->setPos(40, 60, 50);
		mSceneMgr->createLight(0)->setPos(-120, -80, -50);

		// pre-load the mesh so that we can tweak it with a manual animation
		mHeadMesh = N_Only(Mesh).load("facial.mesh", GroupDefault);

		// create a manual animation, create a pose track for it, and create a keyframe in that track
        VertexOffsetKeyFrameTrack * ret = N_new VertexOffsetKeyFrameTrack(mHeadMesh->createAnimation("Manual", 0), 4);
        mManualKeyFrame = ret->createVertexPoseKeyFrame(0);
		// create pose references for the first 15 poses
		for (unsigned int i = 0; i < 15; i++)
            mManualKeyFrame->add(i, 0);

		// create a head entity from the mesh and attach it to a node with a vertical offset to center it
		GeometrySpace* head = mSceneMgr->createGeo("Head", "facial.mesh");
		static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create(Vector3(0, -30, 0)))->attachObject(head);

		// get the animation states
		mSpeakAnimState = head->getFusion("Speak");
		mManualAnimState = head->getFusion("Manual");

		// make the camera orbit around the head, and show the cursor
		mCameraMan->setStyle(CS_ORBIT);
		mCameraMan->setYawPitchDist(Radian(0), Radian(0), 130);
		mTrayMgr->showCursor();

		mPlayAnimation = true;   // by default, the speaking animation is enabled

		setupControls();
	}

	void setupControls()
	{
		// make logo and frame stats a little more compact to make room for controls
		mTrayMgr->showLogo(TL_BOTTOMLEFT);
		mTrayMgr->toggleAdvancedFrameStats();

		// create group labels for the different sliders
		mExpressions.push_back(mTrayMgr->createLabel(TL_NONE, "ExpressionLabel", "Expressions"));
		mMouthShapes.push_back(mTrayMgr->createLabel(TL_NONE, "MouthShapeLabel", "Mouth Shapes"));

		// create sliders to adjust pose weight
		for (unsigned int i = 0; i < mManualKeyFrame->getValue().size(); i++)
		{
			String sliderName = "VertexOffset" + StrConv::conv(i);
			String poseName = mHeadMesh->getOffset(i)->getName();

			if (poseName.find("Expression") != std::string::npos)
				mExpressions.push_back(mTrayMgr->createLongSlider(TL_NONE, sliderName, poseName.substr(11), 200, 80, 44, 0, 1, 11));
			else mMouthShapes.push_back(mTrayMgr->createLongSlider(TL_NONE, sliderName, poseName.substr(0, 1), 160, 80, 44, 0, 1, 11));
		}

		// checkbox to switch between automatic animation and manual animation.
		mTrayMgr->createCheckBox(TL_TOP, "Manual", "Manual Animation")->setChecked(!mPlayAnimation);
	}

	void cleanupContent()
	{
		mExpressions.clear();
		mMouthShapes.clear();
		mSpeakAnimState = 0;
		mManualAnimState = 0;
		mManualKeyFrame = 0;
		mPlayAnimation = false;
		MeshManager::getSingleton().unload(mHeadMesh->getPrc());
	}

	void checkBoxToggled(OgreBites::CheckBox * box)
	{
		mPlayAnimation = !box->isChecked();

		// toggle animation states
		mSpeakAnimState->setEnabled(mPlayAnimation);
		mManualAnimState->setEnabled(!mPlayAnimation);

		// toggle expression controls
		for(NCount i = 0; i < mExpressions.size(); ++i)
		{
			mTrayMgr->moveWidgetToTray(mExpressions[i], mPlayAnimation ? TL_NONE : TL_TOPLEFT);
			if(mPlayAnimation) 
                mExpressions[i]->hide();
			else 
                mExpressions[i]->show();
		}

		// toggle mouth shape controls
		for(NCount i = 0; i < mMouthShapes.size(); ++i)
		{
			mTrayMgr->moveWidgetToTray(mMouthShapes[i], mPlayAnimation ? TL_NONE : TL_TOPRIGHT);
			if(mPlayAnimation) 
                mMouthShapes[i]->hide();
			else 
                mMouthShapes[i]->show();
		}
	}

	void sliderMoved(OgreBites::Slider * slider)
	{
		// update the pose reference controlled by this slider
		mManualKeyFrame->set(StringConverter::parseInt(slider->getName().substr(4)), slider->getValue());
		// dirty animation state since we're fudging this manually
		mManualAnimState->getParent()->_notifyFrame();
	}

   MeshPtr mHeadMesh;
   AnimationFusion * mSpeakAnimState;
   AnimationFusion * mManualAnimState;
   VertexPoseKeyFrame* mManualKeyFrame;
   bool mPlayAnimation;
   WidgetList mExpressions;
   WidgetList mMouthShapes;
};

#endif
