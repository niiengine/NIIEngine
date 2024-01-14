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

#include "SIDCodec.h"
#include "cores/DllLoader/DllLoader.h"
#include "FileItem.h"
#include "utils/log.h"
#include "utils/URIUtils.h"
#include "cores/AudioEngine/Utils/AEUtil.h"

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    SIDCodec::SIDCodec()
    {
        m_CodecName = "sid";
        m_sid = 0;
        m_iTrack = -1;
        m_iDataPos = -1;
    }
    //------------------------------------------------------------------------
    SIDCodec::~SIDCodec()
    {
        DeInit();
    }
    //------------------------------------------------------------------------
    bool SIDCodec::Init(const CStdString & strFile, unsigned int filecache)
    {
        if (!m_dll.Load())
            return false; // error logged previously

        m_dll.Init();

        CStdString strFileToLoad = strFile;
        m_iTrack = 0;
        CStdString strExtension;
        URIUtils::GetExtension(strFile,strExtension);
        strExtension.MakeLower();
        if (strExtension==".sidstream")
        {
            //  Extract the track to play
            CStdString strFileName=URIUtils::GetFileName(strFile);
            int iStart=strFileName.ReverseFind('-')+1;
            m_iTrack = atoi(strFileName.substr(iStart, strFileName.size()-iStart-10).c_str());
            //  The directory we are in, is the file
            //  that contains the bitstream to play,
            //  so extract it
            CStdString strPath=strFile;
            URIUtils::GetDirectory(strPath, strFileToLoad);
            URIUtils::RemoveSlashAtEnd(strFileToLoad); // we want the filename
        }

        m_sid = m_dll.LoadSID(strFileToLoad.c_str());
        if (!m_sid)
        {
            CLog::Log(LOGERROR,"SIDCodec: error opening file %s!",strFile.c_str());
            return false;
        }

        m_Channels = 1;
        m_SampleRate = 48000;
        m_BitsPerSample = 16;
        m_TotalTime = 4*60*1000;
        m_DataFormat = SF_S16;

        return true;
    }
    //------------------------------------------------------------------------
    void SIDCodec::DeInit()
    {
        if (m_sid)
            m_dll.FreeSID(m_sid);
        m_sid = 0;
    }
    //------------------------------------------------------------------------
    int64_t SIDCodec::Seek(int64_t iSeekTime)
    {
        char temp[3840*2];
        if (m_iDataPos > iSeekTime/1000*48000*2)
        {
            m_dll.StartPlayback(m_sid,m_iTrack);
            m_iDataPos = 0;
        }

        while (m_iDataPos < iSeekTime/1000*48000*2)
        {
            int64_t iRead = iSeekTime/1000*48000*2-m_iDataPos;
            if (iRead > 3840*2)
            {
                m_dll.SetSpeed(m_sid,32*100);
                iRead = 3840*2;
            }
            else
                m_dll.SetSpeed(m_sid,100);

            iRead = m_dll.FillBuffer(m_sid,temp,int(iRead));
            if (!iRead)
                break; // get out of here
            if (iRead == 3840*2)
                m_iDataPos += iRead*32;
            else 
                m_iDataPos += iRead;
        }
        return iSeekTime;
    }
    //------------------------------------------------------------------------
    int SIDCodec::ReadPCM(BYTE *pBuffer, int size, int *actualsize)
    {
        if (m_iDataPos == -1)
        {
            m_dll.StartPlayback(m_sid,m_iTrack);
            m_iDataPos = 0;
        }

        if (m_iDataPos >= m_TotalTime/1000*48000*2)
            return READ_EOF;

        m_dll.SetSpeed(m_sid,100);
        if ((*actualsize=m_dll.FillBuffer(m_sid,pBuffer,size))> 0)
        {
            m_iDataPos += *actualsize;
            return READ_SUCCESS;
        }

        return READ_ERROR;
    }
    //------------------------------------------------------------------------
    bool SIDCodec::CanInit()
    {
        return m_dll.CanLoad();
    }
    //------------------------------------------------------------------------
    AudioChannel SIDCodec::GetChannelInfo()
    {
        static enum AudioChannelType map[1][2] = 
        {
            {ACT_FC, ACT_Unknow}
        };

        if (m_Channels > 1)
            return CAEUtil::GuessChLayout(m_Channels);

        return AudioChannel(map[m_Channels - 1]);
    }
    //------------------------------------------------------------------------
}
}