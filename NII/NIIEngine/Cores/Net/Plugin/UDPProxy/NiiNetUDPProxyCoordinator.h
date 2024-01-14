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
#if _RAKNET_SUPPORT_UDPProxyCoordinator==1 && _RAKNET_SUPPORT_UDPForwarder==1

#ifndef __UDP_PROXY_COORDINATOR_H
#define __UDP_PROXY_COORDINATOR_H

#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"
#include "NiiNetPlugin.h"
#include "RakString.h"
#include "NiiNetSerializer.h"
#include "DS_Queue.h"
#include "DS_OrderedList.h"

namespace NII
{
namespace NII_NET
{
	/// When NAT Punchthrough fails, it is possible to use a non-NAT system to forward messages from us to the recipient, and vice-versa
	/// The class to forward messages is UDPForwarder, and it is triggered over the network via the UDPProxyServer plugin.
	/// The UDPProxyClient connects to UDPProxyCoordinator to get a list of servers running UDPProxyServer, and the coordinator will relay our forwarding request
	/// \brief Middleman between UDPProxyServer and UDPProxyClient, maintaining a list of UDPProxyServer, and managing state for clients to find an available forwarding server.
	/// \ingroup NAT_PUNCHTHROUGH_GROUP
	class _EngineAPI UDPProxyCoordinator : public Plugin
	{
	public:
		UDPProxyCoordinator();
		virtual ~UDPProxyCoordinator();

		/// For UDPProxyServers logging in remotely, they must pass a password to UDPProxyServer::LoginToCoordinator(). It must match the password set here.
		/// If no password is set, they cannot login remotely.
		/// By default, no password is set
		void SetRemoteLoginPassword(RakString password);

		/// \internal
		virtual void update();
		virtual ConnectReceiveType onMessage(SocketMessage * msg);
		virtual void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason );

		struct SenderAndTargetAddress
		{
			Address * senderClientAddress;
			UniqueID senderClientGuid;
			Address * targetClientAddress;
			UniqueID targetClientGuid;
		};

		struct ServerWithPing
		{
			Nui16 ping;
			Address * serverAddress;
		};

		struct ForwardingRequest
		{
			TimeDurMS timeoutOnNoDataMS;
			TimeDurMS timeoutAfterSuccess;
			SenderAndTargetAddress sata;
			Address * requestingAddress; // Which system originally sent the network message to start forwarding
			Address * currentlyAttemptedServerAddress;
			DataStructures::Queue<Address *> remainingServersToTry;
			NetSerializer serverSelectionBitstream;

			DataStructures::List<ServerWithPing> sourceServerPings, targetServerPings;
			TimeDurMS timeRequestedPings;
			// Order based on sourceServerPings and targetServerPings
			void OrderRemainingServersToTry(void);
		};
	protected:
		static int ServerWithPingComp(const Nui16 & key,
            const UDPProxyCoordinator::ServerWithPing & data);
		static int ForwardingRequestComp(const SenderAndTargetAddress & key,
            ForwardingRequest * const & data);

		void OnForwardingRequestFromClientToCoordinator(SocketMessage * msg);
		void OnLoginRequestFromServerToCoordinator(SocketMessage * msg);
		void OnForwardingReplyFromServerToCoordinator(SocketMessage * msg);
		void OnPingServersReplyFromClientToCoordinator(SocketMessage * msg);
		void TryNextServer(SenderAndTargetAddress sata, ForwardingRequest * fw);
		void SendAllBusy(Address * senderClientAddress, Address * targetClientAddress, UniqueID * targetClientGuid, Address requestingAddress);
		void Clear(void);

		void SendForwardingRequest(Address * sourceAddress, Address * targetAddress, Address * serverAddress, TimeDurMS timeoutOnNoDataMS);

		DataStructures::List<Address *> serverList;
		DataStructures::OrderedList<SenderAndTargetAddress, ForwardingRequest*, ForwardingRequestComp> forwardingRequestList;

		RakString remoteLoginPassword;
	};
}
}
#endif
#endif