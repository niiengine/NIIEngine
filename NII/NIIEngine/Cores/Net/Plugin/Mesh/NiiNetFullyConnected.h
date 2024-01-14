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
#if _RAKNET_SUPPORT_FullyConnectedMesh2 == 1

#ifndef __FULLY_CONNECTED_MESH_2_H
#define __FULLY_CONNECTED_MESH_2_H

#include "NiiNetPreInclude.h"
#include "NiiNetPlugin.h"
#include "DS_List.h"
#include "RakString.h"

typedef Ni64 FCM2Guid;

namespace NII
{
namespace NII_NET
{
    /// \brief Fully connected mesh plugin, revision 2
    /// \details This will connect NetworkSys to all connecting peers, and all peers the connecting peer knows about.<BR>
    /// It will also calculate which system has been running longest, to find out who should be host, if you need one system to act as a host
    /// \pre You must also install the LinkState plugin in order to use SetConnectOnNewRemoteConnection()
    /// \ingroup FULLY_CONNECTED_MESH_GROUP
    class _EngineAPI FullyConnected : public Plugin
    {
    public:
        FullyConnected();
        virtual ~FullyConnected();

        /// When the message ID_REMOTE_NEW_INCOMING_CONNECTION arrives, we try to connect to that system
        /// If \a attemptConnection is false, you can manually connect to all systems listed in ID_REMOTE_NEW_INCOMING_CONNECTION with ConnectToRemoteNewIncomingConnections()
        /// \note This will not work on any console. It will also not work if NAT punchthrough is needed. Generally, this should be false and you should connect manually. It is here for legacy reasons.
        /// \param[in] attemptConnection If true, we try to connect to any systems we are notified about with ID_REMOTE_NEW_INCOMING_CONNECTION, which comes from the LinkState plugin. Defaults to true.
        /// \param[in] pw The password to use to connect with. Only used if \a attemptConnection is true
        void SetConnectOnNewRemoteConnection(bool attemptConnection, RakString pw);

        /// \brief The connected host is whichever system we are connected to that has been running the longest.
        /// \details Will return UniqueID::INVALID if we are not connected to anyone, or if we are connected and are calculating the host
        /// If includeCalculating is true, will return the estimated calculated host as long as the calculation is nearly complete
        /// includeCalculating should be true if you are taking action based on another system becoming host, because not all host calculations may complete at the exact same time
        /// \sa LinkState::GetLowestAveragePingSystem() . If you need one system in the peer to peer group to relay data, have the host call this function after host migration, and use that system
        /// \return System address of whichever system is host.
        UniqueID GetConnectedHost(void) const;
        Address * GetConnectedHostAddr(void) const;

        /// \return System address of whichever system is host. Always returns something, even though it may be our own system.
        UniqueID GetHostSystem(void) const;

        /// \return If our system is host
        bool IsHostSystem(void) const;

        /// Get the list of connected systems, from oldest connected to newest
        /// This is also the order that the hosts will be chosen in
        void GetHostOrder(DataStructures::List<UniqueID> & hostList);

        /// \param[in] includeCalculating If true, and we are currently calculating a new host, return the new host if the calculation is nearly complete
        /// \return If our system is host
        bool IsConnectedHost(void) const;

        /// \brief Automatically add new connections to the fully connected mesh.
        /// Each remote system that you want to check should be added as a participant, either through SetAutoparticipateConnections() or by calling this function
        /// \details Defaults to true.
        /// \param[in] b As stated
        void SetAutoparticipateConnections(bool b);

        /// Clear our own host order, and recalculate as if we had just reconnected
        /// Call this to reset the running time of the host just before joining/creating a game room for networking
        void ResetHostCalculation(void);

        /// \brief if SetAutoparticipateConnections() is called with false, then you need to use AddParticipant before these systems will be added to the mesh
        /// FullyConnected will track who is the who host among a fully connected mesh of participants
        /// Each remote system that you want to check should be added as a participant, either through SetAutoparticipateConnections() or by calling this function
        /// \param[in] participant The new participant
        /// \sa StartVerifiedJoin()
        void AddParticipant(UniqueID id);

