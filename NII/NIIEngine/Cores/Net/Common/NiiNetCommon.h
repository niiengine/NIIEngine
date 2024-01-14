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
#ifndef _NII_NET_COMMON_H_
#define _NII_NET_COMMON_H_

#include "NiiPreinclude.h"
#include "NiiSharedPtr.h"
#include "RakString.h"
#include "NiiNetPlat.h"

namespace NII
{
    enum OutOfBandIdentifiers
    {
        ID_NAT_ESTABLISH_UNIDIRECTIONAL,
        ID_NAT_ESTABLISH_BIDIRECTIONAL,
        ID_NAT_TYPE_DETECT,
        ID_ROUTER_2_REPLY_TO_SENDER_PORT,
        ID_ROUTER_2_REPLY_TO_SPECIFIED_PORT,
        ID_ROUTER_2_MINI_PUNCH_REPLY,
        ID_ROUTER_2_MINI_PUNCH_REPLY_BOUNCE,
        ID_XBOX_360_VOICE,
        ID_XBOX_360_GET_NETWORK_ROOM,
        ID_XBOX_360_RETURN_NETWORK_ROOM,
        ID_NAT_PING,
        ID_NAT_PONG,
    };

    /// You should not edit the file NiiNetCommon.h as it is a part of RakNet static library
    /// To define your own message id, define an enum following the code example that follows.
    ///
    /// \code
    /// enum {
    ///   ID_MYPROJECT_MSG_1 = ID_USER_PACKET_ENUM,
    ///   ID_MYPROJECT_MSG_2,
    ///    ...
    /// };
    /// \endcode
    ///
    /// \note All these enumerations should be casted to (Nui8) before writing them to NetSerializer
    enum DefaultMessageIDTypes
    {
        //
        // RESERVED TYPES - DO NOT CHANGE THESE
        // All types from NetworkSys
        //
        /// These types are never returned to the user.
        /// ping from a connected system.  update timestamps (internal use only)
        ID_CONNECTED_PING,
        /// ping from an unconnected system.  Reply but do not update timestamps. (internal use only)
        ID_UNCONNECTED_PING,
        /// ping from an unconnected system.  Only reply if we have open connections. Do not update timestamps. (internal use only)
        ID_UNCONNECTED_PING_OPEN_CONNECTIONS,
        /// Pong from a connected system.  update timestamps (internal use only)
        ID_CONNECTED_PONG,
        /// A reliable packet to detect lost connections (internal use only)
        ID_DETECT_LOST_CONNECTIONS,
        /// C2S: Initial query: Header(1), OfflineMesageID(16), Protocol number(1), Pad(toMTU), sent with no fragment set.
        /// If protocol fails on server, returns ID_INCOMPATIBLE_PROTOCOL_VERSION to client
        ID_OPEN_CONNECTION_REQUEST_1,
        /// S2C: Header(1), OfflineMesageID(16), server UniqueID(8), HasSecurity(1), Cookie(4, if HasSecurity)
        /// , public key (if do security is true), MTU(2). If public key fails on client, returns ID_PUBLIC_KEY_MISMATCH
        ID_OPEN_CONNECTION_REPLY_1,
        /// C2S: Header(1), OfflineMesageID(16), Cookie(4, if HasSecurity is true on the server), clientSupportsSecurity(1 bit),
        /// handshakeChallenge (if has security on both server and client), remoteBindingAddress(6), MTU(2), client UniqueID(8)
        /// Connection slot allocated if cookie is valid, server is not full, UniqueID and IP not already in use.
        ID_OPEN_CONNECTION_REQUEST_2,
        /// S2C: Header(1), OfflineMesageID(16), server UniqueID(8), mtu(2), doSecurity(1 bit), handshakeAnswer (if do security is true)
        ID_OPEN_CONNECTION_REPLY_2,
        /// C2S: Header(1), UniqueID(8), Timestamp, HasSecurity(1), Proof(32)
        ID_CONNECTION_REQUEST,
        /// NetworkSys - Remote system requires secure connections, pass a public key to LinkBase::Connect()
        ID_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY,
        /// NetworkSys - We passed a public key to LinkBase::Connect(), but the other system did not have security turned on
        ID_OUR_SYSTEM_REQUIRES_SECURITY,
        /// NetworkSys - Wrong public key passed to LinkBase::Connect()
        ID_PUBLIC_KEY_MISMATCH,
        /// NetworkSys - Same as ID_ADVERTISE_SYSTEM, but intended for internal use rather than being passed to the user.
        /// Second byte indicates type. Used currently for NAT punchthrough for receiver port advertisement. See ID_NAT_ADVERTISE_RECIPIENT_PORT
        ID_OUT_OF_BAND_INTERNAL,
        /// If LinkBase::send() is called where PacketType contains _WITH_ACK_RECEIPT, then on a later call to
        /// LinkBase::receive() you will get ID_SND_RECEIPT_ACKED or ID_SND_RECEIPT_LOSS. The message will be 5 bytes long,
        /// and bytes 1-4 inclusive will contain a number in native order containing a number that identifies this message.
        /// This number will be returned by LinkBase::send() or LinkBase::send(). ID_SND_RECEIPT_ACKED means that
        /// the message arrived
        ID_SND_RECEIPT_ACKED,
        /// If LinkBase::send() is called where PacketType contains PT_RECEIPT_WILL_LOST, then on a later call to
        /// LinkBase::receive() you will get ID_SND_RECEIPT_ACKED or ID_SND_RECEIPT_LOSS. The message will be 5 bytes long,
        /// and bytes 1-4 inclusive will contain a number in native order containing a number that identifies this message. This number
        /// will be returned by LinkBase::send() or LinkBase::send(). ID_SND_RECEIPT_LOSS means that an ack for the
        /// message did not arrive (it may or may not have been delivered, probably not). On disconnect or shutdown, you will not get
        /// ID_SND_RECEIPT_LOSS for unsent messages, you should consider those messages as all lost.
        ID_SND_RECEIPT_LOSS,


