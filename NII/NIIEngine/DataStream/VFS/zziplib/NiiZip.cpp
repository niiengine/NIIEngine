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

#include "NiiPreProcess.h"
#include "NiiZip.h"
#include "NiiLogManager.h"
#include "NiiException.h"
#include "NiiEngine.h"

#include "zzip/zzip.h"
#include "zzip/plugin.h"

namespace NII
{
    /// 专用的DataStream,从zip档案中处理流数据
    class _EngineInner ZipDataStream : public DataStream
    {
    public:
        /** 无命名构造函数
        @param[in] zzipFile 文件的进口
        @param[in] size 未压缩时候的大小
        */
        ZipDataStream(ZZIP_FILE * zzipFile, NCount size);

        /** 创建命名流的构造函数
        @param[in] name 一般是文件的名字
        @param[in] zzipFile 文件的进口
        @param[in] size 未压缩时候的大小
        */
        ZipDataStream(const String & name, ZZIP_FILE * zzipFile, NCount size);

        ~ZipDataStream();

        /// @copydetails DataStream::read
        NCount read(void * buf, NCount count) const;

        /// @copydetails DataStream::write
        NCount write(const void * buf, NCount count);

        /// @copydetails DataStream::skip
        void skip(NIIl count);

        /// @copydetails DataStream::seek
        void seek(NCount pos);

        /// @copydetails DataStream::seek
        NCount tell() const;

        /// @copydetails DataStream::end
        bool end() const;

