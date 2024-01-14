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

#ifndef __DIRECTORY_DELTA_TRANSFER_H
#define __DIRECTORY_DELTA_TRANSFER_H

#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"
#include "NiiNetPlugin.h"
#include "DS_Map.h"
#include "PacketLevel.h"

/// \defgroup DIRECTORY_DELTA_TRANSFER_GROUP DirectoryDeltaTransfer
/// \brief Simple class to send changes between directories
/// \details
/// \ingroup PLUGINS_GROUP

/// \brief Simple class to send changes between directories.  In essence, a simple autopatcher that can be used for transmitting levels, skins, etc.
/// \details
/// \sa AutopatcherClient class for database driven patching, including binary deltas and search by date.
///
/// To use, first set the path to your application.  For example "C:/Games/MyRPG/"<BR>
/// To allow other systems to download files, call AddUploadsFromSubdirectory, where the parameter is a path relative<BR>
/// to the path to your application.  This includes subdirectories.<BR>
/// For example:<BR>
/// SetApplicationDirectory("C:/Games/MyRPG/");<BR>
/// AddUploadsFromSubdirectory("Mods/Skins/");<BR>
/// would allow downloads from<BR>
/// "C:/Games/MyRPG/Mods/Skins/*.*" as well as "C:/Games/MyRPG/Mods/Skins/Level1/*.*"<BR>
/// It would NOT allow downloads from C:/Games/MyRPG/Levels, nor would it allow downloads from C:/Windows<BR>
/// While pathToApplication can be anything you want, applicationSubdirectory must match either partially or fully between systems.
/// \ingroup DIRECTORY_DELTA_TRANSFER_GROUP

namespace NII
{
namespace NII_NET
{
    class _EngineAPI DirectoryDeltaTransfer : public Plugin
    {
    public:
        // Constructor
        DirectoryDeltaTransfer();

        // Destructor
        virtual ~DirectoryDeltaTransfer();

        /// \brief This plugin has a dependency on the FileListTransfer plugin, which it uses to actually send the files.
        /// \details So you need an instance of that plugin registered with LinkBase, and a pointer to that interface should be passed here.
        /// \param[in] flt A pointer to a registered instance of FileListTransfer
        void SetFileListTransferPlugin(FileListTransfer * flt);

        /// \brief Set the local root directory to base all file uploads and downloads off of.
        /// \param[in] pathToApplication This path will be prepended to \a applicationSubdirectory in AddUploadsFromSubdirectory to find the actual path on disk.
        void SetApplicationDirectory(const char * pathToApplication);

        /// \brief What parameters to use for the LinkBase::send() call when uploading files.
        /// \param[in] _plevel See LinkBase::send()
        /// \param[in] _pchannel See LinkBase::send()
        void SetUploadSendParameters(PacketLevel _plevel, char _pchannel);

        /// \brief Add all files in the specified subdirectory recursively.
        /// \details \a subdir is appended to \a pathToApplication in SetApplicationDirectory().
        /// All files in the resultant directory and subdirectories are then hashed so that users can download them.
        /// \pre You must call SetFileListTransferPlugin with a valid FileListTransfer plugin
        /// \param[in] subdir Concatenated with pathToApplication to form the final path from which to allow uploads.
        void AddUploadsFromSubdirectory(const char *subdir);

        /// \brief Downloads files from the matching parameter \a subdir in AddUploadsFromSubdirectory.
        /// \details \a subdir must contain all starting characters in \a subdir in AddUploadsFromSubdirectory
        /// Therefore,
        /// AddUploadsFromSubdirectory("Levels/Level1/"); would allow you to download using DownloadFromSubdirectory("Levels/Level1/Textures/"...
        /// but it would NOT allow you to download from DownloadFromSubdirectory("Levels/"... or DownloadFromSubdirectory("Levels/Level2/"...
        /// \pre You must call SetFileListTransferPlugin with a valid FileListTransfer plugin
        /// \note Blocking. Will block while hashes of the local files are generated
        /// \param[in] subdir A directory passed to AddUploadsFromSubdirectory on the remote system.  The passed dir can be more specific than the remote dir.
        /// \param[in] outputSubdir The directory to write the output to.  Usually this will match \a subdir but it can be different if you want.
        /// \param[in] prependAppDirToOutputSubdir True to prepend outputSubdir with pathToApplication when determining the final output path.  Usually you want this to be true.
        /// \param[in] host The address of the remote system to send the message to.
        /// \param[in] onFileCallback Callback to call per-file (optional).  When fileIndex+1==setCount in the callback then the download is done
        /// \param[in] _plevel See LinkBase::send()
        /// \param[in] _pchannel See LinkBase::send()
        /// \param[in] cb Callback to get progress updates. Pass 0 to not use.
        /// \return A set ID, identifying this download set.  Returns 65535 on host unreachable.
        Nui16 DownloadFromSubdirectory(const char * subdir, const char * outputSubdir,
            bool prependAppDirToOutputSubdir, Address * host,
                FileListTransferCBInterface *onFileCallback, PacketLevel _plevel,
                    char _pchannel, FileListProgress *cb);

