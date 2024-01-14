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

#include "gsgsSocketBase.h"
#include <cerrno>
#include <cstdio>
#include <sstream>
#include <memory>

#ifndef _WIN32
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif

namespace gsgs
{
    SocketBase::SocketBase(socket_t sockfd)
        : m_socket(sockfd)
        , m_closeOnExit(true)
    {
    }

    SocketBase::~SocketBase() { DestroySocket(); }

    void SocketBase::Initialize()
    {
    #ifdef _WIN32
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);
    #endif
    }

    // Read API
    int SocketBase::Read(wxMemoryBuffer& content, long timeout)
    {
        content.Clear();

        char buffer[4096];
        timeout = (timeout * 1000); // convert to MS
        while(true && timeout) {
            int rc = SelectReadMS(10);
            timeout -= 10;
            if(rc == kSuccess) {
                memset(buffer, 0x0, sizeof(buffer));
                int bytesRead = recv(m_socket, buffer, sizeof(buffer), 0);
                if(bytesRead < 0) {
                    const int err = GetLastError();

                    if(eWouldBlock != err) {
                        // Error
                        throw SocketException("Read failed: " + error(err));
                    }

                } else if(bytesRead == 0) {
                    // connection closed
                    return kError;

                } else {
                    content.AppendData(buffer, bytesRead);
                    continue;
                }
            } else {
                if(content.IsEmpty())
                    continue; // keep waiting until time ends
                else
                    return kSuccess; // we already read our content
            }
        }
        return kTimeout;
    }

    int SocketBase::Read(wxString& content, const wxMBConv& conv, long timeout)
    {
        wxMemoryBuffer mb;
        int rc = Read(mb, timeout);
        if(rc == kSuccess) {
            content = wxString((const char*)mb.GetData(), conv, mb.GetDataLen());
        }
        return rc;
    }

    int SocketBase::Read(char* buffer, size_t bufferSize, size_t& bytesRead, long timeout)
    {
        if(SelectRead(timeout) == kTimeout) {
            return kTimeout;
        }
        memset(buffer, 0, bufferSize);
        const int res = recv(m_socket, buffer, bufferSize, 0);

        if(res < 0) {
            const int err = GetLastError();
            if(eWouldBlock == err) {
                return kTimeout;
            }

            throw SocketException("Read failed: " + error(err));
        }
        else if(0 == res) {
            throw SocketException("Read failed: " + error());
        }

        bytesRead = static_cast<size_t>(res);
        return kSuccess;
    }

    int SocketBase::SelectRead(long seconds)
    {
        if(seconds == -1) {
            return kSuccess;
        }

        if(m_socket == INVALID_SOCKET) {
            throw SocketException("Invalid socket!");
        }

        struct timeval tv = { seconds, 0 };

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(m_socket, &readfds);
        int rc = select(m_socket + 1, &readfds, NULL, NULL, &tv);
        if(rc == 0) {
            // timeout
            return kTimeout;

        } else if(rc < 0) {
            // an error occurred
            throw SocketException("SelectRead failed: " + error());

        } else {
            // we got something to read
            return kSuccess;
        }
    }

    // Send API
    void SocketBase::Send(const wxString& msg, const wxMBConv& conv)
    {
        if(m_socket == INVALID_SOCKET) {
            throw SocketException("Invalid socket!");
        }
        wxCharBuffer cb = msg.mb_str(conv).data();
        wxMemoryBuffer mb;
        mb.AppendData(cb.data(), cb.length());
        Send(mb);
    }

    void SocketBase::Send(const std::string& msg)
    {
        if(m_socket == INVALID_SOCKET) {
            throw SocketException("Invalid socket!");
        }
        wxMemoryBuffer mb;
        mb.AppendData(msg.c_str(), msg.length());
        Send(mb);
    }

    void SocketBase::Send(const wxMemoryBuffer& msg)
    {
        if(m_socket == INVALID_SOCKET) {
            throw SocketException("Invalid socket!");
        }
        char* pdata = (char*)msg.GetData();
        int bytesLeft = msg.GetDataLen();
        while(bytesLeft) {
            if(SelectWriteMS(1000) == kTimeout) continue;
            int bytesSent = ::send(m_socket, (const char*)pdata, bytesLeft, 0);
            if(bytesSent <= 0) throw SocketException("Send error: " + error());
            pdata += bytesSent;
            bytesLeft -= bytesSent;
        }
    }

    int SocketBase::GetLastError()
    {
    #ifdef _WIN32
        return ::WSAGetLastError();
    #else
        return errno;
    #endif
    }

    std::string SocketBase::error()
    {
        return error(GetLastError());
    }

    std::string SocketBase::error(const int errorCode)
    {
        std::string err;
    #ifdef _WIN32
        // Get the error message, if any.
        if(errorCode == 0) return "No error message has been recorded";

        LPSTR messageBuffer = nullptr;
        size_t size =
            FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                           NULL,
                           errorCode,
                           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                           (LPSTR)&messageBuffer,
                           0,
                           NULL);

        std::string message(messageBuffer, size);

        // Free the buffer.
        LocalFree(messageBuffer);
        err = message;
    #else
        err = strerror(errorCode);
    #endif
        return err;
    }

    void SocketBase::DestroySocket()
    {
        if(IsCloseOnExit()) {
            if(m_socket != INVALID_SOCKET) {
    #ifdef _WIN32
                ::shutdown(m_socket, 2);
                ::closesocket(m_socket);
    #else
                ::shutdown(m_socket, 2);
                ::close(m_socket);
    #endif
            }
        }
        m_socket = INVALID_SOCKET;
    }

    int SocketBase::ReadMessage(wxString& message, int timeout)
    {
        // send the length in string form to avoid binary / arch differences between remote and local machine
        char msglen[11];
        memset(msglen, 0, sizeof(msglen));

        size_t message_len(0);
        size_t bytesRead(0);
        int rc = Read((char*)msglen, sizeof(msglen) - 1, bytesRead, timeout);
        if(rc != kSuccess) {
            // timeout
            return rc;
        }

        // convert the string to int
        message_len = ::atoi(msglen);

        bytesRead = 0;
        std::unique_ptr<char> pBuff(new char[message_len]);

        // read the entire amount we need
        int bytesLeft = message_len;
        int totalRead = 0;
        while(bytesLeft > 0) {
            rc = Read(pBuff.get() + totalRead, bytesLeft, bytesRead, timeout);
            if(rc != kSuccess) {
                return rc;

            } else {
                bytesLeft -= bytesRead;
                totalRead += bytesRead;
                bytesRead = 0;
            }
        }

        message.assign(pBuff.get(), message_len);
        return kSuccess;
    }

    void SocketBase::WriteMessage(const wxString& message)
    {
        if(m_socket == INVALID_SOCKET) {
            throw SocketException("Invalid socket!");
        }

        // Write the message length
        std::string c_str = message.mb_str(wxConvUTF8).data();
        int len = c_str.length();

        // send the length in string form to avoid binary / arch differences between remote and local machine
        char msglen[11];
        memset(msglen, 0, sizeof(msglen));
        sprintf(msglen, "%010d", len);
        // send it without the NULL byte
        if(::write(m_socket, msglen, sizeof(msglen) - 1) < 0) {
            throw SocketException("Send error: " + error(errno));
        }

        // now send the actual data
        Send(c_str);
    }

    socket_t SocketBase::Release()
    {
        int fd = m_socket;
        m_socket = INVALID_SOCKET;
        return fd;
    }

    void SocketBase::MakeSocketBlocking(bool blocking)
    {
    #ifndef __WXMSW__
        // set socket to non-blocking mode
        int flags;
        flags = ::fcntl(m_socket, F_GETFL);
        if(blocking) {
            flags &= ~O_NONBLOCK;
        } else {
            flags |= O_NONBLOCK;
        }
        ::fcntl(m_socket, F_SETFL, flags);
    #else
        u_long iMode = blocking ? 0 : 1;
        ::ioctlsocket(m_socket, FIONBIO, &iMode);
    #endif
    }

    int SocketBase::SelectWriteMS(long milliSeconds)
    {
        if(milliSeconds == -1) {
            return kSuccess;
        }

        if(m_socket == INVALID_SOCKET) {
            throw SocketException("Invalid socket!");
        }

        struct timeval tv;
        tv.tv_sec = milliSeconds / 1000;
        tv.tv_usec = (milliSeconds % 1000) * 1000;

        fd_set write_set;
        FD_ZERO(&write_set);
        FD_SET(m_socket, &write_set);
        errno = 0;
        int rc = select(m_socket + 1, NULL, &write_set, NULL, &tv);
        if(rc == 0) {
            // timeout
            return kTimeout;

        } else if(rc < 0) {
            // an error occurred
            throw SocketException("SelectWriteMS failed: " + error());

        } else {
            // we got something to read
            return kSuccess;
        }
    }

    int SocketBase::SelectWrite(long seconds)
    {
        if(seconds == -1) {
            return kSuccess;
        }

        if(m_socket == INVALID_SOCKET) {
            throw SocketException("Invalid socket!");
        }

        struct timeval tv = { seconds, 0 };

        fd_set write_set;
        FD_ZERO(&write_set);
        FD_SET(m_socket, &write_set);
        errno = 0;
        int rc = select(m_socket + 1, NULL, &write_set, NULL, &tv);
        if(rc == 0) {
            // timeout
            return kTimeout;

        } else if(rc < 0) {
            // an error occurred
            throw SocketException("SelectRead failed: " + error());

        } else {
            // we got something to read
            return kSuccess;
        }
    }

    int SocketBase::SelectReadMS(long milliSeconds)
    {
        if(milliSeconds == -1) {
            return kSuccess;
        }

        if(m_socket == INVALID_SOCKET) {
            throw SocketException("Invalid socket!");
        }

        int seconds = milliSeconds / 1000; // convert the number into seconds
        int ms = milliSeconds % 1000;      // the remainder is less than a second
        struct timeval tv = { seconds, ms * 1000 };

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(m_socket, &readfds);
        int rc = select(m_socket + 1, &readfds, NULL, NULL, &tv);
        if(rc == 0) {
            // timeout
            return kTimeout;

        } else if(rc < 0) {
            // an error occurred
            throw SocketException("SelectRead failed: " + error());

        } else {
            // we got something to read
            return kSuccess;
        }
    }
}