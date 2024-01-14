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

#ifndef _RAK_PEER_INTERFACE_H_
#define _RAK_PEER_INTERFACE_H_

#include "NiiNetPreInclude.h"
#include "PacketLevel.h"
#include "NiiNetCommon.h"
#include "DS_List.h"
#include "NiiSharedPtr.h"
#include "SocketObj.h"

namespace NII
{
namespace NII_NET
{
    class Plugin;
    struct RPCMap;
    struct NetStats;
    struct RakNetBandwidth;
    class RouterInterface;
    class NetworkIDManager;

	/**
	@note ´«Êä²ã¸ÅÄî
	*/
    class _EngineAPI LinkBase
    {
    public:
        virtual ~LinkBase(){}

        /** Starts the network threads, opens the listen port.
        @par On Android, add the necessary permission to your application's androidmanifest.xml: <uses-permission android:name="android.permission.INTERNET" />
            Multiple calls while already active are ignored.  To call this function again with different settings, you must first call shutdown().
        @note Call setMaxRemoteCount if you want to accept incoming connections
        @param[in] max The maximum number of connections between this instance of NetworkSys and another instance of NetworkSys. Required so the network can preallocate and for thread safety. A pure client would set this to 1.  A pure server would set it to the number of allowed clients.- A hybrid would set it to the sum of both types of connections
        @param[in] localPort The port to listen for connections on. On linux the system may be set up so thast ports under 1024 are restricted for everything but the root user. Use a higher port for maximum compatibility.
        @param[in] detail An array of HostPeer structures to force RakNet to listen on a particular IP address or port (or both).  Each HostPeer will represent one unique socket.  Do not pass redundant structures.  To listen on a specific port, you can pass HostPeer(myPort,0); such as for a server.  For a client, it is usually OK to just pass HostPeer(); However, on the XBOX be sure to use IPPROTO_VDP
        @param[in] count The size of the \a detail array.  Pass 1 if you are not sure what to pass.
        @param[in] level Passed to the thread creation routine. Use THREAD_PRIORITY_NORMAL for Windows. For Linux based systems, you MUST pass something reasonable based on the thread priorities for your application.
        @return RAKNET_STARTED on success, otherwise appropriate failure enumeration.
        */
        virtual StartupResult init(HostPeerList hosts, NCount maxlink, int prclevel = -99999)=0;

        /** Sets how many incoming connections are allowed. If this is less than the number of players currently connected,
            no more players will be allowed to connect.  If this is greater than the maximum number of peers allowed,
            it will be reduced to the maximum number of peers allowed.
            Defaults to 0, meaning by default, nobody can connect to you
        @param[in] count Maximum number of incoming connections allowed.
        */
        virtual void setMaxRemoteCount(NCount cnt)=0;

        /** Returns the value passed to setMaxRemoteCount()
        @return the maximum number of incoming connections, which is always <= maxConnections
        */
        virtual NCount getMaxRemoteCount() const = 0;

        /** Returns how many open connections there are at this time
        @return the number of open connections
        */
        virtual Nui16 getActiveRemoteCount() const=0;

        /** Sets the password incoming connections must match in the call to connect (defaults to none). Pass 0 to passwordData to specify no password
            This is a way to set a low level password for all incoming connections.  To selectively reject connections, implement your own scheme using close() to remove unwanted connections
        @param[in] data A data block that incoming connections must match.  This can be just a password, or can be a stream of data. Specify 0 for no password data
        @param[in] length The length in bytes of passwordData
        */
        virtual void setEnterPassword(const Nui8 * data, NCount size)=0;

        /** Gets the password passed to setEnterPassword
        @param[out] data  Should point to a block large enough to hold the password data you passed to setEnterPassword()
        @param[in,out] length Maximum size of the array passwordData.  Modified to hold the number of bytes actually written
        */
        virtual void getEnterPassword(Nui8 * data, NCount * size)=0;

