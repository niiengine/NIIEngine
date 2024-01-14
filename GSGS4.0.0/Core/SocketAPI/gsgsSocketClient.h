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

#ifndef gsgsSOCKETCLIENT_H
#define gsgsSOCKETCLIENT_H

#include "gsgsSocketBase.h"
#include <wx/string.h>

namespace gsgs
{
    class _gsgsAPI SocketClient : public SocketBase
    {
        wxString m_path;

    public:
        SocketClient();
        virtual ~SocketClient();

        /**
         * @brief connect to a remote socket, using unix-domain socket
         */
        bool ConnectLocal(const wxString& socketPath);
        /**
         * @brief connect to a remote server using ip/port
         * when using non-blocking mode, wouldBlock will be set to true
         * incase the connect fails
         */
        bool ConnectRemote(const wxString& address, int port, bool& wouldBlock, bool nonBlockingMode = false);

        /**
         * @brief connect using connection string
         * @param connectionString in the format of unix:///path/to/unix/socket or tcp://127.0.0.1:1234
         * @return
         */
        bool Connect(const wxString& connectionString, bool nonBlockingMode = false);

        /**
         * @brief connect using non-blocking method
         */
        bool ConnectNonBlocking(const wxString& connectionString, bool& wouldBlock);
    };
}
#endif // CLSOCKETCLIENT_H
