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

#include "NiiNetLinkBase.h"
#include "SQLite3ServerPlugin.h"
#include "SQLite3ClientPlugin.h"
#include "NiiNetSerializer.h"
#include "Nsleep.h"
#include "NiiNetCommon.h"
#include "Kbhit.h"

namespace NII
{
namespace NII_NET
{
    /// A sample derived implementation that will automatically update the table with all connected systems
    class ConnectionStatePlugin : public SQLite3ServerPlugin
    {
    public:
        ConnectionStatePlugin() {lastTimeRemovedDeadRows = 0;}

        // Custom function to create the table we want
        // Assumes the database was already added with AddDBHandle
        bool CreateConnectionStateTable(RakString dbIdentifier)
        {
            // dbHandles is a member variable of SQLite3Plugin and contains the mappings of identifiers to sql database pointers
            unsigned int idx = dbHandles.GetIndexOf(dbIdentifier);
            if (idx==(unsigned int)-1)
                return false;

            // Store the identifier for the connection state table for use in onConnectClose and onConnect
            connectionStateIdentifier=dbIdentifier;

            // Create the table.
            sqlite3_exec(
                // Pointer to sqlite instance previously added with SQLite3Plugin::AddDBHandle()
                dbHandles[idx].dbHandle,
                // Query
                "CREATE TABLE connectionState(address varchar(64),"
                "rowCreationTime timestamp DATE DEFAULT (datetime('now','localtime')),"
                "lastRowUpdateTime timestamp DATE DEFAULT (datetime('now','localtime')),"
                "id varchar(64))",
                // Ignore per-row callback, callback parameter, and error message destination
                0,0,0);

            return true;
        }

        // Implemented event callback from base class Plugin
        virtual void onConnectClose(const Address &address, UniqueID id, ConnectCloseType lostConnectionReason )
        {
            // Call down to the base class in case it does anything in the future (right now it does nothing)
            SQLite3ServerPlugin::onConnectClose(address, id, lostConnectionReason);

            // Get the database index associated with the table used for this class
            unsigned int idx = dbHandles.GetIndexOf(connectionStateIdentifier);
            if (idx==(unsigned int)-1)
                return;

            // Remove dropped system by primary key system address
            String systemAddressString;
            address.read(systemAddressString, true);
            RakString query("DELETE FROM connectionState WHERE address='%s';",
                systemAddressString.SQLEscape().C_String());
            sqlite3_exec(dbHandles[idx].dbHandle,query.C_String(),0,0,0);
        }

        // Implemented event callback from base class Plugin
        virtual void onConnect(const Address &address, UniqueID id)
        {
            // Call down to the base class in case it does anything in the future (right now it does nothing)
            SQLite3ServerPlugin::onConnect(address, id);

            // Get the database index associated with the table used for this class
            unsigned int idx = dbHandles.GetIndexOf(connectionStateIdentifier);
            if (idx==(unsigned int)-1)
                return;

            // Store new system's system address and guid. rowCreationTime column is created automatically
            String systemAddressString;
            address.read(systemAddressString, true);
            String guidString;
            id.read(guidString);
            RakString query(
                "INSERT INTO connectionState (address,id) VALUES ('%s','%s');",
                systemAddressString.SQLEscape().C_String(),
                guidString.SQLEscape().C_String());
            sqlite3_exec(dbHandles[idx].dbHandle,query.C_String(),0,0,0);
        }

        RakString connectionStateIdentifier;
        TimeDurMS lastTimeRemovedDeadRows;
    };

    int main(void)
    {
        printf("Demonstration of SQLite3Plugin.\n");
        printf("Allows you to send SQL queries to a remote system running SQLite3.\n");
        printf("System is a basis from which to add more functionality (security, etc.)\n");
        printf("Difficulty: Intermediate\n\n");

        LinkBase *rakClient=LinkBase::GetInstance();
        LinkBase *rakServer=LinkBase::GetInstance();
        // Client just needs the base class to do sends
        SQLite3ClientPlugin sqlite3ClientPlugin;
        // Server uses our sample derived class to track logins
        ConnectionStatePlugin sqlite3ServerPlugin;
        // Default result handler to print what happens on the client
        SQLite3PluginResultInterface_Printf sqlite3ResultHandler;
        rakClient->AttachPlugin(&sqlite3ClientPlugin);
        rakServer->AttachPlugin(&sqlite3ServerPlugin);
        sqlite3ClientPlugin.AddResultHandler(&sqlite3ResultHandler);

        // Create a database, and tell the plugin about it
        sqlite3 *database;
        // Here :memory: means create the database in memory only.
        // Normally the first parameter refers to a path on the disk to the database file
        if (sqlite3_open_v2(":memory:", &database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0)!=SQLITE_OK)
            return 1;

        static const char* DATABASE_IDENTIFIER="ConnectionStateDBInMemory";
        sqlite3ServerPlugin.AddDBHandle(DATABASE_IDENTIFIER, database);
        sqlite3ServerPlugin.CreateConnectionStateTable(DATABASE_IDENTIFIER);

        // Start and connect RakNet as usual
        HostPeer socketDescriptor(10000,0);
        if (rakServer->Startup(1,&socketDescriptor, 1)!=RAKNET_STARTED)
        {
            printf("Start call failed!\n");
            return 0;
        }
        rakServer->SetMaximumIncomingConnections(1);
        socketDescriptor.port=0;
        rakClient->Startup(1, &socketDescriptor, 1);
        if (rakClient->Connect("127.0.0.1", 10000, 0, 0)!=CONNECTION_ATTEMPT_STARTED)
        {
            printf("Connect call failed\n");
            return 0;
        }

        // Wait for the connection to complete
        Nsleep(500);


        printf("Enter QUIT to quit, anything else is sent as a query.\n");
        while (1)
        {
            if (kbhit())
            {
                printf("Enter query: ");
                char query[512];
                Gets(query,sizeof(query));
                if (stricmp(query, "QUIT")==0)
                {
                    printf("Bye\n");
                    break;
                }
                else
                {
                    // Send a query to the database through RakNet
                    // Result will be printed through SQLite3PluginResultInterface_Printf
                    sqlite3ClientPlugin._sqlite3_exec(DATABASE_IDENTIFIER, query, HIGH_PRIORITY, RELIABLE_ORDERED, 0, rakClient->GetSystemAddressFromIndex(0));

                }
            }

            N_delete rakClient->recevie();
            N_delete rakServer->recevie();

            Nsleep(30);
        }

        rakClient->Shutdown(100,0);
        rakServer->Shutdown(100,0);

        LinkBase::DestroyInstance(rakClient);
        LinkBase::DestroyInstance(rakServer);

        sqlite3_close(database);

        return 0;
    }
}
}