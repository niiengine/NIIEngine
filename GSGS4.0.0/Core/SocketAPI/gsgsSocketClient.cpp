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

#include "gsgsConnectionString.h"
#include "gsgsSocketClient.h"

#ifndef _WIN32
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#endif

#ifdef __WXMSW__
#define RESET_ERRNO() WSASetLastError(0)
#else
#define RESET_ERRNO() errno = 0
#endif

namespace gsgs
{
    //------------------------------------------------------------------------
    SocketClient::SocketClient() {}
    //------------------------------------------------------------------------
    SocketClient::~SocketClient() {}
    //------------------------------------------------------------------------
    bool SocketClient::ConnectLocal(const wxString& socketPath)
    {
        DestroySocket();
    #ifndef __WXMSW__
        struct sockaddr_un server;
        m_socket = socket(AF_UNIX, SOCK_STREAM, 0);
        server.sun_family = AF_UNIX;
        strcpy(server.sun_path, socketPath.mb_str(wxConvUTF8).data());
        if(::connect(m_socket, (struct sockaddr*)&server, sizeof(struct sockaddr_un)) < 0) { return false; }
        return true;
    #else
        return false;
    #endif
    }
    //------------------------------------------------------------------------
    bool SocketClient::ConnectRemote(const wxString& address, int port, bool& wouldBlock, bool nonBlockingMode)
    {
        wouldBlock = false;
        DestroySocket();
        m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
        if(nonBlockingMode) { MakeSocketBlocking(false); }

        const char* ip_addr = address.mb_str(wxConvUTF8).data();
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

    #ifndef __WXMSW__
        if(inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) <= 0) {
            // restore socket to blocking mode
            if(nonBlockingMode) { MakeSocketBlocking(true); }
            return false;
        }
    #else
        serv_addr.sin_addr.s_addr = inet_addr(ip_addr);
    #endif

        RESET_ERRNO();
        int rc = ::connect(m_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    #ifdef __WXMSW__
        wouldBlock = (WSAGetLastError() == WSAEWOULDBLOCK);
    #else
        wouldBlock = (errno == EINPROGRESS);
    #endif

        // restore socket to blocking mode
        if(nonBlockingMode) { MakeSocketBlocking(true); }
        return rc == 0;
    }
    //------------------------------------------------------------------------
    bool SocketClient::Connect(const wxString& connectionString, bool nonBlockingMode)
    {
        ConnectionString cs(connectionString);
        if(!cs.IsOK()) { return false; }
        if(cs.GetProtocol() == ConnectionString::kUnixLocalSocket) {
            return ConnectLocal(cs.GetPath());
        } else {
            // TCP
            bool wouldBlock = false;
            return ConnectRemote(cs.GetHost(), cs.GetPort(), wouldBlock, nonBlockingMode);
        }
    }
    //------------------------------------------------------------------------
    bool SocketClient::ConnectNonBlocking(const wxString& connectionString, bool& wouldBlock)
    {
        wouldBlock = false;
        ConnectionString cs(connectionString);
        if(!cs.IsOK()) { return false; }
        if(cs.GetProtocol() == ConnectionString::kUnixLocalSocket) {
            return ConnectLocal(cs.GetPath());
        } else {
            // TCP
            return ConnectRemote(cs.GetHost(), cs.GetPort(), wouldBlock, true);
        }
    }
    //------------------------------------------------------------------------
}