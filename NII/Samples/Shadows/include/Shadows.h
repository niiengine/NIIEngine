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

/**
    \file
        Shadows.cpp
    \brief
        Shows a few ways to use Ogre's shadowing techniques
*/

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

// New depth shadowmapping
String CUSTOM_ROCKWALL_MATERIAL("Ogre/DepthShadowmap/Receiver/RockWall");
String CUSTOM_CASTER_MATERIAL("Ogre/DepthShadowmap/Caster/Float");
String CUSTOM_RECEIVER_MATERIAL("Ogre/DepthShadowmap/Receiver/Float");
String CUSTOM_ATHENE_MATERIAL("Ogre/DepthShadowmap/Receiver/Athene");

String BASIC_ROCKWALL_MATERIAL("Examples/Rockwall");
String BASIC_ATHENE_MATERIAL("Examples/Athene/NormalMapped");

/** This class 'wibbles' the light and billboard */
class LightWibbler : public DataValue<NIIf>
{
public:
    LightWibbler(Light * light, SyncParticle * billboard, const Colour & minColour,
        const Colour & maxColour, NIIf minSize, NIIf maxSize)
    {
        mLight = light;
        mBillboard = billboard;
        mMinColour = minColour;
        mColourRange.r = maxColour.r - minColour.r;
        mColourRange.g = maxColour.g - minColour.g;
        mColourRange.b = maxColour.b - minColour.b;
        mMinSize = minSize;
        mSizeRange = maxSize - minSize;
    }

    virtual NIIf getValue(void) const
    {
        return intensity;
    }

    virtual void  setValue(NIIf value)
    {
        intensity = value;

        Colour newColour;

        // Attenuate the brightness of the light
        newColour.r = mMinColour.r + (mColourRange.r * intensity);
        newColour.g = mMinColour.g + (mColourRange.g * intensity);
        newColour.b = mMinColour.b + (mColourRange.b * intensity);

        mLight->setDiffuse(newColour);
        mBillboard->setColour(newColour);
        // set billboard size
        NIIf newSize = mMinSize + (intensity * mSizeRange);
        mBillboard->setSize(newSize, newSize);
    }
protected:
    Light * mLight;
    SyncParticle * mBillboard;
    Colour mColourRange;
    Colour mMinColour;
    NIIf mMinSize;
    NIIf mSizeRange;
    NIIf intensity;
};

NIIf timeDelay = 0;

class _OgreSampleClassExport Sample_Shadows : public SdkSample
{
protected:
	GeometrySpace* mAthene;
	AnimationFusion* mAnimState;
	GeometrySpace* pPlaneEnt;
	vector<GeometrySpace*>::type pColumns;
	Light* mLight;
	Light* mSunLight;
	SpaceNode* mLightNode;
	AnimationFusion* mLightAnimationState;
	Colour mMinLightColour;
	Colour mMaxLightColour;
	NIIf mMinFlareSize;
	NIIf mMaxFlareSize;
	DataEquation<NIIf>* mController;

	enum ShadowProjection
	{
		UNIFORM,
		UNIFORM_FOCUSED,
		LISPSM,
		PLANE_OPTIMAL
	};

	enum ShadowMaterial
	{
		MAT_STANDARD,
		MAT_DEPTH_FLOAT,
		MAT_DEPTH_FLOAT_PCF
	};

	ShadowTechnique mCurrentShadowTechnique;
	ShadowProjection mCurrentProjection;
	ShadowMaterial mCurrentMaterial;

	GpuProgramParam * mCustomRockwallVparams;
	GpuProgramParam * mCustomRockwallFparams;
	GpuProgramParam * mCustomAtheneVparams;
	GpuProgramParam * mCustomAtheneFparams;

	// transient pointer to LiSPSM setup if present
	LiSPSMShadowSetup* mLiSPSMSetup;    
	CameraSetup * mCurrentShadowGenCamera;
	/// Plane that defines plane-optimal shadow mapping basis
	MovablePlane* mPlane;
    BoxSky * mBoxSky;
	bool mIsOpenGL;
    
public:
	bool onEndRender(const RenderFrameArgs * evt)
    {
		if(mAnimState)
            mAnimState->addTime(evt->mCurrent);
		return SdkSample::onEndRender(evt);
	}

