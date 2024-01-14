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
#if _RAKNET_SUPPORT_NatTypeDetectionServer == 1

#ifndef __NAT_TYPE_DETECTION_SERVER_H
#define __NAT_TYPE_DETECTION_SERVER_H

#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"
#include "NiiNetPlugin.h"
#include "PacketLevel.h"
#include "DS_OrderedList.h"
#include "RakString.h"
#include "NatTypeDetectionCommon.h"

namespace NII
{
namespace NII_NET
{
    /// \brief Server code for NatTypeDetection
    /// \details
    /// Sends to a remote system on certain ports and addresses to determine what type of router, if any, that client is behind
    /// Requires that the server have 4 external IP addresses
    /// <OL>
    /// <LI>Server has 1 instance of RakNet. Server has four external ip addresses S1 to S4. Five ports are used in total P1 to P5. RakNet is bound to S1P1. Sockets are bound to S1P2, S2P3, S3P4, S4P5
    /// <LI>Client with one port using RakNet (C1). Another port not using anything (C2).
    /// <LI>C1 connects to S1P1 for normal communication.
    /// <LI>S4P5 sends to C2. If arrived, no NAT. Done. (If didn't arrive, S4P5 potentially banned, do not use again).
    /// <LI>S2P3 sends to C1 (Different address, different port, to previously used port on client). If received, Full-cone nat. Done.  (If didn't arrive, S2P3 potentially banned, do not use again).
    /// <LI>S1P2 sends to C1 (Same address, different port, to previously used port on client). If received, address-restricted cone nat. Done.
    /// <LI>Server via RakNet connection tells C1 to send to to S3P4. If address of C1 as seen by S3P4 is the same as the address of C1 as seen by S1P1 (RakNet connection), then port-restricted cone nat. Done
    /// <LI>Else symmetric nat. Done.
    /// </OL>
    /// See also http://www.jenkinssoftware.com/raknet/manual/natpunchthrough.html
    /// \sa RouterMapServer
    /// \sa NatTypeDetectionClient
    /// \ingroup NAT_TYPE_DETECTION_GROUP
    class _EngineAPI NatTypeDetectionServer : public Plugin, public RNS2EventHandler
    {
    public:
        // Constructor
        NatTypeDetectionServer();

        // Destructor
        virtual ~NatTypeDetectionServer();

        /// Start the system, binding to 3 external IPs not already in useS
        /// \param[in] nonRakNetIP2 First unused external IP
        /// \param[in] nonRakNetIP3 Second unused external IP
        /// \param[in] nonRakNetIP4 Third unused external IP
        void Startup(const char * nonRakNetIP2, const char * nonRakNetIP3, 
            const char * nonRakNetIP4
    #ifdef N_PLAT_NACL
            ,NPP_Instance chromeInstance
    #endif
            );
        // Releases the sockets created in Startup();
        void Shutdown();

        /// \internal For plugin handling
        virtual void update();

        /// \internal For plugin handling
        virtual ConnectReceiveType onMessage(SocketMessage * msg);
        virtual void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason );

        enum NATDetectionState
        {
            STATE_NONE,
            STATE_TESTING_NONE_1,
            STATE_TESTING_NONE_2,
            STATE_TESTING_FULL_CONE_1,
            STATE_TESTING_FULL_CONE_2,
            STATE_TESTING_ADDRESS_RESTRICTED_1,
            STATE_TESTING_ADDRESS_RESTRICTED_2,
            STATE_TESTING_PORT_RESTRICTED_1,
            STATE_TESTING_PORT_RESTRICTED_2,
            STATE_DONE,
        };

        struct NATDetectionAttempt
        {
            Address * mAddress;
            NATDetectionState detectionState;
            TimeDurMS nextStateTime;
            TimeDurMS timeBetweenAttempts;
            Nui16 c2Port;
            UniqueID mID;
        };

        virtual void onMessage(SocketMessageData * recvStruct);
        virtual void destroyMessage(SocketMessageData * s);
        virtual SocketMessageData * createMessage();
    protected:
        DataStructures::Queue<SocketMessageData *> bufferedPackets;
        ThreadMutex bufferedPacketsMutex;

        void OnDetectionRequest(SocketMessage * msg);
        DataStructures::List<NATDetectionAttempt> natDetectionAttempts;
        Nui32 GetDetectionAttemptIndex(const Address & sa);
        Nui32 GetDetectionAttemptIndex(UniqueID id);

        // s1p1 is rakpeer itself
        SocketObj *s1p2,*s2p3,*s3p4,*s4p5;
        //Nui16 s1p2Port, s2p3Port, s3p4Port, s4p5Port;
        char s3p4Address[64];
    };
}
}
#endif
#endif