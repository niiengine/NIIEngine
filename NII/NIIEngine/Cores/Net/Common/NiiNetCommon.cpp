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
#include "NiiNetCommon.h"
#include "NiiNetLink.h"
#include "NiiNetContrlWindow.h"
#include "LinuxStrings.h"
#include "NetworkObj.h"
#include "DynDNS.h"
#include "NiiSocketManager.h"

namespace NII
{
namespace NII_NET
{
#if N_PLAT != N_PLAT_WIN32
    char getche()
    {
        struct termios oldt, newt;
        char ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
#endif

    // http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#ip4to6
    // http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#getaddrinfo

    #if N_NET_PROTOCOL_VER == 6
    void PrepareAddrInfoHints(addrinfo * hints)
    {
        memset(hints, 0, sizeof (addrinfo)); // make sure the struct is empty
        hints->ai_socktype = SOCK_DGRAM; // UDP sockets
        hints->ai_flags = AI_PASSIVE;     // fill in my IP for me
    }
    #endif
    //------------------------------------------------------------------------
    extern void process(const Address * address, const Nui8 * data, NCount size, NetworkSys * link, TimeDurUS read);
    //------------------------------------------------------------------------
    char * Gets ( char * str, int num )
    {
        fgets(str, num, stdin);
        if (str[0]=='\n' || str[0]=='\r')
            str[0]=0;

        size_t len=strlen(str);
        if (len>0 && (str[len-1]=='\n' || str[len-1]=='\r'))
            str[len-1]=0;
        if (len>1 && (str[len-2]=='\n' || str[len-2]=='\r'))
            str[len-2]=0;

        return str;
    }
    
    /****************************************************************************
    *        CheckSum::add
    * Inputs:
    *   Nui32 d: word to add
    * Result: void
    *
    * Effect:
    *   Adds the bytes of the Nui32 to the CheckSum
    ****************************************************************************/
    void CheckSum::Add(Nui32 value)
    {
        union
        {
            Nui32 value;
            Nui8 bytes[4];
        }data;

        data.value = value;

        for (Nui32 i = 0; i < sizeof( data.bytes ); i++)
            Add (data.bytes[i]);
    } // CheckSum::add(Nui32)

    /****************************************************************************
    *       CheckSum::add
    * Inputs:
    *   Nui16 value:
    * Result: void
    *
    * Effect:
    *   Adds the bytes of the Nui16 value to the CheckSum
    ****************************************************************************/
    void CheckSum::Add (Nui16 value)
    {
        union
        {
            Nui16 value;
            Nui8 bytes[2];
        }data;

        data.value = value;

        for (Nui32 i = 0; i < sizeof( data.bytes ); i++)
            Add (data.bytes[i])

            ;
    } // CheckSum::add(Nui16)
    /****************************************************************************
    *       CheckSum::add
    * Inputs:
    *   Nui8 value:
    * Result: void
    *
    * Effect:
    *   Adds the byte to the CheckSum
    ****************************************************************************/
    void CheckSum::Add ( Nui8 value )
    {
        Nui8 cipher = (Nui8)( value ^ ( r >> 8 ) );
        r = ( cipher + r ) * c1 + c2;
        sum += cipher;
    } // CheckSum::add(Nui8)
    /****************************************************************************
    *       CheckSum::add
    * Inputs:
    *   LPunsigned char b: pointer to byte array
    *   Nui32 length: count
    * Result: void
    *
    * Effect:
    *   Adds the bytes to the CheckSum
    ****************************************************************************/
    void CheckSum::Add (Nui8 * b, Nui32 length)
    {
        for (NCount i = 0; i < length; ++i)
            Add(b[i]);
    } // CheckSum::add(LPunsigned char, Nui32)
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // SocketUtil
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class SendToBlockJob : public Job
    {
    public:
        SendToBlockJob(SocketUtil::SendToThreadBlock * block) : Job(0), mBlock(block){}
        ~SendToBlockJob()
        { 
            if (mBlock)
            {
                N_delete mBlock;
                mBlock = 0;
            }
        }
        SocketUtil::SendToThreadBlock * mBlock;
    };
    //------------------------------------------------------------------------
    class SendToBlockJobPrc : public JobPrc
    {
    public:
        SendToBlockJobPrc() : JobPrc(true){}
        
        JobResult * handle(Job * jop)
        {
            SocketUtil::SendToThreadBlock * input = static_cast<SendToBlockJob *>(jop)->mBlock;
            SocketUtil::SendTo(input->s, input->mData, input->dataWriteOffset, input->mAddress);
            SocketUtil::objectQueue.Push(input);
        }
        
        void onPrcBegin(Job * job)
        {
            
        }
        
        void onPrcEnd(Job * job)
        {
            if(input->mBlock)
            {
                SendToBlockJob * input = static_cast<SendToBlockJob *>(jop);
                
                SocketUtil::objectQueue.Push(input->mBlock);
                
                input->mBlock = 0;
            }
        }
    };
    //------------------------------------------------------------------------
    int SocketUtil::mRefCount = 0;
    //------------------------------------------------------------------------
    DataStructures::ThreadsafeAllocatingQueue<SocketUtil::SendToThreadBlock> SocketUtil::objectQueue;
    //------------------------------------------------------------------------
    ThreadManager<SocketUtil::SendToThreadBlock *, SocketUtil::SendToThreadBlock *> SocketUtil::threadPool;
    //------------------------------------------------------------------------
    void SocketUtil::AddRef()
    {
        if (++mRefCount==1)
        {
            threadPool.startup(1);
        }
    }
    //------------------------------------------------------------------------
    void SocketUtil::Deref()
    {
        if (mRefCount > 0)
        {
            if (--mRefCount == 0)
            {
                threadPool.shutdown();
                threadPool.removeAllInput();
            }
        }
    }
    //------------------------------------------------------------------------
    SocketUtil::SendToThreadBlock * SocketUtil::AllocateBlock(void)
    {
        SocketUtil::SendToThreadBlock * b = objectQueue.Pop();
        if(b == 0)
            b = objectQueue.Allocate();
        return b;
    }
    //------------------------------------------------------------------------
    void SocketUtil::ProcessBlock(SocketUtil::SendToThreadBlock * threadedSend)
    {
        N_assert(threadedSend->dataWriteOffset > 0 && threadedSend->dataWriteOffset <= N_Net_MaxMtuSize - UDP_HEADER_SIZE);
        SendToBlockJob * job = N_new SendToBlockJob(threadedSend);
        SendToBlockJobPrc * prc = N_new SendToBlockJobPrc();
        threadPool.add(job, prc);
    }
    //------------------------------------------------------------------------
    RakString SocketUtil::GetSubNetForSocketAndIp(Nudpsocket inSock, RakString inIpString)
    {
        RakString netMaskString;
        RakString ipString;

    #if   defined(N_PLAT_WINRT)
        N_assert("Not yet supported" && 0);
        return "";
    #elif defined(_WIN32)
        INTERFACE_INFO InterfaceList[20];
        Nul nBytesReturned;
        if (WSAIoctl(inSock, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList,
            sizeof(InterfaceList), &nBytesReturned, 0, 0) == N_SocketError) 
        {
                return "";
        }

        int nNumInterfaces = nBytesReturned / sizeof(INTERFACE_INFO);

        for (int i = 0; i < nNumInterfaces; ++i)
        {
            sockaddr_in *pAddress;
            pAddress = (sockaddr_in *) & (InterfaceList[i].iiAddress);
            ipString=inet_ntoa(pAddress->sin_addr);

            if(inIpString == ipString)
            {
                pAddress = (sockaddr_in *) & (InterfaceList[i].iiNetmask);
                netMaskString = inet_ntoa(pAddress->sin_addr);
                return netMaskString;
            }
        }
        return "";
    #else

        int fd,fd2;
        fd2 = N_net_socket(AF_INET, SOCK_DGRAM, 0);

        if(fd2 < 0)
        {
            return "";
        }

        struct ifconf ifc;
        char          buf[1999];
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = buf;
        if(ioctl(fd2, SIOCGIFCONF, &ifc) < 0)
        {
            return "";
        }

        struct ifreq * ifr = ifc.ifc_req;
        int intNum = ifc.ifc_len / sizeof(struct ifreq);
        for(int i = 0; i < intNum; i++)
        {
            ipString=inet_ntoa(((struct sockaddr_in *)&ifr[i].ifr_addr)->sin_addr);

            if (inIpString==ipString)
            {
                struct ifreq ifr2;
                fd = N_net_socket(AF_INET, SOCK_DGRAM, 0);
                if(fd < 0)
                {
                    return "";
                }
                ifr2.ifr_addr.sa_family = AF_INET;

                strncpy(ifr2.ifr_name, ifr[i].ifr_name, IFNAMSIZ-1);

                ioctl(fd, SIOCGIFNETMASK, &ifr2);

                close(fd);
                close(fd2);
                netMaskString=inet_ntoa(((struct sockaddr_in *)&ifr2.ifr_addr)->sin_addr);

                return netMaskString;
            }
        }

        close(fd2);
        return "";

    #endif

    }
    //------------------------------------------------------------------------
#if defined(N_PLAT_WINRT)
    void GetMyIP_WinRT(AddressList & out)
    {
        // Perhaps DatagramSocket.BindEndpointAsynch, use localHostName as an empty string, then query what it bound to?
        N_assert("Not yet supported" && 0);
    }
#else
    //------------------------------------------------------------------------
    void GetMyIP_Win32(AddressList & out)
    {
        int idx = 0;
        idx = 0;
        char ac[80];
        if(gethostname(ac, sizeof(ac)) == -1)
        {
     #if defined(_WIN32) && !defined(N_PLAT_WIN8_PHONE)
            DWORD dwIOError = GetLastError();
            LPVOID messageBuffer;
            FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
                ( LPTSTR ) & messageBuffer, 0, NULL );
            // something has gone wrong here...
            N_printf( "gethostname failed:Error code - %d\n%s", dwIOError, messageBuffer );
            //Free the buffer.
            LocalFree(messageBuffer);
            #endif
            return ;
        }
    #if N_NET_PROTOCOL_VER == 6
        struct addrinfo hints;
        struct addrinfo * servinfo = 0, *aip;  // will point to the results
        PrepareAddrInfoHints(&hints);
        getaddrinfo(ac, "", &hints, &servinfo);

        for(idx = 0, aip = servinfo; aip != NULL && idx < N_Socket_MaxLocalIP; aip = aip->ai_next, ++idx)
        {
            if(aip->ai_family == AF_INET)
            {
                IPV4Address * ip = N_new IPV4Address();
                struct sockaddr_in * ipv4 = (struct sockaddr_in *)aip->ai_addr;
                memcpy(&ip.mSrc, ipv4, sizeof(sockaddr_in));
            }
            else
            {
                IPV6Address * ip = N_new IPV6Address();
                struct sockaddr_in6 * ipv6 = (struct sockaddr_in6 *)aip->ai_addr;
                memcpy(&ip.mSrc, ipv6, sizeof(sockaddr_in6));
            }
            out.push_back(ip);
        }

        freeaddrinfo(servinfo); // free the linked-list
    #else
        struct hostent * phe = gethostbyname(ac);

        if(phe == 0)
        {
     #if defined(_WIN32) && !defined(N_PLAT_WIN8_PHONE)
            DWORD dwIOError = GetLastError();
            LPVOID messageBuffer;
            FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
                ( LPTSTR ) & messageBuffer, 0, NULL );
            // something has gone wrong here...
            N_printf( "gethostbyname failed:Error code - %d\n%s", dwIOError, messageBuffer );

            //Free the buffer.
            LocalFree(messageBuffer);
        #endif
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
    #endif // else N_NET_PROTOCOL_VER==6
    }
#endif
    //------------------------------------------------------------------------
    void SocketUtil::getLocalHost(AddressList & out)
    {
#if defined(N_PLAT_WINRT)
        GetMyIP_WinRT(out);
#elif defined(_WIN32)
        GetMyIP_Win32(out);
#else
        GetMyIP_Win32(out);
#endif
    }
    //------------------------------------------------------------------------
    Nui16 SocketUtil::GetLocalPort(Nudpsocket s)
    {
        Address * sa;
        GetSystemAddress(s, sa);
        if(sa)
            return sa->getHSBPort();
        return 0;
    }
    //------------------------------------------------------------------------
    void SocketUtil::GetSystemAddress_Old (Nudpsocket s, Address *& systemAddressOut)
    {
    #if defined(N_PLAT_NACL)
        systemAddressOut = 0;
    #else
        sockaddr_in sa;
        memset(&sa,0,sizeof(sockaddr_in));
        socklen_t len = sizeof(sa);
        if (N_net_getsockname(s, (sockaddr*)&sa, &len)!=0)
        {
    #if defined(_WIN32) && defined(_DEBUG) && !defined(N_PLAT_WIN8_PHONE)
            DWORD dwIOError = GetLastError();
            LPVOID messageBuffer;
            FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
                ( LPTSTR ) & messageBuffer, 0, NULL );
            // something has gone wrong here...
            N_printf( "getsockname failed:Error code - %d\n%s", dwIOError, messageBuffer );

            //Free the buffer.
            LocalFree( messageBuffer );
    #endif
            systemAddressOut = 0;
            return;
        }

