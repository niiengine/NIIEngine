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
#include "RoomsPlugin.h"
#include "NiiNetSerializer.h"
#include "RoomsErrorCodes.h"
#include "TableSerializer.h"
#include "N_assert.h"
#include "NiiNetCommon.h"
#include "NiiNetLinkBase.h"
#include "ProfanityFilter.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    int RoomsPlugin::RoomsPluginParticipantCompByRakString(const RakString & key,
        RoomsPluginParticipant * const & data)
    {
        return strcmp(key.C_String(), data->GetName().C_String());
    }
    //------------------------------------------------------------------------
    void RoomsPluginFunc::PrintResult()
    {
        printf("Result for user %s: %s\n", userName.C_String(), RoomsErrorCodeDescription::ToEnglish(resultCode));
    }
    //------------------------------------------------------------------------
    void CreateRoom_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_CREATE_ROOM;
        out->write(messageId);
        networkedRoomCreationParameters.read(out);
        out->write(userName);
        out->write(gameIdentifier);

        TableSerializer::SerializeTable(&initialRoomProperties, out);
    }
    //------------------------------------------------------------------------
    void CreateRoom_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_CREATE_ROOM;
        in->read(messageId);
        networkedRoomCreationParameters.write(in);
        in->read(userName);
        in->read(gameIdentifier);

        TableSerializer::DeserializeTable(in, &initialRoomProperties);
    }
    //------------------------------------------------------------------------
    void CreateRoom_Func::readOut(NetSerializer * in)
    {
        readIn(in);
        in->write(resultCode);
        in->write(roomId);
        roomDescriptor.read(in);
    }
    //------------------------------------------------------------------------
    void CreateRoom_Func::writeOut(NetSerializer * out)
    {
        writeIn(out);
        out->read(resultCode);
        out->read(roomId);
        roomDescriptor.write(out);
    }
    //------------------------------------------------------------------------
    void EnterRoom_Func::readIn(NetSerializer * in)
    {
        Token messageId = RPO_ENTER_ROOM;
        in->write(messageId);
        networkedRoomCreationParameters.read(in);
        in->write(roomMemberMode);
        query.read(in);
        in->write(userName);
        in->write(gameIdentifier);
    }
    //------------------------------------------------------------------------
    void EnterRoom_Func::writeIn(NetSerializer * out)
    {
        Token messageId = RPO_ENTER_ROOM;
        out->read(messageId);
        networkedRoomCreationParameters.write(out);
        out->read(roomMemberMode);
        query.write(out);
        out->read(userName);
        out->read(gameIdentifier);
    }
    //------------------------------------------------------------------------
    void EnterRoom_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
        out->write(createdRoom);
        joinedRoomResult.read(out);
        out->write(roomId);
    }
    //------------------------------------------------------------------------
    void EnterRoom_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
        in->read(createdRoom);
        joinedRoomResult.write(in);
        in->read(roomId);
    }
    //------------------------------------------------------------------------
    void JoinByFilter_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_JOIN_BY_FILTER;
        out->write(messageId);
        out->write(gameIdentifier);
        out->write(roomMemberMode);
        out->write(userName);
        query.read(out);
    }
    //------------------------------------------------------------------------
    void JoinByFilter_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_JOIN_BY_FILTER;
        in->read(messageId);
        in->read(gameIdentifier);
        in->read(roomMemberMode);
        in->read(userName);
        query.write(in);
    }
    //------------------------------------------------------------------------
    void JoinByFilter_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
        joinedRoomResult.read(out);
    }
    //------------------------------------------------------------------------
    void JoinByFilter_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
        joinedRoomResult.write(in);
    }
    //------------------------------------------------------------------------
    void LeaveRoom_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_LEAVE_ROOM;
        in->read(messageId);
        in->read(userName);
    }
    //------------------------------------------------------------------------
    void LeaveRoom_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_LEAVE_ROOM;
        out->write(messageId);
        out->write(userName);
    }
    //------------------------------------------------------------------------
    void LeaveRoom_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
        removeUserResult.write(in);
    }
    //------------------------------------------------------------------------
    void LeaveRoom_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
        removeUserResult.read(out);
    }
    //------------------------------------------------------------------------
    void GetInvitesToParticipant_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_GET_INVITES_TO_PARTICIPANT;
        in->read(messageId);
        in->read(userName);
    }
    //------------------------------------------------------------------------
    void GetInvitesToParticipant_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_GET_INVITES_TO_PARTICIPANT;
        out->write(messageId);
        out->write(userName);
    }
    //------------------------------------------------------------------------
    void GetInvitesToParticipant_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
        NCount i;
        Nui32 listSize;
        in->read(listSize);
        for(i = 0; i < listSize; ++i)
        {
            InvitedUser invitedUser;
            invitedUser.write(in);
            invitedUsers.Insert(invitedUser);
        }
    }
    //------------------------------------------------------------------------
    void GetInvitesToParticipant_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
        NCount i;
        Nui32 listSize= invitedUsers.Size();
        out->write(listSize);
        for(i = 0; i < listSize; ++i)
        {
            invitedUsers[i].read(out);
        }
    }
    //------------------------------------------------------------------------
    void SendInvite_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_SEND_INVITE;
        in->read(messageId);
        in->read(userName);
        in->read(inviteeName);
        in->read(inviteToSpectatorSlot);
        in->read(subject);
        in->read(body);
    }
    //------------------------------------------------------------------------
    void SendInvite_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_SEND_INVITE;
        out->write(messageId);
        out->write(userName);
        out->write(inviteeName);
        out->write(inviteToSpectatorSlot);
        out->write(subject);
        out->write(body);
    }
    //------------------------------------------------------------------------
    void SendInvite_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void SendInvite_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void AcceptInvite_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_ACCEPT_INVITE;
        in->read(messageId);
        in->read(userName);
        in->read(inviteSender);
        in->read(roomId);
    }
    //------------------------------------------------------------------------
    void AcceptInvite_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_ACCEPT_INVITE;
        out->write(messageId);
        out->write(userName);
        out->write(inviteSender);
        out->write(roomId);
    }
    //------------------------------------------------------------------------
    void AcceptInvite_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void AcceptInvite_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void StartSpectating_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_START_SPECTATING;
        in->read(messageId);
        in->read(userName);
    }
    //------------------------------------------------------------------------
    void StartSpectating_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_START_SPECTATING;
        out->write(messageId);
        out->write(userName);
    }
    //------------------------------------------------------------------------
    void StartSpectating_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void StartSpectating_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void StopSpectating_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_STOP_SPECTATING;
        in->read(messageId);
        in->read(userName);
    }
    //------------------------------------------------------------------------
    void StopSpectating_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_STOP_SPECTATING;
        out->write(messageId);
        out->write(userName);
    }
    //------------------------------------------------------------------------
    void StopSpectating_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void GrantModerator_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_GRANT_MODERATOR;
        in->read(messageId);
        in->read(userName);
        in->read(newModerator);
    }
    //------------------------------------------------------------------------
    void GrantModerator_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_GRANT_MODERATOR;
        out->write(messageId);
        out->write(userName);
        out->write(newModerator);
    }
    //------------------------------------------------------------------------
    void GrantModerator_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void GrantModerator_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void ChangeSlotCounts_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_CHANGE_SLOT_COUNTS;
        in->read(messageId);
        in->read(userName);
        slots.write(in);
    }
    //------------------------------------------------------------------------
    void ChangeSlotCounts_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_CHANGE_SLOT_COUNTS;
        out->write(messageId);
        out->write(userName);
        slots.read(out);
    }
    //------------------------------------------------------------------------
    void ChangeSlotCounts_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void ChangeSlotCounts_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void SetCustomRoomProperties_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_SET_CUSTOM_ROOM_PROPERTIES;
        in->read(messageId);
        in->read(userName);
        TableSerializer::DeserializeTable(in, &table);
    }
    //------------------------------------------------------------------------
    void SetCustomRoomProperties_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_SET_CUSTOM_ROOM_PROPERTIES;
        out->write(messageId);
        out->write(userName);
        TableSerializer::SerializeTable(&table, out);
    }
    //------------------------------------------------------------------------
    void SetCustomRoomProperties_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void SetCustomRoomProperties_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void SetCustomRoomProperties_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void SetCustomRoomProperties_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void GetRoomProperties_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_GET_ROOM_PROPERTIES;
        in->read(messageId);
        in->read(userName);
        in->read(roomName);
    }
    //------------------------------------------------------------------------
    void GetRoomProperties_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_GET_ROOM_PROPERTIES;
        out->write(messageId);
        out->write(userName);
        out->write(roomName);
    }
    //------------------------------------------------------------------------
    void GetRoomProperties_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
        roomDescriptor.write(in);
    }
    //------------------------------------------------------------------------
    void GetRoomProperties_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
        roomDescriptor.read(out);
    }
    //------------------------------------------------------------------------
    void ChangeRoomName_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_CHANGE_ROOM_NAME;
        in->read(messageId);
        in->read(userName);
        in->read(newRoomName);
    }
    //------------------------------------------------------------------------
    void ChangeRoomName_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_CHANGE_ROOM_NAME;
        out->write(messageId);
        out->write(userName);
        out->write(newRoomName);
    }
    //------------------------------------------------------------------------
    void ChangeRoomName_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void ChangeRoomName_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void SetHiddenFromSearches_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_SET_HIDDEN_FROM_SEARCHES;
        in->read(messageId);
        in->read(userName);
        in->read(hiddenFromSearches);
    }
    //------------------------------------------------------------------------
    void SetHiddenFromSearches_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_SET_HIDDEN_FROM_SEARCHES;
        out->write(messageId);
        out->write(userName);
        out->write(hiddenFromSearches);
    }
    //------------------------------------------------------------------------
    void SetHiddenFromSearches_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void SetHiddenFromSearches_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void SetDestroyOnModeratorLeave_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_SET_DESTROY_ON_MODERATOR_LEAVE;
        in->read(messageId);
        in->read(userName);
        in->read(destroyOnModeratorLeave);
    }
    //------------------------------------------------------------------------
    void SetDestroyOnModeratorLeave_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_SET_DESTROY_ON_MODERATOR_LEAVE;
        out->write(messageId);
        out->write(userName);
        out->write(destroyOnModeratorLeave);
    }
    //------------------------------------------------------------------------
    void SetDestroyOnModeratorLeave_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void SetDestroyOnModeratorLeave_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void SetReadyStatus_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_SET_READY_STATUS;
        in->read(messageId);
        in->read(userName);
        in->read(isReady);
    }
    //------------------------------------------------------------------------
    void SetReadyStatus_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_SET_READY_STATUS;
        out->write(messageId);
        out->write(userName);
        out->write(isReady);
    }
    //------------------------------------------------------------------------
    void SetReadyStatus_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);

        Nui32 listSize;
        NCount i;
        RakString name;
        in->read(listSize);
        for(i = 0; i < listSize; ++i)
        {
            in->read(name);
            readyUsers.Insert(name);
        }

        in->read(listSize);
        for(i = 0; i < listSize; ++i)
        {
            in->read(name);
            unreadyUsers.Insert(name);
        }
    }
    //------------------------------------------------------------------------
    void SetReadyStatus_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);

        Nui32 listSize;
        NCount i;

        listSize = readyUsers.Size();
        out->write(listSize);
        for(i = 0; i < listSize; ++i)
            out->write(readyUsers[i]);

        listSize = unreadyUsers.Size();
        out->write(listSize);
        for(i = 0; i < listSize; ++i)
            out->write(unreadyUsers[i]);
    }
    //------------------------------------------------------------------------
    void GetReadyStatus_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_GET_READY_STATUS;
        in->read(messageId);
        in->read(userName);
        Nui32 listSize;
        RakString name;
        Nui32 i;

        in->read(listSize);
        for(i = 0; i < listSize; ++i)
        {
            in->read(name);
            readyUsers.Insert(name);
        }

        in->read(listSize);

        for(i = 0; i < listSize; ++i)
        {
            in->read(name);
            unreadyUsers.Insert(name);
        }
    }
    //------------------------------------------------------------------------
    void GetReadyStatus_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_GET_READY_STATUS;
        out->write(messageId);
        out->write(userName);
        Nui32 listSize;
        RakString name;
        Nui32 i;

        listSize = readyUsers.Size();
        out->write(listSize);
        for(i = 0; i < listSize; ++i)
            out->write(readyUsers[i]);

        listSize = unreadyUsers.Size();
        out->write(listSize);
        for(i = 0; i < listSize; ++i)
            out->write(unreadyUsers[i]);
    }
    //------------------------------------------------------------------------
    void GetReadyStatus_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void GetReadyStatus_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void SetRoomLockState_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_SET_ROOM_LOCK_STATE;
        in->read(messageId);
        in->read(userName);
        in->read(roomLockState);
    }
    //------------------------------------------------------------------------
    void SetRoomLockState_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_SET_ROOM_LOCK_STATE;
        out->write(messageId);
        out->write(userName);
        out->write(roomLockState);
    }
    //------------------------------------------------------------------------
    void SetRoomLockState_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void SetRoomLockState_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->out(resultCode);
    }
    //------------------------------------------------------------------------
    void GetRoomLockState_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_GET_ROOM_LOCK_STATE;
        in->read(messageId);
        in->read(userName);
    }
    //------------------------------------------------------------------------
    void GetRoomLockState_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_GET_ROOM_LOCK_STATE;
        out->write(messageId);
        out->write(userName);
    }
    //------------------------------------------------------------------------
    void GetRoomLockState_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
        in->read(roomLockState);
    }
    //------------------------------------------------------------------------
    void GetRoomLockState_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
        out->write(roomLockState);
    }
    //------------------------------------------------------------------------
    void AreAllMembersReady_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_ARE_ALL_MEMBERS_READY;
        in->read(messageId);
        in->read(userName);
        in->read(allReady);
    }
    //------------------------------------------------------------------------
    void AreAllMembersReady_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_ARE_ALL_MEMBERS_READY;
        out->write(messageId);
        out->write(userName);
        out->write(allReady);
    }
    //------------------------------------------------------------------------
    void AreAllMembersReady_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void AreAllMembersReady_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void KickMember_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_KICK_MEMBER;
        in->read(messageId);
        in->read(userName);
        in->read(kickedMember);
        in->read(reason);
    }
    //------------------------------------------------------------------------
    void KickMember_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_KICK_MEMBER;
        out->write(messageId);
        out->write(userName);
        out->write(kickedMember);
        out->write(reason);
    }
    //------------------------------------------------------------------------
    void KickMember_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void KickMember_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void UnbanMember_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_UNBAN_MEMBER;
        in->read(messageId);
        in->read(userName);
        in->read(bannedMemberName);
    }
    //------------------------------------------------------------------------
    void UnbanMember_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_UNBAN_MEMBER;
        out->write(messageId);
        out->write(userName);
        out->write(bannedMemberName);
    }
    //------------------------------------------------------------------------
    void UnbanMember_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void UnbanMember_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void GetBanReason_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_GET_BAN_REASON;
        in->read(messageId);
        in->read(roomId);
        in->read(userName);
        in->read(reason);
    }
    //------------------------------------------------------------------------
    void GetBanReason_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_GET_BAN_REASON;
        out->write(messageId);
        out->write(roomId);
        out->write(userName);
        out->write(reason);
    }
    //------------------------------------------------------------------------
    void GetBanReason_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void GetBanReason_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void AddUserToQuickJoin_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_ADD_USER_TO_QUICK_JOIN;
        in->read(messageId);
        in->read(userName);
        in->read(gameIdentifier);
        networkedQuickJoinUser.write(in);
    }
    //------------------------------------------------------------------------
    void AddUserToQuickJoin_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_ADD_USER_TO_QUICK_JOIN;
        out->write(messageId);
        out->write(userName);
        out->write(gameIdentifier);
        networkedQuickJoinUser.read(out);
    }
    //------------------------------------------------------------------------
    void AddUserToQuickJoin_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void AddUserToQuickJoin_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->out(resultCode);
    }
    //------------------------------------------------------------------------
    void RemoveUserFromQuickJoin_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_REMOVE_USER_FROM_QUICK_JOIN;
        in->read(messageId);
        in->read(userName);
    }
    //------------------------------------------------------------------------
    void RemoveUserFromQuickJoin_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_REMOVE_USER_FROM_QUICK_JOIN;
        out->write(messageId);
        out->write(userName);
    }
    //------------------------------------------------------------------------
    void RemoveUserFromQuickJoin_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void RemoveUserFromQuickJoin_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void IsInQuickJoin_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_IS_IN_QUICK_JOIN;
        in->read(messageId);
        in->read(userName);
        in->read(isInQuickJoin);
    }
    //------------------------------------------------------------------------
    void IsInQuickJoin_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_IS_IN_QUICK_JOIN;
        out->write(messageId);
        out->write(userName);
        out->write(isInQuickJoin);
    }
    //------------------------------------------------------------------------
    void IsInQuickJoin_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void IsInQuickJoin_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    SearchByFilter_Func::~SearchByFilter_Func()
    {
        for(NCount i = 0; i < roomsOutput.Size(); ++i)
            OP_DELETE(roomsOutput[i]);
    }
    //------------------------------------------------------------------------
    void SearchByFilter_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_SEARCH_BY_FILTER;
        in->read(messageId);
        in->read(userName);
        in->read(gameIdentifier);
        in->read(onlyJoinable);
        roomQuery.write(in);
    }
    //------------------------------------------------------------------------
    void SearchByFilter_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_SEARCH_BY_FILTER;
        out->write(messageId);
        out->write(userName);
        out->write(gameIdentifier);
        out->write(onlyJoinable);
        roomQuery.read(out);
    }
    //------------------------------------------------------------------------
    void SearchByFilter_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
        Nui32 i;
        Nui32 listSize;
        in->read(listSize);
        for(i = 0; i < listSize; ++i)
        {
            RoomDescriptor * desc = OP_NEW<RoomDescriptor>();
            desc->write(in);
            roomsOutput.Insert(desc);
        }
    }
    //------------------------------------------------------------------------
    void SearchByFilter_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
        unsigned int i;
        unsigned int listSize;
        listSize = roomsOutput.Size();
        out->write(listSize);
        for(i = 0; i < listSize; ++i)
        {
            roomsOutput[i]->read(out);
        }
    }
    //------------------------------------------------------------------------
    void ChangeHandle_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_CHANGE_HANDLE;
        in->read(messageId);
        in->read(userName);
        in->read(newHandle);
    }
    //------------------------------------------------------------------------
    void ChangeHandle_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_CHANGE_HANDLE;
        out->write(messageId);
        out->write(userName);
        out->write(newHandle);
    }
    //------------------------------------------------------------------------
    void ChangeHandle_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void ChangeHandle_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void Chat_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_CHAT;
        in->read(messageId);
        in->read(userName);
        in->read(chatMessage);
        in->read(privateMessageRecipient);
        in->read(chatDirectedToRoom);
    }
    //------------------------------------------------------------------------
    void Chat_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_CHAT;
        out->write(messageId);
        out->write(userName);
        out->write(chatMessage);
        out->write(privateMessageRecipient);
        out->write(chatDirectedToRoom);
    }
    //------------------------------------------------------------------------
    void Chat_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void Chat_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void Bitstream_Func::writeIn(NetSerializer * in)
    {
        Token messageId = RPO_BITSTREAM;
        in->read(messageId);
        in->read(userName);
        NBitCount numBits;
        in->read(numBits);
        bsToSend.enlargeBit(numBits);
        in->readBit(bsToSend.getData(), numBits, true);
        bsToSend.seekWrite(numBits);
        in->read(privateMessageRecipient);
        in->read(directedToRoom);
    }
    //------------------------------------------------------------------------
    void Bitstream_Func::readIn(NetSerializer * out)
    {
        Token messageId = RPO_BITSTREAM;
        out->wirte(messageId);
        out->wirte(userName);
        NBitCount numBits = bsToSend.getWriteOffset();
        out->wirte(numBits);
        out->writeBit(bsToSend.getData(), numBits, true);
        bsToSend.seekRead(numBits);
        out->wirte(privateMessageRecipient);
        out->wirte(directedToRoom);
    }
    //------------------------------------------------------------------------
    void Bitstream_Func::writeOut(NetSerializer * in)
    {
        writeIn(in);
        in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void Bitstream_Func::readOut(NetSerializer * out)
    {
        readIn(out);
        out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void QuickJoinExpired_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_QUICK_JOIN_EXPIRED;
        in->read(messageId);
        in->read(recipient);
        networkedQuickJoinUser.write(in);
    }
    //------------------------------------------------------------------------
    void QuickJoinExpired_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_QUICK_JOIN_EXPIRED;
        out->write(messageId);
        out->write(recipient);
        networkedQuickJoinUser.read(out);
    }
    //------------------------------------------------------------------------
    void QuickJoinEnteredRoom_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_QUICK_JOIN_ENTERED_ROOM;
        in->read(messageId);
        in->read(recipient);
        joinedRoomResult.write(in);
    }
    //------------------------------------------------------------------------
    void QuickJoinEnteredRoom_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_QUICK_JOIN_ENTERED_ROOM;
        out->write(messageId);
        out->write(recipient);
        joinedRoomResult.read(out);
    }
    //------------------------------------------------------------------------
    void RoomMemberStartedSpectating_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_ROOM_MEMBER_STARTED_SPECTATING;
        in->read(messageId);
        in->read(recipient);
        in->read(userName);
        in->read(roomId);
    }
    //------------------------------------------------------------------------
    void RoomMemberStartedSpectating_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_ROOM_MEMBER_STARTED_SPECTATING;
        out->write(messageId);
        out->write(recipient);
        out->write(userName);
        out->write(roomId);
    }
    //------------------------------------------------------------------------
    void RoomMemberStoppedSpectating_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_ROOM_MEMBER_STARTED_SPECTATING;
        in->read(messageId);
        in->read(recipient);
        in->read(userName);
        in->read(roomId);
    }
    //------------------------------------------------------------------------
    void RoomMemberStoppedSpectating_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_ROOM_MEMBER_STARTED_SPECTATING;
        out->write(messageId);
        out->write(recipient);
        out->write(userName);
        out->write(roomId);
    }
    //------------------------------------------------------------------------
    void ModeratorChanged_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_MODERATOR_CHANGED;
        in->read(messageId);
        in->read(recipient);
        in->read(newModerator);
        in->read(oldModerator);
        in->read(roomId);
    }
    //------------------------------------------------------------------------
    void ModeratorChanged_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_MODERATOR_CHANGED;
        out->write(messageId);
        out->write(recipient);
        out->write(newModerator);
        out->write(oldModerator);
        out->write(roomId);
    }
    //------------------------------------------------------------------------
    void SlotCountsSet_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_SLOT_COUNTS_SET;
        in->read(messageId);
        in->read(recipient);
        slots.write(in);
        in->read(roomId);
    }
    //------------------------------------------------------------------------
    void SlotCountsSet_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_SLOT_COUNTS_SET;
        out->write(messageId);
        out->write(recipient);
        slots.read(out);
        out->write(roomId);
    }
    //------------------------------------------------------------------------
    void CustomRoomPropertiesSet_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_CUSTOM_ROOM_PROPERTIES_SET;
        in->read(messageId);
        in->read(recipient);
        in->read(roomId);

        TableSerializer::DeserializeTable(in, &table);
        tablePtr = &table;
    }
    //------------------------------------------------------------------------
    void CustomRoomPropertiesSet_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_CUSTOM_ROOM_PROPERTIES_SET;
        out->write(messageId);
        out->write(recipient);
        out->write(roomId);

        if(tablePtr == 0)
            tablePtr = &table;
        TableSerializer::SerializeTable(tablePtr, out);
    }
    //------------------------------------------------------------------------
    void RoomNameSet_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_ROOM_NAME_SET;
        in->read(messageId);
        in->read(recipient);
        in->read(oldName);
        in->read(newName);
        in->read(roomId);
    }
    //------------------------------------------------------------------------
    void RoomNameSet_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_ROOM_NAME_SET;
        out->write(messageId);
        out->write(recipient);
        out->write(oldName);
        out->write(newName);
        out->write(roomId);
    }
    //------------------------------------------------------------------------
    void HiddenFromSearchesSet_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_HIDDEN_FROM_SEARCHES_SET;
        in->read(messageId);
        in->read(recipient);
        in->read(roomId);
        in->read(hiddenFromSearches);
    }
    //------------------------------------------------------------------------
    void HiddenFromSearchesSet_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_HIDDEN_FROM_SEARCHES_SET;
        out->write(messageId);
        out->write(recipient);
        out->write(roomId);
        out->write(hiddenFromSearches);
    }
    //------------------------------------------------------------------------
    void RoomMemberReadyStatusSet_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_ROOM_MEMBER_READY_STATUS_SET;
        in->read(messageId);
        in->read(recipient);
        in->read(roomId);
        in->read(isReady);
        in->read(roomMember);

        unsigned int listSize;
        NCount i;
        RakString name;

        in->read(listSize);
        for(i = 0; i < listSize; ++i)
        {
            in->read(name);
            readyUsers.Insert(name);
        }

        in->read(listSize);
        for(i = 0; i < listSize; ++i)
        {
            in->read(name);
            unreadyUsers.Insert(name);
        }
    }
    //------------------------------------------------------------------------
    void RoomMemberReadyStatusSet_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_ROOM_MEMBER_READY_STATUS_SET;
        out->write(messageId);
        out->write(recipient);
        out->write(roomId);
        out->write(isReady);
        out->write(roomMember);

        unsigned int listSize;
        NCount i;
        RakString name;

        listSize = readyUsers.Size();
        out->write(write, listSize);
        for(i = 0; i < listSize; ++i)
            out->write(write, readyUsers[i]);

        listSize = unreadyUsers.Size();
        out->write(write, listSize);
        for(i = 0; i < listSize; ++i)
            out->write(write, unreadyUsers[i]);
    }
    //------------------------------------------------------------------------
    void RoomLockStateSet_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_ROOM_LOCK_STATE_SET;
        in->read(messageId);
        in->read(recipient);
        in->read(roomId);
        in->read(roomLockState);
    }
    //------------------------------------------------------------------------
    void RoomLockStateSet_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_ROOM_LOCK_STATE_SET;
        out->write(messageId);
        out->write(recipient);
        out->write(roomId);
        out->write(roomLockState);
    }
    //------------------------------------------------------------------------
    void RoomMemberKicked_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_ROOM_MEMBER_KICKED;
        in->read(messageId);
        in->read(recipient);
        in->read(roomId);
        in->read(kickedMember);
        in->read(moderator);
        in->read(reason);
    }
    //------------------------------------------------------------------------
    void RoomMemberKicked_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_ROOM_MEMBER_KICKED;
        out->write(messageId);
        out->write(recipient);
        out->write(roomId);
        out->write(kickedMember);
        out->write(moderator);
        out->write(reason);
    }
    //------------------------------------------------------------------------
    void RoomMemberHandleSet_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_ROOM_MEMBER_HANDLE_SET;
        in->read(messageId);
        in->read(recipient);
        in->read(oldName);
        in->read(newName);
        in->read(roomId);
    }
    //------------------------------------------------------------------------
    void RoomMemberHandleSet_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_ROOM_MEMBER_HANDLE_SET;
        out->write(messageId);
        out->write(recipient);
        out->write(oldName);
        out->write(newName);
        out->write(roomId);
    }
    //------------------------------------------------------------------------
    void RoomMemberLeftRoom_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_ROOM_MEMBER_LEFT_ROOM;
        in->read(messageId);
        in->read(recipient);
        in->read(roomMember);
        in->read(roomId);
    }
    //------------------------------------------------------------------------
    void RoomMemberLeftRoom_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_ROOM_MEMBER_LEFT_ROOM;
        out->write(messageId);
        out->write(recipient);
        out->write(roomMember);
        out->write(roomId);
    }
    //------------------------------------------------------------------------
    void RoomMemberJoinedRoom_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_ROOM_MEMBER_JOINED_ROOM;
        in->read(messageId);
        in->read(recipient);
        in->read(roomId);
        if(joinedRoomResult == 0)
            joinedRoomResult = OP_NEW<JoinedRoomResult>();
        joinedRoomResult->write(in);
    }
    //------------------------------------------------------------------------
    void RoomMemberJoinedRoom_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_ROOM_MEMBER_JOINED_ROOM;
        out->write(messageId);
        out->write(recipient);
        out->write(roomId);
        joinedRoomResult->read(out);
    }
    //------------------------------------------------------------------------
    void RoomInvitationSent_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_ROOM_INVITATION_SENT;
        in->read(messageId);
        in->read(recipient);
        in->read(invitorName);
        in->read(inviteeName);
        in->read(inviteToSpectatorSlot);
        in->read(subject);
        in->read(body);
        in->read(roomId);
    }
    //------------------------------------------------------------------------
    void RoomInvitationSent_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_ROOM_INVITATION_SENT;
        out->write(messageId);
        out->write(recipient);
        out->write(invitorName);
        out->write(inviteeName);
        out->write(inviteToSpectatorSlot);
        out->write(subject);
        out->write(body);
        out->write(roomId);
    }
    //------------------------------------------------------------------------
    void RoomInvitationWithdrawn_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_ROOM_INVITATION_WITHDRAWN;
        in->read(write, messageId);
        in->read(write, recipient);
        invitedUser.write(write, in);
    }
    //------------------------------------------------------------------------
    void RoomInvitationWithdrawn_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_ROOM_INVITATION_WITHDRAWN;
        out->write(messageId);
        out->write(recipient);
        invitedUser.read(out);
    }
    //------------------------------------------------------------------------
    void RoomDestroyedOnModeratorLeft_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_ROOM_DESTROYED_ON_MODERATOR_LEFT;
        in->read(messageId);
        in->read(recipient);
        in->read(roomId);
        in->read(oldModerator);
        roomDescriptor.write(in);
    }
    //------------------------------------------------------------------------
    void RoomDestroyedOnModeratorLeft_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_ROOM_DESTROYED_ON_MODERATOR_LEFT;
        out->write(messageId);
        out->write(recipient);
        out->write(roomId);
        out->write(oldModerator);
        roomDescriptor.read(out);
    }
    //------------------------------------------------------------------------
    void Chat_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_CHAT_NOTIFICATION;
        in->read(messageId);
        in->read(recipient);
        in->read(sender);
        in->read(privateMessageRecipient);
        in->read(chatMessage);
        in->read(filteredChatMessage);
    }
    //------------------------------------------------------------------------
    void Chat_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_CHAT_NOTIFICATION;
        out->write(messageId);
        out->write(recipient);
        out->write(sender);
        out->write(privateMessageRecipient);
        out->write(chatMessage);
        out->write(filteredChatMessage);
    }
    //------------------------------------------------------------------------
    void Bitstream_Notification::write(NetSerializer * in)
    {
        Token messageId = RPN_BITSTREAM_NOTIFICATION;
        in->read(messageId);
        in->read(recipient);
        in->read(sender);
        in->read(privateMessageRecipient);
        in->read(numBits);
        bitStreamReceived.enlargeBit(numBits);
        in->readBit(bitStreamReceived.getData(), numBits, true);
        bitStreamReceived.seekWrite(numBits);
    }
    //------------------------------------------------------------------------
    void Bitstream_Notification::read(NetSerializer * out)
    {
        Token messageId = RPN_BITSTREAM_NOTIFICATION;
        in->write(messageId);
        in->write(recipient);
        in->write(sender);
        in->write(privateMessageRecipient);
        NBitCount numBits = bitStreamReceived.getWriteOffset();
        in->write(numBits);
        if(write == false)
        {
            bitStreamReceived.enlargeBit(numBits);
        }
        in->writeBit(bitStreamReceived.getData(), numBits, true);
        bitStreamReceived.seekRead(numBits);
    }
    //------------------------------------------------------------------------
    RoomsPlugin::RoomsPlugin()
    {
        lastUpdateTime = 0;
        mPacketCh = 0;
        profanityFilter = 0;
        packetPriority = HIGH_PRIORITY;
        serverAddress = 0;
        SetRoomsCallback(this);
    }
    //------------------------------------------------------------------------
    RoomsPlugin::~RoomsPlugin()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::SetOrderingChannel(char oc)
    {
        mPacketCh=oc;
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::SetSendPriority(PacketLevel pp)
    {
        packetPriority=pp;
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::SetRoomsCallback(RoomsCallback *_roomsCallback)
    {
        roomsCallback.Clear(true);
        roomsCallback.Push(_roomsCallback);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::AddRoomsCallback(RoomsCallback *_roomsCallback)
    {
        if (roomsCallback.GetIndexOf(_roomsCallback)==(unsigned int) -1)
            roomsCallback.Push(_roomsCallback);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::RemoveRoomsCallback(RoomsCallback *_roomsCallback)
    {
        unsigned int idx = roomsCallback.GetIndexOf(_roomsCallback);
        if (idx!=(unsigned int) -1)
            roomsCallback.RemoveAtIndex(idx);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::ExecuteFunc(RoomsPluginFunc * func)
    {
        ExecuteFunc(func,serverAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::ExecuteFunc(RoomsPluginFunc * func, Address * remoteAddress)
    {
        NetSerializer bs;
        bs.write((Token)ID_ROOMS_EXECUTE_FUNC);
        if(IsServer())
            func->readOut(&bs);
        else
            func->readIn(&bs);
        send(&bs, packetPriority, RELIABLE_ORDERED, mPacketCh, remoteAddress, false);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::ExecuteNotification(RoomsPluginNotification *func, RoomsPluginParticipant *recipient)
    {
        NetSerializer bs;
        bs.write((Token)ID_ROOMS_EXECUTE_FUNC);
        func->recipient=recipient->GetName();
        func->read(&bs);
        send(&bs, packetPriority, RELIABLE_ORDERED, mPacketCh, recipient->GetSystemAddress(), false);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::SetServerAddress(Address * address)
    {
        serverAddress = address ? *address : 0;
        if (serverAddress == 0 || serverAddress->isInvalid())
        {
            RemoveRoomsCallback(this);
        }
    }
    //------------------------------------------------------------------------
    bool RoomsPlugin::LoginRoomsParticipant(RakString userName, Address * roomsParticipantAddress,
        UniqueID guid, Address * loginServerAddress)
    {
        if (loginServerAddress && !loginServerAddress->isInvalid() && loginServers.GetIndexOf(loginServerAddress)==(unsigned int) -1)
            return false;
        bool objectExists;
        unsigned int index;
        index = roomsParticipants.GetIndexFromKey(userName, &objectExists);
        if (objectExists == false)
        {
            RoomsPluginParticipant *rpp = OP_NEW<RoomsPluginParticipant>();
            rpp->SetSystemAddress(roomsParticipantAddress);
            rpp->SetGUID(guid);
            rpp->SetName(userName);
            roomsParticipants.InsertAtIndex(rpp, index);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool RoomsPlugin::LogoffRoomsParticipant(RakString userName, Address * loginServerAddress)
    {
        if (loginServerAddress && !loginServerAddress->isInvalid() && loginServers.GetIndexOf(loginServerAddress)==(unsigned int) -1)
            return false;
        bool objectExists;
        unsigned int index;
        index=roomsParticipants.GetIndexFromKey(userName, &objectExists);
        if (objectExists==true)
        {
            RemoveUserResult removeUserResult;
            roomsContainer.RemoveUser(roomsParticipants[index], &removeUserResult);
            ProcessRemoveUserResult(&removeUserResult);
            OP_DELETE(roomsParticipants[index]);
            roomsParticipants.RemoveAtIndex(index);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::ClearRoomMembers(void)
    {
        unsigned int i;
        for (i=0; i < roomsParticipants.Size(); i++)
            OP_DELETE(roomsParticipants[i]);
        roomsParticipants.Clear(false);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::SerializeLogin(RakString userName, Address * userAddress, 
        UniqueID guid, NetSerializer * bs)
    {
        bs->write((Token)ID_ROOMS_LOGON_STATUS);
        bs->write(userName);
        bs->write1();
        bs->write(userAddress);
        bs->write(guid);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::SerializeLogoff(RakString userName, NetSerializer *bs)
    {
        bs->write((Token)ID_ROOMS_LOGON_STATUS);
        bs->write(userName);
        bs->write0();
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::SerializeChangeHandle(RakString oldHandle, RakString newHandle, NetSerializer *bs)
    {
        bs->write((Token)ID_ROOMS_HANDLE_CHANGE);
        bs->write(oldHandle);
        bs->write(newHandle);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::ChangeHandle(RakString oldHandle, RakString newHandle)
    {
        RoomMemberHandleSet_Notification notification;
        notification.oldName=oldHandle;
        notification.newName=newHandle;
        roomsContainer.ChangeHandle( oldHandle, newHandle );
        RoomsPluginParticipant* roomsPluginParticipant = GetParticipantByHandle( newHandle, 0);
        if (roomsPluginParticipant && roomsPluginParticipant->GetRoom())
        {
            notification.roomId=roomsPluginParticipant->GetRoom()->GetID();
            ExecuteNotificationToOtherRoomMembers(roomsPluginParticipant->GetRoom(), roomsPluginParticipant, &notification);
        }
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::AddLoginServerAddress(Address * address)
    {
        unsigned int index = loginServers.GetIndexOf(address);
        if (index==(unsigned int) -1)
            loginServers.Push(address);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::RemoveLoginServerAddress(Address * address)
    {
        unsigned int index = loginServers.GetIndexOf(address);
        if (index!=(unsigned int) -1)
            loginServers.RemoveAtIndexFast(index);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::ClearLoginServerAdddresses()
    {
        loginServers.Clear(false);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::SetProfanityFilter(ProfanityFilter * pf)
    {
        profanityFilter=pf;
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::onDetach()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::shutdown()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::update()
    {
        if (IsServer()==false)
            return;

        DataStructures::List<QuickJoinUser *> timeoutExpired;
        DataStructures::List<QuickJoinUser *> dereferencedPointers;
        DataStructures::List<JoinedRoomResult> joinedRoomMembers;

        TimeDurMS curTime = N_Engine().getTimer().getMS();
        if (lastUpdateTime != 0)
        {
            TimeDurMS elapsedTime = curTime-lastUpdateTime;
            roomsContainer.ProcessQuickJoins( timeoutExpired, joinedRoomMembers, dereferencedPointers, elapsedTime );
            unsigned int i;
            for (i=0; i < timeoutExpired.Size(); i++)
            {
                QuickJoinExpired_Notification notification;
                notification.networkedQuickJoinUser=timeoutExpired[i]->networkedQuickJoinUser;
                ExecuteNotification(&notification, ((RoomsPluginParticipant*)timeoutExpired[i]->roomsParticipant));
            }
            for (i=0; i < joinedRoomMembers.Size(); i++)
            {
                ((RoomsPluginParticipant*)joinedRoomMembers[i].joiningMember)->lastRoomJoined=joinedRoomMembers[i].roomOutput->GetID();

                QuickJoinEnteredRoom_Notification notificationToTarget;
                notificationToTarget.joinedRoomResult=joinedRoomMembers[i];
                notificationToTarget.joinedRoomResult.agrc=&roomsContainer;
                ExecuteNotification(&notificationToTarget, ((RoomsPluginParticipant*)joinedRoomMembers[i].joiningMember));
            }

            for (i=0; i < joinedRoomMembers.Size(); i++)
            {
                RoomMemberJoinedRoom_Notification notificationToRoom;
                notificationToRoom.joinedRoomResult=&joinedRoomMembers[i];
                notificationToRoom.joinedRoomResult->agrc=&roomsContainer;
                notificationToRoom.roomId=notificationToRoom.joinedRoomResult->roomDescriptor.lobbyRoomId;
                ExecuteNotificationToOtherRoomMembers(joinedRoomMembers[i].joiningMember->GetRoom(), (RoomsPluginParticipant*)joinedRoomMembers[i].joiningMember, &notificationToRoom);
                notificationToRoom.joinedRoomResult=0;
            }

            for (i=0; i < dereferencedPointers.Size(); i++)
                OP_DELETE(dereferencedPointers[i]);
        }

        lastUpdateTime=curTime;
    }
    //------------------------------------------------------------------------
    ConnectReceiveType RoomsPlugin::onMessage(SocketMessage *msg)
    {
        switch (msg->data[0])
        {
        case ID_ROOMS_EXECUTE_FUNC:
            OnRoomsExecuteFunc(msg);
            break;
        case ID_ROOMS_LOGON_STATUS:
            OnLoginStatus(msg);
            break;
        case ID_ROOMS_HANDLE_CHANGE:
            OnHandleChange(msg);
            break;
        }

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::OnLoginStatus(SocketMessage * msg)
    {
        NCount i;
        for (i=0; i < loginServers.Size(); ++i)
        {
            if (loginServers[i] == msg->mAddress)
            {
                NetSerializer bs(msg->data, msg->length, false);
                bs.skipRead(1, 0);
                RakString name;
                bs.read(name);
                bool loggedOn;
                bs.read(loggedOn);
                Address * userAddress;
                UniqueID guid;
                if (loggedOn)
                {
                    bs.read(&userAddress);
                    bs.read(guid);
                    LoginRoomsParticipant(name, userAddress, guid, msg->mAddress);
                }
                else
                {
                    LogoffRoomsParticipant(name, msg->mAddress);
                }
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::OnHandleChange(SocketMessage * msg)
    {
        NCount i;
        for (i = 0; i < loginServers.Size(); ++i)
        {
            if (loginServers[i] == msg->mAddress)
            {
                NetSerializer bs(msg->data, msg->length, false);
                bs.skipRead(1, 0);
                RakString oldHandle, newHandle;
                bs.read(oldHandle);
                bs.read(newHandle);
                ChangeHandle(oldHandle, newHandle);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::OnRoomsExecuteFunc(SocketMessage * msg)
    {
        NetSerializer bs(msg->data, msg->length, false);
        bs.skipRead(1, 0);
        if (msg->length < 2)
            return;
        if (roomsCallback.Size() == 0)
            return;
        switch (msg->data[1])
        {
        case RPO_CREATE_ROOM:
            {
                CreateRoom_Func func;
                if(IsServer() == false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->CreateRoom_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_ENTER_ROOM:
            {
                EnterRoom_Func func;
                func.joinedRoomResult.agrc=&roomsContainer;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->EnterRoom_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_JOIN_BY_FILTER:
            {
                JoinByFilter_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->JoinByFilter_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_LEAVE_ROOM:
            {
                LeaveRoom_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->LeaveRoom_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_GET_INVITES_TO_PARTICIPANT:
            {
                GetInvitesToParticipant_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->GetInvitesToParticipant_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_SEND_INVITE:
            {
                SendInvite_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->SendInvite_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_ACCEPT_INVITE:
            {
                AcceptInvite_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->AcceptInvite_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_START_SPECTATING:
            {
                StartSpectating_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->StartSpectating_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_STOP_SPECTATING:
            {
                StopSpectating_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->StopSpectating_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_GRANT_MODERATOR:
            {
                GrantModerator_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->GrantModerator_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_CHANGE_SLOT_COUNTS:
            {
                ChangeSlotCounts_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->ChangeSlotCounts_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_SET_CUSTOM_ROOM_PROPERTIES:
            {
                SetCustomRoomProperties_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->SetCustomRoomProperties_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_GET_ROOM_PROPERTIES:
            {
                GetRoomProperties_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->GetRoomProperties_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_CHANGE_ROOM_NAME:
            {
                ChangeRoomName_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->ChangeRoomName_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_SET_HIDDEN_FROM_SEARCHES:
            {
                SetHiddenFromSearches_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->SetHiddenFromSearches_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_SET_DESTROY_ON_MODERATOR_LEAVE:
            {
                SetDestroyOnModeratorLeave_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->SetDestroyOnModeratorLeave_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_SET_READY_STATUS:
            {
                SetReadyStatus_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->SetReadyStatus_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_GET_READY_STATUS:
            {
                GetReadyStatus_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->GetReadyStatus_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_SET_ROOM_LOCK_STATE:
            {
                SetRoomLockState_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->SetRoomLockState_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_GET_ROOM_LOCK_STATE:
            {
                GetRoomLockState_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->GetRoomLockState_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_ARE_ALL_MEMBERS_READY:
            {
                AreAllMembersReady_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->AreAllMembersReady_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_KICK_MEMBER:
            {
                KickMember_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->KickMember_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_UNBAN_MEMBER:
            {
                UnbanMember_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->UnbanMember_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_GET_BAN_REASON:
            {
                GetBanReason_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->GetBanReason_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_ADD_USER_TO_QUICK_JOIN:
            {
                AddUserToQuickJoin_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->AddUserToQuickJoin_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_REMOVE_USER_FROM_QUICK_JOIN:
            {
                RemoveUserFromQuickJoin_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->RemoveUserFromQuickJoin_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_IS_IN_QUICK_JOIN:
            {
                IsInQuickJoin_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->IsInQuickJoin_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_SEARCH_BY_FILTER:
            {
                SearchByFilter_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->SearchByFilter_Callback(msg->mAddress,&func);
            }
            break;

        case RPO_CHANGE_HANDLE:
            {
                ChangeHandle_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->ChangeHandle_Callback(msg->mAddress,&func);
            }
            break;
        case RPO_CHAT:
            {
                Chat_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->Chat_Callback(msg->mAddress,&func);
            }
            break;
        case RPO_BITSTREAM:
            {
                Bitstream_Func func;
                if (IsServer()==false)
                    func.writeOut(&bs);
                else
                    func.writeIn(&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->Bitstream_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_QUICK_JOIN_EXPIRED:
            {
                QuickJoinExpired_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->QuickJoinExpired_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_QUICK_JOIN_ENTERED_ROOM:
            {
                QuickJoinEnteredRoom_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->QuickJoinEnteredRoom_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_ROOM_MEMBER_STARTED_SPECTATING:
            {
                RoomMemberStartedSpectating_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->RoomMemberStartedSpectating_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_ROOM_MEMBER_STOPPED_SPECTATING:
            {
                RoomMemberStoppedSpectating_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->RoomMemberStoppedSpectating_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_MODERATOR_CHANGED:
            {
                ModeratorChanged_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->ModeratorChanged_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_SLOT_COUNTS_SET:
            {
                SlotCountsSet_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->SlotCountsSet_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_CUSTOM_ROOM_PROPERTIES_SET:
            {
                CustomRoomPropertiesSet_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->CustomRoomPropertiesSet_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_ROOM_NAME_SET:
            {
                RoomNameSet_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->RoomNameSet_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_HIDDEN_FROM_SEARCHES_SET:
            {
                HiddenFromSearchesSet_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->HiddenFromSearchesSet_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_ROOM_MEMBER_READY_STATUS_SET:
            {
                RoomMemberReadyStatusSet_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->RoomMemberReadyStatusSet_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_ROOM_LOCK_STATE_SET:
            {
                RoomLockStateSet_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->RoomLockStateSet_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_ROOM_MEMBER_KICKED:
            {
                RoomMemberKicked_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->RoomMemberKicked_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_ROOM_MEMBER_HANDLE_SET:
            {
                RoomMemberHandleSet_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->RoomMemberHandleSet_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_ROOM_MEMBER_LEFT_ROOM:
            {
                RoomMemberLeftRoom_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->RoomMemberLeftRoom_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_ROOM_MEMBER_JOINED_ROOM:
            {
                RoomMemberJoinedRoom_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->RoomMemberJoinedRoom_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_ROOM_INVITATION_SENT:
            {
                RoomInvitationSent_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->RoomInvitationSent_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_ROOM_INVITATION_WITHDRAWN:
            {
                RoomInvitationWithdrawn_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->RoomInvitationWithdrawn_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_ROOM_DESTROYED_ON_MODERATOR_LEFT:
            {
                RoomDestroyedOnModeratorLeft_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->RoomDestroyedOnModeratorLeft_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_CHAT_NOTIFICATION:
            {
                Chat_Notification func;
                func.export(IsServer(),&bs);
                // When the filtered chat message is empty, that means the original chat message didn't have profanity anyway.
                if (func.filteredChatMessage.IsEmpty())
                    func.filteredChatMessage=func.chatMessage;
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->Chat_Callback(msg->mAddress,&func);
            }
            break;
        case RPN_BITSTREAM_NOTIFICATION:
            {
                Bitstream_Notification func;
                func.export(IsServer(),&bs);
                for (unsigned int rci=0; rci < roomsCallback.Size(); rci++)
                    roomsCallback[rci]->Bitstream_Callback(msg->mAddress,&func);
            }
            break;
        }
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::onConnectClose(const Address & address, UniqueID id,
        ConnectCloseType lostConnectionReason)
    {
        (void) lostConnectionReason;
        (void) id;

        RemoveUserResult removeUserResult;
        unsigned i;
        i = 0;
        while (i < roomsParticipants.Size())
        {
            if (roomsParticipants[i]->GetSystemAddress() == address)
            {
                roomsContainer.RemoveUser(roomsParticipants[i], &removeUserResult);
                ProcessRemoveUserResult(&removeUserResult);
                OP_DELETE(roomsParticipants[i]);
                roomsParticipants.RemoveAtIndex(i);
            }
            else
            {
                i++;
            }
        }

    }
    //------------------------------------------------------------------------
    void RoomsPlugin::Clear(void)
    {
        ClearRoomMembers();
        ClearLoginServerAdddresses();
    }
    //------------------------------------------------------------------------
    bool RoomsPlugin::IsServer(void) const
    {
        RoomsCallback *rc=(RoomsCallback *) this;
        return roomsCallback.GetIndexOf(rc)!=(unsigned int) -1;
    }
    //------------------------------------------------------------------------
    RoomsPlugin::RoomsPluginParticipant * RoomsPlugin::GetParticipantByHandle(RakString handle,
        const Address &senderAddress)
    {
        if (roomsParticipants.HasData(handle))
        {
            RoomsPluginParticipant * rp = roomsParticipants.GetElementFromKey(handle);
            N_assert(IsServer());
            // 08/21/2012 - Uncommented: This was a bug with SendInvite_Func checking the sender's address against the invitees address
            // 10/27/2010 - what was this for? Caused the bug http://www.jenkinssoftware.com/forum/index.php?topic=3720.0;topicseen
            // This apparently validates the address to match the one in the function call
            if (!senderAddress || senderAddress.isInvalid() || senderAddress == *serverAddress)
                return rp;
            if (rp->GetSystemAddress() != senderAddress)
                return 0;
            return rp;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    RoomsPlugin::RoomsPluginParticipant* RoomsPlugin::ValidateUserHandle(RoomsPluginFunc* func, const Address &address)
    {
        if (func->userName.IsEmpty())
        {
            func->resultCode=REC_USERNAME_IS_EMPTY;
            ExecuteFunc(func, address);
            return 0;
        }
        RoomsPluginParticipant* roomsPluginParticipant = GetParticipantByHandle(func->userName, address);
        if (roomsPluginParticipant==0)
        {
            func->resultCode=REC_NOT_LOGGED_IN;
            ExecuteFunc(func, address);
        }
        return roomsPluginParticipant;
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::CreateRoom_Callback(const Address & senderAddress,
        CreateRoom_Func * callResult)
    {
        RoomCreationParameters rcp;
        rcp.networkedRoomCreationParameters=callResult->networkedRoomCreationParameters;
        rcp.gameIdentifier=callResult->gameIdentifier;
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        rcp.firstUser=roomsPluginParticipant;

        callResult->resultCode=roomsContainer.CreateRoom(&rcp, profanityFilter);
        if (callResult->resultCode==REC_SUCCESS)
        {
            roomsPluginParticipant->lastRoomJoined=roomsPluginParticipant->GetRoom()->GetID();
            callResult->roomId=roomsPluginParticipant->lastRoomJoined;
            if (callResult->initialRoomProperties.GetRowCount()>0)
                callResult->resultCode=roomsContainer.SetCustomRoomProperties( roomsPluginParticipant, &callResult->initialRoomProperties );
            callResult->roomDescriptor.FromRoom(roomsPluginParticipant->GetRoom(), &roomsContainer);
        }
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::EnterRoom_Callback(const Address & senderAddress,
        EnterRoom_Func * callResult)
    {
        RoomCreationParameters rcp;
        rcp.networkedRoomCreationParameters=callResult->networkedRoomCreationParameters;
        rcp.gameIdentifier=callResult->gameIdentifier;
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        rcp.firstUser=roomsPluginParticipant;

        callResult->resultCode=roomsContainer.EnterRoom(&rcp, callResult->roomMemberMode, profanityFilter, &callResult->query, &callResult->joinedRoomResult);
        callResult->createdRoom=rcp.createdRoom;
        if (callResult->resultCode==REC_SUCCESS)
        {
            roomsPluginParticipant->lastRoomJoined=roomsPluginParticipant->GetRoom()->GetID();
            callResult->roomId=roomsPluginParticipant->lastRoomJoined;

            if (callResult->joinedRoomResult.roomOutput)
            {
                RoomMemberJoinedRoom_Notification notificationToRoom;
                notificationToRoom.joinedRoomResult=&callResult->joinedRoomResult;
                notificationToRoom.roomId=notificationToRoom.joinedRoomResult->roomDescriptor.lobbyRoomId;
                ExecuteNotificationToOtherRoomMembers(callResult->joinedRoomResult.roomOutput, roomsPluginParticipant, &notificationToRoom);
                notificationToRoom.joinedRoomResult=0;
            }
        }
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::JoinByFilter_Callback(const Address & senderAddress,
        JoinByFilter_Func * callResult)
    {
        RoomsPluginParticipant * roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        callResult->resultCode=roomsContainer.JoinByFilter(callResult->gameIdentifier, callResult->roomMemberMode, roomsPluginParticipant, roomsPluginParticipant->lastRoomJoined, &callResult->query, &callResult->joinedRoomResult );
        if (callResult->resultCode==REC_SUCCESS)
        {
            N_assert(callResult->joinedRoomResult.roomOutput->GetNumericProperty(DefaultRoomColumns::TC_USED_SLOTS)==callResult->joinedRoomResult.roomOutput->roomMemberList.Size()-1);

            roomsPluginParticipant->lastRoomJoined=roomsPluginParticipant->GetRoom()->GetID();

            RoomMemberJoinedRoom_Notification notificationToRoom;
            notificationToRoom.joinedRoomResult=&callResult->joinedRoomResult;
            notificationToRoom.roomId=notificationToRoom.joinedRoomResult->roomDescriptor.lobbyRoomId;
            ExecuteNotificationToOtherRoomMembers(roomsPluginParticipant->GetRoom(), roomsPluginParticipant, &notificationToRoom);
            notificationToRoom.joinedRoomResult=0;
        }
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::LeaveRoom_Callback(const Address & senderAddress,
        LeaveRoom_Func * callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        callResult->resultCode=roomsContainer.LeaveRoom( roomsPluginParticipant, &callResult->removeUserResult );
        ProcessRemoveUserResult(&callResult->removeUserResult);
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::GetInvitesToParticipant_Callback(const Address & senderAddress, GetInvitesToParticipant_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        DataStructures::List<InvitedUser*> invitedUsers;
        callResult->resultCode=roomsContainer.GetInvitesToParticipant( roomsPluginParticipant, invitedUsers );
        unsigned int i;
        for (i=0; i < invitedUsers.Size(); i++)
            callResult->invitedUsers.Insert(* (invitedUsers[i]));
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::SendInvite_Callback(const Address & senderAddress,
        SendInvite_Func * callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant == 0)
            return;
        RoomsPluginParticipant* inviteeId = GetParticipantByHandle( callResult->inviteeName, 0);
        if (inviteeId==0)
        {
            callResult->resultCode = REC_SEND_INVITE_RECIPIENT_NOT_ONLINE;
            ExecuteFunc(callResult, senderAddress);
            return;
        }
        callResult->resultCode = roomsContainer.SendInvite(roomsPluginParticipant, inviteeId, callResult->inviteToSpectatorSlot, callResult->subject, callResult->body );
        if (callResult->resultCode == REC_SUCCESS)
        {
            RoomInvitationSent_Notification notification;
            notification.invitorName=roomsPluginParticipant->GetName();
            notification.inviteeName=inviteeId->GetName();
            notification.inviteToSpectatorSlot=callResult->inviteToSpectatorSlot;
            notification.subject=callResult->subject;
            notification.body=callResult->body;
            notification.roomId=roomsPluginParticipant->GetRoom()->GetID();
            ExecuteNotification(&notification, inviteeId);
        }
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::AcceptInvite_Callback(const Address & senderAddress,
        AcceptInvite_Func * callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        Room *room;
        callResult->resultCode=roomsContainer.AcceptInvite( callResult->roomId, &room, roomsPluginParticipant, callResult->inviteSender );
        if (callResult->resultCode==REC_SUCCESS)
        {
            RoomMemberJoinedRoom_Notification notificationToRoom;
            notificationToRoom.joinedRoomResult=OP_NEW<JoinedRoomResult>();
            notificationToRoom.joinedRoomResult->acceptedInvitor=0;
            notificationToRoom.joinedRoomResult->acceptedInvitorName=callResult->inviteSender;
            notificationToRoom.joinedRoomResult->joiningMember=roomsPluginParticipant;
            notificationToRoom.joinedRoomResult->joiningMemberName=roomsPluginParticipant->GetName();
            notificationToRoom.joinedRoomResult->roomDescriptor.FromRoom(roomsPluginParticipant->GetRoom(), &roomsContainer);
            notificationToRoom.joinedRoomResult->roomOutput=roomsPluginParticipant->GetRoom();
            notificationToRoom.joinedRoomResult->agrc=&roomsContainer;
            notificationToRoom.roomId=notificationToRoom.joinedRoomResult->roomDescriptor.lobbyRoomId;
            ExecuteNotificationToOtherRoomMembers(roomsPluginParticipant->GetRoom(), roomsPluginParticipant, &notificationToRoom);
        }
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::StartSpectating_Callback(const Address & senderAddress,
        StartSpectating_Func * callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        callResult->resultCode=roomsContainer.StartSpectating( roomsPluginParticipant );
        if (callResult->resultCode==REC_SUCCESS)
        {
            RoomMemberStartedSpectating_Notification notification;
            //notification.userName=roomsPluginParticipant->GetName();
            notification.roomId=roomsPluginParticipant->GetRoom()->GetID();
            ExecuteNotificationToOtherRoomMembers(roomsPluginParticipant->GetRoom(), roomsPluginParticipant, &notification);
        }
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::StopSpectating_Callback( const Address &senderAddress, StopSpectating_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        callResult->resultCode=roomsContainer.StopSpectating( roomsPluginParticipant );
        if (callResult->resultCode==REC_SUCCESS)
        {
            RoomMemberStoppedSpectating_Notification notification;
            //notification.userName=roomsPluginParticipant->GetName();
            notification.roomId=roomsPluginParticipant->GetRoom()->GetID();
            ExecuteNotificationToOtherRoomMembers(roomsPluginParticipant->GetRoom(), roomsPluginParticipant, &notification);
        }
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::GrantModerator_Callback( const Address &senderAddress, GrantModerator_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        RoomsPluginParticipant* newModerator = GetParticipantByHandle( callResult->newModerator, Address::invalid );
        if (newModerator==0)
        {
            callResult->resultCode=REC_GRANT_MODERATOR_NEW_MODERATOR_NOT_ONLINE;
            ExecuteFunc(callResult, senderAddress);
            return;
        }

        DataStructures::List<InvitedUser> clearedInvites;
        callResult->resultCode=roomsContainer.GrantModerator( roomsPluginParticipant, newModerator, clearedInvites );

        if (callResult->resultCode==REC_SUCCESS)
        {
            ModeratorChanged_Notification notification;
            notification.oldModerator=roomsPluginParticipant->GetName();
            notification.newModerator=newModerator->GetName();
            notification.roomId=roomsPluginParticipant->GetRoom()->GetID();
            ExecuteNotificationToOtherRoomMembers(roomsPluginParticipant->GetRoom(), roomsPluginParticipant, &notification);

            for (unsigned int i=0; i < clearedInvites.Size(); i++)
            {
                //			Room *room = newModerator->GetRoom();
                RoomInvitationWithdrawn_Notification notification;
                notification.invitedUser=clearedInvites[i];
                ExecuteNotification(&notification,  GetParticipantByHandle(clearedInvites[i].target, 0));
            }
        }
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::ChangeSlotCounts_Callback( const Address &senderAddress, ChangeSlotCounts_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        callResult->resultCode=roomsContainer.ChangeSlotCounts( roomsPluginParticipant, callResult->slots );

        if (callResult->resultCode==REC_SUCCESS)
        {
            SlotCountsSet_Notification notification;
            notification.slots=callResult->slots;
            notification.roomId=roomsPluginParticipant->GetRoom()->GetID();
            ExecuteNotificationToOtherRoomMembers(roomsPluginParticipant->GetRoom(), roomsPluginParticipant, &notification);
        }

        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::SetCustomRoomProperties_Callback( const Address &senderAddress, SetCustomRoomProperties_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        callResult->resultCode=roomsContainer.SetCustomRoomProperties( roomsPluginParticipant, &callResult->table );
        if (callResult->resultCode==REC_SUCCESS)
        {
            CustomRoomPropertiesSet_Notification notification;
            notification.roomId=roomsPluginParticipant->GetRoom()->GetID();
            notification.tablePtr=&callResult->table;
            ExecuteNotificationToOtherRoomMembers(roomsPluginParticipant->GetRoom(), roomsPluginParticipant, &notification);
        }
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::GetRoomProperties_Callback( const Address &senderAddress, GetRoomProperties_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;

        Room *room;
        if (callResult->roomName.IsEmpty())
        {
            room=roomsPluginParticipant->GetRoom();
            if (room==0)
            {
                callResult->resultCode=REC_GET_ROOM_PROPERTIES_EMPTY_ROOM_NAME_AND_NOT_IN_A_ROOM;
                ExecuteFunc(callResult, senderAddress);
                return;
            }
            callResult->roomName=roomsPluginParticipant->GetRoom()->GetStringProperty(DefaultRoomColumns::TC_ROOM_NAME);
        }
        else
        {
            room = roomsContainer.GetRoomByName(callResult->roomName);
            if (room==0)
            {
                callResult->resultCode=REC_GET_ROOM_PROPERTIES_UNKNOWN_ROOM_NAME;
                ExecuteFunc(callResult, senderAddress);
                return;
            }
        }

        callResult->roomDescriptor.FromRoom(room, &roomsContainer);
        callResult->resultCode=REC_SUCCESS;
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::ChangeRoomName_Callback( const Address &senderAddress, ChangeRoomName_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        RoomNameSet_Notification notification;
        if (roomsPluginParticipant->GetRoom())
            notification.oldName=roomsPluginParticipant->GetRoom()->GetStringProperty(DefaultRoomColumns::TC_ROOM_NAME);
        callResult->resultCode=roomsContainer.ChangeRoomName( roomsPluginParticipant, callResult->newRoomName, profanityFilter );
        if (callResult->resultCode==REC_SUCCESS)
        {
            notification.roomId=roomsPluginParticipant->GetRoom()->GetID();
            notification.newName=callResult->newRoomName;
            ExecuteNotificationToOtherRoomMembers(roomsPluginParticipant->GetRoom(), roomsPluginParticipant, &notification);
        }
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::SetHiddenFromSearches_Callback( const Address &senderAddress, SetHiddenFromSearches_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        callResult->resultCode=roomsContainer.SetHiddenFromSearches( roomsPluginParticipant, callResult->hiddenFromSearches );
        if (callResult->resultCode==REC_SUCCESS)
        {
            HiddenFromSearchesSet_Notification notification;
            notification.roomId=roomsPluginParticipant->GetRoom()->GetID();
            notification.hiddenFromSearches=callResult->hiddenFromSearches;
            ExecuteNotificationToOtherRoomMembers(roomsPluginParticipant->GetRoom(), roomsPluginParticipant, &notification);
        }
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::SetDestroyOnModeratorLeave_Callback( const Address &senderAddress, SetDestroyOnModeratorLeave_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        callResult->resultCode=roomsContainer.SetDestroyOnModeratorLeave( roomsPluginParticipant, callResult->destroyOnModeratorLeave );
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::SetReadyStatus_Callback( const Address &senderAddress, SetReadyStatus_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        callResult->resultCode=roomsContainer.SetReadyStatus( roomsPluginParticipant, callResult->isReady );

        DataStructures::List<RoomsParticipant*> readyUsers;
        DataStructures::List<RoomsParticipant*> unreadyUsers;
        unsigned int i;
        roomsPluginParticipant->GetRoom()->GetReadyStatus(readyUsers, unreadyUsers);

        if (callResult->resultCode==REC_SUCCESS)
        {
            RoomMemberReadyStatusSet_Notification notification;
            notification.roomId=roomsPluginParticipant->GetRoom()->GetID();
            notification.isReady=callResult->isReady;
            notification.roomMember=roomsPluginParticipant->GetName();

            for (i=0; i < readyUsers.Size(); i++)
                notification.readyUsers.Insert(readyUsers[i]->GetName());
            for (i=0; i < unreadyUsers.Size(); i++)
                notification.unreadyUsers.Insert(unreadyUsers[i]->GetName());

            ExecuteNotificationToOtherRoomMembers(roomsPluginParticipant->GetRoom(), roomsPluginParticipant, &notification);
        }

        for (i=0; i < readyUsers.Size(); i++)
            callResult->readyUsers.Insert(readyUsers[i]->GetName());
        for (i=0; i < unreadyUsers.Size(); i++)
            callResult->unreadyUsers.Insert(unreadyUsers[i]->GetName());

        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::GetReadyStatus_Callback( const Address &senderAddress, GetReadyStatus_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        if (roomsPluginParticipant->GetRoom()==0)
        {
            callResult->resultCode=REC_GET_READY_STATUS_NOT_IN_ROOM;
            ExecuteFunc(callResult, senderAddress);
            return;
        }

        Room * room;
        DataStructures::List<RoomsParticipant*> readyUsers;
        DataStructures::List<RoomsParticipant*> unreadyUsers;
        callResult->resultCode=roomsContainer.GetReadyStatus( roomsPluginParticipant->GetRoom()->GetID(), &room, readyUsers, unreadyUsers );
        unsigned int i;
        for (i=0; i < readyUsers.Size(); i++)
            callResult->readyUsers.Insert(readyUsers[i]->GetName());
        for (i=0; i < unreadyUsers.Size(); i++)
            callResult->unreadyUsers.Insert(unreadyUsers[i]->GetName());
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::SetRoomLockState_Callback( const Address &senderAddress, SetRoomLockState_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        callResult->resultCode=roomsContainer.SetRoomLockState( roomsPluginParticipant, callResult->roomLockState );
        if (callResult->resultCode==REC_SUCCESS)
        {
            RoomLockStateSet_Notification notification;
            notification.roomId=roomsPluginParticipant->GetRoom()->GetID();
            notification.roomLockState=callResult->roomLockState;
            ExecuteNotificationToOtherRoomMembers(roomsPluginParticipant->GetRoom(), roomsPluginParticipant, &notification);
        }
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::GetRoomLockState_Callback( const Address &senderAddress, GetRoomLockState_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        if (roomsPluginParticipant->GetRoom()==0)
        {
            callResult->resultCode=REC_GET_ROOM_LOCK_STATE_NOT_IN_ROOM;
            ExecuteFunc(callResult, senderAddress);
            return;
        }
        Room * room;
        callResult->resultCode=roomsContainer.GetRoomLockState( roomsPluginParticipant->GetRoom()->GetID(), &room, &callResult->roomLockState );
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::AreAllMembersReady_Callback( const Address &senderAddress, AreAllMembersReady_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        if (roomsPluginParticipant->GetRoom()==0)
        {
            callResult->resultCode=REC_ARE_ALL_MEMBERS_READY_NOT_IN_ROOM;
            ExecuteFunc(callResult, senderAddress);
            return;
        }
        Room * room;
        callResult->resultCode=roomsContainer.AreAllMembersReady( roomsPluginParticipant->GetRoom()->GetID(), &room, &callResult->allReady );
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::KickMember_Callback( const Address &senderAddress, KickMember_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        RoomsPluginParticipant* kickedMember = GetParticipantByHandle(callResult->kickedMember, 0);
        if (kickedMember==0)
        {
            callResult->resultCode=REC_KICK_MEMBER_TARGET_NOT_ONLINE;
            ExecuteFunc(callResult, senderAddress);
            return;
        }
        callResult->resultCode=roomsContainer.KickMember( roomsPluginParticipant, kickedMember, callResult->reason );

        if (callResult->resultCode==REC_SUCCESS)
        {
            RoomMemberKicked_Notification notification;
            notification.roomId=roomsPluginParticipant->GetRoom()->GetID();
            notification.moderator=roomsPluginParticipant->GetName();
            notification.kickedMember=callResult->kickedMember;
            notification.reason=callResult->reason;
            ExecuteNotificationToOtherRoomMembers(roomsPluginParticipant->GetRoom(), roomsPluginParticipant, &notification);

            // Also send notification to the guy kicked
            ExecuteNotification(&notification, kickedMember);
        }

        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::UnbanMember_Callback( const Address &senderAddress, UnbanMember_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        callResult->resultCode=roomsContainer.UnbanMember( roomsPluginParticipant, callResult->bannedMemberName );
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::GetBanReason_Callback( const Address &senderAddress, GetBanReason_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        Room *room;
        callResult->resultCode=roomsContainer.GetBanReason( callResult->roomId, &room, callResult->userName, &callResult->reason );
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::AddUserToQuickJoin_Callback( const Address &senderAddress, AddUserToQuickJoin_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        QuickJoinUser * qju = OP_NEW<QuickJoinUser>();
        qju->networkedQuickJoinUser=callResult->networkedQuickJoinUser;
        qju->roomsParticipant=roomsPluginParticipant;
        callResult->resultCode=roomsContainer.AddUserToQuickJoin( callResult->gameIdentifier, qju );
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::RemoveUserFromQuickJoin_Callback( const Address &senderAddress, RemoveUserFromQuickJoin_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        QuickJoinUser *qju;
        callResult->resultCode=roomsContainer.RemoveUserFromQuickJoin( roomsPluginParticipant, &qju );
        if (qju)
            OP_DELETE(qju);
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::IsInQuickJoin_Callback( const Address &senderAddress, IsInQuickJoin_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        callResult->resultCode=REC_SUCCESS;
        callResult->isInQuickJoin=roomsContainer.IsInQuickJoin( roomsPluginParticipant );
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::SearchByFilter_Callback( const Address &senderAddress, SearchByFilter_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;

        DataStructures::OrderedList<Room*, Room*, AllGamesRoomsContainer::RoomsSortByName> roomsOutput;
        callResult->resultCode=roomsContainer.SearchByFilter( callResult->gameIdentifier, roomsPluginParticipant, &callResult->roomQuery, roomsOutput, callResult->onlyJoinable);
        unsigned i;
        RoomDescriptor *desc;
        for (i=0; i < roomsOutput.Size(); i++)
        {
            desc = OP_NEW<RoomDescriptor>();
            desc->FromRoom(roomsOutput[i], &roomsContainer);
            callResult->roomsOutput.Insert(desc);
        }
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::ChangeHandle_Callback( const Address &senderAddress, ChangeHandle_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        if (profanityFilter->HasProfanity(callResult->newHandle.C_String()))
        {
            callResult->resultCode=REC_CHANGE_HANDLE_CONTAINS_PROFANITY;
            ExecuteFunc(callResult, senderAddress);
            return;
        }

        RoomMemberHandleSet_Notification notification;
        if (roomsPluginParticipant)
            notification.oldName=roomsPluginParticipant->GetName();
        if (GetParticipantByHandle(callResult->newHandle, 0))
        {
            callResult->resultCode=REC_CHANGE_HANDLE_NEW_HANDLE_IN_USE;
            ExecuteFunc(callResult, senderAddress);
            return;
        }
        callResult->resultCode=REC_SUCCESS;
        roomsContainer.ChangeHandle( roomsPluginParticipant->GetName(), callResult->newHandle );
        if (roomsPluginParticipant->GetRoom())
        {
            notification.roomId=roomsPluginParticipant->GetRoom()->GetID();
            notification.newName=callResult->newHandle;
            ExecuteNotificationToOtherRoomMembers(roomsPluginParticipant->GetRoom(), roomsPluginParticipant, &notification);
        }
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::Chat_Callback( const Address &senderAddress, Chat_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant==0)
            return;
        if (roomsPluginParticipant->GetRoom()==0 && callResult->chatDirectedToRoom)
        {
            callResult->resultCode=REC_CHAT_USER_NOT_IN_ROOM;
            ExecuteFunc(callResult, senderAddress);
            return;
        }
        Chat_Notification notification;
        notification.sender=roomsPluginParticipant->GetName();
        notification.privateMessageRecipient=callResult->privateMessageRecipient;
        notification.chatMessage=callResult->chatMessage;
        notification.filteredChatMessage=callResult->chatMessage;
        if (profanityFilter)
            profanityFilter->FilterProfanity(notification.filteredChatMessage.C_String(), notification.filteredChatMessage.C_StringUnsafe(), true);
        if (notification.filteredChatMessage==notification.chatMessage)
            notification.filteredChatMessage.Clear(); // Save bandwidth
        if (callResult->privateMessageRecipient.IsEmpty()==false)
        {
            RoomsPluginParticipant* recipient = GetParticipantByHandle(callResult->privateMessageRecipient, 0);
            if (recipient==0)
            {
                callResult->resultCode=REC_CHAT_RECIPIENT_NOT_ONLINE;
                ExecuteFunc(callResult, senderAddress);
                return;
            }

            if (callResult->chatDirectedToRoom)
            {
                if (recipient->GetRoom()==0)
                {
                    callResult->resultCode=REC_CHAT_RECIPIENT_NOT_IN_ANY_ROOM;
                    ExecuteFunc(callResult, senderAddress);
                    return;
                }

                if (recipient->GetRoom()!=roomsPluginParticipant->GetRoom())
                {
                    callResult->resultCode=REC_CHAT_RECIPIENT_NOT_IN_YOUR_ROOM;
                    ExecuteFunc(callResult, senderAddress);
                    return;
                }
            }

            callResult->resultCode=REC_SUCCESS;
            ExecuteNotification(&notification, recipient);
            ExecuteFunc(callResult, senderAddress);
            return;
        }
        else
        {
            if (callResult->chatDirectedToRoom==false)
            {
                // Chat not directed to room, and no recipients.
                callResult->resultCode=REC_CHAT_RECIPIENT_NOT_ONLINE;
                ExecuteFunc(callResult, senderAddress);
                return;
            }
        }

        callResult->resultCode=REC_SUCCESS;
        ExecuteNotificationToOtherRoomMembers(roomsPluginParticipant->GetRoom(), roomsPluginParticipant, &notification);
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::Bitstream_Callback( const Address &senderAddress, Bitstream_Func *callResult)
    {
        RoomsPluginParticipant* roomsPluginParticipant = ValidateUserHandle(callResult, senderAddress);
        if (roomsPluginParticipant == 0)
            return;
        if (roomsPluginParticipant->GetRoom() == 0 && callResult->directedToRoom)
        {
            callResult->resultCode=REC_BITSTREAM_USER_NOT_IN_ROOM;
            ExecuteFunc(callResult, senderAddress);
            return;
        }
        Bitstream_Notification notification;
        notification.sender=roomsPluginParticipant->GetName();
        notification.privateMessageRecipient=callResult->privateMessageRecipient;
        notification.bitStreamReceived.write(callResult->bsToSend);
        if (callResult->privateMessageRecipient.IsEmpty()==false)
        {
            RoomsPluginParticipant* recipient = GetParticipantByHandle(callResult->privateMessageRecipient, 0);
            if (recipient==0)
            {
                callResult->resultCode=REC_BITSTREAM_RECIPIENT_NOT_ONLINE;
                ExecuteFunc(callResult, senderAddress);
                return;
            }

            if (callResult->directedToRoom)
            {
                if (recipient->GetRoom()==0)
                {
                    callResult->resultCode=REC_BITSTREAM_RECIPIENT_NOT_IN_ANY_ROOM;
                    ExecuteFunc(callResult, senderAddress);
                    return;
                }

                if (recipient->GetRoom()!=roomsPluginParticipant->GetRoom())
                {
                    callResult->resultCode=REC_BITSTREAM_RECIPIENT_NOT_IN_YOUR_ROOM;
                    ExecuteFunc(callResult, senderAddress);
                    return;
                }
            }

            callResult->resultCode=REC_SUCCESS;
            ExecuteNotification(&notification, recipient);
            ExecuteFunc(callResult, senderAddress);
            return;
        }
        else
        {
            if (callResult->directedToRoom==false)
            {
                // Chat not directed to room, and no recipients.
                callResult->resultCode=REC_BITSTREAM_RECIPIENT_NOT_ONLINE;
                ExecuteFunc(callResult, senderAddress);
                return;
            }
        }

        callResult->resultCode=REC_SUCCESS;
        ExecuteNotificationToOtherRoomMembers(roomsPluginParticipant->GetRoom(), roomsPluginParticipant, &notification);
        ExecuteFunc(callResult, senderAddress);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::ProcessRemoveUserResult(RemoveUserResult *removeUserResult)
    {
        unsigned int j;
        for (j=0; j < removeUserResult->clearedInvitations.Size(); j++)
        {
            RoomsPluginParticipant* invitationRecipient = GetParticipantByHandle(removeUserResult->clearedInvitations[j].target, 0);
            if (invitationRecipient)
            {
                RoomInvitationWithdrawn_Notification notification;
                notification.invitedUser=removeUserResult->clearedInvitations[j];
                ExecuteNotification(&notification, invitationRecipient);
            }
        }
        if (removeUserResult->removedFromRoom)
        {
            if (removeUserResult->room)
            {
                if (removeUserResult->roomDestroyed==false)
                {
                    if (removeUserResult->gotNewModerator)
                    {
                        ModeratorChanged_Notification notification;
                        notification.oldModerator=removeUserResult->removedUserName;
                        notification.newModerator=removeUserResult->room->GetModerator()->GetName();
                        ExecuteNotificationToOtherRoomMembers(removeUserResult->room, 0, &notification);
                    }

                    RoomMemberLeftRoom_Notification notification;
                    notification.roomId=removeUserResult->room->GetID();
                    notification.roomMember=removeUserResult->removedUserName;
                    ExecuteNotificationToOtherRoomMembers(removeUserResult->room, 0, &notification);
                }
                else
                {
                    RoomDestroyedOnModeratorLeft_Notification notification;
                    notification.oldModerator=removeUserResult->removedUserName;
                    notification.roomId=removeUserResult->room->GetID();
                    notification.roomDescriptor.FromRoom(removeUserResult->room, &roomsContainer);
                    ExecuteNotificationToOtherRoomMembers(removeUserResult->room, 0, &notification);
                }
            }
        }
        if (removeUserResult->removedFromQuickJoin)
        {
            if (removeUserResult->qju)
                OP_DELETE(removeUserResult->qju);
        }
        roomsContainer.DestroyRoomIfDead(removeUserResult->room);
    }
    //------------------------------------------------------------------------
    void RoomsPlugin::ExecuteNotificationToOtherRoomMembers(Room *room, RoomsPluginParticipant* roomsPluginParticipant, RoomsPluginNotification *notification)
    {
        unsigned roomMemberIndex;
        for (roomMemberIndex=0; roomMemberIndex < room->roomMemberList.Size(); roomMemberIndex++)
        {
            if (room->roomMemberList[roomMemberIndex]->roomsParticipant!=roomsPluginParticipant )
                ExecuteNotification(notification, ((RoomsPluginParticipant*)room->roomMemberList[roomMemberIndex]->roomsParticipant));
        }
    }
}
}