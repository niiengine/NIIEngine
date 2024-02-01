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
#include "Lobby2Message_Steam.h"
#include "steam_api.h"
#include "Lobby2Client_Steam_Impl.h"
    
namespace NII
{
namespace NII_NET
{
    bool Client_Login_Steam::ClientImpl(Lobby2Plugin * client)
    {
        (void)client;

        if(!SteamAPI_Init())
            resultCode = L2RC_GENERAL_ERROR;
        else
            resultCode = L2RC_SUCCESS;
        return true; // Done immediately
    }
    bool Client_Logoff_Steam::ClientImpl(Lobby2Plugin * client)
    {
        Lobby2Client_Steam_Impl * steam = (Lobby2Client_Steam_Impl *)client;
        steam->NotifyLeaveRoom();

        resultCode=L2RC_SUCCESS;
        SteamAPI_Shutdown();

        return true; // Done immediately
    }
    Console_SearchRooms_Steam::Console_SearchRooms_Steam()
    {
        m_SteamCallResultLobbyMatchList = OP_NEW<CCallResult<Lobby2Client_Steam_Impl, LobbyMatchList_t> >();
    }

    Console_SearchRooms_Steam::~Console_SearchRooms_Steam()
    {
        // Cast to make sure destructor gets called
        OP_DELETE((CCallResult<Lobby2Client_Steam_Impl, LobbyMatchList_t>*)m_SteamCallResultLobbyMatchList);
    }

    bool Console_SearchRooms_Steam::ClientImpl( Lobby2Plugin *client)
    {
        (void) client;

        requestId = SteamMatchmaking()->RequestLobbyList();
        ((CCallResult<Lobby2Client_Steam_Impl, LobbyMatchList_t>*)m_SteamCallResultLobbyMatchList)->Set( requestId, (Lobby2Client_Steam_Impl*) client, &Lobby2Client_Steam_Impl::OnLobbyMatchListCallback );
        return false; // Asynch
    }

    void Console_SearchRooms_Steam::DebugMsg(RakString &out) const
    {
        if (resultCode!=L2RC_SUCCESS)
        {
            Console_SearchRooms::DebugMsg(out);
            return;
        }
        out.Set("%i rooms found", roomNames.GetSize());
        for (DataStructures::DefaultIndexType i=0; i < roomNames.GetSize(); i++)
        {
            out += RakString("\n%i. %s. ID=%" N_64BIT_MOD "u", i+1, roomNames[i].C_String(), roomIds[i]);
        }
    }

    bool Console_GetRoomDetails_Steam::ClientImpl(Lobby2Plugin * client)
    {
        (void) client;

        SteamMatchmaking()->RequestLobbyData( roomId );

        return false; // Asynch
    }

    Console_CreateRoom_Steam::Console_CreateRoom_Steam()
    {
        m_SteamCallResultLobbyCreated = OP_NEW<CCallResult<Lobby2Client_Steam_Impl, LobbyCreated_t> >();
    }

    Console_CreateRoom_Steam::~Console_CreateRoom_Steam()
    {
        // Cast to make sure destructor gets called
        OP_DELETE((CCallResult<Lobby2Client_Steam_Impl, LobbyCreated_t>*)m_SteamCallResultLobbyCreated);
    }

    bool Console_CreateRoom_Steam::ClientImpl(Lobby2Plugin * client)
    {
        if (roomIsPublic)
            requestId = SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, publicSlots);
        else
            requestId = SteamMatchmaking()->CreateLobby(k_ELobbyTypeFriendsOnly, publicSlots);

        // set the function to call when this completes
        ((CCallResult<Lobby2Client_Steam_Impl, LobbyCreated_t>*)m_SteamCallResultLobbyCreated)->Set(requestId, (Lobby2Client_Steam_Impl*) client, &Lobby2Client_Steam_Impl::OnLobbyCreated );

        return false; // Asynch
    }

    Console_JoinRoom_Steam::Console_JoinRoom_Steam()
    {
        m_SteamCallResultLobbyEntered = OP_NEW<CCallResult<Lobby2Client_Steam_Impl, LobbyEnter_t> > ();
    }

    Console_JoinRoom_Steam::~Console_JoinRoom_Steam()
    {
        // Cast to make sure destructor gets called
        OP_DELETE((CCallResult<Lobby2Client_Steam_Impl, LobbyEnter_t>*)m_SteamCallResultLobbyEntered);
    }

    bool Console_JoinRoom_Steam::ClientImpl( Lobby2Plugin *client)
    {
        requestId = SteamMatchmaking()->JoinLobby( roomId  );

        // set the function to call when this completes
        ((CCallResult<Lobby2Client_Steam_Impl, LobbyEnter_t>*)m_SteamCallResultLobbyEntered)->Set( requestId, (Lobby2Client_Steam_Impl*) client, &Lobby2Client_Steam_Impl::OnLobbyJoined );

        return false; // Asynch
    }

    bool Console_LeaveRoom_Steam::ClientImpl( Lobby2Plugin *client)
    {
        SteamMatchmaking()->LeaveLobby( roomId );

        Lobby2Client_Steam_Impl *steam = (Lobby2Client_Steam_Impl *)client;
        steam->NotifyLeaveRoom();

        resultCode=L2RC_SUCCESS;
        return true; // Synchronous
    }

    bool Console_SendRoomChatMessage_Steam::ClientImpl( Lobby2Plugin *client)
    {
        (void) client;

        SteamMatchmaking()->SendLobbyChatMsg(roomId, message.C_String(), (int) message.GetLength()+1);

        // ISteamMatchmaking.h
        /*
        // Broadcasts a chat message to the all the users in the lobby
        // users in the lobby (including the local user) will receive a LobbyChatMsg_t callback
        // returns true if the message is successfully sent
        // pvMsgBody can be binary or text data, up to 4k
        // if pvMsgBody is text, cubMsgBody should be strlen( text ) + 1, to include the null terminator
        virtual bool SendLobbyChatMsg( CSteamID steamIDLobby, const void *pvMsgBody, int cubMsgBody ) = 0;
        */

        resultCode=L2RC_SUCCESS;
        return true; // Synchronous
    }
}
}