        //
        // USER TYPES - DO NOT CHANGE THESE
        //

        /// NetworkSys - In a client/server environment, our connection request to the server has been accepted.
        ID_CONNECTION_REQUEST_ACCEPTED,
        /// NetworkSys - Sent to the player when a connection request cannot be completed due to inability to connect.
        ID_CONNECTION_ATTEMPT_FAILED,
        /// NetworkSys - Sent a connect request to a system we are currently connected to.
        ID_ALREADY_CONNECTED,
        /// NetworkSys - A remote system has successfully connected.
        ID_NEW_INCOMING_CONNECTION,
        /// NetworkSys - The system we attempted to connect to is not accepting new connections.
        ID_NO_FREE_INCOMING_CONNECTIONS,
        /// 连接,指定地址与我断开,对于客户端而言,服务器已经关闭了
        ID_DISCONNECTION_NOTIFICATION,
        /// NetworkSys - Reliable packets cannot be delivered to the system specified in SocketMessage::mAddress.  The connection to that
        /// system has been closed.
        ID_CONNECTION_LOST,
        /// NetworkSys - We are banned from the system we attempted to connect to.
        ID_CONNECTION_BANNED,
        /// NetworkSys - The remote system is using a password and has refused our connection because we did not set the correct password.
        ID_INVALID_PASSWORD,
        // N_NET_PROTOCOL_VER in NiiPreInclude.h does not match on the remote system what we have on our system
        // This means the two systems cannot communicate.
        // The 2nd byte of the message contains the value of N_NET_PROTOCOL_VER for the remote system
        ID_INCOMPATIBLE_PROTOCOL_VERSION,
        // Means that this IP address connected recently, and can't connect again as a security measure. See
        /// NetworkSys::setConnectTimeLimit()
        ID_IP_RECENTLY_CONNECTED,
        /// NetworkSys - The sizeof(RakNetTime) bytes following this byte represent a value which is automatically modified by the difference
        /// in system times between the sender and the recipient. Requires that you call setPingTest.
        ID_TIMESTAMP,
        /// NetworkSys - Pong from an unconnected system.  First byte is ID_UNCONNECTED_PONG, second sizeof(TimeDurMS) bytes is the ping,
        /// following bytes is system specific enumeration data.
        /// read using bitstreams
        ID_UNCONNECTED_PONG,
        /// NetworkSys - Inform a remote system of our IP/Port. On the recipient, all data past ID_ADVERTISE_SYSTEM is whatever was passed to
        /// the data parameter
        ID_ADVERTISE_SYSTEM,
        // NetworkSys - Downloading a large message. Format is ID_DOWNLOAD_PROGRESS (Token), partCount (Nui32),
        ///  partTotal (Nui32),
        /// partLength (Nui32), first part data (length <= MAX_MTU_SIZE). See the three parameters partCount, partTotal
        ///  and partLength in OnFileProgress in FileListTransferCBInterface.h
        ID_DOWNLOAD_PROGRESS,

        /// LinkState plugin - In a client/server environment, a client other than ourselves has disconnected gracefully.
        ///   SocketMessage::mAddress is modified to reflect the address of this client.
        ID_REMOTE_DISCONNECTION_NOTIFICATION,
        /// LinkState plugin - In a client/server environment, a client other than ourselves has been forcefully dropped.
        ///  SocketMessage::mAddress is modified to reflect the address of this client.
        ID_REMOTE_CONNECTION_LOST,
        /// LinkState plugin: Bytes 1-4 = count. for (count items) contains {Address, UniqueID, 2 byte ping}
        ID_REMOTE_NEW_INCOMING_CONNECTION,

        /// FileListTransfer plugin - Setup data
        ID_FILE_LIST_TRANSFER_HEADER,
        /// FileListTransfer plugin - A file
        ID_FILE_LIST_TRANSFER_FILE,
        // Ack for reference push, to send more of the file
        ID_FILE_LIST_REFERENCE_PUSH_ACK,

        /// DirectoryDeltaTransfer plugin - Request from a remote system for a download of a directory
        ID_DDT_DOWNLOAD_REQUEST,

        /// RakNetTransport plugin - Transport provider message, used for remote console
        ID_TRANSPORT_STRING,

        /// ReplicaManager plugin - Create an object
        ID_REPLICA_MANAGER_CONSTRUCTION,
        /// ReplicaManager plugin - Changed scope of an object
        ID_REPLICA_MANAGER_SCOPE_CHANGE,
        /// ReplicaManager plugin - Serialized data of an object
        ID_REPLICA_MANAGER_SERIALIZE,
        /// ReplicaManager plugin - New connection, about to send all world objects
        ID_REPLICA_MANAGER_DOWNLOAD_STARTED,
        /// ReplicaManager plugin - Finished downloading all serialized objects
        ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE,

        /// RakVoice plugin - Open a communication channel
        ID_RAKVOICE_OPEN_CHANNEL_REQUEST,
        /// RakVoice plugin - Communication channel accepted
        ID_RAKVOICE_OPEN_CHANNEL_REPLY,
        /// RakVoice plugin - Close a communication channel
        ID_RAKVOICE_CLOSE_CHANNEL,
        /// RakVoice plugin - Voice data
        ID_RAKVOICE_DATA,

