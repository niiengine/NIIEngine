#ifndef __CelShading_H__
#define __CelShading_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_CelShading : public SdkSample
{
public:
	Sample_CelShading()
	{
		mInfo["Title"] = "Cel-shading";
		mInfo["Description"] = "A demo of cel-shaded graphics using vertex & fragment programs.";
		mInfo["Thumbnail"] = "thumb_cel.png";
		mInfo["Category"] = "Lighting";
	}

	StringList getRequiredPlugins()
	{
		StringList names;
        if(!GpuProgramManager::getSingleton().isSyntaxSupported("glsles"))
            names.push_back("Cg Program Manager");
		return names;
	}

	void testCapabilities(const RenderSysFeature * caps)
	{
		if(!caps->hasCapability(GF_Program_Vertex) || !caps->hasCapability(GF_Program_Fragment))
        {
			N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your graphics card does not support vertex and fragment"
				" programs, so you cannot run this sample. Sorry!");
        }
	}

	bool onRenderCore(const RenderFrameArgs * evt)
	{
		// make the light revolve around our model if and only if the check box is checked
		if (mMoveLight->isChecked()) mLightPivot->yaw(Angle(evt->mCurrent * 30));

		return SdkSample::onRenderCore(evt);  // don't forget the parent class updates!
	}
protected:

	void setupContent()
	{     
		mViewport->setBgColour(Colour::White);

		// set our camera to orbit around the origin and show cursor
		mCameraMan->setStyle(CS_ORBIT);
		mTrayMgr->showCursor();

		// create a basic point light with an offset
		Light* light = mSceneMgr->createLight(0);
		light->setPos(20, 40, 50);

		// attach the light to a pivot node
		mLightPivot = mSceneMgr->getOriginNode()->create();
		mLightPivot->attachObject(light);

		// create our model, give it the shader material, and place it at the origin
        GeometrySpace * ent = mSceneMgr->createGeo("Head", "ogrehead.mesh");
		ent->setMaterialName("Examples/CelShading");
		mSceneMgr->getOriginNode()->attachObject(ent);

        /* We set the same material for all parts of the head, but use custom shader parameters to set the
		colours for each part. See Examples-Advanced.material for how these are bound to GPU parameters. */

        SubEntity * sub;
        
        sub = ent->getCom(0);    // eyes
        sub->setCustonParam(SP_SHININESS, Vector4(35, 0, 0, 0));
        sub->setCustonParam(SP_DIFFUSE, Vector4(1, 0.3, 0.3, 1));
        sub->setCustonParam(SP_SPECULAR, Vector4(1, 0.6, 0.6, 1));

        sub = ent->getCom(1);    // skin
        sub->setCustonParam(SP_SHININESS, Vector4(10, 0, 0, 0));
        sub->setCustonParam(SP_DIFFUSE, Vector4(0, 0.5, 0, 1));
        sub->setCustonParam(SP_SPECULAR, Vector4(0.3, 0.5, 0.3, 1));

        sub = ent->getCom(2);    // earring
        sub->setCustonParam(SP_SHININESS, Vector4(25, 0, 0, 0));
        sub->setCustonParam(SP_DIFFUSE, Vector4(1, 1, 0, 1));
        sub->setCustonParam(SP_SPECULAR, Vector4(1, 1, 0.7, 1));

        sub = ent->getCom(3);    // teeth
        sub->setCustonParam(SP_SHININESS, Vector4(20, 0, 0, 0));
        sub->setCustonParam(SP_DIFFUSE, Vector4(1, 1, 0.7, 1));
        sub->setCustonParam(SP_SPECULAR, Vector4(1, 1, 1, 1));

		// create a check box to toggle light movement
		mMoveLight = mTrayMgr->createCheckBox(TL_TOPLEFT, "MoveLight", "Move Light");
		mMoveLight->setChecked(true);
	}

	// custom shader parameter bindings
	enum ShaderParam { SP_SHININESS = 1, SP_DIFFUSE, SP_SPECULAR };

	SpaceNode* mLightPivot;
	CheckBox* mMoveLight;
};

#endif
