/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2012 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
same license as the rest of the engine.
-----------------------------------------------------------------------------
*/

#ifndef _Sample_Ocean_H_
#define _Sample_Ocean_H_

#include "SdkSample.h"
#include "SamplePlugin.h"
#include "MaterialControls.h"

using namespace Ogre;
using namespace OgreBites;

#define MINSPEED .150f
#define MOVESPEED 30
#define MAXSPEED 1.800f

#define CONTROLS_PER_PAGE 5

enum OceanMaterial {
	OCEAN1_CG,
	OCEAN1_NATIVE,
	OCEAN2_CG,
	OCEAN2_NATIVE
};

class _OgreSampleClassExport Sample_Ocean : public SdkSample
{
public:
	Sample_Ocean();
    ~Sample_Ocean();
protected:
	//Things from the frame listener
	Ogre::Vector3 mTranslateVector;
    int mSceneDetailIndex;
    float mUpdateFreq;
	bool mSpinLight;
    // just to stop toggles flipping too fast
    Ogre::TextureFilterMode mFiltering;
    int mAniso;

	Ogre::SpaceNode*	  mMainNode;
	Ogre::GeometrySpace*         mOceanSurfaceEnt;

	size_t				  mCurrentMaterial;
	size_t				  mCurrentPage;
	size_t				  mNumPages;
	Ogre::MaterialPtr	  mActiveMaterial;
	Ogre::Pass*			  mActivePass;
	Ogre::GpuProgram *	  mActiveFragmentProgram;
	Ogre::GpuProgram *	  mActiveVertexProgram;
	Ogre::GpuProgramParam * mActiveFragmentParameters;
	Ogre::GpuProgramParam * mActiveVertexParameters;
	NIIf				  mRotateSpeed;
	Slider* mShaderControls[CONTROLS_PER_PAGE];

	ShaderControlsContainer    mShaderControlContainer;
    MaterialControlsContainer mMaterialControlsContainer;
    BoxSky * mBoxSky;
	void setupGUI();
	void setupScene();
	virtual void setupContent();
	virtual void cleanupContent();

	void sliderMoved(Slider* slider);
	void buttonHit(OgreBites::Button* button);
	void checkBoxToggled(CheckBox* box);
	void selectOceanMaterial(OceanMaterial newMaterial);
	void itemSelected(SelectMenu* menu);
	void changePage(int nextPage = -1);
	virtual bool onRenderCore(const RenderFrameArgs * evt);
};

/**********************************************************************
  Static declarations
**********************************************************************/
// Lights
#define NUM_LIGHTS 1

// the light
Ogre::Light* mLights[NUM_LIGHTS];
// billboards for lights
Ogre::ParticleCtrl* mLightFlareSets[NUM_LIGHTS];
Ogre::SyncParticle* mLightFlares[NUM_LIGHTS];
// Positions for lights
Ogre::Vector3 mLightPositions[NUM_LIGHTS] =
{
	Ogre::Vector3(00, 400, 00)
};
// Base orientations of the lights
Ogre::NIIf mLightRotationAngles[NUM_LIGHTS] = { 35 };
Ogre::Vector3 mLightRotationAxes[NUM_LIGHTS] = {
    Ogre::Vector3::X
};
// Rotation speed for lights, degrees per second
Ogre::NIIf mLightSpeeds[NUM_LIGHTS] = { 30};

// Colours for the lights
Ogre::Colour mDiffuseLightColours[NUM_LIGHTS] =
{
	Ogre::Colour(0.6, 0.6, 0.6)
};

Ogre::Colour mSpecularLightColours[NUM_LIGHTS] =
{
	Ogre::Colour(0.5, 0.5, 0.5)
};

// Which lights are enabled
bool mLightState[NUM_LIGHTS] =
{
	true
};

// the light nodes
Ogre::SpaceNode* mLightNodes[NUM_LIGHTS];
// the light node pivots
Ogre::SpaceNode* mLightPivots[NUM_LIGHTS];


Sample_Ocean::Sample_Ocean()
{
	mInfo["Title"] = "Ocean";
	mInfo["Description"] = "An example demonstrating ocean rendering using shaders.";
	mInfo["Thumbnail"] = "thumb_ocean.png";
	mInfo["Category"] = "Environment";
    mBoxSky = 0;
}
Sample_Ocean::~Sample_Ocean()
{
    if(BoxSky)
    {
        mSceneMgr->removeSky(mBoxSky);
        N_delete mBoxSky;
        mBoxSky = 0;
    }
}

