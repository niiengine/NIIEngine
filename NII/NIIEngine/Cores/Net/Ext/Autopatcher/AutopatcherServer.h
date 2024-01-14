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

// TODO - bsdiff doesn't work for files above 100 megabytes.
// See http://xdelta.org/
// XDelta is GPL 2, however I could run that as a separate EXE and invoke to only transmit the delta file.

// See http://pocketsoft.com/rtpatch.html
// See use rdiff instead of bsdiff, or perhaps librsync

// See https://code.google.com/p/open-vcdiff/
// https://code.google.com/p/open-vcdiff/wiki/HowToUseOpenVcdiff
// https://github.com/gtoubassi/femtozip/wiki/Sdch

#ifndef __AUTOPATCHER_SERVER_H
#define __AUTOPATCHER_SERVER_H

#include "NiiNetCommon.h"
#include "Export.h"
#include "NiiNetPlugin.h"
#include "PacketLevel.h"
#include "NiiThreadManager.h"
#include "NiiNetSerializer.h"
#include "RakString.h"
#include "FileList.h"
#include "NiiNetBufferReadBase.h"

namespace NII
{
namespace NII_NET
{
    /// Forward declarations
    class LinkBase;
    struct SocketMessage;
    class AutopatcherRepositoryInterface;
    class FileListTransfer;

    class RAK_DLL_EXPORT AutopatcherServerLoadNotifier
    {
    public:
        /// Current queue state of the autopatcher
        struct AutopatcherState
        {
            /// How many requests have been queued to be processed later
            unsigned int requestsQueued;

            /// How many requests are currently working (including downloading files).
            /// This will not normally exceed AutopatcherServer::SetMaxConurrentUsers()
            unsigned int requestsWorking;
        };

        /// The server only handles two types of requests - to get a change list since a certain date, or to get a patch
        enum RequestType
        {
            ASUMC_GET_CHANGELIST,
            ASUMC_GET_PATCH,
        };

        enum QueueOperation
        {
            QO_WAS_ADDED,
            QO_WAS_ABORTED,
            QO_POPPED_ONTO_TO_PROCESSING_THREAD
        };

        enum GetChangelistResult
        {
            GCR_DELETE_FILES,
            GCR_ADD_FILES,
            GCR_ADD_AND_DELETE_FILES,
            GCR_NOTHING_TO_DO,
            GCR_REPOSITORY_ERROR,
        };

        enum PatchResult
        {
            PR_NO_FILES_NEEDED_PATCHING,
            PR_REPOSITORY_ERROR,
            PR_DISALLOWED_DOWNLOADING_ORIGINAL_FILES,
            PR_PATCHES_WERE_SENT,
            PR_ABORTED_FROM_INPUT_THREAD,
            PR_ABORTED_FROM_DOWNLOAD_THREAD,
        };

        /// The server queues have been updated
        /// \param[out] remote Which system this refers to
        /// \param[out] requestType Either added to / removed a changelist request, or a get patch request
        /// \param[out] queueOperation The operation was added to the queue, removed from the queue to be processed, or removed because it was aborted
        /// \param[out] autopatcherState Current size of the request queue, and how many requests are working
        virtual void OnQueueUpdate(Address * remote,
            AutopatcherServerLoadNotifier::RequestType requestType,
            AutopatcherServerLoadNotifier::QueueOperation queueOperation,
            AutopatcherServerLoadNotifier::AutopatcherState *autopatcherState)
        {(void) remote; (void) requestType; (void) queueOperation; (void) autopatcherState;}

        virtual void OnGetChangelistCompleted(
            Address * remote,
            AutopatcherServerLoadNotifier::GetChangelistResult getChangelistResult,
            AutopatcherServerLoadNotifier::AutopatcherState *autopatcherState)
        {(void) remote; (void) autopatcherState;}

        /// A file transfer has completed, or was not necessary
        /// \param[out] remote Which system this refers to
        /// \param[out] autopatcherState Current size of the request queue, and how many requests are working
        virtual void OnGetPatchCompleted(Address * remote,
            AutopatcherServerLoadNotifier::PatchResult patchResult,
            AutopatcherServerLoadNotifier::AutopatcherState *autopatcherState)
        {(void) remote; (void) patchResult; (void) autopatcherState;};
    };

