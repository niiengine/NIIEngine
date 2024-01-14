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


#ifndef __ROOMS_PLUGIN_H
#define __ROOMS_PLUGIN_H

#include "NiiNetCommon.h"
#include "NiiNetPlugin.h"
#include "DS_OrderedList.h"
#include "Export.h"
#include "RoomsContainer.h"
#include "PacketLevel.h"
#include "NiiNetSerializer.h"

/// \defgroup ROOMS_GROUP RoomsPlugin
/// \brief Networked implementation of a rooms system, where members join and leave rooms.
/// \details
/// \ingroup PLUGINS_GROUP

/// \defgroup ROOMS_COMMANDS RoomsCommands
/// \brief Commands that can be sent to RoomsPlugin
/// \details
/// \ingroup ROOMS_GROUP

/// \defgroup ROOMS_NOTIFICATIONS RoomsNotifications
/// \brief Notifications that RoomsPlugin sends to you
/// \details
/// \ingroup ROOMS_GROUP

namespace NII
{
namespace NII_NET
{
	class LinkBase;

    /// \brief Base class for rooms functionality
    /// \details Operations performed on rooms are not in the RoomsPlugin - instead, each structure encapsulates one operation
    /// \ingroup ROOMS_COMMANDS
    struct RoomsPluginFunc
    {
        RoomsPluginFunc() {}
        virtual ~RoomsPluginFunc() {}
        virtual void readIn(NetSerializer * out) = 0;
        virtual void writeIn(NetSerializer * in) = 0;
        virtual void readOut(NetSerializer * out) = 0;
        virtual void readIn(NetSerializer * in) = 0;
        void PrintResult(void);
        /// \param[in] userName Who is performing this operation. Input parameter
        RakString userName;

        /// \param[out] resultCode Result of this operation
        RoomsErrorCode resultCode;
    };
    /// \brief Create a room. Each user can be in at most one room, so will fail if the user is already in a room
    /// \ingroup ROOMS_COMMANDS
    struct CreateRoom_Func : public RoomsPluginFunc
    {
        // Input parameters
        NetworkedRoomCreationParameters networkedRoomCreationParameters;
        GameIdentifier gameIdentifier;
        // If initialRoomProperties is not empty, then SetCustomRoomProperties_Func will be run after the room is created successfully
        DataStructures::Table initialRoomProperties;

        // Output parameters
        RoomID roomId;
        RoomDescriptor roomDescriptor;

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Joins a room if possible. If not, creates a room.
    /// \ingroup ROOMS_COMMANDS
    struct EnterRoom_Func : public RoomsPluginFunc {

        // Input parameters
        NetworkedRoomCreationParameters networkedRoomCreationParameters;
        RoomMemberMode roomMemberMode;
        RoomQuery query;
        GameIdentifier gameIdentifier;

        // Output parameters
        JoinedRoomResult joinedRoomResult;
        bool createdRoom;
        RoomID roomId;

        void PrintResult(void)
        {
            if (resultCode!=REC_SUCCESS)
                printf("Result for user %s: %s\n", userName.C_String(), RoomsErrorCodeDescription::ToEnglish(resultCode));
            else if (createdRoom)
                printf("%s created a new room\n", userName.C_String());
            else
                printf("%s entered room with %i members\n", userName.C_String(), joinedRoomResult.roomDescriptor.roomMemberList.Size());
        }

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Joins a room given the filter parameters and desired room member mode.
    /// \details Room joins may fail if the room is locked, or does not have slots of the particular type
    /// Room joins may succeed where they would otherwise fail if the user has an invitation to the room
    /// \ingroup ROOMS_COMMANDS
    struct JoinByFilter_Func : public RoomsPluginFunc
    {
        // Input parameters
        GameIdentifier gameIdentifier;
        RoomMemberMode roomMemberMode;
        RoomQuery query;

        // Output parameters
        JoinedRoomResult joinedRoomResult;

