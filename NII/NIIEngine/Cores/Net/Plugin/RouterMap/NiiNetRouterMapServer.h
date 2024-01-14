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
#if NII_RouterMapServer == 1

#ifndef __NAT_PUNCHTHROUGH_SERVER_H
#define __NAT_PUNCHTHROUGH_SERVER_H

#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"
#include "NiiNetPlugin.h"
#include "PacketLevel.h"
#include "DS_OrderedList.h"
#include "RakString.h"

namespace NII
{
namespace NII_NET
{
    class LinkBase;
    struct SocketMessage;
    #if _RAKNET_SUPPORT_PacketLogger==1
    class PacketLogger;
    #endif

    /// \defgroup NAT_PUNCHTHROUGH_GROUP NatPunchthrough
    /// \brief connect systems despite both systems being behind a router
    /// \details
    /// \ingroup PLUGINS_GROUP

    /// \ingroup NAT_PUNCHTHROUGH_GROUP
    struct _EngineAPI NatPunchthroughServerDebugInterface
    {
        NatPunchthroughServerDebugInterface() {}
        virtual ~NatPunchthroughServerDebugInterface() {}
        virtual void OnServerMessage(const char * msg) = 0;
    };

    /// \ingroup NAT_PUNCHTHROUGH_GROUP
    struct _EngineAPI NatPunchthroughServerDebugInterface_Printf : public NatPunchthroughServerDebugInterface
    {
        virtual void OnServerMessage(const char * msg);
    };

    #if _RAKNET_SUPPORT_PacketLogger == 1
    /// \ingroup NAT_PUNCHTHROUGH_GROUP
    struct _EngineAPI NatPunchthroughServerDebugInterface_PacketLogger : public NatPunchthroughServerDebugInterface
    {
        // Set to non-zero to write to the packetlogger!
        PacketLogger * pl;

        NatPunchthroughServerDebugInterface_PacketLogger() {pl=0;}
        ~NatPunchthroughServerDebugInterface_PacketLogger() {}
        virtual void OnServerMessage(const char * msg);
    };
    #endif

    /// \brief Server code for NATPunchthrough
    /// \details Maintain connection to RouterMapServer to process incoming connection attempts through RouterMapClient<BR>
    /// Server maintains two sockets clients can connect to so as to estimate the next port choice<BR>
    /// Server tells other client about port estimate, current public port to the server, and a time to start connection attempts
    /// \sa NatTypeDetectionClient
    /// See also http://www.jenkinssoftware.com/raknet/manual/natpunchthrough.html
    /// \ingroup NAT_PUNCHTHROUGH_GROUP
    class _EngineAPI RouterMapServer : public Plugin
    {
    public:
        // Constructor
        RouterMapServer();

        // Destructor
        virtual ~RouterMapServer();

        /// Sets a callback to be called with debug messages
        /// \param[in] i Pointer to an interface. The pointer is stored, so don't delete it while in progress. Pass 0 to clear.
        void SetDebugInterface(NatPunchthroughServerDebugInterface *i);

        /// \internal For plugin handling
        virtual void update();

        /// \internal For plugin handling
        virtual ConnectReceiveType onMessage(SocketMessage * msg);

        /// \internal For plugin handling
        virtual void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason );
        virtual void onConnect(const Address & address, UniqueID id);

        // Each connected user has a ready state. Ready means ready for nat punchthrough.
        struct User;
        struct ConnectionAttempt
        {
            ConnectionAttempt() {sender=0; recipient=0; startTime=0; attemptPhase=NAT_ATTEMPT_PHASE_NOT_STARTED;}
            User *sender, *recipient;
            Nui16 sessionId;
            TimeDurMS startTime;
            enum
            {
                NAT_ATTEMPT_PHASE_NOT_STARTED,
                NAT_ATTEMPT_PHASE_GETTING_RECENT_PORTS,
            } attemptPhase;
        };
        struct User
        {
            UniqueID mID;
            Address * mAddress;
            Nui16 mostRecentPort;
            bool isReady;
            DataStructures::OrderedList<UniqueID,UniqueID> groupPunchthroughRequests;

            DataStructures::List<ConnectionAttempt *> connectionAttempts;
            bool HasConnectionAttemptToUser(User * user);
            void DerefConnectionAttempt(ConnectionAttempt * ca);
            void DeleteConnectionAttempt(ConnectionAttempt * ca);
            void LogConnectionAttempts(RakString & rs);
        };
        TimeDurMS lastUpdate;
        static int NatPunchthroughUserComp(const UniqueID & key, User * const & data);
    protected:
        void OnNATPunchthroughRequest(SocketMessage * msg);
        DataStructures::OrderedList<UniqueID, User *, RouterMapServer::NatPunchthroughUserComp> users;

        void OnGetMostRecentPort(SocketMessage * msg);
        void OnClientReady(SocketMessage * msg);

        void SendTimestamps(void);
        void StartPendingPunchthrough(void);
        void StartPunchthroughForUser(User * user);
        Nui16 sessionId;
        NatPunchthroughServerDebugInterface * natPunchthroughServerDebugInterface;

        Address * boundAddresses[N_Socket_MaxLocalIP];
        Nui8 boundAddressCount;
    };
}
}
#endif
#endif