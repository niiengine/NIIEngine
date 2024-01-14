#include "gsgsConnectionString.h"
#include "gsgsLogManager.h"

namespace gsgs
{
    ConnectionString::ConnectionString(const wxString& connectionString) : 
        m_port(wxNOT_FOUND), 
        m_isOK(false)
    {
        DoParse(connectionString);
    }

    ConnectionString::~ConnectionString() {}

    void ConnectionString::DoParse(const wxString& connectionString)
    {
        m_isOK = false; // default
        // get the protocol part
        gsgsDEBUG() << "Parsing connection string:" << connectionString << gsgs::clEndl;
        wxString protocol = connectionString.BeforeFirst(':');
        if(protocol == "tcp") 
        {
            m_protocol = kTcp;
        } 
        else if(protocol == "unix") 
        {
    #ifdef __WXMSW__
            gsgsWARNING() << "unix protocol is not suppported on Windows" << gsgs::clEndl;
            return;
    #endif
            m_protocol = kUnixLocalSocket;
        } 
        else 
        {
            gsgsWARNING() << "Invalid protocol in connection string:" << connectionString << gsgs::clEndl;
            return;
        }

        wxString address = connectionString.AfterFirst(':');
        address = address.Mid(2); // Skip the "//"
        if(m_protocol == kUnixLocalSocket) {
            // The rest is the file path
            m_path = address;
            m_isOK = !m_path.IsEmpty();
        } else {
            // we now expect host[:port]
            m_host = address.BeforeFirst(':');
            wxString port = address.AfterFirst(':');
            if(!port.IsEmpty()) {
                port.ToCLong(&m_port);
            }
            m_isOK = !m_host.IsEmpty() && (m_port != wxNOT_FOUND);
        }
    }
}