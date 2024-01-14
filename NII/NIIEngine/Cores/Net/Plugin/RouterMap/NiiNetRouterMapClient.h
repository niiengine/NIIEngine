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
#if NII_RouterMapClient == 1

#ifndef _NII_NET_NAT_PUNCHTHROUGH_CLIENT_H_
#define _NII_NET_NAT_PUNCHTHROUGH_CLIENT_H_

#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"
#include "NiiNetPlugin.h"
#include "PacketLevel.h"
#include "DS_List.h"
#include "RakString.h"
#include "DS_Queue.h"

// Trendnet TEW-632BRP sometimes starts at port 1024 and increments sequentially.
// Zonnet zsr1134we. Replies go out on the net, but are always absorbed by the remote router??
// Dlink ebr2310 to Trendnet ok
// Trendnet TEW-652BRP to Trendnet 632BRP OK
// Trendnet TEW-632BRP to Trendnet 632BRP OK
// Buffalo WHR-HP-G54 OK
// Netgear WGR614 ok

namespace NII
{
namespace NII_NET
{
    class LinkBase;
    struct SocketMessage;
#if _RAKNET_SUPPORT_PacketLogger == 1
    class PacketLogger;
#endif

    /// \ingroup NAT_PUNCHTHROUGH_GROUP
    struct _EngineAPI PunchthroughConfiguration
    {
        /// internal: (15 ms * 2 tries + 30 wait) * 5 ports * 8 players = 2.4 seconds
        /// external: (50 ms * 8 sends + 200 wait) * 2 port * 8 players = 9.6 seconds
        /// Total: 8 seconds
        PunchthroughConfiguration()
        {
            TIME_BETWEEN_PUNCH_ATTEMPTS_INTERNAL = 15;
            TIME_BETWEEN_PUNCH_ATTEMPTS_EXTERNAL = 50;
            UDP_SENDS_PER_PORT_INTERNAL = 2;
            UDP_SENDS_PER_PORT_EXTERNAL = 8;
            INTERNAL_IP_WAIT_AFTER_ATTEMPTS = 30;
            MAXIMUM_NUMBER_OF_INTERNAL_IDS_TO_CHECK = 5; /// set to 0 to not do lan connects
            MAX_PREDICTIVE_PORT_RANGE = 2;
            EXTERNAL_IP_WAIT_BETWEEN_PORTS = 200;
            EXTERNAL_IP_WAIT_AFTER_FIRST_TTL = 100;
            EXTERNAL_IP_WAIT_AFTER_ALL_ATTEMPTS = EXTERNAL_IP_WAIT_BETWEEN_PORTS;
            retryOnFailure = false;
        }

        /// How much time between each UDP send
        TimeDurMS TIME_BETWEEN_PUNCH_ATTEMPTS_INTERNAL;
        TimeDurMS TIME_BETWEEN_PUNCH_ATTEMPTS_EXTERNAL;

        /// How many tries for one port before giving up and going to the next port
        int UDP_SENDS_PER_PORT_INTERNAL;
        int UDP_SENDS_PER_PORT_EXTERNAL;

        /// After giving up on one internal port, how long to wait before trying the next port
        int INTERNAL_IP_WAIT_AFTER_ATTEMPTS;

        /// How many external ports to try past the last known starting port
        int MAX_PREDICTIVE_PORT_RANGE;

        /// After sending TTL, how long to wait until first punch attempt
        int EXTERNAL_IP_WAIT_AFTER_FIRST_TTL;

        /// After giving up on one external  port, how long to wait before trying the next port
        int EXTERNAL_IP_WAIT_BETWEEN_PORTS;

        /// After trying all external ports, how long to wait before returning ID_NAT_PUNCHTHROUGH_FAILED
        int EXTERNAL_IP_WAIT_AFTER_ALL_ATTEMPTS;

