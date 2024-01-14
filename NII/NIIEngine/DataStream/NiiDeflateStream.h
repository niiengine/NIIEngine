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

#ifndef _NII_DEFLATE_H__
#define _NII_DEFLATE_H__

#include "NiiPreInclude.h"
#include "NiiDataStream.h"
#include "NiiTempBuffer.h"

struct z_stream_s;
typedef struct z_stream_s z_stream;

namespace NII
{
    /** Stream which compresses / uncompresses data using the 'deflate' compression
        algorithm.
    @remarks
        This stream is designed to wrap another stream for the actual source / destination
        of the compressed data, it has no concrete source / data itself. The idea is
        that you pass uncompressed data through this stream, and the underlying
        stream reads/writes compressed data to the final source.
    @note
        This is an alternative to using a compressed archive since it is able to
        compress & decompress regardless of the actual source of the stream.
        You should avoid using this with already compressed archives.
        Also note that this cannot be used as a read / write stream, only a read-only
        or write-only stream.
    */
    class _EngineAPI DeflateStream : public DataStream
    {
    public:
        DeflateStream(DataStream * compressedStream, const String& tmpFileName = _T(""));
        DeflateStream(const String& name, DataStream * compressedStream, const String& tmpFileName= _T(""));

        ~DeflateStream();

        bool isDeflateData() const { return mDeflateData; }

        /// @copydoc DataStream::read
        NCount read(void * buf, NCount count) const;

        /// @copydoc DataStream::write
        NCount write(const void * buf, NCount count);

        /// @copydoc DataStream::skip
        void skip(NIIl count);

        /// @copydoc DataStream::seek
        void seek(NCount pos);

        /// @copydoc DataStream::tell
        NCount tell(void) const;

        /// @copydoc DataStream::end
        bool end(void) const;

        /// @copydoc DataStream::close
        void close(void);
    protected:
        void init();
        void destroy();
        void compressFinal();        
    protected:
        DataStream * mCompressedStream;
        DataStream * mTmpWriteStream;
        String mTempFileName;
        z_stream * mZStream;
        NCount mCurrentPos;
        
        TempBuffer<16 * NII_STREAM_TEMP_SIZE> mReadCache;
        Nui8 * mTmp;

        bool mDeflateData;       
    };
}
#endif