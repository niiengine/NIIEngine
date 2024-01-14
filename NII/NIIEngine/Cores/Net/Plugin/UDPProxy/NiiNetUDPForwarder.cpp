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

#include "UDPForwarder.h"

#if _RAKNET_SUPPORT_UDPForwarder == 1

#include "NiiPreProcess.h"
#include "NiiNetCommon.h"
#include "NiiSocketManager.h"
#include "Nsleep.h"
#include "DS_OrderedList.h"
#include "LinuxStrings.h"
#include "errno.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    static const Nui16 DEFAULT_MAX_FORWARD_ENTRIES = 64;
    //------------------------------------------------------------------------
    UDPForwarder::ForwardEntry::ForwardEntry()
    {
        socket = -1;
        timeLastDatagramForwarded = N_Engine().getTimer().getMS();
        addr1Confirmed = 0;
        addr2Confirmed = 0;
    }
    //------------------------------------------------------------------------
    UDPForwarder::ForwardEntry::~ForwardEntry()
    {
        if(socket != -1)
            N_net_close(socket);
    }
    //------------------------------------------------------------------------
    UDPForwarder::UDPForwarder():
        mThread(0)
    {
    #if N_PLAT == N_PLAT_WIN32
        N_Only(Socket).init();
    #endif

        maxForwardEntries = DEFAULT_MAX_FORWARD_ENTRIES;
        nextInputId = 0;
        startForwardingInput.SetPageSize(sizeof(StartForwardingInputStruct)*16);
        stopForwardingCommands.SetPageSize(sizeof(StopForwardingStruct)*16);
    }
    //------------------------------------------------------------------------
    UDPForwarder::~UDPForwarder()
    {
        Shutdown();

        if(mThread)
        {
            delete mThread;
            mThread = 0;
        }
    #if N_PLAT == N_PLAT_WIN32
        N_Only(Socket).destroy();
    #endif
    }
    //------------------------------------------------------------------------
    void UDPForwarder::run()
    {
        UDPForwarder * udpForwarder = ( UDPForwarder * ) arguments;
        udpForwarder->threadRunning.Increment();
        while (udpForwarder->isRunning.GetValue()>0)
        {
            udpForwarder->UpdateUDPForwarder();

            // 12/1/2010 Do not change  0
            // See http://www.jenkinssoftware.com/forum/index.php?topic=4033.0;topicseen
            // Avoid 100% reported CPU usage
            if (udpForwarder->forwardListNotUpdated.Size()==0)
                Nsleep(30);
            else
                Nsleep(0);
        }
        udpForwarder->threadRunning.Decrement();
    }
    //------------------------------------------------------------------------
    void UDPForwarder::Startup(void)
    {
        if (isRunning.GetValue() > 0)
            return;

        isRunning.Increment();

        mThread = new Thread("", this, false);
        mThread->setPriority(0);
        mThread->create();

        while (threadRunning.GetValue()==0)
            Nsleep(30);
    }
    //------------------------------------------------------------------------
    void UDPForwarder::Shutdown(void)
    {
        if (isRunning.GetValue() == 0)
            return;
        isRunning.Decrement();

        while (threadRunning.GetValue() > 0)
            Nsleep(30);

        Nui32 j;
        for (j = 0; j < forwardListNotUpdated.Size(); ++j)
            N_delete forwardListNotUpdated[j];
        forwardListNotUpdated.Clear(false);
    }
    //------------------------------------------------------------------------
    void UDPForwarder::SetMaxForwardEntries(Nui16 maxEntries)
    {
        N_assert(maxEntries > 0 && maxEntries < 65535 / 2);
        maxForwardEntries = maxEntries;
    }
    //------------------------------------------------------------------------
    int UDPForwarder::GetMaxForwardEntries(void) const
    {
        return maxForwardEntries;
    }
    //------------------------------------------------------------------------
    int UDPForwarder::GetUsedForwardEntries(void) const
    {
        return (int) forwardListNotUpdated.Size();
    }
    //------------------------------------------------------------------------
    UDPForwarderResult UDPForwarder::StartForwarding(Address * source,
        Address * destination, TimeDurMS timeoutOnNoDataMS,
            const char * forceHostAddress, Nui16 socketFamily,
                Nui16 * forwardingPort, Nudpsocket * forwardingSocket)
    {
        // Invalid parameters?
        if(timeoutOnNoDataMS == 0 || timeoutOnNoDataMS > UDP_FORWARDER_MAXIMUM_TIMEOUT ||
            source == 0 || destination == 0 || source->isInvalid() || destination->isInvalid())
            return UDPFORWARDER_INVALID_PARAMETERS;

        if (isRunning.GetValue() == 0)
            return UDPFORWARDER_NOT_RUNNING;

        (void) socketFamily;

        Nui32 inputId = nextInputId++;

        StartForwardingInputStruct * sfis;
        sfis = startForwardingInput.Allocate();
        sfis->source = source;
        sfis->destination = destination;
        sfis->timeoutOnNoDataMS = timeoutOnNoDataMS;
        N_assert(timeoutOnNoDataMS != 0);
        if (forceHostAddress && forceHostAddress[0])
            sfis->forceHostAddress = forceHostAddress;
        sfis->socketFamily = socketFamily;
        sfis->inputId = inputId;
        startForwardingInput.Push(sfis);

    #ifdef _MSC_VER
    #pragma warning( disable : 4127 ) // warning C4127: conditional expression is constant
    #endif
        while (1)
        {
            Nsleep(0);
            startForwardingOutputMutex.Lock();
            for (Nui32 i=0; i < startForwardingOutput.Size(); ++i)
            {
                if (startForwardingOutput[i].inputId == inputId)
                {
                    if (startForwardingOutput[i].result == UDPFORWARDER_SUCCESS)
                    {
                        if (forwardingPort)
                            *forwardingPort = startForwardingOutput[i].forwardingPort;
                        if (forwardingSocket)
                            *forwardingSocket = startForwardingOutput[i].forwardingSocket;
                    }
                    UDPForwarderResult res = startForwardingOutput[i].result;
                    startForwardingOutput.RemoveAtIndex(i);
                    startForwardingOutputMutex.Unlock();
                    return res;
                }
            }
            startForwardingOutputMutex.Unlock();
        }

        return UDPFORWARDER_RESULT_COUNT;
    }
    //------------------------------------------------------------------------
    void UDPForwarder::StopForwarding(Address * source, Address * destination)
    {
        StopForwardingStruct *sfs;
        sfs = stopForwardingCommands.Allocate();
        sfs->destination=destination;
        sfs->source=source;
        stopForwardingCommands.Push(sfs);
    }
    //------------------------------------------------------------------------
    void UDPForwarder::RecvFrom(TimeDurMS curTime, ForwardEntry * forwardEntry)
    {
    #ifndef N_PLAT_NACL
        char data[N_Net_MaxMtuSize];
    #if N_NET_PROTOCOL_VER==6
        sockaddr_storage their_addr;
        memset(&their_addr,0,sizeof(their_addr));
        sockaddr * sockAddrPtr;
        socklen_t sockLen;
        socklen_t * socketlenPtr = (socklen_t *)&sockLen;
        sockaddr_in * sockAddrIn;
        sockaddr_in6 * sockAddrIn6;
        sockLen = sizeof(their_addr);
        sockAddrPtr = (sockaddr *)&their_addr;
    #else
        sockaddr_in sockAddrIn;
        memset(&sockAddrIn,0,sizeof(sockaddr_in));
        socklen_t len2;
        len2 = sizeof(sockAddrIn);
        sockAddrIn.sin_family = AF_INET;
    #endif

    #if defined(__GNUC__)
        #if defined(MSG_DONTWAIT)
            const int flag = MSG_DONTWAIT;
        #else
            const int flag = 0x40;
        #endif
    #else
        const int flag = 0;
    #endif
        int receivedDataLen, len = 0;
    #if N_NET_PROTOCOL_VER == 6
        receivedDataLen = N_net_recvfrom(forwardEntry->socket, data, N_Net_MaxMtuSize,
            flag, sockAddrPtr, socketlenPtr);
    #else
        receivedDataLen = N_net_recvfrom(forwardEntry->socket, data, N_Net_MaxMtuSize,
            flag, (sockaddr *)&sockAddrIn, (socklen_t *)&len2);
    #endif

        if (receivedDataLen < 0)
        {
    #if defined(_WIN32) && defined(_DEBUG) && !defined(N_PLAT_WIN8_PHONE) && !defined(N_PLAT_WINRT)
            DWORD dwIOError = WSAGetLastError();

            if (dwIOError!=WSAECONNRESET && dwIOError!=WSAEINTR && dwIOError!=WSAETIMEDOUT && dwIOError!=WSAEWOULDBLOCK)
            {
                LPVOID messageBuffer;
                FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
                    ( LPTSTR ) & messageBuffer, 0, NULL );
                // something has gone wrong here...
                N_printf( "recvfrom failed:Error code - %d\n%s", dwIOError, messageBuffer );

                //Free the buffer.
                LocalFree( messageBuffer );
            }
    #else
            if (errno!=EAGAIN
                && errno!=0
    #if defined(__GNUC__)
                && errno!=EWOULDBLOCK
    #endif
                )
            {
                printf("errno=%i\n", errno);
            }
    #endif
        }

        if (receivedDataLen<=0)
            return;

        Address * receivedAddr;
    #if N_NET_PROTOCOL_VER==6
        if (their_addr.ss_family==AF_INET)
        {
            sockAddrIn=(sockaddr_in *)&their_addr;
            sockAddrIn6=0;
            memcpy(&receivedAddr.mSrc,sockAddrIn,sizeof(sockaddr_in));
        }
        else
        {
            sockAddrIn=0;
            sockAddrIn6=(sockaddr_in6 *)&their_addr;
            memcpy(&receivedAddr.mSrc,sockAddrIn6,sizeof(sockaddr_in6));
        }
    #else
        memcpy(&receivedAddr.mSrc,&sockAddrIn,sizeof(sockaddr_in));
    #endif
        //portnum=receivedAddr.getHSBPort();

        Address * forwardTarget;

        bool confirmed1 = forwardEntry->addr1Confirmed !=0 && !forwardEntry->addr1Confirmed->isInvalid();
        bool confirmed2 = forwardEntry->addr2Confirmed !=0 && !forwardEntry->addr2Confirmed->isInvalid();
        bool matchConfirmed1 = confirmed1 && forwardEntry->addr1Confirmed==receivedAddr;
        bool matchConfirmed2 = confirmed2 && forwardEntry->addr2Confirmed==receivedAddr;
        bool matchUnconfirmed1 = forwardEntry->addr1Unconfirmed.equal(receivedAddr);
        bool matchUnconfirmed2 = forwardEntry->addr2Unconfirmed.equal(receivedAddr);

        if (matchConfirmed1==true || (matchConfirmed2==false && confirmed1==false && matchUnconfirmed1==true))
        {
            // Forward to addr2
            if (forwardEntry->addr1Confirmed == 0 || forwardEntry->addr1Confirmed->isInvalid())
            {
                forwardEntry->addr1Confirmed = receivedAddr;
            }
            if (forwardEntry->addr2Confirmed != 0 && !forwardEntry->addr2Confirmed->isInvalid())
                forwardTarget=forwardEntry->addr2Confirmed;
            else
                forwardTarget=forwardEntry->addr2Unconfirmed;
        }
        else if (matchConfirmed2==true || (confirmed2==false && matchUnconfirmed2==true))
        {
            // Forward to addr1
            if (forwardEntry->addr2Confirmed == 0 || forwardEntry->addr2Confirmed->isInvalid())
            {
                forwardEntry->addr2Confirmed = receivedAddr;
            }
            if (forwardEntry->addr1Confirmed != 0 && !forwardEntry->addr1Confirmed->isInvalid())
                forwardTarget=forwardEntry->addr1Confirmed;
            else
                forwardTarget=forwardEntry->addr1Unconfirmed;
        }
        else
        {
            return;
        }

        len = 0;

        do
        {
    #if N_NET_PROTOCOL_VER == 6
            if(forwardTarget.mSrc.sin_family == AF_INET)
            {
                do
                {
                    len = N_net_sendto(forwardEntry->socket, data, receivedDataLen, 0,
                        (const sockaddr *) & forwardTarget.mSrc, sizeof(sockaddr_in));
                }
                while(len == 0);
            }
            else
            {
                do
                {
                    len = N_net_sendto(forwardEntry->socket, data, receivedDataLen, 0,
                        (const sockaddr *) & forwardTarget.mSrc, sizeof(sockaddr_in6));
                }
                while (len == 0);
            }
    #else
            do
            {
                len = N_net_sendto(forwardEntry->socket, data, receivedDataLen, 0,
                    (const sockaddr *) & forwardTarget.mSrc, sizeof(sockaddr_in));
            }
            while(len == 0);
    #endif
        }
        while(len == 0);

        forwardEntry->timeLastDatagramForwarded=curTime;
    #endif  // N_PLAT_NACL
    }
    //------------------------------------------------------------------------
    void UDPForwarder::UpdateUDPForwarder(void)
    {
        TimeDurMS curTime = N_Engine().getTimer().getMS();

        StartForwardingInputStruct *sfis;
        StartForwardingOutputStruct sfos;
        sfos.forwardingSocket=-1;
        sfos.forwardingPort=0;
        sfos.inputId=0;
        sfos.result=UDPFORWARDER_RESULT_COUNT;

    #ifdef _MSC_VER
    #pragma warning( disable : 4127 ) // warning C4127: conditional expression is constant
    #endif
        while (1)
        {
            sfis = startForwardingInput.Pop();
            if (sfis==0)
                break;

            if (GetUsedForwardEntries()>maxForwardEntries)
            {
                sfos.result = UDPFORWARDER_NO_SOCKETS;
            }
            else
            {
                sfos.result = UDPFORWARDER_RESULT_COUNT;

                for(Nui32 i = 0; i < forwardListNotUpdated.Size(); i++)
                {
                    if (
                        (forwardListNotUpdated[i]->addr1Unconfirmed==sfis->source &&
                        forwardListNotUpdated[i]->addr2Unconfirmed==sfis->destination)
                        ||
                        (forwardListNotUpdated[i]->addr1Unconfirmed==sfis->destination &&
                        forwardListNotUpdated[i]->addr2Unconfirmed==sfis->source)
                        )
                    {
                        ForwardEntry * fe = forwardListNotUpdated[i];
                        sfos.forwardingPort = SocketUtil::GetLocalPort(fe->socket);
                        sfos.forwardingSocket = fe->socket;
                        sfos.result=UDPFORWARDER_FORWARDING_ALREADY_EXISTS;
                        break;
                    }
                }

                if (sfos.result == UDPFORWARDER_RESULT_COUNT)
                {
                    int sock_opt;
                    sockaddr_in listenerSocketAddress;
                    listenerSocketAddress.sin_port = 0;
                    ForwardEntry *fe = N_new UDPForwarder::ForwardEntry();
                    fe->addr1Unconfirmed=sfis->source;
                    fe->addr2Unconfirmed=sfis->destination;
                    fe->timeoutOnNoDataMS=sfis->timeoutOnNoDataMS;

    #if N_NET_PROTOCOL_VER != 6
                    fe->socket = N_net_socket(AF_INET, SOCK_DGRAM, 0);
                    listenerSocketAddress.sin_family = AF_INET;
                    if (sfis->forceHostAddress.IsEmpty() == false)
                    {
                        listenerSocketAddress.sin_addr.s_addr = N_net_inet_addr( sfis->forceHostAddress.C_String() );
                    }
                    else
                    {
                        listenerSocketAddress.sin_addr.s_addr = INADDR_ANY;
                    }
                    int ret = N_net_bind( fe->socket, (struct sockaddr *) & listenerSocketAddress, sizeof( listenerSocketAddress ) );
                    if (ret==-1)
                    {
                        N_delete fe;
                        sfos.result=UDPFORWARDER_BIND_FAILED;
                    }
                    else
                    {
                        sfos.result=UDPFORWARDER_SUCCESS;
                    }

    #else // N_NET_PROTOCOL_VER == 6
                    struct addrinfo hints;
                    memset(&hints, 0, sizeof (addrinfo)); // make sure the struct is empty
                    hints.ai_family = sfis->socketFamily;
                    hints.ai_socktype = SOCK_DGRAM; // UDP sockets
                    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
                    struct addrinfo *servinfo=0, *aip;  // will point to the results

                    if (sfis->forceHostAddress.IsEmpty() || sfis->forceHostAddress=="INVALID")
                        getaddrinfo(0, "0", &hints, &servinfo);
                    else
                        getaddrinfo(sfis->forceHostAddress.C_String(), "0", &hints, &servinfo);

                    for (aip = servinfo; aip != NULL; aip = aip->ai_next)
                    {
                        // Open socket. The address type depends on what
                        // getaddrinfo() gave us.
                        fe->socket = N_net_socket(aip->ai_family, aip->ai_socktype, aip->ai_protocol);
                        if (fe->socket != -1)
                        {
                            int ret = N_net_bind( fe->socket, aip->ai_addr, (int) aip->ai_addrlen );
                            if (ret>=0)
                            {
                                break;
                            }
                            else
                            {
                                N_net_close(fe->socket);
                                fe->socket=-1;
                            }
                        }
                    }

                    if (fe->socket==-1)
                        sfos.result=UDPFORWARDER_BIND_FAILED;
                    else
                        sfos.result=UDPFORWARDER_SUCCESS;
    #endif  // N_NET_PROTOCOL_VER==6

                    if (sfos.result == UDPFORWARDER_SUCCESS)
                    {
                        sfos.forwardingPort = SocketUtil::GetLocalPort(fe->socket);
                        sfos.forwardingSocket=fe->socket;

                        sock_opt = 1024 * 256;
                        N_net_setsockopt(fe->socket, SOL_SOCKET, SO_RCVBUF, (char *)&sock_opt, sizeof (sock_opt));
                        sock_opt = 0;
                        N_net_setsockopt(fe->socket, SOL_SOCKET, SO_LINGER, (char *)&sock_opt, sizeof (sock_opt));
    #ifdef _WIN32
                        Nul nonblocking = 1;
                        N_net_ioctl(fe->socket, FIONBIO, &nonblocking);
    #else
                        fcntl(fe->socket, F_SETFL, O_NONBLOCK);
    #endif
                        forwardListNotUpdated.Insert(fe);
                    }
                }
            }

            // Push result
            sfos.inputId = sfis->inputId;
            startForwardingOutputMutex.Lock();
            startForwardingOutput.Push(sfos);
            startForwardingOutputMutex.Unlock();

            startForwardingInput.Deallocate(sfis);
        }

        StopForwardingStruct * sfs;

    #ifdef _MSC_VER
    #pragma warning( disable : 4127 ) // warning C4127: conditional expression is constant
    #endif
        while (1)
        {
            sfs = stopForwardingCommands.Pop();
            if (sfs == 0)
                break;

            ForwardEntry * fe;
            for (NCount i = 0; i < forwardListNotUpdated.Size(); ++i)
            {
                if ((forwardListNotUpdated[i]->addr1Unconfirmed==sfs->source &&
                    forwardListNotUpdated[i]->addr2Unconfirmed==sfs->destination)
                    ||
                    (forwardListNotUpdated[i]->addr1Unconfirmed==sfs->destination &&
                    forwardListNotUpdated[i]->addr2Unconfirmed==sfs->source))
                {
                    fe = forwardListNotUpdated[i];
                    forwardListNotUpdated.RemoveAtIndexFast(i);
                    N_delete fe;
                    break;
                }
            }
            stopForwardingCommands.Deallocate(sfs);
        }

        Nui32 i;
        i=0;
        while (i < forwardListNotUpdated.Size())
        {
            if (curTime > forwardListNotUpdated[i]->timeLastDatagramForwarded && // Account for timestamp wrap
                curTime > forwardListNotUpdated[i]->timeLastDatagramForwarded+forwardListNotUpdated[i]->timeoutOnNoDataMS)
            {
                N_delete forwardListNotUpdated[i];
                forwardListNotUpdated.RemoveAtIndex(i);
            }
            else
                i++;
        }

        ForwardEntry *forwardEntry;
        for (i=0; i < forwardListNotUpdated.Size(); i++)
        {
            forwardEntry = forwardListNotUpdated[i];
            RecvFrom(curTime, forwardEntry);
        }
    }
    //------------------------------------------------------------------------
}
}
#endif // #if _RAKNET_SUPPORT_FileOperations==1