        /// Autopatcher plugin - Get a list of files that have changed since a certain date
        ID_AUTOPATCHER_GET_CHANGELIST_SINCE_DATE,
        /// Autopatcher plugin - A list of files to create
        ID_AUTOPATCHER_CREATION_LIST,
        /// Autopatcher plugin - A list of files to delete
        ID_AUTOPATCHER_DELETION_LIST,
        /// Autopatcher plugin - A list of files to get patches for
        ID_AUTOPATCHER_GET_PATCH,
        /// Autopatcher plugin - A list of patches for a list of files
        ID_AUTOPATCHER_PATCH_LIST,
        /// Autopatcher plugin - Returned to the user: An error from the database repository for the autopatcher.
        ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR,
        /// Autopatcher plugin - Returned to the user: The server does not allow downloading unmodified game files.
        ID_AUTOPATCHER_CANNOT_DOWNLOAD_ORIGINAL_UNMODIFIED_FILES,
        /// Autopatcher plugin - Finished getting all files from the autopatcher
        ID_AUTOPATCHER_FINISHED_INTERNAL,
        ID_AUTOPATCHER_FINISHED,
        /// Autopatcher plugin - Returned to the user: You must restart the application to finish patching.
        ID_AUTOPATCHER_RESTART_APPLICATION,

        /// NATPunchthrough plugin: internal
        ID_NAT_PUNCHTHROUGH_REQUEST,
        /// NATPunchthrough plugin: internal
        ID_NAT_CONNECT_AT_TIME,
        /// NATPunchthrough plugin: internal
        ID_NAT_GET_MOST_RECENT_PORT,
        /// NATPunchthrough plugin: internal
        ID_NAT_CLIENT_READY,

        /// NATPunchthrough plugin: Destination system is not connected to the server. Bytes starting at offset 1 contains the
        ///  UniqueID destination field of RouterMapClient::OpenNAT().
        ID_NAT_TARGET_NOT_CONNECTED,
        /// NATPunchthrough plugin: Destination system is not responding to ID_NAT_GET_MOST_RECENT_PORT. Possibly the plugin is not installed.
        ///  Bytes starting at offset 1 contains the UniqueID  destination field of RouterMapClient::OpenNAT().
        ID_NAT_TARGET_UNRESPONSIVE,
        /// NATPunchthrough plugin: The server lost the connection to the destination system while setting up punchthrough.
        ///  Possibly the plugin is not installed. Bytes starting at offset 1 contains the UniqueID  destination
        ///  field of RouterMapClient::OpenNAT().
        ID_NAT_CONNECTION_TO_TARGET_LOST,
        /// NATPunchthrough plugin: This punchthrough is already in progress. Possibly the plugin is not installed.
        ///  Bytes starting at offset 1 contains the UniqueID destination field of RouterMapClient::OpenNAT().
        ID_NAT_ALREADY_IN_PROGRESS,
        /// NATPunchthrough plugin: This message is generated on the local system, and does not come from the network.
        ///  packet::mID contains the destination field of RouterMapClient::OpenNAT(). Byte 1 contains 1 if you are the sender, 0 if not
        ID_NAT_PUNCHTHROUGH_FAILED,
        /// NATPunchthrough plugin: Punchthrough succeeded. See packet::mAddress and packet::mID. Byte 1 contains 1 if you are the sender,
        ///  0 if not. You can now use NetworkSys::Connect() or other calls to communicate with this system.
        ID_NAT_PUNCHTHROUGH_SUCCEEDED,

        /// ReadyEvent plugin - Set the ready state for a particular system
        /// First 4 bytes after the message contains the id
        ID_READY_EVENT_SET,
        /// ReadyEvent plugin - Unset the ready state for a particular system
        /// First 4 bytes after the message contains the id
        ID_READY_EVENT_UNSET,
        /// All systems are in state ID_READY_EVENT_SET
        /// First 4 bytes after the message contains the id
        ID_READY_EVENT_ALL_SET,
        /// \internal, do not process in your game
        /// ReadyEvent plugin - Request of ready event state - used for pulling data when newly connecting
        ID_READY_EVENT_QUERY,

        /// Lobby packets. Second byte indicates type.
        ID_LOBBY_GENERAL,

        // RPC3, RPC error
        ID_RPC_REMOTE_ERROR,
        /// Plugin based replacement for RPC system
        ID_RPC_PLUGIN,

        /// FileListTransfer transferring large files in chunks that are read only when needed, to save memory
        ID_FILE_LIST_REFERENCE_PUSH,
        /// Force the ready event to all set
        ID_READY_EVENT_FORCE_ALL_SET,

        /// Rooms function
        ID_ROOMS_EXECUTE_FUNC,
        ID_ROOMS_LOGON_STATUS,
        ID_ROOMS_HANDLE_CHANGE,

        /// Lobby2 message
        ID_LOBBY2_SEND_MESSAGE,
        ID_LOBBY2_SERVER_ERROR,

