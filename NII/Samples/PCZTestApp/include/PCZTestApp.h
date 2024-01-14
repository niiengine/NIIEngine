/*
-----------------------------------------------------------------------------
This file based on the Example App framework from Ogre3d (www.ogre3d.org)
-----------------------------------------------------------------------------
*/

/**
    \file 
        PCZTestApp.h
    \brief
        Specialisation of OGRE's framework application to test/demo the
		Portal Connected Zone (PCZ) Scene Manager Plugin.
*/

#include "ExampleApplication.h" 
#include "RoomObject.h"
#include "OgreTerrainZone.h"

PCZSceneNode * buildingNode;
Vector3 buildingTranslate;
RaySpaceQuery * raySpaceQuery = 0;
SpaceObj * targetMO = 0;

class PCZTestFrameListener : public ExampleFrameListener
{
public:
    PCZTestFrameListener(ViewWindow * win, Camera * cam) : 
        ExampleFrameListener(win, cam)
    {
		mMoveSpeed = 15.0;
    }

	void moveCamera()
	{
		// Make all the spatial changes to the camera's scene node
		// Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW
		//(e.g. airplane)
        mCamera->getSceneNode()->translate(mTranslateVector, PosNode::SM_Local);
		mCamera->getSceneNode()->pitch(mRotY);
		mCamera->getSceneNode()->yaw(mRotX, PosNode::SM_Space);
        buildingNode->translate(buildingTranslate, PosNode::SM_Local);
	}

    bool onRenderCore(const RenderFrameArgs * evt)
    {
        if(ExampleFrameListener::onRenderCore(evt) == false)
		return false;

        buildingTranslate = Vector3(0,0,0);
		if(mKeyboard->isKeyDown(OIS::KC_LBRACKET))
        {
            buildingTranslate = Vector3(0,-10,0);
        }
		if(mKeyboard->isKeyDown(OIS::KC_RBRACKET))
        {
            buildingTranslate = Vector3(0,10,0);
        }

		if(mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT))
        {
            mMoveSpeed = 150;
        }
		else
		{
            mMoveSpeed = 15;
		}

        // test the ray scene query by showing bounding box of whatever the camera is pointing directly at 
        // (takes furthest hit)
        static Ray updateRay;
        updateRay.setOrigin(mCamera->getSceneNode()->getPos());
        updateRay.setDirection(mCamera->getSceneNode()->getOri() * Vector3::INV_Z);
        raySpaceQuery->setRay(updateRay);
        PCZone * zone = ((PCZSceneNode *)(mCamera->getSceneNode()))->getHomeZone();
        ((PCZRaySpaceQuery *)raySpaceQuery)->setStartZone(zone);
        ((PCZRaySpaceQuery *)raySpaceQuery)->setExcludeNode(mCamera->getSceneNode());
        RaySpaceQueryResult & qryResult = raySpaceQuery->execute();
        RaySpaceQueryResult::iterator i = qryResult.begin();
        if(i != qryResult.end())
        {
			RaySpaceQueryResult::reverse_iterator ri = qryResult.rbegin();
            SpaceObj * mo = ri->movable;
            if(targetMO != mo)
            {
                if(targetMO != 0)
                {
                    targetMO->getSceneNode()->setAABBView(false);
                }
                targetMO = mo;
                targetMO->getSceneNode()->setAABBView(true);
            }
        }

        return true;
    }
};

