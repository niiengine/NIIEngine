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

#ifndef __ROUTER_2_PLUGIN_H
#define __ROUTER_2_PLUGIN_H

#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"
#include "NiiNetPlugin.h"
#include "PacketLevel.h"
#include "UDPForwarder.h"
#include "NiiNetCommon.h"
#include "DS_List.h"
#include "NiiThreadManager.h"

namespace NII
{
namespace NII_NET
{
    struct Router2DebugInterface
    {
        Router2DebugInterface() {}
        virtual ~Router2DebugInterface() {}
        virtual void ShowFailure(const char * message);
        virtual void ShowDiagnostic(const char * message);
    };

    /// \brief Part of the NAT punchthrough solution, allowing you to connect to systems by routing through a shared connection.
    /// \details Router2 routes datagrams between two systems that are not directly connected by using the bandwidth of a third system, to which the other two systems were connected
    /// It is of benefit when a fully connected mesh topology is desired, but could not be completely established due to routers and/or firewalls
    /// As the system address of a remote system will be the system address of the intermediary, it is necessary to use the UniqueID object to refer to systems, including with other plugins
    /// \ingroup PLUGINS_GROUP

    /// \ingroup ROUTER_2_GROUP
    /// \brief Class interface for the Router2 system
    /// \details
    class _EngineAPI Router2 : public Plugin
    {
    public:
        Router2();
        virtual ~Router2();

        /// Sets the socket family to use, either IPV4 or IPV6
        /// \param[in] socketFamily For IPV4, use AF_INET (default). For IPV6, use AF_INET6. To autoselect, use AF_UNSPEC.
        void SetSocketFamily(Nui16 _socketFamily);

        /// \brief Query all connected systems to connect through them to a third system.
        /// System will return ID_ROUTER_2_FORWARDING_NO_PATH if unable to connect.
        /// Else you will get ID_ROUTER_2_FORWARDING_ESTABLISHED
        ///
        /// On ID_ROUTER_2_FORWARDING_ESTABLISHED, EstablishRouting as follows:
        ///
        /// NetSerializer bs(msg->mData, msg->mLength, false);
        /// bs.skipRead(1, 0);
        /// UniqueID endpointGuid;
        /// bs.read(endpointGuid);
        /// Nui16 sourceToDestPort;
        /// bs.read(sourceToDestPort);
        /// String ipAddressString;
        /// msg->mAddress.read(ipAddressString, false);
        /// linkbase->EstablishRouting(ipAddressString, sourceToDestPort, 0,0);
        ///
        /// \note The Address for a connection should not be used - always use RakNetGuid as the address can change at any time.
        /// When the address changes, you will get ID_ROUTER_2_REROUTED
        void EstablishRouting(UniqueID endpointGuid);

        /// Set the maximum number of bidirectional connections this system will support
        /// Defaults to 0
        void SetMaximumForwardingRequests(int max);

        /// For testing and debugging
        void SetDebugInterface(Router2DebugInterface *_debugInterface);

        /// Get the pointer passed to SetDebugInterface()
        Router2DebugInterface *GetDebugInterface(void) const;

        // --------------------------------------------------
        // SocketMessage handling functions
        // --------------------------------------------------
        virtual ConnectReceiveType onMessage(SocketMessage * msg);
        virtual void update();
        virtual void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason);

        virtual void onConnectFail(SocketMessage * msg,
            ConnectFailType failedConnectionAttemptReason);

        virtual void shutdown();

        enum Router2RequestStates
        {
            R2RS_REQUEST_STATE_QUERY_FORWARDING,
            REQUEST_STATE_REQUEST_FORWARDING,
        };

        struct ConnectionRequestSystem
        {
            UniqueID mID;
            int pingToEndpoint;
            Nui16 usedForwardingEntries;
        };

        struct ConnnectRequest
        {
            ConnnectRequest();
            ~ConnnectRequest();

            DataStructures::List<ConnectionRequestSystem> connectionRequestSystems;
            ThreadMutex connectionRequestSystemsMutex;
            Router2RequestStates requestState;
            TimeDurMS pingTimeout;
            UniqueID endpointGuid;
            UniqueID lastRequestedForwardingSystem;
            bool returnConnectionLostOnFailure;
            Nui32 GetGuidIndex(UniqueID id);
        };

        Nui32 GetConnectionRequestIndex(UniqueID endpointGuid);

        struct MiniPunchRequest
        {
            UniqueID endpointGuid;
            Address * endpointAddress;
            bool gotReplyFromEndpoint;
            UniqueID sourceGuid;
            Address * sourceAddress;
            bool gotReplyFromSource;
            TimeDurMS timeout;
            TimeDurMS nextAction;
            Nui16 forwardingPort;
            Nudpsocket forwardingSocket;
        };

        struct ForwardedConnection
        {
            UniqueID endpointGuid;
            UniqueID intermediaryGuid;
            Address * intermediaryAddress;
            bool returnConnectionLostOnFailure;
            bool weInitiatedForwarding;
        };
    protected:
        bool UpdateForwarding(ConnnectRequest * connectionRequest);
        void RemoveConnectionRequest(Nui32 connectionRequestIndex);
        void RequestForwarding(ConnnectRequest * connectionRequest);
        void OnQueryForwarding(SocketMessage * msg);
        void OnQueryForwardingReply(SocketMessage * msg);
        void OnRequestForwarding(SocketMessage * msg);
        void OnRerouted(SocketMessage * msg);
        void OnMiniPunchReply(SocketMessage * msg);
        void OnMiniPunchReplyBounce(SocketMessage * msg);
        bool OnForwardingSuccess(SocketMessage * msg);
        int GetLargestPingAmongConnectedSystems() const;
        void ReturnToUser(Token messageId, UniqueID endpointGuid, const Address & address,
            bool local);
        bool ConnectInternal(UniqueID endpointGuid, bool returnConnectionLostOnFailure);

        UDPForwarder * udpForwarder;
        int maximumForwardingRequests;
        ThreadMutex connectionRequestsMutex, miniPunchesInProgressMutex, forwardedConnectionListMutex;
        DataStructures::List<ConnnectRequest *> connectionRequests;
        DataStructures::List<MiniPunchRequest> miniPunchesInProgress;
        // Forwarding we have initiated
        DataStructures::List<ForwardedConnection> forwardedConnectionList;

        void ClearConnectionRequests(void);
        void ClearMinipunches(void);
        void ClearForwardedConnections(void);
        void ClearAll(void);
        int ReturnFailureOnCannotForward(UniqueID sourceGuid, UniqueID endpointGuid);
        void SendFailureOnCannotForward(UniqueID sourceGuid, UniqueID endpointGuid);
        void SendForwardingSuccess(Token messageId, UniqueID sourceGuid, UniqueID endpointGuid, Nui16 sourceToDstPort);
        void SendOOBFromRakNetPort(OutOfBandIdentifiers oob, NetSerializer * extraData, Address * sa);
        void SendOOBFromSpecifiedSocket(OutOfBandIdentifiers oob, Address * sa, Nudpsocket socket);
        void SendOOBMessages(MiniPunchRequest * mpr);

        Router2DebugInterface * debugInterface;
        Nui16 socketFamily;
    };
}
}
#endif
#endif