        /// Maximum number of internal IP address to try to connect to.
        /// Cannot be greater than N_Socket_MaxLocalIP
        /// Should be high enough to try all internal IP addresses on the majority of computers
        int MAXIMUM_NUMBER_OF_INTERNAL_IDS_TO_CHECK;

        /// If the first punchthrough attempt fails, try again
        /// This sometimes works because the remote router was looking for an incoming message on a higher numbered port before responding to a lower numbered port from the other system
        bool retryOnFailure;
    };

    /// \ingroup NAT_PUNCHTHROUGH_GROUP
    struct _EngineAPI NatPunchthroughDebugInterface
    {
        NatPunchthroughDebugInterface() {}
        virtual ~NatPunchthroughDebugInterface() {}
        virtual void OnClientMessage(const char * msg) = 0;
    };

    /// \ingroup NAT_PUNCHTHROUGH_GROUP
    struct _EngineAPI NatPunchthroughDebugInterface_Printf : public NatPunchthroughDebugInterface
    {
        virtual void OnClientMessage(const char * msg);
    };

#if _RAKNET_SUPPORT_PacketLogger == 1
    /// \ingroup NAT_PUNCHTHROUGH_GROUP
    struct _EngineAPI NatPunchthroughDebugInterface_PacketLogger : public NatPunchthroughDebugInterface
    {
        // Set to non-zero to write to the packetlogger!
        PacketLogger * pl;

        NatPunchthroughDebugInterface_PacketLogger() {pl=0;}
        ~NatPunchthroughDebugInterface_PacketLogger() {}
        virtual void OnClientMessage(const char * msg);
    };
#endif

    /// \details Maintain connection to RouterMapServer to process incoming connection attempts through RouterMapClient<BR>
    /// Client will send datagrams to port to estimate next port<BR>
    /// Will simultaneously connect with another client once ports are estimated.
    /// \sa NatTypeDetectionClient
    /// See also http://www.jenkinssoftware.com/raknet/manual/natpunchthrough.html
    /// \ingroup NAT_PUNCHTHROUGH_GROUP
    class _EngineAPI RouterMapClient : public Plugin
    {
    public:
        RouterMapClient();
        ~RouterMapClient();

        /// If the instance of NetworkSys running NATPunchthroughServer was bound to two IP addresses, then you can call FindRouterPortStride()
        /// This will determine the stride that your router uses when assigning ports, if your router is full-cone
        /// This function is also called automatically when you call OpenNAT - however, calling it earlier when you are connected to the facilitator will speed up the process
        /// \param[in] destination The system to punch. Must already be connected to \a facilitator
        void FindRouterPortStride(const Address & facilitator);

        /// Punchthrough a NAT. Doesn't connect, just tries to setup the routing table
        /// \param[in] destination The system to punch. Must already be connected to \a facilitator
        /// \param[in] facilitator A system we are already connected to running the RouterMapServer plugin
        /// \sa OpenNATGroup()
        /// You will get ID_NAT_PUNCHTHROUGH_SUCCEEDED on success
        /// You will get ID_NAT_TARGET_NOT_CONNECTED, ID_NAT_TARGET_UNRESPONSIVE, ID_NAT_CONNECTION_TO_TARGET_LOST, ID_NAT_ALREADY_IN_PROGRESS, or ID_NAT_PUNCHTHROUGH_FAILED on failures of various types
        /// However, if you lose connection to the facilitator, you may not necessarily get above
        bool OpenNAT(UniqueID destination, const Address & facilitator);

        /// Modify the system configuration if desired
        /// Don't modify the variables in the structure while punchthrough is in progress
        PunchthroughConfiguration * GetPunchthroughConfiguration(void);

        /// Sets a callback to be called with debug messages
        /// \param[in] i Pointer to an interface. The pointer is stored, so don't delete it while in progress. Pass 0 to clear.
        void SetDebugInterface(NatPunchthroughDebugInterface * i);