        /** connect to the specified host (ip or domain name) and server port.
        @remark Calling connect and not calling setMaxRemoteCount acts as a dedicated client.
            Calling both acts as a true peer. This is a non-blocking connection.
            You know the connection is successful when getState() returns IS_CONNECTED or receive() gets a message with the type identifier ID_CONNECTION_REQUEST_ACCEPTED.
            If the connection is not successful, such as a rejected connection or no response then neither of these things will happen.
        @pre Requires that you first call init()
        @param[in] data A data block that must match the data block on the server passed to setEnterPassword.  This can be a string or can be a stream of data.  Use 0 for no password.
        @param[in] lentgh The length in bytes of passwordData
        @param[in] publicKey The public key the server is using. If 0, the server is not using security. If non-zero, the publicKeyMode member determines how to connect
        @param[in] sindex Index into the array of socket descriptors passed to socketDescriptors in NetworkSys::init() to send on.
        @param[in] attempt How many datagrams to send to the other system to try to connect.
        @param[in] timeout How long to keep the connection alive before dropping it on unable to send a reliable message. 0 to use the default from setTimeOut(Address::invalid);
        @return CONNECTION_ATTEMPT_STARTED on successful initiation. Otherwise, an appropriate enumeration indicating failure.
        @note CONNECTION_ATTEMPT_STARTED does not mean you are already connected!
        @note It is possible to immediately get back ID_CONNECTION_ATTEMPT_FAILED if you exceed the maxConnections parameter passed to init(). This could happen if you call close() with sendDisconnectionNotificaiton true, then immediately call connect() before the connection has closed.
        */
        virtual ConnectResultType connect(const Address * ad, const Nui8 * data,
            NCount lentgh, PublicKey * publicKey = 0, Nui32 sindex = 0, NCount attempt = 12,
				TimeDurMS interval = 500, TimeDurMS timeout = 0)=0;

        /** connect to the specified host (ip or domain name) and server port, using a shared socket from another instance of RakNet
        @param[in] host Either a dotted IP address or a domain name
        @param[in] remotePort Which port to connect to on the remote machine.
        @param[in] data A data block that must match the data block on the server passed to setEnterPassword.  This can be a string or can be a stream of data.  Use 0 for no password.
        @param[in] length The length in bytes of data
        @param[in] socket A bound socket returned by another instance of LinkBase
        @param[in] attempt How many datagrams to send to the other system to try to connect.
        @param[in] timeout How long to keep the connection alive before dropping it on unable to send a reliable message. 0 to use the default from setTimeOut(Address::invalid);
        @return CONNECTION_ATTEMPT_STARTED on successful initiation. Otherwise, an appropriate enumeration indicating failure.
        @note CONNECTION_ATTEMPT_STARTED does not mean you are already connected!
        */
        virtual ConnectResultType connect(const Address * ad, const Nui8 * data,
            NCount length, SocketObj * socket, PublicKey * publicKey = 0,
                NCount attempt = 12, TimeDurMS interval = 500, TimeDurMS timeout = 0) = 0;

        /** brief Stops the network threads and closes all connections.
        @param[in] blockDuration How long, in milliseconds, you should wait for all remaining messages to go out, including ID_DISCONNECTION_NOTIFICATION.  If 0, it doesn't wait at all.
        @param[in] pch If blockDuration > 0, ID_DISCONNECTION_NOTIFICATION will be sent on this channel
        @param[in] disconnectionNotificationPriority Priority to send ID_DISCONNECTION_NOTIFICATION on.
            If you set it to 0 then the disconnection notification won't be sent
        */
        virtual void shutdown(TimeDurMS blockDuration, Nui8 pch = 0, PacketLevel disconnectionNotificationPriority = PL_LOW) = 0;

        /** Returns if the network thread is running
        @return true if the network thread is running, false otherwise
        */
        virtual bool isActive() const=0;

