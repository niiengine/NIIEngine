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
#include "RakNetStuff.h"

#include "NetworkIDManager.h"
#include "CDemo.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    LinkBase * mLink;
    NetworkIDManager * networkIDManager;
    ReplicaManager3Irrlicht * replicaManager3;
    RouterMapClient * natPunchthroughClient;
    CloudClient * cloudClient;
    FullyConnected * fullyConnectedMesh2;
    PlayerReplica * playerReplica;
    //------------------------------------------------------------------------
    DataStructures::List<PlayerReplica *> PlayerReplica::playerList;
    //------------------------------------------------------------------------
    // Take this many milliseconds to move the visible position to the real position
    static const NIIf INTERP_TIME_MS = 100.0f;
    //------------------------------------------------------------------------
    void InstantiateRakNetClasses(void)
    {
        static const int MAX_PLAYERS = 32;
        static const unsigned short TCP_PORT = 0;
        static const TimeDurMS UDP_SLEEP_TIMER = 30;

        // Basis of all UDP communications
        mLink = LinkBase::GetInstance();
        // Using fixed port so we can use AdvertiseSystem and connect on the LAN if the server is not available.
        HostPeer sd(1234, 0);
        sd.socketFamily = AF_INET; // Only IPV4 supports broadcast on 255.255.255.255
        while(BSDSocketParam::IsPortInUse(sd.port, sd.hostAddress, sd.socketFamily, SOCK_DGRAM) == true)
            sd.port++;
        // +1 is for the connection to the NAT punchthrough server
        StartupResult sr = mLink->Startup(MAX_PLAYERS + 1, &sd, 1);
        N_assert(sr == RAKNET_STARTED);
        mLink->SetMaximumIncomingConnections(MAX_PLAYERS);
        // Fast disconnect for easier testing of host migration
        mLink->setTimeOut(0, 5000);
        // ReplicaManager3 replies on NetworkIDManager. It assigns numbers to objects so they can be looked up over the network
        // It's a class in case you wanted to have multiple worlds, then you could have multiple instances of NetworkIDManager
        networkIDManager=new NetworkIDManager;
        // Automatically sends around new / deleted / changed game objects
        replicaManager3=new ReplicaManager3Irrlicht;
        replicaManager3->SetNetworkIDManager(networkIDManager);
        mLink->AttachPlugin(replicaManager3);
        // Automatically destroy connections, but don't create them so we have more control over when a system is considered ready to play
        replicaManager3->SetAutoManageConnections(false,true);
        // Create and register the network object that represents the player
        playerReplica = new PlayerReplica;
        replicaManager3->Reference(playerReplica);
        // Lets you connect through routers
        natPunchthroughClient=new RouterMapClient;
        mLink->AttachPlugin(natPunchthroughClient);
        // Uploads game instance, basically client half of a directory server
        // Server code is in NATCompleteServer sample
        cloudClient=new CloudClient;
        mLink->AttachPlugin(cloudClient);
        fullyConnectedMesh2=new FullyConnected;
        fullyConnectedMesh2->SetAutoparticipateConnections(false);
        fullyConnectedMesh2->SetConnectOnNewRemoteConnection(false, "");
        mLink->AttachPlugin(fullyConnectedMesh2);
        // Connect to the NAT punchthrough server
        ConnectResultType car = mLink->Connect(DEFAULT_NAT_PUNCHTHROUGH_FACILITATOR_IP, DEFAULT_NAT_PUNCHTHROUGH_FACILITATOR_PORT,0,0);
        N_assert(car==CONNECTION_ATTEMPT_STARTED);
    }
    //------------------------------------------------------------------------
    void DeinitializeRakNetClasses(void)
    {
        // Shutdown so the server knows we stopped
        mLink->Shutdown(100,0);
        LinkBase::DestroyInstance(mLink);
        delete networkIDManager;
        delete replicaManager3;
        delete natPunchthroughClient;
        delete cloudClient;
        delete fullyConnectedMesh2;
    }
    //------------------------------------------------------------------------
    BaseIrrlichtReplica::BaseIrrlichtReplica()
    {
    }
    //------------------------------------------------------------------------
    BaseIrrlichtReplica::~BaseIrrlichtReplica()
    {

    }
    //------------------------------------------------------------------------
    void BaseIrrlichtReplica::SerializeConstruction(NetSerializer * constructionBitstream,
        Connection_RM3 *destinationConnection)
    {
        constructionBitstream->write(position);
    }
    //------------------------------------------------------------------------
    bool BaseIrrlichtReplica::DeserializeConstruction(NetSerializer * constructionBitstream,
        Connection_RM3 *sourceConnection)
    {
        constructionBitstream->read(position);
        return true;
    }
    //------------------------------------------------------------------------
    RM3SerializationResult BaseIrrlichtReplica::read(SerializeParameters * serializeParameters)
    {
        return RM3SR_BROADCAST_IDENTICALLY;
    }
    //------------------------------------------------------------------------
    void BaseIrrlichtReplica::write(DeserializeParameters *deserializeParameters)
    {
    }
    //------------------------------------------------------------------------
    void BaseIrrlichtReplica::update(TimeDurMS curTime)
    {
    }
    //------------------------------------------------------------------------
    PlayerReplica::PlayerReplica()
    {
        model = 0;
        rotationDeltaPerMS = 0.0f;
        isMoving = false;
        deathTimeout = 0;
        lastUpdate = N_Engine().getTimer().getMS();
        playerList.Push(this);
    }
    //------------------------------------------------------------------------
    PlayerReplica::~PlayerReplica()
    {
        unsigned int index = playerList.GetIndexOf(this);
        if (index != (unsigned int) -1)
            playerList.RemoveAtIndexFast(index);
    }
    //------------------------------------------------------------------------
    void PlayerReplica::WriteAllocationID(Connection_RM3 * destinationConnection,
        NetSerializer * allocationIdBitstream) const
    {
        allocationIdBitstream->write(RakString("PlayerReplica"));
    }
    //------------------------------------------------------------------------
    void PlayerReplica::SerializeConstruction(NetSerializer *constructionBitstream,
        Connection_RM3 * destinationConnection)
    {
        BaseIrrlichtReplica::SerializeConstruction(constructionBitstream, destinationConnection);
        constructionBitstream->write(rotationAroundYAxis);
        constructionBitstream->write(playerName);
        constructionBitstream->write(IsDead());
    }
    //------------------------------------------------------------------------
    bool PlayerReplica::DeserializeConstruction(NetSerializer *constructionBitstream,
        Connection_RM3 * sourceConnection)
    {
        if (!BaseIrrlichtReplica::DeserializeConstruction(constructionBitstream, sourceConnection))
            return false;
        constructionBitstream->read(rotationAroundYAxis);
        constructionBitstream->read(playerName);
        constructionBitstream->read(isDead);
        return true;
    }
    //------------------------------------------------------------------------
    void PlayerReplica::PostDeserializeConstruction(NetSerializer * constructionBitstream,
        Connection_RM3 * destinationConnection)
    {
        // Object was remotely created and all data loaded. Now we can make the object visible
        scene::IAnimatedMesh* mesh = 0;
        scene::ISceneManager *sm = demo->GetSceneManager();
        mesh = sm->getMesh(IRRLICHT_MEDIA_PATH "sydney.md2");
        model = sm->addAnimatedMeshSceneNode(mesh, 0);

        model->setPosition(position);
        model->setRotation(core::vector3df(0, rotationAroundYAxis, 0));
        model->setScale(core::vector3df(2,2,2));
        model->setMD2Animation(scene::EMAT_STAND);
        curAnim=scene::EMAT_STAND;
        model->setMaterialTexture(0, demo->GetDevice()->getVideoDriver()->getTexture(IRRLICHT_MEDIA_PATH "sydney.bmp"));
        model->setMaterialFlag(video::EMF_LIGHTING, true);
        model->addShadowVolumeSceneNode();
        model->setAutomaticCulling ( scene::EAC_BOX );
        model->setVisible(true);
        model->setAnimationEndCallback(this);
        wchar_t playerNameWChar[1024];
        mbstowcs(playerNameWChar, playerName.C_String(), 1024);
        scene::IBillboardSceneNode *bb = sm->addBillboardTextSceneNode(0, playerNameWChar, model);
        bb->setSize(core::dimension2df(40,20));
        bb->setPosition(core::vector3df(0,model->getBoundingBox().MaxEdge.Y+bb->getBoundingBox().MaxEdge.Y-bb->getBoundingBox().MinEdge.Y+5.0,0));
        bb->setColor(video::SColor(255,255,128,128), video::SColor(255,255,128,128));
    }
    //------------------------------------------------------------------------
    void PlayerReplica::PreDestruction(Connection_RM3 *sourceConnection)
    {
        if (model)
            model->remove();
    }
    //------------------------------------------------------------------------
    RM3SerializationResult PlayerReplica::read(SerializeParameters *serializeParameters)
    {
        BaseIrrlichtReplica::read(serializeParameters);
        serializeParameters->outputBitstream[0].write(position);
        serializeParameters->outputBitstream[0].write(rotationAroundYAxis);
        serializeParameters->outputBitstream[0].write(isMoving);
        serializeParameters->outputBitstream[0].write(IsDead());
        return RM3SR_BROADCAST_IDENTICALLY;
    }
    //------------------------------------------------------------------------
    void PlayerReplica::write(DeserializeParameters *deserializeParameters)
    {
        BaseIrrlichtReplica::write(deserializeParameters);
        deserializeParameters->serializationBitstream[0].read(position);
        deserializeParameters->serializationBitstream[0].read(rotationAroundYAxis);
        deserializeParameters->serializationBitstream[0].read(isMoving);
        bool wasDead=isDead;
        deserializeParameters->serializationBitstream[0].read(isDead);
        if (isDead==true && wasDead==false)
        {
            demo->PlayDeathSound(position);
        }

        core::vector3df positionOffset;
        positionOffset=position-model->getPosition();
        positionDeltaPerMS = positionOffset / INTERP_TIME_MS;
        NIIf rotationOffset;
        rotationOffset=GetRotationDifference(rotationAroundYAxis,model->getRotation().Y);
        rotationDeltaPerMS = rotationOffset / INTERP_TIME_MS;
        interpEndTime = N_Engine().getTimer().getMS() + (TimeDurMS) INTERP_TIME_MS;
    }
    //------------------------------------------------------------------------
    void PlayerReplica::update(TimeDurMS curTime)
    {
        // Is a locally created object?
        if (creatingSystemGUID == mLink->getGUID((Address *)0))
        {
            // Local player has no mesh to interpolate
            // Input our camera position as our player position
            playerReplica->position = demo->GetSceneManager()->getActiveCamera()->getPosition()-irr::core::vector3df(0,CAMERA_HEIGHT,0);
            playerReplica->rotationAroundYAxis = demo->GetSceneManager()->getActiveCamera()->getRotation().Y-90.0f;
            isMoving = demo->IsMovementKeyDown();
            return;
        }

        // update interpolation
        TimeDurMS elapsed = curTime-lastUpdate;
        if (elapsed <= 1)
            return;
        if (elapsed > 100)
            elapsed = 100;

        lastUpdate = curTime;
        irr::core::vector3df curPositionDelta = position-model->getPosition();
        irr::core::vector3df interpThisTick = positionDeltaPerMS*(NIIf) elapsed;
        if (curTime < interpEndTime && interpThisTick.getLengthSQ() < curPositionDelta.getLengthSQ())
        {
            model->setPosition(model->getPosition()+positionDeltaPerMS*(NIIf) elapsed);
        }
        else
        {
            model->setPosition(position);
        }

        NIIf curRotationDelta = GetRotationDifference(rotationAroundYAxis,model->getRotation().Y);
        NIIf interpThisTickRotation = rotationDeltaPerMS*(NIIf)elapsed;
        if (curTime < interpEndTime && fabs(interpThisTickRotation) < fabs(curRotationDelta))
        {
            model->setRotation(model->getRotation()+core::vector3df(0,interpThisTickRotation,0));
        }
        else
        {
            model->setRotation(core::vector3df(0,rotationAroundYAxis,0));
        }

        if (isDead)
        {
            UpdateAnimation(scene::EMAT_DEATH_FALLBACK);
            model->setLoopMode(false);
        }
        else if (curAnim!=scene::EMAT_ATTACK)
        {
            if (isMoving)
            {
                UpdateAnimation(scene::EMAT_RUN);
                model->setLoopMode(true);
            }
            else
            {
                UpdateAnimation(scene::EMAT_STAND);
                model->setLoopMode(true);
            }
        }
    }
    //------------------------------------------------------------------------
    void PlayerReplica::UpdateAnimation(irr::scene::EMD2_ANIMATION_TYPE anim)
    {
        if (anim!=curAnim)
            model->setMD2Animation(anim);
        curAnim=anim;
    }
    //------------------------------------------------------------------------
    NIIf PlayerReplica::GetRotationDifference(NIIf r1, NIIf r2)
    {
        NIIf diff = r1-r2;
        while (diff>180.0f)
            diff-=360.0f;
        while (diff<-180.0f)
            diff+=360.0f;
        return diff;
    }
    //------------------------------------------------------------------------
    void PlayerReplica::OnAnimationEnd(scene::IAnimatedMeshSceneNode* node)
    {
        if (curAnim==scene::EMAT_ATTACK)
        {
            if (isMoving)
            {
                UpdateAnimation(scene::EMAT_RUN);
                model->setLoopMode(true);
            }
            else
            {
                UpdateAnimation(scene::EMAT_STAND);
                model->setLoopMode(true);
            }
        }
    }
    //------------------------------------------------------------------------
    void PlayerReplica::PlayAttackAnimation(void)
    {
        if (isDead==false)
        {
            UpdateAnimation(scene::EMAT_ATTACK);
            model->setLoopMode(false);
        }
    }
    //------------------------------------------------------------------------
    bool PlayerReplica::IsDead(void) const
    {
        return deathTimeout > N_Engine().getTimer().getMS();
    }
    //------------------------------------------------------------------------
    BallReplica::BallReplica()
    {
        creationTime = N_Engine().getTimer().getMS();
    }
    //------------------------------------------------------------------------
    BallReplica::~BallReplica()
    {
    }
    //------------------------------------------------------------------------
    void BallReplica::WriteAllocationID(Connection_RM3 * destinationConnection,
        NetSerializer * allocationIdBitstream) const
    {
        allocationIdBitstream->write(RakString("BallReplica"));
    }
    //------------------------------------------------------------------------
    void BallReplica::SerializeConstruction(NetSerializer *constructionBitstream,
        Connection_RM3 * destinationConnection)
    {
        BaseIrrlichtReplica::SerializeConstruction(constructionBitstream, destinationConnection);
        constructionBitstream->write(shotDirection);
    }
    //------------------------------------------------------------------------
    bool BallReplica::DeserializeConstruction(NetSerializer * constructionBitstream,
        Connection_RM3 * sourceConnection)
    {
        if (!BaseIrrlichtReplica::DeserializeConstruction(constructionBitstream, sourceConnection))
            return false;
        constructionBitstream->read(shotDirection);
        return true;
    }
    //------------------------------------------------------------------------
    void BallReplica::PostDeserializeConstruction(NetSerializer * constructionBitstream,
        Connection_RM3 * destinationConnection)
    {
        // Shot visible effect and BallReplica classes are not linked, but they update the same way, such that
        // they are in the same spot all the time
        demo->shootFromOrigin(position, shotDirection);

        // Find the owner of this ball, and make them play the attack animation
        unsigned int idx;
        for (idx=0; idx < PlayerReplica::playerList.Size(); idx++)
        {
            if (PlayerReplica::playerList[idx]->creatingSystemGUID==creatingSystemGUID)
            {
                PlayerReplica::playerList[idx]->PlayAttackAnimation();
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void BallReplica::PreDestruction(Connection_RM3 * sourceConnection)
    {
        // The system that shot this ball destroyed it, or disconnected
        // Technically we should clear out the node visible effect too, but it's not important for now
    }
    //------------------------------------------------------------------------
    RM3SerializationResult BallReplica::read(SerializeParameters * serializeParameters)
    {
        BaseIrrlichtReplica::read(serializeParameters);
        return RM3SR_BROADCAST_IDENTICALLY;
    }
    //------------------------------------------------------------------------
    void BallReplica::write(DeserializeParameters *deserializeParameters)
    {
        BaseIrrlichtReplica::write(deserializeParameters);
    }
    //------------------------------------------------------------------------
    void BallReplica::update(TimeDurMS curTime)
    {
        // Is a locally created object?
        if(creatingSystemGUID == mLink->getGUID((Address *)0))
        {
            // Destroy if shot expired
            if(curTime > shotLifetime)
            {
                // Destroy on network
                BroadcastDestruction();
                delete this;
                return;
            }
        }

        // Keep at the same position as the visible effect
        // Deterministic, so no need to actually transmit position
        // The variable position is the origin that the ball was created at. For the player, it is their actual position
        TimeDurMS elapsedTime;
        // Due to ping variances and timestamp miscalculations, it's possible with very low pings to get a slightly negative time, so we have to check
        if (curTime >= creationTime)
            elapsedTime = curTime - creationTime;
        else
            elapsedTime = 0;
        irr::core::vector3df updatedPosition = position + shotDirection * (NIIf) elapsedTime * SHOT_SPEED;

        // See if the bullet hit us
        if(creatingSystemGUID != mLink->getGUID((Address *)0))
        {
            if(playerReplica->IsDead() == false)
            {
                NIIf playerHalfHeight = demo->GetSyndeyBoundingBox().getExtent().Y / 2;
                irr::core::vector3df positionRelativeToCharacter = updatedPosition-playerReplica->position;//+core::vector3df(0,playerHalfHeight,0);
                if(demo->GetSyndeyBoundingBox().isPointInside(positionRelativeToCharacter))
                //if ((playerReplica->position+core::vector3df(0,playerHalfHeight,0)-updatedPosition).getLengthSQ() < BALL_DIAMETER*BALL_DIAMETER/4.0f)
                {
                    // We're dead for 3 seconds
                    playerReplica->deathTimeout=curTime+3000;
                }
            }
        }
    }
    //------------------------------------------------------------------------
    Replica3 * Connection_RM3Irrlicht::AllocReplica(NetSerializer * allocationId,
        ReplicaManager3 * replicaManager3)
    {
        RakString typeName; allocationId->read(typeName);
        if (typeName=="PlayerReplica")
        {
            BaseIrrlichtReplica *r = new PlayerReplica;
            r->demo=demo;
            return r;
        }
        if (typeName=="BallReplica")
        {
            BaseIrrlichtReplica *r = new BallReplica;
            r->demo=demo;
            return r;
        }
        return 0;
    }
    //------------------------------------------------------------------------
}
}