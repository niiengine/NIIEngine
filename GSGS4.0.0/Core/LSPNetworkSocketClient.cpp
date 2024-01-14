#include "LSPNetworkSocketClient.h"
#include "gsgsLogManager.h"
#include "gsgsCommon.h"

LSPNetworkSocketClient::LSPNetworkSocketClient() {}

LSPNetworkSocketClient::~LSPNetworkSocketClient() { Close(); }

void LSPNetworkSocketClient::Close()
{
    if(m_lspServer) { m_lspServer->Detach(); }
    wxDELETE(m_lspServer);
    m_socket.reset(nullptr);
    m_pid = wxNOT_FOUND;
}

static wxString& wrap_with_quotes(wxString& str)
{
    if(str.Contains(" ")) { str.Prepend("\"").Append("\""); }
    return str;
}

wxString BuildCommand(const wxArrayString& args)
{
    if(args.empty()) { return ""; }
    // Build command line from the array
    wxString command;
    command << args[0];

    wrap_with_quotes(command);
    for(size_t i = 1; i < args.size(); ++i) {
        wxString argument = args[i];
        wrap_with_quotes(argument);
        command << " " << argument;
    }
    return command;
}

bool LSPNetworkSocketClient::IsConnected() const { return m_socket != nullptr; }
void LSPNetworkSocketClient::Open(const LSPStartupInfo& info)
{
    m_startupInfo = info;
    // Start the process
    wxString cmd = BuildCommand(m_startupInfo.GetLspServerCommand());
    m_lspServer = ::CreateAsyncProcess(this, cmd, IProcessCreateDefault, m_startupInfo.GetWorkingDirectory());
    if(!m_lspServer) {
        gsgs::Event evt(wxEVT_LSP_NET_ERROR);
        evt.SetString(wxString() << "Failed to execute: " << cmd);
        AddPendingEvent(evt);
        return;
    }

    m_lspServer->Detach(); // we dont want events
    m_pid = m_lspServer->GetPid();

    // Now that the process is up, connect to the server
    m_socket.reset(new gsgs::AsyncSocket(m_startupInfo.GetConnectioString(), kAsyncSocketBuffer | kAsyncSocketClient));
    m_socket->Bind(wxEVT_ASYNC_SOCKET_CONNECTED, &LSPNetworkSocketClient::OnSocketConnected, this);
    m_socket->Bind(wxEVT_ASYNC_SOCKET_CONNECTION_LOST, &LSPNetworkSocketClient::OnSocketConnectionLost, this);
    m_socket->Bind(wxEVT_ASYNC_SOCKET_CONNECT_ERROR, &LSPNetworkSocketClient::OnSocketConnectionError, this);
    m_socket->Bind(wxEVT_ASYNC_SOCKET_ERROR, &LSPNetworkSocketClient::OnSocketError, this);
    m_socket->Bind(wxEVT_ASYNC_SOCKET_INPUT, &LSPNetworkSocketClient::OnSocketData, this);
    m_socket->Start();
}

void LSPNetworkSocketClient::Send(const std::string& data)
{
    if(m_socket) {
        gsgsDEBUG() << "LSP socket:\n" << data;
        m_socket->Send(data);
    } else {
        gsgsDEBUG() << "LSP socket: no socket !?";
    }
}

void LSPNetworkSocketClient::OnSocketConnected(gsgs::Event& event)
{
    // Now that everything is up, send the "initialize" request
    gsgs::Event evtReady(wxEVT_LSP_NET_CONNECTED);
    AddPendingEvent(evtReady);
}

void LSPNetworkSocketClient::OnSocketConnectionLost(gsgs::Event& event)
{
    gsgs::Event evt(wxEVT_LSP_NET_ERROR);
    evt.SetString(event.GetString());
    AddPendingEvent(evt);
}

void LSPNetworkSocketClient::OnSocketConnectionError(gsgs::Event& event)
{
    gsgs::Event evt(wxEVT_LSP_NET_ERROR);
    evt.SetString(event.GetString());
    AddPendingEvent(evt);
}

void LSPNetworkSocketClient::OnSocketError(gsgs::Event& event)
{
    gsgs::Event evt(wxEVT_LSP_NET_ERROR);
    evt.SetString(event.GetString());
    AddPendingEvent(evt);
}

void LSPNetworkSocketClient::OnSocketData(gsgs::Event& event)
{
    const wxString& dataRead = event.GetString();
    gsgs::Event evt(wxEVT_LSP_NET_DATA_READY);
    evt.SetString(dataRead);
    AddPendingEvent(evt);
}