        /// Informs user of a new host UniqueID. SocketMessage::Guid contains this new host RakNetGuid. The old host can be read out using NetSerializer->read(RakNetGuid) starting on byte 1
        /// This is not returned until connected to a remote system
        /// If the oldHost is UniqueID::INVALID, then this is the first time the host has been determined
        ID_FCM2_NEW_HOST,
        /// \internal For FullyConnected plugin
        ID_FCM2_REQUEST_FCMGUID,
        /// \internal For FullyConnected plugin
        ID_FCM2_RESPOND_CONNECTION_COUNT,
        /// \internal For FullyConnected plugin
        ID_FCM2_INFORM_FCMGUID,
        /// \internal For FullyConnected plugin
        ID_FCM2_UPDATE_MIN_TOTAL_CONNECTION_COUNT,
        /// A remote system (not necessarily the host) called FullyConnected::StartVerifiedJoin() with our system as the client
        /// Use FullyConnected::GetVerifiedJoinRequiredProcessingList() to read systems
        /// For each system, attempt RouterMapClient::OpenNAT() and/or LinkBase::Connect()
        /// When this has been done for all systems, the remote system will automatically be informed of the results
        /// \note Only the designated client gets this message
        /// \note You won't get this message if you are already connected to all target systems
        /// \note If you fail to connect to a system, this does not automatically mean you will get ID_FCM2_VERIFIED_JOIN_FAILED as that system may have been shutting down from the host too
        /// \sa FullyConnected::StartVerifiedJoin()
        ID_FCM2_VERIFIED_JOIN_START,
        /// \internal The client has completed processing for all systems designated in ID_FCM2_VERIFIED_JOIN_START
        ID_FCM2_VERIFIED_JOIN_CAPABLE,
        /// Client failed to connect to a required systems notified via FullyConnected::StartVerifiedJoin()
        /// LinkBase::close() was automatically called for all systems connected due to ID_FCM2_VERIFIED_JOIN_START
        /// Programmer should inform the player via the UI that they cannot join this session, and to choose a different session
        /// \note Server normally sends us this message, however if connection to the server was lost, message will be returned locally
        /// \note Only the designated client gets this message
        ID_FCM2_VERIFIED_JOIN_FAILED,
        /// The system that called StartVerifiedJoin() got ID_FCM2_VERIFIED_JOIN_CAPABLE from the client and then called RespondOnVerifiedJoinCapable() with true
        /// AddParticipant() has automatically been called for this system
        /// Use GetVerifiedJoinAcceptedAdditionalData() to read any additional data passed to RespondOnVerifiedJoinCapable()
        /// \note All systems in the mesh get this message
        /// \sa RespondOnVerifiedJoinCapable()
        ID_FCM2_VERIFIED_JOIN_ACCEPTED,
        /// The system that called StartVerifiedJoin() got ID_FCM2_VERIFIED_JOIN_CAPABLE from the client and then called RespondOnVerifiedJoinCapable() with false
        /// close() has been automatically called for each system connected to since ID_FCM2_VERIFIED_JOIN_START.
        /// The connection is NOT automatically closed to the original host that sent StartVerifiedJoin()
        /// Use GetVerifiedJoinRejectedAdditionalData() to read any additional data passed to RespondOnVerifiedJoinCapable()
        /// \note Only the designated client gets this message
        /// \sa RespondOnVerifiedJoinCapable()
        ID_FCM2_VERIFIED_JOIN_REJECTED,

        /// UDP proxy messages. Second byte indicates type.
        ID_UDP_PROXY_GENERAL,

        /// SQLite3Plugin - execute
        ID_SQLite3_EXEC,
        /// SQLite3Plugin - Remote database is unknown
        ID_SQLite3_UNKNOWN_DB,
        /// Events happening with SQLiteClientLoggerPlugin
        ID_SQLLITE_LOGGER,

        /// Sent to NatTypeDetectionServer
        ID_NAT_TYPE_DETECTION_REQUEST,
        /// Sent to NatTypeDetectionClient. Byte 1 contains the type of NAT detected.
        ID_NAT_TYPE_DETECTION_RESULT,

        /// Used by the router2 plugin
        ID_ROUTER_2_INTERNAL,
        /// No path is available or can be established to the remote system
        /// SocketMessage::mID contains the endpoint guid that we were trying to reach
        ID_ROUTER_2_FORWARDING_NO_PATH,
        /// \brief You can now call connect, ping, or other operations to the destination system.
        ///
        /// Connect as follows:
        ///
        /// NetSerializer bs(packet->mData, packet->mLength, false);
        /// bs.skipRead(sizeof(Token), 0);
        /// UniqueID endpointGuid;
        /// bs.read(endpointGuid);
        /// Nui16 sourceToDestPort;
        /// bs.read(sourceToDestPort);
        /// String ipAddressString;
        /// packet->mAddress.read(ipAddressString, false);
        /// linkbase->Connect(ipAddressString, sourceToDestPort, 0,0);
        ID_ROUTER_2_FORWARDING_ESTABLISHED,
        /// The IP address for a forwarded connection has changed
        /// read endpointGuid and port as per ID_ROUTER_2_FORWARDING_ESTABLISHED
        ID_ROUTER_2_REROUTED,

        /// \internal Used by the team balancer plugin
        ID_TEAM_BALANCER_INTERNAL,
        /// Cannot switch to the desired team because it is full. However, if someone on that team leaves, you will
        ///  get ID_TEAM_BALANCER_TEAM_ASSIGNED later.
        /// For TeamBalancer: Byte 1 contains the team you requested to join. Following bytes contain NetworkID of which member
        ID_TEAM_BALANCER_REQUESTED_TEAM_FULL,
        /// Cannot switch to the desired team because all teams are locked. However, if someone on that team leaves,
        ///  you will get ID_TEAM_BALANCER_SET_TEAM later.
        /// For TeamBalancer: Byte 1 contains the team you requested to join.
        ID_TEAM_BALANCER_REQUESTED_TEAM_LOCKED,
        ID_TEAM_BALANCER_TEAM_REQUESTED_CANCELLED,
        /// Team balancer plugin informing you of your team. Byte 1 contains the team you requested to join. Following bytes contain NetworkID of which member.
        ID_TEAM_BALANCER_TEAM_ASSIGNED,