        /// Get the participants added with AddParticipant()
        /// \param[out] participantList Participants added with AddParticipant();
        void GetParticipantList(DataStructures::List<UniqueID> &participantList);

        /// \brief Returns if a participant is in the participant list
        /// \param[in] UniqueID of the participant to query
        /// \return True if in the list
        bool HasParticipant(UniqueID participantGuid);

        /// connect to all systems from ID_REMOTE_NEW_INCOMING_CONNECTION
        /// You can call this if SetConnectOnNewRemoteConnection is false
        /// \param[in] msg The msg containing ID_REMOTE_NEW_INCOMING_CONNECTION
        /// \param[in] connectionPassword Password passed to LinkBase::connect()
        /// \param[in] connectionPasswordLength Password length passed to LinkBase::connect()
        void ConnectToRemoteNewIncomingConnections(SocketMessage * msg);

        /// \brief Clear all memory and reset everything
        void Clear(void);

        Nui32 GetParticipantCount(void) const;
        void GetParticipantCount(Nui32 * participantListSize) const;

        /// In the simple case of forming a peer to peer mesh:
        ///
        /// 1. AddParticipant() is called on the host whenever you get a new connection
        /// 2. The host sends all participants to the new client
        /// 3. The client connects to the participant list
        ///
        /// However, the above steps assumes connections to all systems in the mesh always complete.
        /// When there is a risk of failure, such as if relying on NATPunchthroughClient, you may not want to call AddParticipant() until are connections have completed to all other particpants
        /// StartVerifiedJoin() can manage the overhead of the negotiation involved so the programmer only has to deal with overall success or failure
        ///
        /// Processing:
        /// 1. send the UniqueID and Address values of GetParticipantList() to the client with ID_FCM2_VERIFIED_JOIN_START
        /// 2. The client, on ID_FCM2_VERIFIED_JOIN_START, can execute RouterMapClient::OpenNAT() (optional), followed by LinkBase::connect() if punchthrough success, for each system returned from GetVerifiedJoinRequiredProcessingList()
        /// 3. After all participants in step 2 have connected, failed to connect, or failed NatPunchthrough, the client automatically sends the results to the server.
        /// 4. The server compares the results of the operations in step 2 with the values from GetParticpantList().
        /// 4A. If the client failed to connect to a current participant, return ID_FCM2_VERIFIED_JOIN_FAILED to the client. close() is automatically called on the client for the failed participants.
        /// 4B. If AddParticipant() was called between steps 1 and 4, go back to step 1, transmitting new participants.
        /// 4C. If the client successfully connected to all participants, the server gets ID_FCM2_VERIFIED_JOIN_CAPABLE. The server programmer, on the same frame, should execute RespondOnVerifiedJoinCapable() to either accept or reject the client.
        /// 5. If the client got ID_FCM2_VERIFIED_JOIN_ACCEPTED, AddParticipant() is automatically called for each system in the mesh.
        /// 6. If the client got ID_FCM2_VERIFIED_JOIN_REJECTED, close() is automatically called for each system in the mesh. The connection is NOT automatically closed to the original host that sent StartVerifiedJoin().
        /// 7. If the client's connection to the server was lost before getting ID_FCM2_VERIFIED_JOIN_ACCEPTED or ID_FCM2_VERIFIED_JOIN_REJECTED, return to the programmer ID_FCM2_VERIFIED_JOIN_FAILED and call LinkBase::close()
        ///
        /// \brief Notify the client of GetParticipantList() in order to connect to each of those systems until the mesh has been completed
        /// \param[in] client The system to send ID_FCM2_VERIFIED_JOIN_START to
        virtual void StartVerifiedJoin(UniqueID client);

