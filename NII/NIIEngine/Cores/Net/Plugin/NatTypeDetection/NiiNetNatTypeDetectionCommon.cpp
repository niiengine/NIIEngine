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

#include "NatTypeDetectionCommon.h"

#if _RAKNET_SUPPORT_NatTypeDetectionServer == 1 || _RAKNET_SUPPORT_NatTypeDetectionClient == 1

#include "NiiPreProcess.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    bool CanConnect(NATTypeDetectionResult type1, NATTypeDetectionResult type2)
    {
        /// If one system is NAT_TYPE_SYMMETRIC, the other must be NAT_TYPE_ADDRESS_RESTRICTED or less
        /// If one system is NAT_TYPE_PORT_RESTRICTED, the other must be NAT_TYPE_PORT_RESTRICTED or less
        bool connectionGraph[NAT_TYPE_COUNT][NAT_TYPE_COUNT] =
        {
            // None,	Full Cone,	Address Restricted,		Port Restricted,	Symmetric,	Unknown,	InProgress,	Supports_UPNP
            {true, 		true, 		true, 					true, 				true,		false,		false,		true},		// None
            {true, 		true, 		true, 					true, 				true,		false,		false,		true},		// Full Cone
            {true, 		true, 		true, 					true, 				true,		false,		false,		true},		// Address restricted
            {true, 		true, 		true, 					true, 				false,		false,		false,		true},		// Port restricted
            {true, 		true, 		true, 					false, 				false,		false,		false,		true},		// Symmetric
            {false,		false,		false,					false,				false,		false,		false,		false},		// Unknown
            {false,		false,		false,					false,				false,		false,		false,		false},		// InProgress
            {true,		true,		true,					true,				true,		false,		false,		true}		// Supports_UPNP
        };

        return connectionGraph[(int) type1][(int) type2];
    }
    //------------------------------------------------------------------------
    const char * NATTypeDetectionResultToString(NATTypeDetectionResult type)
    {
        switch (type)
        {
        case NAT_TYPE_NONE:
            return "None";
        case NAT_TYPE_FULL_CONE:
            return "Full cone";
        case NAT_TYPE_ADDRESS_RESTRICTED:
            return "Address restricted";
        case NAT_TYPE_PORT_RESTRICTED:
            return "Port restricted";
        case NAT_TYPE_SYMMETRIC:
            return "Symmetric";
        case NAT_TYPE_UNKNOWN:
            return "Unknown";
        case NAT_TYPE_DETECTION_IN_PROGRESS:
            return "In Progress";
        case NAT_TYPE_SUPPORTS_UPNP:
            return "Supports UPNP";
        case NAT_TYPE_COUNT:
            return "NAT_TYPE_COUNT";
        }
        return "Error, unknown enum in NATTypeDetectionResult";
    }
    //------------------------------------------------------------------------
    // None and relaxed can connect to anything
    // Moderate can connect to moderate or less
    // Strict can connect to relaxed or less
    const char * NATTypeDetectionResultToStringFriendly(NATTypeDetectionResult type)
    {
        switch(type)
        {
        case NAT_TYPE_NONE:
            return "Open";
        case NAT_TYPE_FULL_CONE:
            return "Relaxed";
        case NAT_TYPE_ADDRESS_RESTRICTED:
            return "Relaxed";
        case NAT_TYPE_PORT_RESTRICTED:
            return "Moderate";
        case NAT_TYPE_SYMMETRIC:
            return "Strict";
        case NAT_TYPE_UNKNOWN:
            return "Unknown";
        case NAT_TYPE_DETECTION_IN_PROGRESS:
            return "In Progress";
        case NAT_TYPE_SUPPORTS_UPNP:
            return "Supports UPNP";
        case NAT_TYPE_COUNT:
            return "NAT_TYPE_COUNT";
        }
        return "Error, unknown enum in NATTypeDetectionResult";
    }
    //------------------------------------------------------------------------
    SocketObj * CreateNonblockingBoundSocket(const char * bindAddr
    #ifdef N_PLAT_NACL
        ,NPP_Instance chromeInstance
    #endif
        , RNS2EventHandler *eventHandler
        )
    {
        SocketObj * r2;
        N_Only(Net).alloc(r2);
    #if defined(N_PLAT_NACL)
        NaClSocketParam ncbp;
        RNS2_NativeClient * nativeClientSocket = (RNS2_NativeClient*) r2;
        ncbp.eventHandler = eventHandler;
        ncbp.forceHostAddress = (char*) bindAddr;
        ncbp.is_ipv6 = false;
        ncbp.nativeClientInstance = chromeInstance;
        ncbp.port = 0;
        nativeClientSocket->Bind(&ncbp);
    #elif defined(N_PLAT_WINRT)
        N_assert("TODO" && 0);
    #else
        if(r2->IsBerkleySocket())
        {
            BSDSocketParam bbp;
            bbp.port = 0;
            bbp.hostAddress = (char*)bindAddr;
            bbp.addressFamily = AF_INET;
            bbp.type = SOCK_DGRAM;
            bbp.protocol = 0;
            bbp.mBlock = false;
            bbp.mBroadcast = true;
            bbp.mIPHdrIncl = false;
            bbp.doNotFragment = false;
            bbp.pollingThreadPriority = 0;
            bbp.eventHandler = eventHandler;
            RNS2BindResult br = ((RNS2_Berkley*) r2)->Bind(&bbp);

            if(br == BR_FAILED_TO_BIND_SOCKET)
            {
                N_delete r2;
                return 0;
            }
            else if(br == BR_FAILED_SEND_TEST)
            {
                N_delete r2;
                return 0;
            }
            else
            {
                N_assert(br == BR_SUCCESS);
            }

            ((RNS2_Berkley *)r2)->CreateRecvPollingThread(0);
        }
        else
        {
            N_assert("TODO" && 0);
        }
    #endif
        return r2;
    }
    //------------------------------------------------------------------------
#endif // #if _RAKNET_SUPPORT_NatTypeDetectionServer==1 || _RAKNET_SUPPORT_NatTypeDetectionClient==1