        /// Gamebryo Lightspeed integration
        ID_LIGHTSPEED_INTEGRATION,

        /// XBOX integration
        ID_XBOX_LOBBY,

        /// The password we used to challenge the other system passed, meaning the other system has called AuthFilter::AddPassword() with the same password we passed to AuthFilter::Challenge()
        /// You can read the identifier used to challenge as follows:
        /// NetSerializer bs(packet->mData, packet->mLength, false); bs.skipRead(sizeof(Token), 0); RakString password; bs.read(password);
        ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_SUCCESS,
        ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_SUCCESS,
        /// A remote system sent us a challenge using AuthFilter::Challenge(), and the challenge failed.
        /// If the other system must pass the challenge to stay connected, you should call NetworkSys::close() to terminate the connection to the other system.
        ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_FAILURE,
        /// The other system did not add the password we used to AuthFilter::AddPassword()
        /// You can read the identifier used to challenge as follows:
        /// NetSerializer bs(packet->mData, packet->mLength, false); bs.skipRead(sizeof(Token), 0); RakString password; bs.read(password);
        ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_FAILURE,
        /// The other system did not respond within a timeout threshhold. Either the other system is not running the plugin or the other system was blocking on some operation for a long time.
        /// You can read the identifier used to challenge as follows:
        /// NetSerializer bs(packet->mData, packet->mLength, false); bs.skipRead(sizeof(Token), 0); RakString password; bs.read(password);
        ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_TIMEOUT,
        /// \internal
        ID_TWO_WAY_AUTHENTICATION_NEGOTIATION,

        /// CloudClient / CloudServer
        ID_CLOUD_POST_REQUEST,
        ID_CLOUD_RELEASE_REQUEST,
        ID_CLOUD_GET_REQUEST,
        ID_CLOUD_GET_RESPONSE,
        ID_CLOUD_UNSUBSCRIBE_REQUEST,
        ID_CLOUD_SERVER_TO_SERVER_COMMAND,
        ID_CLOUD_SUBSCRIPTION_NOTIFICATION,

        // LibVoice
        ID_LIB_VOICE,

        ID_RELAY_PLUGIN,
        ID_NAT_REQUEST_BOUND_ADDRESSES,
        ID_NAT_RESPOND_BOUND_ADDRESSES,
        ID_RESERVED_2,
        ID_RESERVED_3,
        ID_RESERVED_4,
        ID_RESERVED_5,
        ID_RESERVED_6,
        ID_RESERVED_7,
        ID_RESERVED_8,
        ID_RESERVED_9,

        // For the user to use.  Start your first enumeration at this value.
        ID_USER_PACKET_ENUM
        //-------------------------------------------------------------------------------------------------------------

    };
    
    /// These enumerations are used to describe when packets are delivered.
    enum PacketLevel
    {
        PL_RAPID,
        /// For every 2 PL_RAPID messages, 1 PL_HIGH will be sent.
        /// Messages at this plevel and lower are buffered to be sent in groups at 10 millisecond intervals to reduce UDP overhead and better measure congestion control.
        PL_HIGH,

        /// For every 2 PL_HIGH messages, 1 PL_MEDIUM will be sent.
        /// Messages at this plevel and lower are buffered to be sent in groups at 10 millisecond intervals to reduce UDP overhead and better measure congestion control.
        PL_MEDIUM,

        /// For every 2 PL_MEDIUM messages, 1 PL_LOW will be sent.
        /// Messages at this plevel and lower are buffered to be sent in groups at 10 millisecond intervals to reduce UDP overhead and better measure congestion control.
        PL_LOW,
        PL_COUNT
    };
    
    /// These enumerations are used to describe how packets are delivered.
    /// \note  Note to self: I write this with 3 bits in the stream.  If I add more remember to change that
    /// \note In PacketPrc::write I assume there are 5 major types
    /// \note Do not reorder, I check on >= PT_RECEIPT_WILL_LOST
    enum PacketType
    {
        /** Same as regular UDP, except that it will also discard duplicate datagrams.
            RakNet adds (6 to 17) + 21 bits of overhead, 16 of which is used to detect
            duplicate packets and 6 to 17 of which is used for message length.
        */
        PT_WILL_LOST,

        /** Regular UDP with a sequence counter.  Out of order messages will be discarded.
            Sequenced and ordered messages sent on the same channel will arrive in the
            order sent.
        */
        PT_ONE_WILL_LOST,

        /** The message is sent reliably, but not necessarily in any order.
            Same overhead as PT_WILL_LOST.
        */
        PT_BE_SURE,

        /** This message is reliable and will arrive in the order you sent it.
            Messages will be delayed while waiting for out of order messages.
            Same overhead as PT_ONE_WILL_LOST. Sequenced and ordered messages
            sent on the same channel will arrive in the order sent.
        */
        PT_ORDER_MAKE_SURE,

        /** This message is reliable and will arrive in the sequence you sent it.
            Out or order messages will be dropped.  Same overhead as PT_ONE_WILL_LOST.
            Sequenced and ordered messages sent on the same channel will arrive in the order sent.
        */
        PT_ONE_MAKE_SURE,

        /** Same as PT_WILL_LOST, however the user will get either ID_SND_RECEIPT_ACKED
            or ID_SND_RECEIPT_LOSS based on the result of sending this message when
            calling LinkBase::receive(). Bytes 1-4 will contain the number returned from
            the send() function. On disconnect or shutdown, all messages not previously
            acked should be considered lost.
        */
        PT_RECEIPT_WILL_LOST,

