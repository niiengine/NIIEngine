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
#ifndef __LOBBY_2_SERVER_PGSQL_H
#define __LOBBY_2_SERVER_PGSQL_H

#include "Lobby2Server.h"

class PostgreSQLInterface;

namespace NII
{
namespace NII_NET
{
    /// PostgreSQL specific functionality to the lobby server
    class RAK_DLL_EXPORT Lobby2Server_PGSQL : public Lobby2Server
    {
    public:	
        Lobby2Server_PGSQL();
        virtual ~Lobby2Server_PGSQL();

        STATIC_FACTORY_DECLARATIONS(Lobby2Server_PGSQL)
        
        /// ConnectTo to the database \a threadcnt times using the connection string
        /// \param[in] conninfo See the postgre docs
        /// \return True on success, false on failure.
        virtual bool ConnectToDB(const char * conninfo, int threadcnt);

        /// Add input to the worker threads, from a thread already running
        virtual void AddInputFromThread(Lobby2Message * msg, unsigned int targetUserId, RakString targetUserHandle);
        /// Add output from the worker threads, from a thread already running. This is in addition to the current message, so is used for notifications
        virtual void AddOutputFromThread(Lobby2Message * msg, unsigned int targetUserId, RakString targetUserHandle);
    protected:
        virtual void AddInputCommand(Lobby2ServerCommand command);

        virtual void ClearConnections(void);
        DataStructures::List<PostgreSQLInterface *> connectionPool;
    };
}
}
#endif