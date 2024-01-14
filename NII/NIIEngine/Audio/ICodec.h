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

#ifndef _NII_AUDIO_CODEC_H_
#define _NII_AUDIO_CODEC_H_

#include "utils/StdString.h"
#include "filesystem/File.h"
#include "music/tags/MusicInfoTag.h"

#include "NiiAudioFormat.h"

#define READ_EOF      -1
#define READ_SUCCESS   0
#define READ_ERROR     1

namespace NII
{
namespace NII_MEDIA
{
    class ICodec
    {
    public:
        ICodec()
        {
            m_TotalTime = 0;
            m_SampleRate = 0;
            m_EncodedSampleRate = 0;
            m_BitsPerSample = 0;
            m_DataFormat = SF_Unknow;
            m_Channels = 0;
            m_Bitrate = 0;
            m_CodecName = "";
        };
        virtual ~ICodec() {};

        // Virtual functions that all codecs should implement.  Note that these may need
        // enhancing and or refactoring at a later date.  It works currently well for MP3 and
        // APE codecs.

        // Init(filename)
        // This routine should handle any initialization necessary.  At a minimum it needs to:
        // 1.  Load any dlls and make sure any buffers etc. are allocated.
        // 2.  If it's using a filereader, initialize it with the appropriate cache size.
        // 3.  Load the file (or at least attempt to load it)
        // 4.  Fill in the m_TotalTime, m_SampleRate, m_BitsPerSample and m_Channels parameters.
        virtual bool Init(const CStdString & strFile, unsigned int filecache)=0;

        // DeInit()
        // Should just cleanup anything as necessary.  No need to free buffers here if they
        // are allocated and destroyed in the destructor.
        virtual void DeInit() = 0;

        virtual bool CanSeek() {return true;}

        // Seek()
        // Should seek to the appropriate time (in ms) in the file, and return the
        // time to which we managed to seek (in the case where seeking is problematic)
        // This is used in FFwd/Rewd so can be called very often.
        virtual int64_t Seek(int64_t iSeekTime)=0;

        // ReadPCM()
        // Decodes audio into pBuffer up to size bytes.  The actual amount of returned data
        // is given in actualsize.  Returns READ_SUCCESS on success.  Returns READ_EOF when
        // the data has been exhausted, and READ_ERROR on error.
        virtual int ReadPCM(BYTE * pBuffer, int size, int *actualsize)=0;

        // CanInit()
        // Should return true if the codec can be initialized
        // eg. check if a dll needed for the codec exists
        virtual bool CanInit()=0;

        // SkipNext()
        // Skip to next track/item inside the current media (if supported).
        virtual bool SkipNext(){return false;}

        // set the total time - useful when info comes from a preset tag
        virtual void SetTotalTime(int64_t totaltime) {}

        virtual bool IsCaching()const    {return false;}
        virtual int GetCacheLevel()const    {return -1;}

        // GetChannelInfo()
        // Return the channel layout and count information in an AudioChannel object
        // Implemented in PAPlayer.cpp to avoid an include here
        virtual AudioChannel GetChannelInfo();

        int64_t m_TotalTime;  // time in ms
        int m_SampleRate;
        int m_EncodedSampleRate;
        int m_BitsPerSample;
        enum SampleFormat m_DataFormat;
        int m_Bitrate;
        CStdString m_CodecName;
        MUSIC_INFO::CMusicInfoTag m_tag;
        XFILE::CFile m_file;

    protected:
        int m_Channels; /* remove this soon, its being deprecated */
    };
}
}

#endif