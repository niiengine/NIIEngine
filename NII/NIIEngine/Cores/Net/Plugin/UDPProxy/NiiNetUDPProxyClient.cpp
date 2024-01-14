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
#if _RAKNET_SUPPORT_UDPProxyClient == 1

#include "NiiPreProcess.h"
#include "UDPProxyClient.h"
#include "NiiNetSerializer.h"
#include "UDPProxyCommon.h"
#include "NiiNetLinkBase.h"
#include "NiiNetCommon.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    static const int DEFAULT_UNRESPONSIVE_PING_TIME_COORDINATOR = 1000;
    //------------------------------------------------------------------------
    UDPProxyClient::UDPProxyClient()
    {
        resultHandler=0;
    }
    //------------------------------------------------------------------------
    UDPProxyClient::~UDPProxyClient()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void UDPProxyClient::SetResultHandler(UDPProxyClientResultHandler * rh)
    {
        resultHandler=rh;
    }
    //------------------------------------------------------------------------
    bool UDPProxyClient::RequestForwarding(Address * proxyCoordinator,
        Address * sourceAddress, UniqueID targetGuid, TimeDurMS timeoutOnNoDataMS,
            NetSerializer * serverSelectionBitstream)
    {
        // Return false if not connected
        ConnectionState cs = mPrc2->getState(proxyCoordinator);
        if (cs!=IS_CONNECTED)
            return false;

        // Pretty much a bug not to set the result handler, as otherwise you won't know if the operation succeeed or not
        N_assert(resultHandler != 0);
        if (resultHandler == 0)
            return false;

        NetSerializer out;
        out.write((Token)ID_UDP_PROXY_GENERAL);
        out.write((Token)ID_UDP_PROXY_FORWARDING_REQUEST_FROM_CLIENT_TO_COORDINATOR);
        out.write(sourceAddress);
        out.write0();
        out.write(targetGuid);
        out.write(timeoutOnNoDataMS);
        if (serverSelectionBitstream && serverSelectionBitstream->getWriteOffset() > 0)
        {
            out.write1();
            out.write(serverSelectionBitstream);
        }
        else
        {
            out.write0();
        }
        mPrc2->send(&out, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, proxyCoordinator, false);

        return true;
    }
    //------------------------------------------------------------------------
    bool UDPProxyClient::RequestForwarding(Address * proxyCoordinator,
        Address * sourceAddress, Address * targetAddressAsSeenFromCoordinator,
            TimeDurMS timeoutOnNoDataMS, NetSerializer * serverSelectionBitstream)
    {
        // Return false if not connected
        ConnectionState cs = mPrc2->getState(proxyCoordinator);
        if(cs != IS_CONNECTED)
            return false;

        // Pretty much a bug not to set the result handler, as otherwise you won't know if the operation succeeed or not
        N_assert(resultHandler != 0);
        if(resultHandler == 0)
            return false;

        NetSerializer out;
        out.write((Token)ID_UDP_PROXY_GENERAL);
        out.write((Token)ID_UDP_PROXY_FORWARDING_REQUEST_FROM_CLIENT_TO_COORDINATOR);
        out.write(sourceAddress);
        out.write1();
        out.write(targetAddressAsSeenFromCoordinator);
        out.write(timeoutOnNoDataMS);
        if (serverSelectionBitstream && serverSelectionBitstream->getWriteOffset() > 0)
        {
            out.write1();
            out.write(serverSelectionBitstream);
        }
        else
        {
            out.write0();
        }
        mPrc2->send(&out, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, proxyCoordinator, false);

        return true;
    }
    //------------------------------------------------------------------------
    void UDPProxyClient::update()
    {
        Nui32 idx1 = 0;
        while (idx1 < pingServerGroups.Size())
        {
            PingServerGroup * psg = pingServerGroups[idx1];

            if (psg->serversToPing.Size() > 0 &&
                N_Engine().getTimer().getMS() > psg->startPingTime+DEFAULT_UNRESPONSIVE_PING_TIME_COORDINATOR)
            {
                // If they didn't reply within DEFAULT_UNRESPONSIVE_PING_TIME_COORDINATOR, just give up on them
                psg->SendPingedServersToCoordinator(mPrc2);

                N_delete psg;
                pingServerGroups.RemoveAtIndex(idx1);
            }
            else
                idx1++;
        }
    }
    //------------------------------------------------------------------------
    ConnectReceiveType UDPProxyClient::onMessage(SocketMessage * msg)
    {
        if (msg->mData[0]==ID_UNCONNECTED_PONG)
        {
            NCount idx1, idx2;
            PingServerGroup * psg;
            for (idx1 = 0; idx1 < pingServerGroups.Size(); ++idx1)
            {
                psg = pingServerGroups[idx1];
                for (idx2 = 0; idx2 < psg->serversToPing.Size(); ++idx2)
                {
                    if (psg->serversToPing[idx2].serverAddress == msg->mAddress)
                    {
                        NetSerializer bsIn(msg->mData, msg->mLength, false);
                        bsIn.skipRead(1, 0);
                        TimeDurMS sentTime;
                        bsIn.read(sentTime);
                        TimeDurMS curTime = N_Engine().getTimer().getMS();
                        int ping;
                        if (curTime > sentTime)
                            ping = (int)(curTime - sentTime);
                        else
                            ping = 0;
                        psg->serversToPing[idx2].ping = (Nui16)ping;

                        // If all servers to ping are now pinged, reply to coordinator
                        if (psg->AreAllServersPinged())
                        {
                            psg->SendPingedServersToCoordinator(mPrc2);
                            N_delete psg;
                            pingServerGroups.RemoveAtIndex(idx1);
                        }

                        return CRT_Destroy;
                    }
                }

            }
        }
        else if (msg->mData[0] == ID_UDP_PROXY_GENERAL && msg->mLength > 1)
        {
            switch (msg->mData[1])
            {
            case ID_UDP_PROXY_PING_SERVERS_FROM_COORDINATOR_TO_CLIENT:
                OnPingServers(msg);
                break;
            case ID_UDP_PROXY_FORWARDING_SUCCEEDED:
            case ID_UDP_PROXY_ALL_SERVERS_BUSY:
            case ID_UDP_PROXY_IN_PROGRESS:
            case ID_UDP_PROXY_NO_SERVERS_ONLINE:
            case ID_UDP_PROXY_RECIPIENT_GUID_NOT_CONNECTED_TO_COORDINATOR:
            case ID_UDP_PROXY_FORWARDING_NOTIFICATION:
                {
                    UniqueID targetGuid;
                    Address * senderAddress, * targetAddress;
                    NetSerializer incomingBs(msg->mData, msg->mLength, false);
                    incomingBs.skipRead(2, 0);
                    incomingBs.read(senderAddress);
                    incomingBs.read(targetAddress);
                    incomingBs.read(targetGuid);

                    switch (msg->mData[1])
                    {
                    case ID_UDP_PROXY_FORWARDING_NOTIFICATION:
                    case ID_UDP_PROXY_FORWARDING_SUCCEEDED:
                    case ID_UDP_PROXY_IN_PROGRESS:
                        {
                            Nui16 forwardingPort;
                            RakString serverIP;
                            incomingBs.read(serverIP);
                            incomingBs.read(forwardingPort);
                            if (msg->mData[1]==ID_UDP_PROXY_FORWARDING_SUCCEEDED)
                            {
                                if (resultHandler)
                                    resultHandler->OnForwardingSuccess(serverIP.C_String(), forwardingPort, msg->mAddress, senderAddress, targetAddress, targetGuid, this);
                            }
                            else if (msg->mData[1]==ID_UDP_PROXY_IN_PROGRESS)
                            {
                                if (resultHandler)
                                    resultHandler->OnForwardingInProgress(serverIP.C_String(), forwardingPort, msg->mAddress, senderAddress, targetAddress, targetGuid, this);
                            }
                            else
                            {
                                // send a datagram to the proxy, so if we are behind a router, that router adds an entry to the routing table.
                                // Otherwise the router would block the incoming datagrams from source
                                // It doesn't matter if the message actually arrives as long as it goes through the router
                                mPrc2->ping(serverIP, forwardingPort, false);

                                if (resultHandler)
                                    resultHandler->OnForwardingNotification(serverIP.C_String(), forwardingPort, msg->mAddress, senderAddress, targetAddress, targetGuid, this);
                            }
                        }
                        break;
                    case ID_UDP_PROXY_ALL_SERVERS_BUSY:
                        if (resultHandler)
                            resultHandler->OnAllServersBusy(msg->mAddress, senderAddress, targetAddress, targetGuid, this);
                        break;
                    case ID_UDP_PROXY_NO_SERVERS_ONLINE:
                        if (resultHandler)
                            resultHandler->OnNoServersOnline(msg->mAddress, senderAddress, targetAddress, targetGuid, this);
                        break;
                    case ID_UDP_PROXY_RECIPIENT_GUID_NOT_CONNECTED_TO_COORDINATOR:
                        {
                            if (resultHandler)
                                resultHandler->OnRecipientNotConnected(msg->mAddress, senderAddress, targetAddress, targetGuid, this);
                            break;
                        }
                    }

                }
                return CRT_Destroy;
            }
        }
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void UDPProxyClient::shutdown()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void UDPProxyClient::OnPingServers(SocketMessage * msg)
    {
        NetSerializer incomingBs(msg->mData, msg->mLength, false);
        incomingBs.skipRead(2, 0);
        PingServerGroup * psg = N_new PingServerGroup();

        ServerWithPing swp;
        incomingBs.read(psg->sata.senderClientAddress);
        incomingBs.read(psg->sata.targetClientAddress);
        psg->startPingTime = N_Engine().getTimer().getMS();
        psg->coordinatorAddressForPings = msg->mAddress;
        Nui16 serverListSize;
        incomingBs.read(serverListSize);
        Address * serverAddress;
        Nui16 serverListIndex;
        String ipStr;
        for(serverListIndex = 0; serverListIndex < serverListSize; ++serverListIndex)
        {
            incomingBs.read(swp.serverAddress);
            swp.ping = DEFAULT_UNRESPONSIVE_PING_TIME_COORDINATOR;
            psg->serversToPing.Push(swp);
            swp.serverAddress.read(ipStr, false);
            mPrc2->ping(ipStr, swp.serverAddress.getHSBPort(), false, 0);
        }
        pingServerGroups.Push(psg);
    }
    //------------------------------------------------------------------------
    bool UDPProxyClient::PingServerGroup::AreAllServersPinged() const
    {
        Nui32 serversToPingIndex;
        for(serversToPingIndex = 0; serversToPingIndex < serversToPing.Size(); serversToPingIndex++)
        {
            if (serversToPing[serversToPingIndex].ping == DEFAULT_UNRESPONSIVE_PING_TIME_COORDINATOR)
                return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    void UDPProxyClient::PingServerGroup::SendPingedServersToCoordinator(LinkBase * linkbase)
    {
        NetSerializer out;
        out.write((Token)ID_UDP_PROXY_GENERAL);
        out.write((Token)ID_UDP_PROXY_PING_SERVERS_REPLY_FROM_CLIENT_TO_COORDINATOR);
        out.write(sata.senderClientAddress);
        out.write(sata.targetClientAddress);
        Nui16 serversToPingSize = (Nui16) serversToPing.Size();
        out.write(serversToPingSize);
        NCount i;
        for(i = 0; i < serversToPingSize; ++i)
        {
            out.write(serversToPing[i].serverAddress);
            out.write(serversToPing[i].ping);
        }
        linkbase->send(&out, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, coordinatorAddressForPings, false);
    }
    //------------------------------------------------------------------------
    void UDPProxyClient::Clear(void)
    {
        for(NCount i = 0; i < pingServerGroups.Size(); ++i)
            N_delete pingServerGroups[i];
        pingServerGroups.Clear(false);
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*