        /// \brief On ID_FCM2_VERIFIED_JOIN_CAPABLE , accept or reject the new connection
        /// \code
        /// fullyConnectedMesh->RespondOnVerifiedJoinCapable(msg, true, 0);
        /// \endcode
        /// \param[in] msg The system that sent ID_FCM2_VERIFIED_JOIN_CAPABLE. Based on \accept, ID_FCM2_VERIFIED_JOIN_ACCEPTED or ID_FCM2_VERIFIED_JOIN_REJECTED will be sent in reply
        /// \param[in] accept True to accept, and thereby automatically call AddParticipant() on all systems on the mesh. False to reject, and call close() to all mesh systems on the target
        /// \param[in] additionalData Any additional data you want to add to the ID_FCM2_VERIFIED_JOIN_ACCEPTED or ID_FCM2_VERIFIED_JOIN_REJECTED messages
        /// \sa WriteVJCUserData()
        virtual void RespondOnVerifiedJoinCapable(SocketMessage * msg, bool accept, NetSerializer * additionalData);

        /// \brief On ID_FCM2_VERIFIED_JOIN_START, read the Address and UniqueID values of each system to connect to
        /// \code
        /// DataStructures::List<Address *> addresses;
        /// DataStructures::List<UniqueID> guids;
        /// fullyConnectedMesh->GetVerifiedJoinRequiredProcessingList(msg->mID, addresses, guids);
        /// for(Nui32 i = 0; i < addresses.Size(); ++i)
        /// {
        ///     String temp; 
        ///     addresses[i].read(temp, false);
        ///     mLink[i]->connect(temp, addresses[i].getHSBPort(), 0, 0);
        /// }
        /// \endcode
        /// \param[in] host Which system sent ID_FCM2_VERIFIED_JOIN_START
        /// \param[out] addresses Address values of systems to connect to. List has the same number and order as \a guids
        /// \param[out] guids UniqueID values of systems to connect to. List has the same number and order as \a guids
        virtual void GetVerifiedJoinRequiredProcessingList(UniqueID host, DataStructures::List<Address *> &addresses, DataStructures::List<UniqueID> &guids);

        /// \brief On ID_FCM2_VERIFIED_JOIN_ACCEPTED, read additional data passed to RespondOnVerifiedJoinCapable()
        /// \code
        /// bool thisSystemAccepted;
        /// DataStructures::List<UniqueID> systemsAccepted;
        /// NetSerializer additionalData;
        /// fullyConnectedMesh->GetVerifiedJoinAcceptedAdditionalData(msg, &thisSystemAccepted, systemsAccepted, &additionalData);
        /// \endcode
        /// \param[in] msg SocketMessage containing the ID_FCM2_VERIFIED_JOIN_ACCEPTED message
        /// \param[out] thisSystemAccepted If true, it was this instance of LinkBase that was accepted. If false, this is notification for another system
        /// \param[out] systemsAccepted Which system(s) were added with AddParticipant(). If \a thisSystemAccepted is false, this list will only have length 1
        /// \param[out] additionalData \a additionalData parameter passed to RespondOnVerifiedJoinCapable()
        virtual void GetVerifiedJoinAcceptedAdditionalData(SocketMessage * msg,
            bool * thisSystemAccepted, DataStructures::List<UniqueID> & systemsAccepted,
                NetSerializer * additionalData);

        /// \brief On ID_FCM2_VERIFIED_JOIN_REJECTED, read additional data passed to RespondOnVerifiedJoinCapable()
        /// \details This does not automatically close the connection. The following code will do so:
        /// \code
        /// mLink[i]->close(msg->mID, true);
        /// \endcode
        /// \param[in] msg SocketMessage containing the ID_FCM2_VERIFIED_JOIN_REJECTED message
        /// \param[out] additionalData \a additionalData parameter passed to RespondOnVerifiedJoinCapable().
        virtual void GetVerifiedJoinRejectedAdditionalData(SocketMessage * msg, NetSerializer * additionalData);

