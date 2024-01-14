#include "LSPNetwork.h"

LSPStartupInfo::LSPStartupInfo() {}

LSPStartupInfo::LSPStartupInfo(const LSPStartupInfo& other) { *this = other; }

LSPStartupInfo& LSPStartupInfo::operator=(const LSPStartupInfo& other)
{
    m_lspServerCommand = other.m_lspServerCommand;
    m_workingDirectory = other.m_workingDirectory;
    m_connectioString = other.m_connectioString;
    m_flags = other.m_flags;
    return *this;
}

LSPStartupInfo::~LSPStartupInfo() {}

wxDEFINE_EVENT(wxEVT_LSP_NET_DATA_READY, gsgs::Event);
wxDEFINE_EVENT(wxEVT_LSP_NET_ERROR, gsgs::Event);
wxDEFINE_EVENT(wxEVT_LSP_NET_CONNECTED, gsgs::Event);

LSPNetwork::LSPNetwork()
{
}

LSPNetwork::~LSPNetwork()
{
}

