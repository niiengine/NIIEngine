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

#include "NiiPreProcess.h"
#include "NiiNetNaClSocket.h"

using namespace pp;

namespace NII
{
    //------------------------------------------------------------------------
    struct NaClSendSocketParam : public SocketSendData
    {
        RNS2_NativeClient * socket2;
    };
    //------------------------------------------------------------------------
    RNS2_NativeClient::RNS2_NativeClient()
    {
        bindState = BS_UNBOUND;
        sendInProgress = false;
    }
    //------------------------------------------------------------------------
    RNS2_NativeClient::~RNS2_NativeClient()
    {
        bufferedSendsMutex.Lock();
        while(bufferedSends.Size())
            N_delete bufferedSends.Pop();
        bufferedSendsMutex.Unlock();
    }
    //------------------------------------------------------------------------
    RNS2SendResult RNS2_NativeClient::send(SocketSendData * param)
    {
        if(bindState == BS_FAILED)
            return -1;

        NaClSendSocketParam * sp = N_new NaClSendSocketParam();
        sp->mData = N_alloc(param->mLength);
        memcpy(sp->mData, param->mData, param->mLength);
        sp->mLength = param->mLength;
        sp->socket2 = this;
        sp->mAddress = param->mAddress;
        sp->ttl = 0; // Unused

        bufferedSendsMutex.Lock();
        bufferedSends.Push(sp);
        bufferedSendsMutex.Unlock();

        // Do not check to send immediately, because this was probably invoked from a thread and native client is not threadsafe
        return param->mLength;
    }
    //------------------------------------------------------------------------
    RNS2BindResult RNS2_NativeClient::Bind(NaClSocketParam * param)
    {
        memcpy(&binding, param, sizeof(NaClSocketParam));

        if(Module::Get()->GetBrowserInterface(PPB_UDPSOCKET_PRIVATE_INTERFACE_0_4))
        {
            mSocket = ((PPB_UDPSocket_Private_0_4 *)Module::Get()->GetBrowserInterface(PPB_UDPSOCKET_PRIVATE_INTERFACE_0_4))->Create(param->nativeClientInstance);
            N_printf("CreateChromeSocket(%d,%s,0x%08x,%d) ==> 0x%08x\n", param->port, param->forceHostAddress?param->forceHostAddress:"(null)",param->nativeClientInstance,param->is_ipv6, mSocket);

            // Enable the broadcast feature on the socket(must happen before the bind call)
            ((PPB_UDPSocket_Private_0_4 *)pp::Module::Get()->GetBrowserInterface(PPB_UDPSOCKET_PRIVATE_INTERFACE_0_4))->SetSocketFeature(mSocket, PP_UDPSOCKETFEATURE_BROADCAST, PP_MakeBool(PP_TRUE));

            PP_NetAddress_Private client_addr;
            Nui8 ipv6[16], ipv4[4];
            if(param->forceHostAddress)
            {
                Nui32 ipIdx = 0;
                char * pch;
                pch = strtok ((char *)param->forceHostAddress,".");
                if(param->is_ipv6)
                {
                    while(pch != NULL && ipIdx < 16)
                    {
                        ipv6[ipIdx++] = atoi(pch);
                        pch = strtok(NULL, ".");
                    }
                    NetAddressPrivate::CreateFromIPv6Address(ipv6, 0, param->port, &client_addr);
                }
                else
                {
                    while(pch != NULL && ipIdx < 4)
                    {
                        ipv4[ipIdx++] = atoi(pch);
                        pch = strtok(NULL, ".");
                    }
                    NetAddressPrivate::CreateFromIPv4Address(ipv4, param->port, &client_addr);
                }
            }
            else
            {
                NetAddressPrivate::GetAnyAddress(param->is_ipv6, &client_addr);
                NetAddressPrivate::ReplacePort(client_addr, param->port, &client_addr);
            }

            bindState = BS_IN_PROGRESS;

            N_printf("attempting to bind to %s\n", NetAddressPrivate::Describe(client_addr, true).c_str());
            PP_CompletionCallback cc = PP_MakeCompletionCallback(RNS2_NativeClient::onSocketBound, this);
            ((PPB_UDPSocket_Private_0_4 *) Module::Get()->GetBrowserInterface(PPB_UDPSOCKET_PRIVATE_INTERFACE_0_4))->Bind(mSocket, &client_addr, cc);
            return BR_SUCCESS;
        }
        return BR_FAILED_TO_BIND_SOCKET;
    }
    //------------------------------------------------------------------------
    void RNS2_NativeClient::ProcessBufferedSend()
    {
        // Don't send until bound
        if(bindState != BS_BOUND)
            return;
        // Fast non-threadsafe check
        if(bufferedSends.IsEmpty() == true)
            return;

        sendInProgressMutex.Lock();

        if(sendInProgress == true)
        {
            sendInProgressMutex.Unlock();
            return;
        }
        else
        {
            sendInProgress = true;
        }
        sendInProgressMutex.Unlock();

        NaClSendSocketParam * sp;
        bufferedSendsMutex.Lock();

        if(bufferedSends.IsEmpty() == false)
            sp = bufferedSends.Pop();
        else
            sp = 0;

        bufferedSendsMutex.Unlock();

        if(sp == 0)
        {
            sendInProgressMutex.Lock();
            sendInProgress = false;
            sendInProgressMutex.Unlock();
            return; // Nothing to send after all
        }

        // Assuming data does not have to remain valid until callback called
        PP_NetAddress_Private client_addr;

#if N_NET_PROTOCOL_VER == 6
        NetAddressPrivate::CreateFromIPv6Address(sp->mAddress.mSrc.sin6_addr.u.Byte,0,sp->mAddress.getHSBPort(),&client_addr);
#else
        NetAddressPrivate::CreateFromIPv4Address((const Nui8 *)&sp->mAddress.mSrc.sin_addr, sp->mAddress.getHSBPort(),&client_addr);
#endif
        // sp remains in memory until the callback completes
        PP_CompletionCallback cc = PP_MakeCompletionCallback(onSendTo, sp);
        ((PPB_UDPSocket_Private_0_4 *)Module::Get()->
            GetBrowserInterface(PPB_UDPSOCKET_PRIVATE_INTERFACE_0_4))->
                SendTo(sp->socket2->mSocket, sp->mData, sp->mLength, &client_addr, cc);
    }
    //------------------------------------------------------------------------
    void RNS2_NativeClient::IssueReceiveCall()
    {
        SocketMessageData * recv = NULL;
        binding.eventHandler->alloc(&recv);
        if(recv != NULL)
        {
            recv->mSocket = this;
            PP_CompletionCallback cc = PP_MakeCompletionCallback(onRecvFrom, recv);
            ((PPB_UDPSocket_Private_0_4 *)Module::Get()->GetBrowserInterface(PPB_UDPSOCKET_PRIVATE_INTERFACE_0_4))->RecvFrom(mSocket, recv->mData, N_Net_MaxMtuSize, cc);
        }
    }
    //------------------------------------------------------------------------
    void RNS2_NativeClient::onSendTo(void * data, NCount size)
    {
        if(size <= 0)
            N_printf("onSendTo: send failed with error %d\n", size);

        NaClSendSocketParam * sp = (NaClSendSocketParam *)data;

        // Caller will check sendInProgress to send again if needed
        sp->socket2->sendInProgressMutex.Lock();
        sp->socket2->sendInProgress = false;
        sp->socket2->sendInProgressMutex.Unlock();

        N_free(sp->mData);
        N_delete sp;
    }
    //------------------------------------------------------------------------
    void RNS2_NativeClient::onRecvFrom(void * data, NCount size)
    {
        SocketMessageData * recv = (SocketMessageData *)data;
        RNS2_NativeClient * socket2 = (RNS2_NativeClient *)recv->mSocket;

        //any error codes will be given to us in the size value; see pp_errors.h for a list of errors
        if(size <= 0 || !data)
        {
            // Free data
            socket2->eventHandler->dealloc(recv);

            // This value indicates failure due to an asynchronous operation being
            // interrupted. The most common cause of this error code is destroying
            // a resource that still has a callback pending. All callbacks are
            // guaranteed to execute, so any callbacks pending on a destroyed
            // resource will be issued with PP_ERROR_ABORTED.
            if(size == PP_ERROR_ABORTED)
            {
                N_printf("onRecvFrom error PP_ERROR_ABORTED, killing recvfrom loop\n");
            }
            else
            {
                N_printf("onRecvFrom error %d\n", size);

                // Reissue call
                socket2->IssueReceiveCall();
            }

            return;
        }

        recv->mCount = size;
        recv->mRead = N_Engine().getTimer().getUS();

        PP_NetAddress_Private addr;
        bool ok = false;
        if(((PPB_UDPSocket_Private_0_4 *)Module::Get()->GetBrowserInterface(PPB_UDPSOCKET_PRIVATE_INTERFACE_0_4))->GetRecvFromAddress(socket2->mSocket, &addr) == PP_TRUE)
        {
            PP_NetAddressFamily_Private family = NetAddressPrivate::GetFamily(addr);
            if(family == PP_NETADDRESSFAMILY_IPV4)
            {
                ok = NetAddressPrivate::GetAddress(addr, &recv->mAddress.mSrc.sin_addr, sizeof(in_addr));
            }
    #if N_NET_PROTOCOL_VER == 6
            else
            {
                ok = NetAddressPrivate::GetAddress(addr, &recv->mAddress.mSrc.sin6_addr, sizeof(in6_addr));
            }
    #endif
        }

        if(ok)
        {
            recv->mAddress.setHSBPort(pp::NetAddressPrivate::getHSBPort(addr));
            socket2->binding.eventHandler->receive(recv);
        }

        // Reissue call
        socket2->IssueReceiveCall();
    }
    //------------------------------------------------------------------------
    void RNS2_NativeClient::onSocketBound(void * data, NCount size)
    {
        N_printf("onSocketBound ==> %d\n", size);
        RNS2_NativeClient * socket = (RNS2_NativeClient *)data;

        //any error codes will be given to us in the size value
        if(size < 0)
        {
            socket->bindState = BS_FAILED;
            fprintf(stderr,"onSocketBound exiting, size = %d\n", size);
            return;
        }

        socket->bindState = BS_BOUND;
        socket->ProcessBufferedSend();
        socket->IssueReceiveCall();
    }
    //------------------------------------------------------------------------
    const NaClSocketParam * RNS2_NativeClient::GetBindings() const
    {
        return &binding;
    }
    //------------------------------------------------------------------------
    void RNS2_NativeClient::update()
    {
        // Don't send until bound
        if(bindState == BS_BOUND)
        {
            do
            {
                ProcessBufferedSend();
            } while (sendInProgress == false && bufferedSends.Size() > 1);
        }
    }
    //------------------------------------------------------------------------
}