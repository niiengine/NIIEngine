#include "SamplePlugin.h"
#include "OgreShaderSubRenderState.h"
#include "ShaderSystem.h"
#include "ShaderExReflectionMap.h"
#include "OgreShaderExInstancedViewports.h"
#include "OgreShaderExTextureAtlasSampler.h"
using namespace Ogre;
using namespace OgreBites;

//-----------------------------------------------------------------------
const String DIRECTIONAL_LIGHT_NAME		= "DirectionalLight";
const String POINT_LIGHT_NAME			= "PointLight";
const String INSTANCED_VIEWPORTS_NAME	= "InstancedViewports";
const String ADD_LOTS_OF_MODELS_NAME	= "AddLotsOfModels";
const String SPOT_LIGHT_NAME			= "SpotLight";
const String PER_PIXEL_FOG_BOX			= "PerPixelFog";
const String ATLAS_AUTO_BORDER_MODE		= "AutoBorderAtlasing";
const String MAIN_ENTITY_MESH			= "ShaderSystem.mesh";
const String SPECULAR_BOX				= "SpecularBox";
const String REFLECTIONMAP_BOX			= "ReflectionMapBox";
const String REFLECTIONMAP_POWER_SLIDER	= "ReflectionPowerSlider";
const String MAIN_ENTITY_NAME			= "MainEntity";
const String EXPORT_BUTTON_NAME			= "ExportMaterial";
const String FLUSH_BUTTON_NAME			= "FlushShaderCache";
const String LAYERBLEND_BUTTON_NAME		= "ChangeLayerBlendType";
const String MODIFIER_VALUE_SLIDER  	= "ModifierValueSlider";
const String SAMPLE_MATERIAL_GROUP		= "RTShaderSystemMaterialsGroup";
const int MESH_ARRAY_SIZE = 2;
const String MESH_ARRAY[MESH_ARRAY_SIZE] =
{
	MAIN_ENTITY_MESH,
	"knot.mesh"
};

#ifndef OGRE_STATIC_LIB

SamplePlugin* sp;
Sample* s;

//-----------------------------------------------------------------------
extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new Sample_ShaderSystem;
	sp = N_new SamplePlugin(s->getInfo()["Title"] + " Sample");
	sp->addSample(s);
	Root::getSingleton().installPlugin(sp);
}
//-----------------------------------------------------------------------
extern "C" _OgreSampleExport void dllStopPlugin()
{
	Root::getSingleton().uninstallPlugin(sp);
	N_delete sp;
	delete s;
}
#endif


//-----------------------------------------------------------------------
Sample_ShaderSystem::Sample_ShaderSystem() :
	mLayeredBlendingEntity(NULL)
{
	mInfo["Title"] = "Shader System";
	mInfo["Description"] = "Demonstrate the capabilities of the RT Shader System component."
		"1. Fixed Function Pipeline emulation."
		"2. On the fly shader generation based on existing material."
		"3. On the fly shader synchronization with scene state (Lights, Fog)."
		"4. Built in lighting models: Per vertex, Per pixel, Normal map tangent and object space."
		"5. Pluggable custom shaders extensions."
		"6. Built in material script parsing that includes extended attributes."
		"7. Built in material script serialization."
		;
	mInfo["Thumbnail"] = "thumb_shadersystem.png";
	mInfo["Category"] = "Lighting";
	mInfo["Help"] = "F2 Toggle Shader System globally. "
				    "F3 Toggles Global Lighting Model. "
					"Modify target model attributes and scene settings and observe the generated shaders count. "
					"Press the export button in order to export current target model material. "
					"The model above the target will import this material next time the sample reloads. "
					"Right click on object to see the shaders it currently uses. "
					;
	mPointLightNode = NULL;
	mReflectionMapFactory = NULL;
	mInstancedViewportsEnable = false;
	mInstancedViewportsSubRenderState = NULL;
	mInstancedViewportsFactory = NULL;
	mBbsFlare = NULL;
    mAddedLotsOfModels = false;
    mNumberOfModelsAdded = 0;
    mBoxSky = 0;
}
//-----------------------------------------------------------------------
Sample_ShaderSystem::~Sample_ShaderSystem()
{
    if(mBoxSky)
    {
        mSceneMgr->removeSky(mBoxSky);
        N_delete mBoxSky;
        mBoxSky = 0;
    }
}
//-----------------------------------------------------------------------

void Sample_ShaderSystem::_shutdown()
{
	destroyInstancedViewports();
	SdkSample::_shutdown();
}

//-----------------------------------------------------------------------

