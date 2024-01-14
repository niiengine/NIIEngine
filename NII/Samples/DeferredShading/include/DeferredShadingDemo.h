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
Implementation of a Deferred Shading engine in OGRE, using Multiple Render Targets and
CG high level language shaders.
	// W.J. :wumpus: van der Laan 2005 / Noam Gat 2009 //

Deferred shading renders the scene to a 'fat' texture format, using a shader that outputs colour,
normal, depth, and possible other attributes per fragment. Multi Render Target is required as we
are dealing with many outputs which get written into multiple render textures in the same pass.

After rendering the scene in this format, the shading (lighting) can be done as a post process.
This means that lighting is done in screen space, using light-representing geometry (sphere for
point light, cone for spot light and quad for directional) to render their contribution.

The wiki article explaining this demo can be found here :
  http://www.ogre3d.org/wiki/index.php/Deferred_Shading
*/

#ifndef H_DeferredShadingDemo
#define H_DeferredShadingDemo

#include "Ogre.h"
#include "SdkSample.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include "SharedData.h"
#include "OgreCompositorInstance.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreMaterial.h"
#include "OgreFrameObjListener.h"
#include "GeomUtils.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_DeferredShading : public SdkSample, public FrameObjListener
{
protected:
	DeferredShadingSystem *mSystem;
	SelectMenu* mDisplayModeMenu;
    BoxSky * mBoxSky;
public:
    Sample_DeferredShading()
	{
		mInfo["Title"] = "Deferred Shading";
		mInfo["Description"] = "A sample implementation of a deferred renderer using the compositor framework.";
		mInfo["Thumbnail"] = "thumb_deferred.png";
		mInfo["Category"] = "Lighting";
		mInfo["Help"] = "See http://www.ogre3d.org/wiki/index.php/Deferred_Shading for more info";
        mBoxSky = 0;
    }
    ~Sample_DeferredShading()
    {
        if(mBoxSky)
        {
            N_delete mBoxSky;
            mSceneMgr->removeSky(mBoxSky);
            mBoxSky = 0;
        }
    }
protected:

	void cleanupContent(void)
	{
		delete ( SharedData::getSingletonPtr() );

        delete mSystem;
	}

	bool onRenderCore(const RenderFrameArgs * evt)
	{
		if( SdkSample::onRenderCore(evt) == false )
			return false;
		SharedData::getSingleton().iLastFrameTime = evt->mCurrent;

		if (SharedData::getSingleton().mMLAnimState)
			SharedData::getSingleton().mMLAnimState->addTime(evt->mCurrent);
		return true;
	}

	void setupControls()
	{
		mTrayMgr->showCursor();

		// create checkboxs to toggle ssao and shadows
		mTrayMgr->createCheckBox(TL_TOPLEFT, "DeferredShading", "Deferred Shading", 220)->setChecked(true, false);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "SSAO", "Ambient Occlusion", 220)->setChecked(false, false);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "GlobalLight", "Global Light", 220)->setChecked(true, false);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Shadows", "Shadows", 220)->setChecked(true, false);

		// create a menu to choose the model displayed
		mDisplayModeMenu = mTrayMgr->createThickSelectMenu(TL_TOPLEFT, "DisplayMode", "Display Mode", 220, 4);
		mDisplayModeMenu->addItem("Regular view");
		mDisplayModeMenu->addItem("Debug colours");
		mDisplayModeMenu->addItem("Debug normals");
		mDisplayModeMenu->addItem("Debug depth / specular");
	}

	void itemSelected(SelectMenu* menu)
	{
		//Options are aligned with the mode enum
		SharedData::getSingleton().iSystem->setMode(
                                                    (DeferredShadingSystem::DSMode)menu->getSelectionIndex());
	}

	void checkBoxToggled(CheckBox* box)
	{
		if (box->getName() == "SSAO")
		{
			SharedData::getSingleton().iSystem->setSSAO(box->isChecked());
		}
		else if (box->getName() == "GlobalLight")
		{
			SharedData::getSingleton().iGlobalActivate = box->isChecked();
			SharedData::getSingleton().iMainLight->setVisible(box->isChecked());
		}
		else if (box->getName() == "Shadows")
		{
			mSceneMgr->setShadowTechnique(box->isChecked() ?
                SHADOWTYPE_TEXTURE_ADDITIVE : SHADOWTYPE_NONE);
		}
		else if (box->getName() == "DeferredShading")
		{
			SharedData::getSingleton().iSystem->setActive(box->isChecked());
		}
	}

    //Utility function to help set scene up
    void setEntityHeight(GeometrySpace * ent, NIIf newHeight)
    {
        NIIf curHeight = ent->getMesh()->getAABB().getSize().y;
        NIIf scaleFactor = newHeight / curHeight;

        SpaceNode * parentNode = ent->getSceneNode();
        parentNode->setScale(Vector3f(scaleFactor, scaleFactor, scaleFactor));
    }

	void createAtheneScene(SpaceNode * rootNode)
	{
        unsigned short src, dest;
		// Prepare athene mesh for normalmapping
        MeshPtr pAthene = N_Only(Mesh).load("athene.mesh", GroupDefault);
        if(!pAthene->checkTangentBuffer(false))
            pAthene->buildTangentBuffer(VT_Tangent);

		//Create an athena statue
        GeometrySpace * athena = mSceneMgr->createGeo("Athena", "athene.mesh");
		athena->setMaterialName("DeferredDemo/DeferredAthena");
		SpaceNode * aNode = rootNode->create();
		aNode->attachObject(athena);
		aNode->setPos(Vector3f(-8.5, 4.5, 0));
        setEntityHeight(athena, 4.0);
        aNode->yaw(Ogre::Angle(90));
		// Create some happy little lights to decorate the athena statue
		createSampleLights();
	}

	void createKnotScene(SpaceNode * rootNode)
	{
        unsigned short src, dest;
		// Prepare knot mesh for normal mapping
		MeshPtr pKnot = N_Only(Mesh).load("knot.mesh", GroupDefault);
        if(!pKnot->checkTangentBuffer(false))
            pKnot->buildTangentBuffer(VT_Tangent);

		// Create a bunch of knots with spotlights hanging from above
		GeometrySpace* knotEnt = mSceneMgr->createGeo("Knot", "knot.mesh");
		knotEnt->setMaterialName("DeferredDemo/RockWall");
		//knotEnt->setMeshLodBias(0.25f);
        Vector3 knotStartPos(25.5, 2, 5.5);
        Vector3 knotDiff(-3.7, 0, 0);
        for (int i=0; i < 5; i++)
        {
            char cloneName[16];
			sprintf(cloneName, "Knot%d", i);
            GeometrySpace * cloneKnot = knotEnt->clone(cloneName);
            Vector3 clonePos = knotStartPos + knotDiff * i;
			SpaceNode * cloneNode = rootNode->create(clonePos);
            cloneNode->attachObject(cloneKnot);
            setEntityHeight(cloneKnot, 3);
            cloneNode->yaw(Angle(i * 17));
            cloneNode->roll(Angle(i * 31));

            sprintf(cloneName, "KnotLight%d", i);
            Light * knotLight = mSceneMgr->createLight(cloneName);
            knotLight->setType(LT_SPOT);
            knotLight->setDiffuse(SAMPLE_COLORS[i]);
            knotLight->setSpecular(Colour::White);
            knotLight->setPos(clonePos + Vector3(0,3,0));
            knotLight->setDirection(Vector3::INV_Y);
            knotLight->setSpotlightInner(Angle(25));
            knotLight->setSpotlightOuter(Angle(45));
            knotLight->setSpotlightFalloff(1);
            knotLight->setRange(6);
            knotLight->setAttenuationConstant(1);
            knotLight->setAttenuationLinear(0.2);
            knotLight->setAttenuationQuadric(0);
        }
	}

	void createObjects(SpaceNode* rootNode)
	{
		// Create ogre heads to decorate the wall
		GeometrySpace* ogreHead = mSceneMgr->createGeo("Head", "ogrehead.mesh");
        Vector3 headStartPos[2] = { Vector3(25.25,11,3), Vector3(25.25,11,-3) };
        Vector3 headDiff(-3.7,0,0);
        for (int i=0; i < 12; i++)
        {
            char cloneName[16];
			sprintf(cloneName, "OgreHead%d", i);
            GeometrySpace* cloneHead = ogreHead->clone(cloneName);
            Vector3 clonePos = headStartPos[i%2] + headDiff*(i/2);
            if ((i/2) >= 4) clonePos.x -= 0.75;
			SpaceNode* cloneNode = rootNode->create(clonePos);
            cloneNode->attachObject(cloneHead);
            setEntityHeight(cloneHead, 1.5);
            if (i % 2 == 0)
            {
                cloneNode->yaw(Angle(180));
            }
        }

		// Create a pile of wood pallets
        GeometrySpace* woodPallet = mSceneMgr->createGeo("Pallet", "WoodPallet.mesh");
        Vector3 woodStartPos(10, 0.5, -5.5);
        Vector3 woodDiff(0, 0.3, 0);
        for (int i=0; i < 5; i++)
        {
            char cloneName[16];
			sprintf(cloneName, "WoodPallet%d", i);
            GeometrySpace* clonePallet = woodPallet->clone(cloneName);
            Vector3 clonePos = woodStartPos + woodDiff*i;
			SpaceNode* cloneNode = rootNode->create(clonePos);
            cloneNode->attachObject(clonePallet);
            setEntityHeight(clonePallet, 0.3);
            cloneNode->yaw(Angle(i*20));
        }

	}

    void testCapabilities(const RenderSysFeature* caps)
    {
        if (!caps->hasCapability(GF_Program_Vertex) || !(caps->hasCapability(GF_Program_Fragment)))
        {
            N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your card does not support vertex and fragment programs, so cannot "
                        "run this demo. Sorry!",
                        "DeferredShading::createScene");
        }
		if (caps->getMaxMultiFrameObj()<2)
        {
            N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your card does not support at least two simultaneous render targets, so cannot "
                        "run this demo. Sorry!",
                        "DeferredShading::createScene");
        }
    }

    // Just override the mandatory create scene method
    void setupContent(void)
    {
		mCameraMan->setTopSpeed(20.0);
		new SharedData();
		mSystem = 0;

        // Set ambient light
        mSceneMgr->setAmbient(Colour(0.15, 0.00, 0.00));
        mBoxSky = N_new BoxSky(mSceneMgr);
        mBoxSky->setRenderGroup(RQG_Sky);
        mBoxSky->setMaterial("DeferredDemo/SkyBox");
        mBoxSky->buildGeo(500);
        mSceneMgr->addSky(mBoxSky);
        // Create main, static light
		Light * l1 = mSceneMgr->createLight(0);
        l1->setType(LT_DIRECTIONAL);
        l1->setDiffuse(0.5f, 0.45f, 0.1f);
		l1->setDirection(Vector3f(1, -0.5, -0.2));
		l1->setShadowFarClip(250);
		l1->setShadowExtent(75);
		//Turn this on to have the directional light cast shadows
		l1->setCastShadow(false);

		mCamera->setPos(NII::Vector3f(25, 5, 0));
        mCamera->lookAt(NII::Vector3f(0,0,0));
		mCamera->setFarClipDistance(1000.0);
        mCamera->setNearClipDistance(0.5);
		setDragLook(true);

		mSystem = new DeferredShadingSystem(mWindow->getViewport(0), mSceneMgr, mCamera);
		SharedData::getSingleton().iSystem = mSystem;
		mSystem->initialize();

        // safely setup application's (not postfilter!) shared data
		SharedData::getSingleton().iCamera = mCamera;
		SharedData::getSingleton().iRoot = mRoot;
		SharedData::getSingleton().iWindow = mWindow;
		SharedData::getSingleton().iActivate = true;
		SharedData::getSingleton().iGlobalActivate = true;
		SharedData::getSingleton().iMainLight = l1;

		//Create the scene
		// Create "root" node
		SpaceNode* rootNode = mSceneMgr->getOriginNode()->create();

        // Create the cathedral - this will be the static scene
		GeometrySpace* cathedralEnt = mSceneMgr->createGeo("Cathedral", "sibenik.mesh");
        SpaceNode* cathedralNode = rootNode->create();
        cathedralNode->attachObject(cathedralEnt);

        createAtheneScene(rootNode);
		createKnotScene(rootNode);
		createObjects(rootNode);

		setupControls();
	}

	void createSampleLights()
	{
		// Create some lights
		vector<Light *>::type lights;
		SpaceNode * parentNode = mSceneMgr->getOriginNode()->create("LightsParent");
		// Create light nodes
		vector<PosNode *>::type nodes;

        Vector4 attParams = Vector4(4,1,0,7);
        NIIf lightRadius = 25;

		Light * a = mSceneMgr->createLight(0);
		SpaceNode * an = parentNode->create();
		an->attachObject(a);
        a->setRange(attParams.x);
        a->setAttenuationConstant(attParams.y);
        a->setAttenuationLinear(attParams.z);
        a->setAttenuationQuadric(attParams.w);

		an->setPos(Vector3f(0,0,lightRadius));
		a->setDiffuse(1,0,0);
		//a->setSpecular(0.5,0,0);
		lights.push_back(a);
		nodes.push_back(an);

		Light *b = mSceneMgr->createLight(0);
		SpaceNode *bn = parentNode->create();
		bn->attachObject(b);
        b->setRange(attParams.x);
        b->setAttenuationConstant(attParams.y);
        b->setAttenuationLinear(attParams.z);
        b->setAttenuationQuadric(attParams.w);
		bn->setPos(Vector3f(lightRadius, 0, 0));
		b->setDiffuse(1,1,0);
		//b->setSpecular(0.5,0.5,0);
		lights.push_back(b);
		nodes.push_back(bn);

		Light *c = mSceneMgr->createLight(0);
		SpaceNode *cn = parentNode->create();
		cn->attachObject(c);
        c->setRange(attParams.x);
        c->setAttenuationConstant(attParams.y);
        c->setAttenuationLinear(attParams.z);
        c->setAttenuationQuadric(attParams.w);
		cn->setPos(Vector3f(0,0,-lightRadius));
		c->setDiffuse(0,1,1);
		c->setSpecular(0.25,1.0,1.0); // Cyan light has specular component
		lights.push_back(c);
		nodes.push_back(cn);

		Light *d = mSceneMgr->createLight(0);
		SpaceNode *dn = parentNode->create();
		dn->attachObject(d);
        d->setRange(attParams.x);
        d->setAttenuationConstant(attParams.y);
        d->setAttenuationLinear(attParams.z);
        d->setAttenuationQuadric(attParams.w);
		dn->setPos(Vector3f(-lightRadius, 0, 0));
		d->setDiffuse(1,0,1);
		d->setSpecular(0.0,0,0.0);
		lights.push_back(d);
		nodes.push_back(dn);

		Light *e = mSceneMgr->createLight(0);
		SpaceNode *en = parentNode->create();
		en->attachObject(e);
        e->setRange(attParams.x);
        e->setAttenuationConstant(attParams.y);
        e->setAttenuationLinear(attParams.z);
        e->setAttenuationQuadric(attParams.w);
		en->setPos(Vector3f(lightRadius,0,lightRadius));
		e->setDiffuse(0,0,1);
		e->setSpecular(0,0,0);
		lights.push_back(e);
		nodes.push_back(en);

		Light *f = mSceneMgr->createLight(0);
		SpaceNode *fn = parentNode->create();
		fn->attachObject(f);
        f->setRange(attParams.x);
        f->setAttenuationConstant(attParams.y);
        f->setAttenuationLinear(attParams.z);
        f->setAttenuationQuadric(attParams.w);
		fn->setPos(Vector3f(-lightRadius,0,-lightRadius));
		f->setDiffuse(0,1,0);
		f->setSpecular(0,0.0,0.0);
		lights.push_back(f);
		nodes.push_back(fn);

		// Create marker meshes to show user where the lights are
		GeometrySpace * ent;
		GeomUtils::createSphere("PointLightMesh", 0.05f, 5, 5, true, true);
		for(vector<Light*>::type::iterator i = lights.begin(); i != lights.end(); ++i)
		{
			Light * light = *i;
			ent = mSceneMgr->createGeo(light->getName()+"v", "PointLightMesh");
			String matname = light->getName()+"m";
			// Create coloured material
			MaterialPtr mat = N_Only(Material).create(matname, GroupDefault);
            ShaderChColour * chc = mat->get(0)->get(0)->setColour(ShaderCh::SOT_Set);
            chc->setDiffuse(Colour(0.0f, 0.0f, 0.0f, 1.0f));
			chc->setAmbient(Colour(0.0f, 0.0f, 0.0f));
			chc->setEmissive(light->getDiffuse());

			ent->setMaterialName(matname);
			ent->setRenderGroup(DeferredShadingSystem::POST_GBUFFER_RENDER_QUEUE);
			static_cast<SpaceNode *>(light->getSpaceNode())->attachObject(ent);
			ent->setVisible(true);
		}

		// Store nodes for hiding/showing
		SharedData::getSingleton().mLightNodes = nodes;

		// Do some animation for node a-f
		// Generate helix structure
		float seconds_per_station = 1.0f;
		float r = 1.0;
		//Vector3 base(0,-30,0);
		Vector3 base(-8.75, 3.5, 0);

		float h=3;
		const size_t s_to_top = 16;
		const size_t stations = s_to_top*2-1;
		float ascend = h/((float)s_to_top);
		float stations_per_revolution = 3.5f;
		size_t skip = 2; // stations between lights
		Vector3 station_pos[stations];
		for(size_t x=0; x<s_to_top; ++x)
		{
			float theta = ((float)x/stations_per_revolution)*2.0f*Math::PI;
			station_pos[x] = base+Vector3(Math::Sin(theta)*r, ascend*x, Math::Cos(theta)*r);
		}
		for(size_t x=s_to_top; x<stations; ++x)
		{
			float theta = ((float)x/stations_per_revolution)*2.0f*Math::PI;
			station_pos[x] = base+Vector3(Math::Sin(theta)*r, h-ascend*(x-s_to_top), Math::Cos(theta)*r);
		}
		// Create a track for the light swarm
		Animation * anim = mSceneMgr->createAnimation("LightSwarmTrack", stations*seconds_per_station);
		// Spline it for nice curves
		anim->setInterpolationMode(NodeFusion::M_SPLINE);
		for(unsigned int x = 0; x < nodes.size(); ++x)
		{
			// Create a track to animate the camera's node
            NodeKeyFrameTrack * track = N_new NodeKeyFrameTrack(anim, x);
            track->setAssociatedNode(nodes[x]);
			for(size_t y = 0; y <= stations; ++y)
			{
				// Setup keyframes
				NodeKeyFrame * key = static_cast<NodeKeyFrame *>(track->create(y*seconds_per_station)); // A start position
				key->setTranslate(station_pos[(x*skip+y)%stations]);
				// Make sure size of light doesn't change
				key->setScale(nodes[x]->getScale());
			}
            track->buildSpline();
		}
		// Create a new animation state to track this
		SharedData::getSingleton().mMLAnimState = mSceneMgr->createFusion("LightSwarmTrack");
		SharedData::getSingleton().mMLAnimState->setEnabled(true);

		/*Light* spotLight = mSceneMgr->createLight("Spotlight1");
         spotLight->setType(LT_SPOT);
         spotLight->setRange(200);
         spotLight->setAttenuationConstant(1.0f);
         spotLight->setAttenuationLinear(0);
         spotLight->setAttenuationQuadric(0);
         spotLight->setSpotlightInner(Angle(30.0));
         spotLight->setSpotlightOuter(Angle(45.0));
         spotLight->setSpotlightFalloff(0.8);
         spotLight->setPos(0,120,0);
         spotLight->setDirection(Vector3f(0, -1, 0));
         spotLight->setDiffuse(1,1,1);
         spotLight->setSpecular(1,1,1);*/
	}
};

#endif
