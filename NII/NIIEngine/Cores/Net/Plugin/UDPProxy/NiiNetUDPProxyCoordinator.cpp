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
#if _RAKNET_SUPPORT_UDPProxyCoordinator == 1 && _RAKNET_SUPPORT_UDPForwarder == 1

#include "UDPProxyCoordinator.h"
#include "NiiNetSerializer.h"
#include "UDPProxyCommon.h"
#include "NiiNetLinkBase.h"
#include "NiiNetCommon.h"
#include "Rand.h"
#include "UDPForwarder.h"

namespace NII
{
namespace NII_NET
{
    // Larger than the client version
    static const int DEFAULT_CLIENT_UNRESPONSIVE_PING_TIME=2000;
    static const int DEFAULT_UNRESPONSIVE_PING_TIME_COORDINATOR=DEFAULT_CLIENT_UNRESPONSIVE_PING_TIME+1000;
    //------------------------------------------------------------------------
    int UDPProxyCoordinator::ServerWithPingComp(const Nui16 & key,
        const UDPProxyCoordinator::ServerWithPing & data)
    {
        if (key < data.ping)
            return -1;
        if (key > data.ping)
            return 1;
        return 0;
    }
    //------------------------------------------------------------------------
    int UDPProxyCoordinator::ForwardingRequestComp(const SenderAndTargetAddress & key,
        ForwardingRequest * const & data)
    {
        if (key.senderClientAddress < data->sata.senderClientAddress)
            return -1;
        if (key.senderClientAddress > data->sata.senderClientAddress)
            return -1;
        if (key.targetClientAddress < data->sata.targetClientAddress)
            return -1;
        if (key.targetClientAddress > data->sata.targetClientAddress)
            return 1;
        return 0;
    }
    //------------------------------------------------------------------------
    UDPProxyCoordinator::UDPProxyCoordinator()
    {

    }
    //------------------------------------------------------------------------
    UDPProxyCoordinator::~UDPProxyCoordinator()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void UDPProxyCoordinator::SetRemoteLoginPassword(RakString password)
    {
        remoteLoginPassword=password;
    }
    //------------------------------------------------------------------------
    void UDPProxyCoordinator::update(void)
    {
        Nui32 idx;
        TimeDurMS curTime = N_Engine().getTimer().getMS();
        ForwardingRequest * fw;
        idx = 0;
        while(idx < forwardingRequestList.Size())
        {
            fw = forwardingRequestList[idx];
            if(fw->timeRequestedPings!=0 &&
                curTime > fw->timeRequestedPings + DEFAULT_UNRESPONSIVE_PING_TIME_COORDINATOR)
            {
                fw->OrderRemainingServersToTry();
                fw->timeRequestedPings=0;
                TryNextServer(fw->sata, fw);
                idx++;
            }
            else if(fw->timeoutAfterSuccess != 0 && curTime > fw->timeoutAfterSuccess)
            {
                // Forwarding request succeeded, we waited a bit to prevent duplicates. Can forget about the entry now.
                N_delete fw;
                forwardingRequestList.RemoveAtIndex(idx);
            }
            else
                idx++;
        }
    }
    //------------------------------------------------------------------------
    ConnectReceiveType UDPProxyCoordinator::onMessage(SocketMessage * msg)
    {
        if (msg->mData[0] == ID_UDP_PROXY_GENERAL && msg->mLength > 1)
        {
            switch (msg->mData[1])
            {
            case ID_UDP_PROXY_FORWARDING_REQUEST_FROM_CLIENT_TO_COORDINATOR:
                OnForwardingRequestFromClientToCoordinator(msg);
                return CRT_Destroy;
            case ID_UDP_PROXY_LOGIN_REQUEST_FROM_SERVER_TO_COORDINATOR:
                OnLoginRequestFromServerToCoordinator(msg);
                return CRT_Destroy;
            case ID_UDP_PROXY_FORWARDING_REPLY_FROM_SERVER_TO_COORDINATOR:
                OnForwardingReplyFromServerToCoordinator(msg);
                return CRT_Destroy;
            case ID_UDP_PROXY_PING_SERVERS_REPLY_FROM_CLIENT_TO_COORDINATOR:
                OnPingServersReplyFromClientToCoordinator(msg);
                return CRT_Destroy;
            }
        }
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void UDPProxyCoordinator::onConnectClose(const Address & address, UniqueID id,
        ConnectCloseType lostConnectionReason)
    {
        (void) lostConnectionReason;
        (void) id;

        Nui32 idx, idx2;

        idx = 0;
        while (idx < forwardingRequestList.Size())
        {
            if (forwardingRequestList[idx]->requestingAddress == address)
            {
                // Guy disconnected before the attempt completed
                N_delete forwardingRequestList[idx];
                forwardingRequestList.RemoveAtIndex(idx );
            }
            else
                idx++;
        }

        idx = serverList.GetIndexOf(address);
        if (idx!=(Nui32)-1)
        {
            ForwardingRequest *fw;
            // For each pending client for this server, choose from remaining servers.
            for (idx2=0; idx2 < forwardingRequestList.Size(); idx2++)
            {
                fw = forwardingRequestList[idx2];
                if (fw->currentlyAttemptedServerAddress==address)
                {
                    // Try the next server
                    TryNextServer(fw->sata, fw);
                }
            }

            // Remove dead server
            serverList.RemoveAtIndexFast(idx);
        }
    }
    //------------------------------------------------------------------------
    void UDPProxyCoordinator::OnForwardingRequestFromClientToCoordinator(SocketMessage *msg)
    {
        NetSerializer incomingBs(msg->mData, msg->mLength, false);
        incomingBs.skipRead(2, 0);
        Address * sourceAddress;
        incomingBs.read(sourceAddress);
        if (sourceAddress==0 || sourceAddress->isInvalid())
            sourceAddress=msg->mAddress;
        Address * targetAddress;
        UniqueID targetGuid;
        bool usesAddress=false;
        incomingBs.read(usesAddress);
        if (usesAddress)
        {
            incomingBs.read(targetAddress);
            targetGuid=mPrc2->getGUID(targetAddress);
        }
        else
        {
            incomingBs.read(targetGuid);
            targetAddress=mPrc2->getAddress(targetGuid);
        }
        ForwardingRequest *fw = N_new ForwardingRequest();
        fw->timeoutAfterSuccess=0;
        incomingBs.read(fw->timeoutOnNoDataMS);
        bool hasServerSelectionBitstream=false;
        incomingBs.read(hasServerSelectionBitstream);
        if (hasServerSelectionBitstream)
            incomingBs.read(&(fw->serverSelectionBitstream));

        NetSerializer out;
        SenderAndTargetAddress sata;
        sata.senderClientAddress=sourceAddress;
        sata.targetClientAddress=targetAddress;
        sata.targetClientGuid=targetGuid;
        sata.senderClientGuid=mPrc2->getGUID(sourceAddress);
        SenderAndTargetAddress sataReversed;
        sataReversed.senderClientAddress=targetAddress;
        sataReversed.targetClientAddress=sourceAddress;
        sataReversed.senderClientGuid=sata.targetClientGuid;
        sataReversed.targetClientGuid=sata.senderClientGuid;

        Nui32 insertionIndex;
        bool objectExists1, objectExists2;
        insertionIndex=forwardingRequestList.GetIndexFromKey(sata, &objectExists1);
        forwardingRequestList.GetIndexFromKey(sataReversed, &objectExists2);

        if (objectExists1 || objectExists2)
        {
            out.write((Token)ID_UDP_PROXY_GENERAL);
            out.write((Token)ID_UDP_PROXY_IN_PROGRESS);
            out.write(sata.senderClientAddress);
            out.write(targetAddress);
            out.write(targetGuid);
            // Request in progress, not completed
            Nui16 forwardingPort=0;
            RakString serverPublicIp;
            out.write(serverPublicIp);
            out.write(forwardingPort);
            mPrc2->send(&out, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, msg->mAddress, false);
            N_delete fw;
            return;
        }

        if (serverList.Size()==0)
        {
            out.write((Token)ID_UDP_PROXY_GENERAL);
            out.write((Token)ID_UDP_PROXY_NO_SERVERS_ONLINE);
            out.write(sata.senderClientAddress);
            out.write(targetAddress);
            out.write(targetGuid);
            mPrc2->send(&out, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, msg->mAddress, false);
            N_delete fw;
            return;
        }

        if (mPrc2->getState(targetAddress)!=IS_CONNECTED && usesAddress==false)
        {
            out.write((Token)ID_UDP_PROXY_GENERAL);
            out.write((Token)ID_UDP_PROXY_RECIPIENT_GUID_NOT_CONNECTED_TO_COORDINATOR);
            out.write(sata.senderClientAddress);
            out.write(targetAddress);
            out.write(targetGuid);
            mPrc2->send(&out, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, msg->mAddress, false);
            N_delete fw;
            return;
        }

        fw->sata=sata;
        fw->requestingAddress=msg->mAddress;

        if (serverList.Size()>1)
        {
            out.write((Token)ID_UDP_PROXY_GENERAL);
            out.write((Token)ID_UDP_PROXY_PING_SERVERS_FROM_COORDINATOR_TO_CLIENT);
            out.write(sourceAddress);
            out.write(targetAddress);
            out.write(targetGuid);
            Nui16 serverListSize = (Nui16) serverList.Size();
            out.write(serverListSize);
            Nui32 idx;
            for (idx=0; idx < serverList.Size(); idx++)
                out.write(serverList[idx]);
            mPrc2->send(&out, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, sourceAddress, false);
            mPrc2->send(&out, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, targetAddress, false);
            fw->timeRequestedPings=N_Engine().getTimer().getMS();
            Nui32 copyIndex;
            for (copyIndex=0; copyIndex < serverList.Size(); copyIndex++)
                fw->remainingServersToTry.Push(serverList[copyIndex]);
            forwardingRequestList.InsertAtIndex(fw, insertionIndex);
        }
        else
        {
            fw->timeRequestedPings=0;
            fw->currentlyAttemptedServerAddress=serverList[0];
            forwardingRequestList.InsertAtIndex(fw, insertionIndex);
            SendForwardingRequest(sourceAddress, targetAddress, fw->currentlyAttemptedServerAddress, fw->timeoutOnNoDataMS);
        }
    }
    //------------------------------------------------------------------------
    void UDPProxyCoordinator::SendForwardingRequest(Address * sourceAddress,
        Address * targetAddress, Address * serverAddress, TimeDurMS timeoutOnNoDataMS)
    {
        NetSerializer out;
        // send request to desired server
        out.write((Token)ID_UDP_PROXY_GENERAL);
        out.write((Token)ID_UDP_PROXY_FORWARDING_REQUEST_FROM_COORDINATOR_TO_SERVER);
        out.write(sourceAddress);
        out.write(targetAddress);
        out.write(timeoutOnNoDataMS);
        mPrc2->send(&out, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, serverAddress, false);
    }
    //------------------------------------------------------------------------
    void UDPProxyCoordinator::OnLoginRequestFromServerToCoordinator(SocketMessage * msg)
    {
        NetSerializer incomingBs(msg->mData, msg->mLength, false);
        incomingBs.skipRead(2, 0);
        RakString password;
        incomingBs.read(password);
        NetSerializer out;

        if (remoteLoginPassword.IsEmpty())
        {
            out.write((Token)ID_UDP_PROXY_GENERAL);
            out.write((Token)ID_UDP_PROXY_NO_PASSWORD_SET_FROM_COORDINATOR_TO_SERVER);
            out.write(password);
            mPrc2->send(&out, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, msg->mAddress, false);
            return;
        }

        if (remoteLoginPassword!=password)
        {
            out.write((Token)ID_UDP_PROXY_GENERAL);
            out.write((Token)ID_UDP_PROXY_WRONG_PASSWORD_FROM_COORDINATOR_TO_SERVER);
            out.write(password);
            mPrc2->send(&out, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, msg->mAddress, false);
            return;
        }

        Nui32 insertionIndex;
        insertionIndex=serverList.GetIndexOf(msg->mAddress);
        if (insertionIndex!=(Nui32)-1)
        {
            out.write((Token)ID_UDP_PROXY_GENERAL);
            out.write((Token)ID_UDP_PROXY_ALREADY_LOGGED_IN_FROM_COORDINATOR_TO_SERVER);
            out.write(password);
            mPrc2->send(&out, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, msg->mAddress, false);
            return;
        }
        serverList.Push(msg->mAddress);
        out.write((Token)ID_UDP_PROXY_GENERAL);
        out.write((Token)ID_UDP_PROXY_LOGIN_SUCCESS_FROM_COORDINATOR_TO_SERVER);
        out.write(password);
        mPrc2->send(&out, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, msg->mAddress, false);
    }
    //------------------------------------------------------------------------
    void UDPProxyCoordinator::OnForwardingReplyFromServerToCoordinator(SocketMessage * msg)
    {
        NetSerializer incomingBs(msg->mData, msg->mLength, false);
        incomingBs.skipRead(2, 0);
        SenderAndTargetAddress sata;
        incomingBs.read(sata.senderClientAddress);
        incomingBs.read(sata.targetClientAddress);
        bool objectExists;
        Nui32 index = forwardingRequestList.GetIndexFromKey(sata, &objectExists);
        if (objectExists==false)
        {
            // The guy disconnected before the request finished
            return;
        }
        ForwardingRequest *fw = forwardingRequestList[index];
        sata.senderClientGuid = fw->sata.senderClientGuid;
        sata.targetClientGuid = fw->sata.targetClientGuid;

        RakString serverPublicIp;
        incomingBs.read(serverPublicIp);

        if (serverPublicIp.IsEmpty())
        {
            msg->mAddress.read(serverPublicIp, false);
        }

        UDPForwarderResult success;
        Nui8 c;
        incomingBs.read(c);
        success=(UDPForwarderResult)c;

        Nui16 forwardingPort;
        incomingBs.read(forwardingPort);

        NetSerializer out;
        if (success==UDPFORWARDER_SUCCESS)
        {
            out.write((Token)ID_UDP_PROXY_GENERAL);
            out.write((Token)ID_UDP_PROXY_FORWARDING_SUCCEEDED);
            out.write(sata.senderClientAddress);
            out.write(sata.targetClientAddress);
            out.write(sata.targetClientGuid);
            out.write(serverPublicIp);
            out.write(forwardingPort);
            mPrc2->send(&out, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, fw->requestingAddress, false);

            out.seekRead(0);
            out.seekWrite(0);
            out.write((Token)ID_UDP_PROXY_GENERAL);
            out.write((Token)ID_UDP_PROXY_FORWARDING_NOTIFICATION);
            out.write(sata.senderClientAddress);
            out.write(sata.targetClientAddress);
            out.write(sata.targetClientGuid);
            out.write(serverPublicIp);
            out.write(forwardingPort);
            mPrc2->send(&out, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, sata.targetClientAddress, false);

            // 05/18/09 Keep the entry around for some time after success, so duplicates are reported if attempting forwarding from the target system before notification of success
            fw->timeoutAfterSuccess=N_Engine().getTimer().getMS()+fw->timeoutOnNoDataMS;
            // forwardingRequestList.RemoveAtIndex(index);
            // N_delete fw;

            return;
        }
        else if (success==UDPFORWARDER_NO_SOCKETS)
        {
            // Try next server
            TryNextServer(sata, fw);
        }
        else
        {
            N_assert(success==UDPFORWARDER_FORWARDING_ALREADY_EXISTS);

            // Return in progress
            out.write((Token)ID_UDP_PROXY_GENERAL);
            out.write((Token)ID_UDP_PROXY_IN_PROGRESS);
            out.write(sata.senderClientAddress);
            out.write(sata.targetClientAddress);
            out.write(sata.targetClientGuid);
            out.write(serverPublicIp);
            out.write(forwardingPort);
            mPrc2->send(&out, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, fw->requestingAddress, false);
            forwardingRequestList.RemoveAtIndex(index);
            N_delete fw;
        }
    }
    //------------------------------------------------------------------------
    void UDPProxyCoordinator::OnPingServersReplyFromClientToCoordinator(SocketMessage * msg)
    {
        NetSerializer incomingBs(msg->mData, msg->mLength, false);
        incomingBs.skipRead(2, 0);
        Nui16 serversToPingSize;
        Address * serverAddress;
        SenderAndTargetAddress sata;
        incomingBs.read(sata.senderClientAddress);
        incomingBs.read(sata.targetClientAddress);
        bool objectExists;
        Nui32 index = forwardingRequestList.GetIndexFromKey(sata, &objectExists);
        if (objectExists==false)
            return;
        NCount idx;
        ServerWithPing swp;
        ForwardingRequest *fw = forwardingRequestList[index];
        if (fw->timeRequestedPings==0)
            return;

        incomingBs.read(serversToPingSize);
        if (msg->mAddress==sata.senderClientAddress)
        {
            for (idx=0; idx < serversToPingSize; ++idx)
            {
                incomingBs.read(swp.serverAddress);
                incomingBs.read(swp.ping);
                Nui32 index2;
                for (index2=0; index2 < fw->sourceServerPings.Size(); index2++)
                {
                    if (fw->sourceServerPings[index2].ping >= swp.ping )
                        break;
                }
                fw->sourceServerPings.Insert(swp, index2);
            }
        }
        else
        {
            for (idx=0; idx < serversToPingSize; ++idx)
            {
                incomingBs.read(swp.serverAddress);
                incomingBs.read(swp.ping);

                Nui32 index2;
                for (index2=0; index2 < fw->targetServerPings.Size(); index2++)
                {
                    if (fw->targetServerPings[index2].ping >= swp.ping )
                        break;
                }
                fw->sourceServerPings.Insert(swp, index2);
            }
        }

        // Both systems have to give us pings to progress here. Otherwise will timeout in update()
        if (fw->sourceServerPings.Size()>0 &&
            fw->targetServerPings.Size()>0)
        {
            fw->OrderRemainingServersToTry();
            fw->timeRequestedPings=0;
            TryNextServer(fw->sata, fw);
        }
    }
    //------------------------------------------------------------------------
    void UDPProxyCoordinator::TryNextServer(SenderAndTargetAddress sata,
        ForwardingRequest * fw)
    {
        bool pickedGoodServer=false;
        while(fw->remainingServersToTry.Size()>0)
        {
            fw->currentlyAttemptedServerAddress=fw->remainingServersToTry.Pop();
            if (serverList.GetIndexOf(fw->currentlyAttemptedServerAddress)!=(Nui32)-1)
            {
                pickedGoodServer=true;
                break;
            }
        }

        if (pickedGoodServer==false)
        {
            SendAllBusy(sata.senderClientAddress, sata.targetClientAddress, sata.targetClientGuid, fw->requestingAddress);
            forwardingRequestList.Remove(sata);
            N_delete fw;
            return;
        }

        SendForwardingRequest(sata.senderClientAddress, sata.targetClientAddress, fw->currentlyAttemptedServerAddress, fw->timeoutOnNoDataMS);
    }
    //------------------------------------------------------------------------
    void UDPProxyCoordinator::SendAllBusy(Address /*8*/ senderClientAddress,
        Address * targetClientAddress, UniqueID targetClientGuid, Address * requestingAddress)
    {
        NetSerializer out;
        out.write((Token)ID_UDP_PROXY_GENERAL);
        out.write((Token)ID_UDP_PROXY_ALL_SERVERS_BUSY);
        out.write(senderClientAddress);
        out.write(targetClientAddress);
        out.write(targetClientGuid);
        mPrc2->send(&out, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, requestingAddress, false);
    }
    //------------------------------------------------------------------------
    void UDPProxyCoordinator::Clear(void)
    {
        serverList.Clear(true);
        for (Nui32 i=0; i < forwardingRequestList.Size(); i++)
        {
            N_delete forwardingRequestList[i];
        }
        forwardingRequestList.Clear(false);
    }
    //------------------------------------------------------------------------
    void UDPProxyCoordinator::ForwardingRequest::OrderRemainingServersToTry(void)
    {
        //DataStructures::Multilist<ML_ORDERED_LIST,UDPProxyCoordinator::ServerWithPing,Nui16> swpList;
        DataStructures::OrderedList<Nui16, UDPProxyCoordinator::ServerWithPing, ServerWithPingComp> swpList;
        // swpList.SetSortOrder(true);

        if (sourceServerPings.Size()==0 && targetServerPings.Size()==0)
            return;

        NCount idx;
        UDPProxyCoordinator::ServerWithPing swp;
        for (idx = 0; idx < remainingServersToTry.Size(); ++idx)
        {
            swp.serverAddress=remainingServersToTry[idx];
            swp.ping=0;
            if (sourceServerPings.Size())
                swp.ping += (Nui16)(sourceServerPings[idx].ping);
            else
                swp.ping += (Nui16)(DEFAULT_CLIENT_UNRESPONSIVE_PING_TIME);
            if (targetServerPings.Size())
                swp.ping += (Nui16)(targetServerPings[idx].ping);
            else
                swp.ping += (Nui16)(DEFAULT_CLIENT_UNRESPONSIVE_PING_TIME);
            swpList.Insert(swp.ping, swp, false);
        }
        remainingServersToTry.Clear();
        for (idx=0; idx < swpList.Size(); ++idx)
        {
            remainingServersToTry.Push(swpList[idx].serverAddress);
        }
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*