class PCZTestApplication : public ExampleApplication
{
public:
    PCZTestApplication() 
    {
        mBoxSky = 0;
    }
    ~PCZTestApplication() 
    {
        delete raySpaceQuery;
        if(mBoxSky)
        {
            mSceneMgr->removeSky(mBoxSky);
            N_delete mBoxSky;
            mBoxSky = 0;
        }
    }
protected:
	SpaceNode * mCameraNode;
    BoxSky * mBoxSky;
	virtual void chooseSceneManager(void)
    {
        // Create the SpaceManager, in this case a generic one
        mSceneMgr = mRoot->createSpaceManager("PCZSceneManager", "PCZSceneManager");
		// initialize the scene manager using terrain as default zone
		String zoneTypeName = "ZoneType_Default";
		String zoneFilename = "none";
		((PCZSceneManager*)mSceneMgr)->init(zoneTypeName);
		//mSceneMgr->setAABBView(true);
    }
    virtual void createFrameListener(void)
    {
        mFrameListener= new PCZTestFrameListener(mWindow, mCamera);
        mRoot->addFrameListener(mFrameListener);
    }
    virtual void createCamera(void)
    {
        // Create the camera
        mCamera = mSceneMgr->createCamera("PlayerCam");

		// NEW: create a node for the camera and control that instead of camera directly.
		// We do this because PCZSceneManager requires camera to have a node 
		mCameraNode = mSceneMgr->getOriginNode()->create("PlayerCamNode");
		// attach the camera to the node
		mCameraNode->attachObject(mCamera);
		// fix the yaw axis of the camera
		mCameraNode->setCustomYaw(true);

        mCamera->setNearClipDistance(2);
        mCamera->setFarClipDistance( 1000 );
		// set camera zone
//		((PCZSceneNode*)(mCameraNode))->setHomeZone(((PCZSceneManager*)(mSceneMgr))->getDefaultZone());

    }
	// utility function to create terrain zones easily
	PCZone * createTerrainZone(String & zoneName, String & terrain_cfg)
	{
		// load terrain into the terrain zone
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
        terrain_cfg = mResourcePath + terrain_cfg;
#endif
		PCZone * terrainZone = ((PCZSceneManager*)mSceneMgr)->createZone(String("ZoneType_Terrain"), zoneName);
		terrainZone->notifyCameraCreated(mCamera);
		((PCZSceneManager*)mSceneMgr)->setZoneGeometry( zoneName, (PCZSceneNode*)mSceneMgr->getOriginNode(), terrain_cfg );

		// create aab portal(s) around the terrain
		String portalName;
		Vector3 corners[2];
		AABox aabb;

		// make portal from terrain to default
		Portal * p;
		terrainZone->getAABB(aabb);
		portalName = Ogre::String("PortalFrom"+zoneName+"ToDefault_Zone");
		p = ((PCZSceneManager*)mSceneMgr)->createPortal(portalName, Ogre::Portal::PORTAL_TYPE_AABB);
		corners[0] = aabb.getMin();
		corners[1] = aabb.getMax();
		p->setCorner(0, corners[0]);
		p->setCorner(1, corners[1]);
		p->setDirection(Ogre::Vector3::INV_Z); // this indicates an "inward" pointing normal
		// associate the portal with the terrain's main node
		p->setNode(terrainZone->getEnclosureNode());
		// IMPORTANT: Update the derived values of the portal
		p->updateDerivedValues();
		// add the portal to the zone
		terrainZone->_addPortal(p);
	
		// make portal from default to terrain
		portalName = Ogre::String("PortalFromDefault_ZoneTo"+zoneName);
		Portal * p2;
		p2 = ((PCZSceneManager*)mSceneMgr)->createPortal(portalName, Ogre::Portal::PORTAL_TYPE_AABB);
		corners[0] = aabb.getMin();
		corners[1] = aabb.getMax();
		p2->setCorner(0, corners[0]);
		p2->setCorner(1, corners[1]);
		p2->setDirection(Ogre::Vector3::Z); // this indicates an "outward" pointing normal
		// associate the portal with the terrain's main node
		p2->setNode(terrainZone->getEnclosureNode());
		// IMPORTANT: Update the derived values of the portal
		p2->updateDerivedValues();
		// add the portal to the zone
		((PCZSceneManager*)mSceneMgr)->getDefaultZone()->_addPortal(p2);

		// connect the portals manually
		p->setTargetPortal(p2);
		p2->setTargetPortal(p);
		p->setTargetZone(((PCZSceneManager*)mSceneMgr)->getDefaultZone());
		p2->setTargetZone(terrainZone);

		return terrainZone;
	}

