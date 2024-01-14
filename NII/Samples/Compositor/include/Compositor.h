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

#ifndef _CompositorDemo_H_
#define _CompositorDemo_H_

#include "OgreConfigFile.h"
#include "OgreStringConverter.h"
#include "OgreException.h"

#include "SdkSample.h"
#include "SamplePlugin.h"

using namespace Ogre;
using namespace OgreBites;

#define COMPOSITORS_PER_PAGE 8

class _OgreSampleClassExport Sample_Compositor : public SdkSample
{
public:
	Sample_Compositor();

    void setupContent(void);
    void cleanupContent(void);

	bool onRenderCore(const RenderFrameArgs * evt);

	void checkBoxToggled(OgreBites::CheckBox * box);
	void buttonHit(OgreBites::Button* button);
	void itemSelected(OgreBites::SelectMenu* menu);

protected:

	void setupView(void);
	void setupControls(void);
    void setupScene(void);
    void createEffects(void);
	void createTextures(void);

	void registerCompositors();
	void changePage(size_t pageNum);

	SpaceNode * mSpinny;
	StringList mCompositorNames;
	size_t mActiveCompositorPage;
	size_t mNumCompositorPages;

	//Used to unregister compositor logics and free memory
	typedef map<String, CompositorLogic*>::type CompositorLogicMap;
	CompositorLogicMap mCompositorLogics;

	String mDebugCompositorName;
	SelectMenu* mDebugTextureSelectMenu;
	ShaderChTextureUnit* mDebugTextureTUS;
    BoxSky * mBoxSky;
};

/**
    \file
        Compositor.cpp
    \brief
        Shows OGRE's Compositor feature
	\author
		W.J. :wumpus: van der Laan
			Ogre compositor framework
		Manuel Bua
			Postfilter ideas and original out-of-core implementation
        Jeff (nfz) Doyle
            added gui framework to demo
*/

#include <Ogre.h>

#include "HelperLogics.h"