void Sample_ShaderSystem::checkBoxToggled(CheckBox* box)
{
	const String& cbName = box->getName();

	if (cbName == SPECULAR_BOX)
	{
		setSpecularEnable(box->isChecked());
	}
	else if (cbName == REFLECTIONMAP_BOX)
	{
		setReflectionMapEnable(box->isChecked());
	}
	else if (cbName == DIRECTIONAL_LIGHT_NAME)
	{
		updateLightState(cbName, box->isChecked());
	}
	else if (cbName == POINT_LIGHT_NAME)
	{
		updateLightState(cbName, box->isChecked());
	}
	else if (cbName == INSTANCED_VIEWPORTS_NAME)
	{
		updateInstancedViewports(box->isChecked());
	}
	else if (cbName == ADD_LOTS_OF_MODELS_NAME)
	{
		updateAddLotsOfModels(box->isChecked());
	}
	else if (cbName == SPOT_LIGHT_NAME)
	{
		updateLightState(cbName, box->isChecked());
	}
	else if (cbName == PER_PIXEL_FOG_BOX)
	{
		setPerPixelFogEnable(box->isChecked());
	}
	else if (cbName == ATLAS_AUTO_BORDER_MODE)
	{
		setAtlasBorderMode(box->isChecked());
	}
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::itemSelected(SelectMenu* menu)
{
	if (menu == mLightingModelMenu)
	{
		int curModelIndex = menu->getSelectionIndex();

		if (curModelIndex >= SSLM_PerVertexLighting && curModelIndex <= SSLM_NormalMapLightingObjectSpace)
		{
			setCurrentLightingModel((ShaderSystemLightingModel)curModelIndex);
		}
	}
	else if (menu == mFogModeMenu)
	{
		int curModeIndex = menu->getSelectionIndex();

		if (curModeIndex >= FM_NONE && curModeIndex <= FM_LINEAR)
		{
			mSceneMgr->setFog(ShaderChFog((FogMode)curModeIndex, Colour(1.0, 1.0, 1.0, 0.0), 0.0015, 350.0, 1500.0));
		}
	}
	else if (menu == mShadowMenu)
	{
		int curShadowTypeIndex = menu->getSelectionIndex();

		applyShadowType(curShadowTypeIndex);
	}
	else if(menu == mLanguageMenu)
	{
		ShaderGenerator::getSingletonPtr()->setTargetLanguage(menu->getSelectItem());
	}
}
//-----------------------------------------------------------------------
void Sample_ShaderSystem::buttonHit( OgreBites::Button* b )
{
	// Case the current material of the main entity should be exported.
	if (b->getName() == EXPORT_BUTTON_NAME)
	{
		const String& materialName = mSceneMgr->getGeo(MAIN_ENTITY_NAME)->getCom(0)->getMaterialID();

		exportRTShaderSystemMaterial(mExportMaterialPath + "ShaderSystemExport.material", materialName);
	}
	// Case the shader cache should be flushed.
	else if (b->getName() == FLUSH_BUTTON_NAME)
	{
		mShaderGenerator->flushShaderCache();
	}

	// Case the blend layer type modified.
	else if (b->getName() == LAYERBLEND_BUTTON_NAME && mLayerBlendSubRS != NULL)
	{
		changeTextureLayerBlendMode();

	}
}
//--------------------------------------------------------------------------
void Sample_ShaderSystem::sliderMoved(Slider* slider)
{
	if (slider->getName() == REFLECTIONMAP_POWER_SLIDER)
	{
		NIIf reflectionPower = slider->getValue();

		if (mReflectionMapSubRS != NULL)
		{
			ShaderExReflectionMap* reflectionMapSubRS = static_cast<ShaderExReflectionMap*>(mReflectionMapSubRS);

			// Since RTSS export caps based on the template sub render states we have to update the template reflection sub render state.
			reflectionMapSubRS->setReflectionPower(reflectionPower);

			// Grab the instances set and update them with the new reflection power value.
			// The instances are the actual sub render states that have been assembled to create the final shaders.
			// Every time that the shaders have to be re-generated (light changes, fog changes etc..) a new set of sub render states
			// based on the template sub render states assembled for each pass.
			// From that set of instances a CPU program is generated and afterward a GPU program finally generated.
			RTShader::SubRenderStateSet instanceSet = mReflectionMapSubRS->getAccessor()->getSubRenderStateInstanceSet();
			RTShader::SubRenderStateSetIterator it = instanceSet.begin();
			RTShader::SubRenderStateSetIterator itEnd = instanceSet.end();

			for(; it != itEnd; ++it)
			{
				ShaderExReflectionMap* reflectionMapSubRSInstance = static_cast<ShaderExReflectionMap*>(*it);

				reflectionMapSubRSInstance->setReflectionPower(reflectionPower);
			}
		}
	}

	if(slider->getName() == MODIFIER_VALUE_SLIDER)
	{
		if(mLayeredBlendingEntity != NULL)
		{
			Ogre::NIIf val = mModifierValueSlider->getValue();
			mLayeredBlendingEntity->getCom(0)->setCustonParam(2, Vector4(val,val,val,0));
		}
	}
}
//-----------------------------------------------------------------------
bool Sample_ShaderSystem::onRenderCore(const RenderFrameArgs * evt)
{
	if(mSceneMgr->isLightExist(SPOT_LIGHT_NAME))
	{
		Light * light = mSceneMgr->getLight(SPOT_LIGHT_NAME);

		light->setPos(mCamera->getSpacePos() + mCamera->getSpaceUp() * 20.0);
		light->setDirection(mCamera->getSpaceDirection());
	}

	if(mPointLightNode != NULL)
	{
		static NIIf sToatalTime = 0.0;

		sToatalTime += evt->mCurrent;
		mPointLightNode->yaw(Angle(evt->mCurrent * 15));
		mPointLightNode->setPos(Vector3f(0.0, Math::Sin(sToatalTime) * 30.0, 0.0));
	}

	updateTargetObjInfo();

	return SdkSample::onRenderCore(evt);
}
//-----------------------------------------------------------------------
void Sample_ShaderSystem::setupView()
{
	// setup default viewport layout and camera
	mCamera = mSceneMgr->createCamera("MainCamera");
	mViewport = mWindow->createViewport(mCamera);
	mCamera->setAspectRatio((Ogre::NIIf)mViewport->getPixelWidth() / (Ogre::NIIf)mViewport->getPixelHeight());
	mCamera->setNearClipDistance(5);

	mCameraMan = new SdkCameraMan(mCamera);   // create a default camera controller
}
//-----------------------------------------------------------------------
void Sample_ShaderSystem::setupContent()
{
	// Setup default effects values.
	mCurLightingModel 		= SSLM_PerVertexLighting;
	mPerPixelFogEnable		= false;
	mSpecularEnable   		= false;
	mReflectionMapEnable	= false;
	mReflectionMapSubRS		= NULL;
	mLayerBlendSubRS		= NULL;

	mRayQuery = mSceneMgr->createRayQuery(Ray());
	mTargetObj = NULL;

	// Set ambient lighting.
	mSceneMgr->setAmbient(Colour(0.2, 0.2, 0.2));
    mBoxSky = N_new BoxSky(mSceneMgr);
    mBoxSky->setMaterial("Examples/SceneCubeMap2");
    mBoxSky->setRenderGroup(RQG_Sky);
	// Setup the sky box,
	mBoxSky->setSkyBox(500);
    mSceneMgr->removeSky(mBoxSky);
	MeshManager::getSingleton().createPlane("Myplane",
		ResourceSchemeManager::GroupDefault, Plane(Vector3::Y, 0),
            1500,1500,25,25,true,1,60,60,Vector3::Z);

	GeometrySpace* pPlaneEnt = mSceneMgr->createGeo( "plane", "Myplane" );
	pPlaneEnt->setMaterialName("Examples/Rockwall");
	pPlaneEnt->setCastShadow(false);
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create(Vector3(0,0,0)))->attachObject(pPlaneEnt);

	// Load sample meshes and generate tangent vectors.
	for(int i = 0; i < MESH_ARRAY_SIZE; ++i)
	{
		const String & curMeshName = MESH_ARRAY[i];

		MeshPtr pMesh = N_Only(Mesh).load(curMeshName, GroupDefault,
			Buffer::M_WRITE, Buffer::Buffer::M_DEV | Buffer::M_WRITE, true, true); //so we can still read it

		// Build tangent vectors, all our meshes use only 1 texture coordset
		// Note we can build into VT_Tangent now (SM2+)
		unsigned short src, dest;
		if(!pMesh->checkTangentBuffer(false))
		{
			pMesh->buildTangentBuffer(VT_Tangent);
		}
	}

	GeometrySpace* entity;
	SpaceNode* childNode;

	// Create the main entity and mark it as the current target object.
	entity = mSceneMgr->createGeo(MAIN_ENTITY_NAME, MAIN_ENTITY_MESH);
	mTargetEntities.push_back(entity);
	childNode = mSceneMgr->getOriginNode()->create();
	childNode->attachObject(entity);
	mTargetObj = entity;
	childNode->setAABBView(true);

	// Create reflection entity that will show the exported material.
	const String & mainExportedMaterial = mSceneMgr->getGeo(MAIN_ENTITY_NAME)->getCom(0)->getMaterialID() + "_RTSS_Export";
	MaterialPtr matMainEnt = MaterialManager::getSingleton().getByName(mainExportedMaterial, SAMPLE_MATERIAL_GROUP);

	entity = mSceneMgr->createGeo("ExportedMaterialEntity", MAIN_ENTITY_MESH);
	entity->setMaterial(matMainEnt);
	childNode = mSceneMgr->getOriginNode()->create();
	childNode->setPos(Vector3f(0.0, 200.0, -200.0));
	childNode->attachObject(entity);

	// Create texture layer blending demonstration entity.
	mLayeredBlendingEntity = mSceneMgr->createGeo("LayeredBlendingMaterialEntity", MAIN_ENTITY_MESH);
	mLayeredBlendingEntity->setMaterialName("RTSS/LayeredBlending");
	mLayeredBlendingEntity->getCom(0)->setCustonParam(2, Vector4::ZERO);
	childNode = mSceneMgr->getOriginNode()->create();
	childNode->setPos(Vector3f(300.0, 200.0, -200.0));
	childNode->attachObject(mLayeredBlendingEntity);

	// Grab the render state of the material.
	RTShader::RenderState* renderState = mShaderGenerator->getRenderState(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, "RTSS/LayeredBlending", 0);

	if(renderState != NULL)
	{
		const SubRenderStateList& subRenderStateList = renderState->getTemplateSubRenderStateList();
		SubRenderStateListConstIterator it = subRenderStateList.begin();
		SubRenderStateListConstIterator itEnd = subRenderStateList.end();

		// Search for the texture layer blend sub state.
		for(; it != itEnd; ++it)
		{
			SubRenderState * curSubRenderState = *it;

			if(curSubRenderState->getType() == LayeredBlending::Type)
			{
				mLayerBlendSubRS = static_cast<LayeredBlending*>(curSubRenderState);
				break;
			}
		}
	}

	// Create per pixel lighting demonstration entity.
	entity = mSceneMgr->createGeo("PerPixelEntity", "knot.mesh");
	entity->setMaterialName("RTSS/PerPixel_SinglePass");
	childNode = mSceneMgr->getOriginNode()->create();
	childNode->setPos(Vector3f(300.0, 100.0, -100.0));
	childNode->attachObject(entity);

	// Create normal map lighting demonstration entity.
	entity = mSceneMgr->createGeo("NormalMapEntity", "knot.mesh");
	entity->setMaterialName("RTSS/NormalMapping_SinglePass");
	childNode = mSceneMgr->getOriginNode()->create();
	childNode->setPos(Vector3f(-300.0, 100.0, -100.0));
	childNode->attachObject(entity);

    // OpenGL ES 2.0 does not support texture atlases
	if(Ogre::Root::getSingletonPtr()->getRender()->getName().find("OpenGL ES 2") == String::npos)
    {
        RTShader::RenderState * pMainRenderState =
            RTShader::ShaderGenerator::getSingleton().createOrRetrieveRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME).first;
        pMainRenderState->addTemplateSubRenderState(
            Ogre::RTShader::ShaderGenerator::getSingleton().createSubRenderState(
            Ogre::RTShader::TextureAtlasSampler::Type));

        // Create texture atlas object and node
        CustomSpaceObj * atlasObject = createTextureAtlasObject();
        childNode = mSceneMgr->getOriginNode()->create();
        childNode->setPos(Vector3f(-600.0, 0, -850.0));
        childNode->attachObject(atlasObject);
    }

	createDirectionalLight();
	createPointLight();
	createSpotLight();

	RenderState * schemRenderState = mShaderGenerator->getRenderState(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

	// Take responsibility for updating the light count manually.
	schemRenderState->setLightCountAutoUpdate(false);

	setupUI();

	mCamera->setPos(NII::Vector3(0.0, 300.0, 450.0));
	mCamera->lookAt(NII::Vector3(0.0, 150.0, 0.0));

	// Make this viewport work with shader generator scheme.
	mViewport->setMaterial(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

	// Mark system as on.
	mDetailsPanel->setParamValue(11, "On");

	// a friendly reminder
	StringList names;
	names.push_back("Help");
	mTrayMgr->createParamsPanel(TL_TOPLEFT, "Help", 100, names)->setParamValue(0, "H/F1");

	updateSystemShaders();
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::setupUI()
{
	// Create language selection
	mLanguageMenu = mTrayMgr->createLongSelectMenu(TL_TOPLEFT, "LangMode", "Language", 220, 120, 10);

    // Use GLSL ES in case of OpenGL ES 2 render system.
	if (Ogre::Root::getSingletonPtr()->getRender()->getName().find("OpenGL ES 2") != String::npos)
	{
		mLanguageMenu->addItem("glsles");
		mShaderGenerator->setTargetLanguage("glsles");
	}

    // Use GLSL in case of OpenGL render system.
    else if (Ogre::Root::getSingletonPtr()->getRender()->getName().find("OpenGL") != String::npos)
	{
		mLanguageMenu->addItem("glsl");
		mShaderGenerator->setTargetLanguage("glsl");
	}

	// Use HLSL in case of D3D9 render system.
	else if (Ogre::Root::getSingletonPtr()->getRender()->getName().find("Direct3D9") != String::npos)
	{
		mLanguageMenu->addItem("hlsl");
		mShaderGenerator->setTargetLanguage("hlsl");
	}
	mLanguageMenu->addItem("cg");

	// create check boxes to toggle lights.
	mDirLightCheckBox = mTrayMgr->createCheckBox(TL_TOPLEFT, DIRECTIONAL_LIGHT_NAME, "Directional Light", 220);
	mPointLightCheckBox = mTrayMgr->createCheckBox(TL_TOPLEFT, POINT_LIGHT_NAME, "Point Light", 220);
	mSpotLightCheckBox = mTrayMgr->createCheckBox(TL_TOPLEFT, SPOT_LIGHT_NAME, "Spot Light", 220);

	mInstancedViewportsCheckBox = mTrayMgr->createCheckBox(TL_TOPLEFT, INSTANCED_VIEWPORTS_NAME, "Instanced Viewports", 220);
	mAddLotsOfModels = mTrayMgr->createCheckBox(TL_TOPLEFT, ADD_LOTS_OF_MODELS_NAME, "Add lots of models", 220);

	mDirLightCheckBox->setChecked(true);
	mPointLightCheckBox->setChecked(true);
	mSpotLightCheckBox->setChecked(false);
	mInstancedViewportsCheckBox->setChecked(false);
	mAddLotsOfModels->setChecked(false);


#ifdef RTSHADER_SYSTEM_BUILD_CORE_SHADERS
	mTrayMgr->createCheckBox(TL_TOPLEFT, PER_PIXEL_FOG_BOX, "Per Pixel Fog", 220)->setChecked(mPerPixelFogEnable);
#endif

#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
	mTrayMgr->createCheckBox(TL_TOPLEFT, ATLAS_AUTO_BORDER_MODE, "Atlas auto border", 220)->setChecked(true);
	setAtlasBorderMode(true);
#endif

	// Create fog widgets.
	mFogModeMenu = mTrayMgr->createLongSelectMenu(TL_TOPLEFT, "FogMode", "Fog Mode", 220, 120, 10);
	mFogModeMenu->addItem("None");
	mFogModeMenu->addItem("Exp");
	mFogModeMenu->addItem("Exp2");
	mFogModeMenu->addItem("Linear");

	// Create shadow menu.
	mShadowMenu = mTrayMgr->createLongSelectMenu(TL_TOPLEFT, "ShadowType", "Shadow", 220, 120, 10);
	mShadowMenu->addItem("None");

#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
	mShadowMenu->addItem("PSSM 3");
#endif


	// Flush shader cache button.
	mTrayMgr->createButton(TL_TOPLEFT, FLUSH_BUTTON_NAME, "Flush Shader Cache", 220);

	// create target model widgets.
	mTargetObjMatName = mTrayMgr->createLabel(TL_TOPLEFT, "TargetObjMatName", "", 220);
	mTargetObjVS = mTrayMgr->createLabel(TL_TOPLEFT, "TargetObjVS", "", 220);
	mTargetObjFS = mTrayMgr->createLabel(TL_TOPLEFT, "TargetObjFS", "", 220);


	// Create main entity widgets.
	mTrayMgr->createLabel(TL_BOTTOM, "MainEntityLabel", "Main GeometrySpace Settings", 240);
	mTrayMgr->createCheckBox(TL_BOTTOM, SPECULAR_BOX, "Specular", 240)->setChecked(mSpecularEnable);

	// Allow reflection map only on PS3 and above since with all lights on + specular + bump we
	// exceed the instruction count limits of PS2.
	if (GpuProgramManager::getSingleton().isSyntaxSupported("ps_3_0") ||
        GpuProgramManager::getSingleton().isSyntaxSupported("glsles") ||
		GpuProgramManager::getSingleton().isSyntaxSupported("fp30"))
	{
		mTrayMgr->createCheckBox(TL_BOTTOM, REFLECTIONMAP_BOX, "Reflection Map", 240)->setChecked(mReflectionMapEnable);
		mReflectionPowerSlider = mTrayMgr->createThickSlider(TL_BOTTOM, REFLECTIONMAP_POWER_SLIDER, "Reflection Power", 240, 80, 0, 1, 100);
		mReflectionPowerSlider->setValue(0.5, false);
	}

	mLightingModelMenu = mTrayMgr->createLongSelectMenu(TL_BOTTOM, "TargetModelLighting", "", 240, 230, 10);
	mLightingModelMenu ->addItem("Per Vertex");

#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
	mLightingModelMenu ->addItem("Per Pixel");
	mLightingModelMenu ->addItem("Normal Map - Tangent Space");
	mLightingModelMenu ->addItem("Normal Map - Object Space");
#endif

	mTrayMgr->createButton(TL_BOTTOM, EXPORT_BUTTON_NAME, "Export Material", 240);

#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
	mLayerBlendLabel = mTrayMgr->createLabel(TL_RIGHT, "Blend Type", "Blend Type", 240);
	mTrayMgr->createButton(TL_RIGHT, LAYERBLEND_BUTTON_NAME, "Change Blend Type", 220);
	mModifierValueSlider = mTrayMgr->createThickSlider(TL_RIGHT, MODIFIER_VALUE_SLIDER, "Modifier", 240, 80, 0, 1, 100);
	mModifierValueSlider->setValue(0.0,false);
	// Update the caption.
	updateLayerBlendingCaption(mLayerBlendSubRS->getBlendMode(1));

#endif

	mTrayMgr->showCursor();
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::cleanupContent()
{
	// UnLoad sample meshes and generate tangent vectors.
	for (int i=0; i < MESH_ARRAY_SIZE; ++i)
	{
		const String& curMeshName = MESH_ARRAY[i];
		MeshManager::getSingleton().unload(curMeshName);
	}

	MeshManager::getSingleton().remove(MAIN_ENTITY_MESH);
	mTargetEntities.clear();

    MeshManager::getSingleton().remove("Myplane");

	mSceneMgr->destroyQuery(mRayQuery);
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::setCurrentLightingModel(ShaderSystemLightingModel lightingModel)
{
	if (mCurLightingModel != lightingModel)
	{
		mCurLightingModel  = lightingModel;

		EntityListIterator it = mTargetEntities.begin();
		EntityListIterator itEnd = mTargetEntities.end();

		for (; it != itEnd; ++it)
		{
			generateShaders(*it);
		}
	}
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::setSpecularEnable(bool enable)
{
	if (mSpecularEnable != enable)
	{
		mSpecularEnable = enable;
		updateSystemShaders();
	}
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::setReflectionMapEnable(bool enable)
{
	if (mReflectionMapEnable != enable)
	{
		mReflectionMapEnable = enable;
		updateSystemShaders();
	}
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::setPerPixelFogEnable( bool enable )
{
#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
	if (mPerPixelFogEnable != enable)
	{
		mPerPixelFogEnable = enable;

		// Grab the scheme render state.
		RenderState* schemRenderState = mShaderGenerator->getRenderState(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
		const SubRenderStateList& subRenderStateList = schemRenderState->getTemplateSubRenderStateList();
		SubRenderStateListConstIterator it = subRenderStateList.begin();
		SubRenderStateListConstIterator itEnd = subRenderStateList.end();
		FFPFog* fogSubRenderState = NULL;

		// Search for the fog sub state.
		for (; it != itEnd; ++it)
		{
			SubRenderState* curSubRenderState = *it;

			if (curSubRenderState->getType() == FFPFog::Type)
			{
				fogSubRenderState = static_cast<FFPFog*>(curSubRenderState);
				break;
			}
		}

		// Create the fog sub render state if need to.
		if (fogSubRenderState == NULL)
		{
			SubRenderState* subRenderState = mShaderGenerator->createSubRenderState(FFPFog::Type);

			fogSubRenderState = static_cast<FFPFog*>(subRenderState);
			schemRenderState->addTemplateSubRenderState(fogSubRenderState);
		}


		// Select the desired fog calculation mode.
		if (mPerPixelFogEnable)
		{
			fogSubRenderState->setCalcMode(FFPFog::CM_PER_PIXEL);
		}
		else
		{
			fogSubRenderState->setCalcMode(FFPFog::CM_PER_VERTEX);
		}

		// Invalidate the scheme in order to re-generate all shaders based technique related to this scheme.
		mShaderGenerator->invalidateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
	}
#endif

}

void Sample_ShaderSystem::setAtlasBorderMode( bool enable )
{
#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
	TextureAtlasSamplerFactory::getSingleton().setDefaultAtlasingAttributes(
		TextureAtlasSamplerFactory::ipmRelative, 1, enable);
	mShaderGenerator->invalidateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
#endif
}


//-----------------------------------------------------------------------
void Sample_ShaderSystem::updateSystemShaders()
{
	EntityListIterator it = mTargetEntities.begin();
	EntityListIterator itEnd = mTargetEntities.end();

	for (; it != itEnd; ++it)
	{
		generateShaders(*it);
	}
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::generateShaders(GeometrySpace* entity)
{
	for (unsigned int i=0; i < entity->getNumSubEntities(); ++i)
	{
		SubEntity* curSubEntity = entity->getCom(i);
		const String& curMaterialName = curSubEntity->getMaterialID();
		bool success;

		// Create the shader based technique of this material.
		success = mShaderGenerator->createShaderBasedTechnique(curMaterialName,
            0, RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);


		// Setup custom shader sub render states according to current setup.
		if(success)
		{
			MaterialPtr curMaterial = MaterialManager::getSingleton().getByName(curMaterialName);
			Pass * curPass = curMaterial->get(0)->get(0);

			if(mSpecularEnable)
			{
				curPass->setColour(SOT_Set)->setSpecular(Colour::White);
				curPass->setColour(SOT_Set)->setShininess(32.0);
			}
			else
			{
				curPass->setColour(SOT_Set)->setSpecular(Colour::Black);
				curPass->setColour(SOT_Set)->setShininess(0.0);
			}


			// Grab the first pass render state.
			// NOTE: For more complicated samples iterate over the passes and build each one of them as desired.
			RTShader::RenderState* renderState = mShaderGenerator->getRenderState(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, curMaterialName, 0);

			// Remove all sub render states.
			renderState->reset();


#ifdef RTSHADER_SYSTEM_BUILD_CORE_SHADERS
			if (mCurLightingModel == SSLM_PerVertexLighting)
			{
				RTShader::SubRenderState* perPerVertexLightModel = mShaderGenerator->createSubRenderState(RTShader::FFPLighting::Type);

				renderState->addTemplateSubRenderState(perPerVertexLightModel);
			}
#endif

#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
			else if (mCurLightingModel == SSLM_PerPixelLighting)
			{
				RTShader::SubRenderState* perPixelLightModel = mShaderGenerator->createSubRenderState(RTShader::PerPixelLighting::Type);

				renderState->addTemplateSubRenderState(perPixelLightModel);
			}
			else if (mCurLightingModel == SSLM_NormalMapLightingTangentSpace)
			{
				// Apply normal map only on main entity.
				if (entity->getName() == MAIN_ENTITY_NAME)
				{
					RTShader::SubRenderState* subRenderState = mShaderGenerator->createSubRenderState(RTShader::NormalMapLighting::Type);
					RTShader::NormalMapLighting* normalMapSubRS = static_cast<RTShader::NormalMapLighting*>(subRenderState);

					normalMapSubRS->setNormalMapSpace(RTShader::NormalMapLighting::NMS_TANGENT);
					normalMapSubRS->setNormalMapTextureName("Panels_Normal_Tangent.png");

					renderState->addTemplateSubRenderState(normalMapSubRS);
				}

				// It is secondary entity -> use simple per pixel lighting.
				else
				{
					RTShader::SubRenderState* perPixelLightModel = mShaderGenerator->createSubRenderState(RTShader::PerPixelLighting::Type);
					renderState->addTemplateSubRenderState(perPixelLightModel);
				}
			}
			else if (mCurLightingModel == SSLM_NormalMapLightingObjectSpace)
			{
				// Apply normal map only on main entity.
				if (entity->getName() == MAIN_ENTITY_NAME)
				{
					RTShader::SubRenderState* subRenderState = mShaderGenerator->createSubRenderState(RTShader::NormalMapLighting::Type);
					RTShader::NormalMapLighting* normalMapSubRS = static_cast<RTShader::NormalMapLighting*>(subRenderState);

					normalMapSubRS->setNormalMapSpace(RTShader::NormalMapLighting::NMS_OBJECT);
					normalMapSubRS->setNormalMapTextureName("Panels_Normal_Obj.png");

					renderState->addTemplateSubRenderState(normalMapSubRS);
				}

				// It is secondary entity -> use simple per pixel lighting.
				else
				{
					RTShader::SubRenderState* perPixelLightModel = mShaderGenerator->createSubRenderState(RTShader::PerPixelLighting::Type);
					renderState->addTemplateSubRenderState(perPixelLightModel);
				}
			}

#endif
			if (mReflectionMapEnable)
			{
				RTShader::SubRenderState* subRenderState = mShaderGenerator->createSubRenderState(ShaderExReflectionMap::Type);
				ShaderExReflectionMap* reflectionMapSubRS = static_cast<ShaderExReflectionMap*>(subRenderState);

				reflectionMapSubRS->setReflectionMapType(TEX_TYPE_CUBE_MAP);
				reflectionMapSubRS->setReflectionPower(mReflectionPowerSlider->getValue());

				// Setup the textures needed by the reflection effect.
				reflectionMapSubRS->setMaskMapTextureName("Panels_refmask.png");
				reflectionMapSubRS->setReflectionMapTextureName("cubescene.jpg");

				renderState->addTemplateSubRenderState(subRenderState);
				mReflectionMapSubRS = subRenderState;
			}
			else
			{
				mReflectionMapSubRS = NULL;
			}

			// Invalidate this material in order to re-generate its shaders.
			mShaderGenerator->invalidateMaterial(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, curMaterialName);
		}
	}
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::createDirectionalLight()
{
	Light*  light;
	Vector3 dir;

	light = mSceneMgr->createLight(DIRECTIONAL_LIGHT_NAME);
    light->setType(LT_DIRECTIONAL);
	light->setCastShadow(true);
    dir.x = 0.5;
	dir.y = -1.0;
	dir.z = 0.3;
    dir.normalise();
    light->setDirection(dir);
    light->setDiffuse(0.65, 0.15, 0.15);
    light->setSpecular(0.5, 0.5, 0.5);

	// create pivot node
	mDirectionalLightNode = mSceneMgr->getOriginNode()->create();

	// Create billboard set.
	mBbsFlare = mSceneMgr->createParticleCtrl(0, 20);
	mBbsFlare->setMaterialName("Examples/Flare3");
	mBbsFlare->create(-dir * 500.0)->setColour(light->getDiffuse());
	mBbsFlare->setCastShadow(false);

	mDirectionalLightNode->attachObject(mBbsFlare);
	mDirectionalLightNode->attachObject(light);
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::createPointLight()
{
	Light * light;
	Vector3 dir;

	light = mSceneMgr->createLight(POINT_LIGHT_NAME);
	light->setType(LT_POINT);
	light->setCastShadow(false);
	dir.x = 0.5;
	dir.y = 0.0;
	dir.z = 0.0f;
	dir.normalise();
	light->setDirection(dir);
	light->setDiffuse(0.15, 0.65, 0.15);
	light->setSpecular(0.5, 0.5, 0.5);
    light->setRange(200.0);
    light->setAttenuationConstant(1.0);
    light->setAttenuationLinear(0.0005);
    light->setAttenuationQuadric(0.0);

	// create pivot node
	mPointLightNode = mSceneMgr->getOriginNode()->create();

	ParticleCtrl * bbs;

	// Create billboard set.
	bbs = mSceneMgr->createParticleCtrl(0, 20);
	bbs->setMaterialName("Examples/Flare3");
	bbs->create(Vector3f(200, 100, 0))->setColour(light->getDiffuse());
	bbs->setCastShadow(false);

	mPointLightNode->attachObject(bbs);
	static_cast<SpaceNode *>(mPointLightNode->create(Vector3(200, 100, 0)))->attachObject(light);
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::createSpotLight()
{
	Light * light;
	Vector3 dir;

	light = mSceneMgr->createLight(SPOT_LIGHT_NAME);
	light->setType(LT_SPOT);
	light->setCastShadow(false);
	dir.x = 0.0;
	dir.y = 0.0;
	dir.z = -1.0f;
	dir.normalise();
    light->setSpotlightInner(Angle(20.0));
    light->setSpotlightOuter(Angle(25.0));
    light->setSpotlightFalloff(0.95);
	light->setDirection(dir);
	light->setDiffuse(0.15, 0.15, 0.65);
	light->setSpecular(0.5, 0.5, 0.5);
    light->setRange(1000.0);
    light->setAttenuationConstant(1.0);
    light->setAttenuationLinear(0.0005);
    light->setAttenuationQuadric(0.0);
}

void Sample_ShaderSystem::addModelToScene(const String &  modelName)
{
    mNumberOfModelsAdded++;
    for(int i = 0 ; i < 8 ; i++)
    {
        float scaleFactor = 30;
	    GeometrySpace* entity;
	    SpaceNode* childNode;
        entity = mSceneMgr->createGeo(0, modelName);
	    mLotsOfModelsEntities.push_back(entity);
	    childNode = mSceneMgr->getOriginNode()->create();
	    mLotsOfModelsNodes.push_back(childNode);
	    childNode->setPos(Vector3f(mNumberOfModelsAdded * scaleFactor, 15,  i * scaleFactor));
	    childNode->attachObject(entity);
        MeshPtr modelMesh = MeshManager::getSingleton().getByName(modelName);
        Vector3 modelSize = modelMesh->getAABB().getSize();
        childNode->scale(Vector3f(1 / modelSize.x * scaleFactor,
            1 / modelSize.y * scaleFactor, 1 / modelSize.z * scaleFactor));
    }
}

void Sample_ShaderSystem::updateAddLotsOfModels(bool addThem)
{
	if (mAddedLotsOfModels != addThem)
	{
        mAddedLotsOfModels = addThem;

        if(mNumberOfModelsAdded == 0)
        {
            addModelToScene("Barrel.mesh");
            addModelToScene("facial.mesh");
            addModelToScene("fish.mesh");
            addModelToScene("ninja.mesh");
            addModelToScene("penguin.mesh");
            addModelToScene("razor.mesh");
            addModelToScene("RZR-002.mesh");
            addModelToScene("tudorhouse.mesh");
            addModelToScene("WoodPallet.mesh");
        }
        for (size_t i = 0 ; i < mLotsOfModelsNodes.size() ; i++)
        {
            mLotsOfModelsNodes[i]->setVisible(mAddedLotsOfModels);
        }
    }
}
//-----------------------------------------------------------------------
void Sample_ShaderSystem::updateInstancedViewports(bool enabled)
{
	if(mInstancedViewportsEnable != enabled)
	{
		mInstancedViewportsEnable = enabled;

		if(mInstancedViewportsEnable)
		{
			mCamera->setExtCulling(&mInfiniteFrustum);

			// having problems with bb...
			mDirectionalLightNode->detach(mBbsFlare);
		}
		else
		{
			mCamera->setExtCulling(0);
			mDirectionalLightNode->attachObject(mBbsFlare);
		}

		if(mInstancedViewportsEnable)
		{
			createInstancedViewports();
		}
		else
		{
			destroyInstancedViewports();
		}
	}
}
//-----------------------------------------------------------------------
void Sample_ShaderSystem::updateLightState(const String& lightName, bool visible)
{
	if (mSceneMgr->isLightExist(lightName))
	{
		// Case it is the point light,
		// toggle its visibility and billboard set visibility.
		if (lightName == POINT_LIGHT_NAME)
		{
			if (visible)
			{
				if (mPointLightNode->isEnable() == false)
				{
					mSceneMgr->getOriginNode()->add(mPointLightNode);
				}
			}
			else
			{
				if (mPointLightNode->isEnable() == true)
				{
					mSceneMgr->getOriginNode()->remove(mPointLightNode);
				}
			}
			mSceneMgr->getLight(lightName)->setVisible(visible);
		}

		// Case it is the directional light,
		// toggle its visibility and billboard set visibility.
		else if (lightName == DIRECTIONAL_LIGHT_NAME)
		{
			SpaceNode::AttachList::iterator it, itend = mDirectionalLightNode->getAttachList().end();
            for(it = mDirectionalLightNode->getAttachList().begin(); it != itend; ++it)
			{
				SpaceObj* o = it->second;
				o->setVisible(visible);
			}
		}

		// Spot light has no scene node representation.
		else
		{
			mSceneMgr->getLight(lightName)->setVisible(visible);
		}

		RenderState* schemRenderState = mShaderGenerator->getRenderState(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

		int lightCount[3] = {0};

		// Update point light count.
		if (mSceneMgr->getLight(POINT_LIGHT_NAME)->isVisible())
		{
			lightCount[0] = 1;
		}

		// Update directional light count.
		if (mSceneMgr->getLight(DIRECTIONAL_LIGHT_NAME)->isVisible())
		{
			lightCount[1] = 1;
		}

		// Update spot light count.
		if (mSceneMgr->getLight(SPOT_LIGHT_NAME)->isVisible())
		{
			lightCount[2] = 1;
		}

		// Update the scheme light count.
		schemRenderState->setLightCount(lightCount);


		// Invalidate the scheme in order to re-generate all shaders based technique related to this scheme.
		mShaderGenerator->invalidateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
	}
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::applyShadowType(int menuIndex)
{
	// Grab the scheme render state.
	Ogre::RTShader::RenderState* schemRenderState = mShaderGenerator->getRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);


	// No shadow
	if (menuIndex == 0)
	{
		mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);

#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
		const Ogre::RTShader::SubRenderStateList& subRenderStateList = schemRenderState->getTemplateSubRenderStateList();
		Ogre::RTShader::SubRenderStateListConstIterator it = subRenderStateList.begin();
		Ogre::RTShader::SubRenderStateListConstIterator itEnd = subRenderStateList.end();

		for (; it != itEnd; ++it)
		{
			Ogre::RTShader::SubRenderState* curSubRenderState = *it;

			// This is the pssm3 sub render state -> remove it.
			if (curSubRenderState->getType() == Ogre::RTShader::IntegratedPSSM3::Type)
			{
				schemRenderState->removeTemplateSubRenderState(*it);
				break;
			}
		}
#endif

		mTrayMgr->moveWidgetToTray(mDirLightCheckBox, TL_TOPLEFT, 1);
		mTrayMgr->moveWidgetToTray(mPointLightCheckBox, TL_TOPLEFT, 2);
		mTrayMgr->moveWidgetToTray(mSpotLightCheckBox, TL_TOPLEFT, 3);

		mDirLightCheckBox->show();
		mPointLightCheckBox->show();
		mSpotLightCheckBox->show();

	}

#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
	// Integrated shadow PSSM with 3 splits.
	else if (menuIndex == 1)
	{
		mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);

		// 3 textures per directional light
		mSceneMgr->getRenderPattern()->setTextureCount(NII::LT_DIRECTIONAL, 3);
		mSceneMgr->getRenderPattern()->setTextureConfig(512, 512, 3, PF_R32);
		mSceneMgr->setSelfShadow(true);

		// Leave only directional light.
		mDirLightCheckBox->setChecked(true);
		mPointLightCheckBox->setChecked(false);
		mSpotLightCheckBox->setChecked(false);

		mTrayMgr->removeWidgetFromTray(mDirLightCheckBox);
		mTrayMgr->removeWidgetFromTray(mPointLightCheckBox);
		mTrayMgr->removeWidgetFromTray(mSpotLightCheckBox);
		mDirLightCheckBox->hide();
		mPointLightCheckBox->hide();
		mSpotLightCheckBox->hide();

		// Set up caster material - this is just a standard depth/shadow map caster
		mSceneMgr->getRenderPattern()->setTextureCasterMaterial("PSSM/shadow_caster");

		// Disable fog on the caster pass.
		MaterialPtr passCaterMaterial = MaterialManager::getSingleton().getByName("PSSM/shadow_caster");
		Pass * pssmCasterPass = passCaterMaterial->get(0)->get(0);
		pssmCasterPass->setFog(ShaderChFog());

		// shadow camera setup
		PSSMShadowSetup * pssmSetup = N_new PSSMShadowSetup();
		pssmSetup->calculateSplitPoints(3, 5, 3000);
		pssmSetup->setSplitPadding(10);
		pssmSetup->setOptimalAdjustFactor(0, 2);
		pssmSetup->setOptimalAdjustFactor(1, 1);
		pssmSetup->setOptimalAdjustFactor(2, 0.5);

		mSceneMgr->setShadowGenCamera(pssmSetup);

		Ogre::RTShader::SubRenderState* subRenderState = mShaderGenerator->createSubRenderState(Ogre::RTShader::IntegratedPSSM3::Type);
		Ogre::RTShader::IntegratedPSSM3* pssm3SubRenderState = static_cast<Ogre::RTShader::IntegratedPSSM3*>(subRenderState);
		const PSSMShadowSetup::SplitPointList& srcSplitPoints = pssmSetup->getSplitPoints();
		Ogre::RTShader::IntegratedPSSM3::SplitPointList dstSplitPoints;

		for (unsigned int i=0; i < srcSplitPoints.size(); ++i)
		{
			dstSplitPoints.push_back(srcSplitPoints[i]);
		}

		pssm3SubRenderState->setSplitPoints(dstSplitPoints);
		schemRenderState->addTemplateSubRenderState(subRenderState);
	}
#endif

	// Invalidate the scheme in order to re-generate all shaders based technique related to this scheme.
	mShaderGenerator->invalidateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::exportRTShaderSystemMaterial(const String& fileName, const String& materialName)
{
	// Grab material pointer.
	MaterialPtr materialPtr = MaterialManager::getSingleton().getByName(materialName);

	// Create shader based technique.
	bool success = mShaderGenerator->createShaderBasedTechnique(materialName,
		0, RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

	// Case creation of shader based technique succeeded.
	if (success)
	{
		// Force shader generation of the given material.
		RTShader::ShaderGenerator::getSingleton().validateMaterial(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, materialName);

		// Grab the RTSS material serializer listener.
		MaterialSerializer::Listener* matRTSSListener = RTShader::ShaderGenerator::getSingleton().getMaterialSerializerListener();
		MaterialSerializer matSer;

		// Add the custom RTSS listener to the serializer.
		// It will make sure that every custom parameter needed by the RTSS
		// will be added to the exported material script.
		matSer.add(matRTSSListener);

		// Simply export the material.
		matSer.exportMaterial(materialPtr, fileName, false, false, "", materialPtr->getName() + "_RTSS_Export");
	}
}

//-----------------------------------------------------------------------
Ogre::StringList Sample_ShaderSystem::getRequiredPlugins()
{
	StringList names;
    if (!GpuProgramManager::getSingleton().isSyntaxSupported("glsles") &&
        !GpuProgramManager::getSingleton().isSyntaxSupported("glsl"))
        names.push_back("Cg Program Manager");
	return names;
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::testCapabilities(const RenderSysFeature * caps)
{
	if(!caps->hasCapability(GF_Program_Vertex) || !(caps->hasCapability(GF_Program_Fragment)))
	{
		N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your graphics card does not support vertex and fragment programs, "
			"so you cannot run this sample. Sorry!", "Sample_ShaderSystem::testCapabilities");
	}

	// Check if D3D10 shader is supported - is so - then we are OK.
	if (GpuProgramManager::getSingleton().isSyntaxSupported("ps_4_0"))
	{
		return;
	}

	// Check if GLSL type shaders are supported - is so - then we are OK.
	if (GpuProgramManager::getSingleton().isSyntaxSupported("glsles") ||
        GpuProgramManager::getSingleton().isSyntaxSupported("glsl"))
	{
		return;
	}

	if (!GpuProgramManager::getSingleton().isSyntaxSupported("arbfp1") &&
		!GpuProgramManager::getSingleton().isSyntaxSupported("ps_2_0"))
	{
		N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your card does not support shader model 2, "
			"so you cannot run this sample. Sorry!", "Sample_ShaderSystem::testCapabilities");
	}
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::loadResources()
{
	// Create and add the custom reflection map shader extension factory to the shader generator.
	mReflectionMapFactory = N_new ShaderExReflectionMapFactory;
	mShaderGenerator->addSubRenderStateFactory(mReflectionMapFactory);

	createPrivateResourceGroup();
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::createPrivateResourceGroup()
{
	// Create the resource group of the RT Shader System Sample.
	ResourceSchemeManager& rgm = ResourceSchemeManager::getSingleton();

	mExportMaterialPath = "C:/";

	rgm.create(SAMPLE_MATERIAL_GROUP, false);
	rgm.add(mExportMaterialPath, "FileSystem", SAMPLE_MATERIAL_GROUP);
	rgm.setup(SAMPLE_MATERIAL_GROUP);
	rgm.load(SAMPLE_MATERIAL_GROUP, true);
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::unloadResources()
{
	destroyPrivateResourceGroup();

	mShaderGenerator->removeAllShaderBasedTechniques("Panels");
	mShaderGenerator->removeAllShaderBasedTechniques("Panels_RTSS_Export");

	if (mReflectionMapFactory != NULL)
	{
		mShaderGenerator->removeSubRenderStateFactory(mReflectionMapFactory);
		N_delete mReflectionMapFactory;
		mReflectionMapFactory = NULL;
	}
}
//-----------------------------------------------------------------------
void Sample_ShaderSystem::destroyPrivateResourceGroup()
{
	// Destroy the resource group of the RT Shader System
	ResourceSchemeManager& rgm = ResourceSchemeManager::getSingleton();

	rgm.destroyResourceGroup(SAMPLE_MATERIAL_GROUP);
}
//-----------------------------------------------------------------------
void Sample_ShaderSystem::pickTargetObject(const OIS::MouseEvent & evt)
{
	int xPos   = evt.state.X.abs;
	int yPos   = evt.state.Y.abs;
	int width  = evt.state.width;
	int height = evt.state.height;

	Ray mouseRay = mCamera->getRay(xPos / float(width), yPos/float(height));
	mRayQuery->setRay(mouseRay);

	RaySpaceQueryResult &result = mRayQuery->execute();
	RaySpaceQueryResult::iterator it = result.begin();
	RaySpaceQueryResult::iterator itEnd = result.end();

	for(; it != itEnd; ++it)
	{
		QueryDetail & curEntry = *it;

		if(mTargetObj != NULL)
		{
			mTargetObj->getSceneNode()->setAABBView(false);
		}

		mTargetObj = curEntry.mSpaceObj;
		mTargetObj ->getSceneNode()->setAABBView(true);
	}
}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::updateTargetObjInfo()
{
	if (mTargetObj == NULL)
		return;

	String targetObjMaterialName;

	if(mTargetObj->getFactoryID() == "GeometrySpace")
	{
		GeometrySpace * targetEnt = static_cast<GeometrySpace *>(mTargetObj);
		targetObjMaterialName = targetEnt->getCom(0)->getMaterialID();
	}

	mTargetObjMatName->setCaption(targetObjMaterialName);

	if (mViewport->getMaterial() == RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
	{
		MaterialPtr matMainEnt = MaterialManager::getSingleton().getByName(targetObjMaterialName);

		if(matMainEnt.isNull() == false)
		{
			Technique * shaderGeneratedTech = NULL;
			for(unsigned int i = 0; i < matMainEnt->getCount(); ++i)
			{
				Technique * curTech = matMainEnt->get(i);
				if(curTech->getSchemeID() == RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
				{
					shaderGeneratedTech = curTech;
					break;
				}
			}

			if(shaderGeneratedTech != NULL)
			{
                GpuProgramID id;
                shaderGeneratedTech->get(0)->getProgram().get(0, ST_VS, &id);
				mTargetObjVS->setCaption("VS: " + id);
				shaderGeneratedTech->get(0)->getProgram().get(0, ST_FS, &id);
                mTargetObjFS->setCaption("FS: " + id);
			}
		}
	}
	else
	{
		mTargetObjVS->setCaption("VS: N/A");
		mTargetObjFS->setCaption("FS: N/A");
	}
}
//-----------------------------------------------------------------------
void Sample_ShaderSystem::changeTextureLayerBlendMode()
{
	LayeredBlending::BlendMode curBlendMode = mLayerBlendSubRS->getBlendMode(1);
	LayeredBlending::BlendMode nextBlendMode;

	// Update the next blend layer mode.
	if (curBlendMode == LayeredBlending::LB_BlendLuminosity)
	{
		nextBlendMode = LayeredBlending::LB_FFPBlend;
	}
	else
	{
		nextBlendMode = (LayeredBlending::BlendMode)(curBlendMode + 1);
	}


	mLayerBlendSubRS->setBlendMode(1, nextBlendMode);
	mShaderGenerator->invalidateMaterial(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, "RTSS/LayeredBlending");

	// Update the caption.
	updateLayerBlendingCaption(nextBlendMode);

}

//-----------------------------------------------------------------------
void Sample_ShaderSystem::updateLayerBlendingCaption( LayeredBlending::BlendMode nextBlendMode )
{
	switch (nextBlendMode)
	{
	case LayeredBlending::LB_FFPBlend:
		mLayerBlendLabel->setCaption("FFP Blend");
		break;

	case LayeredBlending::LB_BlendNormal:
		mLayerBlendLabel->setCaption("Normal");
		break;

	case LayeredBlending::LB_BlendLighten:
		mLayerBlendLabel->setCaption("Lighten");
		break;

	case LayeredBlending::LB_BlendDarken:
		mLayerBlendLabel->setCaption("Darken");
		break;

	case LayeredBlending::LB_BlendMultiply:
		mLayerBlendLabel->setCaption("Multiply");
		break;

	case LayeredBlending::LB_BlendAverage:
		mLayerBlendLabel->setCaption("Average");
		break;

	case LayeredBlending::LB_BlendAdd:
		mLayerBlendLabel->setCaption("Add");
		break;

	case LayeredBlending::LB_BlendSubtract:
		mLayerBlendLabel->setCaption("Subtract");
		break;

	case LayeredBlending::LB_BlendDifference:
		mLayerBlendLabel->setCaption("Difference");
		break;

	case LayeredBlending::LB_BlendNegation:
		mLayerBlendLabel->setCaption("Negation");
		break;

	case LayeredBlending::LB_BlendExclusion:
		mLayerBlendLabel->setCaption("Exclusion");
		break;

	case LayeredBlending::LB_BlendScreen:
		mLayerBlendLabel->setCaption("Screen");
		break;

	case LayeredBlending::LB_BlendOverlay:
		mLayerBlendLabel->setCaption("Overlay");
		break;

	case LayeredBlending::LB_BlendSoftLight:
		mLayerBlendLabel->setCaption("SoftLight");
		break;

	case LayeredBlending::LB_BlendHardLight:
		mLayerBlendLabel->setCaption("HardLight");
		break;

	case LayeredBlending::LB_BlendColorDodge:
		mLayerBlendLabel->setCaption("ColorDodge");
		break;

	case LayeredBlending::LB_BlendColorBurn:
		mLayerBlendLabel->setCaption("ColorBurn");
		break;

	case LayeredBlending::LB_BlendLinearDodge:
		mLayerBlendLabel->setCaption("LinearDodge");
		break;

	case LayeredBlending::LB_BlendLinearBurn:
		mLayerBlendLabel->setCaption("LinearBurn");
		break;

	case LayeredBlending::LB_BlendLinearLight:
		mLayerBlendLabel->setCaption("LinearLight");
		break;

	case LayeredBlending::LB_BlendVividLight:
		mLayerBlendLabel->setCaption("VividLight");
		break;

	case LayeredBlending::LB_BlendPinLight:
		mLayerBlendLabel->setCaption("PinLight");
		break;

	case LayeredBlending::LB_BlendHardMix:
		mLayerBlendLabel->setCaption("HardMix");
		break;

	case LayeredBlending::LB_BlendReflect:
		mLayerBlendLabel->setCaption("Reflect");
		break;

	case LayeredBlending::LB_BlendGlow:
		mLayerBlendLabel->setCaption("Glow");
		break;

	case LayeredBlending::LB_BlendPhoenix:
		mLayerBlendLabel->setCaption("Phoenix");
		break;

	case LayeredBlending::LB_BlendSaturation:
		mLayerBlendLabel->setCaption("Saturation");
		break;

	case LayeredBlending::LB_BlendColor:
		mLayerBlendLabel->setCaption("Color");
		break;

	case LayeredBlending::LB_BlendLuminosity:
		mLayerBlendLabel->setCaption("Luminosity");
		break;
	default:
		break;
	}
}

#if (OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS) && (OGRE_PLATFORM != OGRE_PLATFORM_ANDROID)

//-----------------------------------------------------------------------
bool Sample_ShaderSystem::mousePressed( const OIS::MouseEvent& evt, OIS::MouseButtonID id )
{
	if(mTrayMgr->injectMouseDown(evt, id))
		return true;
	if(id == OIS::MB_Left)
		mTrayMgr->hideCursor();  // hide the cursor if user left-clicks in the scene
	if(id == OIS::MB_Right)
		pickTargetObject(evt);

	return true;
}
//-----------------------------------------------------------------------
bool Sample_ShaderSystem::mouseReleased(const OIS::MouseEvent & evt, OIS::MouseButtonID id )
{
	if(mTrayMgr->injectMouseUp(evt, id))
		return true;
	if(id == OIS::MB_Left)
		mTrayMgr->showCursor();  // unhide the cursor if user lets go of LMB

	return true;
}
//-----------------------------------------------------------------------
bool Sample_ShaderSystem::mouseMoved(const OIS::MouseEvent & evt)
{
	// only rotate the camera if cursor is hidden
	if(mTrayMgr->isCursorVisible())
		mTrayMgr->injectMouseMove(evt);
	else
		mCameraMan->injectMouseMove(evt);
	return true;
}
#endif
//-----------------------------------------------------------------------
void Sample_ShaderSystem::destroyInstancedViewports()
{
	if(mInstancedViewportsSubRenderState)
	{
		Ogre::RTShader::RenderState * renderState = mShaderGenerator->getRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
		renderState->removeTemplateSubRenderState(mInstancedViewportsSubRenderState);
		mInstancedViewportsSubRenderState = NULL;
	}

	if(mRoot->getRender()->getInstanceData() != NULL)
	{
		NII::GBufferManager::getSingleton().destroy(
			mRoot->getRender()->getInstanceData());
		mRoot->getRender()->setInstanceData((VertexData *)0);
	}
	mRoot->getRender()->setInstanceCount(1);
	mRoot->getRender()->setInstanceData((VertexBuffer *) 0);

	mShaderGenerator->invalidateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
	mShaderGenerator->validateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

	destroyInstancedViewportsFactory();
}
//-----------------------------------------------------------------------
void Sample_ShaderSystem::destroyInstancedViewportsFactory()
{
	if (mInstancedViewportsFactory != NULL)
	{
		mInstancedViewportsFactory->destroyAllInstances();
		mShaderGenerator->removeSubRenderStateFactory(mInstancedViewportsFactory);
		delete mInstancedViewportsFactory;
		mInstancedViewportsFactory = NULL;
	}
}
//-----------------------------------------------------------------------
void Sample_ShaderSystem::createInstancedViewports()
{
	if (mInstancedViewportsFactory == NULL)
	{
		mInstancedViewportsFactory = N_new ShaderExInstancedViewportsFactory;
		mShaderGenerator->addSubRenderStateFactory(mInstancedViewportsFactory);
	}

	Ogre::Vector2 monitorCount(2.0, 2.0);
	mInstancedViewportsSubRenderState = mShaderGenerator->createSubRenderState(Ogre::RTShader::ShaderExInstancedViewports::Type);
	Ogre::RTShader::ShaderExInstancedViewports * shaderExInstancedViewports
		= static_cast<Ogre::RTShader::ShaderExInstancedViewports *>(mInstancedViewportsSubRenderState);
	shaderExInstancedViewports->setMonitorsCount(monitorCount);
	Ogre::RTShader::RenderState * renderState = mShaderGenerator->getRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
	renderState->addTemplateSubRenderState(mInstancedViewportsSubRenderState);

	NII::VertexData * vd;
    NII::N_Only(GBuffer).create(vd);
	size_t offset = 0;
	offset = vd->getUnitSize(0);
	vd->add(Ogre::DT_Float4x, Ogre::VT_Tex_Coord, 0, offset, 3);
	offset = vd->getUnitSize(0);
	vd->add(Ogre::DT_Float4x, Ogre::VT_Tex_Coord, 0, offset, 4);
	offset = vd->getUnitSize(0);
	vd->add(Ogre::DT_Float4x, Ogre::VT_Tex_Coord, 0, offset, 5);
	offset = vd->getUnitSize(0);
	vd->add(Ogre::DT_Float4x, Ogre::VT_Tex_Coord, 0, offset, 6);
	offset = vd->getUnitSize(0);
	vd->add(Ogre::DT_Float4x, Ogre::VT_Tex_Coord, 0, offset, 7);

	Ogre::VertexBuffer * vbuf;
    Ogre::GBufferManager::getSingleton().create(vbuf,
		vd->getUnitSize(0), monitorCount.x * monitorCount.y, NII::Buffer::Buffer::M_DEV | NII::Buffer::M_WRITE);
	vbuf->setInstancingDivisor(1);

	float * buf = (float *)vbuf->lock(NII::Buffer::MM_WHOLE);
	for (float x = 0 ; x < monitorCount.x ; x++)
		for (float y = 0 ; y < monitorCount.y ; y++)
		{
			*buf = x; buf++;
			*buf = y; buf++;
			*buf = 0; buf++;
			*buf = 0; buf++;

			NII::Quaternion q;
			NII::Radian angle = NII::Angle(90 / ( monitorCount.x *  monitorCount.y) * (x + y * monitorCount.x) );
			q.from(angle, NII::Vector3::Y);
			q.normalise();
			NII::Matrix3 rotMat;
			q.to(rotMat);

			*buf = rotMat.GetColumn(0).x; buf++;
			*buf = rotMat.GetColumn(0).y; buf++;
			*buf = rotMat.GetColumn(0).z; buf++;
			*buf = x * -20; buf++;

			*buf = rotMat.GetColumn(1).x; buf++;
			*buf = rotMat.GetColumn(1).y; buf++;
			*buf = rotMat.GetColumn(1).z; buf++;
			*buf = 0; buf++;

			*buf = rotMat.GetColumn(2).x; buf++;
			*buf = rotMat.GetColumn(2).y; buf++;
			*buf = rotMat.GetColumn(2).z; buf++;
			*buf =  y * 20; buf++;

			*buf = 0; buf++;
			*buf = 0; buf++;
			*buf = 0; buf++;
			*buf = 1; buf++;
		}
		vbuf->unlock();

		mRoot->getRender()->setInstanceData(vbuf);
		mRoot->getRender()->setInstanceData(vd);
		mRoot->getRender()->setInstanceCount(monitorCount.x * monitorCount.y);

		// Invalidate the scheme in order to re-generate all shaders based technique related to this scheme.
		mShaderGenerator->invalidateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
		mShaderGenerator->validateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
}

void Sample_ShaderSystem::createMaterialForTexture(const String & texName, bool isTextureAtlasTexture )
{
	MaterialManager * mag = MaterialManager::getSingletonPtr();
	if(mag->isExist(texName) == false)
	{
		MaterialPtr mat = mag->create(texName, GroupDefault);
		mat->get(0)->get(0)->setLight(false);
		ShaderChTextureUnit * unit = mat->get(0)->get(0)->getTexture().create(texName);
		if(isTextureAtlasTexture)
		{
			// to solve wrap edge bleed
			unit->setFilter(TFM_TRILINEAR);
		}
	}
}

CustomSpaceObj * Sample_ShaderSystem::createTextureAtlasObject()
{
	TextureAtlasSamplerFactory * textureAtlasSamplerFactory =
		static_cast<TextureAtlasSamplerFactory *>(mShaderGenerator->getSubRenderStateFactory(TextureAtlasSampler::Type));
	TextureAtlasTablePtr textureAtlasTable(new TextureAtlasTable);

	DataStream * taiFile = NII::N_Only(ResourceScheme).openResource("TextureAtlasSampleWrap.tai");

	textureAtlasSamplerFactory->addTexutreAtlasDefinition(taiFile, textureAtlasTable);

    N_delete taiFile;

	//Generate the geometry that will seed the particle system
	CustomSpaceObj* textureAtlasObject = mSceneMgr->createCustomGeo("TextureAtlasObject");

	int sliceSize = 30.0;
	int wrapSize = 5.0;

	String curMatName;

	// create original texture geometry
	for( size_t i = 0 ; i < textureAtlasTable->size() ; i++ )
	{
		bool changeMat = (curMatName != (*textureAtlasTable)[i].atlasTextureName);

		if (changeMat)
		{
			if (curMatName.empty() == false) // we don't want to end before we begin
			{
				textureAtlasObject->end();
			}

			curMatName = (*textureAtlasTable)[i].originalTextureName;
			createMaterialForTexture(curMatName, false);
			textureAtlasObject->begin(GeometryRaw::OT_TRIANGLE_LIST, curMatName);
		}

		// triangle 0
		textureAtlasObject->position(i * sliceSize, 0, 0); //Position
		textureAtlasObject->textureCoord(0,0); //UV

		textureAtlasObject->position(i * sliceSize, 0, sliceSize); //Position
		textureAtlasObject->textureCoord(0,wrapSize); //UV

		textureAtlasObject->position((i + 1) * sliceSize, 0 , sliceSize); //Position
		textureAtlasObject->textureCoord(wrapSize,wrapSize); //UV

		// triangle 1
		textureAtlasObject->position(i * sliceSize, 0, 0); //Position
		textureAtlasObject->textureCoord(0,0); //UV

		textureAtlasObject->position((i + 1) * sliceSize, 0, sliceSize); //Position
		textureAtlasObject->textureCoord(wrapSize,wrapSize); //UV

		textureAtlasObject->position((i + 1) * sliceSize, 0, 0); //Position
		textureAtlasObject->textureCoord(wrapSize, 0); //UV

	}

	// create texture atlas geometry
	for( size_t i = 0 ; i < (*textureAtlasTable).size() ; i++ )
	{
		bool changeMat = (curMatName != (*textureAtlasTable)[i].atlasTextureName);

		if (changeMat)
		{
			if (curMatName.empty() == false) // we don't want to end before we begin
			{
				textureAtlasObject->end();
			}

			curMatName = (*textureAtlasTable)[i].atlasTextureName;
			createMaterialForTexture(curMatName, true);
			textureAtlasObject->begin(GeometryRaw::OT_TRIANGLE_LIST, curMatName);
		}

		// triangle 0
		textureAtlasObject->position(i * sliceSize, 0, sliceSize); //Position
		textureAtlasObject->textureCoord(0,0); //UV
		textureAtlasObject->textureCoord((*textureAtlasTable)[i].indexInAtlas); //Texture ID

		textureAtlasObject->position(i * sliceSize, 0, sliceSize * 2); //Position
		textureAtlasObject->textureCoord(0,wrapSize); //UV
		textureAtlasObject->textureCoord((*textureAtlasTable)[i].indexInAtlas); //Texture ID

		textureAtlasObject->position((i + 1) * sliceSize, 0 , sliceSize * 2); //Position
		textureAtlasObject->textureCoord(wrapSize,wrapSize); //UV
		textureAtlasObject->textureCoord((*textureAtlasTable)[i].indexInAtlas); //Texture ID

		// triangle 1
		textureAtlasObject->position(i * sliceSize, 0, sliceSize); //Position
		textureAtlasObject->textureCoord(0,0); //UV
		textureAtlasObject->textureCoord((*textureAtlasTable)[i].indexInAtlas); //Texture ID

		textureAtlasObject->position((i + 1) * sliceSize, 0, sliceSize * 2); //Position
		textureAtlasObject->textureCoord(wrapSize,wrapSize); //UV
		textureAtlasObject->textureCoord((*textureAtlasTable)[i].indexInAtlas); //Texture ID

		textureAtlasObject->position((i + 1) * sliceSize, 0, sliceSize); //Position
		textureAtlasObject->textureCoord(wrapSize, 0); //UV
		textureAtlasObject->textureCoord((*textureAtlasTable)[i].indexInAtlas); //Texture ID
	}
	textureAtlasObject->end();
	return textureAtlasObject;
}