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

#ifndef _NII_WAV_CODEC_H_
#define _NII_WAV_CODEC_H_

#include "ICodec.h"
#include "CachingCodec.h"
#include "ADPCMCodec.h"

namespace NII
{
namespace NII_MEDIA
{
    class WAVCodec : public CachingCodec
    {
    public:
        WAVCodec();
        virtual ~WAVCodec();

        virtual bool Init(const CStdString &strFile, unsigned int filecache);
        virtual void DeInit();
        virtual int64_t Seek(int64_t iSeekTime);
        virtual int ReadPCM(BYTE *pBuffer, int size, int *actualsize);
        virtual bool CanInit();
        virtual AudioChannel GetChannelInfo();

    private:
        uint32_t m_iDataStart;
        uint32_t m_iDataLen;
        DWORD m_ChannelMask;
    };
}
}
#endif