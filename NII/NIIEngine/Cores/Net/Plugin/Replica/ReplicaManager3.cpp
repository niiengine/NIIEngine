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
#include "NativeFeatureIncludes.h"
#if _RAKNET_SUPPORT_ReplicaManager3==1

#include "ReplicaManager3.h"
#include "NiiNetCommon.h"
#include "NiiNetLinkBase.h"
#include "NetworkIDManager.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    bool PRO::operator==(const PRO & right) const
    {
        return mPacketLevel == right.mPacketLevel &&
            mPacketType == right.mPacketType && mPacketCh == right.mPacketCh &&
                sendReceipt == right.sendReceipt;
    }
    //------------------------------------------------------------------------
    bool PRO::operator!=(const PRO & right) const
    {
        return mPacketLevel != right.mPacketLevel ||
            mPacketType != right.mPacketType || mPacketCh != right.mPacketCh ||
                sendReceipt != right.sendReceipt;
    }
    //------------------------------------------------------------------------
    int Connection_RM3::Replica3LSRComp(Replica3 * const & replica3,
        LastSerializationResult * const & data)
    {

        // 7/28/2013 - If GetNetworkID chagned during runtime, the list would be out of order and lookup would always fail or go out of bounds
        // I remember before that I could not directly compare
        if (replica3->referenceIndex < data->replica->referenceIndex)
            return -1;
        if (replica3->referenceIndex > data->replica->referenceIndex)
            return 1;
        return 0;
    }
    //------------------------------------------------------------------------
    LastSerializationResult::LastSerializationResult()
    {
        replica = 0;
        lastSerializationResultBS = 0;
        whenLastSerialized = N_Engine().getTimer().getMS();
    }
    //------------------------------------------------------------------------
    LastSerializationResult::~LastSerializationResult()
    {
        if (lastSerializationResultBS)
            N_delete lastSerializationResultBS;
    }
    //------------------------------------------------------------------------
    void LastSerializationResult::AllocBS(void)
    {
        if (lastSerializationResultBS == 0)
        {
            lastSerializationResultBS = N_new LastSerializationResultBS();
        }
    }
    //------------------------------------------------------------------------
    ReplicaManager3::ReplicaManager3()
    {
        defaultSendParameters.mPacketCh = 0;
        defaultSendParameters.mPacketLevel = PL_HIGH;
        defaultSendParameters.mPacketType = PT_ORDER_MAKE_SURE;
        defaultSendParameters.sendReceipt = 0;
        autoSerializeInterval = 30;
        lastAutoSerializeOccurance = 0;
        autoCreateConnections = true;
        autoDestroyConnections = true;
        currentlyDeallocatingReplica = 0;

        for (Nui32 i=0; i < 255; ++i)
            worldsArray[i] = 0;

        AddWorld(0);
    }
    //------------------------------------------------------------------------
    ReplicaManager3::~ReplicaManager3()
    {
        if (autoDestroyConnections)
        {
            for (NCount i = 0; i < worldsList.Size(); ++i)
            {
                N_assert(worldsList[i]->connectionList.Size()==0);
            }
        }
        Clear(true);
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::SetAutoManageConnections(bool autoCreate, bool autoDestroy)
    {
        autoCreateConnections=autoCreate;
        autoDestroyConnections=autoDestroy;
    }
    //------------------------------------------------------------------------
    bool ReplicaManager3::GetAutoCreateConnections(void) const
    {
        return autoCreateConnections;
    }
    //------------------------------------------------------------------------
    bool ReplicaManager3::GetAutoDestroyConnections(void) const
    {
        return autoDestroyConnections;
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::AutoCreateConnectionList(
        DataStructures::List<UniqueID> &participantListIn,
        DataStructures::List<Connection_RM3*> &participantListOut,
        WorldId gid)
    {
        for(NCount index = 0; index < participantListIn.Size(); ++index)
        {
            if(GetConnectionByGUID(participantListIn[index], gid) == false)
            {
                Connection_RM3 * connection = AllocConnection(mPrc2->getAddress(participantListIn[index]), participantListIn[index]);
                if(connection)
                {
                    PushConnection(connection);
                    participantListOut.Push(connection);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    bool ReplicaManager3::PushConnection(Connection_RM3 * newConnection,
        WorldId gid)
    {
        if (newConnection == 0)
            return false;
        if (GetConnectionByGUID(newConnection->GetRakNetGUID(), gid))
            return false;
        // Was this intended?
        N_assert(newConnection->GetRakNetGUID() != mPrc2->getGuid());

        N_assert(worldsArray[gid] != 0 && "World not in use");
        RM3World * world = worldsArray[gid];

        Nui32 index = world->connectionList.GetIndexOf(newConnection);
        if (index==(Nui32)-1)
        {
            world->connectionList.Push(newConnection);

            // send message to validate the connection
            newConnection->SendValidation(mPrc2, gid);

            Connection_RM3::ConstructionMode constructionMode = newConnection->QueryConstructionMode();
            if (constructionMode==Connection_RM3::QUERY_REPLICA_FOR_CONSTRUCTION || constructionMode==Connection_RM3::QUERY_REPLICA_FOR_CONSTRUCTION_AND_DESTRUCTION)
            {
                Nui32 pushIdx;
                for (pushIdx=0; pushIdx < world->userReplicaList.Size(); pushIdx++)
                    newConnection->OnLocalReference(world->userReplicaList[pushIdx], this);
            }
        }
        return true;
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::DeallocReplicaNoBroadcastDestruction(Connection_RM3 * connection, Replica3 *replica3)
    {
        currentlyDeallocatingReplica=replica3;
        replica3->DeallocReplica(connection);
        currentlyDeallocatingReplica=0;
    }
    //------------------------------------------------------------------------
    Connection_RM3 * ReplicaManager3::PopConnection(Nui32 index, WorldId gid)
    {
        DataStructures::List<Replica3 *> replicaList;
        DataStructures::List<NetworkID> destructionList;
        DataStructures::List<Replica3 *> broadcastList;
        Connection_RM3 * connection;
        Nui32 index2;
        RM3ActionOnPopConnection action;

        N_assert(worldsArray[gid]!=0 && "World not in use");
        RM3World *world = worldsArray[gid];

        connection=world->connectionList[index];

        // Clear out downloadGroup
        connection->ClearDownloadGroup(mPrc2);

        UniqueID id = connection->GetRakNetGUID();
        // This might be wrong, I am relying on the variable creatingSystemGuid which is transmitted
        // automatically from the first system to reference the object. However, if an object changes
        // owners then it is not going to be returned here, and therefore QueryActionOnPopConnection()
        // will not be called for the new owner.
        GetReplicasCreatedByGuid(id, replicaList);

        for (index2 = 0; index2 < replicaList.Size(); ++index2)
        {
            action = replicaList[index2]->QueryActionOnPopConnection(connection);
            replicaList[index2]->OnPoppedConnection(connection);
            if (action==RM3AOPC_DELETE_REPLICA)
            {
                if (replicaList[index2]->GetNetworkIDManager())
                    destructionList.Push( replicaList[index2]->GetNetworkID());
            }
            else if (action==RM3AOPC_DELETE_REPLICA_AND_BROADCAST_DESTRUCTION)
            {
                if (replicaList[index2]->GetNetworkIDManager())
                    destructionList.Push(replicaList[index2]->GetNetworkID());

                broadcastList.Push(replicaList[index2]);
            }
            else if (action==RM3AOPC_DO_NOTHING)
            {
                for (Nui32 index3 = 0; index3 < connection->queryToSerializeReplicaList.Size(); index3++)
                {
                    LastSerializationResult * lsr = connection->queryToSerializeReplicaList[index3];
                    lsr->whenLastSerialized = 0;
                    if(lsr->lastSerializationResultBS)
                    {
                        for(int z = 0; z < RM3_NUM_OUTPUT_BITSTREAM_CHANNELS; ++z)
                        {
                            lsr->lastSerializationResultBS->bitStream[z].seekRead(0);
                            lsr->lastSerializationResultBS->bitStream[z].seekWrite(0);
                        }
                    }
                }
            }
        }
        BroadcastDestructionList(broadcastList, connection->GetSystemAddress());
        for (index2=0; index2 < destructionList.Size(); index2++)
        {
            // Do lookup in case DeallocReplica destroyed one of of the later Replica3 instances in the list
            Replica3* replicaToDestroy = world->networkIDManager->GET_OBJECT_FROM_ID<Replica3*>(destructionList[index2]);
            if (replicaToDestroy)
            {
                replicaToDestroy->PreDestruction(connection);
                replicaToDestroy->DeallocReplica(connection);
            }
        }

        world->connectionList.RemoveAtIndex(index);
        return connection;
    }
    //------------------------------------------------------------------------
    Connection_RM3 * ReplicaManager3::PopConnection(UniqueID id, WorldId gid)
    {
        NCount index;

        N_assert(worldsArray[gid]!=0 && "World not in use");
        RM3World *world = worldsArray[gid];

        for (index = 0; index < world->connectionList.Size(); ++index)
        {
            if (world->connectionList[index]->GetRakNetGUID() == id)
            {
                return PopConnection(index, gid);
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::Reference(Replica3 * replica3, WorldId gid)
    {
        N_assert(worldsArray[gid]!=0 && "World not in use");
        RM3World * world = worldsArray[gid];

        Nui32 index = ReferenceInternal(replica3, gid);

        if (index != (Nui32)-1)
        {
            NCount pushIdx;
            for (pushIdx = 0; pushIdx < world->connectionList.Size(); ++pushIdx)
            {
                Connection_RM3::ConstructionMode constructionMode = world->connectionList[pushIdx]->QueryConstructionMode();
                if(constructionMode == Connection_RM3::QUERY_REPLICA_FOR_CONSTRUCTION ||
                    constructionMode == Connection_RM3::QUERY_REPLICA_FOR_CONSTRUCTION_AND_DESTRUCTION)
                {
                    world->connectionList[pushIdx]->OnLocalReference(replica3, this);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    Nui32 ReplicaManager3::ReferenceInternal(Replica3 * replica3, WorldId gid)
    {
        N_assert(worldsArray[gid] != 0 && "World not in use");
        RM3World * world = worldsArray[gid];

        Nui32 index;
        index = world->userReplicaList.GetIndexOf(replica3);
        if(index == (Nui32)-1)
        {
            N_assert(world->networkIDManager);
            replica3->SetNetworkIDManager(world->networkIDManager);
            // If it crashes on mPrc2==0 then you didn't call LinkBase::attach()
            if(replica3->creatingSystemGUID == UniqueID::INVALID)
                replica3->creatingSystemGUID = mPrc2->getGUID((Address *)0);
            replica3->replicaManager = this;
            if(replica3->referenceIndex == (Nui32)-1)
            {
                replica3->referenceIndex = nextReferenceIndex++;
            }
            world->userReplicaList.Push(replica3);
            return world->userReplicaList.Size()-1;
        }
        return (Nui32) -1;
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::Dereference(Replica3 * replica3, WorldId gid)
    {
        N_assert(worldsArray[gid] != 0 && "World not in use");
        RM3World * world = worldsArray[gid];

        Nui32 index, index2;
        for (index = 0; index < world->userReplicaList.Size(); ++index)
        {
            if (world->userReplicaList[index] == replica3)
            {
                world->userReplicaList.RemoveAtIndex(index);
                break;
            }
        }

        // Remove from all connections
        for (index2=0; index2 < world->connectionList.Size(); index2++)
        {
            world->connectionList[index2]->OnDereference(replica3, this);
        }
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::DereferenceList(DataStructures::List<Replica3 *> & replicaListIn,
        WorldId gid)
    {
        Nui32 index;
        for (index=0; index < replicaListIn.Size(); index++)
            Dereference(replicaListIn[index], gid);
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::GetReplicasCreatedByMe(DataStructures::List<Replica3 *> & replicaListOut, WorldId gid)
    {
        GetReplicasCreatedByGuid(mPrc2->getGUID((Address *)0), replicaListOut, gid);
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::GetReferencedReplicaList(DataStructures::List<Replica3 *> & replicaListOut, WorldId gid)
    {
        N_assert(worldsArray[gid]!=0 && "World not in use");
        RM3World *world = worldsArray[gid];

        replicaListOut = world->userReplicaList;
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::GetReplicasCreatedByGuid(UniqueID id, DataStructures::List<Replica3*> &replicaListOut, WorldId gid)
    {
        N_assert(worldsArray[gid]!=0 && "World not in use");
        RM3World *world = worldsArray[gid];

        replicaListOut.Clear(false);
        NCount index;
        for (index = 0; index < world->userReplicaList.Size(); ++index)
        {
            if (world->userReplicaList[index]->creatingSystemGUID == id)
                replicaListOut.Push(world->userReplicaList[index]);
        }
    }
    //------------------------------------------------------------------------
    Nui32 ReplicaManager3::GetReplicaCount(WorldId gid) const
    {
        N_assert(worldsArray[gid] != 0 && "World not in use");
        RM3World *world = worldsArray[gid];

        return world->userReplicaList.Size();
    }
    //------------------------------------------------------------------------
    Replica3 *ReplicaManager3::GetReplicaAtIndex(Nui32 index, WorldId gid)
    {
        N_assert(worldsArray[gid] != 0 && "World not in use");
        RM3World *world = worldsArray[gid];

        return world->userReplicaList[index];
    }
    //------------------------------------------------------------------------
    Nui32 ReplicaManager3::GetConnectionCount(WorldId gid) const
    {
        N_assert(worldsArray[gid] != 0 && "World not in use");
        RM3World * world = worldsArray[gid];

        return world->connectionList.Size();
    }
    //------------------------------------------------------------------------
    Connection_RM3 * ReplicaManager3::GetConnectionAtIndex(Nui32 index, WorldId gid) const
    {
        N_assert(worldsArray[gid] != 0 && "World not in use");
        RM3World * world = worldsArray[gid];

        return world->connectionList[index];
    }
    //------------------------------------------------------------------------
    Connection_RM3 * ReplicaManager3::GetConnectionBySystemAddress(const Address & sa, WorldId gid) const
    {
        N_assert(worldsArray[gid] != 0 && "World not in use");
        RM3World * world = worldsArray[gid];

        NCount i;
        for (i = 0; i < world->connectionList.Size(); ++i)
        {
            if (world->connectionList[i]->GetSystemAddress()==sa)
            {
                return world->connectionList[i];
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    Connection_RM3 * ReplicaManager3::GetConnectionByGUID(UniqueID id, WorldId gid) const
    {
        N_assert(worldsArray[gid] != 0 && "World not in use");
        RM3World * world = worldsArray[gid];

        NCount i;
        for (i = 0; i < world->connectionList.Size(); ++i)
        {
            if (world->connectionList[i]->GetRakNetGUID() == id)
            {
                return world->connectionList[i];
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::SetDefaultOrderingChannel(char def)
    {
        defaultSendParameters.mPacketCh=def;
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::SetDefaultPacketPriority(PacketLevel def)
    {
        defaultSendParameters.mPacketLevel=def;
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::SetDefaultPacketReliability(PacketType def)
    {
        defaultSendParameters.mPacketType=def;
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::SetAutoSerializeInterval(TimeDurMS intervalMS)
    {
        autoSerializeInterval = intervalMS;
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::GetConnectionsThatHaveReplicaConstructed(Replica3 * replica,
        DataStructures::List<Connection_RM3 *> &connectionsThatHaveConstructedThisReplica,
            WorldId gid)
    {
        N_assert(worldsArray[gid] != 0 && "World not in use");
        RM3World * world = worldsArray[gid];

        connectionsThatHaveConstructedThisReplica.Clear(false);
        NCount index;
        for (index = 0; index < world->connectionList.Size(); ++index)
        {
            if (world->connectionList[index]->HasReplicaConstructed(replica))
                connectionsThatHaveConstructedThisReplica.Push(world->connectionList[index]);
        }
    }
    //------------------------------------------------------------------------
    bool ReplicaManager3::GetAllConnectionDownloadsCompleted(WorldId gid) const
    {
        N_assert(worldsArray[gid]!=0 && "World not in use");
        RM3World * world = worldsArray[gid];

        NCount index;
        for (index = 0; index < world->connectionList.Size(); ++index)
        {
            if (world->connectionList[index]->GetDownloadWasCompleted()==false)
                return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::Clear(bool deleteWorlds)
    {
        for (NCount i = 0; i < worldsList.Size(); ++i)
        {
            worldsList[i]->Clear(this);
            if (deleteWorlds)
            {
                worldsArray[worldsList[i]->worldId]=0;
                delete worldsList[i];
            }
        }
        if (deleteWorlds)
            worldsList.Clear(false);
    }
    //------------------------------------------------------------------------
    ReplicaManager3::RM3World::RM3World()
    {
        networkIDManager=0;
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::RM3World::Clear(ReplicaManager3 *replicaManager3)
    {
        if (replicaManager3->GetAutoDestroyConnections())
        {
            for (NCount i=0; i < connectionList.Size(); ++i)
            {
                replicaManager3->DeallocConnection(connectionList[i]);
            }
        }
        else
        {
            // Clear out downloadGroup even if not auto destroying the connection, since the packets need to go back to NetworkSys
            for (NCount i=0; i < connectionList.Size(); ++i)
            {
                connectionList[i]->ClearDownloadGroup(replicaManager3->getPrc());
            }
        }

        for (NCount i = 0; i < userReplicaList.Size(); ++i)
        {
            userReplicaList[i]->replicaManager = 0;
            userReplicaList[i]->SetNetworkIDManager(0);
        }
        connectionList.Clear(true);
        userReplicaList.Clear(true);
    }
    //------------------------------------------------------------------------
    PRO ReplicaManager3::GetDefaultSendParameters(void) const
    {
        return defaultSendParameters;
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::AddWorld(WorldId gid)
    {
        N_assert(worldsArray[gid]==0 && "World already in use");

        RM3World * newWorld = N_new RM3World();
        newWorld->worldId = gid;
        worldsArray[gid] = newWorld;
        worldsList.Push(newWorld);
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::RemoveWorld(WorldId gid)
    {
        N_assert(worldsArray[gid]!=0 && "World not in use");
        for (NCount i = 0; i < worldsList.Size(); ++i)
        {
            if (worldsList[i] == worldsArray[gid])
            {
                N_delete worldsList[i];
                worldsList.RemoveAtIndexFast(i);
                break;
            }
        }
        worldsArray[gid]=0;

    }
    //------------------------------------------------------------------------
    NetworkIDManager *ReplicaManager3::GetNetworkIDManager(WorldId gid) const
    {
        N_assert(worldsArray[gid]!=0 && "World not in use");
        RM3World *world = worldsArray[gid];

        return world->networkIDManager;
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::SetNetworkIDManager(NetworkIDManager *_networkIDManager,
        WorldId gid)
    {
        N_assert(worldsArray[gid]!=0 && "World not in use");
        RM3World * world = worldsArray[gid];

        world->networkIDManager=_networkIDManager;
    }
    //------------------------------------------------------------------------
    ConnectReceiveType ReplicaManager3::onMessage(SocketMessage * msg)
    {
        if (msg->mLength < 2)
            return CRT_Normal;

        WorldId tgid;

        TimeDurMS timestamp = 0;
        Nui8 token, offset;
        if ((Nui8)msg->mData[0] == ID_TIMESTAMP)
        {
            if (msg->mLength > sizeof(Nui8) + sizeof(TimeDurMS))
            {
                token = (Nui8)msg->mData[sizeof(Nui8) + sizeof(TimeDurMS)];
                // Required for proper endian swapping
                NetSerializer tsBs(msg->mData+sizeof(Token), msg->mLength - 1,false);
                tsBs.read(timestamp);
                // Next line assumes worldId is only 1 byte
                N_assert(sizeof(WorldId)==1);
                tgid = msg->mData[sizeof(Nui8) * 2 + sizeof(TimeDurMS)];
                offset = sizeof(Nui8) * 3 + sizeof(TimeDurMS);
            }
            else
                return CRT_Destroy;
        }
        else
        {
            token = (Nui8)msg->mData[0];
            // Next line assumes worldId is only 1 byte
            N_assert(sizeof(WorldId) == 1);
            tgid = msg->mData[sizeof(Nui8)];
            offset = sizeof(Nui8) * 2;
        }

        if (worldsArray[tgid] == 0)
            return CRT_Normal;

        switch (token)
        {
        case ID_REPLICA_MANAGER_CONSTRUCTION:
            return OnConstruction(msg, msg->mData, msg->mLength, msg->mID,
                offset, tgid);
        case ID_REPLICA_MANAGER_SERIALIZE:
            return OnSerialize(msg, msg->mData, msg->mLength, msg->mID, timestamp,
                offset, tgid);
        case ID_REPLICA_MANAGER_DOWNLOAD_STARTED:
            if (msg->mLocal == false)
            {
                return OnDownloadStarted(msg, msg->mData, msg->mLength, msg->mID, offset, tgid);
            }
            else
                break;
        case ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE:
            if (msg->mLocal == false)
            {
                return OnDownloadComplete(msg, msg->mData, msg->mLength, msg->mID, offset, tgid);
            }
            else
                break;
        case ID_REPLICA_MANAGER_SCOPE_CHANGE:
            {
                Connection_RM3 * connection = GetConnectionByGUID(msg->mID, tgid);
                if (connection && connection->isValidated == false)
                {
                    // This connection is now confirmed bidirectional
                    connection->isValidated = true;
                    // Reply back on validation
                    connection->SendValidation(mPrc2,tgid);
                }
            }
        }

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void Connection_RM3::AutoConstructByQuery(ReplicaManager3 *replicaManager3, WorldId gid)
    {
        ValidateLists(replicaManager3);

        ConstructionMode constructionMode = QueryConstructionMode();

        Nui32 index;
        RM3ConstructionState constructionState;
        LastSerializationResult *lsr;
        index=0;

        constructedReplicasCulled.Clear(false);
        destroyedReplicasCulled.Clear(false);

        if (constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION || constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION_AND_DESTRUCTION)
        {
            while (index < queryToConstructReplicaList.Size())
            {
                lsr=queryToConstructReplicaList[index];
                constructionState=lsr->replica->QueryConstruction(this, replicaManager3);
                if (constructionState==RM3CS_ALREADY_EXISTS_REMOTELY || constructionState==RM3CS_ALREADY_EXISTS_REMOTELY_DO_NOT_CONSTRUCT)
                {
                    OnReplicaAlreadyExists(index, replicaManager3);
                    if (constructionState==RM3CS_ALREADY_EXISTS_REMOTELY)
                        constructedReplicasCulled.Push(lsr->replica);
                }
                else if (constructionState==RM3CS_SEND_CONSTRUCTION)
                {
                    OnConstructToThisConnection(index, replicaManager3);
                    N_assert(lsr->replica);
                    constructedReplicasCulled.Push(lsr->replica);
                }
                else if (constructionState==RM3CS_NEVER_CONSTRUCT)
                {
                    OnNeverConstruct(index, replicaManager3);
                }
                else//  if (constructionState==RM3CS_NO_ACTION)
                {
                    // Do nothing
                    index++;
                }
            }

            if (constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION_AND_DESTRUCTION)
            {
                RM3DestructionState destructionState;
                index=0;
                while (index < queryToDestructReplicaList.Size())
                {
                    lsr=queryToDestructReplicaList[index];
                    destructionState=lsr->replica->QueryDestruction(this, replicaManager3);
                    if (destructionState==RM3DS_SEND_DESTRUCTION)
                    {
                        OnSendDestructionFromQuery(index, replicaManager3);
                        destroyedReplicasCulled.Push(lsr->replica);
                    }
                    else if (destructionState==RM3DS_DO_NOT_QUERY_DESTRUCTION)
                    {
                        OnDoNotQueryDestruction(index, replicaManager3);
                    }
                    else//  if (destructionState==RM3CS_NO_ACTION)
                    {
                        // Do nothing
                        index++;
                    }
                }
            }
        }
        else if (constructionMode==QUERY_CONNECTION_FOR_REPLICA_LIST)
        {
            QueryReplicaList(constructedReplicasCulled,destroyedReplicasCulled);

            Nui32 idx1, idx2;

            // Create new
            for (idx2=0; idx2 < constructedReplicasCulled.Size(); idx2++)
                OnConstructToThisConnection(constructedReplicasCulled[idx2], replicaManager3);

            bool exists;
            for (idx2=0; idx2 < destroyedReplicasCulled.Size(); idx2++)
            {
                exists=false;
                bool objectExists;
                idx1=constructedReplicaList.GetIndexFromKey(destroyedReplicasCulled[idx2], &objectExists);
                if (objectExists)
                {
                    constructedReplicaList.RemoveAtIndex(idx1);

                    Nui32 j;
                    for (j=0; j < queryToSerializeReplicaList.Size(); j++)
                    {
                        if (queryToSerializeReplicaList[j]->replica==destroyedReplicasCulled[idx2] )
                        {
                            queryToSerializeReplicaList.RemoveAtIndex(j);
                            break;
                        }
                    }
                }
            }
        }

        SendConstruction(constructedReplicasCulled, destroyedReplicasCulled,
            replicaManager3->defaultSendParameters, replicaManager3->mPrc2,
                gid, replicaManager3);
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::update(void)
    {
        Nui32 index,index2,index3;

        WorldId gid;
        RM3World * world;
        TimeDurMS time = N_Engine().getTimer().getMS();

        for (index3=0; index3 < worldsList.Size(); ++index3)
        {
            world = worldsList[index3];
            gid = world->worldId;

            for (index=0; index < world->connectionList.Size(); ++index)
            {
                if (world->connectionList[index]->isValidated==false)
                    continue;
                world->connectionList[index]->AutoConstructByQuery(this, gid);
            }
        }

        if (time - lastAutoSerializeOccurance >= autoSerializeInterval)
        {
            for (index3 = 0; index3 < worldsList.Size(); index3++)
            {
                world = worldsList[index3];
                gid = world->worldId;

                for (index=0; index < world->userReplicaList.Size(); ++index)
                {
                    world->userReplicaList[index]->forceSendUntilNextUpdate=false;
                    world->userReplicaList[index]->OnUserReplicaPreSerializeTick();
                }

                Nui32 index;
                SerializeParameters sp;
                sp.curTime=time;
                Connection_RM3 *connection;
                SendSerializeIfChangedResult ssicr;
                LastSerializationResult *lsr;

                sp.messageTimestamp=0;
                for (NCount i=0; i < RM3_NUM_OUTPUT_BITSTREAM_CHANNELS; ++i)
                    sp.pro[i]=defaultSendParameters;
                index2=0;
                for (index=0; index < world->connectionList.Size(); index++)
                {
                    connection = world->connectionList[index];
                    sp.bitsWrittenSoFar = 0;
                    index2 = 0;
                    sp.destinationConnection = connection;

                    DataStructures::List<Replica3*> replicasToSerialize;
                    replicasToSerialize.Clear(true);
                    if (connection->QuerySerializationList(replicasToSerialize))
                    {
                        // update replica->lsr so we can lookup in the next block
                        // lsr is per connection / per replica
                        while (index2 < connection->queryToSerializeReplicaList.Size())
                        {
                            connection->queryToSerializeReplicaList[index2]->replica->lsr=connection->queryToSerializeReplicaList[index2];
                            index2++;
                        }


                        // User is manually specifying list of replicas to serialize
                        index2 = 0;
                        while (index2 < replicasToSerialize.Size())
                        {
                            lsr = replicasToSerialize[index2]->lsr;
                            N_assert(lsr->replica==replicasToSerialize[index2]);

                            sp.whenLastSerialized=lsr->whenLastSerialized;
                            ssicr = connection->SendSerializeIfChanged(lsr, &sp, getPrc(), gid, this, time);
                            if (ssicr == SSICR_SENT_DATA)
                                lsr->whenLastSerialized=time;
                            index2++;
                        }
                    }
                    else
                    {
                        while (index2 < connection->queryToSerializeReplicaList.Size())
                        {
                            lsr = connection->queryToSerializeReplicaList[index2];

                            sp.destinationConnection=connection;
                            sp.whenLastSerialized = lsr->whenLastSerialized;
                            ssicr = connection->SendSerializeIfChanged(lsr, &sp, getPrc(), gid, this, time);
                            if (ssicr == SSICR_SENT_DATA)
                            {
                                lsr->whenLastSerialized=time;
                                index2++;
                            }
                            else if (ssicr == SSICR_NEVER_SERIALIZE)
                            {
                                // Removed from the middle of the list
                            }
                            else
                                index2++;
                        }
                    }
                }
            }

            lastAutoSerializeOccurance=time;
        }
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::onConnectClose(const Address &, UniqueID id, ConnectCloseType)
    {
        if (autoDestroyConnections)
        {
            Connection_RM3 *connection = PopConnection(id);
            if (connection)
                DeallocConnection(connection);
        }
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::onConnect(const Address & address, UniqueID id)
    {
        if (autoCreateConnections)
        {
            Connection_RM3 *connection = AllocConnection(address, id);
            if (connection)
                PushConnection(connection);
        }
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::shutdown(void)
    {
        if(autoDestroyConnections)
        {
            RM3World * world;
            NCount i;
            for (i = 0; i < worldsList.Size(); ++i)
            {
                world = worldsList[i];

                while (world->connectionList.Size())
                {
                    Connection_RM3 * connection =
                        PopConnection(world->connectionList.Size()-1, world->worldId);
                    if (connection)
                        DeallocConnection(connection);
                }
            }
        }
        
        Clear(false);
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::onDetach()
    {
        shutdown();
    }
    //------------------------------------------------------------------------
    ConnectReceiveType ReplicaManager3::OnConstruction(SocketMessage * msg,
        Nui8 * data, int count, UniqueID senderGuid, Nui8 offset, WorldId gid)
    {
        RM3World * world = worldsArray[gid];

        Connection_RM3 * connection = GetConnectionByGUID(senderGuid, gid);
        if (connection == 0)
        {
            // Almost certainly a bug
            N_assert("Got OnConstruction but no connection yet" && 0);
            return CRT_Normal;
        }
        if (connection->groupConstructionAndSerialize)
        {
            connection->downloadGroup.Push(msg);
            return CRT_Inner;
        }

        NetSerializer bsIn(data, count, false);
        bsIn.skipWrite(offset, 0);
        Nui16 constructionObjectListSize, destructionObjectListSize, index, index2;
        NBitCount streamEnd, writeAllocationIDEnd;
        Replica3 * replica;
        NetworkID networkId;
        UniqueID creatingSystemGuid;
        bool actuallyCreateObject = false;

        DataStructures::List<bool> actuallyCreateObjectList;
        DataStructures::List<Replica3 *> constructionTickStack;

        N_assert(world->networkIDManager);

        bsIn.read(constructionObjectListSize);
        for(index = 0; index < constructionObjectListSize; ++index)
        {
            bsIn.read(streamEnd);
            bsIn.read(networkId);
            Replica3 * existingReplica = world->networkIDManager->GET_OBJECT_FROM_ID<Replica3*>(networkId);
            bsIn.read(actuallyCreateObject);
            actuallyCreateObjectList.Push(actuallyCreateObject);
            bsIn.readAlign();

            if (actuallyCreateObject)
            {
                bsIn.read(creatingSystemGuid);
                bsIn.read(writeAllocationIDEnd);

                if (existingReplica)
                {
                    existingReplica->replicaManager = this;

                    // Network ID already in use
                    connection->OnDownloadExisting(existingReplica, this);

                    constructionTickStack.Push(0);
                    bsIn.seekRead(streamEnd);
                    continue;
                }

                bsIn.readAlign();
                replica = connection->AllocReplica(&bsIn, this);
                if(replica == 0)
                {
                    constructionTickStack.Push(0);
                    bsIn.seekRead(streamEnd);
                    continue;
                }

                // Go past the bitStream written to with WriteAllocationID(). Necessary in case the user didn't read out the bitStream the same way it was written
                // bitOffset2 is already aligned
                bsIn.seekRead(writeAllocationIDEnd);

                replica->SetNetworkIDManager(world->networkIDManager);
                replica->SetNetworkID(networkId);

                replica->replicaManager = this;
                replica->creatingSystemGUID = creatingSystemGuid;

                if (!replica->QueryRemoteConstruction(connection) ||
                    !replica->DeserializeConstruction(&bsIn, connection))
                {
                    DeallocReplicaNoBroadcastDestruction(connection, replica);
                    bsIn.seekRead(streamEnd);
                    constructionTickStack.Push(0);
                    continue;
                }

                constructionTickStack.Push(replica);

                // Register the replica
                ReferenceInternal(replica, gid);
            }
            else
            {
                if (existingReplica)
                {
                    existingReplica->DeserializeConstructionExisting(&bsIn, connection);
                    constructionTickStack.Push(existingReplica);
                }
                else
                {
                    constructionTickStack.Push(0);
                }
            }


            bsIn.seekRead(streamEnd);
            bsIn.readAlign();
        }

        N_assert(constructionTickStack.Size()==constructionObjectListSize);
        N_assert(actuallyCreateObjectList.Size()==constructionObjectListSize);

        NetSerializer empty;
        for (index=0; index < constructionObjectListSize; index++)
        {
            bool pdcWritten=false;
            bsIn.read(pdcWritten);
            if (pdcWritten)
            {
                bsIn.readAlign();
                bsIn.read(streamEnd);
                bsIn.read(networkId);
                if (constructionTickStack[index]!=0)
                {
                    bsIn.readAlign();
                    if (actuallyCreateObjectList[index])
                        constructionTickStack[index]->PostDeserializeConstruction(&bsIn, connection);
                    else
                        constructionTickStack[index]->PostDeserializeConstructionExisting(&bsIn, connection);
                }
                bsIn.seekRead(streamEnd);
            }
            else
            {
                if (constructionTickStack[index]!=0)
                {
                    if (actuallyCreateObjectList[index])
                        constructionTickStack[index]->PostDeserializeConstruction(&empty, connection);
                    else
                        constructionTickStack[index]->PostDeserializeConstructionExisting(&empty, connection);
                }
            }
        }

        for (index=0; index < constructionObjectListSize; index++)
        {
            if (constructionTickStack[index]!=0)
            {
                if (actuallyCreateObjectList[index])
                {
                    // Tell the connection(s) that this object exists since they just sent it to us
                    connection->OnDownloadFromThisSystem(constructionTickStack[index], this);

                    for (index2=0; index2 < world->connectionList.Size(); index2++)
                    {
                        if (world->connectionList[index2]!=connection)
                            world->connectionList[index2]->OnDownloadFromOtherSystem(constructionTickStack[index], this);
                    }
                }
            }
        }

        // Destructions
        bool b = bsIn.read(destructionObjectListSize);
        (void) b;
        N_assert(b);
        for (index=0; index < destructionObjectListSize; index++)
        {
            bsIn.read(networkId);
            bsIn.read(streamEnd);
            replica = world->networkIDManager->GET_OBJECT_FROM_ID<Replica3*>(networkId);
            if (replica==0)
            {
                // Unknown object
                bsIn.seekRead(streamEnd);
                continue;
            }
            bsIn.read(replica->deletingSystemGUID);
            if (replica->DeserializeDestruction(&bsIn,connection))
            {
                // Make sure it wasn't deleted in DeserializeDestruction
                if (world->networkIDManager->GET_OBJECT_FROM_ID<Replica3*>(networkId))
                {
                    replica->PreDestruction(connection);

                    // Forward deletion by remote system
                    if (replica->QueryRelayDestruction(connection))
                        BroadcastDestruction(replica,connection->GetSystemAddress());
                    Dereference(replica);
                    DeallocReplicaNoBroadcastDestruction(connection, replica);
                }
            }
            else
            {
                replica->PreDestruction(connection);
                connection->OnDereference(replica, this);
            }

            bsIn.readAlign();
        }
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    ConnectReceiveType ReplicaManager3::OnSerialize(SocketMessage * msg, Nui8 * data,
        int count, UniqueID senderGuid, TimeDurMS timestamp, Nui8 offset, WorldId gid)
    {
        Connection_RM3 * connection = GetConnectionByGUID(senderGuid, gid);
        if(connection == 0)
            return CRT_Normal;
        if(connection->groupConstructionAndSerialize)
        {
            connection->downloadGroup.Push(msg);
            return CRT_Inner;
        }

        RM3World * world = worldsArray[gid];
        N_assert(world->networkIDManager);
        NetSerializer bsIn(data, count, false);
        bsIn.skipWrite(offset, 0);
        struct DeserializeParameters ds;
        ds.timeStamp = timestamp;
        ds.sourceConnection = connection;

        Replica3 * replica;
        NetworkID networkId;
        NBitCount bitsUsed;
        bsIn.read(networkId);

        replica = world->networkIDManager->GET_OBJECT_FROM_ID<Replica3 *>(networkId);
        if (replica)
        {
            for (NCount z = 0; z < RM3_NUM_OUTPUT_BITSTREAM_CHANNELS; ++z)
            {
                bsIn.read(ds.bitstreamWrittenTo[z]);
                if (ds.bitstreamWrittenTo[z])
                {
                    bsIn.readCompress(bitsUsed);
                    bsIn.readAlign();
                    bsIn.read(ds.serializationBitstream[z], bitsUsed);
                }
            }
            replica->import(&ds);
        }
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    ConnectReceiveType ReplicaManager3::OnDownloadStarted(SocketMessage * msg,
        Nui8 * data, int count, UniqueID senderGuid, Nui8 offset,
            WorldId gid)
    {
        Connection_RM3 * connection = GetConnectionByGUID(senderGuid, gid);
        if(connection == 0)
            return CRT_Normal;
        if(connection->QueryGroupDownloadMessages() &&
            // ID_DOWNLOAD_STARTED will be processed twice, being processed the second time once ID_DOWNLOAD_COMPLETE arrives.
            // However, the second time groupConstructionAndSerialize will be set to true so it won't be processed a third time
            connection->groupConstructionAndSerialize == false)
        {
            // These messages will be held by the plugin and returned when the download is complete
            connection->groupConstructionAndSerialize = true;
            N_assert(connection->downloadGroup.Size() == 0);
            connection->downloadGroup.Push(msg);
            return CRT_Inner;
        }

        connection->groupConstructionAndSerialize = false;
        NetSerializer bsIn(data, count, false);
        bsIn.skipWrite(offset, 0);
        connection->DeserializeOnDownloadStarted(&bsIn);
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    ConnectReceiveType ReplicaManager3::OnDownloadComplete(SocketMessage * msg,
        Nui8 * data, int count, UniqueID senderGuid, Nui8 offset, WorldId gid)
    {
        Connection_RM3 *connection = GetConnectionByGUID(senderGuid, gid);
        if (connection==0)
            return CRT_Normal;

        if (connection->groupConstructionAndSerialize==true && connection->downloadGroup.Size()>0)
        {
            // Push back buffered packets in front of this one
            for (NCount i = 0; i < connection->downloadGroup.Size(); ++i)
                mPrc2->addMessage(connection->downloadGroup[i], false);

            // Push this one to be last too. It will be processed again, but the second time
            // groupConstructionAndSerialize will be false and downloadGroup will be empty, so it will go past this block
            connection->downloadGroup.Clear();
            mPrc2->addMessage(msg, false);

            return CRT_Inner;
        }

        NetSerializer bsIn(data, count, false);
        bsIn.skipWrite(offset, 0);
        connection->gotDownloadComplete = true;
        connection->DeserializeOnDownloadComplete(&bsIn);
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    Replica3 * ReplicaManager3::GetReplicaByNetworkID(NetworkID networkId, WorldId gid)
    {
        RM3World * world = worldsArray[gid];

        NCount i;
        for (i = 0; i < world->userReplicaList.Size(); ++i)
        {
            if (world->userReplicaList[i]->GetNetworkID() == networkId)
                return world->userReplicaList[i];
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::BroadcastDestructionList(DataStructures::List<Replica3 *> &replicaListSource,
        const Address * exclusionAddress, WorldId gid)
    {
        NetSerializer out;
        NCount i,j;

        N_assert(worldsArray[gid]!=0 && "World not in use");
        RM3World * world = worldsArray[gid];

        DataStructures::List<Replica3 *> replicaList;

        for(i = 0; i < replicaListSource.Size(); ++i)
        {
            if (replicaListSource[i] == currentlyDeallocatingReplica)
                continue;
            replicaList.Push(replicaListSource[i]);
        }

        if (replicaList.Size() == 0)
            return;

        for(i = 0; i < replicaList.Size(); ++i)
        {
            if (replicaList[i]->deletingSystemGUID == UniqueID::INVALID)
                replicaList[i]->deletingSystemGUID = getPrc()->getGUID((Address *)0);
        }

        for(j = 0; j < world->connectionList.Size(); ++j)
        {
            if(world->connectionList[j]->GetSystemAddress() == *exclusionAddress)
                continue;

            out.seekWrite(0);
            out.seekRead(0);
            out.write((Token)ID_REPLICA_MANAGER_CONSTRUCTION);
            out.write(gid);
            Nui16 cnt = 0;
            out.write(cnt); // No construction
            cnt = (Nui16)replicaList.Size();
            NBitCount cntBOffset = out.getWriteOffset();
            out.write(cnt); // Overwritten at send call
            cnt = 0;

            for(i = 0; i < replicaList.Size(); ++i)
            {
                if(world->connectionList[j]->HasReplicaConstructed(replicaList[i]) == false)
                    continue;
                ++cnt;

                NetworkID networkId;
                networkId = replicaList[i]->GetNetworkID();
                out.write(networkId);
                NBitCount offsetBStart, offsetBEnd;
                offsetBStart = out.getWriteOffset();
                out.write(offsetBStart);
                out.write(replicaList[i]->deletingSystemGUID);
                replicaList[i]->SerializeDestruction(&out, world->connectionList[j]);
                out.writeAlign();
                offsetBEnd = out.getWriteOffset();
                out.seekWrite(offsetBStart);
                out.write(offsetBEnd);
                out.seekWrite(offsetBEnd);
            }

            if (cnt > 0)
            {
                NBitCount curBOffset = out.getWriteOffset();
                out.seekWrite(cntBOffset);
                out.write(cnt);
                out.seekWrite(curBOffset);
                mPrc2->send(&out,defaultSendParameters.mPacketLevel,
                    defaultSendParameters.mPacketType,defaultSendParameters.mPacketCh,
                        world->connectionList[j]->GetSystemAddress(),false,
                            defaultSendParameters.sendReceipt);
            }
        }
    }
    //------------------------------------------------------------------------
    void ReplicaManager3::BroadcastDestruction(Replica3 *replica, const Address * exclusionAddress)
    {
        DataStructures::List<Replica3 *> replicaList;
        replicaList.Push(replica);
        BroadcastDestructionList(replicaList,exclusionAddress);
    }
    //------------------------------------------------------------------------
    Connection_RM3::Connection_RM3(const Address &_systemAddress, UniqueID _guid)
    : mAddress(_systemAddress), mID(_guid)
    {
        isValidated=false;
        isFirstConstruction=true;
        groupConstructionAndSerialize=false;
        gotDownloadComplete=false;
    }
    //------------------------------------------------------------------------
    Connection_RM3::~Connection_RM3()
    {
        Nui32 i;
        for (i=0; i < constructedReplicaList.Size(); i++)
            N_delete constructedReplicaList[i];
        for (i=0; i < queryToConstructReplicaList.Size(); i++)
            N_delete queryToConstructReplicaList[i];
    }
    //------------------------------------------------------------------------
    void Connection_RM3::GetConstructedReplicas(DataStructures::List<Replica3 *> &objectsTheyDoHave)
    {
        objectsTheyDoHave.Clear(true);
        for (NCount idx = 0; idx < constructedReplicaList.Size(); ++idx)
        {
            objectsTheyDoHave.Push(constructedReplicaList[idx]->replica);
        }
    }
    //------------------------------------------------------------------------
    bool Connection_RM3::HasReplicaConstructed(Replica3 * replica)
    {
        bool objectExists;
        constructedReplicaList.GetIndexFromKey(replica, &objectExists);
        return objectExists;
    }
    //------------------------------------------------------------------------
    void Connection_RM3::SendSerializeHeader(Replica3 * replica, TimeDurMS timestamp, 
        NetSerializer * bs, WorldId gid)
    {
        bs->seekRead(0);
        bs->seekWrite(0);
        if(timestamp != 0)
        {
            bs->write((Token)ID_TIMESTAMP);
            bs->write(timestamp);
        }
        bs->write((Token)ID_REPLICA_MANAGER_SERIALIZE);
        bs->write(gid);
        bs->write(replica->GetNetworkID());
    }
    //------------------------------------------------------------------------
    void Connection_RM3::ClearDownloadGroup(LinkBase * linkbase)
    {
        Nui32 i;
        for (i = 0; i < downloadGroup.Size(); ++i)
            N_delete downloadGroup[i];
        downloadGroup.Clear();
    }
    //------------------------------------------------------------------------
    SendSerializeIfChangedResult Connection_RM3::SendSerialize(Replica3 * replica,
        bool indicesToSend[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS],
            NetSerializer serializationData[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS],
                TimeDurMS timestamp, PRO sendParameters[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS],
                    LinkBase * link, Nui8 gid, TimeDurMS curTime)
    {
        bool channelHasData;
        NBitCount sum = 0;
        for(int z = 0; z < RM3_NUM_OUTPUT_BITSTREAM_CHANNELS; ++z)
        {
            if(indicesToSend[z])
                sum += serializationData[z].getWriteOffset();
        }

        NetSerializer out;
        NBitCount bitsPerChannel[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS];

        if(sum == 0)
        {
            memset(bitsPerChannel, 0, sizeof(bitsPerChannel));
            replica->OnSerializeTransmission(&out, this, bitsPerChannel, curTime);
            return SSICR_DID_NOT_SEND_DATA;
        }

        N_assert(replica->GetNetworkID()!=UNASSIGNED_NETWORK_ID);

        NBitCount bitsUsed;

        int channelIndex;
        PRO lastPro=sendParameters[0];

        for(channelIndex = 0; channelIndex < RM3_NUM_OUTPUT_BITSTREAM_CHANNELS; channelIndex++)
        {
            if(channelIndex == 0)
            {
                SendSerializeHeader(replica, timestamp, &out, gid);
            }
            else if(lastPro != sendParameters[channelIndex])
            {
                // write out remainder
                for(int channelIndex2 = channelIndex; channelIndex2 < RM3_NUM_OUTPUT_BITSTREAM_CHANNELS; channelIndex2++)
                {
                    bitsPerChannel[channelIndex2] = 0;
                    out.write0();
                }

                // send remainder
                replica->OnSerializeTransmission(&out, this, bitsPerChannel, curTime);
                link->send(&out,lastPro.mPacketLevel,lastPro.mPacketType,lastPro.mPacketCh,mAddress,false,lastPro.sendReceipt);

                // If no data left to send, quit out
                bool anyData = false;
                for(int channelIndex2 = channelIndex; channelIndex2 < RM3_NUM_OUTPUT_BITSTREAM_CHANNELS; channelIndex2++)
                {
                    if(serializationData[channelIndex2].getWriteOffset() > 0)
                    {
                        anyData=true;
                        break;
                    }
                }
                if (anyData==false)
                    return SSICR_SENT_DATA;

                // Restart stream
                SendSerializeHeader(replica, timestamp, &out, gid);

                for (int channelIndex2=0; channelIndex2 < channelIndex; channelIndex2++)
                {
                    bitsPerChannel[channelIndex2]=0;
                    out.write0();
                }
                lastPro=sendParameters[channelIndex];
            }

            bitsUsed=serializationData[channelIndex].getWriteOffset();
            channelHasData = indicesToSend[channelIndex] == true && bitsUsed>0;
            out.write(channelHasData);
            if (channelHasData)
            {
                bitsPerChannel[channelIndex] = bitsUsed;
                out.writeCompress(bitsUsed);
                out.writeAlign();
                out.write(serializationData[channelIndex]);
                // Crap, forgot this line, was a huge bug in that I'd only send to the first 3 systems
                serializationData[channelIndex].seekRead(0);
            }
            else
            {
                bitsPerChannel[channelIndex] = 0;
            }
        }
        replica->OnSerializeTransmission(&out, this, bitsPerChannel, curTime);
        link->send(&out,lastPro.mPacketLevel,lastPro.mPacketType,lastPro.mPacketCh,mAddress,false,lastPro.sendReceipt);
        return SSICR_SENT_DATA;
    }
    //------------------------------------------------------------------------
    SendSerializeIfChangedResult Connection_RM3::SendSerializeIfChanged(
        LastSerializationResult * lsr, SerializeParameters * sp, LinkBase * link,
            Nui8 gid, ReplicaManager3 * replicaManager, TimeDurMS curTime)
    {
        Replica3 * replica = lsr->replica;

        if(replica->GetNetworkID() == UNASSIGNED_NETWORK_ID)
            return SSICR_DID_NOT_SEND_DATA;

        RM3QuerySerializationResult rm3qsr = replica->QuerySerialization(this);
        if(rm3qsr == RM3QSR_NEVER_CALL_SERIALIZE)
        {
            // Never again for this connection and replica pair
            OnNeverSerialize(lsr, replicaManager);
            return SSICR_NEVER_SERIALIZE;
        }

        if(rm3qsr == RM3QSR_DO_NOT_CALL_SERIALIZE)
            return SSICR_DID_NOT_SEND_DATA;

        if(replica->forceSendUntilNextUpdate)
        {
            for(int z=0; z < RM3_NUM_OUTPUT_BITSTREAM_CHANNELS; ++z)
            {
                if(replica->lastSentSerialization.indicesToSend[z])
                    sp->bitsWrittenSoFar+=replica->lastSentSerialization.bitStream[z].getWriteOffset();
            }
            return SendSerialize(replica, replica->lastSentSerialization.indicesToSend,
                replica->lastSentSerialization.bitStream, sp->messageTimestamp, sp->pro,
                    link, gid, curTime);
        }

        for(int i=0; i < RM3_NUM_OUTPUT_BITSTREAM_CHANNELS; ++i)
        {
            sp->outputBitstream[i].seekRead(0);
            sp->outputBitstream[i].seekWrite(0);
            if (lsr->lastSerializationResultBS)
                sp->lastSentBitstream[i]=&lsr->lastSerializationResultBS->bitStream[i];
            else
                sp->lastSentBitstream[i]=&replica->lastSentSerialization.bitStream[i];
        }

        RM3SerializationResult serializationResult = replica->read(sp);

        if (serializationResult==RM3SR_NEVER_SERIALIZE_FOR_THIS_CONNECTION)
        {
            // Never again for this connection and replica pair
            OnNeverSerialize(lsr, replicaManager);
            return SSICR_NEVER_SERIALIZE;
        }

        if (serializationResult == RM3SR_DO_NOT_SERIALIZE)
        {
            // Don't serialize this tick only
            return SSICR_DID_NOT_SEND_DATA;
        }

        // This is necessary in case the user in the read() function for some reason read the bitstream they also wrote
        // WIthout this code, the write calls to another bitstream would not write the entire bitstream
        NBitCount sum = 0;
        for (int z = 0; z < RM3_NUM_OUTPUT_BITSTREAM_CHANNELS; ++z)
        {
            sp->outputBitstream[z].seekRead(0);
            sum += sp->outputBitstream[z].getWriteOffset();
        }

        if (sum == 0)
        {
            // Don't serialize this tick only
            return SSICR_DID_NOT_SEND_DATA;
        }

        if (serializationResult == RM3SR_SERIALIZED_ALWAYS)
        {
            bool allIndices[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS];
            for (int z = 0; z < RM3_NUM_OUTPUT_BITSTREAM_CHANNELS; ++z)
            {
                sp->bitsWrittenSoFar += sp->outputBitstream[z].getWriteOffset();
                allIndices[z] = true;

                lsr->AllocBS();
                lsr->lastSerializationResultBS->bitStream[z].seekWrite(0);
                lsr->lastSerializationResultBS->bitStream[z].seekRead(0);
                lsr->lastSerializationResultBS->bitStream[z].write(&sp->outputBitstream[z]);
                sp->outputBitstream[z].seekRead(0);
            }
            return SendSerialize(replica, allIndices, sp->outputBitstream,
                sp->messageTimestamp, sp->pro, link, gid, curTime);
        }

        if (serializationResult==RM3SR_SERIALIZED_ALWAYS_IDENTICALLY)
        {
            for (int z = 0; z < RM3_NUM_OUTPUT_BITSTREAM_CHANNELS; ++z)
            {
                replica->lastSentSerialization.indicesToSend[z] = sp->outputBitstream[z].getWriteOffset()>0;
                sp->bitsWrittenSoFar += sp->outputBitstream[z].getWriteOffset();
                replica->lastSentSerialization.bitStream[z].seekWrite(0);
                replica->lastSentSerialization.bitStream[z].seekRead(0);
                replica->lastSentSerialization.bitStream[z].write(&sp->outputBitstream[z]);
                sp->outputBitstream[z].seekRead(0);
                replica->forceSendUntilNextUpdate = true;
            }
            return SendSerialize(replica, replica->lastSentSerialization.indicesToSend,
                sp->outputBitstream, sp->messageTimestamp, sp->pro, link, gid, curTime);
        }

        bool indicesToSend[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS];
        if (serializationResult==RM3SR_BROADCAST_IDENTICALLY || serializationResult==RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION)
        {
            for (int z = 0; z < RM3_NUM_OUTPUT_BITSTREAM_CHANNELS; ++z)
            {
                if (sp->outputBitstream[z].getWriteOffset() > 0 &&
                    (serializationResult==RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION ||
                    ((sp->outputBitstream[z].getWriteOffset()!=replica->lastSentSerialization.bitStream[z].getWriteOffset() ||
                    memcmp(sp->outputBitstream[z].getData(), replica->lastSentSerialization.bitStream[z].getData(), sp->outputBitstream[z].getSize())!=0))))
                {
                    indicesToSend[z]=true;
                    replica->lastSentSerialization.indicesToSend[z] = true;
                    sp->bitsWrittenSoFar+=sp->outputBitstream[z].getWriteOffset();
                    replica->lastSentSerialization.bitStream[z].seekWrite(0);
                    replica->lastSentSerialization.bitStream[z].seekRead(0);
                    replica->lastSentSerialization.bitStream[z].write(&sp->outputBitstream[z]);
                    sp->outputBitstream[z].seekRead(0);
                    replica->forceSendUntilNextUpdate = true;
                }
                else
                {
                    indicesToSend[z]=false;
                    replica->lastSentSerialization.indicesToSend[z]=false;
                }
            }
        }
        else
        {
            lsr->AllocBS();

            // RM3SR_SERIALIZED_UNIQUELY
            for (int z = 0; z < RM3_NUM_OUTPUT_BITSTREAM_CHANNELS; ++z)
            {
                if (sp->outputBitstream[z].getWriteOffset() > 0 &&
                    (sp->outputBitstream[z].getWriteOffset()!=lsr->lastSerializationResultBS->bitStream[z].getWriteOffset() ||
                        memcmp(sp->outputBitstream[z].getData(), lsr->lastSerializationResultBS->bitStream[z].getData(), sp->outputBitstream[z].getSize())!=0))
                {
                    indicesToSend[z] = true;
                    sp->bitsWrittenSoFar += sp->outputBitstream[z].getWriteOffset();
                    lsr->lastSerializationResultBS->bitStream[z].seekWrite(0);
                    lsr->lastSerializationResultBS->bitStream[z].seekRead(0);
                    lsr->lastSerializationResultBS->bitStream[z].write(&sp->outputBitstream[z]);
                    sp->outputBitstream[z].seekRead(0);
                }
                else
                {
                    indicesToSend[z] = false;
                }
            }
        }


        if (serializationResult == RM3SR_BROADCAST_IDENTICALLY || serializationResult==RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION)
            replica->forceSendUntilNextUpdate = true;

        // send out the data
        return SendSerialize(replica, indicesToSend, sp->outputBitstream,
            sp->messageTimestamp, sp->pro, link, gid, curTime);
    }
    //------------------------------------------------------------------------
    void Connection_RM3::OnLocalReference(Replica3* replica3, ReplicaManager3 *)
    {
        ConstructionMode constructionMode = QueryConstructionMode();
        N_assert(constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION || constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION_AND_DESTRUCTION);
        N_assert(replica3);

    #ifdef _DEBUG
        for (Nui32 i=0; i < queryToConstructReplicaList.Size(); i++)
        {
            if (queryToConstructReplicaList[i]->replica==replica3)
            {
                N_assert("replica added twice to queryToConstructReplicaList" && 0);
            }
        }

        if (constructedReplicaList.HasData(replica3) == true)
        {
            N_assert("replica added to queryToConstructReplicaList when already in constructedReplicaList" && 0);
        }
    #endif

        LastSerializationResult * lsr = N_new LastSerializationResult();
        lsr->replica = replica3;
        queryToConstructReplicaList.Push(lsr);
    }
    //------------------------------------------------------------------------
    void Connection_RM3::OnDereference(Replica3 * replica3, ReplicaManager3 * replicaManager)
    {
        ValidateLists(replicaManager);

        if (replica3->GetNetworkIDManager() == 0)
            return;

        LastSerializationResult* lsr=0;
        Nui32 idx;

        bool objectExists;
        idx=constructedReplicaList.GetIndexFromKey(replica3, &objectExists);
        if (objectExists)
        {
            lsr=constructedReplicaList[idx];
            constructedReplicaList.RemoveAtIndex(idx);
        }

        for (idx=0; idx < queryToConstructReplicaList.Size(); idx++)
        {
            if (queryToConstructReplicaList[idx]->replica==replica3)
            {
                lsr=queryToConstructReplicaList[idx];
                queryToConstructReplicaList.RemoveAtIndex(idx);
                break;
            }
        }

        for (idx=0; idx < queryToSerializeReplicaList.Size(); idx++)
        {
            if (queryToSerializeReplicaList[idx]->replica==replica3)
            {
                lsr=queryToSerializeReplicaList[idx];
                queryToSerializeReplicaList.RemoveAtIndex(idx);
                break;
            }
        }

        for (idx=0; idx < queryToDestructReplicaList.Size(); idx++)
        {
            if (queryToDestructReplicaList[idx]->replica==replica3)
            {
                lsr=queryToDestructReplicaList[idx];
                queryToDestructReplicaList.RemoveAtIndex(idx);
                break;
            }
        }

        ValidateLists(replicaManager);

        if (lsr)
            N_delete lsr;

        ValidateLists(replicaManager);
    }
    //------------------------------------------------------------------------
    void Connection_RM3::OnDownloadFromThisSystem(Replica3* replica3, ReplicaManager3 *replicaManager)
    {
        N_assert(replica3);

        ValidateLists(replicaManager);
        LastSerializationResult* lsr= N_new LastSerializationResult();
        lsr->replica=replica3;

        ConstructionMode constructionMode = QueryConstructionMode();
        if (constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION || constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION_AND_DESTRUCTION)
        {
            Nui32 j;
            for (j=0; j < queryToConstructReplicaList.Size(); j++)
            {
                if (queryToConstructReplicaList[j]->replica==replica3 )
                {
                    queryToConstructReplicaList.RemoveAtIndex(j);
                    break;
                }
            }

            queryToDestructReplicaList.Push(lsr);
        }

        if (constructedReplicaList.Insert(lsr->replica, lsr, true) != (Nui32) -1)
        {
            queryToSerializeReplicaList.Push(lsr);
        }

        ValidateLists(replicaManager);
    }
    //------------------------------------------------------------------------
    void Connection_RM3::OnDownloadFromOtherSystem(Replica3* replica3, ReplicaManager3 *replicaManager)
    {
        ConstructionMode constructionMode = QueryConstructionMode();
        if (constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION || constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION_AND_DESTRUCTION)
        {
            Nui32 j;
            for (j=0; j < queryToConstructReplicaList.Size(); j++)
            {
                if (queryToConstructReplicaList[j]->replica==replica3 )
                {
                    return;
                }
            }

            OnLocalReference(replica3, replicaManager);
        }
    }
    //------------------------------------------------------------------------
    void Connection_RM3::OnNeverConstruct(Nui32 queryToConstructIdx, ReplicaManager3 *replicaManager)
    {
        ConstructionMode constructionMode = QueryConstructionMode();
        N_assert(constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION || constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION_AND_DESTRUCTION);
        (void) constructionMode;

        ValidateLists(replicaManager);
        LastSerializationResult* lsr = queryToConstructReplicaList[queryToConstructIdx];
        queryToConstructReplicaList.RemoveAtIndex(queryToConstructIdx);
        N_delete lsr;
        ValidateLists(replicaManager);
    }
    //------------------------------------------------------------------------
    void Connection_RM3::OnConstructToThisConnection(Nui32 queryToConstructIdx,
        ReplicaManager3 * replicaManager)
    {
        ConstructionMode constructionMode = QueryConstructionMode();
        N_assert(constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION || constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION_AND_DESTRUCTION);
        (void) constructionMode;

        ValidateLists(replicaManager);
        LastSerializationResult* lsr = queryToConstructReplicaList[queryToConstructIdx];
        queryToConstructReplicaList.RemoveAtIndex(queryToConstructIdx);
        constructedReplicaList.Insert(lsr->replica,lsr,true);
        queryToDestructReplicaList.Push(lsr);
        queryToSerializeReplicaList.Push(lsr);
        ValidateLists(replicaManager);
    }
    //------------------------------------------------------------------------
    void Connection_RM3::OnConstructToThisConnection(Replica3 * replica,
        ReplicaManager3 * replicaManager)
    {
        N_assert(replica);
        N_assert(QueryConstructionMode()==QUERY_CONNECTION_FOR_REPLICA_LIST);
        (void) replicaManager;

        LastSerializationResult* lsr= N_new LastSerializationResult();
        lsr->replica=replica;
        constructedReplicaList.Insert(replica,lsr,true);
        queryToSerializeReplicaList.Push(lsr);
    }
    //------------------------------------------------------------------------
    void Connection_RM3::OnNeverSerialize(LastSerializationResult *lsr, ReplicaManager3 *replicaManager)
    {
        ValidateLists(replicaManager);

        Nui32 j;
        for (j=0; j < queryToSerializeReplicaList.Size(); j++)
        {
            if (queryToSerializeReplicaList[j]==lsr )
            {
                queryToSerializeReplicaList.RemoveAtIndex(j);
                break;
            }
        }

        ValidateLists(replicaManager);
    }
    //------------------------------------------------------------------------
    void Connection_RM3::OnReplicaAlreadyExists(Nui32 queryToConstructIdx, ReplicaManager3 *replicaManager)
    {
        ConstructionMode constructionMode = QueryConstructionMode();
        N_assert(constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION || constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION_AND_DESTRUCTION);
        (void) constructionMode;

        ValidateLists(replicaManager);
        LastSerializationResult * lsr = queryToConstructReplicaList[queryToConstructIdx];
        queryToConstructReplicaList.RemoveAtIndex(queryToConstructIdx);
        constructedReplicaList.Insert(lsr->replica,lsr,true);
        queryToDestructReplicaList.Push(lsr);
        queryToSerializeReplicaList.Push(lsr);
        ValidateLists(replicaManager);
    }
    //------------------------------------------------------------------------
    void Connection_RM3::OnDownloadExisting(Replica3* replica3, ReplicaManager3 *replicaManager)
    {
        ValidateLists(replicaManager);

        ConstructionMode constructionMode = QueryConstructionMode();
        if (constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION || constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION_AND_DESTRUCTION)
        {
            Nui32 idx;
            for (idx=0; idx < queryToConstructReplicaList.Size(); idx++)
            {
                if (queryToConstructReplicaList[idx]->replica==replica3)
                {
                    OnConstructToThisConnection(idx, replicaManager);
                    return;
                }
            }
        }
        else
        {
            OnConstructToThisConnection(replica3, replicaManager);
        }
    }
    //------------------------------------------------------------------------
    void Connection_RM3::OnSendDestructionFromQuery(Nui32 queryToDestructIdx, ReplicaManager3 *replicaManager)
    {
        ConstructionMode constructionMode = QueryConstructionMode();
        N_assert(constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION || constructionMode==QUERY_REPLICA_FOR_CONSTRUCTION_AND_DESTRUCTION);
        (void) constructionMode;

        ValidateLists(replicaManager);
        LastSerializationResult* lsr = queryToDestructReplicaList[queryToDestructIdx];
        queryToDestructReplicaList.RemoveAtIndex(queryToDestructIdx);
        Nui32 j;
        for (j=0; j < queryToSerializeReplicaList.Size(); j++)
        {
            if (queryToSerializeReplicaList[j]->replica==lsr->replica )
            {
                queryToSerializeReplicaList.RemoveAtIndex(j);
                break;
            }
        }
        for (j=0; j < constructedReplicaList.Size(); j++)
        {
            if (constructedReplicaList[j]->replica==lsr->replica )
            {
                constructedReplicaList.RemoveAtIndex(j);
                break;
            }
        }

        queryToConstructReplicaList.Push(lsr);
        ValidateLists(replicaManager);
    }
    //------------------------------------------------------------------------
    void Connection_RM3::OnDoNotQueryDestruction(Nui32 queryToDestructIdx, ReplicaManager3 *replicaManager)
    {
        ValidateLists(replicaManager);
        queryToDestructReplicaList.RemoveAtIndex(queryToDestructIdx);
        ValidateLists(replicaManager);
    }
    //------------------------------------------------------------------------
    void Connection_RM3::ValidateLists(ReplicaManager3 *replicaManager) const
    {
        (void) replicaManager;
    }
    //------------------------------------------------------------------------
    void Connection_RM3::SendConstruction(DataStructures::List<Replica3 *> & newObjects,
        DataStructures::List<Replica3 *> & deletedObjects, PRO sendParameters,
            LinkBase * link, Nui8 gid, ReplicaManager3 * replicaManager3)
    {
        if(newObjects.Size() == 0 && deletedObjects.Size() == 0)
            return;

        // All construction and destruction takes place in the same network message
        // Otherwise, if objects rely on each other being created the same tick to be valid, this won't always be true
        //	DataStructures::List<LastSerializationResult* > serializedObjects;
        NBitCount offsetBStart, offsetBStart2, offsetBEnd;
        Nui32 newListIndex, oldListIndex;
        NetSerializer out;
        NetworkID networkId;
        if (isFirstConstruction)
        {
            out.write((Token)ID_REPLICA_MANAGER_DOWNLOAD_STARTED);
            out.write(gid);
            SerializeOnDownloadStarted(&out);
            link->send(&out, sendParameters.mPacketLevel, PT_ORDER_MAKE_SURE,
                sendParameters.mPacketCh, mAddress, false, sendParameters.sendReceipt);
        }

        out.seekWrite(0);
        out.seekRead(0);
        out.write((Token)ID_REPLICA_MANAGER_CONSTRUCTION);
        out.write(gid);
        Nui16 objectSize = (Nui16)newObjects.Size();
        out.write(objectSize);

        // Construction
        for (newListIndex = 0; newListIndex < newObjects.Size(); ++newListIndex)
        {
            offsetBStart = out.getWriteOffset();
            out.write(offsetBStart); // overwritten to point to the end of the stream
            networkId = newObjects[newListIndex]->GetNetworkID();
            out.write(networkId);

            RM3ConstructionState cs = newObjects[newListIndex]->QueryConstruction(this, replicaManager3);
            bool actuallyCreateObject = cs==RM3CS_SEND_CONSTRUCTION;
            out.write(actuallyCreateObject);
            out.writeAlign();

            if(actuallyCreateObject)
            {
                // Actually create the object
                out.write(newObjects[newListIndex]->creatingSystemGUID);
                offsetBStart2 = out.getWriteOffset();
                out.write(offsetBStart2); // overwritten to point to after the call to WriteAllocationID
                out.writeAlign(); // Give the user an aligned bitStream in case they use memcpy
                newObjects[newListIndex]->WriteAllocationID(this, &out);
                out.writeAlign();  // Give the user an aligned bitStream in case they use memcpy
                offsetBEnd = out.getWriteOffset();
                out.seekWrite(offsetBStart2);
                out.write(offsetBEnd);
                out.seekWrite(offsetBEnd);
                newObjects[newListIndex]->SerializeConstruction(&out, this);
            }
            else
            {
                newObjects[newListIndex]->SerializeConstructionExisting(&out, this);
            }

            out.writeAlign();
            offsetBEnd = out.getWriteOffset();
            out.seekWrite(offsetBStart);
            out.write(offsetBEnd);
            out.seekWrite(offsetBEnd);
        }

        NetSerializer bsOut2;
        for(newListIndex = 0; newListIndex < newObjects.Size(); ++newListIndex)
        {
            bsOut2.seekWrite(0);
            bsOut2.seekRead(0);
            RM3ConstructionState cs = newObjects[newListIndex]->QueryConstruction(this, replicaManager3);
            if (cs == RM3CS_SEND_CONSTRUCTION)
            {
                newObjects[newListIndex]->PostSerializeConstruction(&bsOut2, this);
            }
            else
            {
                N_assert(cs == RM3CS_ALREADY_EXISTS_REMOTELY);
                newObjects[newListIndex]->PostSerializeConstructionExisting(&bsOut2, this);
            }
            if(bsOut2.getWriteOffset() > 0)
            {
                out.write1();
                out.writeAlign();
                offsetBStart = out.getWriteOffset();
                out.write(offsetBStart); // overwritten to point to the end of the stream
                networkId = newObjects[newListIndex]->GetNetworkID();
                out.write(networkId);
                out.writeAlign(); // Give the user an aligned bitStream in case they use memcpy
                out.write(&bsOut2);
                out.writeAlign(); // Give the user an aligned bitStream in case they use memcpy
                offsetBEnd = out.getWriteOffset();
                out.seekWrite(offsetBStart);
                out.write(offsetBEnd);
                out.seekWrite(offsetBEnd);
            }
            else
                out.write0();
        }
        out.writeAlign();

        // Destruction
        objectSize = (Nui16) deletedObjects.Size();
        out.write(objectSize);
        for(oldListIndex = 0; oldListIndex < deletedObjects.Size(); ++oldListIndex)
        {
            networkId = deletedObjects[oldListIndex]->GetNetworkID();
            out.write(networkId);
            offsetBStart = out.getWriteOffset();
            out.write(offsetBStart);
            deletedObjects[oldListIndex]->deletingSystemGUID = link->getGUID((Address *)0);
            out.write(deletedObjects[oldListIndex]->deletingSystemGUID);
            deletedObjects[oldListIndex]->SerializeDestruction(&out, this);
            out.writeAlign();
            offsetBEnd = out.getWriteOffset();
            out.seekWrite(offsetBStart);
            out.write(offsetBEnd);
            out.seekWrite(offsetBEnd);
        }
        link->send(&out, sendParameters.mPacketLevel, PT_ORDER_MAKE_SURE,
            sendParameters.mPacketCh, mAddress, false, sendParameters.sendReceipt);

        // TODO - shouldn't this be part of construction?

        // Initial Download serialize to a new system
        // Immediately send serialize after construction if the replica object already has saved data
        // If the object was serialized identically, and does not change later on, then the new connection never gets the data
        SerializeParameters sp;
        sp.whenLastSerialized = 0;
        NetSerializer emptyBs;
        for(int index = 0; index < RM3_NUM_OUTPUT_BITSTREAM_CHANNELS; ++index)
        {
            sp.lastSentBitstream[index] = &emptyBs;
            sp.pro[index] = sendParameters;
            sp.pro[index].mPacketType = PT_ORDER_MAKE_SURE;
        }

        sp.bitsWrittenSoFar = 0;

        for(newListIndex = 0; newListIndex < newObjects.Size(); ++newListIndex)
        {
            sp.destinationConnection = this;
            sp.messageTimestamp = 0;
            Replica3 * replica = newObjects[newListIndex];
            // 8/22/09 Forgot 
            for(int z = 0; z < RM3_NUM_OUTPUT_BITSTREAM_CHANNELS; ++z)
            {
                sp.outputBitstream[z].seekWrite(0);
            }

            RM3SerializationResult res = replica->read(&sp);
            if(res != RM3SR_NEVER_SERIALIZE_FOR_THIS_CONNECTION &&
                res != RM3SR_DO_NOT_SERIALIZE && res != RM3SR_SERIALIZED_UNIQUELY)
            {
                bool allIndices[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS];
                for(int z = 0; z < RM3_NUM_OUTPUT_BITSTREAM_CHANNELS; ++z)
                {
                    sp.bitsWrittenSoFar += sp.outputBitstream[z].getWriteOffset();
                    allIndices[z] = true;
                }
                SendSerialize(replica, allIndices, sp.outputBitstream,
                    sp.messageTimestamp, sp.pro, link, gid, N_Engine().getTimer().getMS());

            }
            // else wait for construction request accepted before serializing
        }

        if(isFirstConstruction)
        {
            out.seekWrite(0);
            out.seekRead(0);
            out.write((Token)ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE);
            out.write(gid);
            SerializeOnDownloadComplete(&out);
            link->send(&out, sendParameters.mPacketLevel, PT_ORDER_MAKE_SURE,
                sendParameters.mPacketCh, mAddress, false, sendParameters.sendReceipt);
        }
        isFirstConstruction=false;
    }
    //------------------------------------------------------------------------
    void Connection_RM3::SendValidation(LinkBase * link, WorldId gid)
    {
        // Hijack to mean sendValidation
        NetSerializer out;
        out.write((Token)ID_REPLICA_MANAGER_SCOPE_CHANGE);
        out.write(gid);
        link->send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0, mAddress, false);
    }
    //------------------------------------------------------------------------
    Replica3::Replica3()
    {
        creatingSystemGUID=UniqueID::INVALID;
        deletingSystemGUID=UniqueID::INVALID;
        replicaManager=0;
        forceSendUntilNextUpdate=false;
        lsr=0;
        referenceIndex = (Nui32)-1;
    }
    //------------------------------------------------------------------------
    Replica3::~Replica3()
    {
        if (replicaManager)
        {
            replicaManager->Dereference(this);
        }
    }
    //------------------------------------------------------------------------
    void Replica3::BroadcastDestruction(void)
    {
        replicaManager->BroadcastDestruction(this, 0);
    }
    //------------------------------------------------------------------------
    UniqueID Replica3::GetCreatingSystemGUID(void) const
    {
        return creatingSystemGUID;
    }
    //------------------------------------------------------------------------
    RM3ConstructionState Replica3::QueryConstruction_ClientConstruction(Connection_RM3 *, bool isThisTheServer)
    {
        if(creatingSystemGUID == replicaManager->getPrc()->getGUID((Address *)0))
            return RM3CS_SEND_CONSTRUCTION;
        // send back to the owner client too, because they couldn't assign the network ID
        if (isThisTheServer)
            return RM3CS_SEND_CONSTRUCTION;
        return RM3CS_NEVER_CONSTRUCT;
    }
    //------------------------------------------------------------------------
    bool Replica3::QueryRemoteConstruction_ClientConstruction(Connection_RM3 *, bool)
    {
        // OK to create
        return true;
    }
    //------------------------------------------------------------------------
    RM3ConstructionState Replica3::QueryConstruction_ServerConstruction(Connection_RM3 *, bool isThisTheServer)
    {
        if (isThisTheServer)
            return RM3CS_SEND_CONSTRUCTION;
        return RM3CS_NEVER_CONSTRUCT;
    }
    //------------------------------------------------------------------------
    bool Replica3::QueryRemoteConstruction_ServerConstruction(Connection_RM3 *, bool isThisTheServer)
    {
        if (isThisTheServer)
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    RM3ConstructionState Replica3::QueryConstruction_PeerToPeer(Connection_RM3 *, Replica3P2PMode p2pMode)
    {
        if(p2pMode == R3P2PM_SINGLE_OWNER)
        {
            // We send to all, others do nothing
            if (creatingSystemGUID == replicaManager->getPrc()->getGUID((Address *)0))
                return RM3CS_SEND_CONSTRUCTION;

            // RM3CS_NEVER_CONSTRUCT will not send the object, and will not read() it
            return RM3CS_NEVER_CONSTRUCT;
        }
        else if (p2pMode==R3P2PM_MULTI_OWNER_CURRENTLY_AUTHORITATIVE)
        {
            return RM3CS_SEND_CONSTRUCTION;
        }
        else if (p2pMode==R3P2PM_STATIC_OBJECT_CURRENTLY_AUTHORITATIVE)
        {
            return RM3CS_ALREADY_EXISTS_REMOTELY;
        }
        else if (p2pMode==R3P2PM_STATIC_OBJECT_NOT_CURRENTLY_AUTHORITATIVE)
        {
            return RM3CS_ALREADY_EXISTS_REMOTELY_DO_NOT_CONSTRUCT;
        }
        else
        {
            N_assert(p2pMode==R3P2PM_MULTI_OWNER_NOT_CURRENTLY_AUTHORITATIVE);

            // RM3CS_ALREADY_EXISTS_REMOTELY will not send the object, but WILL call QuerySerialization() and read() on it.
            return RM3CS_ALREADY_EXISTS_REMOTELY;
        }
    }
    //------------------------------------------------------------------------
    bool Replica3::QueryRemoteConstruction_PeerToPeer(Connection_RM3 * sourceConnection)
    {
        (void) sourceConnection;

        return true;
    }
    //------------------------------------------------------------------------
    RM3QuerySerializationResult Replica3::QuerySerialization_ClientSerializable(Connection_RM3 *destinationConnection, bool isThisTheServer)
    {
        // Owner client sends to all
        if (creatingSystemGUID==replicaManager->getPrc()->getGUID((Address *)0))
            return RM3QSR_CALL_SERIALIZE;
        // Server sends to all but owner client
        if (isThisTheServer && destinationConnection->GetRakNetGUID()!=creatingSystemGUID)
            return RM3QSR_CALL_SERIALIZE;
        // Remote clients do not send
        return RM3QSR_NEVER_CALL_SERIALIZE;
    }
    //------------------------------------------------------------------------
    RM3QuerySerializationResult Replica3::QuerySerialization_ServerSerializable(Connection_RM3 *, bool isThisTheServer)
    {
        // Server sends to all
        if (isThisTheServer)
            return RM3QSR_CALL_SERIALIZE;

        // Clients do not send
        return RM3QSR_NEVER_CALL_SERIALIZE;
    }
    //------------------------------------------------------------------------
    RM3QuerySerializationResult Replica3::QuerySerialization_PeerToPeer(Connection_RM3 *, Replica3P2PMode p2pMode)
    {
        if(p2pMode == R3P2PM_SINGLE_OWNER)
        {
            // Owner peer sends to all
            if (creatingSystemGUID == replicaManager->getPrc()->getGUID((Address *)0))
                return RM3QSR_CALL_SERIALIZE;

            // Remote peers do not send
            return RM3QSR_NEVER_CALL_SERIALIZE;
        }
        else if(p2pMode == R3P2PM_MULTI_OWNER_CURRENTLY_AUTHORITATIVE)
        {
            return RM3QSR_CALL_SERIALIZE;
        }
        else if(p2pMode == R3P2PM_STATIC_OBJECT_CURRENTLY_AUTHORITATIVE)
        {
            return RM3QSR_CALL_SERIALIZE;
        }
        else if(p2pMode == R3P2PM_STATIC_OBJECT_NOT_CURRENTLY_AUTHORITATIVE)
        {
            return RM3QSR_DO_NOT_CALL_SERIALIZE;
        }
        else
        {
            N_assert(p2pMode == R3P2PM_MULTI_OWNER_NOT_CURRENTLY_AUTHORITATIVE);
            return RM3QSR_DO_NOT_CALL_SERIALIZE;
        }
    }
    //------------------------------------------------------------------------
    RM3ActionOnPopConnection Replica3::QueryActionOnPopConnection_Client(Connection_RM3 *droppedConnection) const
    {
        (void) droppedConnection;
        return RM3AOPC_DELETE_REPLICA;
    }
    //------------------------------------------------------------------------
    RM3ActionOnPopConnection Replica3::QueryActionOnPopConnection_Server(Connection_RM3 *droppedConnection) const
    {
        (void) droppedConnection;
        return RM3AOPC_DELETE_REPLICA_AND_BROADCAST_DESTRUCTION;
    }
    //------------------------------------------------------------------------
    RM3ActionOnPopConnection Replica3::QueryActionOnPopConnection_PeerToPeer(Connection_RM3 *droppedConnection) const
    {
        (void) droppedConnection;
        return RM3AOPC_DELETE_REPLICA;
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*
