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
#include "RoomsContainer.h"
#include "ProfanityFilter.h"
#include "N_assert.h"
#include "NiiNetSerializer.h"
#include "TableSerializer.h"

static const TimeDurMS MINIMUM_QUICK_JOIN_TIMEOUT=5000;
static const TimeDurMS MAXIMUM_QUICK_JOIN_TIMEOUT=60000 * 5;
static const int MAX_CUSTOM_QUERY_FIELDS = 50;
static const TimeDurMS PROCESS_QUICK_JOINS_INTERVAL=1000;
#define QUICK_JOIN_ROOM_NAME "Quick Join "

namespace NII
{
namespace NII_NET
{
    DataStructures::Table::FilterQuery RoomQuery::fq[32];
    DataStructures::Table::Cell RoomQuery::cells[32];

    int AllGamesRoomsContainer::RoomsSortByName( Room* const &key, Room* const &data )
    {
        return strcmp(key->GetStringProperty(DefaultRoomColumns::TC_ROOM_NAME),data->GetStringProperty(DefaultRoomColumns::TC_ROOM_NAME));
    }
    // ----------------------------  QuickJoinMember  ----------------------------
    QuickJoinUser::QuickJoinUser()
    {
        networkedQuickJoinUser.query.queries = 0;
        totalTimeWaiting = 0;
    }
    //--------------------------------------------------------------------------
    QuickJoinUser::~QuickJoinUser()
    {
    }
    //------------------------------------------------------------------------
    int QuickJoinUser::SortByTotalTimeWaiting(QuickJoinUser * const & key,
        QuickJoinUser * const & data)
    {
        if (key->totalTimeWaiting > data->totalTimeWaiting)
            return -1;
        if (key->totalTimeWaiting < data->totalTimeWaiting)
            return 1;
        if (key->networkedQuickJoinUser.minimumPlayers > data->networkedQuickJoinUser.minimumPlayers)
            return -1;
        if (key->networkedQuickJoinUser.minimumPlayers < data->networkedQuickJoinUser.minimumPlayers)
            return 1;
        if (key->networkedQuickJoinUser.timeout < data->networkedQuickJoinUser.timeout)
            return -1;
        if (key->networkedQuickJoinUser.timeout > data->networkedQuickJoinUser.timeout)
            return 1;
        if (key < data)
            return -1;
        return -1;
    }
    //------------------------------------------------------------------------
    int QuickJoinUser::SortByMinimumSlots( QuickJoinUser* const &key, QuickJoinUser* const &data )
    {
        if (key->networkedQuickJoinUser.minimumPlayers > data->networkedQuickJoinUser.minimumPlayers)
            return -1;
        if (key->networkedQuickJoinUser.minimumPlayers < data->networkedQuickJoinUser.minimumPlayers)
            return 1;
        if (key->totalTimeWaiting > data->totalTimeWaiting)
            return -1;
        if (key->totalTimeWaiting < data->totalTimeWaiting)
            return 1;
        if (key->networkedQuickJoinUser.timeout < data->networkedQuickJoinUser.timeout)
            return -1;
        if (key->networkedQuickJoinUser.timeout > data->networkedQuickJoinUser.timeout)
            return 1;
        if (key < data)
            return -1;
        return -1;
    }
    //------------------------------------------------------------------------
    void NetworkedQuickJoinUser::read(NetSerializer * in)
    {
        query.read(in);
        in->write(timeout);
        in->write(minimumPlayers);
    }
    //-----------------------------------------------------------------------
    void NetworkedQuickJoinUser::write(NetSerializer * out)
    {
        query.write(out);
        out->read(timeout);
        out->read(minimumPlayers);
    }
    // ----------------------------  RoomMember  ----------------------------
    //------------------------------------------------------------------------
    RoomMember::RoomMember() {isReady=false; newMemberNotificationProcessed=false; joinTime=0; roomMemberMode=RMM_PUBLIC; roomsParticipant=0;}
    RoomMember::~RoomMember() {}

    // ----------------------------  Slots  ----------------------------
    //------------------------------------------------------------------------
    Slots::Slots() {publicSlots=reservedSlots=spectatorSlots=0;}
    Slots::~Slots() {}
    RoomsErrorCode Slots::Validate(void) const
    {
        if (publicSlots+reservedSlots < 1)
            return REC_SLOTS_VALIDATION_NO_PLAYABLE_SLOTS;
        if (publicSlots<0)
            return REC_SLOTS_VALIDATION_NEGATIVE_PUBLIC_SLOTS;
        if (reservedSlots<0)
            return REC_SLOTS_VALIDATION_NEGATIVE_RESERVED_SLOTS;
        if (spectatorSlots<0)
            return REC_SLOTS_VALIDATION_NEGATIVE_SPECTATOR_SLOTS;
        return REC_SUCCESS;
    }
    //------------------------------------------------------------------------
    void Slots::write(NetSerializer * in)
    {
        in->read(publicSlots);
        in->read(reservedSlots);
        in->read(spectatorSlots);
    }
    //------------------------------------------------------------------------
    void Slots::read(NetSerializer * out)
    {
        out->write(publicSlots);
        out->write(reservedSlots);
        out->write(spectatorSlots);
    }
    // ----------------------------  InvitedUser  ----------------------------
    void InvitedUser::write(NetSerializer * in)
    {
        if(room)
            roomId = room->GetID();
        in->read(roomId);
        in->read(invitorName);
        in->read(invitorSystemAddress);
        in->read(target);
        in->read(subject);
        in->read(body);
        in->read(invitedAsSpectator);
    }
    //-----------------------------------------------------------------------
    void InvitedUser::read(NetSerializer * out)
    {
        if (room)
            roomId = room->GetID();
        out->write(roomId);
        out->write(invitorName);
        out->write(invitorSystemAddress);
        out->write(target);
        out->write(subject);
        out->write(body);
        out->write(invitedAsSpectator);
    }
    // ----------------------------  BannedUser  ----------------------------
    void BannedUser::write(NetSerializer * in)
    {
        in->read(target);
        in->read(reason);
    }
    //-----------------------------------------------------------------------
    void BannedUser::read(NetSerializer * out)
    {
        out->write(target);
        out->write(reason);
    }
    // ----------------------------  RoomMemberDescriptor  ----------------------------
    void RoomMemberDescriptor::FromRoomMember(RoomMember *roomMember)
    {
        name=roomMember->roomsParticipant->GetName();
        roomMemberMode=roomMember->roomMemberMode;
        isReady=roomMember->isReady;
        mAddress=roomMember->roomsParticipant->GetSystemAddress();
        guid=roomMember->roomsParticipant->GetGUID();
    }
    //------------------------------------------------------------------------
    void RoomMemberDescriptor::write(NetSerializer * in)
    {
        in->read(name);
        in->read(roomMemberMode);
        in->read(isReady);
        in->read(mAddress);
        in->read(guid);
    }
    //------------------------------------------------------------------------
    void RoomMemberDescriptor::read(NetSerializer * out)
    {
        out->write(name);
        out->write(roomMemberMode);
        out->write(isReady);
        out->write(mAddress);
        out->write(guid);
    }
    // ----------------------------  RemoveUserResult  ----------------------------
    //------------------------------------------------------------------------
    RemoveUserResult::RemoveUserResult()
    {
        removedFromQuickJoin=false;
        removedFromRoom=false;
        room=0;
        gotNewModerator=false;
        roomDestroyed=false;
    }
    //------------------------------------------------------------------------
    RemoveUserResult::~RemoveUserResult()
    {

    }
    //------------------------------------------------------------------------
    void RemoveUserResult::write(NetSerializer * in)
    {
        in->read(removedFromQuickJoin);
        in->read(removedFromRoom);
        if(room)
            roomId = room->GetID();
        in->read(roomId);
        in->read(removedUserName);
        in->read(removedUserAddress);
        in->read(gotNewModerator);
        in->read(roomDestroyed);
        NCount clearedInvitationsSize;
        in->read(clearedInvitationsSize);

        clearedInvitations.Clear(false);
        InvitedUser invitedUser;
        for(NCount i = 0; i < clearedInvitationsSize; ++i)
        {
            invitedUser.write(in);
            clearedInvitations.Insert(invitedUser);
        }
    }
    //------------------------------------------------------------------------
    void RemoveUserResult::read(NetSerializer * out)
    {
        out->write(removedFromQuickJoin);
        out->write(removedFromRoom);
        if(room)
            roomId = room->GetID();
        out->write(roomId);
        out->write(removedUserName);
        out->write(removedUserAddress);
        out->write(gotNewModerator);
        out->write(roomDestroyed);
        NCount clearedInvitationsSize = clearedInvitations.Size();
        out->write(clearedInvitationsSize);
        for(NCount i = 0; i < clearedInvitationsSize; ++i)
        {
            clearedInvitations[i].read(out);
        }
    }
    //------------------------------------------------------------------------
    // ----------------------------  JoinedRoomResult  ----------------------------
    void JoinedRoomResult::write(NetSerializer * in)
    {
        if(acceptedInvitor)
        {
            acceptedInvitorName = acceptedInvitor->GetName();
            acceptedInvitorAddress = acceptedInvitor->GetSystemAddress();
        }
        if(joiningMember)
        {
            joiningMemberName = joiningMember->GetName();
            joiningMemberAddress = joiningMember->GetSystemAddress();
            joiningMemberGuid = joiningMember->GetGUID();
        }
        in->read(acceptedInvitorName);
        in->read(joiningMemberName);
        in->read(acceptedInvitorAddress);
        in->read(joiningMemberAddress);
        in->read(joiningMemberGuid);
        roomDescriptor.FromRoom(roomOutput, agrc);
        roomDescriptor.write(in);
    }
    //------------------------------------------------------------------------
    void JoinedRoomResult::read(NetSerializer * out)
    {
        if(acceptedInvitor)
        {
            acceptedInvitorName = acceptedInvitor->GetName();
            acceptedInvitorAddress = acceptedInvitor->GetSystemAddress();
        }
        if(joiningMember)
        {
            joiningMemberName = joiningMember->GetName();
            joiningMemberAddress = joiningMember->GetSystemAddress();
            joiningMemberGuid = joiningMember->GetGUID();
        }
        out->write(acceptedInvitorName);
        out->write(joiningMemberName);
        out->write(acceptedInvitorAddress);
        out->write(joiningMemberAddress);
        out->write(joiningMemberGuid);
        roomDescriptor.FromRoom(roomOutput, agrc);
        roomDescriptor.read(out);
    }
    //------------------------------------------------------------------------
    // ----------------------------  RoomDescriptor  ----------------------------
    void RoomDescriptor::FromRoom(Room *room, AllGamesRoomsContainer *agrc)
    {
        if(room == 0)
            return;

        Clear();

        roomLockState = room->roomLockState;
        lobbyRoomId = room->lobbyRoomId;
        autoLockReadyStatus = room->autoLockReadyStatus;
        hiddenFromSearches = room->hiddenFromSearches;
        inviteToRoomPermission = room->inviteToRoomPermission;
        inviteToSpectatorSlotPermission = room->inviteToSpectatorSlotPermission;
        RoomMemberDescriptor rmd;
        NCount i;
        for(i = 0; i < room->roomMemberList.Size(); ++i)
        {
            rmd.FromRoomMember(room->roomMemberList[i]);
            roomMemberList.Insert(rmd);
        }
        for(i = 0; i < room->banList.Size(); ++i)
        {
            banList.Insert(room->banList[i]);
        }

        N_assert(agrc);
        if(agrc)
        {
            Room * r;
            agrc->GetRoomProperties(room->GetID(), &r, &roomProperties);
        }
    }
    //------------------------------------------------------------------------
    void RoomDescriptor::write(NetSerializer * in)
    {
        in->read(roomLockState);
        in->read(lobbyRoomId);
        in->read(autoLockReadyStatus);
        in->read(hiddenFromSearches);
        in->read(inviteToRoomPermission);
        in->read(inviteToSpectatorSlotPermission);
        NCount i;
        NCount roomMemberListSize, banListSize;
        in->read(roomMemberListSize);
        for(i = 0; i < roomMemberListSize; ++i)
        {
            RoomMemberDescriptor rmd;
            rmd.write(in);
            roomMemberList.Insert(rmd);
        }
        banListSize;
        in->read(banListSize);
        for(i = 0; i < banListSize; ++i)
        {
            BannedUser bu;
            bu.write(in);
            banList.Insert(bu);
        }

        TableSerializer::DeserializeTable(in, &roomProperties);
    }
    //----------------------------------------------------------------------
    void RoomDescriptor::read(NetSerializer * out)
    {
        out->write(roomLockState);
        out->write(lobbyRoomId);
        out->write(autoLockReadyStatus);
        out->write(hiddenFromSearches);
        out->write(inviteToRoomPermission);
        out->write(inviteToSpectatorSlotPermission);
        NCount i;
        NCount roomMemberListSize, banListSize;
        roomMemberListSize = roomMemberList.Size();
        out->write(roomMemberListSize);
        for(i = 0; i < roomMemberListSize; ++i)
        {
            roomMemberList[i].write(out);
        }
        banListSize = banList.Size();
        out->write(banListSize);
        for(i = 0; i < banListSize; ++i)
        {
            banList[i].write(out);
        }
        TableSerializer::SerializeTable(&roomProperties, out);
    }
    // ----------------------------  RoomQuery  ----------------------------
    //------------------------------------------------------------------------
    RoomsErrorCode RoomQuery::Validate(void)
    {
        if (numQueries > DefaultRoomColumns::TC_TABLE_COLUMNS_COUNT+MAX_CUSTOM_QUERY_FIELDS)
            return REC_ROOM_QUERY_TOO_MANY_QUERIES;
        if (numQueries>0 && queries==0)
            return REC_ROOM_QUERY_INVALID_QUERIES_POINTER;
        return REC_SUCCESS;
    }
    //------------------------------------------------------------------------
    void RoomQuery::write(NetSerializer * in)
    {
        bool hasQuery = numQueries != 0;
        in->read(hasQuery);
        if(hasQuery)
        {
            in->read(numQueries);
            NCount i;
            for(i = 0; i < numQueries; ++i)
            {
                TableSerializer::DeserializeFilterQuery(in, &queries[i]);
            }
        }
    }
    //------------------------------------------------------------------------
    void RoomQuery::read(NetSerializer * out)
    {
        bool hasQuery = numQueries!=0;
        out->write(hasQuery);
        if(hasQuery)
        {
            out->write(numQueries);
            NCount i;
            for(i = 0; i < numQueries; ++i)
            {
                TableSerializer::SerializeFilterQuery(out, &queries[i]);
            }
        }
    }
    // ----------------------------  RoomCreationParameters  ----------------------------
    void NetworkedRoomCreationParameters::write(NetSerializer * in)
    {
        slots.write(in);
        in->read(hiddenFromSearches);
        in->read(destroyOnModeratorLeave);
        in->read(autoLockReadyStatus);
        in->read(inviteToRoomPermission);
        in->read(inviteToSpectatorSlotPermission);
        in->read(clearInvitesOnNewModerator);
        in->read(roomName);
    }
    //----------------------------------------------------------------------------------
    void NetworkedRoomCreationParameters::read(NetSerializer * out)
    {
        slots.read(out);
        out->write(hiddenFromSearches);
        out->write(destroyOnModeratorLeave);
        out->write(autoLockReadyStatus);
        out->write(inviteToRoomPermission);
        out->write(inviteToSpectatorSlotPermission);
        out->write(clearInvitesOnNewModerator);
        out->write(roomName);
    }
    //------------------------------------------------------------------------
    const char *NetworkedRoomCreationParameters::SendInvitePermissionToEnum(SendInvitePermission e)
    {
        switch (e)
        {
        case INVITE_MODE_ANYONE_CAN_INVITE:
            return "INVITE_MODE_ANYONE_CAN_INVITE";
        case INVITE_MODE_MODERATOR_CAN_INVITE:
            return "INVITE_MODE_MODERATOR_CAN_INVITE";
        case INVITE_MODE_PUBLIC_SLOTS_CAN_INVITE:
            return "INVITE_MODE_PUBLIC_SLOTS_CAN_INVITE";
        case INVITE_MODE_RESERVED_SLOTS_CAN_INVITE:
            return "INVITE_MODE_RESERVED_SLOTS_CAN_INVITE";
        case INVITE_MODE_SPECTATOR_SLOTS_CAN_INVITE:
            return "INVITE_MODE_SPECTATOR_SLOTS_CAN_INVITE";
        case INVITE_MODE_MODERATOR_OR_PUBLIC_SLOTS_CAN_INVITE:
            return "INVITE_MODE_MODERATOR_OR_PUBLIC_SLOTS_CAN_INVITE";
        case INVITE_MODE_MODERATOR_OR_PUBLIC_OR_RESERVED_SLOTS_CAN_INVITE:
            return "INVITE_MODE_MODERATOR_OR_PUBLIC_OR_RESERVED_SLOTS_CAN_INVITE";
        }
        return "Error in NetworkedRoomCreationParameters::SendInvitePermissionToEnum";
    }
    //------------------------------------------------------------------------
    RoomCreationParameters::RoomCreationParameters()
    {
        networkedRoomCreationParameters.hiddenFromSearches=false;
        networkedRoomCreationParameters.destroyOnModeratorLeave=false;
        firstUser=0;
        createdRoom=false;
        roomOutput=0;
        networkedRoomCreationParameters.inviteToRoomPermission=NetworkedRoomCreationParameters::INVITE_MODE_ANYONE_CAN_INVITE;
        networkedRoomCreationParameters.inviteToSpectatorSlotPermission=NetworkedRoomCreationParameters::INVITE_MODE_ANYONE_CAN_INVITE;
        networkedRoomCreationParameters.clearInvitesOnNewModerator=false;
    }
    //------------------------------------------------------------------------
    RoomCreationParameters::~RoomCreationParameters()
    {

    }
    //------------------------------------------------------------------------
    RoomsErrorCode RoomCreationParameters::Validate(const DataStructures::List<RakString> &otherRoomNames,
                                                     ProfanityFilter *profanityFilter) const
    {
        static size_t QUICK_JOIN_ROOM_NAME_LENGTH = strlen(QUICK_JOIN_ROOM_NAME);
        if (networkedRoomCreationParameters.roomName.IsEmpty())
            return REC_ROOM_CREATION_PARAMETERS_EMPTY_ROOM_NAME;
        if (profanityFilter && profanityFilter->HasProfanity(networkedRoomCreationParameters.roomName.C_String()))
            return REC_ROOM_CREATION_PARAMETERS_ROOM_NAME_HAS_PROFANITY;
        if (otherRoomNames.GetIndexOf(networkedRoomCreationParameters.roomName)!=(unsigned)-1)
            return REC_ROOM_CREATION_PARAMETERS_ROOM_NAME_IN_USE;
        if (networkedRoomCreationParameters.slots.publicSlots+networkedRoomCreationParameters.slots.reservedSlots<1)
            return REC_ROOM_CREATION_PARAMETERS_NO_PLAYABLE_SLOTS;
        if (networkedRoomCreationParameters.roomName.SubStr(0,(NCount) QUICK_JOIN_ROOM_NAME_LENGTH)==QUICK_JOIN_ROOM_NAME)
            return REC_ROOM_CREATION_PARAMETERS_RESERVED_QUICK_JOIN_ROOM_NAME;
        return REC_SUCCESS;
    }
    //------------------------------------------------------------------------
    // ----------------------------  RoomQuery  ----------------------------

