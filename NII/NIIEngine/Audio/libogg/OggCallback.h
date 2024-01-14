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

#include "DllVorbisfile.h"
#include "filesystem/File.h"

namespace NII
{
namespace NII_MEDIA
{
    class COggCallback
    {
    public:
        COggCallback(XFILE::CFile & file);

        ov_callbacks Get(const CStdString & strFile);

        static size_t ReadCallback(void * ptr, size_t size, size_t nmemb, void * datasource);
        static int SeekCallback(void * datasource, ogg_int64_t offset, int whence);
        static int NoSeekCallback(void * datasource, ogg_int64_t offset, int whence);
        static int CloseCallback(void * datasource);
        static long TellCallback(void * datasource);
    protected:
      XFILE::CFile & m_file;
    };
}
}