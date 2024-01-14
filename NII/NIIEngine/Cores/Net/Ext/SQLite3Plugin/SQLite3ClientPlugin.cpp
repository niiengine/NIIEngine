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
#include "SQLite3ClientPlugin.h"
#include "NiiNetCommon.h"
#include "NiiNetSerializer.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    void SQLite3PluginResultInterface_Printf::_sqlite3_exec(
        RakString inputStatement,
        unsigned int queryId,
        RakString dbIdentifier,
        const SQLite3Table &table,
        RakString errorMsg)
    {
        if (errorMsg.IsEmpty()==false)
        {
            printf("Error for query: %s\n", inputStatement.C_String());
            printf("%s\n", errorMsg.C_String());
            return;
        }

        unsigned int idx;
        for (idx=0; idx < table.columnNames.Size(); idx++)
            printf("%s ", table.columnNames[idx].C_String());
        printf("\n");
        if (table.rows.Size()==0)
        {
            printf("<NO ROWS>\n");
        }
        else
        {
            for (idx=0; idx < table.rows.Size(); idx++)
            {
                unsigned int idx2;
                for (idx2=0; idx2 < table.rows[idx]->entries.Size(); idx2++)
                    printf("%s ", table.rows[idx]->entries[idx2].C_String());
                printf("\n");
            }
        }
    }
    //------------------------------------------------------------------------
    void SQLite3PluginResultInterface_Printf::OnUnknownDBIdentifier(
        RakString inputStatement,
        unsigned int queryId,
        RakString dbIdentifier)
    {
        printf("Unknown DB %s\n", dbIdentifier.C_String());
    }
    //------------------------------------------------------------------------
    SQLite3ClientPlugin::SQLite3ClientPlugin()
    {
        nextQueryId=0;
    }
    //------------------------------------------------------------------------
    SQLite3ClientPlugin::~SQLite3ClientPlugin()
    {
    }
    //------------------------------------------------------------------------
    void SQLite3ClientPlugin::AddResultHandler(SQLite3PluginResultInterface *res)
    {
        resultHandlers.Push(res);
    }
    //------------------------------------------------------------------------
    void SQLite3ClientPlugin::RemoveResultHandler(SQLite3PluginResultInterface *res)
    {
        unsigned int idx = resultHandlers.GetIndexOf(res);
        if (idx!=-1)
            resultHandlers.RemoveAtIndex(idx);
    }
    //------------------------------------------------------------------------
    void SQLite3ClientPlugin::ClearResultHandlers(void)
    {
        resultHandlers.Clear(true);
    }
    //------------------------------------------------------------------------
    unsigned int SQLite3ClientPlugin::_sqlite3_exec(RakString dbIdentifier, RakString inputStatement, 
        PacketLevel plevel,PacketType ptype, char pchannel, const Address & address)
    {
        NetSerializer bsOut;
        bsOut.write((Token)ID_SQLite3_EXEC);
        bsOut.write(nextQueryId);
        bsOut.write(dbIdentifier);
        bsOut.write(inputStatement);
        bsOut.write1();
        send(&bsOut, plevel,ptype,pchannel,address,false);
        ++nextQueryId;
        return nextQueryId-1;
    }
    //------------------------------------------------------------------------
    ConnectReceiveType SQLite3ClientPlugin::onMessage(SocketMessage * msg)
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
                    N_assert(0);
                }
                else
                {
                    // Client code
                    RakString errorMsgStr;
                    SQLite3Table inputTable;

                    // read it
                    bsIn.read(errorMsgStr);
                    inputTable.write(&bsIn);

                    unsigned int idx;
                    for (idx=0; idx < resultHandlers.Size(); idx++)
                        resultHandlers[idx]->_sqlite3_exec(inputStatement, queryId, dbIdentifier, inputTable,errorMsgStr);
                }

                return CRT_Destroy;
            }
            break;
        case ID_SQLite3_UNKNOWN_DB:
            {
                unsigned int queryId;
                RakString dbIdentifier;
                RakString inputStatement;
                NetSerializer bsIn(msg->data, msg->length, false);
                bsIn.skipRead(1, 0);
                bsIn.read(queryId);
                bsIn.read(dbIdentifier);
                bsIn.read(inputStatement);
                unsigned int idx;
                for (idx=0; idx < resultHandlers.Size(); idx++)
                    resultHandlers[idx]->OnUnknownDBIdentifier(inputStatement, queryId, dbIdentifier);
            }
        }

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
}
}