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

#include "Lobby2Server_PGSQL.h"
#include "PostgreSQLInterface.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    class Lobby2ServerJopPrc : public Lobby2JobPrc
    {
    public:
        Lobby2ServerJopPrc(Lobby2Server_PGSQL * server) : Lobby2JobPrc(server), mSql(0){}
        JobResult * handle(Job * job)
        {
            Lobby2ServerCommand & input = static_cast<Lobby2ServerJob *>(job)->mCommand;
            input.returnToSender = input.lobby2Message->ServerDBImpl(&input, mSql);
            if (input.deallocMsgWhenDone)
            {
                OP_DELETE(input.lobby2Message);
                input.lobby2Message = 0;
            }
            if(input.returnToSender == false)
                return 0;
            Lobby2ServerJobResult * re = N_new Lobby2ServerJobResult(&input, job, true);
            return re;
        }

        void onPrcBegin(Job * job)
        {
            Lobby2Server_PGSQL * tServer = static_cast<Lobby2Server_PGSQL *>(mServer);
            tServer->connectionPoolMutex.Lock();
            mSql = tServer->connectionPool.Pop();
            tServer->connectionPoolMutex.Unlock();
        }

        void onPrcEnd(Job * job)
        {
            Lobby2Server_PGSQL * tServer = static_cast<Lobby2Server_PGSQL *>(mServer);
            tServer->connectionPoolMutex.Lock();
            tServer->connectionPool.Insert(mSql);
            tServer->connectionPoolMutex.Unlock();
            mSql = 0;
        }
    protected:
        PostgreSQLInterface * mSql;
    };
    //------------------------------------------------------------------------
    class Lobby2ServerJopPrc1 : public Lobby2JobPrc
    {
    public:
        Lobby2ServerJopPrc1(Lobby2Server_PGSQL * server) : Lobby2JobPrc(server), mSql(0){}
        JobResult * handle(Job * job)
        {
            Lobby2ServerCommand & input = static_cast<Lobby2ServerJob *>(job)->mCommand;

            Lobby2ServerJobResult * re = N_new Lobby2ServerJobResult(&input, job, true);
            return re;
        }
    };
    //------------------------------------------------------------------------
    Lobby2Server_PGSQL::Lobby2Server_PGSQL()
    {
    }
    //------------------------------------------------------------------------
    Lobby2Server_PGSQL::~Lobby2Server_PGSQL()
    {
        ClearConnections();
        Clear();
    }
    //------------------------------------------------------------------------
    void Lobby2Server_PGSQL::AddInputFromThread(Lobby2Message * msg, unsigned int targetUserId, RakString targetUserHandle)
    {
        Lobby2ServerCommand command;
        command.lobby2Message = msg;
        command.deallocMsgWhenDone = true;
        command.returnToSender = true;
        command.callerUserId = targetUserId;
        command.callingUserName = targetUserHandle;
        command.server = this;
        AddInputCommand(command);
    }
    //------------------------------------------------------------------------
    void Lobby2Server_PGSQL::AddInputCommand(Lobby2ServerCommand command)
    {
        Lobby2ServerJob * temp = N_new Lobby2ServerJob(command);
        Lobby2ServerJopPrc * prc = N_new Lobby2ServerJopPrc(this);
        threadPool.add(temp, prc);
    }
    //------------------------------------------------------------------------
    void Lobby2Server_PGSQL::AddOutputFromThread(Lobby2Message * msg, unsigned int targetUserId, RakString targetUserHandle)
    {
        Lobby2ServerCommand command;
        command.lobby2Message = msg;
        command.deallocMsgWhenDone = true;
        command.returnToSender = true;
        command.callerUserId = targetUserId;
        command.callingUserName = targetUserHandle;
        command.server = this;
        msg->resultCode = L2RC_SUCCESS;
        
        Lobby2ServerJob * temp = N_new Lobby2ServerJob(command);
        Lobby2ServerJopPrc1 * prc = N_new Lobby2ServerJopPrc1(this);
        threadPool.add(temp, prc);
    }
    //------------------------------------------------------------------------
    bool Lobby2Server_PGSQL::ConnectToDB(const char * conninfo, int threadcnt)
    {
        if (threadcnt <= 0)
            return false;

        StopThreads();

        PostgreSQLInterface * connection;
        for (int i = 0; i < threadcnt; i++)
        {
            connection = OP_NEW<PostgreSQLInterface>();
            if (connection->Connect(conninfo) == false)
            {
                OP_DELETE(connection);
                ClearConnections();
                return false;
            }
            connectionPoolMutex.Lock();
            connectionPool.Insert(connection);
            connectionPoolMutex.Unlock();
        }

        threadPool.startup(threadcnt);
        return true;
    }
    //------------------------------------------------------------------------
    void Lobby2Server_PGSQL::ClearConnections()
    {
        connectionPoolMutex.Lock();
        for (NCount i = 0; i < connectionPool.Size(); ++i)
            OP_DELETE(connectionPool[i]);
        connectionPool.Clear(false);
        connectionPoolMutex.Unlock();arConnections()
    }
    //------------------------------------------------------------------------
}
}