/*************************************************************************
	                    Sample_Compositor Methods
*************************************************************************/
Sample_Compositor::Sample_Compositor()
{
	mInfo["Title"] = "Compositor";
	mInfo["Description"] = "A demo of Ogre's post-processing framework.";
	mInfo["Thumbnail"] = "thumb_comp.png";
	mInfo["Category"] = "Effects";
    mBoxSky = 0;
}
Sample_Compositor::~Sample_Compositor()
{
    if(mBoxSky)
    {
        N_delete mBoxSky;
        mSceneMgr->removeSky(mBoxSky);
        mBoxSky = 0;
    }
}
//--------------------------------------------------------------------------
void Sample_Compositor::setupView()
{
	SdkSample::setupView();
    mCamera->setPos(NII::Vector3f(0, 0, 0));
    mCamera->lookAt(NII::Vector3f(0, 0, -300));
    mCamera->setNearClipDistance(1);
}
//-----------------------------------------------------------------------------------
void Sample_Compositor::setupContent(void)
{
	// Register the compositor logics
	// See comment in beginning of HelperLogics.h for explanation
	Ogre::CompositorManager& compMgr = Ogre::CompositorManager::getSingleton();
	mCompositorLogics["GaussianBlur"]	= new GaussianBlurLogic;
	mCompositorLogics["HDR"]			= new HDRLogic;
	mCompositorLogics["HeatVision"]		= new HeatVisionLogic;
	compMgr.registerCompositorLogic("GaussianBlur", mCompositorLogics["GaussianBlur"]);
	compMgr.registerCompositorLogic("HDR", mCompositorLogics["HDR"]);
	compMgr.registerCompositorLogic("HeatVision", mCompositorLogics["HeatVision"]);

	createTextures();
    /// Create a couple of hard coded postfilter effects as an example of how to do it
	/// but the preferred method is to use compositor scripts.
	createEffects();

	setupScene();

	registerCompositors();

	setupControls();

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
	setDragLook(true);
#endif
}
//-----------------------------------------------------------------------------------
void Sample_Compositor::registerCompositors(void)
{
	Ogre::Viewport *vp = mViewport;

    //iterate through Compositor Managers resources and add name keys to menu
    NII::ResourceManager::PrcIDMap::const_iterator i, iend = NII::N_Only(Compositor).getPrcResList().end();
    // add all compositor resources to the view container
    for(i = NII::N_Only(Compositor).getPrcResList().begin(); i != iend; ++i)
    {
        Ogre::Resource * resource = i->second;
        const Ogre::String & compositorName = resource->getSrc();
        // Don't add base Ogre/Scene compositor to view
        if (Ogre::StringUtil::beginsWith(compositorName, "Ogre/Scene/", false))
            continue;
		// Don't add the deferred shading compositors, thats a different demo.
		if (Ogre::StringUtil::beginsWith(compositorName, "DeferredShading", false))
			continue;
		// Don't add the SSAO compositors, thats a different demo.
		if (Ogre::StringUtil::beginsWith(compositorName, "SSAO", false))
			continue;
		// Don't add the TestMRT compositor, it needs extra scene setup so doesn't currently work.
		if (Ogre::StringUtil::beginsWith(compositorName, "TestMRT", false))
			continue;

		mCompositorNames.push_back(compositorName);
		int addPosition = -1;
		if (compositorName == "HDR")
		{
			// HDR must be first in the chain
			addPosition = 0;
		}
		try
		{
			Ogre::CompositorManager::getSingleton().addCompositor(vp, compositorName, addPosition);
			Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, compositorName, false);
		} catch (...) {
			/// Warn user
			LogManager::getSingleton().logMessage("Could not load compositor " + compositorName);
		}
    }

	mNumCompositorPages = (mCompositorNames.size() / COMPOSITORS_PER_PAGE) +
		((mCompositorNames.size() % COMPOSITORS_PER_PAGE == 0) ? 0 : 1);
}
//-----------------------------------------------------------------------------------
void Sample_Compositor::changePage(size_t pageNum)
{
	assert(pageNum < mNumCompositorPages);

	mActiveCompositorPage = pageNum;
	size_t maxCompositorsInPage = mCompositorNames.size() - (pageNum * COMPOSITORS_PER_PAGE);
	for (size_t i=0; i < COMPOSITORS_PER_PAGE; i++)
	{
		String checkBoxName = "Compositor_" + Ogre::StrConv::conv(i);
		CheckBox* cb = static_cast<CheckBox*>(mTrayMgr->getWidget(TL_TOPLEFT, checkBoxName));
		if (i < maxCompositorsInPage)
		{
			String compositorName = mCompositorNames[pageNum * COMPOSITORS_PER_PAGE + i];
			CompositorInstance *tmpCompo = CompositorManager::getSingleton().getCompositorChain(mViewport)
				->getCompositor(compositorName);

			cb->setCaption(compositorName);

			if( tmpCompo )
			{
				cb->setChecked( tmpCompo->getEnabled(), false );
				cb->show();
			}
			else
			{
				cb->setChecked( false, false );
				cb->hide();
			}

		}
		else
		{
			cb->hide();
		}
	}

	OgreBites::Button* pageButton = static_cast<OgreBites::Button*>(mTrayMgr->getWidget(TL_TOPLEFT, "PageButton"));
	Ogre::StringStream ss;
	ss << "Compositors " << pageNum + 1 << "/" << mNumCompositorPages;
	pageButton->setCaption(ss.str());
}
//-----------------------------------------------------------------------------------
void Sample_Compositor::cleanupContent(void)
{
	mDebugTextureTUS->setContentType(ShaderChTexture::T_NORMAL);
	CompositorManager::getSingleton().removeCompositorChain(mViewport);
	mCompositorNames.clear();

    TextureManager::getSingleton().remove("DitherTex");
    TextureManager::getSingleton().remove("HalftoneVolume");

	Ogre::CompositorManager& compMgr = Ogre::CompositorManager::getSingleton();
	CompositorLogicMap::const_iterator itor = mCompositorLogics.begin();
	CompositorLogicMap::const_iterator end  = mCompositorLogics.end();
	while( itor != end )
	{
		compMgr.unregisterCompositorLogic( itor->first );
		delete itor->second;
		++itor;
	}
	mCompositorLogics.clear();
    MeshManager::getSingleton().remove("Myplane");
}
//-----------------------------------------------------------------------------------
void Sample_Compositor::setupControls(void)
{
	mTrayMgr->createButton(TL_TOPLEFT, "PageButton", "Compositors", 175);

	for(size_t i = 0; i < COMPOSITORS_PER_PAGE; i++)
	{
		String checkBoxName = "Compositor_" + Ogre::StrConv::conv(i);
		CheckBox * cb = mTrayMgr->createCheckBox(TL_TOPLEFT, checkBoxName, "Compositor", 175);
		cb->hide();
	}

	changePage(0);

	mDebugTextureSelectMenu = mTrayMgr->createThickSelectMenu(TL_TOPRIGHT, "DebugRTTSelectMenu", "Debug RTT", 180, 5);
	mDebugTextureSelectMenu->addItem("None");

	mTrayMgr->createSeparator(TL_TOPRIGHT, "DebugRTTSep1");  // this is a hack to give the debug RTT a bit more room

	DecorWidget * debugRTTPanel = mTrayMgr->createDecorWidget(TL_NONE, "DebugRTTPanel", "SdkTrays/Picture");
	OverlayView * debugRTTContainer = (OverlayView *)debugRTTPanel->getOverlayGeo();
	mDebugTextureTUS = debugRTTContainer->getMaterial()->getLOD()->get(0)->getTexture().get(0);
	//mDebugTextureTUS->setSrc("CompositorDemo/DebugView", Texture::T_2D);
	debugRTTContainer->setSize(128, 128);
	debugRTTContainer->get("DebugRTTPanel/PictureFrame")->setSize(144, 144);
	debugRTTPanel->hide();

	mTrayMgr->createSeparator(TL_TOPRIGHT, "DebugRTTSep2");  // this is a hack to give the debug RTT a bit more room

	mTrayMgr->showCursor();
	mTrayMgr->showLogo(TL_BOTTOMLEFT);
	mTrayMgr->toggleAdvancedFrameStats();
}
//-----------------------------------------------------------------------------------
void Sample_Compositor::checkBoxToggled(OgreBites::CheckBox * box)
{
	if(Ogre::StringUtil::beginsWith(box->getName(), "Compositor_", false))
	{
		String compositorName = box->getCaption();

		String activeTex = mDebugTextureSelectMenu->getSelectItem();

		if (!box->isChecked())
		{
			//Remove the items from the debug menu and remove debug texture if from disabled compositor
			bool debuggingRemovedTex = StringUtil::beginsWith(activeTex, compositorName, false);
			if (debuggingRemovedTex)
			{
				mDebugTextureTUS->setContentType(ShaderChTexture::T_NORMAL);
				mDebugTextureSelectMenu->selectItem(0, true);
			}
			for (unsigned int i = 1; i < mDebugTextureSelectMenu->getNumItems(); i++)
			{
				if (StringUtil::beginsWith(mDebugTextureSelectMenu->getItems()[i], compositorName, false))
				{
					mDebugTextureSelectMenu->removeItem(i);
					i--;
				}
			}
			if (!debuggingRemovedTex)
			{
				//Selection clears itself when removing items. Restore.
				mDebugTextureSelectMenu->selectItem(activeTex, false);
			}
		}

		CompositorManager::getSingleton().setCompositorEnabled(mViewport, compositorName, box->isChecked());


		if (box->isChecked())
		{
			//Add the items to the selectable texture menu
			CompositorInstance* instance = CompositorManager::getSingleton().getCompositorChain(mViewport)->getCompositor(compositorName);
			if (instance)
			{
				CompositionTechnique::TextureDefinitionIterator it = instance->getShaderFusion()->getTextureDefinitionIterator();
				while (it.isNextValid())
				{
					CompositionTechnique::FrameDefine* texDef = it.getAndNext();
					size_t numTextures = texDef->formatList.size();
					if (numTextures > 1)
					{
						for (size_t i=0; i<numTextures; i++)
						{
							//Dirty string composition. NOT ROBUST!
							mDebugTextureSelectMenu->addItem(compositorName + ";" + texDef->name + ";" +
								Ogre::StrConv::conv((Ogre::uint32)i));
						}
					}
					else
					{
						mDebugTextureSelectMenu->addItem(compositorName + ";" + texDef->name);
					}
				}
				mDebugTextureSelectMenu->selectItem(activeTex, false);
			}
		}
	}
}
//-----------------------------------------------------------------------------------
void Sample_Compositor::buttonHit(OgreBites::Button* button)
{
	size_t nextPage = (mActiveCompositorPage + 1) % mNumCompositorPages;
	changePage(nextPage);
}
//-----------------------------------------------------------------------------------
void Sample_Compositor::itemSelected(OgreBites::SelectMenu* menu)
{
	if(menu->getSelectionIndex() == 0)
	{
		mDebugTextureTUS->setContentType(ShaderChTexture::T_NORMAL);
		mTrayMgr->getWidget("DebugRTTPanel")->hide();
		mTrayMgr->removeWidgetFromTray("DebugRTTPanel");
		return;
	}

	mTrayMgr->getWidget("DebugRTTPanel")->show();
	mTrayMgr->moveWidgetToTray("DebugRTTPanel", TL_TOPRIGHT, mTrayMgr->getNumWidgets(TL_TOPRIGHT) - 1);
	StringList parts = StringUtil::split(menu->getSelectItem(), ";");
	mDebugTextureTUS->setContentType(ShaderChTexture::T_FUSION);

	if (parts.size() == 2)
	{
		mDebugTextureTUS->setFusion(parts[0], parts[1]);
	}
	else
	{
		mDebugTextureTUS->setFusion(parts[0], parts[1]);
        mDebugTextureTUS->setMultiIndex(StringConverter::parseUnsignedInt(parts[2]));
	}
}
//-----------------------------------------------------------------------------------
void Sample_Compositor::setupScene(void)
{
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
	mSceneMgr->getRenderPattern()->setMaxExtent(1000);

	///Ogre::SpaceObj::setDefaultVisibilityFlags(0x00000001);

	// Set ambient light
	mSceneMgr->setAmbient(NII::Colour(0.3, 0.3, 0.2));

	Ogre::Light * l = mSceneMgr->createLight("Light2");
	Ogre::Vector3 dir(-1,-1,0);
	dir.normalise();
	l->setType(Ogre::LT_DIRECTIONAL);
	l->setDirection(dir);
	l->setDiffuse(1, 1, 0.8);
	l->setSpecular(1, 1, 1);

	Ogre::GeometrySpace * pEnt;

	// House
	pEnt = mSceneMgr->createGeo( "1", "tudorhouse.mesh" );
	Ogre::SpaceNode* n1 = mSceneMgr->getOriginNode()->create(Ogre::Vector3(350, 450, -200));
	n1->attachObject( pEnt );

	pEnt = mSceneMgr->createGeo( "2", "tudorhouse.mesh" );
	Ogre::SpaceNode* n2 = mSceneMgr->getOriginNode()->create(Ogre::Vector3(-350, 450, -200));
	n2->attachObject( pEnt );

	pEnt = mSceneMgr->createGeo( "3", "knot.mesh" );
	mSpinny = mSceneMgr->getOriginNode()->create(Ogre::Vector3(0, 0, 300));
	mSpinny->attachObject( pEnt );
	pEnt->setMaterialName("Examples/MorningCubeMap");

    mBoxSky = N_new BoxSky(mSceneMgr);
    mBoxSky->setMaterial("Examples/MorningSkyBox");
    mBoxSky->setRenderGroup(RQG_Sky);
	mBoxSky->buildGeo(5000);
    mSceneMgr->addSky(mBoxSky);

	Ogre::Plane plane;
	plane.normal = Ogre::Vector3::Y;
	plane.d = 100;
	Ogre::MeshManager::getSingleton().createPlane("Myplane",
		Ogre::ResourceSchemeManager::GroupDefault, plane,
            1500, 1500, 10, 10, true, 1, 5, 5, Ogre::Vector3::Z);
	Ogre::GeometrySpace* pPlaneEnt = mSceneMgr->createGeo( "plane", "Myplane" );
	pPlaneEnt->setMaterialName("Examples/Rockwall");
	pPlaneEnt->setCastShadow(false);
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create())->attachObject(pPlaneEnt);

	mCamera->setPos(NII::Vector3f(-400, 50, 900));
	mCamera->lookAt(NII::Vector3f(0,80,0));
}
//-----------------------------------------------------------------------------------
bool Sample_Compositor::onRenderCore(const RenderFrameArgs * evt)
{
	mSpinny->yaw(Ogre::Angle(10 * evt->mCurrent));
	return SdkSample::onRenderCore(evt);
}
//-----------------------------------------------------------------------------------
/// Create the hard coded postfilter effects
void Sample_Compositor::createEffects(void)
{
	    // Bloom compositor is loaded from script but here is the hard coded equivalent
//		CompositorPtr comp = CompositorManager::getSingleton().create(
//				"Bloom", GroupDefault);
//		{
//			CompositionTechnique *t = comp->createTechnique();
//			{
//				CompositionTechnique::FrameDefine *def = t->createTextureDefinition("rt0");
//				def->width = 128;
//				def->height = 128;
//				def->format = PF_A8R8G8B8;
//			}
//			{
//				CompositionTechnique::FrameDefine *def = t->createTextureDefinition("rt1");
//				def->width = 128;
//				def->height = 128;
//				def->format = PF_A8R8G8B8;
//			}
//			{
//				CompositionTargetPass * tp = t->createTargetPass();
//				tp->setInputMode(CompositionTargetPass::IM_PREVIOUS);
//				tp->setOutputName("rt1");
//			}
//			{
//				CompositionTargetPass * tp = t->createTargetPass();
//				tp->setInputMode(CompositionTargetPass::IM_NONE);
//				tp->setOutputName("rt0");
//				CompositionPass * pass = tp->create();
//				pass->setType(CompositionPass::PT_RENDERQUAD);
//				pass->setMaterialName("Ogre/Compositor/Blur0");
//				pass->setInput(0, "rt1");
//			}
//			{
//				CompositionTargetPass * tp = t->createTargetPass();
//				tp->setInputMode(CompositionTargetPass::IM_NONE);
//				tp->setOutputName("rt1");
//				CompositionPass * pass = tp->create();
//				pass->setType(CompositionPass::PT_RENDERQUAD);
//				pass->setMaterialName("Ogre/Compositor/Blur1");
//				pass->setInput(0, "rt0");
//			}
//			{
//				CompositionTargetPass *tp = t->getOutputTargetPass();
//				tp->setInputMode(CompositionTargetPass::IM_PREVIOUS);
//				{ CompositionPass *pass = tp->create();
//				pass->setType(CompositionPass::PT_RENDERQUAD);
//				pass->setMaterialName("Ogre/Compositor/BloomBlend");
//				pass->setInput(0, "rt1");
//				}
//			}
//		}
	    // Glass compositor is loaded from script but here is the hard coded equivalent
		/// Glass effect
//		CompositorPtr comp2 = CompositorManager::getSingleton().create(
//				"Glass", ResourceSchemeManager::GroupDefault);
//		{
//			CompositionTechnique *t = comp2->createTechnique();
//			{
//				CompositionTechnique::FrameDefine *def = t->createTextureDefinition("rt0");
//				def->width = 0;
//				def->height = 0;
//				def->format = PF_R8G8B8;
//			}
//			{
//				CompositionTargetPass *tp = t->createTargetPass();
//				tp->setInputMode(CompositionTargetPass::IM_PREVIOUS);
//				tp->setOutputName("rt0");
//			}
//			{
//				CompositionTargetPass *tp = t->getOutputTargetPass();
//				tp->setInputMode(CompositionTargetPass::IM_NONE);
//				{ CompositionPass * pass = tp->create();
//				pass->setType(CompositionPass::PT_RENDERQUAD);
//				pass->setMaterialName("Ogre/Compositor/GlassPass");
//				pass->setInput(0, "rt0");
//				}
//			}
//		}
		/// Motion blur effect
	Ogre::CompositorPtr comp3 = Ogre::CompositorManager::getSingleton().create(
			"Motion Blur", Ogre::ResourceSchemeManager::GroupDefault);
	{
		Ogre::CompositionTechnique *t = comp3->createTechnique();
		{
			Ogre::CompositionTechnique::FrameDefine *def = t->createTextureDefinition("scene");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::FrameDefine *def = t->createTextureDefinition("sum");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::FrameDefine *def = t->createTextureDefinition("temp");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		/// Render scene
		{
			Ogre::CompositionTargetPass * tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("scene");
		}
		/// Initialisation pass for sum texture
		{
			Ogre::CompositionTargetPass * tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("sum");
			tp->setOnlyInitial(true);
		}
		/// Do the motion blur
		{
			Ogre::CompositionTargetPass * tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("temp");
			{ Ogre::CompositionPass * pass = tp->create();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/Combine");
			pass->setInput(0, "scene");
			pass->setInput(1, "sum");
			}
		}
		/// Copy back sum texture
		{
			Ogre::CompositionTargetPass * tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("sum");
			{ Ogre::CompositionPass * pass = tp->create();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/Copyback");
			pass->setInput(0, "temp");
			}
		}
		/// Display result
		{
			Ogre::CompositionTargetPass * tp = t->getOutputTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			{ Ogre::CompositionPass * pass = tp->create();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/MotionBlur");
			pass->setInput(0, "sum");
			}
		}
	}
	/// Heat vision effect
	Ogre::CompositorPtr comp4 = Ogre::CompositorManager::getSingleton().create(
        "Heat Vision", Ogre::GroupDefault);
	{
		Ogre::CompositionTechnique *t = comp4->createTechnique();
		t->setCompositorLogicName("HeatVision");
		{
			Ogre::CompositionTechnique::FrameDefine *def = t->createTextureDefinition("scene");
			def->width = 256;
			def->height = 256;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::FrameDefine *def = t->createTextureDefinition("temp");
			def->width = 256;
			def->height = 256;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		/// Render scene
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("scene");
		}
		/// Light to heat pass
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("temp");
			{
				Ogre::CompositionPass * pass = tp->create();
				pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
				pass->setIdentifier(0xDEADBABE); /// Identify pass for use in listener
				pass->setMaterialName("Fury/HeatVision/LightToHeat");
				pass->setInput(0, "scene");
			}
		}
		/// Display result
		{
			Ogre::CompositionTargetPass * tp = t->getOutputTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			{
				Ogre::CompositionPass * pass = tp->create();
				pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
				pass->setMaterialName("Fury/HeatVision/Blur");
				pass->setInput(0, "temp");
			}
		}
	}
}
//--------------------------------------------------------------------------
void Sample_Compositor::createTextures(void)
{
	using namespace Ogre;

	Texture * tex = N_Only(Texture).createTexture("HalftoneVolume", "General",
        TEX_TYPE_3D, 64, 64, 64, 0, PF_A8ui);

	FrameBuffer * ptr = tex->getBuffer(0);
	ptr->lock(Buffer::MM_WHOLE);
	const PixelBlock & pb = ptr->getMutexData();
	Ogre::uint8 * data = static_cast<Ogre::uint8 *>(pb.data);

	size_t height = pb.getHeight();
	size_t width = pb.getWidth();
	size_t depth = pb.getDepth();
	size_t rowPitch = pb.rowPitch;
	size_t slicePitch = pb.slicePitch;

	for (size_t z = 0; z < depth; ++z)
	{
		for (size_t y = 0; y < height; ++y)
		{
			for(size_t x = 0; x < width; ++x)
			{
				float fx = 32-(float)x+0.5f;
				float fy = 32-(float)y+0.5f;
				float fz = 32-((float)z)/3+0.5f;
				float distanceSquare = fx*fx+fy*fy+fz*fz;
				data[slicePitch*z + rowPitch*y + x] =  0x00;
				if (distanceSquare < 1024.0f)
					data[slicePitch*z + rowPitch*y + x] +=  0xFF;
			}
		}
	}
	ptr->unlock();

	Ogre::Viewport * vp = mRoot->getView()->getViewport(0);

	Texture * tex2 = N_Only(Texture).createTexture( "DitherTex", "General",
		TEX_TYPE_2D, vp->getPixelWidth(), vp->getPixelHeight(), 1, 0, PF_A8ui);

	FrameBuffer * ptr2 = tex2->getBuffer(0);
	ptr2->lock(Buffer::MM_WHOLE);
	const PixelBlock &pb2 = ptr2->getMutexData();
	Ogre::uint8 *data2 = static_cast<Ogre::uint8*>(pb2.data);

	size_t height2 = pb2.getHeight();
	size_t width2 = pb2.getWidth();
	size_t rowPitch2 = pb2.rowPitch;

	for(size_t y = 0; y < height2; ++y)
	{
		for(size_t x = 0; x < width2; ++x)
		{
			data2[rowPitch2*y + x] = Ogre::Math::random(64.0,192);
		}
	}

	ptr2->unlock();
}

#endif	// end _CompositorDemo_H_
