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
#if _RAKNET_SUPPORT_Router2==1 && _RAKNET_SUPPORT_UDPForwarder==1

#include "NiiPreProcess.h"
#include "Router2.h"
#include "NiiNetLinkBase.h"
#include "NiiNetSerializer.h"
#include "DS_OrderedList.h"

/*
Algorithm:

1. Sender calls ConnectInternal(). A ConnnectRequest structure is allocated and stored in the connectionRequests list, containing a list of every system we are connected to. ID_ROUTER_2_QUERY_FORWARDING is sent to all connected systems.

2. Upon the router getting ID_ROUTER_2_QUERY_FORWARDING, ID_ROUTER_2_REPLY_FORWARDING is sent to the sender indicating if that router is connected to the endpoint, along with the ping from the router to the endpoint.

3. Upon the sender getting ID_ROUTER_2_REPLY_FORWARDING, the connection request structure is looked up in Router2::UpdateForwarding. The ping is stored in that structure. Once all systems have replied, the system continues to the next state. If every system in step 1 has been exhausted, and routing has occured at least once, then ID_CONNECTION_LOST is returned. If every system in step 1 has been exhausted and routing has never occured, then ID_ROUTER_2_FORWARDING_NO_PATH is returned. Otherwise, the router with the lowest ping is chosen, and RequestForwarding() is called with that system, which sends ID_ROUTER_2_REQUEST_FORWARDING to the router.

4. When the router gets ID_ROUTER_2_REQUEST_FORWARDING, a MiniPunchRequest structure is allocated and stored in the miniPunchesInProgress list. The function SendOOBMessages() sends ID_ROUTER_2_REPLY_TO_SENDER_PORT from the routing sockets to both the sender and endpoint. It also sends ID_ROUTER_2_REPLY_TO_SPECIFIED_PORT through the regular RakNet connection.

5. The sender and endpoint should get ID_ROUTER_2_REPLY_TO_SENDER_PORT and/or ID_ROUTER_2_REPLY_TO_SPECIFIED_PORT depending on what type of router they have. If ID_ROUTER_2_REPLY_TO_SENDER_PORT arrives, then this will reply back to the routing socket directly. If ID_ROUTER_2_REPLY_TO_SPECIFIED_PORT arrives, then the reply port is modified to be the port specified by the router system. In both cases, ID_ROUTER_2_MINI_PUNCH_REPLY is sent. As the router has already setup the forwarding, ID_ROUTER_2_MINI_PUNCH_REPLY will actually arrive to the endpoint from the sender, and from the sender to the endpoint.

6. When ID_ROUTER_2_MINI_PUNCH_REPLY arrives, ID_ROUTER_2_MINI_PUNCH_REPLY_BOUNCE will be sent to the router. This is to tell the router that the forwarding has succeeded.

7. When ID_ROUTER_2_MINI_PUNCH_REPLY_BOUNCE arrives on the router, the router will find the two systems in the miniPunchesInProgress list, which was added in step 4 (See OnMiniPunchReplyBounce()). gotReplyFromSource or gotReplyFromEndpoint will be set to true, depending on the sender. When both gotReplyFromSource and gotReplyFromEndpoint have replied, then ID_ROUTER_2_REROUTE is sent to the endpoint, and ID_ROUTER_2_FORWARDING_ESTABLISHED is sent to the sender.

8. When the endpoint gets ID_ROUTER_2_REROUTE, the system address is updated for the guid of the sender using NetworkSys::swap(). This happens in OnRerouted().

9. When the sender gets ID_ROUTER_2_FORWARDING_ESTABLISHED, then in OnForwardingSuccess() the endpoint is removed from the connectionRequest list and moved to the forwardedConnectionList.

10. In onConnectClose(), for the sender, if the closed connection is the endpoint, then the endpoint is removed from the forwardedConnectionList (this is a graceful disconnect). If the connection was instead lost to the router, then ConnectInternal() gets called, which goes back to step 1. If instead this was a connection requset in progress, then UpdateForwarding() gets called, which goes back to step 3.

11. When the user connects the endpoint and sender, then the sender will get ID_CONNECTION_REQUEST_ACCEPTED. The sender will look up the endpoint in the forwardedConnectionList, and send ID_ROUTER_2_INCREASE_TIMEOUT to the endpoint. This message will call setTimeOut() on the endpoint, so that if the router disconnects, enough time is available for the reroute to complete.
*/

