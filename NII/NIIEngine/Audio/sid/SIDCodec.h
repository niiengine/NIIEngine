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
#ifndef _NII_SID_CODEC_H_

#include "ICodec.h"
#include "DllSidplay2.h"

namespace NII
{
namespace NII_MEDIA
{
    class SIDCodec : public ICodec
    {
    public:
        SIDCodec();
        virtual ~SIDCodec();

        virtual bool Init(const CStdString & strFile, unsigned int filecache);
        virtual void DeInit();
        virtual int64_t Seek(int64_t iSeekTime);
        virtual int ReadPCM(BYTE * pBuffer, int size, int *actualsize);
        virtual bool CanInit();
        virtual AudioChannel GetChannelInfo();

        virtual void SetTotalTime(int64_t totaltime)
        {
            m_TotalTime = totaltime*1000;
        }
    private:
        DllSidplay2 m_dll;
        void * m_sid;
        int m_iTrack;
        int64_t m_iDataPos;
    };
}
}
#endif