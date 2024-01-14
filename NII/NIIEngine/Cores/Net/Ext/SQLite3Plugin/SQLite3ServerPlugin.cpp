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
#include "SQLite3ServerPlugin.h"
#include "NiiNetCommon.h"
#include "NiiNetSerializer.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    bool operator<(const DataStructures::MLKeyRef<RakString> & inputKey,
        const SQLite3ServerPlugin::NamedDBHandle & cls)
    {
        return inputKey.Get() < cls.dbIdentifier;
    }
    bool operator>(const DataStructures::MLKeyRef<RakString> & inputKey,
        const SQLite3ServerPlugin::NamedDBHandle & cls)
    {
        return inputKey.Get() > cls.dbIdentifier;
    }
    bool operator==(const DataStructures::MLKeyRef<RakString> & inputKey,
        const SQLite3ServerPlugin::NamedDBHandle & cls)
    {
        return inputKey.Get() == cls.dbIdentifier;
    }
    //------------------------------------------------------------------------
    int PerRowCallback(void *userArgument, int argc, char **argv, char **azColName)
    {
        SQLite3Table *outputTable = (SQLite3Table*)userArgument;
        unsigned int idx;
        if (outputTable->columnNames.Size()==0)
        {
            for (idx=0; idx < (unsigned int) argc; idx++)
                outputTable->columnNames.Push(azColName[idx]);
        }
        SQLite3Row *row = OP_NEW<SQLite3Row>();
        outputTable->rows.Push(row);
        for (idx=0; idx < (unsigned int) argc; idx++)
        {
            if (argv[idx])
                row->entries.Push(argv[idx]);
            else
                row->entries.Push("");
        }
        return 0;
    }
    //------------------------------------------------------------------------
    SQLite3ServerPlugin::SQLite3ServerPlugin()
    {
    }
    //------------------------------------------------------------------------
    SQLite3ServerPlugin::~SQLite3ServerPlugin()
    {
        StopThreads();
    }
    //------------------------------------------------------------------------
    bool SQLite3ServerPlugin::AddDBHandle(RakString dbIdentifier, sqlite3 * dbHandle, bool dbAutoCreated)
    {
        if (dbIdentifier.IsEmpty())
            return false;
        NCount idx = dbHandles.GetInsertionIndex(dbIdentifier);
        if (idx == (unsigned int)-1)
            return false;
        NamedDBHandle ndbh;
        ndbh.dbHandle = dbHandle;
        ndbh.dbIdentifier = dbIdentifier;
        ndbh.dbAutoCreated = dbAutoCreated;
        ndbh.whenCreated = N_Engine().getTimer().getMS();
        dbHandles.InsertAtIndex(ndbh, idx);

    #ifdef SQLite3_STATEMENT_EXECUTE_THREADED
        if (sqlThreadPool.isRun() == false)
            sqlThreadPool.startup(1);
    #endif

        return true;
    }
    //------------------------------------------------------------------------
    void SQLite3ServerPlugin::RemoveDBHandle(RakString dbIdentifier, bool alsoCloseConnection)
    {
        NCount idx = dbHandles.GetIndexOf(dbIdentifier);
        if (idx!=(unsigned int)-1)
        {
            if (alsoCloseConnection)
            {
                printf("Closed %s\n", dbIdentifier.C_String());
                sqlite3_close(dbHandles[idx].dbHandle);
            }
            dbHandles.RemoveAtIndex(idx);
    #ifdef SQLite3_STATEMENT_EXECUTE_THREADED
        if (dbHandles.GetSize() == 0)
            StopThreads();
    #endif // SQLite3_STATEMENT_EXECUTE_THREADED
        }
    }
    //------------------------------------------------------------------------
    void SQLite3ServerPlugin::RemoveDBHandle(sqlite3 * dbHandle, bool alsoCloseConnection)
    {
        NCount idx;
        for (idx=0; idx < dbHandles.GetSize(); idx++)
        {
            if (dbHandles[idx].dbHandle == dbHandle)
            {
                if (alsoCloseConnection)
                {
                    printf("Closed %s\n", dbHandles[idx].dbIdentifier.C_String());
                    sqlite3_close(dbHandles[idx].dbHandle);
                }
                dbHandles.RemoveAtIndex(idx);
    #ifdef SQLite3_STATEMENT_EXECUTE_THREADED
                if (dbHandles.GetSize() == 0)
                    StopThreads();
    #endif // SQLite3_STATEMENT_EXECUTE_THREADED
                return;
            }
        }
    }
    //------------------------------------------------------------------------
    #ifdef SQLite3_STATEMENT_EXECUTE_THREADED
    class SQLExecJob : public Job
    {
    public:
        SQLExecJob() : Job(0)
        {
            data=0; 
            mMsg=0;
        }
        ~SQLExecJob()
        {
            OP_DELETE(data)
        }
        char *data;
        unsigned int length;
        Address * sender;
        TimeDurMS whenMessageArrived;
        sqlite3 *dbHandle;
        SocketMessage *mMsg;
    };
    
    class ExecStatementJobPrc : public JobPrc
    {
    public:
        ExecStatementJobPrc() : JobPrc(false)
        {
        }
        
        JobResult * handle(Job * jop);
        
        void handle(JobResult * result);
    };
    
    class SQLExecJobResult : public JobResult
    {
    public:
        SQLExecJobResult() : JobResult(true)
        {
            data=0; 
            mMsg=0;
        }
        ~SQLExecJobResult()
        {
            OP_DELETE(data);
        }
        char * data;
        unsigned int length;
        Address * sender;
        SocketMessage *mMsg;
    };
    
    void ExecStatementJobPrc::handle(JobResult * result)
    {
        SQLExecJobResult * output = static_cast<SQLExecJobResult *>(result);

        NetSerializer bsOut((unsigned char *)output->data, output->length, false);
        send(&bsOut, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, output->sender, false);
        rakFree_Ex(output->data);
    }
    //------------------------------------------------------------------------
    JobResult * ExecStatementJobPrc::handle(Job * jop)
    {
        SQLExecJob * threadInput = static_cast<SQLExecJob *>(jop);
        unsigned int queryId;
        RakString dbIdentifier;
        RakString inputStatement;
        NetSerializer bsIn((unsigned char *)threadInput->data, threadInput->length, false);
        bsIn.skipRead(1, 0);
        bsIn.read(queryId);
        bsIn.read(dbIdentifier);
        bsIn.read(inputStatement);
        bsIn.skipRead(0, 1);
        char * errorMsg;
        RakString errorMsgStr;
        SQLite3Table outputTable;
        sqlite3_exec(threadInput->dbHandle, inputStatement.C_String(), PerRowCallback, &outputTable, &errorMsg);

        if (errorMsg)
        {
            errorMsgStr = errorMsg;
            sqlite3_free(errorMsg);
        }

        NetSerializer bsOut;
        bsOut.write((Token)ID_SQLite3_EXEC);
        bsOut.write(queryId);
        bsOut.write(dbIdentifier);
        bsOut.write(inputStatement);
        bsOut.write0();
        bsOut.write(errorMsgStr);
        outputTable.read(&bsOut);

        // Free input data
        rakFree_Ex(threadInput->data);

        // Copy to output data
        SQLExecJobResult * threadOutput = N_new SQLExecJobResult();
        threadOutput->data = (char*) rakMalloc_Ex(bsOut.getSize());
        memcpy(threadOutput->data,bsOut.getData(),bsOut.getSize());
        threadOutput->length = bsOut.getSize();
        threadOutput->sender = threadInput->sender;

        //*returnOutput = true;
        return threadOutput;
    }
    #endif // SQLite3_STATEMENT_EXECUTE_THREADED
    //------------------------------------------------------------------------
    ConnectReceiveType SQLite3ServerPlugin::onMessage(SocketMessage * msg)
    {
        switch (msg->data[0])
        {
        case ID_SQLite3_EXEC:
            {
                unsigned int queryId;
                RakString dbIdentifier;
                RakString inputStatement;
                NetSerializer bsIn(msg->data, msg->length, false);
                bsIn.skipRead(1, 0);
                bsIn.read(queryId);
                bsIn.read(dbIdentifier);
                bsIn.read(inputStatement);
                bool isRequest;
                bsIn.read(isRequest);
                if (isRequest)
                {
                    // Server code
                    unsigned int idx = dbHandles.GetIndexOf(dbIdentifier);
                    if (idx==-1)
                    {
                        NetSerializer bsOut;
                        bsOut.write((Token)ID_SQLite3_UNKNOWN_DB);
                        bsOut.write(queryId);
                        bsOut.write(dbIdentifier);
                        bsOut.write(inputStatement);
                        send(&bsOut, MEDIUM_PRIORITY,RELIABLE_ORDERED,0,msg->mAddress,false);
                    }
                    else
                    {
    #ifdef SQLite3_STATEMENT_EXECUTE_THREADED
                        // Push to the thread
                        SQLExecJob * input = N_new SQLExecJob();
                        ExecStatementJobPrc * prc = N_new ExecStatementJobPrc();
                        input->data = (char*) rakMalloc_Ex(msg->length);
                        memcpy(input->data,msg->data,msg->length);
                        input->dbHandle = dbHandles[idx].dbHandle;
                        input->length = msg->length;
                        input->sender = msg->mAddress;
                        sqlThreadPool.add(input, prc);
    #else
                        char *errorMsg;
                        RakString errorMsgStr;
                        SQLite3Table outputTable;
                        sqlite3_exec(dbHandles[idx].dbHandle, inputStatement.C_String(), PerRowCallback, &outputTable, &errorMsg);
                        if (errorMsg)
                        {
                            errorMsgStr=errorMsg;
                            sqlite3_free(errorMsg);
                        }
                        NetSerializer bsOut;
                        bsOut.write((Token)ID_SQLite3_EXEC);
                        bsOut.write(queryId);
                        bsOut.write(dbIdentifier);
                        bsOut.write(inputStatement);
                        bsOut.write0();
                        bsOut.write(errorMsgStr);
                        outputTable.read(&bsOut);
                        send(&bsOut, MEDIUM_PRIORITY,RELIABLE_ORDERED,0,msg->mAddress,false);
    #endif
                    }
                }
                return CRT_Destroy;
            }
            break;
        }
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void SQLite3ServerPlugin::onAttach()
    {
    }
    //------------------------------------------------------------------------
    void SQLite3ServerPlugin::onDetach()
    {
        StopThreads();
    }
    //------------------------------------------------------------------------
    void SQLite3ServerPlugin::StopThreads()
    {
    #ifdef SQLite3_STATEMENT_EXECUTE_THREADED
        sqlThreadPool.shutdown();
        sqlThreadPool.removeAllInput();
        sqlThreadPool.removeAllOutput();
    #endif
    }
    //------------------------------------------------------------------------
}
}