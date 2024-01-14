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
#if _RAKNET_SUPPORT_NatTypeDetectionClient == 1

#include "NiiPreProcess.h"
#include "NatTypeDetectionClient.h"
#include "NiiSharedPtr.h"
#include "NiiNetSerializer.h"
#include "RakString.h"
#include "NiiNetLinkBase.h"
#include "NiiNetCommon.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    NatTypeDetectionClient::NatTypeDetectionClient()
    {
        c2=0;
    }
    //------------------------------------------------------------------------
    NatTypeDetectionClient::~NatTypeDetectionClient()
    {
        if (c2!=0)
        {
            N_delete c2;
        }
    }
    //------------------------------------------------------------------------
    void NatTypeDetectionClient::DetectNATType(Address * _serverAddress)
    {
        if(IsInProgress())
            return;

        if(c2 == 0)
        {
            DataStructures::List<SocketObj *> sockets;
            mPrc2->getSockets(sockets);
            String str;
            sockets[0]->GetBoundAddress().read(str, false);
            c2 = CreateNonblockingBoundSocket(str.c_str()
    #ifdef N_PLAT_NACL
                , sockets[0]->chromeInstance
    #endif
                ,this
                );
        }

    #if !defined(N_PLAT_NACL) && !defined(N_PLAT_WINRT)
        if (c2->IsBerkleySocket())
            ((RNS2_Berkley *) c2)->CreateRecvPollingThread(0);
    #endif
        serverAddress = _serverAddress;
        NetSerializer bs;
        bs.write((Nui8)ID_NAT_TYPE_DETECTION_REQUEST);
        bs.write1(); // IsRequest
        bs.write(c2->GetBoundAddress().getHSBPort());
        mPrc2->send(&bs, PL_MEDIUM, PT_BE_SURE, 0, serverAddress, false);
    }
    //------------------------------------------------------------------------
    void NatTypeDetectionClient::OnCompletion(NATTypeDetectionResult result)
    {
        SocketMessage * p = createMessage(sizeof(Token) + sizeof(Nui8) * 2);
        //printf("Returning nat detection result to the user\n");
        p->mData[0] = ID_NAT_TYPE_DETECTION_RESULT;
        p->mAddress = serverAddress;
        p->mAddress.mIndex = (Nindex)-1;
        p->mID = mPrc2->getGUID(serverAddress);
        p->mData[1] = (Nui8) result;
        p->mLocal = true;
        mPrc2->addMessage(p, true);

        // Symmetric and port restricted are determined by server, so no need to notify server we are done
        if (result != NAT_TYPE_PORT_RESTRICTED && result != NAT_TYPE_SYMMETRIC)
        {
            // Otherwise tell the server we got this message, so it stops sending tests to us
            NetSerializer bs;
            bs.write((Nui8)ID_NAT_TYPE_DETECTION_REQUEST);
            bs.write0(); // Done
            mPrc2->send(&bs, PL_HIGH, PT_BE_SURE, 0, serverAddress, false);
        }

        Shutdown();
    }
    //------------------------------------------------------------------------
    bool NatTypeDetectionClient::IsInProgress(void) const
    {
        return serverAddress!=0 && serverAddress->isInvalid();
    }
    //------------------------------------------------------------------------
    void NatTypeDetectionClient::update()
    {
        if(IsInProgress())
        {
            SocketMessageData *recvStruct;
            bufferedPacketsMutex.Lock();
            if(bufferedPackets.Size() > 0)
                recvStruct = bufferedPackets.Pop();
            else
                recvStruct = 0;
            bufferedPacketsMutex.Unlock();
            while(recvStruct)
            {
                if(recvStruct->mCount == 1 && recvStruct->mData[0] == NAT_TYPE_NONE)
                {
                    OnCompletion(NAT_TYPE_NONE);
                    N_assert(IsInProgress() == false);
                }
                dealloc(recvStruct);

                bufferedPacketsMutex.Lock();
                if(bufferedPackets.Size() > 0)
                    recvStruct = bufferedPackets.Pop();
                else
                    recvStruct = 0;
                bufferedPacketsMutex.Unlock();
            }
        }
    }
    //------------------------------------------------------------------------
    ConnectReceiveType NatTypeDetectionClient::onMessage(SocketMessage *msg)
    {
        if (IsInProgress())
        {
            switch (msg->mData[0])
            {
            case ID_OUT_OF_BAND_INTERNAL:
                {
                    if (msg->mLength>=3 && msg->mData[1]==ID_NAT_TYPE_DETECT)
                    {
                        OnCompletion((NATTypeDetectionResult)msg->mData[2]);
                        return CRT_Destroy;
                    }
                }
                break;
            case ID_NAT_TYPE_DETECTION_RESULT:
                if (msg->mLocal == false)
                {
                    OnCompletion((NATTypeDetectionResult)msg->mData[1]);
                    return CRT_Destroy;
                }
                else
                    break;
            case ID_NAT_TYPE_DETECTION_REQUEST:
                OnTestPortRestricted(msg);
                return CRT_Destroy;
            }
        }
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void NatTypeDetectionClient::onConnectClose(const Address & address,
        UniqueID id, ConnectCloseType lostConnectionReason)
    {
        (void) lostConnectionReason;
        (void) id;

        if (IsInProgress() && address == serverAddress)
            Shutdown();
    }
    //------------------------------------------------------------------------
    void NatTypeDetectionClient::shutdown()
    {
        Shutdown();
    }
    //------------------------------------------------------------------------
    void NatTypeDetectionClient::onDetach()
    {
        Shutdown();
    }
    //------------------------------------------------------------------------
    void NatTypeDetectionClient::OnTestPortRestricted(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        RakString s3p4StrAddress;
        bsIn.read(s3p4StrAddress);
        Nui16 s3p4Port;
        bsIn.read(s3p4Port);

        DataStructures::List<SocketObj *> sockets;
        mPrc2->getSockets(sockets);
        Address * s3p4Addr = sockets[0]->GetBoundAddress();
        s3p4Addr.write(s3p4StrAddress.C_String(), s3p4Port);

        // send off the RakNet socket to the specified address, message is unformatted
        // Server does this twice, so don't have to unduly worry about packetloss
        NetSerializer bsOut;
        bsOut.write((Token) NAT_TYPE_PORT_RESTRICTED);
        bsOut.write(mPrc2->getGUID((Address *)0));

        SocketSendData bsp;
        bsp.mData = bsOut.getData();
        bsp.mLength = bsOut.getSize();
        bsp.mAddress=s3p4Addr;
        sockets[0]->send(&bsp);
    }
    //------------------------------------------------------------------------
    void NatTypeDetectionClient::Shutdown(void)
    {
        serverAddress = 0;
        if(c2 != 0)
        {
    #if !defined(N_PLAT_NACL) && !defined(N_PLAT_WINRT)
            if (c2->IsBerkleySocket())
                ((RNS2_Berkley *)c2)->BlockOnStopRecvPollingThread();
    #endif
            N_delete c2;
            c2 = 0;
        }

        bufferedPacketsMutex.Lock();
        while(bufferedPackets.Size())
            N_delete bufferedPackets.Pop();
        bufferedPacketsMutex.Unlock();
    }

    //------------------------------------------------------------------------
    void NatTypeDetectionClient::destroyMessage(SocketMessageData * s)
    {
        N_delete s;
    }
    //------------------------------------------------------------------------
    SocketMessageData * NatTypeDetectionClient::createMessage()
    {
        return N_new SocketMessageData();
    }
    //------------------------------------------------------------------------
    void NatTypeDetectionClient::onMessage(SocketMessageData * recvStruct)
    {
        bufferedPacketsMutex.Lock();
        bufferedPackets.Push(recvStruct);
        bufferedPacketsMutex.Unlock();
    }
    //------------------------------------------------------------------------
#endif // _RAKNET_SUPPORT_*