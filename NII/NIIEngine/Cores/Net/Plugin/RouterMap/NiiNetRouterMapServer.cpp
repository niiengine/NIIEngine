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
#include "NativeFeatureIncludes.h"
#if NII_RouterMapServer == 1

#include "NiiNetRouterMapServer.h"
#include "NiiNetSerializer.h"
#include "NiiNetCommon.h"
#include "NiiNetLinkBase.h"
#include "PacketLogger.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    void NatPunchthroughServerDebugInterface_Printf::OnServerMessage(const char * msg)
    {
        printf("%s\n", msg);
    }
    //------------------------------------------------------------------------
    #if _RAKNET_SUPPORT_PacketLogger==1
    void NatPunchthroughServerDebugInterface_PacketLogger::OnServerMessage(const char * msg)
    {
        if (pl)
        {
            pl->WriteMiscellaneous("Nat", msg);
        }
    }
    #endif
    //------------------------------------------------------------------------
    void RouterMapServer::User::DeleteConnectionAttempt(RouterMapServer::ConnectionAttempt *ca)
    {
        Nui32 index = connectionAttempts.GetIndexOf(ca);
        if ((Nui32)index!=(Nui32)-1)
        {
            N_delete ca;
            connectionAttempts.RemoveAtIndex(index);
        }
    }
    //------------------------------------------------------------------------
    void RouterMapServer::User::DerefConnectionAttempt(RouterMapServer::ConnectionAttempt *ca)
    {
        Nui32 index = connectionAttempts.GetIndexOf(ca);
        if ((Nui32)index!=(Nui32)-1)
        {
            connectionAttempts.RemoveAtIndex(index);
        }
    }
    //------------------------------------------------------------------------
    bool RouterMapServer::User::HasConnectionAttemptToUser(User *user)
    {
        Nui32 index;
        for (index=0; index < connectionAttempts.Size(); index++)
        {
            if (connectionAttempts[index]->recipient->mID==user->mID ||
                connectionAttempts[index]->sender->mID==user->mID)
                return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void RouterMapServer::User::LogConnectionAttempts(RakString &rs)
    {
        rs.Clear();
        Nui32 index;
        String guidStr, ipStr;
        mID.read(guidStr);
        mAddress.read(ipStr, true);
        rs = RakString("User address=%s id=%s\n", ipStr, guidStr);
        rs += RakString("%i attempts in list:\n", connectionAttempts.Size());
        for(index = 0; index < connectionAttempts.Size(); ++index)
        {
            rs += RakString("%i. SessionID=%i ", index + 1, connectionAttempts[index]->sessionId);
            if(connectionAttempts[index]->sender == this)
                rs+="(We are sender) ";
            else
                rs+="(We are recipient) ";
            if (isReady)
                rs+="(READY TO START) ";
            else
                rs+="(NOT READY TO START) ";
            if (connectionAttempts[index]->attemptPhase==RouterMapServer::ConnectionAttempt::NAT_ATTEMPT_PHASE_NOT_STARTED)
                rs+="(NOT_STARTED). ";
            else
                rs+="(GETTING_RECENT_PORTS). ";
            if (connectionAttempts[index]->sender==this)
            {
                connectionAttempts[index]->recipient->mID.read(guidStr);
                connectionAttempts[index]->recipient->mAddress.read(ipStr, true);
            }
            else
            {
                connectionAttempts[index]->sender->mID.read(guidStr);
                connectionAttempts[index]->sender->mAddress.read(ipStr, true);
            }

            rs+=RakString("Target address=%s, id=%s.\n", ipStr, guidStr);
        }
    }
    //------------------------------------------------------------------------
    int RouterMapServer::NatPunchthroughUserComp(const UniqueID & key,
        User * const & data)
    {
        if (key < data->mID)
            return -1;
        if (key > data->mID)
            return 1;
        return 0;
    }
    //------------------------------------------------------------------------
    RouterMapServer::RouterMapServer()
    {
        lastUpdate=0;
        sessionId=0;
        natPunchthroughServerDebugInterface=0;
        for(int i = 0; i < N_Socket_MaxLocalIP; ++i)
            boundAddresses[i] = 0;
        boundAddressCount = 0;
    }
    //------------------------------------------------------------------------
    RouterMapServer::~RouterMapServer()
    {
        User *user, *otherUser;
        ConnectionAttempt *connectionAttempt;
        Nui32 j;
        while(users.Size())
        {
            user = users[0];
            for (j=0; j < user->connectionAttempts.Size(); j++)
            {
                connectionAttempt=user->connectionAttempts[j];
                if (connectionAttempt->sender==user)
                    otherUser=connectionAttempt->recipient;
                else
                    otherUser=connectionAttempt->sender;
                otherUser->DeleteConnectionAttempt(connectionAttempt);
            }
            N_delete user;
            users[0]=users[users.Size()-1];
            users.RemoveAtIndex(users.Size()-1);
        }
    }
    //------------------------------------------------------------------------
    void RouterMapServer::SetDebugInterface(NatPunchthroughServerDebugInterface *i)
    {
        natPunchthroughServerDebugInterface=i;
    }
    //------------------------------------------------------------------------
    void RouterMapServer::update()
    {
        ConnectionAttempt *connectionAttempt;
        User *user, *recipient;
        Nui32 i,j;
        TimeDurMS time = N_Engine().getTimer().getMS();
        if (time > lastUpdate+250)
        {
            lastUpdate=time;

            for (i=0; i < users.Size(); i++)
            {
                user=users[i];
                for (j=0; j < user->connectionAttempts.Size(); j++)
                {
                    connectionAttempt=user->connectionAttempts[j];
                    if (connectionAttempt->sender==user)
                    {
                        if (connectionAttempt->attemptPhase!=ConnectionAttempt::NAT_ATTEMPT_PHASE_NOT_STARTED &&
                            time > connectionAttempt->startTime &&
                            time > 10000 + connectionAttempt->startTime ) // Formerly 5000, but sometimes false positives
                        {
                            NetSerializer out;

                            // that other system might not be running the plugin
                            out.write((Token)ID_NAT_TARGET_UNRESPONSIVE);
                            out.write(connectionAttempt->recipient->mID);
                            out.write(connectionAttempt->sessionId);
                            mPrc2->send(&out, PL_HIGH, PT_ORDER_MAKE_SURE,
                                0, connectionAttempt->sender->mAddress, false);

                            // 05/28/09 Previously only told sender about ID_NAT_CONNECTION_TO_TARGET_LOST
                            // However, recipient may be expecting it due to external code
                            // In that case, recipient would never get any response if the sender dropped

                            out.seekWrite(0);
                            out.seekRead(0);
                            out.write((Token)ID_NAT_TARGET_UNRESPONSIVE);
                            out.write(connectionAttempt->sender->mID);
                            out.write(connectionAttempt->sessionId);
                            mPrc2->send(&out, PL_HIGH, PT_ORDER_MAKE_SURE,
                                0, connectionAttempt->recipient->mAddress, false);

                            connectionAttempt->sender->isReady = true;
                            connectionAttempt->recipient->isReady = true;
                            recipient=connectionAttempt->recipient;


                            if(natPunchthroughServerDebugInterface)
                            {
                                char str[1024];
                                String addr1, addr2;
                                // 8/01/09 Fixed bug where this was after DeleteConnectionAttempt()
                                connectionAttempt->sender->mAddress.read(addr1, true);
                                connectionAttempt->recipient->mAddress.read(addr2, true);
                                sprintf(str, "Sending ID_NAT_TARGET_UNRESPONSIVE to sender %s and recipient %s.", addr1, addr2);
                                natPunchthroughServerDebugInterface->OnServerMessage(str);
                                RakString log;
                                connectionAttempt->sender->LogConnectionAttempts(log);
                                connectionAttempt->recipient->LogConnectionAttempts(log);
                            }


                            connectionAttempt->sender->DerefConnectionAttempt(connectionAttempt);
                            connectionAttempt->recipient->DeleteConnectionAttempt(connectionAttempt);

                            StartPunchthroughForUser(user);
                            StartPunchthroughForUser(recipient);

                            break;
                        }
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
    ConnectReceiveType RouterMapServer::onMessage(SocketMessage * msg)
    {
        switch (msg->mData[0])
        {
        case ID_NAT_PUNCHTHROUGH_REQUEST:
            OnNATPunchthroughRequest(msg);
            return CRT_Destroy;
        case ID_NAT_GET_MOST_RECENT_PORT:
            OnGetMostRecentPort(msg);
            return CRT_Destroy;
        case ID_NAT_CLIENT_READY:
            OnClientReady(msg);
            return CRT_Destroy;
        case ID_NAT_REQUEST_BOUND_ADDRESSES:
            {
                NetSerializer out;
                out.write((Token)ID_NAT_RESPOND_BOUND_ADDRESSES);

                if(boundAddresses[0] == 0 || boundAddresses[0]->isInvalid())
                {
                    DataStructures::List<SocketObj *> sockets;
                    mPrc2->getSockets(sockets);
                    for(NCount i = 0; i < sockets.Size() && i < N_Socket_MaxLocalIP; ++i)
                    {
                        boundAddresses[i] = sockets[i]->GetBoundAddress();
                        ++boundAddressCount;
                    }
                }

                out.write(boundAddressCount);
                for(NCount i = 0; i < boundAddressCount; ++i)
                {
                    out.write(boundAddresses[i]);
                }

                mPrc2->send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0, msg->mAddress, false);
            }
            return CRT_Destroy;
        case ID_NAT_PING:
            {
            }
            return CRT_Destroy;
        case ID_OUT_OF_BAND_INTERNAL:
            if(msg->mLength >= 2 && msg->mData[1] == ID_NAT_PING)
            {
                String temp;
                NetSerializer bs(msg->mData, msg->mLength, false);
                bs.skipRead(2, 0);
                Nui16 externalPort;
                bs.read(externalPort);

                NetSerializer out;
                out.write((Token)ID_NAT_PONG);
                out.write(externalPort);
                Nui16 externalPort2 = msg->mAddress.getHSBPort();
                out.write(externalPort2);
                msg->mAddress.read(temp, false);
                mPrc2->sendExpedite(temp, msg->mAddress.getHSBPort(), out.getData(), out.getSize());

                return CRT_Destroy;
            }
        }
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void RouterMapServer::onConnectClose(const Address & address, UniqueID id,
        ConnectCloseType lostConnectionReason)
    {
        (void) lostConnectionReason;
        (void) address;

        NCount i = 0;
        bool objectExists;
        i = users.GetIndexFromKey(id, &objectExists);
        if (objectExists)
        {
            NetSerializer out;
            DataStructures::List<User *> freedUpInProgressUsers;
            User * user = users[i];
            User * otherUser;
            Nui32 connectionAttemptIndex;
            ConnectionAttempt * connectionAttempt;
            for(connectionAttemptIndex = 0; connectionAttemptIndex < user->connectionAttempts.Size(); connectionAttemptIndex++)
            {
                connectionAttempt = user->connectionAttempts[connectionAttemptIndex];

                out.seekWrite(0);
                out.seekRead(0);
                if (connectionAttempt->recipient == user)
                {
                    otherUser = connectionAttempt->sender;
                }
                else
                {
                    otherUser = connectionAttempt->recipient;
                }

                // 05/28/09 Previously only told sender about ID_NAT_CONNECTION_TO_TARGET_LOST
                // However, recipient may be expecting it due to external code
                // In that case, recipient would never get any response if the sender dropped
                out.write((Token)ID_NAT_CONNECTION_TO_TARGET_LOST);
                out.write(id);
                out.write(connectionAttempt->sessionId);
                mPrc2->send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0,
                    otherUser->mAddress, false);

                // 4/22/09 - Bug: was checking inProgress, legacy variable not used elsewhere
                if (connectionAttempt->attemptPhase == ConnectionAttempt::NAT_ATTEMPT_PHASE_GETTING_RECENT_PORTS)
                {
                    otherUser->isReady = true;
                    freedUpInProgressUsers.Insert(otherUser);
                }

                otherUser->DeleteConnectionAttempt(connectionAttempt);
            }

            N_delete users[i];
            users.RemoveAtIndex(i);

            for (i = 0; i < freedUpInProgressUsers.Size(); ++i)
            {
                StartPunchthroughForUser(freedUpInProgressUsers[i]);
            }
        }
    }
    //------------------------------------------------------------------------
    void RouterMapServer::onConnect(const Address & address, UniqueID id)
    {
        User * user = N_new User();
        user->mID = id;
        user->mostRecentPort = 0;
        user->mAddress = &address;
        user->isReady = true;
        users.Insert(id, user, true);
    }
    //------------------------------------------------------------------------
    void RouterMapServer::OnNATPunchthroughRequest(SocketMessage * msg)
    {
        NetSerializer out;
        NetSerializer incomingBs(msg->mData, msg->mLength, false);
        incomingBs.skipRead(1, 0);
        UniqueID recipientGuid, senderGuid;
        incomingBs.read(recipientGuid);
        senderGuid = msg->mID;
        Nui32 i;
        bool objectExists;
        i = users.GetIndexFromKey(senderGuid, &objectExists);
        N_assert(objectExists);

        ConnectionAttempt * ca = N_new ConnectionAttempt();
        ca->sender = users[i];
        ca->sessionId = sessionId++;
        i = users.GetIndexFromKey(recipientGuid, &objectExists);
        if (objectExists == false || ca->sender == ca->recipient)
        {
            out.write((Token)ID_NAT_TARGET_NOT_CONNECTED);
            out.write(recipientGuid);
            mPrc2->send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0, msg->mAddress, false);
            N_delete ca;
            return;
        }
        ca->recipient=users[i];
        if (ca->recipient->HasConnectionAttemptToUser(ca->sender))
        {
            out.write((Token)ID_NAT_ALREADY_IN_PROGRESS);
            out.write(recipientGuid);
            mPrc2->send(&out,PL_HIGH,PT_ORDER_MAKE_SURE,0,msg->mAddress,false);
            N_delete ca;
            return;
        }

        ca->sender->connectionAttempts.Insert(ca);
        ca->recipient->connectionAttempts.Insert(ca);

        StartPunchthroughForUser(ca->sender);
    }
    //------------------------------------------------------------------------
    void RouterMapServer::OnClientReady(SocketMessage *msg)
    {
        Nui32 i;
        bool objectExists;
        i = users.GetIndexFromKey(msg->mID, &objectExists);
        if (objectExists)
        {
            users[i]->isReady=true;
            StartPunchthroughForUser(users[i]);
        }
    }
    //------------------------------------------------------------------------
    void RouterMapServer::OnGetMostRecentPort(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        Nui16 sessionId;
        Nui16 mostRecentPort;
        bsIn.read(sessionId);
        bsIn.read(mostRecentPort);

        Nui32 i,j;
        User *user;
        ConnectionAttempt *connectionAttempt;
        bool objectExists;
        i = users.GetIndexFromKey(msg->mID, &objectExists);

        if(natPunchthroughServerDebugInterface)
        {
            RakString log;
            String addr1[128], addr2[128];
            msg->mAddress.read(addr1, true);
            msg->mID.read(addr2);
            log = RakString("Got ID_NAT_GET_MOST_RECENT_PORT from address %s id %s. port=%i. sessionId=%i. userFound=%i.", addr1, addr2, mostRecentPort, sessionId, objectExists);
            natPunchthroughServerDebugInterface->OnServerMessage(log.C_String());
        }

        if(objectExists)
        {
            user = users[i];
            user->mostRecentPort = mostRecentPort;
            TimeDurMS time = N_Engine().getTimer().getMS();

            for(j = 0; j < user->connectionAttempts.Size(); ++j)
            {
                connectionAttempt=user->connectionAttempts[j];
                if(connectionAttempt->attemptPhase == ConnectionAttempt::NAT_ATTEMPT_PHASE_GETTING_RECENT_PORTS &&
                    connectionAttempt->sender->mostRecentPort != 0 &&
                    connectionAttempt->recipient->mostRecentPort != 0 &&
                    // 04/29/08 add sessionId to prevent processing for other systems
                    connectionAttempt->sessionId == sessionId)
                {
                    Address * senderSystemAddress = connectionAttempt->sender->mAddress;
                    Address * recipientSystemAddress = connectionAttempt->recipient->mAddress;
                    Address * recipientTargetAddress = recipientSystemAddress;
                    Address * senderTargetAddress = senderSystemAddress;
                    recipientTargetAddress->setHSBPort(connectionAttempt->recipient->mostRecentPort);
                    senderTargetAddress->setHSBPort(connectionAttempt->sender->mostRecentPort);

                    // Pick a time far enough in the future that both systems will have gotten the message
                    int targetPing = mPrc2->getAvgPing(recipientTargetAddress);
                    int senderPing = mPrc2->getAvgPing(senderSystemAddress);
                    TimeDurMS simultaneousAttemptTime;
                    if (targetPing==-1 || senderPing==-1)
                        simultaneousAttemptTime = time + 1500;
                    else
                    {
                        int largerPing = targetPing > senderPing ? targetPing : senderPing;
                        if(largerPing * 4 < 100)
                            simultaneousAttemptTime = time + 100;
                        else
                            simultaneousAttemptTime = time + (largerPing * 4);
                    }

                    if(natPunchthroughServerDebugInterface)
                    {
                        RakString log;
                        String addr1, addr2;
                        recipientSystemAddress->read(addr1, true);
                        connectionAttempt->recipient->mID.read(addr2);
                        log=RakString("Sending ID_NAT_CONNECT_AT_TIME to recipient address %s id %s", addr1, addr2);
                        natPunchthroughServerDebugInterface->OnServerMessage(log.C_String());
                    }

                    // send to recipient timestamped message to connect at time
                    NetSerializer out;
                    out.write((Token)ID_TIMESTAMP);
                    out.write(simultaneousAttemptTime);
                    out.write((Token)ID_NAT_CONNECT_AT_TIME);
                    out.write(connectionAttempt->sessionId);
                    out.write(senderTargetAddress); // Public IP, using most recent port
                    for(j = 0; j < N_Socket_MaxLocalIP; ++j) // Internal IP
                        out.write(mPrc2->getLAN(j, senderSystemAddress));
                    out.write(connectionAttempt->sender->mID);
                    out.write0();
                    mPrc2->send(&out,PL_HIGH,PT_ORDER_MAKE_SURE,0,recipientSystemAddress,false);

                    if(natPunchthroughServerDebugInterface)
                    {
                        RakString log;
                        String addr1, addr2;
                        senderSystemAddress->read(addr1, true);
                        connectionAttempt->sender->mID.read(addr2);
                        log=RakString("Sending ID_NAT_CONNECT_AT_TIME to sender address %s id %s", addr1, addr2);
                        natPunchthroughServerDebugInterface->OnServerMessage(log.C_String());
                    }

                    // Same for sender

                    out.seekWrite(0);
                    out.seekRead(0);
                    out.write((Token)ID_TIMESTAMP);
                    out.write(simultaneousAttemptTime);
                    out.write((Token)ID_NAT_CONNECT_AT_TIME);
                    out.write(connectionAttempt->sessionId);
                    out.write(recipientTargetAddress); // Public IP, using most recent port
                    for(j = 0; j < N_Socket_MaxLocalIP; ++j) // Internal IP
                        out.write(mPrc2->getLAN(j, recipientSystemAddress));
                    out.write(connectionAttempt->recipient->mID);
                    out.write1();
                    mPrc2->send(&out,PL_HIGH,PT_ORDER_MAKE_SURE,0,senderSystemAddress,false);

                    connectionAttempt->recipient->DerefConnectionAttempt(connectionAttempt);
                    connectionAttempt->sender->DeleteConnectionAttempt(connectionAttempt);

                    // 04/29/08 missing return
                    return;
                }
            }
        }
        else
        {

            if (natPunchthroughServerDebugInterface)
            {
                RakString log;
                String addr1, addr2;
                msg->mAddress.read(addr1, true);
                msg->mID.read(addr2);
                log=RakString("Ignoring ID_NAT_GET_MOST_RECENT_PORT from address %s id %s", addr1, addr2);
                natPunchthroughServerDebugInterface->OnServerMessage(log.C_String());
            }

        }
    }
    //------------------------------------------------------------------------
    void RouterMapServer::StartPunchthroughForUser(User *user)
    {
        if (user->isReady==false)
            return;

        ConnectionAttempt *connectionAttempt;
        User *sender,*recipient,*otherUser;
        Nui32 i;
        for (i=0; i < user->connectionAttempts.Size(); i++)
        {
            connectionAttempt=user->connectionAttempts[i];
            if (connectionAttempt->sender==user)
            {
                otherUser=connectionAttempt->recipient;
                sender=user;
                recipient=otherUser;
            }
            else
            {
                otherUser=connectionAttempt->sender;
                recipient=user;
                sender=otherUser;
            }

            if (otherUser->isReady)
            {
                if (natPunchthroughServerDebugInterface)
                {
                    char str[1024];
                    String addr1, addr2;
                    sender->mAddress.read(true, addr1);
                    recipient->mAddress.read(true, addr2);
                    sprintf(str, "Sending NAT_ATTEMPT_PHASE_GETTING_RECENT_PORTS to sender %s and recipient %s.", addr1, addr2);
                    natPunchthroughServerDebugInterface->OnServerMessage(str);
                }

                sender->isReady=false;
                recipient->isReady=false;
                connectionAttempt->attemptPhase = ConnectionAttempt::NAT_ATTEMPT_PHASE_GETTING_RECENT_PORTS;
                connectionAttempt->startTime = N_Engine().getTimer().getMS();

                sender->mostRecentPort = 0;
                recipient->mostRecentPort = 0;

                NetSerializer out;
                out.write((Token)ID_NAT_GET_MOST_RECENT_PORT);
                // 4/29/09 write sessionID so we don't use returned port for a system we don't want
                out.write(connectionAttempt->sessionId);
                mPrc2->send(&out,PL_HIGH,PT_ORDER_MAKE_SURE,0,sender->mAddress,false);
                mPrc2->send(&out,PL_HIGH,PT_ORDER_MAKE_SURE,0,recipient->mAddress,false);

                // 4/22/09 - BUG: missing break statement here
                break;
            }
        }
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*
