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

#include "AutopatcherServer.h"
#include "DirectoryDeltaTransfer.h"
#include "FileList.h"
#include "StringTable.h"
#include "NiiNetLinkBase.h"
#include "FileListTransfer.h"
#include "FileListTransferCBInterface.h"
#include "NiiNetSerializer.h"
#include "NiiNetCommon.h"
#include "AutopatcherRepositoryInterface.h"
#include "N_assert.h"
#include "AutopatcherPatchContext.h"
#include <stdio.h>
#include <time.h>

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    const static unsigned HASH_LENGTH = 4;
    //------------------------------------------------------------------------
    void AutopatcherServerLoadNotifier_Printf::OnQueueUpdate(Address * remote,
        AutopatcherServerLoadNotifier::RequestType requestType,
            AutopatcherServerLoadNotifier::QueueOperation queueOperation,
                AutopatcherServerLoadNotifier::AutopatcherState * autopatcherState)
    {
        char * operationString;
        char * requestTypeString;
        String systemAddressString;
        remote.read(systemAddressString, true);
        if(requestType==ASUMC_GET_CHANGELIST)
            requestTypeString="GetChangelist";
        else
            requestTypeString="GetPatch";
        if(queueOperation == QO_WAS_ADDED)
            operationString="added";
        else if(queueOperation == QO_POPPED_ONTO_TO_PROCESSING_THREAD)
            operationString="processing";
        else if(queueOperation == QO_WAS_ABORTED)
            operationString="aborted";

        printf("%s %s %s. %i queued. %i working.\n", systemAddressString.c_str(), requestTypeString, operationString, autopatcherState->requestsQueued, autopatcherState->requestsWorking);
    }
    //------------------------------------------------------------------------
    void AutopatcherServerLoadNotifier_Printf::OnGetChangelistCompleted(
        Address * remote, AutopatcherServerLoadNotifier::GetChangelistResult getChangelistResult,
            AutopatcherServerLoadNotifier::AutopatcherState * autopatcherState)
    {
        String systemAddressString;
        remote.read(systemAddressString, true);

        char *changelistString;
        if (getChangelistResult==GCR_DELETE_FILES)
            changelistString="Delete files";
        else if (getChangelistResult==GCR_ADD_FILES)
            changelistString="Add files";
        else if (getChangelistResult==GCR_ADD_AND_DELETE_FILES)
            changelistString="Add and delete files";
        else if (getChangelistResult==GCR_NOTHING_TO_DO)
            changelistString="No files in changelist";
        else if (getChangelistResult==GCR_REPOSITORY_ERROR)
            changelistString="Repository error";

        printf("%s GetChangelist complete. %s. %i queued. %i working.\n", systemAddressString.c_str(), changelistString, autopatcherState->requestsQueued, autopatcherState->requestsWorking);
    }
    //------------------------------------------------------------------------
    void AutopatcherServerLoadNotifier_Printf::OnGetPatchCompleted(Address * remote,
        AutopatcherServerLoadNotifier::PatchResult patchResult,
            AutopatcherServerLoadNotifier::AutopatcherState * autopatcherState)
    {
        String systemAddressString;
        remote.read(systemAddressString, true);

        char * patchResultString;
        if (patchResult == PR_NO_FILES_NEEDED_PATCHING)
            patchResultString = "No files needed patching";
        else if (patchResult == PR_REPOSITORY_ERROR)
            patchResultString = "Repository error";
        else if (patchResult == PR_DISALLOWED_DOWNLOADING_ORIGINAL_FILES)
            patchResultString = "Disallowed downloading original files";
        else if (patchResult == PR_PATCHES_WERE_SENT)
            patchResultString = "Files pushed for patching";
        else if (patchResult == PR_ABORTED_FROM_INPUT_THREAD)
            patchResultString = "Aborted from input thread";
        else if (patchResult == PR_ABORTED_FROM_DOWNLOAD_THREAD)
            patchResultString = "Aborted from download thread";

        printf("%s GetPatch complete. %s. %i queued. %i working.\n", 
            systemAddressString.c_str(), patchResultString, autopatcherState->requestsQueued, autopatcherState->requestsWorking);
    }
    //------------------------------------------------------------------------
    AutopatcherServer::AutopatcherServer()
    {
        fileListTransfer = 0;
        mPacketLevel = HIGH_PRIORITY;
        mPacketCh = 0;
        maxConcurrentUsers = 0;
        loadNotifier = 0;
        cache_minTime = 0;
        cache_maxTime = 0;
        cacheLoaded = false;
        allowDownloadOfOriginalUnmodifiedFiles = true;
    }
    //------------------------------------------------------------------------
    AutopatcherServer::~AutopatcherServer()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::SetUploadSendParameters(PacketLevel plevel, char pchannel)
    {
        mPacketLevel = plevel;
        mPacketCh = pchannel;
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::SetFileListTransferPlugin(FileListTransfer * flt)
    {
        if (fileListTransfer)
            fileListTransfer->RemoveCallback(this);
        fileListTransfer=flt;
        if (fileListTransfer)
            fileListTransfer->AddCallback(this);
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::StartThreads(int numThreads, int numSQLConnections,
        AutopatcherRepositoryInterface ** sqlConnectionPtrArray)
    {
        N_assert(numSQLConnections >= numThreads);

        connectionPoolMutex.Lock();
        for(int i = 0; i < numSQLConnections; i++)
        {
            // Test the pointers passed, in case the user incorrectly casted an array of a different type
            sqlConnectionPtrArray[i]->GetLastError();
            connectionPool.Push(sqlConnectionPtrArray[i]);
        }
        connectionPoolMutex.Unlock();
        threadPool.startup(numThreads);
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::CacheMostRecentPatch(const char * applicationName)
    {
        if (connectionPool.Size()>0)
        {
            if (applicationName)
                cache_appName = applicationName;
            else
                cache_appName.Clear();
            cache_patchedFiles.Clear();
            cache_addedFiles.Clear();
            cache_deletedFiles.Clear();
            cache_addedOrModifiedFileHashes.Clear();
            cache_minTime = 0;
            cache_maxTime = 0;

            cacheLoaded = connectionPool[0]->GetMostRecentChangelistWithPatches(cache_appName, &cache_patchedFiles, &cache_addedFiles, &cache_addedOrModifiedFileHashes, &cache_deletedFiles, &cache_minTime, &cache_maxTime);
            if (cacheLoaded == false)
            {
                printf("Warning: Cache not loaded. This is OK if no patch was ever saved.\n");
            }
        }
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::onAttach()
    {
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::onDetach()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::update()
    {
        while (PatchingUserLimitReached() == false && userRequestWaitingQueue.Size() > 0)
        {
            SocketMessage * msg = PopOffWaitingQueue();
            switch (msg->data[0])
            {
            case ID_AUTOPATCHER_GET_CHANGELIST_SINCE_DATE:
                OnGetChangelistSinceDateInt(msg);
                break;
            // Client sends ID_AUTOPATCHER_GET_PATCH with files that they have different or missing
            case ID_AUTOPATCHER_GET_PATCH:
                OnGetPatchInt(msg);
                break;
            }
            N_delete msg;
        }
    }
    //------------------------------------------------------------------------
    ConnectReceiveType AutopatcherServer::onMessage(SocketMessage * msg)
    {
        switch (msg->data[0])
        {
        case ID_AUTOPATCHER_GET_CHANGELIST_SINCE_DATE:
            return OnGetChangelistSinceDate(msg);
        case ID_AUTOPATCHER_GET_PATCH:
            return OnGetPatch(msg);
        }

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::shutdown()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::Clear()
    {
        // Clear the waiting input and output from the thread pool.
        threadPool.shutdown();
        threadPool.removeAllInput();

        while (userRequestWaitingQueue.Size())
            N_delete AbortOffWaitingQueue();

        patchingUsers.Clear(true);
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::OnStartup(LinkBase * peer)
    {
        (void) peer;
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::onConnectClose(const Address & address, UniqueID id, 
        ConnectCloseType lostConnectionReason)
    {
        RemoveFromThreadPool(address);

        NCount i = 0;
        patchingUsersMutex.Lock();
        while (i < patchingUsers.Size())
        {
            if (patchingUsers[i] == address)
                patchingUsers.RemoveAtIndexFast(i);
            else
                i++;
        }
        patchingUsersMutex.Unlock();

        i = 0;
        while (i < userRequestWaitingQueue.Size())
        {
            if (userRequestWaitingQueue[i]->mAddress == address)
                userRequestWaitingQueue.RemoveAtIndex(i);
            else
                i++;
        }
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::RemoveFromThreadPool(Address * address)
    {
        unsigned i = 0;
        AutopatcherServerJobIdentifier temp(address);
        threadPool.remove(temp);
    }
    //------------------------------------------------------------------------
    class AutopatcherServerJob : public Job
    {
    public:
        AutopatcherServerJob() : Job(0){}
        
        ~AutopatcherServerJob()
        {
            if (server->DecrementPatchingUserCount(mAddress))
                server->CallPatchCompleteCallback(mAddress, AutopatcherServerLoadNotifier::PR_ABORTED_FROM_INPUT_THREAD);
            OP_DELETE(clientList);
        }
        
        class AutopatcherServerJobIdentifier : public Job::Identifier
        {
        public:
            AutopatcherServerJobIdentifier(Address * addrs) : mAddress(addrs) {}
            bool isOnly() const { return false; }
        public:
            const Address * mAddress;
        };
        
        bool equal(const Identifier * id)
        {
            const AutopatcherServerJobIdentifier * temp = dynamic_cast<const Identifier *>(id);
            if(temp)
            {
                if(mAddress == *temp->mAddress)
                    return true;
            }
            return false;
        }
        
        AutopatcherServer * server;
        RakString applicationName;
        NIId lastUpdateDate;
        Address * mAddress;
        FileList *clientList;
        unsigned short setId;
    };
    //------------------------------------------------------------------------
    class ChangelistSinceDateJobPrc : public JobPrc
    {
    public:
        ChangelistSinceDateJobPrc() : JobPrc(true), mRepository(0){}
        JobResult * handle(Job * jop);
        
        void onPrcBegin(Job * jop)
        {
            AutopatcherServer * ser = static_cast<AutopatcherServerJob>(jop)->server;
            ser->connectionPoolMutex.Lock();
            mRepository = ser->connectionPool.Pop();
            ser->connectionPoolMutex.Unlock();
        }

        void onPrcEnd(Job * jop)
        {
            AutopatcherServer * ser = static_cast<AutopatcherServerJob>(jop)->server;
            ser->connectionPoolMutex.Lock();
            ser->connectionPool.Push(mRepository);
            ser->connectionPoolMutex.Unlock();
            mRepository = 0;
        }
    protected:
        AutopatcherRepositoryInterface * mRepository;
    };
    //------------------------------------------------------------------------
    JobResult * ChangelistSinceDateJobPrc::handle(Job * jop)
    {
        AutopatcherServerJob * threadData = static_cast<AutopatcherServerJob *>(jop);
        FileList addedOrModifiedFilesWithHashData, deletedFiles;
        AutopatcherServer * server = threadData->server;

        AutopatcherServer::ResultTypeAndBitstream rtab;
        rtab.mAddress = threadData->mAddress;
        rtab.deletedFiles = &deletedFiles;
        rtab.addedOrModifiedFilesWithHashData = &addedOrModifiedFilesWithHashData;

        // Query the database for a changelist since this date
        N_assert(server);

        if (mRepository->GetChangelistSinceDate(threadData->applicationName.C_String(), rtab.addedOrModifiedFilesWithHashData, rtab.deletedFiles, threadData->lastUpdateDate))
        {
            rtab.resultCode = 1;
        }
        else
        {
            rtab.resultCode = 0;
        }

        rtab.operation = AutopatcherServer::ResultTypeAndBitstream::GET_CHANGELIST_SINCE_DATE;
        rtab.currentDate = (NIId) time(NULL);

        if (rtab.resultCode == 1)
        {
            if (rtab.deletedFiles->fileList.Size())
            {
                rtab.bitStream1.write((unsigned char) ID_AUTOPATCHER_DELETION_LIST);
                rtab.deletedFiles->read(&rtab.bitStream1);
            }

            if (rtab.addedOrModifiedFilesWithHashData->fileList.Size())
            {
                rtab.bitStream2.write((unsigned char) ID_AUTOPATCHER_CREATION_LIST);
                rtab.addedOrModifiedFilesWithHashData->read(&rtab.bitStream2);
                rtab.bitStream2.write(rtab.currentDate);
                rtab.bitStream2.write((NIId)0);

                rtab.addedOrModifiedFilesWithHashData->Clear();
            }
            else
            {
                rtab.bitStream2.write((unsigned char) ID_AUTOPATCHER_FINISHED);
                rtab.bitStream2.write(rtab.currentDate);
            }
        }
        else
        {
            rtab.bitStream2.write((unsigned char) ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR);
            StringCompressor::Instance()->EncodeString(mRepository->GetLastError(), 256, &rtab.bitStream2);
        }
        //*returnOutput=false;

        if (server->DecrementPatchingUserCount(rtab.mAddress))
        {
            if (rtab.bitStream1.getWriteOffset() > 0)
                server->send(&(rtab.bitStream1), server->mPacketLevel, RELIABLE_ORDERED, server->mPacketCh, rtab.mAddress, false);
            if (rtab.bitStream2.getWriteOffset() > 0)
                server->send(&(rtab.bitStream2), server->mPacketLevel, RELIABLE_ORDERED, server->mPacketCh, rtab.mAddress, false);

            if (server->loadNotifier)
            {
                AutopatcherServerLoadNotifier::AutopatcherState autopatcherState;
                autopatcherState.requestsQueued = server->userRequestWaitingQueue.Size();
                autopatcherState.requestsWorking = server->patchingUsers.Size();

                AutopatcherServerLoadNotifier::GetChangelistResult getChangelistResult;
                if (rtab.resultCode!=1)
                    getChangelistResult=AutopatcherServerLoadNotifier::GCR_REPOSITORY_ERROR;
                else if (rtab.deletedFiles->fileList.Size()==0 && rtab.addedOrModifiedFilesWithHashData->fileList.Size()==0)
                    getChangelistResult=AutopatcherServerLoadNotifier::GCR_NOTHING_TO_DO;
                else if (rtab.deletedFiles->fileList.Size()==0)
                    getChangelistResult=AutopatcherServerLoadNotifier::GCR_ADD_FILES;
                else if (rtab.addedOrModifiedFilesWithHashData->fileList.Size()==0)
                    getChangelistResult=AutopatcherServerLoadNotifier::GCR_DELETE_FILES;
                else
                    getChangelistResult=AutopatcherServerLoadNotifier::GCR_ADD_AND_DELETE_FILES;

                server->loadNotifier->OnGetChangelistCompleted(rtab.mAddress, getChangelistResult, &autopatcherState);
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    ConnectReceiveType AutopatcherServer::OnGetChangelistSinceDate(SocketMessage * msg)
    {
        NetSerializer in(msg->data, msg->length, false);
        AutopatcherServerJob threadData;
        threadData.clientList=0;
        in.skipRead(1, 0);
        in.readCompress(threadData.applicationName);
        in.read(threadData.lastUpdateDate);

        if (cacheLoaded && threadData.lastUpdateDate!=0 && threadData.applicationName==cache_appName)
        {
            NetSerializer bitStream1;
            NetSerializer bitStream2;
            NIId currentDate=(NIId) time(NULL);
            if (cache_maxTime!=0 && threadData.lastUpdateDate>cache_maxTime)
            {
                bitStream2.write((unsigned char) ID_AUTOPATCHER_FINISHED);
                bitStream2.write(currentDate);
                send(&bitStream2, mPacketLevel, RELIABLE_ORDERED,mPacketCh, msg->mAddress, false);
                return CRT_Destroy;
            }

            // Check in-memory cache, use if possible rather than accessing database
            if (cache_minTime!=0 && threadData.lastUpdateDate>cache_minTime)
            {
                if (cache_deletedFiles.fileList.Size())
                {
                    bitStream1.write((unsigned char) ID_AUTOPATCHER_DELETION_LIST);
                    cache_deletedFiles.read(&bitStream1);
                    send(&bitStream1, mPacketLevel, RELIABLE_ORDERED,mPacketCh, msg->mAddress, false);
                }
                if (cache_addedOrModifiedFileHashes.fileList.Size())
                {
                    bitStream2.write((unsigned char) ID_AUTOPATCHER_CREATION_LIST);
                    cache_addedOrModifiedFileHashes.read(&bitStream2);
                    bitStream2.write(currentDate);
                    bitStream2.write(threadData.lastUpdateDate);
                }
                else
                {
                    bitStream2.write((unsigned char) ID_AUTOPATCHER_FINISHED);
                    bitStream2.write(currentDate);
                }
                send(&bitStream2, mPacketLevel, RELIABLE_ORDERED,mPacketCh, msg->mAddress, false);

                return CRT_Destroy;
            }
        }
        if (PatchingUserLimitReached())
        {
            AddToWaitingQueue(msg);
            return CRT_Inner;
        }
        OnGetChangelistSinceDateInt(msg);
        return CRT_Destroy;
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::OnGetChangelistSinceDateInt(SocketMessage * msg)
    {
        NetSerializer in(msg->data, msg->length, false);
        AutopatcherServerJob threadData;
        threadData.clientList = 0;
        in.skipRead(1, 0);
        in.readCompress(threadData.applicationName);
        in.read(threadData.lastUpdateDate);

        if (IncrementPatchingUserCount(msg->mAddress))
        {
            CallPacketCallback(msg, AutopatcherServerLoadNotifier::QO_POPPED_ONTO_TO_PROCESSING_THREAD);

            threadData.server = this;
            threadData.mAddress = msg->mAddress;
            threadPool.add(threadData, ChangelistSinceDateJobPrc);
        }
    }
    //------------------------------------------------------------------------
    class PatchJobPrc : public JobPrc
    {
    public:
        PatchJobPrc() : JobPrc(true), mRepository(0){}
        JobResult * handle(Job * jop);
        
        void onPrcBegin(Job * jop)
        {
            AutopatcherServer * ser = static_cast<AutopatcherServerJob>(jop)->server;
            ser->connectionPoolMutex.Lock();
            mRepository = ser->connectionPool.Pop();
            ser->connectionPoolMutex.Unlock();
        }

        void onPrcEnd(Job * jop)
        {
            AutopatcherServer * ser = static_cast<AutopatcherServerJob>(jop)->server;
            ser->connectionPoolMutex.Lock();
            ser->connectionPool.Push(mRepository);
            ser->connectionPoolMutex.Unlock();
            mRepository = 0;
        }
    protected:
        AutopatcherRepositoryInterface * mRepository;
    };
    //------------------------------------------------------------------------
    JobResult * PatchJobPrc::handle(Job * jop)
    {
        AutopatcherServerJob * threadData = static_cast<AutopatcherServerJob *>(jop);
        AutopatcherServer * server = threadData->server;

        AutopatcherServer::ResultTypeAndBitstream rtab;
        rtab.mAddress = threadData->mAddress;
        FileList fileList;
        rtab.patchList = &fileList;
        N_assert(server);
        rtab.resultCode = mRepository->GetPatches(threadData->applicationName.C_String(), threadData->clientList, server->allowDownloadOfOriginalUnmodifiedFiles, rtab.patchList);
        rtab.operation = AutopatcherServer::ResultTypeAndBitstream::GET_PATCH;
        rtab.setId = threadData->setId;
        rtab.currentDate = (NIId) time(NULL);

        OP_DELETE(threadData->clientList);

        if (rtab.resultCode == 1)
        {
            if (rtab.patchList->fileList.Size())
            {
                server->fileListTransfer->send(rtab.patchList, 0, rtab.mAddress, rtab.setId, server->mPacketLevel, server->mPacketCh,
                    repository, repository->GetIncrementalReadChunkSize());
            }
            else
            {
                // No files needed to send
                if (server->DecrementPatchingUserCount(rtab.mAddress))
                    server->CallPatchCompleteCallback(rtab.mAddress, AutopatcherServerLoadNotifier::PR_NO_FILES_NEEDED_PATCHING);
            }

            rtab.bitStream1.write((unsigned char) ID_AUTOPATCHER_FINISHED_INTERNAL);
            rtab.bitStream1.write(rtab.currentDate);
        }
        else
        {
            AutopatcherServerLoadNotifier::PatchResult pr;
            if (rtab.resultCode==0)
            {
                rtab.bitStream1.write((unsigned char) ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR);
                StringCompressor::Instance()->EncodeString(repository->GetLastError(), 256, &rtab.bitStream1);
                pr = AutopatcherServerLoadNotifier::PR_REPOSITORY_ERROR;
            }
            else
            {
                rtab.bitStream1.write((unsigned char) ID_AUTOPATCHER_CANNOT_DOWNLOAD_ORIGINAL_UNMODIFIED_FILES);
                pr = AutopatcherServerLoadNotifier::PR_DISALLOWED_DOWNLOADING_ORIGINAL_FILES;
            }

            if (server->DecrementPatchingUserCount(rtab.mAddress))
            {
                server->CallPatchCompleteCallback(rtab.mAddress, pr);
            }
            else
            {
                //*returnOutput = false;
                return 0;
            }
        }

        //*returnOutput = false;

        if (rtab.bitStream1.getWriteOffset()>0)
            server->send(&(rtab.bitStream1), server->mPacketLevel, RELIABLE_ORDERED, server->mPacketCh, rtab.mAddress, false);

        if (rtab.bitStream2.getWriteOffset()>0)
            server->send(&(rtab.bitStream2), server->mPacketLevel, RELIABLE_ORDERED, server->mPacketCh, rtab.mAddress, false);

        return 0;
    }
    //------------------------------------------------------------------------
    ConnectReceiveType AutopatcherServer::OnGetPatch(SocketMessage *msg)
    {
        NetSerializer in(msg->data, msg->length, false);
        AutopatcherServerJob threadData;
        in.skipRead(1, 0);
        in.read(threadData.setId);
        NIId lastUpdateDate;
        in.read(lastUpdateDate);
        in.readCompress(threadData.applicationName);
        threadData.clientList=0;

        // Check in-memory cache, use if possible rather than accessing database
        if (threadData.applicationName == cache_appName && lastUpdateDate != 0 && cacheLoaded && cache_minTime!=0 && lastUpdateDate>cache_minTime)
        {
            threadData.mAddress = msg->mAddress;
            threadData.server=this;
            threadData.clientList=OP_NEW<FileList>();

            if (threadData.clientList->Deserialize(&in) == false)
            {
                OP_DELETE(threadData.clientList);
                return CRT_Destroy;
            }
            if (threadData.clientList->fileList.Size() == 0)
            {
                N_assert(0);
                OP_DELETE(threadData.clientList);
                return CRT_Destroy;
            }

            char *userHash;
            RakString userFilename;
            FileList patchList;
            bool cacheUpdateFailed=false;

            NCount i,j;
            // FileList is the list of all files missing or changed as determined by the client
            for (i=0; i < threadData.clientList->fileList.Size(); i++)
            {
                userHash=threadData.clientList->fileList[i].data;
                userFilename=threadData.clientList->fileList[i].filename;

                if (userHash)
                {
                    // If the user has a hash, check for this file in cache_patchedFiles. If not found, or hash is wrong, use DB
                    if (threadData.clientList->fileList[i].dataLengthBytes != HASH_LENGTH)
                    {
                        OP_DELETE(threadData.clientList);
                        return CRT_Destroy;
                    }

                    for (j = 0; j < cache_patchedFiles.fileList.Size(); ++j)
                    {
                        if (userFilename == cache_patchedFiles.fileList[j].filename)
                        {
                            if (memcmp(cache_patchedFiles.fileList[j].data, userHash, HASH_LENGTH)==0)
                            {
                                // send patch
                                N_assert(cache_patchedFiles.fileList[j].context.op==PC_HASH_2_WITH_PATCH);
                                patchList.AddFile(userFilename,userFilename, 0, cache_patchedFiles.fileList[j].dataLengthBytes, cache_patchedFiles.fileList[j].fileLengthBytes, cache_patchedFiles.fileList[j].context, true, false);
                            }
                            else
                            {
                                // Bad hash
                                cacheUpdateFailed=true;
                            }

                            break;
                        }
                    }

                    if (j==cache_patchedFiles.fileList.Size())
                    {
                        // Didn't find the patch even though the client has an older version of the file
                        cacheUpdateFailed=true;
                    }
                }
                else
                {
                    // If the user does not have a hash, check for this file in cache_addedFiles. If not found, use DB
                    for (j=0; j < cache_addedFiles.fileList.Size(); j++)
                    {
                        if (userFilename == cache_addedFiles.fileList[j].filename)
                        {
                            // send added file
                            patchList.AddFile(userFilename,userFilename, 0, cache_addedFiles.fileList[j].dataLengthBytes, cache_addedFiles.fileList[j].fileLengthBytes, cache_addedFiles.fileList[j].context, true, false);
                            break;
                        }
                    }

                    if (j==cache_addedFiles.fileList.Size())
                    {
                        // Didn't find the file in the cache even though the client asked for it
                        cacheUpdateFailed=true;
                    }
                }

                if (cacheUpdateFailed==true)
                {
                    // Failure to find file in cache
                    // Will fall to use database
                    patchList.Clear();
                    break;
                }
            }

            if (patchList.fileList.Size()>0)
            {
                if (IncrementPatchingUserCount(msg->mAddress))
                {
                    fileListTransfer->send(&patchList, 0, msg->mAddress, threadData.setId,
                        mPacketLevel, mPacketCh, this, 262144*4*4);
                    NetSerializer bitStream1;
                    bitStream1.write((unsigned char) ID_AUTOPATCHER_FINISHED_INTERNAL);
                    NIId t =(NIId) time(NULL);
                    bitStream1.write(t);
                    send(&bitStream1, mPacketLevel, RELIABLE_ORDERED, mPacketCh, msg->mAddress, false);

                    OP_DELETE(threadData.clientList);
                    return CRT_Destroy;
                }
            }
        }

        OP_DELETE(threadData.clientList);

        if (PatchingUserLimitReached())
        {
            AddToWaitingQueue(msg);
            return CRT_Inner;
        }

        OnGetPatchInt(msg);
        return CRT_Destroy;
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::OnGetPatchInt(SocketMessage * msg)
    {
        NetSerializer in(msg->data, msg->length, false);
        AutopatcherServerJob threadData;
        in.skipRead(1, 0);
        in.read(threadData.setId);
        NIId lastUpdateDate;
        in.read(lastUpdateDate);
        in.readCompress(threadData.applicationName);
        threadData.mAddress = msg->mAddress;
        threadData.server = this;
        threadData.clientList = OP_NEW<FileList>();

        if(threadData.clientList->Deserialize(&in) == false)
        {
            OP_DELETE(threadData.clientList);
            return;
        }
        if(threadData.clientList->fileList.Size() == 0)
        {
            N_assert(0);
            OP_DELETE(threadData.clientList);
            return;
        }

        if (IncrementPatchingUserCount(msg->mAddress))
            CallPacketCallback(msg, AutopatcherServerLoadNotifier::QO_POPPED_ONTO_TO_PROCESSING_THREAD);

        threadPool.add(threadData, PatchJobPrc);
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::OnFilePushesComplete(Address * address, unsigned short setID)
    {
        if (DecrementPatchingUserCount(address))
            CallPatchCompleteCallback(address, AutopatcherServerLoadNotifier::PR_PATCHES_WERE_SENT);
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::OnSendAborted(Address * address)
    {
        if (DecrementPatchingUserCount(address))
            CallPatchCompleteCallback(address, AutopatcherServerLoadNotifier::PR_ABORTED_FROM_DOWNLOAD_THREAD);
    }
    //------------------------------------------------------------------------
    bool AutopatcherServer::IncrementPatchingUserCount(Address * sa)
    {
        // A system address may exist more than once in patchingUsers
        patchingUsersMutex.Lock();
        patchingUsers.Insert(sa);
        patchingUsersMutex.Unlock();
        return true;
    }
    //------------------------------------------------------------------------
    bool AutopatcherServer::DecrementPatchingUserCount(Address * sa)
    {
        NCount i;
        patchingUsersMutex.Lock();
        for (i=0; i < patchingUsers.Size(); i++)
        {
            if (patchingUsers[i]==sa)
            {
                patchingUsers.RemoveAtIndexFast(i);
                patchingUsersMutex.Unlock();
                return true;
            }
        }
        patchingUsersMutex.Unlock();
        return false;
    }
    //------------------------------------------------------------------------
    bool AutopatcherServer::PatchingUserLimitReached(void) const
    {
        if (maxConcurrentUsers==0)
            return false;

        return patchingUsers.Size()>=maxConcurrentUsers;
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::SetMaxConurrentUsers(NCount _maxConcurrentUsers)
    {
        maxConcurrentUsers=_maxConcurrentUsers;
    }
    //------------------------------------------------------------------------
    NCount AutopatcherServer::GetMaxConurrentUsers(void) const
    {
        return maxConcurrentUsers;
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::CallPacketCallback(SocketMessage *msg,
        AutopatcherServerLoadNotifier::QueueOperation queueOperation)
    {
        if (loadNotifier)
        {
            AutopatcherServerLoadNotifier::AutopatcherState autopatcherState;
            autopatcherState.requestsQueued=userRequestWaitingQueue.Size();
            autopatcherState.requestsWorking=patchingUsers.Size();

            AutopatcherServerLoadNotifier::RequestType requestType;
            if (msg->data[0]==ID_AUTOPATCHER_GET_CHANGELIST_SINCE_DATE)
                requestType=AutopatcherServerLoadNotifier::ASUMC_GET_CHANGELIST;
            else
                requestType=AutopatcherServerLoadNotifier::ASUMC_GET_PATCH;

            loadNotifier->OnQueueUpdate(msg->mAddress, requestType, queueOperation, &autopatcherState);
        }
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::CallPatchCompleteCallback(const Address &address, AutopatcherServerLoadNotifier::PatchResult patchResult)
    {
        if (loadNotifier)
        {
            AutopatcherServerLoadNotifier::AutopatcherState autopatcherState;
            autopatcherState.requestsQueued=userRequestWaitingQueue.Size();
            autopatcherState.requestsWorking=patchingUsers.Size();

            loadNotifier->OnGetPatchCompleted(address, patchResult, &autopatcherState);
        }
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::AddToWaitingQueue(SocketMessage * msg)
    {
        userRequestWaitingQueue.Push(msg);
        CallPacketCallback(msg, AutopatcherServerLoadNotifier::QO_WAS_ADDED);
    }
    //------------------------------------------------------------------------
    SocketMessage * AutopatcherServer::AbortOffWaitingQueue(void)
    {
        SocketMessage * msg = userRequestWaitingQueue.Pop();
        CallPacketCallback(msg,AutopatcherServerLoadNotifier::QO_WAS_ABORTED);
        return msg;
    }
    //------------------------------------------------------------------------
    SocketMessage *AutopatcherServer::PopOffWaitingQueue(void)
    {
        return userRequestWaitingQueue.Pop();;
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::SetLoadManagementCallback(AutopatcherServerLoadNotifier *asumc)
    {
        loadNotifier=asumc;
    }
    //------------------------------------------------------------------------
    void AutopatcherServer::SetAllowDownloadOfOriginalUnmodifiedFiles(bool allow)
    {
        allowDownloadOfOriginalUnmodifiedFiles = allow;
    }
    //------------------------------------------------------------------------
    NCount AutopatcherServer::GetFilePart(const char *filename, NCount startReadBytes,
        NCount numBytesToRead, void * preallocatedDestination, FileListNodeContext context)
    {
        /*
        int offset;
        if (context.op==PC_HASH_1_WITH_PATCH)
            offset=HASH_LENGTH;
        else if (context.op==PC_HASH_2_WITH_PATCH)
            offset=HASH_LENGTH*2;
        else
            offset=0;

        int bytesToRead;
        if (startReadBytes + numBytesToRead > context.dataLength-offset)
            bytesToRead=(context.dataLength-offset)-startReadBytes;
        else
            bytesToRead=numBytesToRead;

        memcpy(preallocatedDestination, ((char*)context.dataPtr)+offset, bytesToRead);
        */

        int bytesToRead;
        if (startReadBytes + numBytesToRead > context.dataLength)
            bytesToRead=(context.dataLength)-startReadBytes;
        else
            bytesToRead=numBytesToRead;

        memcpy(preallocatedDestination, context.dataPtr, bytesToRead);
        return bytesToRead;
    }
    //------------------------------------------------------------------------
}
}
