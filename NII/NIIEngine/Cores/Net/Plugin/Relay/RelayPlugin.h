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
#if _RAKNET_SUPPORT_RelayPlugin==1

#ifndef __RELAY_PLUGIN_H
#define __RELAY_PLUGIN_H

#include "NiiNetPlugin.h"
#include "RakString.h"
#include "DS_Hash.h"

/// \defgroup RELAY_PLUGIN_GROUP RelayPlugin
/// \brief A simple class to relay messages from one system to another through an intermediary
/// \ingroup PLUGINS_GROUP

namespace NII
{
namespace NII_NET
{
    enum RelayPluginEnums
    {
        // Server handled messages
        RPE_MESSAGE_TO_SERVER_FROM_CLIENT,
        RPE_ADD_CLIENT_REQUEST_FROM_CLIENT,
        RPE_REMOVE_CLIENT_REQUEST_FROM_CLIENT,
        RPE_GROUP_MESSAGE_FROM_CLIENT,
        RPE_JOIN_GROUP_REQUEST_FROM_CLIENT,
        RPE_LEAVE_GROUP_REQUEST_FROM_CLIENT,
        RPE_GET_GROUP_LIST_REQUEST_FROM_CLIENT,
        // Client handled messages
        RPE_MESSAGE_TO_CLIENT_FROM_SERVER,
        RPE_ADD_CLIENT_NOT_ALLOWED,
        RPE_ADD_CLIENT_TARGET_NOT_CONNECTED,
        RPE_ADD_CLIENT_NAME_ALREADY_IN_USE,
        RPE_ADD_CLIENT_SUCCESS,
        RPE_USER_ENTERED_ROOM,
        RPE_USER_LEFT_ROOM,
        RPE_GROUP_MSG_FROM_SERVER,
        RPE_GET_GROUP_LIST_REPLY_FROM_SERVER,
        RPE_JOIN_GROUP_SUCCESS,
        RPE_JOIN_GROUP_FAILURE,
    };

    /// \brief A simple class to relay messages from one system to another, identifying remote systems by a string.
    /// \ingroup RELAY_PLUGIN_GROUP
    class _EngineAPI RelayPlugin : public Plugin
    {
    public:
        /// Constructor
        RelayPlugin();

        /// Destructor
        virtual ~RelayPlugin();

        /// \brief Forward messages from any system, to the system specified by the combination of key and guid. The sending system only needs to know the key.
        /// \param[in] key A string to identify the target's UniqueID. This is so the sending system does not need to know the UniqueID of the target system. The key should be unique among all guids added. If the key is not unique, only one system will be sent to (at random).
        /// \param[in] id The RakNetGuid of the system to send to. If this system disconnects, it is removed from the internal hash
        /// \return RPE_ADD_CLIENT_TARGET_NOT_CONNECTED, RPE_ADD_CLIENT_NAME_ALREADY_IN_USE, or RPE_ADD_CLIENT_OK
        RelayPluginEnums AddParticipantOnServer(const RakString & key, const UniqueID & id);

        /// \brief Remove a chat participant
        void RemoveParticipantOnServer(const UniqueID & id);

        /// \brief If true, then if the client calls AddParticipantRequestFromClient(), the server will call AddParticipantOnServer() automatically
        /// Defaults to false
        /// \param[in] accept true to accept, false to not.
        void SetAcceptAddParticipantRequests(bool accept);

        /// \brief Request from the client for the server to call AddParticipantOnServer()
        /// \pre The server must have called SetAcceptAddParticipantRequests(true) or the request will be ignored
        /// \param[in] key A string to identify out system. Passed to \a key on AddParticipantOnServer()
        /// \param[in] relayPluginServerGuid the UniqueID of the system running RelayPlugin
        void AddParticipantRequestFromClient(const RakString &key, const UniqueID &relayPluginServerGuid);

        /// \brief Remove yourself as a participant
        void RemoveParticipantRequestFromClient(const UniqueID &relayPluginServerGuid);

        /// \brief Request that the server relay \a bitStream to the system designated by \a key
        /// \param[in] relayPluginServerGuid the UniqueID of the system running RelayPlugin
        /// \param[in] destinationGuid The key value passed to AddParticipant() earlier on the server. If this was not done, the server will not relay the message (it will be silently discarded).
        /// \param[in] bitStream The data to relay
        /// \param[in] plevel See the parameter of the same name in LinkBase::send()
        /// \param[in] ptype See the parameter of the same name in LinkBase::send()
        /// \param[in] pchannel See the parameter of the same name in LinkBase::send()
        void SendToParticipant(const UniqueID & relayPluginServerGuid, const RakString & destinationGuid, NetSerializer * bitStream, PacketLevel plevel, PacketType ptype, char pchannel);

        void SendGroupMessage(const UniqueID & relayPluginServerGuid, NetSerializer * bitStream, PacketLevel plevel, PacketType ptype, char pchannel);
        void JoinGroupRequest(const UniqueID & relayPluginServerGuid, RakString groupName);
        void LeaveGroup(const UniqueID & relayPluginServerGuid);
        void GetGroupList(const UniqueID & relayPluginServerGuid);

        /// \internal
        virtual ConnectReceiveType onMessage(Message * msg);
        /// \internal
        virtual void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason );

        struct StrAndGuidAndRoom
        {
            RakString str;
            UniqueID mID;
            RakString currentRoom;
        };

        struct StrAndGuid
        {
            RakString str;
            UniqueID mID;
        };

        struct RP_Group
        {
            RakString roomName;
            DataStructures::List<StrAndGuid> usersInRoom;
        };
    protected:
        RelayPlugin::RP_Group * JoinGroup(UniqueID userGuid, RakString roomName);
        RelayPlugin::RP_Group * JoinGroup(RP_Group * room, StrAndGuidAndRoom **strAndGuidSender);
        void LeaveGroup(StrAndGuidAndRoom **strAndGuidSender);
        void NotifyUsersInRoom(RP_Group *room, int msg, const RakString& message);
        void SendMessageToRoom(StrAndGuidAndRoom **strAndGuidSender, NetSerializer* message);
        void SendChatRoomsList(UniqueID target);
        void OnGroupMessageFromClient(Message *msg);
        void OnJoinGroupRequestFromClient(Message *msg);
        void OnLeaveGroupRequestFromClient(Message *msg);
        DataStructures::Hash<RakString, StrAndGuidAndRoom*, 8096, RakString::ToInteger> strToGuidHash;
        DataStructures::Hash<UniqueID, StrAndGuidAndRoom*, 8096, UniqueID::toNum> guidToStrHash;
        DataStructures::List<RP_Group*> chatRooms;
        bool acceptAddParticipantRequests;
    };
}
}
#endif
#endif