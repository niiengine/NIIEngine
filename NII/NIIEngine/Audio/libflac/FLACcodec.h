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
#include "DllLibFlac.h"

namespace NII
{
namespace NII_MEDIA
{
    class FLACCodec : public CachingCodec
    {
    public:
        FLACCodec();
        virtual ~FLACCodec();

        virtual bool Init(const CStdString &strFile, unsigned int filecache);
        virtual void DeInit();
        virtual int64_t Seek(int64_t iSeekTime);
        virtual int ReadPCM(BYTE *pBuffer, int size, int *actualsize);
        virtual bool CanInit();
        virtual AudioChannel GetChannelInfo() {return m_ChannelInfo;}

    private:
        //  I/O callbacks for the flac decoder
        static FLAC__StreamDecoderReadStatus DecoderReadCallback(const FLAC__StreamDecoder *decoder, FLAC__byte buffer[], size_t *bytes, void *client_data);
        static FLAC__StreamDecoderSeekStatus DecoderSeekCallback(const FLAC__StreamDecoder *decoder, FLAC__uint64 absolute_byte_offset, void *client_data);
        static FLAC__StreamDecoderTellStatus DecoderTellCallback(const FLAC__StreamDecoder *decoder, FLAC__uint64 *absolute_byte_offset, void *client_data);
        static FLAC__StreamDecoderLengthStatus DecoderLengthCallback(const FLAC__StreamDecoder *decoder, FLAC__uint64 *stream_length, void *client_data);
        static FLAC__bool DecoderEofCallback(const FLAC__StreamDecoder *decoder, void *client_data);
        static FLAC__StreamDecoderWriteStatus DecoderWriteCallback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 * const buffer[], void *client_data);
        static void DecoderMetadataCallback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data);
        static void DecoderErrorCallback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data);

        void FreeDecoder();

        DllLibFlac  m_dll;
        BYTE * m_pBuffer;                    //  buffer to hold the decoded audio data
        int m_BufferSize;                   //  size of buffer is filled with decoded audio data
        int m_MaxFrameSize;                 //  size of a single decoded frame
        FLAC__StreamDecoder* m_pFlacDecoder;
        AudioChannel m_ChannelInfo;
    };
}
}