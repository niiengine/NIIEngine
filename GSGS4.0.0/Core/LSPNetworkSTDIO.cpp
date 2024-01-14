#include "LSPNetworkSTDIO.h"
#include "gsgsJSON.h"
#include <sstream>
#include "gsgsLogManager.h"
#include "gsgsCommon.h"
#include "gsgsSubPrc.h"
#include "process_impl.h"

LSPNetworkSTDIO::LSPNetworkSTDIO() {}

LSPNetworkSTDIO::~LSPNetworkSTDIO() { Close(); }

void LSPNetworkSTDIO::Close() { wxDELETE(m_server); }

void LSPNetworkSTDIO::Open(const LSPStartupInfo& siInfo)
{
    m_startupInfo = siInfo;

    // Start the LSP server first
    Close();

    m_server = new gsgs::SubPrc();
    m_server->Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &LSPNetworkSTDIO::OnProcessOutput, this);
    m_server->Bind(wxEVT_ASYNC_PROCESS_STDERR, &LSPNetworkSTDIO::OnProcessStderr, this);
    m_server->Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &LSPNetworkSTDIO::OnProcessTerminated, this);

    DirSaver ds;
    if(!siInfo.GetWorkingDirectory().IsEmpty()) { ::wxSetWorkingDirectory(siInfo.GetWorkingDirectory()); }
    m_server->Start(siInfo.GetLspServerCommand());
    
    gsgs::Event evtReady(wxEVT_LSP_NET_CONNECTED);
    AddPendingEvent(evtReady);
}

void LSPNetworkSTDIO::Send(const std::string& data)
{
    if(m_server) {
        gsgsDEBUG() << "LSPNetworkSTDIO:\n" << data;
        m_server->Write(data);
    } else {
        gsgsDEBUG() << "LSPNetworkSTDIO: no process !?";
    }
}

bool LSPNetworkSTDIO::IsConnected() const { return m_server != nullptr; }

void LSPNetworkSTDIO::OnProcessTerminated(gsgs::ProcessEvent& event)
{
    wxDELETE(m_server);
    gsgsDEBUG() << "LSPNetworkSTDIO: program terminated:" << m_startupInfo.GetLspServerCommand();
    gsgs::Event evt(wxEVT_LSP_NET_ERROR);
    AddPendingEvent(evt);
}

void LSPNetworkSTDIO::OnProcessOutput(gsgs::ProcessEvent& event)
{
    const wxString& dataRead = event.GetOutput();
    gsgs::Event evt(wxEVT_LSP_NET_DATA_READY);
    evt.SetString(dataRead);
    AddPendingEvent(evt);
}

void LSPNetworkSTDIO::OnProcessStderr(gsgs::ProcessEvent& event) { gsgsDEBUG() << event.GetOutput(); }
