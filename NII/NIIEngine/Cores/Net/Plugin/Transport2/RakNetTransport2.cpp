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
#if _RAKNET_SUPPORT_TelnetTransport==1

#include "RakNetTransport2.h"

#include "NiiNetLinkBase.h"
#include "NiiNetSerializer.h"
#include "NiiNetCommon.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "LinuxStrings.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    RakNetTransport2::RakNetTransport2()
    {
    }
    //------------------------------------------------------------------------
    RakNetTransport2::~RakNetTransport2()
    {
        Stop();
    }
    //------------------------------------------------------------------------
    bool RakNetTransport2::Start(Nui16 port, bool serverMode)
    {
        (void) port;
        (void) serverMode;
        return true;
    }
    //------------------------------------------------------------------------
    void RakNetTransport2::Stop(void)
    {
        newConnections.Clear();
        lostConnections.Clear();
        for (NCount i = 0; i < packetQueue.Size(); ++i)
        {
            N_free(packetQueue[i]->mData);
            N_delete packetQueue[i];
        }
        packetQueue.Clear();
    }
    //------------------------------------------------------------------------
    void RakNetTransport2::send(Address * address, const Nui8 * data, ...)
    {
        if (data==0 || data[0]==0) return;

        char text[REMOTE_MAX_TEXT_INPUT];
        va_list ap;
        va_start(ap, data);
        _vsnprintf(text, REMOTE_MAX_TEXT_INPUT, data, ap);
        va_end(ap);
        text[REMOTE_MAX_TEXT_INPUT-1]=0;

        NetSerializer str;
        str.write((Token)ID_TRANSPORT_STRING);
        str.write(text, (int) strlen(text));
        str.write((Nui8) 0); // Null terminate the string
        mPrc2->send(&str, PL_MEDIUM, PT_ORDER_MAKE_SURE, 0, address, (address==0 || address->isInvalid())!=0);
    }
    //------------------------------------------------------------------------
    void RakNetTransport2::close(Address * address)
    {
        mPrc2->close(address, true, 0);
    }
    //------------------------------------------------------------------------
    SocketMessage * RakNetTransport2::receive()
    {
        if(packetQueue.Size() == 0)
            return 0;
        return packetQueue.Pop();
    }
    //------------------------------------------------------------------------
    void RakNetTransport2::onServerConnect(Address * address)
    {
        if(newConnections.Size())
            return newConnections.Pop();
        return 0;
    }
    //------------------------------------------------------------------------
    void RakNetTransport2::onConnectClose(Address * address)
    {
        if (lostConnections.Size())
            return lostConnections.Pop();
        return 0;
    }
    //------------------------------------------------------------------------
    ConnectReceiveType RakNetTransport2::onMessage(SocketMessage * msg)
    {
        switch (msg->mData[0])
        {
        case ID_TRANSPORT_STRING:
            {
            if(msg->mLength == sizeof(Token))
                return CRT_Destroy;

            SocketMessage * p = N_new SocketMessage();
            *p = *msg;
            p->mBitCount-=8;
            --p->mLength;
            p->mData=(Nui8 *) N_alloc(p->mLength);
            memcpy(p->mData, msg->mData+1, p->mLength);
            packetQueue.Push(p);

            }
            return CRT_Destroy;
        }
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void RakNetTransport2::onConnectClose(const Address & address,
        UniqueID id, ConnectCloseType lostConnectionReason)
    {
        (void) id;
        (void) lostConnectionReason;
        lostConnections.Push(address);
    }
    //------------------------------------------------------------------------
    void RakNetTransport2::onConnect(const Address & address, UniqueID id)
    {
        (void) id;
        newConnections.Push(address);
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*
