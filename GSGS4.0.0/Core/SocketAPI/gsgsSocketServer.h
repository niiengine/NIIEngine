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

#ifndef gsgsSOCKETSERVER_H
#define gsgsSOCKETSERVER_H

#include "gsgsPreInclude.h"
#include "gsgsSocketBase.h"

namespace gsgs
{
    class _gsgsAPI SocketServer : public SocketBase
    {
    public:
        SocketServer();
        virtual ~SocketServer();

    protected:
        /**
         * @brief On Windows, the 'pipePath' is a string containing the port number.
         * LocalSocket is implemented under Windows with TCP/IP with IP always set to 127.0.0.1
         * @param pipePath
         * @throw gsgs::SocketException
         */
        int CreateServer(const std::string& pipePath);
        /**
         * @throw gsgs::SocketException
         * @return port number
         */
        int CreateServer(const std::string& address, int port);

    public:
        /**
         * @brief Create server using connection string
         * @return port number on success
         * @throw gsgs::SocketException
         */
        int Start(const wxString& connectionString);
        gsgs::SocketBase::Ptr_t WaitForNewConnection(long timeout = -1);
        /**
         * @brief same as above, however, return a pointer to the connection that should be freed by the caller
         */
        gsgs::SocketBase* WaitForNewConnectionRaw(long timeout = -1);
    };
}
#endif // CLSOCKETSERVER_H
