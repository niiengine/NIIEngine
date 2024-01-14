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

#include "NiiPreProcess.h"
#include "NiiDeflate.h"
#include "NiiException.h"

#include <zlib.h>

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // È«¾Öº¯Êý
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    void * NIIZalloc(void * opaque, Nui32 items, Nui32 size)
    {
        return N_alloc(items * size);
    }
    //------------------------------------------------------------------------
    void NIIZfree(void * opaque, void* address)
    {
        N_free(address);
    }
    #define NII_DEFLATE_TMP_SIZE 16384
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // DeflateStream
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    DeflateStream::DeflateStream(DataStream * compressedStream, const String & tmpFileName):
        DataStream(compressedStream->getAccessMode()),
        mCompressedStream(compressedStream),
        mTempFileName(tmpFileName),
        mZStream(0),
        mCurrentPos(0),
        mTmp(0),
        mDeflateData(true)
    {
        init();
    }
    //------------------------------------------------------------------------
    DeflateStream::DeflateStream(const String & name, DataStream * compressedStream,
        const String & tmpFileName) :
            DataStream(name, compressedStream->getAccessMode()),
            mCompressedStream(compressedStream),
            mTempFileName(tmpFileName),
            mZStream(0),
            mCurrentPos(0),
            mTmp(0),
            mDeflateData(true)
    {
        init();
    }
    //------------------------------------------------------------------------
    void DeflateStream::init()
    {
        mZStream = N_alloc_t(z_stream, 1);
        mZStream->zalloc = NIIZalloc;
        mZStream->zfree = NIIZfree;

        if (getAccessMode() == AM_READ)
        {
            mTmp = (Nui8 *)N_alloc(NII_DEFLATE_TMP_SIZE);
            NCount restorePoint = mCompressedStream->tell();
            // read early chunk
            mZStream->next_in = mTmp;
            mZStream->avail_in = mCompressedStream->read(mTmp, NII_DEFLATE_TMP_SIZE);

            if (inflateInit(mZStream) != Z_OK)
            {
                mDeflateData = false;
            }
            else
                mDeflateData = true;

            if (mDeflateData)
            {
                // in fact, inflateInit on some implementations doesn't try to read
                // anything. We need to at least read something to test
                Bytef testOut[4];
                NCount savedIn = mZStream->avail_in;
                mZStream->avail_out = 4;
                mZStream->next_out = testOut;
                if (inflate(mZStream, Z_SYNC_FLUSH) != Z_OK)
                    mDeflateData = false;
                // restore for reading
                mZStream->avail_in = savedIn;
                mZStream->next_in = mTmp;

                inflateReset(mZStream);
            }

            if (!mDeflateData)
            {
                // Not compressed data!
                // Fail gracefully, fall back on reading the underlying stream direct
                destroy();
                mCompressedStream->seek(restorePoint);
            }
        }
        else
        {
            if(mTempFileName.empty())
            {
                // Write to temp file
                NIIb tmpname[L_tmpnam];
                tmpnam(tmpname);
                mTempFileName = tmpname;
            }

            std::fstream *f = N_new_t(std::fstream)();
            f->open(mTempFileName.c_str(), std::ios::binary | std::ios::out);
            mTmpWriteStream = N_new FileDataStream(f);
        }
    }
    //------------------------------------------------------------------------
    void DeflateStream::destroy()
    {
        if (getAccessMode() == AM_READ)
            inflateEnd(mZStream);

        N_free(mZStream);
        mZStream = 0;
        N_free(mTmp);
        mTmp = 0;
    }
    //------------------------------------------------------------------------
    DeflateStream::~DeflateStream()
    {
        close();
        destroy();
    }
    //------------------------------------------------------------------------
    NCount DeflateStream::read(void* buf, NCount count) const
    {
        if (!mDeflateData)
        {
            return mCompressedStream->read(buf, count);
        }

        if (getAccessMode() & AM_WRITE)
        {
            return mTmpWriteStream->read(buf, count);
        }
        else
        {

            NCount restorePoint = mCompressedStream->tell();
            // read from cache first
            NCount cachereads = mReadCache.read(buf, count);

            NCount newReadUncompressed = 0;

            if (cachereads < count)
            {
                mZStream->avail_out = count - cachereads;
                mZStream->next_out = (Bytef*)buf + cachereads;

                while (mZStream->avail_out)
                {
                    // Pull next chunk of compressed data from the underlying stream
                    if (!mZStream->avail_in && !mCompressedStream->end())
                    {
                        mZStream->avail_in = mCompressedStream->read(mTmp, NII_DEFLATE_TMP_SIZE);
                        mZStream->next_in = mTmp;
                    }

                    if (mZStream->avail_in)
                    {
                        NIIi availpre = mZStream->avail_out;
                        NIIi status = inflate(mZStream, Z_SYNC_FLUSH);
                        NCount readUncompressed = availpre - mZStream->avail_out;
                        newReadUncompressed += readUncompressed;
                        if (status != Z_OK)
                        {
                            // End of data, or error
                            if (status != Z_STREAM_END)
                            {
                                mCompressedStream->seek(restorePoint);
                                N_EXCEPT(InvalidState,
                                    _I18n("Error in compressed stream"));
                            }
                            else
                            {
                                // back up the stream so that it can be used from the end onwards
                                NIIl unusedCompressed = mZStream->avail_in;
                                mCompressedStream->skip(-unusedCompressed);
                            }

                            break;
                        }
                    }
                }
            }

            // Cache the last bytes read
            mReadCache.cacheData((NIIb*)buf + cachereads, newReadUncompressed);

            mCurrentPos += newReadUncompressed + cachereads;

            return newReadUncompressed + cachereads;
        }
    }
    //------------------------------------------------------------------------
    NCount DeflateStream::write(const void * buf, NCount count)
    {
        if ((getAccessMode() & AM_WRITE) == 0)
            N_EXCEPT(InvalidParam, _I18n("Not a writable stream"));

        return mTmpWriteStream->write(buf, count);
    }
    //------------------------------------------------------------------------
    void DeflateStream::compressFinal()
    {
        // Close temp stream
        mTmpWriteStream->close();

        // Copy & compress
        // We do this rather than compress directly because some code seeks
        // around while writing (e.g. to update size blocks) which is not
        // possible when compressing on the fly

        NIIi ret, flush;
        NIIb in[NII_DEFLATE_TMP_SIZE];
        NIIb out[NII_DEFLATE_TMP_SIZE];

        if (deflateInit(mZStream, Z_DEFAULT_COMPRESSION) != Z_OK)
        {
            destroy();
            N_EXCEPT(InvalidState,
                _I18n("Error initialising deflate compressed stream!"));
        }

        std::ifstream inFile;
        inFile.open(mTempFileName.c_str(), std::ios::in | std::ios::binary);

        do
        {
            inFile.read(in, NII_DEFLATE_TMP_SIZE);
            mZStream->avail_in = inFile.gcount();
            if (inFile.bad())
            {
                deflateEnd(mZStream);
                N_EXCEPT(InvalidState,
                    _I18n("Error reading temp uncompressed stream!"));
            }
            flush = inFile.eof() ? Z_FINISH : Z_NO_FLUSH;
            mZStream->next_in = (Bytef*)in;

            /* run deflate() on input until output buffer not full, finish
             compression if all of source has been read in */
            do
            {
                mZStream->avail_out = NII_DEFLATE_TMP_SIZE;
                mZStream->next_out = (Bytef*)out;
                ret = deflate(mZStream, flush);    /* no bad return value */
                N_assert(ret != Z_STREAM_ERROR, "error");  /* state not clobbered */
                NCount compressed = NII_DEFLATE_TMP_SIZE - mZStream->avail_out;
                mCompressedStream->write(out, compressed);
            } while (mZStream->avail_out == 0);
            N_assert(mZStream->avail_in == 0, "error");     /* all input will be used */

            /* done when last data in file processed */
        } while (flush != Z_FINISH);
        N_assert(ret == Z_STREAM_END, "error");        /* stream will be complete */

        deflateEnd(mZStream);

        inFile.close();
        remove(mTempFileName.c_str());

    }
    //------------------------------------------------------------------------
    void DeflateStream::skip(NIIl count)
    {
        if (!mDeflateData)
        {
            mCompressedStream->skip(count);
            return;
        }

        if (getAccessMode() & AM_WRITE)
        {
            mTmpWriteStream->skip(count);
        }
        else
        {
            if (count > 0)
            {
                if (!mReadCache.skip(count))
                {
                    N_EXCEPT(InvalidState,
                        _I18n("You can only skip within the cache range in a deflate stream."));
                }
            }
            else if (count < 0)
            {
                if (!mReadCache.rewind((NCount)(-count)))
                {
                    N_EXCEPT(InvalidState,
                        _I18n("You can only skip within the cache range in a deflate stream."));
                }
            }
        }
        mCurrentPos = static_cast<NCount>(static_cast<NIIl>(mCurrentPos) + count);


    }
    //------------------------------------------------------------------------
    void DeflateStream::seek(NCount pos)
    {
        if (!mDeflateData)
        {
            mCompressedStream->seek(pos);
            return;
        }
        if (getAccessMode() & AM_WRITE)
        {
            mTmpWriteStream->seek(pos);
        }
        else
        {
            if (pos == 0)
            {
                mCurrentPos = 0;
                mZStream->next_in = mTmp;
                mCompressedStream->seek(0);
                mZStream->avail_in = mCompressedStream->read(mTmp, NII_DEFLATE_TMP_SIZE);
                inflateReset(mZStream);
            }
            else
            {
                skip(pos - tell());
            }
        }
    }
    //------------------------------------------------------------------------
    NCount DeflateStream::tell(void) const
    {
        if (!mDeflateData)
        {
            return mCompressedStream->tell();
        }
        else if(getAccessMode() & AM_WRITE)
        {
            return mTmpWriteStream->tell();
        }
        else
        {
            return mCurrentPos;
        }

    }
    //------------------------------------------------------------------------
    bool DeflateStream::end() const
    {
        if (getAccessMode() & AM_WRITE)
            return mTmpWriteStream->end();
        else
        {
            if (!mDeflateData)
                return mCompressedStream->end();
            else
                return mCompressedStream->end() && mZStream->avail_in == 0;
        }
    }
    //------------------------------------------------------------------------
    void DeflateStream::close(void)
    {
        if (getAccessMode() & AM_WRITE)
        {
            compressFinal();
        }

        // don't close underlying compressed stream in case used for something else
    }
    //------------------------------------------------------------------------
}