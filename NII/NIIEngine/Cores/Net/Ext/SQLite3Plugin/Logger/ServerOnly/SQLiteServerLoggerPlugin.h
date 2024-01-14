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


#ifndef ___SQLITE_SERVER_LOGGER_PLUGIN_H
#define ___SQLITE_SERVER_LOGGER_PLUGIN_H

#include "SQLite3ServerPlugin.h"
#include "SQLiteLoggerCommon.h"

class LinkBase;

#define MAX_PACKETS_PER_CPU_INPUT_THREAD 16

namespace NII
{
namespace NII_NET
{
	/// \brief Extends SQLite3ServerPlugin to support logging functions, including compressing images.
	/// \details SQLiteClientLoggerPlugin has the ability to send logs. Logs contain a description of the name of the table, the name of the columns, the types, and the data.<BR>
	/// This class will create tables as necessary to support the client inputs.<BR>
	/// Also, if images are sent, the server will format them from uncompressed to compressed JPG using jpeg-7 in a thread.<BR>
	/// Compatible as a plugin with both LinkBase and ClusterNetworkObj
	/// \ingroup SQL_LITE_3_PLUGIN
	class RAK_DLL_EXPORT SQLiteServerLoggerPlugin : public SQLite3ServerPlugin
	{
	public:
		SQLiteServerLoggerPlugin();
		virtual ~SQLiteServerLoggerPlugin();

		/// Used with SetSessionManagementMode()
		enum SessionManagementMode
		{
			/// \brief USE_ANY_DB_HANDLE is for games where the clients are not allowed to create new databases, and the clients don't care which database is written to. Typically only used if there is only one database ever.
			/// \details Ignore SQLiteClientLoggerPlugin::StartSession(), and rely on a prior call to SQLite3ServerPlugin::AddDBHandle().
			/// If no handles exist, logging calls silently fail.
			USE_ANY_DB_HANDLE,

			/// \brief USE_NAMED_DB_HANDLE is for games where the clients are not allowed to create new databases. Instead, they use named databases precreated.
			/// \details Interpret the sessionId parameter passed to SQLiteClientLoggerPlugin::StartSession() as the dbIdentifier parameter passed to SQLite3ServerPlugin::AddDBHandle().
			/// Use this database if it exists. If not, logging calls silently fail.
			USE_NAMED_DB_HANDLE,

			/// \brief CREATE_EACH_NAMED_DB_HANDLE is for single player games or multiplayer games where every game has a unique sesionId.
			/// \details Use the sessionId parameter passed to SQLiteClientLoggerPlugin::StartSession() as a dbIdentifier.
			/// A new database is created for each user
			CREATE_EACH_NAMED_DB_HANDLE,

			/// \brief CREATE_SHARED_NAMED_DB_HANDLE is for multiplayer games where you don't want to have to transmit and synchronize a unique sessionId. Everyone is in the same sessionId.
			/// \details Use the sessionId parameter passed to SQLiteClientLoggerPlugin::StartSession() as a dbIdentifier.
			/// A new database is created only if the sessionId is different. Two users using the same sessionId will write to the same database
			CREATE_SHARED_NAMED_DB_HANDLE,
		};

		/// \brief Determine if and how to automatically create databases, rather than call SQLite3ServerPlugin::AddDBHandle()
		/// \details Typically you want one database to hold the events of a single application session, rather than one database for all sessions over all time.
		/// A user of SQLiteClientLoggerPlugin can optionally call SQLiteClientLoggerPlugin::StartSession() in order to join a session to enable this.
		/// Call this before calling AddDBHandle(), and before any clients connect
		/// \param[in] _sessionManagementMode See SessionManagementMode. Default is CREATE_EACH_NAMED_DB_HANDLE.
		/// \param[in] _createDirectoryForFile If true, uses the current server date as a directory to the filename. This ensures filenames do not overwrite each other, even if the sessionId is reused. Defaults to true.
		/// \param[in] _newDatabaseFilePath For CREATE_EACH_NAMED_DB_HANDLE and CREATE_SHARED_NAMED_DB_HANDLE, will create the databases here. Not used for the other modes. Defaults to whatever the operating system picks (usually the application directory).
		void SetSessionManagementMode(SessionManagementMode _sessionManagementMode, bool _createDirectoryForFile, const char *_newDatabaseFilePath);

		/// Close all connected sessions, writing all databases immediately
		void CloseAllSessions(void);

