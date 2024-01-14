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

#include "Lobby2Message_PGSQL.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    NCount GetUserRowFromHandle(RakString & userName, PostgreSQLInterface * sql)
    {
        PGresult * result = sql->QueryVariadic("SELECT userId_pk,handle from lobby2.users WHERE handleLower=lower(%s)", userName.C_String());
        if(result)
        {
            NCount primaryKey;
            int numRowsReturned = PQntuples(result);
            if(numRowsReturned > 0)
            {
                PostgreSQLInterface::PQGetValueFromBinary(&primaryKey, result, 0, "userId_pk");
                PostgreSQLInterface::PQGetValueFromBinary(&userName, result, 0, "handle");
                PQclear(result);
                return primaryKey;
            }
            PQclear(result);
            return 0;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    NCount GetClanIdFromHandle(RakString clanName, PostgreSQLInterface * sql)
    {
        PGresult * result = sql->QueryVariadic("SELECT clanId_pk from lobby2.clans WHERE clanHandleLower=lower(%s)", clanName.C_String());
        if(result)
        {
            NCount primaryKey;
            int numRowsReturned = PQntuples(result);
            if(numRowsReturned > 0)
            {
                PostgreSQLInterface::PQGetValueFromBinary(&primaryKey, result, 0, "clanId_pk");
                PQclear(result);
                return primaryKey;
            }
            PQclear(result);
            return 0;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    bool IsClanLeader(RakString clanName, NCount userId, PostgreSQLInterface * sql)
    {
        NCount clanId = GetClanIdFromHandle(clanName, sql);
        if (clanId == 0)
            return false;
        return IsClanLeader(clanId, userId, sql);
    }
    //------------------------------------------------------------------------
    NCount GetClanLeaderId(NCount clanId, PostgreSQLInterface * sql)
    {
        PGresult * result = sql->QueryVariadic("SELECT leaderUserId_fk FROM lobby2.clans WHERE clanId_pk=%i", clanId);
        if(result == 0)
            return 0;
        int numRowsReturned = PQntuples(result);
        if(numRowsReturned == 0)
        {
            PQclear(result);
            return 0;
        }
        int leaderId;
        PostgreSQLInterface::PQGetValueFromBinary(&leaderId, result, 0, "leaderUserId_fk");
        PQclear(result);
        return leaderId;
    }
    //------------------------------------------------------------------------
    bool IsClanLeader(NCount clanId, NCount userId, PostgreSQLInterface * sql)
    {
        return userId != 0 && GetClanLeaderId(clanId, sql) == userId;
    }
    //------------------------------------------------------------------------
    ClanMemberState GetClanMemberState(NCount clanId, NCount userId, bool * isSubleader,
        PostgreSQLInterface * sql)
    {
        PGresult * result = sql->QueryVariadic("SELECT memberState_fk FROM lobby2.clanMembers WHERE userId_fk=%i AND clanId_fk=%i", userId, clanId);
        if(result == 0)
            return CMD_UNDEFINED;
        int numRowsReturned = PQntuples(result);
        if(numRowsReturned==0)
        {
            PQclear(result);
            return CMD_UNDEFINED;
        }
        int memberState;
        PostgreSQLInterface::PQGetValueFromBinary(&memberState, result, 0, "memberState_fk");
        PostgreSQLInterface::PQGetValueFromBinary(isSubleader, result, 0, "isSubleader");
        PQclear(result);
        return (ClanMemberState) memberState;
    }
    //------------------------------------------------------------------------
    void GetClanMembers(NCount clanId, DataStructures::List<ClanMemberDescriptor> & clanMembers,
        PostgreSQLInterface * sql)
    {
        ClanMemberDescriptor cmd;

        PGresult * result = sql->QueryVariadic(
            "SELECT M.userId_fk, M.isSubleader, M.memberState_fk, M.banReason, U.handle "
                "FROM lobby2.clanMembers AS M, lobby2.users AS U WHERE M.clanId_fk=%i AND U.userId_pk=M.userId_fk", clanId);
        if (result == 0)
            return;
        int numRowsReturned = PQntuples(result);
        int idx;
        for (idx = 0; idx < numRowsReturned; ++idx)
        {
            PostgreSQLInterface::PQGetValueFromBinary(&cmd.userId, result, idx, "userId_fk");
            PostgreSQLInterface::PQGetValueFromBinary(&cmd.isSubleader, result, idx, "isSubleader");
            int cms;
            PostgreSQLInterface::PQGetValueFromBinary(&cms, result, idx, "memberState_fk");
            cmd.memberState=(ClanMemberState)cms;
            PostgreSQLInterface::PQGetValueFromBinary(&cmd.banReason, result, idx, "banReason");
            PostgreSQLInterface::PQGetValueFromBinary(&cmd.name, result, idx, "handle");
            clanMembers.Insert(cmd);
        }
        PQclear(result);
    }
    //------------------------------------------------------------------------
    bool IsTitleInUse(RakString titleName, PostgreSQLInterface * sql)
    {
        PGresult * result = sql->QueryVariadic("SELECT titleName_pk FROM lobby2.titles where titleName_pk=%s", titleName.C_String());
        if (result == 0)
            return false;
        int numRowsReturned = PQntuples(result);
        PQclear(result);
        if (numRowsReturned == 0)
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    bool StringContainsProfanity(RakString string, PostgreSQLInterface * sql)
    {
        RakString strLower1 = " " + string;
        RakString strLower2 = string + " ";
        RakString strLower3 = " " + string + " ";
        RakString strLower4 = string;
        strLower1.ToLower();
        strLower2.ToLower();
        strLower3.ToLower();
        strLower4.ToLower();
        PGresult * result = sql->QueryVariadic("SELECT wordLower FROM lobby2.profanity WHERE "
            "wordLower LIKE %s OR wordLower LIKE %s OR wordLower LIKE %s OR wordLower LIKE %s",
                strLower1.C_String(), strLower2.C_String(), strLower3.C_String(),
                    strLower4.C_String());
        if(result==0)
            return false;
        int numRowsReturned = PQntuples(result);
        PQclear(result);
        if(numRowsReturned==0)
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    bool IsValidCountry(RakString string, bool * countryHasStates, PostgreSQLInterface * sql)
    {
        PGresult * result = sql->QueryVariadic("SELECT country_name,country_has_states FROM lobby2.country where lower(country_name)=lower(%s)", string.C_String());
        if(result == 0)
            return false;
        int numRowsReturned = PQntuples(result);
        if(countryHasStates && numRowsReturned>0)
            PostgreSQLInterface::PQGetValueFromBinary(countryHasStates, result, 0, "country_has_states");
        PQclear(result);
        if(numRowsReturned == 0)
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    bool IsValidState(RakString string, PostgreSQLInterface * sql)
    {
        PGresult * result = sql->QueryVariadic("SELECT state_name FROM lobby2.state WHERE lower(state_name)=lower(%s)", string.C_String());
            if (result == 0)
                return false;
        if(result == 0)
            return false;
        int numRowsReturned = PQntuples(result);
        PQclear(result);
        if(numRowsReturned == 0)
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    bool IsValidRace(RakString string, PostgreSQLInterface * sql)
    {
        PGresult * result = sql->QueryVariadic("SELECT race_text FROM lobby2.race WHERE lower(race_text)=lower(%s)", string.C_String());
            if (result == 0)
                return false;
        if (result == 0)
            return false;
        int numRowsReturned = PQntuples(result);
        PQclear(result);
        if (numRowsReturned == 0)
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void GetFriendIDs(NCount callerUserId, bool excludeIfIgnored, PostgreSQLInterface * sql,
        DataStructures::List<NCount> & output)
    {
        PGresult * result = sql->QueryVariadic("SELECT userTwo_fk from lobby2.friends WHERE userOne_fk=%i AND "
            "actionId_fk=(SELECT actionId_pk from lobby2.friendActions WHERE description='isFriends');", callerUserId);
        if (result==0)
            return;
        int numRowsReturned = PQntuples(result);
        int idx;
        NCount id;
        for (idx = 0; idx < numRowsReturned; ++idx)
        {
            PostgreSQLInterface::PQGetValueFromBinary(&id, result, idx, "userTwo_fk");
            if (excludeIfIgnored==false || IsIgnoredByTarget(callerUserId, id, sql)==false)
                output.Insert(id);
        }
        PQclear(result);
    }
    //------------------------------------------------------------------------
    void GetClanMateIDs(NCount callerUserId, bool excludeIfIgnored, PostgreSQLInterface * sql,
        DataStructures::List<NCount> & output)
    {
        PGresult * result = sql->QueryVariadic(
            "select userId_fk from lobby2.clanMembers where clanId_fk="
                "(select clanId_fk from lobby2.clanMembers where userId_fk=%i)"
                    "AND userId_fk !=%i;", callerUserId, callerUserId);
        if (result == 0)
            return;
        int numRowsReturned = PQntuples(result);
        int idx;
        NCount id;
        for (idx = 0; idx < numRowsReturned; ++idx)
        {
            PostgreSQLInterface::PQGetValueFromBinary(&id, result, idx, "userId_fk");
            if (excludeIfIgnored == false || IsIgnoredByTarget(callerUserId, id, sql)==false)
                output.Insert(id);
        }
        PQclear(result);
    }
    //------------------------------------------------------------------------
    bool IsIgnoredByTarget(NCount callerUserId, NCount targetUserId, PostgreSQLInterface * sql)
    {
        PGresult *result = sql->QueryVariadic(
            "select userMe_fk from lobby2.ignore where userMe_fk=%i AND userOther_fk=%i"
            , callerUserId, targetUserId);
        if (result==0)
            return false;
        int numRowsReturned = PQntuples(result);
        PQclear(result);
        return numRowsReturned>0;
    }
    //------------------------------------------------------------------------
    void OutputFriendsNotification(Notification_Friends_StatusChange::Status notificationType,
        Lobby2ServerCommand * scmd, PostgreSQLInterface * sql)
    {
        // Tell all friends about this new login
        DataStructures::List<NCount> output;
        GetFriendIDs(scmd->callerUserId, true, sql, output);

        NCount idx;
        for (idx=0; idx < output.Size(); ++idx)
        {
            Notification_Friends_StatusChange *notification = (Notification_Friends_StatusChange *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Friends_StatusChange);
            N_assert(scmd->callingUserName.IsEmpty()==false);
            notification->otherHandle=scmd->callingUserName;
            notification->op=notificationType;
            notification->resultCode=L2RC_SUCCESS;
            scmd->server->AddOutputFromThread(notification, output[idx], "");
        }
    }
    //------------------------------------------------------------------------
    void GetFriendInfosByStatus(NCount callerUserId, RakString status, PostgreSQLInterface *sql,
        DataStructures::List<FriendInfo> & output, bool callerIsUserOne)
    {
        RakString query;

        if (callerIsUserOne)
        {
            query = "SELECT handle from lobby2.friends,lobby2.users WHERE userId_pk=userTwo_fk AND userOne_fk=%i";
        }
        else
        {
            query = "SELECT handle from lobby2.friends,lobby2.users WHERE userId_pk=userOne_fk AND userTwo_fk=%i";
        }
        query += " AND actionId_fk=(SELECT actionId_pk FROM lobby2.friendActions where description='";
        query += status;
        query += "');";

        PGresult * result = sql->QueryVariadic(query.C_String(), callerUserId);
        N_assert(result);

        int numRowsReturned = PQntuples(result);
        NCount i;
        for (i = 0; i < numRowsReturned; ++i)
        {
            FriendInfo fi;
            PostgreSQLInterface::PQGetValueFromBinary(&fi.usernameAndStatus.handle, result, i, "handle");
            fi.usernameAndStatus.isOnline=false;
            output.Insert(fi);
        }

        PQclear(result);
    }
    //------------------------------------------------------------------------
    void SendEmail(DataStructures::List<RakString> & recipientNames, NCount senderUserId,
        RakString senderUserName, Lobby2Server * server, RakString subject,
            RakString body, SharedPtr<BinaryDataBlock>binaryData, int status,
                RakString triggerString, PostgreSQLInterface * sql)
    {
        DataStructures::List<NCount> targetUserIds;
        NCount targetUserId;
        for (NCount i = 0; i < recipientNames.Size(); ++i)
        {
            targetUserId = GetUserRowFromHandle(recipientNames[i], sql);
            if (targetUserId!=0)
                targetUserIds.Insert(targetUserId);
        }
        SendEmail(targetUserIds, senderUserId, senderUserName, server, subject, body, binaryData, status, triggerString, sql);
    }
    //------------------------------------------------------------------------
    void SendEmail(NCount targetUserId, NCount senderUserId, RakString senderUserName,
        Lobby2Server * server, RakString subject, RakString body,
            SharedPtr<BinaryDataBlock>binaryData, int status,
                RakString triggerString, PostgreSQLInterface * sql)
    {
        DataStructures::List<NCount> targetUserIds;
        targetUserIds.Insert(targetUserId);
        SendEmail(targetUserIds, senderUserId, senderUserName, server, subject, body, binaryData, status, triggerString, sql);
    }
    //------------------------------------------------------------------------
    void SendEmail(DataStructures::List<NCount> &targetUserIds, NCount senderUserId,
        RakString senderUserName, Lobby2Server * server, RakString subject,
            RakString body, SharedPtr<BinaryDataBlock>binaryData,
                int status, RakString triggerString, PostgreSQLInterface * sql)
    {
        if(targetUserIds.Size() == 0)
            return;

        PGresult * result = 0;
        if(binaryData.IsNull() == false)
        {
            result = sql->QueryVariadic(
                "INSERT INTO lobby2.emails (subject, body, binaryData, triggerId_fk) VALUES "
                "(%s, %s, %a, (SELECT triggerId_pk FROM lobby2.emailTriggers WHERE description=%s) ) RETURNING emailId_pk;",
                    subject.C_String(), body.C_String(), binaryData->binaryData,
                        binaryData->binaryDataLength, triggerString.C_String());
        }
        else
        {
            result = sql->QueryVariadic(
                "INSERT INTO lobby2.emails (subject, body, triggerId_fk) VALUES "
                "(%s, %s, %a, (SELECT triggerId_pk FROM lobby2.emailTriggers WHERE description=%s) ) RETURNING emailId_pk;",
                    subject.C_String(), body.C_String(), triggerString.C_String());
        }

        N_assert(result);
        NCount emailId_pk;
        PostgreSQLInterface::PQGetValueFromBinary(&emailId_pk, result, 0, "emailId_pk");
        PQclear(result);

        NCount i;
        for(i = 0; i < targetUserIds.Size(); ++i)
        {
            // Once in my inbox
            result = sql->QueryVariadic(
                "INSERT INTO lobby2.emailTargets (emailId_fk, userMe_fk, userOther_fk, status, wasRead, ISentThisEmail) VALUES "
                "(%i, %i, %i, %i, %b, %b);", emailId_pk, senderUserId, targetUserIds[i], status, true, true);
            N_assert(result);
            PQclear(result);

            // Once in the destination inbox
            result = sql->QueryVariadic(
                "INSERT INTO lobby2.emailTargets (emailId_fk, userMe_fk, userOther_fk, status, wasRead, ISentThisEmail) VALUES "
                "(%i, %i, %i, %i, %b, %b) RETURNING emailTarget_pk;", emailId_pk, targetUserIds[i], senderUserId, status, false, false);
            N_assert(result);
            NCount emailTarget_pk;
            PostgreSQLInterface::PQGetValueFromBinary(&emailTarget_pk, result, 0, "emailTarget_pk");
            PQclear(result);

            // Notify recipient that they got an email
            Notification_Emails_Received *notification = (Notification_Emails_Received *) server->GetMessageFactory()->Alloc(L2MID_Notification_Emails_Received);
            notification->sender=senderUserName;
            notification->subject=subject;
            notification->emailId=emailTarget_pk;
            notification->resultCode=L2RC_SUCCESS;
            server->AddOutputFromThread(notification, targetUserIds[i], "");
        }
    }
    //------------------------------------------------------------------------
    int GetActiveClanCount(NCount userId, PostgreSQLInterface * sql)
    {
        PGresult * result = sql->QueryVariadic("SELECT clanMemberId_pk FROM lobby2.clanMembers WHERE userId_fk=%i AND memberState_fk=(SELECT stateId_pk FROM lobby2.clanMemberStates WHERE description='ClanMember_Active')", userId);
        int numRowsReturned = PQntuples(result);
        PQclear(result);
        return numRowsReturned;
    }
    //------------------------------------------------------------------------
    bool CreateAccountParametersFailed(CreateAccountParameters & createAccountParameters,
        Lobby2ResultCode & resultCode, Lobby2ServerCommand * scmd, PostgreSQLInterface * sql)
    {
        bool hasStates = true;

        if (createAccountParameters.homeCountry.IsEmpty() == false)
        {
            if (IsValidCountry(createAccountParameters.homeCountry, &hasStates, sql)==false)
            {
                resultCode = L2RC_Client_RegisterAccount_INVALID_COUNTRY;
                return true;
            }
        }
        if (hasStates == true)
        {
            if (createAccountParameters.homeState.IsEmpty()==false && IsValidState(createAccountParameters.homeState, sql)==false)
            {
                resultCode=L2RC_Client_RegisterAccount_INVALID_STATE;
                return true;
            }
        }
        else
            createAccountParameters.homeState.Clear();

        if (createAccountParameters.billingCountry.IsEmpty()==false)
        {
            if (IsValidCountry(createAccountParameters.billingCountry, &hasStates, sql)==false)
            {
                resultCode=L2RC_Client_RegisterAccount_INVALID_COUNTRY;
                return true;
            }
        }
        if (hasStates==true)
        {
            if (createAccountParameters.billingState.IsEmpty()==false && IsValidState(createAccountParameters.billingState, sql)==false)
            {
                resultCode=L2RC_Client_RegisterAccount_INVALID_STATE;
                return true;
            }
        }
        else
            createAccountParameters.billingState.Clear();

        if (createAccountParameters.race.IsEmpty()==false &&
            IsValidRace(createAccountParameters.race, sql)==false)
        {
            resultCode=L2RC_Client_RegisterAccount_INVALID_RACE;
            return true;
        }
        NCount requiredAgeYears = scmd->server->GetConfigurationProperties()->accountRegistrationRequiredAgeYears;
        if (createAccountParameters.ageInDays < requiredAgeYears*365 )
        {
            resultCode=L2RC_Client_RegisterAccount_REQUIRED_AGE_NOT_MET;
            return true;
        }
        return false;

    }
    //------------------------------------------------------------------------
    void UpdateAccountFromMissingCreationParameters(CreateAccountParameters & createAccountParameters,
        NCount userPrimaryKey, Lobby2ServerCommand * scmd, PostgreSQLInterface * sql)
    {
        (void)scmd;

        PGresult * result=0;
        NCount key;

        if (createAccountParameters.homeState.IsEmpty() == false)
        {
            result = sql->QueryVariadic("SELECT state_id FROM lobby2.state where lower(state_name)=lower(%s)", createAccountParameters.homeState.C_String());
            if (result)
            {
                if (PQntuples(result))
                {
                    PostgreSQLInterface::PQGetValueFromBinary(&key, result, 0, "state_id");
                    PQclear( sql->QueryVariadic("UPDATE lobby2.users SET homeStateId_fk=%i WHERE userId_pk=%i", key, userPrimaryKey ));
                }
                PQclear(result);
            }
        }
        if (createAccountParameters.homeCountry.IsEmpty() == false)
        {
            result = sql->QueryVariadic("SELECT country_id FROM lobby2.country where lower(country_name)=lower(%s)", createAccountParameters.homeCountry.C_String());
            if (result)
            {
                if (PQntuples(result))
                {
                    PostgreSQLInterface::PQGetValueFromBinary(&key, result, 0, "country_id");
                    PQclear( sql->QueryVariadic("UPDATE lobby2.users SET homeCountryId_fk=%i WHERE userId_pk=%i", key, userPrimaryKey ));
                }
                PQclear(result);
            }
        }
        if (createAccountParameters.billingState.IsEmpty() == false)
        {
            result = sql->QueryVariadic("SELECT state_id FROM lobby2.state where lower(state_name)=lower(%s)", createAccountParameters.billingState.C_String());
            if (result)
            {
                if (PQntuples(result))
                {
                    PostgreSQLInterface::PQGetValueFromBinary(&key, result, 0, "state_id");
                    PQclear( sql->QueryVariadic("UPDATE lobby2.users SET billingStateId_fk=%i WHERE userId_pk=%i", key, userPrimaryKey ));
                }
                PQclear(result);
            }
        }
        if (createAccountParameters.billingCountry.IsEmpty() == false)
        {
            result = sql->QueryVariadic("SELECT country_id FROM lobby2.country where lower(country_name)=lower(%s)", createAccountParameters.billingCountry.C_String());
            if (result)
            {
                if (PQntuples(result))
                {
                    PostgreSQLInterface::PQGetValueFromBinary(&key, result, 0, "country_id");
                    PQclear( sql->QueryVariadic("UPDATE lobby2.users SET billingCountryId_fk=%i WHERE userId_pk=%i", key, userPrimaryKey ));
                }
                PQclear(result);
            }
        }
        if (createAccountParameters.race.IsEmpty() == false)
        {
            result = sql->QueryVariadic("SELECT race_id FROM lobby2.race where lower(race_text)=lower(%s)", createAccountParameters.race.C_String());
            if (result)
            {
                if (PQntuples(result))
                {
                    PostgreSQLInterface::PQGetValueFromBinary(&key, result, 0, "race_id");
                    PQclear( sql->QueryVariadic("UPDATE lobby2.users SET raceId_fk=%i WHERE userId_pk=%i", key, userPrimaryKey ));
                }
                PQclear(result);
            }
        }
    }
    //------------------------------------------------------------------------
    bool System_CreateDatabase_PGSQL::ServerDBImpl(Lobby2ServerCommand *  scmd,
        void * datatype)
    {
        (void)scmd;
        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        bool success;
        PGresult * result = 0;
        sql->ExecuteBlockingCommand("DROP SCHEMA lobby2 CASCADE;", &result, false);
        PQclear(result);
        sql->ExecuteBlockingCommand("DROP LANGUAGE plpgsql;", &result, false);
        PQclear(result);
        FILE * fp = fopen("Lobby2Schema.txt", "rb");
        N_assert(fp && "Can't find Lobby2Schema.txt");
        fseek( fp, 0, SEEK_END );
        NCount fileSize = ftell( fp );
        fseek( fp, 0, SEEK_SET );
        char * cmd = (char*) rakMalloc_Ex(fileSize+1);
        fread(cmd, 1, fileSize, fp);
        fclose(fp);
        cmd[fileSize] = 0;
        success = sql->ExecuteBlockingCommand(cmd, &result, false);
        PQclear(result);
        if (success)
        {
            resultCode = L2RC_SUCCESS;
        }
        else
        {
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
            printf(cmd);
            printf(sql->GetLastError());
        }
        rakFree_Ex(cmd);
        return true;
    }
    //------------------------------------------------------------------------
    bool System_DestroyDatabase_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd,
        void * datatype)
    {
        (void)scmd;

        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        bool success;
        PGresult * result = 0;
        success = sql->ExecuteBlockingCommand("DROP SCHEMA lobby2 CASCADE;",
            &result, false);
        PQclear(result);
        if (success)
            resultCode = L2RC_SUCCESS;
        else
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
        return true;
    }
    //------------------------------------------------------------------------
    bool System_CreateTitle_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd,
        void * datatype)
    {
        (void)scmd;

        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        PGresult * result = sql->QueryVariadic("INSERT INTO lobby2.titles (titleName_pk, titleSecretKey, requiredAge, binaryData) VALUES (%s,%s,%i,%a)",
            titleName.C_String(),
            titleSecretKey.C_String(),
            requiredAge,
            binaryData->binaryData,
            binaryData->binaryDataLength);
        if (result != 0)
        {
            PQclear(result);
            resultCode = L2RC_SUCCESS;
        }
        else
        {
            resultCode = L2RC_System_CreateTitle_TITLE_ALREADY_IN_USE;
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool System_DestroyTitle_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;
        result = sql->QueryVariadic("DELETE FROM lobby2.titles WHERE titlename_pk=%s", titleName.C_String());
        if (result!=0)
        {
            PQclear(result);
            resultCode=L2RC_SUCCESS;
        }
        else
        {
            resultCode=L2RC_System_DestroyTitle_TITLE_NOT_IN_USE;
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool System_GetTitleRequiredAge_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result = sql->QueryVariadic("SELECT requiredAge FROM lobby2.titles where titlename_pk=%s", titleName.C_String());
        if (result!=0)
        {
            PostgreSQLInterface::PQGetValueFromBinary(&requiredAge, result, 0, "requiredAge");
            PQclear(result);
            resultCode=L2RC_SUCCESS;
        }
        else
        {
            resultCode=L2RC_System_GetTitleRequiredAge_TITLE_NOT_IN_USE;
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool System_GetTitleBinaryData_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd,
        void * datatype)
    {
        (void)scmd;

        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        PGresult * result = sql->QueryVariadic("SELECT binaryData FROM lobby2.titles where titlename_pk=%s", titleName.C_String());
        if (result != 0)
        {
            PostgreSQLInterface::PQGetValueFromBinary(&binaryData->binaryData,
                &binaryData->binaryDataLength, result, 0, "binaryData");
            PQclear(result);
            resultCode = L2RC_SUCCESS;
        }
        else
        {
            resultCode = L2RC_System_GetTitleBinaryData_TITLE_NOT_IN_USE;
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool System_RegisterProfanity_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd,
        void * datatype)
    {
        (void)scmd;

        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        PGresult * result = 0;
        resultCode = L2RC_SUCCESS;
        for (NCount i = 0; i < profanityWords.Size(); ++i)
        {
            result = sql->QueryVariadic("INSERT INTO lobby2.profanity (word) VALUES (%s)", profanityWords[i].C_String());
            if (result == 0)
                resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
            PQclear(result);
            if (resultCode == L2RC_DATABASE_CONSTRAINT_FAILURE)
                return true;
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool System_BanUser_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        NCount userRow = GetUserRowFromHandle(userName, sql);
        if (userRow==0)
        {
            resultCode=L2RC_UNKNOWN_USER;
            return true;
        }
        PGresult *result = sql->QueryVariadic("INSERT INTO lobby2.bannedUsers (userId_fk, description, timeout) VALUES (%i, %s, now () + %i * interval '1 hours')", userRow, banReason.C_String(), durationHours);
        if (result!=0)
        {
            PQclear(result);
            result = sql->QueryVariadic("INSERT INTO lobby2.userHistory (userId_fk, description, triggerId_fk) "
                "VALUES (%i, %s, (SELECT triggerId_pk FROM lobby2.userHistoryTriggers WHERE description='Banned'))", userRow, banReason.C_String());
            N_assert(result);
            PQclear(result);

            resultCode=L2RC_SUCCESS;
        }
        else
        {
            resultCode=L2RC_System_BanUser_ALREADY_BANNED;
        }

        return true;
    }
    //------------------------------------------------------------------------
    bool System_UnbanUser_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        NCount userRow = GetUserRowFromHandle(userName, sql);
        if (userRow==0)
        {
            resultCode=L2RC_UNKNOWN_USER;
            return true;
        }
        PGresult *result = sql->QueryVariadic("DELETE FROM lobby2.bannedUsers WHERE userId_fk=%i", userRow);
        if (result!=0)
        {
            PQclear(result);
            result = sql->QueryVariadic("INSERT INTO lobby2.userHistory (userId_fk, description, triggerId_fk) "
                "VALUES (%i, %s, (SELECT triggerId_pk FROM lobby2.userHistoryTriggers WHERE description='Unbanned'))", userRow, reason.C_String());
            N_assert(result);
            PQclear(result);
            resultCode=L2RC_SUCCESS;
        }
        else
        {
            resultCode=L2RC_System_BanUser_ALREADY_BANNED;
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool CDKey_Add_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;
        if (IsTitleInUse(titleName, sql)==false)
        {
            resultCode=L2RC_CDKey_Add_TITLE_NOT_IN_USE;
            return true;
        }
        NCount i;
        for (i=0; i < cdKeys.Size(); i++)
        {
            result = sql->QueryVariadic("INSERT INTO lobby2.cdkeys (cdKey, usable, stolen, titleName_fk) VALUES (%s, true, false, %s);", cdKeys[i].C_String(), titleName.C_String());
            N_assert(result);
            PQclear(result);
        }
        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool CDKey_GetStatus_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd, void * datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;
        if (IsTitleInUse(titleName, sql)==false)
        {
            resultCode=L2RC_CDKey_GetStatus_TITLE_NOT_IN_USE;
            return true;
        }

        result = sql->QueryVariadic(
            "SELECT lobby2.cdkeys.usable, lobby2.cdkeys.stolen, lobby2.cdkeys.activationDate, lobby2.users.handle "
            "FROM lobby2.cdkeys LEFT OUTER JOIN lobby2.users ON lobby2.users.userId_pk=lobby2.cdkeys.userId_fk "
            "WHERE lobby2.cdkeys.cdKey=%s AND lobby2.cdkeys.titleName_fk=%s;"
            , cdKey.C_String(), titleName.C_String());

        int numRowsReturned = PQntuples(result);
        if (numRowsReturned==0)
        {
            PQclear(result);
            resultCode=L2RC_CDKey_GetStatus_UNKNOWN_CD_KEY;
            return true;
        }

        PostgreSQLInterface::PQGetValueFromBinary(&usable, result, 0, "lobby2.cdkeys.usable");
        PostgreSQLInterface::PQGetValueFromBinary(&wasStolen, result, 0, "lobby2.cdkeys.stolen");
        PostgreSQLInterface::PQGetValueFromBinary(&usedBy, result, 0, "lobby2.users.handle");
        PostgreSQLInterface::PQGetValueFromBinary(&activationDate, result, 0, "lobby2.cdkeys.activationDate");

        PQclear(result);
        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool CDKey_Use_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;
        if (IsTitleInUse(titleName, sql)==false)
        {
            resultCode=L2RC_CDKey_Use_TITLE_NOT_IN_USE;
            return true;
        }

        result = sql->QueryVariadic("SELECT lobby2.cdkeys.usable, lobby2.cdkeys.stolen, lobby2.cdkeys.userId_fk "
            "FROM lobby2.cdkeys, lobby2.users WHERE lobby2.cdkeys.cdKey=%s AND lobby2.cdkeys.titleName_fk=%s",
            cdKey.C_String(), titleName.C_String());
        int numRowsReturned = PQntuples(result);
        if (numRowsReturned==0)
        {
            PQclear(result);
            resultCode=L2RC_CDKey_Use_UNKNOWN_CD_KEY;
            return true;
        }
        bool usable, wasStolen, alreadyUsed;
        NCount userId;
        PostgreSQLInterface::PQGetValueFromBinary(&usable, result, 0, "lobby2.cdkeys.usable");
        PostgreSQLInterface::PQGetValueFromBinary(&wasStolen, result, 0, "lobby2.cdkeys.stolen");
        alreadyUsed=PostgreSQLInterface::PQGetValueFromBinary(&userId, result, 0, "lobby2.cdkeys.userId_fk");
        PQclear(result);

        if (wasStolen)
            resultCode=L2RC_CDKey_Use_CD_KEY_STOLEN;
        else if (alreadyUsed)
            resultCode=L2RC_CDKey_Use_CD_KEY_ALREADY_USED;
        else if (usable==false)
            resultCode=L2RC_CDKey_Use_NOT_USABLE;
        else
        {
            NCount userRow = GetUserRowFromHandle(userName, sql);
            if (userRow==0)
            {
                resultCode=L2RC_UNKNOWN_USER;
                return true;
            }
            result = sql->QueryVariadic("UPDATE lobby2.cdKeys SET activationDate=now(),"
                "userId_fk=%i WHERE lobby2.cdkeys.cdKey=%s AND lobby2.cdkeys.titleName_fk=%s", userRow, cdKey.C_String(), titleName.C_String());
            if (result==0)
            {
                resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
                return true;
            }
            PQclear(result);
            resultCode=L2RC_SUCCESS;
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool CDKey_FlagStolen_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;
        if (IsTitleInUse(titleName, sql)==false)
        {
            resultCode=L2RC_CDKey_FlagStolen_TITLE_NOT_IN_USE;
            return true;
        }

        result = sql->QueryVariadic("SELECT lobby2.cdkeys.cdKey, lobby2.cdkeys.userId_fk FROM lobby2.cdkeys WHERE lobby2.cdkeys.cdKey=%s AND lobby2.cdkeys.titleName_fk=%s",
            cdKey.C_String(), titleName.C_String());
        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        int numRowsReturned = PQntuples(result);
        if (numRowsReturned==0)
        {
            PQclear(result);
            resultCode=L2RC_CDKey_Use_UNKNOWN_CD_KEY;
            return true;
        }
        NCount userId_fk;
        if (PostgreSQLInterface::PQGetValueFromBinary(&userId_fk, result, 0, "userId_fk"))
        {
            PQclear(result);
            result = sql->QueryVariadic("SELECT handle from lobby2.users WHERE userId_pk=%i", userId_fk);
            PostgreSQLInterface::PQGetValueFromBinary(&userUsingThisKey, result, 0, "handle");
            PQclear(result);

        }
        else
            PQclear(result);

        result = sql->QueryVariadic("UPDATE lobby2.cdKeys SET stolen=%b WHERE lobby2.cdkeys.cdKey=%s AND lobby2.cdkeys.titleName_fk=%s", wasStolen, cdKey.C_String(), titleName.C_String());
        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_Login_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;
        NCount userRow = GetUserRowFromHandle(userName, sql);
        if (userRow==0)
        {
            resultCode=L2RC_Client_Login_HANDLE_NOT_IN_USE_OR_BAD_SECRET_KEY;
            return true;
        }
        result = sql->QueryVariadic("SELECT password FROM lobby2.users WHERE userId_pk=%i", userRow);
        RakString password;
        PostgreSQLInterface::PQGetValueFromBinary(&password, result, 0, "password");
        PQclear(result);
        if (password!=userPassword)
        {
            resultCode=L2RC_Client_Login_HANDLE_NOT_IN_USE_OR_BAD_SECRET_KEY;
            return true;
        }
        if (scmd->server->GetConfigurationProperties()->requiresEmailAddressValidationToLogin)
        {
            result = sql->QueryVariadic("SELECT emailAddressValidated FROM lobby2.users WHERE userId_pk=%i", userRow);
            bool emailAddressValidated;
            PostgreSQLInterface::PQGetValueFromBinary(&emailAddressValidated, result, 0, "emailAddressValidated");
            PQclear(result);
            if (emailAddressValidated==false)
            {
                resultCode=L2RC_Client_Login_EMAIL_ADDRESS_NOT_VALIDATED;
                return true;
            }
        }
        if (scmd->server->GetConfigurationProperties()->requiresTitleToLogin)
        {
            RakString titleDBSecretKey;
            result = sql->QueryVariadic("SELECT titleSecretKey FROM lobby2.titles where titleName_pk=%s", titleName.C_String());
            int numRowsReturned = PQntuples(result);
            if (numRowsReturned==0)
            {
                resultCode=L2RC_Client_Login_BAD_TITLE_OR_TITLE_SECRET_KEY;
                PQclear(result);
                return true;
            }
            PostgreSQLInterface::PQGetValueFromBinary(&titleDBSecretKey, result, 0, "titleSecretKey");
            PQclear(result);
            // title can have no secret key, in which case you just have to specify a valid title
            if (titleDBSecretKey.IsEmpty()==false && titleDBSecretKey!=titleSecretKey)
            {
                resultCode=L2RC_Client_Login_BAD_TITLE_OR_TITLE_SECRET_KEY;
                return true;
            }
        }

        // Does this user have any stolen CD keys?
        result = sql->QueryVariadic("SELECT stolen FROM lobby2.cdkeys WHERE userId_fk=%i AND stolen=TRUE",	userRow);
        int numRowsReturned = PQntuples(result);
        PQclear(result);
        if (numRowsReturned!=0)
        {
            resultCode=L2RC_Client_Login_CDKEY_STOLEN;
            return true;
        }

        result = sql->QueryVariadic("SELECT description, timeout, creationDate from lobby2.bannedUsers WHERE userId_fk=%i AND now() < timeout", userRow);
        numRowsReturned = PQntuples(result);
        if (numRowsReturned != 0)
        {
            PostgreSQLInterface::PQGetValueFromBinary(&bannedReason, result, 0, "description");
            PostgreSQLInterface::PQGetValueFromBinary(&bannedExpiration, result, 0, "timeout");
            PostgreSQLInterface::PQGetValueFromBinary(&whenBanned, result, 0, "creationDate");
            PQclear(result);
            resultCode=L2RC_Client_Login_BANNED;
            return true;
        }
        PQclear(result);

        result = sql->QueryVariadic("INSERT INTO lobby2.userHistory (userId_fk, triggerId_fk) "
            "VALUES "
            "(%i,"
            "(SELECT triggerId_pk FROM lobby2.userHistoryTriggers WHERE description='Login'))", userRow);
        PQclear(result);

        scmd->callingUserName=userName;
        scmd->callerUserId=userRow;

        OutputFriendsNotification(Notification_Friends_StatusChange::FRIEND_LOGGED_IN, scmd, sql);

        // Have the server record in memory this new login
        scmd->server->OnLogin(scmd, true);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_Logoff_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd, void *datatype )
    {
        if (scmd->callerUserId == 0)
            return false;

        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        PGresult * result = 0;
        result = sql->QueryVariadic("INSERT INTO lobby2.userHistory (userId_fk, triggerId_fk) "
            "VALUES "
            "(%i,"
            "(SELECT triggerId_pk FROM lobby2.userHistoryTriggers WHERE description='Logoff'));", scmd->callerUserId);
        PQclear(result);

        // Notification is done below
        scmd->server->OnLogoff(scmd, true);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_RegisterAccount_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd,
        void * datatype)
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        if (StringContainsProfanity(userName, sql))
        {
            resultCode=L2RC_PROFANITY_FILTER_CHECK_FAILED;
            return true;
        }

        NCount userRow = GetUserRowFromHandle(userName, sql);
        if (userRow!=0)
        {
            resultCode=L2RC_Client_RegisterAccount_HANDLE_ALREADY_IN_USE;
            return true;
        }

        if (CreateAccountParametersFailed(createAccountParameters, resultCode, scmd, sql))
            return true;


        if (scmd->server->GetConfigurationProperties()->accountRegistrationRequiresCDKey)
        {
            if (cdKey.IsEmpty())
            {
                resultCode=L2RC_Client_RegisterAccount_REQUIRES_CD_KEY;
                return true;
            }

            if (titleName.IsEmpty())
            {
                resultCode=L2RC_Client_RegisterAccount_REQUIRES_CD_KEY;
                return true;
            }
            result = sql->QueryVariadic("SELECT usable, stolen "
                "FROM lobby2.cdkeys WHERE lobby2.cdkeys.cdKey=%s AND lobby2.cdkeys.titleName_fk=%s",
                cdKey.C_String(), titleName.C_String());

            int numRowsReturned = PQntuples(result);
            if (numRowsReturned==0)
            {
                PQclear(result);
                resultCode=L2RC_Client_RegisterAccount_CD_KEY_NOT_USABLE;
                return true;
            }

            bool usable;
            bool wasStolen;
            RakString usedBy;
            PostgreSQLInterface::PQGetValueFromBinary(&usable, result, 0, "usable");
            PostgreSQLInterface::PQGetValueFromBinary(&wasStolen, result, 0, "stolen");
            PQclear(result);
            if (usable==false)
            {
                PQclear(result);
                resultCode=L2RC_Client_RegisterAccount_CD_KEY_NOT_USABLE;
                return true;
            }
            if (wasStolen)
            {
                PQclear(result);
                resultCode=L2RC_Client_RegisterAccount_CD_KEY_STOLEN;
                return true;
            }
            if (usedBy.IsEmpty()==false)
            {
                PQclear(result);
                resultCode=L2RC_Client_RegisterAccount_CD_KEY_ALREADY_USED;
                return true;
            }
        }

        result = sql->QueryVariadic(
            "INSERT INTO lobby2.users ("
            "handle, firstname, middlename, lastname,"
            "sex_male, homeaddress1, homeaddress2, homecity, "
            "homezipcode, billingaddress1, billingaddress2, billingcity,"
            "billingzipcode, emailaddress, password, passwordrecoveryquestion,"
            "passwordrecoveryanswer, caption1, caption2, dateOfBirth, binaryData) "
            "VALUES ("
            "%s, %s, %s, %s,"
            "%b, %s, %s, %s,"
            "%s, %s, %s, %s,"
            "%s, %s, %s, %s,"
            "%s, %s, %s, (select now() - %i * interval '1 day'), %a) RETURNING userId_pk",
            userName.C_String(), createAccountParameters.firstName.C_String(), createAccountParameters.middleName.C_String(), createAccountParameters.lastName.C_String(),
            createAccountParameters.sex_male, createAccountParameters.homeAddress1.C_String(), createAccountParameters.homeAddress2.C_String(), createAccountParameters.homeCity.C_String(),
            createAccountParameters.homeZipCode.C_String(), createAccountParameters.billingAddress1.C_String(),	createAccountParameters.billingAddress2.C_String(), createAccountParameters.billingCity.C_String(),
            createAccountParameters.billingZipCode.C_String(), createAccountParameters.emailAddress.C_String(), createAccountParameters.password.C_String(), createAccountParameters.passwordRecoveryQuestion.C_String(),
            createAccountParameters.passwordRecoveryAnswer.C_String(), createAccountParameters.caption1.C_String(),createAccountParameters.caption2.C_String(),
            createAccountParameters.ageInDays, createAccountParameters.binaryData->binaryData,
                createAccountParameters.binaryData->binaryDataLength);
        if (result == 0)
        {
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        NCount userPrimaryKey;
        PostgreSQLInterface::PQGetValueFromBinary(&userPrimaryKey, result, 0, "userId_pk");
        PQclear(result);

        // Assign the cd key, already validated earlier
        if (scmd->server->GetConfigurationProperties()->accountRegistrationRequiresCDKey)
        {
            PQclear(sql->QueryVariadic("UPDATE lobby2.cdKeys SET activationDate=now(),"
                "userId_fk=%i WHERE lobby2.cdkeys.cdKey=%s AND lobby2.cdkeys.titleName_fk=%s", userPrimaryKey, cdKey.C_String(), titleName.C_String()));
        }

        UpdateAccountFromMissingCreationParameters(createAccountParameters, userPrimaryKey, scmd, sql);

        resultCode = L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool System_SetEmailAddressValidated_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd,
        void * datatype)
    {
        (void)scmd;

        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        NCount userRow = GetUserRowFromHandle(userName, sql);
        if (userRow == 0)
        {
            resultCode = L2RC_UNKNOWN_USER;
            return true;
        }
        PGresult * result = sql->QueryVariadic("UPDATE lobby2.users SET emailAddressValidated=%b WHERE userId_pk=%i", validated, userRow);
        PQclear(result);
        if (result != 0)
            resultCode = L2RC_SUCCESS;
        else
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_ValidateHandle_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd,
        void * datatype)
    {
        (void)scmd;

        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        NCount userRow = GetUserRowFromHandle(userName, sql);
        if (userRow != 0)
        {
            resultCode = L2RC_Client_ValidateHandle_HANDLE_ALREADY_IN_USE;
            return true;
        }
        if (StringContainsProfanity(userName, sql))
        {
            resultCode = L2RC_PROFANITY_FILTER_CHECK_FAILED;
            return true;
        }
        resultCode = L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool System_DeleteAccount_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd,
        void * datatype)
    {
        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        NCount userRow = GetUserRowFromHandle(userName, sql);
        if (userRow == 0)
        {
            resultCode = L2RC_UNKNOWN_USER;
            return true;
        }

        PGresult * result = sql->QueryVariadic(
            "SELECT password from lobby2.users WHERE userId_pk = %i", userRow);
        if (result == 0)
        {
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        int numRowsReturned = PQntuples(result);
        if (numRowsReturned == 0)
        {
            PQclear(result);
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        RakString passwordFromDB;
        PostgreSQLInterface::PQGetValueFromBinary(&passwordFromDB, result, 0, "password");
        PQclear(result);
        if (passwordFromDB != password)
        {
            PQclear(result);
            resultCode = L2RC_System_DeleteAccount_INVALID_PASSWORD;
            return true;
        }

        // Notify friends of account deletion
        scmd->callingUserName = userName;
        scmd->callerUserId = userRow;
        OutputFriendsNotification(Notification_Friends_StatusChange::FRIEND_ACCOUNT_WAS_DELETED, scmd, sql);

        // Trigger logoff as well
        Client_Logoff * logoffRequest = (Client_Logoff *) scmd->server->GetMessageFactory()->Alloc(L2MID_Client_Logoff);
        logoffRequest->ServerDBImpl( scmd, datatype );
        scmd->server->AddOutputFromThread(logoffRequest, userRow, userName);

        // Delete the account
        result = sql->QueryVariadic("DELETE FROM lobby2.users WHERE userId_pk=%i", userRow);
        PQclear(result);
        if (result != 0)
            resultCode = L2RC_SUCCESS;
        else
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
        return true;
    }
    //------------------------------------------------------------------------
    bool System_PruneAccounts_PGSQL::ServerDBImpl(Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result = sql->QueryVariadic(
            "SELECT handle from lobby2.users WHERE userId_pk ="
            "(SELECT userId_fk FROM lobby2.userHistory WHERE creationDate < now() - %i * interval '1 day' ORDER by creationDate DESC LIMIT 1 AND triggerid_fk="
            "(SELECT triggerId_pk from lobby2.userHistoryTriggers where description='Login')"
            ") ;", deleteAccountsNotLoggedInDays);

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        int numRowsReturned = PQntuples(result);
        if (numRowsReturned==0)
        {
            PQclear(result);
            resultCode=L2RC_SUCCESS;
            return true;
        }

        // Delete all accounts where the user has not logged in deleteAccountsNotLoggedInDays
        System_DeleteAccount *deleteAccount = (System_DeleteAccount *) scmd->server->GetMessageFactory()->Alloc(L2MID_System_DeleteAccount);

        RakString userName;
        for (int i=0; i < numRowsReturned; i++)
        {
            PostgreSQLInterface::PQGetValueFromBinary(&userName, result, i, "handle");
            deleteAccount->userName=userName;
            deleteAccount->ServerDBImpl( scmd, datatype );
        }
        scmd->server->GetMessageFactory()->Dealloc(deleteAccount);
        PQclear(result);

        return true;
    }
    //------------------------------------------------------------------------
    bool Client_GetEmailAddress_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        NCount userRow = GetUserRowFromHandle(userName, sql);
        if (userRow==0)
        {
            resultCode=L2RC_UNKNOWN_USER;
            return true;
        }

        PGresult *result = sql->QueryVariadic(
            "SELECT emailaddress, emailAddressValidated from lobby2.users WHERE userId_pk = %i", userRow);

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        PostgreSQLInterface::PQGetValueFromBinary(&emailAddress, result, 0, "emailAddress");
        PostgreSQLInterface::PQGetValueFromBinary(&emailAddressValidated, result, 0, "emailAddressValidated");
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_GetPasswordRecoveryQuestionByHandle_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        NCount userRow = GetUserRowFromHandle(userName, sql);
        if (userRow==0)
        {
            resultCode=L2RC_UNKNOWN_USER;
            return true;
        }

        PGresult *result = sql->QueryVariadic(
            "SELECT passwordRecoveryQuestion,emailAddress from lobby2.users WHERE userId_pk = %i", userRow);

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }


        int numRowsReturned = PQntuples(result);
        if (numRowsReturned==0)
        {
            PQclear(result);
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        PostgreSQLInterface::PQGetValueFromBinary(&emailAddress, result, 0, "emailAddress");
        PostgreSQLInterface::PQGetValueFromBinary(&passwordRecoveryQuestion, result, 0, "passwordRecoveryQuestion");
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_GetPasswordByPasswordRecoveryAnswer_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        NCount userRow = GetUserRowFromHandle(userName, sql);
        if (userRow==0)
        {
            resultCode=L2RC_UNKNOWN_USER;
            return true;
        }

        PGresult *result = sql->QueryVariadic(
            "SELECT password from lobby2.users WHERE lower(passwordrecoveryanswer) = lower(%s) AND userId_pk = %i", passwordRecoveryAnswer.C_String(), userRow);

        if (result==0)
        {
            resultCode=L2RC_Client_GetPasswordByPasswordRecoveryAnswer_BAD_ANSWER;
            return true;
        }

        int numRowsReturned = PQntuples(result);
        if (numRowsReturned==0)
        {
            PQclear(result);
            resultCode=L2RC_Client_GetPasswordByPasswordRecoveryAnswer_BAD_ANSWER;
            return true;
        }

        PostgreSQLInterface::PQGetValueFromBinary(&password, result, 0, "password");
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_ChangeHandle_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        NCount userRow = GetUserRowFromHandle(userName, sql);
        if (userRow==0)
        {
            resultCode=L2RC_UNKNOWN_USER;
            return true;
        }

        NCount userRow2 = GetUserRowFromHandle(newHandle, sql);
        if (userRow2!=0)
        {
            resultCode=L2RC_Client_ChangeHandle_NEW_HANDLE_ALREADY_IN_USE;
            return true;
        }

        if (StringContainsProfanity(newHandle, sql))
        {
            resultCode=L2RC_PROFANITY_FILTER_CHECK_FAILED;
            return true;
        }

        if (requiresPasswordToChangeHandle)
        {
            PGresult *result = sql->QueryVariadic(
                "SELECT password from lobby2.users WHERE userId_pk = %i", userRow);
            if (result==0)
            {
                resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
                return true;
            }
            int numRowsReturned = PQntuples(result);
            if (numRowsReturned==0)
            {
                PQclear(result);
                resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
                return true;
            }
            RakString passwordFromDB;
            PostgreSQLInterface::PQGetValueFromBinary(&passwordFromDB, result, 0, "password");
            PQclear(result);
            if (passwordFromDB!=password)
            {
                PQclear(result);
                resultCode=L2RC_Client_ChangeHandle_INVALID_PASSWORD;
                return true;
            }
        }

        PGresult *result = sql->QueryVariadic(
            "UPDATE lobby2.users SET handle=%s WHERE userId_pk=%i", newHandle.C_String(), userRow);
        PQclear(result);

        // Tell all friends and clanMembers
        DataStructures::List<NCount> output;
        GetFriendIDs(scmd->callerUserId, false, sql, output);
        GetClanMateIDs(scmd->callerUserId, false, sql, output);

        NCount i;
        for (i = 0; i < output.Size(); ++i)
        {
            Notification_User_ChangedHandle * notification = (Notification_User_ChangedHandle *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_User_ChangedHandle);
            notification->oldHandle = userName;
            notification->newHandle = newHandle;
            notification->resultCode = L2RC_SUCCESS;
            scmd->server->AddOutputFromThread(notification, output[i], "");
        }

        scmd->callingUserName=newHandle;
        scmd->server->OnChangeHandle(scmd, true);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_UpdateAccount_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd,
        void * datatype)
    {
        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        PGresult * result = 0;

        if (CreateAccountParametersFailed(createAccountParameters, resultCode, scmd, sql))
            return true;
        result = sql->QueryVariadic(
            "UPDATE lobby2.users SET "
            "firstname=%s, middlename=%s, lastname=%s,"
            "sex_male=%b, homeaddress1=%s, homeaddress2=%s, homecity=%s, "
            "homezipcode=%s, billingaddress1=%s, billingaddress2=%s, billingcity=%s,"
            "billingzipcode=%s, emailaddress=%s, password=%s, passwordrecoveryquestion=%s,"
            "passwordrecoveryanswer=%s, caption1=%s, caption2=%s, dateOfBirth=(select now() - %i * interval '1 day'), binaryData=%a "
            "WHERE userId_pk = %i",
            createAccountParameters.firstName.C_String(), createAccountParameters.middleName.C_String(), createAccountParameters.lastName.C_String(),
            createAccountParameters.sex_male, createAccountParameters.homeAddress1.C_String(), createAccountParameters.homeAddress2.C_String(), createAccountParameters.homeCity.C_String(),
            createAccountParameters.homeZipCode.C_String(), createAccountParameters.billingAddress1.C_String(),	createAccountParameters.billingAddress2.C_String(), createAccountParameters.billingCity.C_String(),
            createAccountParameters.billingZipCode.C_String(), createAccountParameters.emailAddress.C_String(), createAccountParameters.password.C_String(), createAccountParameters.passwordRecoveryQuestion.C_String(),
            createAccountParameters.passwordRecoveryAnswer.C_String(), createAccountParameters.caption1.C_String(),createAccountParameters.caption2.C_String(),
            createAccountParameters.ageInDays, createAccountParameters.binaryData->binaryData,
                createAccountParameters.binaryData->binaryDataLength,
                    scmd->callerUserId);
        if (result == 0)
        {
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        UpdateAccountFromMissingCreationParameters(createAccountParameters, scmd->callerUserId, scmd, sql);

        resultCode = L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_GetAccountDetails_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd,
        void * datatype)
    {
        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        PGresult * result = 0;

        result = sql->QueryVariadic(
            "SELECT firstname, middlename, lastname, (SELECT race_text FROM lobby2.race WHERE "
            "race_id=(SELECT raceid_fk FROM lobby2.users WHERE userId_pk = %i )) as race, sex_male, homeaddress1, "
            "homeaddress2, homecity,(SELECT state_name FROM lobby2.state WHERE state_id=(SELECT homestateid_fk FROM lobby2.users WHERE userId_pk = %i)) as homeState,(SELECT country_name FROM lobby2.country "
            "WHERE country_id=(SELECT homeCountryid_fk FROM lobby2.users WHERE userId_pk = %i)) as homeCountry,homezipcode, billingaddress1, billingaddress2, billingcity,"
            "(SELECT state_name FROM lobby2.state WHERE state_id=(SELECT billingstateid_fk FROM lobby2.users WHERE userId_pk = %i)) as billingState,(SELECT country_name FROM "
            "lobby2.country WHERE country_id=(SELECT billingCountryid_fk FROM lobby2.users WHERE userId_pk = %i)) as billingCountry,billingzipcode, emailaddress, password, passwordrecoveryquestion,"
            "passwordrecoveryanswer, caption1, caption2, (SELECT (EXTRACT(EPOCH FROM now()) - extract(epoch from dateofbirth))/(24*60*60) AS days_old "
            "FROM lobby2.users WHERE userId_pk = %i) as ageInDays, binaryData FROM lobby2.users WHERE userId_pk = %i",
            scmd->callerUserId, scmd->callerUserId, scmd->callerUserId, scmd->callerUserId,
                scmd->callerUserId, scmd->callerUserId, scmd->callerUserId);

        if (result == 0)
        {
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        int numRowsReturned = PQntuples(result);
        if (numRowsReturned == 0)
        {
            PQclear(result);
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.firstName,
            result, 0, "firstname");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.middleName,
            result, 0, "middleName");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.lastName,
            result, 0, "lastName");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.race,
            result, 0, "race");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.sex_male,
            result, 0, "sex_male");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.homeAddress1,
            result, 0, "homeAddress1");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.homeAddress2,
            result, 0, "homeAddress2");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.homeCity,
            result, 0, "homeCity");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.homeState,
            result, 0, "homeState");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.homeCountry,
            result, 0, "homeCountry");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.homeZipCode,
            result, 0, "homeZipCode");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.billingAddress1,
            result, 0, "billingAddress1");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.billingAddress2,
            result, 0, "billingAddress2");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.billingCity,
            result, 0, "billingCity");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.billingState,
            result, 0, "billingState");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.billingCountry,
            result, 0, "billingCountry");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.billingZipCode,
            result, 0, "billingZipCode");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.emailAddress,
            result, 0, "emailAddress");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.password,
            result, 0, "password");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.passwordRecoveryQuestion,
            result, 0, "passwordRecoveryQuestion");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.passwordRecoveryAnswer,
            result, 0, "passwordRecoveryAnswer");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.caption1,
            result, 0, "caption1");
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.caption2,
            result, 0, "caption2");
        NIId d;
        PostgreSQLInterface::PQGetValueFromBinary(&d, result, 0, "ageInDays");
        createAccountParameters.ageInDays = (NCount)d;
        PostgreSQLInterface::PQGetValueFromBinary(&createAccountParameters.binaryData->binaryData,
            &createAccountParameters.binaryData->binaryDataLength, result, 0,
                "binaryData");
        PQclear(result);

        resultCode = L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_StartIgnore_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd,
        void * datatype)
    {
        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        NCount targetUserId = GetUserRowFromHandle(targetHandle, sql);
        if (targetUserId == 0)
        {
            resultCode = L2RC_Client_StartIgnore_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        if (targetUserId == scmd->callerUserId)
        {
            resultCode = L2RC_Client_StartIgnore_CANNOT_PERFORM_ON_SELF;
            return true;
        }

        PGresult * result = sql->QueryVariadic(
            "INSERT INTO lobby2.ignore (userMe_fk, userOther_fk) VALUES (%i, %i)", scmd->callerUserId, targetUserId);

        if (result == 0)
        {
            resultCode = L2RC_Client_StartIgnore_ALREADY_IGNORED;
            return true;
        }
        PQclear(result);

        Notification_Client_IgnoreStatus *notification = (Notification_Client_IgnoreStatus *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Client_IgnoreStatus);
        notification->otherHandle = scmd->callingUserName;
        notification->nowIgnored = true;
        notification->resultCode = L2RC_SUCCESS;
        scmd->server->AddOutputFromThread(notification, targetUserId, targetHandle);

        resultCode = L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_GetIgnoreList_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result = sql->QueryVariadic("SELECT handle FROM lobby2.users WHERE userId_pk="
        "(SELECT userOther_fk FROM lobby2.ignore WHERE userMe_fk=%i);", scmd->callerUserId);

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        RakString handle;
        ignoredHandles.Clear(false);
        int numRowsReturned = PQntuples(result);
        int i;
        for (i=0; i < numRowsReturned; i++)
        {
            PostgreSQLInterface::PQGetValueFromBinary(&handle, result, i, "handle");
            ignoredHandles.Insert(handle);
        }
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_PerTitleIntegerStorage_PGSQL::write( Lobby2ServerCommand *scmd, void *datatype )
    {
        PGresult *result=0;
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        result = sql->QueryVariadic(
            "INSERT INTO lobby2.perTitlePerUserIntegerStorage (titleName_fk,slotIndex,userId_fk,value) VALUES (%s,%i,%i,%g);",
            titleName.C_String(), slotIndex, scmd->callerUserId, inputValue);
        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_PerTitleIntegerStorage_PGSQL::read( Lobby2ServerCommand *scmd, void *datatype )
    {
        PGresult *result=0;
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;

        result = sql->QueryVariadic("SELECT value FROM lobby2.perTitlePerUserIntegerStorage WHERE titleName_fk=%s AND slotIndex=%i AND userId_fk=%i", titleName.C_String(), slotIndex, scmd->callerUserId);
        int numRowsReturned = PQntuples(result);
        if (numRowsReturned==0)
        {
            PQclear(result);
            resultCode=L2RC_Client_PerTitleIntegerStorage_ROW_EMPTY;
            return true;
        }

        PostgreSQLInterface::PQGetValueFromBinary(&outputValue, result, 0, "value");
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_PerTitleIntegerStorage_PGSQL::Delete( Lobby2ServerCommand *scmd, void *datatype )
    {
        PGresult *result=0;
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        result = sql->QueryVariadic("DELETE FROM lobby2.perTitlePerUserIntegerStorage WHERE titleName_fk=%s AND slotIndex=%i AND userId_fk=%i", titleName.C_String(), slotIndex, scmd->callerUserId);
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_PerTitleIntegerStorage_PGSQL::Add( Lobby2ServerCommand *scmd, void *datatype )
    {
        // In MySQL I think you can do this:
        // INSERT INTO lobby2.perTitlePerUserIntegerStorage (titleName_fk,slotIndex,value) VALUES ('tn',1,2) ON DUPLICATE KEY UPDATE titleName_fk='tn2', slotIndex=2, value=3;
        // But not in PostgreSQL

        read(scmd, datatype);
        if (resultCode==L2RC_Client_PerTitleIntegerStorage_ROW_EMPTY)
            return write(scmd, datatype);

        outputValue+=inputValue;

        PGresult *result=0;
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        switch (addConditionForOperation)
        {
        case PTISC_EQUAL:
            result = sql->QueryVariadic(
                "UPDATE lobby2.perTitlePerUserIntegerStorage SET value=%g WHERE titleName_fk=%s AND slotIndex=%i AND userId_fk=%i AND value=%g RETURNING value;",
                titleName.C_String(), slotIndex, scmd->callerUserId, outputValue, conditionValue);
            break;
        case PTISC_NOT_EQUAL:
            result = sql->QueryVariadic(
                "UPDATE lobby2.perTitlePerUserIntegerStorage SET value=%g WHERE titleName_fk=%s AND slotIndex=%i AND userId_fk=%i AND value!=%g RETURNING value;",
                titleName.C_String(), slotIndex, scmd->callerUserId, outputValue);
            break;
        case PTISC_GREATER_THAN:
            result = sql->QueryVariadic(
                "UPDATE lobby2.perTitlePerUserIntegerStorage SET value=%g WHERE titleName_fk=%s AND slotIndex=%i AND userId_fk=%i AND value<%g RETURNING value;",
                titleName.C_String(), slotIndex, scmd->callerUserId, outputValue, conditionValue);
            break;
        case PTISC_GREATER_OR_EQUAL:
            result = sql->QueryVariadic(
                "UPDATE lobby2.perTitlePerUserIntegerStorage SET value=%g WHERE titleName_fk=%s AND slotIndex=%i AND userId_fk=%i AND value<=%g RETURNING value;",
                titleName.C_String(), slotIndex, scmd->callerUserId, outputValue, conditionValue);
            break;
        case PTISC_LESS_THAN:
            result = sql->QueryVariadic(
                "UPDATE lobby2.perTitlePerUserIntegerStorage SET value=%g WHERE titleName_fk=%s AND slotIndex=%i AND userId_fk=%i AND value>%g RETURNING value;",
                titleName.C_String(), slotIndex, scmd->callerUserId, outputValue, conditionValue);
            break;
        case PTISC_LESS_OR_EQUAL:
            result = sql->QueryVariadic(
                "UPDATE lobby2.perTitlePerUserIntegerStorage SET value=%g WHERE titleName_fk=%s AND slotIndex=%i AND userId_fk=%i AND value>=%g RETURNING value;",
                titleName.C_String(), slotIndex, scmd->callerUserId, outputValue, conditionValue);
            break;
        }

        int numRowsReturned = PQntuples(result);
        if (numRowsReturned==0)
        {
            PQclear(result);
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        PostgreSQLInterface::PQGetValueFromBinary(&outputValue, result, 0, "value");
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_PerTitleIntegerStorage_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        // Verify title name
        if (IsTitleInUse(titleName, sql)==false)
        {
            resultCode=L2RC_Client_PerTitleIntegerStorage_TITLE_NOT_IN_USE;
            return true;
        }

        switch (operationToPerform)
        {
        case PTISO_WRITE:
            return write(scmd,datatype);
            break;
        case PTISO_READ:
            return read(scmd,datatype);
            break;
        case PTISO_DELETE:
            return Delete(scmd,datatype);
            break;
        case PTISO_ADD:
            return Add(scmd,datatype);
            break;
        }

        return true;
    }
    //------------------------------------------------------------------------
    bool Client_PerTitleBinaryStorage_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd,
        void * datatype)
    {
        (void)scmd;

        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        PGresult * result = 0;
        // Verify title name
        if (IsTitleInUse(titleName, sql) == false)
        {
            resultCode = L2RC_Client_PerTitleBinaryStorage_TITLE_NOT_IN_USE;
            return true;
        }

        int numRowsReturned;
        switch (operationToPerform)
        {
        case PTISO_WRITE:
            result = sql->QueryVariadic(
                "INSERT INTO lobby2.perTitlePerUserBinaryStorage (titleName_fk,slotIndex,userId_fk,binaryData) VALUES (%s,%i,%i,%a);",
                    titleName.C_String(), slotIndex, scmd->callerUserId, binaryData->binaryData,
                        binaryData->binaryDataLength);

            if (result == 0)
            {
                resultCode = L2RC_GENERAL_ERROR;
                return true;
            }
            break;
        case PTISO_READ:
            result = sql->QueryVariadic(
                "SELECT binaryData FROM lobby2.perTitlePerUserBinaryStorage WHERE titleName_fk=%s AND slotIndex=%i AND userId_fk=%i;",
                    titleName.C_String(), slotIndex, scmd->callerUserId);

            if (result == 0)
            {
                resultCode = L2RC_GENERAL_ERROR;
                return true;
            }

            numRowsReturned = PQntuples(result);
            if (numRowsReturned == 0)
            {
                resultCode = L2RC_Client_PerTitleBinaryStorage_ROW_EMPTY;
                return true;
            }

            PostgreSQLInterface::PQGetValueFromBinary(&binaryData->binaryData,
                &binaryData->binaryDataLength, result, 0, "binaryData");
            break;
        case PTISO_DELETE:
            result = sql->QueryVariadic("DELETE FROM lobby2.perTitlePerUserBinaryStorage WHERE titleName_fk=%s AND slotIndex=%i AND userId_fk=%i;",
                titleName.C_String(), slotIndex, scmd->callerUserId);
            break;
        }

        PQclear(result);

        resultCode = L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_SetPresence_PGSQL::ServerPreDBMemoryImpl(Lobby2Server * server,
        RakString userHandle)
    {
        server->SetPresence(presence, userHandle);
        resultCode = L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_GetPresence_PGSQL::ServerPreDBMemoryImpl(Lobby2Server * server,
        RakString userHandle)
    {
        server->GetPresence(presence, userHandle);
        resultCode = L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Client_StopIgnore_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd,
        void * datatype)
    {
        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        NCount targetUserId = GetUserRowFromHandle(targetHandle, sql);
        if (targetUserId == 0)
        {
            resultCode = L2RC_Client_StopIgnore_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        if (targetUserId == scmd->callerUserId)
        {
            resultCode = L2RC_Client_StopIgnore_CANNOT_PERFORM_ON_SELF;
            return true;
        }

        PGresult * result = sql->QueryVariadic(
            "DELETE FROM lobby2.ignore WHERE userMe_fk=%i AND userOther_fk=%i", scmd->callerUserId, targetUserId);
        PQclear(result);

        Notification_Client_IgnoreStatus *notification = (Notification_Client_IgnoreStatus *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Client_IgnoreStatus);
        notification->otherHandle=scmd->callingUserName;
        notification->nowIgnored=false;
        notification->resultCode=L2RC_SUCCESS;
        scmd->server->AddOutputFromThread(notification, targetUserId, targetHandle);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Friends_SendInvite_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount targetUserId = GetUserRowFromHandle(targetHandle, sql);
        if (targetUserId==0)
        {
            resultCode=L2RC_Friends_SendInvite_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        if (targetUserId==scmd->callerUserId)
        {
            resultCode=L2RC_Friends_SendInvite_CANNOT_PERFORM_ON_SELF;
            return true;
        }

        // Don't do if already in friends table (already friends, or already has an invite)
        result = sql->QueryVariadic(
            "SELECT description FROM lobby2.friendActions WHERE actionId_pk="
            "(SELECT actionId_fk from lobby2.friends WHERE userOne_fk=%i AND userTwo_fk=%i);"
        , scmd->callerUserId, targetUserId);
        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        int numRowsReturned = PQntuples(result);
        if (numRowsReturned!=0)
        {
            RakString description;
            PostgreSQLInterface::PQGetValueFromBinary(&description, result, 0, "description");
            if (description=="sentInvite")
                resultCode=L2RC_Friends_SendInvite_ALREADY_SENT_INVITE;
            else if (description=="isFriends")
                resultCode=L2RC_Friends_SendInvite_ALREADY_FRIENDS;
            else
                resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            PQclear(result);
            return true;
        }
        PQclear(result);

        // Add friend invite
        result = sql->QueryVariadic(
            "INSERT INTO lobby2.friends (userOne_fk, userTwo_fk, actionId_fk) VALUES "
            "(%i, %i, (SELECT actionId_pk FROM lobby2.friendActions WHERE description='sentInvite'));"
            , scmd->callerUserId, targetUserId);
        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        PQclear(result);

        // Notify by email
        SendEmail(targetUserId, scmd->callerUserId, scmd->callingUserName, scmd->server, subject, body, binaryData, emailStatus, "Friends_SendInvite", sql);

        // Tell the other system the invitation was sent
        Notification_Friends_StatusChange *notification = (Notification_Friends_StatusChange *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Friends_StatusChange);
        N_assert(scmd->callingUserName.IsEmpty()==false);
        notification->otherHandle=scmd->callingUserName;
        notification->subject=subject;
        notification->body=body;
        notification->op=Notification_Friends_StatusChange::GOT_INVITATION_TO_BE_FRIENDS;
        notification->resultCode=L2RC_SUCCESS;
        scmd->server->AddOutputFromThread(notification, targetUserId, targetHandle);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Friends_AcceptInvite_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount targetUserId = GetUserRowFromHandle(targetHandle, sql);
        if (targetUserId==0)
        {
            resultCode=L2RC_Friends_AcceptInvite_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        if (targetUserId==scmd->callerUserId)
        {
            resultCode=L2RC_Friends_AcceptInvite_CANNOT_PERFORM_ON_SELF;
            return true;
        }

        // Make sure we have an invite from the other user
        result = sql->QueryVariadic(
            "SELECT description FROM lobby2.friendActions WHERE actionId_pk="
            "(SELECT actionId_fk from lobby2.friends WHERE userOne_fk=%i AND userTwo_fk=%i);"
            , targetUserId, scmd->callerUserId );

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        int numRowsReturned = PQntuples(result);
        if (numRowsReturned!=0)
        {
            RakString description;
            PostgreSQLInterface::PQGetValueFromBinary(&description, result, 0, "description");
            PQclear(result);
            if (description!=RakString("sentInvite"))
            {
                resultCode=L2RC_Friends_AcceptInvite_NO_INVITE;
                return true;
            }
        }
        else
        {
            PQclear(result);
            resultCode=L2RC_Friends_AcceptInvite_NO_INVITE;
            return true;
        }

        // Change from invited to friends, insert twice
        result = sql->QueryVariadic(
            "UPDATE lobby2.friends SET actionId_fk=(SELECT actionId_pk from lobby2.friendActions WHERE description='isFriends') WHERE userOne_fk=%i AND userTwo_fk=%i;"
            , targetUserId, scmd->callerUserId );
        N_assert(result);
        PQclear(result);

        // Delete any existing invites, etc. if there are any
        result = sql->QueryVariadic(
            "DELETE FROM lobby2.friends WHERE userOne_fk=%i AND userTwo_fk=%i;"
            , scmd->callerUserId, targetUserId );
        PQclear(result);

        // Insert as a friend
        result = sql->QueryVariadic(
            "INSERT INTO lobby2.friends (userOne_fk, userTwo_fk, actionId_fk) VALUES (%i, %i, (SELECT actionId_pk from lobby2.friendActions WHERE description='isFriends'));"
            ,scmd->callerUserId, targetUserId);
        N_assert(result);
        PQclear(result);

        SendEmail(targetUserId, scmd->callerUserId, scmd->callingUserName, scmd->server, subject, body, binaryData, emailStatus, "Friends_AcceptInvite", (PostgreSQLInterface *) datatype);

        // Tell the other system the invitation was accepted
        Notification_Friends_StatusChange *notification = (Notification_Friends_StatusChange *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Friends_StatusChange);
        N_assert(scmd->callingUserName.IsEmpty()==false);
        notification->otherHandle=scmd->callingUserName;
        notification->subject=subject;
        notification->body=body;
        notification->op=Notification_Friends_StatusChange::THEY_ACCEPTED_OUR_INVITATION_TO_BE_FRIENDS;
        notification->resultCode=L2RC_SUCCESS;
        scmd->server->AddOutputFromThread(notification, targetUserId, targetHandle);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    void Friends_AcceptInvite_PGSQL::ServerPostDBMemoryImpl( Lobby2Server *server, RakString userHandle )
    {
        (void)userHandle;
        server->GetPresence(presence,targetHandle);
    }
    //------------------------------------------------------------------------
    bool Friends_RejectInvite_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount targetUserId = GetUserRowFromHandle(targetHandle, sql);
        if (targetUserId==0)
        {
            resultCode=L2RC_Friends_RejectInvite_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        if (targetUserId==scmd->callerUserId)
        {
            resultCode=L2RC_Friends_RejectInvite_CANNOT_PERFORM_ON_SELF;
            return true;
        }

        // Make sure we have an invite from the other user
        result = sql->QueryVariadic(
            "SELECT description FROM lobby2.friendActions WHERE actionId_pk="
            "(SELECT actionId_fk from lobby2.friends WHERE userOne_fk=%i AND userTwo_fk=%i);"
            , targetUserId, scmd->callerUserId );

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        int numRowsReturned = PQntuples(result);
        if (numRowsReturned!=0)
        {
            RakString description;
            PostgreSQLInterface::PQGetValueFromBinary(&description, result, 0, "description");
            PQclear(result);
            if (description!=RakString("sentInvite"))
            {
                resultCode=L2RC_Friends_RejectInvite_NO_INVITE;
                return true;
            }
        }
        else
        {
            PQclear(result);
            resultCode=L2RC_Friends_RejectInvite_NO_INVITE;
            return true;
        }

        // Delete friend invite (both ways)
        result = sql->QueryVariadic(
            "DELETE FROM lobby2.friends WHERE userOne_fk=%i AND userTwo_fk=%i;"
            , targetUserId, scmd->callerUserId );
        PQclear(result);
        result = sql->QueryVariadic(
            "DELETE FROM lobby2.friends WHERE userOne_fk=%i AND userTwo_fk=%i;"
            , scmd->callerUserId, targetUserId );
        PQclear(result);

        SendEmail(targetUserId, scmd->callerUserId, scmd->callingUserName, scmd->server, subject, body, binaryData, emailStatus, "Friends_RejectInvite", (PostgreSQLInterface *) datatype);

        // Tell the other system the invitation was rejected
        Notification_Friends_StatusChange *notification = (Notification_Friends_StatusChange *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Friends_StatusChange);
        N_assert(scmd->callingUserName.IsEmpty()==false);
        notification->otherHandle=scmd->callingUserName;
        notification->subject=subject;
        notification->body=body;
        notification->op=Notification_Friends_StatusChange::THEY_REJECTED_OUR_INVITATION_TO_BE_FRIENDS;
        notification->resultCode=L2RC_SUCCESS;
        scmd->server->AddOutputFromThread(notification, targetUserId, targetHandle);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Friends_GetInvites_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        GetFriendInfosByStatus(scmd->callerUserId, "sentInvite", sql, invitesSent, true);
        GetFriendInfosByStatus(scmd->callerUserId, "sentInvite", sql, invitesReceived, false);
        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    void Friends_GetInvites_PGSQL::ServerPostDBMemoryImpl( Lobby2Server *server, RakString userHandle )
    {
        (void)userHandle;

        for (NCount i=0; i < invitesSent.Size(); i++)
            server->GetUserOnlineStatus(invitesSent[i].usernameAndStatus);
        for (NCount i=0; i < invitesReceived.Size(); i++)
            server->GetUserOnlineStatus(invitesReceived[i].usernameAndStatus);
    }
    //------------------------------------------------------------------------
    bool Friends_GetFriends_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        GetFriendInfosByStatus(scmd->callerUserId, "isFriends", sql, myFriends, true);
        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    void Friends_GetFriends_PGSQL::ServerPostDBMemoryImpl( Lobby2Server *server, RakString userHandle )
    {
        (void)userHandle;

        for (NCount i=0; i < myFriends.Size(); i++)
            server->GetUserOnlineStatus(myFriends[i].usernameAndStatus);
    }
    //------------------------------------------------------------------------
    bool Friends_Remove_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;
        NCount targetUserId = GetUserRowFromHandle(targetHandle, sql);
        if (targetUserId==0)
        {
            resultCode=L2RC_Friends_Remove_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        if (targetUserId==scmd->callerUserId)
        {
            resultCode=L2RC_Friends_Remove_CANNOT_PERFORM_ON_SELF;
            return true;
        }

        result = sql->QueryVariadic(
            "SELECT userOne_fk FROM lobby2.friends WHERE userOne_fk=%i AND userTwo_fk=%i;"
            , scmd->callerUserId, targetUserId );
        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        int numRowsReturned = PQntuples(result);
        PQclear(result);
        if (numRowsReturned==0)
        {
            resultCode=L2RC_Friends_Remove_NOT_FRIENDS;
            return true;
        }

        // Bidirectional delete
        result = sql->QueryVariadic("DELETE FROM lobby2.friends WHERE (userOne_fk=%i AND userTwo_fk=%i) OR (userOne_fk=%i AND userTwo_fk=%i)",
            scmd->callerUserId, targetUserId, targetUserId, scmd->callerUserId);
        N_assert(result);
        PQclear(result);

        SendEmail(targetUserId, scmd->callerUserId, scmd->callingUserName, scmd->server, subject, body, binaryData, emailStatus, "Friends_Remove", (PostgreSQLInterface *) datatype);

        Notification_Friends_StatusChange *notification = (Notification_Friends_StatusChange *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Friends_StatusChange);
        N_assert(scmd->callingUserName.IsEmpty()==false);
        notification->otherHandle=scmd->callingUserName;
        notification->subject=subject;
        notification->body=body;
        notification->op=Notification_Friends_StatusChange::YOU_WERE_REMOVED_AS_A_FRIEND;
        notification->resultCode=L2RC_SUCCESS;
        scmd->server->AddOutputFromThread(notification, targetUserId, targetHandle);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool BookmarkedUsers_Add_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;
        NCount targetUserId = GetUserRowFromHandle(targetHandle, sql);
        if (targetUserId==0)
        {
            resultCode=L2RC_BookmarkedUsers_Add_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        if (targetUserId==scmd->callerUserId)
        {
            resultCode=L2RC_BookmarkedUsers_Add_CANNOT_PERFORM_ON_SELF;
            return true;
        }

        result = sql->QueryVariadic(
            "INSERT INTO lobby2.bookmarkedUsers (userMe_fk, userOther_fk, type, description) VALUES (%i, %i, %i, %s)",
            scmd->callerUserId, targetUserId, type, description.C_String() );
        if (result==0)
        {
            resultCode=L2RC_BookmarkedUsers_Add_ALREADY_BOOKMARKED;
            return true;
        }
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool BookmarkedUsers_Remove_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;
        NCount targetUserId = GetUserRowFromHandle(targetHandle, sql);
        if (targetUserId==0)
        {
            resultCode=L2RC_BookmarkedUsers_Remove_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        if (targetUserId==scmd->callerUserId)
        {
            resultCode=L2RC_BookmarkedUsers_Remove_CANNOT_PERFORM_ON_SELF;
            return true;
        }

        result = sql->QueryVariadic(
            "DELETE FROM lobby2.bookmarkedUsers WHERE userOther_fk=%i AND type=%i",
            targetUserId, type);
        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool BookmarkedUsers_Get_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;
        result = sql->QueryVariadic(
            "SELECT (SELECT handle FROM lobby2.users where userId_pk=userOther_fk) as handle, *"
            "FROM (SELECT userOther_fk, type, description, creationDate from lobby2.bookmarkedUsers WHERE userMe_fk=%i) as bm ORDER BY type, creationDate ASC;",
            scmd->callerUserId);

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        int numRowsReturned = PQntuples(result);
        int i;
        for (i=0; i < numRowsReturned; i++)
        {
            BookmarkedUser bm;
            PostgreSQLInterface::PQGetValueFromBinary(&bm.targetHandle, result, i, "handle");
            PostgreSQLInterface::PQGetValueFromBinary(&bm.type, result, i, "type");
            PostgreSQLInterface::PQGetValueFromBinary(&bm.description, result, i, "description");
            PostgreSQLInterface::PQGetValueFromBinary(&bm.dateWhenAdded, result, i, "creationDate");
            bookmarkedUsers.Insert(bm);
        }

        PQclear(result);
        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Emails_Send_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        SendEmail(recipients, scmd->callerUserId, scmd->callingUserName, scmd->server, subject, body, binaryData, status, "Emails_Send", (PostgreSQLInterface *) datatype);
        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Emails_Get_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result;

        if (unreadEmailsOnly==true)
        {
            if (emailIdsOnly)
            {
                result = sql->QueryVariadic(
                "SELECT tbl2.emailid_fk from lobby2.users, ( "
                "SELECT tbl1.*, lobby2.emails.creationDate FROM "
                "(SELECT emailId_fk, userMe_fk, userOther_fk, isDeleted, wasRead FROM lobby2.emailTargets) as tbl1, lobby2.emails "
                "WHERE tbl1.emailId_fk=lobby2.emails.emailId_pk AND tbl1.userMe_fk=%i AND tbl1.isDeleted=FALSE AND tbl1.wasRead=FALSE "
                ") as tbl2 "
                "WHERE userId_pk=tbl2.userother_fk ORDER BY creationDate ASC;"
                , scmd->callerUserId);
            }
            else
            {
                result = sql->QueryVariadic(
                "SELECT handle, tbl2.* from lobby2.users, ("
                "SELECT tbl1.*, lobby2.emails.subject, lobby2.emails.body, lobby2.emails.binaryData, lobby2.emails.creationDate FROM"
                "(SELECT emailId_fk, emailTarget_pk, userMe_fk, userOther_fk, status, wasRead, ISentThisEmail, isDeleted FROM lobby2.emailTargets) as tbl1, lobby2.emails "
                "WHERE tbl1.emailId_fk=lobby2.emails.emailId_pk AND tbl1.userMe_fk=%i AND tbl1.isDeleted=FALSE AND tbl1.wasRead=FALSE"
                ") as tbl2 "
                "WHERE userId_pk=tbl2.userother_fk ORDER BY creationDate ASC;"
                , scmd->callerUserId);
            }
        }
        else
        {
            if (emailIdsOnly)
            {
                result = sql->QueryVariadic(
                    "SELECT tbl2.emailid_fk from lobby2.users, ( "
                    "SELECT tbl1.*, lobby2.emails.creationDate FROM "
                    "(SELECT emailId_fk, userMe_fk, userOther_fk, isDeleted FROM lobby2.emailTargets) as tbl1, lobby2.emails "
                    "WHERE tbl1.emailId_fk=lobby2.emails.emailId_pk AND tbl1.userMe_fk=%i AND tbl1.isDeleted=FALSE "
                    ") as tbl2 "
                    "WHERE userId_pk=tbl2.userother_fk ORDER BY creationDate ASC;"
                    , scmd->callerUserId);
            }
            else
            {
                result = sql->QueryVariadic(
                    "SELECT handle, tbl2.* from lobby2.users, ("
                    "SELECT tbl1.*, lobby2.emails.subject, lobby2.emails.body, lobby2.emails.binaryData, lobby2.emails.creationDate FROM"
                    "(SELECT emailId_fk, emailTarget_pk, userMe_fk, userOther_fk, status, wasRead, ISentThisEmail, isDeleted FROM lobby2.emailTargets) as tbl1, lobby2.emails "
                    "WHERE tbl1.emailId_fk=lobby2.emails.emailId_pk AND tbl1.userMe_fk=%i AND tbl1.isDeleted=FALSE"
                    ") as tbl2 "
                    "WHERE userId_pk=tbl2.userother_fk ORDER BY creationDate ASC;"
                    , scmd->callerUserId);
            }
        }

        if (result == 0)
        {
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        int numRowsReturned = PQntuples(result);
        int i;
        for (i=0; i < numRowsReturned; i++)
        {
            EmailResult emailResult;
            RakString otherHandle;
            RakString myHandle = scmd->callingUserName;
            // 4/6/2011 emailTarget_pk is correct, this is used by Emails_Delete and Emails_SetStatus
            // 11/4/2010 - I think this was a copy/paste error
            PostgreSQLInterface::PQGetValueFromBinary(&emailResult.emailID, result, i, "emailTarget_pk");
            // PostgreSQLInterface::PQGetValueFromBinary(&emailResult.emailID, result, i, "emailId_fk");
            bool getThisEmail;
            if (emailsToRetrieve.Size()>0)
            {
                getThisEmail=false;
                for (NCount i=0; i < emailsToRetrieve.Size(); i++)
                {
                    if (emailsToRetrieve[i]==emailResult.emailID)
                    {
                        getThisEmail=true;
                        break;
                    }
                }
            }
            else
                getThisEmail = true;
            if (getThisEmail)
            {
                if (emailIdsOnly == false)
                {
                    PostgreSQLInterface::PQGetValueFromBinary(&otherHandle, result, i, "handle");
                    PostgreSQLInterface::PQGetValueFromBinary(&emailResult.status, result, i, "status");
                    PostgreSQLInterface::PQGetValueFromBinary(&emailResult.wasReadByMe, result, i, "wasRead");
                    PostgreSQLInterface::PQGetValueFromBinary(&emailResult.wasSendByMe, result, i, "ISentThisEmail");
                    PostgreSQLInterface::PQGetValueFromBinary(&emailResult.subject, result, i, "subject");
                    PostgreSQLInterface::PQGetValueFromBinary(&emailResult.body, result, i, "body");
                    PostgreSQLInterface::PQGetValueFromBinary(&emailResult.binaryData->binaryData,
                        &emailResult.binaryData->binaryDataLength, result, i, "binaryData");
                    PostgreSQLInterface::PQGetValueFromBinary(&emailResult.creationDate, result, i, "creationDate");
                    if (emailResult.wasSendByMe)
                    {
                        emailResult.sender = myHandle;
                        emailResult.recipient = otherHandle;
                    }
                    else
                    {
                        emailResult.sender = otherHandle;
                        emailResult.recipient = myHandle;
                    }
                }
                emailResults.Insert(emailResult);
            }
            emailResults.Insert(emailResult);
        }

        resultCode = L2RC_SUCCESS;
        PQclear(result);
        return true;
    }
    //------------------------------------------------------------------------
    bool Emails_Delete_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd, void * datatype)
    {
        (void)scmd;

        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        PGresult * result = sql->QueryVariadic("SELECT isDeleted FROM lobby2.emailTargets WHERE emailTarget_pk = %i", emailId);
        if (result == 0)
        {
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        int numRowsReturned = PQntuples(result);
        if (numRowsReturned == 0)
        {
            PQclear(result);
            resultCode = L2RC_Emails_Delete_UNKNOWN_EMAIL_ID;
            return true;
        }
        bool isDeleted;
        PostgreSQLInterface::PQGetValueFromBinary(&isDeleted, result, 0, "isDeleted");
        PQclear(result);
        if (isDeleted)
        {
            resultCode = L2RC_Emails_Delete_ALREADY_DELETED;
            return true;
        }
        // Don't actually delete, just flag as deleted. This is so the admin can investigate reports of abuse.
        result = sql->QueryVariadic("UPDATE lobby2.emailTargets SET isDeleted=TRUE WHERE emailTarget_pk = %i", emailId);
        if (result == 0)
        {
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        PQclear(result);
        resultCode = L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Emails_SetStatus_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd, void * datatype)
    {
        (void)scmd;

        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        PGresult * result = sql->QueryVariadic("SELECT isDeleted FROM lobby2.emailTargets WHERE emailTarget_pk = %i", emailId);
        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        int numRowsReturned = PQntuples(result);
        if (numRowsReturned==0)
        {
            PQclear(result);
            resultCode=L2RC_Emails_SetStatus_UNKNOWN_EMAIL_ID;
            return true;
        }
        bool isDeleted;
        PostgreSQLInterface::PQGetValueFromBinary(&isDeleted, result, 0, "isDeleted");
        PQclear(result);
        if (isDeleted)
        {
            resultCode=L2RC_Emails_SetStatus_WAS_DELETED;
            return true;
        }
        if (updateStatusFlag)
        {
            result = sql->QueryVariadic("UPDATE lobby2.emailTargets SET status=%i WHERE emailTarget_pk = %i", newStatusFlag, emailId);
            if (result==0)
            {
                resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
                return true;
            }
            PQclear(result);
        }
        if (updateMarkedRead)
        {
            result = sql->QueryVariadic("UPDATE lobby2.emailTargets SET wasRead=%b WHERE emailTarget_pk = %i", isNowMarkedRead, emailId);
            if (result==0)
            {
                resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
                return true;
            }
        }
        PQclear(result);


        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Ranking_SubmitMatch_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;
        // Verify title name
        if (IsTitleInUse(titleName, sql)==false)
        {
            resultCode=L2RC_Ranking_SubmitMatch_TITLE_NOT_IN_USE;
            return true;
        }

        // Insert
        result = sql->QueryVariadic("INSERT INTO lobby2.matches (gameTypeName, titleName_fk, matchNote, binaryData) VALUES "
            "(%s, %s, %s, %a) RETURNING matchId_pk;",
            gameType.C_String(), titleName.C_String(), submittedMatch.matchNote.C_String(),
                submittedMatch.binaryData->binaryData,
                    submittedMatch.binaryData->binaryDataLength);

        int numRowsReturned = PQntuples(result);
        if(numRowsReturned == 0)
        {
            PQclear(result);
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        PostgreSQLInterface::PQGetValueFromBinary(&submittedMatch.matchID, result, 0, "matchId_pk");
        PQclear(result);

        // For each match participant, add to lobby2.matchParticipants
        NCount i;
        for (i=0; i < submittedMatch.matchParticipants.Size(); i++)
        {
            result = sql->QueryVariadic("INSERT INTO lobby2.matchParticipants (matchId_fk, userId_fk, score) VALUES "
                "(%i, (SELECT userId_pk FROM lobby2.users WHERE handleLower=lower(%s)), %f);",
                submittedMatch.matchID, submittedMatch.matchParticipants[i].handle.C_String(), submittedMatch.matchParticipants[i].score);
            // May fail if a user is deleted at the same time this is running
            if (result)
                PQclear(result);
        }

        result = sql->QueryVariadic("SELECT EXTRACT(EPOCH FROM now()) as whenSubmittedDate;");
        PostgreSQLInterface::PQGetValueFromBinary(&submittedMatch.whenSubmittedDate, result, 0, "whenSubmittedDate");
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Ranking_GetMatches_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result1, *result2;

        // Verify title name
        if (IsTitleInUse(titleName, sql)==false)
        {
            resultCode=L2RC_Ranking_SubmitMatch_TITLE_NOT_IN_USE;
            return true;
        }

        result1 = sql->QueryVariadic("SELECT matchId_pk, matchNote, binaryData, EXTRACT(EPOCH FROM creationDate) as creationDate from lobby2.matches WHERE gameTypeName=%s AND titleName_fk=%s;", gameType.C_String(), titleName.C_String());
        if (result1==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        int numMatchesReturned = PQntuples(result1);
        int i;
        for (i=0; i < numMatchesReturned; i++)
        {
            SubmittedMatch submittedMatch;
            MatchParticipant matchParticipant;
            PostgreSQLInterface::PQGetValueFromBinary(&submittedMatch.matchID, result1, i, "matchId_pk");
            PostgreSQLInterface::PQGetValueFromBinary(&submittedMatch.matchNote, result1, i, "matchNote");
            PostgreSQLInterface::PQGetValueFromBinary(&submittedMatch.whenSubmittedDate, result1, i, "creationDate");
            PostgreSQLInterface::PQGetValueFromBinary(&submittedMatch.matchID, result1, i, "matchId_pk");

            result2=sql->QueryVariadic("SELECT (SELECT handle FROM lobby2.users where userId_pk=userId_fk) as handle, score from lobby2.matchParticipants where matchId_fk=%i;", submittedMatch.matchID);
            int numParticipants = PQntuples(result2);
            for (int j=0; j < numParticipants; j++)
            {
                PostgreSQLInterface::PQGetValueFromBinary(&matchParticipant.handle, result2, j, "handle");
                PostgreSQLInterface::PQGetValueFromBinary(&matchParticipant.score, result2, j, "score");
                submittedMatch.matchParticipants.Insert(matchParticipant);
            }

            PQclear(result2);

            submittedMatches.Insert(submittedMatch);
        }
        PQclear(result1);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Ranking_GetMatchBinaryData_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;
        result = sql->QueryVariadic("SELECT binaryData from lobby2.matches WHERE matchId_pk=%i", matchID);
        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        int numRowsReturned = PQntuples(result);
        if (numRowsReturned==0)
        {
            PQclear(result);
            resultCode=L2RC_Ranking_GetMatchBinaryData_INVALID_MATCH_ID;
            return false;
        }

        PostgreSQLInterface::PQGetValueFromBinary(&binaryData->binaryData,
            &binaryData->binaryDataLength, result, 0, "binaryData");
        PQclear(result);
        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Ranking_GetTotalScore_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        // Verify title name
        if (IsTitleInUse(titleName, sql)==false)
        {
            resultCode=L2RC_Ranking_GetTotalScore_TITLE_NOT_IN_USE;
            return true;
        }

        NCount userRow = GetUserRowFromHandle(targetHandle, sql);
        if (userRow==0)
        {
            resultCode=L2RC_UNKNOWN_USER;
            return true;
        }

        result = sql->QueryVariadic("SELECT COUNT(score) as count, sum(score) as sum from lobby2.matchParticipants WHERE userId_fk=%i AND matchId_fk IN"
            "(SELECT matchId_pk from lobby2.matches WHERE gameTypeName=%s AND titleName_fk=%s);", userRow, gameType.C_String(), titleName.C_String());

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        int64_t count;
        PostgreSQLInterface::PQGetValueFromBinary(&count, result, 0, "count");
        numScoresSubmitted = (NCount) count;
        if (numScoresSubmitted>0)
            PostgreSQLInterface::PQGetValueFromBinary(&scoreSum, result, 0, "sum");
        else
            scoreSum=0.0f;
        PQclear(result);
        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Ranking_WipeScoresForPlayer_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        // Verify title name
        if (IsTitleInUse(titleName, sql)==false)
        {
            resultCode=L2RC_Ranking_WipeScoresForPlayer_TITLE_NOT_IN_USE;
            return true;
        }

        NCount userRow = GetUserRowFromHandle(targetHandle, sql);
        if (userRow==0)
        {
            resultCode=L2RC_UNKNOWN_USER;
            return true;
        }

        result = sql->QueryVariadic("DELETE FROM lobby2.matchParticipants WHERE userId_fk=%i AND matchId_fk IN"
            "(SELECT matchId_pk from lobby2.matches WHERE gameTypeName=%s AND titleName_fk=%s);", userRow, gameType.C_String(), titleName.C_String());

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Ranking_WipeMatches_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        // Verify title name
        if (IsTitleInUse(titleName, sql)==false)
        {
            resultCode=L2RC_Ranking_WipeMatches_TITLE_NOT_IN_USE;
            return true;
        }

        result = sql->QueryVariadic("DELETE FROM lobby2.matches WHERE gameTypeName=%s AND titleName_fk=%s;", gameType.C_String(), titleName.C_String());

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Ranking_PruneMatches_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;
        result = sql->QueryVariadic("DELETE FROM lobby2.matches WHERE creationDate < (select now() - %i * interval '1 day')", pruneTimeDays);

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Ranking_UpdateRating_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        if (IsTitleInUse(titleName, sql)==false)
        {
            resultCode=L2RC_Ranking_UpdateRating_TITLE_NOT_IN_USE;
            return true;
        }

        NCount targetUserId = GetUserRowFromHandle(targetHandle, sql);
        if (targetUserId==0)
        {
            resultCode=L2RC_Ranking_UpdateRating_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        result = sql->QueryVariadic("INSERT INTO lobby2.ratings (userId_fk, gameTypeName, titleName_fk, userRating) VALUES (%i, %s, %s, %f)", targetUserId, gameType.C_String(), titleName.C_String(), targetRating);

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Ranking_WipeRatings_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        // Verify title name
        if (IsTitleInUse(titleName, sql)==false)
        {
            resultCode=L2RC_Ranking_WipeRatings_TITLE_NOT_IN_USE;
            return true;
        }

        result = sql->QueryVariadic("DELETE FROM lobby2.ratings WHERE gameTypeName=%s AND titleName_fk=%s;", gameType.C_String(), titleName.C_String());

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Ranking_GetRating_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        if (IsTitleInUse(titleName, sql)==false)
        {
            resultCode=L2RC_Ranking_GetRating_TITLE_NOT_IN_USE;
            return true;
        }

        NCount targetUserId = GetUserRowFromHandle(targetHandle, sql);
        if (targetUserId==0)
        {
            resultCode=L2RC_Ranking_GetRating_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        result = sql->QueryVariadic("SELECT userRating, creationDate FROM lobby2.ratings WHERE gameTypeName=%s AND titleName_fk=%s and userId_fk=%i ORDER by creationDate LIMIT 1;", gameType.C_String(), titleName.C_String(), targetUserId);
        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }


        int numRowsReturned = PQntuples(result);
        if (numRowsReturned==0)
        {
            PQclear(result);
            resultCode=L2RC_Ranking_GetRating_NO_RATING;
            return true;
        }

        PostgreSQLInterface::PQGetValueFromBinary(&currentRating, result, 0, "userRating");
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_Create_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        if (StringContainsProfanity(clanHandle, sql))
        {
            resultCode=L2RC_PROFANITY_FILTER_CHECK_FAILED;
            return true;
        }

        if (GetClanIdFromHandle(clanHandle, sql))
        {
            resultCode=L2RC_Clans_Create_CLAN_HANDLE_IN_USE;
            return true;
        }

        result = sql->QueryVariadic(
            "INSERT INTO lobby2.clans (leaderUserId_fk, clanHandle, requiresInvitationsToJoin, description, binaryData) VALUES "
            "(%i, %s, %b, %s, %a) RETURNING clanId_pk;", scmd->callerUserId,
                clanHandle.C_String(), requiresInvitationsToJoin, description.C_String(),
                    binaryData->binaryData, binaryData->binaryDataLength);

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        NCount clanId_pk;
        PostgreSQLInterface::PQGetValueFromBinary(&clanId_pk, result, 0, "clanId_pk");
        PQclear(result);

        if (failIfAlreadyInClan)
        {
            // Checking after rather than before in case this user creates a clan in another thread at the same time
            result = sql->QueryVariadic("SELECT COUNT(*) as count from lobby2.clans WHERE leaderUserId_fk=%i", scmd->callerUserId);
            long long count;
            PostgreSQLInterface::PQGetValueFromBinary(&count, result, 0, "count");
            PQclear(result);
            if (count>1)
            {
                result = sql->QueryVariadic("DELETE FROM lobby2.clans WHERE (clanId_pk=%i);", clanId_pk);
                PQclear(result);
                resultCode=L2RC_Clans_Create_ALREADY_IN_A_CLAN;
                return true;
            }
        }

        // Add yourself as a clan member
        result = sql->QueryVariadic(
            "INSERT INTO lobby2.clanMembers (userId_fk, clanId_fk, isSubleader, memberState_fk) VALUES "
            "(%i, %i, false, (SELECT stateId_Pk FROM lobby2.clanMemberStates WHERE description='ClanMember_Active'));", scmd->callerUserId, clanId_pk );

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        PQclear(result);

        // Tell all friends about this new clan
        DataStructures::List<NCount> output;
        GetFriendIDs(scmd->callerUserId, true, sql, output);

        NCount idx;
        for (idx=0; idx < output.Size(); idx++)
        {
            Notification_Friends_CreatedClan *notification = (Notification_Friends_CreatedClan *)scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Friends_CreatedClan);
            N_assert(scmd->callingUserName.IsEmpty()==false);
            notification->otherHandle=scmd->callingUserName;
            notification->clanName=clanHandle;
            notification->resultCode=L2RC_SUCCESS;
            scmd->server->AddOutputFromThread(notification, output[idx], "");
        }

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_SetProperties_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId == 0)
        {
            resultCode = L2RC_Clans_SetProperties_UNKNOWN_CLAN;
            return true;
        }
        if (IsClanLeader(clanId, scmd->callerUserId, sql)==false)
        {
            resultCode = L2RC_Clans_SetProperties_MUST_BE_LEADER;
            return true;
        }

        result = sql->QueryVariadic("UPDATE lobby2.clans SET description=%s, binaryData=%a WHERE clanId_pk=%i",
            description.C_String(), binaryData->binaryData, binaryData->binaryDataLength,
                clanId);

        if (result == 0)
        {
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        PQclear(result);

        resultCode = L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_GetProperties_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_GetProperties_UNKNOWN_CLAN;
            return true;
        }

        result = sql->QueryVariadic("SELECT description, binaryData FROM lobby2.clans WHERE clanId_pk=%i", clanId);

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        PostgreSQLInterface::PQGetValueFromBinary(&description, result, 0, "description");
        PostgreSQLInterface::PQGetValueFromBinary(&binaryData->binaryData,
            &binaryData->binaryDataLength, result, 0, "binaryData");
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_SetMyMemberProperties_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd,
        void * datatype)
    {
        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        PGresult * result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId == 0)
        {
            resultCode = L2RC_Clans_SetMyMemberProperties_UNKNOWN_CLAN;
            return true;
        }

        bool isSubleader;
        ClanMemberState clanMemberState = GetClanMemberState(clanId, scmd->callerUserId, &isSubleader, sql);
        if (clanMemberState != CMD_ACTIVE)
        {
            resultCode = L2RC_Clans_SetMyMemberProperties_NOT_IN_CLAN;
            return true;
        }

        result = sql->QueryVariadic("UPDATE lobby2.clanMembers SET description=%s, binaryData=%a WHERE userId_fk=%i AND clanId_fk=%i",
            description.C_String(), binaryData->binaryData, binaryData->binaryDataLength,
                scmd->callerUserId, clanId);

        if (result == 0)
        {
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        PQclear(result);

        resultCode = L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_Get_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd, void * datatype)
    {
        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;

        // Gets all clans that I am in, as active
        PGresult * result = sql->QueryVariadic(
            "SELECT U.handle, C.clanHandle, C.description, C.binaryData, C.clanId_fk, C.leaderUserId_fk "
            "FROM lobby2.users AS U, (SELECT clanHandle, description, binaryData, leaderUserId_fk, myClans.* FROM lobby2.clans, "
            "(SELECT clanId_fk FROM lobby2.clanMembers WHERE userId_fk=%i "
            "AND memberState_fk=(SELECT stateId_pk FROM lobby2.clanMemberStates WHERE description='ClanMember_Active')) AS myClans "
            "WHERE clanId_pk=myClans.clanId_fk) AS C WHERE U.userId_pk=C.leaderUserId_fk;", scmd->callerUserId);

        if (result == 0)
        {
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        int numRowsReturned = PQntuples(result);
        if (numRowsReturned == 0)
        {
            PQclear(result);
            resultCode = L2RC_SUCCESS;
            return true;
        }

        for (int i=0; i < numRowsReturned; ++i)
        {
            ClanInfo ci;
            NCount clanId;
            int leaderId;
            PostgreSQLInterface::PQGetValueFromBinary(&ci.clanName, result, i, "clanHandle");
            PostgreSQLInterface::PQGetValueFromBinary(&ci.description, result, i, "description");
            PostgreSQLInterface::PQGetValueFromBinary(&ci.clanLeader, result, i, "handle");
            PostgreSQLInterface::PQGetValueFromBinary(&ci.binaryData->binaryData,
                &ci.binaryData->binaryDataLength, result, i, "binaryData");
            PostgreSQLInterface::PQGetValueFromBinary(&clanId, result, i, "clanId_fk");
            PostgreSQLInterface::PQGetValueFromBinary(&leaderId, result, i, "leaderUserId_fk");

            // Get the names of all other active members in this clan
            PGresult *result2 = sql->QueryVariadic(
                "SELECT U.handle FROM lobby2.clanMembers AS M, lobby2.users AS U "
                "WHERE M.clanId_fk=%i AND M.userId_fk!=%i AND U.userId_pk=M.userId_fk "
                "AND M.memberState_fk=(SELECT stateId_pk FROM lobby2.clanMemberStates WHERE description='ClanMember_Active');",
                clanId, leaderId);

            int numRowsReturned2 = PQntuples(result2);
            RakString memberHandle;
            for (int j=0; j < numRowsReturned2; j++)
            {
                PostgreSQLInterface::PQGetValueFromBinary(&memberHandle, result2, j, "handle");
                ci.clanMembersOtherThanLeader.Insert(memberHandle);
            }
            PQclear(result2);

            clans.Insert(ci);
        }
        PQclear(result);
        resultCode=L2RC_SUCCESS;
        return true;
    };
    //------------------------------------------------------------------------
    bool Clans_GrantLeader_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd,
        void * datatype)
    {
        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        PGresult * result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId == 0)
        {
            resultCode = L2RC_Clans_GrantLeader_UNKNOWN_CLAN;
            return true;
        }

        if (IsClanLeader(clanId, scmd->callerUserId, sql) == false)
        {
            resultCode = L2RC_Clans_GrantLeader_MUST_BE_LEADER;
            return true;
        }

        NCount targetUserId = GetUserRowFromHandle(targetHandle, sql);
        if (targetUserId == 0)
        {
            resultCode=L2RC_Clans_GrantLeader_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        if (targetUserId == scmd->callerUserId)
        {
            resultCode = L2RC_Clans_GrantLeader_CANNOT_PERFORM_ON_SELF;
            return true;
        }

        bool isSubleader;
        ClanMemberState clanMemberState = GetClanMemberState(clanId, targetUserId, &isSubleader, sql);
        if (clanMemberState != CMD_ACTIVE)
        {
            resultCode = L2RC_Clans_GrantLeader_TARGET_NOT_IN_CLAN;
            return true;
        }

        result = sql->QueryVariadic("UPDATE lobby2.clans SET leaderUserId_fk=%i WHERE clanId_pk=%i;",targetUserId, clanId);

        if (result == 0)
        {
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        PQclear(result);
        resultCode=L2RC_SUCCESS;

        Notification_Clans_GrantLeader *notification;
        // Tell all clan members of the new leader
        DataStructures::List<ClanMemberDescriptor> clanMembers;
        GetClanMembers(clanId, clanMembers, sql);

        // Tell all clan members, except the scmd originator, about the new clan leader
        for (NCount i=0; i < clanMembers.Size(); i++)
        {
            if (clanMembers[i].memberState!=CMD_ACTIVE)
                continue;
            if (clanMembers[i].userId==scmd->callerUserId)
                continue;
            notification = (Notification_Clans_GrantLeader *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_GrantLeader);
            notification->clanHandle=clanHandle;
            notification->oldLeader=scmd->callingUserName;
            notification->newLeader=targetHandle;
            scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name);
        }

        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_SetSubleaderStatus_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_SetSubleaderStatus_UNKNOWN_CLAN;
            return true;
        }

        if (IsClanLeader(clanId, scmd->callerUserId, sql)==false)
        {
            resultCode=L2RC_Clans_SetProperties_MUST_BE_LEADER;
            return true;
        }

        NCount targetUserId = GetUserRowFromHandle(targetHandle, sql);
        if (targetUserId==0)
        {
            resultCode=L2RC_Clans_SetSubleaderStatus_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        if (targetUserId==scmd->callerUserId)
        {
            resultCode=L2RC_Clans_SetSubleaderStatus_CANNOT_PERFORM_ON_SELF;
            return true;
        }

        bool isSubleader;
        ClanMemberState clanMemberState = GetClanMemberState(clanId, targetUserId, &isSubleader, sql);
        if (clanMemberState!=CMD_ACTIVE)
        {
            resultCode=L2RC_Clans_SetSubleaderStatus_TARGET_NOT_IN_CLAN;
            return true;
        }
        result = sql->QueryVariadic("UPDATE lobby2.clanMembers SET isSubleader=%b WHERE clanId_fk=%i AND userId_fk=%i;",setToSubleader,clanId,targetUserId);

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        PQclear(result);


        Notification_Clans_SetSubleaderStatus *notification;
        // Tell all clan members of the new leader
        DataStructures::List<ClanMemberDescriptor> clanMembers;
        GetClanMembers(clanId, clanMembers, sql);

        // Tell all clan members, except the scmd originator, about the new clan leader
        for (NCount i=0; i < clanMembers.Size(); i++)
        {
            if (clanMembers[i].memberState!=CMD_ACTIVE)
                continue;
            if (clanMembers[i].userId==scmd->callerUserId)
                continue;
            notification = (Notification_Clans_SetSubleaderStatus *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_SetSubleaderStatus);
            notification->clanHandle=clanHandle;
            notification->leaderHandle=scmd->callingUserName;
            notification->targetHandle=targetHandle;
            notification->setToSubleader=setToSubleader;
            scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name);
        }

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_SetMemberRank_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_SetMemberRank_UNKNOWN_CLAN;
            return true;
        }

        if (IsClanLeader(clanId, scmd->callerUserId, sql)==false)
        {
            resultCode=L2RC_Clans_SetMemberRank_MUST_BE_LEADER;
            return true;
        }

        NCount targetUserId = GetUserRowFromHandle(targetHandle, sql);
        if (targetUserId==0)
        {
            resultCode=L2RC_Clans_SetMemberRank_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        if (targetUserId==scmd->callerUserId)
        {
            resultCode=L2RC_Clans_SetMemberRank_CANNOT_PERFORM_ON_SELF;
            return true;
        }

        bool isSubleader;
        ClanMemberState clanMemberState = GetClanMemberState(clanId, targetUserId, &isSubleader, sql);
        if (clanMemberState!=CMD_ACTIVE)
        {
            resultCode=L2RC_Clans_SetMemberRank_TARGET_NOT_IN_CLAN;
            return true;
        }
        result = sql->QueryVariadic("UPDATE lobby2.clanMembers SET rank=%i WHERE clanId_fk=%i AND userId_fk=%i;",newRank,clanId,targetUserId);

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        PQclear(result);


        Notification_Clans_SetMemberRank *notification;
        // Tell all clan members of the new leader
        DataStructures::List<ClanMemberDescriptor> clanMembers;
        GetClanMembers(clanId, clanMembers, sql);

        // Tell all clan members, except the scmd originator, about the new clan leader
        for (NCount i=0; i < clanMembers.Size(); i++)
        {
            if (clanMembers[i].memberState!=CMD_ACTIVE)
                continue;
            if (clanMembers[i].userId==scmd->callerUserId)
                continue;
            notification = (Notification_Clans_SetMemberRank *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_SetMemberRank);
            notification->clanHandle=clanHandle;
            notification->leaderHandle=scmd->callingUserName;
            notification->targetHandle=targetHandle;
            notification->newRank=newRank;
            scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name);
        }

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_GetMemberProperties_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PGresult *result=0;
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_GetMemberProperties_UNKNOWN_CLAN;
            return true;
        }
        NCount targetUserId = GetUserRowFromHandle(targetHandle, sql);
        if (targetUserId==0)
        {
            resultCode=L2RC_Clans_GetMemberProperties_UNKNOWN_TARGET_HANDLE;
            return true;
        }
        ClanMemberState clanMemberState = GetClanMemberState(clanId, targetUserId, &isSubleader, sql);
        if (clanMemberState==CMD_UNDEFINED)
        {
            resultCode=L2RC_Clans_GetMemberProperties_TARGET_NOT_IN_CLAN;
            return true;
        }
        result = sql->QueryVariadic("SELECT description, binaryData, isSubleader, rank, memberState_fk, banReason FROM lobby2.clanMembers where userId_fk=%i AND clanId_fk=%i",
            targetUserId, clanId);

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        PostgreSQLInterface::PQGetValueFromBinary(&description, result, 0, "description");
        PostgreSQLInterface::PQGetValueFromBinary(&binaryData->binaryData,
            &binaryData->binaryDataLength, result, 0, "binaryData");
        PostgreSQLInterface::PQGetValueFromBinary(&isSubleader, result, 0, "isSubleader");
        PostgreSQLInterface::PQGetValueFromBinary(&rank, result, 0, "rank");
        int cms;
        PostgreSQLInterface::PQGetValueFromBinary(&cms, result, 0, "memberState_fk");
        clanMemberState=(ClanMemberState)cms;
        PostgreSQLInterface::PQGetValueFromBinary(&banReason, result, 0, "banReason");
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_ChangeHandle_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;
        PGresult * result=0;

        NCount clanId = GetClanIdFromHandle(oldClanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_ChangeHandle_UNKNOWN_CLAN;
            return true;
        }

        if (IsClanLeader(clanId, scmd->callerUserId, sql)==false)
        {
            resultCode=L2RC_Clans_ChangeHandle_MUST_BE_LEADER;
            return true;
        }

        result = sql->QueryVariadic("UPDATE lobby2.clans SET clanHandle=%s WHERE clanId_pk=%i;", newClanHandle.C_String(), clanId);

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        PQclear(result);

        Notification_Clans_ChangeHandle *notification;
        // Tell all clan members of the new leader
        DataStructures::List<ClanMemberDescriptor> clanMembers;
        GetClanMembers(clanId, clanMembers, sql);

        // Tell all clan members, except the scmd originator, about the new clan name
        for (NCount i=0; i < clanMembers.Size(); i++)
        {
            if (clanMembers[i].memberState!=CMD_ACTIVE)
                continue;
            if (clanMembers[i].userId==scmd->callerUserId)
                continue;
            notification = (Notification_Clans_ChangeHandle *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_ChangeHandle);
            notification->oldClanHandle=oldClanHandle;
            notification->leaderHandle=scmd->callingUserName;
            notification->newClanHandle=newClanHandle;
            scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name);
        }

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_Leave_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_Leave_UNKNOWN_CLAN;
            return true;
        }

        bool isSubleader;
        ClanMemberState state = GetClanMemberState(clanId, scmd->callerUserId, &isSubleader, sql);
        if (state!=CMD_ACTIVE)
        {
            resultCode=L2RC_Clans_Leave_NOT_IN_CLAN;
            return true;
        }

        bool isClanLeader = IsClanLeader(clanId, scmd->callerUserId, sql);

        // Remove from the clanMembers table
        result = sql->QueryVariadic("DELETE FROM lobby2.clanMembers WHERE userId_fk=%i AND clanId_fk=%i", scmd->callerUserId, clanId);

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        PQclear(result);

        // Tell all clan members of the new leader
        DataStructures::List<ClanMemberDescriptor> clanMembers;
        GetClanMembers(clanId, clanMembers, sql);

        if (subject.IsEmpty()==false || body.IsEmpty()==false)
        {
            // Send this email to the members
            DataStructures::List<NCount> targetUserIds;
            for (NCount i=0; i < clanMembers.Size(); i++)
            {
                if (clanMembers[i].memberState==CMD_ACTIVE)
                    targetUserIds.Insert(clanMembers[i].userId);
            }
            SendEmail(targetUserIds, scmd->callerUserId, scmd->callingUserName, scmd->server, subject, body, binaryData, emailStatus, "Clans_Leave", sql);
        }

        NCount validUserCount=0;
        for (NCount i=0; i < clanMembers.Size(); i++)
        {
            if (clanMembers[i].memberState!=CMD_ACTIVE)
                continue;
            validUserCount++;
        }

        wasDissolved = false;

        if (isClanLeader)
        {
            if (dissolveIfClanLeader || validUserCount==0)
            {
                wasDissolved = true;

                // Send notification to clan members that the clan was destroyed, then destroy the clan
                Notification_Clans_Destroyed *notification;
                for (NCount i=0; i < clanMembers.Size(); i++)
                {
                    if (clanMembers[i].memberState!=CMD_ACTIVE)
                        continue;
                    notification = (Notification_Clans_Destroyed *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_Destroyed);
                    notification->clanHandle=clanHandle;
                    notification->oldClanLeader=scmd->callingUserName;
                    notification->resultCode=L2RC_SUCCESS;
                    scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name);
                }

                // Tell the former leader the clan was destroyed too
                notification = (Notification_Clans_Destroyed *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_Destroyed);
                notification->clanHandle=clanHandle;
                notification->oldClanLeader=scmd->callingUserName;
                notification->resultCode=L2RC_SUCCESS;
                scmd->server->AddOutputFromThread(notification, scmd->callerUserId, scmd->callingUserName);

                // Destroy the clan
                result = sql->QueryVariadic("DELETE FROM lobby2.clans WHERE clanId_pk=%i", clanId);
                PQclear(result);

                resultCode=L2RC_SUCCESS;
                return true;
            }
            else
            {
                // Choose the oldest subleader to lead, or if no subleaders, the oldest member
                result = sql->QueryVariadic("SELECT handle, userId_pk FROM lobby2.users WHERE userId_pk=(SELECT userId_fk FROM lobby2.clanMembers WHERE clanId_fk=%i ORDER BY isSubleader, creationDate DESC LIMIT 1)",
                    clanId);

                int numRowsReturned = PQntuples(result);
                if (numRowsReturned==0)
                {
                    // Destroy the clan if no possible leader (due to asynch)
                    wasDissolved = true;
                    PQclear(result);
                    result = sql->QueryVariadic("DELETE FROM lobby2.clans WHERE clanId_pk=%i", clanId);
                    PQclear(result);
                    resultCode=L2RC_SUCCESS;
                    return true;
                }

                NCount newLeaderId;
                PostgreSQLInterface::PQGetValueFromBinary(&newClanLeader, result, 0, "handle");
                PostgreSQLInterface::PQGetValueFromBinary(&newLeaderId, result, 0, "userId_pk");

                PQclear(result);

                // Promote this member to the leader
                result = sql->QueryVariadic("UPDATE lobby2.clans SET leaderUserId_fk=%i WHERE clanId_pk = %i", newLeaderId, clanId);
                PQclear(result);

                result = sql->QueryVariadic("UPDATE lobby2.clanMembers SET isSubleader=FALSE WHERE userId_fk = %i AND clanId_fk = %i", newLeaderId, clanId);
                PQclear(result);

                // Notify users of new leader
                Notification_Clans_GrantLeader *notification;
                for (NCount i=0; i < clanMembers.Size(); i++)
                {
                    if (clanMembers[i].memberState!=CMD_ACTIVE)
                        continue;
                    if (clanMembers[i].userId==scmd->callerUserId)
                        continue;
                    notification = (Notification_Clans_GrantLeader *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_GrantLeader);
                    notification->clanHandle=clanHandle;
                    notification->newLeader=newClanLeader;
                    notification->oldLeader=scmd->callingUserName;
                    notification->resultCode=L2RC_SUCCESS;
                    scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name);
                }
            }
        }

        // Send notification to clan members that the member has left the clan
        Notification_Clans_Leave *notification;
        for (NCount i=0; i < clanMembers.Size(); i++)
        {
            if (clanMembers[i].memberState!=CMD_ACTIVE)
                continue;
            if (clanMembers[i].userId==scmd->callerUserId)
                continue;
            notification = (Notification_Clans_Leave *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_Leave);
            notification->clanHandle=clanHandle;
            notification->targetHandle=scmd->callingUserName;
            notification->resultCode=L2RC_SUCCESS;
            scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name);
        }

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_SendJoinInvitation_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_SendJoinInvitation_UNKNOWN_CLAN;
            return true;
        }

        bool isSubleader;
        ClanMemberState clanMemberState = GetClanMemberState(clanId, scmd->callerUserId, &isSubleader, sql);
        if (clanMemberState!=CMD_ACTIVE)
        {
            resultCode=L2RC_Clans_SendJoinInvitation_NOT_IN_CLAN;
            return true;
        }

        NCount clanLeaderId = GetClanLeaderId(clanId, sql);

        if (isSubleader==false && clanLeaderId!=scmd->callerUserId)
        {
            resultCode=L2RC_Clans_SendJoinInvitation_MUST_BE_LEADER_OR_SUBLEADER;
            return true;
        }

        // Does target already have an entry?
        NCount targetId = GetUserRowFromHandle(targetHandle, sql);
        if (targetId==0)
        {
            resultCode=L2RC_Clans_SendJoinInvitation_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        if (targetId==scmd->callerUserId)
        {
            resultCode=L2RC_Clans_SendJoinInvitation_CANNOT_PERFORM_ON_SELF;
            return true;
        }

        bool isTargetSubleader;
        ClanMemberState targetClanMemberState = GetClanMemberState(clanId, targetId, &isTargetSubleader, sql);
        if (targetClanMemberState==CMD_ACTIVE)
        {
            // active member
            resultCode=L2RC_Clans_SendJoinInvitation_TARGET_ALREADY_IN_CLAN;
            return true;
        }

        if (targetClanMemberState==CMD_BANNED)
        {
            // banned
            resultCode=L2RC_Clans_SendJoinInvitation_TARGET_IS_BANNED;
            return true;
        }

        if (targetClanMemberState==CMD_JOIN_INVITED)
        {
            // already invited
            resultCode=L2RC_Clans_SendJoinInvitation_REQUEST_ALREADY_PENDING;
            return true;
        }

        if (targetClanMemberState==CMD_JOIN_REQUESTED)
        {
            resultCode=L2RC_Clans_SendJoinInvitation_TARGET_ALREADY_REQUESTED;
            // already requested
            return true;
        }

        // Add row to lobby2.clanMembers
        result = sql->QueryVariadic(
            "INSERT INTO lobby2.clanMembers (userId_fk, clanId_fk, isSubleader, memberState_fk) VALUES "
            "(%i, %i, false, (SELECT stateId_pk FROM lobby2.clanMemberStates WHERE description='ClanMember_JoinInvited') );"
            ,targetId, clanId);
        PQclear(result);

        // Send email to targetman
        SendEmail(targetId, scmd->callerUserId, scmd->callingUserName, scmd->server, subject, body, binaryData, emailStatus, "Clans_SendJoinInvitation", sql);

        // Send notification to target, leader, subleaders about this invite
        Notification_Clans_PendingJoinStatus *notification;
        notification = (Notification_Clans_PendingJoinStatus *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_PendingJoinStatus);
        notification->clanHandle=clanHandle;
        notification->targetHandle=targetHandle;
        notification->sourceHandle=scmd->callingUserName;
        notification->majorOp=Notification_Clans_PendingJoinStatus::JOIN_CLAN_INVITATION;
        notification->minorOp=Notification_Clans_PendingJoinStatus::JOIN_SENT;
        scmd->server->AddOutputFromThread(notification, targetId, targetHandle); // target

        DataStructures::List<ClanMemberDescriptor> clanMembers;
        GetClanMembers(clanId, clanMembers, sql);
        for (NCount i=0; i < clanMembers.Size(); i++)
        {
            if (clanMembers[i].memberState!=CMD_ACTIVE)
                continue;
            if (clanMembers[i].isSubleader==false && clanMembers[i].userId!=clanLeaderId)
                continue;

            notification = (Notification_Clans_PendingJoinStatus *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_PendingJoinStatus);
            notification->clanHandle=clanHandle;
            notification->targetHandle=targetHandle;
            notification->sourceHandle=scmd->callingUserName;
            notification->majorOp=Notification_Clans_PendingJoinStatus::JOIN_CLAN_INVITATION;
            notification->minorOp=Notification_Clans_PendingJoinStatus::JOIN_SENT;
            scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name); // subleader

        }

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_WithdrawJoinInvitation_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_WithdrawJoinInvitation_UNKNOWN_CLAN;
            return true;
        }

        // Does target already have an entry?
        NCount targetId = GetUserRowFromHandle(targetHandle, sql);
        if (targetId==0)
        {
            resultCode=L2RC_Clans_WithdrawJoinInvitation_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        if (targetId==scmd->callerUserId)
        {
            resultCode=L2RC_Clans_WithdrawJoinInvitation_CANNOT_PERFORM_ON_SELF;
            return true;
        }

        bool isSubleader;
        ClanMemberState clanMemberState = GetClanMemberState(clanId, targetId, &isSubleader, sql);
        if (clanMemberState!=CMD_JOIN_INVITED)
        {
            resultCode=L2RC_Clans_WithdrawJoinInvitation_NO_SUCH_INVITATION_EXISTS;
            return true;
        }

        NCount clanLeaderId = GetClanLeaderId(clanId, sql);
        if (isSubleader==false && clanLeaderId!=scmd->callerUserId)
        {
            resultCode=L2RC_Clans_WithdrawJoinInvitation_MUST_BE_LEADER_OR_SUBLEADER;
            return true;
        }

        result = sql->QueryVariadic("DELETE FROM lobby2.clanMembers WHERE userId_fk=%i AND clanId_fk=%i", targetId, clanId);
        PQclear(result);

        // Send email to target
        SendEmail(targetId, scmd->callerUserId, scmd->callingUserName, scmd->server, subject, body, binaryData, emailStatus, "Clans_WithdrawJoinInvitation", sql);

        // Send notification to target, leader, subleaders
        Notification_Clans_PendingJoinStatus *notification;
        notification = (Notification_Clans_PendingJoinStatus *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_PendingJoinStatus);
        notification->clanHandle=clanHandle;
        notification->targetHandle=targetHandle;
        notification->sourceHandle=scmd->callingUserName;
        notification->majorOp=Notification_Clans_PendingJoinStatus::JOIN_CLAN_INVITATION;
        notification->minorOp=Notification_Clans_PendingJoinStatus::JOIN_WITHDRAWN;
        scmd->server->AddOutputFromThread(notification, targetId, targetHandle); // target


        DataStructures::List<ClanMemberDescriptor> clanMembers;
        GetClanMembers(clanId, clanMembers, sql);
        for (NCount i=0; i < clanMembers.Size(); i++)
        {
            if (clanMembers[i].memberState!=CMD_ACTIVE)
                continue;
            if (clanMembers[i].isSubleader==false && clanMembers[i].userId!=clanLeaderId)
                continue;
            if (scmd->callerUserId==clanMembers[i].userId)
                continue;

            notification = (Notification_Clans_PendingJoinStatus *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_PendingJoinStatus);
            notification->clanHandle=clanHandle;
            notification->targetHandle=targetHandle;
            notification->sourceHandle=scmd->callingUserName;
            notification->majorOp=Notification_Clans_PendingJoinStatus::JOIN_CLAN_INVITATION;
            notification->minorOp=Notification_Clans_PendingJoinStatus::JOIN_WITHDRAWN;
            scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name); // subleader

        }

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_AcceptJoinInvitation_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_AcceptJoinInvitation_UNKNOWN_CLAN;
            return true;
        }

        bool isSubleader;
        ClanMemberState clanMemberState = GetClanMemberState(clanId, scmd->callerUserId, &isSubleader, sql);
        if (clanMemberState!=CMD_JOIN_INVITED)
        {
            resultCode=L2RC_Clans_AcceptJoinInvitation_NO_SUCH_INVITATION_EXISTS;
            return true;
        }

        // Change status from invited to clan member
        result = sql->QueryVariadic("UPDATE lobby2.clanMembers SET memberState_fk=(SELECT stateId_pk FROM lobby2.clanMemberStates WHERE description='ClanMember_Active') WHERE userId_fk=%i AND clanId_fk=%i", scmd->callerUserId, clanId);
        PQclear(result);


        // Do AFTER the update in case another thread also added to a clan
        if (failIfAlreadyInClan)
        {
            int count = GetActiveClanCount(scmd->callerUserId, sql);
            if (count>1)
            {
                result = sql->QueryVariadic("DELETE FROM lobby2.clanMembers WHERE userId_fk=%i AND clanId_fk=%i;", scmd->callerUserId, clanId);
                PQclear(result);
                resultCode=L2RC_Clans_AcceptJoinInvitation_ALREADY_IN_DIFFERENT_CLAN;
                return true;
            }
        }

        // Notify all members about this new member
        DataStructures::List<ClanMemberDescriptor> clanMembers;
        GetClanMembers(clanId, clanMembers, sql);
        for (NCount i=0; i < clanMembers.Size(); i++)
        {
            if (clanMembers[i].memberState!=CMD_ACTIVE)
                continue;
            if (clanMembers[i].userId==scmd->callerUserId )
                continue;

            Notification_Clans_NewClanMember *notification = (Notification_Clans_NewClanMember *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_NewClanMember);
            notification->clanHandle=clanHandle;
            notification->targetHandle=scmd->callingUserName;
            scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name); // subleader

        }

        NCount clanLeaderId = GetClanLeaderId(clanId, sql);

        // Send email to leader
        SendEmail(clanLeaderId, scmd->callerUserId, scmd->callingUserName, scmd->server, subject, body, binaryData, emailStatus, "Clans_AcceptJoinInvitation", sql);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_RejectJoinInvitation_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_RejectJoinInvitation_UNKNOWN_CLAN;
            return true;
        }

        bool isSubleader;
        ClanMemberState clanMemberState = GetClanMemberState(clanId, scmd->callerUserId, &isSubleader, sql);
        if (clanMemberState!=CMD_JOIN_INVITED)
        {
            resultCode=L2RC_Clans_RejectJoinInvitation_NO_SUCH_INVITATION_EXISTS;
            return true;
        }

        result = sql->QueryVariadic("DELETE FROM lobby2.clanMembers WHERE userId_fk=%i AND clanId_fk=%i", scmd->callerUserId, clanId);
        PQclear(result);

        NCount clanLeaderId = GetClanLeaderId(clanId, sql);

        // Send email to leader
        SendEmail(clanLeaderId, scmd->callerUserId, scmd->callingUserName, scmd->server, subject, body, binaryData, emailStatus, "Clans_RejectJoinInvitation", sql);

        // Subleader and leader notification
        DataStructures::List<ClanMemberDescriptor> clanMembers;
        GetClanMembers(clanId, clanMembers, sql);
        for (NCount i=0; i < clanMembers.Size(); i++)
        {
            if (clanMembers[i].memberState!=CMD_ACTIVE)
                continue;
            if (clanMembers[i].isSubleader==false && clanMembers[i].userId!=clanLeaderId)
                continue;

            Notification_Clans_PendingJoinStatus *notification = (Notification_Clans_PendingJoinStatus *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_PendingJoinStatus);
            notification->clanHandle=clanHandle;
            notification->sourceHandle=scmd->callingUserName;
            notification->majorOp=Notification_Clans_PendingJoinStatus::JOIN_CLAN_INVITATION;
            notification->minorOp=Notification_Clans_PendingJoinStatus::JOIN_WITHDRAWN;
            scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name); // subleader

        }

        resultCode=L2RC_SUCCESS;
        return true;
    }

    bool Clans_DownloadInvitationList_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result = sql->QueryVariadic(
            " SELECT clanHandle FROM lobby2.clans INNER JOIN "
            " (SELECT clanId_fk FROM lobby2.clanMembers WHERE userId_fk=%i AND memberState_fk = "
            " (SELECT stateId_pk FROM lobby2.clanMemberStates WHERE description='ClanMember_JoinInvited') ) as tbl1 "
            " ON tbl1.clanId_fk = lobby2.clans.clanId_pk;", scmd->callerUserId);

        if (result==0)
        {
            PQclear(result);
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        int numRowsReturned = PQntuples(result);
        int i;
        for (i=0; i < numRowsReturned; i++)
        {
            OpenInvite oi;
            PostgreSQLInterface::PQGetValueFromBinary(&oi.clanHandle, result, i, "clanHandle");
            invitationsSentToMe.Insert(oi);
        }

        PQclear(result);

        // Gets all users in clans that I am in, as active or leader, where an invitation has been sent
        result = sql->QueryVariadic(
        "SELECT creationDate, handle, clanHandle FROM "
        "( "
        "SELECT userId_fk, creationDate, myClans.clanId_fk FROM lobby2.clanMembers INNER JOIN (SELECT clanId_fk FROM lobby2.clanMembers WHERE userId_fk=%i) as myClans ON "
        "myClans.clanId_fk=lobby2.clanMembers.clanId_fk AND memberState_fk=(SELECT stateId_pk FROM lobby2.clanMemberStates WHERE description='ClanMember_JoinInvited') "
        ") as t1 "
        "INNER JOIN lobby2.users as t2 on (t1.userId_fk=t2.userId_pk) "
        "INNER JOIN lobby2.clans as t3 on (t1.clanId_fk=t3.clanId_pk) ", scmd->callerUserId);


        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        numRowsReturned = PQntuples(result);
        if (numRowsReturned==0)
        {
            PQclear(result);
            resultCode=L2RC_SUCCESS;
            return true;
        }

        for (i=0; i < PQntuples(result); i++)
        {
            ClanJoinInvite cjr;
            PostgreSQLInterface::PQGetValueFromBinary(&cjr.sourceClan, result, i, "clanHandle");
            PostgreSQLInterface::PQGetValueFromBinary(&cjr.dateSent, result, i, "creationDate");
            PostgreSQLInterface::PQGetValueFromBinary(&cjr.joinRequestTarget, result, i, "joinRequestTarget");
            usersThatHaveAnInvitationFromClansThatIAmAMemberOf.Insert(cjr);
        }
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_SendJoinRequest_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        clanJoined=false;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_SendJoinRequest_UNKNOWN_CLAN;
            return true;
        }

        bool isSubleader;
        ClanMemberState clanMemberState = GetClanMemberState(clanId, scmd->callerUserId, &isSubleader, sql);
        if (clanMemberState==CMD_ACTIVE)
        {
            resultCode=L2RC_Clans_SendJoinRequest_ALREADY_IN_CLAN;
            return true;
        }
        if (clanMemberState==CMD_BANNED)
        {
            resultCode=L2RC_Clans_SendJoinRequest_BANNED;
            return true;
        }
        if (clanMemberState==CMD_JOIN_REQUESTED)
        {
            resultCode=L2RC_Clans_SendJoinRequest_REQUEST_ALREADY_PENDING;
            return true;
        }
        if (clanMemberState==CMD_JOIN_INVITED)
        {
            resultCode=L2RC_Clans_SendJoinRequest_ALREADY_INVITED;
            return true;
        }

        result = sql->QueryVariadic("SELECT requiresInvitationsToJoin FROM lobby2.clans WHERE clanId_pk=%i",clanId);
        bool requiresInvitationsToJoin;
        PostgreSQLInterface::PQGetValueFromBinary(&requiresInvitationsToJoin, result, 0, "requiresInvitationsToJoin");
        PQclear(result);

        if (requiresInvitationsToJoin==false)
        {
            result = sql->QueryVariadic(
                "INSERT INTO lobby2.clanMembers (userId_fk, clanId_fk, isSubleader, memberState_fk) VALUES "
                "(%i, %i, false, (SELECT stateId_pk FROM lobby2.clanMemberStates WHERE description='ClanMember_Active') );"
                ,scmd->callerUserId, clanId);
            PQclear(result);

            // Send notification all members about the new member
            // Notify all members about this new member
            DataStructures::List<ClanMemberDescriptor> clanMembers;
            GetClanMembers(clanId, clanMembers, sql);
            for (NCount i=0; i < clanMembers.Size(); i++)
            {
                if (clanMembers[i].memberState!=CMD_ACTIVE)
                    continue;
                if (clanMembers[i].userId==scmd->callerUserId)
                    continue;

                Notification_Clans_NewClanMember *notification = (Notification_Clans_NewClanMember *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_NewClanMember);
                notification->clanHandle=clanHandle;
                notification->targetHandle=scmd->callingUserName;
                scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name);
            }

            NCount clanLeaderId = GetClanLeaderId(clanId, sql);

            // Send email to leader
            SendEmail(clanLeaderId, scmd->callerUserId, scmd->callingUserName, scmd->server, subject, body, binaryData, emailStatus, "Clans_SendJoinRequest", sql);

            clanJoined=true;
        }
        else
        {
            // Add row to lobby2.clanMembers
            result = sql->QueryVariadic(
                "INSERT INTO lobby2.clanMembers (userId_fk, clanId_fk, isSubleader, memberState_fk) VALUES "
                "(%i, %i, false, (SELECT stateId_pk FROM lobby2.clanMemberStates WHERE description='ClanMember_JoinRequested') );"
                ,scmd->callerUserId, clanId);
            PQclear(result);

            // Send notification to leader, subleaders about this invite
            NCount clanLeaderId = GetClanLeaderId(clanId, sql);
            DataStructures::List<ClanMemberDescriptor> clanMembers;
            GetClanMembers(clanId, clanMembers, sql);
            for (NCount i=0; i < clanMembers.Size(); i++)
            {
                if (clanMembers[i].memberState!=CMD_ACTIVE)
                    continue;
                if (clanMembers[i].isSubleader==false && clanMembers[i].userId!=clanLeaderId)
                    continue;

                Notification_Clans_PendingJoinStatus *notification = (Notification_Clans_PendingJoinStatus *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_PendingJoinStatus);
                notification->clanHandle=clanHandle;
                notification->targetHandle=clanMembers[i].name;
                notification->sourceHandle=scmd->callingUserName;
                notification->majorOp=Notification_Clans_PendingJoinStatus::JOIN_CLAN_REQUEST;
                notification->minorOp=Notification_Clans_PendingJoinStatus::JOIN_SENT;
                scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name); // subleader
            }
        }

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_WithdrawJoinRequest_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_WithdrawJoinRequest_UNKNOWN_CLAN;
            return true;
        }

        bool isSubleader;
        ClanMemberState clanMemberState = GetClanMemberState(clanId, scmd->callerUserId, &isSubleader, sql);
        if (clanMemberState==CMD_ACTIVE)
        {
            resultCode=L2RC_Clans_WithdrawJoinRequest_ALREADY_IN_CLAN;
            return true;
        }

        if (clanMemberState!=CMD_JOIN_REQUESTED)
        {
            resultCode=L2RC_Clans_WithdrawJoinRequest_NO_SUCH_INVITATION_EXISTS;
            return true;
        }

        result = sql->QueryVariadic("DELETE FROM lobby2.clanMembers WHERE userId_fk=%i AND clanId_fk=%i", scmd->callerUserId, clanId);

        if (result==0)
        {
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        PQclear(result);

        // Send email to leader
        NCount clanLeaderId = GetClanLeaderId(clanId, sql);
        SendEmail(clanLeaderId, scmd->callerUserId, scmd->callingUserName, scmd->server, subject, body, binaryData, emailStatus, "Clans_WithdrawJoinRequest", sql);

        // Send notification to leader, subleaders
        DataStructures::List<ClanMemberDescriptor> clanMembers;
        GetClanMembers(clanId, clanMembers, sql);
        for (NCount i=0; i < clanMembers.Size(); i++)
        {
            if (clanMembers[i].memberState!=CMD_ACTIVE)
                continue;
            if (clanMembers[i].isSubleader==false && clanMembers[i].userId!=clanLeaderId)
                continue;

            Notification_Clans_PendingJoinStatus *notification = (Notification_Clans_PendingJoinStatus *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_PendingJoinStatus);
            notification->clanHandle=clanHandle;
            notification->targetHandle=clanMembers[i].name;
            notification->sourceHandle=scmd->callingUserName;
            notification->majorOp=Notification_Clans_PendingJoinStatus::JOIN_CLAN_REQUEST;
            notification->minorOp=Notification_Clans_PendingJoinStatus::JOIN_WITHDRAWN;
            scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name); // subleader
        }

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_AcceptJoinRequest_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_AcceptJoinRequest_UNKNOWN_CLAN;
            return true;
        }

        bool isSubleader;
        ClanMemberState clanMemberState = GetClanMemberState(clanId, scmd->callerUserId, &isSubleader, sql);
        if (clanMemberState!=CMD_ACTIVE)
        {
            resultCode=L2RC_Clans_AcceptJoinRequest_NOT_IN_CLAN;
            return true;
        }

        NCount clanLeaderId = GetClanLeaderId(clanId, sql);
        if (isSubleader==false && clanLeaderId!=scmd->callerUserId)
        {
            resultCode=L2RC_Clans_AcceptJoinRequest_MUST_BE_LEADER_OR_SUBLEADER;
            return true;
        }

        // Does target already have an entry?
        NCount targetId = GetUserRowFromHandle(requestingUserHandle, sql);
        if (targetId==0)
        {
            resultCode=L2RC_Clans_AcceptJoinRequest_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        if (targetId==scmd->callerUserId)
        {
            resultCode=L2RC_Clans_AcceptJoinRequest_CANNOT_PERFORM_ON_SELF;
            return true;
        }

        bool isTargetSubleader;
        ClanMemberState targetClanMemberState = GetClanMemberState(clanId, targetId, &isTargetSubleader, sql);
        if (targetClanMemberState==CMD_ACTIVE)
        {
            // active member
            resultCode=L2RC_Clans_AcceptJoinRequest_TARGET_ALREADY_IN_CLAN;
            return true;
        }

        if (targetClanMemberState==CMD_BANNED)
        {
            resultCode=L2RC_Clans_AcceptJoinRequest_TARGET_IS_BANNED;
            return true;
        }

        if (targetClanMemberState!=CMD_JOIN_REQUESTED)
        {
            resultCode=L2RC_Clans_AcceptJoinRequest_REQUEST_NOT_PENDING;
            return true;
        }

        // Change status to clan member
        result = sql->QueryVariadic("UPDATE lobby2.clanMembers SET memberState_fk=(SELECT stateId_pk FROM lobby2.clanMemberStates WHERE description='ClanMember_Active') WHERE userId_fk=%i AND clanId_fk=%i", targetId, clanId);
        PQclear(result);

        // Do AFTER the update in case another thread also added to a clan
        if (failIfAlreadyInClan)
        {
            int count = GetActiveClanCount(targetId, sql);
            if (count>1)
            {
                result = sql->QueryVariadic("DELETE FROM lobby2.clanMembers WHERE userId_fk=%i AND clanId_fk=%i;", targetId, clanId);
                PQclear(result);
                resultCode=L2RC_Clans_AcceptJoinRequest_TARGET_ALREADY_IN_DIFFERENT_CLAN;
                return true;
            }
        }

        // Notify all members about this new member
        DataStructures::List<ClanMemberDescriptor> clanMembers;
        GetClanMembers(clanId, clanMembers, sql);
        for (NCount i=0; i < clanMembers.Size(); i++)
        {
            if (clanMembers[i].memberState!=CMD_ACTIVE)
                continue;
            if (clanMembers[i].userId==scmd->callerUserId )
                continue;

            Notification_Clans_NewClanMember *notification = (Notification_Clans_NewClanMember *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_NewClanMember);
            notification->clanHandle=clanHandle;
            notification->targetHandle=requestingUserHandle;
            scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name); // subleader
        }

        // Send email to member
        SendEmail(targetId, scmd->callerUserId, scmd->callingUserName, scmd->server, subject, body, binaryData, emailStatus, "Clans_AcceptJoinRequest", sql);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_RejectJoinRequest_PGSQL::ServerDBImpl(Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_RejectJoinRequest_UNKNOWN_CLAN;
            return true;
        }

        bool isSubleader;
        if (GetClanMemberState(clanId, scmd->callerUserId, &isSubleader, sql)!=CMD_ACTIVE)
        {
            resultCode=L2RC_Clans_RejectJoinRequest_NOT_IN_CLAN;
            return true;
        }

        NCount clanLeaderId = GetClanLeaderId(clanId, sql);

        if (isSubleader==false && clanLeaderId!=scmd->callerUserId)
        {
            resultCode=L2RC_Clans_RejectJoinRequest_MUST_BE_LEADER_OR_SUBLEADER;
            return true;
        }

        NCount targetId = GetUserRowFromHandle(requestingUserHandle, sql);
        if (targetId==0)
        {
            resultCode=L2RC_Clans_RejectJoinRequest_REQUESTING_USER_HANDLE_UNKNOWN;
            return true;
        }

        if (GetClanMemberState(clanId, targetId, &isSubleader, sql)!=CMD_JOIN_REQUESTED)
        {
            resultCode=L2RC_Clans_RejectJoinRequest_NO_SUCH_INVITATION_EXISTS;
            return true;
        }

        result = sql->QueryVariadic("DELETE FROM lobby2.clanMembers WHERE userId_fk=%i AND clanId_fk=%i", targetId, clanId);
        PQclear(result);

        // Subleader and leader notification
        DataStructures::List<ClanMemberDescriptor> clanMembers;
        GetClanMembers(clanId, clanMembers, sql);
        // requestingUserHandle will be notified too, insert a fake record
        ClanMemberDescriptor cmd;
        cmd.memberState=CMD_ACTIVE;
        cmd.isSubleader=true;
        cmd.userId=targetId;
        clanMembers.Insert(cmd);

        for (NCount i=0; i < clanMembers.Size(); i++)
        {
            if (clanMembers[i].memberState!=CMD_ACTIVE)
                continue;
            if (clanMembers[i].isSubleader==false && clanMembers[i].userId!=clanLeaderId)
                continue;

            Notification_Clans_PendingJoinStatus *notification = (Notification_Clans_PendingJoinStatus *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_PendingJoinStatus);
            notification->clanHandle=clanHandle;
            notification->sourceHandle=scmd->callingUserName;
            notification->targetHandle=requestingUserHandle;
            notification->clanMemberHandle=scmd->callingUserName;
            notification->majorOp=Notification_Clans_PendingJoinStatus::JOIN_CLAN_REQUEST;
            notification->minorOp=Notification_Clans_PendingJoinStatus::JOIN_REJECTED;
            notification->resultCode=L2RC_SUCCESS;
            scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name);
        }

        // Send email to member
        SendEmail(targetId, scmd->callerUserId, scmd->callingUserName, scmd->server, subject, body, binaryData, emailStatus, "Clans_RejectJoinRequest", sql);

        resultCode=L2RC_SUCCESS;
        return true;
    };
    //------------------------------------------------------------------------
    bool Clans_DownloadRequestList_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;
        int i;

        // Get all clanMembers that are in state requested of all clans where I am the leader or subleader
        result = sql->QueryVariadic(
            "SELECT C.clanHandle, creationDate, U.handle FROM lobby2.clanMembers INNER JOIN "
            "(SELECT clanId_pk as clanId FROM lobby2.clans WHERE leaderUserId_fk=%i "
            "UNION ALL "
            "SELECT clanId_fk as clanId FROM lobby2.clanMembers WHERE isSubleader=TRUE AND userId_fk=%i) AS clansWhereIAmLeaderOrSubleader "
            "ON clansWhereIAmLeaderOrSubleader.clanId=lobby2.clanMembers.clanId_fk "
            "INNER JOIN lobby2.users U ON U.userId_pk = userId_fk "
            "INNER JOIN lobby2.clans C ON C.clanId_pk = clanId "
            "WHERE lobby2.clanMembers.memberState_fk=(SELECT stateId_pk FROM lobby2.clanMemberStates WHERE description='ClanMember_JoinRequested');",
            scmd->callerUserId, scmd->callerUserId);
        if (result==0)
        {
            PQclear(result);
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        for (i=0; i < PQntuples(result); i++)
        {
            ClanJoinRequest cjr;
            PostgreSQLInterface::PQGetValueFromBinary(&cjr.targetClan, result, i, "clanHandle");
            PostgreSQLInterface::PQGetValueFromBinary(&cjr.dateSent, result, i, "creationDate");
            PostgreSQLInterface::PQGetValueFromBinary(&cjr.joinRequestSender, result, i, "handle");
            joinRequestsToMyClan.Insert(cjr);
        }
        PQclear(result);

        // Get all clanMembers where I am in state requested
        result = sql->QueryVariadic(
            "SELECT C.clanHandle, M.creationDate FROM lobby2.clanMembers AS M, lobby2.clans AS C WHERE "
            "M.userId_fk=%i AND C.clanId_pk=M.clanId_fk AND M.memberState_fk = "
            "(SELECT stateId_pk FROM lobby2.clanMemberStates WHERE description='ClanMember_JoinRequested');",
            scmd->callerUserId);
        if (result==0)
        {
            PQclear(result);
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        for (i=0; i < PQntuples(result); i++)
        {
            ClanJoinRequest cjr;
            PostgreSQLInterface::PQGetValueFromBinary(&cjr.targetClan, result, i, "clanHandle");
            PostgreSQLInterface::PQGetValueFromBinary(&cjr.dateSent, result, i, "creationDate");
            joinRequestsFromMe.Insert(cjr);
        }
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_KickAndBlacklistUser_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;
        (void)datatype;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_KickAndBlacklistUser_UNKNOWN_CLAN;
            return true;
        }

        bool isSubleader;
        if (GetClanMemberState(clanId, scmd->callerUserId, &isSubleader, sql)!=CMD_ACTIVE)
        {
            resultCode=L2RC_Clans_KickAndBlacklistUser_NOT_IN_CLAN;
            return true;
        }

        NCount clanLeaderId = GetClanLeaderId(clanId, sql);
        if (isSubleader==false && clanLeaderId!=scmd->callerUserId)
        {
            resultCode=L2RC_Clans_KickAndBlacklistUser_MUST_BE_LEADER_OR_SUBLEADER;
            return true;
        }

        NCount targetId = GetUserRowFromHandle(targetHandle, sql);
        if (targetId==0)
        {
            resultCode=L2RC_Clans_KickAndBlacklistUser_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        if (targetId==scmd->callerUserId)
        {
            resultCode=L2RC_Clans_KickAndBlacklistUser_CANNOT_PERFORM_ON_SELF;
            return true;
        }

        if (targetId==clanLeaderId)
        {
            resultCode=L2RC_Clans_KickAndBlacklistUser_CANNOT_PERFORM_ON_LEADER;
            return true;
        }

        ClanMemberState clanMemberState = GetClanMemberState(clanId, targetId, &isSubleader, sql);
        if (clanMemberState==CMD_BANNED)
        {
            resultCode=L2RC_Clans_KickAndBlacklistUser_ALREADY_BLACKLISTED;
            return true;
        }

        if (blacklist)
        {
            if (clanMemberState!=CMD_UNDEFINED)
            {
                // Change status to banned
                result = sql->QueryVariadic(
                    "UPDATE lobby2.clanMembers "
                    "SET banReason=%s, memberState_fk=(SELECT stateId_pk FROM lobby2.clanMemberStates WHERE description='ClanMember_Banned') "
                    "WHERE userId_fk=%i AND clanId_fk=%i;", reason.C_String(), targetId, clanId);
            }
            else
            {
                // Add row as banned
                result = sql->QueryVariadic(
                    "INSERT INTO lobby2.clanMembers (userId_fk, clanId_fk, isSubleader, memberState_fk, banReason) VALUES "
                    "(%i, %i, false, (SELECT stateId_pk FROM lobby2.clanMemberStates WHERE description='ClanMember_Banned'), %s );"
                    , targetId, clanId, reason.C_String());
            }
            PQclear(result);
        }
        else if (kick)
        {
            // Remove from the clanMembers table
            result = sql->QueryVariadic("DELETE FROM lobby2.clanMembers WHERE userId_fk=%i AND clanId_fk=%i", targetId, clanId);
            PQclear(result);
        }

        // Subleader and leader notification
        DataStructures::List<ClanMemberDescriptor> clanMembers;
        GetClanMembers(clanId, clanMembers, sql);

        for (NCount i=0; i < clanMembers.Size(); i++)
        {
            if (clanMembers[i].memberState!=CMD_ACTIVE)
                continue;
            if (clanMembers[i].isSubleader==false && clanMembers[i].userId!=clanLeaderId)
                continue;
            if (clanMembers[i].userId==targetId)
                continue;

            Notification_Clans_KickAndBlacklistUser* notification = (Notification_Clans_KickAndBlacklistUser*) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_KickAndBlacklistUser);
            notification->clanHandle=clanHandle;
            notification->targetHandle=targetHandle;
            notification->blacklistingUserHandle=scmd->callingUserName;
            notification->targetHandleWasKicked=(clanMemberState!=CMD_UNDEFINED);
            notification->reason=reason;
            notification->resultCode=L2RC_SUCCESS;
            scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name);
        }

        // If user was in the clan, send Notification_Clans_Leave to them.
        if (clanMemberState!=CMD_UNDEFINED)
        {
            Notification_Clans_Leave *notification = (Notification_Clans_Leave *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_Leave);
            notification->clanHandle=clanHandle;
            notification->targetHandle=targetHandle;
            notification->resultCode=L2RC_SUCCESS;
            scmd->server->AddOutputFromThread(notification, targetId, targetHandle);
        }

        // Send email to member
        SendEmail(targetId, scmd->callerUserId, scmd->callingUserName, scmd->server, subject, body, binaryData, emailStatus, "Clans_KickAndBlacklistUser", sql);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_UnblacklistUser_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_UnblacklistUser_UNKNOWN_CLAN;
            return true;
        }

        bool isSubleader;
        if (GetClanMemberState(clanId, scmd->callerUserId, &isSubleader, sql)!=CMD_ACTIVE)
        {
            resultCode=L2RC_Clans_UnblacklistUser_NOT_IN_CLAN;
            return true;
        }

        NCount clanLeaderId = GetClanLeaderId(clanId, sql);

        if (isSubleader==false && clanLeaderId!=scmd->callerUserId)
        {
            resultCode=L2RC_Clans_UnblacklistUser_MUST_BE_LEADER_OR_SUBLEADER;
            return true;
        }

        NCount targetId = GetUserRowFromHandle(targetHandle, sql);
        if (targetId==0)
        {
            resultCode=L2RC_Clans_UnblacklistUser_UNKNOWN_TARGET_HANDLE;
            return true;
        }

        if (GetClanMemberState(clanId, targetId, &isSubleader, sql)!=CMD_BANNED)
        {
            resultCode=L2RC_Clans_UnblacklistUser_NOT_BLACKLISTED;
            return true;
        }

        // Remove from the clanMembers table
        result = sql->QueryVariadic("DELETE FROM lobby2.clanMembers WHERE userId_fk=%i AND clanId_fk=%i", targetId, clanId);
        PQclear(result);

        // Subleader and leader notification
        DataStructures::List<ClanMemberDescriptor> clanMembers;
        GetClanMembers(clanId, clanMembers, sql);

        for (NCount i=0; i < clanMembers.Size(); i++)
        {
            if (clanMembers[i].memberState!=CMD_ACTIVE)
                continue;
            if (clanMembers[i].isSubleader==false && clanMembers[i].userId!=clanLeaderId)
                continue;

            Notification_Clans_UnblacklistUser *notification = (Notification_Clans_UnblacklistUser *) scmd->server->GetMessageFactory()->Alloc(L2MID_Notification_Clans_UnblacklistUser);
            notification->clanHandle=clanHandle;
            notification->targetHandle=targetHandle;
            notification->unblacklistingUserHandle=scmd->callingUserName;
            notification->resultCode=L2RC_SUCCESS;
            scmd->server->AddOutputFromThread(notification, clanMembers[i].userId, clanMembers[i].name);
        }

        // Send email to member
        SendEmail(targetId, scmd->callerUserId, scmd->callingUserName, scmd->server, subject, body, binaryData, emailStatus, "Clans_UnblacklistUser", sql);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_GetBlacklist_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_GetBlacklist_UNKNOWN_CLAN;
            return true;
        }

        // Get the names of all banned members in this clan
        result = sql->QueryVariadic(
            "SELECT U.handle FROM lobby2.clanMembers AS M, lobby2.users AS U "
            "WHERE M.clanId_fk=%i AND M.userId_fk=U.userId_pk "
            "AND M.memberState_fk=(SELECT stateId_pk FROM lobby2.clanMemberStates WHERE description='ClanMember_Banned');",
            clanId );
        if (result==0)
        {
            PQclear(result);
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        RakString memberName;
        for (int i=0; i < PQntuples(result); i++)
        {
            PostgreSQLInterface::PQGetValueFromBinary(&memberName, result, i, "handle");
            blacklistedUsers.Insert(memberName);
        }
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_GetMembers_PGSQL::ServerDBImpl( Lobby2ServerCommand *scmd, void *datatype )
    {
        (void)scmd;

        PostgreSQLInterface *sql = (PostgreSQLInterface *)datatype;
        PGresult *result=0;

        NCount clanId = GetClanIdFromHandle(clanHandle, sql);
        if (clanId==0)
        {
            resultCode=L2RC_Clans_GetMembers_UNKNOWN_CLAN;
            return true;
        }

        NCount clanLeaderId = GetClanLeaderId(clanId, sql);

        result = sql->QueryVariadic("SELECT handle from lobby2.users WHERE userId_pk=%i", clanLeaderId);
        if (result==0)
        {
            PQclear(result);
            resultCode=L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }
        if (PQntuples(result)!=0)
        {
            PostgreSQLInterface::PQGetValueFromBinary(&clanLeader, result, 0, "handle");
        }
        PQclear(result);

        // Get the names of all other active members in this clan
        result = sql->QueryVariadic(
            "SELECT U.handle FROM lobby2.clanMembers AS M, lobby2.users AS U "
            "WHERE M.clanId_fk=%i AND M.userId_fk!=%i AND M.userId_fk=U.userId_pk "
            "AND M.memberState_fk=(SELECT stateId_pk FROM lobby2.clanMemberStates WHERE description='ClanMember_Active');",
            clanId, clanLeaderId);
        if (result == 0)
        {
            PQclear(result);
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        RakString memberName;
        for (NCount i = 0; i < PQntuples(result); ++i)
        {
            PostgreSQLInterface::PQGetValueFromBinary(&memberName, result, i, "handle");
            clanMembersOtherThanLeader.Insert(memberName);
        }
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
    bool Clans_GetList_PGSQL::ServerDBImpl(Lobby2ServerCommand * scmd, void * datatype)
    {
        (void)scmd;

        PostgreSQLInterface * sql = (PostgreSQLInterface *)datatype;

        PGresult * result = sql->QueryVariadic("SELECT clanhandle from lobby2.clans");
        if (result == 0)
        {
            resultCode = L2RC_DATABASE_CONSTRAINT_FAILURE;
            return true;
        }

        RakString clanName;
        for (NCount i = 0; i < PQntuples(result); ++i)
        {
            PostgreSQLInterface::PQGetValueFromBinary(&clanName, result, i, "clanhandle");
            clanNames.Insert(clanName);
        }
        PQclear(result);

        resultCode=L2RC_SUCCESS;
        return true;
    }
    //------------------------------------------------------------------------
}
}