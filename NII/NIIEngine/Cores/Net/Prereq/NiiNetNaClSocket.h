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

#ifndef _NII_NET_NACL_SOCKET_H_
#define _NII_NET_NACL_SOCKET_H_

#include "NiiPreInclude.h"

namespace NII
{
namespace NII_NET
{
    class NaClSendSocketParam;

    struct NaClSocketParam
    {
        
        NPP_Instance nativeClientInstance;
        RNS2EventHandler * eventHandler;
        const char * forceHostAddress;
        Nui16 port;
        bool is_ipv6;
    };

    class RNS2_NativeClient : public SocketObj
    {
    public:
        RNS2_NativeClient();

        virtual ~RNS2_NativeClient();

        ///@copydetails SocketObj::send
        RNS2SendResult send(SocketSendData * param);

        RNS2BindResult Bind(NaClSocketParam * param);

        const NaClSocketParam * GetBindings() const;

        void update();
    protected:
        void ProcessBufferedSend();
        void IssueReceiveCall();
        
        static void onSendTo(void * data, NCount size);
        static void onRecvFrom(void * data, NCount size);
        static void onSocketBound(void * data, NCount size);

        PP_Resource mSocket;
        NaClSocketParam binding;
        ThreadMutex sendInProgressMutex;
        ThreadMutex bufferedSendsMutex;
        DataStructures::Queue<NaClSendSocketParam *> bufferedSends;
        bool sendInProgress;
        enum BindState
        {
            BS_UNBOUND,
            BS_IN_PROGRESS,
            BS_BOUND,
            BS_FAILED
        } bindState;
    };
}
}