        systemAddressOut->setNSBPort(sa.sin_port);
        systemAddressOut->mSrc.sin_addr.s_addr=sa.sin_addr.s_addr;
    #endif
    }
    //------------------------------------------------------------------------
    void SocketUtil::GetSystemAddress(Nudpsocket s, Address *& systemAddressOut)
    {
    #if N_NET_PROTOCOL_VER != 6
        GetSystemAddress_Old(s, systemAddressOut);
    #else
        socklen_t slen;
        sockaddr_storage ss;
        slen = sizeof(ss);

        if (N_net_getsockname(s, (struct sockaddr *)&ss, &slen) != 0)
        {
    #if defined(_WIN32) && defined(_DEBUG)
            DWORD dwIOError = GetLastError();
            LPVOID messageBuffer;
            FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
                ( LPTSTR ) & messageBuffer, 0, NULL );
            // something has gone wrong here...
            N_printf( "getsockname failed:Error code - %d\n%s", dwIOError, messageBuffer );

            //Free the buffer.
            LocalFree( messageBuffer );
    #endif
            systemAddressOut->write(String(""));
            return;
        }

        if (ss.ss_family==AF_INET)
        {
            memcpy(&systemAddressOut->mSrc,(sockaddr_in *)&ss,sizeof(sockaddr_in));

            Nui32 zero = 0;
            if(memcmp(&systemAddressOut->mSrc.sin_addr.s_addr, &zero, sizeof(zero))==0)
                systemAddressOut->setLocalHost();
            //    systemAddressOut->mSrc.sin_port = ntohs(systemAddressOut->mSrc.sin_port);
        }
        else
        {
            memcpy(&systemAddressOut->mSrc,(sockaddr_in6 *)&ss,sizeof(sockaddr_in6));

            char zero[16];
            memset(zero,0,sizeof(zero));
            if(memcmp(&systemAddressOut->mSrc.sin6_addr.s6_addr, &zero, sizeof(zero))==0)
                systemAddressOut->setLocalHost();
        }
    #endif // #if N_NET_PROTOCOL_VER != 6
    }
    //------------------------------------------------------------------------
    bool SocketUtil::GetFirstBindableIP(String & firstBindable, int ipProto)
    {
        AddressList ipList;
        SocketUtil::getLocalHost(ipList);

        if (ipProto == AF_UNSPEC && ipList.size() > 0)
        {
            ipList[0].read(firstBindable, false);
            return true;
        }

        Address * ad;
        AddressList::iterator i, iend = ipList.end();
        for(i = ipList.begin(); i != iend; ++i)
        {
            ad = *i;
            if(ad == 0 || ad->isInvalid())
                continue;
            if(ad->getVersion() == AF_INET && ipProto == AF_INET)
                break;
            if(ad->getVersion() == AF_INET6 && ipProto == AF_INET6)
                break;
        }

        if(ad == 0 || ad->isInvalid())
            return false;

        ad.read(firstBindable, false);
        return true;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // DynDNS
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    struct DynDnsResult
    {
        const char *description;
        const char *code;
        DynDnsResultCode resultCode;
    };
    //------------------------------------------------------------------------
    DynDnsResult resultTable[13] =
    {
        // See http://www.dyndns.com/developers/specs/flow.pdf
        {"DNS update success.\nPlease wait up to 60 seconds for the change to take effect.\n", "good", RC_SUCCESS}, // Even with success, it takes time for the cache to update!
        {"No change", "nochg", RC_NO_CHANGE},
        {"Host has been blocked. You will need to contact DynDNS to reenable.", "abuse", RC_ABUSE},
        {"Useragent is blocked", "badagent", RC_BAD_AGENT},
        {"Username/password pair bad", "badauth", RC_BAD_AUTH},
        {"Bad system parameter", "badsys", RC_BAD_SYS},
        {"DNS inconsistency", "dnserr", RC_DNS_ERROR},
        {"Paid account feature", "!donator", RC_NOT_DONATOR},
        {"No such host in system", "nohost", RC_NO_HOST},
        {"Invalid hostname format", "notfqdn", RC_NOT_FQDN},
        {"Serious error", "numhost", RC_NUM_HOST},
        {"This host exists, but does not belong to you", "!yours", RC_NOT_YOURS},
        {"911", "911", RC_911}
    };
    //------------------------------------------------------------------------
    DynDNS::DynDNS()
    {
        connectPhase=CP_IDLE;
        tcp = 0;
    }
    //------------------------------------------------------------------------
    DynDNS::~DynDNS()
    {
        if (tcp)
        {
            N_delete tcp;
            tcp = 0;
        }
    }
    //------------------------------------------------------------------------
    void DynDNS::Stop(void)
    {
        tcp->Stop();
        connectPhase = CP_IDLE;
        N_delete tcp;
        tcp = 0;
    }
    //------------------------------------------------------------------------
    // newIPAddress is optional - if left out, DynDNS will use whatever it receives
    void DynDNS::UpdateHostIPAsynch(const char *dnsHost, const char *newIPAddress, const char *usernameAndPassword )
    {
        myIPStr = "";

        if (tcp == 0)
            tcp = N_new NetworkObj();
        connectPhase = CP_IDLE;
        host = dnsHost;

        if (tcp->Start(0, 1)==false)
        {
            SetCompleted(RC_TCP_FAILED_TO_START, "TCP failed to start");
            return;
        }

        connectPhase = CP_CONNECTING_TO_CHECKIP;
        tcp->Connect("checkip.dyndns.org", 80, false);

        // See https://www.dyndns.com/developers/specs/syntax.html
        getString = "GET /nic/update?hostname=";
        getString += dnsHost;
        if (newIPAddress)
        {
            getString += "&myip=";
            getString += newIPAddress;
        }
        getString += "&wildcard=NOCHG&mx=NOCHG&backmx=NOCHG HTTP/1.0\n";
        getString += "Host: members.dyndns.org\n";
        getString += "Authorization: Basic ";
        VString outputData;
        Base64::Encode((const Nui8*) usernameAndPassword, (int) strlen(usernameAndPassword), outputData);
        getString += outputData;
        getString += "User-Agent: Jenkins Software LLC - PC - 1.0\n\n";
    }
    //------------------------------------------------------------------------
    
    //------------------------------------------------------------------------
    void DynDNS::update()
    {
        if(connectPhase == CP_IDLE)
            return;

        serverAddress = tcp->onConnectFail(serverAddress);
        if(serverAddress != 0)
        {
            SetCompleted(RC_TCP_DID_NOT_CONNECT, "Could not connect to DynDNS");
            return;
        }

        serverAddress = tcp->onClientConnect(serverAddress);
        if(serverAddress != 0)
        {
            if(connectPhase == CP_CONNECTING_TO_CHECKIP)
            {
                checkIpAddress = serverAddress;
                connectPhase = CP_WAITING_FOR_CHECKIP_RESPONSE;
                tcp->send("GET\n\n", (Nui32) strlen("GET\n\n"), serverAddress, false); // Needs 2 newlines! This is not documented and wasted a lot of my time
            }
            else
            {
                connectPhase = CP_WAITING_FOR_DYNDNS_RESPONSE;
                tcp->send(getString.C_String(), (Nui32) getString.GetLength(), serverAddress, false);
            }
            phaseTimeout = N_Engine().getTimer().getMS() + 1000;
        }

        if(connectPhase == CP_WAITING_FOR_CHECKIP_RESPONSE && N_Engine().getTimer().getMS()>phaseTimeout)
        {
            connectPhase = CP_CONNECTING_TO_DYNDNS;
            tcp->close(checkIpAddress);
            tcp->Connect("members.dyndns.org", 80, false);
        }
        else if(connectPhase == CP_WAITING_FOR_DYNDNS_RESPONSE && N_Engine().getTimer().getMS()>phaseTimeout)
        {
            SetCompleted(RC_DYNDNS_TIMEOUT, "DynDNS did not respond");
            return;
        }

        SocketMessage * msg = tcp->receive();
        if(msg)
        {
            if(connectPhase==CP_WAITING_FOR_DYNDNS_RESPONSE)
            {
                Nui32 i;

                char *result;
                result=strstr((char*) msg->mData, "Connection: close");
                if (result!=0)
                {
                    result+=strlen("Connection: close");
                    while (*result && ((*result=='\r') || (*result=='\n') || (*result==' ')) )
                        result++;
                    for (i=0; i < 13; i++)
                    {
                        if (strncmp(resultTable[i].code, result, strlen(resultTable[i].code))==0)
                        {
                            if (resultTable[i].resultCode==RC_SUCCESS)
                            {
                                // Read my external IP into myIPStr
                                // Advance until we hit a number
                                while (*result && ((*result<'0') || (*result>'9')) )
                                    result++;
                                if (*result)
                                {
                                    //Address * parser;
                                    parser.write(String(result));
                                    parser.read(myIPStr, false);
                                }
                            }
                            N_delete msg;
                            SetCompleted(resultTable[i].resultCode, resultTable[i].description);
                            break;
                        }
                    }
                    if (i==13)
                    {
                        N_delete msg;
                        SetCompleted(RC_UNKNOWN_RESULT, "DynDNS returned unknown result");
                    }
                }
                else
                {
                    N_delete msg;
                    SetCompleted(RC_PARSING_FAILURE, "Parsing failure on returned string from DynDNS");
                }

                return;
            }
            else
            {
                /*
                HTTP/1.1 200 OK
                Content-Type: text/html
                Server: DynDNS-CheckIP/1.0
                Connection: close
                Cache-Control: no-cache
                Pragma: no-cache
                Content-Length: 105

                <html><head><title>Current IP Check</title></head><body>Current IP Address: 98.1
                89.219.22</body></html>


                Connection to host lost.
                */

                char * result;
                result = strstr((char*) msg->mData, "Current IP Address: ");
                if(result != 0)
                {
                    result += strlen("Current IP Address: ");
                    //Address * myIp;
                    myIp.write(String(result));
                    myIp.read(myIPStr, false);

                    char existingHost[65];
                    existingHost[0] = 0;
                    // Resolve DNS we are setting. If equal to current then abort
                    N_Only(Manager).resolve(host.C_String(), existingHost);
                    if (existingHost && strcmp(existingHost, myIPStr.c_str())==0)
                    {
                        // DynDNS considers setting the IP to what it is already set abuse
                        N_delete msg;
                        SetCompleted(RC_DNS_ALREADY_SET, "No action needed");
                        return;
                    }
                }

                N_delete msg;
                tcp->close(msg->mAddress);

                connectPhase = CP_CONNECTING_TO_DYNDNS;
                tcp->Connect("members.dyndns.org", 80, false);
            }
        }
        if(tcp->onConnectClose() != 0 && !tcp->onConnectClose()->isInvalid())
        {
            if(connectPhase == CP_WAITING_FOR_DYNDNS_RESPONSE)
            {
                SetCompleted(RC_CONNECTION_LOST_WITHOUT_RESPONSE, "Connection lost to DynDNS during GET operation");
            }
        }
    }
    //------------------------------------------------------------------------
    bool NonNumericHostString(const String & host)
    {
        // Return false if IP address. Return true if domain
        Nui32 i=0;
        const char * temp = host.c_str();
        while(temp[i])
        {
            // IPV4: natpunch.jenkinssoftware.com
            // IPV6: fe80::7c:31f7:fec4:27de%14
            if((temp[i]>='g' && temp[i]<='z') || (temp[i]>='A' && temp[i]<='Z'))
                return true;
            ++i;
        }
        return false;
    }
    //------------------------------------------------------------------------
    HostPeer::HostPeer()
    {
    #ifdef N_PLAT_NACL
        blockingSocket=false;
    #else
        blockingSocket=true;
    #endif
        port = 0;
        hostAddress[0] = 0;
        extraSocketOptions = 0;
        socketFamily = AF_INET;
    }
    //------------------------------------------------------------------------
    HostPeer::HostPeer(Nui16 _port, const String & host)
    {
        #ifdef N_PLAT_NACL
            blockingSocket = false;
        #else
            blockingSocket = true;
        #endif

        port = _port;
        if(host != String(""))
            strcpy(hostAddress, host.c_str());
        else
            hostAddress[0] = 0;
        extraSocketOptions = 0;
        socketFamily = AF_INET;
    }
    //------------------------------------------------------------------------
}
}