        void PrintResult(void)
        {
            if(resultCode != REC_SUCCESS)
                printf("Result for user %s: %s\n", userName.C_String(), RoomsErrorCodeDescription::ToEnglish(resultCode));
            else
            {
                printf("Joined room %s with id %i and %.0f used slots\n",
                    joinedRoomResult.roomDescriptor.GetProperty(DefaultRoomColumns::TC_ROOM_NAME)->c, joinedRoomResult.roomDescriptor.lobbyRoomId,
                        joinedRoomResult.roomDescriptor.GetProperty(DefaultRoomColumns::TC_USED_SLOTS)->i);
                for(unsigned int i = 0; i < joinedRoomResult.roomDescriptor.roomMemberList.Size(); i++)
                {
                    String temp;
                    joinedRoomResult.roomDescriptor.roomMemberList[i].mAddress.read(temp);
                    printf("%i. %s (%s)\n", i + 1, joinedRoomResult.roomDescriptor.roomMemberList[i].name.C_String(), temp);
                }
            }
        }

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Leaves a room. You can leave at any time, even if the room is locked.
    /// \ingroup ROOMS_COMMANDS
    struct LeaveRoom_Func : public RoomsPluginFunc
    {
        // Input parameters
        // Output parameters
        RemoveUserResult removeUserResult;

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// Gets all invitations to you to various rooms.
    /// \ingroup ROOMS_COMMANDS
    struct GetInvitesToParticipant_Func : public RoomsPluginFunc
    {
        // Input parameters
        // Output parameters
        DataStructures::List<InvitedUser> invitedUsers;

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Sends an invitation to someone.
    /// \details Each user may have at most one invitation to the same room, although they have have invitations to multiple rooms.<BR>
    /// This may fail depending on the room settings - the moderator may not allow other users to send invitations.<BR>
    /// Invitations may be cleared when the moderator changes, depending on the room settings
    /// \ingroup ROOMS_COMMANDS
    struct SendInvite_Func : public RoomsPluginFunc
    {
        // Input parameters
        RakString inviteeName;
        bool inviteToSpectatorSlot;
        RakString subject;
        RakString body;
        // Output parameters

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Accept an invitation from a user to a room.
    /// \details Invitations are the only way to join reserved slots. If all reserved slots are full, will join a public slot if possible.
    /// \ingroup ROOMS_COMMANDS
    struct AcceptInvite_Func : public RoomsPluginFunc
    {
        // Input parameters
        RakString inviteSender;
        RoomID roomId;
        // Output parameters

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Begin spectating. Spectators are considered room members that are not playing the game, only watching
    /// \details Spectators do not count towards room ready states. The moderator can lock the room, preventing users from spectating, or not allow spectators at all
    /// \ingroup ROOMS_COMMANDS
    struct StartSpectating_Func : public RoomsPluginFunc
    {
        // Input parameters
        // Output parameters

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Stop spectating. This will rejoin the room as a player, using a reserved slot if we were invited, and a public slot if not or if there are no reserved slots
    /// \details This may fail if the moderator locked the room, or if no slots are available.
    /// \ingroup ROOMS_COMMANDS
    struct StopSpectating_Func : public RoomsPluginFunc
    {
        // Input parameters
        // Output parameters

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Give moderator to another player. Moderators cannot be spectators.
    /// \ingroup ROOMS_COMMANDS
    struct GrantModerator_Func : public RoomsPluginFunc
    {
        // Input parameters
        RakString newModerator;
        // Output parameters

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Change the allowed slot counts for the room. Setting fewer slot counts than the number of players does not kick out players, though it may prevent changing spectator status
    /// \ingroup ROOMS_COMMANDS
    struct ChangeSlotCounts_Func : public RoomsPluginFunc
    {
        // Input parameters
        Slots slots;
        // Output parameters

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Sets a table of user-defined room properties
    /// \details Fill out the table with the column name, type, and value.<BR>
    /// These properties are read when searching for rooms, and can be used as query filters to only join rooms with specified properties<BR>
    /// See RoomTypes.h for default room properties.
    /// \ingroup ROOMS_COMMANDS
    struct SetCustomRoomProperties_Func : public RoomsPluginFunc
    {
        // Input parameters
        DataStructures::Table table;
        // Output parameters

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Given a named room, return the properties of that room, including member list
    /// \ingroup ROOMS_COMMANDS
    struct GetRoomProperties_Func : public RoomsPluginFunc
    {
        // Input parameters
        // If blank, uses the room the user is currently in.
        RakString roomName;
        // Output parameters
        RoomDescriptor roomDescriptor;

        void PrintResult(void)
        {

            if (resultCode!=REC_SUCCESS)
            {
                printf("Result for user %s: %s\n", userName.C_String(), RoomsErrorCodeDescription::ToEnglish(resultCode));
            }
            else
            {
    //			char out[8096];
                printf("room %s has %i columns and %i used slots\n", roomName.C_String(),
                    roomDescriptor.roomProperties.GetColumnCount(), roomDescriptor.roomMemberList.Size());
                N_assert(roomDescriptor.GetProperty(DefaultRoomColumns::TC_USED_SLOTS)->i==roomDescriptor.roomMemberList.Size()-1);
        //		roomDescriptor.roomProperties.PrintColumnHeaders(out,8096,',');
        //		printf(out);
        //		roomDescriptor.roomProperties.PrintRow(out,8096,',',false,roomDescriptor.roomProperties.GetRowByIndex(0,0));
        //		printf(out);
            }
        }

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };

    /// \brief Change the name of the room
    /// \ingroup ROOMS_COMMANDS
    struct ChangeRoomName_Func : public RoomsPluginFunc {
        // Input parameters
        RakString newRoomName;
        // Output parameters

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Set or unset the room hidden from searches. If a room is hidden from searches, it can only be joined through invitations
    /// \ingroup ROOMS_COMMANDS
    struct SetHiddenFromSearches_Func : public RoomsPluginFunc
    {
        // Input parameters
        bool hiddenFromSearches;
        // Output parameters

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Sets or unsets to destroy the room when the moderator leaves the room
    /// \details If false, the next moderator will be the oldest member that is not a spectator.
    /// \ingroup ROOMS_COMMANDS
    struct SetDestroyOnModeratorLeave_Func : public RoomsPluginFunc
    {
        // Input parameters
        bool destroyOnModeratorLeave;
        // Output parameters

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Sets or unsets the user as flagged 'ready'
    /// \details Ready means ready to play the game. This flag can be set or unset by each room member.BR>
    /// When all players are ready, the can will presumably start (although you can use this flag however you want).
    /// \ingroup ROOMS_COMMANDS
    struct SetReadyStatus_Func : public RoomsPluginFunc {
        // Input parameters
        bool isReady;
        // Output parameters
        DataStructures::List<RakString> readyUsers;
        DataStructures::List<RakString> unreadyUsers;

