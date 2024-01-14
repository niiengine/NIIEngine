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

#ifdef WIN32

#else
#define HWND void*
#endif

// Ogre includes
#include "OgreTextOverlayGeo.h"
#include "Ogre.h"
#include <OIS.h>

// Stuff to help run ogre
#include "OverlayHelper.h"
#include "App3D.h"

#include "Nsleep.h"
#include "N_assert.h"
#include "StringTable.h"
#include "NiiNetLinkBase.h"

#include "NiiNetSerializer.h"
#include "NiiNetCommon.h"
#include "ReplicaManager3.h"
#include "NetworkIDManager.h"
#include "StringTable.h"
#include "Rand.h"
#include "TransformationHistory.h"

using namespace NII;
using namespace DataStructures;
using namespace OIS;

class Popcorn;

// Network variables
static const char *SERVER_IP_ADDRESS="127.0.0.1";
static const unsigned short SERVER_PORT=12345;
// How often the server sends position updates to the client
static const int DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES=250;

// Demo variables
static const int MIN_KERNELS=100;
static const int KERNELS_VARIANCE=60;
static const TimeDurMS POP_COUNTDOWN_MIN_DELAY_MS=1000;
static const TimeDurMS POP_COUNTDOWN_VARIANCE_MS=5000;
static const TimeDurMS RESTART_TIMER_MS=14000;
static const NIIf POSITION_VARIANCE=100.0f;
static const NIIf PLANE_VELOCITY_VARIANCE=30.0f;
static const NIIf UPWARD_VELOCITY_MINIMUM=35.0f;
static const NIIf UPWARD_VELOCITY_VARIANCE=25.0f;
static const NIIf DOWNWARD_ACCELERATION = -15.0f;

bool isServer;
Ogre::GeometrySpace *popcornKernel, *popcornPopped;
LinkBase *mLink;
DataStructures::List<Popcorn*> popcornList;
bool enableInterpolation;

App3D *app;

// This class represents a kernel of popcorn, which pops after a short delay
// When to pop, and the physics the popcorn takes is entirely controlled by the server
// Every DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES the server will send an update
// The client intentionally lags this far behind, so it always has a recent position to interpolate to
class Popcorn : public Replica3
{
public:
	Popcorn() {
		// Buffer up for 3 seconds if we were to get 30 updates per second
		transformationHistory.Init(30,3000);

		position=Ogre::Vector3::ZERO;
		orientation=Ogre::Quaternion::IDENTITY;
		// Visible position is where we are interpolating at, which is behind the real position
		visiblePosition=Ogre::Vector3::ZERO;
		visibleOrientation=Ogre::Quaternion::IDENTITY;
		isKernel=true;
	}
	virtual ~Popcorn()
	{
		if(isServer)
			BroadcastDestruction();

		app->GetSceneManager()->destroyGeo(sceneNode->getAttach(0)->getID());
		app->GetSceneManager()->getOriginNode()->destroy(sceneNode->getID());
		popcornList.RemoveAtIndex(popcornList.GetIndexOf(this));
	}

	bool isKernel;
	Ogre::Vector3 position;
	Ogre::Quaternion orientation;
	Ogre::Quaternion rotationalVelocity;
	Ogre::Vector3 velocity;
	Ogre::SpaceNode *sceneNode;
	TimeDurMS popCountdown;
	Ogre::Vector3 visiblePosition;
	Ogre::Quaternion visibleOrientation;
	TransformationHistory transformationHistory;

