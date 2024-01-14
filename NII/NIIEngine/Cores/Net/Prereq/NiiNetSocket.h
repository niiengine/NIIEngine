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

#ifndef _NII_NET_SOCKET_H_
#define _NII_NET_SOCKET_H_

#include "NiiNetSocket.h"

namespace NII
{
    enum RNS2BindResult
    {
        BR_SUCCESS,
        BR_REQUIRES_RAKNET_SUPPORT_IPV6_DEFINE,
        BR_FAILED_TO_BIND_SOCKET,
        BR_FAILED_SEND_TEST,
    };

    struct SocketSendData
    {
        SocketSendData()
        {
            ttl = 0;
        }
        const Nui8 * mData;
        NCount mLength;
        Address * mAddress;
        int ttl;
    };

    typedef int RNS2SendResult;

    enum RNS2Type
    {
        RNS2T_WINDOWS_STORE_8,
        RNS2T_PS3,
        RNS2T_PS4,
        RNS2T_CHROME,
        RNS2T_VITA,
        RNS2T_XBOX_360,
        RNS2T_XBOX_720,
        RNS2T_WINDOWS,
        RNS2T_LINUX,

        IPV4,
        IPV6,
    };

    struct SocketMessageData
    {
        Nui8 mData[N_Net_MaxMtuSize];
        NCount mCount;
        TimeDurUS mRead;
        
        Address * mAddress;
        SocketObj * mSocket;
    };

    class _EngineAPI RNS2EventHandler
    {
    public:
        RNS2EventHandler() {}
        virtual ~RNS2EventHandler() {}

        virtual SocketMessageData * createMessage() = 0;
        virtual void destroyMessage(SocketMessageData * obj) = 0;
        virtual void onMessage(SocketMessageData * obj) = 0;
    };

    class SocketObj : public NetAlloc
    {
    public:
        SocketObj();
        virtual ~SocketObj();

        bool IsBerkleySocket() const;

        virtual RNS2SendResult send(SocketSendData * param) = 0;

        void SetSocketType(RNS2Type t);
        
        RNS2Type GetSocketType() const;

        // In order for the handler to trigger, some platforms must call PollRecvFrom, some platforms this create an internal thread.
        void SetRecvEventHandler(RNS2EventHandler * hander);

        RNS2EventHandler * GetEventHandler() const;

        const Address * GetBoundAddress() const;
    protected:
        RNS2Type socketType;
        Address * boundAddress;
        RNS2EventHandler * eventHandler;
    };
    typedef vector<SocketObj *>::type SocketObjList;
}