/*************************************************************************
	                    Sample_Ocean Methods
*************************************************************************/
void Sample_Ocean::cleanupContent()
{
    MeshManager::getSingleton().remove("OceanSurface");

	// get rid of the shared pointers before shutting down ogre or exceptions occur
    mActiveFragmentProgram.setNull();
    mActiveFragmentParameters.setNull();
    mActiveVertexProgram.setNull();
    mActiveVertexParameters.setNull();
    mActiveMaterial.setNull();
}

//--------------------------------------------------------------------------
void Sample_Ocean::setupGUI(void)
{
	SelectMenu* selectMenu = mTrayMgr->createLongSelectMenu(
		TL_TOPLEFT, "MaterialSelectMenu", "Material", 300, 200, 5);

	for (size_t i=0; i<mMaterialControlsContainer.size(); i++)
	{
		selectMenu->addItem(mMaterialControlsContainer[i].getDisplayName());
	}

	mTrayMgr->createCheckBox(TL_TOPLEFT, "SpinLightButton", "Spin Light", 175)->setChecked(true);

	mTrayMgr->createButton(TL_TOPRIGHT, "PageButtonControl", "Page", 175);

	for (size_t i=0; i<CONTROLS_PER_PAGE; i++)
	{
		mShaderControls[i] = mTrayMgr->createThickSlider(TL_TOPRIGHT,
			"ShaderControlSlider" + StrConv::conv(i), "Control", 256, 80, 0, 1, 100);
	}

	selectMenu->selectItem(0);
	mTrayMgr->showCursor();

}
//--------------------------------------------------------------------------
void Sample_Ocean::setupContent(void)
{
	loadAllMaterialControlFiles(mMaterialControlsContainer);
	setupScene();
	setupGUI();

	// Position it at 500 in Z direction
    mCamera->setPos(NII::Vector3f(0, 0, 0));
    // Look back along -Z
    mCamera->lookAt(NII::Vector3f(0, 0, -300));
    mCamera->setNearClipDistance(1);

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
    setDragLook(true);
#endif
}

