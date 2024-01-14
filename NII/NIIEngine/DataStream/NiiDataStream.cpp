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
#include "NiiDataStream.h"
#include "NiiLogManager.h"
#include "NiiException.h"

namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // DataStream
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    DataStream::DataStream(Nmark mode) :
        mSize(0),
        mMark(mode)
    {
    }
    //-----------------------------------------------------------------------
    DataStream::DataStream(const String & name, Nmark mode) :
        mName(name),
        mSize(0),
        mMark(mode)
    {
    }
    //-----------------------------------------------------------------------
    DataStream::~DataStream()
    {
    }
    //-----------------------------------------------------------------------
    String DataStream::readLine(bool trim) const
    {
        char tmpBuf[NII_STREAM_TEMP_SIZE + 1];
        String re;
        NCount count;
        // Keep looping while not hitting delimiter
        while((count = read(tmpBuf, NII_STREAM_TEMP_SIZE)) != 0)
        {
            tmpBuf[count] = '\0';
#if NII_STRING
            NCount bsize = 0;
            StrUtil::getUtf8Length(tmpBuf, &bsize, '\n');
            if(bsize < count)
            {
                const_cast<DataStream *>(this)->skip((NIIl)(bsize - count + 1));
                tmpBuf[bsize] = '\0';
#else
            Ntchar * p = Nstrchr(tmpBuf, _T('\n'));
            if(p != 0)
                const_cast<DataStream *>(this)->skip((NIIl)(p - tmpBuf - count + 1));

            *p = _T('\0');
#endif
            }

#if NII_STRING
            s2wsAppend(tmpBuf, re);
            if (bsize < count)
            {
                if (re.length() && re[re.length() - 1] == _T('\r'))
                {
                    re.erase(re.length() - 1, 1);
                }
                break;
            }
#else
            re += tmpBuf;
            if(p != 0)
            {
                // Trim off trailing CR if this was a CR/LF entry
                if(re.length() && re[re.length() - 1] == _T('\r'))
                {
                    re.erase(re.length() - 1, 1);
                }
                break;
            }
#endif
        }
        if(trim)
        {
            StrUtil::trim(re);
        }
        return re;
    }
    //-----------------------------------------------------------------------
    NCount DataStream::readLine(Ntchar * buf, NCount maxCount, const String & delim) const
    {
        // Deal with both Unix & Windows LFs
        bool trimCR = false;
        if(delim.find_first_of(_T('\n')) != String::npos)
        {
            trimCR = true;
        }
        char tmpBuf[NII_STREAM_TEMP_SIZE + 1];
        String tempre;
        NCount chunkSize = std::min(maxCount, (NCount)NII_STREAM_TEMP_SIZE) * sizeof(Ntchar);
        NCount totalCount = 0;
        NCount totalSize = 0;
        NCount count;
        while(chunkSize && (count = read(tmpBuf, chunkSize)) != 0)
        {
            tmpBuf[count] = '\0';
#if NII_STRING
            NCount bsize = 0;
            StrUtil::getUtf8Length(tmpBuf, &bsize, delim.c_str()[0]);
            if (bsize < count)
            {
                tmpBuf[bsize] = '\0';
                s2wsAppend(tmpBuf, tempre);
                if (buf)
                {
                    Nmemcpy(buf + totalCount, tempre.c_str(), tempre.length());
                }
                totalCount += tempre.length();
                totalSize += bsize;
                // Found terminator, reposition backwards
                const_cast<DataStream *>(this)->skip((NIIl)(bsize - count + 1));
                // Trim off trailing CR if this was a CR/LF entry
                if (trimCR && totalCount && buf && buf[totalCount - 1] == _T('\r'))
                {
                    --totalCount;
                }
                if (buf)
                    buf[totalCount] = _T('\0');
                break;
            }
            else
            {
                s2wsAppend(tmpBuf, tempre);
                if (buf)
                {
                    Nmemcpy(buf + totalCount, tempre.c_str(), tempre.length());
                }
                totalCount += tempre.length();
                totalSize += bsize;
            }
            chunkSize = std::min(maxCount - totalCount, (NCount)NII_STREAM_TEMP_SIZE) * sizeof(Ntchar);
#else
            // Find first delimiter
            NCount pos = Nstrcspn(tmpBuf, delim.c_str());
            if(pos < count)
            {
                if (buf)
                {
                    Nmemcpy(buf + totalSize, tmpBuf, pos);
                }
                totalSize += pos + 1;
                // Found terminator, reposition backwards
                const_cast<DataStream *>(this)->skip((NIIl)(pos - count + 1));
                // Trim off trailing CR if this was a CR/LF entry
                if(trimCR && totalSize && buf && buf[totalSize - 1] == _T('\r'))
                {
                    --totalSize;
                }
                if (buf)
                    buf[totalSize] = _T('\0');
                break;
            }
            else
            {
                if (buf)
                {
                    Nmemcpy(buf + totalSize, tmpBuf, pos);
                }
                totalSize += pos;
            }
            // Adjust chunkSize for next time
            chunkSize = std::min(maxCount - totalSize, (NCount)NII_STREAM_TEMP_SIZE);
#endif
        }
        return totalSize;
    }
    //-----------------------------------------------------------------------
    NCount DataStream::skipLine(const String & delim) const
    {
        char tmpBuf[NII_STREAM_TEMP_SIZE + 1];
        NCount total = 0;
        NCount count;
        // Keep looping while not hitting delimiter
        while((count = read(tmpBuf, NII_STREAM_TEMP_SIZE)) != 0)
        {
            // Terminate string
            tmpBuf[count] = '\0';
#if NII_STRING
            NCount tsize = 0;
            StrUtil::getUtf8Length(tmpBuf, &tsize, delim.c_str()[0]);
            if(tsize < count)
            {
                const_cast<DataStream *>(this)->skip((NIIl)(tsize - count + 1));
                total += tsize;
#else
            // Find first delimiter
            NCount pos = Nstrcspn(tmpBuf, delim.c_str());
            if(pos < count)
            {
                // Found terminator, reposition backwards
                skip((NIIl)(pos - count + 1));
                total += pos + 1;
#endif
                // break out
                break;
            }
#if NII_STRING
            total += tsize;
#else
            total += count;
#endif
        }
        return total;
    }
    //-----------------------------------------------------------------------
    void DataStream::skipCloseBrace()
    {
        String line;
        while(!end() && line != _T("}"))
        {
            line = readLine();
        }
    }
    //-----------------------------------------------------------------------
    NCount DataStream::cloneData(Nui8 *& out, NCount oft)
    {
        NCount dst = mSize - oft;
        if (dst > 0)
        {
            NCount lasttell = tell();
            seek(oft);

            out = N_alloc_t(Nui8, dst);
            read(out, dst);

            seek(lasttell);
            return dst;
        }
        out = 0;
        return 0;
    }
    //-----------------------------------------------------------------------
    void DataStream::skipOpenBrace()
    {
        String line;
        while(!end() && line != _T("{"))
        {
            line = readLine();
        }
    }
    //-----------------------------------------------------------------------
    bool DataStream::isReadable() const
    {
        return (mMark & AM_READ) != 0;
    }
    //-----------------------------------------------------------------------
    bool DataStream::isWriteable() const
    {
        return (mMark & AM_WRITE) != 0;
    }
    //-----------------------------------------------------------------------
    bool DataStream::isSysMemory() const
    {
        return false;
    }
    //-----------------------------------------------------------------------
    NCount DataStream::read(void * out, NCount count) const
    {
        return 0;
    }
    //-----------------------------------------------------------------------
    NCount DataStream::write(const void *, NCount)
    {
        return 0;
    }
    //-----------------------------------------------------------------------
    void DataStream::ws2sAppend(const WString & src, VString & out)
    {
        std::locale sys_locale("");

        const wchar_t* data_from = src.c_str();
        const wchar_t* data_from_end = src.c_str() + src.size();
        const wchar_t* data_from_next = 0;

        size_t wchar_size = 4;
        char* data_to = (char *)malloc((src.size() + 1) * wchar_size);
        char* data_to_end = data_to + (src.size() + 1) * wchar_size;
        char* data_to_next = 0;

        memset(data_to, 0, (src.size() + 1) * wchar_size);

        typedef std::codecvt<wchar_t, char, mbstate_t> convert_facet;
        mbstate_t out_state;
        memset(&out_state, 0, sizeof(mbstate_t));
        auto result = std::use_facet<convert_facet>(sys_locale).out(
            out_state, data_from, data_from_end, data_from_next,
            data_to, data_to_end, data_to_next);
        if (result == convert_facet::ok || result == convert_facet::partial)
        {
            out.append(data_to);
            free(data_to);
        }
        else
        {
            //printf("convert error!\n");
            free(data_to);
        }
    }
    //-----------------------------------------------------------------------
    void DataStream::s2wsAppend(const VString& src, WString & out)
    {
        std::locale sys_locale("");

        const char* data_from = src.c_str();
        const char* data_from_end = src.c_str() + src.size();
        const char* data_from_next = 0;

        wchar_t* data_to = (wchar_t *)malloc(sizeof(wchar_t) * (src.size() + 1));
        wchar_t* data_to_end = data_to + src.size() + 1;
        wchar_t* data_to_next = 0;

        wmemset(data_to, 0, src.size() + 1);

        typedef std::codecvt<wchar_t, char, mbstate_t> convert_facet;
        mbstate_t in_state;
        memset(&in_state, 0, sizeof(mbstate_t));
        auto result = std::use_facet<convert_facet>(sys_locale).in(
            in_state, data_from, data_from_end, data_from_next,
            data_to, data_to_end, data_to_next);
        if (result == convert_facet::ok || result == convert_facet::partial)
        {
            out.append(data_to);
            free(data_to);
        }
        else
        {
            //printf("convert error!\n");
            free(data_to);
        }
    }
    //-----------------------------------------------------------------------
    void DataStream::ws2sByte(const Nwchar * src, NCount srccnt, Nchar * out, NCount outcnt)
    {
        std::locale sys_locale("");

        const wchar_t* data_from = src;
        const wchar_t* data_from_end = src + srccnt;
        const wchar_t* data_from_next = 0;

        size_t wchar_size = 4;
        char* data_to = (char *)malloc((srccnt + 1) * wchar_size);
        char* data_to_end = data_to + (srccnt + 1) * wchar_size;
        char* data_to_next = 0;

        memset(data_to, 0, (srccnt + 1) * wchar_size);

        typedef std::codecvt<wchar_t, char, mbstate_t> convert_facet;
        mbstate_t out_state;
        memset(&out_state, 0, sizeof(mbstate_t));
        auto result = std::use_facet<convert_facet>(sys_locale).out(
            out_state, data_from, data_from_end, data_from_next,
            data_to, data_to_end, data_to_next);
        if (result == convert_facet::ok || result == convert_facet::partial)
        {
            ::memcpy(out, data_to, outcnt < srccnt ? outcnt : srccnt);
            free(data_to);
        }
        else
        {
            //printf("convert error!\n");
            free(data_to);
        }
    }
    //-----------------------------------------------------------------------
    void DataStream::s2wsByte(const Nchar * src, NCount srccnt, Nwchar * out, NCount outcnt)
    {
        std::locale sys_locale("");

        const char* data_from = src;
        const char* data_from_end = src + srccnt;
        const char* data_from_next = 0;

        wchar_t* data_to = (wchar_t *)malloc(sizeof(wchar_t) * (srccnt + 1));
        wchar_t* data_to_end = data_to + srccnt + 1;
        wchar_t* data_to_next = 0;

        wmemset(data_to, 0, srccnt + 1);

        typedef std::codecvt<wchar_t, char, mbstate_t> convert_facet;
        mbstate_t in_state;
        memset(&in_state, 0, sizeof(mbstate_t));
        auto result = std::use_facet<convert_facet>(sys_locale).in(
            in_state, data_from, data_from_end, data_from_next,
            data_to, data_to_end, data_to_next);
        if (result == convert_facet::ok || result == convert_facet::partial)
        {
            ::wmemcpy(out, data_to, outcnt < srccnt ? outcnt : srccnt);
            free(data_to);
        }
        else
        {
            //printf("convert error!\n");
            free(data_to);
        }
    }
    //-----------------------------------------------------------------------
    void DataStream::ws2s(const WString & in, VString & out)
    {
        out.clear();
        ws2sAppend(in, out);
    }
    //-----------------------------------------------------------------------
    void DataStream::s2ws(const VString & in, WString & out)
    {
        out.clear();
        s2wsAppend(in, out);
    }
    //-----------------------------------------------------------------------
}