	virtual void WriteAllocationID(Connection_RM3 *destinationConnection, NetSerializer *allocationIdBitstream) const
	{
		StringTable::Instance()->EncodeString("Popcorn", 128, allocationIdBitstream);
	}
	virtual RM3ConstructionState QueryConstruction(Connection_RM3 *destinationConnection, ReplicaManager3 *replicaManager3)
	{
		if (isServer)
			return QueryConstruction_ServerConstruction(destinationConnection, isServer);
		else
			return QueryConstruction_ClientConstruction(destinationConnection, isServer);
	}
	virtual bool QueryRemoteConstruction(Connection_RM3 *sourceConnection){
		if (isServer)
			return QueryRemoteConstruction_ServerConstruction(sourceConnection, isServer);
		else
			return QueryRemoteConstruction_ClientConstruction(sourceConnection, isServer);
	}
	virtual void SerializeConstruction(NetSerializer *constructionBitstream, Connection_RM3 *destinationConnection){}
	virtual bool DeserializeConstruction(NetSerializer *constructionBitstream, Connection_RM3 *sourceConnection){return true;}
	virtual void SerializeDestruction(NetSerializer *destructionBitstream, Connection_RM3 *destinationConnection){}
	virtual bool DeserializeDestruction(NetSerializer *destructionBitstream, Connection_RM3 *sourceConnection){return true;}
	virtual RM3ActionOnPopConnection QueryActionOnPopConnection(Connection_RM3 *droppedConnection) const
	{
		if (isServer)
			return QueryActionOnPopConnection_Server(droppedConnection);
		else
			return QueryActionOnPopConnection_Client(droppedConnection);
	}
	virtual void DeallocReplica(Connection_RM3 *sourceConnection) {delete this;}
	virtual RM3QuerySerializationResult QuerySerialization(Connection_RM3 *destinationConnection)
	{
		if (isServer)
			return QuerySerialization_ServerSerializable(destinationConnection, isServer);
		else
			return QuerySerialization_ClientSerializable(destinationConnection, isServer);
	}
	virtual RM3SerializationResult read(SerializeParameters * serializeParameters)
	{
		// Autoserialize causes a network msg to go out when any of these member variables change.
		N_assert(isServer == true);
		serializeParameters->outputBitstream[0].write(isKernel);
		serializeParameters->outputBitstream[0].write((const Nui8 *)&position,sizeof(position));
		serializeParameters->outputBitstream[0].write((const Nui8 *)&velocity,sizeof(velocity));
		serializeParameters->outputBitstream[0].write((const Nui8 *)&orientation,sizeof(orientation));
		return RM3SR_BROADCAST_IDENTICALLY;
	}
	virtual void write(DeserializeParameters *deserializeParameters)
	{
		bool lastIsKernel = isKernel;

		// Doing this because we are also lagging position and orientation behind by DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES
		// Without it, the kernel would pop immediately but would not start moving
		deserializeParameters->serializationBitstream[0].read(isKernel);
		if (isKernel==false && lastIsKernel==true)
		popCountdown=DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES;

		deserializeParameters->serializationBitstream[0].read((unsigned char*)&position,sizeof(position));
		deserializeParameters->serializationBitstream[0].read((unsigned char*)&velocity,sizeof(velocity));
		deserializeParameters->serializationBitstream[0].read((unsigned char*)&orientation,sizeof(orientation));

		// Scene node starts invisible until we deserialize the intial startup data
		// This data could also have been passed in SerializeConstruction()
		sceneNode->setVisible(true,true);

		// Every time we get a network msg, we write it to the transformation history class.
		// This class, given a time in the past, can then return to us an interpolated position of where we should be in at that time
		transformationHistory.write(position,velocity,orientation,N_Engine().getTimer().getMS());
	}

	virtual void SetToPopped(void)
	{
		// Change the mesh, and add some velocity.
        String temp;
		isKernel=false;
		if (sceneNode->getAttach(0))
			app->GetSceneManager()->destroyGeo(sceneNode->getAttach(0)->getID());
		sceneNode->detachAll();
		sceneNode->attach(popcornPopped->clone(StrUtil::format(temp, "%p",this)));
		if (isServer)
		{
			velocity.x=-PLANE_VELOCITY_VARIANCE/2.0f+frandomMT()*PLANE_VELOCITY_VARIANCE;
			velocity.y=UPWARD_VELOCITY_MINIMUM+frandomMT()*UPWARD_VELOCITY_VARIANCE;
			velocity.z=-PLANE_VELOCITY_VARIANCE/2.0f+frandomMT()*PLANE_VELOCITY_VARIANCE;
		}
	}

