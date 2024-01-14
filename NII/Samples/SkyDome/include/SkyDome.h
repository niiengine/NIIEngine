#ifndef __SkyDome_H__
#define __SkyDome_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_SkyDome : public SdkSample
{
public:
	Sample_SkyDome()
	{
		mInfo["Title"] = "Sky Dome";
		mInfo["Description"] = "Shows how to use skydomes (fixed-distance domes used for backgrounds).";
		mInfo["Thumbnail"] = "thumb_skydome.png";
		mInfo["Category"] = "Environment";
        mDomeSky = 0;
	}
    ~Sample_SkyDome()
    {
        if(mDomeSky)
        {
            mSceneMgr->removeSky(mDomeSky);
            N_delete mDomeSky;
            mDomeSky = 0;
        }
    }
	void sliderMoved(Slider* slider)
	{
		// use the values from the sliders to update the skydome properties
        mDomeSky->setMaterial("Examples/CloudySky");
        mDomeSky->setRenderGroup(RQG_Sky);
		mDomeSky->buildGeo(mCurvatureSlider->getValue(), 100, mTilingSlider->getValue(), mTilingSlider->getValue());
        mSceneMgr->addSky(mDomeSky);
    }
protected:
	void setupContent()
	{
		// setup some basic lighting for our scene
        mSceneMgr->setAmbient(Colour(0.3, 0.3, 0.3));
        mSceneMgr->createLight(0)->setPos(20, 80, 50);

		// set our camera to orbit around the origin and show cursor
		mCameraMan->setStyle(CS_ORBIT);
		mCameraMan->setYawPitchDist(Angle(0), Angle(0), 250);
		mTrayMgr->showCursor();

		// create a floor mesh resource
		MeshManager::getSingleton().createPlane("floor", GroupDefault,
			Plane(Vector3::Y, -30), 1000, 1000, 10, 10, true, 1, 8, 8, Vector3::Z);

		// create a floor entity, give it a material, and place it at the origin
        GeometrySpace* floor = mSceneMgr->createGeo("Floor", "floor");
        floor->setMaterialName("Examples/BumpyMetal");
        mSceneMgr->getOriginNode()->attachObject(floor);

		// create an ogre head entity and place it at the origin
        mSceneMgr->getOriginNode()->attachObject(mSceneMgr->createGeo("Head", "ogrehead.mesh"));

		// create slider bars to control the dome curvature and texture tiling
		mCurvatureSlider = mTrayMgr->createThickSlider(TL_TOPLEFT, "Curvature", "Dome Curvature", 200, 60, 0, 50, 11);
		mTilingSlider = mTrayMgr->createThickSlider(TL_TOPLEFT, "Tiling", "Dome Tiling", 200, 60, 1, 20, 191);

		/* Here, we set default values for our sliders. We do not need to setup a skydome here, because when
		slider values change, the sliderMoved callback is invoked, and we setup the skydome with the appropriate
		values in there. See its definition above. */
		mCurvatureSlider->setValue(10);
		mTilingSlider->setValue(8);
	}

	void cleanupContent()
	{
		MeshManager::getSingleton().remove("floor");
	}

	Slider* mCurvatureSlider;
	Slider* mTilingSlider;
    DomeSky * mDomeSky;
};

#endif
