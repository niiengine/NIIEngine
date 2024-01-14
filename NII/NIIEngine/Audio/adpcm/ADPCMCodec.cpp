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

#include "ADPCMCodec.h"
#include "utils/log.h"
#include "cores/AudioEngine/Utils/AEUtil.h"

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    ADPCMCodec::ADPCMCodec()
    {
        m_CodecName = "adpcm";
        m_adpcm = NULL;
        m_bIsPlaying = false;
    }
    //------------------------------------------------------------------------
    ADPCMCodec::~ADPCMCodec()
    {
        DeInit();
    }
    //------------------------------------------------------------------------
    bool ADPCMCodec::Init(const CStdString &strFile, unsigned int filecache)
    {
        DeInit();

        if (!m_dll.Load())
            return false; // error logged previously

        m_adpcm = m_dll.LoadXWAV(strFile.c_str());
        if (!m_adpcm)
        {
            CLog::Log(LOGERROR,"ADPCMCodec: error opening file %s!",strFile.c_str());
            return false;
        }

        m_Channels = m_dll.GetNumberOfChannels(m_adpcm);
        m_SampleRate = m_dll.GetPlaybackRate(m_adpcm);
        m_BitsPerSample = 16;//m_dll.GetSampleSize(m_adpcm);
        m_DataFormat = SF_S16;
        m_TotalTime = m_dll.GetLength(m_adpcm); // fixme?

        return true;
    }
    //------------------------------------------------------------------------
    void ADPCMCodec::DeInit()
    {
        if (m_adpcm)
            m_dll.FreeXWAV(m_adpcm);

        m_adpcm = NULL;
        m_bIsPlaying = false;
    }
    //------------------------------------------------------------------------
    int64_t ADPCMCodec::Seek(int64_t iSeekTime)
    {
        m_dll.Seek(m_adpcm,(int)iSeekTime);
        return iSeekTime;
    }
    //------------------------------------------------------------------------
    int ADPCMCodec::ReadPCM(BYTE *pBuffer, int size, int *actualsize)
    {
        if (!m_adpcm)
            return READ_ERROR;

        *actualsize  = m_dll.FillBuffer(m_adpcm,(char*)pBuffer,size);

        if (*actualsize == 0)
            return READ_ERROR;

        return READ_SUCCESS;
    }
    //------------------------------------------------------------------------
    bool ADPCMCodec::CanInit()
    {
        return m_dll.CanLoad();
    }
    //------------------------------------------------------------------------
    AudioChannel ADPCMCodec::GetChannelInfo()
    {
        static AudioChannelType map[2][3] =
        {
            {ACT_FC, ACT_Unknow},
            {ACT_FL, ACT_FR  , ACT_Unknow}
        };

        if (m_Channels > 2)
            return CAEUtil::GuessChLayout(m_Channels);

        return AudioChannel(map[m_Channels - 1]);
    }
    //------------------------------------------------------------------------
}
}