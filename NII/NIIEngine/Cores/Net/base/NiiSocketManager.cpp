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

#include "NiiSocketManager.h"
#include "NiiEngine.h"
#include "NiiTimer.h"

namespace NII
{
    //------------------------------------------------------------------------
    typedef struct 
    {
        ThreadMain	mFunc;
        void *      mFuncData;
        uint64_t	mInterval;
        uint64_t	mNextInterval;
    } TimerItem;
    //------------------------------------------------------------------------
    #define MIN_TIMER_DURATION	100	// 100 ms
    //------------------------------------------------------------------------
    template<> SocketManager * Singleton<SocketManager>::mOnly = 0;
    //------------------------------------------------------------------------
    /*SocketManager * SocketManager::getOnlyPtr()
    {
        return mOnly;
    }
    //------------------------------------------------------------------------
    SocketManager & SocketManager::getOnly()
    {
        N_assert(mOnly, "error");
        return (*mOnly);
    }*/
    //------------------------------------------------------------------------
    SocketManager::SocketManager()
    {
        mRunning = false;
#ifdef _WIN32
        FD_ZERO(&mReadSet);
        FD_ZERO(&mWriteSet);
        FD_ZERO(&mExceptionSet);
#elif __APPLE__
        mKqueue = kqueue();
        if (mKqueue == -1)
        {
            log("kqueue failed");
        }
#else
        mEpoll = epoll_create(1024);
        if (mEpoll == -1)
        {
            log("epoll_create failed");
        }
#endif
    }
    //------------------------------------------------------------------------
    SocketManager::~SocketManager()
    {
    #ifdef _WIN32

    #elif __APPLE__
        close(mKqueue);
    #else
        close(mEpoll);
    #endif
    }
    //------------------------------------------------------------------------
    SocketObj * SocketManager::createSocket()
    {
        SocketObj * s = 0;
#if N_PLAT == N_PLAT_WINRT
        s = N_new WinStore8Socket();
        s->SetSocketType(RNS2T_WINDOWS_STORE_8);
#elif N_PLAT == N_PLAT_NACL
        s = N_new RNS2_NativeClient();
        s->SetSocketType(RNS2T_CHROME);
#elif (N_PLAT == N_PLAT_WIN32)
        s = N_new RNS2_Windows();
        s->SetSocketType(RNS2T_WINDOWS);
#else
        s = N_new RNS2_Linux();
        s->SetSocketType(RNS2T_LINUX);
#endif
        return s; 
    }
    //------------------------------------------------------------------------
    void SocketManager::getLocalHost(AddressList & out)
    {
#if N_PLAT == N_PLAT_WINRT
        N_assert("不支持" && 0);
#elif N_PLAT == N_PLAT_NACL
        N_assert("不支持" && 0);
#else
    #if N_NET_PROTOCOL_VER == 6
        NCount idx;
        char ac[80];
        int err = gethostname(ac, sizeof(ac));
        N_assert(err != -1);

        struct addrinfo hints;
        struct addrinfo * servinfo = 0, *aip;  // will point to the results
        memset(&hints, 0, sizeof (addrinfo));// make sure the struct is empty
        hints.ai_socktype = SOCK_DGRAM;    // UDP sockets
        hints.ai_flags = AI_PASSIVE;       // fill in my IP for me

        getaddrinfo(ac, "", &hints, &servinfo);

        for(idx = 0, aip = servinfo; aip != NULL && idx < N_Socket_MaxLocalIP; aip = aip->ai_next, ++idx)
        {
            if(aip->ai_family == AF_INET)
            {
                IPV4Address * ip = N_new IPV4Address();
                struct sockaddr_in *ipv4 = (struct sockaddr_in *)aip->ai_addr;
                memcpy(&ip.mSrc, ipv4, sizeof(sockaddr_in));
            }
            else
            {
                IPV6Address * ip = N_new IPV6Address();
                struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)aip->ai_addr;
                memcpy(&ip.mSrc, ipv6, sizeof(sockaddr_in6));
            }
            out.push_back(ip);
        }
        freeaddrinfo(servinfo); // free the linked-list
    #else
        int idx = 0;
        char ac[80];
        int err = gethostname(ac, sizeof(ac));
        N_assert(err != -1);

        struct hostent * phe = gethostbyname(ac);

        if(phe == 0)
        {
            N_assert(phe != 0);
            return ;
        }

