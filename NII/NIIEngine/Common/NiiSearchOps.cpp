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

// Emulate _findfirst, _findnext on non-Windows platforms

#include "NiiPreProcess.h"
#include "NiiSearchOps.h"
#include <stdio.h>
#include <ctype.h>

#if N_PLAT == N_PLAT_SYMBIAN
#include "NiiString.h"

// SYMBIAN todo - possibly use - CDirScan from C:\Symbian\9.2\S60_3rd_FP1\Epoc32\include\f32file.h
// see this sample - http://wiki.forum.nokia.com/index.php/Find_Files

bool fnmatch (NII::String pattern, NII::String name, NIIi dummy)
{
    if (pattern == "*")
    {
        return true;
    }
    if (pattern.substr(0,2) == "*.")
    {
        NII::StrUtil::toLower(pattern);
        NII::StrUtil::toLower(name);
        NII::String extToFind = pattern.substr(2, pattern.size() - 2);
        if ((name.size() > extToFind.size()) &&(extToFind == name.substr(name.size() - extToFind.size(), extToFind.size())))
        {
            return 0; // match
        }
        else
        {
            return 1; // don't match
        }
    }
    return false;
}
#endif
/* Win32 directory operations emulation */
#if N_PLAT != N_PLAT_WIN32

struct _find_search_t
{
    NIIb *pattern;
    NIIb *curfn;
    NIIb *directory;
    NIIi dirlen;
    DIR *dirfd;
};
//-------------------------------------------------------------
intptr_t _findfirst(const char *pattern, struct _finddata_t *data)
{
    _find_search_t *fs = new _find_search_t;
    fs->curfn = NULL;
    fs->pattern = NULL;

    // Separate the mask from directory name
    const NIIb *mask = strrchr (pattern, '/');
    if (mask)
    {
        fs->dirlen = mask - pattern;
        mask++;
        fs->directory = (NIIb *)malloc (fs->dirlen + 1);
        memcpy (fs->directory, pattern, fs->dirlen);
        fs->directory [fs->dirlen] = 0;
    }
    else
    {
        mask = pattern;
        fs->directory = strdup (".");
        fs->dirlen = 1;
    }

    fs->dirfd = opendir (fs->directory);
    if (!fs->dirfd)
    {
        _findclose ((intptr_t)fs);
        return -1;
    }

    /* Hack for "*.*" -> "*' from DOS/Windows */
    if (strcmp (mask, "*.*") == 0)
        mask += 2;
    fs->pattern = strdup (mask);

    /* Get the first entry */
    if (_findnext ((intptr_t)fs, data) < 0)
    {
        _findclose ((intptr_t)fs);
        return -1;
    }

    return (intptr_t)fs;
}
//--------------------------------------------------------------
NIIi _findnext(intptr_t id, struct _finddata_t * data)
{
    _find_search_t * fs = (_find_search_t *)id;

    /* Loop until we run out of entries or find the next one */
    dirent * entry;
    for (;;)
    {
        if (!(entry = readdir (fs->dirfd)))
            return -1;

        /* See if the filename matches our pattern */
        if (fnmatch (fs->pattern, entry->d_name, 0) == 0)
            break;
    }

    if (fs->curfn)
        free (fs->curfn);
    data->name = fs->curfn = strdup (entry->d_name);

    NCount namelen = strlen (entry->d_name);
    NIIb *xfn = new NIIb [fs->dirlen + 1 + namelen + 1];
    sprintf (xfn, "%s/%s", fs->directory, entry->d_name);

    /* stat the file to get if it's a subdir and to find its length */
    struct stat stat_buf;
    if (stat (xfn, &stat_buf))
    {
        // Hmm strange, imitate a zero-length file then
        data->attrib = _A_NORMAL;
        data->size = 0;
    }
    else
    {
        if (S_ISDIR(stat_buf.st_mode))
            data->attrib = _A_SUBDIR;
        else
            /* Default type to a normal file */
            data->attrib = _A_NORMAL;

        data->size = stat_buf.st_size;
    }

    delete [] xfn;

    /* Files starting with a dot are hidden files in Unix */
    if (data->name [0] == '.')
        data->attrib |= _A_HIDDEN;

    return 0;
}
//------------------------------------------------------------------
NIIi _findclose(intptr_t id)
{
    NIIi ret;
    _find_search_t *fs = (_find_search_t *)id;
    
    ret = fs->dirfd ? closedir (fs->dirfd) : 0;
    free (fs->pattern);
    free (fs->directory);
    if (fs->curfn)
        free (fs->curfn);
    delete fs;

    return ret;
}
//------------------------------------------------------------------
#endif
