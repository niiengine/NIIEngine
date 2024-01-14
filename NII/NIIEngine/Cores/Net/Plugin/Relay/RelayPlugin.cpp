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

#include "RelayPlugin.h"
#include "NiiNetCommon.h"
#include "NiiNetLinkBase.h"
#include "NiiNetSerializer.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    RelayPlugin::RelayPlugin()
    {
        acceptAddParticipantRequests=false;
    }
    //------------------------------------------------------------------------
    RelayPlugin::~RelayPlugin()
    {
        DataStructures::List<StrAndGuidAndRoom *> itemList;
        DataStructures::List<RakString> keyList;
        strToGuidHash.GetAsList(itemList, keyList);
        guidToStrHash.Clear();
        for(NCount i = 0; i < itemList.Size(); ++i)
            N_delete itemList[i];
        for(NCount i = 0; i < chatRooms.Size(); ++i)
            N_delete chatRooms[i];
    }
    //------------------------------------------------------------------------
    RelayPluginEnums RelayPlugin::AddParticipantOnServer(const RakString & key, const UniqueID & id)
    {
        ConnectionState cs = mPrc2->getState(id);
        if (cs!=IS_CONNECTED)
            return RPE_ADD_CLIENT_TARGET_NOT_CONNECTED;

        if (strToGuidHash.HasData(key)==true)
            return RPE_ADD_CLIENT_NAME_ALREADY_IN_USE; // Name already in use

        // If UniqueID is already in use, remove existing
        StrAndGuidAndRoom * strAndGuidExisting;
        if (guidToStrHash.Pop(strAndGuidExisting, id))
        {
            strToGuidHash.Remove(strAndGuidExisting->str);
            N_delete strAndGuidExisting;
        }

        StrAndGuidAndRoom * strAndGuid = N_new StrAndGuidAndRoom();
        strAndGuid->mID = id;
        strAndGuid->str = key;

        strToGuidHash.Push(key, strAndGuid);
        guidToStrHash.Push(id, strAndGuid);

        return RPE_ADD_CLIENT_SUCCESS;
    }
    //------------------------------------------------------------------------
    void RelayPlugin::RemoveParticipantOnServer(const UniqueID & id)
    {
        StrAndGuidAndRoom *strAndGuid;
        if (guidToStrHash.Pop(strAndGuid, id))
        {
            LeaveGroup(&strAndGuid);
            strToGuidHash.Remove(strAndGuid->str);
            N_delete strAndGuid;
        }
    }
    //------------------------------------------------------------------------
    void RelayPlugin::SetAcceptAddParticipantRequests(bool accept)
    {
        acceptAddParticipantRequests=accept;
    }
    //------------------------------------------------------------------------
    void RelayPlugin::AddParticipantRequestFromClient(const RakString & key,
        const UniqueID & relayPluginServerGuid)
    {
        NetSerializer bsOut;
        bsOut.write((Nui8)ID_RELAY_PLUGIN);
        bsOut.write((Nui8)RPE_ADD_CLIENT_REQUEST_FROM_CLIENT);
        bsOut.writeCompress(key);
        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, relayPluginServerGuid, false);
    }
    //------------------------------------------------------------------------
    void RelayPlugin::RemoveParticipantRequestFromClient(const UniqueID & relayPluginServerGuid)
    {
        NetSerializer bsOut;
        bsOut.write((Nui8)ID_RELAY_PLUGIN);
        bsOut.write((Nui8)RPE_REMOVE_CLIENT_REQUEST_FROM_CLIENT);
        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, relayPluginServerGuid, false);
    }
    //------------------------------------------------------------------------
    // send a message to a server running RelayPlugin, to forward a message to the system identified by \a key
    void RelayPlugin::SendToParticipant(const UniqueID & relayPluginServerGuid,
        const RakString & key, NetSerializer * bitStream, PacketLevel plevel,
            PacketType ptype, char pchannel)
    {
        NetSerializer bsOut;
        bsOut.write((Nui8)ID_RELAY_PLUGIN);
        bsOut.write((Nui8)RPE_MESSAGE_TO_SERVER_FROM_CLIENT);
        bsOut.write((Nui8)plevel);
        bsOut.write((Nui8)ptype);
        bsOut.write(pchannel);
        bsOut.writeCompress(key);
        bsOut.write(bitStream);
        send(&bsOut, plevel, ptype, pchannel, relayPluginServerGuid, false);
    }
    //------------------------------------------------------------------------
    void RelayPlugin::SendGroupMessage(const UniqueID &relayPluginServerGuid,
        NetSerializer * bitStream, PacketLevel plevel, PacketType ptype,
            char pchannel)
    {
        NetSerializer bsOut;
        bsOut.write((Nui8)ID_RELAY_PLUGIN);
        bsOut.write((Nui8)RPE_GROUP_MESSAGE_FROM_CLIENT);
        bsOut.write((Nui8)plevel);
        bsOut.write((Nui8)ptype);
        bsOut.write(pchannel);
        bsOut.write(bitStream);
        send(&bsOut, plevel, ptype, pchannel, relayPluginServerGuid, false);
    }
    //------------------------------------------------------------------------
    void RelayPlugin::LeaveGroup(const UniqueID &relayPluginServerGuid)
    {
        NetSerializer bsOut;
        bsOut.write((Nui8)ID_RELAY_PLUGIN);
        bsOut.write((Nui8)RPE_LEAVE_GROUP_REQUEST_FROM_CLIENT);
        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, relayPluginServerGuid, false);
    }
    //------------------------------------------------------------------------
    void RelayPlugin::GetGroupList(const UniqueID &relayPluginServerGuid)
    {
        NetSerializer bsOut;
        bsOut.write((Nui8)ID_RELAY_PLUGIN);
        bsOut.write((Nui8)RPE_GET_GROUP_LIST_REQUEST_FROM_CLIENT);
        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, relayPluginServerGuid, false);
    }
    //------------------------------------------------------------------------
    ConnectReceiveType RelayPlugin::onMessage(SocketMessage * msg)
    {
        if (msg->mData[0]==ID_RELAY_PLUGIN)
        {
            switch (msg->mData[1])
            {
            case RPE_MESSAGE_TO_SERVER_FROM_CLIENT:
                NetSerializer bsIn(msg->mData, msg->mLength, false);
                bsIn.skipRead(2, 0);
                PacketLevel plevel;
                PacketType ptype;
                char pchannel;
                Nui8 cIn;
                bsIn.read(cIn);
                plevel = (PacketLevel) cIn;
                bsIn.read(cIn);
                ptype = (PacketType) cIn;
                bsIn.read(pchannel);
                RakString key;
                bsIn.readCompress(key);
                NetSerializer bsData;
                bsIn.read(&bsData);
                StrAndGuidAndRoom **strAndGuid = strToGuidHash.Peek(key);
                StrAndGuidAndRoom **strAndGuidSender = guidToStrHash.Peek(msg->mID);
                if (strAndGuid && strAndGuidSender)
                {
                    NetSerializer bsOut;
                    bsOut.write((Nui8)ID_RELAY_PLUGIN);
                    bsOut.write((Nui8)RPE_MESSAGE_TO_CLIENT_FROM_SERVER);
                    bsOut.writeCompress((*strAndGuidSender)->str);
                    bsOut.writeAlign();
                    bsOut.write(bsData);
                    send(&bsOut, plevel, ptype, pchannel, (*strAndGuid)->mID, false);
                }
                return CRT_Destroy;
            case RPE_ADD_CLIENT_REQUEST_FROM_CLIENT:
                NetSerializer bsIn(msg->mData, msg->mLength, false);
                bsIn.skipRead(2, 0);
                RakString key;
                bsIn.readCompress(key);
                NetSerializer bsOut;
                bsOut.write((Nui8)ID_RELAY_PLUGIN);
                if (acceptAddParticipantRequests)
                    bsOut.write((Nui8)AddParticipantOnServer(key, msg->mID));
                else
                    bsOut.write((Nui8)RPE_ADD_CLIENT_NOT_ALLOWED);
                bsOut.writeCompress(key);
                send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, msg->mAddress, false);

                return CRT_Destroy;
            case RPE_REMOVE_CLIENT_REQUEST_FROM_CLIENT:
                RemoveParticipantOnServer(msg->mID);
                return CRT_Destroy;
            case RPE_GROUP_MESSAGE_FROM_CLIENT:
                OnGroupMessageFromClient(msg);
                return CRT_Destroy;
            case RPE_JOIN_GROUP_REQUEST_FROM_CLIENT:
                OnJoinGroupRequestFromClient(msg);
                return CRT_Destroy;
            case RPE_LEAVE_GROUP_REQUEST_FROM_CLIENT:
                OnLeaveGroupRequestFromClient(msg);
                return CRT_Destroy;
            case RPE_GET_GROUP_LIST_REQUEST_FROM_CLIENT:
                SendChatRoomsList(msg->mID);
                return CRT_Destroy;
            }
        }
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void RelayPlugin::onConnectClose(const Address &address, UniqueID id,
        ConnectCloseType lostConnectionReason)
    {
        (void) lostConnectionReason;
        (void) address;

        RemoveParticipantOnServer(id);
    }
    //------------------------------------------------------------------------
    RelayPlugin::RP_Group* RelayPlugin::JoinGroup(RP_Group* room, StrAndGuidAndRoom **strAndGuidSender)
    {
        if (strAndGuidSender==0)
            return 0;

        NotifyUsersInRoom(room, RPE_USER_ENTERED_ROOM, (*strAndGuidSender)->str);
        StrAndGuid sag;
        sag.mID=(*strAndGuidSender)->mID;
        sag.str=(*strAndGuidSender)->str;

        room->usersInRoom.Push(sag);
        (*strAndGuidSender)->currentRoom=room->roomName;

        return room;
    }
    //------------------------------------------------------------------------
    void RelayPlugin::JoinGroupRequest(const UniqueID & relayPluginServerGuid,
        RakString groupName)
    {
        NetSerializer bsOut;
        bsOut.write((Nui8)ID_RELAY_PLUGIN);
        bsOut.write((Nui8)RPE_JOIN_GROUP_REQUEST_FROM_CLIENT);
        bsOut.writeCompress(groupName);
        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, relayPluginServerGuid, false);
    }
    //------------------------------------------------------------------------
    RelayPlugin::RP_Group * RelayPlugin::JoinGroup(UniqueID userGuid, RakString roomName)
    {
        StrAndGuidAndRoom ** strAndGuidSender = guidToStrHash.Peek(userGuid);
        if (strAndGuidSender)
        {
            if (roomName.IsEmpty())
                return 0;

            if ((*strAndGuidSender)->currentRoom==roomName)
                return 0;

            if ((*strAndGuidSender)->currentRoom.IsEmpty()==false)
                LeaveGroup(strAndGuidSender);

            RakString userName = (*strAndGuidSender)->str;

            for (Nui32 i=0; i < chatRooms.Size(); i++)
            {
                if (chatRooms[i]->roomName==roomName)
                {
                    // Join existing room
                    return JoinGroup(chatRooms[i],strAndGuidSender);
                }
            }
            // Create new room
            RP_Group *room = N_new RP_Group();
            room->roomName=roomName;
            chatRooms.Push(room);
            return JoinGroup(room,strAndGuidSender);
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void RelayPlugin::LeaveGroup(StrAndGuidAndRoom **strAndGuidSender)
    {
        if (strAndGuidSender==0)
            return;

        RakString userName = (*strAndGuidSender)->str;
        for (NCount i = 0; i < chatRooms.Size(); ++i)
        {
            if (chatRooms[i]->roomName==(*strAndGuidSender)->currentRoom)
            {
                (*strAndGuidSender)->currentRoom.Clear();

                RP_Group *room = chatRooms[i];
                for (NCount j = 0; j < room->usersInRoom.Size(); ++j)
                {
                    if (room->usersInRoom[j].mID==(*strAndGuidSender)->mID)
                    {
                        room->usersInRoom.RemoveAtIndexFast(j);

                        if (room->usersInRoom.Size()==0)
                        {
                            N_delete room;
                            chatRooms.RemoveAtIndexFast(i);
                            return;
                        }
                    }
                }

                NotifyUsersInRoom(room, RPE_USER_LEFT_ROOM, userName);

                return;

            }
        }
    }
    //------------------------------------------------------------------------
    void RelayPlugin::NotifyUsersInRoom(RP_Group *room, int msg, const RakString& message)
    {
        for (Nui32 i=0; i < room->usersInRoom.Size(); i++)
        {
            NetSerializer bsOut;
            bsOut.write((Nui8)ID_RELAY_PLUGIN);
            bsOut.write((Nui8)msg);
            bsOut.writeCompress(message);

            send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, room->usersInRoom[i].mID, false);
        }
    }
    //------------------------------------------------------------------------
    void RelayPlugin::SendMessageToRoom(StrAndGuidAndRoom ** strAndGuidSender, NetSerializer* message)
    {
        if ((*strAndGuidSender)->currentRoom.IsEmpty())
            return;

        for (NCount i=0; i < chatRooms.Size(); ++i)
        {
            if (chatRooms[i]->roomName==(*strAndGuidSender)->currentRoom)
            {
                NetSerializer bsOut;
                bsOut.write((Nui8)ID_RELAY_PLUGIN);
                bsOut.write((Nui8)RPE_GROUP_MSG_FROM_SERVER);
                message->seekRead(0);
                bsOut.writeCompress((*strAndGuidSender)->str);
                bsOut.writeAlign();
                bsOut.write(message);

                RP_Group * room = chatRooms[i];
                for (NCunt i = 0; i < room->usersInRoom.Size(); ++i)
                {
                    if (room->usersInRoom[i].mID!=(*strAndGuidSender)->mID)
                        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, room->usersInRoom[i].mID, false);
                }

                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void RelayPlugin::SendChatRoomsList(UniqueID target)
    {
        NetSerializer bsOut;
        bsOut.write((Nui8)ID_RELAY_PLUGIN);
        bsOut.write((Nui8)RPE_GET_GROUP_LIST_REPLY_FROM_SERVER);
        bsOut.write((Nui16)chatRooms.Size());
        for (Nui32 i=0; i < chatRooms.Size(); i++)
        {
            bsOut.writeCompress(chatRooms[i]->roomName);
            bsOut.write((Nui16)chatRooms[i]->usersInRoom.Size());
        }
        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, target, false);
    }
    //------------------------------------------------------------------------
    void RelayPlugin::OnGroupMessageFromClient(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2, 0);
        PacketLevel plevel;
        PacketType ptype;
        char pchannel;
        Nui8 cIn;
        bsIn.read(cIn);
        plevel = (PacketLevel) cIn;
        bsIn.read(cIn);
        ptype = (PacketType) cIn;
        bsIn.read(pchannel);
        NetSerializer bsData;
        bsIn.read(&bsData);

        StrAndGuidAndRoom **strAndGuidSender = guidToStrHash.Peek(msg->mID);
        if (strAndGuidSender)
        {
            SendMessageToRoom(strAndGuidSender,&bsData);
        }
    }
    //------------------------------------------------------------------------
    void RelayPlugin::OnJoinGroupRequestFromClient(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2, 0);
        RakString groupName;
        bsIn.readCompress(groupName);
        RelayPlugin::RP_Group * groupJoined = JoinGroup(msg->mID, groupName);

        NetSerializer bsOut;
        bsOut.write((Nui8)ID_RELAY_PLUGIN);
        if (groupJoined)
        {
            bsOut.write((Nui8)RPE_JOIN_GROUP_SUCCESS);
            bsOut.write((Nui16)groupJoined->usersInRoom.Size());
            for (Nui32 i=0; i < groupJoined->usersInRoom.Size(); i++)
            {
                bsOut.writeCompress(groupJoined->usersInRoom[i].str);
            }
        }
        else
        {
            bsOut.write((Nui8)RPE_JOIN_GROUP_FAILURE);
        }

        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, msg->mID, false);
    }
    //------------------------------------------------------------------------
    void RelayPlugin::OnLeaveGroupRequestFromClient(SocketMessage *msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2, 0);
        StrAndGuidAndRoom **strAndGuidSender = guidToStrHash.Peek(msg->mID);
        if (strAndGuidSender)
            LeaveGroup(strAndGuidSender);
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*