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
#ifndef __LOBBY_2_MESSAGE_STEAM_H
#define __LOBBY_2_MESSAGE_STEAM_H

#include "Lobby2Message.h"
#include "DS_Multilist.h"
#include "Lobby2Client_Steam.h"

namespace NII
{
namespace NII_NET
{
#define __L2_MSG_DB_HEADER(__NAME__,__DB__) \
	struct __NAME__##_##__DB__ : public __NAME__

	__L2_MSG_DB_HEADER(Client_Login, Steam)
	{
		virtual bool ClientImpl( Lobby2Plugin * client);

		virtual void DebugMsg(RakString & out) const
		{
			if(resultCode != L2RC_SUCCESS)
			{
				Client_Login::DebugMsg(out);
				return;
			}
			out.Set("Login success");
		}
	};

	__L2_MSG_DB_HEADER(Client_Logoff, Steam)
	{
		virtual bool ClientImpl(Lobby2Plugin * client);

		virtual void DebugMsg(RakString & out) const
		{
			if(resultCode != L2RC_SUCCESS)
			{
				Client_Logoff::DebugMsg(out);
				return;
			}
			out.Set("Logoff success");
		}
	};

	__L2_MSG_DB_HEADER(Console_SearchRooms, Steam)
	{
		Console_SearchRooms_Steam();
		virtual ~Console_SearchRooms_Steam();
		virtual bool ClientImpl( Lobby2Plugin *client);

		virtual void DebugMsg(RakString &out) const;

		// Output
		// Use CConsoleCommand_GetRoomDetails to get room names for unknown rooms, which will have blank names
		DataStructures::Multilist<ML_UNORDERED_LIST, RakString> roomNames;
		// Type of uint64_ts is uint64_t
		DataStructures::Multilist<ML_UNORDERED_LIST, uint64_t> roomIds;

		/// \internal
		// uint32_t is LobbyMatchList_t
		// CCallResult<Lobby2Client_Steam, uint32_t> m_SteamCallResultLobbyMatchList;
		void *m_SteamCallResultLobbyMatchList;
	};

	__L2_MSG_DB_HEADER(Console_GetRoomDetails, Steam)
	{
		virtual bool ClientImpl( Lobby2Plugin *client);

		virtual void DebugMsg(RakString &out) const
		{
			if (resultCode!=L2RC_SUCCESS)
			{
				Console_GetRoomDetails::DebugMsg(out);
				return;
			}
			out.Set("GetRoomDetails: roomName=%s for id %" N_64BIT_MOD "u", roomName.C_String(), roomId);
		}

		/// Input
		uint64_t roomId;

		/// Output
		RakString roomName;
	};

	__L2_MSG_DB_HEADER(Console_CreateRoom, Steam)
	{
		Console_CreateRoom_Steam();
		virtual ~Console_CreateRoom_Steam();

		virtual bool ClientImpl( Lobby2Plugin *client);

		virtual void DebugMsg(RakString &out) const
		{
			if (resultCode!=L2RC_SUCCESS)
			{
				Console_CreateRoom::DebugMsg(out);
				return;
			}
			out.Set("Console_CreateRoom: roomName %s created for id %" N_64BIT_MOD "u", roomName.C_String(), roomId);
		}

		/// Input
		/// If public, anyone can join. Else friends only
		bool roomIsPublic;
		RakString roomName;

		/// Output
		uint64_t roomId;

		/// \internal
		// CCallResult<Lobby2Client_Steam, LobbyCreated_t> m_SteamCallResultLobbyCreated;
		void *m_SteamCallResultLobbyCreated;
	};

	__L2_MSG_DB_HEADER(Console_JoinRoom, Steam)
	{
		Console_JoinRoom_Steam();
		virtual ~Console_JoinRoom_Steam();

		virtual bool ClientImpl( Lobby2Plugin *client);

		virtual void DebugMsg(RakString &out) const
		{
			if (resultCode!=L2RC_SUCCESS)
			{
				Console_JoinRoom::DebugMsg(out);
				return;
			}
			out.Set("Console_JoinRoom: Joined id %" N_64BIT_MOD "u", roomId);
		}

		/// Input
		uint64_t roomId;

		/// \internal
		//CCallResult<Lobby2Client_Steam, LobbyEnter_t> m_SteamCallResultLobbyEntered;
		void *m_SteamCallResultLobbyEntered;
	};

	__L2_MSG_DB_HEADER(Console_LeaveRoom, Steam)
	{
		virtual bool ClientImpl( Lobby2Plugin *client);

		virtual void DebugMsg(RakString &out) const
		{
			if (resultCode!=L2RC_SUCCESS)
			{
				Console_LeaveRoom::DebugMsg(out);
				return;
			}
			out.Set("Left room %" N_64BIT_MOD "u", roomId);
		}

		/// Input
		uint64_t roomId;
	};

	__L2_MSG_DB_HEADER(Console_SendRoomChatMessage, Steam)
	{
		virtual bool ClientImpl( Lobby2Plugin *client);

		virtual void DebugMsg(RakString &out) const
		{
			if (resultCode!=L2RC_SUCCESS)
			{
				Console_SendRoomChatMessage::DebugMsg(out);
				return;
			}
			out.Set("Sent %s to room %" N_64BIT_MOD "u", message.C_String(), roomId);
		}

		/// Input
		uint64_t roomId;
		RakString message;
	};


