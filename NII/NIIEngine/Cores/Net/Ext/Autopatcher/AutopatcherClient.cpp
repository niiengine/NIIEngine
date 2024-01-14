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
#include "AutopatcherClient.h"
#include "DirectoryDeltaTransfer.h"
#include "FileList.h"
#include "StringTable.h"
#include "NiiNetLinkBase.h"
#include "FileListTransfer.h"
#include "FileListTransferCBInterface.h"
#include "NiiNetSerializer.h"
#include "NiiNetCommon.h"
#include "AutopatcherPatchContext.h"
#include "ApplyPatch.h"
#include "NiiVFS.h"
#include "NiiThreadManager.h"

static const unsigned HASH_LENGTH = 4;

#define COPY_ON_RESTART_EXTENSION ".patched.tmp"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    PatchContext AutopatcherClientCBInterface::ApplyPatchBase(const char * oldFilePath,
        char ** newFileContents, unsigned int * newFileSize, char * patchContents,
            unsigned int patchSize, uint32_t patchAlgorithm)
    {
        return ApplyPatchBSDiff(oldFilePath, newFileContents, newFileSize, patchContents, patchSize);
    }
    //------------------------------------------------------------------------
    PatchContext AutopatcherClientCBInterface::ApplyPatchBSDiff(const char * oldFilePath,
        char ** newFileContents, unsigned int * newFileSize, char * patchContents,
            unsigned int patchSize)
    {
        FILE * fp;
        fp = fopen(oldFilePath, "rb");
        if (fp == 0)
            return PC_ERROR_PATCH_TARGET_MISSING;

        fseek(fp, 0, SEEK_END);
        unsigned int prePatchLength = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char * prePatchFile = (char *)rakMalloc_Ex(prePatchLength);
        fread(prePatchFile, prePatchLength, 1, fp);
        fclose(fp);

        bool result = ApplyPatch(prePatchFile, prePatchLength, newFileContents,
            newFileSize, patchContents, patchSize);
        rakFree_Ex(prePatchFile);

        if (result==false)
            return PC_ERROR_PATCH_APPLICATION_FAILURE;

        return PC_WRITE_FILE;
    }
    //------------------------------------------------------------------------
    struct AutopatcherClientThreadInfo
    {
        FileListTransferCBInterface::OnFileStruct onFileStruct;
        char applicationDirectory[512];
        PatchContext result;
        // unsigned prePatchLength;
        // char *prePatchFile;

        // postPatchFile is passed in PC_NOTICE_WILL_COPY_ON_RESTART
        char * postPatchFile;
        unsigned postPatchLength;
        AutopatcherClientCBInterface * cbInterface;
    };
    // -----------------------------------------------------------------------
    class AutopatcherClientJob : public Job
    {
    public:
        AutopatcherClientJob(AutopatcherClientThreadInfo * input) : Job(0), mInput(input){}
        ~AutopatcherClientJob()
        {
            if(mInput)
            {
                if (mInput->postPatchFile)
                    rakFree_Ex(mInput->postPatchFile);
                if (mInput->onFileStruct.fileData)
                    rakFree_Ex(mInput->onFileStruct.fileData);
                OP_DELETE(mInput);
                mInput = 0;
            }
        }
        AutopatcherClientThreadInfo * mInput;
    };
    
    class AutopatcherClientJobPrc : public JobPrc
    {
    public:
        AutopatcherClientJobPrc() : JobPrc(true){}
        
        JobResult * handle(Job * jop);
        
        void handle(JobResult * result);
    };
    
    class AutopatcherClientJobResult : public JobResult
    {
    public:
        AutopatcherClientJobResult(Job * src, AutopatcherClientThreadInfo * re) : JobResult(src), mResult(re){}
        
        ~AutopatcherClientJobResult()
        {
            if(mInput)
            {
                if (mInput->postPatchFile)
                    rakFree_Ex(mInput->postPatchFile);
                if (mInput->onFileStruct.fileData)
                    rakFree_Ex(mInput->onFileStruct.fileData);
                OP_DELETE(mInput);
                mInput = 0;
            }
        }
        AutopatcherClientThreadInfo * mResult;
    };
    //------------------------------------------------------------------------
    void AutopatcherClientJobPrc::handle(JobResult * result)
    {
            //if (threadPool.getOutputSize())
            //{
                AutopatcherClientThreadInfo *threadInfo = static_cast<AutopatcherClientJobResult *>(JobResult)->mResult;
                threadInfo->onFileStruct.context.op=threadInfo->result;
                switch (threadInfo->result)
                {
                    case PC_NOTICE_WILL_COPY_ON_RESTART:
                    {
                        client->CopyAndRestart(threadInfo->onFileStruct.fileName);
                        if (threadInfo->onFileStruct.context.op==PC_WRITE_FILE)
                        {
                            // Regular file in use but we can write the temporary file.  Restart and copy it over the existing
                            onFileCallback->OnFile(&threadInfo->onFileStruct);
                        }
                        else
                        {
                            // Regular file in use but we can write the temporary file.  Restart and copy it over the existing
                            rakFree_Ex(threadInfo->onFileStruct.fileData);
                            threadInfo->onFileStruct.fileData=threadInfo->postPatchFile;
                            onFileCallback->OnFile(&threadInfo->onFileStruct);
                            threadInfo->onFileStruct.fileData=0;
                        }
                    }
                    break;
                    case PC_ERROR_FILE_WRITE_FAILURE:
                    {
                        if (threadInfo->onFileStruct.context.op==PC_WRITE_FILE)
                        {
                            onFileCallback->OnFile(&threadInfo->onFileStruct);
                        }
                        else
                        {
                            rakFree_Ex(threadInfo->onFileStruct.fileData);
                            threadInfo->onFileStruct.fileData=threadInfo->postPatchFile;
                            threadInfo->onFileStruct.byteLengthOfThisFile=threadInfo->postPatchLength;
                            onFileCallback->OnFile(&threadInfo->onFileStruct);
                            threadInfo->onFileStruct.fileData=0;
                        }
                    }
                    break;
                    case PC_ERROR_PATCH_TARGET_MISSING:
                    {
                        onFileCallback->OnFile(&threadInfo->onFileStruct);
                        client->Redownload(threadInfo->onFileStruct.fileName);
                    }
                    break;
                    case PC_ERROR_PATCH_APPLICATION_FAILURE:
                    {
                        // Failure - signal class and download this file.
                        onFileCallback->OnFile(&threadInfo->onFileStruct);
                        client->Redownload(threadInfo->onFileStruct.fileName);
                    }
                    break;
                    case PC_ERROR_PATCH_RESULT_CHECKSUM_FAILURE:
                    {
                        // Failure - signal class and download this file.
                        onFileCallback->OnFile(&threadInfo->onFileStruct);
                        client->Redownload(threadInfo->onFileStruct.fileName);
                    }
                    break;
                    default:
                    {
                        if (threadInfo->onFileStruct.context.op==PC_WRITE_FILE)
                        {
                            onFileCallback->OnFile(&threadInfo->onFileStruct);
                        }
                        else
                        {
                            rakFree_Ex(threadInfo->onFileStruct.fileData);
                            threadInfo->onFileStruct.fileData=threadInfo->postPatchFile;
                            onFileCallback->OnFile(&threadInfo->onFileStruct);
                            threadInfo->onFileStruct.fileData=0;
                        }
                    }
                    break;
                }

                if (threadInfo->postPatchFile)
                    rakFree_Ex(threadInfo->postPatchFile);
                if (threadInfo->onFileStruct.fileData)
                    rakFree_Ex(threadInfo->onFileStruct.fileData);
                OP_DELETE(threadInfo);
            //}
    }
    //------------------------------------------------------------------------
    JobResult * AutopatcherClientJobPrc::handle(Job * jop)
    { 
        AutopatcherClientThreadInfo * input = static_cast<AutopatcherClientJob *>(jop)->mInput;
        char fullPathToDir[1024];
        strcpy(fullPathToDir, input->applicationDirectory);
        strcat(fullPathToDir, input->onFileStruct.fileName);
        if (input->onFileStruct.context.op == PC_WRITE_FILE)
        {
            if(FileSystem::wirteFile(fullPathToDir, (char*)input->onFileStruct.fileData, input->onFileStruct.byteLengthOfThisFile)==false)
            {
                char newDir[1024];
                strcpy(newDir, fullPathToDir);
                strcat(newDir, COPY_ON_RESTART_EXTENSION);
                if (FileSystem::wirteFile(newDir, (char*)input->onFileStruct.fileData, input->onFileStruct.byteLengthOfThisFile))
                {
                    input->result=PC_NOTICE_WILL_COPY_ON_RESTART;
                }
                else
                {
                    input->result=PC_ERROR_FILE_WRITE_FAILURE;
                }
            }
            else
            {
                input->result=(PatchContext) input->onFileStruct.context.op;
            }
        }
        else
        {
            N_assert(input->onFileStruct.context.op==PC_HASH_1_WITH_PATCH || input->onFileStruct.context.op==PC_HASH_2_WITH_PATCH);

    //		CSHA1 sha1;


            //				printf("apply patch %i bytes\n", byteLengthOfThisFile-SHA1_LENGTH);
            //				for (int i=0; i < byteLengthOfThisFile-SHA1_LENGTH; i++)
            //					printf("%i ", fileData[SHA1_LENGTH+i]);
            //				printf("\n");
            int hashMultiplier;
            if (input->onFileStruct.context.op==PC_HASH_1_WITH_PATCH)
                hashMultiplier=1;
            else
                hashMultiplier=2; // else op==PC_HASH_2_WITH_PATCH

            PatchContext result = input->cbInterface->ApplyPatchBase(fullPathToDir, &input->postPatchFile, &input->postPatchLength, (char*)input->onFileStruct.fileData+HASH_LENGTH*hashMultiplier, input->onFileStruct.byteLengthOfThisFile-HASH_LENGTH*hashMultiplier, input->onFileStruct.context.flnc_extraData2);
            if (result == PC_ERROR_PATCH_APPLICATION_FAILURE || input->result==PC_ERROR_PATCH_TARGET_MISSING)
            {
                input->result = result;
                AutopatcherClientJobResult * re = N_new AutopatcherClientJobResult(job, input);
                return re;
            }

            unsigned int hash = FastHash(input->postPatchFile, input->postPatchLength);
            if(NetSerializer::isFlipData())
                Serializer::mirror((unsigned char*) &hash, sizeof(hash));

            //if (memcmp(sha1.GetHash(), input->onFileStruct.fileData, HASH_LENGTH)!=0)

            if (memcmp(&hash, input->onFileStruct.fileData+HASH_LENGTH*(hashMultiplier-1), HASH_LENGTH)!=0)
            {
                input->result = PC_ERROR_PATCH_RESULT_CHECKSUM_FAILURE;
            }
            else
            {
                // write postPatchFile over the existing file
                if (FileSystem::wirteFile(fullPathToDir, (char*)input->postPatchFile, input->postPatchLength)==false)
                {
                    char newDir[1024];
                    strcpy(newDir, fullPathToDir);
                    strcat(newDir, COPY_ON_RESTART_EXTENSION);
                    if (FileSystem::wirteFile(newDir, (char*)input->postPatchFile, input->postPatchLength))
                    {
                        input->result=PC_NOTICE_WILL_COPY_ON_RESTART;
                    }
                    else
                    {
                        input->result=PC_ERROR_FILE_WRITE_FAILURE;
                    }
                }
                else
                {
                    input->result=(PatchContext)input->onFileStruct.context.op;
                }
            }
        }
        AutopatcherClientJobResult * re = N_new AutopatcherClientJobResult(job, input);
        return re;
    }
    // -----------------------------------------------------------------
    class AutopatcherClientCallback : public FileListTransferCBInterface
    {
    public:
        ThreadManager<AutopatcherClientThreadInfo *, AutopatcherClientThreadInfo *> threadPool;
        char applicationDirectory[512];
        AutopatcherClientCBInterface *onFileCallback;
        AutopatcherClient *client;
        bool downloadComplete;
        bool canDeleteUser;

        AutopatcherClientCallback(void)
        {
            threadPool.startup(1);
            canDeleteUser = false;
            downloadComplete = false;
        }
        virtual ~AutopatcherClientCallback(void)
        {
            StopThreads();
        }
        void StopThreads(void)
        {
            threadPool.shutdown();
            N_assert(threadPool.getWorkingCount() == 0);

            threadPool.removeAllInput();
            threadPool.removeAllOutput();
        }
        // update is run in the user thread
        virtual bool update(void)
        {
            // If both input and output are empty, we are done.
            if (onFileCallback->update() == false)
                canDeleteUser = true;

            if (downloadComplete && canDeleteUser && threadPool.isWorking() == false)
            {
                // Stop threads before calling OnThreadCompletion, in case the other thread starts a new instance of this thread.
                StopThreads();
                client->OnThreadCompletion();
                return false;
            }

            return true;
        }
        //------------------------------------------------------------------------
        virtual bool OnDownloadComplete(DownloadCompleteStruct * dcs)
        {
            downloadComplete=true;
            if (onFileCallback->OnDownloadComplete(dcs) == false)
            {
                canDeleteUser=true;
            }
            return true;
        };
        //------------------------------------------------------------------------
        virtual void OnDereference(void)
        {
            onFileCallback->OnDereference();
            StopThreads();
        }
        //------------------------------------------------------------------------
        virtual bool OnFile(OnFileStruct * onFileStruct)
        {
            AutopatcherClientThreadInfo * inStruct = OP_NEW<AutopatcherClientThreadInfo>();
            memset(inStruct,0,sizeof(AutopatcherClientThreadInfo));
            inStruct->postPatchFile = 0;
            inStruct->cbInterface = onFileCallback;
            memcpy(&(inStruct->onFileStruct), onFileStruct, sizeof(OnFileStruct));
            strcpy(inStruct->applicationDirectory,applicationDirectory);
            if (onFileStruct->context.op == PC_HASH_1_WITH_PATCH || onFileStruct->context.op==PC_HASH_2_WITH_PATCH)
                onFileStruct->context.op = PC_NOTICE_FILE_DOWNLOADED_PATCH;
            else
                onFileStruct->context.op = PC_NOTICE_FILE_DOWNLOADED;
            onFileCallback->OnFile(onFileStruct);
            AutopatcherClientJob * job = N_new AutopatcherClientJob(inStruct);
            AutopatcherClientJobPrc * prc = N_new AutopatcherClientJobPrc();
            threadPool.add(job, prc);

            // Return false means don't delete OnFileStruct::data
            return false;
        }
        virtual void OnFileProgress(FileProgressStruct * fps)
        {
            char fullPathToDir[1024];

            if (fps->onFileStruct->fileName)
            {
                strcpy(fullPathToDir, applicationDirectory);
                strcat(fullPathToDir, fps->onFileStruct->fileName);
                onFileCallback->OnFileProgress(fps);
            }
        }
    };
    //------------------------------------------------------------------------
    AutopatcherClient::AutopatcherClient()
    {
        serverId = 0;
        serverIdIndex=-1;
        applicationDirectory[0]=0;
        fileListTransfer=0;
        mPacketLevel=HIGH_PRIORITY;
        mPacketCh=0;
        serverDate=0;
        userCB=0;
        processThreadCompletion=false;
    }
    //------------------------------------------------------------------------
    AutopatcherClient::~AutopatcherClient()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void AutopatcherClient::Clear(void)
    {
        if(fileListTransfer)
            fileListTransfer->RemoveReceiver(serverId);
        serverId = 0;
        setId = (unsigned short)-1;
        redownloadList.Clear();
        copyAndRestartList.Clear();
    }
    //------------------------------------------------------------------------
    void AutopatcherClient::SetUploadSendParameters(PacketLevel plevel, char pchannel)
    {
        mPacketLevel=plevel;
        mPacketCh=pchannel;
    }
    //------------------------------------------------------------------------
    void AutopatcherClient::SetFileListTransferPlugin(FileListTransfer *flt)
    {
        fileListTransfer=flt;
    }
    //------------------------------------------------------------------------
    NIId AutopatcherClient::GetServerDate(void) const
    {
        return serverDate;
    }
    //------------------------------------------------------------------------
    void AutopatcherClient::CancelDownload(void)
    {
        fileListTransfer->CancelReceive(setId);
        Clear();
    }
    //------------------------------------------------------------------------
    void AutopatcherClient::OnThreadCompletion(void)
    {
        processThreadCompletionMutex.Lock();
        processThreadCompletion=true;
        processThreadCompletionMutex.Unlock();
    }
    //------------------------------------------------------------------------
    bool AutopatcherClient::IsPatching(void) const
    {
        return fileListTransfer->IsHandlerActive(setId);
    }
    //------------------------------------------------------------------------
    bool AutopatcherClient::PatchApplication(const char * _applicationName,
        const char * _applicationDirectory, NIId lastUpdateDate, Address * host,
            AutopatcherClientCBInterface * onFileCallback, const char * restartOutputFilename,
                const char * pathToRestartExe)
    {
        N_assert(applicationName);
        N_assert(applicationDirectory);
        N_assert(pathToRestartExe);
        N_assert(restartOutputFilename);

        if (IsPatching())
            return false; // Already in the middle of patching.

        strcpy(applicationDirectory, _applicationDirectory);
        FileList::FixEndingSlash(applicationDirectory);
        strcpy(applicationName, _applicationName);
        serverId = host;
        patchComplete = false;
        userCB = onFileCallback;
        strcpy(copyOnRestartOut, restartOutputFilename);
        strcpy(restartExe, pathToRestartExe);
        processThreadCompletionMutex.Lock();
        processThreadCompletion = false;
        processThreadCompletionMutex.Unlock();

        NetSerializer out;
        out.write((unsigned char)ID_AUTOPATCHER_GET_CHANGELIST_SINCE_DATE);
        StringCompressor::Instance()->EncodeString(applicationName, 512, &out);
        out.write(lastUpdateDate);
        send(&out, mPacketLevel, RELIABLE_ORDERED, mPacketCh, host, false);
        return true;
    }
    //------------------------------------------------------------------------
    #ifdef _MSC_VER
    #pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
    #endif
    void AutopatcherClient::update()
    {
        processThreadCompletionMutex.Lock();
        if (processThreadCompletion)
        {
            processThreadCompletion = false;
            processThreadCompletionMutex.Unlock();

            fileListTransfer->RemoveReceiver(serverId);

            // If redownload list, process it
            if (redownloadList.fileList.Size())
            {
                NetSerializer out;
                AutopatcherClientCallback * transferCallback;
                transferCallback = OP_NEW<AutopatcherClientCallback>();
                strcpy(transferCallback->applicationDirectory, applicationDirectory);
                transferCallback->onFileCallback = userCB;
                transferCallback->client = this;
                setId = fileListTransfer->SetupReceive(transferCallback, true, serverId);

                // Ask for patches for the files in the list that are different from what we have.
                out.write((unsigned char)ID_AUTOPATCHER_GET_PATCH);
                out.write(setId);
                NIId lastUpdateData=0;
                out.write(lastUpdateData);
                StringCompressor::Instance()->EncodeString(applicationName, 512, &out);
                redownloadList.read(&out);
                send(&out, mPacketLevel, RELIABLE_ORDERED, mPacketCh, serverId, false);
                redownloadList.Clear();
            }
            else if (copyAndRestartList.fileList.Size())
            {
                SocketMessage *p = createMessage(1);
                p->bitSize = p->length*8;
                p->data[0] = ID_AUTOPATCHER_RESTART_APPLICATION;
                p->mAddress = serverId;
                p->mAddress.mIndex=serverIdIndex;
                addMessage(p,false);

                FILE *fp;
                fp = fopen(copyOnRestartOut, "wt");
                N_assert(fp);
                if (fp)
                {
                    fprintf(fp, "#Sleep 1000\n");
                    unsigned i;
                    for (i=0; i < copyAndRestartList.fileList.Size(); i++)
                    {
    #ifdef _WIN32
                        fprintf(fp, "del /q \"%s%s\"\n", applicationDirectory, copyAndRestartList.fileList[i].filename.C_String());
                        fprintf(fp, "rename \"%s%s%s\" \"%s\"\n", applicationDirectory, copyAndRestartList.fileList[i].filename.C_String(), COPY_ON_RESTART_EXTENSION, copyAndRestartList.fileList[i].filename.C_String());
    #else
                        fprintf(fp, "rm -f \"%s%s\"\n", applicationDirectory, copyAndRestartList.fileList[i].filename.C_String());
                        fprintf(fp, "mv \"%s%s%s\" \"%s\"\n", applicationDirectory, copyAndRestartList.fileList[i].filename.C_String(), COPY_ON_RESTART_EXTENSION, copyAndRestartList.fileList[i].filename.C_String());
    #endif
                    }
    #ifdef _WIN32
                    fprintf(fp, "#CreateProcess \"%s\"\n", restartExe);
    #else
                    fprintf(fp, "chmod +x \"%s\"\n", restartExe);
                    fprintf(fp, "#CreateProcess \"%s\" &\n", restartExe);
    #endif
                    fprintf(fp, "#DeleteThisFile\n");
                    fclose(fp);
                }
            }
            else
            {
                SocketMessage * p = createMessage(1);
                p->bitSize = p->length * 8;
                p->data[0] = ID_AUTOPATCHER_FINISHED;
                p->mAddress = serverId;
                p->mAddress.mIndex=serverIdIndex;
                addMessage(p, false);
            }
        }
        else
        {
            processThreadCompletionMutex.Unlock();
        }
    }
    //------------------------------------------------------------------------
    void AutopatcherClient::onConnectClose(const Address & address,
        UniqueID id, ConnectCloseType lostConnectionReason)
    {
        if (serverId && address == *serverId)
            Clear();
    }
    //------------------------------------------------------------------------
    ConnectReceiveType AutopatcherClient::onMessage(SocketMessage * msg)
    {
        switch (msg->data[0])
        {
        case ID_AUTOPATCHER_CREATION_LIST:
            return OnCreationList(msg);
        case ID_AUTOPATCHER_DELETION_LIST:
            OnDeletionList(msg);
            return CRT_Destroy;
        case ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR:
        case ID_AUTOPATCHER_CANNOT_DOWNLOAD_ORIGINAL_UNMODIFIED_FILES:
            fileListTransfer->RemoveReceiver(serverId);
            Clear();
            return CRT_Normal;
        case ID_AUTOPATCHER_FINISHED_INTERNAL:
            return OnDownloadFinishedInternal(msg);
        case ID_AUTOPATCHER_FINISHED:
            return OnDownloadFinished(msg);
        }
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    #ifdef _MSC_VER
    #pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
    #endif
    void AutopatcherClient::shutdown(void)
    {
        // TODO
    }
    //------------------------------------------------------------------------
    ConnectReceiveType AutopatcherClient::OnCreationList(SocketMessage * msg)
    {
        N_assert(fileListTransfer);
        if (serverId && msg->mAddress != *serverId)
            return CRT_Destroy;

        NetSerializer in(msg->data, msg->length, false);
        NetSerializer out;
        FileList remoteFileList, missingOrChanged;
        in.skipRead(1, 0);
        if (remoteFileList.write(&in) == false)
            return CRT_Destroy;

        in.read(serverDate);
        NIId patchApplicationLastUpdateDate;
        in.read(patchApplicationLastUpdateDate);

        // Go through the list of hashes.  For each file we already have, remove it from the list.
        remoteFileList.ListMissingOrChangedFiles(applicationDirectory,
            &missingOrChanged, true, false);

        if (missingOrChanged.fileList.Size() == 0)
        {
            msg->data[0]=ID_AUTOPATCHER_FINISHED;
            return CRT_Normal; // Pass to user
        }

        // Prepare the transfer plugin to get a file list.
        AutopatcherClientCallback * transferCallback;
        transferCallback = OP_NEW<AutopatcherClientCallback>();
        strcpy(transferCallback->applicationDirectory, applicationDirectory);
        transferCallback->onFileCallback = userCB;
        transferCallback->client = this;
        setId = fileListTransfer->SetupReceive(transferCallback, true, msg->mAddress);

        // Ask for patches for the files in the list that are different from what we have.
        out.write((unsigned char)ID_AUTOPATCHER_GET_PATCH);
        out.write(setId);
        out.write(patchApplicationLastUpdateDate);
        StringCompressor::Instance()->EncodeString(applicationName, 512, &out);
        missingOrChanged.read(&out);
        send(&out, mPacketLevel, RELIABLE_ORDERED, mPacketCh,
            msg->mAddress, false);

        return CRT_Destroy; // Absorb this message
    }
    //------------------------------------------------------------------------
    void AutopatcherClient::OnDeletionList(SocketMessage * msg)
    {
        if (serverId && msg->mAddress != *serverId)
            return;

        NetSerializer in(msg->data, msg->length, false);
        in.skipRead(1, 0);
        FileList fileList;
        if (fileList.write(&in)==false)
            return;
        fileList.DeleteFiles(applicationDirectory);
    }
    //------------------------------------------------------------------------
    ConnectReceiveType AutopatcherClient::OnDownloadFinished(SocketMessage * msg)
    {
        NetSerializer in(msg->data, msg->length, false);
        in.skipRead(1, 0);
        // This may have been created internally, with no serverDate written (line 469 or so)
        if (in.getRemainCount() > 7)
        {
            in.read(serverDate);
        }
        serverId = msg->mAddress;
        serverIdIndex = msg->mAddress.mIndex;

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    ConnectReceiveType AutopatcherClient::OnDownloadFinishedInternal(SocketMessage * msg)
    {
        NetSerializer in(msg->data, msg->length, false);
        in.skipRead(1, 0);
        serverId = msg->mAddress;
        serverIdIndex = msg->mAddress.mIndex;
        in.read(serverDate);

        return CRT_Destroy;
    }
    //------------------------------------------------------------------------
    void AutopatcherClient::CopyAndRestart(const char *filePath)
    {
        // We weren't able to write applicationDirectory + filePath so we wrote applicationDirectory + filePath + COPY_ON_RESTART_EXTENSION instead
        copyAndRestartList.AddFile(filePath,filePath, 0, 0, 0, FileListNodeContext(0,0,0,0));
    }
    //------------------------------------------------------------------------
    void AutopatcherClient::Redownload(const char *filePath)
    {
        redownloadList.AddFile(filePath,filePath, 0, 0, 0, FileListNodeContext(0,0,0,0));
    }
    //------------------------------------------------------------------------
}
}