        /** Fills the array remotes with the Address of all the systems we are connected to
        @param[out] remotes An array of Address structures to be filled with the SystemAddresss of the systems we are connected to. Pass 0 to remotes to only get the number of systems we are connected to
        @param[in, out] numberOfSystems As input, the size of remotes array.  As output, the number of elements put into the array
        */
        virtual bool getConnect(Address * remote, Nui16 * numberOfSystems) const=0;

        /** Sends a block of data to the specified system that you are connected to.
            This function only works while connected
            The first byte should be a message identifier starting at ID_USER_PACKET_ENUM
        @param[in] data The block of data to send
        @param[in] length The size in bytes of the data to send
        @param[in] plevel What plevel to send on.  See PacketLevel.h
        @param[in] ptype How ptype to send this data.  See PacketLevel.h
        @param[in] pch When using ordered or sequenced messages, what channel to order these on. Messages are only ordered relative to other messages on the same stream
        @param[in] usys Who to send this msg to, or in the case of broadcasting who not to send it to.  Pass either a Address structure or a UniqueID structure. Use Address::invalid or to specify none
        @param[in] broadcast True to send this msg to all connected systems. If true, then address specifies who not to send the msg to.
        @param[in] forceReceipt If 0, will automatically determine the receipt number to return. If non-zero, will return what you give it.
        @return 0 on bad input. Otherwise a number that identifies this message. If \a ptype is a type that returns a receipt, on a later call to receive() you will get ID_SND_RECEIPT_ACKED or ID_SND_RECEIPT_LOSS with bytes 1-4 inclusive containing this number
        */
        virtual Nui32 send(const Nui8 * data, NCount size, PacketLevel plevel, PacketType ptype, Nui8 pch, 
			const UniqueAddress & usys, bool broadcast, Nui32 receipt = 0)=0;

        /** "send" to yourself rather than a remote system. The message will be processed through the plugins and returned to the game as usual
            This function works anytime
            The first byte should be a message identifier starting at ID_USER_PACKET_ENUM
        @param[in] data The block of data to send
        @param[in] size The size in bytes of the data to send
        */
        virtual void send(const Nui8 * data, NCount size)=0;

        /** Sends a block of data to the specified system that you are connected to.  Same as the above version, but takes a NetSerializer as input.
        @param[in] bitStream The bitstream to send
        @param[in] plevel What plevel to send on.  See PacketLevel.h
        @param[in] ptype How ptype to send this data.  See PacketLevel.h
        @param[in] pch When using ordered or sequenced messages, what channel to order these on. Messages are only ordered relative to other messages on the same stream
        @param[in] usys Who to send this msg to, or in the case of broadcasting who not to send it to. Pass either a Address structure or a UniqueID structure. Use Address::invalid or to specify none
        @param[in] broadcast True to send this msg to all connected systems. If true, then address specifies who not to send the msg to.
        @param[in] forceReceipt If 0, will automatically determine the receipt number to return. If non-zero, will return what you give it.
        @return 0 on bad input. Otherwise a number that identifies this message. If \a ptype is a type that returns a receipt, on a later call to receive() you will get ID_SND_RECEIPT_ACKED or ID_SND_RECEIPT_LOSS with bytes 1-4 inclusive containing this number
        @note COMMON MISTAKE: When writing the first byte, bitStream->write((Nui8) ID_MY_TYPE) be sure it is casted to a byte, and you are not writing a 4 byte enumeration.
        */
        virtual Nui32 send(const NetSerializer * data, PacketLevel plevel, PacketType ptype, char pch,
			const UniqueAddress & usys, bool broadcast, Nui32 receipt = 0)=0;

