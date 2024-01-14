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

#define CAT_NEUTER_EXPORT /* Neuter dllimport for libcat */

#include "NiiPreProcess.h"
#include "NiiNetPlat.h"
#include "NiiThreadManager.h"
#include "NiiNetLink.h"
#include "NiiNetCommon.h"
#include "DS_HuffmanEncodingTree.h"
#include "NiiNetPacketPrc.h"
#include "NiiNetPlugin.h"
#include "StringTable.h"
#include "NetworkIDObject.h"
#include "NetworkIDManager.h"
#include "NiiSocketManager.h"

#ifdef CAT_AUDIT
	#define CAT_AUDIT_PRINTF(...) printf(__VA_ARGS__)
#else
	#define CAT_AUDIT_PRINTF(...)
#endif

#define REMOTE_SYSTEM_LOOKUP_HASH_MULTIPLE      8
#define Net_PING_TIME_SIZE                      5

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    bool AddressSort(const Address * v1, const Address * v2)
    {
        return *v1 < *v2;       //升序排列  
    }
    //------------------------------------------------------------------------
    struct RemoteHash
    {
        Nindex index;
        RemoteHash * next;
    };
    //------------------------------------------------------------------------
    struct PingTimeEquation
    {
        Nui16 mPingTime;
        TimeDurMS mTimeEquation;
    };
    //------------------------------------------------------------------------
    struct Remote
    {
        Nindex mIndex;                  ///<
        UniqueID mID;                   ///<
        SocketObj * mSocket;            ///<
        Address * mAddress;             ///< 自己地址
        Address * mHostAddress;         ///< 对方地址
        AddressList mLocalHost;         ///< 自己地址(包含内部地址)
        PacketPrc mPacketPrc;           ///< 包处理机制
        PingTimeEquation mTimeEquation[Net_PING_TIME_SIZE];
        TimeDurMS mNetTimeEquationIndex;
        Nui16 mLowPing;                 ///< The lowest ping value encountered
        TimeDurMS mNextPing;            ///< When to next ping this player
        TimeDurMS mLastSure;            ///<
        TimeDurMS mConnect;             ///< 最近连接
        NCount mMTU;                    ///< mtu
        NetworkSys::ConnectMode mMode;  ///<

        bool mActive;                   ///< Is this structure in use?
    #if LIBCAT_SECURITY == 1
        // Cached answer used internally by NetworkSys to prevent DoS attacks based on the connexion handshake
        char answer[cat::EasyHandshake::ANSWER_BYTES];

        // If the server has bRequireClientKey = true, then this is set to the validated public key of the connected client
        // Valid after mMode reaches CM_Process
        char client_public_key[cat::EasyHandshake::PUBLIC_KEY_BYTES];
    #endif
    };
    #define BCS_SEND                    1
    #define BCS_CLOSE_CONNECTION        2
    #define BCS_GET_SOCKET              3
    #define BCS_CHANGE_SYSTEM_ADDRESS   4/* BCS_USE_USER_SOCKET, BCS_REBIND_SOCKET_ADDRESS, BCS_RPC, BCS_RPC_SHIFT,*/
    #define BCS_DO_NOTHING              5 
    //------------------------------------------------------------------------
    struct OutMessage
    {
        Nui32 mReceipt;
        Nindex sindex;
        UniqueAddress mUniquePeer;
        NBitCount mBitCount;
        PacketLevel mPacketLevel;
        PacketType mPacketType;
        Remote::ConnectMode mMode;
        Nui8 * mData;
        SocketObj * mSocket;
        Nui8 mPacketCh;
        Nui8 mType;

        bool mBroadcast;
        bool blockingCommand; // Only used for RPC
        bool haveRakNetCloseSocket;
    };
    //------------------------------------------------------------------------
    struct ConnectRequest
    {
        Address * mAddress;
        TimeDurMS nextRequestTime;
        Nui8 requestsMade;
        Nui8 * mData;
        Nui16 dataLength;
        String outgoingPassword;
        Nui32 socketIndex;
        NCount mAttempt;
        TimeDurMS mAttemptInterval;
        TimeDurMS mTimeOut;
        PublicKeyMode publicKeyMode;
        SocketObj * mSocket;
        enum {CONNECT = 1, /*PING=2, PING_OPEN_CONNECTIONS=4,*/ /*ADVERTISE_SYSTEM=2*/} actionToTake;
#if LIBCAT_SECURITY == 1
        char handshakeChallenge[cat::EasyHandshake::CHALLENGE_BYTES];
        cat::ClientEasyHandshake * client_handshake;
        char remote_public_key[cat::EasyHandshake::PUBLIC_KEY_BYTES];
#endif
    };
    //------------------------------------------------------------------------
    static const int NUM_MTU_SIZES = 3;
    static const int mtuSizes[NUM_MTU_SIZES] = {N_Net_MaxMtuSize, 1200, 576};
    static RakNetRandom rnr;
    static const Nui32 MAX_OFFLINE_DATA_LENGTH = 400; // I set this because I limit ID_CONNECTION_REQUEST to 512 bytes, and the password is appended to that packet.
    //------------------------------------------------------------------------
    // Used to distinguish between offline messages with data, and messages from the packet process
    // Should be different than any message that could result from messages from the packet process
    #if  !defined(__GNUC__)
    #pragma warning(disable:4309) // 'initializing' : truncation of constant value
    #endif
    // Make sure highest bit is 0, so isValid in DatagramHeaderFormat is false
    static const Nui8 OFFLINE_MESSAGE_DATA_ID[16] =
    {
        0x00,0xFF,0xFF,0x00,
        0xFE,0xFE,0xFE,0xFE,
        0xFD,0xFD,0xFD,0xFD,
        0x12,0x34,0x56,0x78
    };
    //------------------------------------------------------------------------
    NetworkSys::NetworkSys():
        mThread(0)
    {
        StringCompressor::AddReference();
        StringTable::AddReference();
        N_Only(Socket).init();

        mMTUSize = mtuSizes[NUM_MTU_SIZES - 1];

        mMaxRemoteCount = 0;
        mRemoteHashList = 0;

        mStop = true;
        mRun = false;
    //#if defined(GET_TIME_SPIKE_LIMIT) && GET_TIME_SPIKE_LIMIT > 0
    //    mPingTest = true;
    //#else
        mPingTest = false;
   // #endif

        mAlterIP = false;
        mSplitMessageFactor = 0;
        mMessageTimeOut = 1000;
        mSendSize = 0;
        mLocalHostID = UniqueID::INVALID;
    #ifdef _DEBUG
        // Wait longer to disconnect in debug so I don't get disconnected while tracing
        mTimeOut = 30000;
    #else
        mTimeOut = 10000;
    #endif
        mQuerySocketList.SetPageSize(sizeof(SocketList) * 8);
        mLocalHostID.mGuid = UniqueID::genLocalGuid();
        mInputEvent = new ThreadEvent();
        mConnectTimeLimit = false;
        mNextReceiptMutex.lock();
        mNextReceipt = 1;
        mNextReceiptMutex.unlock();
    }
    //------------------------------------------------------------------------
    NetworkSys::~NetworkSys()
    {
        shutdown(0, 0);
        mBan->removeAll();
        StringCompressor::RemoveReference();
        StringTable::RemoveReference();
        N_Only(Socket).destroy();
        delete mInputEvent;
        AddressList::iterator i, iend = mLocalHost.end();
        for(i = mLocalHost.begin(); i != iend; ++i)
        {
            if(*i != 0)
                N_delete *i;
        }
        mLocalHost.clear();
        if(mThread)
        {
            delete mThread;
            mThread = 0;
        }
    }
    //------------------------------------------------------------------------
    void NetworkSys::run()
    {
        NetSerializer upload(N_Net_MaxMtuSize
    #if LIBCAT_SECURITY == 1
            + cat::AuthenticatedEncryption::OVERHEAD_BYTES
    #endif
        );
        mRun = true;

        while(!mStop)
        {
            Remote * remote;
            SocketMessage * msg;

            NBitCount bitcount;
            Nui32 byteSize;
            Nui8 * data;
            Address address;
            OutMessage * bcs;
            bool callerDataAllocationUsed;
            NetStats * rnss;
            TimeDurUS timeNS = 0;
            TimeDurMS timeMS = 0;

            // This is here so RecvFromBlocking actually gets data from the same thread
    #if N_PLAT == N_PLAT_WINRT
    #elif N_PLAT == N_PLAT_WIN32
            if(mSocketList[0]->GetSocketType() == RNS2T_WINDOWS &&
                ((RNS2_Windows *)mSocketList[0])->GetSocketLayerOverride())
            {
                int len;
                Address * sender = 0;
                char dataOut[N_Net_MaxMtuSize];
                do
                {
                    len = ((RNS2_Windows *)mSocketList[0])->GetSocketLayerOverride()->RakNetRecvFrom(dataOut, sender, true);
                    if(len > 0)
                        process(sender, dataOut, len, this, mSocketList[0], N_Engine().getTimer().getUS(), upload);
                } while(len > 0);
            }
    #endif
            SocketMessageData * recv;
            while(1)
            {
                mReveiceMessageMute.lock();
                if(mReveiceDataList.Size() > 0)
                {
                    recv = mReveiceDataList.Pop();
                    mReveiceMessageMute.unlock();
                }
                else
                {
                    recv = NULL;
                }
                mReveiceMessageMute.unlock();

                if(recv)
                {
                    process(recv->mAddress, recv->mData, recv->mCount, this, recv->mSocket, recv->mRead, upload);
                    dealloc(recv);
                }
                else
                    break;
            }

            while((bcs = mSendList.PopInaccurate()) != 0)
            {
                if(bcs->mType == BCS_SEND)
                {
                    // N_Engine().getTimer().getMS is a very slow call so do it once and as late as possible
                    if(timeNS == 0)
                    {
                        timeNS = N_Engine().getTimer().getUS();
                        timeMS = (TimeDurMS)(timeNS/(TimeDurUS)1000);
                    }

                    callerDataAllocationUsed = sendRapid(bcs->mData, bcs->mBitCount, bcs->mPacketLevel,
                        bcs->mPacketType, bcs->mPacketCh, bcs->mUniquePeer, bcs->mBroadcast, true, timeNS, bcs->mReceipt);

                    if(callerDataAllocationUsed == false)
                        N_free(bcs->mData);

                    // Set the new connection state AFTER we call sendImmediate in case we are setting it to a disconnection state, which does not allow further sends
                    if(bcs->mMode != CM_None)
                    {
                        remote = getRemoteSystem(bcs->mUniquePeer, true, true);
                        if(remote)
                            remote->mMode = bcs->mMode;
                    }
                }
                else if(bcs->mType == BCS_CLOSE_CONNECTION)
                {
                    closeImpl(bcs->mUniquePeer, false, true, bcs->mPacketCh, bcs->mPacketLevel);
                }
                else if(bcs->mType == BCS_CHANGE_SYSTEM_ADDRESS)
                {
                    // Reroute
                    Remote * rssFromGuid = getRemoteSystem(bcs->mUniquePeer.mID, true, true);
                    if(rssFromGuid != 0)
                    {
                        Nui32 existingSystemIndex = getIndexImpl(rssFromGuid->mAddress);
                        attach(bcs->mUniquePeer.mAddress, existingSystemIndex);
                    }
                }
                else if(bcs->mType == BCS_GET_SOCKET)
                {
                    SocketList * sqo;
                    if(bcs->mUniquePeer.isValid())
                    {
                        remote = getRemoteSystem(bcs->mUniquePeer, true, true);
                        sqo = mQuerySocketList.Allocate();

                        sqo.Clear(false);
                        if(remote)
                        {
                            sqo.Push(remote->mSocket);
                        }
                        mQuerySocketList.Push(sqo);
                    }
                    else
                    {
                        sqo = mQuerySocketList.Allocate();
                        sqo = mSocketList;
                        mQuerySocketList.Push(sqo);
                    }
                }

        #ifdef _DEBUG
                bcs->mData = 0;
        #endif
                mSendList.Deallocate(bcs);
            }

            if(mRequestConnectList.IsEmpty() == false)
            {
                if(timeNS == 0)
                {
                    timeNS = N_Engine().getTimer().getUS();
                    timeMS = (TimeDurMS)(timeNS / 1000);
                }

                bool condition1, condition2;
                Nui32 requestedConnectionQueueIndex = 0;
                mRequestConnectMutex.lock();
                while(requestedConnectionQueueIndex < mRequestConnectList.Size())
                {
                    ConnectRequest * cr = mRequestConnectList[requestedConnectionQueueIndex];
                    mRequestConnectMutex.unlock();
                    if(cr->nextRequestTime < timeMS)
                    {
                        condition1 = cr->requestsMade == cr->mAttempt + 1;
                        condition2 = cr->mAddress == 0 || cr->mAddress->isInvalid();
                        // If too many requests made or a hole then remove this if possible, otherwise invalidate it
                        if(condition1 || condition2)
                        {
                            if(cr->mData)
                            {
                                N_free(cr->mData);
                                cr->mData = 0;
                            }

                            if(condition1 && !condition2 && cr->actionToTake == ConnectRequest::CONNECT)
                            {
                                // Tell user of connection attempt failed
                                msg = N_new SocketMessage(sizeof(char));
                                msg->mData[0] = ID_CONNECTION_ATTEMPT_FAILED; // Attempted a connection and couldn't
                                msg->mBitCount = (sizeof(char) * 8);
                                msg->mAddress = cr->mAddress;

                                mReceiveMute.lock();
                                mReceiveList.Push(msg);
                                mReceiveMute.unlock();
                            }

        #if LIBCAT_SECURITY == 1
                            CAT_AUDIT_PRINTF("AUDIT: Connection attempt FAILED so deleting cr->client_handshake object %x\n", cr->client_handshake);
                            N_delete cr->client_handshake;
        #endif
                            N_delete cr;

                            mRequestConnectMutex.lock();
                            for(Nui32 k = 0; k < mRequestConnectList.Size(); ++k)
                            {
                                if(mRequestConnectList[k] == cr)
                                {
                                    mRequestConnectList.RemoveAtIndex(k);
                                    break;
                                }
                            }
                            mRequestConnectMutex.unlock();
                        }
                        else
                        {
                            int MTUSizeIndex = cr->requestsMade / (cr->mAttempt / NUM_MTU_SIZES);
                            if(MTUSizeIndex >= NUM_MTU_SIZES)
                                MTUSizeIndex = NUM_MTU_SIZES - 1;
                            ++cr->requestsMade;
                            cr->nextRequestTime = timeMS + cr->mAttemptInterval;

                            NetSerializer out;
                            out.write((Token)ID_OPEN_CONNECTION_REQUEST_1);
                            out.write((const Nui8 *)OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                            out.write((Token)N_NET_PROTOCOL_VER);
                            out.writeAlign(mtuSizes[MTUSizeIndex] - UDP_HEADER_SIZE);

                            //for(NCount i = 0; i < mRawPluginList.Size(); ++i)
                            //    mRawPluginList[i]->onRawSend(out.getData(), out.getWriteOffset(), cr->mAddress);

                            SocketObj * socketToUse;
                            if(cr->mSocket == 0)
                                socketToUse = mSocketList[cr->socketIndex];
                            else
                                socketToUse = cr->mSocket;

        #if (N_PLAT != N_PLAT_NACL) && (N_PLAT != N_PLAT_WINRT)
                            if(socketToUse->IsBerkleySocket())
                                ((RNS2_Berkley *)socketToUse)->SetDoNotFragment(1);
        #endif
                            TimeDurMS sendToStart = N_Engine().getTimer().getMS();

                            SocketSendData bsp;
                            bsp.mData = out.getData();
                            bsp.mLength = out.getSize();
                            bsp.mAddress = cr->mAddress;
                            if(socketToUse->send(&bsp) == 10040)
                            {
                                // Don't use this MTU size again
                                cr->requestsMade = (Nui8)((MTUSizeIndex + 1) * (cr->mAttempt / NUM_MTU_SIZES));
                                cr->nextRequestTime = timeMS;
                            }
                            else
                            {
                                TimeDurMS sendToEnd = N_Engine().getTimer().getMS();
                                if(sendToEnd - sendToStart > 100)
                                {
                                    // Drop to lowest MTU
                                    int lowestMtuIndex = cr->mAttempt / NUM_MTU_SIZES * (NUM_MTU_SIZES - 1);
                                    if(lowestMtuIndex > cr->requestsMade)
                                    {
                                        cr->requestsMade = (Nui8) lowestMtuIndex;
                                        cr->nextRequestTime = timeMS;
                                    }
                                    else
                                        cr->requestsMade = (Nui8)(cr->mAttempt + 1);
                                }
                            }
                            // SocketUtil::SetDoNotFragment(socketToUse, 0);
        #if (N_PLAT != N_PLAT_NACL) && (N_PLAT != N_PLAT_WINRT)
                            if (socketToUse->IsBerkleySocket())
                                ((RNS2_Berkley *)socketToUse)->SetDoNotFragment(0);
        #endif
                            ++requestedConnectionQueueIndex;
                        }
                    }
                    else
                        ++requestedConnectionQueueIndex;

                    mRequestConnectMutex.lock();
                }
                mRequestConnectMutex.unlock();
            }

            // mRemote in network thread
            RemoteList::iterator j, jend = mActiveRemote.end();
            for(j = 0; j != jend; ++j)
            {
                // Found an active remote system
                remote = *j;
                address = remote->mAddress;
                N_assert(address != 0, "error");
                // update is only safe to call from the same thread that calls process,
                // which is this thread

                if(timeNS == 0)
                {
                    timeNS = N_Engine().getTimer().getUS();
                    timeMS = (TimeDurMS)(timeNS / (TimeDurUS)1000);
                }

                if(timeMS > remote->mLastSure && timeMS - remote->mLastSure > remote->mPacketPrc.getTimeOut() / 2 &&
                    remote->mMode == CM_Connect)
                {
                    // If no reliable packets are waiting for an ack, do a one byte reliable send so that disconnections are noticed
                    NetStats rakNetStatistics;
                    rnss = remote->mPacketPrc.getReport(&rakNetStatistics);
                    if(rnss->messagesInResendBuffer == 0)
                    {
                        pingImpl(address, true, PT_BE_SURE);
                        remote->mLastSure = timeMS;
                    }
                }

                remote->mPacketPrc.process(remote->mSocket, address, remote->mMTU, timeNS, mSendSize, mRawPluginList, &rnr, upload);

                // Check for failure conditions
                if(remote->mPacketPrc.isTimeOut() || 
                    ((remote->mMode == CM_ShutDown || remote->mMode == CM_Close) && remote->mPacketPrc.isUpload() == false) ||
                    (remote->mMode == CM_CloseNotify && (remote->mPacketPrc.isResponse() == false || remote->mPacketPrc.isTimeOut(timeMS) == true)) ||
                    (((remote->mMode == CM_Request || remote->mMode == CM_Process || remote->mMode == CM_Identify) &&
                        timeMS > remote->mConnect && timeMS - remote->mConnect > 10000)))
                {
                    // Failed.  Inform the user?
                    // TODO - RakNet 4.0 - Return a different message identifier for CM_Close and CM_ShutDown than for CM_CloseNotify
                    // The first two mean we called close(), the last means the other system sent us ID_DISCONNECTION_NOTIFICATION
                    if(remote->mMode == CM_Connect || remote->mMode == CM_Request ||
                        remote->mMode == CM_ShutDown || remote->mMode == CM_CloseNotify)
                    {
                        msg = N_new SocketMessage(sizeof(char));
                        if(remote->mMode == CM_Request)
                            msg->mData[0] = ID_CONNECTION_ATTEMPT_FAILED;
                        else if(remote->mMode == CM_Connect)
                            msg->mData[0] = ID_CONNECTION_LOST;
                        else
                            msg->mData[0] = ID_DISCONNECTION_NOTIFICATION;

                        msg->mID = remote->mID;
                        msg->mAddress = address;
                        msg->mID.mIndex = remote->mIndex;
                        msg->mAddress.mIndex = remote->mIndex;

                        mReceiveMute.lock();
                        mReceiveList.Push(msg);
                        mReceiveMute.unlock();
                    }
                    // else connection shutting down, don't bother telling the user
        #ifdef _DO_PRINTF
                    N_printf("Connection dropped for player %i:%i\n", address);
        #endif
                    closeImpl(address, false, true, 0, PL_LOW);
                    continue;
                }

                // ping this guy if it is time to do so
                if(remote->mMode == CM_Connect && timeMS > remote->mNextPing && (mPingTest || remote->mLowPing == (Nui16)-1))
                {
                    remote->mNextPing = timeMS + 5000;
                    pingImpl(address, true, PT_WILL_LOST);

                    // update again immediately after this tick so the ping goes out right away
                    mInputEvent.set();
                }

                // Does the msg process have any packets waiting for us?
                // To be thread safe, this has to be called in the same thread as process
                bitcount = remote->mPacketPrc.receive(&data);

                while(bitcount > 0)
                {
                    // These types are for internal use and should never arrive from a network msg
                    if(data[0] == ID_CONNECTION_ATTEMPT_FAILED)
                    {
                        N_assert(0, "error");
                        bitcount = 0;
                        continue;
                    }

                    // Fast and easy - just use the data that was returned
                    byteSize = (Nui32) N_B_TO_b(bitcount);

                    // For unknown senders we only accept a few specific packets
                    if(remote->mMode == CM_Identify)
                    {
                        if((Nui8)(data)[0] == ID_CONNECTION_REQUEST)
                        {
                            processConnect(remote, address, data, byteSize);
                            N_free(data);
                        }
                        else
                        {
                            closeImpl(address, false, true, 0, PL_LOW);
        #ifdef _DO_PRINTF
                            N_printf("Temporarily banning %i:%i for sending nonsense data\n", address);
        #endif
                            String str1;
                            address.read(str1, false);
                            mBan->add(str1.c_str(), remote->mPacketPrc.getTimeOut());
                            N_free(data);
                        }
                    }
                    else
                    {
                        // However, if we are connected we still take a connection request in case both systems are trying to connect to each other
                        // at the same time
                        if((Nui8)(data)[0] == ID_CONNECTION_REQUEST)
                        {
                            // 04/27/06 This is wrong.  With cross connections, we can both have initiated the connection are in state CM_Request
                            // 04/28/06 Downgrading connections from connected will close the connection due to security at ((remote->mMode!=CM_Connect && time > remote->mConnect && time - remote->mConnect > 10000))
                            if(remote->mMode == CM_Request)
                            {
                                processConnect(remote, address, data, byteSize);
                            }
                            else
                            {
                                NetSerializer bs(data, byteSize, false);
                                bs.skipRead(1, 0);
                                bs.skipRead(sizeof(OFFLINE_MESSAGE_DATA_ID), 0);
                                bs.skipRead(UniqueID::size(), 0);
                                TimeDurMS incomingTimestamp;
                                bs.read(incomingTimestamp);

                                // Got a connection request message from someone we are already connected to. Just reply normally.
                                // This can happen due to race conditions with the fully connected mesh
                                onRequest(remote, incomingTimestamp);
                            }
                            N_free(data);
                        }
                        else if((Nui8)data[0] == ID_NEW_INCOMING_CONNECTION && byteSize > sizeof(Nui8) + sizeof(Nui32) + sizeof(Nui16) + sizeof(TimeDurMS) * 2)
                        {
                            if(remote->mMode == CM_Process)
                            {
                                remote->mMode = CM_Connect;
                                pingImpl(address, true, PT_WILL_LOST);

                                // update again immediately after this tick so the ping goes out right away
                                mInputEvent.set();

                                NetSerializer in(data, byteSize, false);
                                Address * bsSystemAddress, * tad;
                                Nui8 count;
                                in.skipRead(1, 0);
                                in.read(bsSystemAddress);
                                in.read((Nui8)count);
                                for(NCount i = 0; i < count; ++i)
                                {
                                    in.read(tad);
                                    remote->mLocalHost.push_back(tad);
                                }
                                TimeDurMS sendPingTime, sendPongTime;
                                in.read(sendPingTime);
                                in.read(sendPongTime);
                                onConnect(remote, sendPingTime, sendPongTime);

                                remote->mHostAddress = bsSystemAddress;

                                // Bug: If A connects to B through R, A's mLocalHost[0] is set to R. If A tries to send to R, sends to loopback because R==mLocalHost[0]
                                // Correct fix is to specify in connect() if target is through a proxy.
                                // However, in practice you have to connect to something else first anyway to know about the proxy. So setting once only is good enough
                                if(mLocalHost[0] == 0)
                                {
                                    mLocalHost[0] = bsSystemAddress->clone;
                                }

                                // send this info down to the game
                                msg = N_new SocketMessage(byteSize, data);
                                msg->mBitCount = bitcount;
                                msg->mAddress = address;
                                msg->mAddress.mIndex = remote->mIndex;
                                msg->mID = remote->mID;
                                msg->mID.mIndex = msg->mAddress.mIndex;

                                mReceiveMute.lock();
                                mReceiveList.Push(msg);
                                mReceiveMute.unlock();
                            }
                        }
                        else if((Nui8)data[0] == ID_CONNECTED_PONG && byteSize == sizeof(Nui8)+sizeof(TimeDurMS)*2 )
                        {
                            TimeDurMS sendPingTime, sendPongTime;

                            // Copy into the ping times array the current time - the value returned
                            // First extract the sent ping
                            NetSerializer in(data, byteSize, false);
                            in.skipRead(1, 0);
                            in.read(sendPingTime);
                            in.read(sendPongTime);

                            onConnect(remote, sendPingTime, sendPongTime);

                            N_free(data);
                        }
                        else if((Nui8)data[0] == ID_CONNECTED_PING && byteSize == sizeof(Nui8) + sizeof(TimeDurMS) )
                        {
                            NetSerializer in(data, byteSize, false);
                            in.skipRead(1, 0);
                            TimeDurMS sendPingTime;
                            in.read(sendPingTime);

                            NetSerializer out3;
                            out3.write((Token)ID_CONNECTED_PONG);
                            out3.write(sendPingTime);
                            out3.write(N_Engine().getTimer().getMS());
                            sendRapid(out3.getData(), out3.getWriteOffset(), PL_RAPID, PT_WILL_LOST, 0,
                                address, false, false, N_Engine().getTimer().getUS(), 0 );

                            // update again immediately after this tick so the ping goes out right away
                            mInputEvent.set();

                            N_free(data);
                        }
                        else if((Nui8)data[0] == ID_DISCONNECTION_NOTIFICATION)
                        {
                            // We shouldn't close the connection immediately because we need to ack the ID_DISCONNECTION_NOTIFICATION
                            remote->mMode = CM_CloseNotify;
                            N_free(data);
                        }
                        else if((Nui8)(data)[0] == ID_DETECT_LOST_CONNECTIONS && byteSize == sizeof(Nui8) )
                        {
                            // Do nothing
                            N_free(data);
                        }
                        else if((Nui8)(data)[0] == ID_INVALID_PASSWORD)
                        {
                            if(remote->mMode == CM_Request)
                            {
                                msg = N_new SocketMessage(byteSize, data);
                                msg->mBitCount = bitcount;
                                msg->mAddress = address;
                                msg->mAddress.mIndex = remote->mIndex;
                                msg->mID = remote->mID;
                                msg->mID.mIndex = msg->mAddress.mIndex;

                                mReceiveMute.lock();
                                mReceiveList.Push(msg);
                                mReceiveMute.unlock();
                                remote->mMode = CM_Close;
                            }
                            else
                            {
                                N_free(data);
                            }
                        }
                        else if((Nui8)(data)[0] == ID_CONNECTION_REQUEST_ACCEPTED)
                        {
                            if(byteSize > sizeof(Token) + sizeof(Nui32) + sizeof(Nui16) + sizeof(Nindex)+sizeof(TimeDurMS)*2)
                            {
                                // Make sure this connection accept is from someone we wanted to connect to
                                bool allowConnection, alreadyConnected;

                                if(remote->mMode == CM_Process || remote->mMode == CM_Request || mAlterIP)
                                    allowConnection = true;
                                else
                                    allowConnection = false;

                                if(remote->mMode == CM_Process)
                                    alreadyConnected = true;
                                else
                                    alreadyConnected = false;

                                if(allowConnection)
                                {
                                    Address * externalID, tad;
                                    Nindex systemIndex;
                                    Nui8 count;
                                    NetSerializer in(data, byteSize, false);
                                    in.skipRead(1, 0);
                                    in.read(externalID);
                                    in.read(systemIndex);
                                    in.read((Nui8)count);
                                    for(NCount i = 0; i < count; ++i)
                                    {
                                        in.read(tad);
                                        remote->mLocalHost.push_back(tad);
                                    }
                                    TimeDurMS sendPingTime, sendPongTime;
                                    in.read(sendPingTime);
                                    in.read(sendPongTime);
                                    onConnect(remote, sendPingTime, sendPongTime);

                                    // The remote system told us our external IP, so save it
                                    remote->mHostAddress = externalID;
                                    remote->mMode = CM_Connect;

                                    // Bug: If A connects to B through R, A's mLocalHost[0] is set to R. If A tries to send to R, sends to loopback because R==mLocalHost[0]
                                    // Correct fix is to specify in connect() if target is through a proxy.
                                    // However, in practice you have to connect to something else first anyway to know about the proxy. So setting once only is good enough
                                    if(mLocalHost[0] == 0)
                                    {
                                        mLocalHost[0] = externalID->clone();
                                    }

                                    // send the connection request complete to the game
                                    msg = N_new SocketMessage(byteSize, data);
                                    msg->mBitCount = byteSize * 8;
                                    msg->mAddress = address;
                                    msg->mAddress.mIndex = getRemoteIndex(address, true);
                                    msg->mID = remote->mID;
                                    msg->mID.mIndex = msg->mAddress.mIndex;

                                    mReceiveMute.lock();
                                    mReceiveList.Push(msg);
                                    mReceiveMute.unlock();

                                    NetSerializer out4;
                                    out4.write((Token)ID_NEW_INCOMING_CONNECTION);
                                    out4.write(address);
                                    out4.write((Nui8)mLocalHost.size() - 1);
                                    AddressList::iterator i, iend = mLocalHost.end();
                                    for(i = mLocalHost.begin() + 1; i != iend; ++i)
                                        out4.write(*i);
                                    out4.write(sendPongTime);
                                    out4.write(N_Engine().getTimer().getMS());

                                    sendRapid(out4.getData(), out4.getWriteOffset(), PL_RAPID,
                                        PT_ORDER_MAKE_SURE, 0, address, false, false,
                                            N_Engine().getTimer().getUS(), 0);

                                    if(alreadyConnected == false)
                                    {
                                        pingImpl(address, true, PT_WILL_LOST);
                                    }
                                }
                                else
                                {
                                    // Ignore, already connected
                                    N_free(data);
                                }
                            }
                            else
                            {
                                // Version mismatch error?
                                N_assert(0, "error");
                                N_free(data);
                            }
                        }
                        else
                        {
                            // What do I do if I get a message from a system, before I am fully connected?
                            // I can either ignore it or give it to the user
                            // It seems like giving it to the user is a better option
                            if((data[0] >= (Token)ID_TIMESTAMP || data[0] == ID_SND_RECEIPT_ACKED ||
                                data[0] == ID_SND_RECEIPT_LOSS) && remote->mActive)
                            {
                                msg = N_new SocketMessage(byteSize, data);
                                msg->mBitCount = bitcount;
                                msg->mAddress = address;
                                msg->mAddress.mIndex = remote->mIndex;
                                msg->mID = remote->mID;
                                msg->mID.mIndex = msg->mAddress.mIndex;

                                mReceiveMute.lock();
                                mReceiveList.Push(msg);
                                mReceiveMute.unlock();
                            }
                            else
                            {
                                N_free(data);
                            }
                        }
                    }
                    // Does the msg process have any more packets waiting for us?
                    // To be thread safe, this has to be called in the same thread as process
                    bitcount = remote->mPacketPrc.receive(&data);
                }
            }

            // Pending sends go out this often, unless mInputEvent is set
            mInputEvent.wait(10);
        }

        mRun = false;
    }
    //------------------------------------------------------------------------
    StartupResult NetworkSys::listen(const HostPeerList & hosts, NCount maxlink, NIIi prclevel)
    {
        if(mStop == false)
            return RAKNET_ALREADY_STARTED;

        // If getting the guid failed in the constructor, try again
        if(mLocalHostID.mGuid == 0)
        {
            mLocalHostID.mGuid = UniqueID::genLocalGuid();
            if(mLocalHostID.mGuid == 0)
                return COULD_NOT_GENERATE_GUID;
        }

        if (mLocalHost.size() > 0)
            return;
        mLocalHost.push_back(0);

#if N_PLAT != N_PLAT_WINRT
        N_Only(Net).getLocalHost(mLocalHost);
#endif
        std::sort(mLocalHost.begin() + 1, mLocalHost.end(), AddressSort);

        if(mLocalHostID == UniqueID::INVALID)
        {
            rnr.SeedMT((Nui32)((mLocalHostID.mGuid >> 32) ^ mLocalHostID.mGuid));
        }

        releaseSocket();

        // Go through all socket descriptors and precreate sockets on the specified addresses
        HostPeerList::iterator h, hend = hosts.end();
        for(h = hosts.begin(); h != hend; ++h)
        {
            SocketObj * r2 = N_Only(Net).createSocket();
        #if N_PLAT == N_PLAT_NACL
            NaClSocketParam ncbp;
            RNS2_NativeClient * nativeClientSocket = (RNS2_NativeClient *)r2;
            ncbp.eventHandler = this;
            ncbp.forceHostAddress = (char *)(*h)->hostAddress;
            ncbp.is_ipv6 = (*h)->socketFamily == AF_INET6;
            ncbp.nativeClientInstance = (*h)->chromeInstance;
            ncbp.port = (*h)->port;
            nativeClientSocket->Bind(&ncbp);
        #elif N_PLAT == N_PLAT_WINRT
            RNS2BindResult br;
            ((WinStore8Socket *)r2)->SetRecvEventHandler(this);
            br = ((WinStore8Socket *)r2)->Bind(ref new Platform::String());
            if(br != BR_SUCCESS)
            {
                N_delete r2;
                releaseSocket();
                return SOCKET_FAILED_TO_BIND;
            }
        #else
            if(r2->IsBerkleySocket())
            {
                BSDSocketParam bbp;
                bbp.port = (*h)->port;
                bbp.hostAddress = (char *)(*h)->hostAddress;
                bbp.addressFamily = (*h)->socketFamily;
                bbp.type = SOCK_DGRAM;
                bbp.protocol = (*h)->extraSocketOptions;
                bbp.mBlock = true;
                bbp.mBroadcast = true;
                bbp.mIPHdrIncl = false;
                bbp.doNotFragment = false;
                bbp.pollingThreadPriority = prclevel;
                bbp.eventHandler = this;
                RNS2BindResult br = ((RNS2_Berkley *)r2)->Bind(&bbp);

                if(
                #if N_NET_PROTOCOL_VER == 4
                    (*h)->socketFamily != AF_INET ||
                #endif
                    br == BR_REQUIRES_RAKNET_SUPPORT_IPV6_DEFINE)
                {
                    N_delete r2;
                    releaseSocket();
                    return SOCKET_FAMILY_NOT_SUPPORTED;
                }
                else if(br == BR_FAILED_TO_BIND_SOCKET)
                {
                    N_delete r2;
                    releaseSocket();
                    return SOCKET_PORT_ALREADY_IN_USE;
                }
                else if(br == BR_FAILED_SEND_TEST)
                {
                    N_delete r2;
                    releaseSocket();
                    return SOCKET_FAILED_TEST_SEND;
                }
                else
                {
                    N_assert(br == BR_SUCCESS, "error");
                }
            }
            else
            {
                N_assert(0, "error");
            }
        #endif
            mSocketList.push_back(r2);
        }

    #if (N_PLAT != N_PLAT_NACL) && (N_PLAT != N_PLAT_WINRT)
        SocketList::iterator t, tend = mSocketList.end();
        for(t = mSocketList.begin(); t != tend; ++t)
        {
            if(*t->IsBerkleySocket())
                ((RNS2_Berkley *)*t)->CreateRecvPollingThread(prclevel);
        }
    #endif

        AddressList::iterator i, iend = mLocalHost.end();
        for(i = mLocalHost.begin() + 1; i != iend; ++i)
        {
            Address * ad = *i;
            if(ad == 0 || ad->isInvalid())
                break;
    #if (N_PLAT != N_PLAT_NACL) && (N_PLAT != N_PLAT_WINRT)
            if(mSocketList[0]->IsBerkleySocket())
            {
                Nui16 port = ((RNS2_Berkley *)mSocketList[0])->GetBoundAddress().getHSBPort();
                ad->setHSBPort(port);
            }
    #endif
        }

        if(mRemote.size() == 0)
        {
            // Don't allow more incoming connections than we have peers.
            if(mMaxRemoteCount > maxlink)
                mMaxRemoteCount = maxlink;

            mRemoteHashList.resize(maxlink * REMOTE_SYSTEM_LOOKUP_HASH_MULTIPLE);

            for(NCount i = 0; i < maxlink; ++i)
            {
                Remote * remote = N_alloc_t(Remote, 1);
                // mRemote in Single thread
                remote->mActive = false;
                remote->mAddress = 0;
                remote->mID = UniqueID::INVALID;
                remote->mHostAddress = 0;
                remote->mMode = CM_None;
                remote->mMTU = mMTUSize;
                remote->mIndex = (Nindex)i;
                mRemote.push_back(remote);
            }

            for(NCount i = 0; i < maxlink * REMOTE_SYSTEM_LOOKUP_HASH_MULTIPLE; ++i)
            {
                mRemoteHashList[i] = 0;
            }
        }

        if(mStop)
        {
            mStop = false;
            mLocalHost[0] = 0;
            
            OutMessage * bcs;
            while((bcs = mSendList.Pop()) != 0)
            {
                if (bcs->mData)
                    N_free(bcs->mData);

                mSendList.Deallocate(bcs);
            }
            mSendList.Clear();

            mCreateMessageMutex.lock();
            while(mMessagePoolList.Size() > 0)
                N_delete mMessagePoolList.Pop();
            mCreateMessageMutex.unlock();

            mReveiceMessageMute.lock();
            while(mReveiceDataList.Size() > 0)
                N_delete mReveiceDataList.Pop();
            mReveiceMessageMute.unlock();
            mQuerySocketList.Clear();

            if(mRun == false)
            {
    #if RAKPEER_USER_THREADED != 1
                mThread = new Thread("", this, false);
                mThread->setPriority(prclevel);
                mThread->create();
                if(0)
                {
                    shutdown(0, 0);
                    return FAILED_TO_CREATE_NETWORK_THREAD;
                }
    #endif // RAKPEER_USER_THREADED!=1

    #if RAKPEER_USER_THREADED != 1
                // Wait for the threads to activate.  When they are active they will set these variables to true
                while(mRun == false)
                    Nsleep(10);
    #endif // RAKPEER_USER_THREADED != 1
            }
        }
        Plugin::iterator p, pend = mPluginList.end();
        for(p = mPluginList.begin(); p != pend; ++p)
        {
            (*p)->init();
        }

    #ifdef USE_THREADED_SEND
        SocketUtil::AddRef();
    #endif

        return RAKNET_STARTED;
    }
    //------------------------------------------------------------------------
    void NetworkSys::setMaxRemoteCount(NCount cnt)
    {
        mMaxRemoteCount = cnt;
    }
    //------------------------------------------------------------------------
    NCount NetworkSys::getMaxRemoteCount() const
    {
        return mMaxRemoteCount;
    }
    //------------------------------------------------------------------------
    NCount NetworkSys::getActiveRemoteCount() const
    {
        if(mStop == true)
            return 0;

        NCount re = 0;
        RemoteList::iterator i, iend = mActiveRemote.end();
        for(i = mActiveRemote.begin(); i != iend; ++i)
        {
            if((*i)->mActive && (*i)->mMode == CM_Connect)
            {
                ++re;
            }
        }

        return re;
    }
    //------------------------------------------------------------------------
    void NetworkSys::setEnterPassword(const String & pw)
    {
        mPassword = pw;
    }
    //------------------------------------------------------------------------
    const String & NetworkSys::getEnterPassword() const
    {
        return mPassword;
    }
    //------------------------------------------------------------------------
    ConnectResultType NetworkSys::connect(const Address * ad, const String & password, SocketObj * socket, 
        Nindex sindex, PublicKey * key, NCount attempt, TimeDurMS attemptdelay, TimeDurMS timeout)
    {
        if(mStop || (socket == 0 && sindex >= mSocketList.size()))
            return INVALID_PARAMETER;

        return connectImpl(ad, password, key, sindex, 0, attempt, attemptdelay, timeout, socket);
    }
    //------------------------------------------------------------------------
    void NetworkSys::shutdown(TimeDurMS delay, PacketCH pch, PacketLevel plevel)
    {
        bool anyActive;
        TimeDurMS startWaitingTime;
        TimeDurMS time;

        RemoteList::iterator j, jend = mRemote.end();
        if(delay > 0)
        {
            for(j = mRemote.begin(); j != jend; ++j)
            {
                if((*j)->mActive)
                    shutdown((*j)->mAddress, false, pch, plevel);
            }

            time = N_Engine().getTimer().getMS();
            startWaitingTime = time;
            while(time - startWaitingTime < delay)
            {
                anyActive = false;
                for(j = mRemote.begin(); j != jend; ++j)
                {
                    if((*j)->mActive)
                    {
                        anyActive = true;
                        break;
                    }
                }

                // If this system is out of packets to send, then stop waiting
                if(anyActive == false)
                    break;

                // This will probably cause the update thread to run which will probably
                // send the disconnection notification
                Nsleep(15);
                time = N_Engine().getTimer().getMS();
            }
        }

        PluginList:iterator p, pend = mPluginList.end();
        for(p = mPluginList.begin(); p != pend; ++p)
        {
            (*p)->shutdown();
        }

        mInputEvent.set();
        mStop = true;

#if RAKPEER_USER_THREADED != 1
        SocketList::iterator i, iend = mSocketList.end();
    #if (N_PLAT != N_PLAT_NACL) && (N_PLAT != N_PLAT_WINRT)
        for(i = mSocketList.begin(); i != iend; ++i)
        {
            if(*i->IsBerkleySocket())
            {
                ((RNS2_Berkley *)*i)->SignalStopRecvPollingThread();
            }
        }
    #endif
        while(mRun)
        {
            mStop = true;
            Nsleep(15);
        }
    #if (N_PLAT != N_PLAT_NACL) && (N_PLAT != N_PLAT_WINRT)
        for(i = mSocketList.begin(); i != iend; ++i)
        {
            if(*i->IsBerkleySocket())
            {
                ((RNS2_Berkley *)*i)->BlockOnStopRecvPollingThread();
            }
        }
    #endif
#endif // RAKPEER_USER_THREADED != 1
        for(j = 0; j != jend; ++j)
        {
            Remote * remote = *j;
            // Reserve this packet process for ourselves
            remote->mActive = false;

            // Remove any remaining packets
            N_assert(remote->mMTU <= N_Net_MaxMtuSize, "error");
            remote->mPacketPrc.reset(false, remote->mMTU, false);
            remote->mSocket = 0;
        }

        // Free any packets the user didn't deallocate
        mReceiveMute.lock();
        NCount i;
        for(i = 0; i < mReceiveList.Size(); ++i)
            N_delete mReceiveList[i];
        mReceiveList.Clear();
        mReceiveMute.unlock();

        releaseSocket();

        OutMessage * bcs;
        while((bcs = mSendList.Pop()) != 0)
        {
            if (bcs->mData)
                N_free(bcs->mData);

            mSendList.Deallocate(bcs);
        }
        mSendList.Clear();

        mCreateMessageMutex.lock();
        while(mMessagePoolList.Size() > 0)
            N_delete mMessagePoolList.Pop();
        mCreateMessageMutex.unlock();

        mReveiceMessageMute.lock();
        while(mReveiceDataList.Size() > 0)
            N_delete mReveiceDataList.Pop();
        mReveiceMessageMute.unlock();
        mQuerySocketList.Clear();

        DataStructures::Queue<ConnectRequest *> freeQueue;
        mRequestConnectMutex.lock();
        while(mRequestConnectList.Size())
            freeQueue.Push(mRequestConnectList.Pop());
        mRequestConnectMutex.unlock();

        for(i = 0; i < freeQueue.Size(); ++i)
        {
    #if LIBCAT_SECURITY==1
            CAT_AUDIT_PRINTF("AUDIT: Deleting freeQueue index %i client_handshake %x\n", i, freeQueue[i]->client_handshake);
            N_delete freeQueue[i]->client_handshake;
    #endif
            N_delete freeQueue[i];
        }

        RemoteList::iterator i, iend = mRemote.end();
        for(i = mRemote.begin(); i != iend; ++i)
        {
            N_free(*i);
        }
        mRemote.clear();
        mActiveRemote.clear();
        N_free(mRemoteHashList);
        mRemoteHashList = 0;

    #ifdef USE_THREADED_SEND
        SocketUtil::Deref();
    #endif
        mNextReceiptMutex.lock();
        mNextReceipt = 1;
        mNextReceiptMutex.unlock();
    }
    //------------------------------------------------------------------------
    bool NetworkSys::isActive() const
    {
        return mStop == false;
    }
    //------------------------------------------------------------------------
    bool NetworkSys::getConnect(AddressList & remotes) const
    {
        if(mRemote.size() == 0 || mStop == true)
        {
            return false;
        }

        DataStructures::List<Address> addresses;
        DataStructures::List<UniqueID> guids;
        getActive(addresses, guids);

        for(NCount i = 0; i < addresses.Size(); ++i)
            remotes.push_back(addresses[i]);

        return true;
    }
    //------------------------------------------------------------------------
    Nui32 NetworkSys::send(const Nui8 * data, NCount size, PacketLevel plevel, PacketType ptype,
        PacketCH pch, const UniqueAddress & usys, bool broadcast, Nui32 forceReceipt)
    {
        N_assert(ptype < PT_COUNT && ptype > 0, "error");
        N_assert(plevel < PL_COUNT && plevel > 0, "error");
        N_assert(pch < N_NET_CH, "error");

        if(data == 0 || size < 0)
            return 0;

        if(mRemote.size() == 0 || mStop == true)
            return 0;

        if(broadcast == false && !usys.isValid())
            return 0;

        if(forceReceipt == 0)
        {
            mNextReceiptMutex.lock();
            forceReceipt = mNextReceipt;
            if(++mNextReceipt == 0)
                mNextReceipt = 1;
            mNextReceiptMutex.unlock();
        }
        if(broadcast == false && isLocalHost(usys, true))
        {
            send(data, size);
            if(ptype >= PT_RECEIPT_WILL_LOST)
            {
                char buff[5];
                buff[0] = ID_SND_RECEIPT_ACKED;
                mNextReceiptMutex.lock();
                memcpy(buff + 1, &mNextReceipt, 4);
                mNextReceiptMutex.unlock();
                send(buff, 5);
            }

            return forceReceipt;
        }

        sendImpl(data, size * 8, plevel, ptype, pch, usys, broadcast, CM_None, forceReceipt);

        return forceReceipt;
    }
    //------------------------------------------------------------------------
    void NetworkSys::send(const Nui8 * data, NCount size)
    {
        if(data == 0 || size < 0)
            return;
        if(mLocalHost.size() > 1)
        {
            SocketMessage * msg = N_new SocketMessage(size);
            memcpy(msg->mData, data, size);
            msg->mAddress = mLocalHost[1];
            msg->mID = mLocalHostID;
            addMessage(msg, false);
        }
    }
    //------------------------------------------------------------------------
    // Description:
    // Gets a packet from the incoming packet queue
    // Check the SocketMessage struct at the top of CoreNetworkStructures.h for the format of the struct
    //
    // Returns:
    // 0 if no packets are waiting to be handled, otherwise an allocated packet
    // If the client is not active this will also return 0, as all waiting packets are flushed when the client is Disconnected
    // This also updates all memory blocks associated with synchronized memory and distributed objects
    //------------------------------------------------------------------------
    #ifdef _MSC_VER
    #pragma warning(disable : 4701) // warning C4701: local variable <variable name> may be used without having been initialized
    #endif
    SocketMessage * NetworkSys::receive()
    {
        if(mStop == true)
            return 0;

        PluginList::iterator p, pend = mPluginList.end();
        for(p = mPluginList.begin(); p != pend; ++p)
        {
            (*p)->update();
        }

        SocketMessage * msg;
        ConnectReceiveType pluginResult;
        do
        {
            mReceiveMute.lock();
            if(mReceiveList.IsEmpty())
                msg = 0;
            else
                msg = mReceiveList.Pop();
            mReceiveMute.unlock();

            if(msg == 0)
                return 0;

            if((msg->mLength >= sizeof(Nui8) + sizeof(TimeDurMS)) && ((Nui8) msg->mData[0] == ID_TIMESTAMP))
            {
                NetSerializer timeBS(msg->mData + sizeof(Nui8), sizeof(TimeDurMS), false);
                TimeDurMS encodedTimestamp;
                timeBS.read(encodedTimestamp);

                encodedTimestamp = encodedTimestamp - getTimeEquation(msg->mAddress);
                timeBS.seekWrite(0, 0);
                timeBS.write(encodedTimestamp);
            }

            process(msg, mPluginList);
            for(i = 0; i < mPluginList.Size(); ++i)
            {
                pluginResult = mPluginList[i]->onMessage(msg);
                if(pluginResult == CRT_Destroy)
                {
                    N_delete msg;
                    msg = 0; // Will do the loop again and get another msg
                    break; // break out of the enclosing for
                }
                else if(pluginResult == CRT_Inner)
                {
                    msg = 0;
                    break;
                }
            }
        } while(msg == 0);

        N_assert(msg->mData, "error");

        return msg;
    }
    //------------------------------------------------------------------------
    NCount NetworkSys::getRemoteCount() const
    {
        return mRemote.size();
    }
    //------------------------------------------------------------------------
    // notify: True to send ID_DISCONNECTION_NOTIFICATION to the recipient. False to close it silently.
    // channel: If blockDuration > 0, the disconnect msg will be sent on this channel
    //------------------------------------------------------------------------
    void NetworkSys::close(const UniqueAddress & uaddr, bool notify, PacketCH pch, PacketLevel plevel)
    {
        closeImpl(uaddr, notify, false, pch, plevel);

        // 12/14/09 Return ID_CONNECTION_LOST when calling close with notify==false, elsewise it is never returned
        if(notify == false && getState(uaddr) == IS_CONNECTED)
        {
            SocketMessage * msg = N_new SocketMessage(sizeof(char));
            msg->mData[0] = ID_CONNECTION_LOST; // DeadConnection
            msg->mID = uaddr.mID == UniqueID::INVALID ? getGUID(uaddr.mAddress) : uaddr.mID;
            msg->mAddress = uaddr.mAddress == 0 ? getAddress(uaddr.mID) : uaddr.mAddress;
            if(msg->mAddress != 0)
            {
                msg->mAddress.mIndex = getRemoteIndex(msg->mAddress);
                msg->mID.mIndex = msg->mAddress.mIndex;
            }
            msg->mLocal = true; // else processed twice

            mReceiveMute.lock();
            mReceiveList.Push(msg);
            mReceiveMute.unlock();
        }
    }
    //------------------------------------------------------------------------
    // Cancel a pending connection attempt
    // If we are already connected, the connection stays open
    // \param[in] uaddr Which system to cancel
    //------------------------------------------------------------------------
    void NetworkSys::cancel(const Address * uaddr)
    {
        NCount i = 0;
        mRequestConnectMutex.lock();
        while(i < mRequestConnectList.Size())
        {
            if(mRequestConnectList[i]->mAddress == *uaddr)
            {
    #if LIBCAT_SECURITY == 1
                CAT_AUDIT_PRINTF("AUDIT: Deleting mRequestConnectList %i client_handshake %x\n", i, mRequestConnectList[ i ]->client_handshake);
                N_delete mRequestConnectList[i]->client_handshake;
    #endif
                N_delete mRequestConnectList[i];
                mRequestConnectList.RemoveAtIndex(i);
                break;
            }
            else
                ++i;
        }
        mRequestConnectMutex.unlock();
    }
    //------------------------------------------------------------------------
    #ifdef _MSC_VER
    #pragma warning(disable : 4702) // warning C4702: unreachable code
    #endif
    ConnectionState NetworkSys::getState(const UniqueAddress & usys)
    {
        if(usys.mAddress != 0)
        {
            mRequestConnectMutex.lock();
            for(Nui32 i = 0; i < mRequestConnectList.Size(); ++i)
            {
                if(mRequestConnectList[i]->mAddress == *usys.mAddress)
                {
                    mRequestConnectMutex.unlock();
                    return IS_PENDING;
                }
            }
            mRequestConnectMutex.unlock();
        }

        int index;
        if(usys.mAddress != 0)
        {
            index = getRemoteIndex(usys.mAddress, false);
        }
        else
        {
            index = getRemoteIndex(usys.mID);
        }

        if(index == -1)
            return IS_NOT_CONNECTED;

        if(mRemote[index]->mActive == false)
            return IS_DISCONNECTED;

        switch (mRemote[index]->mMode)
        {
        case CM_ShutDown:
            return IS_DISCONNECTING;
        case CM_Close:
            return IS_SILENTLY_DISCONNECTING;
        case CM_CloseNotify:
            return IS_DISCONNECTING;
        case CM_Request:
            return IS_CONNECTING;
        case CM_Process:
            return IS_CONNECTING;
        case CM_Identify:
            return IS_CONNECTING;
        case CM_Connect:
            return IS_CONNECTED;
        default:
            return IS_NOT_CONNECTED;
        }

        return IS_NOT_CONNECTED;
    }
    //------------------------------------------------------------------------
    Nindex NetworkSys::getRemoteIndex(const UniqueID & id)
    {
        if(id == UniqueID::INVALID)
            return -1;

        if(id.mIndex < mRemote.size())
            if(id.mIndex != (Nindex)-1 && mRemote[id.mIndex]->mID == id && mRemote[id.mIndex]->mActive)
            return id.mIndex;

        // mRemote in user and network thread
        Nindex i;
        RemoteList::iterator j, jend = mRemote.end()
        for(i = 0, j = mRemote.begin(); j != jend; ++i, ++j)
            if((*j)->mActive && (*j)->mID == id)
                return i;

        // If no active results found, try previously active results.
        for(i = 0, j = mRemote.begin(); j != jend; ++i, ++j)
            if((*j)->mID == id)
                return i;

        return -1;
    }
    //------------------------------------------------------------------------
    Nindex NetworkSys::getRemoteIndex(const Address * dst, bool thread) const
    {
        if(dst == 0)
            return -1;

        if(dst->mIndex < mRemote.size())
            if(dst->mIndex != (Nindex)-1 && mRemote[dst->mIndex]->mAddress == *dst &&
                mRemote[dst->mIndex]->mActive)
            return dst->mIndex;

        if(thread)
        {
            return getIndexImpl(dst);
        }
        else
        {
            // mRemote in user and network thread
            Nindex i;
            RemoteList::iterator j, jend = mRemote.end();
            for(i = 0, j = mRemote.begin(); j != jend; ++i, ++j)
                if((*j)->mActive && (*j)->mAddress == *dst)
                    return i;

            // If no active results found, try previously active results.
            for(i = 0, j = mRemote.begin(); j != jend; ++i, ++j)
                if((*j)->mAddress == *dst)
                    return i;
        }
        return -1;
    }
    //------------------------------------------------------------------------
    Address * NetworkSys::getAddress(Nindex index)
    {
        if(index < mRemote.size() && mRemote[index]->mActive && mRemote[index]->mMode == CM_Connect)
            return mRemote[index]->mAddress;

        return 0;
    }
    //------------------------------------------------------------------------
    const UniqueID & NetworkSys::getGUID(Nindex index)
    {
        if(index < mRemote.size() && mRemote[index]->mActive && mRemote[index]->mMode == CM_Connect)
            return mRemote[index]->mID;

        return UniqueID::INVALID;
    }
    //------------------------------------------------------------------------
    const UniqueID & NetworkSys::getGUID(const Address * input) const
    {
        if(input == 0)
            return UniqueID::INVALID;

        if(input->mIndex != (Nindex)-1 && input->mIndex < mRemote.size() &&
            mRemote[input->mIndex]->mAddress == *input)
            return mRemote[input->mIndex]->mID;

        RemoteList::iterator j, jend = mRemote.end();
        for(Nindex i = 0, j = mRemote.begin(); j != jend; ++i)
        {
            if((*j)->mAddress == *input)
            {
                // Set the mIndex so future lookups will be fast
                (*j)->mID.mIndex = i;

                return (*j)->mID;
            }
        }
        return UniqueID::INVALID;
    }
    //------------------------------------------------------------------------
    void NetworkSys::getActive(UniqueAddressList & uaddr) const
    {
        if(mActiveRemote.size() == 0 || mStop == true)
            return;

        RemoteList::iterator i, iend = mActiveRemote.end();
        for(i = mActiveRemote.begin(); i != iend; ++i)
        {
            if((*i)->mActive && (*i)->mMode == CM_Connect)
            {
                uaddr.push_back(UniqueAddress((*i)->mAddress, (*i)->mID);
            }
        }
    }
    //------------------------------------------------------------------------
    void NetworkSys::setConnectTimeLimit(bool b)
    {
        mConnectTimeLimit = b;
    }
    //------------------------------------------------------------------------
    bool NetworkSys::isConnectTimeLimit() const
    {
        return mConnectTimeLimit;
    }
    //------------------------------------------------------------------------
    void NetworkSys::ping(const Address * addr)
    {
        pingImpl(addr, false, PT_WILL_LOST);
    }
    //------------------------------------------------------------------------
    // Description:
    // send a ping to the specified unconnected system.
    // The remote system, if it is Initialized, will respond with ID_UNCONNECTED_PONG.
    // The final ping time will be encoded in the following sizeof(TimeDurMS) bytes.
    //
    // Parameters:
    // host: Either a dotted IP address or a domain name.  Can be 255.255.255.255 for LAN broadcast.
    // port: Which port to connect to on the remote machine.
    // onlyReplyOnAcceptingConnections: Only request a reply if the remote system has open connections
    // sindex Index into the array of socket descriptors passed to detail in NetworkSys::init() to send on.
    //------------------------------------------------------------------------
    bool NetworkSys::ping(const String & host, Nui16 port, bool onlyReplyOnAcceptingConnections, Nindex sindex)
    {
        N_assert(sindex < mSocketList.size(), "error");

        if(host == String(""))
            return false;

        NetSerializer out(sizeof(Nui8) + sizeof(TimeDurMS));
        if(onlyReplyOnAcceptingConnections)
            out.write((Token)ID_UNCONNECTED_PING_OPEN_CONNECTIONS);
        else
            out.write((Token)ID_UNCONNECTED_PING);

        out.write(N_Engine().getTimer().getMS());

        out.write((const Nui8 *)OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));

        out.write(mLocalHostID);

        SocketSendData bsp;
        bsp.mData = out.getData();
        bsp.mLength = out.getSize();
        bsp.mAddress.write(host, port);
        if(bsp.mAddress == 0 || bsp.mAddress->isInvalid())
            return false;
        
        mSocketList[sindex]->send(&bsp);

        return true;
    }
    //------------------------------------------------------------------------
    int NetworkSys::getAvgPing(const UniqueAddress & usys) const
    {
        Remote * remote = getRemoteSystem(usys, false, false);
        if(remote == 0)
            return -1;

        int sum, quantity;
        for(sum = 0, quantity = 0; quantity < Net_PING_TIME_SIZE; ++quantity)
        {
            if(remote->mTimeEquation[quantity].mPingTime == 65535)
                break;
            else
                sum += remote->mTimeEquation[quantity].mPingTime;
        }

        if (quantity > 0)
            return sum / quantity;
        else
            return -1;
    }
    //------------------------------------------------------------------------
    int NetworkSys::getLastPing(const UniqueAddress & usys) const
    {
        Remote * remote = getRemoteSystem(usys, false, false);
        if(remote == 0)
            return -1;

        if(remote->mNetTimeEquationIndex == 0)
            return remote->mTimeEquation[Net_PING_TIME_SIZE - 1].mPingTime;
        else
            return remote->mTimeEquation[remote->mNetTimeEquationIndex - 1].mPingTime;
    }
    //------------------------------------------------------------------------
    int NetworkSys::getWorstPing(const UniqueAddress & usys) const
    {
        Remote * remote = getRemoteSystem(usys, false, false);
        if(remote == 0)
            return -1;

        return remote->mLowPing;
    }
    //------------------------------------------------------------------------
    void NetworkSys::setPingTest(bool set)
    {
        mPingTest = set;
    }
    //------------------------------------------------------------------------
    TimeDurMS NetworkSys::getTimeEquation(const UniqueAddress & usys)
    {
        Remote * remote = getRemoteSystem(usys, false, false);
        if(remote == 0)
            return 0;

        NCount i, lping = 65535;
        TimeDurMS re = 0;
        for(i = 0; i < Net_PING_TIME_SIZE; ++i)
        {
            if(remote->mTimeEquation[i].mPingTime == 65535)
                break;

            if(remote->mTimeEquation[i].mPingTime < lping)
            {
                re = remote->mTimeEquation[i].mTimeEquation;
                lping = remote->mTimeEquation[i].mPingTime;
            }
        }

        return re;
    }
    //------------------------------------------------------------------------
    TimeDurMS NetworkSys::getTimeEquation(const Address * ad) const
    {
        Remote * remote = getRemoteSystem(ad, true, true);
        if(remote == 0)
            return 0;

        NCount i, lping = 65535;
        TimeDurMS re = 0;
        for(i = 0; i < Net_PING_TIME_SIZE; ++i)
        {
            if(remote->mTimeEquation[i].mPingTime == 65535)
                break;

            if(remote->mTimeEquation[i].mPingTime < lping)
            {
                re = remote->mTimeEquation[i].mTimeEquation;
                lping = remote->mTimeEquation[i].mPingTime;
            }
        }

        return re;
    }
    //------------------------------------------------------------------------
    // Description:
    // Length should be under 400 bytes, as a security measure against flood attacks
    // Sets the data to send with an  (LAN server discovery) /(offline ping) response
    // See the ping sample project for how this is used.
    // data: a block of data to store, or 0 for none
    // length: The length of data in bytes, or 0 for none
    //------------------------------------------------------------------------
    void NetworkSys::setTempPing(const Nui8 * data, NCount size)
    {
        N_assert(size < 400, "error");

        mPingDataMutex.lock();
        mPingData.seekRead(0);
        mPingData.seekWrite(0);
        if(data && size > 0)
            mPingData.write(data, size);
        mPingDataMutex.unlock();
    }
    //------------------------------------------------------------------------
    // Returns pointers to a copy of the data passed to setTempPing
    // \param[out] data A pointer to a copy of the data passed to \a setTempPing()
    // \param[out] size A pointer filled in with the size parameter passed to setTempPing()
    // \sa setTempPing
    //------------------------------------------------------------------------
    void NetworkSys::getTempPing(Nui8 ** data, Nui32 * size)
    {
        mPingDataMutex.lock();
        *data = mPingData.getData();
        *size = mPingData.getSize();
        mPingDataMutex.unlock();
    }
    //------------------------------------------------------------------------
    // Description:
    // Return the unique Address that represents you on the the network
    // Note that unlike in previous versions, this is a struct and is not sequential
    //------------------------------------------------------------------------
    Address * NetworkSys::getLAN(Nindex index, const Address * host) const
    {
        if(host == 0)
        {
            if(mLocalHost.size() - 1 > index)
                return mLocalHost[index + 1];
        }
        else
        {
            Remote * remote = getRemoteSystem(host, false, true);
            if(remote == 0)
                return 0;
            return remote->mLocalHost[index + 1];
        }
        return 0;
    }
    //------------------------------------------------------------------------
    // Description:
    // Return the unique address identifier that represents you on the the network and is based on your external
    // IP / port (the IP / port the specified player uses to communicate with you)
    // Note that unlike in previous versions, this is a struct and is not sequential
    //
    // Parameters:
    // target: Which remote system you are referring to for your external ID
    //------------------------------------------------------------------------
    Address * NetworkSys::getConnectTo(const Address * target) const
    {
        Address * re = 0;

        if(target == 0)
            return mLocalHost[0];

        // First check for active connection with this address
        RemoteList::iterator i, iend = mRemote.end();
        for(i = mRemote.begin(); i != iend; ++i)
        {
            if((*i)->mAddress == target)
            {
                if((*i)->mActive)
                    return (*i)->mHostAddress;
                else if((*i)->mHostAddress != 0)
                    re = (*i)->mHostAddress;
            }
        }

        return re;
    }
    //------------------------------------------------------------------------
    const UniqueID & NetworkSys::getGuid() const
    {
        return mLocalHostID;
    }
    //------------------------------------------------------------------------
    Address * NetworkSys::getAddress(const UniqueID & input) const
    {
        if(input == UniqueID::INVALID)
            return 0;

        if(input == mLocalHostID)
            return getLAN(mLocalHostID);

        if(input.mIndex < mRemote.size())
            if(input.mIndex != (Nindex)-1 && mRemote[input.mIndex]->mID == input)
                return mRemote[input.mIndex]->mAddress;

        NCount i = 0;
        RemoteList::iterator j, jend = mRemote.end();
        for(j = mRemote.begin(); j != jend; ++i, ++j)
        {
            if((*j)->mID == input)
            {
                // Set the mIndex so future lookups will be fast
                (*j)->mID.mIndex = (Nindex)i;

                return (*j)->mAddress;
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    // Set the time, in MS, to use before considering ourselves disconnected after not being able to deliver a reliable msg
    // \param[in] time TimeDurMS, in MS
    //------------------------------------------------------------------------
    void NetworkSys::setTimeOut(const Address * dst, TimeDurMS time)
    {
        if(dst == 0)
        {
            mTimeOut = time;
            RemoteList::iterator i, iend = mRemote.end();
            for(i = mRemote.begin(); i != iend; ++i)
            {
                if((*i)->mActive)
                    (*i)->mPacketPrc.setTimeOut(time);
            }
        }
        else
        {
            Remote * remote = getRemoteSystem(dst, false, true);
            if(remote != 0)
                remote->mPacketPrc.setTimeOut(time);
        }
    }
    //------------------------------------------------------------------------
    TimeDurMS NetworkSys::getTimeOut(const Address * target)
    {
        if(target == 0)
        {
            return mTimeOut;
        }
        else
        {
            Remote * remote = getRemoteSystem(target, false, true);
            if (remote != 0)
                remote->mPacketPrc.getTimeOut();
        }
        return mTimeOut;
    }
    //------------------------------------------------------------------------
    NCount NetworkSys::getMTU(const Address * target) const
    {
        if(target != 0)
        {
            Remote * rss = getRemoteSystem(target, false, true);
            if(rss)
                return rss->mMTU;
        }
        return mMTUSize;
    }
	//------------------------------------------------------------------------
	Secure * NetworkSys::getSecure() const
	{
		return mSecure;
	}
	//------------------------------------------------------------------------
	Secure * NetworkSys::getBan() const
	{
		return mBan;
	}
    //------------------------------------------------------------------------
    NCount NetworkSys::getLocalHostCount()
    {
        if(mStop == true)
        {
            if (mLocalHost.size() > 0)
                return;
            mLocalHost.push_back(0);

#if N_PLAT != N_PLAT_WINRT
            N_Only(Net).getLocalHost(mLocalHost);
#endif
            std::sort(mLocalHost.begin() + 1, mLocalHost.end(), AddressSort);
        }

        NCount re = 0;
        AddressList::iterator i, iend = mLocalHost.end()
        for(i = mLocalHost.begin() + 1; i != iend; ++i)
        {
            Address * temp = *i;
            if(temp && !temp->isInvalid())
                ++re;
        }

        return re;
    }
    //------------------------------------------------------------------------
    void NetworkSys::addLocalHost(const Address & address)
    {
        N_assert(mLocalHost.size() - 1 > N_Socket_MaxLocalIP, "error");
        mLocalHost.push_back(address);
    }
    //------------------------------------------------------------------------
    String NetworkSys::getLocalHost(Nindex index)
    {
        if(mStop == true)
        {
            if (mLocalHost.size() > 0)
                return;
            mLocalHost.push_back(0);

#if N_PLAT != N_PLAT_WINRT
            N_Only(Net).getLocalHost(mLocalHost);
#endif
            std::sort(mLocalHost.begin() + 1, mLocalHost.end(), AddressSort);
        }
        String temp;
        if(mLocalHost.size() - 1 > index)
            mLocalHost[index + 1].read(temp, false);
        return temp;
    }
    //------------------------------------------------------------------------
    bool NetworkSys::isLocalHost(const String & ip)
    {
        if(ip.size() == 0)
            return false;

        if((ip ==  "127.0.0.1") || (ip == "localhost"))
            return true;

        NCount c;
        RemoteList::iterator i, iend = mLocalHost.end();
        for(c = 0, i = mLocalHost.begin() + 1; i != iend; ++c, ++i)
        {
            if(ip == getLocalHost(c))
                return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void NetworkSys::setAlterIP(bool b)
    {
        mAlterIP = b;
    }
    //------------------------------------------------------------------------
    // Description:
    // Sends a message ID_ADVERTISE_SYSTEM to the remote unconnected system.
    // This will tell the remote system our external IP outside the LAN, and can be used for NAT punch through
    //
    // Requires:
    // The sender and recipient must already be started via a successful call to Initialize
    //
    //------------------------------------------------------------------------
    bool NetworkSys::send(const String & host, Nui16 port, const Nui8 * data, NCount size, Nindex sindex)
    {
        NetSerializer bs;
        bs.write((Token)ID_ADVERTISE_SYSTEM);
        bs.write(data, size);
        return sendExpedite(host, port, bs.getData(), bs.getSize(), sindex);
    }
    //------------------------------------------------------------------------
    void NetworkSys::setSplitFactor(NCount factor)
    {
        mSplitMessageFactor = factor;
        RemoteList::iterator i, iend = mRemote.end();
        for(i = mRemote.begin(); i != iend; ++i)
        {
            (*i)->mPacketPrc.setSplitFactor(mSplitMessageFactor);
        }
    }
    //------------------------------------------------------------------------
    NCount NetworkSys::getSplitFactor() const
    {
        return mSplitMessageFactor;
    }
    //------------------------------------------------------------------------
    void NetworkSys::setMessageTimeOut(TimeDurMS time)
    {
        mMessageTimeOut = time;
        RemoteList::iterator i, iend = mRemote.end();
        for(i = mRemote.begin(); i != iend; ++i)
        {
            (*i)->mPacketPrc.setMessageTimeOut(mMessageTimeOut);
        }
    }
    //------------------------------------------------------------------------
    TimeDurMS NetworkSys::getMessageTimeOut() const
    {
        return mMessageTimeOut;
    }
    //------------------------------------------------------------------------
    void NetworkSys::sendTTL(const String & host, Nui16 port, NCount ttl, Nindex sindex)
    {
#if (N_PLAT != N_PLAT_NACL) && (N_PLAT != N_PLAT_WINRT)
        Nui8 fakeData[2];
        fakeData[0] = 0;
        fakeData[1] = 1;

        if(mSocketList[sindex]->IsBerkleySocket())
        {
            SocketSendData bsp;
            bsp.mData = fakeData;
            bsp.mLength = 2;
            bsp.mAddress.write(host, port);
            bsp.ttl = ttl;

            mSocketList[sindex]->send(&bsp);
        }
#endif
    }
    //------------------------------------------------------------------------
    void NetworkSys::attach(Plugin * plugin)
    {
        if(mPluginList.GetIndexOf(plugin) == MAX_UNSIGNED_LONG)
        {
            plugin->setPrc(this);
            plugin->onAttach();
            mPluginList.Insert(plugin);
        }
    }
    //------------------------------------------------------------------------
    void NetworkSys::detach(Plugin * plugin)
    {
        Nui32 index = mPluginList.GetIndexOf(plugin);
        if (index != MAX_UNSIGNED_LONG)
        {
            // Unordered list so delete from end for speed
            mPluginList[index] = mPluginList[mPluginList.Size() - 1];
            mPluginList.RemoveFromEnd();
        }
        plugin->onDetach();
        plugin->setPrc(0);
    }
    //------------------------------------------------------------------------
    // Put a msg back at the end of the receive queue in case you don't want to deal with it immediately
    //
    // msg The msg you want to push back.
    // head True to push the msg so that the next receive call returns it.  False to push it at the end of the queue (obviously pushing it at the end makes the packets out of order)
    //------------------------------------------------------------------------
    void NetworkSys::addMessage(SocketMessage * msg, bool head)
    {
        if(msg == 0)
            return;

        mReceiveMute.lock();
        if(head)
            mReceiveList.push_front(msg, 0);
        else
            mReceiveList.push_back(msg);
        mReceiveMute.unlock();
    }
    //------------------------------------------------------------------------
    void NetworkSys::sendSwap(UniqueID id, const Address * address)
    {
        OutMessage * bcs = mSendList.Allocate();
        bcs->mData = 0;
        bcs->mUniquePeer.mID = id;
        bcs->mUniquePeer.mAddress = address;
        bcs->mType = BCS_CHANGE_SYSTEM_ADDRESS;
        mSendList.Push(bcs);
    }
    //------------------------------------------------------------------------
    SocketObj * NetworkSys::getSocket(const Address * target)
    {
        // send a query to the thread to get the socket, and return when we got it
        OutMessage * bcs = mSendList.Allocate();
        bcs->mType = BCS_GET_SOCKET;
        bcs->mUniquePeer = target;
        bcs->mData = 0;
        mSendList.Push(bcs);

        // Block up to one second to get the socket, although it should actually take virtually no time
        SocketList * sqo;
        TimeDurMS stopWaiting = N_Engine().getTimer().getMS() + 1000;
        SocketObjList output;
        while(N_Engine().getTimer().getMS() < stopWaiting)
        {
            if(mRun == false)
                return 0;

            Nsleep(0);

            sqo = mQuerySocketList.Pop();
            if(sqo)
            {
                output = sqo;
                sqo.Clear(false);
                mQuerySocketList.Deallocate(sqo);
                if(output.Size())
                    return output[0];
                break;
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void NetworkSys::getSockets(SocketObjList & sockets)
    {
        // send a query to the thread to get the socket, and return when we got it
        OutMessage * bcs = mSendList.Allocate();
        bcs->mType = BCS_GET_SOCKET;
        bcs->mUniquePeer->setInvalid();
        bcs->mData = 0;
        mSendList.Push(bcs);

        // Block up to one second to get the socket, although it should actually take virtually no time
        SocketList * sqo;
        while(1)
        {
            if(mRun == false)
                return;

            Nsleep(0);

            sqo = mQuerySocketList.Pop();
            if(sqo)
            {
                sockets = *sqo;
                //sqo->sockets.Clear(false);
                mQuerySocketList.Deallocate(sqo);
                return;
            }
        }
        return;
    }
    //------------------------------------------------------------------------
    void NetworkSys::setSendSize(NCount count)
    {
        mSendSize = count;
    }
    //------------------------------------------------------------------------
    bool NetworkSys::sendExpedite(const String & host, Nui16 port, const Nui8 * data, NCount count, Nindex sindex)
    {
        N_assert(sindex < mSocketList.size(), "error");

        if(mStop == true)
            return false;

        // This is a security measure.  Don't send data longer than this value
        N_assert(count <= (MAX_OFFLINE_DATA_LENGTH + sizeof(Nui8) + sizeof(TimeDurMS) + UniqueID::size() + sizeof(OFFLINE_MESSAGE_DATA_ID)), "error");

        // 34 bytes
        NetSerializer out;
        out.write((Token)ID_OUT_OF_BAND_INTERNAL);
        out.write(mLocalHostID);
        out.write((const Nui8 *)OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
        
        if(count > 0)
        {
            out.write(data, count);
        }

        SocketSendData bsp;
        bsp.mData = out.getData();
        bsp.mLength = out.getSize();
        bsp.mAddress.write(host, port);

        mSocketList[sindex]->send(&bsp);

        return true;
    }
    //------------------------------------------------------------------------
    NetStats * NetworkSys::getReport(const Address * address, NetStats * rns)
    {
        static NetStats staticStatistics;
        NetStats * systemStats;
        if(rns == 0)
            systemStats = &staticStatistics;
        else
            systemStats = rns;

        if(address == 0)
        {
            bool firstWrite = false;
            RemoteList::iterator i, iend = mRemote.end();
            for(i = mRemote.begin(); i != iend; ++i)
            {
                if((*i)->mActive)
                {
                    NetStats rnsTemp;
                    (*i)->mPacketPrc.getReport(&rnsTemp);

                    if(firstWrite == false)
                    {
                        memcpy(systemStats, &rnsTemp, sizeof(NetStats));
                        firstWrite = true;
                    }
                    else
                        (*systemStats) += rnsTemp;
                }
            }
            return systemStats;
        }
        else
        {
            Remote * rss = getRemoteSystem(address, false, false);
            if(rss && mStop == false)
            {
                rss->mPacketPrc.getReport(systemStats);
                return systemStats;
            }
        }

        return 0;
    }
    //------------------------------------------------------------------------
    void NetworkSys::getReport(UniqueAddressList & addresses, NetStatsList & slist)
    {
        if(mActiveRemote.size() == 0 || mStop == true)
            return;

        RemoteList::iterator i, iend = mActiveRemote.end();
        for(i = mActiveRemote.begin(); i != iend; ++i)
        {
            if((*i)->mActive && (*i)->mMode == CM_Connect)
            {
                addresses.push(UniqueAddress((*i)->mAddress, (*i)->mID);
                NetStats rns;
                (*i)->mPacketPrc.getReport(&rns);
                slist.Push(rns);
            }
        }
    }
    //------------------------------------------------------------------------
    bool NetworkSys::getReport(Nindex index, NetStats & rns)
    {
        if(index < mRemote.size() && mRemote[index]->mActive)
        {
            mRemote[index]->mPacketPrc.getReport(rns);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    #if LIBCAT_SECURITY == 1
    bool NetworkSys::GenerateConnectionRequestChallenge(ConnectRequest * cr, PublicKey * key)
    {
        CAT_AUDIT_PRINTF("AUDIT: In GenerateConnectionRequestChallenge()\n");

        cr->client_handshake = 0;
        cr->publicKeyMode = PKM_INSECURE_CONNECTION;

        if(!key) 
            return true;

        switch (key->publicKeyMode)
        {
        default:
        case PKM_INSECURE_CONNECTION:
            break;
        case PKM_ACCEPT_ANY_PUBLIC_KEY:
            CAT_OBJCLR(cr->remote_public_key);
            cr->client_handshake = N_new cat::ClientEasyHandshake();

            cr->publicKeyMode = PKM_ACCEPT_ANY_PUBLIC_KEY;
            break;
        case PKM_USE_TWO_WAY_AUTHENTICATION:
            if(key->myPublicKey == 0 || key->myPrivateKey == 0 || key->remoteServerPublicKey == 0)
            {
                return false;
            }

            cr->client_handshake = N_new cat::ClientEasyHandshake();
            memcpy(cr->remote_public_key, key->remoteServerPublicKey, cat::EasyHandshake::PUBLIC_KEY_BYTES);

            if (!cr->client_handshake->Initialize(key->remoteServerPublicKey) ||
                !cr->client_handshake->SetIdentity(key->myPublicKey, key->myPrivateKey) ||
                !cr->client_handshake->GenerateChallenge(cr->handshakeChallenge))
            {
                CAT_AUDIT_PRINTF("AUDIT: Failure initializing new client_handshake object with identity for this ConnectRequest\n");
                N_delete cr->client_handshake;
                cr->client_handshake=0;
                return false;
            }

            CAT_AUDIT_PRINTF("AUDIT: Success initializing new client handshake object with identity for this ConnectRequest -- pre-generated challenge\n");

            cr->publicKeyMode = PKM_USE_TWO_WAY_AUTHENTICATION;
            break;
        case PKM_USE_KNOWN_PUBLIC_KEY:
            if (key->remoteServerPublicKey == 0)
                return false;

            cr->client_handshake = N_new cat::ClientEasyHandshake();
            memcpy(cr->remote_public_key, key->remoteServerPublicKey, cat::EasyHandshake::PUBLIC_KEY_BYTES);

            if (!cr->client_handshake->Initialize(key->remoteServerPublicKey) ||
                !cr->client_handshake->GenerateChallenge(cr->handshakeChallenge))
            {
                CAT_AUDIT_PRINTF("AUDIT: Failure initializing new client_handshake object for this ConnectRequest\n");
                N_delete cr->client_handshake;
                cr->client_handshake=0;
                return false;
            }

            CAT_AUDIT_PRINTF("AUDIT: Success initializing new client handshake object for this ConnectRequest -- pre-generated challenge\n");

            cr->publicKeyMode = PKM_USE_KNOWN_PUBLIC_KEY;
            break;
        }

        return true;
    }
    #endif
    //------------------------------------------------------------------------
    ConnectResultType NetworkSys::connectImpl(const Address * ad, const String & password, PublicKey * key, 
        Nindex sindex, NCount attempt, TimeDurMS attemptdelay, TimeDurMS timeout, SocketObj * socket)
    {
        if(getRemoteSystem(ad, false, true))
            return ALREADY_CONNECTED_TO_ENDPOINT;

        ConnectRequest * cr = N_new ConnectRequest();

        cr->mAddress = ad;
        cr->nextRequestTime = N_Engine().getTimer().getMS();
        cr->requestsMade = 0;
        cr->mData = 0;
        cr->socketIndex = sindex;
        cr->actionToTake = ConnectRequest::CONNECT;
        cr->mAttempt = attempt;
        cr->mAttemptInterval = attemptdelay;
        cr->outgoingPassword = password;
        cr->mTimeOut = timeout;
        cr->mSocket = socket;

    #if LIBCAT_SECURITY == 1
        if(!GenerateConnectionRequestChallenge(cr, key))
            return SECURITY_INITIALIZATION_FAILED;
    #else
        (void) key;
    #endif

        // Return false if already pending, else push on queue
        mRequestConnectMutex.lock();
        for(NCount i = 0; i < mRequestConnectList.Size(); ++i)
        {
            if(mRequestConnectList[i]->mAddress == *ad)
            {
                mRequestConnectMutex.unlock();
                N_delete cr;
                return CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS;
            }
        }
        mRequestConnectList.Push(cr);
        mRequestConnectMutex.unlock();

        return CONNECTION_ATTEMPT_STARTED;
    }
    //------------------------------------------------------------------------
    Remote * NetworkSys::getRemoteSystem(const Address * dst, bool thread, bool active) const
    {
        if(thread)
        {
            Nindex index = getIndexImpl(dst);
            if(index != (Nindex) -1)
            {
                if(active == false || mRemote[index]->mActive == true)
                {
                    N_assert(mRemote[index]->mAddress == *dst, "error");
                    return mRemote[index];
                }
            }
        }
        else
        {
            if(dst == 0)
                return 0;
        
            int unconnect = -1;
            // Active connections take plevel.  But if there are no active connections, return the first address match found
            RemoteList::iterator j, jend = mRemote.end();
            for(NCount i = 0, j = mRemote.begin(); j != jend; ++j, ++i)
            {
                if((*j)->mAddress == *dst)
                {
                    if((*j)->mActive)
                        return *j;
                    else if(unconnect == -1)
                        unconnect = i;
                }
            }
            if(unconnect != -1 && active == false)
                return mRemote[unconnect];
        }
        return 0;
    }
    //------------------------------------------------------------------------
    Remote * NetworkSys::getRemoteSystem(const UniqueAddress & dst, bool thread, bool active) const
    {
        if(dst.mID != UniqueID::INVALID)
            return getRemoteSystem(dst.mID, active);

        return getRemoteSystem(dst.mAddress, thread, active);
    }
    //------------------------------------------------------------------------
    Remote * NetworkSys::getRemoteSystem(const UniqueID & dst, bool active) const
    {
        RemoteList::iterator j, jend = mRemote.end();
        for(j = mRemote.begin(); j != jend; ++j)
        {
            if((*j)->mID == dst && (active == false || (*j)->mActive))
            {
                return *j;
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void NetworkSys::processConnect(Remote * remote, const Address & address, const Nui8 * data, NCount byteSize)
    {
        NetSerializer bs(data, byteSize, false);
        bs.skipRead(1, 0);
        UniqueID id;
        bs.read(id);
        TimeDurMS incomingTimestamp;
        bs.read(incomingTimestamp);
        Nui8 doSecurity;
        bs.read(doSecurity);

    #if LIBCAT_SECURITY == 1
        Nui8 doClientKey;
        if(_using_security)
        {
            // Ignore message on bad state
            if(doSecurity != 1 || !remote->mPacketPrc.GetAuthenticatedEncryption())
                return;

            // Validate client proof of key
            Nui8 proof[cat::EasyHandshake::PROOF_BYTES];
            bs.read(proof, sizeof(proof));
            if (!remote->mPacketPrc.GetAuthenticatedEncryption()->ValidateProof(proof, sizeof(proof)))
            {
                remote->mMode = CM_Close;
                return;
            }

            CAT_OBJCLR(remote->client_public_key);

            bs.read(doClientKey);

            // Check if client wants to prove identity
            if (doClientKey == 1)
            {
                // read identity proof
                Nui8 ident[cat::EasyHandshake::IDENTITY_BYTES];
                bs.read(ident, sizeof(ident));

                // If we are listening to these proofs,
                if (_require_client_public_key)
                {
                    // Validate client identity
                    if (!_server_handshake->VerifyInitiatorIdentity(remote->answer, ident, remote->client_public_key))
                    {
                        NetSerializer out;
                        out.write((Token)ID_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY); // Report an error since the client is not providing an identity when it is necessary to connect
                        out.write((Nui8)2); // Indicate client identity is invalid
                        sendRapid(out.getData(), out.getWriteOffset(), PL_RAPID, PT_BE_SURE, 0,
                            address, false, false, N_Engine().getTimer().getUS(), 0);
                        remote->mMode = CM_Close;
                        return;
                    }
                }
                // Otherwise ignore the client public key
            }
            else
            {
                // If no client key was provided but it is required,
                if(_require_client_public_key)
                {
                    NetSerializer out;
                    out.write((Token)ID_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY); // Report an error since the client is not providing an identity when it is necessary to connect
                    out.write((Nui8)1); // Indicate client identity is missing
                    sendRapid(out.getData(), out.getWriteOffset(), PL_RAPID, PT_BE_SURE, 0, address,
                        false, false, N_Engine().getTimer().getUS(), 0);
                    remote->mMode = CM_Close;
                    return;
                }
            }
        }
    #endif // LIBCAT_SECURITY
        Nui8 * password = bs.getData() + N_B_TO_b(bs.getReadOffset());
        int passwordLength = byteSize - N_B_TO_b(bs.getReadOffset());
        if(mPassword != String(password, passwordLength))
        {
            CAT_AUDIT_PRINTF("AUDIT: Invalid password\n");
            // This one we only send once since we don't care if it arrives.
            NetSerializer out;
            out.write((Token)ID_INVALID_PASSWORD);
            out.write(UniqueID::INVALID);
            sendRapid(out.getData(), out.getWriteOffset(), PL_RAPID, PT_BE_SURE, 0,
                address, false, false, N_Engine().getTimer().getUS(), 0);
            remote->mMode = CM_Close;
            return;
        }

        // OK
        remote->mMode = CM_Process;

        onRequest(remote, incomingTimestamp);
    }
    //------------------------------------------------------------------------
    void NetworkSys::onRequest(Remote * remote, TimeDurMS incomingTimestamp)
    {
        NetSerializer out;
        out.write((Token)ID_CONNECTION_REQUEST_ACCEPTED);
        out.write(remote->mAddress);
        Nindex index = getRemoteIndex(remote->mAddress, true);
        N_assert(index != 0, "error");
        out.write(index);
        out.write((Nui8)(mLocalHost.size() - 1))
        AddressList::iterator i, iend = mLocalHost.end();
        for(i = mLocalHost.begin() + 1; i != iend; ++i)
        {
            out.write(*i);
        }
        out.write(incomingTimestamp);
        out.write(N_Engine().getTimer().getMS());

        sendRapid(out.getData(), out.getWriteOffset(), PL_RAPID, PT_ORDER_MAKE_SURE, 0, 
            remote->mAddress, false, false, N_Engine().getTimer().getUS(), 0);
    }
    //------------------------------------------------------------------------
    void NetworkSys::shutdown(const Address * address, bool rapid, PacketCH pch, PacketLevel plevel)
    {
        NetSerializer temp(sizeof(Nui8));
        temp.write((Token)ID_DISCONNECTION_NOTIFICATION);
        if(rapid)
        {
            sendRapid(temp.getData(), temp.getWriteOffset(), plevel, PT_ORDER_MAKE_SURE, pch, address,
                false, false, N_Engine().getTimer().getUS(), 0);
            Remote * rss = getRemoteSystem(address, true, true);
            rss->mMode = CM_ShutDown;
        }
        else
        {
            sendImpl(temp.getData(), temp.getWriteOffset(), plevel, PT_ORDER_MAKE_SURE, pch, address,
                false, CM_ShutDown, 0);
        }
    }
    //------------------------------------------------------------------------
    Remote * NetworkSys::createConnect(const Address & address, SocketObj * socket, bool & recently, 
        Address * bindingAddress, int incomingMTU, UniqueID id, bool useSecurity)
    {
        Remote * remote;
        TimeDurMS time = N_Engine().getTimer().getMS();

        N_assert(!address.isInvalid(), "error");
        RemoteList::iterator i, iend = mRemote.end();
        if(mConnectTimeLimit)
        {
            if(isLocalHost(address, false) == false)
            {
                for(i = mRemote.begin(); i != iend; ++i)
                {
                    remote = *i;
                    if(remote->mAddress.equal(address) && time >= remote->mConnect && time - remote->mConnect < 100)
                    {
                        // 4/13/09 Attackers can flood ID_OPEN_CONNECTION_REQUEST and use up all available connection slots
                        // Ignore connection attempts if this IP address connected within the last 100 milliseconds
                        remote->mConnect = time;
                        recently = true;
                        return 0;
                    }
                }
            }
        }

        // Don't use a different port than what we received on
        bindingAddress->setPort(socket->GetBoundAddress());

        recently = false;
        for(NCount z = 0, i = mRemote.begin(); i != iend; ++z, ++i)
        {
            remote = *i;
            if(remote->mActive == false)
            {
                attach(address, z);
                remote->mMTU = mMTUSize;
                remote->mID = id;
                remote->mActive = true; // This one line causes future incoming packets to go through the packet process
                // Reserve this packet process for ourselves.
                if(incomingMTU > remote->mMTU)
                    remote->mMTU = incomingMTU;
                N_assert(remote->mMTU <= N_Net_MaxMtuSize, "error");
                remote->mPacketPrc.reset(true, remote->mMTU, useSecurity);
                remote->mPacketPrc.setSplitFactor(mSplitMessageFactor);
                remote->mPacketPrc.setMessageTimeOut(mMessageTimeOut);
                remote->mPacketPrc.setTimeOut(mTimeOut);
                mActiveRemote.push_back(remote);
                if(socket->GetBoundAddress() == *bindingAddress)
                {
                    remote->mSocket = socket;
                }
                else
                {
                    // See if this is an internal IP address.
                    // If so, force binding on it so we reply on the same IP address as they sent to.
                    Nui32 foundIndex = (Nui32)-1;
                    NCount lhi = 0;
                    AddressList::iterator i, iend = mLocalHost.end();
                    for(i = mLocalHost.begin() + 1; i != iend; ++i, ++lhi)
                    {
                        if(bindingAddress->equal(*i))
                        {
                            foundIndex = lhi;
                            break;
                        }
                    }

                    // 06/26/09 Unconfirmed report that Vista firewall blocks the reply if we force a binding
                    // For now use the incoming socket only
                    // Originally this code was to force a machine with multiple IP addresses to reply back on the IP
                    // that the datagram came in on
                    if(1 || foundIndex == (Nui32)-1)
                    {
                        // Must not be an internal LAN address. Just use whatever socket it came in on
                        remote->mSocket = socket;
                    }
                }

                for(NCount j = 0; j < Net_PING_TIME_SIZE; ++j)
                {
                    remote->mTimeEquation[j].mPingTime = 65535;
                    remote->mTimeEquation[j].mTimeEquation = 0;
                }
                remote->mMode = CM_Identify;
                remote->mNetTimeEquationIndex = 0;
                remote->mLowPing = 65535;
                remote->mNextPing = 0; // ping immediately
                remote->mConnect = time;
                remote->mHostAddress = 0;
                remote->mLastSure = time;
    #ifdef _DEBUG
                NCount indexLoopupCheck = getRemoteIndex(address, true);
                if(indexLoopupCheck != z)
                {
                    N_assert(indexLoopupCheck == z, "error");
                }
    #endif
                return remote;
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void NetworkSys::attach(const Address * dst, Nindex index)
    {
        Address * old = mRemote[index]->mAddress;
        if(old != 0)
        {
            // Remove the reference if the reference is pointing to this inactive system
            if(getRemoteSystem(old) == *(mRemote[index]))
                detach(old);
        }
        detach(dst);

        mRemote[index]->mAddress = dst->clone();

        Nui32 hash = Nul(*dst) % (mRemote.size() * REMOTE_SYSTEM_LOOKUP_HASH_MULTIPLE);
        RemoteHash * rsi = N_alloc_t(RemoteHash, 1);
        if(mRemoteHashList[hash] == 0)
        {
            rsi->next = 0;
            rsi->index = index;
            mRemoteHashList[hash] = rsi;
        }
        else
        {
            RemoteHash * cur = mRemoteHashList[hash];
            while(cur->next != 0)
            {
                cur = cur->next;
            }

            rsi = N_alloc_t(RemoteHash, 1);
            rsi->next = 0;
            rsi->index = index;
            cur->next = rsi;
        }

        N_assert(getIndexImpl(dst) == index, "error");
    }
    //------------------------------------------------------------------------
    void NetworkSys::detach(const Address * dst)
    {
        Nui32 hashIndex = Nul(*dst) % (mRemote.size() * REMOTE_SYSTEM_LOOKUP_HASH_MULTIPLE);
        RemoteHash * cur = mRemoteHashList[hashIndex];
        RemoteHash * last = 0;
        while(cur != 0)
        {
            if(mRemote[cur->index]->mAddress == *dst)
            {
                if(last == 0)
                {
                    mRemoteHashList[hashIndex] = cur->next;
                }
                else
                {
                    last->next = cur->next;
                }

                N_free(cur);
                N_delete mRemote[cur->index]->mAddress;
                mRemote[cur->index]->mAddress = 0;
                break;
            }
            last = cur;
            cur = cur->next;
        }
    }
    //------------------------------------------------------------------------
    Nindex NetworkSys::getIndexImpl(const Address * dst) const
    {
        Nui32 hash = Nul(*dst) % (mRemote.size() * REMOTE_SYSTEM_LOOKUP_HASH_MULTIPLE);
        RemoteHash * cur = mRemoteHashList[hash];
        while(cur != 0)
        {
            if(mRemote[cur->index].mAddress == *dst)
                return cur->index;
            cur = cur->next;
        }
        return (Nindex) -1;
    }
    //------------------------------------------------------------------------
    bool NetworkSys::isLocalHost(const UniqueAddress & usys, bool port) const
    {
        if(usys.mID != UniqueID::INVALID)
            return usys.mID == mLocalHostID;

        Address * cp = usys.mAddress;
        AddressList::iterator i, iend = mLocalHost.end();
        for(i = mLocalHost.begin() + 1; i != iend; ++i)
        {
            if(cp != 0)
            {
                if(port)
                {
                    if(*i == *cp)
                        return true;
                }
                else
                {
                    if((*i)->equal(*cp))
                        return true;
                }
            }
        }

        if(usys.mAddress != 0 && mLocalHost[0] != 0)
            return (port == true && usys.mAddress == *mLocalHost[0]) ||
                (port == false && usys.mAddress.equal(*mLocalHost[0]));

        return false;
    }
    //------------------------------------------------------------------------
    SocketMessageData * NetworkSys::createMessage()
    {
        SocketMessageData * re;
        mCreateMessageMutex.lock();
        if(mMessagePoolList.Size() > 0)
        {
            re = mMessagePoolList.Pop();
            mCreateMessageMutex.unlock();
        }
        else
        {
            mCreateMessageMutex.unlock();
            re =  N_new SocketMessageData();
        }
        return re;
    }
    //------------------------------------------------------------------------
    void NetworkSys::destroyMessage(SocketMessageData * msg)
    {
        mCreateMessageMutex.lock();
        mMessagePoolList.push_back(msg);
        mCreateMessageMutex.unlock();
    }
    //------------------------------------------------------------------------
    void NetworkSys::onMessage(SocketMessageData * msg)
    {
        mReveiceMessageMute.lock();
        mReveiceDataList.push_back(msg);
        mReveiceMessageMute.unlock();

        mInputEvent.set();
    }
    //------------------------------------------------------------------------
    void NetworkSys::pingImpl(const Address * target, bool rapid, PacketType ptype)
    {
        if(mStop == true)
            return ;

        NetSerializer out(sizeof(Nui8) + sizeof(TimeDurMS));
        out.write((Token)ID_CONNECTED_PING);
        out.write(N_Engine().getTimer().getMS());
        if(rapid)
            sendRapid(out.getData(), out.getWriteOffset(), PL_RAPID, ptype,
                0, target, false, false, N_Engine().getTimer().getUS(), 0);
        else
            send(&out, PL_RAPID, ptype, 0, target, false);
    }
    //------------------------------------------------------------------------
    void NetworkSys::closeImpl(const UniqueAddress & usys, bool notify, bool rapid, PacketCH pch, PacketLevel plevel)
    {
        N_assert(pch < 32, "error");

        if(!usys.isValid())
            return;

        if(mRemote.size() == 0 || mStop == true)
            return;

        Address * target;

        if(usys.mAddress != 0)
        {
            target = usys.mAddress;
        }
        else
        {
            target = getAddress(usys.mID);
        }

        if(notify)
        {
            shutdown(target, rapid, pch, plevel);
        }
        else
        {
            if(rapid)
            {
                Nui32 index = getIndexImpl(target);
                if(index != (Nui32)-1)
                {
                    if(mRemote[index]->mActive)
                    {
                        RemoteList::iterator i, iend = mActiveRemote.end();
                        for(i = mActiveRemote.begin(); i != iend; ++i)
                        {
                            if((*i)->mAddress == *target)
                            {
                                mActiveRemote.erase(i);
                                break;
                            }
                        }
                        mRemote[index]->mActive = false;
                        mRemote[index]->mID = UniqueID::INVALID;

                        // Clear any remaining messages
                        N_assert(mRemote[index]->mMTU <= N_Net_MaxMtuSize, "error");
                        mRemote[index]->mPacketPrc.reset(false, mRemote[index]->mMTU, false);

                        // Not using this socket
                        mRemote[index]->mSocket = 0;
                    }
                }
            }
            else
            {
                OutMessage * bcs = mSendList.Allocate();
                bcs->mType = BCS_CLOSE_CONNECTION;
                bcs->mUniquePeer = *target;
                bcs->mData = 0;
                bcs->mPacketCh = pch;
                bcs->mPacketLevel = plevel;
                mSendList.Push(bcs);
            }
        }
    }
    //------------------------------------------------------------------------
    void NetworkSys::sendImpl(const Nui8 * data, NBitCount count, PacketLevel plevel, PacketType ptype,
        PacketCH pch, const UniqueAddress & usys, bool broadcast, ConnectMode mode, Nui32 receipt)
    {
        N_assert(ptype < PT_COUNT && ptype > 0, "error");
        N_assert(plevel < PL_COUNT && plevel > 0, "error");
        N_assert(pch < N_NET_CH, "error");

        OutMessage * bcs = mSendList.Allocate();
        bcs->mData = N_alloc_t(Nui8, N_B_TO_b(count));
        memcpy(bcs->mData, data, N_B_TO_b(count));
        
        bcs->mBitCount = count;
        bcs->mPacketLevel = plevel;
        bcs->mPacketType = ptype;
        bcs->mPacketCh = pch;
        bcs->mUniquePeer = usys;
        bcs->mBroadcast = broadcast;
        bcs->mMode = mode;
        bcs->mReceipt = receipt;
        bcs->mType = BCS_SEND;
        
        mSendList.Push(bcs);

        if(plevel == PL_RAPID)
        {
            // Forces pending sends to go out now, rather than waiting to the next update interval
            mInputEvent.set();
        }
    }
    //------------------------------------------------------------------------
    bool NetworkSys::sendRapid(Nui8 * data, NBitCount count, PacketLevel plevel, PacketType ptype, PacketCH pch, 
        const UniqueAddress & usys, bool broadcast, bool calleralloc, TimeDurUS current, Nui32 receipt)
    {
        Nui32 * sendList;
        Nindex index; // Iterates into the list of remote systems

        NCount sendListSize = 0;

        if(usys.mAddress != 0)
        {
            index = getRemoteIndex(usys.mAddress, true);
        }
        else if(usys.mID != UniqueID::INVALID)
        {
            if(usys.mID == UniqueID::INVALID)
                index = (Nindex)-1;

            if(usys.mID == mLocalHostID)
                index = (Nindex)-1;

            if(usys.mID.mIndex != (Nindex)-1 && usys.mID.mIndex < mRemote.size() &&
                mRemote[usys.mID.mIndex]->mID == usys.mID)
                index = usys.mID.mIndex;

            RemoteList::iterator j, jend = mRemote.end();
            for(Nindex i = 0, j = 0; j != jend; ++j, ++i)
            {
                if((*j)->mID == usys.mID)
                {
                    // Set the mIndex so future lookups will be fast
                    (*j)->mID.mIndex = i;

                    index = i;
                }
            }
            index = (Nindex)-1;
        }
        else
            index = (Nindex) -1;

        // 03/06/06 - If broadcast is false, use the optimized version of getRemoteIndex
        if(broadcast == false)
        {
            if(index == (Nindex) -1)
            {
                return false;
            }
            sendList = (Nindex *)N_alloc(sizeof(Nindex));

            if(mRemote[index]->mActive &&
                mRemote[index]->mMode != CM_ShutDown &&
                mRemote[index]->mMode != CM_Close &&
                mRemote[index]->mMode != CM_CloseNotify)
            {
                sendList[0] = index;
                sendListSize = 1;
            }
        }
        else
        {
            sendList = (Nindex *)N_alloc(sizeof(Nindex) * mRemote.size());

            Nindex i, iend = mRemote.size();
            for(i = 0; i < iend; ++i)
            {
                if(index != (Nindex) -1 && i == index)
                    continue;

                if(mRemote[i]->mActive && mRemote[i]->mAddress != 0)
                    sendList[sendListSize++] = i;
            }
        }

        if(sendListSize == 0)
        {
            N_free(sendList);
            return false;
        }
        bool callerallocUsed = false;
        for(NCount i = 0; i < sendListSize; ++i)
        {
            bool usecalleralloc = calleralloc && callerallocUsed == false && i + 1 == sendListSize;
            mRemote[sendList[i]]->mPacketPrc.send(data, count, plevel, ptype, pch,
                usecalleralloc == false, mRemote[sendList[i]]->mMTU, current, receipt);

            if(usecalleralloc)
                callerallocUsed = true;

            if(mPacketType == PT_BE_SURE || mPacketType == PT_ORDER_MAKE_SURE || mPacketType == PT_ONE_MAKE_SURE ||
                mPacketType == PT_RECEIPT_MAKE_SURE || mPacketType == PT_ORDER_RECEIPT_MAKE_SURE)
                mRemote[sendList[i]]->mLastSure = (TimeDurMS)(current / 1000);
        }

        N_free(sendList);

        return callerallocUsed;
    }
    //------------------------------------------------------------------------
    void NetworkSys::onConnect(Remote * remote, TimeDurMS sendPingTime, TimeDurMS sendPongTime)
    {
        TimeDurMS ping;
        TimeDurMS time = N_Engine().getTimer().getMS(); // update the time value to be accurate
        if(time > sendPingTime)
            ping = time - sendPingTime;
        else
            ping = 0;

        remote->mTimeEquation[remote->mNetTimeEquationIndex].mPingTime = (Nui16)ping;
        // Thanks to Chris Taylor (cat02e@fsu.edu) for the improved timestamping algorithm
        // Divide each integer by 2, rather than the sum by 2, to prevent overflow
        remote->mTimeEquation[remote->mNetTimeEquationIndex].mTimeEquation = sendPongTime - (time / 2 + sendPingTime / 2);

        if(remote->mLowPing == (Nui16)-1 || remote->mLowPing > (Nui16)ping)
            remote->mLowPing = (Nui16)ping;

        if(++remote->mNetTimeEquationIndex == Net_PING_TIME_SIZE)
            remote->mNetTimeEquationIndex = 0;
    }
    //------------------------------------------------------------------------
    bool process(Address * address, const Nui8 * data, NCount length, NetworkSys * sys, SocketObj * socket, TimeDurUS read)
    {
        Remote * remote;
        SocketMessage * msg;
        NCount i;
        bool offline;
        String str1;
        address->read(str1, false);
        if(sys->mBan->isExist(str1))
        {
            NetSerializer out;
            out.write((Token)ID_CONNECTION_BANNED);
            out.write((const Nui8 *)OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
            out.write(UniqueID::INVALID);

            SocketSendData bsp;
            bsp.mData = out.getData();
            bsp.mLength = out.getSize();
            bsp.mAddress = address;

            socket->send(&bsp);

            return true;
        }

        // The reason for all this is that the packet process has no way to tell between offline messages that arrived late for a player that is now connected,
        // and a regular encoding. So I insert OFFLINE_MESSAGE_DATA_ID into the stream, the encoding of which is essentially impossible to hit by chance
        if (length <= 2)
        {
            offline = true;
        }
        else if (((Nui8)data[0] == ID_UNCONNECTED_PING || (Nui8)data[0] == ID_UNCONNECTED_PING_OPEN_CONNECTIONS) &&
            length >= sizeof(Nui8) + sizeof(TimeDurMS) + sizeof(OFFLINE_MESSAGE_DATA_ID))
        {
            offline = memcmp(data+sizeof(Nui8) + sizeof(TimeDurMS), OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID))==0;
        }
        else if ((Nui8)data[0] == ID_UNCONNECTED_PONG && (size_t) length >= sizeof(Nui8) + sizeof(TimeDurMS) + UniqueID::size() + sizeof(OFFLINE_MESSAGE_DATA_ID))
        {
            offline = memcmp(data+sizeof(Nui8) + sizeof(TimeDurMS) + UniqueID::size(), OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID))==0;
        }
        else if ((Nui8)data[0] == ID_OUT_OF_BAND_INTERNAL &&
            (size_t) length >= sizeof(Token) + UniqueID::size() + sizeof(OFFLINE_MESSAGE_DATA_ID))
        {
            offline = memcmp(data+sizeof(Token) + UniqueID::size(), OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID))==0;
        }
        else if (
            ((Nui8)data[0] == ID_OPEN_CONNECTION_REPLY_1 ||
            (Nui8)data[0] == ID_OPEN_CONNECTION_REPLY_2 ||
            (Nui8)data[0] == ID_OPEN_CONNECTION_REQUEST_1 ||
            (Nui8)data[0] == ID_OPEN_CONNECTION_REQUEST_2 ||
            (Nui8)data[0] == ID_CONNECTION_ATTEMPT_FAILED ||
            (Nui8)data[0] == ID_NO_FREE_INCOMING_CONNECTIONS ||
            (Nui8)data[0] == ID_CONNECTION_BANNED ||
            (Nui8)data[0] == ID_ALREADY_CONNECTED ||
            (Nui8)data[0] == ID_IP_RECENTLY_CONNECTED) &&
            (size_t) length >= sizeof(Token) + UniqueID::size() + sizeof(OFFLINE_MESSAGE_DATA_ID))
        {
            offline = memcmp(data+sizeof(Token), OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID))==0;
        }
        else if (((Nui8)data[0] == ID_INCOMPATIBLE_PROTOCOL_VERSION &&
            (size_t) length == sizeof(Token) * 2 + UniqueID::size() + sizeof(OFFLINE_MESSAGE_DATA_ID)))
        {
            offline = memcmp(data + sizeof(Token) * 2, OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID))==0;
        }
        else
        {
            offline = false;
        }

        if(offline)
        {
            // These are all messages from unconnected systems.  Messages here can be any size, but are never processed from connected systems.
            if(((Nui8)data[0] == ID_UNCONNECTED_PING_OPEN_CONNECTIONS || (Nui8)(data)[0] == ID_UNCONNECTED_PING) &&
                length >= sizeof(Nui8) + sizeof(TimeDurMS) + sizeof(OFFLINE_MESSAGE_DATA_ID))
            {
                NCount count = 0;
                RemoteList::iterator r, rend = sys->mActiveRemote.end();
                for(r = sys->mActiveRemote.begin(); r != rend; ++r)
                {
                    if((*r)->mActive)
                    {
                        ++count;
                    }
                }

                if((Nui8)(data)[0] == ID_UNCONNECTED_PING || count < sys->getMaxRemoteCount())
                {
                    NetSerializer in(data, length, false);
                    in.skipRead(1, 0);
                    TimeDurMS sendPingTime;
                    in.read(sendPingTime);
                    in.skipRead(sizeof(OFFLINE_MESSAGE_DATA_ID), 0);
                    UniqueID remoteGuid = UniqueID::INVALID;
                    in.read(remoteGuid);

                    NetSerializer out5;
                    out5.write((Token)ID_UNCONNECTED_PONG); // Should be named ID_UNCONNECTED_PONG eventually
                    out5.write(sendPingTime);
                    out5.write(sys->mLocalHostID);
                    out5.write((const Nui8 *)OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));

                    sys->mPingDataMutex.lock();
                    // They are connected, so append offline ping data
                    out5.write((char *)sys->mPingData.getData(), sys->mPingData.getSize());
                    sys->mPingDataMutex.unlock();

                    SocketSendData bsp;
                    bsp.mData = out5.getData();
                    bsp.mLength = out5.getSize();
                    bsp.mAddress = address;
                    socket->send(&bsp);

                    msg = N_new SocketMessage(sizeof(Token));
                    msg->mData[0] = data[0];
                    msg->mAddress = address;
                    msg->mID = remoteGuid;
                    msg->mAddress.mIndex = sys->getRemoteIndex(mAddress, true);
                    msg->mID.mIndex = msg->mAddress.mIndex;

                    sys->mReceiveMute.lock();
                    sys->mReceiveList.Push(msg);
                    sys->mReceiveMute.unlock();
                }
            }
            // UNCONNECTED MESSAGE Pong with no data.
            else if ((Nui8)data[0] == ID_UNCONNECTED_PONG &&
                (size_t)length >= sizeof(Nui8) + sizeof(TimeDurMS) +
                UniqueID::size() + sizeof(OFFLINE_MESSAGE_DATA_ID) &&
                (size_t)length < sizeof(Nui8) + sizeof(TimeDurMS) +
                UniqueID::size() + sizeof(OFFLINE_MESSAGE_DATA_ID) + MAX_OFFLINE_DATA_LENGTH)
            {
                msg = N_new SocketMessage((length - sizeof(OFFLINE_MESSAGE_DATA_ID) - UniqueID::size() - sizeof(TimeDurMS) + sizeof(TimeDurMS)));
                NetSerializer bsIn(data, length, false);
                bsIn.skipRead(1, 0);
                TimeDurMS ping;
                bsIn.read(ping);
                bsIn.read(msg->mID);

                NetSerializer out(msg->mData, msg->mLength, false);
                out.seekWrite(0);
                out.write((Nui8)ID_UNCONNECTED_PONG);
                TimeDurMS pingMS = (TimeDurMS)ping;
                out.write(pingMS);
                out.write(
                    (const Nui8 *)data + sizeof(Nui8) + sizeof(TimeDurMS) + UniqueID::size()+sizeof(OFFLINE_MESSAGE_DATA_ID),
                    length - sizeof(Nui8) - sizeof(TimeDurMS)-UniqueID::size()-sizeof(OFFLINE_MESSAGE_DATA_ID));

                msg->mAddress = address;
                msg->mAddress.mIndex = sys->getRemoteIndex(address, true);
                msg->mID.mIndex = msg->mAddress.mIndex;

                sys->mReceiveMute.lock();
                sys->mReceiveList.Push(msg);
                sys->mReceiveMute.unlock();

            }
            else if ((Nui8)data[0] == ID_OUT_OF_BAND_INTERNAL &&
                (size_t) length > sizeof(OFFLINE_MESSAGE_DATA_ID) + sizeof(Token)+UniqueID::size() &&
                (size_t) length < MAX_OFFLINE_DATA_LENGTH + sizeof(OFFLINE_MESSAGE_DATA_ID)+sizeof(Token)+UniqueID::size())
            {
                Nui32 dataLength = (Nui32) (length - sizeof(OFFLINE_MESSAGE_DATA_ID)-UniqueID::size()-sizeof(Token));
                N_assert(dataLength < 1024, "error");
                msg = N_new SocketMessage(dataLength + 1);
                N_assert(msg->mLength < 1024, "error");

                NetSerializer bs2(data, length, false);
                bs2.skipRead(1, 0);
                bs2.read(msg->mID);

                if (data[sizeof(OFFLINE_MESSAGE_DATA_ID) + sizeof(Token) + UniqueID::size()]==ID_ADVERTISE_SYSTEM)
                {
                    msg->mLength--;
                    msg->mBitCount = N_b_TO_B(msg->mLength);
                    msg->mData[0] = ID_ADVERTISE_SYSTEM;
                    memcpy(msg->mData + 1, data + sizeof(OFFLINE_MESSAGE_DATA_ID) + sizeof(Token)*2 + UniqueID::size(), dataLength-1);
                }
                else
                {
                    msg->mData[0] = ID_OUT_OF_BAND_INTERNAL;
                    memcpy(msg->mData + 1, data + sizeof(OFFLINE_MESSAGE_DATA_ID) + sizeof(Token) + UniqueID::size(), dataLength);
                }

                msg->mAddress = address;
                msg->mAddress.mIndex = sys->getRemoteIndex(address, true);
                msg->mID.mIndex = msg->mAddress.mIndex;

                sys->mReceiveMute.lock();
                sys->mReceiveList.Push(msg);
                sys->mReceiveMute.unlock();
            }
            else if ((Nui8)(data)[0] == (Token)ID_OPEN_CONNECTION_REPLY_1)
            {
                //for (i = 0; i < sys->mRawPluginList.Size(); ++i)
                //    sys->mRawPluginList[i]->onRawReceive(data, length * 8, address);

                NetSerializer bsIn(data, length, false);
                bsIn.skipRead(1, 0);
                bsIn.skipRead(sizeof(OFFLINE_MESSAGE_DATA_ID), 0);
                UniqueID serverGuid;
                bsIn.read(serverGuid);
                Nui8 serverHasSecurity;
                Nui32 cookie;
                bsIn.read(serverHasSecurity);
                // Even if the server has security, it may not be required of us if we are in the security exception list
                if (serverHasSecurity)
                {
                    bsIn.read(cookie);
                }

                NetSerializer out;
                out.write((Token)ID_OPEN_CONNECTION_REQUEST_2);
                out.write((const Nui8 *)OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                if(serverHasSecurity)
                    out.write(cookie);

                Nui32 c;
                sys->mRequestConnectMutex.lock();
                for(c = 0; c <  sys->mRequestConnectList.Size(); ++c)
                {
                    ConnectRequest * cr;
                    cr = sys->mRequestConnectList[c];
                    if (cr->mAddress == *address)
                    {
                        if(serverHasSecurity)
                        {
    #if LIBCAT_SECURITY == 1
                            Nui8 public_key[cat::EasyHandshake::PUBLIC_KEY_BYTES];
                            bsIn.read(public_key, sizeof(public_key));

                            if(cr->publicKeyMode==PKM_ACCEPT_ANY_PUBLIC_KEY)
                            {
                                memcpy(cr->remote_public_key, public_key, cat::EasyHandshake::PUBLIC_KEY_BYTES);
                                if(!cr->client_handshake->Initialize(public_key) ||
                                    !cr->client_handshake->GenerateChallenge(cr->handshakeChallenge))
                                {
                                    CAT_AUDIT_PRINTF("AUDIT: Server passed a bad public key with PKM_ACCEPT_ANY_PUBLIC_KEY");
                                    return true;
                                }
                            }

                            if(cat::SecureEqual(public_key, cr->remote_public_key, cat::EasyHandshake::PUBLIC_KEY_BYTES)==false)
                            {
                                sys->mRequestConnectMutex.unlock();
                                CAT_AUDIT_PRINTF("AUDIT: Expected public key does not match what was sent by server -- Reporting back ID_PUBLIC_KEY_MISMATCH to user\n");

                                msg = N_new SocketMessage(sizeof(char));
                                msg->mData[0] = ID_PUBLIC_KEY_MISMATCH; // Attempted a connection and couldn't
                                msg->mBitCount = (sizeof(char) * 8);
                                msg->mAddress = cr->mAddress;
                                msg->mID = serverGuid;
                                sys->mReceiveMute.lock();
                                sys->mReceiveList.Push(msg);
                                sys->mReceiveMute.unlock();
                                return true;
                            }

                            if(cr->client_handshake == 0)
                            {
                                out.write((Nui8)0);
                            }
                            else
                            {
                                // SocketMessage contains a challenge
                                out.write((Nui8)1);
                                // challenge
                                CAT_AUDIT_PRINTF("AUDIT: Sending challenge\n");
                                out.write((const Nui8 *)cr->handshakeChallenge,cat::EasyHandshake::CHALLENGE_BYTES);
                            }
    #else // LIBCAT_SECURITY
                            out.write((Nui8)0);
    #endif // LIBCAT_SECURITY
                        }
                        else
                        {
                            // Server does not need security
    #if LIBCAT_SECURITY == 1
                            if(cr->client_handshake != 0)
                            {
                                sys->mRequestConnectMutex.unlock();
                                CAT_AUDIT_PRINTF("AUDIT: Security disabled by server but we expected security (indicated by client_handshake not null) so failing!\n");

                                msg = N_new SocketMessage(sizeof(char));
                                msg->mData[0] = ID_OUR_SYSTEM_REQUIRES_SECURITY; // Attempted a connection and couldn't
                                msg->mBitCount = (sizeof(char) * 8);
                                msg->mAddress = cr->mAddress;
                                msg->mID = serverGuid;

                                sys->mReceiveMute.lock();
                                sys->mReceiveList.Push(msg);
                                sys->mReceiveMute.unlock();
                                return true;
                            }
    #endif // LIBCAT_SECURITY
                        }
                        Nui16 mtu;
                        bsIn.read(mtu);

                        // Binding address
                        out.write(cr->mAddress);
                        sys->mRequestConnectMutex.unlock();
                        // MTU
                        out.write(mtu);
                        // Our id
                        out.write(UniqueID::INVALID);

                        //for(i = 0; i < sys->mRawPluginList.Size(); ++i)
                        //    sys->mRawPluginList[i]->onRawSend(out.getData(), out.getWriteOffset(), cr->mAddress);

                        SocketSendData bsp;
                        bsp.mData = out.getData();
                        bsp.mLength = out.getSize();
                        bsp.mAddress = address;
                        socket->send(&bsp);

                        return true;
                    }
                }
                sys->mRequestConnectMutex.unlock();
            }
            else if ((Nui8)(data)[0] == (Token)ID_OPEN_CONNECTION_REPLY_2)
            {
                //for (i = 0; i < sys->mRawPluginList.Size(); ++i)
                //    sys->mRawPluginList[i]->onRawReceive(data, length * 8, address);

                NetSerializer bs(data, length, false);
                bs.skipRead(1, 0);
                bs.skipRead(sizeof(OFFLINE_MESSAGE_DATA_ID), 0);
                UniqueID id;
                bs.read(id);
                Address * bindingAddress;
                bool b = bs.read(bindingAddress);
                N_assert(b, "error");
                Nui16 mtu;
                b = bs.read(mtu);
                N_assert(b, "error");
                bool doSecurity = false;
                b = bs.read(doSecurity);
                N_assert(b, "error");

    #if LIBCAT_SECURITY == 1
                char answer[cat::EasyHandshake::ANSWER_BYTES];
                CAT_AUDIT_PRINTF("AUDIT: Got ID_OPEN_CONNECTION_REPLY_2 and given doSecurity=%i\n", doSecurity);
                if (doSecurity)
                {
                    CAT_AUDIT_PRINTF("AUDIT: Reading cookie and public key\n");
                    bs.read((Nui8*) answer, sizeof(answer));
                }
                cat::ClientEasyHandshake *client_handshake=0;
    #endif // LIBCAT_SECURITY

                ConnectRequest * cr;
                bool unlock = true;
                Nui32 c;
                sys->mRequestConnectMutex.lock();
                for(c = 0; c < sys->mRequestConnectList.Size(); ++c)
                {
                    cr = sys->mRequestConnectList[c];
                    if(cr->mAddress == *address)
                    {
    #if LIBCAT_SECURITY == 1
                        CAT_AUDIT_PRINTF("AUDIT: System address matches an entry in the mRequestConnectList and doSecurity=%i\n", doSecurity);
                        if(doSecurity)
                        {
                            if(cr->client_handshake == 0)
                            {
                                CAT_AUDIT_PRINTF("AUDIT: Server wants security but we didn't set a public key -- Reporting back ID_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY to user\n");
                                sys->mRequestConnectMutex.unlock();

                                msg = N_new SocketMessage(2);
                                msg->mData[0] = ID_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY; // Attempted a connection and couldn't
                                msg->mData[1] = 0; // Indicate server public key is missing
                                msg->mBitCount = (sizeof(char) * 8);
                                msg->mAddress = cr->mAddress;
                                msg->mID = id;

                                sys->mReceiveMute.lock();
                                sys->mReceiveList.Push(msg);
                                sys->mReceiveMute.unlock();
                                return true;
                            }

                            CAT_AUDIT_PRINTF("AUDIT: Looks good, preparing to send challenge to server! client_handshake = %x\n", client_handshake);
                        }

    #endif // LIBCAT_SECURITY
                        sys->mRequestConnectMutex.unlock();
                        unlock = false;

                        N_assert(cr->actionToTake == ConnectRequest::CONNECT, "error");
                        // You might get this when already connected because of cross-connections
                        bool recently = false;
                        remote = sys->getRemoteSystem(address, true, true);
                        if(remote == 0)
                        {
                            if(cr->mSocket == 0)
                            {
                                remote = sys->createConnect(address, socket, recently, bindingAddress, mtu, id, doSecurity);
                            }
                            else
                            {
                                remote = sys->createConnect(address, cr->mSocket, recently, bindingAddress, mtu, id, doSecurity);
                            }
                        }

                        // 4/13/09 Attackers can flood ID_OPEN_CONNECTION_REQUEST and use up all available connection slots
                        // Ignore connection attempts if this IP address connected within the last 100 milliseconds
                        if(recently == false)
                        {
                            // Don't check getRemoteSystem, server will verify
                            if(remote)
                            {
                                // Move pointer from ConnectRequest to Remote
    #if LIBCAT_SECURITY == 1
                                cat::u8 ident[cat::EasyHandshake::IDENTITY_BYTES];
                                bool doIdentity = false;

                                if (cr->client_handshake)
                                {
                                    CAT_AUDIT_PRINTF("AUDIT: Processing answer\n");
                                    if (cr->publicKeyMode == PKM_USE_TWO_WAY_AUTHENTICATION)
                                    {
                                        if (!cr->client_handshake->ProcessAnswerWithIdentity(answer, ident, remote->mPacketPrc.GetAuthenticatedEncryption()))
                                        {
                                            CAT_AUDIT_PRINTF("AUDIT: Processing answer -- Invalid Answer\n");
                                            sys->mRequestConnectMutex.unlock();

                                            return true;
                                        }

                                        doIdentity = true;
                                    }
                                    else
                                    {
                                        if (!cr->client_handshake->ProcessAnswer(answer, remote->mPacketPrc.GetAuthenticatedEncryption()))
                                        {
                                            CAT_AUDIT_PRINTF("AUDIT: Processing answer -- Invalid Answer\n");
                                            sys->mRequestConnectMutex.unlock();

                                            return true;
                                        }
                                    }
                                    CAT_AUDIT_PRINTF("AUDIT: Success!\n");

                                    N_delete cr->client_handshake;
                                    cr->client_handshake = 0;
                                }
    #endif // LIBCAT_SECURITY
                                remote->mMode = CM_Request;
                                if(cr->mTimeOut != 0)
                                    remote->mPacketPrc.setTimeOut(cr->mTimeOut);

                                NetSerializer temp;
                                temp.write((Token)ID_CONNECTION_REQUEST);
                                temp.write(UniqueID::INVALID);
                                temp.write(N_Engine().getTimer().getMS());
    #if LIBCAT_SECURITY == 1
                                temp.write((Nui8)(doSecurity ? 1 : 0));

                                if(doSecurity)
                                {
                                    Nui8 proof[32];
                                    remote->mPacketPrc.GetAuthenticatedEncryption()->GenerateProof(proof, sizeof(proof));
                                    temp.write(proof, sizeof(proof));
                                    temp.write((Nui8)(doIdentity ? 1 : 0));

                                    if(doIdentity)
                                    {
                                        temp.write(ident, sizeof(ident));
                                    }
                                }
    #else
                                temp.write((Nui8)0);
    #endif // LIBCAT_SECURITY
                                if(cr->outgoingPassword != String(""))
                                    temp.write(cr->outgoingPassword);

                                sys->sendRapid(temp.getData(), temp.getWriteOffset(),
                                    PL_RAPID, PT_BE_SURE, 0, address, false, false, read, 0);
                            }
                            else
                            {
                                // Failed, no connections available anymore
                                msg = N_new SocketMessage(sizeof(char));
                                msg->mData[0] = ID_CONNECTION_ATTEMPT_FAILED; // Attempted a connection and couldn't
                                msg->mBitCount = (sizeof(char) * 8);
                                msg->mAddress = cr->mAddress;
                                msg->mID = id;

                                sys->mReceiveMute.lock();
                                sys->mReceiveList.Push(msg);
                                sys->mReceiveMute.unlock();
                            }
                        }
                        sys->mRequestConnectMutex.lock();

                        for(Nui32 k = 0; k < sys->mRequestConnectList.Size(); ++k)
                        {
                            if(sys->mRequestConnectList[k]->mAddress == *address)
                            {
                                sys->mRequestConnectList.RemoveAtIndex(k);
                                break;
                            }
                        }
                        sys->mRequestConnectMutex.unlock();
    #if LIBCAT_SECURITY == 1
                        CAT_AUDIT_PRINTF("AUDIT: Deleting client_handshake object %x and cr->client_handshake object %x\n", client_handshake, cr->client_handshake);
                        N_delete client_handshake;
                        N_delete cr->client_handshake;
    #endif // LIBCAT_SECURITY
                        N_delete cr;
                        break;
                    }
                }
                if(unlock)
                    sys->mRequestConnectMutex.unlock();

                return true;
            }
            else if ((Nui8)(data)[0] == (Token)ID_CONNECTION_ATTEMPT_FAILED ||
                (Nui8)(data)[0] == (Token)ID_NO_FREE_INCOMING_CONNECTIONS ||
                (Nui8)(data)[0] == (Token)ID_CONNECTION_BANNED ||
                (Nui8)(data)[0] == (Token)ID_ALREADY_CONNECTED ||
                (Nui8)(data)[0] == (Token)ID_INVALID_PASSWORD ||
                (Nui8)(data)[0] == (Token)ID_IP_RECENTLY_CONNECTED ||
                (Nui8)(data)[0] == (Token)ID_INCOMPATIBLE_PROTOCOL_VERSION)
            {

                NetSerializer out(data, length, false);
                out.skipRead(1, 0);
                out.skipRead(sizeof(OFFLINE_MESSAGE_DATA_ID), 0);
                if((Nui8)(data)[0] == (Token)ID_INCOMPATIBLE_PROTOCOL_VERSION)
                    out.skipRead(1, 0);

                UniqueID id;
                out.read(id);

                ConnectRequest * cr;
                bool connectionAttemptCancelled = false;
                
                sys->mRequestConnectMutex.lock();
                for(i = 0; i <  sys->mRequestConnectList.Size(); ++i)
                {
                    cr = sys->mRequestConnectList[i];
                    if(cr->actionToTake == ConnectRequest::CONNECT && cr->mAddress== *address)
                    {
                        connectionAttemptCancelled = true;
                        sys->mRequestConnectList.RemoveAtIndex(i);
    #if LIBCAT_SECURITY == 1
                        CAT_AUDIT_PRINTF("AUDIT: Connection attempt canceled so deleting cr->client_handshake object %x\n", cr->client_handshake);
                        N_delete cr->client_handshake;
    #endif // LIBCAT_SECURITY
                        N_delete cr;
                        break;
                    }
                }
                sys->mRequestConnectMutex.unlock();

                if(connectionAttemptCancelled)
                {
                    // Tell user of connection attempt failed
                    msg = N_new SocketMessage(sizeof(char));
                    msg->mData[0] = data[0]; // Attempted a connection and couldn't
                    msg->mBitCount = (sizeof(char) * 8);
                    msg->mAddress = address;
                    msg->mID = id;

                    sys->mReceiveMute.lock();
                    sys->mReceiveList.Push(msg);
                    sys->mReceiveMute.unlock();
                }
            }
            else if((Nui8)(data)[0] == ID_OPEN_CONNECTION_REQUEST_1 && length > (1 + sizeof(OFFLINE_MESSAGE_DATA_ID)))
            {
                char remoteProtocol = data[1 + sizeof(OFFLINE_MESSAGE_DATA_ID)];
                if(remoteProtocol != N_NET_PROTOCOL_VER)
                {
                    NetSerializer out;
                    out.write((Token)ID_INCOMPATIBLE_PROTOCOL_VERSION);
                    out.write((Nui8)N_NET_PROTOCOL_VER);
                    out.write((const Nui8 *)OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                    out.write(UniqueID::INVALID);

                    //for(i = 0; i < sys->mRawPluginList.Size(); ++i)
                    //    sys->mRawPluginList[i]->onRawSend(out.getData(), out.getWriteOffset(), address);

                    SocketSendData bsp;
                    bsp.mData = out.getData();
                    bsp.mLength = out.getSize();
                    bsp.mAddress = address;

                    socket->send(&bsp);
                    return true;
                }

                //for(i = 0; i < sys->mRawPluginList.Size(); ++i)
                //    sys->mRawPluginList[i]->onRawReceive(data, length * 8, address);

                NetSerializer out;
                out.write((Token)ID_OPEN_CONNECTION_REPLY_1);
                out.write((const Nui8 *)OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                out.write(UniqueID::INVALID);
    #if LIBCAT_SECURITY == 1
                if (sys->_using_security)
                {
                    out.write((Nui8) 1); // HasCookie Yes
                    // write cookie
                    Nui32 cookie = sys->_cookie_jar->Generate(&address.mSrc, sizeof(address.mSrc));
                    CAT_AUDIT_PRINTF("AUDIT: Writing cookie %i to %i:%i\n", cookie, address);
                    out.write(cookie);
                    // write my public key
                    out.write((const Nui8 *) sys->my_public_key,sizeof(sys->my_public_key));
                }
                else
    #endif // LIBCAT_SECURITY
                    out.write((Nui8) 0);  // HasCookie oN

                // MTU. Lower MTU if it is exceeds our own limit
                if (length + UDP_HEADER_SIZE > N_Net_MaxMtuSize)
                    out.write((Nui16)N_Net_MaxMtuSize);
                else
                    out.write((Nui16)length + UDP_HEADER_SIZE);

                //for(i = 0; i < sys->mRawPluginList.Size(); ++i)
                //    sys->mRawPluginList[i]->onRawSend(out.getData(), out.getWriteOffset(), address);

                SocketSendData bsp;
                bsp.mData = out.getData();
                bsp.mLength = out.getSize();
                bsp.mAddress = address;
                socket->send(&bsp);
            }
            else if((Nui8)(data)[0] == ID_OPEN_CONNECTION_REQUEST_2)
            {
                Address * bindingAddress;
                UniqueID id;
                NetSerializer out;
                NetSerializer bs(data, length, false);
                bs.skipRead(1, 0);
                bs.skipRead(sizeof(OFFLINE_MESSAGE_DATA_ID), 0);

                bool requiresSecurityOfThisClient = false;
    #if LIBCAT_SECURITY == 1
                char remoteHandshakeChallenge[cat::EasyHandshake::CHALLENGE_BYTES];

                if(sys->_using_security)
                {
                    String str1;
                    address->read(str1, false);
                    requiresSecurityOfThisClient = sys->mSecure->isExist(str1) == false;

                    Nui32 cookie;
                    bs.read(cookie);
                    CAT_AUDIT_PRINTF("AUDIT: Got cookie %i from %i:%i\n", cookie, address);
                    if (sys->_cookie_jar->Verify(&address.mSrc, sizeof(address.mSrc), cookie)==false)
                    {
                        return true;
                    }
                    CAT_AUDIT_PRINTF("AUDIT: Cookie good!\n");

                    Nui8 clientWroteChallenge;
                    bs.read(clientWroteChallenge);

                    if(requiresSecurityOfThisClient == true && clientWroteChallenge == 0)
                    {
                        // Fail, client doesn't support security, and it is required
                        return true;
                    }

                    if(clientWroteChallenge)
                    {
                        bs.read((Nui8 *)remoteHandshakeChallenge, cat::EasyHandshake::CHALLENGE_BYTES);
    #ifdef CAT_AUDIT
                        printf("AUDIT: RECV CHALLENGE ");
                        for (int ii = 0; ii < sizeof(remoteHandshakeChallenge); ++ii)
                        {
                            printf("%02x", (cat::u8)remoteHandshakeChallenge[ii]);
                        }
                        printf("\n");
    #endif
                    }
                }
    #endif // LIBCAT_SECURITY
                bs.read(bindingAddress);
                Nui16 mtu;
                bs.read(mtu);
                bs.read(id);

                Remote * rssFromSA = sys->getRemoteSystem(address, true, true);
                Remote * rssFromGuid = sys->getRemoteSystem(id, true);
                bool IPAddrInUse = rssFromSA != 0 && rssFromSA->mActive;
                bool GUIDInUse = rssFromGuid != 0 && rssFromGuid->mActive;

                // IPAddrInUse, GuidInUse, outcome
                // TRUE,	  , TRUE	 , ID_OPEN_CONNECTION_REPLY if they are the same, else ID_ALREADY_CONNECTED
                // FALSE,     , TRUE     , ID_ALREADY_CONNECTED (someone else took this guid)
                // TRUE,	  , FALSE	 , ID_ALREADY_CONNECTED (silently disconnected, restarted rakNet)
                // FALSE	  , FALSE	 , Allow connection

                int outcome;
                if(IPAddrInUse & GUIDInUse)
                {
                    if(rssFromSA == rssFromGuid && rssFromSA->mMode == CM_Identify)
                    {
                        // ID_OPEN_CONNECTION_REPLY if they are the same
                        outcome = 1;

                        // Note to self: If CM_Request, this means two systems attempted to connect to each other at the same time, and one finished first.
                        // Returns ID)_CONNECTION_REQUEST_ACCEPTED to one system, and ID_ALREADY_CONNECTED followed by ID_NEW_INCOMING_CONNECTION to another
                    }
                    else
                    {
                        // ID_ALREADY_CONNECTED (restarted raknet, connected again from same ip, plus someone else took this guid)
                        outcome = 2;
                    }
                }
                else if (IPAddrInUse == false && GUIDInUse == true)
                {
                    // ID_ALREADY_CONNECTED (someone else took this guid)
                    outcome = 3;
                }
                else if (IPAddrInUse == true && GUIDInUse == false)
                {
                    // ID_ALREADY_CONNECTED (silently disconnected, restarted rakNet)
                    outcome = 4;
                }
                else
                {
                    // Allow connection
                    outcome = 0;
                }

                NetSerializer bsAnswer;
                bsAnswer.write((Token)ID_OPEN_CONNECTION_REPLY_2);
                bsAnswer.write((const Nui8 *)OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                bsAnswer.write(UniqueID::INVALID);
                bsAnswer.write(address);
                bsAnswer.write(mtu);
                bsAnswer.write(requiresSecurityOfThisClient);

                if(outcome == 1)
                {
                    // Duplicate connection request msg from packetloss
                    // send back the same answer
    #if LIBCAT_SECURITY == 1
                    if(requiresSecurityOfThisClient)
                    {
                        CAT_AUDIT_PRINTF("AUDIT: Resending public key and answer from packetloss.  Sending ID_OPEN_CONNECTION_REPLY_2\n");
                        bsAnswer.write((const Nui8 *)rssFromSA->answer, sizeof(rssFromSA->answer));
                    }
    #endif // LIBCAT_SECURITY

                    SocketSendData bsp;
                    bsp.mData = bsAnswer.getData();
                    bsp.mLength = bsAnswer.getSize();
                    bsp.mAddress = address;
                    socket->send(&bsp);

                    return true;
                }
                else if(outcome != 0)
                {
                    out.write((Token)ID_ALREADY_CONNECTED);
                    out.write((const Nui8 *)OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                    out.write(sys->mLocalHostID);

                    SocketSendData bsp;
                    bsp.mData = out.getData();
                    bsp.mLength = out.getSize();
                    bsp.mAddress = address;
                    socket->send(&bsp);

                    return true;
                }

                NCount count;
                RemoteList::iterator r, rend = sys->mActiveRemote.end();
                for(r = sys->mActiveRemote.begin(), count = 0; r != rend; ++r)
                {
                    if((*r)->mActive)
                    {
                        ++count;
                    }
                }

                if(count < sys->getMaxRemoteCount() == false)
                {
                    out.write((Token)ID_NO_FREE_INCOMING_CONNECTIONS);
                    out.write((const Nui8 *) OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                    out.write(sys->mLocalHostID);

                    SocketSendData bsp;
                    bsp.mData = out.getData();
                    bsp.mLength = out.getSize();
                    bsp.mAddress = address;
                    socket->send(&bsp);

                    return true;
                }

                bool recently = false;
                rssFromSA = sys->createConnect(address, socket, recently, bindingAddress, mtu, id, requiresSecurityOfThisClient);

                if(recently == true)
                {
                    out.write((Token)ID_IP_RECENTLY_CONNECTED);
                    out.write((const Nui8 *)OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                    out.write(sys->mLocalHostID);

                    SocketSendData bsp;
                    bsp.mData = out.getData();
                    bsp.mLength = out.getSize();
                    bsp.mAddress = address;
                    socket->send(&bsp);

                    return true;
                }

    #if LIBCAT_SECURITY == 1
                if (requiresSecurityOfThisClient)
                {
                    CAT_AUDIT_PRINTF("AUDIT: Writing public key.  Sending ID_OPEN_CONNECTION_REPLY_2\n");
                    if (sys->_server_handshake->ProcessChallenge(remoteHandshakeChallenge, rssFromSA->answer, rssFromSA->mPacketPrc.GetAuthenticatedEncryption() ))
                    {
                        CAT_AUDIT_PRINTF("AUDIT: Challenge good!\n");
                        // Keep going to OK block
                    }
                    else
                    {
                        CAT_AUDIT_PRINTF("AUDIT: Challenge BAD!\n");

                        // Unassign this remote system
                        sys->detach(address);
                        return true;
                    }

                    bsAnswer.write((const Nui8 *)rssFromSA->answer, sizeof(rssFromSA->answer));
                }
    #endif // LIBCAT_SECURITY
                SocketSendData bsp;
                bsp.mData = bsAnswer.getData();
                bsp.mLength = bsAnswer.getSize();
                bsp.mAddress = address;
                socket->send(&bsp);
            }
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void process(Address * address, const Nui8 * data, NCount length, NetworkSys * sys,
        SocketObj * socket, TimeDurUS read, NetSerializer & upload)
    {
#if LIBCAT_SECURITY == 1
    #ifdef CAT_AUDIT
        printf("AUDIT: RECV ");
        for(NCount i = 0; i < length; ++i)
        {
            printf("%02x", (cat::u8)data[i]);
        }
        printf("\n");
    #endif
#endif
        N_assert(address->getHSBPort(), "error");
        if(!process(address, data, length, sys, socket, read))
        {
            Remote * remote = sys->getRemoteSystem(address, true, true);
            if (remote)
            {
                remote->mPacketPrc.process(address, data, length, sys->mRawPluginList,
                    remote->mMTU, socket, &rnr, read, upload);
            }
        }
    }
    //------------------------------------------------------------------------
    void NetworkSys::releaseSocket()
    {
        SocketList::iterator i, iend = mSocketList.end();
        for(i = mSocketList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mSocketList.clear();
    }
    //------------------------------------------------------------------------
    void NetworkSys::process(SocketMessage * msg, PluginList & list)
    {
        NCount i, iend = list.Size();
        for(i = 0; i < iend; ++i)
        {
            switch(msg->mData[0])
            {
            case ID_DISCONNECTION_NOTIFICATION:
                list[i]->onConnectClose(msg->mAddress, msg->mID, CCT_Shutdown);
                break;
            case ID_CONNECTION_LOST:
                list[i]->onConnectClose(msg->mAddress, msg->mID, CCT_Exception);
                break;
            case ID_NEW_INCOMING_CONNECTION:
                list[i]->onConnect(msg->mAddress, msg->mID, true);
                break;
            case ID_CONNECTION_REQUEST_ACCEPTED:
                list[i]->onConfirm(msg->mAddress, msg->mID, false);
                break;
            case ID_CONNECTION_ATTEMPT_FAILED:
                list[i]->onConnectFail(msg, CFT_Fail);
                break;
            case ID_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY:
                list[i]->onConnectFail(msg, CFT_Encrypt);
                break;
            case ID_OUR_SYSTEM_REQUIRES_SECURITY:
                list[i]->onConnectFail(msg, CFT_Encrypt);
                break;
            case ID_PUBLIC_KEY_MISMATCH:
                list[i]->onConnectFail(msg, CFT_Encrypt);
                break;
            case ID_ALREADY_CONNECTED:
                list[i]->onConnectFail(msg, CFT_Duplicate);
                break;
            case ID_NO_FREE_INCOMING_CONNECTIONS:
                list[i]->onConnectFail(msg, CFT_Max);
                break;
            case ID_CONNECTION_BANNED:
                list[i]->onConnectFail(msg, CFT_Ban);
                break;
            case ID_INVALID_PASSWORD:
                list[i]->onConnectFail(msg, CFT_Password);
                break;
            case ID_INCOMPATIBLE_PROTOCOL_VERSION:
                list[i]->onConnectFail(msg, CFT_Proto);
                break;
            case ID_IP_RECENTLY_CONNECTED:
                list[i]->onConnectFail(msg, CFT_Recent);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
}
}