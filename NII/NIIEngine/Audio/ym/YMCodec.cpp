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

#include "YMCodec.h"
#include "cores/DllLoader/DllLoader.h"
#include "utils/log.h"

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    YMCodec::YMCodec()
    {
        m_CodecName = "ym";
        m_ym = 0;
        m_iDataPos = -1;
    }
    //------------------------------------------------------------------------
    YMCodec::~YMCodec()
    {
        DeInit();
    }
    //------------------------------------------------------------------------
    bool YMCodec::Init(const CStdString & strFile, unsigned int filecache)
    {
        if (!m_dll.Load())
            return false; // error logged previously

        m_ym = m_dll.LoadYM(strFile.c_str());
        if (!m_ym)
        {
            CLog::Log(LOGERROR,"YMCodec: error opening file %s!",strFile.c_str());
            return false;
        }

        m_Channels = 1;
        m_SampleRate = 44100;
        m_BitsPerSample = 16;
        m_DataFormat = SF_S16;
        m_TotalTime = m_dll.GetLength(m_ym) * 1000;

        return true;
    }
    //------------------------------------------------------------------------
    void YMCodec::DeInit()
    {
        if (m_ym)
            m_dll.FreeYM(m_ym);
        m_ym = 0;
    }
    //------------------------------------------------------------------------
    int64_t YMCodec::Seek(int64_t iSeekTime)
    {
        return m_dll.Seek(m_ym,(unsigned long)iSeekTime);
    }
    //------------------------------------------------------------------------
    int YMCodec::ReadPCM(BYTE * data, int size, int * actualsize)
    {
        if ((*actualsize = m_dll.FillBuffer(m_ym,(char *)data,size))> 0)
        {
            m_iDataPos += *actualsize;
            return READ_SUCCESS;
        }

        return READ_ERROR;
    }
    //------------------------------------------------------------------------
    bool YMCodec::CanInit()
    {
        return m_dll.CanLoad();
    }
    //------------------------------------------------------------------------
}
}