        /** Sends multiple blocks of data, concatenating them automatically.
        ///
        /// This is equivalent to:
        /// NetSerializer bs;
        /// bs.write(block1, blockLength1);
        /// bs.write(block2, blockLength2);
        /// bs.write(block3, blockLength3);
        /// send(&bs, ...)
        ///
        /// This function only works while connected
        @param[in] data An array of pointers to blocks of data
        @param[in] size An array of integers indicating the length of each block of data
        @param[in] dsize Length of the arrays data and lengths
        @param[in] plevel What plevel to send on.  See PacketLevel.h
        @param[in] ptype How ptype to send this data.  See PacketLevel.h
        @param[in] pch When using ordered or sequenced messages, what channel to order these on. Messages are only ordered relative to other messages on the same stream
        @param[in] usys Who to send this msg to, or in the case of broadcasting who not to send it to. Pass either a Address structure or a UniqueID structure. Use Address::invalid or to specify none
        @param[in] broadcast True to send this msg to all connected systems. If true, then address specifies who not to send the msg to.
        @param[in] forceReceipt If 0, will automatically determine the receipt number to return. If non-zero, will return what you give it.
        @return 0 on bad input. Otherwise a number that identifies this message. If \a ptype is a type that returns a receipt, on a later call to receive() you will get ID_SND_RECEIPT_ACKED or ID_SND_RECEIPT_LOSS with bytes 1-4 inclusive containing this number
        */
        virtual Nui32 send(const Nui8 ** data, NCount * size, NCount dsize,
            PacketLevel plevel, PacketType ptype, Nui8 pch, UniqueAddress usys,
                bool broadcast, Nui32 forceReceiptNumber = 0)=0;

        /** Gets a message from the incoming message queue.
            User-thread functions, such as RPC calls and the plugin function Plugin::update occur here.
        @return 0 if no packets are waiting to be handled, otherwise a pointer to a msg.
        @note COMMON MISTAKE: Be sure to call this in a loop, once per game tick, until it returns 0. If you only process one msg per game tick they will buffer up.
        */
        virtual SocketMessage * receive() = 0;

        /// Return the total number of connections we are allowed
        virtual NCount getRemoteCount() const = 0;

        /** Close the connection to another host (if we initiated the connection it will disconnect, if they did it will kick them out).
        @param[in] target Which system to close the connection to.
        @param[in] sendDisconnectionNotification True to send ID_DISCONNECTION_NOTIFICATION to the recipient.  False to close it silently.
        @param[in] channel Which ordering channel to send the disconnection notification on, if any
        @param[in] disconnectionNotificationPriority Priority to send ID_DISCONNECTION_NOTIFICATION on.
        */
        virtual void close(const UniqueAddress & dst, bool notify, Nui8 pch = 0, PacketLevel plevel = PL_LOW)=0;

        /** Returns if a system is connected, disconnected, connecting in progress, or various other states
        @param[in] usys The system we are referring to
        @note This locks a mutex, do not call too frequently during connection attempts or the attempt will take longer and possibly even timeout
        @return What state the remote system is in
        */
        virtual ConnectionState getState(const UniqueAddress & usys) = 0;

        /** Cancel a pending connection attempt
            If we are already connected, the connection stays open
        @param[in] target Which system to cancel
        */
        virtual void cancel(const Address * target) = 0;

        /** This function is only useful for looping through all systems
            Given an index, will return a Address.
        @param[in] index Index should range between 0 and the maximum number of players allowed - 1.
        @return The Address
        */
        virtual Address * getAddress(Nindex index)=0;

        /** Same as getAddress but returns UniqueID
        @param[in] index Index should range between 0 and the maximum number of players allowed - 1.
        @return The UniqueID
        */
        virtual const UniqueID & getGUID(Nindex index)=0;

        /** Given a connected system, give us the unique UniqueID representing that instance of NetworkSys.
            This will be the same on all systems connected to that instance of NetworkSys, even if the external system addresses are different
            Currently O(log(n)), but this may be improved in the future. If you use this frequently, you may want to cache the value as it won't change.
            Returns UniqueID::INVALID if system address can't be found.
            If \a input is Address::invalid, will return your own UniqueID
        @pre Call init() first, or the function will return UniqueID::INVALID
        @param[in] input The system address of the system we are connected to
        */
        virtual const UniqueID & getGUID(const Address * input) const=0;