	virtual void update(TimeDurMS timeElapsedMs)
	{
		visiblePosition=position;
		visibleOrientation=orientation;

		if (isKernel==false)
		{
			if (isServer)
			{
				// Only the server is doing physics
				NIIf timeElapsedSec = timeElapsedMs * .001f;
				position += velocity * timeElapsedSec + .5f * Ogre::Vector3(0.0f, DOWNWARD_ACCELERATION, 0.0f) * timeElapsedSec*timeElapsedSec;;
				velocity += Ogre::Vector3(0.0f, DOWNWARD_ACCELERATION, 0.0f) * timeElapsedSec;
				orientation = Quaternion::Slerp(timeElapsedSec, orientation, orientation * rotationalVelocity, true);
			}
			else
			{
				// See above - delay the pop until we start moving
				if (popCountdown <= timeElapsedMs)
				{
					SetToPopped();
					popCountdown=-1;
				}
				else
					popCountdown-=timeElapsedMs;

				// interpolate visible position, lagging behind by a small amount so where know where to update to
				if (enableInterpolation)
				{
					// Important: the first 3 parameters are in/out parameters, so set their values to the known current values before calling read()
					// We are subtracting DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES from the current time to get an interpolated position in the past
					// Without this we wouldn't have a node to interpolate to, and wouldn't know where to go
					transformationHistory.read(&visiblePosition, 0, &visibleOrientation, N_Engine().getTimer().getMS()-DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES,N_Engine().getTimer().getMS());
				}
			}
		}
		else
		{
			if (isServer)
			{
				if (popCountdown <= timeElapsedMs)
				{
					// Only the server controls when to pop
					SetToPopped();
				}
				else
					popCountdown-=timeElapsedMs;

			}

		}

		sceneNode->setPosition(visiblePosition);
		sceneNode->setOrientation(visibleOrientation);
	}


	static void ClearPopcorn()
	{
		// Destructor removes itself from this list
		while(popcornList.Size())
			delete popcornList[popcornList.Size()-1];
	}
	static Popcorn * CreateKernel(ReplicaManager3 *replicaManager3)
	{
		Popcorn *p = new Popcorn;
		// Tell the replication system about this new Replica instance.
		replicaManager3->Reference(p);
		static int count = 0;
        String temp;
		count++;
		popcornList.Insert(p);
		p->sceneNode = app->GetSceneManager()->getOriginNode()->create();
		p->sceneNode->attach(popcornKernel->clone(StrUtil::format(temp, "%p",p)));

		// Only server sets up initial positions, etc.
		if (isServer)
		{
            Vector3f norl;
			p->position.x=-POSITION_VARIANCE/2.0f+frandomMT()*POSITION_VARIANCE;
			p->position.y=0.0f;
			p->position.z=-POSITION_VARIANCE/2.0f+frandomMT()*POSITION_VARIANCE;
			p->velocity=Ogre::Vector3::ZERO;
			p->popCountdown=POP_COUNTDOWN_MIN_DELAY_MS + randomMT() % POP_COUNTDOWN_VARIANCE_MS;
            frandomMT()*2.0f).normalise(norl);
			p->orientation.from(Ogre::Radian(frandomMT() * 6.28f), 
                Ogre::Vector3(-1.0f+ frandomMT()*2.0f,-1.0f+frandomMT()*2.0f,-1.0f+norl);
                frandomMT()*2.0f).normalise(norl);
			p->rotationalVelocity.from(Ogre::Radian(frandomMT()*6.28f), 
                Ogre::Vector3(-1.0f+frandomMT()*2.0f,-1.0f+frandomMT()*2.0f,-1.0f+norl);
			p->visiblePosition=p->position;
			p->visibleOrientation=p->orientation;
		}
		else
			p->sceneNode->setVisible(false,true);

		return p;
	}
};

// One instance of Connection_RM2 is implicitly created per connection that uses ReplicaManager2. The most important function to implement is Construct() as this creates your game objects.
// It is designed this way so you can override per-connection behavior in your own game classes
class PopcornSampleConnection : public Connection_RM3
{
public:
	PopcornSampleConnection(const Address &address, UniqueID id) : Connection_RM3(address,id) {}
	virtual ~PopcornSampleConnection() {}

