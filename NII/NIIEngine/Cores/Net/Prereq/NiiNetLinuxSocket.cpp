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
#include "NiiNetIOSSocket.h"

namespace NII
{
    //------------------------------------------------------------------------
    void RNS2_Berkley::SetDoNotFragment(int opt)
    {
#if defined(IP_DONTFRAGMENT)
        N_net_setsockopt(mSocket, boundAddress->getProto(), IP_DONTFRAGMENT, (char *) & opt, sizeof(opt));
#endif
    }
    //------------------------------------------------------------------------
    #ifdef _MSC_VER
    #pragma warning(disable : 4702) // warning C4702: unreachable code
    #endif
    RNS2BindResult RNS2_Berkley::BindSharedIPV4(BSDSocketParam * param)
    {
        int ret;
        memset(&boundAddress->mSrc, 0, sizeof(sockaddr_in));
        boundAddress->mSrc.sin_port = htons(param->port);
        mSocket = (int) N_net_socket(param->addressFamily, param->type, param->protocol);
        if(mSocket == -1)
            return BR_FAILED_TO_BIND_SOCKET;

        int r;
        // This doubles the max throughput rate
        int sock_opt = 1024 * 256;
        r = N_net_setsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (char *)&sock_opt, sizeof(sock_opt));
        N_assert(r == 0);

        // Immediate hard close. Don't linger the socket, or recreating the socket quickly on Vista fails.
        // Fail with voice and xbox

        sock_opt = 0;
        r = N_net_setsockopt(mSocket, SOL_SOCKET, SO_LINGER, (char *)&sock_opt, sizeof(sock_opt));
        // Do not N_assert, ignore failure

