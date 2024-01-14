#include "SocketAPI/gsgsSocketBase.h"
#include "gsgsWebSocketClient.h"

// No boost or other external libraries, just plain old good C++11
#define ASIO_STANDALONE 1
#define _WEBSOCKETPP_CPP11_THREAD_ 1

#include "gsgsLogManager.h"
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> Client_t;
// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

namespace gsgs
{
    wxDEFINE_EVENT(wxEVT_WEBSOCKET_CONNECTED, Event);
    wxDEFINE_EVENT(wxEVT_WEBSOCKET_DISCONNECTED, Event);
    wxDEFINE_EVENT(wxEVT_WEBSOCKET_ONMESSAGE, Event);
    wxDEFINE_EVENT(wxEVT_WEBSOCKET_ERROR, Event);

    class WebSocketHelperThread : public Thread
    {
        wxEvtHandler* m_owner = nullptr;
        wxString m_url;
        Client_t* m_client = nullptr;

    public:
        WebSocketHelperThread(wxEvtHandler* owner, const wxString& url, Client_t* c)
            : m_owner(owner)
            , m_url(url)
            , m_client(c)
        {
        }

        virtual ~WebSocketHelperThread() {}

        void* Entry()
        {
            while(!TestDestroy()) {
                try {
                    // Start the main loop
                    // We use a second try/catch block here to make sure that the error reporting is done
                    // Start the ASIO io_service run loop
                    // this will cause a single connection to be made to the server. c.run()
                    // will exit when this connection is closed.
                    m_client->run();

                } catch(websocketpp::exception& e) {
                    // Report an error
                    Event event(wxEVT_WEBSOCKET_ERROR);
                    event.SetString(e.what());
                    m_owner->AddPendingEvent(event);
                }
            }
            m_owner->CallAfter(&WebSocketClient::OnHelperThreadExit);
            return NULL;
        }
    };

    using websocketpp::lib::bind;
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;

    // This message handler will be invoked once for each incoming message
    static void on_ws_message(WebSocketClient* c, websocketpp::connection_hdl hdl, message_ptr msg)
    {
        gsgsDEBUG1() << "<--" << msg->get_payload();
        Event event(wxEVT_WEBSOCKET_ONMESSAGE);
        event.SetString(msg->get_payload());
        event.SetEventObject(c);
        c->GetOwner()->AddPendingEvent(event);
    }

    static void on_ws_open_handler(WebSocketClient* c, websocketpp::connection_hdl hdl)
    {
        gsgsDEBUG1() << "<-- Connected!";
        c->SetConnectionHandle(hdl);
        Event event(wxEVT_WEBSOCKET_CONNECTED);
        event.SetEventObject(c);
        c->GetOwner()->AddPendingEvent(event);
    }

    static void on_ws_fail_handler(WebSocketClient* c, websocketpp::connection_hdl hdl)
    {
        gsgsDEBUG1() << "<-- Error!";
        Event event(wxEVT_WEBSOCKET_ERROR);
        event.SetEventObject(c);
        c->GetOwner()->AddPendingEvent(event);
    }

    WebSocketClient::WebSocketClient(wxEvtHandler* owner)
        : m_owner(owner)
    {
        DoInit();
    }

    WebSocketClient::~WebSocketClient()
    {
        Close();
        DoCleanup();
    }

    void WebSocketClient::StartLoop(const wxString& url)
    {
        if(m_helperThread) { throw SocketException("A websocket loop is already running"); }

        Client_t* c = GetClient<Client_t>();
        if(!c) { throw SocketException("Invalid connection!"); }
        try {
            std::string uri = url.mb_str(wxConvUTF8).data();
            // Register our message handler
            websocketpp::lib::error_code ec;
            Client_t::connection_ptr con = c->get_connection(uri, ec);
            if(ec) { throw SocketException(ec.message()); }

            // Note that connect here only requests a connection. No network messages are
            // exchanged until the event loop starts running in the next line.
            c->connect(con);
        } catch(websocketpp::exception& e) {
            m_connection_handle.reset();
            throw SocketException(e.what());
        }

        // Run the main loop in background thread
        m_helperThread = new WebSocketHelperThread(this, url, c);
        m_helperThread->Start();
    }

    void WebSocketClient::Send(const wxString& data)
    {
        Client_t* c = GetClient<Client_t>();
        if(!c) { throw SocketException("Invalid connection!"); }
        if(m_connection_handle.expired()) { throw SocketException("Invalid connection handle!"); }

        try {
            std::string str = data.mb_str(wxConvUTF8).data();
            websocketpp::lib::error_code ec;
            c->send(GetConnectionHandle(), str, websocketpp::frame::opcode::TEXT, ec);

        } catch(websocketpp::exception& e) {
            throw SocketException(e.what());
        }
    }

    void WebSocketClient::Close()
    {
        Client_t* c = GetClient<Client_t>();
        if(!c) { return; }
        c->stop();
        DoCleanup();
    }

    void WebSocketClient::DoCleanup()
    {
        // Cleanup: first close the helper thread
        wxDELETE(m_helperThread);
        m_connection_handle.reset();
        Client_t* c = GetClient<Client_t>();
        wxDELETE(c);
        m_client = nullptr;
    }

    void WebSocketClient::OnHelperThreadExit()
    {
        DoCleanup();
        Event event(wxEVT_WEBSOCKET_DISCONNECTED);
        event.SetEventObject(this);
        GetOwner()->AddPendingEvent(event);
    }

    void WebSocketClient::DoInit()
    {
        // Dont initialise again
        if(m_client) { return; }

        try {
            m_client = new Client_t();
            Client_t* c = GetClient<Client_t>();
            //c->set_access_channels(websocketpp::log::alevel::all);
            c->clear_access_channels(websocketpp::log::alevel::all);
            c->init_asio();
            c->set_message_handler(bind(&on_ws_message, this, ::_1, ::_2));
            c->set_open_handler(bind(&on_ws_open_handler, this, ::_1));
            c->set_fail_handler(bind(&on_ws_fail_handler, this, ::_1));
        } catch(websocketpp::exception& e) {
            gsgsERROR() << e.what();
        }
    }

    void WebSocketClient::Initialise() { DoInit(); }
}