        /** Same as calling getAddress and getGUID for all systems, but more efficient
            Indices match each other, so \a addresses[0] and \a guids[0] refer to the same system
        @param[out] addresses All system addresses. Size of the list is the number of connections. Size of the list will match the size of the \a guids list.
        @param[out] guids All guids. Size of the list is the number of connections. Size of the list will match the size of the \a addresses list.
        */
        virtual void getActive(DataStructures::List<Address> & addresses, DataStructures::List<UniqueID> & guids) const = 0;

        /** Enable or disable allowing frequent connections from the same IP adderss
            This is a security measure which is disabled by default, but can be set to true to prevent attackers from using up all connection slots
        @param[in] b True to limit connections from the same ip to at most 1 per 100 milliseconds.
        */
        virtual void setConnectTimeLimit(bool b) = 0;

        /** send a ping to the specified connected system.
        @pre The sender and recipient must already be started via a successful call to init()
        @param[in] target Which system to ping
        */
        virtual void ping(const Address * target) = 0;

        /** send a ping to the specified unconnected system. The remote system, if it is Initialized, will respond with ID_PONG followed by sizeof(TimeDurMS) containing the system time the ping was sent.
            System should reply with ID_PONG if it is active
        @param[in] host Either a dotted IP address or a domain name.  Can be 255.255.255.255 for LAN broadcast.
        @param[in] remotePort Which port to connect to on the remote machine.
        @param[in] onlyReplyOnAcceptingConnections Only request a reply if the remote system is accepting connections
        @param[in] connectionSocketIndex Index into the array of socket descriptors passed to socketDescriptors in NetworkSys::init() to send on.
        @return true on success, false on failure (unknown hostname)
        */
        virtual bool ping(const String & host, Nui16 port, bool onlyReplyOnAcceptingConnections, Nui32 sindex = 0)=0;

        /** Returns the average of all ping times read for the specific system or -1 if none read yet
        @param[in] address Which system we are referring to
        @return The ping time for this system, or -1
        */
        virtual int getAvgPing(const UniqueAddress & usys) = 0;

        /** Returns the last ping time read for the specific system or -1 if none read yet
        @param[in] address Which system we are referring to
        @return The last ping time for this system, or -1
        */
        virtual int getLastPing(const UniqueAddress & usys) const = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual int getBestPing(const UniqueAddress & usys) const = 0;

        /** Returns the lowest ping time read or -1 if none read yet
        @param[in] address Which system we are referring to
        @return The lowest ping time for this system, or -1
        */
        virtual int getWorstPing(const UniqueAddress & usys) const = 0;

        /** ping the remote systems every so often, or not. Can be called anytime.
            By default this is true. Recommended to leave on, because congestion control uses it to determine how often to resend lost packets.
            It would be true by default to prevent timestamp drift, since in the event of a clock spike, the timestamp deltas would no longer be accurate
        @param[in] doPing True to start occasional pings.  False to stop them.
        */
        virtual void setPingTest(bool b) = 0;

        /** Return the clock difference between your system and the specified system
            Subtract getTimeEquation() from a time returned by the remote system to
            get that time relative to your own system Returns 0 if the system is unknown
        @param[in] usys Which system we are referring to
        */
        virtual TimeDurMS getTimeEquation(const UniqueAddress & usys) = 0;

        /** Sets the data to send along with a LAN server discovery or offline ping reply.
            length should be under 400 bytes, as a security measure against flood attacks
        @param[in] data a block of data to store, or 0 for none
        @param[in] length The length of data in bytes, or 0 for none
        */
        virtual void setTempPing(const Nui8 * data, NCount size) = 0;

        /** Returns pointers to a copy of the data passed to setTempPing
        @param[out] data A pointer to a copy of the data passed to \a setTempPing()
        @param[out] length A pointer filled in with the length parameter passed to setTempPing()
        */
        virtual void getTempPing(char ** data, Nui32 * length)=0;

        /**
        @version NIIEngine 3.0.0
        */
        virtual Address getLAN(const Address & address = Address::invalid, const int index=0 ) const=0;

