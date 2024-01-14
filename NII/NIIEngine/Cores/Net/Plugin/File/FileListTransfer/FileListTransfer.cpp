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
#if _RAKNET_SUPPORT_FileListTransfer == 1 && _RAKNET_SUPPORT_FileOperations == 1

#include "NiiPreProcess.h"
#include "FileListTransfer.h"
#include "DS_HuffmanEncodingTree.h"
#include "FileListTransferCBInterface.h"
#include "StringTable.h"
#include "FileList.h"
#include "DS_Queue.h"
#include "NiiNetCommon.h"
#include "NiiNetCommon.h"
#include "NiiNetLinkBase.h"
#include "NetStats.h"
#include "NiiNetBufferReadBase.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    class SendIRIToAddressJob : public Job
    {
    public:
        SendIRIToAddressJob() : Job(0){}
        
        class SendIRIToAddressIdentifier : public Job::Identifier
        {
        public:
            SendIRIToAddressIdentifier();
            ~SendIRIToAddressIdentifier();
            
            bool isOnly() const { return false; }
            
            Address * mAddress;
        }
        
        bool equal(const Identifier * id)
        {
            const SendIRIToAddressIdentifier * temp = dynamic_cast<SendIRIToAddressIdentifier *>(id);
            if(temp)
            {
                if(mAddress == *temp->mAddress)
                    return true;
            }
            return false;
        }
        
        FileListTransfer * fileListTransfer;
        Address * mAddress;
        Nui16 setId;
    };
    
    class SendIRIToAddressJobPrc : public JobPrc
    {
    public:
        SendIRIToAddressJobPrc() : JobPrc(true){}
        
        JobResult * handle(Job * jop);
    };
    //------------------------------------------------------------------------
    struct FLR_MemoryBlock
    {
        char * flrMemoryBlock;
    };
    //------------------------------------------------------------------------
    struct FileListReceiver
    {
        FileListReceiver();
        ~FileListReceiver();
        FileListTransferCBInterface * downloadHandler;
        Address * allowedSender;
        Nui16 setID;
        Nui32 setCount;
        Nui32 setTotalCompressedTransmissionLength;
        Nui32 setTotalFinalLength;
        Nui32 setTotalDownloadedLength;
        bool gotSetHeader;
        bool deleteDownloadHandler;
        bool isCompress;
        int  filesReceived;
        DataStructures::Map<Nui32, FLR_MemoryBlock> pushedFiles;

        // Notifications
        Nui32 partLength;

    };

    //------------------------------------------------------------------------
    FileListReceiver::FileListReceiver()
    {
        filesReceived=0;
        setTotalDownloadedLength=0;
        partLength=1;
        DataStructures::Map<Nui32, FLR_MemoryBlock>::IMPLEMENT_DEFAULT_COMPARISON();
    }
    //------------------------------------------------------------------------
    FileListReceiver::~FileListReceiver()
    {
        NCount i = 0;
        for (i = 0; i < pushedFiles.Size(); ++i)
            N_free(pushedFiles[i].flrMemoryBlock);
    }
    //------------------------------------------------------------------------
    void FileListTransfer::FileToPushRecipient::DeleteThis()
    {
        for (NCoung j = 0; j < filesToPush.Size(); ++j)
            N_delete filesToPush[j];
        N_delete this;
    }
    //------------------------------------------------------------------------
    void FileListTransfer::FileToPushRecipient::AddRef()
    {
        refCountMutex.Lock();
        ++mRefCount;
        refCountMutex.Unlock();
    }
    //------------------------------------------------------------------------
    void FileListTransfer::FileToPushRecipient::Deref()
    {
        refCountMutex.Lock();
        --mRefCount;
        if (mRefCount==0)
        {
            refCountMutex.Unlock();
            DeleteThis();
            return;
        }
        refCountMutex.Unlock();
    }
    //------------------------------------------------------------------------
    FileListTransfer::FileListTransfer()
    {
        setId=0;
        DataStructures::Map<Nui16, FileListReceiver*>::IMPLEMENT_DEFAULT_COMPARISON();
    }
    //------------------------------------------------------------------------
    FileListTransfer::~FileListTransfer()
    {
        threadPool.shutdown();
        Clear();
    }
    //------------------------------------------------------------------------
    void FileListTransfer::StartIncrementalReadThreads(int numThreads, int level)
    {
        threadPool.startup(numThreads);
    }
    //------------------------------------------------------------------------
    Nui16 FileListTransfer::SetupReceive(FileListTransferCBInterface * handler,
        bool deleteHandler, Address * allowedSender)
    {
        if (mPrc2 && mPrc2->getState(allowedSender) != IS_CONNECTED)
            return (Nui16)-1;
        FileListReceiver * receiver;

        if (fileListReceivers.Has(setId))
        {
            receiver=fileListReceivers.Get(setId);
            receiver->downloadHandler->OnDereference();
            if (receiver->deleteDownloadHandler)
                N_delete receiver->downloadHandler;
            N_delete receiver;
            fileListReceivers.Delete(setId);
        }

        Nui16 oldId;
        receiver = N_new FileListReceiver();
        N_assert(handler);
        receiver->downloadHandler=handler;
        receiver->allowedSender=allowedSender;
        receiver->gotSetHeader=false;
        receiver->deleteDownloadHandler=deleteHandler;
        receiver->setID=setId;
        fileListReceivers.Set(setId, receiver);
        oldId = setId;
        if (++setId == (Nui16)-1)
            setId = 0;
        return oldId;
    }
    //------------------------------------------------------------------------
    void FileListTransfer::send(FileList * fileList, LinkBase * link,
        Address * recipient, Nui16 setID, PacketLevel plevel, char pchannel,
            BufferReadBase * _incrementalReadInterface, Nui32 _chunkSize)
    {
        for (Nui32 flpcIndex=0; flpcIndex < fileListProgressCallbacks.Size(); flpcIndex++)
            fileList->AddCallback(fileListProgressCallbacks[flpcIndex]);

        Nui32 i, totalLength;
        NetSerializer out;
        bool sendReference;
        const char * dataBlocks[2];
        int lengths[2];
        totalLength=0;
        for (i=0; i < fileList->fileList.Size(); i++)
        {
            const FileListNode &fileListNode = fileList->fileList[i];
            totalLength+=fileListNode.dataLengthBytes;
        }

        // write the chunk header, which contains the frequency table, the total number of files, and the total number of bytes
        bool anythingToWrite;
        out.write((Token)ID_FILE_LIST_TRANSFER_HEADER);
        out.write(setID);
        anythingToWrite = fileList->fileList.Size() > 0;
        out.write(anythingToWrite);
        if(anythingToWrite)
        {
            out.writeCompress(fileList->fileList.Size());
            out.writeCompress(totalLength);

            if (link)
                link->send(&out, plevel, PT_ORDER_MAKE_SURE, pchannel, recipient, false);
            else
                send(&out, plevel, PT_ORDER_MAKE_SURE, pchannel, recipient, false);

            DataStructures::Queue<FileToPush *> filesToPush;

            for (i = 0; i < fileList->fileList.Size(); ++i)
            {
                sendReference = fileList->fileList[i].isAReference && _incrementalReadInterface!=0;
                if (sendReference)
                {
                    FileToPush * fileToPush = N_new FileToPush();
                    fileToPush->fileListNode.context=fileList->fileList[i].context;
                    fileToPush->setIndex=i;
                    fileToPush->fileListNode.filename=fileList->fileList[i].filename;
                    fileToPush->fileListNode.fullPathToFile=fileList->fileList[i].fullPathToFile;
                    fileToPush->fileListNode.fileLengthBytes=fileList->fileList[i].fileLengthBytes;
                    fileToPush->fileListNode.dataLengthBytes=fileList->fileList[i].dataLengthBytes;
                    fileToPush->packetPriority = plevel;
                    fileToPush->mPacketCh = pchannel;
                    fileToPush->currentOffset = 0;
                    fileToPush->incrementalReadInterface = _incrementalReadInterface;
                    fileToPush->chunkSize = _chunkSize;
                    filesToPush.Push(fileToPush);
                }
                else
                {
                    out.seekRead(0);
                    out.seekWrite(0);
                    out.write((Token)ID_FILE_LIST_TRANSFER_FILE);
                    out << fileList->fileList[i].context;
                    out.write(setID);
                    StringCompressor::Instance()->EncodeString(fileList->fileList[i].filename, 512, &out);

                    out.writeCompress(i);
                    out.writeCompress(fileList->fileList[i].dataLengthBytes); // Original length in bytes

                    out.writeAlign();

                    dataBlocks[0] = (char *)out.getData();
                    lengths[0] = out.getSize();
                    dataBlocks[1] = fileList->fileList[i].mData;
                    lengths[1] = fileList->fileList[i].dataLengthBytes;
                    send(dataBlocks, lengths, 2, plevel, PT_ORDER_MAKE_SURE,
                        pchannel, recipient, false);
                }
            }

            if (filesToPush.IsEmpty() == false)
            {
                FileToPushRecipient * ftpr;

                fileToPushRecipientListMutex.Lock();
                for (NCount i = 0; i < fileToPushRecipientList.Size(); ++i)
                {
                    if (fileToPushRecipientList[i]->mAddress == recipient &&
                        fileToPushRecipientList[i]->setId == setId)
                    {
                        N_assert("setId already in use for this recipient" && 0);
                    }
                }
                fileToPushRecipientListMutex.Unlock();

                ftpr = N_new FileToPushRecipient();
                ftpr->mAddress = recipient;
                ftpr->setId = setID;
                ftpr->mRefCount = 2; // Allocated and in the list
                fileToPushRecipientList.Push(ftpr);

                while (filesToPush.IsEmpty() == false)
                {
                    ftpr->filesToPush.Push(filesToPush.Pop());
                }
                // ftpr out of scope
                ftpr->Deref();
                SendIRIToAddress(recipient, setID);
                return;
            }
            else
            {
                for (Nui32 flpcIndex=0; flpcIndex < fileListProgressCallbacks.Size(); flpcIndex++)
                    fileListProgressCallbacks[flpcIndex]->OnFilePushesComplete(recipient, setID);
            }
        }
        else
        {
            for (Nui32 flpcIndex = 0; flpcIndex < fileListProgressCallbacks.Size(); flpcIndex++)
                fileListProgressCallbacks[flpcIndex]->OnFilePushesComplete(recipient, setID);

            if (link)
                link->send(&out, plevel, PT_ORDER_MAKE_SURE, pchannel,
                    recipient, false);
            else
                send(&out, plevel, PT_ORDER_MAKE_SURE, pchannel,
                    recipient, false);
        }
    }
    //------------------------------------------------------------------------
    bool FileListTransfer::DecodeSetHeader(SocketMessage * msg)
    {
        bool anythingToWrite=false;
        Nui16 setID;
        NetSerializer in(msg->mData, msg->mLength, false);
        in.skipRead(1, 0);
        in.read(setID);
        FileListReceiver *fileListReceiver;
        if (fileListReceivers.Has(setID)==false)
        {
            // If this N_assert hits you didn't call SetupReceive
    #ifdef _DEBUG
            N_assert(0);
    #endif
            return false;
        }
        fileListReceiver=fileListReceivers.Get(setID);
        if (fileListReceiver->allowedSender != msg->mAddress)
        {
    #ifdef _DEBUG
            N_assert(0);
    #endif
            return false;
        }

    #ifdef _DEBUG
        N_assert(fileListReceiver->gotSetHeader==false);
    #endif

        in.read(anythingToWrite);

        if (anythingToWrite)
        {
            in.readCompress(fileListReceiver->setCount);
            if (in.readCompress(fileListReceiver->setTotalFinalLength))
            {
                fileListReceiver->setTotalCompressedTransmissionLength=fileListReceiver->setTotalFinalLength;
                fileListReceiver->gotSetHeader=true;
                return true;
            }
        }
        else
        {
            FileListTransferCBInterface::DownloadCompleteStruct dcs;
            dcs.setID = fileListReceiver->setID;
            dcs.numberOfFilesInThisSet=fileListReceiver->setCount;
            dcs.byteLengthOfThisSet = fileListReceiver->setTotalFinalLength;
            dcs.senderSystemAddress = msg->mAddress;
            dcs.senderGuid = msg->mID;

            if (fileListReceiver->downloadHandler->OnDownloadComplete(&dcs)==false)
            {
                fileListReceiver->downloadHandler->OnDereference();
                fileListReceivers.Delete(setID);
                if (fileListReceiver->deleteDownloadHandler)
                    N_delete fileListReceiver->downloadHandler;
                N_delete fileListReceiver;
            }
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool FileListTransfer::DecodeFile(SocketMessage * msg, bool isTheFullFile)
    {
        FileListTransferCBInterface::OnFileStruct onFileStruct;
        NetSerializer in(msg->mData, msg->mLength, false);
        in.skipRead(1, 0);
        onFileStruct.senderSystemAddress = msg->mAddress;
        onFileStruct.senderGuid = msg->mID;

        Nui32 partCount = 0;
        Nui32 partTotal = 0;
        Nui32 partLength = 0;
        onFileStruct.fileData = 0;
        if(isTheFullFile == false)
        {
            // Disable endian swapping on reading this, as it's generated locally in PacketPrc.cpp
            in.readBit((Nui8 *)&partCount, N_b_TO_B(sizeof(partCount)), true);
            in.readBit((Nui8 *)&partTotal, N_b_TO_B(sizeof(partTotal)), true);
            in.readBit((Nui8 *)&partLength, N_b_TO_B(sizeof(partLength)), true);
            in.skipRead(1, 0);
            // The header is appended to every chunk, which we continue to read after this statement flrMemoryBlock
        }
        in >> onFileStruct.context;
        in.read(onFileStruct.setID);
        FileListReceiver * fileListReceiver;
        if (fileListReceivers.Has(onFileStruct.setID) == false)
        {
            return false;
        }
        fileListReceiver = fileListReceivers.Get(onFileStruct.setID);
        if (fileListReceiver->allowedSender != msg->mAddress)
        {
    #ifdef _DEBUG
            N_assert(0);
    #endif
            return false;
        }

    #ifdef _DEBUG
        N_assert(fileListReceiver->gotSetHeader == true);
    #endif

        if (StringCompressor::Instance()->DecodeString(onFileStruct.fileName, 512, &in)==false)
        {
    #ifdef _DEBUG
            N_assert(0);
    #endif
            return false;
        }

        in.readCompress(onFileStruct.fileIndex);
        in.readCompress(onFileStruct.byteLengthOfThisFile);

        onFileStruct.numberOfFilesInThisSet=fileListReceiver->setCount;
        onFileStruct.byteLengthOfThisSet=fileListReceiver->setTotalFinalLength;

        if (isTheFullFile)
        {
            onFileStruct.bytesDownloadedForThisFile=onFileStruct.byteLengthOfThisFile;
            fileListReceiver->setTotalDownloadedLength+=onFileStruct.byteLengthOfThisFile;
            onFileStruct.bytesDownloadedForThisSet=fileListReceiver->setTotalDownloadedLength;
        }
        else
        {
            onFileStruct.bytesDownloadedForThisFile=partLength*partCount;
            onFileStruct.bytesDownloadedForThisSet=fileListReceiver->setTotalDownloadedLength+onFileStruct.bytesDownloadedForThisFile;
        }

        // User callback for this file.
        if (isTheFullFile)
        {
            in.readAlign();
            onFileStruct.fileData = (char *)N_alloc((size_t) onFileStruct.byteLengthOfThisFile);
            in.read((char *)onFileStruct.fileData, onFileStruct.byteLengthOfThisFile);

            FileListTransferCBInterface::FileProgressStruct fps;
            fps.onFileStruct = &onFileStruct;
            fps.partCount = 1;
            fps.partTotal = 1;
            fps.dataChunkLength = onFileStruct.byteLengthOfThisFile;
            fps.firstDataChunk = onFileStruct.fileData;
            fps.iriDataChunk = onFileStruct.fileData;
            fps.allocateIrIDataChunkAutomatically = true;
            fps.iriWriteOffset = 0;
            fps.senderSystemAddress = msg->mAddress;
            fps.senderGuid = msg->mID;
            fileListReceiver->downloadHandler->OnFileProgress(&fps);

            // Got a complete file
            // Either we are using BufferReadBase and it was a small file or
            // We are not using BufferReadBase
            if (fileListReceiver->downloadHandler->OnFile(&onFileStruct))
                N_free(onFileStruct.fileData);

            ++fileListReceiver->filesReceived;

            // If this set is done, free the memory for it.
            if ((int) fileListReceiver->setCount == fileListReceiver->filesReceived)
            {
                FileListTransferCBInterface::DownloadCompleteStruct dcs;
                dcs.setID = fileListReceiver->setID;
                dcs.numberOfFilesInThisSet = fileListReceiver->setCount;
                dcs.byteLengthOfThisSet = fileListReceiver->setTotalFinalLength;
                dcs.senderSystemAddress = msg->mAddress;
                dcs.senderGuid = msg->mID;

                if (fileListReceiver->downloadHandler->OnDownloadComplete(&dcs)==false)
                {
                    fileListReceiver->downloadHandler->OnDereference();
                    if (fileListReceiver->deleteDownloadHandler)
                        N_delete fileListReceiver->downloadHandler;
                    fileListReceivers.Delete(onFileStruct.setID);
                    N_delete fileListReceiver;
                }
            }
        }
        else
        {
            in.readAlign();

            char * firstDataChunk;
            Nui32 unreadBits = in.getRemainCount();
            Nui32 unreadBytes = N_B_TO_b(unreadBits);
            firstDataChunk = (char *)in.getData() + N_B_TO_b(in.getReadOffset());

            FileListTransferCBInterface::FileProgressStruct fps;
            fps.onFileStruct = &onFileStruct;
            fps.partCount = partCount;
            fps.partTotal = partTotal;
            fps.dataChunkLength = unreadBytes;
            fps.firstDataChunk = firstDataChunk;
            fps.iriDataChunk = 0;
            fps.allocateIrIDataChunkAutomatically = true;
            fps.iriWriteOffset=0;
            fps.senderSystemAddress = msg->mAddress;
            fps.senderGuid = msg->mID;

            // Remote system is sending a complete file, but the file is large enough that we get ID_PROGRESS_NOTIFICATION from the transport layer
            fileListReceiver->downloadHandler->OnFileProgress(&fps);
        }

        return true;
    }
    //------------------------------------------------------------------------
    ConnectReceiveType FileListTransfer::onMessage(SocketMessage * msg)
    {
        switch (msg->mData[0])
        {
        case ID_FILE_LIST_TRANSFER_HEADER:
            DecodeSetHeader(msg);
            return CRT_Destroy;
        case ID_FILE_LIST_TRANSFER_FILE:
            DecodeFile(msg, true);
            return CRT_Destroy;
        case ID_FILE_LIST_REFERENCE_PUSH:
            OnReferencePush(msg, true);
            return CRT_Destroy;
        case ID_FILE_LIST_REFERENCE_PUSH_ACK:
            OnReferencePushAck(msg);
            return CRT_Destroy;
        case ID_DOWNLOAD_PROGRESS:
            if (msg->mLength>sizeof(Token)+sizeof(Nui32)*3)
            {
                if (msg->mData[sizeof(Token)+sizeof(Nui32)*3]==ID_FILE_LIST_TRANSFER_FILE)
                {
                    DecodeFile(msg, false);
                    return CRT_Destroy;
                }
                if (msg->mData[sizeof(Token)+sizeof(Nui32)*3]==ID_FILE_LIST_REFERENCE_PUSH)
                {
                    OnReferencePush(msg, false);
                    return CRT_Destroy;
                }
            }
            break;
        }

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void FileListTransfer::shutdown()
    {
        threadPool.shutdown();
        threadPool.removeAllInput();
        Clear();
    }
    //------------------------------------------------------------------------
    void FileListTransfer::Clear()
    {
        Nui32 i;
        for (i=0; i < fileListReceivers.Size(); i++)
        {
            fileListReceivers[i]->downloadHandler->OnDereference();
            if (fileListReceivers[i]->deleteDownloadHandler)
                N_delete fileListReceivers[i]->downloadHandler;
            N_delete fileListReceivers[i];
        }
        fileListReceivers.Clear();

        fileToPushRecipientListMutex.Lock();
        for (Nui32 i=0; i < fileToPushRecipientList.Size(); i++)
        {
            FileToPushRecipient *ftpr = fileToPushRecipientList[i];
            // Taken out of the list
            ftpr->Deref();
        }
        fileToPushRecipientList.Clear(false);
        fileToPushRecipientListMutex.Unlock();
    }
    //------------------------------------------------------------------------
    void FileListTransfer::onConnectClose(const Address & address, UniqueID id, 
        ConnectCloseType lostConnectionReason)
    {
        (void) lostConnectionReason;
        (void) id;

        RemoveReceiver(address);
    }
    //------------------------------------------------------------------------
    void FileListTransfer::CancelReceive(Nui16 setId)
    {
        if (fileListReceivers.Has(setId)==false)
        {
    #ifdef _DEBUG
            N_assert(0);
    #endif
            return;
        }
        FileListReceiver *fileListReceiver=fileListReceivers.Get(setId);
        fileListReceiver->downloadHandler->OnDereference();
        if (fileListReceiver->deleteDownloadHandler)
            N_delete fileListReceiver->downloadHandler;
        N_delete fileListReceiver;
        fileListReceivers.Delete(setId);
    }
    //------------------------------------------------------------------------
    void FileListTransfer::RemoveReceiver(Address * address)
    {
        SendIRIToAddressIdentifier temp(address);
        threadPool.remove(temp);
        Nui32 i = 0;
        while (i < fileListReceivers.Size())
        {
            if (fileListReceivers[i]->allowedSender == address)
            {
                fileListReceivers[i]->downloadHandler->OnDereference();
                if (fileListReceivers[i]->deleteDownloadHandler)
                    N_delete fileListReceivers[i]->downloadHandler;
                N_delete fileListReceivers[i];
                fileListReceivers.RemoveAtIndex(i);
            }
            else
                i++;
        }

        fileToPushRecipientListMutex.Lock();
        i = 0;
        while (i < fileToPushRecipientList.Size())
        {
            if (fileToPushRecipientList[i]->mAddress == address)
            {
                FileToPushRecipient *ftpr = fileToPushRecipientList[i];

                // Tell the user that this recipient was lost
                for (Nui32 flpcIndex=0; flpcIndex < fileListProgressCallbacks.Size(); flpcIndex++)
                    fileListProgressCallbacks[flpcIndex]->OnSendAborted(ftpr->mAddress);

                fileToPushRecipientList.RemoveAtIndex(i);
                // Taken out of the list
                ftpr->Deref();
            }
            else
            {
                i++;
            }
        }
        fileToPushRecipientListMutex.Unlock();
    }
    //------------------------------------------------------------------------
    bool FileListTransfer::IsHandlerActive(Nui16 setId)
    {
        return fileListReceivers.Has(setId);
    }
    //------------------------------------------------------------------------
    void FileListTransfer::AddCallback(FileListProgress * cb)
    {
        if(cb == 0)
            return;

        if(fileListProgressCallbacks.GetIndexOf(cb)==(Nui32) -1)
            fileListProgressCallbacks.Push(cb);
    }
    //------------------------------------------------------------------------
    void FileListTransfer::RemoveCallback(FileListProgress * cb)
    {
        Nui32 idx = fileListProgressCallbacks.GetIndexOf(cb);
        if (idx!=(Nui32) -1)
            fileListProgressCallbacks.RemoveAtIndex(idx);
    }
    //------------------------------------------------------------------------
    void FileListTransfer::ClearCallbacks(void)
    {
        fileListProgressCallbacks.Clear(true);
    }
    //------------------------------------------------------------------------
    void FileListTransfer::GetCallbacks(DataStructures::List<FileListProgress*> &callbacks)
    {
        callbacks = fileListProgressCallbacks;
    }
    //------------------------------------------------------------------------
    void FileListTransfer::update()
    {
        Nui32 i;
        i=0;
        while (i < fileListReceivers.Size())
        {
            if (fileListReceivers[i]->downloadHandler->update()==false)
            {
                fileListReceivers[i]->downloadHandler->OnDereference();
                if (fileListReceivers[i]->deleteDownloadHandler)
                    N_delete fileListReceivers[i]->downloadHandler;
                N_delete fileListReceivers[i];
                fileListReceivers.RemoveAtIndex(i);
            }
            else
                i++;
        }
    }
    //------------------------------------------------------------------------
    void FileListTransfer::OnReferencePush(SocketMessage * msg, bool isTheFullFile)
    {
        NetSerializer refPushAck;
        if (isTheFullFile == false)
        {
            // 12/23/09 Why do I care about ID_DOWNLOAD_PROGRESS for reference pushes?
            // 2/16/2012 I care because a reference push is 16 megabytes by default. Also, if it is the last file "if (ftpr->filesToPush.Size()<2)" or total file size exceeds smallFileTotalSize it always sends a reference push.
    //        return;
        }

        FileListTransferCBInterface::OnFileStruct onFileStruct;
        NetSerializer in(msg->mData, msg->mLength, false);
        in.skipRead(1, 0);
        Nui32 partCount = 0;
        Nui32 partTotal = 1;
        Nui32 partLength = 0;
        onFileStruct.fileData = 0;
        if(isTheFullFile == false)
        {
            // Disable endian swapping on reading this, as it's generated locally in PacketPrc.cpp
            in.readBit((Nui8 *)&partCount, N_b_TO_B(sizeof(partCount)), true);
            in.readBit((Nui8 *)&partTotal, N_b_TO_B(sizeof(partTotal)), true);
            in.readBit((Nui8 *)&partLength, N_b_TO_B(sizeof(partLength)), true);
            in.skipRead(1, 0);
            // The header is appended to every chunk, which we continue to read after this statement flrMemoryBlock
        }
        in >> onFileStruct.context;
        in.read(onFileStruct.setID);
        // This is not a progress notification, it is actually the entire msg
        if (isTheFullFile == true)
        {
            refPushAck.write((Token)ID_FILE_LIST_REFERENCE_PUSH_ACK);
            refPushAck.write(onFileStruct.setID);
            send(&refPushAck, PL_HIGH, PT_BE_SURE, 0, msg->mAddress, false);
        }

        FileListReceiver * fileListReceiver;
        if (fileListReceivers.Has(onFileStruct.setID)==false)
        {
            return;
        }
        fileListReceiver=fileListReceivers.Get(onFileStruct.setID);
        if (fileListReceiver->allowedSender != msg->mAddress)
        {
    #ifdef _DEBUG
            N_assert(0);
    #endif
            return;
        }

    #ifdef _DEBUG
        N_assert(fileListReceiver->gotSetHeader==true);
    #endif

        if (StringCompressor::Instance()->DecodeString(onFileStruct.fileName, 512, &in)==false)
        {
    #ifdef _DEBUG
            N_assert(0);
    #endif
            return;
        }

        in.readCompress(onFileStruct.fileIndex);
        in.readCompress(onFileStruct.byteLengthOfThisFile);
        Nui32 offset;
        Nui32 chunkLength;
        in.readCompress(offset);
        in.readCompress(chunkLength);

        bool lastChunk=false;
        in.read(lastChunk);
        bool finished = lastChunk && isTheFullFile;

        if (isTheFullFile==false)
            fileListReceiver->partLength=partLength;

        FLR_MemoryBlock mb;
        if (fileListReceiver->pushedFiles.Has(onFileStruct.fileIndex)==false)
        {
            mb.flrMemoryBlock=(char*) N_alloc(onFileStruct.byteLengthOfThisFile);
            fileListReceiver->pushedFiles.SetNew(onFileStruct.fileIndex, mb);
        }
        else
        {
            mb=fileListReceiver->pushedFiles.Get(onFileStruct.fileIndex);
        }

        Nui32 unreadBits = in.getRemainCount();
        Nui32 unreadBytes = N_B_TO_b(unreadBits);
        Nui32 amountToRead;
        if (isTheFullFile)
            amountToRead=chunkLength;
        else
            amountToRead=unreadBytes;

        in.readAlign();

        FileListTransferCBInterface::FileProgressStruct fps;

        if (isTheFullFile)
        {
            if (mb.flrMemoryBlock)
            {
                // Either the very first block, or a subsequent block and allocateIrIDataChunkAutomatically was true for the first block
                memcpy(mb.flrMemoryBlock+offset, in.getData()+N_B_TO_b(in.getReadOffset()), amountToRead);
                fps.iriDataChunk=mb.flrMemoryBlock+offset;
            }
            else
            {
                // In here mb.flrMemoryBlock is null
                // This means the first block explicitly deallocated the memory, and no blocks will be permanently held by RakNet
                fps.iriDataChunk = (char *)in.getData()+N_B_TO_b(in.getReadOffset());
            }

            onFileStruct.bytesDownloadedForThisFile=offset+chunkLength;
            fileListReceiver->setTotalDownloadedLength+=chunkLength;
            onFileStruct.bytesDownloadedForThisSet=fileListReceiver->setTotalDownloadedLength;
        }
        else
        {
            onFileStruct.bytesDownloadedForThisFile=offset+partLength*partCount;
            onFileStruct.bytesDownloadedForThisSet=fileListReceiver->setTotalDownloadedLength+partCount*partLength;
            fps.iriDataChunk = (char *)in.getData()+N_B_TO_b(in.getReadOffset());
        }

        onFileStruct.numberOfFilesInThisSet=fileListReceiver->setCount;
        onFileStruct.byteLengthOfThisSet=fileListReceiver->setTotalFinalLength;
        // Note: mb.flrMemoryBlock may be null here
        onFileStruct.fileData = mb.flrMemoryBlock;
        onFileStruct.senderSystemAddress = msg->mAddress;
        onFileStruct.senderGuid = msg->mID;

        Nui32 totalNotifications;
        Nui32 currentNotificationIndex;
        if (chunkLength == 0 || chunkLength==onFileStruct.byteLengthOfThisFile)
            totalNotifications=1;
        else
            totalNotifications = onFileStruct.byteLengthOfThisFile / chunkLength + 1;

        if (chunkLength == 0)
            currentNotificationIndex = 0;
        else
            currentNotificationIndex = offset / chunkLength;

        fps.onFileStruct = &onFileStruct;
        fps.partCount=currentNotificationIndex;
        fps.partTotal=totalNotifications;
        fps.dataChunkLength=amountToRead;
        fps.firstDataChunk=mb.flrMemoryBlock;
        fps.allocateIrIDataChunkAutomatically=true;
        fps.onFileStruct->fileData=mb.flrMemoryBlock;
        fps.iriWriteOffset=offset;
        fps.senderSystemAddress = msg->mAddress;
        fps.senderGuid = msg->mID;

        if (finished)
        {
            char * oldFileData = fps.onFileStruct->fileData;
            if (fps.partCount == 0)
                fps.firstDataChunk=fps.iriDataChunk;
            if (fps.partTotal == 1)
                fps.onFileStruct->fileData=fps.iriDataChunk;
            fileListReceiver->downloadHandler->OnFileProgress(&fps);

            // Incremental read interface sent us a file chunk
            // This is the last file chunk we were waiting for to consider the file done
            if (fileListReceiver->downloadHandler->OnFile(&onFileStruct))
                N_free(oldFileData);
            fileListReceiver->pushedFiles.Delete(onFileStruct.fileIndex);

            ++fileListReceiver->filesReceived;

            // If this set is done, free the memory for it.
            if ((int) fileListReceiver->setCount==fileListReceiver->filesReceived)
            {
                FileListTransferCBInterface::DownloadCompleteStruct dcs;
                dcs.setID = fileListReceiver->setID;
                dcs.numberOfFilesInThisSet = fileListReceiver->setCount;
                dcs.byteLengthOfThisSet = fileListReceiver->setTotalFinalLength;
                dcs.senderSystemAddress = msg->mAddress;
                dcs.senderGuid = msg->mID;

                if (fileListReceiver->downloadHandler->OnDownloadComplete(&dcs)==false)
                {
                    fileListReceiver->downloadHandler->OnDereference();
                    fileListReceivers.Delete(onFileStruct.setID);
                    if (fileListReceiver->deleteDownloadHandler)
                        N_delete fileListReceiver->downloadHandler;
                    N_delete fileListReceiver;
                }
            }
        }
        else
        {
            if (isTheFullFile)
            {
                // 12/23/09 Don't use OnReferencePush anymore, just use OnFileProgress
                fileListReceiver->downloadHandler->OnFileProgress(&fps);

                if (fps.allocateIrIDataChunkAutomatically==false)
                {
                    N_free(fileListReceiver->pushedFiles.Get(onFileStruct.fileIndex).flrMemoryBlock);
                    fileListReceiver->pushedFiles.Get(onFileStruct.fileIndex).flrMemoryBlock=0;
                }
            }
            else
            {
                // This is a download progress notification for a file chunk using incremental read interface
                // We don't have all the data for this chunk yet

                totalNotifications = onFileStruct.byteLengthOfThisFile / fileListReceiver->partLength + 1;
                if (isTheFullFile == false)
                    currentNotificationIndex = (offset+partCount*fileListReceiver->partLength) / fileListReceiver->partLength ;
                else
                    currentNotificationIndex = (offset+chunkLength) / fileListReceiver->partLength ;
                unreadBytes = onFileStruct.byteLengthOfThisFile - ((currentNotificationIndex+1) * fileListReceiver->partLength);
                fps.partCount = currentNotificationIndex;
                fps.partTotal = totalNotifications;

                {
                    // Thus chunk is incomplete
                    fps.iriDataChunk = 0;

                    fileListReceiver->downloadHandler->OnFileProgress(&fps);
                }
            }
        }

        return;
    }
    //------------------------------------------------------------------------
    /*
    SendIRIToAddress - executes from send(). =
    1, Find the recipient to send for
    2. send ID_FILE_LIST_TRANSFER_FILE for each small file in the queue of ifles to be sent
    3. If the file we are working on is done, remove it from the list
    4. send ID_FILE_LIST_REFERENCE_PUSH for the file we are working on

    File sender:
    ID_FILE_LIST_REFERENCE_PUSH sent from end of SendIRIToAddressCB

    Recipient:
    send ID_FILE_LIST_REFERENCE_PUSH_ACK sent from OnReferencePush() when 2nd parameter is true.

    File sender:
    Got ID_FILE_LIST_REFERENCE_PUSH_ACK. Calls OnReferencePushAck, calls SendIRIToAddress, calls SendIRIToAddressCB
    */
    JobResult * SendIRIToAddressJobPrc::handle(Job * jop)
    {
        SendIRIToAddressJob * threadData = static_cast<SendIRIToAddressJob *>(jop);
        FileListTransfer * fileListTransfer = threadData->fileListTransfer;
        Address * address = threadData->mAddress;
        Nui16 setId = threadData->setId;

        // Was previously using getReport to get outgoing buffer size, but TCP with UnifiedSend doesn't have this
        Nui32 bytesRead;
        const char * dataBlocks[2];
        int lengths[2];
        Nui32 smallFileTotalSize = 0;
        NetSerializer out;
        Nui32 ftpIndex;

        fileListTransfer->fileToPushRecipientListMutex.Lock();
        for(ftpIndex = 0; ftpIndex < fileListTransfer->fileToPushRecipientList.Size(); ftpIndex++)
        {
            FileListTransfer::FileToPushRecipient * ftpr = fileListTransfer->fileToPushRecipientList[ftpIndex];
            // Referenced by both ftpr and list
            ftpr->AddRef();

            fileListTransfer->fileToPushRecipientListMutex.Unlock();

            if (ftpr->mAddress == address && ftpr->setId == setId)
            {
                FileListTransfer::FileToPush * ftp;
                ftp = ftpr->filesToPush.Pop();

                // read and send chunk. If done, delete at this index
                void * buff = N_alloc(ftp->chunkSize);
                if(buff == 0)
                {
                    ftpr->filesToPush.PushAtHead(ftp, 0);
                    ftpr->Deref();
                    return 0;
                }

                // read the next file chunk
                bytesRead = ftp->incrementalReadInterface->GetFilePart(ftp->fileListNode.fullPathToFile, ftp->currentOffset, ftp->chunkSize, buff, ftp->fileListNode.context);

                bool done = ftp->fileListNode.dataLengthBytes == ftp->currentOffset + bytesRead;
                while(done && ftp->currentOffset == 0 && smallFileTotalSize < ftp->chunkSize)
                {
                    ////ftpr->filesToPushMutex.Lock();
                    // The reason for 2 is that ID_FILE_LIST_REFERENCE_PUSH gets ID_FILE_LIST_REFERENCE_PUSH_ACK. WIthout ID_FILE_LIST_REFERENCE_PUSH_ACK, SendIRIToAddressCB would not be called again
                    if(ftpr->filesToPush.Size() < 2)
                    {
                        break;
                    }

                    // send all small files at once, rather than wait for ID_FILE_LIST_REFERENCE_PUSH. But at least one ID_FILE_LIST_REFERENCE_PUSH must be sent
                    out.seekRead(0);
                    out.seekWrite(0);
                    out.write((Token)ID_FILE_LIST_TRANSFER_FILE);
                    out << ftp->fileListNode.context;
                    out.write(setId);
                    StringCompressor::Instance()->EncodeString(ftp->fileListNode.filename, 512, &out);
                    out.writeCompress(ftp->setIndex);
                    out.writeCompress(ftp->fileListNode.dataLengthBytes); // Original length in bytes
                    out.writeAlign();
                    dataBlocks[0] = (char *)out.getData();
                    lengths[0] = out.getSize();
                    dataBlocks[1] = (const char *)buff;
                    lengths[1] = bytesRead;

                    fileListTransfer->send(dataBlocks, lengths, 2, ftp->packetPriority, PT_ORDER_MAKE_SURE, ftp->mPacketCh, address, false);

                    // LWS : fixed freed pointer reference
                    N_delete ftp;
                    smallFileTotalSize += bytesRead;
                    ftp = ftpr->filesToPush.Pop();
                    bytesRead = ftp->incrementalReadInterface->GetFilePart(ftp->fileListNode.fullPathToFile, ftp->currentOffset, ftp->chunkSize, buff, ftp->fileListNode.context);
                    done = ftp->fileListNode.dataLengthBytes == ftp->currentOffset + bytesRead;
                }

                out.seekRead(0);
                out.seekWrite(0);
                out.write((Token)ID_FILE_LIST_REFERENCE_PUSH);
                out << ftp->fileListNode.context;
                out.write(setId);
                StringCompressor::Instance()->EncodeString(ftp->fileListNode.filename, 512, &out);
                out.writeCompress(ftp->setIndex);
                out.writeCompress(ftp->fileListNode.dataLengthBytes); // Original length in bytes
                out.writeCompress(ftp->currentOffset);
                ftp->currentOffset += bytesRead;
                out.writeCompress(bytesRead);
                out.write(done);

                for(Nui32 flpcIndex = 0; flpcIndex < fileListTransfer->fileListProgressCallbacks.Size(); flpcIndex++)
                    fileListTransfer->fileListProgressCallbacks[flpcIndex]->OnFilePush(ftp->fileListNode.filename,
                        ftp->fileListNode.fileLengthBytes, ftp->currentOffset - bytesRead,
                            bytesRead, done, address, setId);

                dataBlocks[0] = (char *)out.getData();
                lengths[0] = out.getSize();
                dataBlocks[1] = (char *)buff;
                lengths[1] = bytesRead;
                char pchannel = ftp->mPacketCh;
                PacketLevel packetPriority = ftp->packetPriority;

                // Mutex state: FileToPushRecipient (ftpr) has AddRef. fileToPushRecipientListMutex not locked.
                if(done)
                {
                    // Done
                    N_delete ftp;
                    if(ftpr->filesToPush.Size() == 0)
                    {
                        for(Nui32 flpcIndex = 0; flpcIndex < fileListTransfer->fileListProgressCallbacks.Size(); flpcIndex++)
                            fileListTransfer->fileListProgressCallbacks[flpcIndex]->OnFilePushesComplete(address, setId);

                        // Remove ftpr from fileToPushRecipientList
                        fileListTransfer->RemoveFromList(ftpr);
                    }
                }
                else
                {
                    ftpr->filesToPush.PushAtHead(ftp, 0);
                }
                // ftpr out of scope
                ftpr->Deref();

                // 2/12/2012 Moved this line at after the if (done) block above.
                // See http://www.jenkinssoftware.com/forum/index.php?topic=4768.msg19738#msg19738
                fileListTransfer->send(dataBlocks, lengths, 2, packetPriority,
                    PT_ORDER_MAKE_SURE, pchannel, address, false);

                N_free(buff);
                return 0;
            }
            else
            {
                ftpr->Deref();
                fileListTransfer->fileToPushRecipientListMutex.Lock();
            }
        }

        fileListTransfer->fileToPushRecipientListMutex.Unlock();

        return 0;
    }
    //------------------------------------------------------------------------
    void FileListTransfer::SendIRIToAddress(Address * address, Nui16 setId)
    {
        SendIRIToAddressJobPrc * prc = N_new SendIRIToAddressJobPrc();
        SendIRIToAddressJob * job = N_new SendIRIToAddressJob();
        job->fileListTransfer = this;
        job->mAddress = address;
        job->setId = setId;

        if (threadPool.isRun())
        {
            threadPool.add(job, prc, false);
        }
        else
        {
            threadPool.add(job, prc, true);
        }
    }
    //------------------------------------------------------------------------
    void FileListTransfer::OnReferencePushAck(SocketMessage * msg)
    {
        NetSerializer in(msg->mData, msg->mLength, false);
        in.skipRead(1, 0);
        Nui16 setId;
        in.read(setId);
        SendIRIToAddress(msg->mAddress, setId);
    }
    //------------------------------------------------------------------------
    void FileListTransfer::RemoveFromList(FileToPushRecipient *ftpr)
    {
        fileToPushRecipientListMutex.Lock();
        for (Nui32 i=0; i < fileToPushRecipientList.Size(); i++)
        {
            if (fileToPushRecipientList[i]==ftpr)
            {
                fileToPushRecipientList.RemoveAtIndex(i);
                // List no longer references
                ftpr->Deref();
                fileToPushRecipientListMutex.Unlock();
                return;
            }
        }
        fileToPushRecipientListMutex.Unlock();
    }
    //------------------------------------------------------------------------
    Nui32 FileListTransfer::GetPendingFilesToAddress(Address * recipient)
    {
        fileToPushRecipientListMutex.Lock();
        for (Nui32 i = 0; i < fileToPushRecipientList.Size(); ++i)
        {
            if (fileToPushRecipientList[i]->mAddress==recipient)
            {
                Nui32 size = fileToPushRecipientList[i]->filesToPush.Size();
                fileToPushRecipientListMutex.Unlock();
                return size;
            }
        }
        fileToPushRecipientListMutex.Unlock();

        return 0;
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*