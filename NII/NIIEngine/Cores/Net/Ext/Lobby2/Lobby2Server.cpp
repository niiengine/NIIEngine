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
#include "Lobby2Server.h"
#include "N_assert.h"
#include "NiiNetCommon.h"

#ifdef __INTEGRATE_LOBBY2_WITH_ROOMS_PLUGIN
    #include "RoomsPlugin.h"
#endif

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    int Lobby2Server::UserCompByUsername(const RakString & key, Lobby2Server::User * const & data)
    {
        if (key < data->userName)
            return -1;
        if (key==data->userName)
            return 0;
        return 1;
    }
    //------------------------------------------------------------------------
    Lobby2Server::Lobby2Server()
    {
        DataStructures::OrderedList<Address *, Address *>::IMPLEMENT_DEFAULT_COMPARISON();
        DataStructures::OrderedList<RakString, RakString>::IMPLEMENT_DEFAULT_COMPARISON();
        roomsPlugin=0;
        roomsPluginAddress=0;
    }
    //------------------------------------------------------------------------
    Lobby2Server::~Lobby2Server()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void Lobby2Server::SendMsg(Lobby2Message * msg, const DataStructures::List<Address *> &recipients)
    {
        NetSerializer bs;
        bs.write((Token)ID_LOBBY2_SEND_MESSAGE);
        bs.write((Token)msg->GetID());
        msg->read(true, &bs);
        SendUnifiedToMultiple(&bs,packetPriority, RELIABLE_ORDERED, mPacketCh, recipients);
    }
    //------------------------------------------------------------------------
    void Lobby2Server::update()
    {
        while (threadActionQueue.Size())
        {
            threadActionQueueMutex.Lock();
            if (threadActionQueue.Size())
            {
                ThreadAction ta = threadActionQueue.Pop();
                threadActionQueueMutex.Unlock();
                if (ta.action == L2MID_Client_Logoff)
                {
                    OnLogoff(&ta.command, false);
                }
                else if (ta.action == L2MID_Client_Login)
                {
                    OnLogin(&ta.command, false);
                }
                else if (ta.action == L2MID_Client_ChangeHandle)
                {
                    OnChangeHandle(&ta.command, false);
                }
            }
            else
            {
                threadActionQueueMutex.Unlock();
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    ConnectReceiveType Lobby2Server::onMessage(SocketMessage * msg)
    {
        N_assert(msg);

        switch (msg->data[0])
        {
        case ID_LOBBY2_SEND_MESSAGE:
            OnMessage1(msg);
            return CRT_Destroy;
        }

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void Lobby2Server::shutdown()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void Lobby2Server::onConnectClose(const Address & address, UniqueID id, 
        ConnectCloseType lostConnectionReason)
    {
        (void)id;
        (void)lostConnectionReason;

        NCount index = GetUserIndexBySystemAddress(address);

        // If address is a user, then notify his friends about him logging off
        if (index!=-1)
        {
            bool found=false;
            User *user = users[index];
            for (NCount i=0; i < user->systemAddresses.Size(); ++i)
            {
                if (user->systemAddresses[i]==address)
                {
                    found=true;
                    user->systemAddresses.RemoveAtIndexFast(i);
                    break;
                }
            }

            if (found && user->systemAddresses.Size()==0)
            {
                // Log this logoff due to closed connection
                Lobby2Message *lobby2Message = msgFactory->Alloc(L2MID_Client_Logoff);
                Lobby2ServerCommand command;
                command.lobby2Message=lobby2Message;
                command.deallocMsgWhenDone=true;
                command.returnToSender=true;
                command.callerUserId=users[index]->callerUserId;
                command.server=this;
                ExecuteCommand(&command);

                RemoveUser(index);
            }
        }
    }
    //------------------------------------------------------------------------
    void Lobby2Server::OnMessage1(SocketMessage * msg)
    {
        NetSerializer in(msg->data, msg->length, false);
        in.skipRead(1, 0); // ID_LOBBY2_SEND_MESSAGE
        Token msgId;
        in.read(msgId);
        Lobby2MessageID lobby2MessageID = (Lobby2MessageID) msgId;
        NCount index;
        Lobby2Message * lobby2Message = msgFactory->Alloc(lobby2MessageID);
        if(lobby2Message)
        {
            lobby2Message->write(false, &in);
            Lobby2ServerCommand command;
            command.lobby2Message = lobby2Message;
            command.deallocMsgWhenDone = true;
            command.returnToSender = true;
            index = GetUserIndexBySystemAddress(msg->mAddress);
            if(index != -1)
            {
                command.callingUserName = users[index]->userName;
                command.callerUserId = users[index]->callerUserId;
            }
            else
            {
                if(lobby2Message->RequiresLogin())
                {
                    NetSerializer out;
                    out.write((Token)ID_LOBBY2_SEND_MESSAGE);
                    out.write((Token)lobby2Message->GetID());
                    lobby2Message->resultCode = L2RC_NOT_LOGGED_IN;
                    lobby2Message->read(true, &out);
                    send(&out, packetPriority, RELIABLE_ORDERED, mPacketCh, msg->mAddress, false);
                    return;
                }
                command.callerUserId=0;
            }
            command.callerSystemAddresses.Push(msg->mAddress);
            command.callerGuids.Push(msg->guid);
            command.server=this;
            ExecuteCommand(&command);
        }
        else
        {
            NetSerializer out;
            out.write((Token)ID_LOBBY2_SERVER_ERROR);
            out.write((unsigned char) L2SE_UNKNOWN_MESSAGE_ID);
            out.write((NCount) msgId);
            send(&out, packetPriority, RELIABLE_ORDERED, mPacketCh, msg->mAddress, false);
        }
    }
    //------------------------------------------------------------------------
    void Lobby2Server::Clear()
    {
        ClearAdminAddresses();
        ClearRankingAddresses();
        ClearUsers();
        ClearConnections();

        threadPool.shutdown();

        threadPool.removeAllInput();
        threadPool.removeAllOutput();

        threadActionQueueMutex.Lock();
        threadActionQueue.Clear();
        threadActionQueueMutex.Unlock();
    }
    //------------------------------------------------------------------------
    void Lobby2Server::AddAdminAddress(Address * addr)
    {
        adminAddresses.Insert(addr,addr,false);
    }
    //------------------------------------------------------------------------
    bool Lobby2Server::HasAdminAddress(const DataStructures::List<Address *> &addresses)
    {
        if (addresses.Size()==0)
            return true;

        NCount j;
        for (j=0; j < addresses.Size(); j++)
        {
            if (adminAddresses.HasData(addresses[j]))
                return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void Lobby2Server::RemoveAdminAddress(Address * addr)
    {
        adminAddresses.RemoveIfExists(addr);
    }
    //------------------------------------------------------------------------
    void Lobby2Server::ClearAdminAddresses(void)
    {
        adminAddresses.Clear(false);
    }
    //------------------------------------------------------------------------
    void Lobby2Server::AddRankingAddress(Address * addr)
    {
        rankingAddresses.Insert(addr, addr, false);
    }
    //------------------------------------------------------------------------
    bool Lobby2Server::HasRankingAddress(const DataStructures::List<Address *> & addresses)
    {
        if (addresses.Size()==0)
            return true;

        NCount j;
        for (j=0; j < addresses.Size(); ++j)
        {
            if (rankingAddresses.HasData(addresses[j]))
                return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void Lobby2Server::RemoveRankingAddress(Address * addr)
    {
        rankingAddresses.RemoveIfExists(addr);
    }
    //------------------------------------------------------------------------
    void Lobby2Server::ClearRankingAddresses(void)
    {
        rankingAddresses.Clear(false);
    }
    //------------------------------------------------------------------------
    void Lobby2Server::ExecuteCommand(Lobby2ServerCommand * command)
    {
        //NetSerializer out;
        if (command->lobby2Message->PrevalidateInput() == false)
        {
            SendMsg(command->lobby2Message, command->callerSystemAddresses);
            if (command->deallocMsgWhenDone)
                msgFactory->Dealloc(command->lobby2Message);
            return;
        }

        if (command->lobby2Message->RequiresAdmin() && HasAdminAddress(command->callerSystemAddresses)==false)
        {
            command->lobby2Message->resultCode=L2RC_REQUIRES_ADMIN;
            SendMsg(command->lobby2Message, command->callerSystemAddresses);
            if (command->deallocMsgWhenDone)
                msgFactory->Dealloc(command->lobby2Message);
            return;
        }

        if (command->lobby2Message->RequiresRankingPermission() && HasRankingAddress(command->callerSystemAddresses)==false)
        {
            command->lobby2Message->resultCode=L2RC_REQUIRES_ADMIN;
            SendMsg(command->lobby2Message, command->callerSystemAddresses);
            if (command->deallocMsgWhenDone)
                msgFactory->Dealloc(command->lobby2Message);
            return;
        }

        if (command->lobby2Message->ServerPreDBMemoryImpl(this, command->callingUserName)==true)
        {
            SendMsg(command->lobby2Message, command->callerSystemAddresses);
            if (command->deallocMsgWhenDone)
                msgFactory->Dealloc(command->lobby2Message);
            return;
        }

        command->server=this;
        AddInputCommand(*command);
    }
    //------------------------------------------------------------------------
    void Lobby2Server::SetRoomsPlugin(RoomsPlugin * rp)
    {
        roomsPlugin=rp;
        roomsPluginAddress=0;
    }
    //------------------------------------------------------------------------
    void Lobby2Server::SetRoomsPluginAddress(Address * address)
    {
        roomsPluginAddress = address;
        roomsPlugin = 0;
    }
    //------------------------------------------------------------------------
    void Lobby2Server::ClearUsers(void)
    {
        NCount i;
        for (i = 0; i < users.Size(); ++i)
            OP_DELETE(users[i]);
        users.Clear(false);
    }
    //------------------------------------------------------------------------
    void Lobby2Server::LogoffFromRooms(User * user)
    {
        // Remove from the room too
    #if defined(__INTEGRATE_LOBBY2_WITH_ROOMS_PLUGIN)
        // Tell the rooms plugin about the logoff event
        if (roomsPlugin)
        {
            roomsPlugin->LogoffRoomsParticipant(user->userName, 0);
        }
        else if (roomsPluginAddress && !roomsPluginAddress->isInvalid())
        {
            NetSerializer bs;
            RoomsPlugin::SerializeLogoff(user->userName,&bs);
            send(&bs,packetPriority, RELIABLE_ORDERED, mPacketCh, roomsPluginAddress, false);
        }
    #endif

    }
    //------------------------------------------------------------------------
    void Lobby2Server::SendRemoteLoginNotification(RakString handle, const DataStructures::List<Address *>& recipients)
    {
        Notification_Client_RemoteLogin notification;
        notification.handle = handle;
        notification.resultCode = L2RC_SUCCESS;
        SendMsg(&notification, recipients);
    }
    //------------------------------------------------------------------------
    void Lobby2Server::OnLogin(Lobby2ServerCommand *command, bool calledFromThread)
    {
        if (calledFromThread)
        {
            ThreadAction ta;
            ta.action=L2MID_Client_Login;
            ta.command=*command;
            threadActionQueueMutex.Lock();
            threadActionQueue.Push(ta);
            threadActionQueueMutex.Unlock();
            return;
        }

        bool objectExists;
        NCount insertionIndex = users.GetIndexFromKey(command->callingUserName, &objectExists);
        if (objectExists)
        {
            User * user = users[insertionIndex];
            if (user->allowMultipleLogins==false)
            {
                SendRemoteLoginNotification(user->userName, user->systemAddresses);
                LogoffFromRooms(user);

                // Already logged in from this system address.
                // Delete the existing entry, which will be reinserted.
                OP_DELETE(user);
                users.RemoveAtIndex(insertionIndex);
            }
            else
            {
                if (user->systemAddresses.GetIndexOf(command->callerSystemAddresses[0])==(NCount) -1)
                {
                    // Just add system address and guid already in use to the list for this user
                    user->systemAddresses.Push(command->callerSystemAddresses[0]);
                    user->guids.Push(command->callerGuids[0]);
                }
                return;
            }
        }
        else
        {
            // Different username, from the same IP address or RakNet instance
            NCount idx2 = GetUserIndexByGUID(command->callerGuids[0]);
            NCount idx3 = GetUserIndexBySystemAddress(command->callerSystemAddresses[0]);
            if (idx2!=(NCount) -1)
            {
                User * user = users[idx2];
                if (user->allowMultipleLogins==true)
                    return;
                SendRemoteLoginNotification(user->userName, user->systemAddresses);
                LogoffFromRooms(user);

                OP_DELETE(user);
                users.RemoveAtIndex(idx2);

                insertionIndex = users.GetIndexFromKey(command->callingUserName, &objectExists);
            }
            else if (idx3!=(NCount) -1)
            {
                User * user = users[idx3];
                if (user->allowMultipleLogins==true)
                    return;
                SendRemoteLoginNotification(user->userName, user->systemAddresses);
                LogoffFromRooms(user);

                OP_DELETE(user);
                users.RemoveAtIndex(idx3);

                insertionIndex = users.GetIndexFromKey(command->callingUserName, &objectExists);
            }
        }


        User *user = OP_NEW<User>();
        user->userName = command->callingUserName;
        user->systemAddresses = command->callerSystemAddresses;
        user->guids = command->callerGuids;
        user->callerUserId = command->callerUserId;
        user->allowMultipleLogins = ((Client_Login *)command->lobby2Message)->allowMultipleLogins;
        users.InsertAtIndex(user, insertionIndex);

    #if defined(__INTEGRATE_LOBBY2_WITH_ROOMS_PLUGIN)
        // Tell the rooms plugin about the login event
        if (roomsPlugin)
        {
            roomsPlugin->LoginRoomsParticipant(user->userName, user->systemAddresses[0], user->guids[0], 0);
        }
        else if (roomsPluginAddress && !roomsPluginAddress->isInvalid())
        {
            NetSerializer bs;
            RoomsPlugin::SerializeLogin(user->userName,user->systemAddresses[0], user->guids[0], &bs);
            send(&bs,packetPriority, RELIABLE_ORDERED, mPacketCh, roomsPluginAddress, false);
        }
    #endif
    }
    //------------------------------------------------------------------------
    void Lobby2Server::OnLogoff(Lobby2ServerCommand *command, bool calledFromThread)
    {
        if (calledFromThread)
        {
            ThreadAction ta;
            ta.action=L2MID_Client_Logoff;
            ta.command=*command;
            threadActionQueueMutex.Lock();
            threadActionQueue.Push(ta);
            threadActionQueueMutex.Unlock();
            return;
        }
        RemoveUser(command->callingUserName);
    }
    //------------------------------------------------------------------------
    void Lobby2Server::OnChangeHandle(Lobby2ServerCommand * command, bool calledFromThread)
    {
        if (calledFromThread)
        {
            ThreadAction ta;
            ta.action = L2MID_Client_ChangeHandle;
            ta.command = *command;
            threadActionQueueMutex.Lock();
            threadActionQueue.Push(ta);
            threadActionQueueMutex.Unlock();
            return;
        }

        NCount i;
        RakString oldHandle;
        for (i = 0; i < users.Size(); i++)
        {
            if (users[i]->callerUserId == command->callerUserId)
            {
                oldHandle = users[i]->userName;
                users[i]->userName = command->callingUserName;
                break;
            }
        }

        if (oldHandle.IsEmpty())
            return;

    #if defined(__INTEGRATE_LOBBY2_WITH_ROOMS_PLUGIN)
        // Tell the rooms plugin about the handle change
        if (roomsPlugin)
        {
            roomsPlugin->ChangeHandle(oldHandle, command->callingUserName);
        }
        else if (roomsPluginAddress && !roomsPluginAddress->isInvalid())
        {
            NetSerializer bs;
            RoomsPlugin::SerializeChangeHandle(oldHandle, command->callingUserName, &bs);
            send(&bs,packetPriority, RELIABLE_ORDERED, mPacketCh, roomsPluginAddress, false);
        }
    #endif
    }
    //------------------------------------------------------------------------
    void Lobby2Server::RemoveUser(RakString userName)
    {
        bool objectExists;
        NCount index = users.GetIndexFromKey(userName, &objectExists);
        if (objectExists)
            RemoveUser(index);
    }
    //------------------------------------------------------------------------
    void Lobby2Server::RemoveUser(NCount index)
    {
        User * user = users[index];

        Lobby2ServerCommand command;
        Notification_Friends_StatusChange * notification = (Notification_Friends_StatusChange *) GetMessageFactory()->Alloc(L2MID_Notification_Friends_StatusChange);
        notification->otherHandle = user->userName;
        notification->op = Notification_Friends_StatusChange::FRIEND_LOGGED_OFF;
        notification->resultCode = L2RC_SUCCESS;
        command.server = this;
        command.deallocMsgWhenDone = true;
        command.lobby2Message = notification;
        command.callerUserId = user->callerUserId;
        command.callingUserName = user->userName;
        ExecuteCommand(&command);

        Lobby2ServerJobIdentifier temp(user);
        threadPool.remove(temp);
        LogoffFromRooms(user);

        OP_DELETE(user);
        users.RemoveAtIndex(index);
    }
    //------------------------------------------------------------------------
    NCount Lobby2Server::GetUserIndexBySystemAddress(Address * address) const
    {
        NCount idx1,idx2;
        if (address)
            return (NCount)-1;
        for (idx1=0; idx1 < users.Size(); idx1++)
        {
            for (idx2=0; idx2 < users[idx1]->systemAddresses.Size(); idx2++)
            {
                if (users[idx1]->systemAddresses[idx2]==*address)
                    return idx1;
            }
        }
        return (NCount) -1;
    }
    //------------------------------------------------------------------------
    NCount Lobby2Server::GetUserIndexByGUID(UniqueID guid) const
    {
        NCount idx1,idx2;
        for(idx1 = 0; idx1 < users.Size(); ++idx1)
        {
            for(idx2 = 0; idx2 < users[idx1]->guids.Size(); ++idx2)
            {
                if (users[idx1]->guids[idx2]==guid)
                    return idx1;
            }
        }
        return (NCount) -1;
    }
    //------------------------------------------------------------------------
    NCount Lobby2Server::GetUserIndexByUsername(RakString userName) const
    {
        NCount idx;
        bool objectExists;
        idx = users.GetIndexFromKey(userName,&objectExists);
        if (objectExists)
            return idx;
        return (NCount) -1;
    }
    //------------------------------------------------------------------------
    void Lobby2Server::StopThreads(void)
    {
        threadPool.shutdown();
    }
    //------------------------------------------------------------------------
    void Lobby2Server::SetConfigurationProperties(ConfigurationProperties c)
    {
        configurationProperties=c;
    }
    //------------------------------------------------------------------------
    const Lobby2Server::ConfigurationProperties * Lobby2Server::GetConfigurationProperties(void) const
    {
        return &configurationProperties;
    }
    //------------------------------------------------------------------------
    void Lobby2Server::GetUserOnlineStatus(UsernameAndOnlineStatus & userInfo) const
    {
        NCount idx = GetUserIndexByUsername(userInfo.handle);
        if(idx != -1)
        {
            userInfo.isOnline = true;
            userInfo.presence = users[idx]->presence;
        }
        else
        {
            userInfo.isOnline = false;
            userInfo.presence.status = Lobby2Presence::NOT_ONLINE;
            userInfo.presence.isVisible = false;
        }
    }
    //------------------------------------------------------------------------
    void Lobby2Server::SetPresence(const Lobby2Presence & presence, RakString userHandle)
    {
        NCount index = GetUserIndexByUsername(userHandle);

        if (index!=-1)
        {
            User * user = users[index];
            user->presence=presence;

            // Push notify presence update to friends
            Lobby2ServerCommand command;
            Notification_Friends_PresenceUpdate *notification = (Notification_Friends_PresenceUpdate *) GetMessageFactory()->Alloc(L2MID_Notification_Friends_PresenceUpdate);
            notification->newPresence = presence;
            notification->otherHandle = user->userName;
            notification->resultCode = L2RC_SUCCESS;
            command.server = this;
            command.deallocMsgWhenDone = true;
            command.lobby2Message = notification;
            command.callerUserId = user->callerUserId;
            command.callingUserName = user->userName;
            ExecuteCommand(&command);
        }
    }
    //------------------------------------------------------------------------
    void Lobby2Server::GetPresence(Lobby2Presence & presence, RakString userHandle)
    {
        NCount userIndex = GetUserIndexByUsername(userHandle);
        if (userIndex != -1)
        {
            presence = users[userIndex]->presence;
        }
        else
        {
            presence.status = Lobby2Presence::NOT_ONLINE;
        }
    }
    //------------------------------------------------------------------------
    void Lobby2Server::SendUnifiedToMultiple( const NetSerializer * bitStream,
        PacketLevel plevel, PacketType ptype, char pchannel,
            const DataStructures::List<Address *> systemAddresses)
    {
        for (NCount i = 0; i < systemAddresses.Size(); i++)
            send(bitStream,plevel,ptype,pchannel,systemAddresses[i],false);
    }
    //------------------------------------------------------------------------
}
}