#ifndef LSPNETWORK_H
#define LSPNETWORK_H

#include "gsgsPreInclude.h"
#include "gsgsEvent.h"

class _gsgsAPI LSPStartupInfo
{
public:
    enum eFlags {
        kShowConsole = (1 << 0),
        kUseTcp = (1 << 1),
    };
public:
    LSPStartupInfo& SetConnectioString(const wxString& connectioString)
    {
        this->m_connectioString = connectioString;
        return *this;
    }
    inline const wxString& GetConnectioString() const           { return m_connectioString; }
    inline void SetLspServerCommand(const wxArrayString& cmd)   { this->m_lspServerCommand = cmd; }
    inline const wxArrayString& GetLspServerCommand() const     { return m_lspServerCommand; }

    inline void SetFlags(size_t flags)                          { this->m_flags = flags; }
    inline size_t GetFlags() const                              { return m_flags; }

    inline LSPStartupInfo& SetWorkingDirectory(const wxString& workingDirectory)
    {
        this->m_workingDirectory = workingDirectory;
        return *this;
    }
    inline const wxString& GetWorkingDirectory() const          { return m_workingDirectory; }

    LSPStartupInfo();
    LSPStartupInfo(const LSPStartupInfo& other);
    virtual ~LSPStartupInfo();
    LSPStartupInfo& operator=(const LSPStartupInfo& other);
protected:
    wxString m_connectioString;
    wxArrayString m_lspServerCommand;
    wxString m_workingDirectory;
    size_t m_flags = 0;
};

wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_LSP_NET_DATA_READY, gsgs::Event);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_LSP_NET_ERROR, gsgs::Event);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_LSP_NET_CONNECTED, gsgs::Event);

enum class eNetworkType {
    kStdio,
    kTcpIP,
};

class _gsgsAPI LSPNetwork : public wxEvtHandler
{
public:
    typedef wxSharedPtr<LSPNetwork> Ptr_t;

    inline LSPNetwork & SetStartupInfo(const LSPStartupInfo & startupInfo)
    {
        this->m_startupInfo = startupInfo;
        return *this;
    }
    inline const LSPStartupInfo& GetStartupInfo() const 
    { 
        return m_startupInfo; 
    }
public:
    LSPNetwork();
    virtual ~LSPNetwork();

    //===-----------------------------------
    // Interface
    //===-----------------------------------

    /**
     * @brief send data to the LSP server
     */
    virtual void Send(const std::string& data) = 0;

    /**
     * @brief close the connection to the LSP server
     */
    virtual void Close() = 0;

    /**
     * @brief open connection to the LSP server
     */
    virtual void Open(const LSPStartupInfo& info) = 0;

    /**
     * @brief are we connected to the LSP server?
     */
    virtual bool IsConnected() const = 0;
protected:
    LSPStartupInfo m_startupInfo;
};

#endif // LSPNETWORK_H