        void PrintResult(void)
        {
            if (resultCode!=REC_SUCCESS)
            {
                printf("Result for user %s: %s\n", userName.C_String(), RoomsErrorCodeDescription::ToEnglish(resultCode));
            }
            else
            {
                printf("SetReadyStatus_Func member ready states:\n");
                if (readyUsers.Size()>0)
                {
                    printf("Ready: ");
                    for (unsigned int i=0; i < readyUsers.Size(); i++)
                    {
                        printf("%s ", readyUsers[i].C_String());
                    }

                    printf("\n");
                }

                if (unreadyUsers.Size()>0)
                {
                    printf("Unready: ");
                    for (unsigned int i=0; i < unreadyUsers.Size(); i++)
                    {
                        printf("%s ", unreadyUsers[i].C_String());
                    }

                    printf("\n");
                }

            }
        }

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Gets the ready states for every user in the room, excluding spectators
    /// \ingroup ROOMS_COMMANDS
    struct GetReadyStatus_Func : public RoomsPluginFunc {
        // Input parameters
        // Output parameters
        DataStructures::List<RakString> readyUsers;
        DataStructures::List<RakString> unreadyUsers;

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Lock or unlock the room
    /// \details If the room is locked, no further players can join regardless of the available room slots. This includes invited players.<BR>
    /// Rooms default to unlocked
    /// \ingroup ROOMS_COMMANDS
    struct SetRoomLockState_Func : public RoomsPluginFunc {
        // Input parameters
        RoomLockState roomLockState;
        // Output parameters

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Gets the lock state of the room
    /// \ingroup ROOMS_COMMANDS
    struct GetRoomLockState_Func : public RoomsPluginFunc {
        // Input parameters
        // Output parameters
        RoomLockState roomLockState;

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief If all members have been set to ready using SetReadyStatus_Func, this operation will set allReady to true.
    /// \ingroup ROOMS_COMMANDS
    struct AreAllMembersReady_Func : public RoomsPluginFunc {
        // Input parameters
        // Output parameters
        bool allReady;

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Kick a member out of the room. This will also automatically ban that member from rejoining as long as the moderator does not change, or the member is unbanned
    /// \ingroup ROOMS_COMMANDS
    struct KickMember_Func : public RoomsPluginFunc {
        // Input parameters
        RakString kickedMember;
        RakString reason;
        // Output parameters

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Allow a member previously kicked out of the room to rejoin.
    /// \ingroup ROOMS_COMMANDS
    struct UnbanMember_Func : public RoomsPluginFunc {
        // Input parameters
        RakString bannedMemberName;
        // Output parameters

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief For a given room, get the \a reason parameter of KickMember_Func when we were kicked out.
    /// \ingroup ROOMS_COMMANDS
    struct GetBanReason_Func : public RoomsPluginFunc
    {
        // Input parameters
        RoomID roomId;
        // Output parameters
        RakString reason;

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Enter quick join mode
    /// \details Quick join mode will wait until the specified timeout to try to automatically join a room each second.<BR>
    /// A room will only be joined if enough quick join members can join at once to fill all playable slots in the room.<BR>
    /// Older rooms are given priority to quick join.<BR>
    /// If no rooms are available to join, but enough quick join members are present to create a room instead, this will be done. The room custom properties will be any equality comparisons with networkedQuickJoinUser.query
    /// \ingroup ROOMS_COMMANDS
    struct AddUserToQuickJoin_Func : public RoomsPluginFunc
    {
        // Input parameters
        NetworkedQuickJoinUser networkedQuickJoinUser;
        GameIdentifier gameIdentifier;
        // Output parameters

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Leave quick join mode
    /// \ingroup ROOMS_COMMANDS
    struct RemoveUserFromQuickJoin_Func : public RoomsPluginFunc
    {
        // Input parameters
        // Output parameters
        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Returns if you are waiting in quick join mode
    /// \ingroup ROOMS_COMMANDS
    struct IsInQuickJoin_Func : public RoomsPluginFunc
    {
        // Input parameters
        // Output parameters
        bool isInQuickJoin;
        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Return all rooms that pass the \a roomQuery filter
    /// \details Use onlyJoinable to further filter by rooms that you can join (not locked, not banned, public or invited slots)
    /// \ingroup ROOMS_COMMANDS
    struct SearchByFilter_Func : public RoomsPluginFunc
    {
        SearchByFilter_Func() {}
        ~SearchByFilter_Func();
        // Input parameters
        GameIdentifier gameIdentifier;
        RoomQuery roomQuery;
        bool onlyJoinable;
        // Output parameters
        DataStructures::List<RoomDescriptor*> roomsOutput;

        void PrintResult(void)
        {
            if (resultCode!=REC_SUCCESS)
            {
                printf("Result for user %s: %s\n", userName.C_String(), RoomsErrorCodeDescription::ToEnglish(resultCode));
            }
            else
            {
                printf("Found %i rooms\n", roomsOutput.Size());
                for (unsigned int i=0; i < roomsOutput.Size(); i++)
                {
                    // Default types such as room name are in RoomTypes.cpp see defaultRoomColumns
                    // Also can be user defined
                    printf("%i. %s\n", i+1,roomsOutput[i]->GetProperty(DefaultRoomColumns::TC_ROOM_NAME)->c);
                }
            }
        }

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Change your handle
    /// \ingroup ROOMS_COMMANDS
    struct ChangeHandle_Func : public RoomsPluginFunc
    {
        // Input parameters
        RakString newHandle;
        // Output parameters

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Send a chat message
    /// \ingroup ROOMS_COMMANDS
    struct Chat_Func : public RoomsPluginFunc
    {
        // Input parameters
        RakString chatMessage;
        // Leave recipient blank for all in room
        RakString privateMessageRecipient;
        /// If true, only sends the chat message if the user is in the same room.
        /// If false, privateMessageRecipient must also be filled out
        bool chatDirectedToRoom;

        // Output parameters

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Send an arbitrary binary message
    /// \ingroup ROOMS_COMMANDS
    struct Bitstream_Func : public RoomsPluginFunc
    {
        // Input parameters
        NetSerializer bsToSend;
        // Leave recipient blank for all in room
        RakString privateMessageRecipient;
        /// If true, only sends the bitstream if the user is in the same room.
        /// If false, privateMessageRecipient must also be filled out
        bool directedToRoom;

        // Output parameters