        /** Same as PT_BE_SURE. The user will also get ID_SND_RECEIPT_ACKED after the
            message is delivered when calling LinkBase::receive(). ID_SND_RECEIPT_ACKED
            is returned when the message arrives, not necessarily the order when it was
            sent. Bytes 1-4 will contain the number returned from the send() function.
            On disconnect or shutdown, all messages not previously acked should be
            considered lost. This does not return ID_SND_RECEIPT_LOSS.
        */
        PT_RECEIPT_MAKE_SURE,

        /** Same as RELIABLE_ORDERED_ACK_RECEIPT. The user will also get
            ID_SND_RECEIPT_ACKED after the message is delivered when calling
            LinkBase::receive(). ID_SND_RECEIPT_ACKED is returned when the message
            arrives, not necessarily the order when it was sent. Bytes 1-4 will contain
            the number returned from the send() function. On disconnect or shutdown,
            all messages not previously acked should be considered lost. This does
            not return ID_SND_RECEIPT_LOSS.
        */
        PT_ORDER_RECEIPT_MAKE_SURE,
        PT_COUNT
    };
    
#if N_PLAT == N_PLAT_WIN32
	#include <conio.h> /* getche() */
#else
	#include <termios.h>
	char getche();
#endif

	char * Gets(char * str, int num);

    /// Generates and validates checksums
    class CheckSum
    {
    public:
        /// Default constructor
        CheckSum()
        {
            Clear();
        }

        void Clear()
        {
            sum = 0;
            r = 55665;
            c1 = 52845;
            c2 = 22719;
        }

        void Add(Nui32 w);

        void Add(Nui16 w);

        void Add(Nui8 * b, Nui32 length);

        void Add(Nui8 b);

        Nui32 Get ()
        {
            return sum;
        }

    protected:
        Nui16 r;
        Nui16 c1;
        Nui16 c2;
        Nui32 sum;
    };

    ///
    class _EngineAPI SocketUtil
    {
    public:
        SocketUtil();
        ~SocketUtil();

        struct SendToThreadBlock
        {
            Nsocket s;
            Address * mAddress;
            Nui32 extraSocketOptions;
            Nui16 dataWriteOffset;
            char mData[N_Net_MaxMtuSize];
        };

        static SendToThreadBlock * AllocateBlock(void);
        
        static void ProcessBlock(SendToThreadBlock * threadedSend);

        static void AddRef(void);
        
        static void Deref(void);
        
        static DataStructures::ThreadsafeAllocatingQueue<SendToThreadBlock> objectQueue;

        static void getLocalHost(AddressList & out);

        static Nui16 GetLocalPort(Nudpsocket s);
        
        static void GetSystemAddress_Old (Nudpsocket s, Address *& address);
        
        static void GetSystemAddress(Nudpsocket s, Address *& address);
        
        /**
        */
        static RakString GetSubNetForSocketAndIp(Nudpsocket inSock, RakString inIpString);

        // AF_INET (default). For IPV6, use AF_INET6. To autoselect, use AF_UNSPEC.
        static bool GetFirstBindableIP(char firstBindable[128], int ipProto);
    protected:
        static int mRefCount;
        static ThreadManager<SendToThreadBlock *,SendToThreadBlock *> threadPool;
    };
    
    enum DynDnsResultCode
    {
        // ----- Success -----
        RC_SUCCESS,
        RC_DNS_ALREADY_SET, // RakNet detects no action is needed

        // ----- Ignorable failure (treat same as success) -----
        RC_NO_CHANGE, // DynDNS detects no action is needed (treated as abuse though)

        // ----- User error -----
        RC_NOT_DONATOR, // You have to pay to do this
        RC_NO_HOST, // This host does not exist at all
        RC_BAD_AUTH, // You set the wrong password
        RC_NOT_YOURS, // This is not your host

        // ----- Permanent failure -----
        RC_ABUSE,                               // Your host has been blocked, too many failures disable your account
        RC_TCP_FAILED_TO_START,                 // TCP port already in use
        RC_TCP_DID_NOT_CONNECT,                 // DynDNS down?
        RC_UNKNOWN_RESULT,                      // DynDNS returned a result code that was not documented as of 12/4/2010 on http://www.dyndns.com/developers/specs/flow.pdf
        RC_PARSING_FAILURE,                     // Can't read the result returned, format change?
        RC_CONNECTION_LOST_WITHOUT_RESPONSE,    // Lost the connection to DynDNS while communicating
        RC_BAD_AGENT,                           // ???
        RC_BAD_SYS,                             // ???
        RC_DNS_ERROR,                           // ???
        RC_NOT_FQDN,                            // ???
        RC_NUM_HOST,                            // ???
        RC_911,                                 // ???  
        RC_DYNDNS_TIMEOUT                       // DynDNS did not respond
    };

    // Can only process one at a time with the current implementation
    class _EngineAPI DynDNS
    {
    public:
        enum ConnectPhase
        {
            CP_CONNECTING_TO_CHECKIP,
            CP_WAITING_FOR_CHECKIP_RESPONSE,
            CP_CONNECTING_TO_DYNDNS,
            CP_WAITING_FOR_DYNDNS_RESPONSE,
            CP_IDLE
        };
    public:
        DynDNS();
        ~DynDNS();

        // Pass 0 for newIPAddress to autodetect whatever you are uploading from
        // usernameAndPassword should be in the format username:password
        void UpdateHostIPAsynch(const char * dnsHost, const char * newIPAddress, const char * usernameAndPassword);
        