        /// Get the port mappings you should pass to UPNP (for miniupnpc-1.6.20120410, for the function UPNP_AddPortMapping)
        void GetUPNPPortMappings(char * externalPort, char * internalPort,
            const Address & natPunchthroughServerAddress);

        /// \internal For plugin handling
        virtual void update();

        /// \internal For plugin handling
        virtual ConnectReceiveType onMessage(SocketMessage * msg);

        /// \internal For plugin handling
        virtual void onConnect(const Address & address, UniqueID id);

        /// \internal For plugin handling
        virtual void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason );

        virtual void onAttach();
        virtual void onDetach();
        virtual void shutdown();
        void Clear(void);

        struct SendPing
        {
            TimeDurMS mLastActive;
            Address * targetAddress;
            Address * facilitator;
            Address * internalIds[N_Socket_MaxLocalIP];
            UniqueID targetGuid;
            bool weAreSender;
            int attemptCount;
            int retryCount;
            int punchingFixedPortAttempts; // only used for TestMode::PUNCHING_FIXED_PORT
            Nui16 sessionId;
            bool sentTTL;
            // Give level to internal IP addresses because if we are on a LAN, we don't want to try to connect through the internet
            enum TestMode
            {
                TESTING_INTERNAL_IPS,
                WAITING_FOR_INTERNAL_IPS_RESPONSE,
                //SEND_WITH_TTL,
                TESTING_EXTERNAL_IPS_FACILITATOR_PORT_TO_FACILITATOR_PORT,
                TESTING_EXTERNAL_IPS_1024_TO_FACILITATOR_PORT,
                TESTING_EXTERNAL_IPS_FACILITATOR_PORT_TO_1024,
                TESTING_EXTERNAL_IPS_1024_TO_1024,
                WAITING_AFTER_ALL_ATTEMPTS,

                // The trendnet remaps the remote port to 1024.
                // If you continue punching on a different port for the same IP it bans you and the communication becomes unidirectioal
                PUNCHING_FIXED_PORT,

                // try port 1024-1028
            } testMode;
        } sp;
    protected:
        Nui16 mostRecentExternalPort;
        void OnFailureNotification(SocketMessage * msg);
        void OnGetMostRecentPort(SocketMessage * msg);
        void OnConnectAtTime(SocketMessage *msg);
        Nui32 GetPendingOpenNATIndex(UniqueID destination, const Address &facilitator);
        void SendPunchthrough(UniqueID destination, const Address &facilitator);
        void QueueOpenNAT(UniqueID destination, const Address &facilitator);
        void SendQueuedOpenNAT(void);
        void sendTTL(const Address & sa);
        void sendExpedite(Address sa, Token oobId);
        void OnPunchthroughFailure(void);
        void OnReadyForNextPunchthrough(void);
        void PushFailure(void);
        bool RemoveFromFailureQueue(void);
        void PushSuccess(void);

        PunchthroughConfiguration pc;
        NatPunchthroughDebugInterface * natPunchthroughDebugInterface;

        // The first time we fail a NAT attempt, we add it to failedAttemptList and try again, since sometimes trying again later fixes the problem
        // The second time we fail, we return ID_NAT_PUNCHTHROUGH_FAILED
        struct AddrAndGuid
        {
            Address * addr;
            UniqueID mID;
        };
        DataStructures::List<AddrAndGuid> failedAttemptList;

        struct DSTAndFac
        {
            UniqueID destination;
            Address * facilitator;
        };
        DataStructures::Queue<DSTAndFac> queuedOpenNat;

        void IncrementExternalAttemptCount(TimeDurMS time, TimeDurMS delta);
        Nui16 portStride;
        enum
        {
            HAS_PORT_STRIDE,
            UNKNOWN_PORT_STRIDE,
            CALCULATING_PORT_STRIDE,
            INCAPABLE_PORT_STRIDE
        } hasPortStride;
        TimeDurMS portStrideCalTimeout;
    };
}
}
#endif
#endif