        for(idx = 0; idx < N_Socket_MaxLocalIP; ++idx)
        {
            if(phe->h_addr_list[idx] == 0)
                break;

            IPV4Address * ip = N_new IPV4Address();
            memcpy(&ip.mSrc.sin_addr, phe->h_addr_list[idx], sizeof(struct in_addr));
            out.push_back(ip);
        }
    #endif
#endif
    }
    //------------------------------------------------------------------------
    VString SocketManager::resolveDomain(const VString & name)
    {
        char ip[65];
#if N_PLAT == N_PLAT_WINRT
        ip[0] = 0;

        RakString rs(domainName);
        WCHAR * w_char = rs.ToWideChar();

        HostName ^hostName = ref new HostName(ref new Platform::String(w_char));

        IAsyncOperation<Windows::Foundation::Collections::IVectorView<Windows::Networking::EndpointPair^>^>^ op = DatagramSocket::GetEndpointPairsAsync(hostName, L"0");

        bool completed = false;
        op->Completed = ref new AsyncOperationCompletedHandler< Windows::Foundation::Collections::IVectorView<Windows::Networking::EndpointPair^>^ >(
            [&] (
            IAsyncOperation< Windows::Foundation::Collections::IVectorView<Windows::Networking::EndpointPair^>^>^ result,
            AsyncStatus status
            )
        {
            Windows::Foundation::Collections::IVectorView<Windows::Networking::EndpointPair^>^ result2 = result->GetResults();
            if(result2->Size > 0)
            {
                Platform::String ^name = result2->GetAt(0)->RemoteHostName->DisplayName;
                RakString rs2;
                rs2.FromWideChar(name->Data());
                strcpy(ip, rs2.C_String());
            }
            else
            {
                ip[0] = 0;
            }

            result->Close();
            completed = true;
        }
        ) ;

        while(!completed)
            Nsleep(0);

        rs.DeallocWideChar(w_char);
#else
    #if N_NET_PROTOCOL_VER == 6
        struct addrinfo hints, *res, *p;
        int status;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
        hints.ai_socktype = SOCK_DGRAM;

        if((status = getaddrinfo(domainName, NULL, &hints, &res)) != 0)
        {
            memset(ip, 0, sizeof(ip));
            return;
        }

        p = res;
        void * addr;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET)
        {
            struct sockaddr_in * ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            strcpy(ip, inet_ntoa(ipv4->sin_addr));
        }
        else
        {
            // TODO - test
            struct sockaddr_in6 * ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            // inet_ntop function does not exist on windows
            // http://www.mail-archive.com/users@ipv6.org/msg02107.html
            getnameinfo((struct sockaddr *)ipv6, sizeof(struct sockaddr_in6), ip, 1, NULL, 0, NI_NUMERICHOST);
        }
        freeaddrinfo(res); // free the linked list
    #else
        static struct in_addr addr;
        memset(&addr,0,sizeof(in_addr));

        // Use inet_addr instead? What is the difference?
        struct hostent * phe = gethostbyname(domainName);

        if(phe == 0 || phe->h_addr_list[0] == 0)
        {
            //cerr << "Yow! Bad host lookup." << endl;
            memset(ip, 0, 65 * sizeof(char));
            return;
        }

        if(phe->h_addr_list[0] == 0)
        {
            memset(ip, 0, 65 * sizeof(char));
            return;
        }

        memcpy( &addr, phe->h_addr_list[0], sizeof(struct in_addr));
        strcpy(ip, inet_ntoa(addr));
    #endif
#endif
        return ip;
    }
    //------------------------------------------------------------------------
    void SocketManager::addTimer(ThreadMain * func, void * data, Nui64 interval, bool sync)
    {
        TimerList::iterator it;
        for (it = mTimerList.begin(); it != mTimerList.end(); it++)
        {
            TimerItem * item = *it;
            if (item->mFunc == func && item->mFuncData == data)
            {
                item->mInterval = interval;
                item->mNextInterval = N_Engine().getTimer()->getMS() + interval;
                return;
            }
        }

        TimerItem * item = new TimerItem;
        item->mFunc = func;
        item->mFuncData = data;
        item->mInterval = interval;
        item->mNextInterval = N_Engine().getTimer()->getMS() + interval;
        mTimerList.push_back(item);
    }
    //------------------------------------------------------------------------
    void SocketManager::removeTimer(ThreadMain * func, void * data)
    {
        TimerList::iterator it;
        for (it = mTimerList.begin(); it != mTimerList.end(); it++)
        {
            TimerItem * item = *it;
            if (item->mFunc == func && item->mFuncData == data)
            {
                mTimerList.erase(it);
                delete item;
                return;
            }
        }
    }
    //------------------------------------------------------------------------
