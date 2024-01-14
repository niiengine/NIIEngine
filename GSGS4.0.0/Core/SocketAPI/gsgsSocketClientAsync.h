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

#ifndef gsgsSOCKETCLIENTASYNC_H
#define gsgsSOCKETCLIENTASYNC_H

#include "gsgsPreInclude.h"
#include "SocketAPI/gsgsSocketBase.h"
#include "SocketAPI/gsgsSocketClient.h"
#include "gsgsEvent.h"
#include "gsgsThread.h"
#include <wx/msgqueue.h>

namespace gsgs
{
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_ASYNC_SOCKET_CONNECTED, gsgs::Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_ASYNC_SOCKET_CONNECTION_LOST, gsgs::Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_ASYNC_SOCKET_INPUT, gsgs::Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_ASYNC_SOCKET_ERROR, gsgs::Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_ASYNC_SOCKET_CONNECT_ERROR, gsgs::Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_ASYNC_SOCKET_SERVER_READY, gsgs::Event);

    enum eAsyncSocketMode {
        kAsyncSocketClient = (1 << 0),
        kAsyncSocketServer = (1 << 1),
        kAsyncSocketMessage = (1 << 2),
        kAsyncSocketBuffer = (1 << 3),
        kAsyncSocketNonBlocking = (1 << 4),
    };

    class _gsgsAPI SocketAsyncThread : public wxThread
    {
    public:
        enum eCommand {
            kSend,
            kDisconnect,
        };

        struct MyRequest {
            eCommand m_command;
            std::string m_buffer;
        };

    protected:
        wxEvtHandler* m_sink;
        wxString m_keepAliveMessage;
        wxString m_connectionString;
        wxMessageQueue<MyRequest> m_queue;
        size_t m_mode = (kAsyncSocketClient | kAsyncSocketBuffer);

    protected:
        void MessageLoop(gsgs::SocketBase::Ptr_t socket);
        void BufferLoop(gsgs::SocketBase::Ptr_t socket);

        void* ClientMain();
        void* ServerMain();

    public:
        virtual void AddRequest(const MyRequest& req) { m_queue.Post(req); }
        virtual void* Entry();

        /**
         * @brief start the worker thread
         */
        void Start()
        {
            Create();
            Run();
        }

        /**
         * @brief stop and wait for the thread to terminate
         */
        void Stop()
        {
            if(IsAlive()) {
                Delete(NULL, wxTHREAD_WAIT_BLOCK);

            } else {
                Wait(wxTHREAD_WAIT_BLOCK);
            }
        }

        SocketAsyncThread(wxEvtHandler* sink, const wxString& connectionString, size_t mode,
                            const wxString& keepAliveMessage = "");
        virtual ~SocketAsyncThread();
    };

    class _gsgsAPI AsyncSocket : public wxEvtHandler
    {
        SocketAsyncThread* m_thread;
        size_t m_mode = kAsyncSocketClient | kAsyncSocketBuffer;
        wxString m_connectionString;

    public:
        typedef wxSharedPtr<AsyncSocket> Ptr_t;

    public:
        AsyncSocket(const wxString& connectionString,
                      size_t mode = (kAsyncSocketClient | kAsyncSocketBuffer));
        virtual ~AsyncSocket();

        /**
         * @brief connect using connection string
         */
        void Start();
        void Stop();
        void Send(const wxString& buffer);
        void Send(const std::string& buffer);
    };
}
#endif // CLSOCKETCLIENTASYNC_H