void Sample_Ocean::setupScene()
{
	// Set ambient light
    mSceneMgr->setAmbient(Ogre::Colour(0.3, 0.3, 0.3));
    mBoxSky = N_new BoxSky(mSceneMgr);
    mBoxSky->setMaterial("SkyBox");
    mBoxSky->setRenderGroup(RQG_Sky);
	mBoxSky->buildGeo(1000);
    mSceneMgr->removeSky(mBoxSky);

    mMainNode = mSceneMgr->getOriginNode()->create();

    for(unsigned int i = 0; i < NUM_LIGHTS; ++i)
    {
        mLightPivots[i] = mSceneMgr->getOriginNode()->create();
        mLightPivots[i]->rotate(mLightRotationAxes[i], Ogre::Angle(mLightRotationAngles[i]).getRadian());
        // Create a light, use default parameters
        mLights[i] = mSceneMgr->createLight("Light" + Ogre::StrConv::conv(i));
		mLights[i]->setPos(mLightPositions[i]);
		mLights[i]->setDiffuse(mDiffuseLightColours[i]);
		mLights[i]->setSpecular(mSpecularLightColours[i]);
		mLights[i]->setVisible(mLightState[i]);
        //mLights[i]->setRange(400);
        //mLights[i]->setAttenuationConstant(0.1);
        //mLights[i]->setAttenuationLinear(1);
        //mLights[i]->setAttenuationQuadric(0);
        // Attach light
        mLightPivots[i]->attachObject(mLights[i]);
		// Create billboard for light
        mLightFlareSets[i] = mSceneMgr->createParticleCtrl("Flare" + Ogre::StrConv::conv(i), 20);
		mLightFlareSets[i]->setMaterialName("LightFlare");
		mLightPivots[i]->attachObject(mLightFlareSets[i]);
		mLightFlares[i] = mLightFlareSets[i]->create(mLightPositions[i]);
		mLightFlares[i]->setColour(mDiffuseLightColours[i]);
		mLightFlareSets[i]->setVisible(mLightState[i]);
    }

    // move the camera a bit right and make it look at the knot
	mCamera->moveRel(NII::Vector3f(50, 0, 100));
	mCamera->lookAt(NII::Vector3f(0, 0, 0));

    // Define a plane mesh that will be used for the ocean surface
    Ogre::Plane oceanSurface;
    oceanSurface.normal = Ogre::Vector3::Y;
    oceanSurface.d = 20;
    Ogre::MeshManager::getSingleton().createPlane("OceanSurface",
        Ogre::GroupDefault, oceanSurface,
        1000, 1000, 50, 50, true, 1, 1, 1, Ogre::Vector3::Z);

    mOceanSurfaceEnt = mSceneMgr->createGeo( "OceanSurface", "OceanSurface" );
    static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create())->attachObject(mOceanSurfaceEnt);
}
//--------------------------------------------------------------------------
void Sample_Ocean::sliderMoved(Slider * slider)
{
	using namespace Ogre;

	int sliderIndex = -1;
	for(int i = 0; i<CONTROLS_PER_PAGE; i++)
	{
		if(mShaderControls[i] == slider)
		{
			sliderIndex = i;
			break;
		}
	}
	assert(sliderIndex != -1);

	size_t index = mCurrentPage * CONTROLS_PER_PAGE + sliderIndex;
    const ShaderControl& ActiveShaderDef = mMaterialControlsContainer[mCurrentMaterial].getShaderControl(index);

	float val = slider->getValue();

	if(mActivePass)
	{
		switch(ActiveShaderDef.ValType)
		{
			case GPU_VERTEX:
			case GPU_FRAGMENT:
				{
					GpuProgramParam * activeParameters =
						(ActiveShaderDef.ValType == GPU_VERTEX) ?
							mActiveVertexParameters : mActiveFragmentParameters;

					if(!activeParameters.isNull())
					{
						activeParameters->_writeRawConstant(
							ActiveShaderDef.PhysicalIndex + ActiveShaderDef.ElementIndex, val);
					}
				}
				break;
			case MAT_SPECULAR:
				{
					// get the specular values from the material pass
					Colour OldSpec(mActivePass->getColour(ShaderCh::SOT_Get)->getSpecular());
					OldSpec[ActiveShaderDef.ElementIndex] = val;
					mActivePass->setColour(ShaderCh::SOT_Set)->setSpecular(OldSpec);
				}

				break;
			case MAT_DIFFUSE:
				{
					// get the specular values from the material pass
					Colour OldSpec(mActivePass->getColour(ShaderCh::SOT_Get)->getDiffuse());
					OldSpec[ActiveShaderDef.ElementIndex] = val;
					mActivePass->setColour(ShaderCh::SOT_Set)->setDiffuse(OldSpec);
				}
				break;
			case MAT_AMBIENT:
				{
					// get the specular values from the material pass
					Colour OldSpec(mActivePass->getColour(ShaderCh::SOT_Get)->getAmbient());
					OldSpec[ActiveShaderDef.ElementIndex] = val;
					mActivePass->setColour(ShaderCh::SOT_Set)->setAmbient(OldSpec);
				}
				break;
			case MAT_SHININESS:
				// get the specular values from the material pass
				mActivePass->setColour(ShaderCh::SOT_Set)->setShininess(val);
				break;

            case MAT_EMISSIVE:
                break;
		}
	}
}

