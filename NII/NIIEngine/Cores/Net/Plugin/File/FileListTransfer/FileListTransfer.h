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
#if _RAKNET_SUPPORT_FileListTransfer==1 && _RAKNET_SUPPORT_FileOperations==1

#ifndef __FILE_LIST_TRANFER_H
#define __FILE_LIST_TRANFER_H

#include "NiiNetPreInclude.h"
#include "NiiNetPlugin.h"
#include "NiiNetCommon.h"
#include "DS_Map.h"
#include "NiiNetCommon.h"
#include "PacketLevel.h"
#include "FileList.h"
#include "DS_Queue.h"
#include "NiiThreadManager.h"
#include "NiiThreadManager.h"

namespace NII
{
namespace NII_NET
{
    /// \defgroup FILE_LIST_TRANSFER_GROUP FileListTransfer
    /// \brief A plugin to provide a simple way to compress and incrementally send the files in the FileList structure.
    /// \details
    /// \ingroup PLUGINS_GROUP

    /// \brief A plugin to provide a simple way to compress and incrementally send the files in the FileList structure.
    /// \details Similar to the DirectoryDeltaTransfer plugin, except that it doesn't send deltas based on pre-existing files or actually write the files to disk.
    ///
    /// Usage:
    /// Call SetupReceive to allow one file set to arrive.  The value returned by FileListTransfer::SetupReceive()<BR>
    /// is the setID that is allowed.<BR>
    /// It's up to you to transmit this value to the other system, along with information indicating what kind of files you want to get.<BR>
    /// The other system should then prepare a FileList and call FileListTransfer::send(), passing the return value of FileListTransfer::SetupReceive()<BR>
    /// as the \a setID parameter to FileListTransfer::send()
    /// \ingroup FILE_LIST_TRANSFER_GROUP
    class _EngineAPI FileListTransfer : public Plugin
    {
    public:
        FileListTransfer();
        virtual ~FileListTransfer();

        /// \brief Optionally start worker threads when using _incrementalReadInterface for the send() operation
        /// \param[in] numThreads how many worker threads to start
        /// \param[in] level Passed to the thread creation routine. Use THREAD_PRIORITY_NORMAL for Windows. For Linux based systems, you MUST pass something reasonable based on the thread priorities for your application.
        void StartIncrementalReadThreads(int numThreads, int level = -99999);

        /// \brief Allows one corresponding send() call from another system to arrive.
        /// \param[in] handler The class to call on each file
        /// \param[in] deleteHandler True to delete the handler when it is no longer needed.  False to not do so.
        /// \param[in] allowedSender Which system to allow files from.
        /// \return A set ID value, which should be passed as the \a setID value to the send() call on the other system.  This value will be returned in the callback and is unique per file set.  Returns 65535 on failure (not connected to sender)
        Nui16 SetupReceive(FileListTransferCBInterface * handler, bool deleteHandler, Address * allowedSender);

        /// \brief send the FileList structure to another system, which must have previously called SetupReceive().
        /// \param[in] fileList A list of files.  The data contained in FileList::mData will be sent incrementally and compressed among all files in the set
        /// \param[in] link The instance of RakNet to use to send the message. Pass 0 to use the instance the plugin is attached to
        /// \param[in] recipient The address of the system to send to
        /// \param[in] setID The return value of SetupReceive() which was previously called on \a recipient
        /// \param[in] plevel Passed to LinkBase::send()
        /// \param[in] pchannel Passed to LinkBase::send()
        /// \param[in] _incrementalReadInterface If a file in \a fileList has no data, _incrementalReadInterface will be used to read the file in chunks of size \a chunkSize
        /// \param[in] _chunkSize How large of a block of a file to read/send at once. Large values use more memory but transfer slightly faster.
        void send(FileList *fileList, LinkBase * link, Address * recipient, Nui16 setID, PacketLevel plevel, 
            char pchannel, BufferReadBase *_incrementalReadInterface = 0, Nui32 _chunkSize = 262144 * 4 * 16);

        /// Return number of files waiting to go out to a particular address
        Nui32 GetPendingFilesToAddress(Address * recipient);

        /// \brief Stop a download.
        void CancelReceive(Nui16 setId);

        /// \brief Remove all handlers associated with a particular system address.
        void RemoveReceiver(Address * address);

        /// \brief Is a handler passed to SetupReceive still running?
        bool IsHandlerActive(Nui16 setId);

        /// \brief Adds a callback to get progress reports about what the file list instances do.
        /// \param[in] cb A pointer to an externally defined instance of FileListProgress. This pointer is held internally, so should remain valid as long as this class is valid.
        void AddCallback(FileListProgress * cb);

        /// \brief Removes a callback
        /// \param[in] cb A pointer to an externally defined instance of FileListProgress that was previously added with AddCallback()
        void RemoveCallback(FileListProgress * cb);

        /// \brief Removes all callbacks
        void ClearCallbacks();

        /// Returns all callbacks added with AddCallback()
        /// \param[out] callbacks The list is set to the list of callbacks
        void GetCallbacks(DataStructures::List<FileListProgress *> & callbacks);

        /// \internal For plugin handling
        virtual void update();
        /// \internal For plugin handling
        virtual ConnectReceiveType onMessage(SocketMessage * msg);
        /// \internal For plugin handling
        virtual void shutdown();
        /// \internal For plugin handling
        virtual void onConnectClose(const Address & address, UniqueID id,
            ConnectCloseType lostConnectionReason);
    protected:
        bool DecodeSetHeader(SocketMessage * msg);
        bool DecodeFile(SocketMessage * msg, bool fullFile);
        void OnReferencePush(SocketMessage * msg, bool fullFile);
        void OnReferencePushAck(SocketMessage * msg);
        void SendIRIToAddress(Address * address, Nui16 setId);
        void Clear();

        Nui16 setId;
        DataStructures::Map<Nui16, FileListReceiver *> fileListReceivers;
        DataStructures::List<FileListProgress *> fileListProgressCallbacks;

        struct FileToPush
        {
            FileListNode fileListNode;
            PacketLevel packetPriority;
            char mPacketCh;
            Nui32 currentOffset;
            ////Nui16 setID;
            Nui32 setIndex;
            BufferReadBase * incrementalReadInterface;
            Nui32 chunkSize;
        };
        struct FileToPushRecipient
        {
            Nui32 mRefCount;
            ThreadMutex refCountMutex;
            void DeleteThis(void);
            void AddRef(void);
            void Deref(void);

            Address * mAddress;
            Nui16 setId;

            //// ThreadMutex filesToPushMutex;
            DataStructures::Queue<FileToPush *> filesToPush;
        };
        DataStructures::List< FileToPushRecipient *> fileToPushRecipientList;
        ThreadMutex fileToPushRecipientListMutex;
        void RemoveFromList(FileToPushRecipient * ftpr);

        /*struct ThreadData
        {
            FileListTransfer * fileListTransfer;
            Address * mAddress;
            Nui16 setId;
        };
        */
        ThreadManager<ThreadData, int> threadPool;
    };
}
}
#endif
#endif