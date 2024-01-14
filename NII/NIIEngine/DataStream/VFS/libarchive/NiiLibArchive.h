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

#ifndef _NII_LIBARCHIVE_H_
#define _NII_LIBARCHIVE_H_

#include "NiiPreInclude.h"
#include "NiiVFS.h"
#include "NiiVFSFactory.h"
#include <archive.h>
#include <archive_entry.h>

// 预定义libarchive头文件声明,以避免依赖.
typedef struct archive ARCHIVE;
typedef struct archive_entry ARCHIVE_ENTRY;
typedef NIIi ARCHIVE_ERROR;

namespace NII
{
    /** 一个NII专用的libarchive的接口具体格式支持可以通过查看具体使用版本所支持的格式
    @par ID 0x04
    */
    class _EngineAPI LibArchive : public VFS
    {
    public:
        LibArchive(const String & name, const String & type);
        ~LibArchive();

        /// @copydetails VFS::peek
        void peek();

        /// @copydetails VFS::open
        DataStream * open(const String & filename, bool readOnly = true) const;

        /// @copydetails VFS::create
        DataStream * create(const String & filename) const;
        
        /// @copydetails VFS::remove
        void remove(const String & filename) const;

        /// @copydetails VFS::list
        void list(StringList & out, bool r = true, bool dirs = false);

        /// @copydetails VFS::list
        void list(FileInfoList & out, bool r = true, bool dirs = false);

        /// @copydetails VFS::find
        void find(StringList & out, const String & pattern, bool r = true, bool dirs = false);

        /// @copydetails VFS::find
        void find(FileInfoList & out, const String & pattern, bool r = true, bool dirs = false);

        /// @copydetails VFS::exists
        bool isExist(const String & filename);

        /// @copydetails VFS::isMatchCase
        bool isMatchCase() const { return false; }

        /// @copydetails VFS::getModifiedTime
        time_t getModifiedTime(const String & filename);
    protected:
        N_mutex1
        FileInfoList mFileList;     ///< 文件列表
    };
    
    /** 为libarchive文件专用的VFSFactory
    @version NIIEngine 3.0.0
    */
    class _EngineInner LibArchiveFactory : public VFSFactory
    {
    public:
        LibArchiveFactory();
        virtual ~LibArchiveFactory();

        /// @copydetails VFSFactory::getType
        VFSType getType() const;

        /// @copydetails VFSFactory::createInstance
        VFS * create(const String & prl);

        /// @copydetails VFSFactory::destroyInstance
        void destroy(VFS * obj);
    };

    /** 专用的DataStream,从libarchive档案中处理流数据
    @version NIIEngine 3.0.0
    */
    class _EngineInner LibArchiveDataStream : public DataStream
    {
    protected:
        ARCHIVE * mArchive;								///< 核心档案文件
        ARCHIVE_ENTRY * mFile;							///< 当前文档
        TempBuffer<2 * NII_STREAM_TEMP_SIZE> mCache;	///< 缓存
    public:
        /** 无命名构造
        @param[in] archive 这个文件的来源
        @param[in] file 文件进口
        @param[in] size 未压缩时候的大小
        */
        LibArchiveDataStream(const ARCHIVE * archive, const ARCHIVE_ENTRY * file, NCount size);

        /** 创建命名流的构造函数
        @param[in] name 文件的名字
        @param[in] archive 这个文件的来源
        @param[in] file 文件进口
        @param[in] size 未压缩时候的大小
        */
        LibArchiveDataStream(const String & name, const ARCHIVE * libFile,
            const ARCHIVE_ENTRY * file, NCount size);

        ~LibArchiveDataStream();

        /// @copydetails DataStream::read
        NCount read(void * buf, NCount count);

        /// @copydetails DataStream::write
        NCount write(const void * buf, NCount count);

        /// @copydetails DataStream::skip
        void skip(NIIl count);

        /// @copydetails DataStream::seek
        void seek(NCount pos);

        /// @copydetails DataStream::seek
        NCount tell(void) const;

        /// @copydetails DataStream::eof
        bool eof(void) const;

        /// @copydetails DataStream::close
        void close(void);
    };
}
#endif