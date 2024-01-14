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
#include "Lobby2Message.h"
#include "Lobby2Client.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    uint32_t Lobby2Callbacks::nextCallbackId = 0;
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Platform_Startup * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Platform_Shutdown * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(System_CreateDatabase * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(System_DestroyDatabase * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(System_CreateTitle * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(System_DestroyTitle * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(System_GetTitleRequiredAge * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(System_GetTitleBinaryData * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(System_RegisterProfanity * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(System_BanUser * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(System_UnbanUser * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(CDKey_Add * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(CDKey_GetStatus * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(CDKey_Use * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(CDKey_FlagStolen * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Client_Login * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Client_Logoff * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Client_RegisterAccount * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(System_SetEmailAddressValidated * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Client_ValidateHandle * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(System_DeleteAccount * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(System_PruneAccounts * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Client_GetEmailAddress *message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Client_GetPasswordRecoveryQuestionByHandle * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Client_GetPasswordByPasswordRecoveryAnswer * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Client_ChangeHandle * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Client_UpdateAccount * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Client_GetAccountDetails * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Client_StartIgnore * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Client_StopIgnore * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Client_GetIgnoreList * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Client_PerTitleIntegerStorage * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Client_PerTitleBinaryStorage * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Client_SetPresence * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Client_GetPresence * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Friends_SendInvite * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Friends_AcceptInvite * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Friends_RejectInvite * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Friends_GetInvites * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Friends_GetFriends * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Friends_Remove * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(BookmarkedUsers_Add * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(BookmarkedUsers_Remove * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(BookmarkedUsers_Get * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Emails_Send * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Emails_Get * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Emails_Delete * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Emails_SetStatus * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Ranking_SubmitMatch * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Ranking_GetMatches * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Ranking_GetMatchBinaryData * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Ranking_GetTotalScore * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Ranking_WipeScoresForPlayer * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Ranking_WipeMatches * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Ranking_PruneMatches * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Ranking_UpdateRating * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Ranking_WipeRatings * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Ranking_GetRating * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_Create * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_SetProperties * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_GetProperties * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_SetMyMemberProperties * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_GrantLeader * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_SetSubleaderStatus * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_SetMemberRank * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_GetMemberProperties * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_ChangeHandle * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_Leave * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_Get * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_SendJoinInvitation * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_WithdrawJoinInvitation * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_AcceptJoinInvitation * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_RejectJoinInvitation * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_DownloadInvitationList * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_SendJoinRequest * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_WithdrawJoinRequest * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_AcceptJoinRequest * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_RejectJoinRequest * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_DownloadRequestList * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_KickAndBlacklistUser * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_UnblacklistUser * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_GetBlacklist * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_GetMembers * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_GetList * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_CreateBoard * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_DestroyBoard * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_CreateNewTopic * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_ReplyToTopic * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_RemovePost * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_GetBoards * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_GetTopics * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Clans_GetPosts * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_GameBootCheck * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_GetGameBootInviteDetails * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_GetServerStatus * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_GetWorldListFromServer * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_GetLobbyListFromWorld * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_JoinLobby * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_LeaveLobby * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_SendLobbyChatMessage * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_SearchRooms * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_GetRoomDetails * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_GetLobbyMemberData * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_CreateRoom * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_SignIntoRoom * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_SetRoomSearchProperties * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_UpdateRoomParameters * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_JoinRoom * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_LeaveRoom * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_SendLobbyInvitationToRoom * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_SendGUIInvitationToRoom * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_SendDataMessageToUser * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_SendRoomChatMessage * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_ShowFriendsUI * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_EndGame * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_StartGame * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_ShowPartyUI * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_ShowMessagesUI * message)
    {
        ExecuteDefaultResult(message);
        }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_ShowGUIInvitationsToRooms * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Console_EnableDisableRoomVoiceChat * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Client_RemoteLogin * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Client_IgnoreStatus * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Friends_StatusChange * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Friends_PresenceUpdate * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_User_ChangedHandle * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Friends_CreatedClan * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Emails_Received * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Clans_GrantLeader * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Clans_SetSubleaderStatus * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Clans_SetMemberRank * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Clans_ChangeHandle * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Clans_Leave * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Clans_PendingJoinStatus * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Clans_NewClanMember * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Clans_KickAndBlacklistUser * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Clans_UnblacklistUser * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Clans_Destroyed * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_CableDisconnected * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_ContextError * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_MemberJoinedLobby * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_MemberLeftLobby * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_LobbyDestroyed * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_LobbyMemberDataUpdated * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_LobbyGotChatMessage * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_LobbyGotRoomInvitation * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_MemberJoinedRoom * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_MemberLeftRoom * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_KickedOutOfRoom * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_RoomWasDestroyed * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_UpdateRoomParameters * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_RoomOwnerChanged * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_RoomChatMessage * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_RoomMessage * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_ChatEvent * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_MuteListChanged * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_Local_Users_Changed * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_ReceivedDataMessageFromUser * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_MemberJoinedParty * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_MemberLeftParty * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_Game_Started * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_Game_Ending * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_Game_Ended * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_Got_Room_Invite * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    void Lobby2Callbacks::MessageResult(Notification_Console_Accepted_Room_Invite * message)
    {
        ExecuteDefaultResult(message);
    }
    //------------------------------------------------------------------------
    Lobby2Message::Lobby2Message()
    {
        refCount = 1;
        requestId = (unsigned int)-1;
        callbackId = (uint32_t)-1;
    }
    //------------------------------------------------------------------------
    void Lobby2Message::writeBase(bool result, NetSerializer * in)
    {
        in->read(requestId);
        in->read(callbackId);
        if(result)
            in->read(resultCode);
    }
    //------------------------------------------------------------------------
    void Lobby2Message::readBase(bool result, NetSerializer * out)
    {
        out->write(requestId);
        out->write(callbackId);
        if(result)
            out->write(resultCode);
    }
    //------------------------------------------------------------------------
    void Lobby2Message::write(bool result, NetSerializer * org)
    {
        writeBase(result, org);
    }
    //------------------------------------------------------------------------
    void Lobby2Message::read(bool result, NetSerializer * org)
    {
        readBase(result, org);
    }
    //------------------------------------------------------------------------
    bool Lobby2Message::ValidateBinary(SharedPtr<BinaryDataBlock>binaryDataBlock)
    {
        if (binaryDataBlock->binaryDataLength > L2_MAX_BINARY_DATA_LENGTH)
        {
            resultCode = L2RC_BINARY_DATA_LENGTH_EXCEEDED;
            return false;
        }

        if (binaryDataBlock->binaryDataLength > 0 && binaryDataBlock->binaryData == 0)
        {
            resultCode = L2RC_BINARY_DATA_NULL_POINTER;
            return false;
        }

        return true;
    }
    //------------------------------------------------------------------------
    bool Lobby2Message::ValidateHandle(RakString * handle)
    {
        if (handle->IsEmpty())
        {
            resultCode=L2RC_HANDLE_IS_EMPTY;
            return false;
        }
        if (handle->C_String()[0]==' ')
        {
            resultCode=L2RC_HANDLE_STARTS_WITH_SPACES;
            return false;
        }
        size_t len = handle->GetLength();
        if (handle->C_String()[len-1]==' ')
        {
            resultCode=L2RC_HANDLE_ENDS_WITH_SPACES;
            return false;
        }
        if (len>50)
        {
            resultCode=L2RC_HANDLE_IS_TOO_LONG;
            return false;
        }
        if (len<=2)
        {
            resultCode=L2RC_HANDLE_IS_TOO_SHORT;
            return false;
        }
        if (handle->ContainsNonprintableExceptSpaces())
        {
            resultCode=L2RC_HANDLE_CONTAINS_NON_PRINTABLE;
            return false;
        }
        if (strstr(handle->C_String(), "  ")!=0)
        {
            resultCode=L2RC_HANDLE_HAS_CONSECUTIVE_SPACES;
            return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool Lobby2Message::ValidateRequiredText(RakString * text)
    {
        if (text->IsEmpty())
        {
            resultCode=L2RC_REQUIRED_TEXT_IS_EMPTY;
            return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool Lobby2Message::ValidatePassword(RakString * text)
    {
        if (text->IsEmpty())
        {
            resultCode=L2RC_PASSWORD_IS_EMPTY;
            return false;
        }

        size_t len = text->GetLength();
        if (len > 50)
        {
            resultCode = L2RC_PASSWORD_IS_TOO_LONG;
            return false;
        }
        if (len < 4)
        {
            resultCode = L2RC_PASSWORD_IS_TOO_SHORT;
            return false;
        }

        return true;
    }
    //------------------------------------------------------------------------
    bool Lobby2Message::ValidateEmailAddress(RakString * text)
    {
        if (text->IsEmpty())
        {
            resultCode = L2RC_EMAIL_ADDRESS_IS_EMPTY;
            return false;
        }
        if (text->IsEmailAddress() == false)
        {
            resultCode = L2RC_EMAIL_ADDRESS_IS_INVALID;
            return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool Lobby2Message::PrevalidateInput(void)
    {
        return true;
    }
    //------------------------------------------------------------------------
    bool Lobby2Message::ClientImpl(Lobby2Plugin * client)
    {
        (void)client;
        return true;
    }
    //------------------------------------------------------------------------
    bool Lobby2Message::ServerPreDBMemoryImpl(Lobby2Server * server, RakString userHandle)
    {
        (void)server;
        (void)userHandle;
        return false;
    }
    //------------------------------------------------------------------------
    void Lobby2Message::ServerPostDBMemoryImpl(Lobby2Server * server, RakString userHandle)
    {
        (void)server;
        (void)userHandle;
    }
    //------------------------------------------------------------------------
    bool Lobby2Message::ServerDBImpl(Lobby2ServerCommand * command, void * databaseInterface)
    {
        (void)command;
        (void)databaseInterface;
        resultCode = L2RC_COUNT;
        return true;
    }
    //------------------------------------------------------------------------
    void CreateAccountParameters::read(NetSerializer * org)
    {
        org->write(firstName);
        org->write(middleName);
        org->write(lastName);
        org->write(race);
        org->write(sex_male);
        org->write(homeAddress1);
        org->write(homeAddress2);
        org->write(homeCity);
        org->write(homeState);
        org->write(homeCountry);
        org->write(homeZipCode);
        org->write(billingAddress1);
        org->write(billingAddress2);
        org->write(billingCity);
        org->write(billingState);
        org->write(billingCountry);
        org->write(billingZipCode);
        org->write(emailAddress);
        org->write(password);
        org->write(passwordRecoveryQuestion);
        org->write(passwordRecoveryAnswer);
        org->write(caption1);
        org->write(caption2);
        org->write(ageInDays);
        binaryData->read(org);
    }
    //------------------------------------------------------------------------
    void CreateAccountParameters::write(NetSerializer * org)
    {
        org->read(firstName);
        org->read(middleName);
        org->read(lastName);
        org->read(race);
        org->read(sex_male);
        org->read(homeAddress1);
        org->read(homeAddress2);
        org->read(homeCity);
        org->read(homeState);
        org->read(homeCountry);
        org->read(homeZipCode);
        org->read(billingAddress1);
        org->read(billingAddress2);
        org->read(billingCity);
        org->read(billingState);
        org->read(billingCountry);
        org->read(billingZipCode);
        org->read(emailAddress);
        org->read(password);
        org->read(passwordRecoveryQuestion);
        org->read(passwordRecoveryAnswer);
        org->read(caption1);
        org->read(caption2);
        org->read(ageInDays);
        binaryData->write(org);
    }
    //------------------------------------------------------------------------
    void BinaryDataBlock::write(NetSerializer * in)
    {
        bool hasData = binaryData && binaryDataLength > 0;
        in->read(hasData);
        if(hasData == false)
            return;
        in->read(binaryDataLength);

        if(binaryData)
            rakFree_Ex(binaryData);

        if(binaryDataLength <= L2_MAX_BINARY_DATA_LENGTH)
            binaryData = N_alloc_t(Nui8, binaryDataLength);
        else
            binaryData = 0;

        if(binaryData)
            in->read(binaryData, binaryDataLength);
        else
            in->skipWrite(binaryDataLength);
    }
    //------------------------------------------------------------------------
    void BinaryDataBlock::read(NetSerializer * out)
    {
        bool hasData = binaryData && binaryDataLength > 0;
        out->write(hasData);
        if(hasData == false)
            return;
        out->write(binaryDataLength);

        if(binaryData)
            out->write(binaryData, binaryDataLength);
    }
    //------------------------------------------------------------------------
    void PendingInvite::write(NetSerializer * in)
    {
        in->read(sender);
        in->read(subject);
        in->read(body);
        binaryData->write(in);
    }
    //------------------------------------------------------------------------
    void PendingInvite::read(NetSerializer * out)
    {
        out->write(sender);
        out->write(subject);
        out->write(body);
        binaryData->read(out);
    }
    //------------------------------------------------------------------------
    FriendInfo::FriendInfo() {}
    //------------------------------------------------------------------------
    FriendInfo::FriendInfo(const FriendInfo & input)
    {
        usernameAndStatus = input.usernameAndStatus;
    }
    //------------------------------------------------------------------------
    FriendInfo& FriendInfo::operator = (const FriendInfo & input)
    {
        usernameAndStatus = input.usernameAndStatus;
        return *this;
    }
    //------------------------------------------------------------------------
    UsernameAndOnlineStatus::UsernameAndOnlineStatus()
    {
        isOnline = false;
        uid = 0;
    }
    //------------------------------------------------------------------------
    UsernameAndOnlineStatus::UsernameAndOnlineStatus(const UsernameAndOnlineStatus & input) {
        handle=input.handle;
        isOnline=input.isOnline;
        uid=input.uid;
        presence=input.presence;
    }
    //------------------------------------------------------------------------
    UsernameAndOnlineStatus & UsernameAndOnlineStatus::operator = (const UsernameAndOnlineStatus& input )
    {
        handle = input.handle;
        isOnline = input.isOnline;
        uid = input.uid;
        presence = input.presence;
        return *this;
    }
    //------------------------------------------------------------------------
    void UsernameAndOnlineStatus::write(NetSerializer * in)
    {
        in->read(handle);
        in->read(isOnline);
        in->read(uid);
        presence.write(in);
    }
    //------------------------------------------------------------------------
    void UsernameAndOnlineStatus::read(NetSerializer * out)
    {
        out->write(handle);
        out->write(isOnline);
        out->write(uid);
        presence.read(out);
    }
    //------------------------------------------------------------------------
    void EmailResult::write(NetSerializer * in)
    {
        in->read(sender);
        in->read(recipient);
        in->read(subject);
        in->read(body);
        in->read(status);
        in->read(wasSendByMe);
        in->read(wasReadByMe);
        in->read(subject);
        in->read(emailID);
        binaryData->write(in);
        in->read(creationDate);
    }
    //------------------------------------------------------------------------
    void EmailResult::read(NetSerializer * out)
    {
        out->write(sender);
        out->write(recipient);
        out->write(subject);
        out->write(body);
        out->write(status);
        out->write(wasSendByMe);
        out->write(wasReadByMe);
        out->write(subject);
        out->write(emailID);
        binaryData->read(out);
        out->write(creationDate);
    }
    //------------------------------------------------------------------------
    void MatchParticipant::write(NetSerializer * in)
    {
        in->read(handle);
        in->read(score);
    }
    //------------------------------------------------------------------------
    void MatchParticipant::read(NetSerializer * out)
    {
        out->write(handle);
        out->write(score);
    }
    //------------------------------------------------------------------------
    void SubmittedMatch::write(NetSerializer * in)
    {
        in->read(matchNote);
        in->read(whenSubmittedDate);
        in->read(matchID);
        binaryData->write(in);
        NCount listSize;
        in->readCompress((Nui16)listSize);
        for(NCount i = 0; i < listSize; ++i)
        {
            MatchParticipant obj;
            obj.write(in);
            matchParticipants.Insert(obj);
        }
    }
    //------------------------------------------------------------------------
    void SubmittedMatch::read(NetSerializer * out)
    {
        out->write(matchNote);
        out->write(whenSubmittedDate);
        out->write(matchID);
        binaryData->read(out);
        NCount listSize = matchParticipants.Size();
        out->writeCompress((Nui16)listSize);
        for(NCount i = 0; i < listSize; ++i)
        {
            matchParticipants[i].read(out);
        }
    }
    //------------------------------------------------------------------------
    void ClanInfo::write(NetSerializer * in)
    {
        in->read(clanName);
        in->read(description);
        in->read(clanLeader);
        binaryData->write(in);
        Nui16 listSize;
        in->readCompress(listSize);
        for(NCount i = 0; i < listSize; ++i)
        {
            RakString obj;
            in->read(obj);
            clanMembersOtherThanLeader.Insert(obj);
        }
    }
    //------------------------------------------------------------------------
    void ClanInfo::read(bool write, NetSerializer * out)
    {
        out->write(clanName);
        out->write(description);
        out->write(clanLeader);
        binaryData->read(out);
        Nui16 listSize = (Nui16)clanMembersOtherThanLeader.Size();
        out->writeCompress(listSize);
        for(NCount i = 0; i < listSize; ++i)
        {
            out->write(clanMembersOtherThanLeader[i]);
        }
    }
    //------------------------------------------------------------------------
    void OpenInvite::write(NetSerializer * in)
    {
        in->read(clanHandle);
    }
    //------------------------------------------------------------------------
    void OpenInvite::read(NetSerializer * out)
    {
        out->write(clanHandle);
    }
    //------------------------------------------------------------------------
    void ClanJoinRequest::write(NetSerializer * in)
    {
        in->read(targetClan);
        in->read(dateSent);
        in->read(joinRequestSender);
    }
    //------------------------------------------------------------------------
    void ClanJoinRequest::read(NetSerializer * out)
    {
        out->write(targetClan);
        out->write(dateSent);
        out->write(joinRequestSender);
    }
    //------------------------------------------------------------------------
    void ClanJoinInvite::write(NetSerializer * in)
    {
        in->read(sourceClan);
        in->read(dateSent);
        in->read(joinRequestTarget);
    }
    //------------------------------------------------------------------------
    void ClanJoinInvite::read(NetSerializer * out)
    {
        out->write(sourceClan);
        out->write(dateSent);
        out->write(joinRequestTarget);
    }
    //------------------------------------------------------------------------
    void BookmarkedUser::write(NetSerializer * in)
    {
        in->read(targetHandle);
        in->read(type);
        in->read(description);
        in->read(dateWhenAdded);
    }
    //------------------------------------------------------------------------
    void BookmarkedUser::read(NetSerializer * out)
    {
        out->write(targetHandle);
        out->write(type);
        out->write(description);
        out->write(dateWhenAdded);
    }
    //------------------------------------------------------------------------
    void System_CreateTitle::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(titleName);
        in->read(titleSecretKey);
        in->read(requiredAge);
        binaryData->write(in);
    }
    //------------------------------------------------------------------------
    void System_CreateTitle::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(titleName);
        out->write(titleSecretKey);
        out->write(requiredAge);
        binaryData->read(out);
    }
    //------------------------------------------------------------------------
    bool System_CreateTitle::PrevalidateInput(void)
    {
        //
        if (!ValidateRequiredText(&titleName))
            return false;
        if (!ValidatePassword(&titleSecretKey))
            return false;
        if (!ValidateBinary(binaryData))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void System_DestroyTitle::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(titleName);
    }
    //------------------------------------------------------------------------
    void System_DestroyTitle::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(titleName);
    }
    //------------------------------------------------------------------------
    void System_GetTitleRequiredAge::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(titleName);
        if(result)
            in->read(requiredAge);
    }
    //------------------------------------------------------------------------
    void System_GetTitleRequiredAge::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(titleName);
        if(result)
            out->write(requiredAge);
    }
    //------------------------------------------------------------------------
    void System_GetTitleBinaryData::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(titleName);
        if (result)
            binaryData->read(in);
    }
    //------------------------------------------------------------------------
    void System_GetTitleBinaryData::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(titleName);
        if(result)
            binaryData->write(out);
    }
    //------------------------------------------------------------------------
    void System_RegisterProfanity::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        Nui16 listSize;
        in->readCompress(listSize);
        for(NCount i = 0; i < listSize; ++i)
        {
            RakString obj;
            in->read(obj);
            profanityWords.Insert(obj);
        }
    }
    //------------------------------------------------------------------------
    void System_RegisterProfanity::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        Nui16 listSize = (Nui16)profanityWords.Size();
        out->writeCompress(listSize);
        for(NCount i = 0; i < listSize; ++i)
        {
            out->write(profanityWords[i]);
        }
    }
    //------------------------------------------------------------------------
    void System_BanUser::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(banReason);
        in->read(durationHours);
        in->read(userName);
    }
    //------------------------------------------------------------------------
    void System_BanUser::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(banReason);
        out->write(durationHours);
        out->write(userName);
    }
    //------------------------------------------------------------------------
    bool System_BanUser::PrevalidateInput(void)
    {
        if(!ValidateHandle(&userName))
            return false;
        if(!ValidateRequiredText(&banReason))
            return false;
        if(durationHours <= 0)
        {
            resultCode = L2RC_System_BanUser_INVALID_DURATION;
            return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    void System_UnbanUser::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(reason);
        in->read(userName);
    }
    //------------------------------------------------------------------------
    void System_UnbanUser::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(reason);
        out->write(userName);
    }
    //------------------------------------------------------------------------
    bool System_UnbanUser::PrevalidateInput(void)
    {
        if (!ValidateHandle(&userName)) return false;
        if (!ValidateRequiredText(&reason)) return false;
        return true;
    }
    //------------------------------------------------------------------------
    void CDKey_Add::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(titleName);
        Nui16 listSize;
        in->readCompress(listSize);
        for(NCount i = 0; i < listSize; ++i)
        {
            RakString obj;
            in->read(obj);
            cdKeys.Insert(obj);
        }
    }
    //------------------------------------------------------------------------
    void CDKey_Add::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(titleName);
        Nui16 listSize = (Nui16)cdKeys.Size();
        out->writeCompress(listSize);
        for(NCount i = 0; i < listSize; ++i)
        {
            out->write(cdKeys[i]);
        }
    }
    //------------------------------------------------------------------------
    bool CDKey_Add::PrevalidateInput(void)
    {
        for(NCount i = 0; i < cdKeys.Size(); ++i)
            if (!ValidateRequiredText(&cdKeys[i])) return false;
        if (!ValidateRequiredText(&titleName)) return false;
        return true;
    }
    //------------------------------------------------------------------------
    void CDKey_GetStatus::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(cdKey);
        in->read(titleName);
        if(result)
        {
            in->read(usable);
            in->read(usedBy);
            in->read(activationDate);
        }
    }
    //------------------------------------------------------------------------
    void CDKey_GetStatus::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(cdKey);
        out->write(titleName);
        if(result)
        {
            out->write(usable);
            out->write(usedBy);
            out->write(activationDate);
        }
    }
    //------------------------------------------------------------------------
    bool CDKey_GetStatus::PrevalidateInput(void)
    {
        if (!ValidateRequiredText(&cdKey))
            return false;
        if (!ValidateRequiredText(&titleName))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void CDKey_Use::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(cdKey);
        in->read(titleName);
        in->read(userName);
    }
    //------------------------------------------------------------------------
    void CDKey_Use::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(cdKey);
        out->write(titleName);
        out->write(userName);
    }
    //------------------------------------------------------------------------
    bool CDKey_Use::PrevalidateInput(void)
    {
        if (!ValidateHandle(&userName))
            return false;
        if (!ValidateRequiredText(&cdKey))
            return false;
        if (!ValidateRequiredText(&titleName))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void CDKey_FlagStolen::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(cdKey);
        in->read(titleName);
        in->read(userUsingThisKey);
        in->read(wasStolen);
    }
    //------------------------------------------------------------------------
    void CDKey_FlagStolen::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(cdKey);
        out->write(titleName);
        out->write(userUsingThisKey);
        out->write(wasStolen);
    }
    //------------------------------------------------------------------------
    bool CDKey_FlagStolen::PrevalidateInput(void)
    {
        if (!ValidateRequiredText(&cdKey))
            return false;
        if (!ValidateRequiredText(&titleName))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Client_Login::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(userPassword);
        in->read(allowMultipleLogins);
        in->read(titleName);
        in->read(titleSecretKey);
        in->read(userName);

        if(result)
        {
            in->read(bannedReason);
            in->read(whenBanned);
            in->read(bannedExpiration);
        }
    }
    //------------------------------------------------------------------------
    void Client_Login::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(userPassword);
        out->write(allowMultipleLogins);
        out->write(titleName);
        out->write(titleSecretKey);
        out->write(userName);

        if(result)
        {
            out->write(bannedReason);
            out->write(whenBanned);
            out->write(bannedExpiration);
        }
    }
    //------------------------------------------------------------------------
    bool Client_Login::PrevalidateInput(void)
    {
        if (!ValidateHandle(&userName))
            return false;
        if (!ValidateHandle(&titleName))
            return false;
        if (!ValidateRequiredText(&userPassword))
            return false;
        if (!ValidateRequiredText(&titleSecretKey))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Client_RegisterAccount::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        createAccountParameters.write(in);
        in->read(cdKey);
        in->read(titleName);
        in->read(userName);
    }
    //------------------------------------------------------------------------
    void Client_RegisterAccount::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        createAccountParameters.read(out);
        out->write(cdKey);
        out->write(titleName);
        out->write(userName);
    }
    //------------------------------------------------------------------------
    bool Client_RegisterAccount::PrevalidateInput(void)
    {
        if (!ValidateHandle(&userName)) return false;
        // If either first name or last name is set, then both must be set correctly.
        if (createAccountParameters.firstName.IsEmpty()==false ||
            createAccountParameters.lastName.IsEmpty()==false)
        {
            if (!ValidateHandle(&createAccountParameters.firstName)) return false;
            if (!ValidateHandle(&createAccountParameters.lastName)) return false;
        }
        if (!ValidatePassword(&createAccountParameters.password)) return false;

        if (createAccountParameters.emailAddress.IsEmpty()==false &&
            !ValidateEmailAddress(&createAccountParameters.emailAddress)) return false;
        return true;
    }
    //------------------------------------------------------------------------
    void System_SetEmailAddressValidated::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(validated);
        in->read(userName);
    }
    //------------------------------------------------------------------------
    void System_SetEmailAddressValidated::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(validated);
        out->write(userName);
    }
    //------------------------------------------------------------------------
    bool System_SetEmailAddressValidated::PrevalidateInput( void )
    {
        if (!ValidateHandle(&userName)) return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Client_ValidateHandle::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(userName);
    }
    //------------------------------------------------------------------------
    void Client_ValidateHandle::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(userName);
    }
    //------------------------------------------------------------------------
    bool Client_ValidateHandle::PrevalidateInput(void)
    {
        if (!ValidateHandle(&userName)) return false;
        return true;
    }
    //------------------------------------------------------------------------
    void System_DeleteAccount::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(password);
        in->read(userName);
    }
    //------------------------------------------------------------------------
    void System_DeleteAccount::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(password);
        out->write(userName);
    }
    //------------------------------------------------------------------------
    bool System_DeleteAccount::PrevalidateInput( void )
    {
        if (!ValidateHandle(&userName)) return false;
        if (!ValidatePassword(&password)) return false;
        return true;
    }
    //------------------------------------------------------------------------
    bool System_PruneAccounts::PrevalidateInput(void)
    {
        if (deleteAccountsNotLoggedInDays==0) return false;
        return true;
    }
    //------------------------------------------------------------------------
    void System_PruneAccounts::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(deleteAccountsNotLoggedInDays);
    }
    //------------------------------------------------------------------------
    void System_PruneAccounts::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(deleteAccountsNotLoggedInDays);
    }
    //------------------------------------------------------------------------
    bool Client_GetEmailAddress::PrevalidateInput(void)
    {
        if (!ValidateHandle(&userName)) return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Client_GetEmailAddress::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(userName);
        if(result)
        {
            in->read(emailAddress);
            in->read(emailAddressValidated);
        }
    }
    //------------------------------------------------------------------------
    void Client_GetEmailAddress::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(userName);
        if(result)
        {
            out->write(emailAddress);
            out->write(emailAddressValidated);
        }
    }
    //------------------------------------------------------------------------
    bool Client_GetPasswordRecoveryQuestionByHandle::PrevalidateInput(void)
    {
        if (!ValidateHandle(&userName)) return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Client_GetPasswordRecoveryQuestionByHandle::write(bool result, NetSerializer * out)
    {
        writeBase(result, out);
        out->read(userName);
        if(result)
        {
            out->read(passwordRecoveryQuestion);
        }
    }
    //------------------------------------------------------------------------
    void Client_GetPasswordRecoveryQuestionByHandle::read(bool result, NetSerializer * in)
    {
        readBase(result, in);
        in->write(userName);
        if(result)
        {
            in->write(passwordRecoveryQuestion);
        }
    }
    //------------------------------------------------------------------------
    bool Client_GetPasswordByPasswordRecoveryAnswer::PrevalidateInput(void)
    {
        if (!ValidateHandle(&userName)) return false;
        if (!ValidateRequiredText( &passwordRecoveryAnswer )) return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Client_GetPasswordByPasswordRecoveryAnswer::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(userName);
        in->read(passwordRecoveryAnswer);
        if(result)
        {
            in->read(password);
        }
    }
    //------------------------------------------------------------------------
    void Client_GetPasswordByPasswordRecoveryAnswer::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(userName);
        out->write(passwordRecoveryAnswer);
        if(result)
        {
            out->write(password);
        }
    }
    //------------------------------------------------------------------------
    void Client_ChangeHandle::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(userName);
        in->read(newHandle);
    }
    //------------------------------------------------------------------------
    void Client_ChangeHandle::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(userName);
        out->write(newHandle);
    }
    //------------------------------------------------------------------------
    bool Client_ChangeHandle::PrevalidateInput(void)
    {
        if(!ValidateHandle(&userName))
            return false;
        if(!ValidateHandle(&newHandle))
            return false;
        if(userName==newHandle)
        {
            resultCode=L2RC_Client_ChangeHandle_HANDLE_NOT_CHANGED;
            return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    void Client_UpdateAccount::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        createAccountParameters.write(in);
    }
    //------------------------------------------------------------------------
    void Client_UpdateAccount::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        createAccountParameters.read(out);
    }
    //------------------------------------------------------------------------
    bool Client_UpdateAccount::PrevalidateInput( void )
    {
        if (!ValidateHandle(&createAccountParameters.firstName)) return false;
        if (!ValidateHandle(&createAccountParameters.lastName)) return false;
        if (!ValidatePassword(&createAccountParameters.password)) return false;
        if (!ValidateRequiredText(&createAccountParameters.passwordRecoveryQuestion)) return false;
        if (!ValidateRequiredText(&createAccountParameters.passwordRecoveryAnswer)) return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Client_GetAccountDetails::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        if(result)
        {
            createAccountParameters.write(in);
        }
    }
    //------------------------------------------------------------------------
    void Client_GetAccountDetails::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        if(result)
        {
            createAccountParameters.read(out);
        }
    }
    //------------------------------------------------------------------------
    void Client_StartIgnore::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(targetHandle);
    }
    //------------------------------------------------------------------------
    void Client_StartIgnore::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(targetHandle);
    }
    //------------------------------------------------------------------------
    bool Client_StartIgnore::PrevalidateInput(void)
    {
        if (!ValidateHandle(&targetHandle)) return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Client_StopIgnore::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(targetHandle);
    }
    //------------------------------------------------------------------------
    void Client_StopIgnore::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(targetHandle);
    }
    //------------------------------------------------------------------------
    bool Client_StopIgnore::PrevalidateInput( void )
    {
        if (!ValidateHandle(&targetHandle)) return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Client_GetIgnoreList::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        if(result)
        {
            Nui16 listSize;
            in->readCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                RakString obj;
                in->read(obj);
                ignoredHandles.Insert(obj);
            }
        }
    }
    //------------------------------------------------------------------------
    void Client_GetIgnoreList::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        if(result)
        {
            Nui16 listSize = (Nui16)ignoredHandles.Size();
            out->writeCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                out->write(ignoredHandles[i]);
            }
        }
    }
    //------------------------------------------------------------------------
    bool Client_PerTitleIntegerStorage::PrevalidateInput(void)
    {
        if(!ValidateRequiredText(&titleName))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Client_PerTitleIntegerStorage::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(titleName);
        in->read(slotIndex);
        unsigned char c;
        c = (unsigned char)addConditionForOperation;
        in->read(c);
        addConditionForOperation = (PTIS_Condition) c;
        c = (unsigned char)operationToPerform;
        in->read(c);
        operationToPerform = (PTIS_Operation) c;
        in->read(conditionValue);
        if(operationToPerform != PTISO_DELETE)
        {
            if(operationToPerform == PTISO_ADD || operationToPerform == PTISO_WRITE)
                in->read(inputValue);
            if(result)
            {
                if(operationToPerform == PTISO_WRITE)
                    outputValue = inputValue;
                else if(operationToPerform == PTISO_READ || operationToPerform == PTISO_ADD)
                    in->read(outputValue);
            }
        }
    }
    //------------------------------------------------------------------------
    void Client_PerTitleIntegerStorage::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(titleName);
        out->write(slotIndex);
        unsigned char c;
        c = (unsigned char)addConditionForOperation;
        out->write(c);
        addConditionForOperation = (PTIS_Condition) c;
        c = (unsigned char)operationToPerform;
        out->write(c);
        operationToPerform = (PTIS_Operation) c;
        out->write(conditionValue);
        if(operationToPerform != PTISO_DELETE)
        {
            if(operationToPerform == PTISO_ADD || operationToPerform == PTISO_WRITE)
                out->write(inputValue);
            if(result)
            {
                if(operationToPerform == PTISO_WRITE)
                    outputValue = inputValue;
                else if(operationToPerform == PTISO_READ || operationToPerform == PTISO_ADD)
                    out->write(outputValue);
            }
        }
    }
    //------------------------------------------------------------------------
    bool Client_PerTitleBinaryStorage::PrevalidateInput(void)
    {
        if (!ValidateRequiredText(&titleName))
            return false;
        if (!ValidateBinary(binaryData))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Client_PerTitleBinaryStorage::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(titleName);
        in->read(slotIndex);
        if(operationToPerform != PTISO_DELETE)
            binaryData->write(in);
    }
    //------------------------------------------------------------------------
    void Client_PerTitleBinaryStorage::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(titleName);
        out->write(slotIndex);
        if(operationToPerform != PTISO_DELETE)
            binaryData->read(out);
    }
    //------------------------------------------------------------------------
    void Client_SetPresence::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        presence.write(in);
    }
    //------------------------------------------------------------------------
    void Client_SetPresence::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        presence.read(out);
    }
    //------------------------------------------------------------------------
    void Client_GetPresence::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(userHandle);
        if(result)
        {
            presence.write(in);
        }
    }
    //------------------------------------------------------------------------
    void Client_GetPresence::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(userHandle);
        if(result)
        {
            presence.read(out);
        }
    }
    //------------------------------------------------------------------------
    void Friends_SendInvite::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(targetHandle);
        in->read(subject);
        in->read(body);
        in->read(emailStatus);
        binaryData->write(in);
    }
    //------------------------------------------------------------------------
    void Friends_SendInvite::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(targetHandle);
        out->write(subject);
        out->write(body);
        out->write(emailStatus);
        binaryData->read(out);
    }
    //------------------------------------------------------------------------
    bool Friends_SendInvite::PrevalidateInput(void)
    {
        if (!ValidateHandle(&targetHandle))
            return false;
        if (!ValidateBinary(binaryData))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Friends_AcceptInvite::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(targetHandle);
        in->read(subject);
        in->read(body);
        in->read(emailStatus);
        binaryData->write(in);
        if(result)
            presence.write(in);
    }
    //------------------------------------------------------------------------
    void Friends_AcceptInvite::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(targetHandle);
        out->write(subject);
        out->write(body);
        out->write(emailStatus);
        binaryData->read(out);
        if(result)
            presence.read(out);
    }
    //------------------------------------------------------------------------
    bool Friends_AcceptInvite::PrevalidateInput(void)
    {
        if (!ValidateHandle(&targetHandle))
            return false;
        if (!ValidateBinary(binaryData))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Friends_RejectInvite::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(targetHandle);
        in->read(subject);
        in->read(body);
        in->read(emailStatus);
        binaryData->write(in);
    }
    //------------------------------------------------------------------------
    void Friends_RejectInvite::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(targetHandle);
        out->write(subject);
        out->write(body);
        out->write(emailStatus);
        binaryData->read(out);
    }
    //------------------------------------------------------------------------
    bool Friends_RejectInvite::PrevalidateInput(void)
    {
        if (!ValidateHandle(&targetHandle))
            return false;
        if (!ValidateBinary(binaryData))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Friends_GetInvites::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        if(result)
        {
            Nui16 listSize;
            in->readCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                FriendInfo obj;
                obj.write(in);
                invitesSent.Insert(obj);
            }
            in->readCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                FriendInfo obj;
                obj.write(in);
                invitesReceived.Insert(obj);
            }
        }
    }
    //------------------------------------------------------------------------
    void Friends_GetInvites::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        if(result)
        {
            Nui16 listSize = (Nui16) invitesSent.Size();
            out->writeCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                invitesSent[i].read(out);
            }
            listSize = (Nui16) invitesReceived.Size();
            out->writeCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                invitesReceived[i].read(out);
            }
        }
    }
    //------------------------------------------------------------------------
    void Friends_GetFriends::write(NetSerializer * in)
    {
        writeBase(result, in);
        if(result)
        {
            Nui16 listSize;
            in->readCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                FriendInfo obj;
                obj.write(in);
                myFriends.Insert(obj);
            }
        }
    }
    //------------------------------------------------------------------------
    void Friends_GetFriends::read(NetSerializer * out)
    {
        readBase(result, out);
        if(result)
        {
            Nui16 listSize = (Nui16) myFriends.Size();
            out->writeCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                myFriends[i].read(out);
            }
        }
    }
    //------------------------------------------------------------------------
    void Friends_Remove::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(targetHandle);
        in->read(subject);
        in->read(body);
        in->read(emailStatus);
        binaryData->write(in);
    }
    //------------------------------------------------------------------------
    void Friends_Remove::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(targetHandle);
        out->write(subject);
        out->write(body);
        out->write(emailStatus);
        binaryData->read(out);
    }
    //------------------------------------------------------------------------
    bool Friends_Remove::PrevalidateInput( void )
    {
        if (!ValidateHandle(&targetHandle))
            return false;
        if (!ValidateBinary(binaryData))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void BookmarkedUsers_Add::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(targetHandle);
        in->read(type);
        in->read(description);
    }
    //------------------------------------------------------------------------
    void BookmarkedUsers_Add::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(targetHandle);
        out->write(type);
        out->write(description);
    }
    //------------------------------------------------------------------------
    bool BookmarkedUsers_Add::PrevalidateInput(void)
    {
        if (!ValidateHandle(&targetHandle)) return false;
        return true;
    }
    //------------------------------------------------------------------------
    void BookmarkedUsers_Remove::read(bool result, NetSerializer * org)
    {
        readBase(result, org);
        org->write(targetHandle);
        org->write(type);
    }
    //------------------------------------------------------------------------
    void BookmarkedUsers_Remove::write(bool result, NetSerializer * org)
    {
        writeBase(result, org);
        org->read(targetHandle);
        org->read(type);
    }
    //------------------------------------------------------------------------
    bool BookmarkedUsers_Remove::PrevalidateInput(void)
    {
        if (!ValidateHandle(&targetHandle)) return false;
        return true;
    }
    //------------------------------------------------------------------------
    void BookmarkedUsers_Get::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        if(result)
        {
            Nui16 listSize;
            in->readCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                BookmarkedUser obj;
                obj.write(in);
                bookmarkedUsers.Insert(obj);
            }
        }
    }
    //------------------------------------------------------------------------
    void BookmarkedUsers_Get::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        if(result)
        {
            Nui16 listSize = (Nui16)bookmarkedUsers.Size();
            out->writeCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                bookmarkedUsers[i].read(out);
            }
        }
    }
    //------------------------------------------------------------------------
    void Emails_Send::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(subject);
        in->read(body);
        in->read(status);
        binaryData->write(in);
        Nui16 listSize;
        in->readCompress(listSize);
        for(NCount i = 0; i < listSize; ++i)
        {
            RakString obj;
            in->read(obj);
            recipients.Insert(obj);
        }
    }
    //------------------------------------------------------------------------
    void Emails_Send::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(subject);
        out->write(body);
        out->write(status);
        binaryData->read(out);
        Nui16 listSize = (Nui16) recipients.Size();
        out->writeCompress(listSize);
        for(NCount i = 0; i < listSize; ++i)
        {
            out->write(recipients[i]);
        }
    }
    //------------------------------------------------------------------------
    bool Emails_Send::PrevalidateInput(void)
    {
        for (NCount i = 0; i < recipients.Size(); ++i)
            if (!ValidateHandle(&recipients[i]))
                return false;
        if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body))
            return false;
        if (!ValidateBinary(binaryData))
            return false;
        if (recipients.Size()==0)
        {
            resultCode=L2RC_Emails_Send_NO_RECIPIENTS;
            return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    void Emails_Get::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);

        Nui16 listSize;

        in->read(unreadEmailsOnly);
        in->read(emailIdsOnly);

        listSize = (Nui16)emailsToRetrieve.Size();
        in->readCompress(listSize);
        for(NCount i = 0; i < listSize; ++i)
        {
            Nui32 id;
            in->read(id);
            emailsToRetrieve.Push(id);
        }

        if(result)
        {
            listSize = (Nui16)emailResults.Size();
            in->readCompress(listSize);
            for (NCount i = 0; i < listSize; ++i)
            {
                EmailResult obj;
                obj.write(in);
                emailResults.Insert(obj);

            }
        }
    }
    //------------------------------------------------------------------------
    void Emails_Get::read(bool result, NetSerializer * out)
    {
        readBase(result, out);

        Nui16 listSize;

        out->write(unreadEmailsOnly);
        out->write(emailIdsOnly);

        listSize = (Nui16)emailsToRetrieve.Size();
        out->writeCompress(listSize);
        for(NCount i = 0; i < listSize; ++i)
        {
            out->write(emailsToRetrieve[i]);
        }

        if(result)
        {
            listSize = (Nui16)emailResults.Size();
            out->writeCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                emailResults[i].read(out);
            }
        }
    }
    //------------------------------------------------------------------------
    void Emails_Delete::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(emailId);
    }
    //------------------------------------------------------------------------
    void Emails_Delete::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(emailId);
    }
    //------------------------------------------------------------------------
    void Emails_SetStatus::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(emailId);
        in->read(updateStatusFlag);
        in->read(updateMarkedRead);
        in->read(newStatusFlag);
        in->read(isNowMarkedRead);
    }
    //------------------------------------------------------------------------
    void Emails_SetStatus::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(emailId);
        out->write(updateStatusFlag);
        out->write(updateMarkedRead);
        out->write(newStatusFlag);
        out->write(isNowMarkedRead);
    }
    //------------------------------------------------------------------------
    bool Emails_SetStatus::PrevalidateInput(void)
    {
        if (updateStatusFlag==false && updateMarkedRead==false)
        {
            resultCode=L2RC_Emails_SetStatus_NOTHING_TO_DO;
            return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    void Ranking_SubmitMatch::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(titleName);
        in->read(gameType);
        submittedMatch.write(in);
    }
    //------------------------------------------------------------------------
    void Ranking_SubmitMatch::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(titleName);
        out->write(gameType);
        submittedMatch.read(out);
    }
    //------------------------------------------------------------------------
    bool Ranking_SubmitMatch::PrevalidateInput(void)
    {
        if(!ValidateRequiredText(&titleName))
            return false;
        if(!ValidateRequiredText(&gameType))
            return false;
        if(!ValidateBinary(submittedMatch.binaryData))
            return false;
        if(submittedMatch.matchParticipants.Size() == 0)
        {
            resultCode=L2RC_Ranking_SubmitMatch_NO_PARTICIPANTS;
            return false;
        }
        for(NCount i = 0; i < submittedMatch.matchParticipants.Size(); ++i)
            if (!ValidateHandle(&submittedMatch.matchParticipants[i].handle))
                return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Ranking_GetMatches::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(titleName);
        in->read(gameType);
        if(result)
        {
            Nui16 listSize;
            in->readCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                SubmittedMatch obj;
                in->read(obj);
                submittedMatches.Insert(obj);
            }
        }
    }
    //------------------------------------------------------------------------
    void Ranking_GetMatches::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(titleName);
        out->write(gameType);
        if(result)
        {
            Nui16 listSize = (Nui16)submittedMatches.Size();
            out->writeCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                out->write(submittedMatches[i]);
            }
        }
    }
    //------------------------------------------------------------------------
    bool Ranking_GetMatches::PrevalidateInput(void)
    {
        if(!ValidateRequiredText(&titleName))
            return false;
        if(!ValidateRequiredText(&gameType))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Ranking_GetMatchBinaryData::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(matchID);
        if(result)
        {
            binaryData->write(in);
        }
    }
    //------------------------------------------------------------------------
    void Ranking_GetMatchBinaryData::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(matchID);
        if(result)
        {
            binaryData->read(out);
        }
    }
    //------------------------------------------------------------------------
    void Ranking_GetTotalScore::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(titleName);
        in->read(gameType);
        in->read(targetHandle);
        if(result)
        {
            in->read(scoreSum);
            in->read(numScoresSubmitted);
        }
    }
    //------------------------------------------------------------------------
    void Ranking_GetTotalScore::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(titleName);
        out->write(gameType);
        out->write(targetHandle);
        if(result)
        {
            out->write(scoreSum);
            out->write(numScoresSubmitted);
        }
    }
    //------------------------------------------------------------------------
    bool Ranking_GetTotalScore::PrevalidateInput(void)
    {
        if(!ValidateHandle(&targetHandle))
            return false;
        if(!ValidateRequiredText(&titleName))
            return false;
        if(!ValidateRequiredText(&gameType))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Ranking_WipeScoresForPlayer::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(titleName);
        in->read(gameType);
        in->read(targetHandle);
    }
    //------------------------------------------------------------------------
    void Ranking_WipeScoresForPlayer::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(titleName);
        out->write(gameType);
        out->write(targetHandle);
    }
    //------------------------------------------------------------------------
    bool Ranking_WipeScoresForPlayer::PrevalidateInput(void)
    {
        if (!ValidateHandle(&targetHandle))
            return false;
        if (!ValidateRequiredText(&titleName))
            return false;
        if (!ValidateRequiredText(&gameType))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Ranking_WipeMatches::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(titleName);
        in->read(gameType);
    }
    //------------------------------------------------------------------------
    void Ranking_WipeMatches::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(titleName);
        out->write(gameType);
    }
    //------------------------------------------------------------------------
    bool Ranking_WipeMatches::PrevalidateInput(void)
    {
        if (!ValidateRequiredText(&titleName))
            return false;
        if (!ValidateRequiredText(&gameType))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Ranking_PruneMatches::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(pruneTimeDays);
    }
    //------------------------------------------------------------------------
    void Ranking_PruneMatches::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(pruneTimeDays);
    }
    //------------------------------------------------------------------------
    void Ranking_UpdateRating::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(titleName);
        in->read(gameType);
        in->read(targetHandle);
        in->read(targetRating);
    }
    //------------------------------------------------------------------------
    void Ranking_UpdateRating::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(titleName);
        out->write(gameType);
        out->write(targetHandle);
        out->write(targetRating);
    }
    //------------------------------------------------------------------------
    bool Ranking_UpdateRating::PrevalidateInput(void)
    {
        if (!ValidateRequiredText(&titleName))
            return false;
        if (!ValidateRequiredText(&gameType))
            return false;
        if (!ValidateHandle(&targetHandle))
            return false;

        return true;
    }
    //------------------------------------------------------------------------
    void Ranking_WipeRatings::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(titleName);
        in->read(gameType);
    }
    //------------------------------------------------------------------------
    void Ranking_WipeRatings::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(titleName);
        out->write(gameType);
    }
    //------------------------------------------------------------------------
    bool Ranking_WipeRatings::PrevalidateInput(void)
    {
        if (!ValidateRequiredText(&titleName))
            return false;
        if (!ValidateRequiredText(&gameType))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Ranking_GetRating::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(titleName);
        in->read(gameType);
        in->read(targetHandle);
        if(result)
            in->read(currentRating);
    }
    //------------------------------------------------------------------------
    void Ranking_GetRating::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(titleName);
        out->write(gameType);
        out->write(targetHandle);
        if(result)
            out->write(currentRating);
    }
    //------------------------------------------------------------------------
    bool Ranking_GetRating::PrevalidateInput(void)
    {
        if (!ValidateRequiredText(&titleName))
            return false;
        if (!ValidateRequiredText(&gameType))
            return false;
        if (!ValidateHandle(&targetHandle))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_Create::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(failIfAlreadyInClan);
        in->read(requiresInvitationsToJoin);
        in->read(description);
        binaryData->write(in);
    }
    //------------------------------------------------------------------------
    void Clans_Create::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(failIfAlreadyInClan);
        out->write(requiresInvitationsToJoin);
        out->write(description);
        binaryData->read(out);
    }
    //------------------------------------------------------------------------
    bool Clans_Create::PrevalidateInput(void)
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_SetProperties::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(description);
        binaryData->write(in);
    }
    //------------------------------------------------------------------------
    void Clans_SetProperties::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(description);
        binaryData->read(out);
    }
    //------------------------------------------------------------------------
    bool Clans_SetProperties::PrevalidateInput(void)
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        if (!ValidateBinary(binaryData))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_GetProperties::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        if(result)
        {
            in->read(description);
            binaryData->write(in);
        }
    }
    //------------------------------------------------------------------------
    void Clans_GetProperties::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        if(result)
        {
            out->write(description);
            binaryData->read(out);
        }
    }
    //------------------------------------------------------------------------
    bool Clans_GetProperties::PrevalidateInput(void)
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_SetMyMemberProperties::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(description);
        binaryData->write(in);
    }
    //------------------------------------------------------------------------
    void Clans_SetMyMemberProperties::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(description);
        binaryData->read(out);
    }
    //------------------------------------------------------------------------
    bool Clans_SetMyMemberProperties::PrevalidateInput(void)
    {
        if(!ValidateHandle(&clanHandle))
            return false;
        if(!ValidateBinary(binaryData))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_GrantLeader::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(targetHandle);
    }
    //------------------------------------------------------------------------
    void Clans_GrantLeader::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(targetHandle);
    }
    //------------------------------------------------------------------------
    bool Clans_GrantLeader::PrevalidateInput(void)
    {
        if(!ValidateHandle(&clanHandle))
            return false;
        if(!ValidateHandle(&targetHandle))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_SetSubleaderStatus::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(targetHandle);
        in->read(setToSubleader);
    }
    //------------------------------------------------------------------------
    void Clans_SetSubleaderStatus::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(targetHandle);
        out->write(setToSubleader);
    }
    //------------------------------------------------------------------------
    bool Clans_SetSubleaderStatus::PrevalidateInput(void)
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        if (!ValidateHandle(&targetHandle))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_SetMemberRank::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(targetHandle);
        in->read(newRank);
    }
    //------------------------------------------------------------------------
    void Clans_SetMemberRank::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(targetHandle);
        out->write(newRank);
    }
    //------------------------------------------------------------------------
    bool Clans_SetMemberRank::PrevalidateInput(void)
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        if (!ValidateHandle(&targetHandle))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_GetMemberProperties::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(targetHandle);
        if(result)
        {
            in->read(description);
            in->read(rank);
            binaryData->write(in);
            in->read(isSubleader);
            in->read(clanMemberState);
            in->read(banReason);
        }
    }
    //------------------------------------------------------------------------
    void Clans_GetMemberProperties::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(targetHandle);
        if(result)
        {
            out->write(description);
            out->write(rank);
            binaryData->read(out);
            out->write(isSubleader);
            out->write(clanMemberState);
            out->write(banReason);
        }
    }
    //------------------------------------------------------------------------
    bool Clans_GetMemberProperties::PrevalidateInput(void)
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_ChangeHandle::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(oldClanHandle);
        in->read(newClanHandle);
    }
    //------------------------------------------------------------------------
    void Clans_ChangeHandle::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(oldClanHandle);
        out->write(newClanHandle);
    }
    //------------------------------------------------------------------------
    bool Clans_ChangeHandle::PrevalidateInput(void)
    {
        if (!ValidateHandle(&oldClanHandle))
            return false;
        if (!ValidateHandle(&newClanHandle))
            return false;
        if (oldClanHandle.StrICmp(newClanHandle)==0)
        {
            resultCode=L2RC_Clans_ChangeHandle_HANDLE_NOT_CHANGED;
            return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_Leave::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(dissolveIfClanLeader);
        in->read(subject);
        in->read(body);
        binaryData->write(in);
        if(result)
        {
            in->read(wasDissolved);
            in->read(newClanLeader);
        }
    }
    //------------------------------------------------------------------------
    void Clans_Leave::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(dissolveIfClanLeader);
        out->write(subject);
        out->write(body);
        binaryData->read(out);
        if(result)
        {
            out->write(wasDissolved);
            out->write(newClanLeader);
        }
    }
    //------------------------------------------------------------------------
    bool Clans_Leave::PrevalidateInput(void)
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_Get::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        if(result)
        {
            Nui16 listSize = (Nui16)clans.Size();
            in->readCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                ClanInfo obj;
                obj.write(in);
                clans.Insert(obj);
            }
        }
    }
    //------------------------------------------------------------------------
    void Clans_Get::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        if(result)
        {
            Nui16 listSize = (Nui16)clans.Size();
            out->writeCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                clans[i].read(out);
            }
        }
    }
    //------------------------------------------------------------------------
    void Clans_SendJoinInvitation::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(targetHandle);
        in->read(subject);
        in->read(body);
        in->read(emailStatus);
        binaryData->write(in);
    }
    //------------------------------------------------------------------------
    void Clans_SendJoinInvitation::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(targetHandle);
        out->write(subject);
        out->write(body);
        out->write(emailStatus);
        binaryData->read(out);
    }
    //------------------------------------------------------------------------
    bool Clans_SendJoinInvitation::PrevalidateInput(void)
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        if (!ValidateHandle(&targetHandle))
            return false;
        if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_WithdrawJoinInvitation::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(targetHandle);
        in->read(subject);
        in->read(body);
        in->read(emailStatus);
        binaryData->write(in);
    }
    //------------------------------------------------------------------------
    void Clans_WithdrawJoinInvitation::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(targetHandle);
        out->write(subject);
        out->write(body);
        out->write(emailStatus);
        binaryData->read(out);
    }
    //------------------------------------------------------------------------
    bool Clans_WithdrawJoinInvitation::PrevalidateInput( void )
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        if (!ValidateHandle(&targetHandle))
            return false;
        if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_AcceptJoinInvitation::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(subject);
        in->read(body);
        in->read(emailStatus);
        binaryData->write(in);
        in->read(failIfAlreadyInClan);
        binaryData->write(in);
    }
    //------------------------------------------------------------------------
    void Clans_AcceptJoinInvitation::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(subject);
        out->write(body);
        out->write(emailStatus);
        binaryData->read(out);
        out->write(failIfAlreadyInClan);
        binaryData->read(out);
    }
    //------------------------------------------------------------------------
    bool Clans_AcceptJoinInvitation::PrevalidateInput( void )
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_RejectJoinInvitation::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(subject);
        in->read(body);
        in->read(emailStatus);
        binaryData->write(in);
        in->read(clanHandle);
    }
    //------------------------------------------------------------------------
    void Clans_RejectJoinInvitation::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(subject);
        out->write(body);
        out->write(emailStatus);
        binaryData->read(out);
        out->write(clanHandle);
    }
    //------------------------------------------------------------------------
    bool Clans_RejectJoinInvitation::PrevalidateInput(void)
    {
        if(!ValidateHandle(&clanHandle))
            return false;
        if(!ValidateRequiredText(&subject) && !ValidateRequiredText(&body))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_DownloadInvitationList::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        if(result)
        {
            Nui16 listSize;
            in->readCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                OpenInvite obj;
                obj.write(in);
                invitationsSentToMe.Insert(obj);
            }
            in->readCompress(listSize);
            for(NCount i = 0; i < listSize; i++)
            {
                ClanJoinInvite obj;
                obj.write(in);
                usersThatHaveAnInvitationFromClansThatIAmAMemberOf.Insert(obj);
            }
        }
    }
    //------------------------------------------------------------------------
    void Clans_DownloadInvitationList::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        if(result)
        {
            Nui16 listSize = (Nui16)invitationsSentToMe.Size();
            out->writeCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                invitationsSentToMe[i].read(out);
            }

            listSize = (Nui16)usersThatHaveAnInvitationFromClansThatIAmAMemberOf.Size();
            out->writeCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                usersThatHaveAnInvitationFromClansThatIAmAMemberOf[i].read(out);
            }
        }
    }
    //------------------------------------------------------------------------
    void Clans_SendJoinRequest::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(subject);
        in->read(body);
        in->read(emailStatus);
        binaryData->write(in);
        if (result)
        {
            in->read(clanJoined);
        }
    }
    //------------------------------------------------------------------------
    void Clans_SendJoinRequest::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(subject);
        out->write(body);
        out->write(emailStatus);
        binaryData->read(out);
        if(result)
        {
            out->write(clanJoined);
        }
    }
    //------------------------------------------------------------------------
    bool Clans_SendJoinRequest::PrevalidateInput(void)
    {

        if (!ValidateHandle(&clanHandle))
            return false;
        if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_WithdrawJoinRequest::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(subject);
        in->read(body);
        in->read(emailStatus);
        binaryData->write(in);
    }
    //------------------------------------------------------------------------
    void Clans_WithdrawJoinRequest::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(subject);
        out->write(body);
        out->write(emailStatus);
        binaryData->read(out);
    }
    //------------------------------------------------------------------------
    bool Clans_WithdrawJoinRequest::PrevalidateInput(void)
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_AcceptJoinRequest::write(bool result, NetSerializer * out)
    {
        writeBase(result, out);
        out->read(clanHandle);
        out->read(subject);
        out->read(body);
        out->read(emailStatus);
        binaryData->write(out);
        out->read(requestingUserHandle);
        out->read(failIfAlreadyInClan);
    }
    //------------------------------------------------------------------------
    void Clans_AcceptJoinRequest::read(bool result, NetSerializer * in)
    {
        readBase(result, in);
        in->write(clanHandle);
        in->write(subject);
        in->write(body);
        in->write(emailStatus);
        binaryData->read(in);
        in->write(requestingUserHandle);
        in->write(failIfAlreadyInClan);
    }
    //------------------------------------------------------------------------
    bool Clans_AcceptJoinRequest::PrevalidateInput(void)
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        if (!ValidateHandle(&requestingUserHandle))
            return false;
        if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_RejectJoinRequest::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(subject);
        in->read(body);
        in->read(emailStatus);
        binaryData->write(in);
        in->read(requestingUserHandle);
    }
    //------------------------------------------------------------------------
    void Clans_RejectJoinRequest::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(subject);
        out->write(body);
        out->write(emailStatus);
        binaryData->read(out);
        out->write(requestingUserHandle);
    }
    //------------------------------------------------------------------------
    bool Clans_RejectJoinRequest::PrevalidateInput(void)
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        if (!ValidateHandle(&requestingUserHandle))
            return false;
        if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_DownloadRequestList::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        if(result)
        {
            Nui16 listSize;
            in->readCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                ClanJoinRequest obj;
                obj.write(in);
                joinRequestsToMyClan.Insert(obj);
            }

            in->readCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                ClanJoinRequest obj;
                obj.write(in);
                joinRequestsFromMe.Insert(obj);
            }
        }
    }
    //------------------------------------------------------------------------
    void Clans_DownloadRequestList::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        if(result)
        {
            Nui16 listSize = (Nui16)joinRequestsToMyClan.Size();
            out->writeCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                joinRequestsToMyClan[i].write(out);
            }

            listSize = (Nui16) joinRequestsFromMe.Size();
            out->writeCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                joinRequestsFromMe[i].write(out);
            }
        }
    }
    //------------------------------------------------------------------------
    void Clans_KickAndBlacklistUser::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(subject);
        in->read(body);
        in->read(targetHandle);
        in->read(kick);
        in->read(blacklist);
        in->read(reason);
        in->read(emailStatus);
        binaryData->write(in);
    }
    //------------------------------------------------------------------------
    void Clans_KickAndBlacklistUser::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(subject);
        out->write(body);
        out->write(targetHandle);
        out->write(kick);
        out->write(blacklist);
        out->write(reason);
        out->write(emailStatus);
        binaryData->read(out);
    }
    //------------------------------------------------------------------------
    bool Clans_KickAndBlacklistUser::PrevalidateInput(void)
    {

        if (!ValidateHandle(&clanHandle))
            return false;
        if (!ValidateHandle(&targetHandle))
            return false;
        if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body))
            return false;
        if (!ValidateRequiredText(&reason))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_UnblacklistUser::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(subject);
        in->read(body);
        in->read(targetHandle);
        in->read(emailStatus);
        binaryData->write(in);
    }
    //------------------------------------------------------------------------
    void Clans_UnblacklistUser::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(subject);
        out->write(body);
        out->write(targetHandle);
        out->write(emailStatus);
        binaryData->read(out);
    }
    //------------------------------------------------------------------------
    bool Clans_UnblacklistUser::PrevalidateInput(void)
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        if (!ValidateHandle(&targetHandle))
            return false;
        if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_GetBlacklist::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        if(result)
        {
            Nui16 listSize;
            in->readCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                RakString obj;
                in->write(obj);
                blacklistedUsers.Insert(obj);
            }
        }
    }
    //------------------------------------------------------------------------
    void Clans_GetBlacklist::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        if(result)
        {
            Nui16 listSize = (Nui16)blacklistedUsers.Size();
            out->writeCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                out->read(blacklistedUsers[i]);
            }
        }
    }
    //------------------------------------------------------------------------
    bool Clans_GetBlacklist::PrevalidateInput(void)
    {
        for(NCount i = 0; i < blacklistedUsers.Size(); ++i)
            if(!ValidateHandle(&blacklistedUsers[i]))
                return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_GetMembers::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        if(result)
        {
            in->read(clanLeader);

            Nui16 listSize;
            in->readCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                RakString obj;
                in->read(obj);
                clanMembersOtherThanLeader.Insert(obj);
            }
        }
    }
    //------------------------------------------------------------------------
    void Clans_GetMembers::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        if (result)
        {
            out->write(clanLeader);

            Nui16 listSize = (Nui16)clanMembersOtherThanLeader.Size();
            out->writeCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                out->write(clanMembersOtherThanLeader[i]);
            }
        }
    }
    //------------------------------------------------------------------------
    bool Clans_GetMembers::PrevalidateInput(void)
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_GetList::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        if(result)
        {
            Nui16 listSize;
            in->readCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                RakString obj;
                in->read(obj);
                clanNames.Insert(obj);
            }
        }
    }
    //------------------------------------------------------------------------
    void Clans_GetList::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        if(result)
        {
            Nui16 listSize = (Nui16)clanNames.Size();
            out->writeCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                out->write(clanNames[i]);
            }
        }
    }
    //------------------------------------------------------------------------
    void Clans_CreateBoard::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(clanBoardName);
        in->read(allowPublicReads);
        in->read(allowPublicWrites);
        in->read(description);
        binaryData->write(in);
    }
    //------------------------------------------------------------------------
    void Clans_CreateBoard::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(clanBoardName);
        out->write(allowPublicReads);
        out->write(allowPublicWrites);
        out->write(description);
        binaryData->read(out);
    }
    //------------------------------------------------------------------------
    bool Clans_CreateBoard::PrevalidateInput(void)
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        if (!ValidateHandle(&clanBoardName))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_DestroyBoard::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(clanBoardName);
    }
    //------------------------------------------------------------------------
    void Clans_DestroyBoard::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(clanBoardName);
    }
    //------------------------------------------------------------------------
    bool Clans_DestroyBoard::PrevalidateInput(void)
    {
        if(!ValidateHandle(&clanHandle))
            return false;
        if(!ValidateHandle(&clanBoardName))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_CreateNewTopic::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(clanBoardName);
        in->read(body);
        in->read(subject);
        binaryData->write(in);

        if(result)
        {
            in->read(postId);
        }
    }
    //------------------------------------------------------------------------
    void Clans_CreateNewTopic::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(clanBoardName);
        out->write(body);
        out->write(subject);
        binaryData->read(out);

        if(result)
        {
            out->write(postId);
        }
    }
    //------------------------------------------------------------------------
    bool Clans_CreateNewTopic::PrevalidateInput(void)
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        if (!ValidateHandle(&clanBoardName))
            return false;
        if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body))
            return false;
        if (!ValidateBinary(binaryData))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_ReplyToTopic::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(postId);
        in->read(body);
        in->read(subject);
        binaryData->write(in);
    }
    //------------------------------------------------------------------------
    void Clans_ReplyToTopic::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(postId);
        out->write(body);
        out->write(subject);
        binaryData->read(out);
    }
    //------------------------------------------------------------------------
    bool Clans_ReplyToTopic::PrevalidateInput(void)
    {
        if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body))
            return false;
        if (!ValidateBinary(binaryData))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_RemovePost::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(postId);
        in->read(removeEntireTopic);
    }
    //------------------------------------------------------------------------
    void Clans_RemovePost::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(postId);
        out->write(removeEntireTopic);
    }
    //------------------------------------------------------------------------
    void Clans_GetBoards::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        if(result)
        {
            Nui16 listSize;
            in->readCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                RakString obj;
                in->read(obj);
                clanBoardsNames.Insert(obj);
            }
        }
    }
    //------------------------------------------------------------------------
    void Clans_GetBoards::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        if(result)
        {
            Nui16 listSize = (Nui16)clanBoardsNames.Size();
            out->writeCompress(listSize);
            for(NCount i = 0; i < listSize; ++i)
            {
                out->write(clanBoardsNames[i]);
            }
        }
    }
    //------------------------------------------------------------------------
    bool Clans_GetBoards::PrevalidateInput(void)
    {
        if(!ValidateHandle(&clanHandle))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_GetTopics::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(clanBoardName);
    }
    //------------------------------------------------------------------------
    void Clans_GetTopics::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(clanBoardName);
    }
    //------------------------------------------------------------------------
    bool Clans_GetTopics::PrevalidateInput(void)
    {
        if (!ValidateHandle(&clanHandle))
            return false;
        if (!ValidateHandle(&clanBoardName))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void Clans_GetPosts::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(postId);
    }
    //------------------------------------------------------------------------
    void Clans_GetPosts::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(postId);
    }
    //------------------------------------------------------------------------
    bool Clans_GetPosts::PrevalidateInput(void)
    {
        return true;
    }
    //------------------------------------------------------------------------
    void Notification_Client_RemoteLogin::write(bool result, NetSerializer * in)
    {
        readBase(result, in);
        in->read(handle);
    }
    //------------------------------------------------------------------------
    void Notification_Client_RemoteLogin::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(handle);
    }
    //------------------------------------------------------------------------
    void Notification_Client_IgnoreStatus::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(nowIgnored);
        in->read(otherHandle);
    }
    //------------------------------------------------------------------------
    void Notification_Client_IgnoreStatus::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(nowIgnored);
        out->write(otherHandle);
    }
    //------------------------------------------------------------------------
    void Notification_Friends_StatusChange::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(op);
        in->read(otherHandle);
        in->read(subject);
        in->read(body);
        presence.write(in);
    }
    //------------------------------------------------------------------------
    void Notification_Friends_StatusChange::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(op);
        out->write(otherHandle);
        out->write(subject);
        out->write(body);
        presence.read(out);
    }
    //------------------------------------------------------------------------
    void Notification_Friends_PresenceUpdate::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        newPresence.write(in);
        in->read(otherHandle);
    }
    //------------------------------------------------------------------------
    void Notification_Friends_PresenceUpdate::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        newPresence.read(out);
        out->write(otherHandle);
    }
    //------------------------------------------------------------------------
    void Notification_User_ChangedHandle::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(oldHandle);
        in->read(newHandle);
    }
    //------------------------------------------------------------------------
    void Notification_User_ChangedHandle::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(oldHandle);
        out->write(newHandle);
    }
    //------------------------------------------------------------------------
    void Notification_Friends_CreatedClan::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(otherHandle);
        in->read(clanName);
    }
    //------------------------------------------------------------------------
    void Notification_Friends_CreatedClan::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(otherHandle);
        out->write(clanName);
    }
    //------------------------------------------------------------------------
    void Notification_Emails_Received::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(sender);
        in->read(subject);
        in->read(emailId);
    }
    //------------------------------------------------------------------------
    void Notification_Emails_Received::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(sender);
        out->write(subject);
        out->write(emailId);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_GrantLeader::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(newLeader);
        in->read(oldLeader);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_GrantLeader::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(newLeader);
        out->write(oldLeader);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_SetSubleaderStatus::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(targetHandle);
        in->read(leaderHandle);
        in->read(setToSubleader);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_SetSubleaderStatus::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(targetHandle);
        out->write(leaderHandle);
        out->write(setToSubleader);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_SetMemberRank::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(targetHandle);
        in->read(leaderHandle);
        in->read(newRank);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_SetMemberRank::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(targetHandle);
        out->write(leaderHandle);
        out->write(newRank);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_ChangeHandle::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(oldClanHandle);
        in->read(newClanHandle);
        in->read(leaderHandle);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_ChangeHandle::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(oldClanHandle);
        out->write(newClanHandle);
        out->write(leaderHandle);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_Leave::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(targetHandle);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_Leave::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(targetHandle);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_PendingJoinStatus::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(sourceHandle);
        in->read(targetHandle);
        in->read(clanMemberHandle);
        unsigned char c1;
        unsigned char c2;
        in->read(c1);
        in->read(c2);
        majorOp=(Notification_Clans_PendingJoinStatus::MajorOp) c1;
        minorOp=(Notification_Clans_PendingJoinStatus::MinorOp) c2;
    }
    //------------------------------------------------------------------------
    void Notification_Clans_PendingJoinStatus::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(sourceHandle);
        out->write(targetHandle);
        out->write(clanMemberHandle);
        unsigned char c1 = (unsigned char) majorOp;
        unsigned char c2 = (unsigned char) minorOp;
        out->write(write, c1);
        out->write(write, c2);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_NewClanMember::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(targetHandle);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_NewClanMember::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(targetHandle);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_KickAndBlacklistUser::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(targetHandle);
        in->read(blacklistingUserHandle);
        in->read(targetHandleWasKicked);
        in->read(reason);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_KickAndBlacklistUser::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(targetHandle);
        out->write(blacklistingUserHandle);
        out->write(targetHandleWasKicked);
        out->write(reason);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_UnblacklistUser::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(clanHandle);
        in->read(targetHandle);
        in->read(unblacklistingUserHandle);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_UnblacklistUser::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(clanHandle);
        out->write(targetHandle);
        out->write(unblacklistingUserHandle);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_Destroyed::write(bool result, NetSerializer * in)
    {
        writeBase(result, in);
        in->read(oldClanLeader);
        in->read(clanHandle);
    }
    //------------------------------------------------------------------------
    void Notification_Clans_Destroyed::read(bool result, NetSerializer * out)
    {
        readBase(result, out);
        out->write(oldClanLeader);
        out->write(clanHandle);
    }
    //------------------------------------------------------------------------
    bool Client_StartIgnore::ClientImpl(Lobby2Plugin * client)
    {
        (void)client;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_StopIgnore::ClientImpl(Lobby2Plugin * client)
    {
        (void)client;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_GetIgnoreList::ClientImpl(Lobby2Plugin * client)
    {
        (void)client;
        return true;
    }
    //------------------------------------------------------------------------
}
}