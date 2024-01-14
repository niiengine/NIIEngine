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

#ifndef _NII_Zip_H_
#define _NII_Zip_H_

#include "NiiPreInclude.h"
#include "NiiDataStream.h"
#include "NiiVFSManager.h"
#include "NiiTempBuffer.h"

typedef struct zzip_dir     ZZIP_DIR;
typedef struct zzip_file    ZZIP_FILE;
typedef union _zzip_plugin_io zzip_plugin_io_handlers;

namespace NII
{
    /** 一个特殊的档案类,可以读取一个zip文件格式存档.
    @remark 此存档格式支持压缩所有档案的标准的zip格式,包括ID pk3文件.
    @par ID 0x02
    */
    class ZipArchive : public VFS
    {
    public:
        ZipArchive(const String & name, VFSType type,
            zzip_plugin_io_handlers * pluginIo = NULL);

        ~ZipArchive();

        /// @copydetails VFS::exists
        bool isExist(const String & filename) const;

        /// @copydetails VFS::peek
        void peek();

        /// @copydetails VFS::open
        DataStream * open(const String & filename, bool readOnly = true);

        /// @copydetails VFS::create
        DataStream * create(const String & filename) const;

        /// @copydetails VFS::remove
        void remove(const String & filename) const;

        /// @copydetails VFS::list
        void list(StringList & out, bool r = true, bool dirs = false)const;

        /// @copydetails VFS::list
        void list(FileInfoList & out, bool r = true, bool dirs = false)const;

        /// @copydetails VFS::find
        void find(StringList & out, const String & pattern, bool r = true, bool dirs = false)const;

        /// @copydetails VFS::find
        void find(FileInfoList & out, const String & pattern, bool r = true, bool dirs = false) const;

        /// @copydetails VFS::close
        void close();

        /// @copydetails VFS::isMatchCase
        bool isMatchCase() const { return false; }

        /// @copydetails VFS::getModifiedTime
        time_t getModifiedTime(const String & filename);
    protected:
        /// 处理任何从zzip的错误
        void checkZzipError(NIIi zzipError, const String & operation) const;
    protected:
        N_mutex1
        zzip_plugin_io_handlers * mPluginIo;///< 指向其他文件io的实现(档中档)
        ZZIP_DIR * mZzipDir;                ///< zip文件根句柄
        FileInfoList mFileList;             ///< 文件列表(因为zziplib似乎只允许扫描目录树一次)
    };

    /// 为Zip文件专用的VFSFactory
    class ZipArchiveFactory : public VFSFactory
    {
    public:
        ZipArchiveFactory();
        virtual ~ZipArchiveFactory();

        /// @copydetails VFSFactory::getType
        VFSType getType() const;

        /// @copydetails VFSFactory::createInstance
        VFS * create(const String & prl);

        /// @copydetails VFSFactory::destroyInstance
        void destroy(VFS * obj);
    };

    /** Specialisation of ZipArchiveFactory for embedded Zip files. */
    class EmbeddedZipArchiveFactory : public ZipArchiveFactory
    {
    public:
        EmbeddedZipArchiveFactory();
        virtual ~EmbeddedZipArchiveFactory();

        /// @copydetails VFSFactory::getType
        VFSType getType() const;

        /// @copydetails VFSFactory::create
        VFS * create(const String & prl);

        /** a function type to decrypt embedded zip file
        @param[in] pos pos in file
        @param[in] buf current buffer to decrypt
        @param[in] len - length of buffer
        @return success
        */
        typedef bool (*DecryptEmbeddedZipFileFunc)(NCount pos, void * buf, NCount len);

        /// Add an embedded file to the embedded file list
        static void addEmbbeddedFile(const String & name, const Nui8 * fileData,
            NCount fileSize, DecryptEmbeddedZipFileFunc decryptFunc);

        /// Remove an embedded file to the embedded file list
        static void removeEmbbeddedFile(const String & name);
    };

#if N_PLAT == N_PLAT_ANDROID
    class APKZipArchiveFactory : public EmbeddedZipArchiveFactory
    {
    protected:
        AAssetManager* mAssetMgr;
    public:
        APKZipArchiveFactory(AAssetManager* assetMgr) : mAssetMgr(assetMgr) {}
        virtual ~APKZipArchiveFactory() {}

        /// @copydetails VFSFactory::getType
        VFSType getType() const;

        /// @copydetails VFSFactory::createInstance
        VFS *create(const String& name);
    };
#endif
}
#endif