    RoomQuery::RoomQuery() {queries=0; numQueries=0; queriesAllocated=false; SetQueriesToStatic();}
    RoomQuery::~RoomQuery() {}
    void RoomQuery::AddQuery_NUMERIC(const char *columnName, NIId numericValue, DataStructures::Table::FilterQueryType op)
    {
        SetupNextQuery(columnName,op);
        cells[numQueries].Set(numericValue);
        numQueries++;
    }
    //------------------------------------------------------------------------
    void RoomQuery::AddQuery_STRING(const char *columnName, const char *charValue, DataStructures::Table::FilterQueryType op)
    {
        if (charValue==0 || charValue[0]==0)
            return;

        SetupNextQuery(columnName,op);
        cells[numQueries].Set(charValue);
        numQueries++;
    }
    //------------------------------------------------------------------------
    void RoomQuery::AddQuery_BINARY(const char *columnName, const char *input, int inputLength, DataStructures::Table::FilterQueryType op)
    {
        if (input==0 || input[0]==0 || inputLength==0)
            return;
        SetupNextQuery(columnName,op);
        cells[numQueries].Set(input, inputLength);
        numQueries++;
    }
    //------------------------------------------------------------------------
    void RoomQuery::AddQuery_POINTER(const char *columnName, void *ptr, DataStructures::Table::FilterQueryType op)
    {
        SetupNextQuery(columnName,op);
        cells[numQueries].SetPtr(ptr);
        numQueries++;
    }
    //------------------------------------------------------------------------
    void RoomQuery::SetupNextQuery(const char *columnName,DataStructures::Table::FilterQueryType op)
    {
        queries=(DataStructures::Table::FilterQuery *)fq;
        fq[numQueries].cellValue=&cells[numQueries];
        strcpy(fq[numQueries].columnName, columnName);
        fq[numQueries].operation=op;
    }
    //------------------------------------------------------------------------
    void RoomQuery::SetQueriesToStatic(void)
    {
        queries = (DataStructures::Table::FilterQuery *)fq;
        for (int i=0; i < 32; i++)
        {
            fq[i].cellValue=&cells[i];
        }
    }
    // ----------------------------  AllGamesRoomsContainer  ----------------------------
    AllGamesRoomsContainer::AllGamesRoomsContainer()
    {
        DataStructures::Map<GameIdentifier, PerGameRoomsContainer*>::IMPLEMENT_DEFAULT_COMPARISON();
        nextRoomId=0;
    }
    //------------------------------------------------------------------------
    AllGamesRoomsContainer::~AllGamesRoomsContainer()
    {

    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::CreateRoom(RoomCreationParameters *roomCreationParameters,
                                                     ProfanityFilter *profanityFilter)
    {
        if (roomCreationParameters->firstUser->GetRoom())
            return REC_CREATE_ROOM_CURRENTLY_IN_A_ROOM;
        else if (roomCreationParameters->firstUser->GetInQuickJoin())
            return REC_CREATE_ROOM_CURRENTLY_IN_QUICK_JOIN;

        if (perGamesRoomsContainers.Has(roomCreationParameters->gameIdentifier)==false)
            return REC_CREATE_ROOM_UNKNOWN_TITLE;
        PerGameRoomsContainer *perGameRoomsContainer = perGamesRoomsContainers.Get(roomCreationParameters->gameIdentifier);
        RoomsErrorCode roomsErrorCode = perGameRoomsContainer->CreateRoom(roomCreationParameters,profanityFilter,++nextRoomId, true);
        if (roomsErrorCode!=REC_SUCCESS)
            nextRoomId--;
        return roomsErrorCode;
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::EnterRoom(RoomCreationParameters *roomCreationParameters,
                            RoomMemberMode roomMemberMode,
                             ProfanityFilter *profanityFilter,
                             RoomQuery *query,
                             JoinedRoomResult *joinRoomResult)
    {
        roomCreationParameters->roomOutput=0;
        joinRoomResult->roomOutput=0;
        joinRoomResult->agrc=this;
        if (roomCreationParameters->firstUser->GetRoom())
            return REC_ENTER_ROOM_CURRENTLY_IN_A_ROOM;
        else if (roomCreationParameters->firstUser->GetInQuickJoin())
            return REC_ENTER_ROOM_CURRENTLY_IN_QUICK_JOIN;
        roomCreationParameters->createdRoom=false;
        if (perGamesRoomsContainers.Has(roomCreationParameters->gameIdentifier)==false)
            return REC_ENTER_ROOM_UNKNOWN_TITLE;
        PerGameRoomsContainer *perGameRoomsContainer = perGamesRoomsContainers.Get(roomCreationParameters->gameIdentifier);
        RoomsErrorCode roomsErrorCode = perGameRoomsContainer->JoinByFilter(roomMemberMode, roomCreationParameters->firstUser, (RoomID) -1, query, joinRoomResult);

        if (roomsErrorCode != REC_JOIN_BY_FILTER_NO_ROOMS || roomsErrorCode==REC_SUCCESS)
            return roomsErrorCode;
        roomsErrorCode = perGameRoomsContainer->CreateRoom(roomCreationParameters,profanityFilter,++nextRoomId, true);
        if (roomsErrorCode!=REC_SUCCESS)
        {
            nextRoomId--;
        }
        else
        {
            roomCreationParameters->createdRoom=true;
        }
        return roomsErrorCode;
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::JoinByFilter(GameIdentifier gameIdentifier, RoomMemberMode roomMemberMode, RoomsParticipant* roomsParticipant, RoomID lastRoomJoined, RoomQuery *query, JoinedRoomResult *joinRoomResult)
    {
        (void) lastRoomJoined;

        if (roomsParticipant->GetRoom())
            return REC_JOIN_BY_FILTER_CURRENTLY_IN_A_ROOM;
        else if (roomsParticipant->GetInQuickJoin())
            return REC_JOIN_BY_FILTER_CURRENTLY_IN_QUICK_JOIN;

        if (perGamesRoomsContainers.Has(gameIdentifier)==false)
        {
            joinRoomResult->roomOutput=0;
            return REC_JOIN_BY_FILTER_UNKNOWN_TITLE;
        }
        PerGameRoomsContainer *perGameRoomsContainer = perGamesRoomsContainers.Get(gameIdentifier);
        joinRoomResult->agrc=this;
        RoomsErrorCode rec = perGameRoomsContainer->JoinByFilter(roomMemberMode, roomsParticipant, (RoomID) -1, query, joinRoomResult);
        joinRoomResult->roomDescriptor.FromRoom(joinRoomResult->roomOutput, this);
        return rec;
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::LeaveRoom(RoomsParticipant* roomsParticipant, RemoveUserResult *removeUserResult)
    {
        if (roomsParticipant->GetRoom()==false)
            return REC_LEAVE_ROOM_NOT_IN_ROOM;
        else if (roomsParticipant->GetInQuickJoin())
            return REC_LEAVE_ROOM_CURRENTLY_IN_QUICK_JOIN;

        RoomsErrorCode roomsErrorCode = roomsParticipant->GetRoom()->RemoveUser(roomsParticipant, removeUserResult);
        return roomsErrorCode;
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::AddUserToQuickJoin(GameIdentifier gameIdentifier, QuickJoinUser *quickJoinMember)
    {
        if (quickJoinMember->roomsParticipant->GetRoom())
            return REC_ADD_TO_QUICK_JOIN_CURRENTLY_IN_A_ROOM;
        else if (quickJoinMember->roomsParticipant->GetInQuickJoin())
            return REC_ADD_TO_QUICK_JOIN_ALREADY_THERE;

        if (perGamesRoomsContainers.Has(gameIdentifier)==false)
            return REC_ADD_TO_QUICK_JOIN_UNKNOWN_TITLE;
        if (quickJoinMember->networkedQuickJoinUser.timeout < MINIMUM_QUICK_JOIN_TIMEOUT)
            return REC_ADD_TO_QUICK_JOIN_INVALID_TIMEOUT_TOO_LOW;
        if (quickJoinMember->networkedQuickJoinUser.timeout > MAXIMUM_QUICK_JOIN_TIMEOUT)
            return REC_ADD_TO_QUICK_JOIN_INVALID_TIMEOUT_TOO_HIGH;
        if (quickJoinMember->networkedQuickJoinUser.minimumPlayers<2)
            return REC_ADD_TO_QUICK_JOIN_MINIMUM_SLOTS_TOO_LOW;
        if (quickJoinMember->networkedQuickJoinUser.minimumPlayers>5000) // Pretty arbitrary, but who would have this many members in a room?
            return REC_ADD_TO_QUICK_JOIN_MINIMUM_SLOTS_TOO_HIGH;
        return perGamesRoomsContainers.Get(gameIdentifier)->AddUserToQuickJoin(quickJoinMember);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::RemoveUserFromQuickJoin(RoomsParticipant* roomsParticipant, QuickJoinUser **qju)
    {
        RoomsErrorCode roomsErrorCode;
        NCount i;
        for (i=0; i < perGamesRoomsContainers.Size(); i++)
        {
            roomsErrorCode = perGamesRoomsContainers[i]->RemoveUserFromQuickJoin(roomsParticipant, qju);
            if (roomsErrorCode!=REC_REMOVE_FROM_QUICK_JOIN_NOT_THERE)
                return roomsErrorCode;
        }
        return REC_REMOVE_FROM_QUICK_JOIN_NOT_THERE;
    }
    //------------------------------------------------------------------------
    bool AllGamesRoomsContainer::IsInQuickJoin(RoomsParticipant* roomsParticipant)
    {
        NCount i;
        for (i=0; i < perGamesRoomsContainers.Size(); i++)
            if (perGamesRoomsContainers[i]->IsInQuickJoin(roomsParticipant))
                return true;
        return false;
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::SearchByFilter( GameIdentifier gameIdentifier, RoomsParticipant* roomsParticipant, RoomQuery *roomQuery, DataStructures::OrderedList<Room*, Room*, RoomsSortByName> &roomsOutput, bool onlyJoinable )
    {
        roomsOutput.Clear(false);
        if (perGamesRoomsContainers.Has(gameIdentifier)==false)
            return REC_SEARCH_BY_FILTER_UNKNOWN_TITLE;
        PerGameRoomsContainer *perGameRoomsContainer = perGamesRoomsContainers.Get(gameIdentifier);
        return perGameRoomsContainer->SearchByFilter(roomsParticipant, roomQuery, roomsOutput, onlyJoinable);
    }
    //------------------------------------------------------------------------
    void AllGamesRoomsContainer::DestroyRoomIfDead(Room *room)
    {
        if (room==0 || room->IsRoomDead()==false)
            return;
        NCount i;
        for (i=0; i < perGamesRoomsContainers.Size(); i++)
        {
            if (perGamesRoomsContainers[i]->DestroyRoomIfDead(room))
                break;
        }
    }
    //------------------------------------------------------------------------
    void AllGamesRoomsContainer::ChangeHandle(RakString oldHandle, RakString newHandle)
    {
        NCount i;
        for (i=0; i < perGamesRoomsContainers.Size(); i++)
            perGamesRoomsContainers[i]->ChangeHandle(oldHandle, newHandle);
    }
    //------------------------------------------------------------------------
    NCount AllGamesRoomsContainer::GetPropertyIndex(RoomID lobbyRoomId, const char *propertyName) const
    {
        NCount i;
        Room *room;
        for (i=0; i < perGamesRoomsContainers.Size(); i++)
        {
            room = perGamesRoomsContainers[i]->GetRoomByLobbyRoomID(lobbyRoomId);
            if (room)
                return perGamesRoomsContainers[i]->roomsTable.ColumnIndex(propertyName);
        }
        return (NCount) -1;
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::GetInvitesToParticipant(RoomsParticipant * roomsParticipant, DataStructures::List<InvitedUser*> &invites)
    {
        NCount i;
        invites.Clear(true);
        for (i = 0; i < perGamesRoomsContainers.Size(); ++i)
            perGamesRoomsContainers[i]->GetInvitesToParticipant(roomsParticipant, invites);
        return REC_SUCCESS;
    }
    //------------------------------------------------------------------------
    // userLocation is optional, but will speed up the function if it's pre-known
    RoomsErrorCode AllGamesRoomsContainer::RemoveUser(RoomsParticipant* roomsParticipant, RemoveUserResult *removeUserResult)
    {
        if (RemoveUserFromQuickJoin(roomsParticipant, &removeUserResult->qju)!=REC_SUCCESS)
        {
            removeUserResult->qju=0;
            removeUserResult->removedFromQuickJoin=false;
            if (roomsParticipant->GetRoom()==0)
                return REC_REMOVE_USER_NOT_IN_ROOM;

            RoomsErrorCode roomsErrorCode = roomsParticipant->GetRoom()->RemoveUser(roomsParticipant, removeUserResult);
            return roomsErrorCode;
        }
        removeUserResult->removedFromQuickJoin=true;
        return REC_SUCCESS;
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::SendInvite(RoomsParticipant* roomsParticipant, RoomsParticipant* inviteeId, bool inviteToSpectatorSlot, RakString subject, RakString body)
    {
        if (roomsParticipant->GetRoom()==0)
            return REC_SEND_INVITE_UNKNOWN_ROOM_ID;

        return roomsParticipant->GetRoom()->SendInvite(roomsParticipant, inviteeId, inviteToSpectatorSlot, subject, body);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::AcceptInvite(RoomID roomId, Room **room, RoomsParticipant* roomsParticipant, RakString inviteSender)
    {
        *room = GetRoomByLobbyRoomID(roomId);
        if (*room==0)
            return REC_ACCEPT_INVITE_UNKNOWN_ROOM_ID;
        if (roomsParticipant->GetRoom())
            return REC_ACCEPT_INVITE_CURRENTLY_IN_A_ROOM;
        if (roomsParticipant->GetInQuickJoin())
            return REC_ACCEPT_INVITE_CURRENTLY_IN_QUICK_JOIN;
        return (*room)->AcceptInvite(roomsParticipant, inviteSender);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::StartSpectating(RoomsParticipant* roomsParticipant)
    {
        if (roomsParticipant->GetRoom()==0)
            return REC_START_SPECTATING_UNKNOWN_ROOM_ID;

        return roomsParticipant->GetRoom()->StartSpectating(roomsParticipant);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::StopSpectating(RoomsParticipant* roomsParticipant)
    {
        if (roomsParticipant->GetRoom()==0)
            return REC_STOP_SPECTATING_UNKNOWN_ROOM_ID;

        return roomsParticipant->GetRoom()->StopSpectating(roomsParticipant);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::GrantModerator(RoomsParticipant* roomsParticipant, RoomsParticipant *newModerator, DataStructures::List<InvitedUser> &clearedInvites)
    {
        if (roomsParticipant->GetRoom()==0)
            return REC_GRANT_MODERATOR_UNKNOWN_ROOM_ID;

        return roomsParticipant->GetRoom()->GrantModerator(roomsParticipant, newModerator, clearedInvites);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::ChangeSlotCounts(RoomsParticipant* roomsParticipant, Slots slots)
    {
        if (roomsParticipant->GetRoom()==0)
            return REC_CHANGE_SLOT_COUNTS_UNKNOWN_ROOM_ID;

        return roomsParticipant->GetRoom()->ChangeSlotCounts(roomsParticipant, slots);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::SetCustomRoomProperties(RoomsParticipant* roomsParticipant, DataStructures::Table *table)
    {
        if (roomsParticipant->GetRoom()==0)
            return REC_SET_CUSTOM_ROOM_PROPERTIES_UNKNOWN_ROOM_ID;

        RoomsErrorCode roomsErrorCode = roomsParticipant->GetRoom()->SetCustomRoomProperties(roomsParticipant, table);
        if (roomsErrorCode!=REC_SUCCESS)
            return roomsErrorCode;

        NCount perGamesRoomContainersIndex;
        for (perGamesRoomContainersIndex=0; perGamesRoomContainersIndex < perGamesRoomsContainers.Size(); perGamesRoomContainersIndex++)
        {
            if (perGamesRoomsContainers[perGamesRoomContainersIndex]->GetRoomByLobbyRoomID(roomsParticipant->GetRoom()->GetID()))
                break;
        }
        N_assert(perGamesRoomContainersIndex != perGamesRoomsContainers.Size());

        PerGameRoomsContainer *perGameRoomsContainer = perGamesRoomsContainers[perGamesRoomContainersIndex];
        NCount newTableIndex, oldTableIndex;
        DataStructures::Table *oldTable = &perGameRoomsContainer->roomsTable;
        DataStructures::Table::Row *row;
        for (newTableIndex=0; newTableIndex < table->GetColumnCount(); newTableIndex++)
        {
            oldTableIndex = oldTable->ColumnIndex(table->ColumnName(newTableIndex));
            if (oldTableIndex==(NCount) -1)
            {
                if (oldTable->GetColumnCount() < (NCount) MAX_CUSTOM_QUERY_FIELDS)
                {
                    oldTable->AddColumn(table->ColumnName(newTableIndex), table->GetColumnType(newTableIndex));
                }
                else
                    continue;

            }
            oldTableIndex=oldTable->ColumnIndex(table->ColumnName(newTableIndex));
            row = roomsParticipant->GetRoom()->tableRow;
            *(row->cells[oldTableIndex])=*(table->GetRowByIndex(0,0)->cells[newTableIndex]);
        }
        return REC_SUCCESS;
    }
    //------------------------------------------------------------------------
    void AllGamesRoomsContainer::GetRoomProperties(RoomID roomId, Room **room, DataStructures::Table *table)
    {
        table->Clear();
        NCount perGamesRoomContainersIndex;
        for (perGamesRoomContainersIndex=0; perGamesRoomContainersIndex < perGamesRoomsContainers.Size(); perGamesRoomContainersIndex++)
        {
            *room=perGamesRoomsContainers[perGamesRoomContainersIndex]->GetRoomByLobbyRoomID(roomId);
            if (*room)
                break;
        }
        N_assert(perGamesRoomContainersIndex != perGamesRoomsContainers.Size());
        PerGameRoomsContainer *perGameRoomsContainer = perGamesRoomsContainers[perGamesRoomContainersIndex];
        DataStructures::Table *oldTable = &perGameRoomsContainer->roomsTable;
        NCount i;
        for (i=0; i < oldTable->GetColumnCount(); i++)
            table->AddColumn(oldTable->ColumnName(i), oldTable->GetColumnType(i));
        table->AddRow(roomId, oldTable->GetRowByID(roomId)->cells, true);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::ChangeRoomName(RoomsParticipant* roomsParticipant, RakString newRoomName, ProfanityFilter *profanityFilter)
    {
        if (roomsParticipant->GetRoom()==0)
            return REC_CHANGE_ROOM_NAME_UNKNOWN_ROOM_ID;

        NCount i;
        DataStructures::List<RakString> roomNames;
        for (i=0; i < perGamesRoomsContainers.Size(); i++)
        {
            perGamesRoomsContainers[i]->GetRoomNames(roomNames);
            if (roomNames.GetIndexOf(newRoomName)!=(NCount) -1)
                return REC_CHANGE_ROOM_NAME_NAME_ALREADY_IN_USE;
        }

        return roomsParticipant->GetRoom()->ChangeRoomName(roomsParticipant, newRoomName, profanityFilter);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::SetHiddenFromSearches(RoomsParticipant* roomsParticipant, bool _hiddenFromSearches)
    {
        if (roomsParticipant->GetRoom()==0)
            return REC_SET_HIDDEN_FROM_SEARCHES_UNKNOWN_ROOM_ID;

        return roomsParticipant->GetRoom()->SetHiddenFromSearches(roomsParticipant, _hiddenFromSearches);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::SetDestroyOnModeratorLeave(RoomsParticipant* roomsParticipant, bool destroyOnModeratorLeave)
    {
        if (roomsParticipant->GetRoom()==0)
            return REC_SET_DESTROY_ON_MODERATOR_LEAVE_UNKNOWN_ROOM_ID;

        return roomsParticipant->GetRoom()->SetDestroyOnModeratorLeave(roomsParticipant, destroyOnModeratorLeave);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::SetReadyStatus(RoomsParticipant* roomsParticipant, bool isReady)
    {
        if (roomsParticipant->GetRoom()==0)
            return REC_SET_READY_STATUS_UNKNOWN_ROOM_ID;

        return roomsParticipant->GetRoom()->SetReadyStatus(roomsParticipant, isReady);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::GetReadyStatus( RoomID roomId, Room **room, DataStructures::List<RoomsParticipant*> &readyUsers, DataStructures::List<RoomsParticipant*> &unreadyUsers)
    {
        *room = GetRoomByLobbyRoomID(roomId);

        if (room==0)
            return REC_GET_READY_STATUS_UNKNOWN_ROOM_ID;

        return (*room)->GetReadyStatus(readyUsers, unreadyUsers);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::SetRoomLockState(RoomsParticipant* roomsParticipant, RoomLockState _roomLockState)
    {
        if (roomsParticipant->GetRoom()==0)
            return REC_SET_ROOM_LOCK_STATE_UNKNOWN_ROOM_ID;

        return roomsParticipant->GetRoom()->SetRoomLockState(roomsParticipant, _roomLockState);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::GetRoomLockState(RoomID roomId, Room **room, RoomLockState *roomLockState)
    {
        *room = GetRoomByLobbyRoomID(roomId);

        if (*room==0)
            return REC_GET_ROOM_LOCK_STATE_UNKNOWN_ROOM_ID;

        return (*room)->GetRoomLockState( roomLockState);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::AreAllMembersReady(RoomID roomId, Room **room, bool *allReady)
    {
        *room = GetRoomByLobbyRoomID(roomId);

        if (*room==0)
            return REC_ARE_ALL_MEMBERS_READY_UNKNOWN_ROOM_ID;

        return (RoomsErrorCode) (*room)->AreAllMembersReady((NCount) -1, allReady);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::KickMember(RoomsParticipant* roomsParticipant, RoomsParticipant *kickedParticipant, RakString reason)
    {
        if (roomsParticipant->GetRoom()==0)
            return REC_KICK_MEMBER_UNKNOWN_ROOM_ID;

        return roomsParticipant->GetRoom()->KickMember(roomsParticipant, kickedParticipant, reason);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::UnbanMember(RoomsParticipant* roomsParticipant, RakString name)
    {
        if (roomsParticipant->GetRoom()==0)
            return REC_UNBAN_MEMBER_UNKNOWN_ROOM_ID;

        return roomsParticipant->GetRoom()->UnbanMember(roomsParticipant, name);
    }
    //------------------------------------------------------------------------
    RoomsErrorCode AllGamesRoomsContainer::GetBanReason( RoomID lobbyRoomId, Room **room, RakString name, RakString *reason)
    {
        *room = GetRoomByLobbyRoomID(lobbyRoomId);
        if (*room==0)
            return REC_GET_BAN_REASON_UNKNOWN_ROOM_ID;
        return (*room)->GetBanReason(name, reason);
    }
    //------------------------------------------------------------------------
    Room * AllGamesRoomsContainer::GetRoomByLobbyRoomID(RoomID lobbyRoomID)
    {
        NCount i;
        Room *room;
        for (i=0; i < perGamesRoomsContainers.Size(); i++)
        {
            room = perGamesRoomsContainers[i]->GetRoomByLobbyRoomID(lobbyRoomID);
            if (room)
                return room;
        }
        return 0;
    }
    Room * AllGamesRoomsContainer::GetRoomByName(RakString roomName)
    {
        NCount i;
        Room *room;
        for (i=0; i < perGamesRoomsContainers.Size(); i++)
        {
            room = perGamesRoomsContainers[i]->GetRoomByName(roomName);
            if (room)
                return room;
        }
        return 0;
    }
    RoomsErrorCode AllGamesRoomsContainer::ProcessQuickJoins(
        DataStructures::List<QuickJoinUser *> &timeoutExpired,
            DataStructures::List<JoinedRoomResult> &joinedRoomMembers,
                DataStructures::List<QuickJoinUser *> &dereferencedPointers,
                    TimeDurMS elapsedTime)
    {
        NCount numRoomsCreated;
        NCount i;
        dereferencedPointers.Clear(false);
        joinedRoomMembers.Clear(false);
        for (i = 0; i < perGamesRoomsContainers.Size(); ++i)
        {
            numRoomsCreated=perGamesRoomsContainers[i]->ProcessQuickJoins(timeoutExpired, joinedRoomMembers, dereferencedPointers, elapsedTime, nextRoomId);
            nextRoomId += numRoomsCreated;
        }
        NCount j;
        for (j=0; j < joinedRoomMembers.Size(); j++)
            joinedRoomMembers[j].agrc=this;
        return REC_SUCCESS;
    }
    RoomsErrorCode AllGamesRoomsContainer::AddTitle(GameIdentifier gameIdentifier)
    {
        if (perGamesRoomsContainers.Has(gameIdentifier)==true)
            return REC_ADD_TITLE_ALREADY_IN_USE;
        perGamesRoomsContainers.SetNew(gameIdentifier, new PerGameRoomsContainer);
        return REC_SUCCESS;
    }

    // ----------------------------  PerGameRoomsContainer  ----------------------------

    PerGameRoomsContainer::PerGameRoomsContainer()
    {
        DefaultRoomColumns::AddDefaultColumnsToTable(&roomsTable);
        nextQuickJoinProcess.SetPeriod(PROCESS_QUICK_JOINS_INTERVAL);
    }
    PerGameRoomsContainer::~PerGameRoomsContainer()
    {
    }
    RoomsErrorCode PerGameRoomsContainer::CreateRoom(RoomCreationParameters *roomCreationParameters,
                                                     ProfanityFilter *profanityFilter,
                                                     RoomID lobbyRoomId,
                                                     bool validate)
    {
        if (validate)
        {
            DataStructures::List<RakString> otherRoomNames;
            GetRoomNames(otherRoomNames);
            RoomsErrorCode roomsErrorCode = roomCreationParameters->Validate(otherRoomNames, profanityFilter);
            if (roomsErrorCode!=REC_SUCCESS)
                return roomsErrorCode;
            roomCreationParameters->createdRoom=true;
        }
        DataStructures::List<DataStructures::Table::Cell> initialCellValues;
        DataStructures::Table::Row *row = roomsTable.AddRow(lobbyRoomId,initialCellValues);
        roomCreationParameters->roomOutput = new Room(lobbyRoomId, roomCreationParameters, row, roomCreationParameters->firstUser);
        roomCreationParameters->firstUser->SetPerGameRoomsContainer(this);
        N_assert(roomCreationParameters->firstUser->GetRoom()==roomCreationParameters->roomOutput);
        return REC_SUCCESS;
    }
    RoomsErrorCode PerGameRoomsContainer::JoinByFilter(RoomMemberMode roomMemberMode, RoomsParticipant* roomsParticipant, RoomID lastRoomJoined, RoomQuery *query, JoinedRoomResult *joinRoomResult)
    {
        joinRoomResult->roomOutput=0;
        if (roomMemberMode==RMM_MODERATOR)
            return REC_JOIN_BY_FILTER_CANNOT_JOIN_AS_MODERATOR;
        RoomsErrorCode roomsErrorCode;
        if (query!=0)
        {
            roomsErrorCode = query->Validate();
            if (roomsErrorCode!=REC_SUCCESS)
                return roomsErrorCode;
        }

        DataStructures::OrderedList<Room*, Room*, PerGameRoomsContainer::RoomsSortByTimeThenTotalSlots> roomsOutput;
        RoomPrioritySort( roomsParticipant, query, roomsOutput );
        if (roomsOutput.Size()==0)
            return REC_JOIN_BY_FILTER_NO_ROOMS;
        if (roomsOutput[0]->GetID()==lastRoomJoined && roomsOutput.Size()>1)
            joinRoomResult->roomOutput = roomsOutput[1];
        else
            joinRoomResult->roomOutput = roomsOutput[0];
        roomsParticipant->SetPerGameRoomsContainer(this);
        return (joinRoomResult->roomOutput)->JoinByFilter(roomsParticipant, roomMemberMode, joinRoomResult);
    }
    RoomsErrorCode PerGameRoomsContainer::AddUserToQuickJoin(QuickJoinUser *quickJoinMember)
    {
        if (GetQuickJoinIndex(quickJoinMember->roomsParticipant)!=(NCount) -1)
            return REC_ADD_TO_QUICK_JOIN_ALREADY_THERE;
        quickJoinMember->roomsParticipant->SetPerGameRoomsContainer(this);
        quickJoinMember->roomsParticipant->SetInQuickJoin(true);
        quickJoinList.Insert(quickJoinMember);
        return REC_SUCCESS;
    }
    RoomsErrorCode PerGameRoomsContainer::RemoveUserFromQuickJoin(RoomsParticipant* roomsParticipant, QuickJoinUser **qju)
    {
        NCount quickJoinIndex = GetQuickJoinIndex(roomsParticipant);
        *qju=0;
        if (quickJoinIndex==(NCount) -1)
            return REC_REMOVE_FROM_QUICK_JOIN_NOT_THERE;
        quickJoinList[quickJoinIndex]->roomsParticipant->SetInQuickJoin(false);
        *qju=quickJoinList[quickJoinIndex];
        quickJoinList.RemoveAtIndex(quickJoinIndex);
        roomsParticipant->SetPerGameRoomsContainer(0);
        return REC_SUCCESS;
    }
    bool PerGameRoomsContainer::IsInQuickJoin(RoomsParticipant* roomsParticipant)
    {
        return roomsParticipant->GetInQuickJoin()!=0;
    }

    int PerGameRoomsContainer::RoomsSortByTimeThenTotalSlots( Room* const &key, Room* const &data )
    {
        NIId keyCreationTime = key->GetNumericProperty(DefaultRoomColumns::TC_CREATION_TIME);
        NIId dataCreationTime = data->GetNumericProperty(DefaultRoomColumns::TC_CREATION_TIME);
        int diff = (int) abs(keyCreationTime-dataCreationTime);
        if (diff < 30 * 1000)
        {
            NIId keyTotalSlots = key->GetNumericProperty(DefaultRoomColumns::TC_USED_PUBLIC_PLUS_RESERVED_SLOTS);
            NIId dataTotalSlots = data->GetNumericProperty(DefaultRoomColumns::TC_USED_PUBLIC_PLUS_RESERVED_SLOTS);
            if (keyTotalSlots > dataTotalSlots)
                return -1;
            if (keyTotalSlots < dataTotalSlots)
                return 1;
            if (key < data)
                return -1;
            return 1;
        }
        if (keyCreationTime < dataCreationTime)
            return -1;
        if (keyCreationTime > dataCreationTime)
            return 1;
        return strcmp(key->GetStringProperty(DefaultRoomColumns::TC_ROOM_NAME),data->GetStringProperty(DefaultRoomColumns::TC_ROOM_NAME));
    }
    unsigned PerGameRoomsContainer::ProcessQuickJoins( DataStructures::List<QuickJoinUser*> &timeoutExpired,
        DataStructures::List<JoinedRoomResult> &joinedRoomMembers,
            DataStructures::List<QuickJoinUser*> &dereferencedPointers,
                TimeDurMS elapsedTime, RoomID startingRoomId)
    {
        unsigned roomIndex, quickJoinIndex;
        for (quickJoinIndex=0; quickJoinIndex < quickJoinList.Size(); quickJoinIndex++)
            quickJoinList[quickJoinIndex]->totalTimeWaiting+=elapsedTime;

        // This is slow, so don't do it very often
        if (nextQuickJoinProcess.UpdateInterval(elapsedTime)==false)
            return 0;

        unsigned numRoomsCreated=0;
        RoomsErrorCode roomsErrorCode;
        Room *room;
        NIId totalRoomSlots, remainingRoomSlots;
        DataStructures::OrderedList<Room*, Room*, RoomsSortByTimeThenTotalSlots> potentialRooms;
        DataStructures::OrderedList<QuickJoinUser *, QuickJoinUser *, QuickJoinUser::SortByTotalTimeWaiting> quickJoinMemberTimeSort;
        DataStructures::List<Room*> allRooms;
        GetAllRooms(allRooms);

        while (1)
        {
            // 1. Clear all quickJoinWorkingList from all rooms
            for (roomIndex=0; roomIndex < allRooms.Size(); roomIndex++)
                allRooms[roomIndex]->quickJoinWorkingList.Clear(true);

            // 2. Use RoomPrioritySort to get all rooms they can potentially join
            // 3. For each of these rooms, record that this member can potentially join by storing a copy of the pointer into quickJoinWorkingList, if minimumPlayers => total room slots
            for (quickJoinIndex=0; quickJoinIndex < quickJoinList.Size(); quickJoinIndex++)
            {
                RoomPrioritySort( quickJoinList[quickJoinIndex]->roomsParticipant, &quickJoinList[quickJoinIndex]->networkedQuickJoinUser.query, potentialRooms );

                for (roomIndex=0; roomIndex < potentialRooms.Size(); roomIndex++)
                {
                    totalRoomSlots = potentialRooms[roomIndex]->GetNumericProperty(DefaultRoomColumns::TC_TOTAL_PUBLIC_PLUS_RESERVED_SLOTS);
                    if (totalRoomSlots >= quickJoinList[quickJoinIndex]->networkedQuickJoinUser.minimumPlayers-1 &&
                        potentialRooms[roomIndex]->ParticipantCanJoinRoom(quickJoinList[quickJoinIndex]->roomsParticipant, false, true)==PCJRR_SUCCESS )
                        potentialRooms[roomIndex]->quickJoinWorkingList.Insert( quickJoinList[quickJoinIndex]);
                }
            }


            // For all rooms:
            // 4. For each room where there are enough potential quick join members to fill the room, join all those members at once. Remove these members from the quick join list. Go to 1.
            for (roomIndex=0; roomIndex < allRooms.Size(); roomIndex++)
            {
                room = allRooms[roomIndex];
                remainingRoomSlots = room->GetNumericProperty(DefaultRoomColumns::TC_REMAINING_PUBLIC_PLUS_RESERVED_SLOTS);
                if (remainingRoomSlots>0 && room->quickJoinWorkingList.Size() >= (NCount) remainingRoomSlots)
                {
                    quickJoinMemberTimeSort.Clear(false);

                    // Sort those waiting in quick join from longest waiting to least waiting. Those longest waiting are processed first
                    for (quickJoinIndex=0; quickJoinIndex < (int) room->quickJoinWorkingList.Size(); quickJoinIndex++)
                        quickJoinMemberTimeSort.Insert( room->quickJoinWorkingList[quickJoinIndex], room->quickJoinWorkingList[quickJoinIndex], true);

                    for (quickJoinIndex=0; quickJoinIndex < (unsigned) remainingRoomSlots; quickJoinIndex++)
                    {
                        JoinedRoomResult jrr;
                        jrr.roomOutput=room;
                        roomsErrorCode=room->JoinByQuickJoin(quickJoinMemberTimeSort[quickJoinIndex]->roomsParticipant, RMM_ANY_PLAYABLE, &jrr);
                        N_assert(roomsErrorCode==REC_SUCCESS);

                        dereferencedPointers.Insert(quickJoinMemberTimeSort[quickJoinIndex]);
                        joinedRoomMembers.Insert(jrr);

                        QuickJoinUser *qju;
                        roomsErrorCode=RemoveUserFromQuickJoin(quickJoinMemberTimeSort[quickJoinIndex]->roomsParticipant, &qju);
                        N_assert(roomsErrorCode==REC_SUCCESS);
                    }

                    continue;
                }
            }

            break;
        }

        DataStructures::OrderedList<QuickJoinUser *, QuickJoinUser *, QuickJoinUser::SortByMinimumSlots> quickJoinMemberSlotSort;
        // 5. Sort quick join members by minimumPlayers, excluding members where minimumPlayers > total number of quick join members
        for (quickJoinIndex=0; quickJoinIndex < quickJoinList.Size(); quickJoinIndex++)
        {
            if (quickJoinList[quickJoinIndex]->networkedQuickJoinUser.minimumPlayers <= (int) quickJoinList.Size())
                quickJoinMemberSlotSort.Insert(quickJoinList[quickJoinIndex],quickJoinList[quickJoinIndex],true);
        }

        DataStructures::Table potentialNewRoom;
        unsigned queryIndex, quickJoinIndex2;
        DataStructures::Table resultTable;
        QuickJoinUser * quickJoinMember;
        RoomCreationParameters roomCreationParameters;
        DataStructures::List<QuickJoinUser *> potentialNewRoommates;

        // 6. If the current member created a roomOutput, find out how many subsequent members would join based on the custom filter
        quickJoinIndex=0;
        while (quickJoinIndex < quickJoinList.Size())
        {
            quickJoinMember = quickJoinList[quickJoinIndex];
            potentialNewRoom.Clear();
            DefaultRoomColumns::AddDefaultColumnsToTable(&potentialNewRoom);
            DataStructures::Table::Row *row = potentialNewRoom.AddRow(0);
            roomCreationParameters.networkedRoomCreationParameters.slots.publicSlots=quickJoinMember->networkedQuickJoinUser.minimumPlayers-1;
            Room::UpdateRowSlots( row, &roomCreationParameters.networkedRoomCreationParameters.slots, &roomCreationParameters.networkedRoomCreationParameters.slots);

            for (queryIndex=0; queryIndex < quickJoinMember->networkedQuickJoinUser.query.numQueries; queryIndex++)
            {
                // For all filters that are equal and custom, create a table with rows with these values
                if ( ( quickJoinMember->networkedQuickJoinUser.query.queries[queryIndex].operation==DataStructures::Table::QF_EQUAL ) &&
                    DefaultRoomColumns::HasColumnName(quickJoinMember->networkedQuickJoinUser.query.queries[queryIndex].columnName)==false &&
                    potentialNewRoom.ColumnIndex(quickJoinMember->networkedQuickJoinUser.query.queries[queryIndex].columnName)==-1 &&
                    quickJoinMember->networkedQuickJoinUser.query.queries[queryIndex].cellValue->isEmpty==false
                    )
                {
                    potentialNewRoom.AddColumn(quickJoinMember->networkedQuickJoinUser.query.queries[queryIndex].columnName, quickJoinMember->networkedQuickJoinUser.query.queries[queryIndex].cellValue->EstimateColumnType());
                    *(row->cells[potentialNewRoom.GetColumnCount()-1]) = *(quickJoinMember->networkedQuickJoinUser.query.queries[queryIndex].cellValue);
                }
            }

            potentialNewRoommates.Clear(true);
            for (quickJoinIndex2 = quickJoinIndex+1; quickJoinIndex2 < quickJoinList.Size(); quickJoinIndex2++)
            {
                resultTable.Clear();
                unsigned columnIndices[1];
                columnIndices[0]=DefaultRoomColumns::TC_LOBBY_ROOM_PTR;
                JoinedRoomResult joinedRoomResult;

                DataStructures::Table::FilterQuery subQueries[MAX_CUSTOM_QUERY_FIELDS];
                NCount subQueryCount;
                NCount subQueryIndex;
                for (subQueryIndex=0, subQueryCount=0; subQueryIndex < quickJoinList[quickJoinIndex2]->networkedQuickJoinUser.query.numQueries; subQueryIndex++)
                {
                    if (potentialNewRoom.ColumnIndex(quickJoinList[quickJoinIndex2]->networkedQuickJoinUser.query.queries[subQueryIndex].columnName)!=-1)
                    {
                        subQueries[subQueryCount++]=quickJoinList[quickJoinIndex2]->networkedQuickJoinUser.query.queries[subQueryIndex];
                    }
                }

                potentialNewRoom.QueryTable(columnIndices,1,subQueries,subQueryCount,0,0,&resultTable);
                if (resultTable.GetRowCount()>0)
                {
                    potentialNewRoommates.Insert(quickJoinList[quickJoinIndex2]);
                    if (potentialNewRoommates.Size()>=(NCount) quickJoinMember->networkedQuickJoinUser.minimumPlayers-1)
                    {
                        // 7. If this satisfies minimumPlayers, have that user create a roomOutput and those subsequent members join.
                        roomCreationParameters.networkedRoomCreationParameters.hiddenFromSearches=false;
                        roomCreationParameters.networkedRoomCreationParameters.destroyOnModeratorLeave=false;
                        roomCreationParameters.networkedRoomCreationParameters.roomName.Set(QUICK_JOIN_ROOM_NAME "%i", startingRoomId+numRoomsCreated);
                        roomCreationParameters.firstUser=quickJoinMember->roomsParticipant;

                        roomsErrorCode = CreateRoom(&roomCreationParameters, 0,startingRoomId+numRoomsCreated, false);
                        joinedRoomResult.roomOutput=roomCreationParameters.roomOutput;
                        numRoomsCreated++;
                        N_assert(roomsErrorCode==REC_SUCCESS);

                        QuickJoinUser *qju;
                        for (quickJoinIndex2=0; quickJoinIndex2 < potentialNewRoommates.Size(); quickJoinIndex2++)
                        {
                            roomsErrorCode = roomCreationParameters.roomOutput->JoinByQuickJoin(potentialNewRoommates[quickJoinIndex2]->roomsParticipant, RMM_PUBLIC, &joinedRoomResult);
                            N_assert(roomsErrorCode==REC_SUCCESS);
                            RemoveUserFromQuickJoin(potentialNewRoommates[quickJoinIndex2]->roomsParticipant, &qju);
                            dereferencedPointers.Insert(qju);
                            joinedRoomResult.joiningMember=potentialNewRoommates[quickJoinIndex2]->roomsParticipant;
                            joinedRoomMembers.Insert(joinedRoomResult);
                        }

                        joinedRoomResult.joiningMember=quickJoinMember->roomsParticipant;
                        joinedRoomMembers.Insert(joinedRoomResult);
                        RemoveUserFromQuickJoin(quickJoinMember->roomsParticipant, &qju);
                        dereferencedPointers.Insert(qju);
                        continue;
                    }
                }
            }
            ++quickJoinIndex;
        }

        // 5. Remove from list if timeout has expired.
        quickJoinIndex = 0;
        while (quickJoinIndex < quickJoinList.Size())
        {
            if (quickJoinList[quickJoinIndex]->totalTimeWaiting >= quickJoinList[quickJoinIndex]->networkedQuickJoinUser.timeout)
            {
                quickJoinList[quickJoinIndex]->roomsParticipant->SetInQuickJoin(false);
                timeoutExpired.Insert(quickJoinList[quickJoinIndex]);
                dereferencedPointers.Insert(quickJoinList[quickJoinIndex]);
                quickJoinList.RemoveAtIndexFast(quickJoinIndex);
            }
            else
                quickJoinIndex++;
        }

        return numRoomsCreated;
    }
    RoomsErrorCode PerGameRoomsContainer::GetInvitesToParticipant(RoomsParticipant* roomsParticipant, DataStructures::List<InvitedUser*> &invites)
    {
        DataStructures::List<Room*> rooms;
        GetAllRooms(rooms);
        unsigned i;
        for (i=0; i < rooms.Size(); i++)
            rooms[i]->GetInvitesToParticipant(roomsParticipant, invites);
        return REC_SUCCESS;
    }
    bool PerGameRoomsContainer::DestroyRoomIfDead(Room *room)
    {
        if (roomsTable.GetRowByID(room->GetID())==room->tableRow)
        {
            roomsTable.RemoveRow(room->GetID());
            delete room;
            return true;
        }
        return false;
    }
    void PerGameRoomsContainer::ChangeHandle(RakString oldHandle, RakString newHandle)
    {
        DataStructures::List<Room*> rooms;
        GetAllRooms(rooms);
        unsigned i;
        for (i=0; i < rooms.Size(); i++)
            rooms[i]->ChangeHandle(oldHandle, newHandle);
    }

    RoomsErrorCode PerGameRoomsContainer::SearchByFilter( RoomsParticipant* roomsParticipant, RoomQuery *roomQuery, DataStructures::OrderedList<Room*, Room*, AllGamesRoomsContainer::RoomsSortByName> &roomsOutput, bool onlyJoinable )
    {
        DataStructures::Table resultTable;
        unsigned columnIndices[1];
        columnIndices[0]=DefaultRoomColumns::TC_LOBBY_ROOM_PTR;

        // Process user queries
        roomsTable.QueryTable(columnIndices,1,roomQuery->queries,roomQuery->numQueries,0,0,&resultTable);

        roomsOutput.Clear(false);
        DataStructures::Page<unsigned, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER> *cur = resultTable.GetRows().GetListHead();
        int i;
        Room *room;
        while (cur)
        {
            for (i=0; i < cur->size; i++)
            {
                // Put all the pointers in the roomSort list, filtering out those you cannot join (full, or no public and you are not invited)
                room = (Room*) cur->data[i]->cells[0]->ptr;
                if ( (onlyJoinable==false || room->ParticipantCanJoinRoom(roomsParticipant, false, true)==PCJRR_SUCCESS) &&
                    room->IsHiddenToParticipant(roomsParticipant)==false)
                    roomsOutput.Insert(room, room, true);
            }
            cur=cur->next;
        }
        return REC_SUCCESS;
    }
    void PerGameRoomsContainer::RoomPrioritySort( RoomsParticipant* roomsParticipant, RoomQuery *roomQuery, DataStructures::OrderedList<Room*, Room*, RoomsSortByTimeThenTotalSlots> &roomsOutput )
    {
        int i;
        // If you don't care about room filters, just join any room
        if (roomQuery==0 || roomQuery->numQueries==0 || roomQuery->queries==0)
        {
            DataStructures::List<Room*> rooms;
            GetAllRooms(rooms);
            for(i = 0; (unsigned) i < rooms.Size(); ++i)
            {
                Room * room = rooms[i];
                if (room->ParticipantCanJoinRoom(roomsParticipant, false, true) == PCJRR_SUCCESS &&
                    room->IsHiddenToParticipant(roomsParticipant) == false)
                        roomsOutput.Insert(rooms[i], rooms[i], true);
            }
            return;
        }

        // Must pass room query
        // Of those that pass room query, sort by time (lower is first). If within one minute of each other, sort by number of users in playable slots (higher if first)
        // Be sure to return columns TC_LOBBY_ROOM_PTR, TC_USED_PUBLIC_PLUS_RESERVED_SLOTS, TC_LOBBY_ROOM_PTR

        DataStructures::Table resultTable;
        unsigned columnIndices[1];
        columnIndices[0]=DefaultRoomColumns::TC_LOBBY_ROOM_PTR;

        // Process user queries
        roomsTable.QueryTable(columnIndices,1,roomQuery->queries,roomQuery->numQueries,0,0,&resultTable);

        roomsOutput.Clear(false);
        DataStructures::Page<unsigned, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER> *cur = resultTable.GetRows().GetListHead();
        Room *room;
        while (cur)
        {
            for (i=0; i < cur->size; i++)
            {
                // Put all the pointers in the roomSort list, filtering out those you cannot join (full, or no public and you are not invited)
                room = (Room*) cur->data[i]->cells[0]->ptr;
                if (room->ParticipantCanJoinRoom(roomsParticipant, false, true)==PCJRR_SUCCESS &&
                    room->IsHiddenToParticipant(roomsParticipant)==false)
                    roomsOutput.Insert(room, room, true);
            }
            cur=cur->next;
        }
    }
    Room* PerGameRoomsContainer::GetRoomByLobbyRoomID(RoomID lobbyRoomID)
    {
        DataStructures::Table::Row *row = roomsTable.GetRowByID(lobbyRoomID);
        if (row==0)
            return 0;

        return (Room*) roomsTable.GetRowByID(lobbyRoomID)->cells[DefaultRoomColumns::TC_LOBBY_ROOM_PTR]->ptr;
    }
    Room * PerGameRoomsContainer::GetRoomByName(RakString roomName)
    {
        DataStructures::List<Room*> rooms;
        GetAllRooms(rooms);
        NCount i;
        for (i=0; i < rooms.Size(); i++)
        {
            if (strcmp(rooms[i]->GetStringProperty(DefaultRoomColumns::TC_ROOM_NAME), roomName.C_String())==0)
            {
                return rooms[i];
            }
        }
        return 0;
    }
    void PerGameRoomsContainer::GetAllRooms(DataStructures::List<Room*> &rooms)
    {
        DataStructures::Page<unsigned, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER> *cur = roomsTable.GetRows().GetListHead();
        int i;
        rooms.Clear(false);
        while (cur)
        {
            for (i=0; i < cur->size; i++)
                rooms.Insert((Room*)cur->data[i]->cells[DefaultRoomColumns::TC_LOBBY_ROOM_PTR]->ptr);
            cur=cur->next;
        }
    }
    void PerGameRoomsContainer::GetRoomNames(DataStructures::List<RakString> &roomNames)
    {
        DataStructures::Page<unsigned, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER> *cur = roomsTable.GetRows().GetListHead();
        int i;
        roomNames.Clear(false);
        while (cur)
        {
            for (i=0; i < cur->size; i++)
                roomNames.Insert(RakString(cur->data[i]->cells[DefaultRoomColumns::TC_ROOM_NAME]->c));
            cur=cur->next;
        }
    }

    NCount PerGameRoomsContainer::GetQuickJoinIndex(RoomsParticipant* roomsParticipant)
    {
        NCount i;
        for (i=0; i < quickJoinList.Size(); i++)
            if (quickJoinList[i]->roomsParticipant==roomsParticipant)
                return i;
        return (NCount) -1;
    }

    // ----------------------------  ROOMS  ----------------------------

    Room::Room( RoomID _roomId, RoomCreationParameters *roomCreationParameters, DataStructures::Table::Row *_row, RoomsParticipant* roomsParticipant )
    {
        roomDestroyed=false;

        lobbyRoomId=_roomId;
        tableRow=_row;

        autoLockReadyStatus=roomCreationParameters->networkedRoomCreationParameters.autoLockReadyStatus;
        hiddenFromSearches=roomCreationParameters->networkedRoomCreationParameters.hiddenFromSearches;
    //	destroyOnModeratorLeave=roomCreationParameters->networkedRoomCreationParameters.destroyOnModeratorLeave;
        clearInvitesOnNewModerator=roomCreationParameters->networkedRoomCreationParameters.clearInvitesOnNewModerator;
        inviteToRoomPermission=roomCreationParameters->networkedRoomCreationParameters.inviteToRoomPermission;
        inviteToSpectatorSlotPermission=roomCreationParameters->networkedRoomCreationParameters.inviteToSpectatorSlotPermission;
        roomLockState=RLS_NOT_LOCKED;

        Slots usedSlots;
        UpdateRowSlots( tableRow, &roomCreationParameters->networkedRoomCreationParameters.slots, &usedSlots);

        RoomMember *roomMember = new RoomMember;
        roomMember->roomMemberMode=RMM_MODERATOR;
        roomMember->roomsParticipant=roomsParticipant;
        roomsParticipant->SetRoom(this);
        roomMemberList.Insert(roomMember);
        tableRow->cells[DefaultRoomColumns::TC_ROOM_NAME]->Set(roomCreationParameters->networkedRoomCreationParameters.roomName.C_String());
        tableRow->cells[DefaultRoomColumns::TC_ROOM_ID]->Set(lobbyRoomId);
        tableRow->cells[DefaultRoomColumns::TC_CREATION_TIME]->Set((int) N_Engine().getTimer().getMS());
        tableRow->cells[DefaultRoomColumns::TC_DESTROY_ON_MODERATOR_LEAVE]->Set((int) roomCreationParameters->networkedRoomCreationParameters.destroyOnModeratorLeave);
        tableRow->cells[DefaultRoomColumns::TC_LOBBY_ROOM_PTR]->SetPtr(this);
    }
    Room::~Room()
    {
    }
    void Room::UpdateRowSlots( DataStructures::Table::Row* row, Slots *totalSlots, Slots *usedSlots)
    {
        row->cells[DefaultRoomColumns::TC_TOTAL_SLOTS]->Set(totalSlots->publicSlots+totalSlots->reservedSlots+totalSlots->spectatorSlots);
        row->cells[DefaultRoomColumns::TC_TOTAL_PUBLIC_PLUS_RESERVED_SLOTS]->Set(totalSlots->publicSlots+totalSlots->reservedSlots);
        row->cells[DefaultRoomColumns::TC_TOTAL_PUBLIC_SLOTS]->Set(totalSlots->publicSlots);
        row->cells[DefaultRoomColumns::TC_TOTAL_RESERVED_SLOTS]->Set(totalSlots->reservedSlots);
        row->cells[DefaultRoomColumns::TC_TOTAL_SPECTATOR_SLOTS]->Set(totalSlots->spectatorSlots);

        UpdateUsedSlots(row, totalSlots, usedSlots);
    }
    void Room::UpdateUsedSlots( void )
    {
        Slots totalSlots = GetTotalSlots();
        Slots usedSlots = GetUsedSlots();
        UpdateUsedSlots(&totalSlots, &usedSlots);
    }
    void Room::UpdateUsedSlots( DataStructures::Table::Row* tableRow, Slots *totalSlots, Slots *usedSlots )
    {
        tableRow->cells[DefaultRoomColumns::TC_USED_SLOTS]->Set(usedSlots->publicSlots+usedSlots->reservedSlots+usedSlots->spectatorSlots);
        tableRow->cells[DefaultRoomColumns::TC_USED_PUBLIC_PLUS_RESERVED_SLOTS]->Set(usedSlots->publicSlots+usedSlots->reservedSlots);
        tableRow->cells[DefaultRoomColumns::TC_REMAINING_SLOTS]->Set(totalSlots->publicSlots+totalSlots->reservedSlots+totalSlots->spectatorSlots-usedSlots->publicSlots-usedSlots->reservedSlots-usedSlots->spectatorSlots);
        tableRow->cells[DefaultRoomColumns::TC_REMAINING_PUBLIC_PLUS_RESERVED_SLOTS]->Set(totalSlots->publicSlots+totalSlots->reservedSlots-usedSlots->publicSlots-usedSlots->reservedSlots);
        tableRow->cells[DefaultRoomColumns::TC_USED_PUBLIC_SLOTS]->Set(usedSlots->publicSlots);
        tableRow->cells[DefaultRoomColumns::TC_USED_RESERVED_SLOTS]->Set(usedSlots->reservedSlots);
        tableRow->cells[DefaultRoomColumns::TC_USED_SPECTATOR_SLOTS]->Set(usedSlots->spectatorSlots);
        tableRow->cells[DefaultRoomColumns::TC_REMAINING_PUBLIC_SLOTS]->Set(totalSlots->publicSlots-usedSlots->publicSlots);
        tableRow->cells[DefaultRoomColumns::TC_REMAINING_RESERVED_SLOTS]->Set(totalSlots->reservedSlots-usedSlots->reservedSlots);
        tableRow->cells[DefaultRoomColumns::TC_REMAINING_SPECTATOR_SLOTS]->Set(totalSlots->spectatorSlots-usedSlots->spectatorSlots);
    }
    void Room::UpdateUsedSlots( Slots *totalSlots, Slots *usedSlots )
    {
        UpdateUsedSlots(tableRow, totalSlots, usedSlots);
    }
    Slots Room::GetTotalSlots(void) const
    {
        Slots totalSlots;
        totalSlots.publicSlots=(int)GetNumericProperty(DefaultRoomColumns::TC_TOTAL_PUBLIC_SLOTS);
        totalSlots.reservedSlots=(int)GetNumericProperty(DefaultRoomColumns::TC_TOTAL_RESERVED_SLOTS);
        totalSlots.spectatorSlots=(int)GetNumericProperty(DefaultRoomColumns::TC_TOTAL_SPECTATOR_SLOTS);
        return totalSlots;
    }
    void Room::SetTotalSlots(Slots *totalSlots)
    {
        SetNumericProperty(DefaultRoomColumns::TC_TOTAL_PUBLIC_SLOTS, totalSlots->publicSlots);
        SetNumericProperty(DefaultRoomColumns::TC_TOTAL_RESERVED_SLOTS, totalSlots->reservedSlots);
        SetNumericProperty(DefaultRoomColumns::TC_TOTAL_SPECTATOR_SLOTS, totalSlots->spectatorSlots);
    }
    Slots Room::GetUsedSlots(void) const
    {
        Slots usedSlots;
        unsigned i;
        for (i=0; i < roomMemberList.Size(); i++)
        {
            if (roomMemberList[i]->roomMemberMode==RMM_PUBLIC)
                usedSlots.publicSlots++;
            else if (roomMemberList[i]->roomMemberMode==RMM_RESERVED)
                usedSlots.reservedSlots++;
            else if (
                roomMemberList[i]->roomMemberMode==RMM_SPECTATOR_PUBLIC ||
                roomMemberList[i]->roomMemberMode==RMM_SPECTATOR_RESERVED)
                usedSlots.spectatorSlots++;
            // Moderator not counted
        }
        return usedSlots;
    }
    RoomsErrorCode Room::SendInvite(RoomsParticipant* roomsParticipant, RoomsParticipant* inviteeId, bool inviteToSpectatorSlot, RakString subject, RakString body)
    {
        N_assert(roomDestroyed==false);

        // Is roomsParticipant in the room?
        NCount roomsParticipantIndex = GetRoomIndex(roomsParticipant);
        if (roomsParticipantIndex==-1) return REC_SEND_INVITE_INVITOR_NOT_IN_ROOM;

        if (roomsParticipant==inviteeId)
            return REC_SEND_INVITE_CANNOT_PERFORM_ON_SELF;

        // Is inviteeId not in the room?
        if (IsInRoom(inviteeId)) return REC_SEND_INVITE_INVITEE_ALREADY_IN_THIS_ROOM;

        // Does inviteeId already have an invite from roomsParticipant (either slot type)?
        if (HasInvite(inviteeId->GetName())) return REC_SEND_INVITE_INVITEE_ALREADY_INVITED;

        // Can inviteeId join?
        ParticipantCanJoinRoomResult pcjrr = ParticipantCanJoinRoom(inviteeId, inviteToSpectatorSlot, false);
        if (pcjrr==PCJRR_BANNED) return REC_SEND_INVITE_INVITEE_BANNED;
        if (pcjrr==PCJRR_NO_PUBLIC_SLOTS && inviteToSpectatorSlot==false) return REC_SEND_INVITE_NO_SLOTS;
        if (pcjrr==PCJRR_NO_PUBLIC_OR_RESERVED_SLOTS) return REC_SEND_INVITE_NO_SLOTS;
        if (pcjrr==PCJRR_LOCKED) return REC_SEND_INVITE_ROOM_LOCKED;
        N_assert(pcjrr==PCJRR_SUCCESS);

        // Can roomsParticipant send an invite, given inviteToRoomPermission or inviteToSpectatorSlotPermission? (leader can always invite no matter what)
        RoomMemberMode memberMode = roomMemberList[roomsParticipantIndex]->roomMemberMode;
        N_assert(memberMode!=RMM_ANY_PLAYABLE);
        bool canInvite;
        if (memberMode==RMM_MODERATOR)
        {
            canInvite=true;
        }
        else
        {
            NetworkedRoomCreationParameters::SendInvitePermission relevantPermission;
            if (inviteToSpectatorSlotPermission)
                relevantPermission=inviteToSpectatorSlotPermission;
            else
                relevantPermission=inviteToRoomPermission;
            if (relevantPermission==NetworkedRoomCreationParameters::INVITE_MODE_MODERATOR_CAN_INVITE)
                return REC_SEND_INVITE_INVITOR_ONLY_MODERATOR_CAN_INVITE;
            if (relevantPermission==NetworkedRoomCreationParameters::INVITE_MODE_ANYONE_CAN_INVITE)
                canInvite=true;
            else if (
                (relevantPermission==NetworkedRoomCreationParameters::INVITE_MODE_PUBLIC_SLOTS_CAN_INVITE ||
                relevantPermission==NetworkedRoomCreationParameters::INVITE_MODE_MODERATOR_OR_PUBLIC_SLOTS_CAN_INVITE ||
                relevantPermission==NetworkedRoomCreationParameters::INVITE_MODE_MODERATOR_OR_PUBLIC_OR_RESERVED_SLOTS_CAN_INVITE)
                && memberMode==RMM_PUBLIC)
                canInvite=true;
            else if (
                (relevantPermission==NetworkedRoomCreationParameters::INVITE_MODE_RESERVED_SLOTS_CAN_INVITE ||
                relevantPermission==NetworkedRoomCreationParameters::INVITE_MODE_MODERATOR_OR_PUBLIC_OR_RESERVED_SLOTS_CAN_INVITE)
                && memberMode==RMM_RESERVED)
                canInvite=true;
            else if (relevantPermission==NetworkedRoomCreationParameters::INVITE_MODE_SPECTATOR_SLOTS_CAN_INVITE && (memberMode==RMM_SPECTATOR_PUBLIC || memberMode==RMM_SPECTATOR_RESERVED))
                canInvite=true;
            else if (relevantPermission==NetworkedRoomCreationParameters::INVITE_MODE_MODERATOR_OR_PUBLIC_SLOTS_CAN_INVITE && (memberMode==RMM_SPECTATOR_PUBLIC || memberMode==RMM_SPECTATOR_RESERVED))
                canInvite=true;
            else
                return REC_SEND_INVITE_INVITOR_LACK_INVITE_PERMISSIONS;
        }

        InvitedUser invitedUser;
        invitedUser.body=body;
        invitedUser.invitedAsSpectator=inviteToSpectatorSlot;
        invitedUser.target=inviteeId->GetName();
        invitedUser.invitorName=roomsParticipant->GetName();
        invitedUser.room=this;
        invitedUser.subject=subject;
        inviteList.Insert(invitedUser);
        return REC_SUCCESS;
    }
    RoomsErrorCode Room::AcceptInvite(RoomsParticipant* roomsParticipant, RakString inviteSender)
    {
        N_assert(roomDestroyed==false);

        // Is roomsParticipant not in the roomOutput?
        NCount roomsParticipantIndex = GetRoomIndex(roomsParticipant);
        if (roomsParticipantIndex!=-1) return REC_ACCEPT_INVITE_CURRENTLY_IN_A_ROOM;

        // Does roomsParticipant have an invite from inviteSender?
        NCount inviteIndex = GetInviteIndex(roomsParticipant->GetName(), inviteSender);
        if (inviteIndex==-1) return REC_ACCEPT_INVITE_NO_SUCH_INVITE;

        // Is the roomOutput locked for the target slot type (any slot and RLS_ALL_LOCKED, or player and RLS_PLAYERS_LOCKED)?
        ParticipantCanJoinRoomResult pcjrr = ParticipantCanJoinRoom(roomsParticipant, inviteList[inviteIndex].invitedAsSpectator, true);
        if (pcjrr==PCJRR_BANNED) return REC_ACCEPT_INVITE_BANNED;
        if (pcjrr==PCJRR_NO_PUBLIC_SLOTS || pcjrr==PCJRR_NO_PUBLIC_OR_RESERVED_SLOTS) return REC_ACCEPT_INVITE_NO_SLOTS;
        if (pcjrr==PCJRR_LOCKED) return REC_ACCEPT_INVITE_ROOM_LOCKED;
        N_assert(pcjrr==PCJRR_SUCCESS);

        // Remove the used invite
        inviteList.RemoveAtIndex(inviteIndex);

        // Add the new roomOutput member
        RoomMember* roomMember = new RoomMember;
        roomMember->isReady = false;
        roomMember->joinTime = N_Engine().getTimer().getMS();
        roomMember->roomMemberMode = RMM_RESERVED;
        roomMember->roomsParticipant = roomsParticipant;
        roomsParticipant->SetRoom(this);
        roomMemberList.Insert(roomMember);
        UpdateUsedSlots();
        return REC_SUCCESS;
    }
    RoomsErrorCode Room::StartSpectating(RoomsParticipant* roomsParticipant)
    {
        N_assert(roomDestroyed == false);

        // Is roomsParticipant in the roomOutput?
        NCount roomsParticipantIndex = GetRoomIndex(roomsParticipant);
        if (roomsParticipantIndex == -1) return REC_START_SPECTATING_NOT_IN_ROOM;

        // Is roomsParticipant the moderator? Moderator must grant moderator to another user first
        // Would this destroy the roomOutput (no players left)? (last player is always leader)
        if (roomMemberList[roomsParticipantIndex]->roomMemberMode == RMM_MODERATOR)
            return REC_START_SPECTATING_REASSIGN_MODERATOR_BEFORE_SPECTATE;

        // Already spectating?
        if (roomMemberList[roomsParticipantIndex]->roomMemberMode == RMM_SPECTATOR_PUBLIC ||
            roomMemberList[roomsParticipantIndex]->roomMemberMode == RMM_SPECTATOR_RESERVED)
            return REC_START_SPECTATING_ALREADY_SPECTATING;

        // Are there open spectator slots?
        if (GetNumericProperty(DefaultRoomColumns::TC_REMAINING_SPECTATOR_SLOTS)<=0.0)
            return REC_START_SPECTATING_NO_SPECTATOR_SLOTS_AVAILABLE;

        // Is the roomOutput locked to spectators (RLS_ALL_LOCKED)?
        if (roomLockState==RLS_ALL_LOCKED)
            return REC_START_SPECTATING_ROOM_LOCKED;

        if (roomMemberList[roomsParticipantIndex]->roomMemberMode == RMM_RESERVED ||
            roomMemberList[roomsParticipantIndex]->roomMemberMode == RMM_MODERATOR)
            roomMemberList[roomsParticipantIndex]->roomMemberMode = RMM_SPECTATOR_RESERVED;
        else
            roomMemberList[roomsParticipantIndex]->roomMemberMode = RMM_SPECTATOR_PUBLIC;

        UpdateUsedSlots();
        return REC_SUCCESS;
    }
    RoomsErrorCode Room::StopSpectating(RoomsParticipant* roomsParticipant)
    {
        N_assert(roomDestroyed==false);

        // Is roomsParticipant a spectator?
        NCount roomsParticipantIndex = GetRoomIndex(roomsParticipant);
        if (roomsParticipantIndex==-1) return REC_STOP_SPECTATING_NOT_IN_ROOM;
        if (roomMemberList[roomsParticipantIndex]->roomMemberMode!=RMM_SPECTATOR_PUBLIC &&
            roomMemberList[roomsParticipantIndex]->roomMemberMode!=RMM_SPECTATOR_RESERVED)
            return REC_STOP_SPECTATING_NOT_CURRENTLY_SPECTATING;

        // Is the roomOutput locked to players (RLS_ALL_LOCKED or RLS_PLAYERS_LOCKED)?
        ParticipantCanJoinRoomResult pcjrr = ParticipantCanJoinAsPlayer(roomsParticipant, false, true);
        N_assert(pcjrr!=PCJRR_BANNED);
        if (pcjrr==PCJRR_NO_PUBLIC_OR_RESERVED_SLOTS) return REC_STOP_SPECTATING_NO_SLOTS;
        if (pcjrr==PCJRR_LOCKED) return REC_STOP_SPECTATING_ROOM_LOCKED;
        N_assert(pcjrr==PCJRR_SUCCESS);

        if (roomMemberList[roomsParticipantIndex]->roomMemberMode==RMM_SPECTATOR_RESERVED && HasOpenReservedSlots())
            roomMemberList[roomsParticipantIndex]->roomMemberMode=RMM_RESERVED;
        else
            roomMemberList[roomsParticipantIndex]->roomMemberMode=RMM_PUBLIC;

        UpdateUsedSlots();
        return REC_SUCCESS;
    }
    RoomsErrorCode Room::GrantModerator(RoomsParticipant* roomsParticipant, RoomsParticipant *newModerator, DataStructures::List<InvitedUser> &clearedInvites)
    {
        N_assert(roomDestroyed==false);

        // Is roomsParticipant in the roomOutput?
        NCount roomsParticipantIndex = GetRoomIndex(roomsParticipant);
        if (roomsParticipantIndex==-1) return REC_GRANT_MODERATOR_NOT_IN_ROOM;

        // Is roomsParticipant the moderator?
        if (roomMemberList[roomsParticipantIndex]->roomMemberMode!=RMM_MODERATOR)
            return REC_GRANT_MODERATOR_MUST_BE_MODERATOR_TO_GRANT_MODERATOR;

        // Is newModerator in the roomOutput as a player?
        NCount newModeratorIndex = GetRoomIndex(newModerator);
        if (newModeratorIndex==-1) return REC_GRANT_MODERATOR_NEW_MODERATOR_NOT_IN_ROOM;

        if (roomMemberList[newModeratorIndex]->roomMemberMode==RMM_SPECTATOR_PUBLIC ||
            roomMemberList[newModeratorIndex]->roomMemberMode==RMM_SPECTATOR_RESERVED)
            return REC_GRANT_MODERATOR_NEW_MODERATOR_NOT_IN_PLAYABLE_SLOT;

        if (roomsParticipant==newModerator)
            return REC_GRANT_MODERATOR_CANNOT_PERFORM_ON_SELF;

        if (clearInvitesOnNewModerator)
        {
            // Clear invites
            clearedInvites = inviteList;
            inviteList.Clear(true);

            // Change RMM_SPECTATOR_RESERVED to RMM_SPECTATOR_PUBLIC
            NCount i;
            for (i = 0; i < roomMemberList.Size(); ++i)
            {
                if (roomMemberList[i]->roomMemberMode == RMM_SPECTATOR_RESERVED)
                    roomMemberList[i]->roomMemberMode = RMM_SPECTATOR_PUBLIC;
            }
        }

        roomMemberList[roomsParticipantIndex]->roomMemberMode=roomMemberList[newModeratorIndex]->roomMemberMode;
        roomMemberList[newModeratorIndex]->roomMemberMode=RMM_MODERATOR;

        UpdateUsedSlots();
        return REC_SUCCESS;
    }
    RoomsErrorCode Room::ChangeSlotCounts(RoomsParticipant* roomsParticipant, Slots slots)
    {
        N_assert(roomDestroyed==false);

        // Is roomsParticipant in the roomOutput?
        NCount roomsParticipantIndex = GetRoomIndex(roomsParticipant);
        if (roomsParticipantIndex==-1) return REC_CHANGE_SLOT_COUNTS_NOT_IN_ROOM;

        // Is roomsParticipant the moderator?
        if (roomMemberList[roomsParticipantIndex]->roomMemberMode!=RMM_MODERATOR)
            return REC_CHANGE_SLOT_COUNTS_MUST_BE_MODERATOR;

        // Validate slots - cannot be negative
        RoomsErrorCode roomsErrorCode = slots.Validate();
        if (roomsErrorCode!=REC_SUCCESS)
            return roomsErrorCode;

        SetTotalSlots(&slots);
        UpdateUsedSlots();

        return REC_SUCCESS;
    }
    RoomsErrorCode Room::SetCustomRoomProperties(RoomsParticipant* roomsParticipant, DataStructures::Table *table)
    {
        N_assert(roomDestroyed==false);

        // Is roomsParticipant in the roomOutput?
        NCount roomsParticipantIndex = GetRoomIndex(roomsParticipant);
        if (roomsParticipantIndex==-1) return REC_SET_CUSTOM_ROOM_PROPERTIES_NOT_IN_ROOM;

        // Is roomsParticipant the moderator?
        if (roomMemberList[roomsParticipantIndex]->roomMemberMode!=RMM_MODERATOR)
            return REC_SET_CUSTOM_ROOM_PROPERTIES_MUST_BE_MODERATOR;

        // Validate table - can only have custom properties, not predefined properties
        if (DefaultRoomColumns::HasDefaultColumns(table))
            return REC_SET_CUSTOM_ROOM_PROPERTIES_CONTAINS_DEFAULT_COLUMNS;

        return REC_SUCCESS;
    }
    RoomsErrorCode Room::ChangeRoomName(RoomsParticipant* roomsParticipant, RakString newRoomName, ProfanityFilter *profanityFilter)
    {
        N_assert(roomDestroyed==false);

        // Is roomsParticipant in the roomOutput?
        NCount roomsParticipantIndex = GetRoomIndex(roomsParticipant);
        if (roomsParticipantIndex==-1) return REC_CHANGE_ROOM_NAME_NOT_IN_ROOM;

        // Is roomsParticipant the moderator?
        if (roomMemberList[roomsParticipantIndex]->roomMemberMode!=RMM_MODERATOR)
            return REC_CHANGE_ROOM_NAME_MUST_BE_MODERATOR;

        // Is newRoomName empty?
        if (newRoomName.IsEmpty())
            return REC_CHANGE_ROOM_NAME_EMPTY_ROOM_NAME;

        // Does newRoomName have profanity?
        if (profanityFilter->HasProfanity(newRoomName.C_String()))
            return REC_CHANGE_ROOM_NAME_HAS_PROFANITY;

        SetStringProperty(DefaultRoomColumns::TC_ROOM_NAME, newRoomName.C_String());
        return REC_SUCCESS;
    }
    RoomsErrorCode Room::SetHiddenFromSearches(RoomsParticipant* roomsParticipant, bool _hiddenFromSearches)
    {
        N_assert(roomDestroyed==false);

        // Is roomsParticipant in the roomOutput?
        NCount roomsParticipantIndex = GetRoomIndex(roomsParticipant);
        if (roomsParticipantIndex==-1) return REC_SET_HIDDEN_FROM_SEARCHES_NOT_IN_ROOM;

        // Is roomsParticipant the moderator?
        if (roomMemberList[roomsParticipantIndex]->roomMemberMode!=RMM_MODERATOR)
            return REC_SET_HIDDEN_FROM_SEARCHES_MUST_BE_MODERATOR;

        hiddenFromSearches=_hiddenFromSearches;
        return REC_SUCCESS;
    }
    RoomsErrorCode Room::SetDestroyOnModeratorLeave(RoomsParticipant* roomsParticipant, bool destroyOnModeratorLeave)
    {
        N_assert(roomDestroyed==false);

        // Is roomsParticipant in the roomOutput?
        NCount roomsParticipantIndex = GetRoomIndex(roomsParticipant);
        if (roomsParticipantIndex==-1) return REC_SET_DESTROY_ON_MODERATOR_LEAVE_NOT_IN_ROOM;

        // Is roomsParticipant the moderator?
        if (roomMemberList[roomsParticipantIndex]->roomMemberMode!=RMM_MODERATOR)
            return REC_SET_DESTROY_ON_MODERATOR_LEAVE_MUST_BE_MODERATOR;

        tableRow->cells[DefaultRoomColumns::TC_DESTROY_ON_MODERATOR_LEAVE]->Set((int) destroyOnModeratorLeave);
        return REC_SUCCESS;
    }
    RoomsErrorCode Room::SetReadyStatus(RoomsParticipant* roomsParticipant, bool isReady)
    {
        N_assert(roomDestroyed==false);

        // Is roomsParticipant in the roomOutput?
        NCount roomsParticipantIndex = GetRoomIndex(roomsParticipant);
        if (roomsParticipantIndex==-1) return REC_SET_READY_STATUS_NOT_IN_ROOM;

        // Is roomsParticipant a player?
        if (roomMemberList[roomsParticipantIndex]->roomMemberMode!=RMM_MODERATOR &&
            roomMemberList[roomsParticipantIndex]->roomMemberMode!=RMM_PUBLIC &&
            roomMemberList[roomsParticipantIndex]->roomMemberMode!=RMM_RESERVED)
            return REC_SET_READY_STATUS_NOT_IN_PLAYABLE_SLOT;

        // Is autoLockReadyStatus true, the roomOutput locked or the roomOutput full of players, and all the other players are ready?
        bool allReady;
        AreAllMembersReady(roomsParticipantIndex,&allReady);
        if (autoLockReadyStatus==true &&
            isReady==false &&
            (AreAllPlayableSlotsFilled() || IsRoomLockedToPlayers()) &&
            allReady)
            return REC_SET_READY_STATUS_AUTO_LOCK_ALL_PLAYERS_READY;

        roomMemberList[roomsParticipantIndex]->isReady=isReady;
        return REC_SUCCESS;
    }
    RoomsErrorCode Room::GetReadyStatus(DataStructures::List<RoomsParticipant*> &readyUsers, DataStructures::List<RoomsParticipant*> &unreadyUsers)
    {
        N_assert(roomDestroyed==false);

        readyUsers.Clear(true);
        unreadyUsers.Clear(true);
        NCount i;
        for(i = 0; i < roomMemberList.Size(); ++i)
        {
            if (roomMemberList[i]->isReady)
                readyUsers.Insert(roomMemberList[i]->roomsParticipant);
            else
                unreadyUsers.Insert(roomMemberList[i]->roomsParticipant);
        }
        return REC_SUCCESS;
    }
    RoomsErrorCode Room::SetRoomLockState(RoomsParticipant* roomsParticipant, RoomLockState _roomLockState)
    {
        N_assert(roomDestroyed==false);

        // Is roomsParticipant in the roomOutput?
        NCount roomsParticipantIndex = GetRoomIndex(roomsParticipant);
        if (roomsParticipantIndex==-1) return REC_SET_ROOM_LOCK_STATE_NOT_IN_ROOM;

        // Is roomsParticipant the moderator?
        if (roomMemberList[roomsParticipantIndex]->roomMemberMode!=RMM_MODERATOR)
            return REC_SET_ROOM_LOCK_STATE_MUST_BE_MODERATOR;

        // Is roomLockState a valid enumeration value?
        if ((int)roomLockState<0 || (int)roomLockState>(int)RLS_ALL_LOCKED)
            return REC_SET_ROOM_LOCK_STATE_BAD_ENUMERATION_VALUE;

        roomLockState=_roomLockState;
        return REC_SUCCESS;
    }
    RoomsErrorCode Room::GetRoomLockState(RoomLockState *_roomLockState)
    {
        N_assert(roomDestroyed==false);

        *_roomLockState=roomLockState;
        return REC_SUCCESS;
    }
    RoomsErrorCode Room::AreAllMembersReady(NCount exceptThisIndex, bool *allReady)
    {
        N_assert(roomDestroyed==false);

        // Is the ready status for all members true?
        NCount i;
        for (i=0; i < roomMemberList.Size(); i++)
        {
            if (roomMemberList[i]->isReady==false && i!=exceptThisIndex)
            {
                *allReady=false;
                return REC_SUCCESS;
            }
        }

        *allReady=true;
        return REC_SUCCESS;
    }
    RoomsErrorCode Room::KickMember(RoomsParticipant * roomsParticipant,
        RoomsParticipant * kickedParticipant, RakString reason)
    {
        N_assert(roomDestroyed == false);

        // Is roomsParticipant in the roomOutput?
        NCount roomsParticipantIndex = GetRoomIndex(roomsParticipant);
        if (roomsParticipantIndex == -1) return REC_KICK_MEMBER_NOT_IN_ROOM;

        // Is roomsParticipant the moderator?
        if (roomMemberList[roomsParticipantIndex]->roomMemberMode!=RMM_MODERATOR)
            return REC_KICK_MEMBER_MUST_BE_MODERATOR;

        // Is kickedParticipant in the roomOutput, or in a spectator slot?
        NCount kickedParticipantIndex = GetRoomIndex(kickedParticipant);
        if (kickedParticipantIndex == -1)
            return REC_KICK_MEMBER_TARGET_NOT_IN_YOUR_ROOM;

        if (roomsParticipant == kickedParticipant)
            return REC_KICK_MEMBER_CANNOT_PERFORM_ON_SELF;

        kickedParticipant->SetRoom(0);
        roomMemberList.RemoveAtIndex(kickedParticipantIndex);

        if (GetBannedIndex(kickedParticipant->GetName()) == -1)
        {
            BannedUser bannedUser;
            bannedUser.reason = reason;
            bannedUser.target = kickedParticipant->GetName();
            banList.Insert(bannedUser);
        }

        return REC_SUCCESS;
    }
    RoomsErrorCode Room::UnbanMember(RoomsParticipant* roomsParticipant, RakString name)
    {
        N_assert(roomDestroyed==false);

        // Is roomsParticipant in the roomOutput?
        NCount roomsParticipantIndex = GetRoomIndex(roomsParticipant);
        if (roomsParticipantIndex==-1) return REC_UNBAN_MEMBER_NOT_IN_ROOM;

        // Is roomsParticipant the moderator?
        if (roomMemberList[roomsParticipantIndex]->roomMemberMode!=RMM_MODERATOR)
            return REC_UNBAN_MEMBER_MUST_BE_MODERATOR;

        NCount banIndex = GetBannedIndex(name);
        if (banIndex==-1)
            return REC_UNBAN_MEMBER_NOT_BANNED;

        banList.RemoveAtIndexFast(banIndex);

        return REC_SUCCESS;
    }
    RoomsErrorCode Room::GetBanReason(RakString name, RakString *reason)
    {
        N_assert(roomDestroyed==false);
        NCount banIndex = GetBannedIndex(name);
        if (banIndex==-1)
            return REC_GET_BAN_REASON_NOT_BANNED;

        *reason = banList[banIndex].reason;

        return REC_SUCCESS;
    }
    RoomsErrorCode Room::LeaveRoom(RoomsParticipant* roomsParticipant, RemoveUserResult *removeUserResult)
    {
        N_assert(roomDestroyed==false);

        return RemoveUser(roomsParticipant,removeUserResult);
    }
    /*
    RoomsErrorCode Room::GetKickReason(RoomsParticipant* roomsParticipant, RakString *kickReason)
    {
        N_assert(roomDestroyed==false);

        // Is roomsParticipant in a kick slot?
        NCount roomsParticipantIndex = GetKickSlotIndex(roomsParticipant);
        if (roomsParticipantIndex==-1) return REC_GET_KICK_REASON_NOT_KICKED;

        *kickReason=kickedList[roomsParticipantIndex].reason;
        return REC_SUCCESS;
    }
    */
    RoomsErrorCode Room::JoinByFilter(RoomsParticipant* roomsParticipant, RoomMemberMode roomMemberMode, JoinedRoomResult *joinRoomResult)
    {
        N_assert(roomDestroyed==false);

        if (roomMemberMode==RMM_MODERATOR) return REC_JOIN_BY_FILTER_CANNOT_JOIN_AS_MODERATOR;
        // Is the roomOutput locked for the target slot type (any slot and RLS_ALL_LOCKED, or player and RLS_PLAYERS_LOCKED)?
        joinRoomResult->roomOutput=0;
        bool joinAsSpectator = roomMemberMode==RMM_SPECTATOR_PUBLIC || roomMemberMode==RMM_SPECTATOR_RESERVED || roomMemberMode==RMM_ANY_SPECTATOR;
        ParticipantCanJoinRoomResult pcjrr = ParticipantCanJoinRoom(roomsParticipant, joinAsSpectator, true);
        if (pcjrr==PCJRR_BANNED) return REC_JOIN_BY_FILTER_BANNED;
        if (pcjrr==PCJRR_NO_PUBLIC_SLOTS || pcjrr==PCJRR_NO_PUBLIC_OR_RESERVED_SLOTS) return REC_JOIN_BY_FILTER_NO_SLOTS;
        if (pcjrr==PCJRR_LOCKED) return REC_JOIN_BY_FILTER_ROOM_LOCKED;
        N_assert(pcjrr==PCJRR_SUCCESS);

        joinRoomResult->roomOutput=this;
        NCount firstInviteIndex = GetFirstInviteIndex(roomsParticipant->GetName());
        if (roomMemberMode==RMM_ANY_SPECTATOR)
        {
            if (firstInviteIndex==-1)
                roomMemberMode=RMM_SPECTATOR_PUBLIC;
            else
                roomMemberMode=RMM_SPECTATOR_RESERVED;
        }
        else if (roomMemberMode==RMM_ANY_PLAYABLE)
        {
            if (firstInviteIndex==-1)
                roomMemberMode=RMM_PUBLIC;
            else
                roomMemberMode=RMM_RESERVED;
        }
        RoomMember *rm = new RoomMember;
        rm->roomsParticipant = roomsParticipant;
        rm->roomMemberMode = roomMemberMode;
        roomMemberList.Insert(rm);
        roomsParticipant->SetRoom(this);

        if (firstInviteIndex != -1)
        {
            joinRoomResult->acceptedInvitor=roomMemberList[firstInviteIndex]->roomsParticipant;
            inviteList.RemoveAtIndex(firstInviteIndex);
        }
        else
            joinRoomResult->acceptedInvitor=0;

        // Moderator does not count towards a slot
        UpdateUsedSlots();
        N_assert(GetNumericProperty(DefaultRoomColumns::TC_USED_SLOTS)==roomMemberList.Size()-1);

        joinRoomResult->joiningMember=roomsParticipant;
        return REC_SUCCESS;
    }
    RoomsErrorCode Room::JoinByQuickJoin(RoomsParticipant* roomsParticipant, RoomMemberMode roomMemberMode, JoinedRoomResult *joinRoomResult)
    {
        N_assert(roomDestroyed==false);

        // Use same function, just add to the error code if one is returned
        RoomsErrorCode roomsErrorCode = JoinByFilter(roomsParticipant, roomMemberMode, joinRoomResult);
        if (roomsErrorCode!=REC_SUCCESS)
        {
            int val = (int) roomsErrorCode;
            val += (int)REC_JOIN_BY_QUICK_JOIN_CANNOT_JOIN_AS_MODERATOR-(int)REC_JOIN_BY_FILTER_CANNOT_JOIN_AS_MODERATOR;
            roomsErrorCode=(RoomsErrorCode)val;
        }
        else
        {
            // Moderator does not count towards a slot
            UpdateUsedSlots();
            N_assert(GetNumericProperty(DefaultRoomColumns::TC_USED_SLOTS)==roomMemberList.Size()-1);
        }
        return roomsErrorCode;
    }
    ParticipantCanJoinRoomResult Room::ParticipantCanJoinRoom( RoomsParticipant* roomsParticipant, bool asSpectator, bool checkHasInvite )
    {
        N_assert(roomDestroyed==false);

        // Is roomsParticipant already in the room?
        NCount roomsParticipantIndex = GetRoomIndex(roomsParticipant);
        if (roomsParticipantIndex!=-1)
            return PCJRR_SLOT_ALREADY_USED;

        return ParticipantCanJoinAsPlayer(roomsParticipant, asSpectator, checkHasInvite);
    }
    ParticipantCanJoinRoomResult Room::ParticipantCanJoinAsPlayer( RoomsParticipant* roomsParticipant, bool asSpectator, bool checkHasInvite )
    {
        // Is the room locked?
        if (roomLockState==RLS_PLAYERS_LOCKED && asSpectator==false)
            return PCJRR_LOCKED;

        // Is the room locked?
        if (roomLockState==RLS_ALL_LOCKED)
            return PCJRR_LOCKED;

        // Is the player banned?
        if (GetBannedIndex(roomsParticipant->GetName())!=-1)
            return PCJRR_BANNED;

        // Any slots?
        bool hasInvite = HasInvite(roomsParticipant->GetName());
        bool publicAvailable = HasOpenPublicSlots();

        if (asSpectator==false)
        {
            if (hasInvite==false && checkHasInvite)
            {
                if (publicAvailable==false)
                    return PCJRR_NO_PUBLIC_SLOTS;
            }

            // Invited will join public if no reserved slots are open
            bool reservedAvailable = HasOpenReservedSlots();
            if (publicAvailable==false && reservedAvailable==false)
                return PCJRR_NO_PUBLIC_OR_RESERVED_SLOTS;
        }
        else
        {
            if (HasOpenSpectatorSlots()==false)
                return PCJRR_NO_SPECTATOR_SLOTS;
        }

        return PCJRR_SUCCESS;
    }
    bool Room::IsRoomDead(void) const
    {
        return roomMemberList.Size()==0 || roomDestroyed==true;
    }
    RoomsErrorCode Room::GetInvitesToParticipant(RoomsParticipant* roomsParticipant, DataStructures::List<InvitedUser*> &invites)
    {
        N_assert(roomDestroyed==false);

        NCount i;
        for (i = 0; i < inviteList.Size(); ++i)
        {
            if (inviteList[i].target == roomsParticipant->GetName())
                invites.Insert(&inviteList[i]);
        }
        return REC_SUCCESS;
    }
    RoomsParticipant* Room::GetModerator(void) const
    {
        NCount i;
        for (i=0; i < roomMemberList.Size(); i++)
            if (roomMemberList[i]->roomMemberMode==RMM_MODERATOR)
                return roomMemberList[i]->roomsParticipant;
        N_assert("Room::GetModerator: Room should always have a moderator" && 0);
        return 0;
    }
    RoomID Room::GetID(void) const
    {
        return lobbyRoomId;
    }
    NIId Room::GetNumericProperty(int index) const
    {
        return tableRow->cells[index]->i;
    }
    const char *Room::GetStringProperty(int index) const
    {
        return tableRow->cells[index]->c;
    }
    void Room::SetNumericProperty(int index, NIId value)
    {
        tableRow->cells[index]->Set(value);
    }
    void Room::SetStringProperty(int index, const char *value)
    {
        tableRow->cells[index]->Set(value);
    }
    RoomsErrorCode Room::RemoveUser(RoomsParticipant* roomsParticipant,RemoveUserResult *removeUserResult)
    {
        N_assert(roomDestroyed==false);

        removeUserResult->gotNewModerator=false;
        removeUserResult->removedFromRoom=false;
        removeUserResult->room=this;
        NCount roomsParticipantIndex = GetRoomIndex(roomsParticipant);
        if (roomsParticipantIndex==-1) return REC_REMOVE_USER_NOT_IN_ROOM;
        removeUserResult->removedFromRoom=true;
        removeUserResult->removedUserAddress=roomsParticipant->GetSystemAddress();
        removeUserResult->removedUserName=roomsParticipant->GetName();

        if (roomMemberList[roomsParticipantIndex]->roomMemberMode==RMM_MODERATOR)
        {
            int destroyOnModeratorLeave;
            tableRow->cells[DefaultRoomColumns::TC_DESTROY_ON_MODERATOR_LEAVE]->Get(&destroyOnModeratorLeave);
            if (destroyOnModeratorLeave || roomMemberList.Size()==1)
            {
                removeUserResult->clearedInvitations=inviteList;
                inviteList.Clear(false);

                removeUserResult->roomDestroyed = true;
                NCount i;
                for (i = 0; i < roomMemberList.Size(); ++i)
                    roomMemberList[i]->roomsParticipant->SetRoom(0);
                roomDestroyed=true;
                removeUserResult->roomDestroyed = true;
                // Up to caller to call PerGamesRoomContainer::DestroyRoom();
                return REC_SUCCESS;
            }

            // Transfer moderator to the next player, if any. Reserved slot players take priority to be moderator.
            NCount nextModIndex;
            for (nextModIndex = 0; nextModIndex < roomMemberList.Size(); ++nextModIndex)
            {
                if (roomMemberList[nextModIndex]->roomMemberMode==RMM_RESERVED)
                {
                    roomMemberList[nextModIndex]->roomMemberMode=RMM_MODERATOR;
                    removeUserResult->gotNewModerator=true;
                    break;
                }
            }
            if (removeUserResult->gotNewModerator==false)
            {
                for (nextModIndex = 0; nextModIndex < roomMemberList.Size(); nextModIndex++)
                {
                    if (roomMemberList[nextModIndex]->roomMemberMode==RMM_PUBLIC)
                    {
                        roomMemberList[nextModIndex]->roomMemberMode=RMM_MODERATOR;
                        removeUserResult->gotNewModerator=true;
                        break;
                    }
                }
            }

            // If moderator and clearInvitesOnNewModerator, clear invitations and add to removeUserResult->clearedInvitations
            if (clearInvitesOnNewModerator)
            {
                removeUserResult->clearedInvitations=inviteList;
                inviteList.Clear(false);

                NCount i;
                for (i = 0; i < roomMemberList.Size(); ++i)
                {
                    if (roomMemberList[i]->roomMemberMode==RMM_SPECTATOR_RESERVED)
                        roomMemberList[i]->roomMemberMode=RMM_SPECTATOR_PUBLIC;
                }
            }
        }

        delete roomMemberList[roomsParticipantIndex];
        roomMemberList.RemoveAtIndex(roomsParticipantIndex);
        roomsParticipant->SetRoom(0);
        removeUserResult->roomDestroyed=IsRoomDead();
        roomDestroyed=removeUserResult->roomDestroyed;
        removeUserResult->removedFromRoom=true;
        if (removeUserResult->roomDestroyed==false)
            UpdateUsedSlots();

        return REC_SUCCESS;

    }
    bool Room::IsRoomLockedToSpectators(void) const
    {
        return roomLockState==RLS_ALL_LOCKED;
    }
    bool Room::IsRoomLockedToPlayers(void) const
    {
        return roomLockState==RLS_PLAYERS_LOCKED || roomLockState==RLS_ALL_LOCKED;
    }
    bool Room::IsInRoom(RoomsParticipant* roomsParticipant) const
    {
        return GetRoomIndex(roomsParticipant)!=-1;
    }
    bool Room::HasInvite(RakString roomsParticipant)
    {
        return GetFirstInviteIndex(roomsParticipant)!=-1;
    }
    NCount Room::GetRoomIndex(RoomsParticipant* roomsParticipant) const
    {
        NCount i;
        for (i=0; i < roomMemberList.Size(); i++)
            if (roomMemberList[i]->roomsParticipant==roomsParticipant)
                return i;
        return (NCount) -1;
    }
    /*
    NCount Room::GetKickSlotIndex(RoomsParticipant* roomsParticipant) const
    {
        NCount i;
        for (i=0; i < kickedList.Size(); i++)
            if (kickedList[i].roomsParticipant==roomsParticipant)
                return i;
        return -1;
    }
    */
    NCount Room::GetBannedIndex(RakString username) const
    {
        NCount i;
        for (i=0; i < banList.Size(); i++)
            if (banList[i].target==username)
                return i;
        return (NCount) -1;
    }
    NCount Room::GetInviteIndex(RakString invitee, RakString invitor) const
    {
        NCount i;
        for (i=0; i < inviteList.Size(); i++)
            if (inviteList[i].target==invitee && inviteList[i].invitorName==invitor)
                return i;
        return (NCount) -1;
    }
    NCount Room::GetFirstInviteIndex(RakString invitee) const
    {
        NCount i;
        for (i=0; i < inviteList.Size(); i++)
            if (inviteList[i].target==invitee)
                return i;
        return (NCount) -1;
    }
    bool Room::AreAllPlayableSlotsFilled(void) const
    {
        return HasOpenPublicSlots()==false && HasOpenReservedSlots()==false;
    }
    bool Room::HasOpenPublicSlots(void) const
    {
        return GetNumericProperty(DefaultRoomColumns::TC_REMAINING_PUBLIC_SLOTS)!=0.0;
    }
    bool Room::HasOpenReservedSlots(void) const
    {
        return GetNumericProperty(DefaultRoomColumns::TC_REMAINING_RESERVED_SLOTS)!=0.0;
    }
    bool Room::HasOpenSpectatorSlots(void) const
    {
        return GetNumericProperty(DefaultRoomColumns::TC_REMAINING_SPECTATOR_SLOTS)!=0.0;
    }
    bool Room::IsHiddenToParticipant(RoomsParticipant* roomsParticipant) const
    {
        if (hiddenFromSearches==false)
            return false;
        if (IsInRoom(roomsParticipant))
            return false;
        if (GetFirstInviteIndex(roomsParticipant->GetName())!=-1)
            return false;
        return true;
    }
    void Room::ChangeHandle(RakString oldHandle, RakString newHandle)
    {
        if (oldHandle==newHandle)
            return;
        NCount index;
        index = GetBannedIndex(newHandle);
        N_assert(index==-1);
        index = GetFirstInviteIndex(newHandle);
        N_assert(index==-1);
        index = GetBannedIndex(oldHandle);
        if (index!=-1)
            banList[index].target=newHandle;
        for (index=0; index < inviteList.Size(); index++)
        {
            if (inviteList[index].target==oldHandle)
                inviteList[index].target=newHandle;
            if (inviteList[index].invitorName==oldHandle)
                inviteList[index].invitorName=newHandle;
        }
    }
    // ----------------------------  UNIT TEST  ----------------------------
    void AllGamesRoomsContainer::UnitTest(void)
    {
        RoomsErrorCodeDescription::Validate();

        AllGamesRoomsContainer agrc;
        RoomsErrorCode roomsErrorCode;
        GameIdentifier gameIdentifier1, gameIdentifier2;
        DataStructures::List<InvitedUser> clearedInvitations;
        gameIdentifier1="gameIdentifier1";
        gameIdentifier2="gameIdentifier2";
        RoomsParticipant roomsParticipant1, roomsParticipant2;
        roomsParticipant1.SetName("roomsParticipant1");
        roomsParticipant2.SetName("roomsParticipant2");
        RoomQuery roomQuery;
        roomQuery.numQueries=2;
        DataStructures::Table::FilterQuery filterQuery[2];
        roomQuery.queries=filterQuery;
        DataStructures::Table::Cell cells[2];
        filterQuery[0].cellValue=&cells[0];
        filterQuery[1].cellValue=&cells[1];
        strcpy(filterQuery[0].columnName, "LevelName");
        strcpy(filterQuery[1].columnName, "FlagCaptureCount");
        filterQuery[0].operation=DataStructures::Table::QF_EQUAL;
        filterQuery[1].operation=DataStructures::Table::QF_NOT_EQUAL;
        cells[0].Set("DogFoodLevel");
        cells[1].Set(10);
        DataStructures::OrderedList<Room*, Room*, RoomsSortByName> roomsOutput;
        bool onlyJoinable;
        RemoveUserResult removeUserResult;
        QuickJoinUser quickJoinUser;
        DataStructures::List<QuickJoinUser*> timeoutExpired;
        DataStructures::List<JoinedRoomResult> joinedRoomMembers;
        Room *room;
        RoomCreationParameters roomCreationParameters;
        quickJoinUser.networkedQuickJoinUser.minimumPlayers=2;
        quickJoinUser.networkedQuickJoinUser.query=roomQuery;
        quickJoinUser.roomsParticipant=&roomsParticipant1;
        quickJoinUser.networkedQuickJoinUser.timeout=5000;
        ProfanityFilter profanityFilter;
        profanityFilter.AddWord("BADWORD");
        DataStructures::Table customRoomProperties;
        customRoomProperties.AddColumn(filterQuery[0].columnName, DataStructures::Table::STRING);
        customRoomProperties.AddColumn(filterQuery[1].columnName, DataStructures::Table::NUMERIC);
        DataStructures::List<QuickJoinUser*> dereferencedPointers;
        DataStructures::List<DataStructures::Table::Cell*> cellList;
        DataStructures::List<InvitedUser*> invites;
        RakString kickReason;
        cellList.Insert(&cells[0]);
        cellList.Insert(&cells[1]);
        customRoomProperties.AddRow(0, cellList);

        // Search by filter
        onlyJoinable=true;
        roomsErrorCode = agrc.SearchByFilter( gameIdentifier1, &roomsParticipant1, &roomQuery, roomsOutput, onlyJoinable );
        N_assert(roomsErrorCode==REC_SEARCH_BY_FILTER_UNKNOWN_TITLE);

        // leave room
        roomsErrorCode = agrc.LeaveRoom(&roomsParticipant1, &removeUserResult);
        N_assert(roomsErrorCode==REC_LEAVE_ROOM_NOT_IN_ROOM);

        // Add title
        roomsErrorCode = agrc.AddTitle(gameIdentifier1);
        N_assert(roomsErrorCode==REC_SUCCESS);
        roomsErrorCode = agrc.AddTitle(gameIdentifier1);
        N_assert(roomsErrorCode==REC_ADD_TITLE_ALREADY_IN_USE);

        // Search by filter
        roomsErrorCode = agrc.SearchByFilter( gameIdentifier1, &roomsParticipant1, &roomQuery, roomsOutput, onlyJoinable );
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Search by filter (another game)
        roomsErrorCode = agrc.SearchByFilter( gameIdentifier2, &roomsParticipant1, &roomQuery, roomsOutput, onlyJoinable );
        N_assert(roomsErrorCode==REC_SEARCH_BY_FILTER_UNKNOWN_TITLE);

        // leave room
        roomsErrorCode = agrc.LeaveRoom( &roomsParticipant1, &removeUserResult);
        N_assert(roomsErrorCode==REC_LEAVE_ROOM_NOT_IN_ROOM);

        // Quick join
        roomsErrorCode = agrc.AddUserToQuickJoin(gameIdentifier1, &quickJoinUser);
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Process quick join
        roomsErrorCode = agrc.ProcessQuickJoins( timeoutExpired, joinedRoomMembers, dereferencedPointers, 1000);
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Let timeout expire
        // Process quick join
        roomsErrorCode = agrc.ProcessQuickJoins( timeoutExpired, joinedRoomMembers, dereferencedPointers, 5000);
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(timeoutExpired.Size()==1 && timeoutExpired[0]==&quickJoinUser);

        // Quick join
        roomsErrorCode = agrc.AddUserToQuickJoin(gameIdentifier1, &quickJoinUser);
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Quick join (should fail)
        roomsErrorCode = agrc.AddUserToQuickJoin(gameIdentifier1, &quickJoinUser);
        N_assert(roomsErrorCode==REC_ADD_TO_QUICK_JOIN_ALREADY_THERE);

        // Create room (should fail due to in quick join)
        roomCreationParameters.networkedRoomCreationParameters.inviteToRoomPermission=NetworkedRoomCreationParameters::INVITE_MODE_RESERVED_SLOTS_CAN_INVITE;
        roomCreationParameters.networkedRoomCreationParameters.inviteToSpectatorSlotPermission=NetworkedRoomCreationParameters::INVITE_MODE_RESERVED_SLOTS_CAN_INVITE;
        roomCreationParameters.networkedRoomCreationParameters.autoLockReadyStatus=true;
        roomCreationParameters.networkedRoomCreationParameters.clearInvitesOnNewModerator=true;
        roomCreationParameters.networkedRoomCreationParameters.roomName="BADWORD room";
        roomCreationParameters.firstUser=&roomsParticipant1;
        roomCreationParameters.gameIdentifier=gameIdentifier1;
        roomCreationParameters.networkedRoomCreationParameters.slots.publicSlots=1;
        roomCreationParameters.networkedRoomCreationParameters.slots.reservedSlots=1;
        roomCreationParameters.networkedRoomCreationParameters.slots.spectatorSlots=1;
        roomsErrorCode = agrc.CreateRoom(&roomCreationParameters, &profanityFilter);
        N_assert(roomsErrorCode==REC_CREATE_ROOM_CURRENTLY_IN_QUICK_JOIN);

        // Cancel quick join
        QuickJoinUser *qju;
        roomsErrorCode = agrc.RemoveUserFromQuickJoin(quickJoinUser.roomsParticipant, &qju);
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Create room (should fail due to bad word)
        roomsErrorCode = agrc.CreateRoom(&roomCreationParameters, &profanityFilter);
        N_assert(roomsErrorCode==REC_ROOM_CREATION_PARAMETERS_ROOM_NAME_HAS_PROFANITY);

        // Create room
        roomCreationParameters.networkedRoomCreationParameters.roomName="My room";
        roomsErrorCode = agrc.CreateRoom(&roomCreationParameters, &profanityFilter);
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomCreationParameters.roomOutput);

        // Create room (should fail due to already in room)
        roomsErrorCode = agrc.CreateRoom(&roomCreationParameters, &profanityFilter);
        N_assert(roomsErrorCode==REC_CREATE_ROOM_CURRENTLY_IN_A_ROOM);

        // Quick join (should fail due to already in room)
        roomsErrorCode = agrc.AddUserToQuickJoin(gameIdentifier1, &quickJoinUser);
        N_assert(roomsErrorCode==REC_ADD_TO_QUICK_JOIN_CURRENTLY_IN_A_ROOM);

        // Search by filter
        roomsErrorCode = agrc.SearchByFilter( gameIdentifier1, &roomsParticipant1, &roomQuery, roomsOutput, onlyJoinable );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomsOutput.Size()==0);

        // Set room properties (Should fail due to not being in the room)
        roomsErrorCode = agrc.SetCustomRoomProperties( &roomsParticipant2, &customRoomProperties );
        N_assert(roomsErrorCode==REC_SET_CUSTOM_ROOM_PROPERTIES_UNKNOWN_ROOM_ID);

        // Set room properties
        roomsErrorCode = agrc.SetCustomRoomProperties( &roomsParticipant1, &customRoomProperties );
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Search by filter, including not joinable
        N_assert(&filterQuery[1]==&roomQuery.queries[1]);
        roomQuery.queries[1].cellValue->Set(1234); // Query 1 is not equal, change to something else
        roomsErrorCode = agrc.SearchByFilter( gameIdentifier1, &roomsParticipant1, &roomQuery, roomsOutput, false );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomsOutput.Size()==1);

        // Invite member to room (yourself, should fail)
        roomsErrorCode = agrc.SendInvite( &roomsParticipant1, &roomsParticipant1, false, "SendInviteSubject", "SendInviteBody" );
        N_assert(roomsErrorCode==REC_SEND_INVITE_CANNOT_PERFORM_ON_SELF);

        // Invite member to room
        roomsErrorCode = agrc.SendInvite( &roomsParticipant1, &roomsParticipant2, false, "SendInviteSubject", "SendInviteBody" );
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Get invites to member
        roomsErrorCode = agrc.GetInvitesToParticipant(&roomsParticipant2, invites);
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(invites.Size()==1);
        N_assert(invites[0]->subject=="SendInviteSubject");
        N_assert(invites[0]->invitorName==roomsParticipant1.GetName());

        // Leave room
        roomsErrorCode = agrc.LeaveRoom( &roomsParticipant1, &removeUserResult);
        N_assert(roomsParticipant1.GetRoom()==0);
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(removeUserResult.removedFromQuickJoin==false);
        N_assert(removeUserResult.removedFromRoom==true);
        N_assert(removeUserResult.room==roomCreationParameters.roomOutput);
        N_assert(removeUserResult.clearedInvitations.Size()==1);
        N_assert(removeUserResult.roomDestroyed==true);

        // Clear room pointer
        agrc.DestroyRoomIfDead(removeUserResult.room);

        // Create room
        // Clear invites on moderator changes or leaves
        roomCreationParameters.networkedRoomCreationParameters.destroyOnModeratorLeave=false;
        roomCreationParameters.networkedRoomCreationParameters.clearInvitesOnNewModerator=true;
        roomsErrorCode = agrc.CreateRoom(&roomCreationParameters, &profanityFilter);
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomCreationParameters.roomOutput);
        N_assert(roomCreationParameters.roomOutput->GetNumericProperty(DefaultRoomColumns::TC_USED_PUBLIC_PLUS_RESERVED_SLOTS)==0);

        // Invite member to room
        roomsErrorCode = agrc.SendInvite( &roomsParticipant1, &roomsParticipant2, false, "SendInviteSubject", "SendInviteBody" );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomCreationParameters.roomOutput->GetNumericProperty(DefaultRoomColumns::TC_USED_PUBLIC_PLUS_RESERVED_SLOTS)==0);

        // member accepts invite (backwards parameters)
        roomsErrorCode = agrc.AcceptInvite( roomsParticipant1.GetRoom()->GetID(), &room, &roomsParticipant1, roomsParticipant2.GetName() );
        N_assert(roomsErrorCode==REC_ACCEPT_INVITE_CURRENTLY_IN_A_ROOM);
        N_assert(roomCreationParameters.roomOutput->GetNumericProperty(DefaultRoomColumns::TC_USED_PUBLIC_PLUS_RESERVED_SLOTS)==0);

        // member accepts invite (correct parameters)
        roomsErrorCode = agrc.AcceptInvite( roomsParticipant1.GetRoom()->GetID(), &room, &roomsParticipant2, roomsParticipant1.GetName() );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomsParticipant2.GetRoom()!=0);
        N_assert(roomsParticipant1.GetRoom()==roomsParticipant2.GetRoom());
        N_assert(roomCreationParameters.roomOutput->GetNumericProperty(DefaultRoomColumns::TC_USED_PUBLIC_PLUS_RESERVED_SLOTS)==1);

        // change moderator
        roomsErrorCode = agrc.GrantModerator(  &roomsParticipant1, &roomsParticipant2, clearedInvitations  );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomCreationParameters.roomOutput->GetNumericProperty(DefaultRoomColumns::TC_USED_PUBLIC_PLUS_RESERVED_SLOTS)==1);

        // new member invites existing member
        roomsErrorCode = agrc.SendInvite( &roomsParticipant2, &roomsParticipant1, false, "SendInviteSubject", "SendInviteBody" );
        N_assert(roomsErrorCode==REC_SEND_INVITE_INVITEE_ALREADY_IN_THIS_ROOM);
        N_assert(roomCreationParameters.roomOutput->GetNumericProperty(DefaultRoomColumns::TC_USED_PUBLIC_PLUS_RESERVED_SLOTS)==1);

        // Moderator leaves
        roomsErrorCode = agrc.LeaveRoom( &roomsParticipant2, &removeUserResult );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(removeUserResult.gotNewModerator==true);
        N_assert(roomCreationParameters.roomOutput->GetModerator()==&roomsParticipant1);
        N_assert(roomsParticipant2.GetRoom()==0);
        N_assert(roomCreationParameters.roomOutput->GetNumericProperty(DefaultRoomColumns::TC_USED_PUBLIC_PLUS_RESERVED_SLOTS)==0);

        // Invite the guy that just left
        roomsErrorCode = agrc.SendInvite( &roomsParticipant1, &roomsParticipant2, false, "SendInviteSubject", "SendInviteBody" );
        N_assert(roomsErrorCode==REC_SUCCESS);

        // member accepts invite
        roomsErrorCode = agrc.AcceptInvite( roomsParticipant1.GetRoom()->GetID(), &room, &roomsParticipant2, roomsParticipant1.GetName() );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomCreationParameters.roomOutput->GetNumericProperty(DefaultRoomColumns::TC_USED_PUBLIC_PLUS_RESERVED_SLOTS)==1);

        // change current moderator to to spectator (should fail)
        roomsErrorCode = agrc.StartSpectating( &roomsParticipant1 );
        N_assert(roomsErrorCode==REC_START_SPECTATING_REASSIGN_MODERATOR_BEFORE_SPECTATE);

        // change other member to spectator
        roomsErrorCode = agrc.StartSpectating( &roomsParticipant2 );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomCreationParameters.roomOutput->GetNumericProperty(DefaultRoomColumns::TC_USED_PUBLIC_PLUS_RESERVED_SLOTS)==0);

        // change member type to player
        roomsErrorCode = agrc.StopSpectating( &roomsParticipant2 );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomCreationParameters.roomOutput->GetNumericProperty(DefaultRoomColumns::TC_USED_PUBLIC_PLUS_RESERVED_SLOTS)==1);

        // Moderator logs off
        roomsErrorCode = agrc.RemoveUser( &roomsParticipant1, &removeUserResult );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomCreationParameters.roomOutput->GetNumericProperty(DefaultRoomColumns::TC_USED_PUBLIC_PLUS_RESERVED_SLOTS)==0);
        N_assert(roomCreationParameters.roomOutput->GetModerator()==&roomsParticipant2);
        N_assert(roomsParticipant1.GetRoom()==0);

        // Other player logs off
        roomsErrorCode = agrc.RemoveUser( roomCreationParameters.roomOutput->GetModerator(), &removeUserResult );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomsParticipant2.GetRoom()==0);
        agrc.DestroyRoomIfDead(removeUserResult.room);

        // both members quick join with a custom filter (equality).
        QuickJoinUser quickJoinUser1;
        quickJoinUser1.networkedQuickJoinUser.minimumPlayers=2;
        quickJoinUser1.networkedQuickJoinUser.query=roomQuery;
        quickJoinUser1.roomsParticipant=&roomsParticipant1;
        quickJoinUser1.networkedQuickJoinUser.timeout=10000;
        roomsErrorCode = agrc.AddUserToQuickJoin(gameIdentifier1, &quickJoinUser1);
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomsParticipant1.GetRoom()==0);
        N_assert(roomsParticipant2.GetRoom()==0);
        N_assert(roomsParticipant1.GetInQuickJoin()==true);

        QuickJoinUser quickJoinUser2;
        quickJoinUser2.networkedQuickJoinUser.minimumPlayers=2;
        quickJoinUser2.networkedQuickJoinUser.query=roomQuery;
        quickJoinUser2.roomsParticipant=&roomsParticipant2;
        quickJoinUser2.networkedQuickJoinUser.timeout=10000;
        roomsErrorCode = agrc.AddUserToQuickJoin(gameIdentifier1, &quickJoinUser2);
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomsParticipant1.GetRoom()==0);
        N_assert(roomsParticipant2.GetRoom()==0);
        N_assert(roomsParticipant2.GetInQuickJoin()==true);

        // update quick join. Should create a room
        roomsErrorCode = agrc.ProcessQuickJoins( timeoutExpired, joinedRoomMembers, dereferencedPointers, 1000);
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomsParticipant1.GetRoom()!=0);
        N_assert(roomsParticipant1.GetRoom()==roomsParticipant2.GetRoom());
        N_assert(joinedRoomMembers.Size()==2);

        // Both members log off
        roomsErrorCode = agrc.RemoveUser( &roomsParticipant1, &removeUserResult );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(removeUserResult.room->GetNumericProperty(DefaultRoomColumns::TC_USED_PUBLIC_PLUS_RESERVED_SLOTS)==0);
        N_assert(removeUserResult.room->GetModerator()==&roomsParticipant2);
        N_assert(roomsParticipant1.GetRoom()==0);
        roomsErrorCode = agrc.RemoveUser( &roomsParticipant2, &removeUserResult );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomsParticipant2.GetRoom()==0);
        agrc.DestroyRoomIfDead(removeUserResult.room);

        // Enter roomOutput (member one) (should create a room)
        JoinedRoomResult joinedRoomResult;
        roomCreationParameters.networkedRoomCreationParameters.slots.publicSlots=0;
        roomsErrorCode = agrc.EnterRoom( &roomCreationParameters, RMM_ANY_PLAYABLE, 0, 0, &joinedRoomResult );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomsParticipant1.GetRoom()!=0);
        N_assert(roomsParticipant1.GetRoom()->GetModerator()==&roomsParticipant1);
        N_assert(roomCreationParameters.roomOutput->GetNumericProperty(DefaultRoomColumns::TC_REMAINING_PUBLIC_PLUS_RESERVED_SLOTS)==1);

        // Set the properties on the new room
        roomsErrorCode = agrc.SetCustomRoomProperties( &roomsParticipant1, &customRoomProperties );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(agrc.GetPropertyIndex( roomsParticipant1.GetRoom()->GetID(), "LevelName")!=-1);
        N_assert(roomsParticipant1.GetRoom()->GetStringProperty(agrc.GetPropertyIndex( roomsParticipant1.GetRoom()->GetID(), "LevelName"))!=0);

        // Invite member two to the new room
        roomsErrorCode = agrc.SendInvite( &roomsParticipant1, &roomsParticipant2, false, "SendInviteSubject", "SendInviteBody" );
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Quick join (member two) (should join the created roomOutput)
        roomsErrorCode = agrc.AddUserToQuickJoin(gameIdentifier1, &quickJoinUser2);
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomsParticipant2.GetRoom()==0);
        N_assert(roomsParticipant2.GetInQuickJoin()==true);

        // update quick join. Should join the room, passing the required filter properties
        roomsErrorCode = agrc.ProcessQuickJoins( timeoutExpired, joinedRoomMembers, dereferencedPointers, 1000);
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomsParticipant1.GetRoom()!=0);
        N_assert(roomsParticipant1.GetRoom()==roomsParticipant2.GetRoom());
        N_assert(joinedRoomMembers.Size()==1);
        N_assert(cells[0].c);
        N_assert(strcmp(roomsParticipant1.GetRoom()->GetStringProperty(agrc.GetPropertyIndex( roomsParticipant1.GetRoom()->GetID(), "LevelName")),cells[0].c)==0);

        // Leave roomOutput (member two)
        roomsErrorCode = agrc.LeaveRoom( &roomsParticipant2, &removeUserResult );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(removeUserResult.gotNewModerator==false);
        N_assert(roomCreationParameters.roomOutput->GetModerator()==&roomsParticipant1);
        N_assert(roomsParticipant2.GetRoom()==0);
        N_assert(roomCreationParameters.roomOutput->GetNumericProperty(DefaultRoomColumns::TC_USED_PUBLIC_PLUS_RESERVED_SLOTS)==0);

        // Lock Room
        roomsErrorCode = agrc.SetRoomLockState( &roomsParticipant1, RLS_PLAYERS_LOCKED );
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Set member ready
        roomsErrorCode = agrc.SetReadyStatus( &roomsParticipant1, true );
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Set member unready
        roomsErrorCode = agrc.SetReadyStatus( &roomsParticipant1, false );
        N_assert(roomsErrorCode==REC_SET_READY_STATUS_AUTO_LOCK_ALL_PLAYERS_READY);

        // Unlock room
        roomsErrorCode = agrc.SetRoomLockState( &roomsParticipant1, RLS_NOT_LOCKED );
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Set member unready
        room=roomCreationParameters.roomOutput;
        roomsErrorCode = agrc.SetReadyStatus( &roomsParticipant1, false );
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Try to enter the room (no public slots, tried to create room of same name, failed)
        roomCreationParameters.firstUser=&roomsParticipant2;
        roomsErrorCode = agrc.EnterRoom( &roomCreationParameters, RMM_ANY_PLAYABLE, 0, 0, &joinedRoomResult );
        N_assert(roomsErrorCode==REC_ROOM_CREATION_PARAMETERS_ROOM_NAME_IN_USE);

        // Open a public slot
        Slots slots;
        slots.publicSlots=1;
        roomsErrorCode = agrc.ChangeSlotCounts( &roomsParticipant1, slots );
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Enter room
        roomCreationParameters.firstUser=&roomsParticipant2;
        roomsErrorCode = agrc.EnterRoom( &roomCreationParameters, RMM_ANY_PLAYABLE, 0, 0, &joinedRoomResult );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(roomsParticipant1.GetRoom()!=0);
        N_assert(roomsParticipant1.GetRoom()->GetModerator()==&roomsParticipant1);
        N_assert(roomsParticipant1.GetRoom()==roomsParticipant2.GetRoom());
        N_assert(joinedRoomResult.roomOutput->GetNumericProperty(DefaultRoomColumns::TC_REMAINING_PUBLIC_PLUS_RESERVED_SLOTS)==1);
        room = joinedRoomResult.roomOutput;

        // Kick member 2
        RakString banReason("Don't like you");
        roomsErrorCode = agrc.KickMember( &roomsParticipant1, &roomsParticipant2, banReason );
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Join by filter
        roomsErrorCode = agrc.JoinByFilter( gameIdentifier1, RMM_ANY_PLAYABLE, &roomsParticipant2, (RoomID)-1, &roomQuery, &joinedRoomResult);
        N_assert(roomsErrorCode==REC_JOIN_BY_FILTER_NO_ROOMS);

        RakString banReasonOut;
        roomsErrorCode = agrc.GetBanReason( room->GetID(), &room, roomsParticipant2.GetName(), &banReasonOut );
        N_assert(roomsErrorCode==REC_SUCCESS);
        N_assert(banReasonOut==banReason);

        // Unban member
        roomsErrorCode = agrc.UnbanMember( &roomsParticipant1, roomsParticipant2.GetName());
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Get ban reason
        roomsErrorCode = agrc.GetBanReason( room->GetID(), &room, roomsParticipant2.GetName(), &banReasonOut );
        N_assert(roomsErrorCode==REC_GET_BAN_REASON_NOT_BANNED);

        // Unban member again (should fail)
        roomsErrorCode = agrc.UnbanMember( &roomsParticipant1, roomsParticipant2.GetName());
        N_assert(roomsErrorCode==REC_UNBAN_MEMBER_NOT_BANNED);

        // Join by filter
        roomsErrorCode = agrc.JoinByFilter( gameIdentifier1, RMM_ANY_PLAYABLE, &roomsParticipant2, (RoomID)-1, &roomQuery, &joinedRoomResult);
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Set both members ready
        roomsErrorCode = agrc.SetReadyStatus( &roomsParticipant1, true );
        N_assert(roomsErrorCode==REC_SUCCESS);
        roomsErrorCode = agrc.SetReadyStatus( &roomsParticipant2, true );
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Set one member unready
        roomsErrorCode = agrc.SetReadyStatus( &roomsParticipant1, false );
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Kick out 2nd member
        roomsErrorCode = agrc.KickMember( &roomsParticipant1, &roomsParticipant2, "Don't like you too" );
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Unban member
        roomsErrorCode = agrc.UnbanMember( &roomsParticipant1, roomsParticipant2.GetName());
        N_assert(roomsErrorCode==REC_SUCCESS);

        // hide roomOutput
        roomsErrorCode = agrc.SetHiddenFromSearches( &roomsParticipant1, true );
        N_assert(roomsErrorCode==REC_SUCCESS);

        // Join by filter (should fail because hidden)
        roomsErrorCode = agrc.JoinByFilter( gameIdentifier1, RMM_ANY_PLAYABLE, &roomsParticipant2, (RoomID)-1, &roomQuery, &joinedRoomResult);
        N_assert(roomsErrorCode==REC_JOIN_BY_FILTER_NO_ROOMS);

    }
}
}