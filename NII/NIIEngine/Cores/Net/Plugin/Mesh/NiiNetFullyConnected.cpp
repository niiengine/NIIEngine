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
#if _RAKNET_SUPPORT_FullyConnectedMesh2 == 1

#include "NiiPreProcess.h"
#include "NiiNetFullyConnected.h"
#include "NiiNetLinkBase.h"
#include "NiiNetCommon.h"
#include "NiiNetSerializer.h"
#include "Rand.h"
#include "DS_OrderedList.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    int FCM2ParticipantComp(const FullyConnected::FCM2Participant & key,
        const FullyConnected::FCM2Participant & data)
    {
        if (key.fcm2Guid < data.fcm2Guid)
            return -1;
        if (key.fcm2Guid > data.fcm2Guid)
            return 1;
        return 0;
    }
    //------------------------------------------------------------------------
    FullyConnected::FullyConnected()
    {
        startupTime = 0;
        totalConnectionCount = 0;
        ourFCMGuid = 0;
        autoParticipateConnections = true;
        connectOnNewRemoteConnections = true;

        hostRakNetGuid=UniqueID::INVALID;
    }
    //------------------------------------------------------------------------
    FullyConnected::~FullyConnected()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    UniqueID FullyConnected::GetConnectedHost(void) const
    {
        if(ourFCMGuid == 0)
            return UniqueID::INVALID;
            
        return hostRakNetGuid;
    }
    //------------------------------------------------------------------------
    Address * FullyConnected::GetConnectedHostAddr(void) const
    {
        if(ourFCMGuid == 0)
            return 0;
            
        return mPrc2->getAddress(hostRakNetGuid);
    }
    //------------------------------------------------------------------------
    UniqueID FullyConnected::GetHostSystem(void) const
    {
        if(ourFCMGuid == 0)
            return mPrc2->getGUID((Address *)0);

        return hostRakNetGuid;
    }
    //------------------------------------------------------------------------
    bool FullyConnected::IsHostSystem(void) const
    {
        return GetHostSystem() == mPrc2->getGUID((Address *)0);
    }
    //------------------------------------------------------------------------
    void FullyConnected::GetHostOrder(DataStructures::List<UniqueID> & hostList)
    {
        hostList.Clear(true);

        if(ourFCMGuid == 0 || fcm2ParticipantList.Size() == 0)
        {
            hostList.Push(mPrc2->getGuid());
            return;
        }

        FCM2Participant fcm2;
        fcm2.fcm2Guid = ourFCMGuid;
        fcm2.mID = mPrc2->getGuid();

        DataStructures::OrderedList<FCM2Participant, FCM2Participant, FCM2ParticipantComp> olist;
        olist.Insert(fcm2, fcm2, true);
        for(Nui32 i = 0; i < fcm2ParticipantList.Size(); ++i)
            olist.Insert(fcm2ParticipantList[i], fcm2ParticipantList[i], true);

        for (Nui32 i = 0; i < olist.Size(); ++i)
        {
            hostList.Push(olist[i].mID);
        }
    }
    //------------------------------------------------------------------------
    bool FullyConnected::IsConnectedHost(void) const
    {
        return GetConnectedHost()==mPrc2->getGUID((Address *)0);
    }
    //------------------------------------------------------------------------
    void FullyConnected::SetAutoparticipateConnections(bool b)
    {
        autoParticipateConnections=b;
    }
    //------------------------------------------------------------------------
    void FullyConnected::ResetHostCalculation(void)
    {
        hostRakNetGuid=UniqueID::INVALID;
        startupTime = N_Engine().getTimer().getUS();
        totalConnectionCount = 0;
        ourFCMGuid = 0;
        for (Nui32 i = 0; i < fcm2ParticipantList.Size(); i++)
            SendFCMGuidRequest(fcm2ParticipantList[i].mID);
    }
    //------------------------------------------------------------------------
    bool FullyConnected::AddParticipantInternal( UniqueID id, FCM2Guid theirFCMGuid )
    {
        for (Nui32 i=0; i < fcm2ParticipantList.Size(); i++)
        {
            if (fcm2ParticipantList[i].mID==id)
            {
                if (theirFCMGuid!=0)
                    fcm2ParticipantList[i].fcm2Guid=theirFCMGuid;
                return false;
            }
        }

        FCM2Participant participant;
        participant.mID=id;
        participant.fcm2Guid=theirFCMGuid;
        fcm2ParticipantList.Push(participant);

        SendFCMGuidRequest(id);

        return true;
    }
    //------------------------------------------------------------------------
    void FullyConnected::AddParticipant(UniqueID id)
    {
        if(mPrc2->getState(mPrc2->getAddress(id)) != IS_CONNECTED)
        {
            String temp;
            id.read(temp);
    #ifdef DEBUG_FCM2
            printf("AddParticipant to %s failed (not connected)\n", temp);
    #endif
            return;
        }

        AddParticipantInternal(id, 0);
    }
    //------------------------------------------------------------------------
    void FullyConnected::GetParticipantList(DataStructures::List<UniqueID> &participantList)
    {
        participantList.Clear(true);
        Nui32 i;
        for (i=0; i < fcm2ParticipantList.Size(); i++)
            participantList.Push(fcm2ParticipantList[i].mID);
    }
    //------------------------------------------------------------------------
    bool FullyConnected::HasParticipant(UniqueID participantGuid)
    {
        Nui32 i;
        for (i=0; i < fcm2ParticipantList.Size(); i++)
        {
            if (fcm2ParticipantList[i].mID==participantGuid)
                return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    ConnectReceiveType FullyConnected::onMessage(SocketMessage * msg)
    {
        switch (msg->mData[0])
        {
        case ID_REMOTE_NEW_INCOMING_CONNECTION:
            {
                if (connectOnNewRemoteConnections)
                    ConnectToRemoteNewIncomingConnections(msg);
            }
            break;
        case ID_FCM2_REQUEST_FCMGUID:
            OnRequestFCMGuid(msg);
            return CRT_Destroy;
        case ID_FCM2_RESPOND_CONNECTION_COUNT:
            OnRespondConnectionCount(msg);
            return CRT_Destroy;
        case ID_FCM2_INFORM_FCMGUID:
            OnInformFCMGuid(msg);
            return CRT_Destroy;
        case ID_FCM2_UPDATE_MIN_TOTAL_CONNECTION_COUNT:
            OnUpdateMinTotalConnectionCount(msg);
            return CRT_Destroy;
        case ID_FCM2_NEW_HOST:
            if (msg->mLocal == false)
                return CRT_Destroy;
            break;
        case ID_FCM2_VERIFIED_JOIN_START:
            return OnVerifiedJoinStart(msg);
        case ID_FCM2_VERIFIED_JOIN_CAPABLE:
            return OnVerifiedJoinCapable(msg);
        case ID_FCM2_VERIFIED_JOIN_FAILED:
            OnVerifiedJoinFailed(msg->mID, true);
            return CRT_Normal;
        case ID_FCM2_VERIFIED_JOIN_ACCEPTED:
            if (msg->mLocal == false)
                OnVerifiedJoinAccepted(msg);
            return CRT_Normal;
        case ID_FCM2_VERIFIED_JOIN_REJECTED:
            OnVerifiedJoinRejected(msg);
            return CRT_Normal;

        case ID_NAT_TARGET_UNRESPONSIVE:
        case ID_NAT_TARGET_NOT_CONNECTED:
        case ID_NAT_CONNECTION_TO_TARGET_LOST:
            {
                UniqueID g;
                NetSerializer b(msg->mData, msg->mLength, false);
                b.skipRead(1, 0);
                b.read(g);
                UpdateVerifiedJoinInProgressMember(g, UniqueID::INVALID, JIPS_FAILED);
                return CRT_Normal;
            }
        case ID_NAT_PUNCHTHROUGH_FAILED:
            UpdateVerifiedJoinInProgressMember(msg->mID, UniqueID::INVALID, JIPS_FAILED);
            return CRT_Normal;
        }

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void FullyConnected::init()
    {
        Clear();
        startupTime = N_Engine().getTimer().getUS();
    }
    //------------------------------------------------------------------------
    void FullyConnected::onAttach()
    {
        Clear();
        // In case init() was called first
        if (mPrc2->isActive())
            startupTime = N_Engine().getTimer().getUS();
    }
    //------------------------------------------------------------------------
    void FullyConnected::shutdown()
    {
        Clear();
        startupTime = 0;
    }
    //------------------------------------------------------------------------
    void FullyConnected::onConnectClose(const Address & address, UniqueID id,
        ConnectCloseType lostConnectionReason)
    {
        (void) lostConnectionReason;
        Nui32 idx = 0;
        while(idx < joinsInProgress.Size())
        {
            if(joinsInProgress[idx]->requester == id)
            {
                SocketMessage * p = createMessage(sizeof(Token) + sizeof(Nui8));
                p->mData[0] = ID_FCM2_VERIFIED_JOIN_FAILED;
                p->mAddress = address;
                p->mAddress.mIndex = (Nindex)-1;
                p->mID = id;
                p->mLocal = true;
                mPrc2->addMessage(p, true);

                N_delete joinsInProgress[idx];
                joinsInProgress.RemoveAtIndex(idx);
            }
            else
            {
                ++idx;
            }
        }

        UpdateVerifiedJoinInProgressMember(id, UniqueID::INVALID, JIPS_FAILED);

        for(idx = 0; idx < fcm2ParticipantList.Size(); ++idx)
        {
            if(fcm2ParticipantList[idx].mID == id)
            {
                String temp;
                fcm2ParticipantList[idx] = fcm2ParticipantList[fcm2ParticipantList.Size()-1];
                fcm2ParticipantList[fcm2ParticipantList.Size()-1].mID.read(temp);
    #ifdef DEBUG_FCM2
                printf("Popping participant %s\n", temp);
    #endif

                fcm2ParticipantList.Pop();
                if(id == hostRakNetGuid && ourFCMGuid != 0)
                {
                    if(fcm2ParticipantList.Size() == 0)
                    {
                        hostRakNetGuid = mPrc2->getGuid();
                        hostFCM2Guid = ourFCMGuid;
                    }
                    else
                    {
                        CalculateHost(&hostRakNetGuid, &hostFCM2Guid);
                    }
                    PushNewHost(hostRakNetGuid, id);
                }
                return;
            }
        }
    }
    //------------------------------------------------------------------------
    TimeDurUS FullyConnected::GetElapsedRuntime(void)
    {
        TimeDurUS curTime=N_Engine().getTimer().getUS();
        if (curTime>startupTime)
            return curTime-startupTime;
        else
            return 0;
    }
    //------------------------------------------------------------------------
    void FullyConnected::onConnect(const Address & address, UniqueID id)
    {
        (void) address;

        UpdateVerifiedJoinInProgressMember(id, id, JIPS_CONNECTED);

        if (autoParticipateConnections)
            AddParticipant(id);
    }
    //------------------------------------------------------------------------
    void FullyConnected::onConnectFail(SocketMessage * msg, ConnectFailType failedConnectionAttemptReason)
    {
        if (failedConnectionAttemptReason == CFT_Duplicate)
        {
            UpdateVerifiedJoinInProgressMember(msg->mID, msg->mID, JIPS_CONNECTED);
        }
        else
        {
            UpdateVerifiedJoinInProgressMember(msg->mAddress, UniqueID::INVALID, JIPS_FAILED);
        }
    }
    //------------------------------------------------------------------------
    void FullyConnected::Clear(void)
    {
        fcm2ParticipantList.Clear(false);

        for (Nui32 i=0; i < joinsInProgress.Size(); i++)
        {
            N_delete joinsInProgress[i];
        }
        joinsInProgress.Clear(true);

        totalConnectionCount=0;
        ourFCMGuid=0;
        lastPushedHost=UniqueID::INVALID;
    }
    //------------------------------------------------------------------------
    void FullyConnected::PushNewHost(const UniqueID & id, UniqueID oldHost)
    {
        SocketMessage * p = createMessage(sizeof(Token) + sizeof(oldHost));
        NetSerializer bs(p->mData, p->mLength, false);
        bs.seekWrite(0, 0);
        bs.write((Token)ID_FCM2_NEW_HOST);
        bs.write(oldHost);
        p->mAddress = mPrc2->getAddress(id);
        p->mAddress.mIndex = (Nindex)-1;
        p->mID = id;
        p->mLocal = true;
        mPrc2->addMessage(p, true);

        lastPushedHost = id;
    }
    //------------------------------------------------------------------------
    void FullyConnected::SendFCMGuidRequest(UniqueID id)
    {
        if(id == mPrc2->getGUID((Address *)0))
            return;

        NetSerializer out;
        out.write((Token)ID_FCM2_REQUEST_FCMGUID);
        if(ourFCMGuid == 0)
        {
            out.write0();
            out.write(GetElapsedRuntime());
        }
        else
        {
            out.write1();
            out.write(totalConnectionCount);
            out.write(ourFCMGuid);
        }
        mPrc2->send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0, id, false);
    }
    //------------------------------------------------------------------------
    void FullyConnected::SendOurFCMGuid(Address * addr)
    {
        NetSerializer bsOut;
        bsOut.write((Token)ID_FCM2_INFORM_FCMGUID);
        N_assert(ourFCMGuid != 0); // Can't inform others of our FCM2Guid if it's unset!
        bsOut.write(ourFCMGuid);
        bsOut.write(totalConnectionCount);
        mPrc2->send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, addr, false);
    }
    //------------------------------------------------------------------------
    void FullyConnected::SendConnectionCountResponse(Address * addr, Nui32 responseTotalConnectionCount)
    {
        NetSerializer bsOut;
        bsOut.write((Token)ID_FCM2_RESPOND_CONNECTION_COUNT);
        bsOut.write(responseTotalConnectionCount);
        mPrc2->send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, addr, false);
    }
    //------------------------------------------------------------------------
    void FullyConnected::AssignOurFCMGuid(void)
    {
        // Only assigned once ever
        N_assert(ourFCMGuid==0);
        Nui32 randomNumber = randomMT();
        randomNumber ^= (Nui32) (N_Engine().getTimer().getUS() & 0xFFFFFFFF);
        randomNumber ^= (Nui32) (mPrc2->getGUID((Address *)0).g & 0xFFFFFFFF);
        ourFCMGuid |= randomNumber;
        Nui64 reponse64 = totalConnectionCount;
        ourFCMGuid |= reponse64<<32;
    }
    //------------------------------------------------------------------------
    void FullyConnected::CalculateHost(UniqueID *id, FCM2Guid *fcm2Guid)
    {
        // Can't calculate host without knowing our own
        N_assert(ourFCMGuid!=0);

        // Can't calculate host without being connected to anyone else
        N_assert(fcm2ParticipantList.Size()>0);

        // Return the lowest value of all FCM2Guid
        FCM2Guid lowestFCMGuid=ourFCMGuid;
        //	Address associatedSystemAddress=Address::invalid;
        UniqueID associatedRakNetGuid=mPrc2->getGUID((Address *)0);

        Nui32 idx;
        for (idx=0; idx < fcm2ParticipantList.Size(); idx++)
        {
            if (fcm2ParticipantList[idx].fcm2Guid!=0 && fcm2ParticipantList[idx].fcm2Guid<lowestFCMGuid)
            {
                lowestFCMGuid=fcm2ParticipantList[idx].fcm2Guid;
                associatedRakNetGuid=fcm2ParticipantList[idx].mID;
            }
        }

        *id=associatedRakNetGuid;
        *fcm2Guid=lowestFCMGuid;
    }
    //------------------------------------------------------------------------
    void FullyConnected::OnRequestFCMGuid(SocketMessage *msg)
    {
        NetSerializer bsIn(msg->mData,msg->mLength,false);
        bsIn.skipRead(1, 0);
        bool hasRemoteFCMGuid=false;
        bsIn.read(hasRemoteFCMGuid);
        TimeDurUS senderElapsedRuntime=0;
        Nui32 remoteTotalConnectionCount=0;
        FCM2Guid theirFCMGuid=0;
        if (hasRemoteFCMGuid)
        {
            bsIn.read(remoteTotalConnectionCount);
            bsIn.read(theirFCMGuid);
        }
        else
        {
            bsIn.read(senderElapsedRuntime);
        }
        AddParticipantInternal(msg->mID,theirFCMGuid);
        if (ourFCMGuid==0)
        {
            if (hasRemoteFCMGuid==false)
            {
                // Nobody has a fcmGuid

                TimeDurUS ourElapsedRuntime = GetElapsedRuntime();
                if (ourElapsedRuntime>senderElapsedRuntime)
                {
                    // We are probably host
                    SendConnectionCountResponse(msg->mAddress, 2);
                }
                else
                {
                    // They are probably host
                    SendConnectionCountResponse(msg->mAddress, 1);
                }
            }
            else
            {
                // They have a fcmGuid, we do not
                IncrementTotalConnectionCount(remoteTotalConnectionCount+1);

                AssignOurFCMGuid();
                Nui32 idx;
                for (idx=0; idx < fcm2ParticipantList.Size(); idx++)
                    SendOurFCMGuid(mPrc2->getAddress(fcm2ParticipantList[idx].mID));
            }
        }
        else
        {
            if (hasRemoteFCMGuid==false)
            {
                // We have a fcmGuid they do not
                SendConnectionCountResponse(msg->mAddress, totalConnectionCount+1);
            }
            else
            {
                // We both have fcmGuids
                IncrementTotalConnectionCount(remoteTotalConnectionCount);

                SendOurFCMGuid(msg->mAddress);
            }
        }
        CalculateAndPushHost();
    }
    //------------------------------------------------------------------------
    void FullyConnected::OnRespondConnectionCount(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        Nui32 responseTotalConnectionCount;
        bsIn.read(responseTotalConnectionCount);
        IncrementTotalConnectionCount(responseTotalConnectionCount);
        bool wasAssigned;
        if (ourFCMGuid==0)
        {
            wasAssigned=true;
            AssignOurFCMGuid();
        }
        else
            wasAssigned=false;

        // 1 is returned to give us lower level, but the actual minimum is 2
        IncrementTotalConnectionCount(2);

        if (wasAssigned==true)
        {
            NCount idx;
            for (idx = 0; idx < fcm2ParticipantList.Size(); ++idx)
                SendOurFCMGuid(mPrc2->getAddress(fcm2ParticipantList[idx].mID));
            CalculateAndPushHost();
        }
    }
    //------------------------------------------------------------------------
    void FullyConnected::OnInformFCMGuid(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);

        FCM2Guid theirFCMGuid;
        Nui32 theirTotalConnectionCount;
        bsIn.read(theirFCMGuid);
        bsIn.read(theirTotalConnectionCount);
        IncrementTotalConnectionCount(theirTotalConnectionCount);

        if (AddParticipantInternal(msg->mID,theirFCMGuid))
        {
            // 1/19/2010 - Relay increased total connection count in case new participant only connects to part of the mesh
            Nui32 idx;
            NetSerializer bsOut;
            bsOut.write((Token)ID_FCM2_UPDATE_MIN_TOTAL_CONNECTION_COUNT);
            bsOut.write(totalConnectionCount);
            for (idx=0; idx < fcm2ParticipantList.Size(); idx++)
            {
                if (msg->mID!=fcm2ParticipantList[idx].mID)
                    mPrc2->send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, fcm2ParticipantList[idx].mID,false);
            }
        }

        if (ourFCMGuid==0)
        {
            AssignOurFCMGuid();
            Nui32 idx;
            for (idx=0; idx < fcm2ParticipantList.Size(); idx++)
                SendOurFCMGuid(mPrc2->getAddress(fcm2ParticipantList[idx].mID));
        }

        CalculateAndPushHost();
    }
    //------------------------------------------------------------------------
    void FullyConnected::OnUpdateMinTotalConnectionCount(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        Nui32 newMin;
        bsIn.read(newMin);
        IncrementTotalConnectionCount(newMin);
    }
    //------------------------------------------------------------------------
    void FullyConnected::GetParticipantCount(Nui32 *participantListSize) const
    {
        *participantListSize=fcm2ParticipantList.Size();
    }
    //------------------------------------------------------------------------
    Nui32 FullyConnected::GetParticipantCount(void) const
    {
        return fcm2ParticipantList.Size();
    }
    //------------------------------------------------------------------------
    void FullyConnected::CalculateAndPushHost(void)
    {
        UniqueID newHostGuid;
        FCM2Guid newFcmGuid;
        if (ParticipantListComplete())
        {
            CalculateHost(&newHostGuid, &newFcmGuid);
            if (newHostGuid!=lastPushedHost)
            {
                hostRakNetGuid=newHostGuid;
                hostFCM2Guid=newFcmGuid;
                PushNewHost(hostRakNetGuid, lastPushedHost);
            }
        }
    }
    //------------------------------------------------------------------------
    bool FullyConnected::ParticipantListComplete(void)
    {
        for (Nui32 i=0; i < fcm2ParticipantList.Size(); i++)
        {
            if (fcm2ParticipantList[i].fcm2Guid==0)
                return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    void FullyConnected::IncrementTotalConnectionCount(Nui32 i)
    {
        if (i>totalConnectionCount)
        {
            totalConnectionCount=i;
        }
    }
    //------------------------------------------------------------------------
    void FullyConnected::SetConnectOnNewRemoteConnection(bool attemptConnection,
        RakString pw)
    {
        connectOnNewRemoteConnections=attemptConnection;
        connectionPassword=pw;
    }
    //------------------------------------------------------------------------
    void FullyConnected::ConnectToRemoteNewIncomingConnections(SocketMessage * msg)
    {
        Nui32 count;
        NetSerializer in(msg->mData, msg->mLength, false);
        in.skipRead(1, 0);
        in.read(count);
        Address remoteAddress;
        UniqueID remoteGuid;
        String str;
        for(Nui32 i = 0; i < count; ++i)
        {
            in.read(remoteAddress);
            in.read(remoteGuid);
            remoteAddress.read(str, false);
            mPrc2->connect(str.c_str(), remoteAddress.getHSBPort(),
                connectionPassword.C_String(), (int)connectionPassword.GetLength());
        }
    }
    //------------------------------------------------------------------------
    Nui32 FullyConnected::GetTotalConnectionCount(void) const
    {
        return totalConnectionCount;
    }
    //------------------------------------------------------------------------
    void FullyConnected::StartVerifiedJoin(UniqueID client)
    {
        // Assert is because there is no point calling StartVerifiedJoin() if this client is already a participant
        N_assert(HasParticipant(client)==false);
        N_assert(client!=mPrc2->getGuid());

        NetSerializer bsOut;
        bsOut.write((Token) ID_FCM2_VERIFIED_JOIN_START);
        bsOut.write((Nui16)fcm2ParticipantList.Size());
        Nui32 i;
        for (i=0; i < fcm2ParticipantList.Size(); i++)
        {
            bsOut.write(fcm2ParticipantList[i].mID);
            bsOut.write(mPrc2->getAddress(fcm2ParticipantList[i].mID));
        }
        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, client, false);
    }
    //------------------------------------------------------------------------
    void FullyConnected::RespondOnVerifiedJoinCapable(SocketMessage *msg, bool accept, NetSerializer *additionalData)
    {
        VerifiedJoinInProgress vjip;
        DecomposeJoinCapable(msg, &vjip);

        DataStructures::List<UniqueID> participatingMembersOnClientSucceeded;
        DataStructures::List<UniqueID> participatingMembersOnClientFailed;
        DataStructures::List<UniqueID> participatingMembersNotOnClient;
        DataStructures::List<UniqueID> clientMembersNotParticipatingSucceeded;
        DataStructures::List<UniqueID> clientMembersNotParticipatingFailed;
        CategorizeVJIP(&vjip,
            participatingMembersOnClientSucceeded,
            participatingMembersOnClientFailed,
            participatingMembersNotOnClient,
            clientMembersNotParticipatingSucceeded,
            clientMembersNotParticipatingFailed);

        if (participatingMembersNotOnClient.Size() > 0)
        {
            NetSerializer bsOut;
            bsOut.write((Token) ID_FCM2_VERIFIED_JOIN_START);
            bsOut.write((Nui16)participatingMembersNotOnClient.Size());
            NCount i;
            for (i = 0; i < participatingMembersNotOnClient.Size(); ++i)
            {
                bsOut.write(participatingMembersNotOnClient[i]);
                bsOut.write(mPrc2->getAddress(participatingMembersNotOnClient[i]));
            }
            send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, msg->mID, false);
            return;
        }

        N_assert(participatingMembersOnClientFailed.Size()==0);
        N_assert(participatingMembersNotOnClient.Size()==0);

        NetSerializer bsOut;
        if (accept)
        {
            bsOut.write((Token)ID_FCM2_VERIFIED_JOIN_ACCEPTED);
            bsOut.write(msg->mID);

            // Tell client to disconnect from clientMembersNotParticipatingSucceeded
            bsOut.write((Nui16)clientMembersNotParticipatingSucceeded.Size());
            for(Nui32 i = 0; i < clientMembersNotParticipatingSucceeded.Size(); i++)
                bsOut.write(clientMembersNotParticipatingSucceeded[i]);

            // Tell client to call AddParticipant() for participatingMembersOnClientSucceeded
            bsOut.write((Nui16)participatingMembersOnClientSucceeded.Size());
            for (NCount i = 0; i < participatingMembersOnClientSucceeded.Size(); ++i)
                bsOut.write(participatingMembersOnClientSucceeded[i]);

            if(additionalData)
                bsOut.write(additionalData);

            for(NCount i = 0; i < fcm2ParticipantList.Size(); ++i)
                send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, fcm2ParticipantList[i].mID, false);

            // Process immediately
            // This is so if another ID_FCM2_VERIFIED_JOIN_CAPABLE is buffered, it responds with ID_FCM2_VERIFIED_JOIN_START
            AddParticipant(msg->mID);

            SocketMessage * p = createMessage(bsOut.getSize());
            memcpy(p->mData, bsOut.getData(), bsOut.getSize());
            p->mAddress = msg->mAddress;
            p->mAddress.mIndex = (Nindex)-1;
            p->mID = msg->mID;
            p->mLocal = true;
            mPrc2->addMessage(p, true);
        }
        else
        {
            // Tell client rejected, otherwise process the same as ID_FCM2_VERIFIED_JOIN_FAILED
            bsOut.write((Token)ID_FCM2_VERIFIED_JOIN_REJECTED);
            if (additionalData)
                bsOut.write(additionalData);
        }

        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, msg->mID, false);
    }
    //------------------------------------------------------------------------
    void FullyConnected::GetVerifiedJoinRequiredProcessingList(UniqueID host, DataStructures::List<Address> &addresses, DataStructures::List<UniqueID> &guids)
    {
        addresses.Clear(true);
        guids.Clear(true);

        Nui32 curIndex = GetJoinsInProgressIndex(host);
        if (curIndex!=(Nui32) -1)
        {
            VerifiedJoinInProgress *vjip = joinsInProgress[curIndex];
            Nui32 j;
            for (j=0; j < vjip->members.Size(); j++)
            {
                if (vjip->members[j].joinInProgressState==JIPS_PROCESSING)
                {
                    addresses.Push(vjip->members[j].mAddress);
                    guids.Push(vjip->members[j].mID);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void FullyConnected::GetVerifiedJoinAcceptedAdditionalData(SocketMessage * msg,
        bool * thisSystemAccepted, DataStructures::List<UniqueID> & systemsAccepted,
            NetSerializer * additionalData)
    {
        systemsAccepted.Clear(true);

        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        UniqueID systemToAddGuid;
        bsIn.read(systemToAddGuid);
        *thisSystemAccepted = systemToAddGuid == mPrc2->getGuid();
        Nui16 listSize;
        bsIn.read(listSize);
        bsIn.skipRead(listSize * UniqueID::size(), 0);
        bsIn.read(listSize);
        if (systemToAddGuid == mPrc2->getGuid())
        {
            for (NCount i = 0; i < listSize; ++i)
            {
                bsIn.read(systemToAddGuid);
                systemsAccepted.Push(systemToAddGuid);
            }
            systemsAccepted.Push(msg->mID);
        }
        else
        {
            systemsAccepted.Push(systemToAddGuid);
            bsIn.skipRead(listSize * UniqueID::size(), 0);
        }
        if (additionalData)
        {
            additionalData->Reset();
            additionalData->write(bsIn);
        }
    }
    //------------------------------------------------------------------------
    void FullyConnected::GetVerifiedJoinRejectedAdditionalData(SocketMessage * msg,
        NetSerializer * additionalData)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        if (additionalData)
        {
            additionalData->Reset();
            additionalData->write(bsIn);
        }
    }
    //------------------------------------------------------------------------
    ConnectReceiveType FullyConnected::OnVerifiedJoinStart(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);

        Nui16 listSize;
        bsIn.read(listSize);

        Nui32 curIndex = GetJoinsInProgressIndex(msg->mID);
        if (curIndex != (Nui32) -1)
        {
            // Got update to existing list
            VerifiedJoinInProgress *vjip = joinsInProgress[curIndex];

            for (Nui32 i=0; i < vjip->members.Size(); i++)
            {
                vjip->members[i].workingFlag=false;
            }

            // Server has updated list of participants
            for (Nui16 i=0; i < listSize; i++)
            {
                VerifiedJoinInProgressMember vjipm;
                ReadVerifiedJoinInProgressMember(&bsIn, &vjipm);

                Nui32 j;
                if (vjipm.mID!=UniqueID::INVALID)
                    j = GetVerifiedJoinInProgressMemberIndex(vjipm.mID, vjip);
                else
                    j = GetVerifiedJoinInProgressMemberIndex(vjipm.mAddress, vjip);
                if (j==(Nui32)-1)
                {
                    // New
                    vjipm.workingFlag=true;
                    vjipm.joinInProgressState=JIPS_PROCESSING;
                    vjip->members.Push(vjipm);

                    // Allow resend of ID_FCM2_VERIFIED_JOIN_CAPABLE
                    //vjip->sentResults=false;
                }
                else
                {
                    vjip->members[j].workingFlag=true;
                }
            }

            for (Nui32 i=0; i < vjip->members.Size(); i++)
            {
                if (vjip->members[i].workingFlag==false)
                    vjip->members[i].joinInProgressState=JIPS_UNNECESSARY;
            }

            if (ProcessVerifiedJoinInProgressIfCompleted(vjip))
            {
                // Completed
                return CRT_Destroy;
            }

            // Else tell user about new list
            return CRT_Normal;
        }

        VerifiedJoinInProgress * vjip = N_new VerifiedJoinInProgress();
        vjip->requester = msg->mID;
        if (listSize == 0)
        {
            // send back result
            NetSerializer bsOut;
            bsOut.write((Token)ID_FCM2_VERIFIED_JOIN_CAPABLE);
            bsOut.write((Nui16)0);
            send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, msg->mID, false);
            //vjip->sentResults=true;
            joinsInProgress.Push(vjip);
            return CRT_Destroy;
        }

        for (Nui16 i=0; i < listSize; i++)
        {
            VerifiedJoinInProgressMember vjipm;
            ReadVerifiedJoinInProgressMember(&bsIn, &vjipm);
            vjip->members.Push(vjipm);
        }
        joinsInProgress.Push(vjip);
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void FullyConnected::SkipToVJCUserData(NetSerializer * in)
    {
        in->skipRead(1, 0);
        Nui16 listSize;
        in->read(listSize);
        for(NCount i=0; i < listSize; ++i)
        {
            in->skipRead(UniqueID::size(), 0);
#if(N_NET_PROTOCOL_VER == 4)
            IPV4Address temp;
#else
            IPV46ddress temp;
#endif
            in->skipRead(temp.getSize(), 0);
        }
    }
    //------------------------------------------------------------------------
    void FullyConnected::DecomposeJoinCapable(SocketMessage * msg, VerifiedJoinInProgress * vjip)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);

        Nui16 listSize;
        bsIn.read(listSize);

        for(Nui16 i = 0; i < listSize; ++i)
        {
            VerifiedJoinInProgressMember member;
            bsIn.read(member.mID);
            bsIn.read(member.mAddress);
            bsIn.read((Nui8)member.joinInProgressState);
            member.workingFlag = false;
            vjip->members.Push(member);
        }
    }
    //------------------------------------------------------------------------
    ConnectReceiveType FullyConnected::OnVerifiedJoinCapable(SocketMessage *msg)
    {
        VerifiedJoinInProgress vjip;
        DecomposeJoinCapable(msg, &vjip);

        // If this N_assert hits, AddParticipant() was called on this system, or another system, which it should not have been.
        N_assert(HasParticipant(msg->mID)==false);

        DataStructures::List<UniqueID> participatingMembersOnClientSucceeded;
        DataStructures::List<UniqueID> participatingMembersOnClientFailed;
        DataStructures::List<UniqueID> participatingMembersNotOnClient;
        DataStructures::List<UniqueID> clientMembersNotParticipatingSucceeded;
        DataStructures::List<UniqueID> clientMembersNotParticipatingFailed;
        CategorizeVJIP(&vjip,
            participatingMembersOnClientSucceeded,
            participatingMembersOnClientFailed,
            participatingMembersNotOnClient,
            clientMembersNotParticipatingSucceeded,
            clientMembersNotParticipatingFailed);

        if (participatingMembersOnClientFailed.Size() > 0)
        {
            // send ID_FCM2_VERIFIED_JOIN_FAILED with GUIDs to disconnect
            NetSerializer bsOut;
            bsOut.write((Token) ID_FCM2_VERIFIED_JOIN_FAILED);
            send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, msg->mID, false);
            return CRT_Destroy;
        }

        if(participatingMembersNotOnClient.Size() > 0)
        {
            NetSerializer bsOut;
            bsOut.write((Token) ID_FCM2_VERIFIED_JOIN_START);
            bsOut.write((Nui16)participatingMembersNotOnClient.Size());
            NCount i;
            for (i = 0; i < participatingMembersNotOnClient.Size(); ++i)
            {
                bsOut.write(participatingMembersNotOnClient[i]);
                bsOut.write(mPrc2->getAddress(participatingMembersNotOnClient[i]));
            }
            send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, msg->mID, false);
            return CRT_Destroy;
        }

        // Let server decide if to accept or reject via RespondOnVerifiedJoinCapable
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void FullyConnected::OnVerifiedJoinFailed(UniqueID hostGuid, bool callCloseConnection)
    {
        Nui32 curIndex = GetJoinsInProgressIndex(hostGuid);
        if (curIndex==(Nui32) -1)
            return;

        if (callCloseConnection)
        {
            VerifiedJoinInProgress *vjip = joinsInProgress[curIndex];
            for (Nui32 j=0; j < vjip->members.Size(); j++)
            {
                if (vjip->members[j].joinInProgressState!=JIPS_FAILED)
                {
                    mPrc2->close(vjip->members[j].mID, true);
                }
            }
        }
        // Clear joinsInProgress for msg->mID
        N_delete joinsInProgress[curIndex];
        joinsInProgress.RemoveAtIndex(curIndex);
    }
    //------------------------------------------------------------------------
    void FullyConnected::OnVerifiedJoinAccepted(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);

        UniqueID systemToAddGuid;
        bsIn.read(systemToAddGuid);

        if (systemToAddGuid==mPrc2->getGuid())
        {
            // My own system
            Nui32 curIndex = GetJoinsInProgressIndex(msg->mID);
            if (curIndex==(Nui32)-1)
                return;

            Nui16 listSize;
            bsIn.read(listSize);
            for (Nui16 i=0; i < listSize; i++)
            {
                // List of clientMembersNotParticipatingSucceeded
                UniqueID id;
                bsIn.read(id);
                mPrc2->close(id, true);
            }

            bsIn.read(listSize);
            for (Nui16 i=0; i < listSize; i++)
            {
                // List of participatingMembersOnClientSucceeded
                UniqueID id;
                bsIn.read(id);
                AddParticipant(id);
            }
            AddParticipant(msg->mID);

            // Clear joinsInProgress for msg->mID
            N_delete joinsInProgress[curIndex];
            joinsInProgress.RemoveAtIndex(curIndex);
        }
        else
        {
            // Another system
            ConnectionState cs = mPrc2->getState(systemToAddGuid);
            N_assert(cs == IS_CONNECTED);
            if (cs == IS_CONNECTED)
                AddParticipant(systemToAddGuid);
        }
    }
    //------------------------------------------------------------------------
    void FullyConnected::OnVerifiedJoinRejected(SocketMessage * msg)
    {
        OnVerifiedJoinFailed(msg->mID, true);
    }
    //------------------------------------------------------------------------
    Nui32 FullyConnected::GetJoinsInProgressIndex(UniqueID requester) const
    {
        for (Nui32 i=0; i < joinsInProgress.Size(); i++)
        {
            if (joinsInProgress[i]->requester==requester)
                return i;
        }
        return (Nui32) -1;
    }
    //------------------------------------------------------------------------
    void FullyConnected::UpdateVerifiedJoinInProgressMember(const UniqueAddress usys, UniqueID guidToAssign, FullyConnected::JoinInProgressState newState)
    {
        bool anythingChanged;

        for(NCount i = 0; i < joinsInProgress.Size(); ++i)
        {
            VerifiedJoinInProgress * vjip = joinsInProgress[i];
            //if (vjip->sentResults==true)
            //	continue;
            anythingChanged = false;

            Nui32 j;
            j = GetVerifiedJoinInProgressMemberIndex(usys, vjip);
            if (j != (Nui32)-1)
            {
                if (vjip->members[j].mID == UniqueID::INVALID &&
                    guidToAssign!=UniqueID::INVALID)
                {
                    vjip->members[j].mID = guidToAssign;
                }

                if (vjip->members[j].joinInProgressState==JIPS_PROCESSING)
                {
                    anythingChanged=true;
                    vjip->members[j].joinInProgressState=newState;
                }
            }

            if (anythingChanged)
            {
                ProcessVerifiedJoinInProgressIfCompleted(vjip);
            }
        }
    }
    //------------------------------------------------------------------------
    bool FullyConnected::ProcessVerifiedJoinInProgressIfCompleted(VerifiedJoinInProgress *vjip)
    {
        // If no systems in processing state, send results to server
        // Return true if this was done
        bool anyProcessing=false;
        for (Nui32 i=0; i < vjip->members.Size(); ++i)
        {
            if (vjip->members[i].joinInProgressState == JIPS_PROCESSING)
            {
                anyProcessing=true;
                break;
            }
        }

        if (anyProcessing==true)
            return false;

        // send results to server
        NetSerializer bsOut;
        WriteVerifiedJoinCapable(&bsOut, vjip);
        WriteVJCUserData(&bsOut);
        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, vjip->requester, false);

        //vjip->sentResults=true;
        return true;
    }
    //------------------------------------------------------------------------
    void FullyConnected::WriteVerifiedJoinCapable(NetSerializer * bsOut,
        VerifiedJoinInProgress * vjip)
    {
        bsOut->write((Token)ID_FCM2_VERIFIED_JOIN_CAPABLE);
        bsOut->write((Nui16)vjip->members.Size());
        NCount i;
        for (i = 0; i < vjip->members.Size(); ++i)
        {
            bsOut->write(vjip->members[i].mID);
            bsOut->write(vjip->members[i].mAddress);
            bsOut->write((Nui8)vjip->members[i].joinInProgressState);
        }
    }
    //------------------------------------------------------------------------
    void FullyConnected::ReadVerifiedJoinInProgressMember(NetSerializer * bsIn,
        VerifiedJoinInProgressMember * vjipm)
    {
        bsIn->read(vjipm->mID);
        bsIn->read(vjipm->mAddress);
        ConnectionState cs = mPrc2->getState(vjipm->mID);
        if (cs == IS_CONNECTED)
            vjipm->joinInProgressState = JIPS_CONNECTED;
        else if (cs == IS_DISCONNECTING || cs == IS_SILENTLY_DISCONNECTING)
            vjipm->joinInProgressState = JIPS_FAILED;
        else
            vjipm->joinInProgressState = JIPS_PROCESSING;
    }
    //------------------------------------------------------------------------
    Nui32 FullyConnected::GetVerifiedJoinInProgressMemberIndex(
        const UniqueAddress usys, VerifiedJoinInProgress * vjip)
    {
        for(NCount j = 0; j < vjip->members.Size(); ++j)
        {
            if((usys.mID != UniqueID::INVALID && vjip->members[j].mID == usys.mID) ||
                (usys.mAddress != Address::invalid && vjip->members[j].mAddress == usys.mAddress))
            {
                return j;
            }
        }
        return (Nui32) -1;
    }
    //------------------------------------------------------------------------
    void FullyConnected::CategorizeVJIP(VerifiedJoinInProgress * vjip,
        DataStructures::List<UniqueID> & participatingMembersOnClientSucceeded,
        DataStructures::List<UniqueID> & participatingMembersOnClientFailed,
        DataStructures::List<UniqueID> & participatingMembersNotOnClient,
        DataStructures::List<UniqueID> & clientMembersNotParticipatingSucceeded,
        DataStructures::List<UniqueID> & clientMembersNotParticipatingFailed)
    {
        for (Nui32 i=0; i < vjip->members.Size(); i++)
            vjip->members[i].workingFlag=false;

        for (Nui32 i=0; i < fcm2ParticipantList.Size(); i++)
        {
            Nui32 j = GetVerifiedJoinInProgressMemberIndex(fcm2ParticipantList[i].mID, vjip);
            if (j==(Nui32)-1)
            {
                participatingMembersNotOnClient.Push(fcm2ParticipantList[i].mID);
            }
            else
            {
                if (vjip->members[j].joinInProgressState==JIPS_FAILED)
                    participatingMembersOnClientFailed.Push(fcm2ParticipantList[i].mID);
                else
                    participatingMembersOnClientSucceeded.Push(fcm2ParticipantList[i].mID);
                vjip->members[j].workingFlag=true;
            }
        }
        for (NCount j = 0; j < vjip->members.Size(); ++j)
        {
            if (vjip->members[j].workingFlag == false)
            {
                if (vjip->members[j].joinInProgressState==JIPS_FAILED)
                    clientMembersNotParticipatingFailed.Push(vjip->members[j].mID);
                else
                    clientMembersNotParticipatingSucceeded.Push(vjip->members[j].mID);
            }
        }
    }
}
}
#endif // _RAKNET_SUPPORT_*