        /// Override to write data when ID_VERIFIED_JOIN_CAPABLE is sent
        virtual void WriteVJCUserData(NetSerializer * bsOut) {(void) bsOut;}
        /// Use to read data written from WriteVJCUserData()
        /// \code
        /// NetSerializer bsIn(msg->mData,msg->mLength,false);
        /// FullyConnected::SkipToVJCUserData(&bsIn);
        /// // Your code here
        static void SkipToVJCUserData(NetSerializer * bsIn);

        /// \internal
        TimeDurUS GetElapsedRuntime();
        
        /// @copydetails Plugin::init
        virtual void init();
        /// @copydetails Plugin::shutdown
        virtual void shutdown();
        /// @copydetails Plugin::onMessage
        virtual ConnectReceiveType onMessage(SocketMessage * msg);        
        /// @copydetails Plugin::onAttach
        virtual void onAttach();
        /// @copydetails Plugin::onConnectClose
        virtual void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason );
        /// @copydetails Plugin::onConnect
        virtual void onConnect(const Address & address, UniqueID id);
        /// @copydetails Plugin::onConnectFail
        virtual void onConnectFail(SocketMessage * msg, ConnectFailType failedConnectionAttemptReason);

        /// \internal
        struct FCM2Participant
        {
            FCM2Participant() {}
            FCM2Participant(const FCM2Guid &_fcm2Guid, const UniqueID & id) :
                fcm2Guid(_fcm2Guid),
                mID(id) {}

            // Low half is a random number.
            // High half is the order we connected in (totalConnectionCount)
            FCM2Guid fcm2Guid;
            UniqueID mID;
        };

        enum JoinInProgressState
        {
            JIPS_PROCESSING,
            JIPS_FAILED,
            JIPS_CONNECTED,
            JIPS_UNNECESSARY,
        };

        struct VerifiedJoinInProgressMember
        {
            Address * mAddress;
            UniqueID mID;
            JoinInProgressState joinInProgressState;

            bool workingFlag;
        };

        /// \internal
        struct VerifiedJoinInProgress
        {
            UniqueID requester;
            DataStructures::List<VerifiedJoinInProgressMember> members;
            //bool sentResults;
        };

        /// \internal for debugging
        Nui32 GetTotalConnectionCount(void) const;

    protected:
        void PushNewHost(const UniqueID & id, UniqueID oldHost);
        void SendOurFCMGuid(Address * addr);
        void SendFCMGuidRequest(UniqueID id);
        void SendConnectionCountResponse(Address * addr, Nui32 responseTotalConnectionCount);
        void OnRequestFCMGuid(SocketMessage * msg);
        void OnRespondConnectionCount(SocketMessage * msg);
        void OnInformFCMGuid(SocketMessage * msg);
        void OnUpdateMinTotalConnectionCount(SocketMessage * msg);
        void AssignOurFCMGuid(void);
        void CalculateHost(UniqueID * id, FCM2Guid * fcm2Guid);
        bool AddParticipantInternal(UniqueID id, FCM2Guid theirFCMGuid);
        void CalculateAndPushHost(void);
        bool ParticipantListComplete(void);
        void IncrementTotalConnectionCount(Nui32 i);
        ConnectReceiveType OnVerifiedJoinStart(SocketMessage * msg);
        ConnectReceiveType OnVerifiedJoinCapable(SocketMessage * msg);
        virtual void OnVerifiedJoinFailed(UniqueID hostGuid, bool callCloseConnection);
        virtual void OnVerifiedJoinAccepted(SocketMessage * msg);
        virtual void OnVerifiedJoinRejected(SocketMessage * msg);
        Nui32 GetJoinsInProgressIndex(UniqueID requester) const;
        void UpdateVerifiedJoinInProgressMember(const UniqueAddress usys, UniqueID guidToAssign, JoinInProgressState newState);
        bool ProcessVerifiedJoinInProgressIfCompleted(VerifiedJoinInProgress * vjip);
        void ReadVerifiedJoinInProgressMember(NetSerializer * bsIn, VerifiedJoinInProgressMember *vjipm);
        Nui32 GetVerifiedJoinInProgressMemberIndex(const UniqueAddress usys, VerifiedJoinInProgress *vjip);
        void DecomposeJoinCapable(SocketMessage * msg, VerifiedJoinInProgress * vjip);
        void WriteVerifiedJoinCapable(NetSerializer * bsOut, VerifiedJoinInProgress *vjip);
        void CategorizeVJIP(VerifiedJoinInProgress * vjip,
            DataStructures::List<UniqueID> &participatingMembersOnClientSucceeded,
            DataStructures::List<UniqueID> &participatingMembersOnClientFailed,
            DataStructures::List<UniqueID> &participatingMembersNotOnClient,
            DataStructures::List<UniqueID> &clientMembersNotParticipatingSucceeded,
            DataStructures::List<UniqueID> &clientMembersNotParticipatingFailed);