	PCZSceneNode* createAntiPortal(const String& name)
	{
		// Create antiportal test.
		PCZSceneNode* antiPortalNode = (PCZSceneNode*)mSceneMgr->getOriginNode()->create();
		PCZone* defaultZone = ((PCZSceneManager*)mSceneMgr)->getDefaultZone();
		AntiPortal* antiPortal = ((PCZSceneManager*)mSceneMgr)->createAntiPortal(name);
		antiPortal->setCorner(0, Vector3(100.0f, 100.0f, 0.0f));
		antiPortal->setCorner(1, Vector3(100.0f, -100.0f, 0.0f));
		antiPortal->setCorner(2, Vector3(-100.0f, -100.0f, 0.0f));
		antiPortal->setCorner(3, Vector3(-100.0f, 100.0f, 0.0f));
		antiPortalNode->attachObject(antiPortal);
		defaultZone->_addAntiPortal(antiPortal);
		((PCZSceneManager*)mSceneMgr)->addPCZSceneNode(antiPortalNode, defaultZone);

		// Anti portal prop.
		GeometrySpace* planeEnt = mSceneMgr->createGeo(name + "GeometrySpace", GSP_Plane);
		planeEnt->setMaterialName("TransparentGlassTinted");
		antiPortalNode->attachObject(planeEnt);

		return antiPortalNode;
	}

