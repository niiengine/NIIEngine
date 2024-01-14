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
#include "NiiVFS.h"
#include "NiiException.h"

#include "NiiLogManager.h"
#include "NiiDefaultStream.h"
#include "NiiEngine.h"

#include <sys/types.h>
#include <sys/stat.h>

#if N_PLAT == N_PLAT_LINUX || N_PLAT == N_PLAT_OSX || \
    N_PLAT == N_PLAT_SYMBIAN || N_PLAT == N_PLAT_IOS || \
    N_PLAT == N_PLAT_ANDROID
    #include "NiiSearchOps.h"
    #include <sys/param.h>
    #define MAX_PATH MAXPATHLEN
#endif

#if N_PLAT == N_PLAT_WIN32
    #define WIN32_LEAN_AND_MEAN
    #if !defined(NOMINMAX) && defined(_MSC_VER)
        #define NOMINMAX // required to stop windows.h messing up std::min
    #endif
    #include <windows.h>
    #include <direct.h>
    #include <io.h>
#endif

#ifndef MAX_PATH
#define MAX_PATH 1024
#endif

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // VFS
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    VFS::VFS(const String & name, VFSType type):
        mName(name),
        mType(type),
        mReadOnly(true)
    {
    }
    //------------------------------------------------------------------------
    VFS::~VFS()
    {
    }
    //------------------------------------------------------------------------
    bool VFS::isReadOnly() const
    {
        return mReadOnly;
    }
    //------------------------------------------------------------------------
    DataStream * VFS::create(const String & file)
    {
       (void)file;
        N_EXCEPT(UnImpl, _I18n("没有实现"));
    }
    //------------------------------------------------------------------------
    void VFS::remove(const String & file)
    {
       (void)file;
        N_EXCEPT(UnImpl, _I18n("没有实现"));
    }
    //------------------------------------------------------------------------
    time_t VFS::getCreateTime(const String & file) const
    {
        return time(NULL);
    }
    //------------------------------------------------------------------------
    time_t VFS::getModifiedTime(const String & file) const
    {
        return time(NULL);
    }
    //------------------------------------------------------------------------
    time_t VFS::getAccessTime(const String & file) const
    {
        return time(NULL);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // FileSystem
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool FileSystem::mHiddenFail = true;
    //-----------------------------------------------------------------------
    FileSystem::FileSystem(const String & name) :
        VFS(name, VFST_Local)
    {
    }
    //-----------------------------------------------------------------------
    bool FileSystem::isMatchCase() const
    {
    #if N_PLAT == N_PLAT_WIN32
        return false;
    #else
        return true;
    #endif
    }
    //-----------------------------------------------------------------------
    static bool is_reserved_dir (const Ntchar * fn)
    {
        return (fn [0] == '.' && (fn [1] == 0 || (fn [1] == '.' && fn [2] == 0)));
    }
    //-----------------------------------------------------------------------
    static bool is_absolute_path(const Ntchar * path)
    {
    #if N_PLAT == N_PLAT_WIN32
        if (isalpha(Nui8(path[0])) && path[1] == ':')
            return true;
    #endif
        return path[0] == '/' || path[0] == '\\';
    }
    //-----------------------------------------------------------------------
    static String concatenate_path(const String & base, const String& name)
    {
        if (base.empty() || is_absolute_path(name.c_str()))
            return name;
        else
            return base + _T('/') + name;
    }
    //-----------------------------------------------------------------------
    void FileSystem::findFiles(const String & pattern, bool r,
        bool dirs, StringList * simpleList, FileInfoList * detailList) const
    {
        intptr_t lHandle, res;
        struct Nfinddata tagData;

        // pattern can contain a directory name, separate it from mask
        NCount pos1 = pattern.rfind ('/');
        NCount pos2 = pattern.rfind ('\\');
        if (pos1 == pattern.npos || ((pos2 != pattern.npos) && (pos1 < pos2)))
            pos1 = pos2;
        String directory;
        if (pos1 != pattern.npos)
            directory = pattern.substr (0, pos1 + 1);

        String full_pattern = concatenate_path(mName, pattern);

        lHandle = Nfindfirst(full_pattern.c_str(), &tagData);
        res = 0;
        while (lHandle != -1 && res != -1)
        {
            if ((dirs == ((tagData.attrib & _A_SUBDIR) != 0)) &&
                ( !mHiddenFail || (tagData.attrib & _A_HIDDEN) == 0 ) &&
                (!dirs || !is_reserved_dir (tagData.name)))
            {
                if (simpleList)
                {
                    simpleList->push_back(directory + tagData.name);
                }
                else if (detailList)
                {
                    FileInfo fi;
                    fi.mSrc = const_cast<FileSystem *>(this);
                    fi.mFullName = directory + tagData.name;
                    fi.mFileName = tagData.name;
                    fi.mPath = directory;
                    fi.mCompressSize = tagData.size;
                    fi.mSize = tagData.size;
                    detailList->push_back(fi);
                }
            }
            res = Nfindnext(lHandle, &tagData);
        }
        // Close if we found any files
        if(lHandle != -1)
            _findclose(lHandle);

        // Now find directories
        if (r)
        {
            String base_dir = mName;
            if (!directory.empty ())
            {
                base_dir = concatenate_path(mName, directory);
                // Remove the last '/'
                base_dir.erase (base_dir.length () - 1);
            }
            base_dir.append (_T("/*"));

            // Remove directory name from pattern
            String mask (_T("/"));
            if (pos1 != pattern.npos)
                mask.append (pattern.substr (pos1 + 1));
            else
                mask.append (pattern);

            lHandle = Nfindfirst(base_dir.c_str (), &tagData);
            res = 0;
            while (lHandle != -1 && res != -1)
            {
                if ((tagData.attrib & _A_SUBDIR) &&
                    ( !mHiddenFail || (tagData.attrib & _A_HIDDEN) == 0 ) &&
                        !is_reserved_dir(tagData.name))
                {
                    // recurse
                    base_dir = directory;
                    base_dir.append (tagData.name).append (mask);
                    findFiles(base_dir, r, dirs, simpleList, detailList);
                }
                res = Nfindnext(lHandle, &tagData);
            }
            // Close if we found any files
            if(lHandle != -1)
                _findclose(lHandle);
        }
    }
    //-----------------------------------------------------------------------
    FileSystem::~FileSystem()
    {
    }
    //-----------------------------------------------------------------------
    void FileSystem::peek()
    {
        N_mutex1_lock
        // test to see if this folder is writeable
        String testPath = concatenate_path(mName, _T("__testwrite.nii"));
        Nofstream writeStream;
        writeStream.open(testPath.c_str());
        if(writeStream.fail())
        {
            mReadOnly = true;
        }
        else
        {
            mReadOnly = false;
            writeStream.close();
            Nremove(testPath.c_str());
        }
    }
    //-----------------------------------------------------------------------
    DataStream * FileSystem::open(const String & filename, bool readOnly)
    {
        String full_path = concatenate_path(mName, filename);

        // Use filesystem to determine size
        // (quicker than streaming to the end and back)
        struct _stat64i32 tagStat;
        NIIi ret = Nstat(full_path.c_str(), &tagStat);
        N_assert(ret == 0, "Problem getting file size");
        (void)ret;  // Silence warning

        // Always open in binary mode
        // Also, always include reading
        std::ios::openmode mode = std::ios::in | std::ios::binary;
        std::istream * baseStream = 0;
        std::ifstream * roStream = 0;
        std::fstream * rwStream = 0;

        if (!readOnly && isReadOnly())
        {
            mode |= std::ios::out;
            rwStream = N_new_t(std::fstream)();
            rwStream->open(full_path.c_str(), mode);
            baseStream = rwStream;
        }
        else
        {
            roStream = N_new_t(std::ifstream)();
            roStream->open(full_path.c_str(), mode);
            baseStream = roStream;
        }


        // Should check ensure open succeeded, in case fail for some reason.
        if (baseStream->fail())
        {
            N_delete_t(roStream, basic_ifstream);
            N_delete_t(rwStream, basic_fstream);
            N_EXCEPT(InvalidFile, _I18n("Cannot open file: ") + filename);
        }

        // Construct return stream, tell it to delete on destroy
        FileDataStream * stream = 0;
        if(rwStream)
        {
            // use the writeable stream
            stream = N_new FileDataStream(rwStream, tagStat.st_size, filename,  true);
        }
        else
        {
            // read-only stream
            stream = N_new FileDataStream(roStream, tagStat.st_size, filename, true);
        }
        return stream;
    }
    //---------------------------------------------------------------------
    DataStream * FileSystem::create(const String & filename)
    {
        if(isReadOnly())
        {
            N_EXCEPT(InvalidParam, _I18n("Cannot create a file in a read-only archive"));
        }

        String full_path = concatenate_path(mName, filename);

        // Always open in binary mode
        // Also, always include reading
        std::ios::openmode mode = std::ios::out | std::ios::binary;
        std::fstream * rwStream = N_new_t(std::fstream)();
        rwStream->open(full_path.c_str(), mode);

        // Should check ensure open succeeded, in case fail for some reason.
        if(rwStream->fail())
        {
            N_delete_t(rwStream, basic_fstream);
            N_EXCEPT(InvalidFile, _I18n("Cannot open file: ") + filename);
        }

        // Construct return stream, tell it to delete on destroy
        FileDataStream * stream = N_new FileDataStream(rwStream, 0, filename, true);

        return stream;
    }
    //---------------------------------------------------------------------
    void FileSystem::remove(const String & filename)
    {
        if(isReadOnly())
        {
            N_EXCEPT(InvalidParam, _I18n("Cannot remove a file from a read-only archive"));
        }
        String full_path = concatenate_path(mName, filename);
        Nremove(full_path.c_str());
    }
    //-----------------------------------------------------------------------
    void FileSystem::close()
    {

    }
    //-----------------------------------------------------------------------
    void FileSystem::list(StringList & out, bool r, bool dirs) const
    {
        findFiles(_T("*"), r, dirs, &out, 0);
    }
    //-----------------------------------------------------------------------
    void FileSystem::list(FileInfoList & out, bool r, bool dirs) const
    {
        findFiles(_T("*"), r, dirs, 0, &out);
    }
    //-----------------------------------------------------------------------
    void FileSystem::find(StringList & out, const String & pattern, bool r, bool dirs) const
    {
        findFiles(pattern, r, dirs, &out, 0);
    }
    //-----------------------------------------------------------------------
    void FileSystem::find(FileInfoList & out, const String & pattern, bool r, bool dirs) const
    {
        findFiles(pattern, r, dirs, 0, &out);
    }
    //-----------------------------------------------------------------------
    bool FileSystem::isExist(const String & filename) const
    {
        String full_path = concatenate_path(mName, filename);

        struct _stat64i32 tagStat;
        bool ret = (Nstat(full_path.c_str(), &tagStat) == 0);

        // stat will return true if the filename is absolute, but we need to check
        // the file is actually in this archive
        if (ret && is_absolute_path(filename.c_str()))
        {
            // only valid if full path starts with our base
        #if N_PLAT == N_PLAT_WIN32
            // case insensitive on windows
            String lowerCaseName = mName;
            StrUtil::toLower(lowerCaseName);
            ret = StrUtil::beginsWith(full_path, lowerCaseName, true);
        #else
            // case sensitive
            ret = StrUtil::beginsWith(full_path, mName, false);
        #endif
        }

        return ret;
    }
    //---------------------------------------------------------------------
    time_t FileSystem::getModifiedTime(const String & filename)
    {
        String full_path = concatenate_path(mName, filename);

        struct _stat64i32 tagStat;
        bool ret = (Nstat(full_path.c_str(), &tagStat) == 0);

        if (ret)
        {
            return tagStat.st_mtime;
        }
        else
        {
            return 0;
        }

    }
    //-----------------------------------------------------------------------
    bool FileSystem::getIgnoreHidden()
    {
        return mHiddenFail;
    }
    //-----------------------------------------------------------------------
    void FileSystem::setIgnoreHidden(bool ignore)
    {
        mHiddenFail = ignore;
    }
    //-----------------------------------------------------------------------
    bool FileSystem::wirteFile(const String & path, Nui8 * data, NCount dataLength)
    {
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4996 )
#endif
        Ntchar pathCopy[MAX_PATH];
        int res;

        if (path.empty())
            return false;

        Nstrcpy(pathCopy, path.c_str());

        // Ignore first / if there is one
        if (pathCopy[0])
        {
            int index = 1;
            while(pathCopy[index])
            {
                if(pathCopy[index] == '/' || pathCopy[index] == '\\')
                {
                    pathCopy[index] = 0;
        
                    res = Nmkdir(pathCopy);
                    
                    if (res<0 && errno!=EEXIST && errno!=EACCES)
                    {
                        return false;
                    }
        
                    pathCopy[index] = '/';
                }
        
                index++;
            }
        }

        if (data)
        {
            FILE * fp = Nfopen(path.c_str(), _T("wb"));

            if ( fp == 0 )
            {
                return false;
            }

            fwrite(data, 1, dataLength, fp);

            fclose(fp );
        }
        else
        {
            res = Nmkdir(pathCopy);

            if(res < 0 && errno != EEXIST)
            {
                return false;
            }
        }
#ifdef _MSC_VER
#pragma warning( pop )
#endif
        return true;
    }
    //-----------------------------------------------------------------------
    bool FileSystem::isPathSlash(Ntchar c)
    {
        return c=='/' || c=='\\';
    }
    //-----------------------------------------------------------------------
    void FS__AddSlash(Ntchar * input)
    {
        if (input==0 || input[0]==0)
            return;

        int lastCharIndex=(int) Nstrlen(input)-1;
        if (input[lastCharIndex]=='\\')
            input[lastCharIndex]='/';
        else if (input[lastCharIndex]!='/')
        {
            input[lastCharIndex+1]='/';
            input[lastCharIndex+2]=0;
        }
    }
    //-----------------------------------------------------------------------
    bool FileSystem::isPathExist(const String & directory)
    {
		Nfinddata fileInfo;
        Ntchar baseDirWithStars[560];
        Nstrcpy(baseDirWithStars, directory.c_str());
        FS__AddSlash(baseDirWithStars);
        Nstrcat(baseDirWithStars, _T("*.*"));
        intptr_t dir = Nfindfirst(baseDirWithStars, &fileInfo );
        if (dir == -1)
            return false;
        _findclose(dir);
        return true;
    }
    //-----------------------------------------------------------------------
    Nui32 FileSystem::getFileSize(const String & path)
    {
        FILE * fp = Nfopen(path.c_str(), _T("rb"));
        if(fp==0) 
            return 0;
        fseek(fp, 0, SEEK_END);
        Nui32 fileLength = ftell(fp);
        fclose(fp);
        return fileLength;

    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // FileSystemArchiveFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    FileSystemArchiveFactory::FileSystemArchiveFactory()
    {
    }
    //-----------------------------------------------------------------------
    FileSystemArchiveFactory::~FileSystemArchiveFactory()
    {
    }
    //-----------------------------------------------------------------------
    VFSType FileSystemArchiveFactory::getType() const
    {
        return VFST_Local;
    }
    //-----------------------------------------------------------------------
    VFS * FileSystemArchiveFactory::create(const String & prl)
    {
        return N_new FileSystem(prl);
    }
    //-----------------------------------------------------------------------
    void FileSystemArchiveFactory::destroy(VFS * obj)
    {
        N_delete obj;
    }
    //-----------------------------------------------------------------------
}