/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
Filename:    BspCollision.cpp
Description: Somewhere to play in the sand...
-----------------------------------------------------------------------------
*/

#include "OgreReferenceAppLayer.h"

#include "ExampleRefAppApplication.h"
#include "OgreStringConverter.h"

// Hacky globals
ApplicationObject * ball;
SpaceNode * targetNode;
RaySpaceQuery * rsq = 0;
static const int num_rows = 3;

// RakNet: Logger includes.
#include "SQLiteClientLoggerPlugin.h"
#include "NetworkObj.h"
#include "Ogre3D_DX9_BackbufferGrabber.h"

// Event handler to add ability to alter curvature
class BspCollisionListener : public ExampleRefAppFrameListener
{
protected:
	// RakNet: For logging video
	ClusterNetworkObj packetizedTCP;
	SQLiteClientLoggerPlugin loggerPlugin;
	Ogre3D_DX9_BackbufferGrabber backbufferGrabber;
	TimeDurMS lastScreenshotTime;

	// Also save the world * so we can log it out
	World * mWorld;
public:
    BspCollisionListener(ViewWindow * win, CollideCamera * cam, World * world) : 
        ExampleRefAppFrameListener(win, cam)
    {
		// RakNet:  Connect to server using TCP, for logging video
		packetizedTCP.AttachPlugin(&loggerPlugin);
		packetizedTCP.Start(0,0);
		loggerPlugin.SetServerParameters(packetizedTCP.Connect("127.0.0.1", 38123, true), "ogrevideo.sqlite");
		// For testing, I'm using 512x512 with a huge memory constraint at 30 FPS
		// For a real game, you probably want to limit this to 256x256, with a 8MB memory constraint, at 15-20 FPS
		loggerPlugin.SetMemoryConstraint(128000000);
		backbufferGrabber.InitBackbufferGrabber(mWindow, 512, 512);
		lastScreenshotTime = 0;

		mWorld = world;
    }

    bool onEndRender(const RenderFrameArgs * evt)
    {
        // local just to stop toggles flipping too fast
        static NIIf timeUntilNextToggle = 0;

        // Deal with time delays that are too large
        // If we exceed this limit, we ignore
        static const TimeDurMS MAX_TIME_INCREMENT = 500;
        if(evt->mCurrent > MAX_TIME_INCREMENT)
        {
            return true;
        }

        if(timeUntilNextToggle >= 0)
            timeUntilNextToggle -= evt->mCurrent;

        // Call superclass
        bool ret = ExampleRefAppFrameListener::onEndRender(evt);

		if(mKeyboard->isKeyDown(OIS::KC_SPACE) && timeUntilNextToggle <= 0)
        {
            timeUntilNextToggle = 2;
            ball->setPosition(mCamera->getPosition() +
                mCamera->getDirection() * mCamera->getNearClipDistance() * 2);
            ball->setLinearVelocity(mCamera->getDirection() * 200);
            ball->setAngularVelocity(Vector3::ZERO);

			// RakNet: Log events, which in this case is only firing the ball. Give the event a color so we can plot it
			rakSqlLog("EventData", "x,y,z,name,color",
				(mCamera->getPosition().x, mCamera->getPosition().y, mCamera->getPosition().z, "Fired Ball", "green"));
        }

        // Move the targeter
        rsq->setRay(mCamera->getRealCamera()->getRay(0.5, 0.5));
        RaySpaceQueryResult & rsqResult = rsq->execute();
        RaySpaceQueryResult::iterator ri = rsqResult.begin();
        if(ri != rsqResult.end())
        {
            QueryDetail & res = *ri;
            targetNode->setPosition(rsq->getRay().getPoint(res.mDistance));
        }

		// RakNet: Send screenshot and FPS info to server if connected, at most once every 30 milliseconds
		// This is constrained so we don't overflow the server with screenshots
		// Also only do it if we connected to the server
		TimeDurMS timeSinceLastLog=N_Engine().getTimer().getMS()-lastScreenshotTime;
		if(packetizedTCP.GetConnectionCount() > 0 && timeSinceLastLog > 30)
		{
			RGBImageBlob blob;
			backbufferGrabber.LockBackbufferCopy(&blob);
			N_assert(blob.data!=0);
			// RakNet: Log frame data, including screenshot and FPS
			SQLLogResult logResult = rakSqlLog("FrameData", "screenshot,averageFPS,mLast,mBest,mWorst,numTris,DebugText",
				(&blob, mWindow->getFPS().mAvg, mWindow->getFPS().mLast, 
                    mWindow->getFPS().mBest, mWindow->getFPS().mWorst,
                        (int)mWindow->getTriangleCount(),mDebugText.c_str()));
			// Release backbuffer as soon as possible, after sending frame data
			backbufferGrabber.ReleaseBackbufferCopy();
			if(logResult == SQLLR_WOULD_EXCEED_MEMORY_CONSTRAINT)
			{
				/// Sending too large of screenshots, or can't transfer data fast enough. See loggerPlugin.SetMemoryConstraint
			}

			// Also log out position of all world objects
			GeometrySpace * entity;
			SpaceNode * sceneNode;
			entity = mWorld->getSceneManager()->getGeo("ball");
			sceneNode = entity->getSceneNode();
			// RakNet: Log object position data over time
			rakSqlLog("ObjectData", "x,y,z,name,color",
				(sceneNode->getPosition().x, sceneNode->getPosition().y, sceneNode->getPosition().z, entity->getName().c_str(), "blue"));
			for(NCount row = 0; row < num_rows; ++row)
			{
				for(NCount i = 0; i < (num_rows - row); ++i)
				{
					String name = "box";
					name += StrConv::conv((row * num_rows) + i);
					entity = mWorld->getSceneManager()->getGeo(name);
					sceneNode = entity->getSceneNode();
					rakSqlLog("ObjectData", "x,y,z,name,color",
						(sceneNode->getPosition().x, sceneNode->getPosition().y, sceneNode->getPosition().z, entity->getName().c_str(), "red"));
				}
			}
			lastScreenshotTime=N_Engine().getTimer().getMS();
		}
        return ret;
    }
};

