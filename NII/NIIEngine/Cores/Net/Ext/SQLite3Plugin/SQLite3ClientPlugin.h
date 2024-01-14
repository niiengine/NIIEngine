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


#ifndef ___SQLITE_3_CLIENT_PLUGIN_H
#define ___SQLITE_3_CLIENT_PLUGIN_H

#include "NiiNetCommon.h"
#include "Export.h"
#include "NiiNetPlugin.h"
#include "PacketLevel.h"
#include "SocketIncludes.h"
#include "DS_Multilist.h"
#include "RakString.h"
#include "SQLite3PluginCommon.h"

namespace NII
{
namespace NII_NET
{
    /// \brief Handles results of calls to SQLite3Plugin::_sqlite3_exec()
    /// Results from calling SQLite3Plugin::_sqlite3_exec() are handled in this callback.
    /// You should implement the callback, and let the plugin know about it via SQLite3Plugin::AddResultHandler()
    /// Be sure to call SQLite3Plugin::RemoveResultHandler() or SQLite3Plugin::ClearResultHandlers() if you delete the callback
    /// \ingroup SQL_LITE_3_PLUGIN
    class SQLite3PluginResultInterface
    {
    public:

        /// Query executed, possibly returning data or an error message
        ///
        /// \param[out] inputStatement Passed to SQLite3Plugin::_sqlite3_exec
        /// \param[out] queryId Returned from SQLite3Plugin::_sqlite3_exec
        /// \param[out] dbIdentifier Passed to SQLite3Plugin::_sqlite3_exec
        /// \param[out] table Result of call to _sqlite3_exec, should that statement return a result
        /// \param[out] errorMsg If _sqlite3_exec failed, then the error message is here, and table will be empty
        /// \ingroup SQL_LITE_3_PLUGIN
        virtual void _sqlite3_exec(
            RakString inputStatement,
            unsigned int queryId,
            RakString dbIdentifier,
            const SQLite3Table &table,
            RakString errorMsg)=0;

        /// dbIdentifier is unknown on the remote system
        ///
        /// \param[out] inputStatement Passed to SQLite3Plugin::_sqlite3_exec
        /// \param[out] queryId Returned from SQLite3Plugin::_sqlite3_exec
        /// \param[out] dbIdentifier Passed to SQLite3Plugin::_sqlite3_exec
        /// \ingroup SQL_LITE_3_PLUGIN
        virtual void OnUnknownDBIdentifier(
            RakString inputStatement,
            unsigned int queryId,
            RakString dbIdentifier)=0;
    };

    /// Sample callback implementation that just prints to the screen the results
    /// \ingroup SQL_LITE_3_PLUGIN
    class SQLite3PluginResultInterface_Printf : public SQLite3PluginResultInterface
    {
        virtual void _sqlite3_exec(
            RakString inputStatement,
            unsigned int queryId,
            RakString dbIdentifier,
            const SQLite3Table &table,
            RakString errorMsg);

        virtual void OnUnknownDBIdentifier(
            RakString inputStatement,
            unsigned int queryId,
            RakString dbIdentifier);
    };

    /// SQLite version 3 supports remote calls via networked file handles, but not over the regular internet
    /// This plugin will serialize calls to and results from sqlite3_exec
    /// That's all it does - any remote system can execute SQL queries.
    /// Intended as a starting platform to derive from for more advanced functionality (security over who can query, etc).
    /// Compatible as a plugin with both LinkBase and ClusterNetworkObj
    /// \ingroup SQL_LITE_3_PLUGIN
    class RAK_DLL_EXPORT SQLite3ClientPlugin : public Plugin
    {
    public:
        SQLite3ClientPlugin();
        virtual ~SQLite3ClientPlugin();

        /// Add an interface to get callbacks for results
        /// Up to user to make sure the pointer is valid through the lifetime of use
        void AddResultHandler(SQLite3PluginResultInterface *res);
        void RemoveResultHandler(SQLite3PluginResultInterface *res);
        void ClearResultHandlers(void);

        /// Execute a statement on the remote system
        /// \note Don't forget to escape your input strings. RakString::SQLEscape() is available for this.
        /// \param[in] dbIdentifier Which database to use, added with AddDBHandle()
        /// \param[in] inputStatement SQL statement to execute
        /// \param[in] plevel See LinkBase::send()
        /// \param[in] ptype See LinkBase::send()
        /// \param[in] pchannel See LinkBase::send()
        /// \param[in] address See LinkBase::send()
        /// \return Query ID. Will be returned in _sqlite3_exec
        unsigned int _sqlite3_exec(RakString dbIdentifier, RakString inputStatement,
            PacketLevel plevel, PacketType ptype, char pchannel, const Address & address);

        /// \internal For plugin handling
        virtual ConnectReceiveType onMessage(SocketMessage * msg);

        // List of result handlers added with AddResultHandler()
        DataStructures::List<SQLite3PluginResultInterface *> resultHandlers;
        // Each query returns a numeric id if you want it. This tracks what id to assign next. Increments sequentially.
        unsigned int nextQueryId;
    };
}
}
#endif