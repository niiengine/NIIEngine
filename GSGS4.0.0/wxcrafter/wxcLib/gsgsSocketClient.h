#ifndef gsgsSOCKETCLIENT_H
#define gsgsSOCKETCLIENT_H

#include "wxcLib/gsgsSocketBase.h"
#include <wx/string.h>

class SocketClient : public clSocketBase1
{
    wxString m_path;

public:
    SocketClient();
    virtual ~SocketClient();

    /**
     * @brief connect to a remote socket, using unix-domain socket
     */
    bool ConnectLocal(const wxString &socketPath);
    /**
     * @brief connect to a remote server using ip/port
     */
    bool ConnectRemote(const wxString &address, int port);
    /**
     * @brief for wx2.8 ...
     */
    bool ConnectRemote(const std::string &address, int port);
};

#endif // CLSOCKETCLIENT_H
