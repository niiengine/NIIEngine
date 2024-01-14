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

#include "CachingCodec.h"
#include "../dvdplayer/DVDCodecs/Audio/DllLibMad.h"

namespace NII
{
namespace NII_MEDIA
{
    enum madx_sig 
    {
        ERROR_OCCURED,
        MORE_INPUT,
        FLUSH_BUFFER,
        CALL_AGAIN,
        SKIP_FRAME
    };

    struct madx_house 
    {
        struct mad_stream stream;
        struct mad_frame frame;
        struct mad_synth synth;
        mad_timer_t timer;
        unsigned long frame_cnt;
        unsigned char * output_ptr;
    };

    struct madx_stat 
    {
        size_t write_size;
        size_t readsize;
        size_t remaining;
        size_t framepcmsize;
        bool flushed;
    };

    class MP3Codec : public CachingCodec
    {
    public:
        MP3Codec();
        virtual ~MP3Codec();

        virtual bool Init(const CStdString & strFile, NCount filecache);
        virtual void DeInit();
        virtual bool CanSeek();
        virtual int64_t Seek(int64_t iSeekTime);
        virtual int ReadPCM(BYTE * pBuffer, int size, int *actualsize);
        virtual bool CanInit();
        virtual bool SkipNext();
        virtual AudioChannel GetChannelInfo();
    private:
        // VBR helpers
        static NCount IsID3v2Header(unsigned char* pBuf, size_t bufLen);
        virtual int ReadDuration();
        bool ReadLAMETagInfo(unsigned char *p);
        int IsMp3FrameHeader(unsigned long head);
        int64_t GetByteOffset(float fTime);
        int64_t GetTimeOffset(int64_t iBytes);
        void SetOffsets(int iSeekOffsets, const float *offsets);
    //  void SetDuration(float fDuration) { m_fTotalDuration = fDuration; };
    //  float GetDuration() const { return m_fTotalDuration; };

        /* TODO decoder functions */
        virtual int Decode(int *out_len);
        virtual void Flush();
        int madx_init(madx_house* mxhouse);
        madx_sig madx_read(madx_house *mxhouse, madx_stat* mxstat, int maxwrite);
        void madx_deinit(madx_house* mxhouse);
        /* END decoder functions */

        void OnFileReaderClearEvent();
        void FlushDecoder();
        int Read(int size, bool init = false);

        /* TODO decoder vars */
        bool m_HaveData;
        unsigned char  flushcnt;

        madx_house mxhouse;
        madx_stat mxstat;
        madx_sig mxsig;
        /* END decoder vars */

        // Decoding variables
        int64_t m_lastByteOffset;
        bool m_eof;
        bool m_Decoding;
        bool m_CallAgainWithSameBuffer;
        int m_readRetries;

        // Input buffer to read our mp3 data into
        BYTE * m_InputBuffer;
        NCount m_InputBufferSize;
        NCount m_InputBufferPos;

        // Output buffer.  We require this, as mp3 decoding means keeping at least 2 frames (1152 * 2 samples)
        // of data in order to remove that data at the end as it may be surplus to requirements.
        BYTE * m_OutputBuffer;
        NCount m_OutputBufferSize;
        NCount m_OutputBufferPos;    // position in our buffer

        NCount m_Formatdata[8];

        // Seeking helpers
        float m_fTotalDuration;
        int m_iSeekOffsets;
        float * m_SeekOffset;
        int m_iFirstSample;
        int m_iLastSample;

        // Gapless playback
        bool m_IgnoreFirst;     // Ignore first samples if this is true (for gapless playback)
        bool m_IgnoreLast;      // Ignore first samples if this is true (for gapless playback)
        int m_IgnoredBytes;     // amount of samples ignored thus far

        DllLibMad m_dll;
    };
}
}