	Sample_Shadows()
		: mAnimState(0)
		, mLightNode(0)
		, mLightAnimationState(0)
		, mMinLightColour(0.2, 0.1, 0.0)
		, mMaxLightColour(0.5, 0.3, 0.1)
		, mMinFlareSize(40)
		, mMaxFlareSize(80)
		, mController(0)
		, mPlane(0)
	{
		mInfo["Title"] = "Shadows";
		mInfo["Description"] = "A demonstration of ogre's various shadowing techniques.";
		mInfo["Thumbnail"] = "thumb_shadows.png";
		mInfo["Category"] = "Lighting";
        mBoxSky = 0;
    }
    ~Sample_Shadows()
    {
        if(mBoxSky)
        {
            mSceneMgr->addSky(mBoxSky);
            N_delete mBoxSky;
            mBoxSky = 0;
        }
    }
protected:

	// Override this to ensure FPU mode
	//bool configure(void)
	//{
	//	// Show the configuration dialog and init the system
	//	// You can skip this and use root.restoreConfig() to load configuration
	//	// settings if you were sure there are valid ones saved in ogre.cfg
	//	if(mRoot->config())
	//	{
	//		// Custom option - to use PlaneOptimalShadowGenCamera we must have
	//		// double-precision. Thus, set the D3D floating point mode if present,
	//		// no matter what was chosen
	//		SysOptionList& optMap = mRoot->getRender()->getConfig();
	//		SysOptionList::iterator i = optMap.find("Floating-point mode");
	//		if (i != optMap.end())
	//		{
	//			if (i->second.mValue != "Consistent")
	//			{
	//				i->second.mValue = "Consistent";
	//				LogManager::getSingleton().logMessage("Demo_Shadows: overriding "
	//					"D3D floating point mode to 'Consistent' to ensure precision "
	//					"for plane-optimal camera setup option");

	//			}
	//		}

