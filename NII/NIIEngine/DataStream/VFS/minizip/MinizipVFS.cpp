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
#include "NiiException.h"
#include "MinizipVFS.h"
#include "NiiString.h"
// exclude code on apple unless feature macro is defined.
#if !defined(__APPLE__) || defined(CEGUI_HAS_MINIZIP_RESOURCE_PROVIDER)

#include <fstream>

#if defined (__WIN32__) || defined(_WIN32)
    #include <Shlwapi.h>
    #define FNMATCH(p, s) PathMatchSpec(s, p)
    #ifdef _MSC_VER
        #pragma comment(lib, "shlwapi.lib")
    #endif
#else
    #include <fnmatch.h>
    #define FNMATCH(p, s) fnmatch(p, s, FNM_PATHNAME)
#endif

namespace NII
{
    //------------------------------------------------------------------------
    MinizipVFS::MinizipVFS(const String & name, VFSType type) :
        d_archive(prl)
    {
    }
    //------------------------------------------------------------------------
    MinizipVFS::~MinizipVFS()
    {
        if(d_zfile)
        {
            if(unzClose(d_zfile) != Z_OK)
            {
                N_Only(Log).log(d_archive + _I18n("' error upon closing"));
            }

            d_zfile = 0;
        }
    }
    //------------------------------------------------------------------------
    bool MinizipVFS::isExist(const String & filename)
    {
        std::ifstream dataFile(filename.c_str(), std::ios::binary | std::ios::ate);

        if(dataFile)
            return true;

        return false;
    }
    //------------------------------------------------------------------------
    void MinizipVFS::peek()
    {
        d_zfile = unzOpen(d_archive.c_str());

        if(d_zfile == 0)
        {
            N_EXCEPT(InvalidRequest, "'" + d_archive + _I18n("' does not exist"));
        }
    }
    //------------------------------------------------------------------------
    DataStream * MinizipVFS::open(const String & filename, bool r)
    {
        if(d_zfile == 0)
        {
            N_EXCEPT(InvalidRequest, "'" + filename +
                _I18n("' cannot be loaded because the archive has not been set"));
        }

        if(unzLocateFile(d_zfile, filename.c_str(), 0) != UNZ_OK)
        {
            N_EXCEPT(InvalidRequest,"'" + filename + _I18n("' does not exist"));
        }

        unz_file_info info;

        if(unzGetCurrentFileInfo(d_zfile, &info, 0, 0, 0, 0, 0, 0) != UNZ_OK)
        {
            N_EXCEPT(IO, "'" + filename + _I18n("' error reading file header"));
        }

        if(unzOpenCurrentFile(d_zfile) != Z_OK)
        {
            N_EXCEPT(IO, "'" + filename + _I18n("' error opening file"));
        }

        Nul size = info.uncompressed_size;
        Nui8 * buffer = N_alloc_t(Nui8, size);

        if(unzReadCurrentFile(d_zfile, buffer, size) < 0)
        {
            N_EXCEPT(IO,"'" + filename + _I18n("' error reading file"));
        }

        if(unzCloseCurrentFile(d_zfile) != UNZ_OK)
        {
            N_EXCEPT(Generic, "'" + filename + _I18n("' error validating file"));
        }
        return N_new MemStream(buffer, size, true);
    }
    //-----------------------------------------------------------------------
    void MinizipVFS::find(StringList & out, const String & pattern, bool r, bool dirs)
    {
        bool full_match = (pattern.find ('/') != String::npos) ||
            (pattern.find ('\\') != String::npos);

        // exit now if no zip file is loaded
        if(!d_zfile)
            return;

        char current_name[1024];
        unz_file_info info;

        if(unzGoToFirstFile(d_zfile) != UNZ_OK)
        {
            N_Only(Log).log(_I18n("unzGoToFirstFile failed, skipping zip file scan."));
        }

        do
        {
            if(unzGetCurrentFileInfo(d_zfile, &info, current_name, 1024, 0, 0, 0, 0) != UNZ_OK)
            {
                N_Only(Log).log(_I18n("unzGetCurrentFileInfo failed, terminating scan."));
            }

            String tname, tpath;
            StrUtil::splitFilename(current_name, tname, tpath);

            if((dirs == (info.compressed_size == Nul(-1))) &&
                (r || full_match || tpath.empty()))
            {
                if(StrUtil::match(full_match ? current_name : tname, pattern, false))
                    out.push_back(current_name);
            }
        }
        while(unzGoToNextFile(d_zfile) == UNZ_OK);
    }
    //-----------------------------------------------------------------------
    MinizipVFSFactory::MinizipVFSFactory()
    {

    }
    //-----------------------------------------------------------------------
    MinizipVFSFactory::~MinizipVFSFactory()
    {
    }
    //-----------------------------------------------------------------------
    VFSType MinizipVFSFactory::getType() const
    {
        return VFST_Zip;
    }
    //-----------------------------------------------------------------------
    VFS * MinizipVFSFactory::create(const String & prl)
    {
        return MinizipVFS(prl, VFST_Zip);
    }
    //-----------------------------------------------------------------------
    void MinizipVFSFactory::destroy(VFS * obj)
    {
        N_delete obj;
    }
    //-----------------------------------------------------------------------
}
#endif