        /// @copydetails DataStream::close
        void close();
    protected:
        ZZIP_FILE * mZzipFile;
        mutable TempBuffer<2 * NII_STREAM_TEMP_SIZE> mCache;
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ZipArchive
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // 实用方法,为格式输出zzip错误
    String getZzipErrorDescription(zzip_error_t error)
    {
        String errorMsg;
        switch(error)
        {
        case ZZIP_NO_ERROR:
            break;
        case ZZIP_OUTOFMEM:
            errorMsg = _I18n("Out of memory.");
            break;
        case ZZIP_DIR_OPEN:
            errorMsg = _I18n("Unable open file");
        case ZZIP_DIR_STAT:
        case ZZIP_DIR_SEEK:
        case ZZIP_DIR_READ:
            errorMsg = _I18n("Unable read file.");
            break;
        case ZZIP_UNSUPP_COMPR:
            errorMsg = _I18n("Unsupported format.");
            break;
        case ZZIP_CORRUPTED:
            errorMsg = _I18n("Corrupted file.");
            break;
        case ZZIP_DIR_TOO_SHORT:
            errorMsg = _I18n("File is too short");
            break;
        case ZZIP_DIR_EDH_MISSING:
            errorMsg = _I18n("File central directory record missing");
            break;
        case ZZIP_ENOENT:
            errorMsg = _I18n("File not in archive");
            break;
        default:
            errorMsg = _I18n("Unknown error.");
            break;
        };
        return errorMsg;
    }
    //-----------------------------------------------------------------------
    ZipArchive::ZipArchive(const String & name, VFSType type,
        zzip_plugin_io_handlers * pluginIo) :
            VFS(name, type),
            mZzipDir(0),
            mPluginIo(pluginIo)
    {
    }
    //-----------------------------------------------------------------------
    ZipArchive::~ZipArchive()
    {
        close();
    }
    //-----------------------------------------------------------------------
    void ZipArchive::close()
    {
        N_mutex1_lock
        if (mZzipDir)
        {
            zzip_dir_close(mZzipDir);
            mZzipDir = 0;
            mFileList.clear();
        }
    }
    //-----------------------------------------------------------------------
    void ZipArchive::peek()
    {
        N_mutex1_lock
        if(!mZzipDir)
        {
            zzip_error_t zzipError;
#if NII_STRING
            VString tempname;
            DataStream::ws2s(mName, tempname);
            mZzipDir = zzip_dir_open_ext_io(tempname.c_str(), &zzipError, 0, mPluginIo);
#else
            mZzipDir = zzip_dir_open_ext_io(mName.c_str(), &zzipError, 0, mPluginIo);
#endif
            checkZzipError(zzipError, _I18n("opening archive"));

            // 缓存名字
            ZZIP_DIRENT zzipEntry;
            while(zzip_dir_read(mZzipDir, &zzipEntry))
            {
                FileInfo info;
                info.mSrc = this;
#if NII_STRING
                String tempname;
                DataStream::s2ws(zzipEntry.d_name, tempname);
                // 获取 基本名/路径
                StrUtil::splitFilename(tempname, info.mFileName, info.mPath);
                info.mFullName = tempname;
#else
                // 获取 基本名/路径
                StrUtil::splitFilename(zzipEntry.d_name, info.mFileName, info.mPath);
                info.mFullName = zzipEntry.d_name;
#endif
                // 获取大小
                info.mCompressSize = static_cast<NCount>(zzipEntry.d_csize);
                info.mSize = static_cast<NCount>(zzipEntry.st_size);
                // 文件夹条目
                if(info.mFileName.empty())
                {
                    info.mFullName = info.mFullName.substr (0, info.mFullName.length () - 1);
                    StrUtil::splitFilename(info.mFullName, info.mFileName, info.mPath);
                    info.mCompressSize = NCount (-1);
                }
                else
                {
                    info.mFullName = info.mFileName;
                }
                mFileList.push_back(info);
            }
        }
    }
    //-----------------------------------------------------------------------
    DataStream * ZipArchive::open(const String & filename, bool readOnly)
    {
        // zziplib不是线程安全的
        N_mutex1_lock

#if NII_STRING
        VString lookUpFileName;
        DataStream::ws2s(filename, lookUpFileName);
#else
        String lookUpFileName = filename;
#endif
        // Format not used here (always binary)
        ZZIP_FILE * zzipFile = zzip_file_open(mZzipDir, lookUpFileName.c_str(), ZZIP_ONLYZIP | ZZIP_CASELESS);
        if(!zzipFile) // Try if we find the file
        {
            FileInfoList fileNfo;
            String basename, path;
            StrUtil::splitFilename(filename, basename, path);
            find(fileNfo, basename, true);
            if(fileNfo.size() == 1) // If there are more files with the same do not open anyone
            {
                FileInfo info = fileNfo.at(0);
#if NII_STRING
                DataStream::ws2s(info.mPath + info.mFileName, lookUpFileName);
#else
                lookUpFileName = info.mPath + info.mFileName;
#endif
                zzipFile = zzip_file_open(mZzipDir, lookUpFileName.c_str(), ZZIP_ONLYZIP | ZZIP_CASELESS); // When an error happens here we will catch it below
            }
        }
        if(!zzipFile)
        {
            NIIi zerr = zzip_error(mZzipDir);
            String zzDesc = getZzipErrorDescription((zzip_error_t)zerr);
            N_Only(Log).log(mName + _I18n(" - Unable to open file ") +
                filename + _I18n(", error was '") + zzDesc + _T("'"));

            // return null pointer
            return 0;
        }

        // Get uncompressed size too
        ZZIP_STAT zstat;
        zzip_dir_stat(mZzipDir, lookUpFileName.c_str(), &zstat, ZZIP_CASELESS);

        // Construct & return stream
        return N_new ZipDataStream(filename, zzipFile, (NCount)zstat.st_size);
    }
    //---------------------------------------------------------------------
    DataStream * ZipArchive::create(const String & filename) const
    {
        N_EXCEPT(UnImpl, _I18n("Modification of zipped archives is not supported"));
    }
    //---------------------------------------------------------------------
    void ZipArchive::remove(const String & filename) const
    {
        //unsupport
    }
    //-----------------------------------------------------------------------
    void ZipArchive::list(StringList & out, bool r, bool dirs) const
    {
        N_mutex1_lock

        FileInfoList::const_iterator i, iend = mFileList.end();
        for(i = mFileList.begin(); i != iend; ++i)
        {
            if((dirs == (i->mCompressSize == NCount(-1))) && (r || i->mPath.empty()))
            {
                out.push_back(i->mFullName);
            }
        }
    }
    //-----------------------------------------------------------------------
    void ZipArchive::list(FileInfoList & out, bool r, bool dirs) const
    {
        N_mutex1_lock

        FileInfoList::const_iterator i, iend = mFileList.end();
        for(i = mFileList.begin(); i != iend; ++i)
        {
            if((dirs == (i->mCompressSize == NCount(-1))) && (r || i->mPath.empty()))
                out.push_back(*i);
        }
    }
    //-----------------------------------------------------------------------
    void ZipArchive::find(StringList & out, const String & pattern, bool r, bool dirs) const
    {
        N_mutex1_lock

        // If pattern contains a directory name, do a full match
        bool full_match = (pattern.find('/') != String::npos) || (pattern.find ('\\') != String::npos);
        bool wildCard = pattern.find('*') != String::npos;

        FileInfoList::const_iterator i, iend = mFileList.end();
        for(i = mFileList.begin(); i != iend; ++i)
        {
            if((dirs == (i->mCompressSize == NCount (-1))) && (r || full_match || wildCard))
            {
                // Check basename matches pattern (zip is case insensitive)
                if(StrUtil::match(full_match ? i->mFullName : i->mFileName, pattern, false))
                    out.push_back(i->mFullName);
            }
        }
    }
    //-----------------------------------------------------------------------
    void ZipArchive::find(FileInfoList & out, const String & pattern, bool r, bool dirs) const
    {
        N_mutex1_lock

        // If pattern contains a directory name, do a full match
        bool full_match = (pattern.find ('/') != String::npos) || (pattern.find ('\\') != String::npos);
        bool wildCard = pattern.find('*') != String::npos;

        FileInfoList::const_iterator i, iend = mFileList.end();
        for(i = mFileList.begin(); i != iend; ++i)
        {
            if((dirs == (i->mCompressSize == NCount (-1))) && (r || full_match || wildCard))
                // Check name matches pattern (zip is case insensitive)
                if(StrUtil::match(full_match ? i->mFullName : i->mFileName, pattern, false))
                    out.push_back(*i);
        }
    }
    //-----------------------------------------------------------------------
    struct FileNameCompare : public std::binary_function<FileInfo, String, bool>
    {
        bool operator()(const NII::FileInfo& lhs, const String& filename) const
        {
            return lhs.mFullName == filename;
        }
    };
    //-----------------------------------------------------------------------
    bool ZipArchive::isExist(const String & filename) const
    {
        // zziplib is not threadsafe
       /* N_mutex1_lock
        ZZIP_STAT zstat;
        NIIi res = zzip_dir_stat(mZzipDir, filename.c_str(), &zstat, ZZIP_CASEINSENSITIVE);

        return (res == ZZIP_NO_ERROR);*/

        N_mutex1_lock;
        String cleanName = filename;

        if (filename.rfind('/') != String::npos)
        {
            StringList tokens;
            StrUtil::split(filename, tokens, _T("/"));
            cleanName = tokens[tokens.size() - 1];
        }

        return std::find_if(mFileList.begin(), mFileList.end(), std::bind2nd<FileNameCompare>(FileNameCompare(), cleanName)) != mFileList.end();
    }
    //---------------------------------------------------------------------
    time_t ZipArchive::getModifiedTime(const String & filename)
    {
        // Zziplib doesn't yet support getting the modification time of individual files
        // so just check the mod time of the zip itself
        Nstruct_stat tagStat;
        bool ret = (Nstat(mName.c_str(), &tagStat) == 0);

        if(ret)
        {
            return tagStat.st_mtime;
        }
        else
        {
            return 0;
        }

    }
    //-----------------------------------------------------------------------
    void ZipArchive::checkZzipError(NIIi zzipError, const String & operation) const
    {
        if(zzipError != ZZIP_NO_ERROR)
        {
            String errorMsg = getZzipErrorDescription((zzip_error_t)zzipError);

            N_EXCEPT(Internal, mName + _I18n(" - error whilst ") + operation + _T(": ") + errorMsg);
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ZipDataStream
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ZipDataStream::ZipDataStream(ZZIP_FILE * zzipFile, NCount size) :
        mZzipFile(zzipFile)
    {
        mSize = size;
    }
    //-----------------------------------------------------------------------
    ZipDataStream::ZipDataStream(const String & name, ZZIP_FILE * zzipFile, NCount size) :
        DataStream(name),
        mZzipFile(zzipFile)
    {
        mSize = size;
    }
    //-----------------------------------------------------------------------
    ZipDataStream::~ZipDataStream()
    {
        close();
    }
    //-----------------------------------------------------------------------
    NCount ZipDataStream::read(void * buf, NCount count) const
    {
        NCount was_avail = mCache.read(buf, count);
        zzip_ssize_t r = 0;
        if(was_avail < count)
        {
            r = zzip_file_read(mZzipFile, (NIIb *)buf + was_avail, count - was_avail);
            if(r < 0)
            {
                ZZIP_DIR * dir = zzip_dirhandle(mZzipFile);
#if NII_STRING
                String msg;
                DataStream::s2ws(zzip_strerror_of(dir), msg);
#else
                String msg = DataStream::s2wszzip_strerror_of(dir);
#endif
                N_EXCEPT(Internal, mName + _I18n(" - error from zziplib: ") + msg);
            }
            mCache.cacheData((NIIb *)buf + was_avail, (NCount)r);
        }
        return was_avail + (NCount)r;
    }
    //---------------------------------------------------------------------
    NCount ZipDataStream::write(const void * buf, NCount count)
    {
        // not supported
        return 0;
    }
    //-----------------------------------------------------------------------
    void ZipDataStream::skip(NIIl count)
    {
        NIIl was_avail = static_cast<NIIl>(mCache.avail());
        if(count > 0)
        {
            if(!mCache.skip(count))
                zzip_seek(mZzipFile, static_cast<zzip_off_t>(count - was_avail), SEEK_CUR);
        }
        else if(count < 0)
        {
            if(!mCache.rewind((NCount)(-count)))
                zzip_seek(mZzipFile, static_cast<zzip_off_t>(count + was_avail), SEEK_CUR);
        }
    }
    //-----------------------------------------------------------------------
    void ZipDataStream::seek(NCount pos)
    {
        zzip_off_t newPos = static_cast<zzip_off_t>(pos);
        zzip_off_t prevPos = static_cast<zzip_off_t>(tell());
        if (prevPos < 0)
        {
            // seek set after invalid pos
            mCache.clear();
            zzip_seek(mZzipFile, newPos, SEEK_SET);
        }
        else
        {
            // everything is going all right, relative seek
            skip(newPos - prevPos);
        }
    }
    //-----------------------------------------------------------------------
    NCount ZipDataStream::tell(void) const
    {
        zzip_off_t pos = zzip_tell(mZzipFile);
        if (pos<0)
            return (NCount)(-1);
        return static_cast<NCount>(pos) - mCache.avail();
    }
    //-----------------------------------------------------------------------
    bool ZipDataStream::end(void) const
    {
        return (tell() >= mSize);
    }
    //-----------------------------------------------------------------------
    void ZipDataStream::close(void)
    {
        if (mZzipFile != 0)
        {
            zzip_file_close(mZzipFile);
            mZzipFile = 0;
        }
        mCache.clear();
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ZipArchiveFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ZipArchiveFactory::ZipArchiveFactory()
    {
    }
    //-----------------------------------------------------------------------
    ZipArchiveFactory::~ZipArchiveFactory()
    {
    }
    //-----------------------------------------------------------------------
    VFSType ZipArchiveFactory::getType() const
    {
        return VFST_Zip;
    }
    //-----------------------------------------------------------------------
    VFS * ZipArchiveFactory::create(const String & prl)
    {
        return N_new ZipArchive(prl, VFST_Zip);
    }
    //-----------------------------------------------------------------------
    void ZipArchiveFactory::destroy(VFS * obj)
    {
        N_delete obj;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // EmbeddedZipArchiveFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    /// a struct to hold embedded file data
    struct EmbeddedFileData
    {
        const Nui8 * fileData;
        zzip_size_t fileSize;
        zzip_size_t curPos;
        bool isFileOpened;
        EmbeddedZipArchiveFactory::DecryptEmbeddedZipFileFunc decryptFunc;
    };
    //-----------------------------------------------------------------------
    /// A type for a map between the file names to file index
    typedef map<String, NIIi>::type FileNameToIndexMap;
    typedef FileNameToIndexMap::iterator FileNameToIndexMapIter;
    /// A type to store the embedded files data
    typedef vector<EmbeddedFileData>::type EmbbedFileDataList;
    zzip_plugin_io_handlers * mPluginIo = NULL;
    namespace {
    /// A static map between the file names to file index
    FileNameToIndexMap * EmbeddedZipArchiveFactory_mFileNameToIndexMap;
    /// A static list to store the embedded files data
    EmbbedFileDataList * EmbeddedZipArchiveFactory_mEmbbedFileDataList;
    _zzip_plugin_io sEmbeddedZipArchiveFactory_PluginIo;
    #define EMBED_IO_BAD_FILE_HANDLE (-1)
    #define EMBED_IO_SUCCESS (0)
    //-----------------------------------------------------------------------
    /// functions for embedded zzip_plugin_io_handlers implementation
    /// The functions are here and not as static members because they
    /// use types that I don't want to define in the header like zzip_char_t,
    //  zzip_ssize_t and such.
    //-----------------------------------------------------------------------
    // get file date by index
    EmbeddedFileData & getEmbeddedFileDataByIndex(NIIi fd)
    {
        return (*EmbeddedZipArchiveFactory_mEmbbedFileDataList)[fd-1];
    }
    //-----------------------------------------------------------------------
    // 打开文件回调
    NIIi EmbeddedZipArchiveFactory_open(zzip_char_t * name, NIIi flags, ...)
    {
        String nameAsString;
        DataStream::s2ws(name, nameAsString);
        FileNameToIndexMapIter foundIter = EmbeddedZipArchiveFactory_mFileNameToIndexMap->find(nameAsString);
        if (foundIter != EmbeddedZipArchiveFactory_mFileNameToIndexMap->end())
        {
            NIIi fd = foundIter->second;
            EmbeddedFileData & curEmbeddedFileData = getEmbeddedFileDataByIndex(fd);
            if(curEmbeddedFileData.isFileOpened)
            {
               // file is opened - return an error handle
               return EMBED_IO_BAD_FILE_HANDLE;
            }

            curEmbeddedFileData.isFileOpened = true;
            return fd;
        }
        else
        {
           // not found - return an error handle
           return EMBED_IO_BAD_FILE_HANDLE;
        }
    }
    //-----------------------------------------------------------------------
    // 关闭文件回调
    // Return Value - On success, close returns 0.
    NIIi EmbeddedZipArchiveFactory_close(NIIi fd)
    {
        if (fd == EMBED_IO_BAD_FILE_HANDLE)
        {
            // bad index - return an error
            return -1;
        }

        EmbeddedFileData & curEmbeddedFileData = getEmbeddedFileDataByIndex(fd);

        if(curEmbeddedFileData.isFileOpened == false)
        {
           // file is opened - return an error handle
           return -1;
        }
        else
        {
            // success
            curEmbeddedFileData.isFileOpened = false;
            curEmbeddedFileData.curPos = 0;
            return 0;
        }

    }
    //-----------------------------------------------------------------------
    // 读取文件回调
    zzip_ssize_t EmbeddedZipArchiveFactory_read(int fd, void* buf, zzip_size_t len)
    {
        if (fd == EMBED_IO_BAD_FILE_HANDLE)
        {
            // bad index - return an error size - negative
            return -1;
        }
        // get the current buffer in file;
        EmbeddedFileData & curEmbeddedFileData = getEmbeddedFileDataByIndex(fd);
        const Nui8 * curFileData = curEmbeddedFileData.fileData;
        if (len + curEmbeddedFileData.curPos > curEmbeddedFileData.fileSize)
        {
            len = curEmbeddedFileData.fileSize - curEmbeddedFileData.curPos;
        }
        curFileData += curEmbeddedFileData.curPos;

        // copy to out buffer
        memcpy(buf, curFileData, len);

        if( curEmbeddedFileData.decryptFunc != NULL )
        {
            if (!curEmbeddedFileData.decryptFunc(curEmbeddedFileData.curPos, buf, len))
            {
                // decrypt failed - return an error size - negative
                return -1;
            }
        }

        // move the cursor to the new pos
        curEmbeddedFileData.curPos += len;

        return len;
    }
    //-----------------------------------------------------------------------
    // 跳转文件回调
    zzip_off_t EmbeddedZipArchiveFactory_seeks(int fd, zzip_off_t offset, int whence)
    {
        if (fd == EMBED_IO_BAD_FILE_HANDLE)
        {
            // bad index - return an error - nonzero value.
            return -1;
        }

        zzip_size_t newPos = -1;
        // get the current buffer in file;
        EmbeddedFileData & curEmbeddedFileData = getEmbeddedFileDataByIndex(fd);
        switch(whence)
        {
            case SEEK_CUR:
                newPos = (zzip_size_t)(curEmbeddedFileData.curPos + offset);
                break;
            case SEEK_END:
                newPos = (zzip_size_t)(curEmbeddedFileData.fileSize - offset);
                break;
            case SEEK_SET:
                newPos = offset;
                break;
            default:
                // bad whence - return an error - nonzero value.
                return -1;
                break;
        };
        if (newPos >= curEmbeddedFileData.fileSize)
        {
            // bad whence - return an error - nonzero value.
            return -1;
        }

        curEmbeddedFileData.curPos = newPos;
        return newPos;
    }
    //-----------------------------------------------------------------------
    // 返回文件大小回调
    zzip_off_t EmbeddedZipArchiveFactory_filesize(int fd)
    {
        if (fd == EMBED_IO_BAD_FILE_HANDLE)
        {
            // bad index - return an error - nonzero value.
            return -1;
        }
        // get the current buffer in file;
        EmbeddedFileData & curEmbeddedFileData = getEmbeddedFileDataByIndex(fd);
        return curEmbeddedFileData.fileSize;
    }
    //-----------------------------------------------------------------------
    zzip_ssize_t EmbeddedZipArchiveFactory_write(NIIi fd, _zzip_const void * buf, zzip_size_t len)
    {
        // the files in this case are read only - return an error  - nonzero value.
        return -1;
    }
    }
    //-----------------------------------------------------------------------
    EmbeddedZipArchiveFactory::EmbeddedZipArchiveFactory()
    {
        // init static member
        if(mPluginIo == NULL)
        {
            mPluginIo = &sEmbeddedZipArchiveFactory_PluginIo;
            mPluginIo->fd.open = EmbeddedZipArchiveFactory_open;
            mPluginIo->fd.close = EmbeddedZipArchiveFactory_close;
            mPluginIo->fd.read = EmbeddedZipArchiveFactory_read;
            mPluginIo->fd.seeks = EmbeddedZipArchiveFactory_seeks;
            mPluginIo->fd.filesize = EmbeddedZipArchiveFactory_filesize;
            mPluginIo->fd.write = EmbeddedZipArchiveFactory_write;
            mPluginIo->fd.sys = 1;
            mPluginIo->fd.type = 1;
        }
    }
    //-----------------------------------------------------------------------
    EmbeddedZipArchiveFactory::~EmbeddedZipArchiveFactory()
    {
    }
    //-----------------------------------------------------------------------
    VFSType EmbeddedZipArchiveFactory::getType() const
    {
        return VFST_ComplexZip;
    }
    //-----------------------------------------------------------------------
    VFS * EmbeddedZipArchiveFactory::create(const String & prl)
    {
        return N_new ZipArchive(prl, VFST_ComplexZip, mPluginIo);
    }
    //-----------------------------------------------------------------------
    void EmbeddedZipArchiveFactory::addEmbbeddedFile(const String & name,
        const Nui8 * fileData, NCount fileSize, DecryptEmbeddedZipFileFunc decryptFunc)
    {
        static bool needToInit = true;
        if(needToInit)
        {
            needToInit = false;

            // we can't be sure when global variables get initialized
            // meaning it is possible our list has not been init when this
            // function is being called. The solution is to use local
            // static members in this function an init the pointers for the
            // global here. We know for use that the local static variables
            // are create in this stage.
            static FileNameToIndexMap sFileNameToIndexMap;
            static EmbbedFileDataList sEmbbedFileDataList;
            EmbeddedZipArchiveFactory_mFileNameToIndexMap = &sFileNameToIndexMap;
            EmbeddedZipArchiveFactory_mEmbbedFileDataList = &sEmbbedFileDataList;
        }

        EmbeddedFileData newEmbeddedFileData;
        newEmbeddedFileData.curPos = 0;
        newEmbeddedFileData.isFileOpened = false;
        newEmbeddedFileData.fileData = fileData;
        newEmbeddedFileData.fileSize = fileSize;
        newEmbeddedFileData.decryptFunc = decryptFunc;
        EmbeddedZipArchiveFactory_mEmbbedFileDataList->push_back(newEmbeddedFileData);
        (*EmbeddedZipArchiveFactory_mFileNameToIndexMap)[name] = (int)EmbeddedZipArchiveFactory_mEmbbedFileDataList->size();
    }
    //-----------------------------------------------------------------------
    void EmbeddedZipArchiveFactory::removeEmbbeddedFile(const String & name)
    {
        EmbeddedZipArchiveFactory_mFileNameToIndexMap->erase(name);
    }
    //-----------------------------------------------------------------------
}