		/// \brief Enable using realtime shader based DXT compression on an Nvidia based video card. This will activate OpenGL
		/// \details Call this before anyone connects.
		/// If not enabled, or initialization fails, then jpg compression is used instead on the CPU.
		/// Defaults to false, in case the target system does not support OpenGL
		/// \param[in] enable True to enable, false to disable.
		void SetEnableDXTCompression(bool enable);

		struct ProcessingStatus
		{
			int packetsBuffered;
			int cpuPendingProcessing;
			int cpuProcessedAwaitingDeallocation;
			int cpuNumThreadsWorking;
			int sqlPendingProcessing;
			int sqlProcessedAwaitingDeallocation;
			int sqlNumThreadsWorking;
		};

		/// Return the thread and command processing statuses
		void GetProcessingStatus(ProcessingStatus * processingStatus);
		/// \internal
		void update();

		/// \internal For plugin handling
		virtual ConnectReceiveType onMessage(SocketMessage * msg);
		/// \internal For plugin handling
		virtual void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason);
		/// \internal For plugin handling
		virtual void shutdown(void);
		virtual void onAttach();
		virtual void onDetach();

		struct SessionNameAndSystemAddress
		{
			RakString sessionName;
			Address * mAddress;
			sqlite3 * referencedPointer;
			TimeDurMS timestampDelta;
		};
		DataStructures::List<SessionNameAndSystemAddress> loggedInSessions;

		// An incoming data msg, and when it arrived
		struct CPUThreadInputNode
		{
			SocketMessage * mMsg;
			// Time difference from their time to server time, plus the age of the database at the time the session was created
			// Applied to CPUThreadOutputNode::clientSendingTime before being passed to SQL
			TimeDurMS timestampDelta;
			RakString dbIdentifier;
		};
		// As packets arrive, they are added to a CPUThreadInput structure.
		// When the structure is full, or when a maximum amount of time has elapsed, whichever is first, then it is pushed to a thread for processing
		// Deallocated in the thread
		struct CPUThreadInput
		{
			// One or more incoming data packets, and when those packets arrived
			// Processed on the CPU, possibly with batch processing for image compression
			CPUThreadInputNode cpuInputArray[MAX_PACKETS_PER_CPU_INPUT_THREAD];
			int arraySize;
		};
		// Each CPUThreadInputNode is unpacked to a CPUThreadOutputNode
		// Images are now in compressed format, should the parameter list indeed have a query
		struct CPUThreadOutputNode
		{
			SocketMessage * mMsg; // Passthrough
			RakString dbIdentifier; // Passthrough
			String ipAddressString;
			RakString tableName;
			RakString file;
			TimeDurMS clientSendingTime;
			unsigned char parameterCount;
			bool isFunctionCall;
			DataStructures::List<RakString> insertingColumnNames;
			LogParameter parameterList[MAX_SQLLITE_LOGGER_PARAMETERS];
			uint32_t tickCount;
			int line;
		};
		// List of CPUThreadOutputNode
		struct CPUThreadOutput
		{
			// cpuOutputNodeArray pointers are not deallocated, just handed over to SQLThreadInput
			// Each element in the array is pushed to one SQLThreadInput
			CPUThreadOutputNode * cpuOutputNodeArray[MAX_PACKETS_PER_CPU_INPUT_THREAD];
			int arraySize;
		};
        /*
		struct SQLThreadInput
		{
			sqlite3 * dbHandle;
			CPUThreadOutputNode * cpuOutputNode;
		};
		struct SQLThreadOutput
		{
			// cpuOutputNode gets deallocated here
			CPUThreadOutputNode * cpuOutputNode;
		};
        */
	protected:
		NCount CreateDBHandle(RakString dbIdentifier);
		void CloseUnreferencedSessions(void);

		SessionManagementMode sessionManagementMode;
		bool createDirectoryForFile;
		RakString newDatabaseFilePath;

		ThreadManager<CPUThreadInput *, CPUThreadOutput *> cpuLoggerThreadPool;
		ThreadManager<SQLThreadInput, SQLThreadOutput> sqlLoggerThreadPool;

		CPUThreadInput * LockCpuThreadInput(void);
		void ClearCpuThreadInput(void);
		void UnlockCpuThreadInput(void);
		void CancelLockCpuThreadInput(void);
		void PushCpuThreadInputIfNecessary(void);
		void PushCpuThreadInput(void);
		void StopCPUSQLThreads(void);

		CPUThreadInput * cpuThreadInput;
		TimeDurMS whenCpuThreadInputAllocated;
		//bool dxtCompressionEnabled;
	};
}
}
#endif