        void update();

        // Output
        bool IsRunning() const { return connectPhase != CP_IDLE;}
        
        bool IsCompleted() const { return connectPhase == CP_IDLE;}
        
        DynDnsResultCode GetCompletedResultCode() const {return result;}
        
        const char * GetCompletedDescription() const {return resultDescription;}
        
        bool WasResultSuccessful() const {return result == RC_SUCCESS || result == RC_DNS_ALREADY_SET || result == RC_NO_CHANGE;}
        
        const String & GetMyPublicIP() const {return myIPStr;} // We get our public IP as part of the process. This is valid once completed
    protected:
        void Stop(void);
        void SetCompleted(DynDnsResultCode _result, const char *_resultDescription) {Stop(); result=_result; resultDescription=_resultDescription;}
    protected:
        NetworkObj * tcp;
        RakString getString;
        Address * serverAddress;
        ConnectPhase connectPhase;
        RakString host;
        TimeDurMS phaseTimeout;
        Address * checkIpAddress;
        const char *resultDescription;
        DynDnsResultCode result;
        String myIPStr;
    };
    
    enum StartupResult
    {
        RAKNET_STARTED,
        RAKNET_ALREADY_STARTED,
        SOCKET_FAMILY_NOT_SUPPORTED,
        SOCKET_PORT_ALREADY_IN_USE,
        SOCKET_FAILED_TO_BIND,
        SOCKET_FAILED_TEST_SEND,
        PORT_CANNOT_BE_ZERO,
        FAILED_TO_CREATE_NETWORK_THREAD,
        COULD_NOT_GENERATE_GUID,
        STARTUP_OTHER_FAILURE
    };

    enum ConnectResultType
    {
        CONNECTION_ATTEMPT_STARTED,
        INVALID_PARAMETER,
        ALREADY_CONNECTED_TO_ENDPOINT,
        CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS,
        SECURITY_INITIALIZATION_FAILED
    };

    /// Returned from LinkBase::getState()
    enum ConnectionState
    {
        /// connect() was called, but the process hasn't started yet
        IS_PENDING,
        /// Processing the connection attempt
        IS_CONNECTING,
        /// Is connected and able to communicate
        IS_CONNECTED,
        /// Was connected, but will disconnect as soon as the remaining messages are delivered
        IS_DISCONNECTING,
        /// A connection attempt failed and will be aborted
        IS_SILENTLY_DISCONNECTING,
        /// No longer connected
        IS_DISCONNECTED,
        /// Was never connected, or else was disconnected long enough ago that the entry has been discarded
        IS_NOT_CONNECTED
    };

    const int MAX_RPC_MAP_SIZE = ((Nui8)-1)-1;
    const int UNDEFINED_RPC_INDEX = ((Nui8)-1);

    /// Used with the PublicKey structure
    enum PublicKeyMode
    {
        /// The connection is insecure. You can also just pass 0 for the pointer to PublicKey in LinkBase::connect()
        PKM_INSECURE_CONNECTION,

        /// Accept whatever public key the server gives us. This is vulnerable to man in the middle, but does not require
        /// distribution of the public key in advance of connecting.
        PKM_ACCEPT_ANY_PUBLIC_KEY,

        /// Use a known remote server public key. PublicKey::remoteServerPublicKey must be non-zero.
        /// This is the recommended mode for secure connections.
        PKM_USE_KNOWN_PUBLIC_KEY,

        /// Use a known remote server public key AND provide a public key for the connecting client.
        /// PublicKey::remoteServerPublicKey, myPublicKey and myPrivateKey must be all be non-zero.
        /// The server must cooperate for this mode to work.
        /// I recommend not using this mode except for server-to-server communication as it significantly increases the CPU requirements during connections for both sides.
        /// Furthermore, when it is used, a connection password should be used as well to avoid DoS attacks.
        PKM_USE_TWO_WAY_AUTHENTICATION
    };

    /// Passed to LinkBase::connect()
    struct _EngineAPI PublicKey
    {
        /// How to interpret the public key, see above
        PublicKeyMode publicKeyMode;

        /// Pointer to a public key of length cat::EasyHandshake::PUBLIC_KEY_BYTES. See the Encryption sample.
        char * remoteServerPublicKey;

        /// (Optional) Pointer to a public key of length cat::EasyHandshake::PUBLIC_KEY_BYTES
        char * myPublicKey;

        /// (Optional) Pointer to a private key of length cat::EasyHandshake::PRIVATE_KEY_BYTES
        char * myPrivateKey;
    };

    /// Describes the local socket to use for NetworkSys::init
    struct _EngineAPI HostPeer
    {
        HostPeer();
        HostPeer(Nui16 port, const String & host);

        /// The local port to bind to.  Pass 0 to have the OS autoassign a port.
        Nui16 port;

        /// The local network card address to bind to, such as "127.0.0.1".  Pass an empty string to use INADDR_ANY.
        char hostAddress[32];

        /// IP version: For IPV4, use AF_INET (default). For IPV6, use AF_INET6. To autoselect, use AF_UNSPEC.
        /// IPV6 is the newer internet protocol. Instead of addresses such as natpunch.jenkinssoftware.com, you may have an address such as fe80::7c:31f7:fec4:27de%14.
        /// Encoding takes 16 bytes instead of 4, so IPV6 is less efficient for bandwidth.
        /// On the positive side, NAT Punchthrough is not needed and should not be used with IPV6 because there are enough addresses that routers do not need to create address mappings.
        /// NetworkSys::init() will fail if this IP version is not supported.
        /// \pre N_NET_PROTOCOL_VER must be set to 6 for AF_INET6
        short socketFamily;

