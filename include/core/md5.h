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

#ifndef _NII_MD5_H_
#define _NII_MD5_H_

#include "NiiPreInclude.h"
#include "NiiString.h"

namespace NII
{
    struct MD5Context 
    {
        Nui32 buf[4];
        Nui32 bytes[2];
        Nui32 in[16];
    };

    class Md5
    {
    public:
        Md5(void);
        ~Md5(void);
        void append(const Nui8 * inBuf, size_t inLen);
        void append(const String & str);
        void getDigest(Nui8 digest[16]);
        void getDigest(String & digest);
    
        /*! \brief Get the MD5 digest of the given text
        \param text text to compute the MD5 for
        \return MD5 digest
        */
        static String GetMD5(const String &text);
    private:
        MD5Context m_ctx;
    };
}

#endif