	// Callback used to create objects
	// See Connection_RM2::Construct in ReplicaManager2.h for a full explanation of each parameter
	virtual Replica3 *AllocReplica(NetSerializer *allocationIdBitstream, ReplicaManager3 *replicaManager3)
	{
		char objectName[128];
		StringTable::Instance()->DecodeString(objectName,128,allocationIdBitstream);
		if (strcmp(objectName,"Popcorn")==0)
		{
			return Popcorn::CreateKernel(replicaManager3);
		}
		return 0;
	}
};
class PopcornDemoRM3 : public ReplicaManager3
{
	virtual Connection_RM3* AllocConnection(const Address &address, UniqueID id) const
    {
        return new PopcornSampleConnection(address,id);
    }
	virtual void DeallocConnection(Connection_RM3 *connection) const
    {
        delete connection;
    }
};
PopcornDemoRM3 replicaManager3;
class ExampleApp : public App3D
{
public:
	ExampleApp()
	{
		quit=false;
	}

	~ExampleApp()
	{
	}

	void OnAppShutdown( void )
	{
		Popcorn::ClearPopcorn();

		App3D::OnAppShutdown();

		if( mInputManager )
		{
			mInputManager->destroyInputObject( mKeyboard );
			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}

		LinkBase::DestroyInstance(mLink);

	}

	void Render(AppTime curTimeMS)
	{
		App3D::Render(curTimeMS);
	}

	bool ShouldQuit(void) const {return quit || window->isClosed();}

	void update(AppTime curTimeMS, AppTime elapsedTimeMS)
	{
#ifndef WIN32
		WindowUtil::messagePump();
#endif

		// update all subsystems
		App3D::update(curTimeMS, elapsedTimeMS);
		overlayHelper.update(elapsedTimeMS);

		mKeyboard->capture();

		Ogre::Vector3 mTranslateVector = Ogre::Vector3::ZERO;

		NIIf mMoveSpeed=50.0f;
		NIIf mRotateSpeed=1.0f;
		NIIf mMoveScale = mMoveSpeed * elapsedTimeMS * .001f;
		// Take about 10 seconds for full rotation
		Ogre::Radian mRotScale(mRotateSpeed * elapsedTimeMS * .001f);

		if(mKeyboard->isKeyDown(KC_A))
			mTranslateVector.x = -mMoveScale;	// Move camera left

		if(mKeyboard->isKeyDown(KC_D))
			mTranslateVector.x = mMoveScale;	// Move camera RIGHT

		if(mKeyboard->isKeyDown(KC_UP) || mKeyboard->isKeyDown(KC_W) )
			mTranslateVector.z = -mMoveScale;	// Move camera forward

		if(mKeyboard->isKeyDown(KC_DOWN) || mKeyboard->isKeyDown(KC_S) )
			mTranslateVector.z = mMoveScale;	// Move camera backward

		if(mKeyboard->isKeyDown(KC_PGUP))
			mTranslateVector.y = mMoveScale;	// Move camera up

		if(mKeyboard->isKeyDown(KC_PGDOWN))
			mTranslateVector.y = -mMoveScale;	// Move camera down

		if(mKeyboard->isKeyDown(KC_RIGHT))
			camera->yaw(-mRotScale);

		if(mKeyboard->isKeyDown(KC_LEFT))
			camera->yaw(mRotScale);

		// Hold down space to see what it looks like without interpolation
		if(mKeyboard->isKeyDown(KC_SPACE))
			enableInterpolation=false;
		else
			enableInterpolation=true;

		static bool didUpdateDelayLastTick=false;
		bool didUpdateDelayThisTick=false;

		if (isStarted==false)
		{
			HostPeer sd;

			if(mKeyboard->isKeyDown(KC_S))
			{
				// Start server
				isStarted=true;
				isServer=true;
				ShowMessage("Server started");
				sd.port=SERVER_PORT;

			}

			if(mKeyboard->isKeyDown(KC_C))
			{
				isStarted=true;
                String temp;
				ShowMessage(StrUtil::format(temp, "Client started, connecting to %s", SERVER_IP_ADDRESS));
				// Start server
				isStarted=true;
				isServer=false;
				sd.port=0;
			}

			if (isStarted)
			{
				// Start RakNet, up to 32 connections if the server
				mLink = LinkBase::GetInstance();
				StartupResult sr = mLink->Startup(isServer ? 32 : 1,&sd,1);
				N_assert(sr==RAKNET_STARTED);
				mLink->AttachPlugin(&replicaManager3);
				replicaManager3.SetNetworkIDManager(&networkIdManager);
				//mLink->SetNetworkIDManager(&networkIdManager);
				// The server should allow systems to connect. Clients do not need to unless you want to use RakVoice or for some other reason want to transmit directly between systems.
				if (isServer)
				{
					mLink->SetMaximumIncomingConnections(32);
				}
				else
				{
					ConnectResultType car = mLink->Connect(SERVER_IP_ADDRESS,SERVER_PORT,0,0);
					N_assert(car==CONNECTION_ATTEMPT_STARTED);

				}
				replicaManager3.SetAutoSerializeInterval(DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES);

				// StringTable has to be called after NetworkSys started, or else first call StringTable::AddRef() yourself
				StringTable::Instance()->AddString("Popcorn",false);
			}
		}

		if(isStarted)
		{
			SocketMessage * msg;
            String temp4;
            String temp9;
			for (msg = mLink->recevie(); msg; N_delete msg, msg = mLink->recevie())
			{
				switch(msg->data[0])
				{
				case ID_CONNECTION_ATTEMPT_FAILED:
					ShowMessage("ID_CONNECTION_ATTEMPT_FAILED\n");
					break;
				case ID_NO_FREE_INCOMING_CONNECTIONS:
					ShowMessage("ID_NO_FREE_INCOMING_CONNECTIONS\n");
					break;
				case ID_CONNECTION_REQUEST_ACCEPTED:
					ShowMessage("ID_CONNECTION_REQUEST_ACCEPTED\n");
					break;
				case ID_NEW_INCOMING_CONNECTION:
                    msg->mAddress.read(temp4);
					ShowMessage(StrUtil::format(temp9, "ID_NEW_INCOMING_CONNECTION from %s\n", temp4.c_str()));
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					ShowMessage("ID_DISCONNECTION_NOTIFICATION\n");
					break;
				case ID_CONNECTION_LOST:
					ShowMessage("ID_CONNECTION_LOST\n");
					break;
				}
			}

			if(isServer)
			{
				// Restart the demo every RESTART_TIMER_MS milliseconds
				if (popcornLifetimeCountdown<=elapsedTimeMS)
				{
					Popcorn::ClearPopcorn();
					CreateKernels(&replicaManager3);
					popcornLifetimeCountdown=RESTART_TIMER_MS;
				}
				popcornLifetimeCountdown-=elapsedTimeMS;
			}

			unsigned i;
			for(i = 0; i < popcornList.Size(); ++i)
			{
				popcornList[i]->update(elapsedTimeMS);
			}
		}

		camera->moveRel(mTranslateVector);

		if(mKeyboard->isKeyDown(KC_ESCAPE) || mKeyboard->isKeyDown(KC_Q))
			quit = true;
	}

