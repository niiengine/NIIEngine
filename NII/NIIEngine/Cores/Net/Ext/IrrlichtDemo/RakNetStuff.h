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

#ifndef __RAKNET_ADDITIONS_FOR_IRRLICHT_DEMO_H
#define __RAKNET_ADDITIONS_FOR_IRRLICHT_DEMO_H

#include "NiiNetLinkBase.h"
#include "ReplicaManager3.h"
#include "NiiNetCommon.h"
#include "NiiNetRouterMapClient.h"
#include "CloudClient.h"
#include "NiiNetFullyConnected.h"
#include "UDPProxyClient.h"
#include "NetworkObj.h"
#include "HTTPConnection.h"
#include "../Samples/PHPDirectoryServer2/PHPDirectoryServer2.h"
#include "vector3d.h"
#include "IAnimatedMeshSceneNode.h"

class ReplicaManager3Irrlicht;
class CDemo;
class PlayerReplica;

// All externs defined in the corresponding CPP file
// Most of these classes has a manual entry, all of them have a demo
extern LinkBase *mLink; // Basic communication
extern NetworkIDManager *networkIDManager; // Unique IDs per network object
extern ReplicaManager3Irrlicht *replicaManager3; // Autoreplicate network objects
extern RouterMapClient *natPunchthroughClient; // Connect peer to peer through routers
extern CloudClient *cloudClient; // Used to upload game instance to the cloud
extern FullyConnected *fullyConnectedMesh2; // Used to find out who is the session host
extern PlayerReplica *playerReplica; // Network object that represents the player

// A NAT punchthrough and proxy server Jenkins Software is hosting for free, should usually be online
#define DEFAULT_NAT_PUNCHTHROUGH_FACILITATOR_PORT 61111
#define DEFAULT_NAT_PUNCHTHROUGH_FACILITATOR_IP "natpunch.jenkinssoftware.com"

void InstantiateRakNetClasses(void);
void DeinitializeRakNetClasses(void);

// Base RakNet custom classes for Replica Manager 3, setup peer to peer networking
class BaseIrrlichtReplica : public Replica3
{
public:
	BaseIrrlichtReplica();
	virtual ~BaseIrrlichtReplica();
	virtual RM3ConstructionState QueryConstruction(Connection_RM3 *destinationConnection, ReplicaManager3 *replicaManager3) {return QueryConstruction_PeerToPeer(destinationConnection);}
	virtual bool QueryRemoteConstruction(Connection_RM3 *sourceConnection) {return QueryRemoteConstruction_PeerToPeer(sourceConnection);}
	virtual void DeallocReplica(Connection_RM3 *sourceConnection) {delete this;}
	virtual RM3QuerySerializationResult QuerySerialization(Connection_RM3 *destinationConnection) {return QuerySerialization_PeerToPeer(destinationConnection);}
	virtual void SerializeConstruction(NetSerializer *constructionBitstream, Connection_RM3 *destinationConnection);
	virtual bool DeserializeConstruction(NetSerializer *constructionBitstream, Connection_RM3 *sourceConnection);
	virtual RM3SerializationResult read(SerializeParameters *serializeParameters);
	virtual void write(DeserializeParameters *deserializeParameters);
	virtual void SerializeDestruction(NetSerializer *destructionBitstream, Connection_RM3 *destinationConnection) {}
	virtual bool DeserializeDestruction(NetSerializer *destructionBitstream, Connection_RM3 *sourceConnection) {return true;}
	virtual RM3ActionOnPopConnection QueryActionOnPopConnection(Connection_RM3 *droppedConnection) const {return QueryActionOnPopConnection_PeerToPeer(droppedConnection);}

	/// This function is not derived from Replica3, it's specific to this app
	/// Called from CDemo::UpdateRakNet
	virtual void update(TimeDurMS curTime);

	// Set when the object is constructed
	CDemo * demo;

