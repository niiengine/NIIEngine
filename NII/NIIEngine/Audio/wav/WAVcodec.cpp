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

#include "system.h"
#include "WAVcodec.h"
#include "utils/EndianSwap.h"
#include "utils/log.h"
#include "cores/AudioEngine/Utils/AEUtil.h"

#if defined(WIN32)
    #include <mmreg.h>
    #include <ks.h>
    #include <ksmedia.h>
#endif

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    typedef struct
    {
        char chunk_id[4];
        DWORD chunksize;
    }WAVE_CHUNK;
    //------------------------------------------------------------------------
    typedef struct
    {
        char riff[4];
        DWORD filesize;
        char rifftype[4];
    }WAVE_RIFFHEADER;
    //------------------------------------------------------------------------
    WAVCodec::WAVCodec()
    {
        m_SampleRate = 0;
        m_Channels = 0;
        m_BitsPerSample = 0;
        m_DataFormat = SF_Unknow;
        m_iDataStart = 0;
        m_iDataLen = 0;
        m_ChannelMask = 0;
        m_Bitrate = 0;
        m_CodecName = "wav";
    }
    //------------------------------------------------------------------------
    WAVCodec::~WAVCodec()
    {
        DeInit();
    }
    //------------------------------------------------------------------------
    bool WAVCodec::Init(const CStdString & strFile, NCount filecache)
    {
        m_file.Close();
        if (!m_file.Open(strFile, READ_CACHED))
            return false;

        int64_t length;
        WAVE_RIFFHEADER riff;
        bool hasFmt = false;
        bool hasData = false;

        length = m_file.GetLength();
        m_file.Seek(0, SEEK_SET);
        m_file.Read(&riff, sizeof(WAVE_RIFFHEADER));
        riff.filesize = Endian_SwapLE32(riff.filesize);
        if ((strncmp(riff.riff, "RIFF", 4) != 0) && (strncmp(riff.rifftype, "WAVE", 4) != 0))
            return false;

        // read in each chunk
        while(length - m_file.GetPosition() >= (int64_t)sizeof(WAVE_CHUNK))
        {
            WAVE_CHUNK chunk;
            m_file.Read(&chunk, sizeof(WAVE_CHUNK));
            chunk.chunksize = Endian_SwapLE32(chunk.chunksize);

            // if it is the "fmt " chunk
            if (!hasFmt && (strncmp(chunk.chunk_id, "fmt ", 4) == 0))
            {
                int64_t read;
                WAVEFORMATEXTENSIBLE wfx;

                read = std::min((DWORD)sizeof(WAVEFORMATEXTENSIBLE), chunk.chunksize);
                m_file.Read(&wfx, read);

                // get the format information
                m_SampleRate = Endian_SwapLE32(wfx.Format.nSamplesPerSec);
                m_Channels = Endian_SwapLE16(wfx.Format.nChannels     );
                m_BitsPerSample = Endian_SwapLE16(wfx.Format.wBitsPerSample);

                switch(m_BitsPerSample)
                {
                case 8:
                    m_DataFormat = SF_U8;
                    break;
                case 16:
                    m_DataFormat = SF_S16L;
                    break;
                case 24:
                    m_DataFormat = SF_S24W3;
                    break;
                case 32:
                    m_DataFormat = SF_F;
                    break;
                }

                CLog::Log(LOGINFO, "WAVCodec::Init - Sample Rate: %d, Bits Per Sample: %d, Channels: %d", m_SampleRate, m_BitsPerSample, m_Channels);
                if ((m_SampleRate == 0) || (m_Channels == 0) || (m_BitsPerSample == 0) || (m_DataFormat == SF_Unknow))
                {
                    CLog::Log(LOGERROR, "WAVCodec::Init - Invalid data in WAVE header");
                    return false;
                }

                wfx.Format.wFormatTag = Endian_SwapLE16(wfx.Format.wFormatTag);
                wfx.Format.cbSize = Endian_SwapLE16(wfx.Format.cbSize    );

                // detect the file type
                switch(wfx.Format.wFormatTag)
                {
                case WAVE_FORMAT_PCM:
                    CLog::Log(LOGINFO, "WAVCodec::Init - WAVE_FORMAT_PCM detected");
                    m_ChannelMask = 0;
                    break;
                case WAVE_FORMAT_IEEE_FLOAT:
                    CLog::Log(LOGINFO, "WAVCodec::Init - WAVE_FORMAT_IEEE_FLOAT detected");
                    if (wfx.Format.wBitsPerSample != 32)
                    {
                        CLog::Log(LOGERROR, "WAVCodec::Init - Only 32bit Float is supported");
                        return false;
                    }

                    m_ChannelMask = 0;
                    break;
                case WAVE_FORMAT_EXTENSIBLE:
                    if (wfx.Format.cbSize < 22)
                    {
                        CLog::Log(LOGERROR, "WAVCodec::Init - Corrupted WAVE_FORMAT_EXTENSIBLE header");
                        return false;
                    }
                    m_ChannelMask = Endian_SwapLE32(wfx.dwChannelMask);
                    CLog::Log(LOGINFO, "WAVCodec::Init - WAVE_FORMAT_EXTENSIBLE detected, channel mask: %d", m_ChannelMask);

                    wfx.SubFormat.Data1 = Endian_SwapLE32(wfx.SubFormat.Data1);
                    wfx.SubFormat.Data2 = Endian_SwapLE16(wfx.SubFormat.Data2);
                    wfx.SubFormat.Data3 = Endian_SwapLE16(wfx.SubFormat.Data3);
                    if (memcmp(&wfx.SubFormat, &KSDATAFORMAT_SUBTYPE_PCM, sizeof(GUID)) == 0)
                    {
                        CLog::Log(LOGINFO, "WAVCodec::Init - SubFormat KSDATAFORMAT_SUBTYPE_PCM Detected");
                    }
                    else if (memcmp(&wfx.SubFormat, &KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, sizeof(GUID)) == 0)
                    {
                        CLog::Log(LOGINFO, "WAVCodec::Init - SubFormat KSDATAFORMAT_SUBTYPE_IEEE_FLOAT Detected");
                        if (wfx.Format.wBitsPerSample != 32)
                        {
                            CLog::Log(LOGERROR, "WAVCodec::Init - Only 32bit Float is supported");
                            return false;
                        }
                    }
                    else
                    {
                        CLog::Log(LOGERROR, "WAVCodec::Init - Unsupported extensible Wave format");
                        return false;
                    }
                    break;

                default:
                    CLog::Log(LOGERROR, "WAVCodec::Init - Unsupported Wave Format %d", wfx.Format.wFormatTag);
                    return false;
                    break;
                }

                // seek past any extra data that may be in the header
                m_file.Seek(chunk.chunksize - read, SEEK_CUR);
                hasFmt = true;

            }
            else if (!hasData && (strncmp(chunk.chunk_id, "data", 4) == 0))
            {
                m_iDataStart = (long)m_file.GetPosition();
                m_iDataLen = chunk.chunksize;
                chunk.chunksize += (chunk.chunksize % 2);

                // sanity check on the data length
                if (m_iDataLen > length - m_iDataStart)
                {
                    CLog::Log(LOGWARNING, "WAVCodec::Init - Wave has corrupt data length of %i when it can't be longer then %"PRId64"", m_iDataLen, length - m_iDataStart);
                    m_iDataLen = (long)(length - m_iDataStart);
                }

                // if this data chunk is empty, we will look for another data chunk that is not empty
                if (m_iDataLen == 0)
                    CLog::Log(LOGWARNING, "WAVCodec::Init - Empty data chunk, will look for another");
                else
                    hasData = true;

                //seek past the data
                m_file.Seek(chunk.chunksize, SEEK_CUR);
            }
            else
            {
                // any unknown chunks just seek past
                m_file.Seek(chunk.chunksize, SEEK_CUR);
            }

            // dont keep reading if we have the chunks we need
            if (hasFmt && hasData)
                break;
        }

        if (!hasFmt || !hasData)
        {
            CLog::Log(LOGERROR, "WAVCodec::Init - Corrupt file, unable to locate both fmt and data chunks");
            return false;
        }

        m_TotalTime = (int)(((float)m_iDataLen / (m_SampleRate * m_Channels * (m_BitsPerSample / 8))) * 1000);
        m_Bitrate = (int)(((float)m_iDataLen * 8) / ((float)m_TotalTime / 1000));

        // ensure the parameters are valid
        if ((m_TotalTime <= 0) || (m_Bitrate <= 0))
        {
            CLog::Log(LOGERROR, "WAVCodec::Init - The total time/bitrate is invalid, possibly corrupt file");
            return false;
        }

        //  Seek to the start of the data chunk
        m_file.Seek(m_iDataStart, SEEK_SET);
        return true;
    }
    //------------------------------------------------------------------------
    void WAVCodec::DeInit()
    {
        m_file.Close();
    }
    //------------------------------------------------------------------------
    int64_t WAVCodec::Seek(int64_t iSeekTime)
    {
        //  Calculate size of a single sample of the file
        int iSampleSize=m_SampleRate*m_Channels*(m_BitsPerSample/8);

        //  Seek to the position in the file
        m_file.Seek(m_iDataStart+((iSeekTime/1000)*iSampleSize));

        return iSeekTime;
    }
    //------------------------------------------------------------------------
    int WAVCodec::ReadPCM(BYTE * data, int size, int * actualsize)
    {
        *actualsize=0;
        NCount iPos=(int)m_file.GetPosition();
        if (iPos >= m_iDataStart+m_iDataLen)
            return READ_EOF;

        int iAmountRead=m_file.Read(data, size);
        if (iAmountRead>0)
        {
            *actualsize=iAmountRead;
            return READ_SUCCESS;
        }

        return READ_ERROR;
    }
    //------------------------------------------------------------------------
    bool WAVCodec::CanInit()
    {
        return true;
    }
    //------------------------------------------------------------------------
    AudioChannel WAVCodec::GetChannelInfo()
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