#ifdef _WIN32
    void SocketManager::addEvent(Nsocket fd, Nmark mark)
    {
        ScopeLock func_lock(&mMutex);

        if ((mark & SET_Read) != 0)
        {
            FD_SET(fd, &mReadSet);
        }
            
        if ((mark & SET_Write) != 0)
        {
            FD_SET(fd, &mWriteSet);
        }

        if ((mark & SET_Exception) != 0)
        {
            FD_SET(fd, &mExceptionSet);
        }
    }
    //------------------------------------------------------------------------
    void SocketManager::removeEvent(Nsocket fd, Nmark mark)
    {
        ScopeLock func_lock(&mMutex);

        if ((mark & SET_Read) != 0)
        {
            FD_CLR(fd, &mReadSet);
        }

        if ((mark & SET_Write) != 0)
        {
            FD_CLR(fd, &mWriteSet);
        }

        if ((mark & SET_Exception) != 0)
        {
            FD_CLR(fd, &mExceptionSet);
        }
    }
    //------------------------------------------------------------------------
    void SocketManager::setup(TimeDurMS timeout)
    {
        fd_set read_set, write_set, excep_set;
        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = timeout * 1000;	// 10 millisecond

        if(mRunning)
            return;
        mRunning = true;
        
        while (mRunning)
        {
            uint64_t curr_tick = N_Engine().getTimer()->getMS();
            TimerList::iterator it;

            for (it = mTimerList.begin(); it != mTimerList.end(); )
            {
                TimerItem * item = *it;
                it++;
                if(item-> mInterval == 0)
                {
                    item->mFunc(item->mFuncData, NETLIB_MSG_LOOP, 0, NULL);
                }
                else
                {
                    if (curr_tick >= item->mNextInterval)
                    {
                        item->mNextInterval += item->mInterval;
                        item->mFunc(item->mFuncData, NETLIB_MSG_TIMER, 0, NULL);
                    }
                }
            }

            if (!mReadSet.fd_count && !mWriteSet.fd_count && !mExceptionSet.fd_count)
            {
                Nsleep(MIN_TIMER_DURATION);
                continue;
            }

            mMutex.lock();
            memcpy(&read_set, &mReadSet, sizeof(fd_set));
            memcpy(&write_set, &mWriteSet, sizeof(fd_set));
            memcpy(&excep_set, &mExceptionSet, sizeof(fd_set));
            mMutex.unlock();

            int nfds = select(0, &read_set, &write_set, &excep_set, &timeout);

            if (nfds == N_SocketError)
            {
                log("select failed, error code: %d", GetLastError());
                Nsleep(MIN_TIMER_DURATION);
                continue;			// select again
            }
            else if (nfds == 0)
            {
                continue;
            }

            for (u_int i = 0; i < read_set.fd_count; i++)
            {
                Nsocket fd = read_set.fd_array[i];
                SocketPrc * prc = getPrc((Nid)fd);
                if (prc)
                {
                    prc->onRead();
                    prc->ReleaseRef();
                }
            }

            for (u_int i = 0; i < write_set.fd_count; i++)
            {
                Nsocket fd = write_set.fd_array[i];
                SocketPrc * prc = getPrc((Nid)fd);
                if (prc)
                {
                    prc->onWrite();
                    prc->ReleaseRef();
                }
            }

            for (u_int i = 0; i < excep_set.fd_count; i++)
            {
                Nsocket fd = excep_set.fd_array[i];
                SocketPrc * prc = getPrc((Nid)fd);
                if (prc)
                {
        #ifdef _DO_PRINTF
                        int err;
                        int errlen = sizeof(err);
                        N_net_getsockopt(prc->getSocket(), SOL_SOCKET, SO_ERROR,(char*)&err, &errlen);
                        N_printf("Socket error %s on listening socket\n", err);
        #endif
                    prc->onClose();
                    prc->ReleaseRef();
                }
            }

        }
    }
    //------------------------------------------------------------------------
    void SocketManager::shutdown()
    {
        mRunning = false;
    }