        /** Sets your internal IP address, for platforms that do not support reading it, or to override a value
        @param[in] address. The address to set. Use Address::write() if you want to use a dotted string
        @param[in] index When you have multiple internal IDs, which index to set?
        */
        virtual void addLocalHost(const Address * address) = 0;

        /** Return the unique address identifier that represents you on the the network and is based on your externalIP / port
            (the IP / port the specified player uses to communicate with you)
        @param[in] target Which remote system you are referring to for your external ID.  Usually the same for all systems, unless you have two or more network cards.
        */
        virtual Address getConnectTo(const Address * target) const = 0;

        /// Return my own UniqueID
        virtual const UniqueID & getGuid() const=0;

        /** Given the UniqueID of a connected system, give us the system address of that system.
            The UniqueID will be the same on all systems connected to that instance of NetworkSys, even if the external system addresses are different
            Currently O(log(n)), but this may be improved in the future. If you use this frequently, you may want to cache the value as it won't change.
            If \a input is UniqueID::INVALID, will return Address::invalid
        @param[in] input The UniqueID of the system we are checking to see if we are connected to
        */
        virtual Address getAddress(const UniqueID & input) const=0;

        /** Set the time, in MS, to use before considering ourselves disconnected after not being able to deliver a reliable message.
            Default time is 10,000 or 10 seconds in release and 30,000 or 30 seconds in debug.
            Do not set different values for different computers that are connected to each other, or you won't be able to reconnect after ID_CONNECTION_LOST
        @param[in] timeMS Time, in MS
        @param[in] target Which system to do this for. Pass Address::invalid for all systems.
        */
        virtual void setTimeOut(const Address * target, TimeDurMS time)=0;

        /**
        @param[in] target Which system to do this for. Pass Address::invalid to get the default value
        @return timeout for a given system.
        */
        virtual TimeDurMS getTimeOut(const Address * target) const = 0;

        /** Returns the current MTU size
        @param[in] target Which system to get this for.  Address::invalid to get the default
        @return The current MTU size
        */
        virtual int getMTU(const Address * target) const = 0;

        /**
        @version NIIEngine 3.0.0
        */
        virtual String getLocalHost(Nindex index) = 0;

        /**
        @version NIIEngine 3.0.0
        */
        virtual NCount getLocalHostCount() = 0;

        /** Is this a local IP?
        @param[in] An IP address to check, excluding the port
        @return True if this is one of the IP addresses returned by getLocalHost
        */
        virtual bool isLocalHost(const String & ip) = 0;

        /** Allow or disallow connection responses from any IP. Normally this should be false, but may be necessary
            when connecting to servers with multiple IP addresses.
        @param[in] allow - True to allow this behavior, false to not allow. Defaults to false. Value persists between connections
        */
        virtual void setAlterIP(bool set) = 0;

        /** Sends a one byte message ID_ADVERTISE_SYSTEM to the remote unconnected system.
            This will tell the remote system our external IP outside the LAN along with some user data.
        @pre The sender and recipient must already be started via a successful call to Initialize
        @param[in] host Either a dotted IP address or a domain name
        @param[in] port Which port to connect to on the remote machine.
        @param[in] data Optional data to append to the msg.
        @param[in] size length of data in bytes.  Use 0 if no data.
        @param[in] connectionSocketIndex Index into the array of socket descriptors passed to socketDescriptors in NetworkSys::init() to send on.
        @return false if isActive() == false or the host is unresolvable. True otherwise
        */
        virtual bool send(const String & host, Nui16 port, const Nui8 * data, NCount size, Nui32 sindex = 0) = 0;

        /** Controls how often to return ID_DOWNLOAD_PROGRESS for large message downloads.
            ID_DOWNLOAD_PROGRESS is returned to indicate a new partial message chunk, roughly the MTU size, has arrived
            As it can be slow or cumbersome to get this notification for every chunk, you can set the interval at which it is returned.
            Defaults to 0 (never return this notification)
        @param[in] interval How many messages to use as an interval
        */
        virtual void setSplitFactor(int interval)=0;

