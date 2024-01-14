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

#include "NativeFeatureIncludes.h"
#if _RAKNET_SUPPORT_DirectoryDeltaTransfer==1 && _RAKNET_SUPPORT_FileOperations==1

#include "DirectoryDeltaTransfer.h"
#include "FileList.h"
#include "StringTable.h"
#include "NiiNetLinkBase.h"
#include "FileListTransfer.h"
#include "FileListTransferCBInterface.h"
#include "NiiNetSerializer.h"
#include "NiiNetCommon.h"
#include "NiiVFS.h"
#include "NiiNetBufferReadBase.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    class DDTCallback : public FileListTransferCBInterface
    {
    public:
        Nui32 subdirLen;
        char outputSubdir[512];
        FileListTransferCBInterface *onFileCallback;

        DDTCallback() {}
        virtual ~DDTCallback() {}

        virtual bool OnFile(OnFileStruct *onFileStruct)
        {
            char fullPathToDir[1024];

            if (onFileStruct->fileName && onFileStruct->fileData && subdirLen < strlen(onFileStruct->fileName))
            {
                strcpy(fullPathToDir, outputSubdir);
                strcat(fullPathToDir, onFileStruct->fileName+subdirLen);
                FileSystem::wirteFile(fullPathToDir, (char*)onFileStruct->fileData, (Nui32 ) onFileStruct->byteLengthOfThisFile);
            }
            else
                fullPathToDir[0]=0;

            return onFileCallback->OnFile(onFileStruct);
        }

        virtual void OnFileProgress(FileProgressStruct *fps)
        {
            char fullPathToDir[1024];

            if (fps->onFileStruct->fileName && subdirLen < strlen(fps->onFileStruct->fileName))
            {
                strcpy(fullPathToDir, outputSubdir);
                strcat(fullPathToDir, fps->onFileStruct->fileName+subdirLen);
            }
            else
                fullPathToDir[0]=0;

            onFileCallback->OnFileProgress(fps);
        }
        virtual bool OnDownloadComplete(DownloadCompleteStruct *dcs)
        {
            return onFileCallback->OnDownloadComplete(dcs);
        }
    };
    //------------------------------------------------------------------------
    DirectoryDeltaTransfer::DirectoryDeltaTransfer()
    {
        applicationDirectory[0] = 0;
        fileListTransfer = 0;
        availableUploads = N_new FileList();
        mPacketLevel = PL_HIGH;
        mPacketCh = 0;
        incrementalReadInterface = 0;
    }
    //------------------------------------------------------------------------
    DirectoryDeltaTransfer::~DirectoryDeltaTransfer()
    {
        N_delete availableUploads;
    }
    //------------------------------------------------------------------------
    void DirectoryDeltaTransfer::SetFileListTransferPlugin(FileListTransfer *flt)
    {
        if (fileListTransfer)
        {
            DataStructures::List<FileListProgress*> fileListProgressList;
            fileListTransfer->GetCallbacks(fileListProgressList);
            Nui32 i;
            for (i=0; i < fileListProgressList.Size(); i++)
                availableUploads->RemoveCallback(fileListProgressList[i]);
        }

        fileListTransfer=flt;

        if (flt)
        {
            DataStructures::List<FileListProgress*> fileListProgressList;
            flt->GetCallbacks(fileListProgressList);
            Nui32 i;
            for (i=0; i < fileListProgressList.Size(); i++)
                availableUploads->AddCallback(fileListProgressList[i]);
        }
        else
        {
            availableUploads->ClearCallbacks();
        }
    }
    //------------------------------------------------------------------------
    void DirectoryDeltaTransfer::SetApplicationDirectory(const char *pathToApplication)
    {
        if (pathToApplication==0 || pathToApplication[0]==0)
            applicationDirectory[0]=0;
        else
        {
            strncpy(applicationDirectory, pathToApplication, 510);
            if (applicationDirectory[strlen(applicationDirectory)-1]!='/' && applicationDirectory[strlen(applicationDirectory)-1]!='\\')
                strcat(applicationDirectory, "/");
            applicationDirectory[511]=0;
        }
    }
    //------------------------------------------------------------------------
    void DirectoryDeltaTransfer::SetUploadSendParameters(PacketLevel _plevel,
        char _pchannel)
    {
        mPacketLevel = _plevel;
        mPacketCh = _pchannel;
    }
    //------------------------------------------------------------------------
    void DirectoryDeltaTransfer::AddUploadsFromSubdirectory(const char *subdir)
    {
        availableUploads->AddFilesFromDirectory(applicationDirectory, subdir, true, false, true, FileListNodeContext(0,0,0,0));
    }
    //------------------------------------------------------------------------
    Nui16 DirectoryDeltaTransfer::DownloadFromSubdirectory(FileList & localFiles,
        const char * subdir, const char * outputSubdir, bool prependAppDirToOutputSubdir,
            Address * host, FileListTransferCBInterface * onFileCallback, PacketLevel _plevel,
                char _pchannel, FileListProgress * cb)
    {
        N_assert(host && !host->isInvalid());

        DDTCallback *transferCallback;

        localFiles.AddCallback(cb);

        // Prepare the callback data
        transferCallback = N_new DDTCallback();
        if (subdir && subdir[0])
        {
            transferCallback->subdirLen = (Nui32)strlen(subdir);
            if (subdir[transferCallback->subdirLen-1]!='/' && subdir[transferCallback->subdirLen-1]!='\\')
                transferCallback->subdirLen++;
        }
        else
            transferCallback->subdirLen = 0;
        if (prependAppDirToOutputSubdir)
            strcpy(transferCallback->outputSubdir, applicationDirectory);
        else
            transferCallback->outputSubdir[0] = 0;
        if (outputSubdir)
            strcat(transferCallback->outputSubdir, outputSubdir);
        if (transferCallback->outputSubdir[strlen(transferCallback->outputSubdir)-1]!='/' &&
            transferCallback->outputSubdir[strlen(transferCallback->outputSubdir)-1]!='\\')
            strcat(transferCallback->outputSubdir, "/");
        transferCallback->onFileCallback=onFileCallback;

        // Setup the transfer plugin to get the response to this download request
        Nui16 setId = fileListTransfer->SetupReceive(transferCallback, true, host);

        // send to the host, telling it to process this request
        NetSerializer outBitstream;
        outBitstream.write((Token)ID_DDT_DOWNLOAD_REQUEST);
        outBitstream.write(setId);
        StringCompressor::Instance()->EncodeString(subdir, 256, &outBitstream);
        StringCompressor::Instance()->EncodeString(outputSubdir, 256, &outBitstream);
        localFiles.read(&outBitstream);
        send(&outBitstream, _plevel, PT_ORDER_MAKE_SURE, _pchannel, host, false);

        return setId;
    }
    //------------------------------------------------------------------------
    Nui16 DirectoryDeltaTransfer::DownloadFromSubdirectory(const char * subdir,
        const char * outputSubdir, bool prependAppDirToOutputSubdir, Address * host,
            FileListTransferCBInterface * onFileCallback, PacketLevel _plevel,
                char _pchannel, FileListProgress * cb)
    {
        FileList localFiles;
        // Get a hash of all the files that we already have (if any)
        localFiles.AddFilesFromDirectory(prependAppDirToOutputSubdir ? applicationDirectory : 0, outputSubdir, true, false, true, FileListNodeContext(0,0,0,0));
        return DownloadFromSubdirectory(localFiles, subdir, outputSubdir, prependAppDirToOutputSubdir, host, onFileCallback, _plevel, _pchannel, cb);
    }
    //------------------------------------------------------------------------
    void DirectoryDeltaTransfer::GenerateHashes(FileList &localFiles,
        const char * outputSubdir, bool prependAppDirToOutputSubdir)
    {
        localFiles.AddFilesFromDirectory(prependAppDirToOutputSubdir ? applicationDirectory : 0, outputSubdir, true, false, true, FileListNodeContext(0,0,0,0));
    }
    //------------------------------------------------------------------------
    void DirectoryDeltaTransfer::ClearUploads(void)
    {
        availableUploads->Clear();
    }
    //------------------------------------------------------------------------
    void DirectoryDeltaTransfer::OnDownloadRequest(SocketMessage * msg)
    {
        char subdir[256];
        char remoteSubdir[256];
        NetSerializer in(msg->mData, msg->mLength, false);
        FileList remoteFileHash;
        FileList delta;
        Nui16 setId;
        in.skipRead(1, 0);
        in.read(setId);
        StringCompressor::Instance()->DecodeString(subdir, 256, &in);
        StringCompressor::Instance()->DecodeString(remoteSubdir, 256, &in);
        if (remoteFileHash.import(&in)==false)
        {
    #ifdef _DEBUG
            N_assert(0);
    #endif
            return;
        }

        availableUploads->GetDeltaToCurrent(&remoteFileHash, &delta, subdir, remoteSubdir);
        if (incrementalReadInterface==0)
            delta.PopulateDataFromDisk(applicationDirectory, true, false, true);
        else
            delta.FlagFilesAsReferences();

        // This will call the ddtCallback interface that was passed to FileListTransfer::SetupReceive on the remote system
        fileListTransfer->send(&delta, mPrc2, msg->mAddress, setId,
            mPacketLevel, mPacketCh, incrementalReadInterface, chunkSize);
    }
    //------------------------------------------------------------------------
    ConnectReceiveType DirectoryDeltaTransfer::onMessage(SocketMessage * msg)
    {
        switch (msg->mData[0])
        {
        case ID_DDT_DOWNLOAD_REQUEST:
            OnDownloadRequest(msg);
            return CRT_Destroy;
        }

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    Nui32 DirectoryDeltaTransfer::GetNumberOfFilesForUpload(void) const
    {
        return availableUploads->fileList.Size();
    }
    //------------------------------------------------------------------------
    void DirectoryDeltaTransfer::SetDownloadRequestBufferReadBase(
        BufferReadBase * _incrementalReadInterface, Nui32 _chunkSize)
    {
        incrementalReadInterface=_incrementalReadInterface;
        chunkSize=_chunkSize;
    }
    //------------------------------------------------------------------------
#endif // _RAKNET_SUPPORT_*