    // Just override the mandatory create scene method
    void createScene(void)
    {
        // Set ambient light
        mSceneMgr->setAmbient(Colour(0.25, 0.25, 0.25));
        mBoxSky = N_new BoxSky(mSceneMgr);
        mBoxSky->setMaterial("Examples/CloudyNoonSkyBox");
        mBoxSky->setRenderGroup(RQG_Sky);
        // Create a skybox
        mBoxSky->buildGeo(500);
        mSceneMgr->addSky(mBoxSky);
		// put the skybox node in the default zone
		((PCZSceneManager*)mSceneMgr)->setSkyZone(0);

        // Create a light
        Light * l = mSceneMgr->createLight("MainLight");
        l->setPos(0,0,0); 
        l->setRange(500);
        l->setAttenuationConstant(0.5);
        l->setAttenuationLinear(1.0);
        l->setAttenuationQuadric(0.0);
        // Accept default settings: point light, white diffuse, just set position
        // attach light to a scene node so the PCZSM can handle it properly (zone-wise)
        // IMPORTANT: Lights (just like cameras) MUST be connected to a scene node!
	    SpaceNode * lightNode = mCameraNode->create("light_Node");
		lightNode->attachObject(l);      

        // Fog
        // NB it's VERY important to set this before calling setSceneGeo 
        // because the vertex program picked will be different
        Colour fadeColour(0.101, 0.125, 0.1836);
        mSceneMgr->setFog(ShaderChFog(FM_LINEAR, fadeColour, .001, 500, 1000));
        mWindow->getViewport(0)->setBgColour(fadeColour);

		// create a terrain zone
//        String terrain_cfg("terrain.cfg");
//		String zoneName("Terrain1_Zone");
//		PCZone * terrainZone = createTerrainZone(zoneName, terrain_cfg);

/*		// Create another terrain zone
        terrain_cfg = "terrain.cfg";
		zoneName = "Terrain2_Zone";
		terrainZone = createTerrainZone(zoneName, terrain_cfg);
		// move second terrain next to first terrain
		terrainZone->getEnclosureNode()->setPos(Vector3f(1500, 0, 0));

		// Create another terrain zone
        terrain_cfg = "terrain.cfg";
		zoneName = "Terrain3_Zone";
		terrainZone = createTerrainZone(zoneName, terrain_cfg);
		// move terrain next to first terrain
		terrainZone->getEnclosureNode()->setPos(Vector3f(0, 0, 1500));

		// Create another terrain zone
        terrain_cfg = "terrain.cfg";
		zoneName = "Terrain4_Zone";
		terrainZone = createTerrainZone(zoneName, terrain_cfg);
		// move terrain next to first terrain
		terrainZone->getEnclosureNode()->setPos(Vector3f(-1500, 0, 0));

		// Create another terrain zone
        terrain_cfg = "terrain.cfg";
		zoneName = "Terrain5_Zone";
		terrainZone = createTerrainZone(zoneName, terrain_cfg);
		// move terrain next to first terrain
		terrainZone->getEnclosureNode()->setPos(Vector3f(0, 0, -1500));

		// Create another terrain zone
        terrain_cfg = "terrain.cfg";
		zoneName = "Terrain6_Zone";
		terrainZone = createTerrainZone(zoneName, terrain_cfg);
		// move terrain next to first terrain
		terrainZone->getEnclosureNode()->setPos(Vector3f(1500, 0, 1500));

		// Create another terrain zone
        terrain_cfg = "terrain.cfg";
		zoneName = "Terrain7_Zone";
		terrainZone = createTerrainZone(zoneName, terrain_cfg);
		// move terrain next to first terrain
		terrainZone->getEnclosureNode()->setPos(Vector3f(-1500, 0, -1500));

		// Create another terrain zone
        terrain_cfg = "terrain.cfg";
		zoneName = "Terrain8_Zone";
		terrainZone = createTerrainZone(zoneName, terrain_cfg);
		// move terrain next to first terrain
		terrainZone->getEnclosureNode()->setPos(Vector3f(-1500, 0, 1500));

		// Create another terrain zone
        terrain_cfg = "terrain.cfg";
		zoneName = "Terrain9_Zone";
		terrainZone = createTerrainZone(zoneName, terrain_cfg);
		// move terrain next to first terrain
		terrainZone->getEnclosureNode()->setPos(Vector3f(1500, 0, -1500));
*/
		// set far clip plane to one terrain zone width (we have a LOT of terrain here, so we need to do far clipping!)
        mCamera->setFarClipDistance(1500);

		// create test buildinig
		RoomObject roomObj;
		buildingNode = roomObj.createTestBuilding(mSceneMgr, String("1"));
		buildingNode->setPos(Vector3f(500, 165, 570));
		//Ogre::Radian r = Radian(3.1416/7.0);
		//buildingNode->rotate(Vector3::Y, r);

		// create another test buildinig
		RoomObject roomObj2;
		buildingNode = roomObj2.createTestBuilding(mSceneMgr, String("2"));
		buildingNode->setPos(Vector3f(400, 165, 570));
		//Ogre::Radian r = Radian(3.1416/7.0);
		//buildingNode->rotate(Vector3::Y, r);

		PCZSceneNode* antiPortalNode1 = createAntiPortal("AntiPortal1");
		antiPortalNode1->setPos(Vector3f(450, 200, 800));
		antiPortalNode1->setScale(Vector3f(0.5f, 0.5f, 0.5f));

		PCZSceneNode* antiPortalNode2 = createAntiPortal("AntiPortal2");
		antiPortalNode2->setPos(Vector3f(460, 200, 700));
		antiPortalNode2->setScale(Vector3f(0.5f, 0.5f, 0.5f));

        // Position camera in the center of the building
        mCameraNode->setPos(buildingNode->getPos());
        // Look back along -Z
		mCamera->lookAt(mCameraNode->getSpacePos() + Vector3(0, 0, -300));
		// Update bounds for camera
		mCameraNode->updateSpaceAABB();

        // create the ray scene query
        raySpaceQuery = mSceneMgr->createRayQuery(
            Ray(mCamera->getSpaceNode()->getPos(), Vector3::INV_Z));
        raySpaceQuery->setSortDistance(true);
        raySpaceQuery->setMaxResult(5);
    }
};