#define MIN_MINIPUNCH_TIMEOUT 5000

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    void Router2DebugInterface::ShowFailure(const char * message)
    {
        printf("%s", message);
    }
    //------------------------------------------------------------------------
    void Router2DebugInterface::ShowDiagnostic(const char * message)
    {
        printf("%s", message);
    }
    //------------------------------------------------------------------------
    enum Router2MessageIdentifiers
    {
        ID_ROUTER_2_QUERY_FORWARDING,
        ID_ROUTER_2_REPLY_FORWARDING,
        ID_ROUTER_2_REQUEST_FORWARDING,
        ID_ROUTER_2_INCREASE_TIMEOUT,
    };
    //------------------------------------------------------------------------
    Router2::ConnnectRequest::ConnnectRequest()
    {

    }
    //------------------------------------------------------------------------
    Router2::ConnnectRequest::~ConnnectRequest()
    {

    }
    //------------------------------------------------------------------------
    Router2::Router2()
    {
        udpForwarder=0;
        maximumForwardingRequests=0;
        debugInterface=0;
        socketFamily=AF_INET;
    }
    //------------------------------------------------------------------------
    Router2::~Router2()
    {
        ClearAll();

        if (udpForwarder)
        {
            udpForwarder->Shutdown();
            N_delete udpForwarder;
        }
    }
    //------------------------------------------------------------------------
    void Router2::ClearMinipunches(void)
    {
        miniPunchesInProgressMutex.Lock();
        miniPunchesInProgress.Clear(false);
        miniPunchesInProgressMutex.Unlock();
    }
    //------------------------------------------------------------------------
    void Router2::ClearConnectionRequests(void)
    {
        connectionRequestsMutex.Lock();
        for (Nui32 i=0; i < connectionRequests.Size(); i++)
        {
            N_delete connectionRequests[i];
        }
        connectionRequests.Clear(false);
        connectionRequestsMutex.Unlock();
    }
    //------------------------------------------------------------------------
    bool Router2::ConnectInternal(UniqueID endpointGuid, bool returnConnectionLostOnFailure)
    {
        int largestPing = GetLargestPingAmongConnectedSystems();
        if(largestPing < 0)
        {
            String buff;
            if(debugInterface)
                debugInterface->ShowFailure(StrUtil::format(buff,"Router2: ConnectInternal(%I64d) failed\n", endpointGuid.g));

            // Not connected to anyone
            return false;
        }

        // ALready in progress?
        connectionRequestsMutex.Lock();
        if (GetConnectionRequestIndex(endpointGuid)!=(Nui32)-1)
        {
            connectionRequestsMutex.Unlock();
            String buff;
            if(debugInterface)
                debugInterface->ShowFailure(StrUtil::format(buff,"Router2: ConnectInternal(%I64d) failed\n", endpointGuid.g));

            return false;
        }
        connectionRequestsMutex.Unlock();

        // StoreRequest(endpointGuid, Largest(ping*2), systemsSentTo). Set state REQUEST_STATE_QUERY_FORWARDING
        Router2::ConnnectRequest *cr = N_new Router2::ConnnectRequest();
        DataStructures::List<Address *> addresses;
        DataStructures::List<UniqueID> guids;
        mPrc2->getActive(addresses, guids);
        if(guids.Size() == 0)
        {
            String buff;
            if(debugInterface)
                debugInterface->ShowFailure(StrUtil::format(buff,"Router2 failed at %s:%i\n"));

            return false;
        }
        cr->requestState=R2RS_REQUEST_STATE_QUERY_FORWARDING;
        cr->pingTimeout=N_Engine().getTimer().getMS()+largestPing*2+1000;
        cr->endpointGuid=endpointGuid;
        cr->returnConnectionLostOnFailure=returnConnectionLostOnFailure;
        for (Nui32 i=0; i < guids.Size(); i++)
        {
            ConnectionRequestSystem crs;
            if (guids[i]!=endpointGuid)
            {
                crs.mID=guids[i];
                crs.pingToEndpoint=-1;
                cr->connectionRequestSystemsMutex.Lock();
                cr->connectionRequestSystems.Push(crs);
                cr->connectionRequestSystemsMutex.Unlock();

                // Broadcast(ID_ROUTER_2_QUERY_FORWARDING, endpointGuid);
                NetSerializer bsOut;
                bsOut.write((Token)ID_ROUTER_2_INTERNAL);
                bsOut.write((Nui8) ID_ROUTER_2_QUERY_FORWARDING);
                bsOut.write(endpointGuid);
                Nui32 pack_id = mPrc2->send(&bsOut,PL_MEDIUM,PT_ORDER_MAKE_SURE,0,crs.mID,false);

                if (debugInterface)
                {
                    String buff;
                    debugInterface->ShowDiagnostic(StrUtil::format(buff, "Router2::ConnectInternal, pack_id = %d", pack_id));
                }

            }
            else
            {
                if (debugInterface)
                {
                    String buff;
                    debugInterface->ShowDiagnostic(StrUtil::format(buff, "Router2::ConnectInternal: %I64d==%I64d",
                        guids[i].g,endpointGuid.g));
                }
            }
        }
        connectionRequestsMutex.Lock();
        connectionRequests.Push(cr);
        connectionRequestsMutex.Unlock();

        if (debugInterface)
        {
            String buff;
            debugInterface->ShowDiagnostic(StrUtil::format(buff, "Broadcasting ID_ROUTER_2_QUERY_FORWARDING to %I64d\n", endpointGuid.g));
        }
        return true;
    }
    //------------------------------------------------------------------------
    void Router2::SetSocketFamily(Nui16 _socketFamily)
    {
        socketFamily=_socketFamily;
    }
    //------------------------------------------------------------------------
    void Router2::EstablishRouting(UniqueID endpointGuid)
    {
        ConnectionState cs = mPrc2->getState(endpointGuid);
        if (cs!=IS_DISCONNECTED && cs!=IS_NOT_CONNECTED)
        {
            String buff;
            if (debugInterface)	debugInterface->ShowFailure(StrUtil::format(buff, "Router2 failed at %s:%i "
                    "(already connected to the %I64d)\n", endpointGuid.g));
            return;
        }

        ConnectInternal(endpointGuid,false);
    }
    //------------------------------------------------------------------------
    void Router2::SetMaximumForwardingRequests(int max)
    {
        if (max > 0 && maximumForwardingRequests <= 0)
        {
            udpForwarder = N_new UDPForwarder();
            udpForwarder->Startup();
        }
        else if (max <= 0 && maximumForwardingRequests > 0)
        {
            udpForwarder->Shutdown();
            N_delete udpForwarder;
            udpForwarder=0;
        }

        maximumForwardingRequests=max;
    }
    //------------------------------------------------------------------------
    ConnectReceiveType Router2::onMessage(SocketMessage *msg)
    {
        Address * sa;
        NetSerializer bs(msg->mData,msg->mLength,false);
        if (msg->mData[0]==ID_ROUTER_2_INTERNAL)
        {
            switch (msg->mData[1])
            {
            case ID_ROUTER_2_QUERY_FORWARDING:
                {
                    OnQueryForwarding(msg);
                    return CRT_Destroy;
                }
            case ID_ROUTER_2_REPLY_FORWARDING:
                {
                    OnQueryForwardingReply(msg);
                    return CRT_Destroy;
                }
            case ID_ROUTER_2_REQUEST_FORWARDING:
                {

                    if(debugInterface)
                    {
                        String buff;
                        String buff2;
                        msg->mAddress.read(buff2, true);
                        debugInterface->ShowDiagnostic(StrUtil::format(buff,"Got ID_ROUTER_2_REQUEST_FORWARDING on ip %s from %I64d, ",
                            buff2.c_str(), msg->mID.g));
                    }

                    OnRequestForwarding(msg);
                    return CRT_Destroy;
                }
            case ID_ROUTER_2_INCREASE_TIMEOUT:
                {
                    /// The routed system wants more time to stay alive on no communication, in case the router drops or crashes
                    mPrc2->setTimeOut(msg->mAddress, mPrc2->getTimeOut(msg->mAddress) + 10000);
                    return CRT_Destroy;
                }
            }
        }
        else if (msg->mData[0] == ID_OUT_OF_BAND_INTERNAL && msg->mLength >= 2)
        {
            switch (msg->mData[1])
            {
                case ID_ROUTER_2_REPLY_TO_SENDER_PORT:
                    {
                        NetSerializer out;
                        out.write(msg->mID);
                        SendOOBFromRakNetPort(ID_ROUTER_2_MINI_PUNCH_REPLY,
                            &out, msg->mAddress);

                        if(debugInterface)
                        {
                            String buff;
                            String buff2;
                            sa->read(buff2, false);
                            debugInterface->ShowDiagnostic(StrUtil::format(buff,
                                "Got ID_ROUTER_2_REPLY_TO_SENDER_PORT %i on address %s, replying with ID_ROUTER_2_MINI_PUNCH_REPLY at %s:%i\n",
                                    sa->getHSBPort(), buff2.c_str()));
                        }

                        return CRT_Destroy;
                    }
                case ID_ROUTER_2_REPLY_TO_SPECIFIED_PORT:
                    {
                        NetSerializer bsOut;
                        bsOut.write(msg->mID);
                        bs.skipRead(2, 0);
                        sa = msg->mAddress;
                        Nui16 port;
                        bs.read(port);
                        sa->setHSBPort(port);
                        N_assert(sa->getHSBPort() != 0);
                        SendOOBFromRakNetPort(ID_ROUTER_2_MINI_PUNCH_REPLY, &bsOut, sa);

                        if(debugInterface)
                        {
                            String buff;
                            String buff2;
                            sa->read(buff2, false);
                            debugInterface->ShowDiagnostic(StrUtil::format(buff,"Got ID_ROUTER_2_REPLY_TO_SPECIFIED_PORT %i on address %s, "
                                "replying with ID_ROUTER_2_MINI_PUNCH_REPLY\n",
                                    sa->getHSBPort(), buff2.c_str()));
                        }

                        return CRT_Destroy;
                    }
                case ID_ROUTER_2_MINI_PUNCH_REPLY:
                    OnMiniPunchReply(msg);
                    return CRT_Destroy;
                case ID_ROUTER_2_MINI_PUNCH_REPLY_BOUNCE:
                    OnMiniPunchReplyBounce(msg);
                    return CRT_Destroy;
            }
        }
        else if (msg->mData[0]==ID_ROUTER_2_FORWARDING_ESTABLISHED)
        {
            if (OnForwardingSuccess(msg)==false)
                return CRT_Destroy;
        }
        else if (msg->mData[0]==ID_ROUTER_2_REROUTED)
        {
            OnRerouted(msg);
        }
        else if (msg->mData[0]==ID_CONNECTION_REQUEST_ACCEPTED)
        {
            Nui32 forwardingIndex;
            forwardedConnectionListMutex.Lock();
            for (forwardingIndex=0; forwardingIndex < forwardedConnectionList.Size(); forwardingIndex++)
            {
                if (forwardedConnectionList[forwardingIndex].endpointGuid==msg->mID && forwardedConnectionList[forwardingIndex].weInitiatedForwarding)
                    break;
            }

            if (forwardingIndex < forwardedConnectionList.Size())
            {
                forwardedConnectionListMutex.Unlock();

                // We connected to this system through a forwarding system
                // Have the endpoint take longer to drop us, in case the intermediary system drops
                NetSerializer bsOut;
                bsOut.write((Token)ID_ROUTER_2_INTERNAL);
                bsOut.write((Nui8) ID_ROUTER_2_INCREASE_TIMEOUT);
                mPrc2->send(&bsOut, PL_HIGH,PT_BE_SURE, 0, msg->mID, false);

                if(debugInterface)
                {
                    String buff;
                    debugInterface->ShowDiagnostic(StrUtil::format(buff,"Got ID_CONNECTION_REQUEST_ACCEPTED, "
                        "sending ID_ROUTER_2_INCREASE_TIMEOUT to the %I64d\n", msg->mID.g));
                }

                // Also take longer ourselves
                mPrc2->setTimeOut(msg->mAddress, mPrc2->getTimeOut(msg->mAddress) + 10000);
            }
            else
            {
                // ~Gwynn: Fix for receive hanging up problem on Windows XP
                // See http://blog.delphi-jedi.net/2008/04/23/the-case-of-the-unexplained-dead-lock-in-a-single-thread/ for details
                forwardedConnectionListMutex.Unlock();
            }
        }
        else if(msg->mData[0] == ID_ROUTER_2_FORWARDING_NO_PATH)
        {
            if(msg->mLocal == false)
                return CRT_Destroy;
        }

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void Router2::update()
    {
        TimeDurMS curTime = N_Engine().getTimer().getMS();
        Nui32 connectionRequestIndex=0;
        connectionRequestsMutex.Lock();
        while (connectionRequestIndex < connectionRequests.Size())
        {
            ConnnectRequest* connectionRequest = connectionRequests[connectionRequestIndex];

            // pingTimeout is only used with R2RS_REQUEST_STATE_QUERY_FORWARDING
            if (connectionRequest->requestState==R2RS_REQUEST_STATE_QUERY_FORWARDING &&
                connectionRequest->pingTimeout < curTime)
            {
                bool anyRemoved=false;
                Nui32 connectionRequestGuidIndex=0;
                connectionRequest->connectionRequestSystemsMutex.Lock();
                while (connectionRequestGuidIndex < connectionRequest->connectionRequestSystems.Size())
                {
                    if (connectionRequest->connectionRequestSystems[connectionRequestGuidIndex].pingToEndpoint<0)
                    {
                        anyRemoved=true;
                        connectionRequest->connectionRequestSystems.RemoveAtIndexFast(connectionRequestGuidIndex);
                    }
                    else
                    {
                        connectionRequestGuidIndex++;
                    }
                }
                connectionRequest->connectionRequestSystemsMutex.Unlock();

                if (anyRemoved)
                {
                    if (connectionRequestIndex!=(Nui32)-1)
                    {
                        // connectionRequestsMutex should be locked before calling this function
                        if (UpdateForwarding(connectionRequest)==false)
                        {
                            RemoveConnectionRequest(connectionRequestIndex);
                        }
                        else
                        {
                            connectionRequestIndex++;
                        }
                    }
                    else
                    {
                        connectionRequestIndex++;
                    }
                }
                else
                {
                    connectionRequestIndex++;
                }
            }
            else
            {
                connectionRequestIndex++;
            }
        }
        connectionRequestsMutex.Unlock();

        Nui32 i=0;
        miniPunchesInProgressMutex.Lock();
        while (i < miniPunchesInProgress.Size())
        {
            if (miniPunchesInProgress[i].timeout<curTime)
            {
                SendFailureOnCannotForward(miniPunchesInProgress[i].sourceGuid, miniPunchesInProgress[i].endpointGuid);
                miniPunchesInProgress.RemoveAtIndexFast(i);
            }
            else if (curTime>miniPunchesInProgress[i].nextAction)
            {
                miniPunchesInProgress[i].nextAction=curTime+100;
                SendOOBMessages(&miniPunchesInProgress[i]);
            }
            else
                i++;
        }
        miniPunchesInProgressMutex.Unlock();
    }
    //------------------------------------------------------------------------
    void Router2::onConnectClose(const Address & address,
        UniqueID id, ConnectCloseType lostConnectionReason)
    {
        (void) lostConnectionReason;
        (void) address;

        Nui32 forwardedConnectionIndex=0;
        forwardedConnectionListMutex.Lock();
        while (forwardedConnectionIndex<forwardedConnectionList.Size())
        {
            if (forwardedConnectionList[forwardedConnectionIndex].endpointGuid==id)
            {
                if (debugInterface)
                {
                    String buff;
                    debugInterface->ShowDiagnostic(StrUtil::format(buff,"Closed connection to the %I64d, removing forwarding from list\n", id.g));
                }

                // No longer need forwarding
                forwardedConnectionList.RemoveAtIndexFast(forwardedConnectionIndex);
            }
            else if (forwardedConnectionList[forwardedConnectionIndex].intermediaryGuid==id && forwardedConnectionList[forwardedConnectionIndex].weInitiatedForwarding)
            {
                // Lost connection to intermediary. Restart process to connect to endpoint. If failed, push ID_CONNECTION_LOST. Also remove connection request if it already is in the list, to restart it
                connectionRequestsMutex.Lock();
                Nui32 pos = GetConnectionRequestIndex(forwardedConnectionList[forwardedConnectionIndex].endpointGuid);
                if((Nui32)-1 != pos)
                {
                    N_delete connectionRequests[pos];
                    connectionRequests.RemoveAtIndexFast(pos);
                }
                connectionRequestsMutex.Unlock();

                ConnectInternal(forwardedConnectionList[forwardedConnectionIndex].endpointGuid, true);

                forwardedConnectionIndex++;

                if (debugInterface)
                {
                    String buff;
                    debugInterface->ShowDiagnostic(StrUtil::format(buff,"Closed connection %I64d, restarting forwarding\n", id.g));
                }
            }
            else
                forwardedConnectionIndex++;
        }
        forwardedConnectionListMutex.Unlock();

        Nui32 connectionRequestIndex=0;
        connectionRequestsMutex.Lock();
        while (connectionRequestIndex < connectionRequests.Size())
        {
            ConnnectRequest *cr = connectionRequests[connectionRequestIndex];
            cr->connectionRequestSystemsMutex.Lock();
            Nui32 connectionRequestGuidIndex = cr->GetGuidIndex(id);
            if (connectionRequestGuidIndex!=(Nui32)-1)
            {
                cr->connectionRequestSystems.RemoveAtIndexFast(connectionRequestGuidIndex);
                cr->connectionRequestSystemsMutex.Unlock();
                if (UpdateForwarding(cr)==false) // If returns false, no connection request systems left
                {
                    if (debugInterface)
                    {
                        String buff;
                        debugInterface->ShowDiagnostic(StrUtil::format(buff,"Aborted connection to the %I64d, aborted forwarding\n", id.g));
                    }

                    RemoveConnectionRequest(connectionRequestIndex);
                }
                else // Else a system in the connection request list dropped. If cr->requestState==R2RS_REQUEST_STATE_QUERY_FORWARDING then we are still waiting for other systems to respond.
                {
                    if (debugInterface)
                    {
                        String buff;
                        debugInterface->ShowDiagnostic(StrUtil::format(buff,"Aborted connection attempt to %I64d, restarting forwarding to %I64d\n", id.g, cr->endpointGuid.g));
                    }
                    connectionRequestIndex++;
                }
            }
            else
            {
                cr->connectionRequestSystemsMutex.Unlock();
                connectionRequestIndex++;
            }
        }
        connectionRequestsMutex.Unlock();

        Nui32 i=0;
        miniPunchesInProgressMutex.Lock();
        while (i < miniPunchesInProgress.Size())
        {
            if (miniPunchesInProgress[i].sourceGuid==id || miniPunchesInProgress[i].endpointGuid==id)
            {
                if (miniPunchesInProgress[i].sourceGuid != id)
                {
                    SendFailureOnCannotForward(miniPunchesInProgress[i].sourceGuid, miniPunchesInProgress[i].endpointGuid);
                }
                miniPunchesInProgress.RemoveAtIndexFast(i);
            }
            else
                i++;
        }
        miniPunchesInProgressMutex.Unlock();
    }
    //------------------------------------------------------------------------
    void Router2::onConnectFail(SocketMessage * msg, ConnectFailType failedConnectionAttemptReason)
    {
        (void) failedConnectionAttemptReason;
        (void) msg;

        Nui32 forwardedConnectionIndex=0;
        forwardedConnectionListMutex.Lock();
        while (forwardedConnectionIndex<forwardedConnectionList.Size())
        {
            if (forwardedConnectionList[forwardedConnectionIndex].intermediaryAddress==msg->mAddress)
            {
                if (debugInterface)
                {
                    String buff;
                    String temp1;
                    msg->mAddress.read(temp1, true);
                    debugInterface->ShowDiagnostic(StrUtil::format(buff,"Failed connection attempt to forwarded system (%I64d : %s)\n",
                        forwardedConnectionList[forwardedConnectionIndex].endpointGuid.g, temp1));
                }

                msg->mID=forwardedConnectionList[forwardedConnectionIndex].endpointGuid;
                forwardedConnectionList.RemoveAtIndexFast(forwardedConnectionIndex);
            }
            else
                ++forwardedConnectionIndex;
        }
        forwardedConnectionListMutex.Unlock();
    }
    //------------------------------------------------------------------------
    void Router2::shutdown()
    {
        ClearAll();
    }
    //------------------------------------------------------------------------
    // connectionRequestsMutex should already be locked
    bool Router2::UpdateForwarding(ConnnectRequest* connectionRequest)
    {
        connectionRequest->connectionRequestSystemsMutex.Lock();

        if (connectionRequest->connectionRequestSystems.Size()==0)
        {
            connectionRequest->connectionRequestSystemsMutex.Unlock();

            if (connectionRequest->returnConnectionLostOnFailure) {
                ReturnToUser(ID_CONNECTION_LOST, connectionRequest->endpointGuid, 0, true); // This is a connection which was previously established. Rerouting is not possible.
            }
            else
                ReturnToUser(ID_ROUTER_2_FORWARDING_NO_PATH, connectionRequest->endpointGuid, 0, false);

            if (debugInterface)
            {
                String buff;
                debugInterface->ShowDiagnostic(StrUtil::format(buff,"Forwarding failed, no remaining systems at %s:%i\n"));
            }

            forwardedConnectionListMutex.Lock();

            for (Nui32 forwardedConnectionIndex=0; forwardedConnectionIndex < forwardedConnectionList.Size(); forwardedConnectionIndex++)
            {
                if (forwardedConnectionList[forwardedConnectionIndex].endpointGuid==connectionRequest->endpointGuid && forwardedConnectionList[forwardedConnectionIndex].weInitiatedForwarding)
                {
                    forwardedConnectionList.RemoveAtIndexFast(forwardedConnectionIndex);
                    break;
                }
            }
            forwardedConnectionListMutex.Unlock();

            return false;
        }
        connectionRequest->connectionRequestSystemsMutex.Unlock();

        if (connectionRequest->requestState==R2RS_REQUEST_STATE_QUERY_FORWARDING)
        {
            connectionRequest->connectionRequestSystemsMutex.Lock();

            for (Nui32 i=0; i < connectionRequest->connectionRequestSystems.Size(); i++)
            {
                if (connectionRequest->connectionRequestSystems[i].pingToEndpoint<0)
                {
                    connectionRequest->connectionRequestSystemsMutex.Unlock();
                    return true; // Forward query still in progress, just return
                }
            }
            connectionRequest->connectionRequestSystemsMutex.Unlock();

            RequestForwarding(connectionRequest);
        }

        return true;
    }
    //------------------------------------------------------------------------
    // connectionRequestsMutex should already be locked
    void Router2::RemoveConnectionRequest(Nui32 connectionRequestIndex)
    {
        N_delete connectionRequests[connectionRequestIndex];
        connectionRequests.RemoveAtIndexFast(connectionRequestIndex);
    }
    //------------------------------------------------------------------------
    int ConnectionRequestSystemComp(const Router2::ConnectionRequestSystem & key,
        const Router2::ConnectionRequestSystem & data)
    {
        if (key.pingToEndpoint * (key.usedForwardingEntries+1) < data.pingToEndpoint * (data.usedForwardingEntries+1))
            return -1;
        if (key.pingToEndpoint * (key.usedForwardingEntries+1) == data.pingToEndpoint * (data.usedForwardingEntries+1))
            return 1;
        if (key.mID < data.mID)
            return -1;
        if (key.mID > data.mID)
            return -1;
        return 0;
    }
    //------------------------------------------------------------------------
    // connectionRequestsMutex should already be locked
    void Router2::RequestForwarding(ConnnectRequest * connectionRequest)
    {
        N_assert(connectionRequest->requestState == R2RS_REQUEST_STATE_QUERY_FORWARDING);
        connectionRequest->requestState = REQUEST_STATE_REQUEST_FORWARDING;

        if (connectionRequest->GetGuidIndex(connectionRequest->lastRequestedForwardingSystem)!=(Nui32)-1)
        {
            String buff;
            if (debugInterface)	debugInterface->ShowFailure(StrUtil::format(buff,"Router2 failed at %s:%i\n"));
            return;
        }

        // Prioritize systems to request forwarding
        DataStructures::OrderedList<ConnectionRequestSystem, ConnectionRequestSystem, ConnectionRequestSystemComp> commandList;
        Nui32 connectionRequestGuidIndex;
        connectionRequest->connectionRequestSystemsMutex.Lock();
        for (connectionRequestGuidIndex = 0; connectionRequestGuidIndex < connectionRequest->connectionRequestSystems.Size(); connectionRequestGuidIndex++)
        {
            N_assert(connectionRequest->connectionRequestSystems[connectionRequestGuidIndex].pingToEndpoint>=0);
            commandList.Insert(connectionRequest->connectionRequestSystems[connectionRequestGuidIndex],
                connectionRequest->connectionRequestSystems[connectionRequestGuidIndex],
                true);
        }
        connectionRequest->connectionRequestSystemsMutex.Unlock();

        connectionRequest->lastRequestedForwardingSystem = commandList[0].mID;

        NetSerializer bsOut;
        bsOut.write((Token)ID_ROUTER_2_INTERNAL);
        bsOut.write((Nui8) ID_ROUTER_2_REQUEST_FORWARDING);
        bsOut.write(connectionRequest->endpointGuid);
        mPrc2->send(&bsOut,PL_MEDIUM,PT_ORDER_MAKE_SURE,0,connectionRequest->lastRequestedForwardingSystem,false);

        if (debugInterface)
        {
            String buff;
            debugInterface->ShowDiagnostic(StrUtil::format(buff, "Sending ID_ROUTER_2_REQUEST_FORWARDING "
                "(connectionRequest->lastRequestedForwardingSystem = %I64d, connectionRequest->endpointGuid = %I64d)\n",
                connectionRequest->lastRequestedForwardingSystem.g, connectionRequest->endpointGuid.g));
        }
    }
    //------------------------------------------------------------------------
    void Router2::SendFailureOnCannotForward(UniqueID sourceGuid, UniqueID endpointGuid)
    {
        NetSerializer bsOut;
        bsOut.write((Token)ID_ROUTER_2_INTERNAL);
        bsOut.write((Nui8) ID_ROUTER_2_REPLY_FORWARDING);
        bsOut.write(endpointGuid);
        bsOut.write0();
        mPrc2->send(&bsOut,PL_MEDIUM,PT_ORDER_MAKE_SURE,0,sourceGuid,false);
    }
    //------------------------------------------------------------------------
    int Router2::ReturnFailureOnCannotForward(UniqueID sourceGuid, UniqueID endpointGuid)
    {
        // If the number of systems we are currently forwarding>=maxForwarding, return ID_ROUTER_2_REPLY_FORWARDING,endpointGuid,false
        if (udpForwarder==0 || udpForwarder->GetUsedForwardEntries()/2>maximumForwardingRequests)
        {
            String buff;
            if (debugInterface)	debugInterface->ShowFailure(
                StrUtil::format(buff, "Router2 failed (%I64d -> %I64d)\n", sourceGuid.g, endpointGuid.g));
            SendFailureOnCannotForward(sourceGuid,endpointGuid);
            return -1;
        }

        // We cannot forward connections which are themselves forwarded. Return fail in that case
        forwardedConnectionListMutex.Lock();
        for (Nui32 i=0; i < forwardedConnectionList.Size(); i++)
        {
            if ((forwardedConnectionList[i].endpointGuid==endpointGuid)
                || (forwardedConnectionList[i].endpointGuid==sourceGuid)) // sourceGuid is here so you do not respond to routing requests from systems you are already routing through.
            {
                forwardedConnectionListMutex.Unlock();

                String buff;
                if (debugInterface)	debugInterface->ShowFailure(StrUtil::format(buff,"Router2 failed\n"));
                SendFailureOnCannotForward(sourceGuid,endpointGuid);
                return -1;
            }
        }
        forwardedConnectionListMutex.Unlock();

        int pingToEndpoint;
        pingToEndpoint = mPrc2->getAvgPing(endpointGuid);
        if (pingToEndpoint==-1)
        {
            String buff;
            if (debugInterface)
                debugInterface->ShowFailure(StrUtil::format(buff,"Router2 failed (%I64d -> %I64d)\n",
                    sourceGuid.g, endpointGuid.g));

            SendFailureOnCannotForward(sourceGuid,endpointGuid);
            return -1;
        }
        return pingToEndpoint;
    }
    //------------------------------------------------------------------------
    void Router2::OnQueryForwarding(SocketMessage * msg)
    {
        NetSerializer bs(msg->mData, msg->mLength, false);
        bs.skipRead(2, 0);
        UniqueID endpointGuid;
        // read endpointGuid
        bs.read(endpointGuid);

        int pingToEndpoint = ReturnFailureOnCannotForward(msg->mID, endpointGuid);
        if (pingToEndpoint==-1)
        {
            String buff;
            if (debugInterface)	debugInterface->ShowFailure(StrUtil::format(buff, "Router2 failed (%I64d)\n", msg->mID.g));
            return;
        }

        // If we are connected to endpointGuid, reply ID_ROUTER_2_REPLY_FORWARDING,endpointGuid,true,ping,numCurrentlyForwarding
        NetSerializer bsOut;
        bsOut.write((Token)ID_ROUTER_2_INTERNAL);
        bsOut.write((Nui8) ID_ROUTER_2_REPLY_FORWARDING);
        bsOut.write(endpointGuid);
        bsOut.write1();
        bsOut.write((Nui16) pingToEndpoint);
        bsOut.write((Nui16) udpForwarder->GetUsedForwardEntries()/2);
        mPrc2->send(&bsOut,PL_MEDIUM,PT_ORDER_MAKE_SURE,0,msg->mID,false);

        if (debugInterface)
        {
            String buff;
            debugInterface->ShowDiagnostic(StrUtil::format(buff, "Sending ID_ROUTER_2_REPLY_FORWARDING to the %I64d\n", msg->mID.g));
        }
    }
    //------------------------------------------------------------------------
    void Router2::OnQueryForwardingReply(SocketMessage * msg)
    {
        NetSerializer bs(msg->mData, msg->mLength, false);
        bs.skipRead(2, 0);
        UniqueID endpointGuid;
        bs.read(endpointGuid);
        // Find endpointGuid among stored requests
        bool canForward=false;
        bs.read(canForward);
        connectionRequestsMutex.Lock();
        Nui32 connectionRequestIndex = GetConnectionRequestIndex(endpointGuid);
        if (connectionRequestIndex==(Nui32)-1)
        {
            connectionRequestsMutex.Unlock();
            String buff;
            if (debugInterface)	debugInterface->ShowFailure(StrUtil::format(buff, "Router2 failed (%I64d)\n", endpointGuid.g));
            return;
        }

        connectionRequests[connectionRequestIndex]->connectionRequestSystemsMutex.Lock();
        Nui32 connectionRequestGuidIndex = connectionRequests[connectionRequestIndex]->GetGuidIndex(msg->mID);
        if (connectionRequestGuidIndex==(Nui32)-1)
        {
            connectionRequests[connectionRequestIndex]->connectionRequestSystemsMutex.Unlock();
            connectionRequestsMutex.Unlock();
            String buff;
            if (debugInterface)	debugInterface->ShowFailure(StrUtil::format(buff, "Router2 failed (%I64d)\n", endpointGuid.g));
            return;
        }

        if (debugInterface)
        {
            String buff;
            String buff2;
            msg->mAddress.read(buff2, true);
            debugInterface->ShowDiagnostic(StrUtil::format(buff,"Got ID_ROUTER_2_REPLY_FORWARDING on address %s(%I64d -> %I64d) canForward=%i\n",
                buff2, msg->mID.g, endpointGuid.g, canForward));
        }

        if (canForward)
        {
            Nui16 pingToEndpoint;
            Nui16 usedEntries;
            bs.read(pingToEndpoint);
            bs.read(usedEntries);
            connectionRequests[connectionRequestIndex]->connectionRequestSystems[connectionRequestGuidIndex].usedForwardingEntries=usedEntries;
            connectionRequests[connectionRequestIndex]->connectionRequestSystems[connectionRequestGuidIndex].pingToEndpoint=mPrc2->getAvgPing(msg->mID)+pingToEndpoint;
        }
        else
        {
            connectionRequests[connectionRequestIndex]->connectionRequestSystems.RemoveAtIndex(connectionRequestGuidIndex);
        }
        connectionRequests[connectionRequestIndex]->connectionRequestSystemsMutex.Unlock();

        if (UpdateForwarding(connectionRequests[connectionRequestIndex])==false)
        {
            RemoveConnectionRequest(connectionRequestIndex);
        }
        connectionRequestsMutex.Unlock();
    }
    //------------------------------------------------------------------------
    void Router2::SendForwardingSuccess(Token messageId, UniqueID sourceGuid, UniqueID endpointGuid, Nui16 sourceToDstPort)
    {
        NetSerializer bsOut;
        bsOut.write(messageId);
        bsOut.write(endpointGuid);
        bsOut.write(sourceToDstPort);
        mPrc2->send(&bsOut, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, sourceGuid, false);

        if (debugInterface)
        {
            String buff;
            if (messageId==ID_ROUTER_2_FORWARDING_ESTABLISHED)
                debugInterface->ShowDiagnostic(StrUtil::format(buff,"Sending ID_ROUTER_2_FORWARDING_ESTABLISHED at %s:%i\n"));
            else
                debugInterface->ShowDiagnostic(StrUtil::format(buff,"Sending ID_ROUTER_2_REROUTED at %s:%i\n"));
        }
    }
    //------------------------------------------------------------------------
    void Router2::SendOOBFromRakNetPort(OutOfBandIdentifiers oob, NetSerializer * extraData, Address * sa)
    {
        NetSerializer oobBs;
        oobBs.write((Nui8)oob);
        if(extraData)
        {
            extraData->seekRead(0);
            oobBs.write(*extraData);
        }
        String ip;
        sa->read(ip, false);
        mPrc2->sendExpedite(ip, sa->getHSBPort(), oobBs.getData(), oobBs.getSize());
    }
    //------------------------------------------------------------------------
    void Router2::SendOOBFromSpecifiedSocket(OutOfBandIdentifiers oob,
        Address * sa, Nudpsocket socket)
    {
        NetSerializer bs;
        bs.write((Token)ID_OUT_OF_BAND_INTERNAL);
        bs.write(mPrc2->getGuid());
        bs.write((const Nui8 *)OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
        bs.write((Nui8) oob);

        if (sa.mSrc.sin_family==AF_INET)
        {
            N_net_sendto(socket, (const char *)bs.getData(), bs.getSize(),
                0, (const sockaddr *) & sa.mSrc, sizeof(sockaddr_in));
        }
        else
        {
            #if N_NET_PROTOCOL_VER==6
            N_net_sendto(socket, (const char *)bs.getData(), bs.getSize(),
                0, (const sockaddr *) & sa.mSrc, sizeof(sockaddr_in6));
            #endif
        }
    }
    //------------------------------------------------------------------------
    void Router2::SendOOBMessages(Router2::MiniPunchRequest * mpr)
    {
        // Mini NAT punch
        // send from srcToDestPort to msg->mAddress (source). If the message arrives, the remote system should reply.
        SendOOBFromSpecifiedSocket(ID_ROUTER_2_REPLY_TO_SENDER_PORT, mpr->sourceAddress, mpr->forwardingSocket);

        // send from destToSourcePort to endpointSystemAddress (destination). If the message arrives, the remote system should reply.
        SendOOBFromSpecifiedSocket(ID_ROUTER_2_REPLY_TO_SENDER_PORT, mpr->endpointAddress, mpr->forwardingSocket);

        if (debugInterface)
        {
            String buff;

            String buff2;

            mpr->sourceAddress.read(buff2, true);

            debugInterface->ShowDiagnostic(StrUtil::format(buff,"call SendOOBFromSpecifiedSocket(...,%s,...)", buff2));

            mpr->endpointAddress.read(buff2, true);

            debugInterface->ShowDiagnostic(StrUtil::format(buff,"call SendOOBFromSpecifiedSocket(...,%s,...)", buff2));
        }

        // Tell source to send to forwardingPort
        NetSerializer extraData;
        extraData.write(mpr->forwardingPort);
        N_assert(mpr->forwardingPort != 0);
        SendOOBFromRakNetPort(ID_ROUTER_2_REPLY_TO_SPECIFIED_PORT, &extraData, mpr->sourceAddress);

        // Tell destination to send to forwardingPort
        extraData.seekWrite(0);
        extraData.seekRead(0);
        extraData.write(mpr->forwardingPort);
        N_assert(mpr->forwardingPort);
        SendOOBFromRakNetPort(ID_ROUTER_2_REPLY_TO_SPECIFIED_PORT, &extraData, mpr->endpointAddress);
    }
    //------------------------------------------------------------------------
    void Router2::OnRequestForwarding(SocketMessage * msg)
    {
        NetSerializer bs(msg->mData, msg->mLength, false);
        bs.skipRead(2, 0);
        UniqueID endpointGuid;
        bs.read(endpointGuid);

        int pingToEndpoint = ReturnFailureOnCannotForward(msg->mID, endpointGuid);
        if (pingToEndpoint==-1)
        {
            String buff;
            if (debugInterface)	debugInterface->ShowFailure(StrUtil::format(buff,"Router2 failed (msg->mID =%I64d, endpointGuid = %I64d)\n",
                    msg->mID.g, endpointGuid.g));
            return;
        }

        Nui16 forwardingPort=0;
        Nudpsocket forwardingSocket = -1;
        Address * endpointSystemAddress = mPrc2->getAddress(endpointGuid);
        UDPForwarderResult result = udpForwarder->StartForwarding(
            msg->mAddress, endpointSystemAddress, 30000, 0, socketFamily,
            &forwardingPort, &forwardingSocket);

        if (result==UDPFORWARDER_FORWARDING_ALREADY_EXISTS)
        {
            if (debugInterface)
            {
                String buff;
                debugInterface->ShowDiagnostic(StrUtil::format(buff,"Got ID_ROUTER_2_REQUEST_FORWARDING, result=UDPFORWARDER_FORWARDING_ALREADY_EXISTS "
                        "(msg->mID =%I64d, endpointGuid = %I64d)\n",
                    msg->mID.g, endpointGuid.g));
            }

            SendForwardingSuccess(ID_ROUTER_2_FORWARDING_ESTABLISHED, msg->mID, endpointGuid, forwardingPort);
        }
        else if (result==UDPFORWARDER_NO_SOCKETS)
        {
            String buff;
            String buff2, buff3;
            msg->mAddress.read(buff2, true);
            endpointSystemAddress.read(buff3, true);
            if (debugInterface)
                debugInterface->ShowFailure(StrUtil::format(buff,"Router2 with UDPFORWARDER_NO_SOCKETS, msg->mAddress=%s, endpointSystemAddress=%s, forwardingPort=%i, forwardingSocket=%i\n",
                    buff2, buff3, forwardingPort, forwardingSocket));
            SendFailureOnCannotForward(msg->mID, endpointGuid);
        }
        else if (result == UDPFORWARDER_INVALID_PARAMETERS)
        {
            String buff;
            String buff2, buff3;
            msg->mAddress.read(buff2, true);
            endpointSystemAddress.read(buff3, true);
            if (debugInterface)
                debugInterface->ShowFailure(StrUtil::format(buff,"Router2 with UDPFORWARDER_INVALID_PARAMETERS, msg->mAddress=%s, endpointSystemAddress=%s, forwardingPort=%i, forwardingSocket=%i\n",
                    buff2, buff3, forwardingPort, forwardingSocket));
            SendFailureOnCannotForward(msg->mID, endpointGuid);
        }
        else if (result==UDPFORWARDER_BIND_FAILED)
        {
            String buff;
            String buff2, buff3;
            msg->mAddress.read(buff2, true);
            endpointSystemAddress.read(buff3, true);
            if (debugInterface)
                debugInterface->ShowFailure(StrUtil::format(buff,"Router2 with UDPFORWARDER_BIND_FAILED, msg->mAddress=%s, endpointSystemAddress=%s, forwardingPort=%i, forwardingSocket=%i\n",
                    buff2, buff3, forwardingPort, forwardingSocket));
            SendFailureOnCannotForward(msg->mID, endpointGuid);
        }
        else
        {
            if (debugInterface)
            {
                String buff2, buff3;
                endpointSystemAddress.read(buff2, true);
                msg->mAddress.read(buff3, true);
                String buff;
                debugInterface->ShowDiagnostic(StrUtil::format(buff,"Got ID_ROUTER_2_REQUEST_FORWARDING.\n"
                    "endpointAddress=%s\nsourceAddress=%s\nforwardingPort=%i\n "
                    "calling SendOOBMessages at %s:%i\n", buff2,buff3,forwardingPort));
            }

            // Store the punch request
            MiniPunchRequest miniPunchRequest;
            miniPunchRequest.endpointAddress=endpointSystemAddress;
            miniPunchRequest.endpointGuid=endpointGuid;
            miniPunchRequest.gotReplyFromEndpoint=false;
            miniPunchRequest.gotReplyFromSource=false;
            miniPunchRequest.sourceGuid=msg->mID;
            miniPunchRequest.sourceAddress=msg->mAddress;
            miniPunchRequest.forwardingPort=forwardingPort;
            miniPunchRequest.forwardingSocket=forwardingSocket;
            int ping1 = mPrc2->getAvgPing(msg->mID);
            int ping2 = mPrc2->getAvgPing(endpointGuid);
            if (ping1>ping2)
                miniPunchRequest.timeout = N_Engine().getTimer().getMS() + ping1*8+300;
            else
                miniPunchRequest.timeout = N_Engine().getTimer().getMS() + ping2*8+300;
            miniPunchRequest.nextAction = N_Engine().getTimer().getMS() + 100;
            SendOOBMessages(&miniPunchRequest);
            miniPunchesInProgressMutex.Lock();
            miniPunchesInProgress.Push(miniPunchRequest);
            miniPunchesInProgressMutex.Unlock();
        }
    }
    //------------------------------------------------------------------------
    void Router2::OnMiniPunchReplyBounce(SocketMessage *msg)
    {
        // Find stored punch request
        Nui32 i=0;

        if (debugInterface)
        {
            String buff;
            debugInterface->ShowDiagnostic(StrUtil::format(buff,"Got ID_ROUTER_2_MINI_PUNCH_REPLY_BOUNCE from id=%I64d (miniPunchesInProgress.Size() = %d)",
                msg->mID.g, miniPunchesInProgress.Size()));
        }

        miniPunchesInProgressMutex.Lock();
        while (i < miniPunchesInProgress.Size())
        {
            if (miniPunchesInProgress[i].sourceGuid==msg->mID || miniPunchesInProgress[i].endpointGuid==msg->mID)
            {
                if (miniPunchesInProgress[i].sourceGuid==msg->mID)
                    miniPunchesInProgress[i].gotReplyFromSource=true;
                if (miniPunchesInProgress[i].endpointGuid==msg->mID)
                    miniPunchesInProgress[i].gotReplyFromEndpoint=true;

                if (debugInterface)
                {
                    String buff;
                    debugInterface->ShowDiagnostic(StrUtil::format(buff, "Processing ID_ROUTER_2_MINI_PUNCH_REPLY_BOUNCE, gotReplyFromSource=%i gotReplyFromEndpoint=%i\n", miniPunchesInProgress[i].gotReplyFromSource, miniPunchesInProgress[i].gotReplyFromEndpoint));
                }

                if (miniPunchesInProgress[i].gotReplyFromEndpoint==true &&
                    miniPunchesInProgress[i].gotReplyFromSource==true)
                {
                    SendForwardingSuccess(ID_ROUTER_2_REROUTED,               miniPunchesInProgress[i].endpointGuid, miniPunchesInProgress[i].sourceGuid,   miniPunchesInProgress[i].forwardingPort);
                    SendForwardingSuccess(ID_ROUTER_2_FORWARDING_ESTABLISHED, miniPunchesInProgress[i].sourceGuid,   miniPunchesInProgress[i].endpointGuid, miniPunchesInProgress[i].forwardingPort);
                    miniPunchesInProgress.RemoveAtIndexFast(i);
                }
                else
                {
                    i++;
                }
            }
            else
                i++;
        }
        miniPunchesInProgressMutex.Unlock();
    }
    //------------------------------------------------------------------------
    void Router2::OnMiniPunchReply(SocketMessage * msg)
    {
        NetSerializer bs(msg->mData, msg->mLength, false);
        bs.skipRead(2, 0);
        UniqueID routerGuid;
        bs.read(routerGuid);
        SendOOBFromRakNetPort(ID_ROUTER_2_MINI_PUNCH_REPLY_BOUNCE, 0, mPrc2->getAddress(routerGuid));

        if(debugInterface)
        {
            String buff;

            String buff2;
            mPrc2->getAddress(routerGuid).read(buff2, true);

            debugInterface->ShowDiagnostic(StrUtil::format(buff, "Sending ID_ROUTER_2_MINI_PUNCH_REPLY_BOUNCE (%s)\n", buff2));
        }
    }
    //------------------------------------------------------------------------
    void Router2::OnRerouted(SocketMessage * msg)
    {
        NetSerializer bs(msg->mData, msg->mLength, false);
        bs.skipRead(1, 0);
        UniqueID endpointGuid;
        bs.read(endpointGuid);
        Nui16 sourceToDestPort;
        bs.read(sourceToDestPort);

        // Return rerouted notice
        Address * intermediaryAddress=msg->mAddress;
        intermediaryAddress.setHSBPort(sourceToDestPort);
        mPrc2->swap(endpointGuid, intermediaryAddress);

        Nui32 forwardingIndex;
        forwardedConnectionListMutex.Lock();
        for (forwardingIndex=0; forwardingIndex < forwardedConnectionList.Size(); forwardingIndex++)
        {
            if (forwardedConnectionList[forwardingIndex].endpointGuid==endpointGuid)
                break;
        }

        if (forwardingIndex<forwardedConnectionList.Size())
        {
            ForwardedConnection & ref_fc = forwardedConnectionList[forwardingIndex];
            forwardedConnectionListMutex.Unlock();

            ref_fc.intermediaryAddress = msg->mAddress;
            ref_fc.intermediaryAddress.setHSBPort(sourceToDestPort);
            ref_fc.intermediaryGuid = msg->mID;

            mPrc2->swap(endpointGuid, intermediaryAddress);

            if(debugInterface)
            {
                String buff;
                String temp;
                intermediaryAddress.read(temp, true);
                debugInterface->ShowDiagnostic(StrUtil::format(buff,"FIX: Got ID_ROUTER_2_REROUTE, returning ID_ROUTER_2_REROUTED,"
                    " Calling NetworkSys::swap(%I64d, %s)\n", endpointGuid.g, temp));
            }
        }
        else
        {
            ForwardedConnection fc;
            fc.endpointGuid = endpointGuid;
            fc.intermediaryAddress = msg->mAddress;
            fc.intermediaryAddress.setHSBPort(sourceToDestPort);
            fc.intermediaryGuid = msg->mID;
            fc.weInitiatedForwarding = false;
            // add to forwarding list. This is only here to avoid reporting direct connections in Router2::ReturnFailureOnCannotForward
            forwardedConnectionList.Push(fc);
            forwardedConnectionListMutex.Unlock();

            mPrc2->swap(endpointGuid, intermediaryAddress);

            if (debugInterface)
            {
                String buff;
                debugInterface->ShowDiagnostic(StrUtil::format(buff,"Got ID_ROUTER_2_REROUTE, returning ID_ROUTER_2_REROUTED, Calling NetworkSys::swap\n"));
            }
        }
    }
    //------------------------------------------------------------------------
    bool Router2::OnForwardingSuccess(SocketMessage * msg)
    {
        NetSerializer bs(msg->mData, msg->mLength, false);
        bs.skipRead(1, 0);
        UniqueID endpointGuid;
        bs.read(endpointGuid);
        Nui16 sourceToDestPort;
        bs.read(sourceToDestPort);

        Nui32 forwardingIndex;
        forwardedConnectionListMutex.Lock();
        for (forwardingIndex = 0; forwardingIndex < forwardedConnectionList.Size(); forwardingIndex++)
        {
            if (forwardedConnectionList[forwardingIndex].endpointGuid==endpointGuid)
                break;
        }

        if (forwardingIndex < forwardedConnectionList.Size())
        {
            // Return rerouted notice
            Address * intermediaryAddress = msg->mAddress;
            intermediaryAddress.setHSBPort(sourceToDestPort);
            mPrc2->swap(endpointGuid, intermediaryAddress);

            ////////////////////////////////////////////////////////////////////////////
            ForwardedConnection& ref_fc     = forwardedConnectionList[forwardingIndex];
            ref_fc.intermediaryAddress      = msg->mAddress;
            ref_fc.intermediaryAddress.setHSBPort(sourceToDestPort);
            ref_fc.intermediaryGuid         = msg->mID;
            ////////////////////////////////////////////////////////////////////////////

            if (debugInterface)
            {
                String buff;
                debugInterface->ShowDiagnostic(StrUtil::format(buff,"Got ID_ROUTER_2_FORWARDING_ESTABLISHED, returning ID_ROUTER_2_REROUTED, Calling NetworkSys::swap at %s:%i\n"));
            }

            msg->mData[0]=ID_ROUTER_2_REROUTED;

            forwardedConnectionListMutex.Unlock();
            return true; // Return msg to user
        }
        else
        {
            forwardedConnectionListMutex.Unlock();

            // removeFrom connectionRequests;
            ForwardedConnection fc;
            connectionRequestsMutex.Lock();
            Nui32 connectionRequestIndex = GetConnectionRequestIndex(endpointGuid);
            fc.returnConnectionLostOnFailure=connectionRequests[connectionRequestIndex]->returnConnectionLostOnFailure;
            connectionRequests.RemoveAtIndexFast(connectionRequestIndex);
            connectionRequestsMutex.Unlock();
            fc.endpointGuid=endpointGuid;
            fc.intermediaryAddress=msg->mAddress;
            fc.intermediaryAddress.setHSBPort(sourceToDestPort);
            fc.intermediaryGuid=msg->mID;
            fc.weInitiatedForwarding=true;

            // add to forwarding list
            forwardedConnectionListMutex.Lock();
            forwardedConnectionList.Push (fc);
            forwardedConnectionListMutex.Unlock();

            if (debugInterface)
            {
                String buff;
                debugInterface->ShowDiagnostic(StrUtil::format(buff,"Got and returning to user ID_ROUTER_2_FORWARDING_ESTABLISHED at %s:%i\n"));
            }

        }
        return true; // Return msg to user
    }
    //------------------------------------------------------------------------
    int Router2::GetLargestPingAmongConnectedSystems(void) const
    {
        int avePing;
        int largestPing = -1;
        Nui32 maxPeers = mPrc2->getRemoteCount();
        if(maxPeers == 0)
            return 9999;
        Nindex index;
        for(index = 0; index < mPrc2->getRemoteCount(); ++index)
        {
            UniqueID g = mPrc2->getGUID(index);
            if(g != UniqueID::INVALID)
            {
                avePing = mPrc2->getAvgPing(mPrc2->getGUID(index));
                if(avePing > largestPing)
                    largestPing = avePing;
            }
        }
        return largestPing;
    }
    //------------------------------------------------------------------------
    Nui32 Router2::GetConnectionRequestIndex(UniqueID endpointGuid)
    {
        Nui32 i;
        for (i=0; i < connectionRequests.Size(); ++i)
        {
            if (connectionRequests[i]->endpointGuid==endpointGuid)
                return i;
        }
        return (Nui32) -1;
    }
    //------------------------------------------------------------------------
    Nui32 Router2::ConnnectRequest::GetGuidIndex(UniqueID id)
    {
        NCount i;
        for (i=0; i < connectionRequestSystems.Size(); ++i)
        {
            if (connectionRequestSystems[i].mID == id)
                return i;
        }
        return (Nui32) -1;
    }
    //------------------------------------------------------------------------
    void Router2::ReturnToUser(Token messageId, UniqueID endpointGuid,
        const Address & address, bool local)
    {
        SocketMessage * p = createMessage(sizeof(Token) + sizeof(Nui8));
        p->mData[0] = messageId;
        p->mAddress = address;
        p->mAddress.mIndex = (Nindex)-1;
        p->mID  = endpointGuid;
        p->mLocal = local;
        mPrc2->addMessage(p, true);
    }
    //------------------------------------------------------------------------
    void Router2::ClearForwardedConnections(void)
    {
        forwardedConnectionListMutex.Lock();
        forwardedConnectionList.Clear(false);
        forwardedConnectionListMutex.Unlock();
    }
    //------------------------------------------------------------------------
    void Router2::ClearAll(void)
    {
        ClearConnectionRequests();
        ClearMinipunches();
        ClearForwardedConnections();
    }
    //------------------------------------------------------------------------
    void Router2::SetDebugInterface(Router2DebugInterface * _debugInterface)
    {
        debugInterface=_debugInterface;
    }
    //------------------------------------------------------------------------
    Router2DebugInterface *Router2::GetDebugInterface(void) const
    {
        return debugInterface;
    }
    //------------------------------------------------------------------------
}
}
#endif