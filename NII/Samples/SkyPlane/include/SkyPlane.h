#ifndef __SkyPlane_H__
#define __SkyPlane_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_SkyPlane : public SdkSample
{
public:
	Sample_SkyPlane()
	{
		mInfo["Title"] = "Sky Plane";
		mInfo["Description"] = "Shows how to use skyplanes (fixed-distance planes used for backgrounds).";
		mInfo["Thumbnail"] = "thumb_skyplane.png";
		mInfo["Category"] = "Environment";
        mPlaneSky = 0;
	}
    ~Sample_SkyPlane()
    {
        if(mPlaneSky != 0)
        {
            mSceneMgr->removeSky(mPlaneSky);
            N_delete mPlaneSky;
            mPlaneSky = 0;
        }
    }
protected:
	void setupContent()
	{
		// setup some basic lighting for our scene
        mSceneMgr->setAmbient(Colour(0.3, 0.3, 0.3));
        mSceneMgr->createLight(0)->setPos(20, 80, 50);
        mPlaneSky = N_new PlaneSky(mSceneMgr);
        mPlaneSky->setMaterial("Examples/SpaceSkyPlane");
        mPlaneSky->setRenderGroup(RQG_Sky);
		// create a skyplane 5000 units away, facing down, 10000 square units large, with 3x texture tiling
        mPlaneSky->buildGeo(Plane(0, -1, 0, 5000), 10000, 3, 3);
        mSceneMgr->addSky(mPlaneSky);

        // and finally... omg it's a DRAGON!
        mSceneMgr->getOriginNode()->attachObject(mSceneMgr->createGeo("Dragon", "dragon.mesh"));

		// turn around and look at the DRAGON!
		mCamera->yaw(Angle(210));
		mCamera->pitch(Angle(-10));
	}
protected:
    PlaneSky * mPlaneSky;
};
#endif