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
#include "NiiSocketPrc.h"
#include "NiiSocketManager.h"

#ifdef _WIN32
    #define _GetErrorCode WSAGetLastError()
#else
    #define _GetErrorCode errno
#endif

namespace NII
{
    //------------------------------------------------------------------------
    SocketPrc::SocketPrc(SocketIO * io) :
        mIO(io),
        mAddress(0),
        mSocket(N_InvalidSocket),
        mState(S_Unknow)
    {
    }
    //------------------------------------------------------------------------
    SocketPrc::SocketPrc():
        mIO(0),
        mAddress(0),
        mSocket(N_InvalidSocket),
        mState(S_Unknow)
    {
    }
    //------------------------------------------------------------------------
    SocketPrc::~SocketPrc()
    {
        if(mAddress)
        {
            N_delete mAddress;
            mAddress = 0;
        }
        if(mIO)
        {
            N_delete mIO;
            mIO = 0;
        }
    }
    //------------------------------------------------------------------------
    Nid SocketPrc::listen(const String & ip, Nui16 port, NCount maxlink, Nui16 sfamily)
    {
    #ifdef N_PLAT_NACL
        return 0;
    #endif
        mIP = ip;
        mPort = port;

#if N_NET_PROTOCOL_VER == 6
        struct addrinfo hints;
        memset(&hints, 0, sizeof (addrinfo)); // make sure the struct is empty
        hints.ai_family = sfamily;     // don't care IPv4 or IPv6
        hints.ai_socktype = SOCK_STREAM; // TCP sockets
        hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
        struct addrinfo *servinfo = 0, *aip;  // will point to the results
        char portStr[32];
        i_a(port, portStr, 10);

        getaddrinfo(0, portStr, &hints, &servinfo);
        for(aip = servinfo; aip != NULL; aip = aip->ai_next)
        {
            mSocket = N_net_socket(aip->ai_family, aip->ai_socktype, aip->ai_protocol);
            if(mSocket != 0)
            {
                int ret = ::bind(mSocket, aip->ai_addr, (int) aip->ai_addrlen);
                if(ret == N_SocketError)
                {
                    N_net_close(mSocket);
                    mSocket = 0;
                }
                else
                {
                    break;
                }
            }
        }
        if(mSocket != 0)
        {
            N_net_listen(mSocket, maxlink);
        }
        else
        {
            return -1;
        }
#else
    #if defined(N_PLAT_WINRT)
        mSocket = WinRTCreateStreamSocket(AF_INET, SOCK_STREAM, 0);
    #else  
        mSocket = N_net_socket(AF_INET, SOCK_STREAM, 0);
        if (mSocket == N_InvalidSocket)
        {
            printf("socket failed, err_code=%d\n", _GetErrorCode);
            return -1;
        }

        setReuseAddress(mSocket);
        setNonBlock(mSocket);

        sockaddr_in dstaddr;
        setAddress(mIP, mPort, &dstaddr);
        int re = ::bind(mSocket, (sockaddr *)&dstaddr, sizeof(dstaddr));
        if (re == N_SocketError)
        {
            log("bind failed, err_code=%d", _GetErrorCode);
            N_net_close(mSocket);
            return -1;
        }

        re = N_net_listen(mSocket, linkcnt);
        if (re == N_SocketError)
        {
            log("listen failed, err_code=%d", _GetErrorCode);
            N_net_close(mSocket);
            return -1;
        }
#endif
        mState = S_Listening;

        log("SocketPrc::listen on %s:%d", ip, port);

        N_Only(Socket)->addPrc(this);
        N_Only(Socket)->addEvent(mSocket, SET_Read | SET_Exception);
        return (Nid)mSocket;
    }
    //------------------------------------------------------------------------
    Nid SocketPrc::connect(const String & ip, Nui16 port, Nui16 sfamily)
    {
    #ifdef N_PLAT_NACL
        return 0;
    #endif
        mRemoteIP = ip;
        mRemotePort = port;
#if N_NET_PROTOCOL_VER == 6
        struct addrinfo hints, *res;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = sfamily;
        hints.ai_socktype = SOCK_STREAM;
        char portStr[32];
        i_a(port, portStr, 10);
        getaddrinfo(ip.c_str(), portStr, &hints, &res);
        mSocket = N_net_socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (mSocket == N_InvalidSocket)
        {
            log("socket failed, err_code=%d", _GetErrorCode);
            return -1;
        }

        setNonBlock(mSocket);
        setNoDelay(mSocket);
        
        int re = N_net_connect(mSocket, res->ai_addr, res->ai_addrlen);

        freeaddrinfo(res);
#else
    #if defined(N_PLAT_WINRT)
        mSocket = = WinRTCreateStreamSocket(AF_INET, SOCK_STREAM, 0);
    #else
        mSocket = N_net_socket(AF_INET, SOCK_STREAM, 0);
    #endif
        if (mSocket == N_InvalidSocket)
        {
            log("socket failed, err_code=%d", _GetErrorCode);
            return -1;
        }

        setNonBlock(mSocket);
        setNoDelay(mSocket);
        
        sockaddr_in dstaddr;
        setAddress(mRemoteIP, mRemotePort, &dstaddr);
        int re = N_net_connect(mSocket, (sockaddr*)&dstaddr, sizeof(dstaddr));
#endif
        if((re == N_SocketError) && (!isBlock(_GetErrorCode)))
        {	
            log("connect failed, err_code=%d", _GetErrorCode);
            N_net_close(mSocket);
            return -1;
        }
        mState = S_Connecting;
        N_Only(Socket)->addPrc(this);
        N_Only(Socket)->addEvent(mSocket, SET_All);
        
        return (Nid)mSocket;
    }
    //------------------------------------------------------------------------
    NIIi SocketPrc::send(void * data, NCount len)
    {
        if (mState != S_Connected)
            return -1;

        int re = N_net_send(mSocket, (char*)data, len, 0);
        if (re == N_SocketError)
        {
            if (isBlock(_GetErrorCode))
            {
    #if ((defined _WIN32) || (defined __APPLE__))
                N_Only(Socket)->addEvent(mSocket, SET_Write);
    #endif
                re = 0;
            }
            else
            {
                onClose();
            }
        }

        return re;
    }
    //------------------------------------------------------------------------
    NIIi SocketPrc::receive(void * data, NCount len)
    {
        return N_net_recv(mSocket, (char *)data, len, 0);
    }
    //------------------------------------------------------------------------
    NIIi SocketPrc::close()
    {
        N_Only(Socket)->removeEvent(mSocket, SET_All);
        N_Only(Socket)->removePrc(this);
        N_net_close(mSocket);

        return 0;
    }
    //------------------------------------------------------------------------
    void SocketPrc::onRead()
    {
        if (mState == S_Listening)
        {
            Nsocket fd = 0;
        #if N_NET_PROTOCOL_VER == 6
            struct sockaddr_storage peer_addr;
            socklen_t addr_len = sizeof(sockaddr_storage);
        #else
            struct sockaddr_in peer_addr;
            socklen_t addr_len = sizeof(sockaddr_in);
        #endif
            
            char ip_str[64];
            while((fd = N_net_accept(mSocket, (sockaddr *)&peer_addr, &addr_len)) != N_InvalidSocket)
            {
                ServerSocketIO * io = mIO->createInstance();
                SocketPrc * prc = N_new SocketPrc(io);
                io->setPrc(prc);

                Nui32 ip = ntohl(peer_addr.sin_addr.s_addr);
                snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", ip >> 24, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);

                prc->setSocket(fd);
                prc->setState(S_Connected);
                prc->mRemoteIP = ip_str;
                prc->mRemotePort = ntohs(peer_addr.sin_port);
                log("AcceptNewSocket, socket=%d from %s:%d\n", fd, ip_str, prc->mRemotePort);

        #if N_NET_PROTOCOL_VER == 6
                if (sockAddr.ss_family == AF_INET)
                {
                    IVP6Address * tempaddr = N_new IVP6Address();
                    prc->mAddress = tempaddr;
                    memcpy(&tempaddr->mSrc,(sockaddr_in *)&sockAddr,sizeof(sockaddr_in));
                }
                else
                {
                    IVP4Address * tempaddr = N_new IVP4Address();
                    prc->mAddress = tempaddr;
                    memcpy(&tempaddr->mSrc,(sockaddr_in6 *)&sockAddr,sizeof(sockaddr_in6));
                }
        #else
                IVP4Address * addr = N_new IVP4Address();
                prc->mAddress = addr;
                addr->mSrc.sin_addr.s_addr = sockAddr.sin_addr.s_addr;
                prc->mAddress->setNSBPort(sockAddr.sin_port);
                prc->mAddress->mIndex = reidx;
        #endif
                setNoDelay(fd);
                setNonBlock(fd);
                
                prc->onConnect();
                N_Only(Socket)->addPrc(prc);
                N_Only(Socket)->addEvent(fd, SET_Read | SET_Exception);
                io->onConnect();
            }
        }
        else
        {
            u_long avail = 0;
            if ((N_net_ioctl(mSocket, FIONREAD, &avail) == N_SocketError) || (avail == 0))
            {
                onClose();
            }
            else
            {
                mIO->onRead();
            }
        }
    }
    //------------------------------------------------------------------------
    void SocketPrc::onWrite()
    {
    #if ((defined _WIN32) || (defined __APPLE__))
        N_Only(Socket)->removeEvent(mSocket, SET_Write);
    #endif

        if (mState == S_Connecting)
        {
            int error = 0;
            socklen_t len = sizeof(error);
    #ifdef _WIN32
            getsockopt(mSocket, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
    #else
            getsockopt(mSocket, SOL_SOCKET, SO_ERROR, (void*)&error, &len);
    #endif
            if (error) 
            {
                onClose();
            } 
            else 
            {
                onConfirm();
                mState = S_Connected;
                static_cast<ClientSocketIO *>(mIO)->onConfirm();
            }
        }
        else
        {
            mIO->onWrite();
        }
    }
    //------------------------------------------------------------------------
    void SocketPrc::onClose()
    {
        mState = S_Closing;
        mIO->abort();
        mIO->onClose();
    }
    //------------------------------------------------------------------------
    void SocketPrc::setSendSize(Nui32 size)
    {
        int re = setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, &size, 4);
        if (re == N_SocketError) 
        {
            log("set SO_SNDBUF failed for fd=%d", mSocket);
        }

        socklen_t len = 4;
        int size = 0;
        getsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, &size, &len);
        log("socket=%d send_buf_size=%d", mSocket, size);
    }
    //------------------------------------------------------------------------
    Nui32 SocketPrc::getSendSize() const
    {
        socklen_t len = 4;
        int size = 0;
        getsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, &size, &len);
        log("socket=%d send_buf_size=%d", mSocket, size);
        return size;
    }
    //------------------------------------------------------------------------
    void SocketPrc::setReceiveSize(Nui32 size)
    {
        int re = setsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, &size, 4);
        if (re == N_SocketError) 
        {
            log("set SO_RCVBUF failed for fd=%d", mSocket);
        }

        socklen_t len = 4;
        int size = 0;
        getsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, &size, &len);
        log("socket=%d recv_buf_size=%d", mSocket, size);
    }
    //------------------------------------------------------------------------
    Nui32 SocketPrc::getReceiveSize() const
    {
        socklen_t len = 4;
        int size = 0;
        getsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, &size, &len);
        log("socket=%d recv_buf_size=%d", mSocket, size);
        return size;
    }
    //------------------------------------------------------------------------
    bool SocketPrc::isBlock(int ecode)
    {
    #ifdef _WIN32
        return ((ecode == WSAEINPROGRESS) || (ecode == WSAEWOULDBLOCK));
    #else
        return ((ecode == EINPROGRESS) || (ecode == EWOULDBLOCK));
    #endif
    }
    //------------------------------------------------------------------------
    void SocketPrc::setNonBlock(Nsocket fd)
    {
    #ifdef _WIN32
        u_long nonblock = 1;
        int re = N_net_ioctl(fd, FIONBIO, &nonblock);
    #else
        int re = fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL));
    #endif
        if (re == N_SocketError)
        {
            log("setNonBlock failed, err_code=%d", _GetErrorCode);
        }
    }
    //------------------------------------------------------------------------
    void SocketPrc::setReuseAddress(Nsocket fd)
    {
        int reuse = 1;
        int re = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));
        if (re == N_SocketError)
        {
            log("setReuseAddress failed, err_code=%d", _GetErrorCode);
        }
    }
    //------------------------------------------------------------------------
    void SocketPrc::setNoDelay(Nsocket fd)
    {
        int nodelay = 1;
        int re = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelay, sizeof(nodelay));
        if (re == N_SocketError)
        {
            log("setNoDelay failed, err_code=%d", _GetErrorCode);
        }
    }
    //------------------------------------------------------------------------
    void SocketPrc::setBroadcast(Nsocket fd)
    {
        int broadcast = 1;
        int re = N_net_setsockopt(listenSocket, SOL_SOCKET, SO_BROADCAST, (char *)&broadcast, sizeof( broadcast ) )
        if(re == N_SocketError)
        {
            log("setNoDelay failed, err_code=%d", _GetErrorCode);
        }
    }
    //------------------------------------------------------------------------
    void SocketPrc::setLinger(Nsocket fd, bool set, TimeDurMS time)
    {
        struct linger linger;
        if(set)
        {
#if (defined(_WIN32) || defined(_WIN64))
            time *= 1000;
#endif
            linger.l_onoff = set;
            linger.l_linger = time;
        }
        else
        {
            linger.l_onoff = FALSE;
            linger.l_linger = 0;
        }
        setsockopt(fd, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger));
    }
    //-----------------------------------------------------------------------
    void SocketPrc::setSendTimeOut(Nsocket fd, TimeDurMS time)
    {
#if (defined(_WIN32) || defined(_WIN64))
        time *= 1000;
#endif
        struct timeval timeout = { time, 0 };
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    }
    //-----------------------------------------------------------------------
    void SocketPrc::setRecvTimeOut(Nsocket fd, TimeDurMS time)
    {
#if (defined(_WIN32) || defined(_WIN64))
        time *= 1000;
#endif
        struct timeval timeout = { time, 0 };
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    }
    //------------------------------------------------------------------------
    void SocketPrc::setAddress(const String & ip, Nui16 port, sockaddr_in * addr)
    {
        memset(addr, 0, sizeof(sockaddr_in));
        addr->sin_family = AF_INET;
        addr->sin_port = htons(port);
        
        addr->sin_addr.s_addr = N_net_inet_addr(ip);
        
        if (addr->sin_addr.s_addr == INADDR_NONE)
        {
            const hostent * host = gethostbyname(ip);
            if (host != NULL)
            {
                addr->sin_addr.s_addr = *(Nui32*)host->h_addr;
            }
        }
    }
    //------------------------------------------------------------------------
    void SocketPrc::setTOS(Nsocket fd, Ni32 tos)
    {
        setsockopt(fd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));
    }
    //------------------------------------------------------------------------
    void SocketPrc::setTTL(Nsocket fd, Ni32 ttl)
    {
        setsockopt(fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
    }
    //------------------------------------------------------------------------
}