        virtual void readIn(NetSerializer *out);
        virtual void writeIn(NetSerializer *in);
        virtual void readOut(NetSerializer *out);
        virtual void readIn(NetSerializer *in);
    };
    /// \brief Base class for notification callbacks
    /// \ingroup ROOMS_GROUP
    struct RoomsPluginNotification
    {
        virtual ~RoomsPluginNotification() {}
        virtual void write(NetSerializer * in)=0;
        virtual void read(NetSerializer * out)=0;
        virtual void PrintResult(void)=0;
        /// Always filled out so you know which user this callback applies to
        RakString recipient;
    };
    /// \brief The quick join duration has expired without joining or creating any rooms
    /// \ingroup ROOMS_NOTIFICATIONS
    struct QuickJoinExpired_Notification : public RoomsPluginNotification
    {
        NetworkedQuickJoinUser networkedQuickJoinUser;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("QuickJoinExpired_Notification to %s\n", recipient.C_String());}
    };
    /// \brief Quick join succeeded, and you are now in a room
    /// \ingroup ROOMS_NOTIFICATIONS
    struct QuickJoinEnteredRoom_Notification : public RoomsPluginNotification
    {
        JoinedRoomResult joinedRoomResult;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("QuickJoinEnteredRoom_Notification to %s. roomId=%i\n", recipient.C_String(), joinedRoomResult.roomDescriptor.lobbyRoomId);}
    };
    /// \brief Another room member has started spectating
    /// \ingroup ROOMS_NOTIFICATIONS
    struct RoomMemberStartedSpectating_Notification : public RoomsPluginNotification
    {
        RoomID roomId;
        RakString userName;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("RoomMemberStartedSpectating_Notification to %s\n", recipient.C_String());}
    };
    /// \brief Another room member has stopped spectating
    /// \ingroup ROOMS_NOTIFICATIONS
    struct RoomMemberStoppedSpectating_Notification : public RoomsPluginNotification
    {
        RakString userName;
        RoomID roomId;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("RoomMemberStoppedSpectating_Notification to %s\n", recipient.C_String());}
    };
    /// \brief The room has a new moderator (possibly you)
    /// \ingroup ROOMS_NOTIFICATIONS
    struct ModeratorChanged_Notification : public RoomsPluginNotification
    {
        RakString newModerator;
        RakString oldModerator;
        RoomID roomId;
        virtual void PrintResult(void) {printf("ModeratorChanged_Notification to %s\n", recipient.C_String());}
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
    };
    /// \brief The slot counts in the room has changed
    /// \ingroup ROOMS_NOTIFICATIONS
    struct SlotCountsSet_Notification : public RoomsPluginNotification
    {
        Slots slots;
        RoomID roomId;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("SlotCountsSet_Notification to %s\n", recipient.C_String());}
    };

    /// \brief The custom properties for the room has changed
    /// \ingroup ROOMS_NOTIFICATIONS
    struct CustomRoomPropertiesSet_Notification : public RoomsPluginNotification
    {
        RoomID roomId;
        // If tablePtr is set, that will be serialized. Otherwise, table will be serialized.
        // Deserialization is written to table
        DataStructures::Table * tablePtr;
        DataStructures::Table table;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("CustomRoomPropertiesSet_Notification to %s\n", recipient.C_String());}
    };
    /// \brief The name of the room has been changed
    /// \ingroup ROOMS_NOTIFICATIONS
    struct RoomNameSet_Notification : public RoomsPluginNotification
    {
        RoomID roomId;
        RakString oldName;
        RakString newName;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("RoomNameSet_Notification to %s\n", recipient.C_String());}
    };
    /// \brief The room is now hidden, or no longer hidden, from searches
    /// \ingroup ROOMS_NOTIFICATIONS
    struct HiddenFromSearchesSet_Notification : public RoomsPluginNotification
    {
        RoomID roomId;
        bool hiddenFromSearches;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("HiddenFromSearchesSet_Notification to %s\n", recipient.C_String());}
    };
    /// \brief Another room member has changed their ready status
    /// \ingroup ROOMS_NOTIFICATIONS
    struct RoomMemberReadyStatusSet_Notification : public RoomsPluginNotification
    {
        RoomID roomId;
        bool isReady;
        RakString roomMember;

