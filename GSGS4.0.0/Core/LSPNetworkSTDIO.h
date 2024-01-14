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
#ifndef LSPNETWORKSOCKET_H
#define LSPNETWORKSOCKET_H

#include "LSPNetwork.h"
#include "gsgsEvent.h"
#include "SocketAPI/gsgsSocketClientAsync.h"
#include <wx/process.h>

///===------------------------------
/// LSPNetwork based on sockets
///===------------------------------

namespace gsgs 
{
    class SubPrc;
}
class LSPNetworkSTDIO : public LSPNetwork
{
protected:
    gsgs::AsyncSocket::Ptr_t m_socket;
    gsgs::SubPrc* m_server = nullptr;

protected:
    void OnProcessTerminated(gsgs::ProcessEvent& event);
    void OnProcessOutput(gsgs::ProcessEvent& event);
    void OnProcessStderr(gsgs::ProcessEvent& event);
    
public:
    virtual void Close();
    virtual void Open(const LSPStartupInfo& info);
    virtual void Send(const std::string& data);
    virtual bool IsConnected() const;

    LSPNetworkSTDIO();
    virtual ~LSPNetworkSTDIO();
};

#endif // LSPNETWORKSOCKET_H