	__L2_MSG_DB_HEADER(Notification_Friends_StatusChange, Steam)
	{
		uint64_t friendId;
		RakString friendNewName;

		virtual void DebugMsg(RakString &out) const
		{
			if (resultCode!=L2RC_SUCCESS)
			{
				Notification_Friends_StatusChange::DebugMsg(out);
				return;
			}
			out.Set("Friend renamed to %s with ID %" N_64BIT_MOD "u", friendNewName.C_String(), friendId);
		}
	};

	__L2_MSG_DB_HEADER(Notification_Console_UpdateRoomParameters, Steam)
	{
		uint64_t roomId;
		RakString roomNewName;

		virtual void DebugMsg(RakString &out) const
		{
			if (resultCode!=L2RC_SUCCESS)
			{
				Notification_Console_UpdateRoomParameters::DebugMsg(out);
				return;
			}
			out.Set("RoomStateChanged: Room named %s with ID %" N_64BIT_MOD "u", roomNewName.C_String(), roomId);
		}
	};

	__L2_MSG_DB_HEADER(Notification_Console_MemberJoinedRoom, Steam)
	{
		uint64_t roomId;
		uint64_t srcMemberId;
		RakString memberName;
		Address * mRemote;

		virtual void DebugMsg(RakString &out) const
		{
			if (resultCode!=L2RC_SUCCESS)
			{
				Notification_Console_MemberJoinedRoom::DebugMsg(out);
				return;
			}
			out.Set("MemberJoinedRoom: Member named %s and ID %" N_64BIT_MOD "u joined room with ID %" N_64BIT_MOD "u", memberName.C_String(), srcMemberId, roomId);
		}
	};

	__L2_MSG_DB_HEADER(Notification_Console_MemberLeftRoom, Steam)
	{
		uint64_t roomId;
		uint64_t srcMemberId;
		RakString memberName;
		Address * mRemote;

		virtual void DebugMsg(RakString &out) const
		{
			if (resultCode!=L2RC_SUCCESS)
			{
				Notification_Console_MemberLeftRoom::DebugMsg(out);
				return;
			}
			out.Set("MemberLeftRoom: Member named %s and ID %" N_64BIT_MOD "u left room with ID %" N_64BIT_MOD "u", memberName.C_String(), srcMemberId, roomId);
		}
	};

	__L2_MSG_DB_HEADER(Notification_Console_RoomChatMessage, Steam)
	{
		RakString message;

		virtual void DebugMsg(RakString &out) const
		{
			if (resultCode!=L2RC_SUCCESS)
			{
				Notification_Console_RoomChatMessage::DebugMsg(out);
				return;
			}
			out=message;
		}
	};

	/*
	__L2_MSG_DB_HEADER(Notification_Console_RoomMemberConnectivityUpdate, Steam)
	{
		bool succeeded;
		Address * mRemote;

		virtual void DebugMsg(RakString & out) const
		{
            String temp;
			if(resultCode != L2RC_SUCCESS)
			{
				Notification_Console_RoomMemberConnectivityUpdate::DebugMsg(out);
				return;
			}
			if(succeeded)
			{
                mRemote.read(temp, true);
				out.Set("Signaling to %s succeeded.", temp);
			}
			else
			{
                mRemote.read(temp, true);
				out.Set("Signaling to %s failed.", temp);
			}
		}
	};
	*/

// --------------------------------------------- Database specific factory class for all messages --------------------------------------------

#define __L2_MSG_FACTORY_IMPL(__NAME__,__DB__) {case L2MID_##__NAME__ : Lobby2Message *m = OP_NEW< __NAME__##_##__DB__ >() ; return m;}

	struct Lobby2MessageFactory_Steam : public Lobby2MessageFactory
	{
		Lobby2MessageFactory_Steam() {}
		virtual ~Lobby2MessageFactory_Steam() {}
		virtual Lobby2Message *Alloc(Lobby2MessageID id)
		{
			switch (id)
			{
				__L2_MSG_FACTORY_IMPL(Client_Login, Steam);
				__L2_MSG_FACTORY_IMPL(Client_Logoff, Steam);
				__L2_MSG_FACTORY_IMPL(Console_SearchRooms, Steam);
				__L2_MSG_FACTORY_IMPL(Console_GetRoomDetails, Steam);
				__L2_MSG_FACTORY_IMPL(Console_CreateRoom, Steam);
				__L2_MSG_FACTORY_IMPL(Console_JoinRoom, Steam);
				__L2_MSG_FACTORY_IMPL(Console_LeaveRoom, Steam);
				__L2_MSG_FACTORY_IMPL(Console_SendRoomChatMessage, Steam);
				__L2_MSG_FACTORY_IMPL(Notification_Friends_StatusChange, Steam);
				__L2_MSG_FACTORY_IMPL(Notification_Console_UpdateRoomParameters, Steam);
				__L2_MSG_FACTORY_IMPL(Notification_Console_MemberJoinedRoom, Steam);
				__L2_MSG_FACTORY_IMPL(Notification_Console_MemberLeftRoom, Steam);
				__L2_MSG_FACTORY_IMPL(Notification_Console_RoomChatMessage, Steam);
				//__L2_MSG_FACTORY_IMPL(Notification_Console_RoomMemberConnectivityUpdate, Steam);

				default:

				return Lobby2MessageFactory::Alloc(id);
			};
		};
	};
}
}
#endif