    /// \brief Sample implementation of AutopatcherServerLoadNotifier using printf
    class RAK_DLL_EXPORT AutopatcherServerLoadNotifier_Printf : public AutopatcherServerLoadNotifier
    {
    public:
        virtual void OnQueueUpdate(Address * remote,
            AutopatcherServerLoadNotifier::RequestType requestType,
            AutopatcherServerLoadNotifier::QueueOperation queueOperation,
            AutopatcherServerLoadNotifier::AutopatcherState * autopatcherState);

        virtual void OnGetChangelistCompleted(Address * remote,
            AutopatcherServerLoadNotifier::GetChangelistResult getChangelistResult,
            AutopatcherServerLoadNotifier::AutopatcherState * autopatcherState);

        virtual void OnGetPatchCompleted(Address * remote,
            AutopatcherServerLoadNotifier::PatchResult patchResult,
            AutopatcherServerLoadNotifier::AutopatcherState * autopatcherState);
    };

    /// \brief The server plugin for the autopatcher.  Must be running for the client to get patches.
    class RAK_DLL_EXPORT AutopatcherServer : public Plugin , /*public ThreadPoolData,*/
        FileListProgress, BufferReadBase
    {
    public:
        AutopatcherServer();
        ~AutopatcherServer();

        /// DO THIS FIRST
        /// Implement to start the worker threads.
        /// Before this is called, no queries will be performed
        /// When this is called, AllocAutopatcherRepositoryInterface will be called with \a repositoryAllocationParameters
        /// The system works in three phases.
        /// 1. Get change list since a given date. This uses one of the worker threads.
        /// 2. If either running a full scan, or files have changed since a given date, get the list of patches. This uses one of the worker threads and does an intensive comparison of the hashes the client has vs. the files in the database
        /// 3. If the total amount of data to be sent exceeds DATABASE_READ_CHUNK_SIZE, defined in the cpp file, then the database will be read from incrementally during this download phase. This uses an sql connection object, which may or may not be also in use by one of the threads.
        /// If you have more sql connections than threads, this tends to prevent the same connection from being used to incrementally read files for a downloader, and to be used in a worker thread.
        /// \param[in] numThreads Number of processing threads, which handles the CPU intensive tasks of generating a patch list and comparing files
        /// \param[in] numSQLConnections Number of SQL connection objects passed to \a sqlConnectionPtrArray. Must be greater than or equal to numThreads
        /// \param[in] sqlConnectionPtrArray List of pointers to AutopatcherRepositoryInterface. C++ note: Don't just cast a derived class array, you need to take the pointer address of each item
        void StartThreads(int numThreads, int numSQLConnections, AutopatcherRepositoryInterface ** sqlConnectionPtrArray);

        /// Load the most recent patch in memory and keep it there
        /// This can take a lot of memory, but greatly speeds up serving patches, since disk access is not incurred
        /// \param[in] applicationName 0 means all, otherwise the name of the application to cache
        void CacheMostRecentPatch(const char * applicationName);

        /// What parameters to use for the LinkBase::send() call when uploading files.
        /// \param[in] plevel See LinkBase::send()
        /// \param[in] pchannel See LinkBase::send()
        void SetUploadSendParameters(PacketLevel plevel, char pchannel);

        /// This plugin has a dependency on the FileListTransfer plugin, which it uses to actually send the files.
        /// So you need an instance of that plugin registered with LinkBase, and a pointer to that interface should be passed here.
        /// \param[in] flt A pointer to a registered instance of FileListTransfer
        void SetFileListTransferPlugin(FileListTransfer *flt);

        /// This is the maximum number of users the patcher will service at one time (generally about equal to the number of downloads at once)
        /// If this limit is exceeded, the request msg will be put into a queue and serviced when slots are available
        /// Defaults to 0 (unlimited)
        /// \param[in] maxConcurrentUsers Pass 0 for unlimited, otherwise the max users to serve at once
        void SetMaxConurrentUsers(unsigned int _maxConcurrentUsers);

        /// \return Returns what was passed to SetMaxConurrentUsers();
        unsigned int GetMaxConurrentUsers(void) const;

        /// Set a callback to get notifications of when user requests are queued and processed
        /// This is primarily of use to load balance the server
        /// \param[in] asumc An externally allocated instance of AutopatcherServerLoadNotifier. Pass 0 to disable.
        void SetLoadManagementCallback(AutopatcherServerLoadNotifier *asumc);

        /// Set whether or not the client can download files that were never modified, that they do not have
        /// Defaults to true
        /// Set to false to disallow downloading the entire game through the autopatcher. In this case, the user must have a copy of the game through other means (such as a CD install)
        /// \param[in] allow True to allow downloading original game files, false to disallow
        void SetAllowDownloadOfOriginalUnmodifiedFiles(bool allow);

        /// Clear buffered input and output
        void Clear(void);

        /// \internal For plugin handling
        virtual void onAttach();

        /// \internal For plugin handling
        virtual void onDetach();;

        /// \internal For plugin handling
        virtual void update();

        /// \internal For plugin handling
        virtual ConnectReceiveType onMessage(SocketMessage * msg);
        
        /// \internal For plugin handling
        virtual void shutdown(void);
        
        /// \internal For plugin handling
        virtual void OnStartup(LinkBase * peer);
        
        /// \internal For plugin handling
        virtual void onConnectClose(const Address &address, UniqueID id, ConnectCloseType lostConnectionReason );

        /*struct ThreadData
        {
            AutopatcherServer *server;
            RakString applicationName;
            NIId lastUpdateDate;
            Address * mAddress;
            FileList *clientList;
            unsigned short setId;
        };
        */
        /// \deprecated
        struct ResultTypeAndBitstream
        {
            ResultTypeAndBitstream() {patchList = 0; deletedFiles = 0; addedOrModifiedFilesWithHashData = 0;}
            int resultType;
            Address * mAddress;
            NetSerializer bitStream1;
            NetSerializer bitStream2;
            FileList *patchList;
            FileList *deletedFiles, *addedOrModifiedFilesWithHashData;
            // bool fatalError;
            int resultCode; // 1 = success, 0 = unknown error, -1 = failed allowDownloadOfOriginalUnmodifiedFiles check
            unsigned short setId;
            NIId currentDate;
            enum
            {
                GET_CHANGELIST_SINCE_DATE,
                GET_PATCH,
            } operation;
        };

    protected:
        friend class ChangelistSinceDateJobPrc;
        friend class PatchJobPrc;
        
        ConnectReceiveType OnGetChangelistSinceDate(SocketMessage * msg);
        ConnectReceiveType OnGetPatch(SocketMessage * msg);
        void OnGetChangelistSinceDateInt(SocketMessage * msg);
        void OnGetPatchInt(SocketMessage * msg);
        
        void RemoveFromThreadPool(Address * address);
        virtual unsigned int GetFilePart(const char * filename, unsigned int startReadBytes,
            unsigned int numBytesToRead, void *preallocatedDestination, FileListNodeContext context);

        //AutopatcherRepositoryInterface *repository;
        FileListTransfer *fileListTransfer;
        PacketLevel mPacketLevel;
        char mPacketCh;

        // The point of the threadPool is so that SQL queries, which are blocking, happen in the thread and don't slow down the rest of the application
        // The threadPool has a queue for incoming processing requests.  As systems disconnect their pending requests are removed from the list.
        ThreadManager</*ThreadData,*/ ResultTypeAndBitstream *> threadPool;

        ThreadMutex connectionPoolMutex;
        DataStructures::Queue<AutopatcherRepositoryInterface *> connectionPool;

        // How many users are currently patching
        // unsigned int patchingUserCount;

        ThreadMutex patchingUsersMutex;
        DataStructures::List<Address *> patchingUsers;
        bool IncrementPatchingUserCount(Address * sa);
        bool DecrementPatchingUserCount(Address * sa);
        bool PatchingUserLimitReached(void) const;
        virtual void OnFilePushesComplete(Address * address, unsigned short setID);
        virtual void OnSendAborted(Address * address);

        unsigned int maxConcurrentUsers;
        // If maxConcurrentUsers is exceeded, then incoming requests are put into this queue
        DataStructures::Queue<SocketMessage *> userRequestWaitingQueue;
        void AddToWaitingQueue(SocketMessage * msg);
        SocketMessage * AbortOffWaitingQueue(void);
        SocketMessage * PopOffWaitingQueue(void);

        AutopatcherServerLoadNotifier * loadNotifier;
        void CallPacketCallback(SocketMessage * msg, AutopatcherServerLoadNotifier::QueueOperation queueOperation);
        void CallPatchCompleteCallback(const Address & address, AutopatcherServerLoadNotifier::PatchResult patchResult);

        RakString cache_appName;
        FileList cache_patchedFiles;
        FileList cache_addedFiles;
        FileList cache_addedOrModifiedFileHashes;
        FileList cache_deletedFiles;
        NIId cache_minTime, cache_maxTime;
        bool cacheLoaded;
        bool allowDownloadOfOriginalUnmodifiedFiles;
    };

}
}
#endif