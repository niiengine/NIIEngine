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
#include "Lobby2Server.h"
// libpq-fe.h is part of PostgreSQL which must be installed on this computer to use the PostgreRepository
#include "libpq-fe.h"
#include "PostgreSQLInterface.h"
#include "EpochTimeToString.h"

#ifndef __LOBBY_2_MESSAGE_PGSQL_H
#define __LOBBY_2_MESSAGE_PGSQL_H

namespace NII
{
namespace NII_NET
{
    // ---------- Database specific message implementations for the server --------
    #define __L2_MSG_DB_HEADER(__NAME__,__DB__) \
    struct __NAME__##_##__DB__ : public __NAME__

    struct ClanMemberDescriptor
    {
        NCount userId;
        RakString name;
        bool isSubleader;
        ClanMemberState memberState;
        RakString banReason;
    };

    // Helper functions
    NCount GetUserRowFromHandle(RakString & userName, PostgreSQLInterface * sql);
    NCount GetClanIdFromHandle(RakString clanName, PostgreSQLInterface * sql);
    bool IsClanLeader(RakString clanName, NCount userId, PostgreSQLInterface * sql);
    NCount GetClanLeaderId(NCount clanId, PostgreSQLInterface * sql);
    bool IsClanLeader(NCount clanId, NCount userId, PostgreSQLInterface * sql);
    ClanMemberState GetClanMemberState(NCount clanId, NCount userId, bool * isSubleader,
        PostgreSQLInterface * sql);
    void GetClanMembers(NCount clanId, DataStructures::List<ClanMemberDescriptor> & clanMembers,
        PostgreSQLInterface *sql);
    bool IsTitleInUse(RakString titleName, PostgreSQLInterface * sql);
    bool StringContainsProfanity(RakString string, PostgreSQLInterface * sql);
    bool IsValidCountry(RakString string, bool * countryHasStates, PostgreSQLInterface * sql);
    bool IsValidState(RakString string, PostgreSQLInterface * sql);
    bool IsValidRace(RakString string, PostgreSQLInterface * sql);
    void GetFriendIDs(NCount callerUserId, bool excludeIfIgnored,
        PostgreSQLInterface * sql, DataStructures::List<NCount> & output);
    void GetClanMateIDs(NCount callerUserId, bool excludeIfIgnored,
        PostgreSQLInterface * sql, DataStructures::List<NCount> & output);
    bool IsIgnoredByTarget(NCount callerUserId, NCount targetUserId,
        PostgreSQLInterface * sql);
    void OutputFriendsNotification(Notification_Friends_StatusChange::Status notificationType, Lobby2ServerCommand *scmd, PostgreSQLInterface *sql);
    // This does NOT return the online status to output, as it is not threadsafe
    void GetFriendInfosByStatus(NCount callerUserId, RakString status,
        PostgreSQLInterface * sql, DataStructures::List<FriendInfo> & output,
            bool callerIsUserOne);
    void SendEmail(DataStructures::List<RakString> & recipientNames,
        NCount senderUserId, RakString senderUserName, Lobby2Server * server,
            RakString subject, RakString body, SharedPtr<BinaryDataBlock>binaryData,
                int status, RakString triggerString, PostgreSQLInterface * sql);
    void SendEmail(DataStructures::List<NCount> & targetUserIds, NCount senderUserId,
        RakString senderUserName, Lobby2Server *server, RakString subject, RakString body,
            SharedPtr<BinaryDataBlock>binaryData, int status, RakString triggerString,
                PostgreSQLInterface * sql);
    void SendEmail(NCount targetUserId, NCount senderUserId, RakString senderUserName,
        Lobby2Server * server, RakString subject, RakString body,
            SharedPtr<BinaryDataBlock>binaryData, int status,
                RakString triggerString, PostgreSQLInterface * sql);
    int GetActiveClanCount(NCount userId, PostgreSQLInterface * sql);
    bool CreateAccountParametersFailed( CreateAccountParameters & createAccountParameters,
        Lobby2ResultCode & resultCode, Lobby2ServerCommand * scmd,
            PostgreSQLInterface * sql);
    void UpdateAccountFromMissingCreationParameters(CreateAccountParameters & createAccountParameters,
        NCount userPrimaryKey, Lobby2ServerCommand * scmd, PostgreSQLInterface * sql);

