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

#ifndef __NAT_TYPE_DETECTION_COMMON_H
#define __NAT_TYPE_DETECTION_COMMON_H

#include "NativeFeatureIncludes.h"

#if _RAKNET_SUPPORT_NatTypeDetectionServer==1 || _RAKNET_SUPPORT_NatTypeDetectionClient==1

#include "NiiPreProcess.h"
#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"
#include "SocketObj.h"

namespace NII
{
namespace NII_NET
{
	/// All possible types of NATs (except NAT_TYPE_COUNT, which is an internal value)
	enum NATTypeDetectionResult
	{
		/// Works with anyone
		NAT_TYPE_NONE,
		/// Accepts any datagrams to a port that has been previously used. Will accept the first datagram from the remote peer.
		NAT_TYPE_FULL_CONE,
		/// Accepts datagrams to a port as long as the datagram source IP address is a system we have already sent to. Will accept the first datagram if both systems send simultaneously. Otherwise, will accept the first datagram after we have sent one datagram.
		NAT_TYPE_ADDRESS_RESTRICTED,
		/// Same as address-restricted cone NAT, but we had to send to both the correct remote IP address and correct remote port. The same source address and port to a different destination uses the same mapping.
		NAT_TYPE_PORT_RESTRICTED,
		/// A different port is chosen for every remote destination. The same source address and port to a different destination uses a different mapping. Since the port will be different, the first external punchthrough attempt will fail. For this to work it requires port-prediction (MAX_PREDICTIVE_PORT_RANGE>1) and that the router chooses ports sequentially.
		NAT_TYPE_SYMMETRIC,
		/// Hasn't been determined. NATTypeDetectionClient does not use this, but other plugins might
		NAT_TYPE_UNKNOWN,
		/// In progress. NATTypeDetectionClient does not use this, but other plugins might
		NAT_TYPE_DETECTION_IN_PROGRESS,
		/// Didn't bother figuring it out, as we support UPNP, so it is equivalent to NAT_TYPE_NONE. NATTypeDetectionClient does not use this, but other plugins might
		NAT_TYPE_SUPPORTS_UPNP,
		/// \internal Must be last
		NAT_TYPE_COUNT
	};

	/// \return Can one system with NATTypeDetectionResult \a type1 connect to \a type2
	bool _EngineAPI CanConnect(NATTypeDetectionResult type1, NATTypeDetectionResult type2);

	/// Return a technical string representin the enumeration
	_EngineAPI const char * NATTypeDetectionResultToString(NATTypeDetectionResult type);

	/// Return a friendly string representing the enumeration
	/// None and relaxed can connect to anything
	/// Moderate can connect to moderate or less
	/// Strict can connect to relaxed or less
	_EngineAPI const char * NATTypeDetectionResultToStringFriendly(NATTypeDetectionResult type);

	/// \internal
	_EngineAPI SocketObj * CreateNonblockingBoundSocket(const char * bindAddr,
#ifdef N_PLAT_NACL
        NPP_Instance chromeInstance
#endif
        RNS2EventHandler *eventHandler;
}
}
#endif
#endif