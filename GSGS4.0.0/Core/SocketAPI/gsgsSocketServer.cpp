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
#include "gsgsSocketServer.h"

#ifndef _WIN32
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#endif

namespace gsgs
{
    SocketServer::SocketServer() {}

    SocketServer::~SocketServer() { DestroySocket(); }

    int SocketServer::CreateServer(const std::string& pipePath)
    {
    #ifndef __WXMSW__
        unlink(pipePath.c_str());

        // Create a socket
        if((m_socket = ::socket(AF_UNIX, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            throw gsgs::SocketException("Could not create socket: " + error());
        }

        // must set reuse-address
        int optval;

        // set SO_REUSEADDR on a socket to true (1):
        optval = 1;
        ::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));

        // Prepare the sockaddr_in structure
        struct sockaddr_un server;
        server.sun_family = AF_UNIX;
        strcpy(server.sun_path, pipePath.c_str());

        // Bind
        if(::bind(m_socket, (struct sockaddr*)&server, sizeof(server)) == -1) {
            throw gsgs::SocketException("CreateServer: bind operation failed: " + error());
        }

        char mode[] = "0777";
        int newMode = ::strtol(mode, 0, 8);
        ::chmod(pipePath.c_str(), newMode);

        // define the accept queue size
        ::listen(m_socket, 10);
        return 0;
    #else
        int port = ::atoi(pipePath.c_str());
        return CreateServer("127.0.0.1", port);
    #endif
    }

    int SocketServer::CreateServer(const std::string& address, int port)
    {
        // Create a socket
        if((m_socket = ::socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            throw gsgs::SocketException("Could not create socket: " + error());
        }

        // must set reuse-address
        int optval;

        // set SO_REUSEADDR on a socket to true (1):
        optval = 1;
        ::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));

        // Prepare the sockaddr_in structure
        struct sockaddr_in server;
        server.sin_family = AF_INET;
    #ifdef __WXMSW__
        server.sin_addr.s_addr = inet_addr(address.c_str());
    #else
        inet_pton(AF_INET, address.c_str(), &server.sin_addr);
    #endif
        server.sin_port = htons(port);

        // Bind
        if(::bind(m_socket, (struct sockaddr*)&server, sizeof(server)) != 0) {
            throw gsgs::SocketException("CreateServer: bind() error: " + error());
        }

        if(port == 0) {
            struct sockaddr_in socket_name;
    #ifdef __WXMSW__
            int name_len = sizeof(socket_name);
    #else
            socklen_t name_len = sizeof(socket_name);
    #endif
            if(::getsockname(m_socket, (struct sockaddr*)&socket_name, &name_len) != 0) {
                throw gsgs::SocketException("CreateServer: getsockname() error: " + error());
            }
            port = ntohs(socket_name.sin_port);
        }
        // define the accept queue size
        if(::listen(m_socket, 10) != 0) { throw gsgs::SocketException("CreateServer: listen() error: " + error()); }

        // return the bound port number
        return port;
    }

    int SocketServer::Start(const wxString& connectionString)
    {
        ConnectionString cs(connectionString);
        if(!cs.IsOK()) { throw gsgs::SocketException("Invalid connection string provided"); }
        if(cs.GetProtocol() == ConnectionString::kTcp) {
            return CreateServer(cs.GetHost().mb_str(wxConvUTF8).data(), cs.GetPort());
        } else {
            return CreateServer(cs.GetPath().mb_str(wxConvUTF8).data());
        }
    }

    gsgs::SocketBase::Ptr_t SocketServer::WaitForNewConnection(long timeout)
    {
        return gsgs::SocketBase::Ptr_t(WaitForNewConnectionRaw(timeout));
    }

    gsgs::SocketBase* SocketServer::WaitForNewConnectionRaw(long timeout)
    {
        if(SelectRead(timeout) == kTimeout) { return NULL; }
        int fd = ::accept(m_socket, 0, 0);
        if(fd < 0) { throw gsgs::SocketException("accept error: " + error()); }
        return new gsgs::SocketBase(fd);
    }
}