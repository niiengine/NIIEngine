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

#ifndef gsgsSOCKETBASE_H
#define gsgsSOCKETBASE_H

#include "gsgsPreInclude.h"
#include <wx/msgqueue.h>
#ifdef __WXOSX__
#include <sys/errno.h>
#endif

#ifdef _WIN32
#ifndef _WIN64
#include <winsock2.h>
#endif

typedef SOCKET socket_t;
typedef int socklen_t;
#else
typedef int socket_t;
#define INVALID_SOCKET -1
#endif

namespace gsgs
{
    class _gsgsAPI SocketException
    {
        std::string m_what;

    public:
        SocketException(const std::string& what)
            : m_what(what)
        {
            // trim whitespaces
            static std::string trimString("\r\n\t\v ");
            m_what.erase(0, m_what.find_first_not_of(trimString));
            m_what.erase(m_what.find_last_not_of(trimString) + 1);
        }
        ~SocketException() {}
        const std::string& what() const { return m_what; }
    };

    class _gsgsAPI SocketBase
    {
    protected:
        socket_t m_socket;
        bool m_closeOnExit;

    public:
        typedef wxSharedPtr<SocketBase> Ptr_t;
        typedef wxMessageQueue<SocketBase::Ptr_t> Queue_t;

        enum {
            kSuccess = 1,
            kTimeout = 2,
            kError = 3,
        };

    #ifdef _WIN32
        static const int eWouldBlock = WSAEWOULDBLOCK;
    #else
        static const int eWouldBlock = EWOULDBLOCK;
    #endif

        static int GetLastError();
        static std::string error();
        static std::string error(const int errorCode);

    public:
        /**
         * @brief set the socket into blocking/non-blocking mode
         * @param blocking
         */
        void MakeSocketBlocking(bool blocking);

        SocketBase(socket_t sockfd = INVALID_SOCKET);
        virtual ~SocketBase();

        void SetCloseOnExit(bool closeOnExit) { this->m_closeOnExit = closeOnExit; }
        bool IsCloseOnExit() const { return m_closeOnExit; }
        /**
         * @brief return the descriptor and clear this socket.
         */
        socket_t Release();

        /**
         * @brief initialize the socket library
         */
        static void Initialize();

        socket_t GetSocket() const { return m_socket; }

        /**
         * @brief
         * @param msg
         */
        void Send(const std::string& msg);
        void Send(const wxMemoryBuffer& msg);
        void Send(const wxString& msg, const wxMBConv& conv = wxConvUTF8);

        /**
         * @brief
         * @return
         */
        int Read(char* buffer, size_t bufferSize, size_t& bytesRead, long timeout = -1);
        /**
         * @brief read string content from remote server
         * @param content [output]
         * @param timeout seconds to wait
         */
        int Read(wxString& content, const wxMBConv& conv = wxConvUTF8, long timeout = -1);

        /**
         * @brief read a buffer from the socket
         * @param content [output]
         * @param timeout seconds to wait
         */
        int Read(wxMemoryBuffer& content, long timeout = -1);

        /**
         * @brief
         * @param seconds
         * @return
         */
        int SelectRead(long seconds = -1);

        /**
         * @brief select for read. Same as above, but use milli seconds instead
         * @param milliSeconds number of _milliseconds_ to wait
         * @return
         */
        int SelectReadMS(long milliSeconds = -1);

        /**
         * @brief select for write
         * @return
         */
        int SelectWrite(long seconds = -1);

        /**
         * @brief select for write (milli seconds version)
         * @return
         */
        int SelectWriteMS(long milliSeconds = -1);

        /**
         * @brief read a full message that was sent with 'SendMessage'
         * @param message
         * @param timeout seconds to wait
         * @return kSuccess, kTimeout or kError
         */
        int ReadMessage(wxString& message, int timeout);

        /**
         * @brief write a full message
         * @param message
         */
        void WriteMessage(const wxString& message);

    protected:
        /**
         * @brief
         */
        void DestroySocket();
    };
}
#endif
