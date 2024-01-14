#ifndef __DynTex_H__
#define __DynTex_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_DynTex : public SdkSample
{
public:
	Sample_DynTex() : TEXTURE_SIZE(128), SQR_BRUSH_RADIUS(Math::Sqr(12))
	{
		mInfo["Title"] = "Dynamic Texturing";
		mInfo["Description"] = "Demonstrates how to create and use dynamically changing textures.";
		mInfo["Thumbnail"] = "thumb_dyntex.png";
		mInfo["Category"] = "Unsorted";
		mInfo["Help"] = "Use the left mouse button to wipe away the frost. "
			"It's cold though, so the frost will return after a while.";

        mBoxSky = 0;
	}
    ~Sample_DynTex()
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
		// shoot a ray from the cursor to the plane
		Ray ray = mTrayMgr->getCursorRay(mCamera);
		mCursorQuery->setRay(ray);
		RaySpaceQueryResult& result = mCursorQuery->execute();

		if (!result.empty())
		{
			// using the point of intersection, find the corresponding texel on our texture
			Vector3 pt = ray.getPoint(result.back().distance);
			mBrushPos = (Vector2(pt.x, -pt.y) / mPlaneSize + Vector2(0.5, 0.5)) * TEXTURE_SIZE;
		}

		uint8 freezeAmount = 0;
		mTimeSinceLastFreeze += evt->mCurrent;

		// find out how much to freeze the plane based on time passed
		while (mTimeSinceLastFreeze >= 0.1)
		{
			mTimeSinceLastFreeze -= 0.1;
			freezeAmount += 0x04;
		}

		updateTexture(freezeAmount);  // rebuild texture contents

		mPenguinAnimState->addTime(evt->mCurrent);  // increment penguin idle animation time
		mPenguinNode->yaw(Radian(evt->mCurrent));   // spin the penguin around

		return SdkSample::onRenderCore(evt);  // don't forget the parent class updates!
	}
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
	bool touchPressed(const OIS::MultiTouchEvent& evt)
	{
		if (mTrayMgr->injectMouseDown(evt)) return true;
		mWiping = true;  // wipe frost if user left clicks in the scene
		return true;
	}

	bool touchReleased(const OIS::MultiTouchEvent& evt)
	{
		if (mTrayMgr->injectMouseUp(evt)) return true;
		mWiping = false;  // stop wiping frost if user releases LMB
		return true;
	}
#else
	bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
	{
		if (mTrayMgr->injectMouseDown(evt, id)) return true;
		mWiping = true;  // wipe frost if user left clicks in the scene
		return true;
	}

	bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
	{
		if (mTrayMgr->injectMouseUp(evt, id)) return true;
		mWiping = false;  // stop wiping frost if user releases LMB
		return true;
	}
#endif
protected:

	void setupContent()
	{
        BoxSky * mBoxSky = N_new BoxSky(mSceneMgr);
        mBoxSky->setMaterial("Examples/StormySkyBox");
        mBoxSky->setRenderGroup(RQG_Sky);
		mBoxSky->buildGeo(5000);  // add a skybox
        mSceneMgr->addSky(mBoxSky);
		// setup some basic lighting for our scene
		mSceneMgr->setAmbient(Colour(0.5, 0.5, 0.5));
        mSceneMgr->createLight(0)->setPos(20, 80, 50);

		// set initial camera position
		mCameraMan->setStyle(CS_MANUAL);
		mCamera->setPos(NII::Vector3f(0, 0, 200));

		mTrayMgr->showCursor();

		// create our dynamic texture with 8-bit luminance texels
		Texture * tex = N_Only(TextureManager).createTexture("thaw", GroupDefault,
			TEX_TYPE_2D, TEXTURE_SIZE, TEXTURE_SIZE, 0, PF_L8ui, Buffer::M_WRITE);

		mTexBuf = tex->getBuffer();  // save off the texture buffer

		// init the texture to have full luminance
		mConstantTexBuf = new uint8[mTexBuf->getSize()];
		memset(mConstantTexBuf, 0xff, mTexBuf->getSize());
		memcpy(mTexBuf->lock(Buffer::MM_WHOLE), mConstantTexBuf, TEXTURE_SIZE * TEXTURE_SIZE);
		mTexBuf->unlock();

		// create a penguin and attach him to our penguin node
		GeometrySpace * penguin = mSceneMgr->createGeo("Penguin", "penguin.mesh");
		mPenguinNode = mSceneMgr->getOriginNode()->create();
		mPenguinNode->attachObject(penguin);

		// get and enable the penguin idle animation
		mPenguinAnimState = penguin->getFusion("amuse");
		mPenguinAnimState->setEnabled(true);

		// create a snowstorm over the scene, and fast forward it a little
        ParticleSpace* ps = mSceneMgr->createParticleSpace("Snow" "Examples/Snow");
        mSceneMgr->getOriginNode()->attachObject(ps);
        ps->_forward(30);

		// create a frosted screen in front of the camera, using our dynamic texture to "thaw" certain areas
		GeometrySpace* ent = mSceneMgr->createGeo("Plane", GSP_Plane);
		ent->setMaterialName("Examples/Frost");
		SpaceNode* node = mSceneMgr->getOriginNode()->create();
		node->setPos(Vector3f(0, 0, 50));
		node->attachObject(ent);

		mPlaneSize = ent->getAABB().getSize().x;   // remember the size of the plane

		mCursorQuery = mSceneMgr->createRayQuery(Ray());  // create a ray scene query for the cursor

		mTimeSinceLastFreeze = 0;
		mWiping = false;
	}

	void updateTexture(uint8 freezeAmount)
	{
		// get access to raw texel data
		uint8* data = mConstantTexBuf;

		uint8 temperature;
		NIIf sqrDistToBrush;

		// go through every texel...
		for (unsigned int y = 0; y < TEXTURE_SIZE; y++)
		{
			for (unsigned int x = 0; x < TEXTURE_SIZE; x++)
			{
				if (freezeAmount != 0)
				{
					// gradually refreeze anything that isn't completely frozen
					temperature = 0xff - *data;
					if (temperature > freezeAmount) *data += freezeAmount;
					else *data = 0xff;
				}

				if (mWiping)
				{
					// wipe frost from under the cursor
					sqrDistToBrush = Math::Sqr(x - mBrushPos.x) + Math::Sqr(y - mBrushPos.y);
					if (sqrDistToBrush <= SQR_BRUSH_RADIUS)
						*data = std::min<uint8>(sqrDistToBrush / SQR_BRUSH_RADIUS * 0xff, *data);
				}

				data++;
			}
		}

		memcpy(mTexBuf->lock(Buffer::MM_WHOLE), mConstantTexBuf, TEXTURE_SIZE * TEXTURE_SIZE);
		mTexBuf->unlock();
	}

	void cleanupContent()
	{
		delete [] mConstantTexBuf;
		TextureManager::getSingleton().remove("thaw");
        mSceneMgr->destroyQuery(mCursorQuery);
	}

	const unsigned int TEXTURE_SIZE;
	const unsigned int SQR_BRUSH_RADIUS;
	FrameBuffer * mTexBuf;
	uint8 * mConstantTexBuf;
	NIIf mPlaneSize;
	RaySpaceQuery * mCursorQuery;
	Vector2 mBrushPos;
	NIIf mTimeSinceLastFreeze;
	bool mWiping;
	SpaceNode * mPenguinNode;
	AnimationFusion * mPenguinAnimState;
};

#endif
