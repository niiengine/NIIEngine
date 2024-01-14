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
#if _RAKNET_SUPPORT_NatTypeDetectionServer==1

#include "NiiPreProcess.h"
#include "NatTypeDetectionServer.h"
#include "NiiSharedPtr.h"
#include "NiiNetLinkBase.h"
#include "NiiNetCommon.h"
#include "NiiNetSerializer.h"

// #define NTDS_VERBOSE
namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    NatTypeDetectionServer::NatTypeDetectionServer()
    {
        s1p2=s2p3=s3p4=s4p5=0;
    }
    //------------------------------------------------------------------------
    NatTypeDetectionServer::~NatTypeDetectionServer()
    {
        Shutdown();
    }
    //------------------------------------------------------------------------
    void NatTypeDetectionServer::Startup(const char * nonRakNetIP2,
        const char * nonRakNetIP3, const char * nonRakNetIP4
    #ifdef N_PLAT_NACL
        ,NPP_Instance chromeInstance
    #endif
        )
    {
        DataStructures::List<SocketObj *> sockets;
        mPrc2->getSockets(sockets);
        String str;
        sockets[0]->GetBoundAddress().read(str, false);
        s1p2 = CreateNonblockingBoundSocket(str,
    #ifdef N_PLAT_NACL
            chromeInstance,
    #endif
            this);

        s2p3 = CreateNonblockingBoundSocket(nonRakNetIP2,
    #ifdef N_PLAT_NACL
            chromeInstance,
    #endif
            this);


        s3p4 = CreateNonblockingBoundSocket(nonRakNetIP3,
    #ifdef N_PLAT_NACL
            chromeInstance,
    #endif
            this);

        s4p5 = CreateNonblockingBoundSocket(nonRakNetIP4,
    #ifdef N_PLAT_NACL
            chromeInstance,
    #endif
            this);

        strcpy(s3p4Address, nonRakNetIP3);

        #if !defined(N_PLAT_NACL) && !defined(N_PLAT_WINRT)
        if (s3p4->IsBerkleySocket())
            ((RNS2_Berkley*) s3p4)->CreateRecvPollingThread(0);
        #endif
    }
    //------------------------------------------------------------------------
    void NatTypeDetectionServer::Shutdown()
    {
        if(s1p2 != 0)
        {
            N_delete s1p2;
            s1p2 = 0;
        }
        if(s2p3 != 0)
        {
            N_delete s2p3;
            s2p3 = 0;
        }
        if(s3p4 != 0)
        {
    #if !defined(N_PLAT_NACL) && !defined(N_PLAT_WINRT)
            if(s3p4->IsBerkleySocket())
                ((RNS2_Berkley *)s3p4)->BlockOnStopRecvPollingThread();
    #endif
            N_delete s3p4;
            s3p4 = 0;
        }
        if(s4p5 != 0)
        {
            N_delete s4p5;
            s4p5 = 0;
        }

        bufferedPacketsMutex.Lock();
        while (bufferedPackets.Size())
            N_delete bufferedPackets.Pop();
        bufferedPacketsMutex.Unlock();
    }
    //------------------------------------------------------------------------
    void NatTypeDetectionServer::update(void)
    {
        int i = 0;
        TimeDurMS time = N_Engine().getTimer().getMS();
        NetSerializer bs;
        Address * boundAddress;

        SocketMessageData * recv;
        bufferedPacketsMutex.Lock();
        if(bufferedPackets.Size() > 0)
            recv = bufferedPackets.Pop();
        else
            recv = 0;
        bufferedPacketsMutex.Unlock();

        while(recv)
        {
            Address * senderAddr = recv->mAddress;
            Nui8 * data = recv->mData;
            if(data[0] == NAT_TYPE_PORT_RESTRICTED && recv->mSocket == s3p4)
            {
                NetSerializer bsIn(data, recv->mCount, false);
                UniqueID senderGuid;
                bsIn.skipRead(1, 0);
                bool readSuccess = bsIn.read(senderGuid);
                N_assert(readSuccess);
                if(readSuccess)
                {
                    Nui32 i = GetDetectionAttemptIndex(senderGuid);
                    if(i != (Nui32)-1)
                    {
                        bs.seekWrite(0);
                        bs.seekRead(0);
                        bs.write((Nui8) ID_NAT_TYPE_DETECTION_RESULT);
                        // If different, then symmetric
                        if(senderAddr!=natDetectionAttempts[i].mAddress)
                        {

    #ifdef NTDS_VERBOSE
                            printf("Determined client is symmetric\n");
    #endif
                            bs.write((Nui8) NAT_TYPE_SYMMETRIC);
                        }
                        else
                        {
                            // else port restricted
    #ifdef NTDS_VERBOSE

                            printf("Determined client is port restricted\n");
    #endif
                            bs.write((Nui8) NAT_TYPE_PORT_RESTRICTED);
                        }

                        mPrc2->send(&bs, PL_HIGH, PT_BE_SURE, 0, natDetectionAttempts[i].mAddress,false);

                        // Done
                        natDetectionAttempts.RemoveAtIndexFast(i);
                    }
                    else
                    {
                        //		N_assert("i==0 in update when looking up UniqueID in NatTypeDetectionServer.cpp. Either a bug or a late resend" && 0);
                    }
                }
                else
                {
                    //	N_assert("Didn't read UniqueID in update in NatTypeDetectionServer.cpp. SocketMessage format error" && 0);
                }
            }

            dealloc(recv);
            bufferedPacketsMutex.Lock();
            if(bufferedPackets.Size() > 0)
                recv = bufferedPackets.Pop();
            else
                recv = 0;
            bufferedPacketsMutex.Unlock();
        }

        while(i < (int) natDetectionAttempts.Size())
        {
            if(time > natDetectionAttempts[i].nextStateTime)
            {
                SocketSendData bsp;
                natDetectionAttempts[i].detectionState=(NATDetectionState)((int)natDetectionAttempts[i].detectionState+1);
                natDetectionAttempts[i].nextStateTime=time+natDetectionAttempts[i].timeBetweenAttempts;
                Address * saOut;
                Nui8 c;
                bs.seekWrite(0);
                bs.seekRead(0);
                switch (natDetectionAttempts[i].detectionState)
                {
                case STATE_TESTING_NONE_1:
                case STATE_TESTING_NONE_2:
                    c = NAT_TYPE_NONE;

    #ifdef NTDS_VERBOSE
                    printf("Testing NAT_TYPE_NONE\n");
    #endif
                    // S4P5 sends to C2. If arrived, no NAT. Done. (Else S4P5 potentially banned, do not use again).
                    saOut = natDetectionAttempts[i].mAddress;
                    saOut.setHSBPort(natDetectionAttempts[i].c2Port);
                    bsp.mData = (char*) &c;
                    bsp.mLength = 1;
                    bsp.mAddress = saOut;
                    s4p5->send(&bsp);
                    break;
                case STATE_TESTING_FULL_CONE_1:
                case STATE_TESTING_FULL_CONE_2:

    #ifdef NTDS_VERBOSE
                    printf("Testing NAT_TYPE_FULL_CONE\n");
    #endif
                    bs.write((Token)ID_OUT_OF_BAND_INTERNAL);
                    bs.write(mPrc2->getGuid());
                    bs.write((const Nui8 *)OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                    
                    bs.write((Nui8) ID_NAT_TYPE_DETECT);
                    bs.write((Nui8) NAT_TYPE_FULL_CONE);
                    // S2P3 sends to C1 (Different address, different port, to previously used port on client). If received, Full-cone nat. Done.  (Else S2P3 potentially banned, do not use again).
                    saOut = natDetectionAttempts[i].mAddress;
                    saOut.setHSBPort(natDetectionAttempts[i].mAddress.getHSBPort());
                    bsp.mData = (char *)bs.getData();
                    bsp.mLength = bs.getSize();
                    bsp.mAddress = saOut;
                    s2p3->send(&bsp);
                    break;
                case STATE_TESTING_ADDRESS_RESTRICTED_1:
                case STATE_TESTING_ADDRESS_RESTRICTED_2:

    #ifdef NTDS_VERBOSE
                    printf("Testing NAT_TYPE_ADDRESS_RESTRICTED\n");
    #endif
                    bs.write((Token)ID_OUT_OF_BAND_INTERNAL);
                    bs.write(mPrc2->getGuid());
                    bs.write((const Nui8 *)OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                    
                    bs.write((Nui8) ID_NAT_TYPE_DETECT);
                    bs.write((Nui8) NAT_TYPE_ADDRESS_RESTRICTED);
                    // S1P2 sends to C1 (Same address, different port, to previously used port on client). If received, address-restricted cone nat. Done.
                    saOut=natDetectionAttempts[i].mAddress;
                    saOut.setHSBPort(natDetectionAttempts[i].mAddress.getHSBPort());
                    bsp.mData = (char *)bs.getData();
                    bsp.mLength = bs.getSize();
                    bsp.mAddress = saOut;
                    s1p2->send(&bsp);
                    break;
                case STATE_TESTING_PORT_RESTRICTED_1:
                case STATE_TESTING_PORT_RESTRICTED_2:
                    // C1 sends to S3P4. If address of C1 as seen by S3P4 is the same as the address of C1 as seen by S1P1, then port-restricted cone nat. Done
    #ifdef NTDS_VERBOSE
                    printf("Testing NAT_TYPE_PORT_RESTRICTED\n");
    #endif
                    bs.write((Nui8)ID_NAT_TYPE_DETECTION_REQUEST);
                    bs.write(RakString::NonVariadic(s3p4Address));
                    bs.write(s3p4->GetBoundAddress().getHSBPort());
                    mPrc2->send(&bs, PL_HIGH, PT_BE_SURE, 0, natDetectionAttempts[i].mAddress,false);
                    break;
                default:

    #ifdef NTDS_VERBOSE
                    printf("Warning, exceeded final check STATE_TESTING_PORT_RESTRICTED_2.\nExpected that client would have sent NAT_TYPE_PORT_RESTRICTED on s3p4.\nDefaulting to Symmetric\n");
    #endif
                    bs.write((Nui8) ID_NAT_TYPE_DETECTION_RESULT);
                    bs.write((Nui8) NAT_TYPE_SYMMETRIC);
                    mPrc2->send(&bs, PL_HIGH, PT_BE_SURE, 0, natDetectionAttempts[i].mAddress,false);
                    natDetectionAttempts.RemoveAtIndexFast(i);
                    i--;
                    break;
                }

            }
            i++;
        }
    }
    //------------------------------------------------------------------------
    ConnectReceiveType NatTypeDetectionServer::onMessage(SocketMessage * msg)
    {
        switch (msg->mData[0])
        {
        case ID_NAT_TYPE_DETECTION_REQUEST:
            OnDetectionRequest(msg);
            return CRT_Destroy;
        }
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void NatTypeDetectionServer::onConnectClose(const Address & address, UniqueID id,
        ConnectCloseType lostConnectionReason )
    {
        (void) lostConnectionReason;
        (void) id;

        Nui32 i = GetDetectionAttemptIndex(address);
        if (i==(Nui32)-1)
            return;
        natDetectionAttempts.RemoveAtIndexFast(i);
    }
    //------------------------------------------------------------------------
    void NatTypeDetectionServer::OnDetectionRequest(SocketMessage * msg)
    {
        Nui32 i = GetDetectionAttemptIndex(msg->mAddress);

        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        bool isRequest=false;
        bsIn.read(isRequest);
        if (isRequest)
        {
            if (i!=(Nui32)-1)
                return; // Already in progress

            NATDetectionAttempt nda;
            nda.detectionState = STATE_NONE;
            nda.mAddress = msg->mAddress;
            nda.mID = msg->mID;
            bsIn.read(nda.c2Port);
            nda.nextStateTime = 0;
            nda.timeBetweenAttempts = mPrc2->getLastPing(nda.mAddress)*3+50;
            natDetectionAttempts.Push(nda);
        }
        else
        {
            if (i==(Nui32)-1)
                return; // Unknown
            // They are done
            natDetectionAttempts.RemoveAtIndexFast(i);
        }
    }
    //------------------------------------------------------------------------
    Nui32 NatTypeDetectionServer::GetDetectionAttemptIndex(const Address & sa)
    {
        for (NCount i = 0; i < natDetectionAttempts.Size(); ++i)
        {
            if (natDetectionAttempts[i].mAddress == sa)
                return i;
        }
        return (Nui32) -1;
    }
    //------------------------------------------------------------------------
    Nui32 NatTypeDetectionServer::GetDetectionAttemptIndex(UniqueID id)
    {
        for (NCount i = 0; i < natDetectionAttempts.Size(); ++i)
        {
            if (natDetectionAttempts[i].mID == id)
                return i;
        }
        return (Nui32) -1;
    }
    //------------------------------------------------------------------------
    void NatTypeDetectionServer::destroyMessage(SocketMessageData * msg)
    {
        N_delete msg;
    }
    //------------------------------------------------------------------------
    SocketMessageData * NatTypeDetectionServer::createMessage()
    {
        return N_new SocketMessageData();
    }
    //------------------------------------------------------------------------
    void NatTypeDetectionServer::onMessage(SocketMessageData * msg)
    {
        bufferedPacketsMutex.Lock();
        bufferedPackets.push_back(msg);
        bufferedPacketsMutex.Unlock();
    }
    //------------------------------------------------------------------------
#endif // _RAKNET_SUPPORT_*