//--------------------------------------------------------------------------
void Sample_Ocean::changePage(int pageNum /* = -1 : toggle */)
{
	if (mMaterialControlsContainer.empty()) return;
	mCurrentPage = (pageNum == -1) ? (mCurrentPage+1) % mNumPages : pageNum;

	static char pageText[64];
	sprintf(pageText, "Parameters %lu / %d", mCurrentPage+1, (int)mNumPages);
	static_cast<OgreBites::Button*>(mTrayMgr->getWidget("PageButtonControl"))->setCaption(pageText);

    if(!mActiveMaterial.isNull() && mActiveMaterial->getNumSupportedTechniques())
	{
        Ogre::Technique * currentTechnique = mActiveMaterial->getSupportedTechnique(0);
		if(currentTechnique)
		{
			mActivePass = currentTechnique->get(0);
			if(mActivePass)
			{
                if(mActivePass->getProgram().isExist(ST_FS))
                {
				    mActivePass->getProgram().get(0, ST_FS, mActiveFragmentProgram);
    				mActivePass->getProgram().get(0, ST_FS, NULL, mActiveFragmentParameters);
                }
                if(mActivePass->getProgram().isExist(ST_VS))
                {
				    mActivePass->getProgram().get(0, ST_VS, mActiveVertexProgram);
                    mActivePass->getProgram().get(0, ST_VS, NULL, mActiveVertexParameters);
                }

                size_t activeControlCount = mMaterialControlsContainer[mCurrentMaterial].getShaderControlCount();

				size_t startControlIndex = mCurrentPage * CONTROLS_PER_PAGE;
				int numControls = activeControlCount - startControlIndex;
				if (numControls <= 0)
				{
					mCurrentPage = 0;
					startControlIndex = 0;
					numControls = activeControlCount;
				}

				for (size_t i=0; i<CONTROLS_PER_PAGE; i++)
				{
					Slider* shaderControlSlider = mShaderControls[i];
					if (i < (size_t)numControls)
					{
						shaderControlSlider->show();
						size_t controlIndex = startControlIndex + i;
						const ShaderControl& ActiveShaderDef = mMaterialControlsContainer[mCurrentMaterial].getShaderControl(controlIndex);
						shaderControlSlider->setRange(ActiveShaderDef.MinVal, ActiveShaderDef.MaxVal, 50, false);
						shaderControlSlider->setCaption(ActiveShaderDef.Name);

						float uniformVal = 0.0;
						switch(ActiveShaderDef.ValType)
						{
							case GPU_VERTEX:
							case GPU_FRAGMENT:
								{
									Ogre::GpuProgramParam * activeParameters =
										(ActiveShaderDef.ValType == GPU_VERTEX) ?
											mActiveVertexParameters : mActiveFragmentParameters;
									if(activeParameters != 0)
									{
										// use param name to get index : use appropriate paramters ptr
										const NII::GpuParamUnit & def = activeParameters->getUnit(ActiveShaderDef.ParamName);
										ActiveShaderDef.PhysicalIndex = def.mMemIndex;
										// use index to get RealConstantEntry
										const float* pFloat = activeParameters->getFloatData(ActiveShaderDef.PhysicalIndex);
										// set position of ScrollWidget as param value
										uniformVal = pFloat[ActiveShaderDef.ElementIndex];
									}
								}
								break;

							case MAT_SPECULAR:
								{
									// get the specular values from the material pass

									Ogre::Colour OldSpec(mActivePass->getColour(ShaderCh::SOT_Get)->getSpecular());
									uniformVal = OldSpec[ActiveShaderDef.ElementIndex];
								}
								break;

							case MAT_DIFFUSE:
								{
									// get the diffuse values from the material pass

									Ogre::Colour OldSpec(mActivePass->getColour(ShaderCh::SOT_Get)->getDiffuse());
									uniformVal = OldSpec[ActiveShaderDef.ElementIndex];
								}
								break;

							case MAT_AMBIENT:
								{
									// get the ambient values from the material pass

									Ogre::Colour OldSpec(mActivePass->getColour(ShaderCh::SOT_Get)->getAmbient());
									uniformVal = OldSpec[ActiveShaderDef.ElementIndex];
								}
								break;

							case MAT_SHININESS:
								{
									// get the ambient values from the material pass
									uniformVal = mActivePass->getColour(ShaderCh::SOT_Get)->getShininess();
								}

								break;

							case MAT_EMISSIVE:
								{
									// get the ambient values from the material pass

									//Colour OldSpec(mActivePass->gete());
									//activeScrollWidget->setScrollPosition( OldSpec.val[ActiveShaderDef->ElementIndex] );
								}
								break;
						}
						shaderControlSlider->setValue(uniformVal);

					}
					else
					{
						shaderControlSlider->hide();
					}
				}
			}
		}
	}
}
void Sample_Ocean::itemSelected(SelectMenu *menu)
{
	//Only one selection menu - the material one
	mCurrentMaterial = menu->getSelectionIndex();
	mActiveMaterial = Ogre::MaterialManager::getSingleton().getByName( mMaterialControlsContainer[mCurrentMaterial].getMaterialName() );
	mActiveMaterial->load();
	size_t numShaders = mMaterialControlsContainer[mCurrentMaterial].getShaderControlCount();
	mNumPages = (numShaders / CONTROLS_PER_PAGE) + (numShaders % CONTROLS_PER_PAGE == 0 ? 0 : 1);
	changePage(0);

	if (mOceanSurfaceEnt)
	    mOceanSurfaceEnt->setMaterialName(mMaterialControlsContainer[mCurrentMaterial].getMaterialName());
}
bool Sample_Ocean::onRenderCore(const RenderFrameArgs * evt)
{
	mRotateSpeed = evt->mCurrent * 20;
	if(mSpinLight)
	{
		mLightPivots[0]->rotate(mLightRotationAxes[0], Ogre::Angle(mRotateSpeed * 2.0f).getRadian());
	}
	return SdkSample::onRenderCore(evt);
}
void Sample_Ocean::buttonHit(OgreBites::Button* button)
{
	//Only one button - change page
	changePage();
}

void Sample_Ocean::checkBoxToggled(CheckBox* cb)
{
	//Only one checkbox
	mSpinLight = cb->isChecked();
}

#endif	// end _Sample_Ocean_H_
