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
#ifndef __LOBBY_2_CLIENT_STEAM_IMPL_H
#define __LOBBY_2_CLIENT_STEAM_IMPL_H

#include "steam_api.h"
#include "Lobby2Client_Steam.h"
#include "DS_Multilist.h"
#include "DS_OrderedList.h"

namespace NII
{
namespace NII_NET
{
    struct Lobby2Message;

    #define STEAM_UNUSED_PORT 1

    class RAK_DLL_EXPORT Lobby2Client_Steam_Impl : public Lobby2Client_Steam, public SocketLayerOverride
    {
    public:
        Lobby2Client_Steam_Impl();
        virtual ~Lobby2Client_Steam_Impl();

        /// Send a command to the server
        /// \param[in] msg The message that represents the command
        virtual void SendMsg(Lobby2Message * msg);

        // Base class implementation
        virtual void update();

        virtual void OnLobbyMatchListCallback( LobbyMatchList_t *pCallback, bool bIOFailure );
        virtual void OnLobbyCreated( LobbyCreated_t *pCallback, bool bIOFailure );
        virtual void OnLobbyJoined( LobbyEnter_t *pCallback, bool bIOFailure );
        virtual bool IsCommandRunning( Lobby2MessageID msgId );
        virtual void GetRoomMembers(DataStructures::OrderedList<uint64_t, uint64_t> &_roomMembers);
        virtual const char * GetRoomMemberName(uint64_t memberId);
        virtual bool IsRoomOwner(const uint64_t cSteamID);
        virtual bool IsInRoom(void) const;
        virtual uint64_t GetNumRoomMembers(const uint64_t roomid){return SteamMatchmaking()->GetNumLobbyMembers(roomid);}
        virtual uint64_t GetMyUserID(void){return SteamUser()->GetSteamID().ConvertToUint64();}
        virtual const char* GetMyUserPersonalName(void) {return SteamFriends()->GetPersonaName();}

        virtual void NotifyLeaveRoom(void);

        // Returns 0 if none
        virtual uint64_t GetRoomID(void) const {return roomId;}

        /// Called when SendTo would otherwise occur.
        virtual int RakNetSendTo(const Nui8 * data, NCount length,
            const Address & address);

        /// Called when RecvFrom would otherwise occur. Return number of bytes read. Write data into dataOut
        virtual int RakNetRecvFrom(char dataOut[N_Net_MaxMtuSize],
            Address *& senderOut, bool calledFromMainThread);

        virtual void shutdown();
        virtual void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason );
        virtual void onConnectFail(SocketMessage * msg, ConnectFailType failedConnectionAttemptReason);
        virtual void onAttach();
        virtual void onDetach();

        struct RoomMember
        {
            Address mAddress;
            uint64_t steamIDRemote;
        };
        static int SystemAddressAndRoomMemberComp( const Address &key, const RoomMember &data );
        static int SteamIDAndRoomMemberComp( const uint64_t &key, const RoomMember &data );
    protected:
        void CallCBWithResultCode(Lobby2Message *msg, Lobby2ResultCode rc);
        void PushDeferredCallback(Lobby2Message *msg);
        void CallRoomCallbacks(void);
        void NotifyNewMember(uint64_t memberId, Address * remote);
        void NotifyDroppedMember(uint64_t memberId, Address * remote);
        void CallCompletedCallbacks(void);

        STEAM_CALLBACK( Lobby2Client_Steam_Impl, OnLobbyDataUpdatedCallback, LobbyDataUpdate_t, m_CallbackLobbyDataUpdated );
        STEAM_CALLBACK( Lobby2Client_Steam_Impl, OnPersonaStateChange, PersonaStateChange_t, m_CallbackPersonaStateChange );
        STEAM_CALLBACK( Lobby2Client_Steam_Impl, OnLobbyDataUpdate, LobbyDataUpdate_t, m_CallbackLobbyDataUpdate );
        STEAM_CALLBACK( Lobby2Client_Steam_Impl, OnLobbyChatUpdate, LobbyChatUpdate_t, m_CallbackChatDataUpdate );
        STEAM_CALLBACK( Lobby2Client_Steam_Impl, OnLobbyChatMessage, LobbyChatMsg_t, m_CallbackChatMessageUpdate );

        STEAM_CALLBACK( Lobby2Client_Steam_Impl, OnP2PSessionRequest, P2PSessionRequest_t, m_CallbackP2PSessionRequest );
        STEAM_CALLBACK( Lobby2Client_Steam_Impl, OnP2PSessionConnectFail, P2PSessionConnectFail_t, m_CallbackP2PSessionConnectFail );

        DataStructures::Multilist<ML_UNORDERED_LIST, Lobby2Message *, uint64_t > deferredCallbacks;

        uint64_t roomId;
        DataStructures::OrderedList<Address *, RoomMember, SystemAddressAndRoomMemberComp> roomMembersByAddr;
        DataStructures::OrderedList<uint64_t, RoomMember, SteamIDAndRoomMemberComp> roomMembersById;
        DataStructures::Multilist<ML_ORDERED_LIST, uint64_t> rooms;

        void ClearRoom(void);

        uint32_t nextFreeSystemAddress;
    };
}
}
#endif