#elif __APPLE__
    //------------------------------------------------------------------------
    void SocketManager::addEvent(Nsocket fd, Nmark mark)
    {
        struct kevent ke;

        if ((mark & SET_Read) != 0)
        {
            EV_SET(&ke, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
            kevent(mKqueue, &ke, 1, NULL, 0, NULL);
        }

        if ((mark & SET_Write) != 0)
        {
            EV_SET(&ke, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
            kevent(mKqueue, &ke, 1, NULL, 0, NULL);
        }
    }
    //------------------------------------------------------------------------
    void SocketManager::removeEvent(Nsocket fd, Nmark mark)
    {
        struct kevent ke;

        if ((mark & SET_Read) != 0)
        {
            EV_SET(&ke, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
            kevent(mKqueue, &ke, 1, NULL, 0, NULL);
        }

        if ((mark & SET_Write) != 0)
        {
            EV_SET(&ke, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
            kevent(mKqueue, &ke, 1, NULL, 0, NULL);
        }
    }
    //------------------------------------------------------------------------
    void SocketManager::setup(TimeDurMS timeout)
    {
        struct kevent events[1024];
        int nfds = 0;
        struct timespec timeout;
        timeout.tv_sec = 0;
        timeout.tv_nsec = timeout * 1000000;

        if(mRunning)
            return;
        mRunning = true;
        
        while (mRunning)
        {
            nfds = kevent(mKqueue, NULL, 0, events, 1024, &timeout);

            for (int i = 0; i < nfds; i++)
            {
                int ev_fd = events[i].ident;
                SocketPrc * prc = getPrc(ev_fd);
                if (!prc)
                    continue;

                else if (events[i].filter == EVFILT_READ)
                {
                    prc->onRead();
                }

                else if (events[i].filter == EVFILT_WRITE)
                {
                    prc->onWrite();
                }

                prc->ReleaseRef();
            }

            uint64_t curr_tick = N_Engine().getTimer()->getMS();
            TimerList::iterator it;

            for (it = mTimerList.begin(); it != mTimerList.end(); )
            {
                TimerItem * item = *it;
                it++;
                if(item-> mInterval == 0)
                {
                    item->mFunc(item->mFuncData, NETLIB_MSG_LOOP, 0, NULL);
                }
                else
                {
                    if (curr_tick >= item->mNextInterval)
                    {
                        item->mNextInterval += item->mInterval;
                        item->mFunc(item->mFuncData, NETLIB_MSG_TIMER, 0, NULL);
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void SocketManager::shutdown()
    {
        mRunning = false;
    }
    //------------------------------------------------------------------------
#else
    void SocketManager::addEvent(Nsocket fd, Nmark mark)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLPRI | EPOLLERR | EPOLLHUP;
        ev.data.fd = fd;
        if(epoll_ctl(mEpoll, EPOLL_CTL_ADD, fd, &ev) != 0)
        {
            log("epoll_ctl() failed, errno=%d", errno);
        }
    }
    //------------------------------------------------------------------------
    void SocketManager::removeEvent(Nsocket fd, Nmark mark)
    {
        if(epoll_ctl(mEpoll, EPOLL_CTL_DEL, fd, NULL) != 0)
        {
            log("epoll_ctl failed, errno=%d", errno);
        }
    }
    //------------------------------------------------------------------------
    int SocketManager::init()
    {
        int re = 0;
#ifdef _WIN32
        WSADATA wsaData;
        WORD wReqest = MAKEWORD(1, 1);
        if (WSAStartup(wReqest, &wsaData) != 0)
        {
            re = -1;
        }
#endif
        return re;
    }
    //------------------------------------------------------------------------
    int SocketManager::destroy()
    {
        int re = 0;
#ifdef _WIN32
        if (WSACleanup() != 0)
        {
            re = -1;
        }
#endif
        return re;
    }
    //------------------------------------------------------------------------
    void SocketManager::setup(TimeDurMS timeout)
    {
        struct epoll_event events[1024];
        int nfds = 0;

        if(mRunning)
            return;
        mRunning = true;
        
        while (mRunning)
        {
            nfds = epoll_wait(mEpoll, events, 1024, timeout);
            for (int i = 0; i < nfds; i++)
            {
                int ev_fd = events[i].data.fd;
                SocketPrc * prc = getPrc(ev_fd);
                if (!prc)
                    continue;
                
                #ifdef EPOLLRDHUP
                if (events[i].events & EPOLLRDHUP)
                {
                    prc->onClose();
                }
                else
                #endif
                // Commit End

                if (events[i].events & EPOLLIN)
                {
                    prc->onRead();
                }

                if (events[i].events & EPOLLOUT)
                {
                    prc->onWrite();
                }

                if (events[i].events & (EPOLLPRI | EPOLLERR | EPOLLHUP))
                {
        #ifdef _DO_PRINTF
                        int err;
                        int errlen = sizeof(err);
                        N_net_getsockopt(prc->getSocket(), SOL_SOCKET, SO_ERROR,(char*)&err, &errlen);
                        N_printf("Socket error %s on listening socket\n", err);
        #endif
                    prc->onClose();
                }

                prc->ReleaseRef();
            }

            uint64_t curr_tick = N_Engine().getTimer()->getMS();
            TimerList::iterator it;

            for (it = mTimerList.begin(); it != mTimerList.end(); )
            {
                TimerItem * item = *it;
                it++;
                if(item-> mInterval == 0)
                {
                    item->mFunc(item->mFuncData);
                }
                else
                {
                    if (curr_tick >= item->mNextInterval)
                    {
                        item->mNextInterval += item->mInterval;
                        item->mFunc(item->mFuncData);
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void SocketManager::shutdown()
    {
        mRunning = false;
    }
    //------------------------------------------------------------------------
    void SocketManager::addPrc(SocketPrc * prc)
    {
        mPrcList.insert(make_pair((Nid)prc->getSocket(), prc));
    }
    //------------------------------------------------------------------------
    void SocketManager::removePrc(SocketPrc * prc)
    {
        mPrcList.erase((Nid)prc->getSocket());
    }
    //------------------------------------------------------------------------
    SocketPrc * SocketManager::getPrc(Nid fd) const
    {
        SocketPrc * prc = 0;
        PrcList::iterator iter = mPrcList.find(fd);
        if (iter != mPrcList.end())
        {
            prc = iter->second;
        }

        return prc;
    }
    //------------------------------------------------------------------------
    Nid SocketManager::listen(const String & ip, Nui16 port, ServerSocketIO * io)
    {
        SocketPrc * sprc = new SocketPrc(io);
        Nid re =  sprc->listen(ip, port);
        if (re == -1)
            delete sprc;
        return re;
    }
    //------------------------------------------------------------------------
    Nid SocketManager::connect(const String & ip, Nui16 port, ClientSocketIO * io)
    {
        SocketPrc * sprc = new SocketPrc(io);
        Nid re = sprc->connect(ip, port);
        if (re == -1)
            delete sprc;
        return re;
    }
    //------------------------------------------------------------------------
    NIIi SocketManager::send(Nid handle, void * buf, int size)
    {
        SocketPrc * sprc = getPrc(handle);
        if (sprc)
        {
            int re = sprc->send(buf, size);
            sprc->ReleaseRef();
            return re;
        }
        return -1;
    }
    //------------------------------------------------------------------------
    NIIi SocketManager::receive(Nid handle, void * buf, int size)
    {
        SocketPrc * sprc = getPrc(handle);
        if (sprc)
        {
            int re = sprc->receive(buf, size);
            sprc->ReleaseRef();
            return re;
        }
        return -1;
    }
    //------------------------------------------------------------------------
    NIIi SocketManager::close(Nid handle)
    {
        SocketPrc * sprc = getPrc(handle);
        if (sprc)
        {
            int re = sprc->close();
            sprc->ReleaseRef();
            return re;
        }
        return -1;
    }
    //------------------------------------------------------------------------
    NIIi SocketManager::setOption(Nid handle, OptionType type, void * value)
    {
        SocketPrc * sprc = getPrc(handle);
        if (sprc)
        {
            if ((type >= OT_GET_RemoteIP) && !value)
                return -1;

            switch (type)
            {
            case OT_GET_RemoteIP:
                *(string *)value = sprc->getRemoteIP();
                break;
            case OT_GET_RemotePort:
                *(Nui16 *)value = sprc->getRemotePort();
                break;
            case OT_GET_LocalIP:
                *(string *)value = sprc->GetLocalIP();
                break;
            case OT_GET_LocalPort:
                *(Nui16 *)value = sprc->GetLocalPort();
                break;
            case OT_SET_SendSize:
                sprc->setSendSize(*(uint32_t*)value);
                break;
            case OT_SET_ReceiveSize:
                sprc->setReceiveSize(*(uint32_t*)value);
                break;
            }

            sprc->ReleaseRef();
            return 0;
        }
        return -1;
    }
    //------------------------------------------------------------------------
}
#endif