	//		// If returned true, user clicked OK so init
	//		// Here we choose to let the system create a default rendering window by passing 'true'
	//		mWindow = mRoot->init(true);
	//		return true;
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}

    // Just override the mandatory create scene method
    void setupContent(void)
    {
		// Need to detect D3D or GL for best depth shadowmapping
		mIsOpenGL = Root::getSingleton().getRender()->getName().find("GL") != String::npos;

		// do this first so we generate edge lists
		if (mRoot->getRender()->getFeature()->hasCapability(GF_HW_Stencil))
        {
            mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
            mCurrentShadowTechnique = SHADOWTYPE_STENCIL_ADDITIVE;
        }
        else
        {
            mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
            mCurrentShadowTechnique = SHADOWTYPE_TEXTURE_MODULATIVE;
        }
        // Set ambient light off
        mSceneMgr->setAmbient(Colour(0.0, 0.0, 0.0));

        // Fixed light, dim
        mSunLight = mSceneMgr->createLight("SunLight");
        mSunLight->setType(LT_SPOT);
        mSunLight->setPos(1500,1750,1300);
        mSunLight->setSpotlightInner(Angle(30));
        mSunLight->setSpotlightOuter(Angle(50));
        mSunLight->setSpotlightFalloff(1.0);
        Vector3 dir;
        dir = -mSunLight->getPos();
        dir.normalise();
        mSunLight->setDirection(dir);
        mSunLight->setDiffuse(0.35, 0.35, 0.38);
        mSunLight->setSpecular(0.9, 0.9, 1);

        // Point light, movable, reddish
        mLight = mSceneMgr->createLight("Light2");
        mLight->setDiffuse(mMinLightColour);
        mLight->setSpecular(1, 1, 1);
        mLight->setRange(8000);
        mLight->setAttenuationConstant(1);
        mLight->setAttenuationLinear(0.0005);
        mLight->setAttenuationQuadric(0);

        // Create light node
        mLightNode = mSceneMgr->getOriginNode()->create("MovingLightNode");
        mLightNode->attachObject(mLight);
        // create billboard set
        ParticleCtrl * bbs = mSceneMgr->createParticleCtrl("lightbbs", 1);
        bbs->setMaterialName("Examples/Flare");
        SyncParticle * bb = bbs->create(Vector3f(0, 0, 0), mMinLightColour);
        // attach
        mLightNode->attachObject(bbs);

        // create controller, after this is will get updated on its own
        DataFunc<NIIf, NIIf> * func = 
            N_new WaveformControllerFunction(Ogre::WFT_SINE, 0.75, 0.5);
        EquationManager & contMgr = EquationManager::getSingleton();
        DataValue<NIIf> * val = N_new LightWibbler(mLight, bb, 
            mMinLightColour, mMaxLightColour, mMinFlareSize, mMaxFlareSize));
        mController = contMgr.createController(contMgr.getFrameTimeSource(), val, func);

        //mLight->setPos(Vector3(300,250,-300));
        mLightNode->setPos(Vector3(300,1750,-700));

        // Create a track for the light
        Animation * anim = mSceneMgr->createAnimation("LightTrack", 20);
        // Spline it for nice curves
        anim->setInterpolationMode(NodeFusion::M_SPLINE);
        // Create a track to animate the camera's node
        NodeKeyFrameTrack * track = N_new NodeKeyFrameTrack(anim, 0);
        track->setAssociatedNode(mLightNode);
        // Setup keyframes
        NodeKeyFrame * key = static_cast<NodeKeyFrame *>(track->create(0)); // A startposition
        key->setTranslate(Vector3(300, 750, -700));
        key = static_cast<NodeKeyFrame *>(track->create(2));//B
        key->setTranslate(Vector3(150, 800, -250));
        key = static_cast<NodeKeyFrame *>(track->create(4));//C
        key->setTranslate(Vector3(-150, 850, -100));
        key = static_cast<NodeKeyFrame *>(track->create(6));//D
        key->setTranslate(Vector3(-400, 700, -200));
        key = static_cast<NodeKeyFrame *>(track->create(8));//E
        key->setTranslate(Vector3(-200, 700,-400));
        key = static_cast<NodeKeyFrame *>(track->create(10));//F
        key->setTranslate(Vector3(-100, 850,-200));
        key = static_cast<NodeKeyFrame *>(track->create(12));//G
        key->setTranslate(Vector3(-100, 575,180));
        key = static_cast<NodeKeyFrame *>(track->create(14));//H
        key->setTranslate(Vector3(0, 750, 300));
        key = static_cast<NodeKeyFrame *>(track->create(16));//I
        key->setTranslate(Vector3(100, 850, 100));
        key = static_cast<NodeKeyFrame *>(track->create(18));//J
        key->setTranslate(Vector3(250, 800, 0));
        key = static_cast<NodeKeyFrame *>(track->create(20));//K == A
        key->setTranslate(Vector3(300, 750, -700));
        track->buildSpline();
        // Create a new animation state to track this
        mAnimState = mSceneMgr->createFusion("LightTrack");
        mAnimState->setEnabled(true);
        // Make light node look at origin, this is for when we
        // change the moving light to a spotlight
        mLightNode->setAutoFocus(true, Vector3f::ZERO, mSceneMgr->getOriginNode());
        
        unsigned short src, dest;
        // Prepare athene mesh for normalmapping
        MeshPtr pAthene = N_Only(Mesh).load("athene.mesh", GroupDefault);
        if(!pAthene->checkTangentBuffer(false))
        {
            pAthene->buildTangentBuffer(VT_Tangent);
        }

        SpaceNode* node;
        node = mSceneMgr->getOriginNode()->create();
        mAthene = mSceneMgr->createGeo( "athene", "athene.mesh" );
        mAthene->setMaterialName(BASIC_ATHENE_MATERIAL);
        node->attachObject( mAthene );
        node->translate(Vector3f(0, -27, 0));
        node->yaw(Angle(90));

        GeometrySpace * pEnt;
		// Columns
		for (int x = -2; x <= 2; ++x)
		{
			for (int z = -2; z <= 2; ++z)
			{
				if (x != 0 || z != 0)
				{
					StringUtil::StrStreamType str;
					str << "col" << x << "_" << z;
					node = mSceneMgr->getOriginNode()->create();
					pEnt = mSceneMgr->createGeo( str.str(), "column.mesh" );
					pEnt->setMaterialName(BASIC_ROCKWALL_MATERIAL);
					pColumns.push_back(pEnt);
					node->attachObject( pEnt );
					node->translate(Vector3f(x*300,0, z*300));

				}
			}

		}
        mBoxSky = N_new BoxSky(mSceneMgr);
        mBoxSky->setMaterial("Examples/StormySkyBox");
        mBoxSky->setRenderGroup(RQG_Sky);
        // Skybox
        mBoxSky->buildGeo(5000);
        mSceneMgr->addSky(mBoxSky);
        // Floor plane (use POSM plane def)
		mPlane = new MovablePlane("*mPlane");
        mPlane->normal = Vector3::Y;
        mPlane->d = 107;
        MeshManager::getSingleton().createPlane("Myplane",
            GroupDefault, *mPlane, 1500, 1500, 50, 50, true, 1, 5, 5, Vector3::Z);
        pPlaneEnt = mSceneMgr->createGeo( "plane", "Myplane" );
        pPlaneEnt->setMaterialName(BASIC_ROCKWALL_MATERIAL);
        pPlaneEnt->setCastShadow(false);
        static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create())->attachObject(pPlaneEnt);

		if (mRoot->getRender()->getFeature()->hasCapability(GF_Render_To_Texture))
        {
            // In D3D, use a 1024x1024 shadow texture
            mSceneMgr->setTextureConfig(1024, 1024, 2);
        }
        else
        {
            // Use 512x512 texture in GL since we can't go higher than the window res
            mSceneMgr->setTextureConfig(512, 512, 2);
        }

        mSceneMgr->setShadowColour(Colour(0.5, 0.5, 0.5));
        //mSceneMgr->getRenderPattern()->setDebugShadow(true);

		setupGUI();
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
		setDragLook(true);