        // Current status of all room members
        DataStructures::List<RakString> readyUsers;
        DataStructures::List<RakString> unreadyUsers;

        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("RoomMemberReadyStatusSet_Notification to %s\n", recipient.C_String());}
    };
    /// \brief The room is now, or is no longer, locked
    /// \ingroup ROOMS_NOTIFICATIONS
    struct RoomLockStateSet_Notification : public RoomsPluginNotification
    {
        RoomID roomId;
        RoomLockState roomLockState;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("RoomLockStateSet_Notification to %s\n", recipient.C_String());}
    };
    /// \brief A room member has been kicked out of the room (possibly you)
    /// \ingroup ROOMS_NOTIFICATIONS
    struct RoomMemberKicked_Notification : public RoomsPluginNotification
    {
        RoomID roomId;
        RakString kickedMember;
        RakString moderator;
        RakString reason;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("RoomMemberKicked_Notification to %s\n", recipient.C_String());}
    };
    /// \brief A room member has changed their handle
    /// \ingroup ROOMS_NOTIFICATIONS
    struct RoomMemberHandleSet_Notification : public RoomsPluginNotification
    {
        RoomID roomId;
        RakString oldName;
        RakString newName;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("RoomMemberHandleSet_Notification to %s\n", recipient.C_String());}
    };
    /// A room member has left the room
    /// \ingroup ROOMS_NOTIFICATIONS
    struct RoomMemberLeftRoom_Notification : public RoomsPluginNotification
    {
        RoomID roomId;
        RakString roomMember;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("RoomMemberLeftRoom_Notification to %s\n", recipient.C_String());}
    };
    /// \brief A room member has joined the room
    /// \ingroup ROOMS_NOTIFICATIONS
    struct RoomMemberJoinedRoom_Notification : public RoomsPluginNotification
    {
        RoomMemberJoinedRoom_Notification()
        {
            joinedRoomResult = 0;
        }
        ~RoomMemberJoinedRoom_Notification()
        {
            if (joinedRoomResult != 0)
                OP_DELETE(joinedRoomResult);
        }
        RoomID roomId;
        JoinedRoomResult * joinedRoomResult;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void)
        {
            String temp;
            joinedRoomResult->joiningMemberAddress.read(temp);
            printf("RoomMemberJoinedRoom_Notification to %s: %s (%s) has joined the room.\n",
                recipient.C_String(), joinedRoomResult->joiningMemberName.C_String(), temp);
        }
    };
    /// \brief You have received an invitation to a room
    /// \ingroup ROOMS_NOTIFICATIONS
    struct RoomInvitationSent_Notification : public RoomsPluginNotification
    {
        RakString invitorName;
        RakString inviteeName;
        bool inviteToSpectatorSlot;
        RakString subject;
        RakString body;
        RoomID roomId;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("RoomInvitationSent_Notification to %s\n", recipient.C_String());}
    };
    /// \brief A previous room invitation is no longer valid (possibly due to moderator change, or the room no longer exists)
    /// \ingroup ROOMS_NOTIFICATIONS
    struct RoomInvitationWithdrawn_Notification : public RoomsPluginNotification
    {
        InvitedUser invitedUser;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("RoomInvitationWithdrawn_Notification to %s\n", recipient.C_String());}
    };
    /// \brief The moderator has left the room, and everyone left is a spectator, or the room was set to be destroyed when the moderator left
    /// \ingroup ROOMS_NOTIFICATIONS
    /// You are no longer in the room
    struct RoomDestroyedOnModeratorLeft_Notification : public RoomsPluginNotification
    {
        RakString oldModerator;
        RoomID roomId;
        RoomDescriptor roomDescriptor;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("RoomDestroyedOnModeratorLeft_Notification to %s\n", recipient.C_String());}
    };
    /// \brief You got a chat message from another user.
    /// \details If you want to support ignoring chat messages from specific users, use Lobby2Client_PC::IsInIgnoreList
    /// \ingroup ROOMS_NOTIFICATIONS
    struct Chat_Notification : public RoomsPluginNotification
    {
        RakString sender;
        // If filled in, this was directed to you. Otherwise it was directed to the room
        RakString privateMessageRecipient;
        // The chat message
        RakString chatMessage;
        // The chat message with profanity filtered, if you want that instead
        RakString filteredChatMessage;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("Chat_Notification to %s\n", recipient.C_String());}
    };
    /// \brief You got a generic bitstream message from another user.
    /// \ingroup ROOMS_NOTIFICATIONS
    struct Bitstream_Notification : public RoomsPluginNotification
    {
        RakString sender;
        // If filled in, this was directed to you. Otherwise it was directed to the room
        RakString privateMessageRecipient;
        // The chat message
        NetSerializer bitStreamReceived;
        virtual void write(NetSerializer * in);
        virtual void read(NetSerializer * out);
        virtual void PrintResult(void) {printf("Bitstream_Notification to %s\n", recipient.C_String());}
    };
    /// \ingroup ROOMS_GROUP
    struct RoomsCallback
    {
        // Results of calls
        RoomsCallback() {}
        virtual ~RoomsCallback() {}
        virtual void CreateRoom_Callback( const Address &senderAddress, CreateRoom_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void EnterRoom_Callback( const Address &senderAddress, EnterRoom_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void JoinByFilter_Callback( const Address &senderAddress, JoinByFilter_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void LeaveRoom_Callback( const Address &senderAddress, LeaveRoom_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void GetInvitesToParticipant_Callback( const Address &senderAddress, GetInvitesToParticipant_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void SendInvite_Callback( const Address &senderAddress, SendInvite_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void AcceptInvite_Callback( const Address &senderAddress, AcceptInvite_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void StartSpectating_Callback( const Address &senderAddress, StartSpectating_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void StopSpectating_Callback( const Address &senderAddress, StopSpectating_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void GrantModerator_Callback( const Address &senderAddress, GrantModerator_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void ChangeSlotCounts_Callback( const Address &senderAddress, ChangeSlotCounts_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void SetCustomRoomProperties_Callback( const Address &senderAddress, SetCustomRoomProperties_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void GetRoomProperties_Callback( const Address &senderAddress, GetRoomProperties_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void ChangeRoomName_Callback( const Address &senderAddress, ChangeRoomName_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void SetHiddenFromSearches_Callback( const Address &senderAddress, SetHiddenFromSearches_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void SetDestroyOnModeratorLeave_Callback( const Address &senderAddress, SetDestroyOnModeratorLeave_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void SetReadyStatus_Callback( const Address &senderAddress, SetReadyStatus_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void GetReadyStatus_Callback( const Address &senderAddress, GetReadyStatus_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void SetRoomLockState_Callback( const Address &senderAddress, SetRoomLockState_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void GetRoomLockState_Callback( const Address &senderAddress, GetRoomLockState_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void AreAllMembersReady_Callback( const Address &senderAddress, AreAllMembersReady_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void KickMember_Callback( const Address &senderAddress, KickMember_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void UnbanMember_Callback( const Address &senderAddress, UnbanMember_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void GetBanReason_Callback( const Address &senderAddress, GetBanReason_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void AddUserToQuickJoin_Callback( const Address &senderAddress, AddUserToQuickJoin_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void RemoveUserFromQuickJoin_Callback( const Address &senderAddress, RemoveUserFromQuickJoin_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void IsInQuickJoin_Callback( const Address &senderAddress, IsInQuickJoin_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void SearchByFilter_Callback( const Address &senderAddress, SearchByFilter_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void ChangeHandle_Callback( const Address &senderAddress, ChangeHandle_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void Chat_Callback( const Address &senderAddress, Chat_Func *callResult) {(void) senderAddress; (void) callResult;}
        virtual void Bitstream_Callback( const Address &senderAddress, Bitstream_Func *callResult) {(void) senderAddress; (void) callResult;}
        // Notifications due to other room members
        virtual void QuickJoinExpired_Callback( const Address &senderAddress, QuickJoinExpired_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void QuickJoinEnteredRoom_Callback( const Address &senderAddress, QuickJoinEnteredRoom_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void RoomMemberStartedSpectating_Callback( const Address &senderAddress, RoomMemberStartedSpectating_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void RoomMemberStoppedSpectating_Callback( const Address &senderAddress, RoomMemberStoppedSpectating_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void ModeratorChanged_Callback( const Address &senderAddress, ModeratorChanged_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void SlotCountsSet_Callback( const Address &senderAddress, SlotCountsSet_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void CustomRoomPropertiesSet_Callback( const Address &senderAddress, CustomRoomPropertiesSet_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void RoomNameSet_Callback( const Address &senderAddress, RoomNameSet_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void HiddenFromSearchesSet_Callback( const Address &senderAddress, HiddenFromSearchesSet_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void RoomMemberReadyStatusSet_Callback( const Address &senderAddress, RoomMemberReadyStatusSet_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void RoomLockStateSet_Callback( const Address &senderAddress, RoomLockStateSet_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void RoomMemberKicked_Callback( const Address &senderAddress, RoomMemberKicked_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void RoomMemberHandleSet_Callback( const Address &senderAddress, RoomMemberHandleSet_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void RoomMemberLeftRoom_Callback( const Address &senderAddress, RoomMemberLeftRoom_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void RoomMemberJoinedRoom_Callback( const Address &senderAddress, RoomMemberJoinedRoom_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void RoomInvitationSent_Callback( const Address &senderAddress, RoomInvitationSent_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void RoomInvitationWithdrawn_Callback( const Address &senderAddress, RoomInvitationWithdrawn_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void RoomDestroyedOnModeratorLeft_Callback( const Address &senderAddress, RoomDestroyedOnModeratorLeft_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void Chat_Callback( const Address &senderAddress, Chat_Notification *notification) {(void) senderAddress; (void) notification;}
        virtual void Bitstream_Callback( const Address &senderAddress, Bitstream_Notification *notification) {(void) senderAddress; (void) notification;}
    };

    /// \brief Used to create rooms for players where they can matchmake
    /// \details A room is similar to the rooms you see in other lobby systems, where groups of players can join together in order to start a game match<BR>
    /// Each player can be in at most one room.<BR>
    /// Each player name must be unique.<BR>
    /// Each room has one moderator, which can perform operations on the room such as kicking out members.<BR>
    /// This plugin networks the AllGamesRoomsContainer class, which performs the actual functionality.<BR>
    /// <BR>
    /// To use as a client:<BR>
    /// <OL>
    /// <LI>Connect to the server and attach the plugin as normal.
    /// <LI>Call SetServerAddress to tell the system where the server is.
    /// <LI>Call RoomsPlugin::SetRoomsCallback() with a pointer to a callback structure
    /// <LI>Fill in the input parameters of the desired structure(s)
    /// <LI>Call RoomsPlugin::ExecuteFunc with a pointer to the structure.
    /// <LI>Process the callback, which will contain the original input parameters, plus the new output parameters. All structures contain resultCode, which indicates if the operation was successful (REC_SUCCESS) or not (Anything else)
    /// </OL>
    /// <BR>
    /// To use as a server:
    /// <OL>
    /// <LI>Start RakNet as usual, accepting connections and attaching the plugin
    /// <LI>Call RoomsPlugin::SetProfanityFilter() with the ProfanityFilter class, if desired
    /// <LI>Call RoomsPlugin::AddTitle() for each title (game) you want to support
    /// <LI>If you want other systems to be able to call RoomsPlugin::LoginRoomsParticipant(), call RoomsPlugin::AddLoginServerAddress() with the addresses of those systems
    /// <LI>As users go online, call RoomsPlugin::LoginRoomsParticipant(). Login and Logoff is up to you to implement (or rely on other systems, such as Lobby2)
    /// <LI>As users go offline, call RoomsPlugin::LogoffRoomsParticipant();
    /// </OL>
    /// \sa AllGamesRoomsContainer
    /// \sa TeamManager TeamManager performs some of the same functions as RoomsPlugin in a more flexible way
    /// \sa
    /// \ingroup ROOMS_GROUP
    class RAK_DLL_EXPORT RoomsPlugin : public Plugin, public RoomsCallback
    {
    public:
        RoomsPlugin();
        virtual ~RoomsPlugin();

        /// \brief Ordering channel to send messages on
        /// \param[in] oc The ordering channel
        void SetOrderingChannel(char oc);

        /// \brief Send priority to send messages on
        /// \param[in] plevel The msg priority
        void SetSendPriority(PacketLevel plevel);

        // -----------------------------------------------------------
        // Client room functions. Will transmit the command to the server specified with SetServerAddress();
        // -----------------------------------------------------------

        /// \brief Set the callback to get notification and ExecuteFunc() results
        /// \param[in] _roomsCallback Class instance to get callbacks on. Should remain a valid pointer for the duration of the plugin execution.
        void SetRoomsCallback(RoomsCallback * _roomsCallback);

        /// \brief Add a callback to get notification and ExecuteFunc() results
        /// \param[in] _roomsCallback Class instance to get callbacks on. Should remain a valid pointer for the duration of the plugin execution.
        void AddRoomsCallback(RoomsCallback * _roomsCallback);

        /// \brief Remove a callback to get notification and ExecuteFunc() results
        /// \param[in] _roomsCallback Class instance to no longer get callbacks on.
        void RemoveRoomsCallback(RoomsCallback * _roomsCallback);

        /// \brief Execute a function, using the system address passed to SetServerAddress();
        /// \param[in] func Pointer to a base class of RoomsPluginFunc to execute
        void ExecuteFunc(RoomsPluginFunc * func);

        /// \brief Execute a function, with a specific address
        /// \param[in] func Pointer to a base class of RoomsPluginFunc to execute
        /// \param[in] remoteAddress Address to send the command to. The remote system should also be running RoomsPlugin
        void ExecuteFunc(RoomsPluginFunc * func, Address * remoteAddress);

        /// \brief Sets the remote server address that is running RoomsPlugin. Send calls will go to this function
        /// \param[in] address The remote system, which should be connected while calling client functions
        void SetServerAddress(Address * address);

        // ----------------------------------------------------------
        // Server functions.
        // ----------------------------------------------------------

        /// \brief Add a participant to the system
        /// \details Only participants can perform operations
        /// \param[in] userName A unique string identifying the user
        /// \param[in] roomsParticipantAddress The address of the user
        /// \param[in] guid The guid of the user
        /// \param[in] loginServerAddress The server adding this user. Use Address::invalid for not applicable. Otherwise, the address must previously have been added using AddLoginServerAddress() or the function will fail.
        bool LoginRoomsParticipant(RakString userName, Address * roomsParticipantAddress, UniqueID guid, Address * loginServerAddress);

        /// \brief Removes a participant from the system
        /// \param[in] userName A unique string identifying the user
        /// \param[in] loginServerAddress The server removing. Use Address::invalid for not applicable. Otherwise, the address must previously have been added using AddLoginServerAddress() or the function will fail.
        bool LogoffRoomsParticipant(RakString userName, Address * loginServerAddress);

        /// \brief Clear all users
        void ClearRoomMembers();

        /// \brief Used for Lobby2. Serializes the same data that the plugin itself uses to login
        /// \internal
        static void SerializeLogin(RakString userName, Address * userAddress, UniqueID guid, NetSerializer *bs);

        /// \brief Used for Lobby2. Serializes the same data that the plugin itself uses to logoff
        /// \internal
        static void SerializeLogoff(RakString userName, NetSerializer * bs);

        /// \brief Used for Lobby2. Serializes the same data that the plugin itself uses to change handles
        /// \internal
        static void SerializeChangeHandle(RakString oldHandle, RakString newHandle, NetSerializer *bs);

        /// \brief Change the handle a user
        /// \param[in] oldHandle The handle previously known by the system
        /// \param[in] newHandle The new handle to use
        void ChangeHandle(RakString oldHandle, RakString newHandle);

        /// \brief Add a Address to a list that will be checked when LoginRoomsParticipant() and LogoffRoomsParticipant() is called
        /// \param[in] address The address to add
        void AddLoginServerAddress(Address * address);

        /// \brief Remove a Address from a list that will be checked when LoginRoomsParticipant() and LogoffRoomsParticipant() is called
        /// \param[in] address The address to remove
        void RemoveLoginServerAddress(Address * address);

        /// \brief Remove all addresses added with AddLoginServerAddress()
        void ClearLoginServerAdddresses(void);

        /// \brief Sets the profanity filter for the system to use (optional)
        /// \details If set, room names and player handles will be checked for profanity.
        /// Room invitations and other messages are not checked.
        /// \param[in] pf An instance of a profanity filter
        void SetProfanityFilter(ProfanityFilter * pf);

        /// \brief Only used on the server. Locally perform any desired functions, such as logging off players
        /// \note Use AllGamesRoomsContainer::AddTitle() to add titles
        AllGamesRoomsContainer roomsContainer;

        /// \internal
        virtual void onDetach();
        /// \internal
        virtual void shutdown();
        /// \internal
        virtual void update();
        /// \internal
        virtual ConnectReceiveType onMessage(SocketMessage * msg);
        /// \internal
        virtual void onConnectClose(const Address & address, UniqueID id,
            ConnectCloseType lostConnectionReason);
    protected:
        void Clear(void);
        bool IsServer(void) const;
        void OnRoomsExecuteFunc(SocketMessage * msg);
        void OnLoginStatus(SocketMessage * msg);
        void OnHandleChange(SocketMessage * msg);

        // Client and server data
        char mPacketCh;
        DataStructures::List<RoomsCallback *> roomsCallback;
        PacketLevel packetPriority;
        ProfanityFilter * profanityFilter;
        TimeDurMS lastUpdateTime;

        // Client data
        Address * serverAddress;

        // Server data
        // Servers that can remotely trigger LoginRoomMember and LogoffRoomMember
        DataStructures::List<Address *> loginServers;
        // Logged in room members
        struct RoomsPluginParticipant : public RoomsParticipant
        {
            RoomsPluginParticipant() {lastRoomJoined=(RoomID)-1;}
            RoomID lastRoomJoined;
        };
        static int RoomsPluginParticipantCompByRakString( const RakString &key, RoomsPluginParticipant* const &data );
        DataStructures::OrderedList<RakString, RoomsPluginParticipant*, RoomsPluginParticipantCompByRakString> roomsParticipants;

        RoomsPluginParticipant* GetParticipantByHandle(RakString handle, const Address &address);
        RoomsPluginParticipant* ValidateUserHandle(RoomsPluginFunc* func, const Address &address);
        void ProcessRemoveUserResult(RemoveUserResult *removeUserResult);

        virtual void CreateRoom_Callback( const Address &senderAddress, CreateRoom_Func *callResult);
        virtual void EnterRoom_Callback( const Address &senderAddress, EnterRoom_Func *callResult);
        virtual void JoinByFilter_Callback( const Address &senderAddress, JoinByFilter_Func *callResult);
        virtual void LeaveRoom_Callback( const Address &senderAddress, LeaveRoom_Func *callResult);
        virtual void GetInvitesToParticipant_Callback( const Address &senderAddress, GetInvitesToParticipant_Func *callResult);
        virtual void SendInvite_Callback( const Address &senderAddress, SendInvite_Func *callResult);
        virtual void AcceptInvite_Callback( const Address &senderAddress, AcceptInvite_Func *callResult);
        virtual void StartSpectating_Callback( const Address &senderAddress, StartSpectating_Func *callResult);
        virtual void StopSpectating_Callback( const Address &senderAddress, StopSpectating_Func *callResult);
        virtual void GrantModerator_Callback( const Address &senderAddress, GrantModerator_Func *callResult);
        virtual void ChangeSlotCounts_Callback( const Address &senderAddress, ChangeSlotCounts_Func *callResult);
        virtual void SetCustomRoomProperties_Callback( const Address &senderAddress, SetCustomRoomProperties_Func *callResult);
        virtual void GetRoomProperties_Callback( const Address &senderAddress, GetRoomProperties_Func *callResult);
        virtual void ChangeRoomName_Callback( const Address &senderAddress, ChangeRoomName_Func *callResult);
        virtual void SetHiddenFromSearches_Callback( const Address &senderAddress, SetHiddenFromSearches_Func *callResult);
        virtual void SetDestroyOnModeratorLeave_Callback( const Address &senderAddress, SetDestroyOnModeratorLeave_Func *callResult);
        virtual void SetReadyStatus_Callback( const Address &senderAddress, SetReadyStatus_Func *callResult);
        virtual void GetReadyStatus_Callback( const Address &senderAddress, GetReadyStatus_Func *callResult);
        virtual void SetRoomLockState_Callback( const Address &senderAddress, SetRoomLockState_Func *callResult);
        virtual void GetRoomLockState_Callback( const Address &senderAddress, GetRoomLockState_Func *callResult);
        virtual void AreAllMembersReady_Callback( const Address &senderAddress, AreAllMembersReady_Func *callResult);
        virtual void KickMember_Callback( const Address &senderAddress, KickMember_Func *callResult);
        virtual void UnbanMember_Callback( const Address &senderAddress, UnbanMember_Func *callResult);
        virtual void GetBanReason_Callback( const Address &senderAddress, GetBanReason_Func *callResult);
        virtual void AddUserToQuickJoin_Callback( const Address &senderAddress, AddUserToQuickJoin_Func *callResult);
        virtual void RemoveUserFromQuickJoin_Callback( const Address &senderAddress, RemoveUserFromQuickJoin_Func *callResult);
        virtual void IsInQuickJoin_Callback( const Address &senderAddress, IsInQuickJoin_Func *callResult);
        virtual void SearchByFilter_Callback( const Address &senderAddress, SearchByFilter_Func *callResult);
        virtual void ChangeHandle_Callback( const Address &senderAddress, ChangeHandle_Func *callResult);
        virtual void Chat_Callback( const Address &senderAddress, Chat_Func *callResult);
        virtual void Bitstream_Callback( const Address &senderAddress, Bitstream_Func *callResult);

        void ExecuteNotification(RoomsPluginNotification *func, RoomsPluginParticipant *recipient);
        void ExecuteNotificationToOtherRoomMembers(Room *room, RoomsPluginParticipant* roomsPluginParticipant, RoomsPluginNotification *notification);
    };

    /// \ingroup ROOMS_GROUP
    enum RoomsPluginOperations
    {
        RPO_CREATE_ROOM,
        RPO_ENTER_ROOM,
        RPO_JOIN_BY_FILTER,
        RPO_LEAVE_ROOM,
        RPO_GET_INVITES_TO_PARTICIPANT,
        RPO_SEND_INVITE,
        RPO_ACCEPT_INVITE,
        RPO_START_SPECTATING,
        RPO_STOP_SPECTATING,
        RPO_GRANT_MODERATOR,
        RPO_CHANGE_SLOT_COUNTS,
        RPO_SET_CUSTOM_ROOM_PROPERTIES,
        RPO_GET_ROOM_PROPERTIES,
        RPO_CHANGE_ROOM_NAME,
        RPO_SET_HIDDEN_FROM_SEARCHES,
        RPO_SET_DESTROY_ON_MODERATOR_LEAVE,
        RPO_SET_READY_STATUS,
        RPO_GET_READY_STATUS,
        RPO_SET_ROOM_LOCK_STATE,
        RPO_GET_ROOM_LOCK_STATE,
        RPO_ARE_ALL_MEMBERS_READY,
        RPO_KICK_MEMBER,
        RPO_UNBAN_MEMBER,
        RPO_GET_BAN_REASON,
        RPO_ADD_USER_TO_QUICK_JOIN,
        RPO_REMOVE_USER_FROM_QUICK_JOIN,
        RPO_IS_IN_QUICK_JOIN,
        RPO_SEARCH_BY_FILTER,
        RPO_CHANGE_HANDLE,
        RPO_CHAT,
        RPO_BITSTREAM,
        RPN_QUICK_JOIN_EXPIRED,
        RPN_QUICK_JOIN_ENTERED_ROOM,
        RPN_ROOM_MEMBER_STARTED_SPECTATING,
        RPN_ROOM_MEMBER_STOPPED_SPECTATING,
        RPN_MODERATOR_CHANGED,
        RPN_SLOT_COUNTS_SET,
        RPN_CUSTOM_ROOM_PROPERTIES_SET,
        RPN_ROOM_NAME_SET,
        RPN_HIDDEN_FROM_SEARCHES_SET,
        RPN_ROOM_MEMBER_READY_STATUS_SET,
        RPN_ROOM_LOCK_STATE_SET,
        RPN_ROOM_MEMBER_KICKED,
        RPN_ROOM_MEMBER_HANDLE_SET,
        RPN_ROOM_MEMBER_LEFT_ROOM,
        RPN_ROOM_MEMBER_JOINED_ROOM,
        RPN_ROOM_INVITATION_SENT,
        RPN_ROOM_INVITATION_WITHDRAWN,
        RPN_ROOM_DESTROYED_ON_MODERATOR_LEFT,
        RPN_CHAT_NOTIFICATION,
        RPN_BITSTREAM_NOTIFICATION,
    };
}
}
#endif