        /** Returns what was passed to setSplitFactor()
        @return What was passed to setSplitFactor(). Default to 0.
        */
        virtual int getSplitFactor() const=0;

        /** Set how long to wait before giving up on sending an unreliable message
            Useful if the network is clogged up.
            Set to 0 or less to never timeout.  Defaults to 0.
        @param[in] timeoutMS How many ms to wait before simply not sending an unreliable message.
        */
        virtual void setMessageTimeOut(TimeDurMS timeoutMS)=0;

        /** send a message to host, with the IP socket option TTL set to 3
            This message will not reach the host, but will open the router.
            Used for NAT-Punchthrough
        */
        virtual void sendTTL(const String & host, Nui16 port, int ttl, Nui32 sindex = 0)=0;

        /** ¸½¼Ó²å¼þ
        @version NIIEngine 3.0.0
        */
        virtual void attach(Plugin * plugin) = 0;

        /** ·ÖÀë²å¼þ
        @version NIIEngine 3.0.0
        */
        virtual void detach(Plugin * plugin) = 0;

        /** Put a message back at the end of the receive queue in case you don't want to deal with it immediately
        @param[in] msg The msg you want to push back.
        @param[in] pushAtHead True to push the msg so that the next receive call returns it.  False to push it at the end of the queue (obviously pushing it at the end makes the packets out of order)
        */
        virtual void addMessage(SocketMessage * msg, bool pushAtHead) = 0;

        /** For a given system identified by id, change the Address to send to.
        @param[in] id The connection we are referring to
        @param[in] address The new address to send to
        */
        virtual void swap(UniqueID id, const Address * address)=0;

        /** Get the socket used with a particular active connection
            The smart pointer reference counts the SocketObj object, so the socket will remain active as long as the smart pointer does, even if RakNet were to shutdown or close the connection.
        @note This sends a query to the thread and blocks on the return value for up to one second. In practice it should only take a millisecond or so.
        @param[in] target Which system
        @return A smart pointer object containing the socket information about the socket. Be sure to check IsNull() which is returned if the update thread is unresponsive, shutting down, or if this system is not connected
        */
        virtual SocketObj * getSocket(const Address * target) = 0;

        /** Get all sockets in use
        @note This sends a query to the thread and blocks on the return value for up to one second. In practice it should only take a millisecond or so.
        @param[out] sockets List of SocketObj structures in use. Sockets will not be closed until \a sockets goes out of scope
        */
        virtual void getSockets(DataStructures::List<SocketObj *> & sockets)=0;

        /** Returns a structure containing a large set of network statistics for the specified system.
            You can map this data to a string using the C style StatisticsToString() function
        @param[in] address: Which connected system to get statistics for
        @param[in] rns If you supply this structure, it will be written to it.  Otherwise it will use a static struct, which is not threadsafe
        @return 0 on can't find the specified system.  A pointer to a set of data otherwise.
        */
        virtual NetStats * getReport(const Address * address, NetStats * rns = 0)=0;

        /**
        @brief Returns the network statistics of the system at the given index in the mRemote.
        @return True if the index is less than the maximum number of peers allowed and the system is active. False otherwise.
        */
        virtual bool getReport(Nui32 index, NetStats * rns)=0;

        /** Returns the list of systems, and statistics for each of those systems
            Each system has one entry in each of the lists, in the same order
        @param[out] addresses Address for each connected system
        @param[out] guids UniqueID for each connected system
        @param[out] statistics Calculated NetStats for each connected system
        */
        virtual void getReport(DataStructures::List<Address> & addresses, DataStructures::List<UniqueID> & guids, 
            DataStructures::List<NetStats> & statistics) = 0;

        /// \internal
        virtual bool sendExpedite(const String & host, Nui16 port, const Nui8 * data, NCount size, Nui32 connectionSocketIndex = 0)=0;

    };
}
}
#endif