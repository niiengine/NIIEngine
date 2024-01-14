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

#ifndef __UDP_PROXY_SERVER_H
#define __UDP_PROXY_SERVER_H

#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"
#include "NiiNetPlugin.h"
#include "UDPForwarder.h"
#include "RakString.h"

namespace NII
{
namespace NII_NET
{
    class UDPProxyServer;

    /// Callback to handle results of calling UDPProxyServer::LoginToCoordinator()
    /// \ingroup UDP_PROXY_GROUP
    struct UDPProxyServerResultHandler
    {
        UDPProxyServerResultHandler() {}
        virtual ~UDPProxyServerResultHandler() {}

        /// Called when our login succeeds
        /// \param[out] pw The password we passed to UDPProxyServer::LoginToCoordinator()
        /// \param[out] proxyServer The plugin calling this callback
        virtual void OnLoginSuccess(RakString pw, UDPProxyServer * proxyServerPlugin)=0;

        /// We are already logged in.
        /// This login failed, but the system is operational as if it succeeded
        /// \param[out] pw The password we passed to UDPProxyServer::LoginToCoordinator()
        /// \param[out] proxyServer The plugin calling this callback
        virtual void OnAlreadyLoggedIn(RakString pw, UDPProxyServer * proxyServerPlugin)=0;

        /// The coordinator operator forgot to call UDPProxyCoordinator::SetRemoteLoginPassword()
        /// \param[out] pw The password we passed to UDPProxyServer::LoginToCoordinator()
        /// \param[out] proxyServer The plugin calling this callback
        virtual void OnNoPasswordSet(RakString pw, UDPProxyServer * proxyServerPlugin)=0;

        /// The coordinator operator set a different password in UDPProxyCoordinator::SetRemoteLoginPassword() than what we passed
        /// \param[out] pw The password we passed to UDPProxyServer::LoginToCoordinator()
        /// \param[out] proxyServer The plugin calling this callback
        virtual void OnWrongPassword(RakString pw, UDPProxyServer * proxyServerPlugin)=0;
    };

    /// \brief UDPProxyServer to control our instance of UDPForwarder
    /// \details When NAT Punchthrough fails, it is possible to use a non-NAT system to forward messages from us to the recipient, and vice-versa.<BR>
    /// The class to forward messages is UDPForwarder, and it is triggered over the network via the UDPProxyServer plugin.<BR>
    /// The UDPProxyServer connects to UDPProxyServer to get a list of servers running UDPProxyServer, and the coordinator will relay our forwarding request.
    /// \ingroup UDP_PROXY_GROUP
    class _EngineAPI UDPProxyServer : public Plugin
    {
    public:
        UDPProxyServer();
        ~UDPProxyServer();

        /// Sets the socket family to use, either IPV4 or IPV6
        /// \param[in] socketFamily For IPV4, use AF_INET (default). For IPV6, use AF_INET6. To autoselect, use AF_UNSPEC.
        void SetSocketFamily(Nui16 _socketFamily);

        /// Receives the results of calling LoginToCoordinator()
        /// Set before calling LoginToCoordinator or you won't know what happened
        /// \param[in] resultHandler
        void SetResultHandler(UDPProxyServerResultHandler * rh);

        /// Before the coordinator will register the UDPProxyServer, you must login
        /// \pre Must be connected to the coordinator
        /// \pre Coordinator must have set a password with UDPProxyCoordinator::SetRemoteLoginPassword()
        /// \returns false if already logged in, or logging in. Returns true otherwise
        bool LoginToCoordinator(RakString password, Address * coordinatorAddress);

        /// \brief The server IP reported to the client is the IP address from the server to the coordinator.
        /// If the server and coordinator are on the same LAN, you need to call SetServerPublicIP() to tell the client what address to connect to
        /// \param[in] ip IP address to report in UDPProxyClientResultHandler::OnForwardingSuccess() and UDPProxyClientResultHandler::OnForwardingNotification() as proxyIPAddress
        void SetServerPublicIP(RakString ip);

        /// Operative class that performs the forwarding
        /// Exposed so you can call UDPForwarder::SetMaxForwardEntries() if you want to change away from the default
        /// UDPForwarder::Startup(), UDPForwarder::Shutdown(), and UDPForwarder::update() are called automatically by the plugin
        UDPForwarder udpForwarder;

        virtual void onAttach();
        virtual void onDetach();

        /// \internal
        virtual void update();
        virtual ConnectReceiveType onMessage(SocketMessage * msg);
        virtual void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason);
        virtual void init();
        virtual void shutdown();
    protected:
        void OnForwardingRequestFromCoordinatorToServer(SocketMessage * msg);

        DataStructures::OrderedList<Address *, Address *> loggingInCoordinators;
        DataStructures::OrderedList<Address *, Address *> loggedInCoordinators;

        UDPProxyServerResultHandler * resultHandler;
        Nui16 socketFamily;
        RakString serverPublicIp;
    };
}
}
#endif
#endif