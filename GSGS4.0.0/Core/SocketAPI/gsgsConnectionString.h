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
#ifndef gsgsCONNECTIONSTRING_H
#define gsgsCONNECTIONSTRING_H

#include <gsgsPreInclude.h>

namespace gsgs
{
    class _gsgsAPI ConnectionString
    {
    public:
        enum eProtocol 
        {
            kTcp,
            kUnixLocalSocket,
        };
    public:
        ConnectionString(const wxString& connectionString);
        ~ConnectionString();

        void SetHost(const wxString& host)              { this->m_host = host; }
        void SetIsOK(bool isOK)                         { this->m_isOK = isOK; }
        void SetPath(const wxString& path)              { this->m_path = path; }
        void SetPort(long port)                         { this->m_port = port; }
        void SetProtocol(const eProtocol& protocol)     { this->m_protocol = protocol; }
        const wxString& GetHost() const                 { return m_host; }
        bool IsOK() const                               { return m_isOK; }
        const wxString& GetPath() const                 { return m_path; }
        long GetPort() const                            { return m_port; }
        const eProtocol& GetProtocol() const            { return m_protocol; }
    protected:
        void DoParse(const wxString& connectionString);
    protected:
        eProtocol m_protocol;
        wxString m_host;
        long m_port;
        wxString m_path;
        bool m_isOK;
    };
}
#endif