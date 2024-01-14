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

#if _RAKNET_SUPPORT_UDPProxyServer == 1 && _RAKNET_SUPPORT_UDPForwarder == 1

#include "UDPProxyServer.h"
#include "NiiNetSerializer.h"
#include "UDPProxyCommon.h"
#include "NiiNetLinkBase.h"
#include "NiiNetCommon.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    UDPProxyServer::UDPProxyServer()
    {
        resultHandler=0;
        socketFamily=AF_INET;
    }
    //------------------------------------------------------------------------
    UDPProxyServer::~UDPProxyServer()
    {

    }
    //------------------------------------------------------------------------
    void UDPProxyServer::SetSocketFamily(Nui16 _socketFamily)
    {
        socketFamily = _socketFamily;
    }
    //------------------------------------------------------------------------
    void UDPProxyServer::SetResultHandler(UDPProxyServerResultHandler * rh)
    {
        resultHandler = rh;
    }
    //------------------------------------------------------------------------
    bool UDPProxyServer::LoginToCoordinator(RakString password, Address * coordinatorAddress)
    {
        Nui32 insertionIndex;
        bool objectExists;
        insertionIndex = loggingInCoordinators.GetIndexFromKey(coordinatorAddress, &objectExists);
        if (objectExists == true)
            return false;
        loggedInCoordinators.GetIndexFromKey(coordinatorAddress, &objectExists);
        if (objectExists == true)
            return false;
        NetSerializer outgoingBs;
        outgoingBs.write((Token)ID_UDP_PROXY_GENERAL);
        outgoingBs.write((Token)ID_UDP_PROXY_LOGIN_REQUEST_FROM_SERVER_TO_COORDINATOR);
        outgoingBs.write(password);
        mPrc2->send(&outgoingBs, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, coordinatorAddress, false);
        loggingInCoordinators.InsertAtIndex(coordinatorAddress, insertionIndex);
        return true;
    }
    //------------------------------------------------------------------------
    void UDPProxyServer::SetServerPublicIP(RakString ip)
    {
        serverPublicIp = ip;
    }
    //------------------------------------------------------------------------
    void UDPProxyServer::update()
    {
    }
    //------------------------------------------------------------------------
    ConnectReceiveType UDPProxyServer::onMessage(SocketMessage * msg)
    {
        // Make sure incoming messages from from UDPProxyCoordinator
        if (msg->mData[0] == ID_UDP_PROXY_GENERAL && msg->mLength > 1)
        {
            bool objectExists;

            switch (msg->mData[1])
            {
            case ID_UDP_PROXY_FORWARDING_REQUEST_FROM_COORDINATOR_TO_SERVER:
                if (loggedInCoordinators.GetIndexFromKey(msg->mAddress, &objectExists)!=(Nui32)-1)
                {
                    OnForwardingRequestFromCoordinatorToServer(msg);
                    return CRT_Destroy;
                }
                break;
            case ID_UDP_PROXY_NO_PASSWORD_SET_FROM_COORDINATOR_TO_SERVER:
            case ID_UDP_PROXY_WRONG_PASSWORD_FROM_COORDINATOR_TO_SERVER:
            case ID_UDP_PROXY_ALREADY_LOGGED_IN_FROM_COORDINATOR_TO_SERVER:
            case ID_UDP_PROXY_LOGIN_SUCCESS_FROM_COORDINATOR_TO_SERVER:
                {
                    Nui32 removalIndex = loggingInCoordinators.GetIndexFromKey(msg->mAddress, &objectExists);
                    if (objectExists)
                    {
                        loggingInCoordinators.RemoveAtIndex(removalIndex);

                        NetSerializer incomingBs(msg->mData, msg->mLength, false);
                        incomingBs.skipRead(2, 0);
                        RakString password;
                        incomingBs.read(password);
                        switch (msg->mData[1])
                        {
                        case ID_UDP_PROXY_NO_PASSWORD_SET_FROM_COORDINATOR_TO_SERVER:
                            if (resultHandler)
                                resultHandler->OnNoPasswordSet(password, this);
                            break;
                        case ID_UDP_PROXY_WRONG_PASSWORD_FROM_COORDINATOR_TO_SERVER:
                            if (resultHandler)
                                resultHandler->OnWrongPassword(password, this);
                            break;
                        case ID_UDP_PROXY_ALREADY_LOGGED_IN_FROM_COORDINATOR_TO_SERVER:
                            if (resultHandler)
                                resultHandler->OnAlreadyLoggedIn(password, this);
                            break;
                        case ID_UDP_PROXY_LOGIN_SUCCESS_FROM_COORDINATOR_TO_SERVER:
                            loggedInCoordinators.Insert(msg->mAddress, msg->mAddress, true);
                            if (resultHandler)
                                resultHandler->OnLoginSuccess(password, this);
                            break;
                        }
                    }


                    return CRT_Destroy;
                }
            }
        }
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void UDPProxyServer::onConnectClose(const Address & address,
        UniqueID id, ConnectCloseType lostConnectionReason)
    {
        (void) lostConnectionReason;
        (void) id;

        loggingInCoordinators.RemoveIfExists(address);
        loggedInCoordinators.RemoveIfExists(address);
    }
    //------------------------------------------------------------------------
    void UDPProxyServer::init(void)
    {
        udpForwarder.Startup();
    }
    //------------------------------------------------------------------------
    void UDPProxyServer::shutdown(void)
    {
        udpForwarder.Shutdown();
        loggingInCoordinators.Clear(true);
        loggedInCoordinators.Clear(true);
    }
    //------------------------------------------------------------------------
    void UDPProxyServer::onAttach()
    {
        if (mPrc2->isActive())
            init();
    }
    //------------------------------------------------------------------------
    void UDPProxyServer::onDetach()
    {
        shutdown();
    }
    //------------------------------------------------------------------------
    void UDPProxyServer::OnForwardingRequestFromCoordinatorToServer(SocketMessage *msg)
    {
        Address * sourceAddress, * targetAddress;
        NetSerializer incomingBs(msg->mData, msg->mLength, false);
        incomingBs.skipRead(2, 0);
        incomingBs.read(sourceAddress);
        incomingBs.read(targetAddress);
        TimeDurMS timeoutOnNoDataMS;
        incomingBs.read(timeoutOnNoDataMS);
        N_assert(timeoutOnNoDataMS > 0 && timeoutOnNoDataMS <= UDP_FORWARDER_MAXIMUM_TIMEOUT);

        Nui16 forwardingPort=0;
        UDPForwarderResult success = udpForwarder.StartForwarding(sourceAddress, targetAddress, timeoutOnNoDataMS, 0, socketFamily, &forwardingPort, 0);
        NetSerializer outgoingBs;
        outgoingBs.write((Token)ID_UDP_PROXY_GENERAL);
        outgoingBs.write((Token)ID_UDP_PROXY_FORWARDING_REPLY_FROM_SERVER_TO_COORDINATOR);
        outgoingBs.write(sourceAddress);
        outgoingBs.write(targetAddress);
        outgoingBs.write(serverPublicIp);
        outgoingBs.write((Nui8) success);
        outgoingBs.write(forwardingPort);
        mPrc2->send(&outgoingBs, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, msg->mAddress, false);
    }
    //------------------------------------------------------------------------
}
}
#endif