        // Used to track how long RakNet has been running. This is so we know who has been running longest
        TimeDurUS startupTime;

        // Option for SetAutoparticipateConnections
        bool autoParticipateConnections;

        // totalConnectionCount is roughly maintained across all systems, and increments by 1 each time a new system connects to the mesh
        // It is always kept at the highest known value
        // It is used as the high 4 bytes for new FCMGuids. This causes newer values of FCM2Guid to be higher than lower values. The lowest value is the host.
        Nui32 totalConnectionCount;

        // Our own ourFCMGuid. Starts at unassigned (0). Assigned once we send ID_FCM2_REQUEST_FCMGUID and get back ID_FCM2_RESPOND_CONNECTION_COUNT
        FCM2Guid ourFCMGuid;

        /// List of systems we know the FCM2Guid for
        DataStructures::List<FCM2Participant> fcm2ParticipantList;

        UniqueID lastPushedHost;

        // Optimization: Store last calculated host in these variables.
        UniqueID hostRakNetGuid;
        FCM2Guid hostFCM2Guid;

        RakString connectionPassword;
        bool connectOnNewRemoteConnections;

        DataStructures::List<VerifiedJoinInProgress *> joinsInProgress;
    };
}
}

/*
init()
ourFCMGuid = unknown
totalConnectionCount = 0
Set startupTime

AddParticipant()
if (sender by guid is a participant)
return;
AddParticipantInternal(guid);
if (ourFCMGuid == unknown)
send to that system a request for their fcmGuid, totalConnectionCount. Inform startupTime.
else
send to that system a request for their fcmGuid. Inform total connection count, our fcmGuid

OnRequestGuid()
if (sender by guid is not a participant)
{
    // They added us as a participant, but we didn't add them. This can be caused by lag where both participants are not added at the same time.
    // It doesn't affect the outcome as long as we still process the data
    AddParticipantInternal(guid);
}
if (ourFCMGuid==unknown)
{
    if (includedStartupTime)
    {
        // Nobody has a fcmGuid

        if (their startup time is greater than our startup time)
            ReplyConnectionCount(1);
        else
            ReplyConnectionCount(2);
    }
    else
    {
        // They have a fcmGuid, we do not

        SetMaxTotalConnectionCount(remoteCount);
        AssignTheirGuid()
        GenerateOurGuid();
        SendOurGuid(all);
    }
}
else
{
    if (includedStartupTime)
    {
        // We have a fcmGuid they do not

        ReplyConnectionCount(totalConnectionCount+1);
        SendOurGuid(sender);
    }
    else
    {
        // We both have fcmGuids

        SetMaxTotalConnectionCount(remoteCount);
        AssignTheirGuid();
        SendOurGuid(sender);
    }
}

OnReplyConnectionCount()
SetMaxTotalConnectionCount(remoteCount);
GenerateOurGuid();
SendOurGuid(allParticipants);

OnReceiveTheirGuid()
AssignTheirGuid()
*/

#endif

#endif // _RAKNET_SUPPORT_*
