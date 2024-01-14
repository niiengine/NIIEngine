#ifndef CLWEBSOCKETCLIENT_H
#define CLWEBSOCKETCLIENT_H

#include "gsgsPreInclude.h"
#include "gsgsThread.h"
#include "gsgsEvent.h"
#include <memory>

namespace gsgs
{
    class _gsgsAPI WebSocketClient : public wxEvtHandler
    {
    public:
        WebSocketClient(wxEvtHandler* owner);
        virtual ~WebSocketClient();
        
        /**
         * @brief prepare the socket before starting the main loop
         */
        void Initialise();
        
        /**
         * @brief connect to a remote URL. This function start a background thread to executes the main loop
         */
        void StartLoop(const wxString& url);

        /**
         * @brief send data over the websocket
         */
        void Send(const wxString& data);

        /**
         * @brief return the owner of this socket. The owner will receive events about new data etc in the form of events
         */
        wxEvtHandler* GetOwner() { return m_owner; }
        void SetConnectionHandle(std::weak_ptr<void> connection_handle) { this->m_connection_handle = connection_handle; }
        std::weak_ptr<void> GetConnectionHandle() const { return m_connection_handle; }

        /**
         * @brief close the connection
         */
        void Close();

        /**
         * @brief are we connected?
         */
        bool IsConnected() const { return m_helperThread != nullptr; }

        void OnHelperThreadExit();
    protected:
        template <typename T> T* GetClient()
        {
            if (!m_client) { return nullptr; }
            return reinterpret_cast<T*>(m_client);
        }
        void DoCleanup();
        void DoInit();
    protected:
        void* m_client = 0;
        std::weak_ptr<void> m_connection_handle;
        wxEvtHandler* m_owner = nullptr;
        Thread* m_helperThread = nullptr;
    };

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WEBSOCKET_CONNECTED, Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WEBSOCKET_DISCONNECTED, Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WEBSOCKET_ONMESSAGE, Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WEBSOCKET_ERROR, Event);
}
#endif
