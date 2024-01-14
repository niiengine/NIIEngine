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


#ifndef ___SQLITE_3_SERVER_PLUGIN_H
#define ___SQLITE_3_SERVER_PLUGIN_H

/// \brief Control if SQLite3 statements execute in a thread
/// \details sqlite3_exec is blocking and will therefore block other operations in the same program<BR>
/// If defined, sqlite3_exec executes in a thread so that doesn't happen<BR>
/// If the only thing this system is doing is running SQLite, then you'll get marginally better performance by commenting it out.
/// \ingroup SQL_LITE_3_PLUGIN
#define SQLite3_STATEMENT_EXECUTE_THREADED

#include "NiiNetCommon.h"
#include "Export.h"
#include "NiiNetPlugin.h"
#include "PacketLevel.h"
#include "SocketIncludes.h"
#include "DS_Multilist.h"
#include "RakString.h"
#include "sqlite3.h"
#include "SQLite3PluginCommon.h"

#ifdef SQLite3_STATEMENT_EXECUTE_THREADED
#include "NiiThreadManager.h"
#endif

namespace NII
{
namespace NII_NET
{
    class LinkBase;
    /// \brief Exec SQLLite commands over the network
    /// \details SQLite version 3 supports remote calls via networked file handles, but not over the regular internet<BR>
    /// This plugin will serialize calls to and results from sqlite3_exec<BR>
    /// That's all it does - any remote system can execute SQL queries.<BR>
    /// Intended as a starting platform to derive from for more advanced functionality (security over who can query, etc).<BR>
    /// Compatible as a plugin with both LinkBase and ClusterNetworkObj
    /// \ingroup SQL_LITE_3_PLUGIN
    class RAK_DLL_EXPORT SQLite3ServerPlugin : public Plugin
    {
    public:
        SQLite3ServerPlugin();
        virtual ~SQLite3ServerPlugin();

        /// Associate identifier with dbHandle, so when we get calls to operate on identifier, we use dbhandle
        /// If SQLite3_STATEMENT_EXECUTE_THREADED is defined, will start the execution thread the first time a dbHandle is added.
        /// \return true on success, false on dbIdentifier empty, or already in use
        virtual bool AddDBHandle(RakString dbIdentifier, sqlite3 *dbHandle, bool dbAutoCreated=false);

        /// Stop using a dbHandle, lookup either by identifier or by pointer.
        /// If SQLite3_STATEMENT_EXECUTE_THREADED is defined, do not call this while processing commands, since the commands run in a thread and might be using the dbHandle
        /// Call before closing the handle or else SQLite3Plugin won't know that it was closed, and will continue using it
        void RemoveDBHandle(RakString dbIdentifier, bool alsoCloseConnection=false);
        void RemoveDBHandle(sqlite3 *dbHandle, bool alsoCloseConnection=false);

        /// \internal For plugin handling
        virtual ConnectReceiveType onMessage(SocketMessage *msg);
        virtual void onAttach();
        virtual void onDetach();

        /// \internal
        struct NamedDBHandle
        {
            RakString dbIdentifier;
            sqlite3 *dbHandle;
            bool dbAutoCreated;
            TimeDurMS whenCreated;
        };

    #ifdef SQLite3_STATEMENT_EXECUTE_THREADED
        //virtual void update(void);
        /// \internal
        /*struct SQLExecThreadInput
        {
            SQLExecThreadInput() {data=0; mMsg=0;}
            char *data;
            unsigned int length;
            Address * sender;
            TimeDurMS whenMessageArrived;
            sqlite3 *dbHandle;
            SocketMessage *mMsg;
        };
        */
        /// \internal
        /*struct SQLExecThreadOutput
        {
            SQLExecThreadOutput() {data=0; mMsg=0;}
            char *data;
            unsigned int length;
            Address * sender;
            SocketMessage *mMsg;
        };*/
    #endif // SQLite3_STATEMENT_EXECUTE_THREADED

    protected:
        virtual void StopThreads(void);

        // List of databases added with AddDBHandle()
        DataStructures::Multilist<ML_ORDERED_LIST, NamedDBHandle, RakString> dbHandles;

    #ifdef SQLite3_STATEMENT_EXECUTE_THREADED
        // The point of the sqlThreadPool is so that SQL queries, which are blocking, happen in the thread and don't slow down the rest of the application
        // The sqlThreadPool has a queue for incoming processing requests.  As systems disconnect their pending requests are removed from the list.
        ThreadManager<SQLExecThreadInput, SQLExecThreadOutput> sqlThreadPool;
    #endif
    }

    extern bool operator<(const DataStructures::MLKeyRef<RakString> &inputKey,
        const SQLite3ServerPlugin::NamedDBHandle & cls);
    extern bool operator>(const DataStructures::MLKeyRef<RakString> &inputKey,
        const SQLite3ServerPlugin::NamedDBHandle & cls);
    extern bool operator==(const DataStructures::MLKeyRef<RakString> &inputKey,
        const SQLite3ServerPlugin::NamedDBHandle & cls);
}
}
#endif