	// real is written on the owner peer, read on the remote peer
	irr::core::vector3df position;
	TimeDurMS creationTime;
};
// Game classes automatically updated by ReplicaManager3
class PlayerReplica : public BaseIrrlichtReplica, irr::scene::IAnimationEndCallBack
{
public:
	PlayerReplica();
	virtual ~PlayerReplica();
	// Every function below, before update overriding a function in Replica3
	virtual void WriteAllocationID(Connection_RM3 *destinationConnection, NetSerializer *allocationIdBitstream) const;
	virtual void SerializeConstruction(NetSerializer *constructionBitstream, Connection_RM3 *destinationConnection);
	virtual bool DeserializeConstruction(NetSerializer *constructionBitstream, Connection_RM3 *sourceConnection);
	virtual RM3SerializationResult read(SerializeParameters *serializeParameters);
	virtual void write(DeserializeParameters *deserializeParameters);
	virtual void PostDeserializeConstruction(NetSerializer *constructionBitstream, Connection_RM3 *destinationConnection);
	virtual void PreDestruction(Connection_RM3 *sourceConnection);

	virtual void update(TimeDurMS curTime);
	void UpdateAnimation(irr::scene::EMD2_ANIMATION_TYPE anim);
	NIIf GetRotationDifference(NIIf r1, NIIf r2);
	virtual void OnAnimationEnd(irr::scene::IAnimatedMeshSceneNode* node);
	void PlayAttackAnimation(void);

	// playerName is only sent in SerializeConstruction, since it doesn't change
	RakString playerName;

	// Networked rotation
	NIIf rotationAroundYAxis;
	// Interpolation variables, not networked
	irr::core::vector3df positionDeltaPerMS;
	NIIf rotationDeltaPerMS;
	TimeDurMS interpEndTime, lastUpdate;

	// Updated based on the keypresses, to control remote animation
	bool isMoving;

	// Only instantiated for remote systems, you never see yourself
	irr::scene::IAnimatedMeshSceneNode* model;
	irr::scene::EMD2_ANIMATION_TYPE curAnim;

	// deathTimeout is set from the local player
	TimeDurMS deathTimeout;
	bool IsDead(void) const;
	// isDead is set from network packets for remote players
	bool isDead;

	// List of all players, including our own
	static DataStructures::List<PlayerReplica*> playerList;
};
class BallReplica : public BaseIrrlichtReplica
{
public:
	BallReplica();
	virtual ~BallReplica();
	// Every function except update is overriding a function in Replica3
	virtual void WriteAllocationID(Connection_RM3 *destinationConnection, NetSerializer *allocationIdBitstream) const;
	virtual void SerializeConstruction(NetSerializer *constructionBitstream, Connection_RM3 *destinationConnection);
	virtual bool DeserializeConstruction(NetSerializer *constructionBitstream, Connection_RM3 *sourceConnection);
	virtual RM3SerializationResult read(SerializeParameters *serializeParameters);
	virtual void write(DeserializeParameters *deserializeParameters);
	virtual void PostDeserializeConstruction(NetSerializer *constructionBitstream, Connection_RM3 *destinationConnection);
	virtual void PreDestruction(Connection_RM3 *sourceConnection);

	virtual void update(TimeDurMS curTime);

	// shotDirection is networked
	irr::core::vector3df shotDirection;

	// shotlifetime is calculated, not networked
	TimeDurMS shotLifetime;
};
class Connection_RM3Irrlicht : public Connection_RM3
{
public:
	Connection_RM3Irrlicht(const Address &address, UniqueID id, CDemo *_demo) : Connection_RM3(address, id)
    {
        demo=_demo;
    }
	virtual ~Connection_RM3Irrlicht() {}

	virtual Replica3 *AllocReplica(NetSerializer *allocationId, ReplicaManager3 *replicaManager3);
protected:
	CDemo *demo;
};

class ReplicaManager3Irrlicht : public ReplicaManager3
{
public:
	virtual Connection_RM3 * AllocConnection(const Address &address, UniqueID id) const
    {
		return new Connection_RM3Irrlicht(address, id, demo);
	}
	virtual void DeallocConnection(Connection_RM3 *connection) const
    {
		delete connection;
	}
	CDemo *demo;
};


#endif
