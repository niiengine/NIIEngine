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
#ifndef LSPNETWORKSOCKETCLIENT_H
#define LSPNETWORKSOCKETCLIENT_H

#include "LSPNetwork.h" // Base class: LSPNetwork
#include "SocketAPI/gsgsSocketClientAsync.h"
#include "wx/process.h"
#include "gsgsProcess.h"

class LSPNetworkSocketClient : public LSPNetwork
{
protected:
    gsgs::AsyncSocket::Ptr_t m_socket;
    IProcess* m_lspServer = nullptr;
    int m_pid = wxNOT_FOUND;
    
protected:
    void OnSocketConnected(gsgs::Event& event);
    void OnSocketConnectionLost(gsgs::Event& event);
    void OnSocketConnectionError(gsgs::Event& event);
    void OnSocketError(gsgs::Event& event);
    void OnSocketData(gsgs::Event& event);
    
public:
    LSPNetworkSocketClient();
    virtual ~LSPNetworkSocketClient();

public:
    virtual void Close();
    virtual bool IsConnected() const;
    virtual void Open(const LSPStartupInfo& info);
    virtual void Send(const std::string& data);
};

#endif // LSPNETWORKSOCKETCLIENT_H