	// Just Ogre startup stuff
	virtual void PostConfigure(const char * defaultResourceConfigurationPath, bool recursive)
	{
		App3D::PostConfigure(defaultResourceConfigurationPath, false);
		App3D::InitSceneManager(0);
		App3D::InitGUIManager();
		App3D::InitCamera(0);
		App3D::InitViewport(0);

		ParamList pl;
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;

		window->getPlatformData("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(Npair(std::string("WINDOW"), windowHndStr.str()));

		mInputManager = InputManager::createInputSystem(pl);
		//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
		mKeyboard = static_cast<Keyboard *>(mInputManager->createInputObject(OISKeyboard, false ));

		// Start the overlay helper class, which handles fading of overlays and other stuff
		overlayHelper.Startup();

		sceneManager->setAmbient(Colour(.5, .5, .5));

		mainLight = sceneManager->createLight("MainLight");
		mainLightNode = sceneManager->getOriginNode()->create("MainLightNode" );
		mainLightNode->attach(mainLight);
		mainLight->setType(LT_POINT);
		mainLight->setPosition(200.0f, 200.0f, 200.0f);

		camera->setPosition(150.0f, 150.0f, 70.0f);
		camera->lookAt(Ogre::Vector3F(0.0f,50.0f,0.0f));
		camera->setNearClipDistance(1.0f);

		popcornKernel = sceneManager->createGeo("PopcornKernel", "PopcornKernel.mesh");
		popcornPopped = sceneManager->createGeo("PopcornPopped", "PopcornPopped.mesh");
		popcornLifetimeCountdown = 0;
        BoxSky * mBoxSky = N_new BoxSky(mSceneMgr);
        mBoxSky->setMaterial();
        mBoxSky->setRenderGroup(RQG_Sky);
		sceneManager->setSkyBox(true, "Examples/SteveCubeSkyBox");
		isStarted = false;
		enableInterpolation = true;

		// Bug: Since updating to ogre OgreSDK_vc8_v1-7-1 from Ogre 3D 1.6.2, the first call to ShowMessage doesn't show up anymore
		ShowMessage("'S'erver. 'C'lient. Hold ' ' to disable interp.");
		ShowMessage("'S'erver. 'C'lient. Hold ' ' to disable interp.");
	}

	virtual void CreateKernels(ReplicaManager3 * replicaManager3)
	{
		N_assert(isServer);
		unsigned int kernelCount;
		if(KERNELS_VARIANCE != 0)
			kernelCount = MIN_KERNELS + randomMT() % KERNELS_VARIANCE;
		else
			kernelCount = MIN_KERNELS;
		for (unsigned int i = 0; i < kernelCount; i++)
			Popcorn::CreateKernel(replicaManager3);
	}
protected:
	virtual char * GetWindowTitle(void) const {return (char *)"Popcorn popper";}
	void ShowMessage(const char * msg, NIIf timescale = 1.0f)
	{
		// Create a panel
		static int count = 0;
        String temp;
		OverlayView * panel = overlayHelper.CreatePanel(StrUtil::format(temp, "%i", count++));
		panel->setSizeMode(NII::SM_Abs);
		panel->setPosition(10, 10);
		panel->setSize(100, 100);
		//panel->setMaterial("MaterialName"); // Optional background material

		// Create a text area
		TextOverlayGeo * textArea = overlayHelper.CreateTextArea(StrUtil::format(temp, "%i", count++), "BlueHighway", panel);
		textArea->setSizeMode(NII::SM_Abs);
		textArea->setPosition(10, 10);
		textArea->setSize(200, 200);
		textArea->setCaption(msg);
		textArea->setGlyphHeight(32);
		textArea->setColour(Colour(0.5, 0.7, 0.5));

		// Destroy the children (the text area) before destroying the parents.
		overlayHelper.FadeOverlayGeo(textArea, 3000*timescale, 1000*timescale, 0.0f, true);
		overlayHelper.FadeOverlayGeo(panel, 3000*timescale, 1000*timescale, 0.0f, true);
	}

	// Our major systems.  Note the base class ExampleApplication has all the Ogre 3D systems
	OverlayHelper overlayHelper;
	bool quit;

	SpaceNode *mainLightNode;
	Light *mainLight;

	OIS::InputManager* mInputManager;
	OIS::Keyboard* mKeyboard;

	NetworkIDManager networkIdManager;
	bool isStarted;
	TimeDurMS popcornLifetimeCountdown;
};


#ifdef WIN32
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main (int argc, char** argv)
#endif
{

	HWND     hWnd;
	TimeDurMS curTime, lastTime, elapsed;
	app = new ExampleApp;
	app->PreConfigure();
	if (app->Configure()==false)
	{
		delete app;
		return 0;
	}
#ifdef WIN32
	app->window->getPlatformData("HWND", &hWnd);
	MSG msg;
#else
	app->window->getPlatformData("GLXWINDOW", &hWnd);
#endif

	app->PostConfigure("resources.cfg",false);
	lastTime = N_Engine().getTimer().getMS();

	while(app->ShouldQuit()==false)
	{
		curTime = N_Engine().getTimer().getMS();
		elapsed = curTime-lastTime;
		if (elapsed > 100)
			elapsed=100; // Spike limiter
		app->update(curTime, elapsed);
		lastTime=curTime;
		app->Render(curTime);
#ifdef WIN32
		if (PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE )>0)
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
#endif
		// Make sure the RakNet thread runs
		Nsleep(0);
	}

	app->OnAppShutdown();

	delete app;

	return 0;
}
