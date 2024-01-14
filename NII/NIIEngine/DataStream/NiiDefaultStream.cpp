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
#include "NiiDefaultStream.h"
#include "NiiLogManager.h"
#include <fstream>

namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // MemStream
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    MemStream::MemStream()
    {
        mData = mPos = mEnd = NULL;
        mSize = 0;
        mMark &= ~AM_CLOSE;
    }
    //-----------------------------------------------------------------------
    MemStream::MemStream(NCount size, const String & name, bool readonly) :
        DataStream(name, readonly ? AM_READ : (AM_READ | AM_WRITE))
    {
        mSize = size;
        mData = N_alloc_t(Nui8, mSize);
        mPos = mData;
        mEnd = mData + mSize;
        mMark |= AM_CLOSE;
        N_assert(mEnd >= mPos, "error");
    }
    //-----------------------------------------------------------------------
    MemStream::MemStream(Nui8 * data, NCount size, const String & name, bool autofree, bool readonly) :
        DataStream(name, readonly ? AM_READ : (AM_READ | AM_WRITE))
    {
        mData = mPos = data;
        mSize = size;
        mEnd = mData + mSize;

        if(autofree)
            mMark |= AM_CLOSE;
        else
            mMark &= ~AM_CLOSE;

        N_assert(mEnd >= mPos, "error");
    }
    //-----------------------------------------------------------------------
    MemStream::MemStream(DataStream * src, const String & name, bool readonly) :
        DataStream(name, readonly ? AM_READ : (AM_READ | AM_WRITE))
    {
        // Copy data from incoming stream
        mSize = src->size();
        if(mSize == 0 && !src->end())
        {
            // size of source is unknown, read all of it into memory
            mSize = src->cloneData(mData);
            mPos = mData;
            mEnd = mData + mSize;
        }
        else
        {
            mData = N_alloc_t(Nui8, mSize);
            mPos = mData;
            mEnd = mData + src->read(mData, mSize);
        }
        mMark |= AM_CLOSE;
        N_assert(mEnd >= mPos, "error");
    }
    //-----------------------------------------------------------------------
    MemStream::~MemStream()
    {
        close();
    }
    //-----------------------------------------------------------------------
    void MemStream::setAutoFree(bool b)
    {
        if(b)
            mMark |= AM_CLOSE;
        else
            mMark &= ~AM_CLOSE;
    }
    //-----------------------------------------------------------------------
    NCount MemStream::read(void * out, NCount count) const
    {
        NCount cnt = count;
        // Read over end of memory?
        if(mPos + cnt > mEnd)
            cnt = mEnd - mPos;
        if(cnt == 0)
            return 0;
        N_assert(cnt <= count, "error");
        memcpy(out, mPos, cnt);
        mPos += cnt;
        return cnt;
    }
    //---------------------------------------------------------------------
    NCount MemStream::write(const void * in, NCount count)
    {
        NCount written = 0;
        if (isWriteable())
        {
            written = count;
            // we only allow writing within the extents of allocated memory
            // check for buffer overrun & disallow
            if(mPos + written > mEnd)
                written = mEnd - mPos;
            if(written == 0)
                return 0;
            memcpy(mPos, in, written);
            mPos += written;
        }
        return written;
    }
    //-----------------------------------------------------------------------
    NCount MemStream::readLine(Ntchar * buf, NCount maxCount, const String & delim) const
    {
        // Deal with both Unix & Windows LFs
        bool trimCR = false;
        if (delim.find_first_of(_T('\n')) != String::npos)
        {
            trimCR = true;
        }
        NCount pos = 0;
#if NII_STRING
        NCount dst = 0;
        StrUtil::getUtf8Length((char *)mPos, (char *)mEnd, &pos, &dst, delim.c_str()[0]);
        dst /= 2;
        dst = dst < maxCount ? dst : maxCount;
        DataStream::s2wsByte((Nchar *)mPos, mEnd - mPos, buf, dst);
        if (trimCR && pos && mPos[dst - 1] == _T('\r'))
        {
            --pos;
        }
        mPos += pos;
        buf[dst] = _T('\0');
#else
        Nui8 * tempend = (mPos + maxCount < mEnd) ? mPos + maxCount : mEnd;
        while(mPos < tempend)
        {
            if (delim.find(*((Ntchar *)mPos)) != String::npos)
            {
                if (trimCR && pos && buf[pos - 1] == _T('\r'))
                {
                    --pos;
                }
                mPos += sizeof(Ntchar);
                break;
            }
            buf[pos++] = *((Ntchar *)mPos);
            mPos += sizeof(Ntchar);
        }
        buf[pos] = _T('\0');
#endif
        return pos;
    }
    //-----------------------------------------------------------------------
    NCount MemStream::skipLine(const String & delim) const
    {
        NCount pos = 0;

#if NII_STRING
        StrUtil::getUtf8Length((char *)mPos, (char *)mEnd, &pos, delim.c_str()[0]);
        mPos += pos;
#else
        while (mPos < mEnd)
        {
            pos += sizeof(Ntchar);
            mPos += sizeof(Ntchar);
            if (delim.find(*mPos) != String::npos)
            {
                break;
            }
        }
#endif
        return pos;
    }
    //-----------------------------------------------------------------------
    NCount MemStream::cloneData(Nui8 *& out, NCount oft)
    {
        NCount dst = mSize - oft;
        if(dst > 0)
        {
            out = N_alloc_t(Nui8, dst);
            memcpy(out, mData + oft, dst);
            return dst;
        }

        out = 0;
        return 0;
    }
    //-----------------------------------------------------------------------
    void MemStream::skip(NIIl count)
    {
        NCount newpos = (NCount)((mPos - mData) + count);
        N_assert(mData + newpos <= mEnd, "error");
        mPos = mData + newpos;
    }
    //-----------------------------------------------------------------------
    void MemStream::seek(NCount pos)
    {
        N_assert(mData + pos <= mEnd, "error");
        mPos = mData + pos;
    }
    //-----------------------------------------------------------------------
    NCount MemStream::tell() const
    {
        //mData is start, mPos is current location
        return mPos - mData;
    }
    //-----------------------------------------------------------------------
    bool MemStream::end() const
    {
        return mPos >= mEnd;
    }
    //-----------------------------------------------------------------------
    void MemStream::close()
    {
        if((mMark & AM_CLOSE) && mData)
        {
            N_free(mData);
            mData = 0;
            mTemp = 0;
        }
    }
    //-----------------------------------------------------------------------
    bool MemStream::isSysMemory() const
    {
        return true;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // FileDataStream
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    FileDataStream::FileDataStream(const String & filename, bool read, const String & name,
        bool close)
    {
        std::fstream * f = N_new_t(std::fstream)();

        if (read)
        {
            f->open(filename.c_str(), std::ios::binary | std::ios::in);
        }
        else
        {
            f->open(filename.c_str(), std::ios::binary | std::ios::out);
        }
        FileDataStream(f, name, close);
    }
    //-----------------------------------------------------------------------
    FileDataStream::FileDataStream(std::ifstream * s, const String & name,
        bool close) :
            DataStream(name),
            mIStream(s),
            mStream(0)
    {
        mIStream->seekg(0, std::ios_base::end);
        mSize = mIStream->tellg();
        mIStream->seekg(0, std::ios_base::beg);
        mMark = 0;
        if(mIStream)
            mMark |= AM_READ;
        if(mStream)
            mMark |= AM_WRITE;

        if(close)
            mMark |= AM_CLOSE;
        else
            mMark &= ~AM_CLOSE;
    }
    //-----------------------------------------------------------------------
    FileDataStream::FileDataStream(std::ifstream * s, NCount size,
        const String & name, bool close) :
            DataStream(name),
            mIStream(s),
            mStream(0)
    {
        mSize = size;
        mMark = 0;
        if(mIStream)
            mMark |= AM_READ;
        if(mStream)
            mMark |= AM_WRITE;

        if(close)
            mMark |= AM_CLOSE;
        else
            mMark &= ~AM_CLOSE;
    }
    //-----------------------------------------------------------------------
    FileDataStream::FileDataStream(std::fstream * s, const String & name,
        bool close) :
            DataStream(name, false),
            mIStream(s),
            mStream(s)
    {
        mIStream->seekg(0, std::ios_base::end);
        mSize = mIStream->tellg();
        mIStream->seekg(0, std::ios_base::beg);
        mMark = 0;
        if(mIStream)
            mMark |= AM_READ;
        if(mStream)
            mMark |= AM_WRITE;

        if(close)
            mMark |= AM_CLOSE;
        else
            mMark &= ~AM_CLOSE;
    }
    //-----------------------------------------------------------------------
    FileDataStream::FileDataStream(std::fstream * s, NCount size, 
        const String & name, bool close) :
            DataStream(name, false),
            mIStream(s),
            mStream(s)
    {
        mSize = size;
        mMark = 0;
        if(mIStream)
            mMark |= AM_READ;
        if(mStream)
            mMark |= AM_WRITE;
        if(close)
            mMark |= AM_CLOSE;
        else
            mMark &= ~AM_CLOSE;
    }
    //-----------------------------------------------------------------------
    FileDataStream::~FileDataStream()
    {
        close();
    }
    //-----------------------------------------------------------------------
    NCount FileDataStream::read(void * buf, NCount count) const
    {
        mIStream->read((char *)buf, static_cast<std::streamsize>(count));
        return mIStream->gcount();
    }
    //-----------------------------------------------------------------------
    NCount FileDataStream::write(const void * buf, NCount count)
    {
        NCount written = 0;
        if(isWriteable() && mStream)
        {
            mStream->write((const char *)buf, static_cast<std::streamsize>(count));
            written = count;
        }
        return written;
    }
    //-----------------------------------------------------------------------
    NCount FileDataStream::readLine(Ntchar * buf, NCount count, const String & delim) const
    {
        if(delim.empty())
        {
            N_EXCEPT(InvalidParam, _I18n("No delimiter provided"));
        }
        else if(delim.size() > 1)
        {
            N_Only(Log).log(_I18n("WARNING: FileDataStream::readLine - using only first delimeter"));
        }
        // Deal with both Unix & Windows LFs
        bool trimCR = false;
        if(delim.at(0) == _T('\n'))
        {
            trimCR = true;
        }
        // count + 1 since count excludes terminator in getline
        mIStream->getline((char *)buf, static_cast<std::streamsize>(count + 1) * sizeof(Ntchar), delim.at(0));
        NCount ret = (mIStream->gcount() + sizeof(Ntchar) - 1) / sizeof(Ntchar);
        // three options
        // 1) we had an eof before we read a whole line
        // 2) we ran out of buffer space
        // 3) we read a whole line - in this case the delim character is taken from the stream but not written in the buffer so the read data is of length ret-1 and thus ends at index ret-2
        // in all cases the buffer will be null terminated for us
        if(mIStream->eof())
        {
            // no problem
        }
        else if(mIStream->fail())
        {
            // Did we fail because of count hit? No - no terminating character
            // in included in the count in this case
            if(ret == count)
            {
                // clear failbit for next time
                mIStream->clear();
            }
            else
            {
                N_EXCEPT(Internal, _I18n("Streaming error occurred"));
            }
        }
        else
        {
            // we need to adjust ret because we want to use it as a
            // pointer to the terminating null character and it is
            // currently the length of the data read from the stream
            // i.e. 1 more than the length of the data in the buffer and
            // hence 1 more than the _index_ of the NULL character
            --ret;
        }

        // trim off CR if we found CR/LF
        if(trimCR && buf && ret > 0 && buf[ret - 1] == _T('\r'))
        {
            buf[ret - 1] = _T('\0');
        }
        return ret;
    }
    //-----------------------------------------------------------------------
    void FileDataStream::skip(NIIl count)
    {
#if defined(STLPORT)
        // Workaround for STLport issues: After reached eof of file stream,
        // it's seems the stream was putted in intermediate state, and will be
        // fail if try to repositioning relative to current position.
        // Note: tellg() fail in this case too.
        if (mIStream->eof())
        {
            mIStream->clear();
            // Use seek relative to either begin or end to bring the stream
            // back to normal state.
            mIStream->seekg(0, std::ios::end);
        }
#endif
        mIStream->clear();
        mIStream->seekg(static_cast<std::ifstream::pos_type>(count), std::ios::cur);
    }
    //-----------------------------------------------------------------------
    void FileDataStream::seek(NCount pos)
    {
        mIStream->clear();
        mIStream->seekg(static_cast<std::streamoff>(pos), std::ios::beg);
    }
    //-----------------------------------------------------------------------
    NCount FileDataStream::tell() const
    {
        mIStream->clear();
        return mIStream->tellg();
    }
    //-----------------------------------------------------------------------
    bool FileDataStream::end() const
    {
        return mIStream->eof();
    }
    //-----------------------------------------------------------------------
    void FileDataStream::close()
    {
        if(mIStream)
        {
            // Unfortunately, there is no file-specific shared class hierarchy between fstream and ifstream (!!)
            if(AM_WRITE & mMark)
                static_cast<std::ifstream *>(mIStream)->close();
            if(mStream)
            {
                mStream->flush();
                mStream->close();
            }
            if(mMark & AM_CLOSE)
            {
                // delete the stream too
                if (AM_WRITE & mMark && mIStream)
                {
                    N_delete mIStream;
                    mIStream = 0;
                }
                if (mStream)
                {
                    N_delete mStream;
                    mStream = 0;
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // FileHandleStream
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    FileHandleStream::FileHandleStream(FILE * file, const String & name,
        Nmark mode) :
            DataStream(name, mode),
            mHandle(file)
    {
        // Determine size
        fseek(mHandle, 0, SEEK_END);
        mSize = ftell(mHandle);
        fseek(mHandle, 0, SEEK_SET);
    }
    //-----------------------------------------------------------------------
    FileHandleStream::~FileHandleStream()
    {
        close();
    }
    //-----------------------------------------------------------------------
    NCount FileHandleStream::read(void * buf, NCount count) const
    {
        return fread(buf, 1, count, mHandle);
    }
    //-----------------------------------------------------------------------
    NCount FileHandleStream::write(const void * buf, NCount count)
    {
        if(!isWriteable())
            return 0;
        else
            return fwrite(buf, 1, count, mHandle);
    }
    //-----------------------------------------------------------------------
    void FileHandleStream::skip(NIIl count)
    {
        fseek(mHandle, count, SEEK_CUR);
    }
    //-----------------------------------------------------------------------
    void FileHandleStream::seek(NCount pos)
    {
        fseek(mHandle, static_cast<NIIl>(pos), SEEK_SET);
    }
    //-----------------------------------------------------------------------
    NCount FileHandleStream::tell() const
    {
        return ftell(mHandle);
    }
    //-----------------------------------------------------------------------
    bool FileHandleStream::end() const
    {
        return feof(mHandle) != 0;
    }
    //-----------------------------------------------------------------------
    void FileHandleStream::close()
    {
        if(mHandle != 0)
        {
            fclose(mHandle);
            mHandle = 0;
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //SocketStream
    //-----------------------------------------------------------------------
    //------------------------------------------------------------------------
    SocketStream::SocketStream(Nsocket socket, const String & name, Nmark mode)
    {

    }
    //------------------------------------------------------------------------
    SocketStream::~SocketStream()
    {
    }
    //------------------------------------------------------------------------
    NCount SocketStream::read(void * buf, NCount count) const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    NCount SocketStream::write(const void * buf, NCount count)
    {
        return 0;
    }
    //------------------------------------------------------------------------
    Nui8 * SocketStream::getData()
    {
        return 0;
    }
    //------------------------------------------------------------------------
    void SocketStream::skip(NIIl count)
    {

    }
    //------------------------------------------------------------------------
    void SocketStream::seek(NCount pos)
    {

    }
    //------------------------------------------------------------------------
    NCount SocketStream::tell() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    bool SocketStream::end() const
    {
        return true;
    }
    //------------------------------------------------------------------------
    void SocketStream::close()
    {

    }
    //------------------------------------------------------------------------
    
    
}