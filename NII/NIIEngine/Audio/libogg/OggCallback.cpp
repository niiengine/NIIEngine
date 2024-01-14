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

#include "OggCallback.h"
#include "FileItem.h"

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    COggCallback::COggCallback(XFILE::CFile & file) : m_file(file)
    {
    }
    //------------------------------------------------------------------------
    ov_callbacks COggCallback::Get(const CStdString & strFile)
    {
        // libvorbis requires that a non-seekable stream would always return -1 from seek actions.
        // so for network streams - tweak the seek method to a static one that always return -1.
        CFileItem item(strFile, false);

        ov_callbacks oggIOCallbacks;
        oggIOCallbacks.read_func = ReadCallback;
        oggIOCallbacks.seek_func = item.IsInternetStream()?NoSeekCallback:SeekCallback;
        oggIOCallbacks.tell_func = TellCallback;
        oggIOCallbacks.close_func = CloseCallback;

        return oggIOCallbacks;
    }
    //------------------------------------------------------------------------
    size_t COggCallback::ReadCallback(void * ptr, size_t size, size_t nmemb, void * datasource)
    {
        COggCallback * pCallback=(COggCallback*)datasource;
        if (!pCallback)
            return 0;

        return pCallback->m_file.Read(ptr, size*nmemb);
    }
    //------------------------------------------------------------------------
    int COggCallback::SeekCallback(void * datasource, ogg_int64_t offset, int whence)
    {
        COggCallback * pCallback=(COggCallback*)datasource;
        if (!pCallback)
            return 0;

        return (int)pCallback->m_file.Seek(offset, whence);
    }
    //------------------------------------------------------------------------
    int COggCallback::NoSeekCallback(void * datasource, ogg_int64_t offset, int whence)
    {
        return -1;
    }
    //------------------------------------------------------------------------
    int COggCallback::CloseCallback(void * datasource)
    {
        COggCallback * pCallback=(COggCallback*)datasource;
        if (!pCallback)
            return 0;

        pCallback->m_file.Close();
        return 1;
    }
    //------------------------------------------------------------------------
    long COggCallback::TellCallback(void *datasource)
    {
        COggCallback * pCallback=(COggCallback*)datasource;
        if (!pCallback)
            return 0;

        return (long)pCallback->m_file.GetPosition();
    }
    //------------------------------------------------------------------------
}
}