class BspCollisionApplication : public ExampleRefAppApplication
{
public:
    BspCollisionApplication()
    {
    }

    ~BspCollisionApplication()
    {
		delete rsq;
    }
protected:

    void chooseSceneManager(void)
    {
        mSceneMgr = mRoot->createSpaceManager("BspSceneManager");
    }
    void createWorld(void)
    {
        // Create BSP-specific world
        mWorld = new World(mSceneMgr, World::WT_REFAPP_BSP);
    }
    void createScene(void)
    {
        mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
        // Set ambient light
        mSceneMgr->setAmbient(Colour(0.2, 0.2, 0.2));
        // Create a point light
        Light* l = mSceneMgr->createLight("MainLight");
        l->setPosition(-100,50,100);
        l->setRange(8000);
        l->setAttenuationConstant(1);
        l->setAttenuationLinear(0);
        l->setAttenuationQuadric(0);

        // Setup World
        mWorld->setGravity(Vector3(0, 0, -60));
        mWorld->getSceneManager()->setSceneGeo("ogretestmap.bsp");

        // modify camera for close work
        mCamera->setNearClipDistance(10);
        mCamera->setFarClipDistance(20000);

        // Also change position, and set Quake-type orientation
        // Get random player start point
        ViewPoint vp = mSceneMgr->getSuggestedViewpoint(true);
        mCamera->setPosition(vp.position);
        mCamera->pitch(Angle(90)); // Quake uses X/Y horizon, Z up
        mCamera->rotate(vp.orientation);
        // Don't yaw along variable axis, causes leaning
        mCamera->setCustomYaw(true, Vector3::Z);
        // Look at the boxes
		mCamera->lookAt(Ogre::Vector3f(-150,40,30));

        ball = mWorld->createBall("ball", 7, vp.position + Vector3(0,0,80));
        ball->setDynamicsEnabled(true);
        ball->getEntity()->setMaterial("Ogre/Eyes");

		OgreRefApp::Box* box = mWorld->createBox("shelf", 75, 125, 5, Vector3(-150, 40, 30));
        box->getEntity()->setMaterial("Examples/Rocky");

        static const NIIf BOX_SIZE = 15.0f;

        for(int row = 0; row < num_rows; ++row)
        {
            for (int i = 0; i < (num_rows-row); ++i)
            {
                NIIf row_size = (num_rows - row) * BOX_SIZE * 1.25;
                String name = "box";
                name += StrConv::conv((row*num_rows) + i);
                box = mWorld->createBox(name, BOX_SIZE,BOX_SIZE,BOX_SIZE ,
                    Vector3(-150,
                        40 - (row_size * 0.5) + (i * BOX_SIZE * 1.25) ,
                        32.5 + (BOX_SIZE / 2) + (row * BOX_SIZE)));
                box->setDynamicsEnabled(false, true);
                box->getEntity()->setMaterial("Examples/10PointBlock");
            }
        }
        mCamera->setCollisionEnabled(false);
        mCamera->getRealCamera()->setIntersectMark(0);

        // Create the targeting sphere
        GeometrySpace * targetEnt = mSceneMgr->createGeo("testray", "sphere.mesh");
        MaterialPtr mat = N_Only(Material).create("targeter", GroupDefault);
        Pass * pass = mat->get(0)->get(0);
        ShaderChTextureUnit * tex = pass->getTexture().create();
        tex->getColourBlend()->setBlend(TBO_OPERAND1, Colour::Red, TBS_PRE_TEXTURE);
        pass->setLight(false);
        pass->setBlend(ShaderCh::SOT_Set)->setBlend(FBM_ADD);
        pass->setStencil(ShaderCh::SOT_Set)->setDepthWrite(false);

        targetEnt->setMaterial("targeter");
        targetEnt->setCastShadow(false);
        targetEnt->setIntersectMark(0);
        targetNode = mSceneMgr->getOriginNode()->create();
        targetNode->scale(Vector3f(0.025, 0.025, 0.025));
        targetNode->attachObject(targetEnt);

        rsq = mSceneMgr->createRayQuery(Ray());
        rsq->setSortDistance(true);
        rsq->setMaxResult(1);
        rsq->setPrimitivesMark(SpaceQuery::PT_Point);
    }
    // Create new frame listener
    void createFrameListener(void)
    {
        mFrameListener= new BspCollisionListener(mWindow, mCamera, mWorld);
        mRoot->addFrameListener(mFrameListener);
    }
public:
};

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char **argv)
#endif
{
    // Create application object
    BspCollisionApplication app;

    try
    {
        app.go();
    }
    catch(Exception & e)
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << "An exception has occured: " << e.getFullDescription();
#endif
    }
    return 0;
}