        // This doesn't make much difference: 10% maybe
        // Not supported on console 2
        sock_opt = 1024 * 16;
        r = N_net_setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (char *)&sock_opt, sizeof(sock_opt));
        N_assert(r == 0);

        if(!param->mBlock)
            fcntl(mSocket, F_SETFL, O_NONBLOCK);

        int temp = param->mBroadcast;
        N_net_setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST, (char *)&temp, sizeof(int));
        temp = param->mIPHdrIncl;
        N_net_setsockopt(mSocket, IPPROTO_IP, IP_HDRINCL, (char *)&temp, sizeof(int));

        // Fill in the rest of the address structure
        boundAddress->mSrc.sin_family = AF_INET;

        if(param->hostAddress && param->hostAddress[0])
        {
            boundAddress->mSrc.sin_addr.s_addr = N_net_inet_addr(param->hostAddress );
        }
        else
        {
            boundAddress->mSrc.sin_addr.s_addr = INADDR_ANY;
        }

        // bind our name to the socket
        ret = N_net_bind(mSocket, (struct sockaddr *) &boundAddress->mSrc,
            sizeof(boundAddress->mSrc));

        if(ret <= -1)
        {
    #if defined(__GNUC__) || defined(__GCCXML__)
            N_net_close(mSocket);
            switch (ret)
            {
            case EBADF:
                N_printf("N_net_bind(): sockfd is not a valid descriptor.\n"); break;
            case ENOTSOCK:
                N_printf("N_net_bind(): Argument is a descriptor for a file, not a socket.\n"); break;
            case EINVAL:
                N_printf("N_net_bind(): The addrlen is wrong, or the socket was not in the AF_UNIX family.\n"); break;
            case EROFS:
                N_printf("N_net_bind(): The socket inode would reside on a read-only file system.\n"); break;
            case EFAULT:
                N_printf("N_net_bind(): my_addr points outside the user's accessible address space.\n"); break;
            case ENAMETOOLONG:
                N_printf("N_net_bind(): my_addr is too long.\n"); break;
            case ENOENT:
                N_printf("N_net_bind(): The file does not exist.\n"); break;
            case ENOMEM:
                N_printf("N_net_bind(): Insufficient kernel memory was available.\n"); break;
            case ENOTDIR:
                N_printf("N_net_bind(): A component of the path prefix is not a directory.\n"); break;
            case EACCES:
                // Port reserved on PS4
                N_printf("N_net_bind(): Search permission is denied on a component of the path prefix.\n"); break;
            case ELOOP:
                N_printf("N_net_bind(): Too many symbolic links were encountered in resolving my_addr.\n"); break;
            default:
                N_printf("Unknown N_net_bind() error %i.\n", ret); break;
            }
    #endif
            return BR_FAILED_TO_BIND_SOCKET;
        }

        sockaddr_in sa;
        memset(&sa,0,sizeof(sockaddr_in));
        socklen_t len = sizeof(sa);
        N_net_getsockname(mSocket, (sockaddr*)&sa, &len);
        boundAddress->setNSBPort(sa.sin_port);
        boundAddress->mSrc.sin_addr.s_addr = sa.sin_addr.s_addr;

        if(boundAddress->mSrc.sin_addr.s_addr == INADDR_ANY)
        {
            boundAddress->mSrc.sin_addr.s_addr = N_net_inet_addr("127.0.0.1");
        }
        return BR_SUCCESS;
    }
    //------------------------------------------------------------------------
    RNS2BindResult RNS2_Berkley::BindSharedIPV4And6(BSDSocketParam * param)
    {
        int ret = 0;
        struct addrinfo hints;
        struct addrinfo * servinfo = 0, *aip;  // will point to the results

        memset(&hints, 0, sizeof (addrinfo));    // make sure the struct is empty
        hints.ai_socktype = SOCK_DGRAM;        // UDP sockets
        hints.ai_flags = AI_PASSIVE;           // fill in my IP for me
        hints.ai_family = param->addressFamily;

        char portStr[32];
        i_a(param->port, portStr, 10);

        // On Ubuntu, "" returns "No address associated with hostname" while 0 works.
        if (param->hostAddress &&
            (_stricmp(param->hostAddress, "INVALID")==0 || param->hostAddress[0]==0))
        {
            getaddrinfo(0, portStr, &hints, &servinfo);
        }
        else
        {
            getaddrinfo(param->hostAddress, portStr, &hints, &servinfo);
        }

        // Try all returned addresses until one works
        for (aip = servinfo; aip != NULL; aip = aip->ai_next)
        {
            // Open socket. The address type depends on what
            // getaddrinfo() gave us.
            mSocket = N_net_socket(aip->ai_family, aip->ai_socktype, aip->ai_protocol);

            if (mSocket == -1)
                return BR_FAILED_TO_BIND_SOCKET;

            ret = N_net_bind(mSocket, aip->ai_addr, (int) aip->ai_addrlen);
            if(ret >= 0)
            {
                // Is this valid?
                memcpy(&boundAddress->mSrc, aip->ai_addr, sizeof(boundAddress->mSrc));

                freeaddrinfo(servinfo); // free the linked-list

                int r;
                // This doubles the max throughput rate
                int sock_opt = 1024 * 256;
                r = N_net_setsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (char *)&sock_opt, sizeof(sock_opt));
                N_assert(r == 0);

                // Immediate hard close. Don't linger the socket, or recreating the socket quickly on Vista fails.
                // Fail with voice and xbox

                sock_opt = 0;
                r = N_net_setsockopt(mSocket, SOL_SOCKET, SO_LINGER, (char *)&sock_opt, sizeof(sock_opt));
                // Do not N_assert, ignore failure

                // This doesn't make much difference: 10% maybe
                // Not supported on console 2
                sock_opt = 1024 * 16;
                r = N_net_setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (char *)&sock_opt, sizeof(sock_opt));
                N_assert(r == 0);

                if(!param->mBlock)
                    fcntl(mSocket, F_SETFL, O_NONBLOCK);

                int temp = param->mBroadcast;
                N_net_setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST, (char *)&temp, sizeof(int));
                temp = param->mIPHdrIncl;
                N_net_setsockopt(mSocket, IPPROTO_IP, IP_HDRINCL, (char *)&temp, sizeof(int));

                socklen_t slen;
                sockaddr_storage ss;
                slen = sizeof(ss);

                if(N_net_getsockname(mSocket, (struct sockaddr *)&ss, &slen) != 0)
                {
                    boundAddress->write(String(""));
                    return;
                }

                if(ss.ss_family == AF_INET)
                {
                    memcpy(&boundAddress->mSrc, (sockaddr_in *)&ss, sizeof(sockaddr_in));

                    Nui32 zero = 0;
                    if(memcmp(&boundAddress->mSrc.sin_addr.s_addr, &zero, sizeof(zero)) == 0)
                        boundAddress->setLocalHost();
                }
                else
                {
                    memcpy(&boundAddress->mSrc,(sockaddr_in6 *)&ss,sizeof(sockaddr_in6));

                    char zero[16];
                    memset(zero,0,sizeof(zero));
                    if(memcmp(&boundAddress->mSrc.sin6_addr.s6_addr, &zero, sizeof(zero)) == 0)
                        boundAddress->setLocalHost();
                }

                return BR_SUCCESS;
            }
            else
            {
                N_net_close(mSocket);
            }
        }

        return BR_FAILED_TO_BIND_SOCKET;
    }
    //------------------------------------------------------------------------
    RNS2BindResult RNS2_Berkley::BindShared(BSDSocketParam * param)
    {
        RNS2BindResult br;
#if N_NET_PROTOCOL_VER == 6
        br = BindSharedIPV4And6(param);
#else
        br = BindSharedIPV4(param);
#endif

        if(br != BR_SUCCESS)
            return br;

        Nul zero = 0;
        SocketSendData bsp;
        bsp.mData = (char *)&zero;
        bsp.mLength = 4;
        bsp.mAddress = boundAddress;
        bsp.ttl = 0;
        RNS2SendResult sr = send(&bsp);
        if(sr < 0)
            return BR_FAILED_SEND_TEST;

        memcpy(&binding, param, sizeof(BSDSocketParam));
        return br;
    }
    //------------------------------------------------------------------------
    Nui32 RNS2_Berkley::RecvFromLoopInt()
    {
        isRecvFromLoopThreadActive.Increment();

        while(mStop == false)
        {
            SocketMessageData * recv = NULL;
            binding.eventHandler->alloc(&recv);
            if(recv != NULL)
            {
                recv->mSocket = this;

                if(socketType & IPV6 != 0)
                {
                    sockaddr_storage their_addr;
                    sockaddr * sockAddrPtr;
                    socklen_t sockLen;
                    socklen_t * socketlenPtr = (socklen_t *) &sockLen;
                    memset(&their_addr,0,sizeof(their_addr));
                    int dataOutSize;
                    const int flag = 0;

                    {
                        sockLen = sizeof(their_addr);
                        sockAddrPtr = (sockaddr *) &their_addr;
                    }

                    dataOutSize = N_Net_MaxMtuSize;

                    recv->mCount = N_net_recvfrom(mSocket, recv->mData, dataOutSize, flag, sockAddrPtr, socketlenPtr);

                    if(recv->mCount <= 0)
                        return;

                    recv->mRead = N_Engine().getTimer().getUS();

                    {
                        if(their_addr.ss_family == AF_INET)
                        {
                            memcpy(&recv->mAddress.mSrc,(sockaddr_in *)&their_addr,sizeof(sockaddr_in));
                        }
                        else
                        {
                            memcpy(&recv->mAddress.mSrc,(sockaddr_in6 *)&their_addr,sizeof(sockaddr_in6));
                        }
                    }
                }
                else
                {
                    sockaddr * sockAddrPtr;
                    socklen_t sockLen;
                    socklen_t * socketlenPtr = (socklen_t *)&sockLen;
                    sockaddr_in sa;
                    memset(&sa,0,sizeof(sockaddr_in));

                    const int flag = 0;

                    {
                        sockLen = sizeof(sa);
                        sa.sin_family = AF_INET;
                        sa.sin_port = 0;
                        sockAddrPtr = (sockaddr *)&sa;
                    }

                    recv->mCount = N_net_recvfrom(getSocket(), recv->mData,
                        sizeof(recv->mData), flag, sockAddrPtr, socketlenPtr);

                    if(recv->mCount <= 0)
                    {
                        return;
                    }
                    recv->mRead = N_Engine().getTimer().getUS();
                    {
                        recv->mAddress.setNSBPort(sa.sin_port);
                        recv->mAddress.mSrc.sin_addr.s_addr=sa.sin_addr.s_addr;
                    }
                }

                if(recv->mCount > 0)
                {
                    N_assert(recv->mAddress.getHSBPort());
                    binding.eventHandler->receive(recv);
                }
                else
                {
                    Nsleep(0);
                    binding.eventHandler->dealloc(recv);
                }
            }
        }
        isRecvFromLoopThreadActive.Decrement();
        return 0;
    }
    //------------------------------------------------------------------------
    RNS2_Berkley::RNS2_Berkley() :
        mThread(0)
    {
        mSocket = (Nsocket)-1;
    }
    //------------------------------------------------------------------------
    RNS2_Berkley::~RNS2_Berkley()
    {
        if(mSocket != -1)
        {
            N_net_close(mSocket);
        }
        if(mThread)
        {
            delete mThread;
            mThread = 0;
        }
    }
    //------------------------------------------------------------------------
    void RNS2_Berkley::run()
    {
        RecvFromLoopInt();
    }
    //------------------------------------------------------------------------
    int RNS2_Berkley::CreateRecvPollingThread(int level)
    {
        mStop = false;
        mThread = = new Thread("", this, false);
        mThread->setPriority(level);
        mThread->create();
        return 0;
    }
    //------------------------------------------------------------------------
    void RNS2_Berkley::SignalStopRecvPollingThread()
    {
        mStop = true;
    }
    //------------------------------------------------------------------------
    void RNS2_Berkley::BlockOnStopRecvPollingThread()
    {
        mStop = true;

        // Get recvfrom to unblock
        SocketSendData bsp;
        Nul zero = 0;
        bsp.mData = (char*) &zero;
        bsp.mLength = 4;
        bsp.mAddress = boundAddress;
        bsp.ttl = 0;
        send(&bsp);

        TimeDurMS timeout = N_Engine().getTimer().getMS() + 1000;
        while(isRecvFromLoopThreadActive.GetValue() > 0 && N_Engine().getTimer().getMS() < timeout)
        {
            // Get recvfrom to unblock
            send(&bsp);
            Nsleep(30);
        }
    }
    //------------------------------------------------------------------------
    const BSDSocketParam * RNS2_Berkley::GetBindings() const
    {
        return &binding;
    }
    //------------------------------------------------------------------------
    Nsocket RNS2_Berkley::getSocket() const
    {
        return mSocket;
    }
    //------------------------------------------------------------------------
    RNS2BindResult RNS2_Linux::Bind(BSDSocketParam * param)
    {
        return BindShared(param);
    }
    //------------------------------------------------------------------------
    RNS2SendResult RNS2_Linux::send(SocketSendData * param)
    {
        int len = 0;
        do
        {
            int oldTTL = -1;
            if(param->ttl > 0)
            {
                socklen_t opLen = sizeof(oldTTL);
                // Get the current TTL
                if(N_net_getsockopt(mSocket, param->mAddress.getProto(), IP_TTL, (char *)&oldTTL, &opLen) != -1)
                {
                    int newTTL = param->ttl;
                    N_net_setsockopt(mSocket, param->mAddress.getProto(), IP_TTL,
                        (char *)&newTTL, sizeof(newTTL));
                }
            }

            if(param->mAddress.mSrc.sin_family == AF_INET)
            {
                len = N_net_sendto(mSocket, param->mData, param->mLength, 0,
                    (const sockaddr *) & param->mAddress.mSrc, sizeof(sockaddr_in));
            }
            else
            {
    #if N_NET_PROTOCOL_VER == 6
                len = N_net_sendto(mSocket, param->mData, param->mLength, 0,
                    (const sockaddr *)&param->mAddress.mSrc, sizeof(sockaddr_in6));
    #endif
            }

            if(len < 0)
            {
                N_printf("sendto failed with code %i for char %i and length %i.\n", len, param->mData[0], param->mLength);
            }

            if(oldTTL != -1)
            {
                N_net_setsockopt(mSocket, param->mAddress.getProto(), IP_TTL,
                    (char *)&oldTTL, sizeof(oldTTL));
            }
        }
        while(len == 0);
        return len;
    }
    //------------------------------------------------------------------------
}