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
#include "NiiLibArchive.h"
#include "NiiLogManager.h"
#include "NiiException.h"
#include "NiiString.h"
#include "NiiEngine.h"

#include <archive.h>
#include <archive_entry.h>

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // LibArchive
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    LibArchive::LibArchive(const String & name, const String & type) :
        VFS(name, type)
    {
    }
    //-----------------------------------------------------------------------
    LibArchive::~LibArchive()
    {
        N_mutex1_lock
        mFileList.clear();
    }
    //-----------------------------------------------------------------------
    void LibArchive::peek()
    {
        N_mutex1_lock
        // 临时对象
        ARCHIVE_ENTRY * entry;

        ARCHIVE * archive = archive_read_new();

        archive_read_support_compression_all(archive);
        archive_read_support_format_all(archive);

        ARCHIVE_ERROR lasterror = archive_read_open_filename(archive, mName.c_str(), 10240);
        checkLibArchiveError(lasterror, _I18n("opening archive"));

        while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
        {
            FileInfo info;
            info.mSrc = this;

            // 获取包中路径名
            info.mFullName = archive_entry_pathname(entry);
            StrUtil::splitFilename(info.mFullName, info.mFileName, info.mPath);

            info.mSize = static_cast<NCount>(archive_entry_size(entry));
            if(info.mFileName.empty())
            {
                info.mFullName = info.mFullName.substr (0, info.mFullName.length () - 1);
                StrUtil::splitFilename(info.mFullName, info.mFileName, info.mPath);
                // 如果是文件夹设置压缩大小为 -1
                info.mCompressSize = NCount (-1);
            }
            mFileList.push_back(info);
            archive_read_data_skip(archive);
        }
        lasterror = archive_read_finish(mArchive);
        checkLibArchiveError(lasterror, _I18n("closeing archive"));
    }
    //-----------------------------------------------------------------------
    DataStream * LibArchive::open(const String & filename, bool readOnly) const
    {
        N_mutex1_lock
        FileInfoList::iterator i, iend = mFileList.end();
        bool exist = false;
        for(i = mFileList.begin(); i != iend; ++i)
        {
            if(filename == i->mFullName)
            {
                exist = true;
            }
        }
        if(exist)
        {
            ARCHIVE_ENTRY * entry;
            ARCHIVE * archive = archive_read_new();
            archive_read_support_compression_all(archive);
            archive_read_support_format_all(archive);

            NCount r = 0;
            // wsting??
            lasterror = archive_read_open_filename(archive, mName.c_str(), 16380);
            checkLibArchiveError(lasterror, _I18n("opening archive"));

            while(archive_read_next_header(a, &entry) == ARCHIVE_OK)
            {
                String _filename,basename,path;

                // 获取 基本名/路径
                _filename = archive_entry_pathname(entry);
                StrUtil::splitFilename(_filename, basename, path);

                if (basename != false)
                {
                    if (filename == basename)
                    {
                        return N_new LibArchiveDataStream(filename,
                            archive, entry, archive_entry_size(entry));
                    }
                }
            }
        }
        return 0;
    }
    //----------------------------------------------------------------------
    DataStream * LibArchive::create(const String & filename) const
    {
        N_EXCEPT(UnImpl, _I18n("不支持创建文件"));
    }
    //----------------------------------------------------------------------
    void LibArchive::remove(const String & filename) const
    {
        N_EXCEPT(UnImpl, _I18n("不支持删除文件"));
    }
    //----------------------------------------------------------------------
    void LibArchive::list(StringList & out, bool r, bool dirs)
    {
        N_mutex1_lock

        FileInfoList::iterator i, iend = mFileList.end();
        for(i = mFileList.begin(); i != iend; ++i)
        {
            if((dirs == (i->mCompressSize == NCount (-1))) && (r || i->mPath.empty()))
            {
                out.push_back(i->mFullName);
            }
        }
    }
    //----------------------------------------------------------------------
    void LibArchive::list(FileInfoList & out, bool r, bool dirs)
    {
        N_mutex1_lock

        FileInfoList::const_iterator i, iend = mFileList.end();
        for(i = mFileList.begin(); i != iend; ++i)
        {
            if((dirs == (i->mCompressSize == NCount (-1))) && (r || i->mPath.empty()))
            {
                out.push_back(*i);
            }
        }
    }
    //----------------------------------------------------------------------
    void LibArchive::find(StringList & out, const String & pattern, bool r, bool dirs)
    {
        N_mutex1_lock

        // If pattern contains a directory name, do a full match
        bool full_match = (pattern.find ('/') != String::npos) ||
            (pattern.find ('\\') != String::npos);

        FileInfoList::iterator i, iend = mFileList.end();
        for(i = mFileList.begin(); i != iend; ++i)
        {
            if((dirs == (i->mCompressSize == NCount (-1))) && (r || full_match || i->mPath.empty()))
            {
                // Check basename matches pattern (zip is case insensitive)
                if(StrUtil::match(full_match ? i->mFullName : i->mFileName, pattern, false))
                {
                    out.push_back(i->mFullName);
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void LibArchive::find(FileInfoList & out, const String & pattern, bool r, bool dirs)
    {
        N_mutex1_lock

        // If pattern contains a directory name, do a full match
        bool full_match = (pattern.find ('/') != String::npos) ||
            (pattern.find ('\\') != String::npos);

        FileInfoList::iterator i, iend = mFileList.end();
        for(i = mFileList.begin(); i != iend; ++i)
        {
            if((dirs == (i->mCompressSize == NCount (-1))) &&
                (r || full_match || i->mPath.empty()))
            {
                // Check name matches pattern (zip is case insensitive)
                if(StrUtil::match(full_match ? i->mFullName : i->mFileName, pattern, false))
                {
                    out.push_back(*i);
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    bool LibArchive::isExist(const String & filename)
    {
        //libarchivelib is not threadsafe
        N_mutex1_lock

        ARCHIVE_ERROR archiveError;
        ARCHIVE_ENTRY *entry;

        mArchive = archive_read_new();

        archive_read_support_compression_all(mArchive);
        archive_read_support_format_all(mArchive);
        //wsting??
        archiveError = archive_read_open_filename(mArchive, mName.c_str(), 10240);
        checkLibArchiveError(archiveError, _I18n("opening archive"));
        // 找到？
        bool found = false;
        while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
        {
            String basename, path, _filename;

            // Get basename / path
            _filename = archive_entry_pathname(entry);
            StrUtil::splitFilename(_filename, basename, path);

            if(basename == filename)
            {
                found = true;
                break;
            }
            archive_read_data_skip(mArchive);
        }
        archiveError = archive_read_finish(mArchive);
        checkLibArchiveError(archiveError, _I18n("closeing archive"));
        return found;
    }
    //---------------------------------------------------------------------
    time_t LibArchive::getModifiedTime(const String & filename)
    {
        struct stat tagStat;
        bool ret = (stat(mName.c_str(), &tagStat) == 0);

        if (ret)
        {
            return tagStat.st_mtime;
        }
        else
        {
            return 0;
        }
    }
    //----------------------------------------------------------------------
    void checkLibArchiveError(NIIi libarchiveError, const String & operation) const
    {
        if (libarchiveError != ARCHIVE_OK)
        {
            String errorMsg;
            switch (libError)
            {
            case ARCHIVE_FATAL:
                errorMsg = _I18n("ARCHIVE Have A Error");
                break;
            case ARCHIVE_FAILED:
                errorMsg = _I18n("Operations on this entry are impossible");
                break;
            case ARCHIVE_WARN:
                errorMsg = _I18n("Occurred a WARNING");
                break;
            default:
                errorMsg = _I18n("Unknown error.");
                break;
            };

            N_EXCEPT(Internal,
                mName + _I18n(" - error whilst ") + operation + ": " + errorMsg);
        }
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // LibArchiveDataStream
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    LibArchiveDataStream::LibArchiveDataStream(const ARCHIVE * archive,
        const ARCHIVE_ENTRY * file, NCount size) :
            mArchive(archive),
            mFile(file)
    {
        mSize = size;
    }
    //---------------------------------------------------------------------
    LibArchiveDataStream::LibArchiveDataStream(const String & name,
        const ARCHIVE * libFile, const ARCHIVE_ENTRY * file, NCount size) :
            DataStream(name),
            mArchive(archive),
            mFile(file)
    {
        mSize = size;
    }
    //---------------------------------------------------------------------
    LibArchiveDataStream::~LibArchiveDataStream()
    {
        close();
    }
    //---------------------------------------------------------------------
    NCount LibArchiveDataStream::read(void * buf, NCount count) const
    {
        NCount was_avail = mCache.read(buf, count);
        NCount readsize;
        if (was_avail < count)
        {
            readsize = archive_read_data(mArchive, (NIIb*)buf + was_avail, count - was_avail);
            if (readsize < 0)
            {
                String msg = checkLibArchiveError(readsize, _I18n("reading libarchive"));
                N_EXCEPT(Internal, mName + _I18n(" - error from libarchive: ")
                    + msg);
            }
            mCache.cacheData((NIIb*)buf + was_avail, (NCount)readsize);
        }
        return was_avail + (NCount)readsize;
    }
    //---------------------------------------------------------------------
    NCount LibArchiveDataStream::write(const void* buf, NCount count)
    {
        //todo
    }
    //---------------------------------------------------------------------
    void LibArchiveDataStream::skip(NIIl count)
    {
        //notsupport

    //---------------------------------------------------------------------
    void LibArchiveDataStream::seek(NCount pos)
    {
        //notsupport
    }
    //---------------------------------------------------------------------
    NCount LibArchiveDataStream::tell(void) const
    {
        //notsupport
    }
    //---------------------------------------------------------------------
    bool LibArchiveDataStream::eof(void) const
    {
        //notsupport
    }
    //---------------------------------------------------------------------
    void LibArchiveDataStream::close(void)
    {
        if(mArchive != 0)
        {
            lasterror = archive_read_finish(mArchive);
            checkLibArchiveError(lasterror, _I18n("closeing archive"));
            mArchive = 0;
            mFile = 0;
        }
        mCache.clear();
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // LibArchiveFactory
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    LibArchiveFactory::LibArchiveFactory()
    {
    }
    //---------------------------------------------------------------------
    LibArchiveFactory::~LibArchiveFactory()
    {
    }
    //---------------------------------------------------------------------
    VFSType LibArchiveFactory::getType() const
    {
        return VFST_Archive;
    }
    //---------------------------------------------------------------------
    VFS * LibArchiveFactory::create(const String & prl)
    {
        return N_new LibArchive(prl, VFST_Archive);
    }
    //---------------------------------------------------------------------
    void LibArchiveFactory::destroy(VFS * obj)
    {
        N_delete obj;
    }
    //---------------------------------------------------------------------
}