    __L2_MSG_DB_HEADER(Platform_Startup, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type)
        {
            (void)scmd; (void)type; return false;
        }
    };
    __L2_MSG_DB_HEADER(Platform_Shutdown, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type)
        {
            (void)scmd;
            (void)type; return false;
        }
    };
    __L2_MSG_DB_HEADER(System_CreateDatabase, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(System_DestroyDatabase, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(System_CreateTitle, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(System_DestroyTitle, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(System_GetTitleRequiredAge, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(System_GetTitleBinaryData, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(System_RegisterProfanity, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(System_BanUser, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(System_UnbanUser, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(CDKey_Add, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(CDKey_GetStatus, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(CDKey_Use, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(CDKey_FlagStolen, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Client_Login, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Client_Logoff, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Client_RegisterAccount, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(System_SetEmailAddressValidated, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Client_ValidateHandle, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(System_DeleteAccount, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(System_PruneAccounts, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Client_GetEmailAddress, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Client_GetPasswordRecoveryQuestionByHandle, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Client_GetPasswordByPasswordRecoveryAnswer, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Client_ChangeHandle, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Client_UpdateAccount, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Client_GetAccountDetails, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Client_StartIgnore, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Client_StopIgnore, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Client_GetIgnoreList, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Client_PerTitleIntegerStorage, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
        virtual bool write(Lobby2ServerCommand * scmd, void * type);
        virtual bool read(Lobby2ServerCommand * scmd, void * type);
        virtual bool Delete(Lobby2ServerCommand * scmd, void * type);
        virtual bool Add(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Client_SetPresence, PGSQL)
    {
        virtual bool ServerPreDBMemoryImpl(Lobby2Server * server, RakString userHandle);
    };
    __L2_MSG_DB_HEADER(Client_GetPresence, PGSQL)
    {
        virtual bool ServerPreDBMemoryImpl(Lobby2Server * server, RakString userHandle);
    };
    __L2_MSG_DB_HEADER(Client_PerTitleBinaryStorage, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Friends_SendInvite, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Friends_AcceptInvite, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
        virtual void ServerPostDBMemoryImpl(Lobby2Server * server, RakString userHandle);
    };
    __L2_MSG_DB_HEADER(Friends_RejectInvite, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Friends_GetInvites, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
        virtual void ServerPostDBMemoryImpl(Lobby2Server * server, RakString userHandle);
    };
    __L2_MSG_DB_HEADER(Friends_GetFriends, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
        virtual void ServerPostDBMemoryImpl(Lobby2Server * server, RakString userHandle);
    };
    __L2_MSG_DB_HEADER(Friends_Remove, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(BookmarkedUsers_Add, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(BookmarkedUsers_Remove, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(BookmarkedUsers_Get, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Emails_Send, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Emails_Get, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Emails_Delete, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Emails_SetStatus, PGSQL)
    {
        virtual bool ServerDBImpl( Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Ranking_SubmitMatch, PGSQL)
    {
        virtual bool ServerDBImpl( Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Ranking_GetMatches, PGSQL)
    {
        virtual bool ServerDBImpl( Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Ranking_GetMatchBinaryData, PGSQL)
    {
        virtual bool ServerDBImpl( Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Ranking_GetTotalScore, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Ranking_WipeScoresForPlayer, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Ranking_WipeMatches, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Ranking_PruneMatches, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Ranking_UpdateRating, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Ranking_WipeRatings, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Ranking_GetRating, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_Create, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_SetProperties, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_GetProperties, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_SetMyMemberProperties, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_GrantLeader, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_SetSubleaderStatus, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_SetMemberRank, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_GetMemberProperties, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_ChangeHandle, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_Leave, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_Get, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_SendJoinInvitation, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_WithdrawJoinInvitation, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_AcceptJoinInvitation, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_RejectJoinInvitation, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_DownloadInvitationList, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_SendJoinRequest, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_WithdrawJoinRequest, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_AcceptJoinRequest, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_RejectJoinRequest, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_DownloadRequestList, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand * scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_KickAndBlacklistUser, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand *scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_UnblacklistUser, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand *scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_GetBlacklist, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand *scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_GetMembers, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand *scmd, void * type);
    };
    __L2_MSG_DB_HEADER(Clans_GetList, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand *scmd, void * type);
    };

    __L2_MSG_DB_HEADER(Clans_CreateBoard, PGSQL)
    {
        virtual bool ServerDBImpl(Lobby2ServerCommand *scmd, void * type)
        {
            (void)scmd;

            PostgreSQLInterface * sql = (PostgreSQLInterface *)type;
            PGresult * result = sql->QueryVariadic("");
            if (result != 0)
            {
                PQclear(result);
                resultCode = L2RC_SUCCESS;
            }
            else
            {
                resultCode = L2RC_SUCCESS;
            }
            return true;
        }
    };

    __L2_MSG_DB_HEADER(Clans_DestroyBoard, PGSQL)
    {
        virtual bool ServerDBImpl( Lobby2ServerCommand *scmd, void *type )
        {
            (void)scmd;

            PostgreSQLInterface *sql = (PostgreSQLInterface *)type;
            PGresult *result = sql->QueryVariadic("");
            if (result!=0)
            {
                PQclear(result);
                resultCode=L2RC_SUCCESS;
            }
            else
            {
                resultCode=L2RC_SUCCESS;
            }
            return true;
        }
    };

    __L2_MSG_DB_HEADER(Clans_CreateNewTopic, PGSQL)
    {
        virtual bool ServerDBImpl( Lobby2ServerCommand *scmd, void *type )
        {
            (void)scmd;

            PostgreSQLInterface *sql = (PostgreSQLInterface *)type;
            PGresult *result = sql->QueryVariadic("");
            if (result!=0)
            {
                PQclear(result);
                resultCode=L2RC_SUCCESS;
            }
            else
            {
                resultCode=L2RC_SUCCESS;
            }
            return true;
        }
    };

    __L2_MSG_DB_HEADER(Clans_ReplyToTopic, PGSQL)
    {
        virtual bool ServerDBImpl( Lobby2ServerCommand *scmd, void *type )
        {
            (void)scmd;

            PostgreSQLInterface *sql = (PostgreSQLInterface *)type;
            PGresult *result = sql->QueryVariadic("");
            if (result!=0)
            {
                PQclear(result);
                resultCode=L2RC_SUCCESS;
            }
            else
            {
                resultCode=L2RC_SUCCESS;
            }
            return true;
        }
    };

    __L2_MSG_DB_HEADER(Clans_RemovePost, PGSQL)
    {
        virtual bool ServerDBImpl( Lobby2ServerCommand *scmd, void *type )
        {
            (void)scmd;

            PostgreSQLInterface *sql = (PostgreSQLInterface *)type;
            PGresult *result = sql->QueryVariadic("");
            if (result!=0)
            {
                PQclear(result);
                resultCode=L2RC_SUCCESS;
            }
            else
            {
                resultCode=L2RC_SUCCESS;
            }
            return true;
        }
    };

    __L2_MSG_DB_HEADER(Clans_GetBoards, PGSQL)
    {
        virtual bool ServerDBImpl( Lobby2ServerCommand *scmd, void *type )
        {
            (void)scmd;

            PostgreSQLInterface *sql = (PostgreSQLInterface *)type;
            PGresult *result = sql->QueryVariadic("");
            if (result!=0)
            {
                PQclear(result);
                resultCode=L2RC_SUCCESS;
            }
            else
            {
                resultCode=L2RC_SUCCESS;
            }
            return true;
        }
    };

    __L2_MSG_DB_HEADER(Clans_GetTopics, PGSQL)
    {
        virtual bool ServerDBImpl( Lobby2ServerCommand *scmd, void *type )
        {
            (void)scmd;

            PostgreSQLInterface *sql = (PostgreSQLInterface *)type;
            PGresult *result = sql->QueryVariadic("");
            if (result!=0)
            {
                PQclear(result);
                resultCode=L2RC_SUCCESS;
            }
            else
            {
                resultCode=L2RC_SUCCESS;
            }
            return true;
        }
    };

    __L2_MSG_DB_HEADER(Clans_GetPosts, PGSQL)
    {
        virtual bool ServerDBImpl( Lobby2ServerCommand *scmd, void *type )
        {
            (void)scmd;

            PostgreSQLInterface *sql = (PostgreSQLInterface *)type;
            PGresult *result = sql->QueryVariadic("");
            if (result!=0)
            {
                PQclear(result);
                resultCode=L2RC_SUCCESS;
            }
            else
            {
                resultCode=L2RC_SUCCESS;
            }
            return true;
        }
    };

    __L2_MSG_DB_HEADER(Notification_Friends_StatusChange, PGSQL)
    {
        virtual bool ServerDBImpl( Lobby2ServerCommand *scmd, void *type )
        {
            PostgreSQLInterface *sql = (PostgreSQLInterface *)type;
            if (scmd->callerSystemAddresses.Size()==0)
            {
                OutputFriendsNotification(Notification_Friends_StatusChange::FRIEND_LOGGED_OFF, scmd, sql);
            }

            // Don't let the thread return this notification with Address::invalid to the user. It's just a message to the thread.
            return false;
        }
        virtual void ServerPostDBMemoryImpl( Lobby2Server *server, RakString userHandle )
        {
            switch (op)
            {
            case Notification_Friends_StatusChange::FRIEND_LOGGED_IN:
            case Notification_Friends_StatusChange::FRIEND_LOGGED_IN_DIFFERENT_CONTEXT:
            case Notification_Friends_StatusChange::THEY_ACCEPTED_OUR_INVITATION_TO_BE_FRIENDS:
                server->GetPresence(presence,otherHandle);
                break;
            case Notification_Friends_StatusChange::FRIEND_LOGGED_OFF:
                presence.isVisible = false;
                presence.status=Lobby2Presence::NOT_ONLINE;
                break;
            case Notification_Friends_StatusChange::FRIEND_ACCOUNT_WAS_DELETED:
            case Notification_Friends_StatusChange::YOU_WERE_REMOVED_AS_A_FRIEND:
            case Notification_Friends_StatusChange::GOT_INVITATION_TO_BE_FRIENDS:
            case Notification_Friends_StatusChange::THEY_REJECTED_OUR_INVITATION_TO_BE_FRIENDS:
                presence.isVisible = false;
                presence.status=Lobby2Presence::UNDEFINED;
                break;
            }
        }
    };

    __L2_MSG_DB_HEADER(Notification_Friends_PresenceUpdate, PGSQL)
    {
        virtual bool ServerDBImpl( Lobby2ServerCommand *scmd, void *type )
        {
            PostgreSQLInterface *sql = (PostgreSQLInterface *)type;

            // Tell all friends about this new login
            DataStructures::List<NCount> output;
            GetFriendIDs(scmd->callerUserId, true, sql, output);

            NCount idx;
            for (idx=0; idx < output.Size(); idx++)
            {
                Notification_Friends_PresenceUpdate *notification = (Notification_Friends_PresenceUpdate *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Friends_PresenceUpdate);
                notification->otherHandle=scmd->callingUserName;
                notification->newPresence=newPresence;
                notification->resultCode=L2RC_SUCCESS;
                scmd->server->AddOutputFromThread(notification, output[idx], "");
            }

            // Don't let the thread return this notification with Address::invalid to the user. It's just a message to the thread.
            return false;
        }
    };


    // --------------------------------------------- Database specific factory class for all messages --------------------------------------------

    #define __L2_MSG_FACTORY_IMPL(__NAME__,__DB__) {case L2MID_##__NAME__ : return OP_NEW< __NAME__##_##__DB__ >() ;}

    struct Lobby2MessageFactory_PGSQL : public Lobby2MessageFactory
    {
        STATIC_FACTORY_DECLARATIONS(Lobby2MessageFactory_PGSQL)

        virtual Lobby2Message *Alloc(Lobby2MessageID id)
        {
            switch (id)
            {
                __L2_MSG_FACTORY_IMPL(Platform_Startup, PGSQL);
                __L2_MSG_FACTORY_IMPL(Platform_Shutdown, PGSQL);
                __L2_MSG_FACTORY_IMPL(System_CreateDatabase, PGSQL);
                __L2_MSG_FACTORY_IMPL(System_DestroyDatabase, PGSQL);
                __L2_MSG_FACTORY_IMPL(System_CreateTitle, PGSQL);
                __L2_MSG_FACTORY_IMPL(System_DestroyTitle, PGSQL);
                __L2_MSG_FACTORY_IMPL(System_GetTitleRequiredAge, PGSQL);
                __L2_MSG_FACTORY_IMPL(System_GetTitleBinaryData, PGSQL);
                __L2_MSG_FACTORY_IMPL(System_RegisterProfanity, PGSQL);
                __L2_MSG_FACTORY_IMPL(System_BanUser, PGSQL);
                __L2_MSG_FACTORY_IMPL(System_UnbanUser, PGSQL);
                __L2_MSG_FACTORY_IMPL(CDKey_Add, PGSQL);
                __L2_MSG_FACTORY_IMPL(CDKey_GetStatus, PGSQL);
                __L2_MSG_FACTORY_IMPL(CDKey_Use, PGSQL);
                __L2_MSG_FACTORY_IMPL(CDKey_FlagStolen, PGSQL);
                __L2_MSG_FACTORY_IMPL(Client_Login, PGSQL);
                __L2_MSG_FACTORY_IMPL(Client_Logoff, PGSQL);
                __L2_MSG_FACTORY_IMPL(Client_RegisterAccount, PGSQL);
                __L2_MSG_FACTORY_IMPL(System_SetEmailAddressValidated, PGSQL);
                __L2_MSG_FACTORY_IMPL(Client_ValidateHandle, PGSQL);
                __L2_MSG_FACTORY_IMPL(System_DeleteAccount, PGSQL);
                __L2_MSG_FACTORY_IMPL(System_PruneAccounts, PGSQL);
                __L2_MSG_FACTORY_IMPL(Client_GetEmailAddress, PGSQL);
                __L2_MSG_FACTORY_IMPL(Client_GetPasswordRecoveryQuestionByHandle, PGSQL);
                __L2_MSG_FACTORY_IMPL(Client_GetPasswordByPasswordRecoveryAnswer, PGSQL);
                __L2_MSG_FACTORY_IMPL(Client_ChangeHandle, PGSQL);
                __L2_MSG_FACTORY_IMPL(Client_UpdateAccount, PGSQL);
                __L2_MSG_FACTORY_IMPL(Client_GetAccountDetails, PGSQL);
                __L2_MSG_FACTORY_IMPL(Client_StartIgnore, PGSQL);
                __L2_MSG_FACTORY_IMPL(Client_StopIgnore, PGSQL);
                __L2_MSG_FACTORY_IMPL(Client_GetIgnoreList, PGSQL);
                __L2_MSG_FACTORY_IMPL(Client_PerTitleIntegerStorage, PGSQL);
                __L2_MSG_FACTORY_IMPL(Client_PerTitleBinaryStorage, PGSQL);
                __L2_MSG_FACTORY_IMPL(Client_SetPresence, PGSQL);
                __L2_MSG_FACTORY_IMPL(Client_GetPresence, PGSQL);
                __L2_MSG_FACTORY_IMPL(Friends_SendInvite, PGSQL);
                __L2_MSG_FACTORY_IMPL(Friends_AcceptInvite, PGSQL);
                __L2_MSG_FACTORY_IMPL(Friends_RejectInvite, PGSQL);
                __L2_MSG_FACTORY_IMPL(Friends_GetInvites, PGSQL);
                __L2_MSG_FACTORY_IMPL(Friends_GetFriends, PGSQL);
                __L2_MSG_FACTORY_IMPL(Friends_Remove, PGSQL);
                __L2_MSG_FACTORY_IMPL(BookmarkedUsers_Add, PGSQL);
                __L2_MSG_FACTORY_IMPL(BookmarkedUsers_Remove, PGSQL);
                __L2_MSG_FACTORY_IMPL(BookmarkedUsers_Get, PGSQL);
                __L2_MSG_FACTORY_IMPL(Emails_Send, PGSQL);
                __L2_MSG_FACTORY_IMPL(Emails_Get, PGSQL);
                __L2_MSG_FACTORY_IMPL(Emails_Delete, PGSQL);
                __L2_MSG_FACTORY_IMPL(Emails_SetStatus, PGSQL);
                __L2_MSG_FACTORY_IMPL(Ranking_SubmitMatch, PGSQL);
                __L2_MSG_FACTORY_IMPL(Ranking_GetMatches, PGSQL);
                __L2_MSG_FACTORY_IMPL(Ranking_GetMatchBinaryData, PGSQL);
                __L2_MSG_FACTORY_IMPL(Ranking_GetTotalScore, PGSQL);
                __L2_MSG_FACTORY_IMPL(Ranking_WipeScoresForPlayer, PGSQL);
                __L2_MSG_FACTORY_IMPL(Ranking_WipeMatches, PGSQL);
                __L2_MSG_FACTORY_IMPL(Ranking_PruneMatches, PGSQL);
                __L2_MSG_FACTORY_IMPL(Ranking_UpdateRating, PGSQL);
                __L2_MSG_FACTORY_IMPL(Ranking_WipeRatings, PGSQL);
                __L2_MSG_FACTORY_IMPL(Ranking_GetRating, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_Create, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_SetProperties, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_GetProperties, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_SetMyMemberProperties, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_GrantLeader, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_SetSubleaderStatus, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_SetMemberRank, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_GetMemberProperties, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_ChangeHandle, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_Leave, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_Get, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_SendJoinInvitation, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_WithdrawJoinInvitation, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_AcceptJoinInvitation, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_RejectJoinInvitation, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_DownloadInvitationList, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_SendJoinRequest, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_WithdrawJoinRequest, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_AcceptJoinRequest, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_RejectJoinRequest, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_DownloadRequestList, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_KickAndBlacklistUser, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_UnblacklistUser, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_GetBlacklist, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_GetMembers, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_GetList, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_CreateBoard, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_DestroyBoard, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_CreateNewTopic, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_ReplyToTopic, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_RemovePost, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_GetBoards, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_GetTopics, PGSQL);
                __L2_MSG_FACTORY_IMPL(Clans_GetPosts, PGSQL);
                __L2_MSG_FACTORY_BASE(Notification_Client_RemoteLogin);
                __L2_MSG_FACTORY_BASE(Notification_Client_IgnoreStatus);
                __L2_MSG_FACTORY_IMPL(Notification_Friends_StatusChange, PGSQL);
                __L2_MSG_FACTORY_IMPL(Notification_Friends_PresenceUpdate, PGSQL);
                __L2_MSG_FACTORY_BASE(Notification_User_ChangedHandle);
                __L2_MSG_FACTORY_BASE(Notification_Friends_CreatedClan);
                __L2_MSG_FACTORY_BASE(Notification_Emails_Received);
                __L2_MSG_FACTORY_BASE(Notification_Clans_GrantLeader);
                __L2_MSG_FACTORY_BASE(Notification_Clans_SetSubleaderStatus);
                __L2_MSG_FACTORY_BASE(Notification_Clans_SetMemberRank);
                __L2_MSG_FACTORY_BASE(Notification_Clans_ChangeHandle);
                __L2_MSG_FACTORY_BASE(Notification_Clans_Leave);
                __L2_MSG_FACTORY_BASE(Notification_Clans_PendingJoinStatus);
                __L2_MSG_FACTORY_BASE(Notification_Clans_NewClanMember);
                __L2_MSG_FACTORY_BASE(Notification_Clans_KickAndBlacklistUser);
                __L2_MSG_FACTORY_BASE(Notification_Clans_UnblacklistUser);
                __L2_MSG_FACTORY_BASE(Notification_Clans_Destroyed);
            default:
                return 0;
            };
        };
    };
}
}
#endif