#endif
    }

	virtual void setupView()
	{
		SdkSample::setupView();

        // incase infinite far distance is not supported
        mCamera->setFarClipDistance(100000);

		mCamera->setPos(NII::Vector3f(250, 20, 400));
		mCamera->lookAt(NII::Vector3f(0, 10, 0));
	}

	virtual void cleanupContent()
	{
		EquationManager::getSingleton().destroyController(mController);

        MeshManager::getSingleton().remove("Myplane");
	}

	/// Change basic shadow technique
	void changeShadowTechnique(ShadowTechnique newTech)
	{
		mSceneMgr->setShadowTechnique(newTech);

		// Below is for projection
		//configureShadowCameras(mCurrentShadowTechnique, newTech);

		configureLights(newTech);

		// Advanced modes - materials / compositors
		//configureCompositors(mCurrentShadowTechnique, newTech);
		//configureTextures(mCurrentShadowTechnique, newTech);
		//configureShadowCasterReceiverMaterials(mCurrentShadowTechnique, newTech);

		updateGUI(newTech);

		mCurrentShadowTechnique = newTech;
	}

	void configureLights(ShadowTechnique newTech)
	{
		Vector3 dir;
		switch (newTech)
		{
		case SHADOWTYPE_STENCIL_ADDITIVE:
			// Fixed light, dim
			mSunLight->setCastShadow(true);

			// Point light, movable, reddish
			mLight->setType(LT_POINT);
			mLight->setCastShadow(true);
			mLight->setDiffuse(mMinLightColour);
			mLight->setSpecular(1, 1, 1);
            mLight->setRange(8000);
            mLight->setAttenuationConstant(1);
            mLight->setAttenuationLinear(0.0005);
            mLight->setAttenuationQuadric(0);

			break;
		case SHADOWTYPE_STENCIL_MODULATIVE:
			// Multiple lights cause obvious silhouette edges in modulative mode
			// So turn off shadows on the direct light
			// Fixed light, dim
			mSunLight->setCastShadow(false);

			// Point light, movable, reddish
			mLight->setType(LT_POINT);
			mLight->setCastShadow(true);
			mLight->setDiffuse(mMinLightColour);
			mLight->setSpecular(1, 1, 1);
            mLight->setRange(8000);
            mLight->setAttenuationConstant(1);
            mLight->setAttenuationLinear(0.0005);
            mLight->setAttenuationQuadric(0);
			break;
		case SHADOWTYPE_TEXTURE_MODULATIVE:
		case SHADOWTYPE_TEXTURE_ADDITIVE:
			// Fixed light, dim
			mSunLight->setCastShadow(true);
			// Change moving light to spotlight
			// Point light, movable, reddish
			mLight->setType(LT_SPOT);
			mLight->setDirection(Vector3::INV_Z);
			mLight->setCastShadow(true);
			mLight->setDiffuse(mMinLightColour);
			mLight->setSpecular(1, 1, 1);
            mLight->setRange(8000);
            mLight->setAttenuationConstant(1);
            mLight->setAttenuationLinear(0.0005);
            mLight->setAttenuationQuadric(0);
            mLight->setSpotlightInner(Angle(80));
            mLight->setSpotlightOuter(Angle(90));
            mLight->setSpotlightFalloff(1.0);
			break;
		default:
			break;
		};
	}

	SelectMenu* mTechniqueMenu;
	SelectMenu* mLightingMenu;
	SelectMenu* mProjectionMenu;
	SelectMenu* mMaterialMenu;

	Slider* mFixedBiasSlider;
	Slider* mSlopedBiasSlider;
	Slider* mClampSlider;

	void setupGUI()
	{
		mTechniqueMenu = mTrayMgr->createLongSelectMenu(
			TL_TOPLEFT, "TechniqueSelectMenu", "Technique", 300, 200, 5);
		mTechniqueMenu->addItem("Stencil");
		mTechniqueMenu->addItem("Texture");
        if(mCurrentShadowTechnique & SHADOWDETAILTYPE_STENCIL)
            mTechniqueMenu->selectItem("Stencil", false);
        else
            mTechniqueMenu->selectItem("Texture", false);

		mLightingMenu = mTrayMgr->createLongSelectMenu(
			TL_TOPLEFT, "LightingSelectMenu", "Lighting", 300, 200, 5);
		mLightingMenu->addItem("Additive");
		mLightingMenu->addItem("Modulative");
        if(mCurrentShadowTechnique & SHADOWTYPE_STENCIL_ADDITIVE)
            mLightingMenu->selectItem("Additive", false);
        else
            mLightingMenu->selectItem("Modulative", false);

		//These values are synchronized with ShadowProjection enum
		mProjectionMenu = mTrayMgr->createLongSelectMenu(
			TL_TOPLEFT, "ProjectionSelectMenu", "Projection", 300, 200, 5);
		mProjectionMenu->addItem("Uniform");
		mProjectionMenu->addItem("Uniform Focused");
		mProjectionMenu->addItem("LiSPSM");
		mProjectionMenu->addItem("Plane Optimal");

		mMaterialMenu = mTrayMgr->createLongSelectMenu(
			TL_TOPLEFT, "MaterialSelectMenu", "Material", 300, 200, 5);
		mMaterialMenu->addItem("Standard");
		mMaterialMenu->addItem("Depth Shadowmap");
		mMaterialMenu->addItem("Depth Shadowmap (PCF)");

		mFixedBiasSlider = mTrayMgr->createThickSlider(TL_NONE, "FixedBiasSlider", "Fixed Bias", 256, 80, 0, 0.02, 100);
		mFixedBiasSlider->setValue(0.0009, false);
		mFixedBiasSlider->hide();

		mSlopedBiasSlider = mTrayMgr->createThickSlider(TL_NONE, "SlopedBiasSlider", "Sloped Bias", 256, 80, 0, 0.2, 100);
		mSlopedBiasSlider->setValue(0.0008, false);
		mSlopedBiasSlider->hide();

		mClampSlider = mTrayMgr->createThickSlider(TL_NONE, "SlopeClampSlider", "Slope Clamp", 256, 80, 0, 2, 100);
		mClampSlider->setValue(0.2, false);
		mClampSlider->hide();

		updateGUI(mCurrentShadowTechnique);
		mTrayMgr->showCursor();
	}

	void updateGUI(ShadowTechnique newTech)
	{
		bool isTextureBased = (newTech & SHADOWDETAILTYPE_TEXTURE) != 0;

		if (isTextureBased)
		{
			mProjectionMenu->show();
			mTrayMgr->moveWidgetToTray(mProjectionMenu, TL_TOPLEFT);
			mMaterialMenu->show();
			mTrayMgr->moveWidgetToTray(mMaterialMenu, TL_TOPLEFT);
		}
		else
		{
			mProjectionMenu->hide();
			mTrayMgr->removeWidgetFromTray(mProjectionMenu);
			mMaterialMenu->hide();
			mTrayMgr->removeWidgetFromTray(mMaterialMenu);
		}
	}

	void itemSelected(SelectMenu* menu)
	{
		if (menu == mTechniqueMenu) handleShadowTypeChanged();
		else if (menu == mLightingMenu) handleShadowTypeChanged();
		else if (menu == mProjectionMenu) handleProjectionChanged();
		else if (menu == mMaterialMenu) handleMaterialChanged();
	}

	void handleShadowTypeChanged()
	{
		bool isStencil = mTechniqueMenu->getSelectionIndex() == 0;
		bool isAdditive = mLightingMenu->getSelectionIndex() == 0;
		ShadowTechnique newTech = mCurrentShadowTechnique;

		if (isStencil)
		{
			newTech = static_cast<ShadowTechnique>(
				(newTech & ~SHADOWDETAILTYPE_TEXTURE) | SHADOWDETAILTYPE_STENCIL);
			resetMaterials();
		}
		else
		{
			newTech = static_cast<ShadowTechnique>(
				(newTech & ~SHADOWDETAILTYPE_STENCIL) | SHADOWDETAILTYPE_TEXTURE);
		}

		if (isAdditive)
		{
			newTech = static_cast<ShadowTechnique>(
				(newTech & ~SHADOWDETAILTYPE_MODULATIVE) | SHADOWDETAILTYPE_ADDITIVE);
		}
		else
		{
			newTech = static_cast<ShadowTechnique>(
				(newTech & ~SHADOWDETAILTYPE_ADDITIVE) | SHADOWDETAILTYPE_MODULATIVE);
		}

		changeShadowTechnique(newTech);
	}

	void handleProjectionChanged()
	{
		ShadowProjection proj = (ShadowProjection)mProjectionMenu->getSelectionIndex();

		if (proj != mCurrentProjection)
		{
			switch(proj)
			{
			case UNIFORM:
				mCurrentShadowGenCamera = N_new CameraSetup();
				break;
			case UNIFORM_FOCUSED:
				mCurrentShadowGenCamera = N_new FocusedShadowCamera();
				break;
			case LISPSM:
				{
					mLiSPSMSetup = N_new LiSPSMShadowSetup();
					//mLiSPSMSetup->setUseAggressiveFocusRegion(false);
					mCurrentShadowGenCamera = mLiSPSMSetup;
				}
				break;
			case PLANE_OPTIMAL:
				mCurrentShadowGenCamera = N_new PlaneOptimalShadowGenCamera(mPlane);
				break;

			};
			mCurrentProjection = proj;

			mSceneMgr->setShadowGenCamera(mCurrentShadowGenCamera);

			//updateTipForCombo(cbo);
			if (!mCustomRockwallVparams.isNull() && !mCustomRockwallFparams.isNull())
			{
				// set
				setDefaultDepthShadowParams();
			}
		}
	}

	void updateDepthShadowParams()
	{
		mCustomRockwallFparams->set("fixedDepthBias", mFixedBiasSlider->getValue());
		mCustomRockwallFparams->set("gradientScaleBias", mSlopedBiasSlider->getValue());
		mCustomRockwallFparams->set("gradientClamp", mClampSlider->getValue());

		mCustomAtheneFparams->set("fixedDepthBias", mFixedBiasSlider->getValue());
		mCustomAtheneFparams->set("gradientScaleBias", mSlopedBiasSlider->getValue());
		mCustomAtheneFparams->set("gradientClamp", mClampSlider->getValue());
	}

	void setDefaultDepthShadowParams()
	{
		switch(mCurrentProjection)
		{
		case UNIFORM:
		case UNIFORM_FOCUSED:
		case PLANE_OPTIMAL:
			mFixedBiasSlider->setValue(0.0f, false);
			mSlopedBiasSlider->setValue(0.0f, false);
			break;
		case LISPSM:
			mFixedBiasSlider->setValue(0.009f, false);
			mSlopedBiasSlider->setValue(0.04f, false);
			break;
		};

		updateDepthShadowParams();

	}

	void sliderMoved(Slider* slider)
	{
		if (!mCustomRockwallVparams.isNull() && !mCustomRockwallFparams.isNull())
		{
			updateDepthShadowParams();
		}
	}

	void rebindDebugShadowOverlays()
	{
		/*MaterialPtr debugMat =
			MaterialManager::getSingleton().getByName("Ogre/DebugShadowMap0");
		TexturePtr shadowTex = mSceneMgr->getRenderPattern()->getTexture(0);
		debugMat->get(0)->get(0)->getTexture().get(0)->setSrc(shadowTex->getName(), Texture::T_2D);

		debugMat =
			MaterialManager::getSingleton().getByName("Ogre/DebugShadowMap1");
		shadowTex = mSceneMgr->getRenderPattern()->getTexture(1);
		debugMat->get(0)->get(0)->getTexture().get(0)->setSrc(shadowTex->getName(), Texture::T_2D);*/
	}

	void resetMaterials()
	{
		// Sort out base materials
		pPlaneEnt->setMaterialName(BASIC_ROCKWALL_MATERIAL);
		mAthene->setMaterialName(BASIC_ATHENE_MATERIAL);
		for (vector<GeometrySpace*>::type::iterator i = pColumns.begin();
			i != pColumns.end(); ++i)
		{
			(*i)->setMaterialName(BASIC_ROCKWALL_MATERIAL);
		}

		mCustomRockwallVparams.setNull();
		mCustomRockwallFparams.setNull();
		mCustomAtheneVparams.setNull();
		mCustomAtheneFparams.setNull();

	}

	void handleMaterialChanged()
	{
		bool showSliders = false;
		ShadowMaterial mat = (ShadowMaterial)mMaterialMenu->getSelectionIndex();
		MaterialPtr themat;
		if (mat != mCurrentMaterial)
		{
			switch(mat)
			{
			case MAT_STANDARD:
				mSceneMgr->getRenderPattern()->setTexturePixelFormat(PF_X8R8G8B8);
				mSceneMgr->getRenderPattern()->setTextureCasterMaterial(StringUtil::BLANK);
				mSceneMgr->getRenderPattern()->setTextureReceiverMaterial(StringUtil::BLANK);
				mSceneMgr->setSelfShadow(false);

				resetMaterials();

				break;
			case MAT_DEPTH_FLOAT:
				if (mIsOpenGL)
				{
					// GL performs much better if you pick half-float format
					mSceneMgr->getRenderPattern()->setTexturePixelFormat(PF_R16);
				}
				else
				{
					// D3D is the opposite - if you ask for PF_R16 you
					// get an integer format instead! You can ask for PF_G16R16
					// but the precision doesn't work well
					mSceneMgr->getRenderPattern()->setTexturePixelFormat(PF_R32);
				}
				mSceneMgr->getRenderPattern()->setTextureCasterMaterial(CUSTOM_CASTER_MATERIAL);
				mSceneMgr->getRenderPattern()->setTextureReceiverMaterial(CUSTOM_RECEIVER_MATERIAL);
				mSceneMgr->setSelfShadow(true);
				// Sort out base materials
				pPlaneEnt->setMaterialName(CUSTOM_ROCKWALL_MATERIAL);
				mAthene->setMaterialName(CUSTOM_ATHENE_MATERIAL);
				for(vector<GeometrySpace *>::type::iterator i = pColumns.begin(); i != pColumns.end(); ++i)
				{
					(*i)->setMaterialName(CUSTOM_ROCKWALL_MATERIAL);
				}

				themat = MaterialManager::getSingleton().getByName(CUSTOM_ROCKWALL_MATERIAL);
				themat->get(0)->get(1)->getProgram().get(0, ST_VS_SR, NULL, mCustomRockwallVparams);
				themat->get(0)->get(1)->getProgram().get(0, ST_FS_SR, NULL, mCustomRockwallFparams);
				themat = MaterialManager::getSingleton().getByName(CUSTOM_ATHENE_MATERIAL);
				themat->get(0)->get(1)->getProgram().get(0, ST_VS_SR, NULL, mCustomAtheneVparams);
				themat->get(0)->get(1)->getProgram().get(0, ST_FS_SR, NULL, mCustomAtheneFparams);
				showSliders = true;

				// set the current params
				setDefaultDepthShadowParams();
				break;
			case MAT_DEPTH_FLOAT_PCF:
				if (mIsOpenGL)
				{
					// GL performs much better if you pick half-float format
					mSceneMgr->getRenderPattern()->setTexturePixelFormat(PF_R16);
				}
				else
				{
					// D3D is the opposite - if you ask for PF_R16 you
					// get an integer format instead! You can ask for PF_G16R16
					// but the precision doesn't work well
					mSceneMgr->getRenderPattern()->setTexturePixelFormat(PF_R32);
				}
				mSceneMgr->getRenderPattern()->setTextureCasterMaterial(CUSTOM_CASTER_MATERIAL);
				mSceneMgr->getRenderPattern()->setTextureReceiverMaterial(CUSTOM_RECEIVER_MATERIAL + "/PCF");
				mSceneMgr->setSelfShadow(true);
				// Sort out base materials
				pPlaneEnt->setMaterialName(CUSTOM_ROCKWALL_MATERIAL + "/PCF");
				mAthene->setMaterialName(CUSTOM_ATHENE_MATERIAL + "/PCF");
				for (vector<GeometrySpace*>::type::iterator i = pColumns.begin();
					i != pColumns.end(); ++i)
				{
					(*i)->setMaterialName(CUSTOM_ROCKWALL_MATERIAL + "/PCF");
				}

				themat = MaterialManager::getSingleton().getByName(CUSTOM_ROCKWALL_MATERIAL + "/PCF");
				themat->get(0)->get(1)->getProgram().get(0, ST_VS_SR, NULL, mCustomRockwallVparams);
				themat->get(0)->get(1)->getProgram().get(0, ST_FS_SR, NULL, mCustomRockwallFparams);
				themat = MaterialManager::getSingleton().getByName(CUSTOM_ATHENE_MATERIAL + "/PCF");
				themat->get(0)->get(1)->getProgram().get(0, ST_VS_SR, NULL, mCustomAtheneVparams);
				themat->get(0)->get(1)->getProgram().get(0, ST_FS_SR, NULL, mCustomAtheneFparams);
				showSliders = true;

				// set the current params
				setDefaultDepthShadowParams();
				break;
			};
			mCurrentMaterial = mat;

			if (showSliders)
			{
				mFixedBiasSlider->show();
				mTrayMgr->moveWidgetToTray(mFixedBiasSlider, TL_TOPRIGHT);
				mSlopedBiasSlider->show();
				mTrayMgr->moveWidgetToTray(mSlopedBiasSlider, TL_TOPRIGHT);
				mClampSlider->show();
				mTrayMgr->moveWidgetToTray(mClampSlider, TL_TOPRIGHT);
			}
			else
			{
				mFixedBiasSlider->hide();
				mTrayMgr->removeWidgetFromTray(mFixedBiasSlider);
				mSlopedBiasSlider->hide();
				mTrayMgr->removeWidgetFromTray(mSlopedBiasSlider);
				mClampSlider->hide();
				mTrayMgr->removeWidgetFromTray(mClampSlider);
			}
			//updateTipForCombo(cbo);
			//rebindDebugShadowOverlays();
		}
	}
};