        NPP_Instance chromeInstance;

        // Set to true to use a blocking socket (default, do not change unless you have a reason to)
        bool blockingSocket;

        /// XBOX only: set IPPROTO_VDP if you want to use VDP. If enabled, this socket does not support broadcast to 255.255.255.255
        Nui32 extraSocketOptions;
    };
    typedef vector<HostPeer *>::type HostPeerList;

    extern bool NonNumericHostString(const String & host);

    typedef Nui64 NetworkID;

    ///  Index of an unassigned player
    const Nindex UNASSIGNED_PLAYER_INDEX = 65535;

    /// Unassigned object ID
    const NetworkID UNASSIGNED_NETWORK_ID = (Nui64) -1;

    struct _EngineAPI uint24_t
    {
        Nui32 val;

        uint24_t() {}
        inline operator Nui32() { return val; }
        inline operator Nui32() const { return val; }

        inline uint24_t(const uint24_t & a) {val = a.val;}
        inline uint24_t operator++() {++val; val &= 0x00FFFFFF; return *this;}
        inline uint24_t operator--() {--val; val &= 0x00FFFFFF; return *this;}
        inline uint24_t operator++(int) {uint24_t temp(val); ++val; val &= 0x00FFFFFF; return temp;}
        inline uint24_t operator--(int) {uint24_t temp(val); --val; val &= 0x00FFFFFF; return temp;}
        inline uint24_t operator&(const uint24_t & a) {return uint24_t(val&a.val);}
        inline uint24_t & operator=(const uint24_t & a) { val = a.val; return *this; }
        inline uint24_t & operator+=(const uint24_t & a) { val += a.val; val &= 0x00FFFFFF; return *this; }
        inline uint24_t & operator-=(const uint24_t & a) { val -= a.val; val &= 0x00FFFFFF; return *this; }
        inline bool operator==(const uint24_t & right) const {return val == right.val;}
        inline bool operator!=(const uint24_t & right) const {return val != right.val;}
        inline bool operator > (const uint24_t & right) const {return val > right.val;}
        inline bool operator < (const uint24_t & right) const {return val < right.val;}
        inline const uint24_t operator+(const uint24_t & other) const { return uint24_t(val+other.val); }
        inline const uint24_t operator-(const uint24_t & other) const { return uint24_t(val-other.val); }
        inline const uint24_t operator/(const uint24_t & other) const { return uint24_t(val/other.val); }
        inline const uint24_t operator*(const uint24_t & other) const { return uint24_t(val*other.val); }

        inline uint24_t(Nui32 a) {val=a; val&=0x00FFFFFF;}
        inline uint24_t operator & (Nui32 a) {return uint24_t(val&a);}
        inline uint24_t & operator=(Nui32 a) { val=a; val&=0x00FFFFFF; return *this; }
        inline uint24_t & operator+=(Nui32 a) { val+=a; val&=0x00FFFFFF; return *this; }
        inline uint24_t & operator-=(Nui32 a) { val-=a; val&=0x00FFFFFF; return *this; }
        inline bool operator == (Nui32 r) const {return val==(r&0x00FFFFFF);}
        inline bool operator != (Nui32 r) const {return val!=(r&0x00FFFFFF);}
        inline bool operator > (Nui32 r) const {return val>(r&0x00FFFFFF);}
        inline bool operator < (Nui32 r) const {return val<(r&0x00FFFFFF);}
        inline const uint24_t operator+(Nui32 o) const { return uint24_t(val+o); }
        inline const uint24_t operator-(Nui32 o) const { return uint24_t(val-o); }
        inline const uint24_t operator/(Nui32 o) const { return uint24_t(val/o); }
        inline const uint24_t operator*(Nui32 o) const { return uint24_t(val*o); }

        void read(const NetSerializer * in)
        {
            readAlign();
            if(mBitRead + 3 * 8 > mBitWrite)
                return false;

            if (isNetEndian() == false)
            {
                ((Nui8 *)&out.val)[0] = mData[(mBitRead >> 3) + 0];
                ((Nui8 *)&out.val)[1] = mData[(mBitRead >> 3) + 1];
                ((Nui8 *)&out.val)[2] = mData[(mBitRead >> 3) + 2];
                ((Nui8 *)&out.val)[3] = 0;
            }
            else
            {

                ((Nui8 *)&out.val)[3] = mData[(mBitRead >> 3) + 0];
                ((Nui8 *)&out.val)[2] = mData[(mBitRead >> 3) + 1];
                ((Nui8 *)&out.val)[1] = mData[(mBitRead >> 3) + 2];
                ((Nui8 *)&out.val)[0] = 0;
            }

            mBitRead += 3 * 8;
            return true;
        }
        void write(NetSerializer * out) const
        {
            writeAlign();
            enlargeBit(3 * 8);

            if (isNetEndian() == false)
            {
                mData[(mBitWrite >> 3) + 0] = ((Nui8 *)&in.val)[0];
                mData[(mBitWrite >> 3) + 1] = ((Nui8 *)&in.val)[1];
                mData[(mBitWrite >> 3) + 2] = ((Nui8 *)&in.val)[2];
            }
            else
            {
                mData[(mBitWrite >> 3) + 0] = ((Nui8 *)&in.val)[3];
                mData[(mBitWrite >> 3) + 1] = ((Nui8 *)&in.val)[2];
                mData[(mBitWrite >> 3) + 2] = ((Nui8 *)&in.val)[1];
            }

            mBitWrite += 3 * 8;
        }
    };
}
#endif