        /// \brief Downloads files from the matching parameter \a subdir in AddUploadsFromSubdirectory.
        /// \details \a subdir must contain all starting characters in \a subdir in AddUploadsFromSubdirectory
        /// Therefore,
        /// AddUploadsFromSubdirectory("Levels/Level1/"); would allow you to download using DownloadFromSubdirectory("Levels/Level1/Textures/"...
        /// but it would NOT allow you to download from DownloadFromSubdirectory("Levels/"... or DownloadFromSubdirectory("Levels/Level2/"...
        /// \pre You must call SetFileListTransferPlugin with a valid FileListTransfer plugin
        /// \note Nonblocking, but requires call to GenerateHashes()
        /// \param[in] localFiles Hashes of local files already on the harddrive. Populate with GenerateHashes(), which you may wish to call from a thread
        /// \param[in] subdir A directory passed to AddUploadsFromSubdirectory on the remote system.  The passed dir can be more specific than the remote dir.
        /// \param[in] outputSubdir The directory to write the output to.  Usually this will match \a subdir but it can be different if you want.
        /// \param[in] prependAppDirToOutputSubdir True to prepend outputSubdir with pathToApplication when determining the final output path.  Usually you want this to be true.
        /// \param[in] host The address of the remote system to send the message to.
        /// \param[in] onFileCallback Callback to call per-file (optional).  When fileIndex+1==setCount in the callback then the download is done
        /// \param[in] _plevel See LinkBase::send()
        /// \param[in] _pchannel See LinkBase::send()
        /// \param[in] cb Callback to get progress updates. Pass 0 to not use.
        /// \return A set ID, identifying this download set.  Returns 65535 on host unreachable.
        Nui16 DownloadFromSubdirectory(FileList & localFiles, const char * subdir,
            const char * outputSubdir, bool prependAppDirToOutputSubdir, Address * host,
                FileListTransferCBInterface * onFileCallback, PacketLevel _plevel,
                    char _pchannel, FileListProgress * cb);

        /// Hash files already on the harddrive, in preparation for a call to DownloadFromSubdirectory(). Passed to second version of DownloadFromSubdirectory()
        /// This is slow, and it is exposed so you can call it from a thread before calling DownloadFromSubdirectory()
        /// \param[out] localFiles List of hashed files populated from \a outputSubdir and \a prependAppDirToOutputSubdir
        /// \param[in] outputSubdir The directory to write the output to.  Usually this will match \a subdir but it can be different if you want.
        /// \param[in] prependAppDirToOutputSubdir True to prepend outputSubdir with pathToApplication when determining the final output path.  Usually you want this to be true.
        void GenerateHashes(FileList & localFiles, const char * outputSubdir,
            bool prependAppDirToOutputSubdir);

        /// \brief Clear all allowed uploads previously set with AddUploadsFromSubdirectory
        void ClearUploads(void);

        /// \brief Returns how many files are available for upload
        /// \return How many files are available for upload
        Nui32 GetNumberOfFilesForUpload(void) const;

        /// \brief Normally, if a remote system requests files, those files are all loaded into memory and sent immediately.
        /// \details This function allows the files to be read in incremental chunks, saving memory
        /// \param[in] _incrementalReadInterface If a file in \a fileList has no data, filePullInterface will be used to read the file in chunks of size \a chunkSize
        /// \param[in] _chunkSize How large of a block of a file to send at once
        void SetDownloadRequestBufferReadBase(BufferReadBase * _incrementalReadInterface,
            Nui32 _chunkSize);

        /// \internal For plugin handling
        virtual ConnectReceiveType onMessage(SocketMessage * msg);
    protected:
        void OnDownloadRequest(SocketMessage * msg);

        char applicationDirectory[512];
        FileListTransfer *fileListTransfer;
        FileList *availableUploads;
        PacketLevel mPacketLevel;
        char mPacketCh;
        BufferReadBase *incrementalReadInterface;
        Nui32 chunkSize;
    };
}
}
#endif
#endif