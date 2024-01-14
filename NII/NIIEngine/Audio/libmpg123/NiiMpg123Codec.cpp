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
#include "NiiOggCodec.h"
#include "NiiException.h"
#include "NiiLogManager.h"

#include "mpg123.h"

namespace NII
{
    //------------------------------------------------------------------------
    const Nui32 ID3V1_MAGIC = NII_MAGIC3('T', 'A', 'G');
    const Nui32 ID3V2_MAGIC = NII_MAGIC3('I', 'D', '3');
    static const NIIl NII_MPG123_RESYNCLIMIT = 32768;
    static NIIi NII_MPG123_NEXTFD = 0;
    //------------------------------------------------------------------------
    Mpg123Codec::Mpg123Codec() : AudioCodec()
    {
        mType = "mp3";
    }
    //------------------------------------------------------------------------
    DataStream * Mpg123Codec::encode(MemStream * input, const String & outFileName,
        AudioFormat format, CodecDataPtr & data) const
    {
    }
    //------------------------------------------------------------------------
    void Mpg123Codec::encode(MemStream * input, const String & outFileName,
        AudioFormat format, CodecDataPtr & data) const
    {
    }
    //------------------------------------------------------------------------
    DecodeResult Mpg123Codec::decode(DataStream * input, NCount size);
    {
        mpg123_frameinfo info;

        Mpg123CodecFile dest = ref(input);

        mpg123_handle * handle = dest.handle;

        //包装输出的缓存到内存中
        MemStream * output;

        //一些关于音频的基本描述
        AudioDate * audioData = N_new AudioDate();

        //实际读出的数据大小
        NCount readbytes = 0;
        NIIi result = mpg123_read(handle, mBuffer, size, &readbytes);

        NIIi tempEncode =0;
        NIIl tempRate = 0;
        NIIi tempChannels = 0;
        mpg123_getformat(handle, &tempRate, &tempChannels, &tempEncode)

        result = mpg123_info(handle ,&info);

        // MPG123_FORMAT_CHANGED is handled as error here since we fixed the output format above and it shouldn't change here.
        if (result!=MPG123_OK && result!=MPG123_DONE)
        {
            N_Only(LogManager).log(LML_CRITICAL,
                _T("MPG123: Error reading from stream:") + mpg123_strerror(handle));
            readbytes = -1;
        }

        if (Result==MPG123_DONE)
        {
            N_Only(Log).log(LML_NORMAL,
                _T("MPG123: End of stream reached Return:") + String(readbytes));
        }
        DecodeResult ret;
        if(readbytes > 0)
        {
            output = N_new MemStream(readbytes);
            // 指向目的缓存
            Nui8 * pDst = output->getData();
            // 复制内容
            memcpy(pDst, mBuffer, readbytes);

            audioData->rate = tempRate;
            audioDate->channel = tempChannels;
            audioDate->format = FromMpg123SampleFormat(tempEncode);
            audioDate->size = readbytes;

            ret.first = output;
            ret.second = CodecDataPtr(audioData);
        }
        return ret;
    }
    //------------------------------------------------------------------------
    DecodeResult Mpg123Codec::decode(DataStream * input, NIId time, NCount size)
    {
        mpg123_frameinfo info;

        Mpg123CodecFile dest = ref(input);

        mpg123_handle * handle = dest.handle;

        // 包装输出的缓存到内存中
        MemStream * output;

        // 一些关于音频的基本描述
        AudioDate * audioData = N_new AudioDate();

        // 跳转
        mpg123_timeframe(handle, time);

        // 实际读出的数据大小
        NCount readbytes = 0;
        NIIi result = mpg123_read(handle, mBuffer, size, &readbytes);

        NIIi tempEncode =0;
        NIIl tempRate = 0;
        NIIi tempChannels = 0;
        mpg123_getformat(handle, &tempRate, &tempChannels, &tempEncode)

        result = mpg123_info(handle ,&info);

        // MPG123_FORMAT_CHANGED is handled as error here since we fixed the output format above and it shouldn't change here.
        if (result != MPG123_OK && result != MPG123_DONE)
        {
            N_Only(Log).log(LML_CRITICAL,
                _T("MPG123: Error reading from stream:") + mpg123_strerror(handle));
            readbytes = -1;
        }

        if (Result == MPG123_DONE)
        {
            N_Only(Log).log(LML_NORMAL,
                _T("MPG123: End of stream reached Return:") + String(readbytes));
        }

        DecodeResult ret;
        if(readbytes > 0)
        {
            output = N_new MemStream(readbytes);
            // 指向目的缓存
            Nui8 * pDst = output->getData();
            // 复制内容
            memcpy(pDst, mBuffer, readbytes);

            audioData->rate = tempRate;
            audioDate->channel = tempChannels;
            audioDate->format = FromMpg123SampleFormat(tempEncode);
            audioDate->size = readbytes;

            ret.first = output;
            ret.second = CodecDataPtr(audioData);
        }
        return ret;
    }
    //------------------------------------------------------------------------
    String Mpg123Codec::getType() const
    {
        return mType;
    }
    //------------------------------------------------------------------------
    bool Mpg123Codec::match(const String & hint) const;
    {
        return false;
    }
    //------------------------------------------------------------------------
    String Mpg123Codec::match(const NIIb * magicNumberPtr, NCount maxbytes) const
    {
        //ID3v1标准是在文件尾128字节的位置存在一个"TAG",而ID3v2标准则是在头部存在一个"ID3"
        //很难实现一头一尾的检测
        return StrUtil::BLANK;
    }
    //------------------------------------------------------------------------
    bool Mpg123Codec::rewind(DataStream * input) const
    {
        HandleList::const_iterator i = mHandles.find(input);
        if(i != mHandles.end())
        {
            if(mpg123_seek(i->second.handle, 0, SEEK_SET) < 0)
                return false;
            else
                return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    Mpg123CodecFile Mpg123Codec::ref(DataStream * input)
    {
        Mpg123CodecFile stream;
        // 找到已经绑定的数据流
        HandleList::const_iterator i = mHandles.find(input.get());
        if(i == mHandles.end())
        {   // 创建新的
            NIIi result = MPG123_OK;
            NIIi tempFd = NII_MPG123_NEXT_FD();

            stream.handle = mpg123_new(NULL, &result);
            stream.fd = tempFd;
            stream.src = input.get();

            mpg123_handle * handle = stream.handle;

            if (handle == NULL)
            {
                String ErrorStr(mpg123_plain_strerror(result));
                mpg123_delete(handle);
                N_EXCEPT(InternalError,
                    _T("MPG123: Couldn't open stream handle") + ErrorStr);
            }

            result = mpg123_param(handle, MPG123_RESYNC_LIMIT, NII_MPG123_RESYNCLIMIT, 0);

            if(result != MPG123_OK)
            {
                std::string ErrorStr = mpg123_strerror(handle);
                mpg123_delete(handle);
                N_EXCEPT(InternalError, _T("MPG123: Couldn't adjust resync limit") + ErrorStr);
            }
            // Replace read and seek function used for filedescriptors, so they work with pointers to FileI objects from the cf::FileSys.
            result = mpg123_replace_reader(handle, &nii_mpg123_read, &nii_mpg123_seek);
            if (Result != MPG123_OK)
            {
                String ErrorStr(mpg123_strerror(handle));
                mpg123_delete(handle);
                N_EXCEPT(InternalError,
                    _T("MPG123: Couldn't replace reader functions") + ErrorStr);
            }

            if (mpg123_open_fd(handle, tempFd)
            {
                String ErrorStr(mpg123_strerror(handle));
                mpg123_delete(handle);
                N_EXCEPT(InternalError,
                    _T("MPG123: Error opening stream:") + ErrorStr);
            }

            NIIi tempEncode =0;
            NIIl tempRate = 0;
            NIIi tempChannels = 0;
            if(mpg123_getformat(handle, &tempRate, &tempChannels, &tempEncode) != MPG123_OK)
            {
                String ErrorStr(mpg123_strerror(handle));
                mpg123_delete(handle);
                N_EXCEPT(InternalError,
                    _T("MPG123: Error to get stream format:") + ErrorStr);
            }
            if (result == MPG123_OK)
            {
                mHandles.insert(HandleList::type_value(input.get(), stream));
                mStreams.insert(StreamList::type_value(tempFd, stream));
            }
            // Signed 16 is the default output format anyways; it would actually be only different if we forced it.
            N_assert(tempEncode == MPG123_ENC_SIGNED_16);

            // 重至输出
            mpg123_format_none(handle);
            // 设置输出
            mpg123_format(handle, tempRate, tempChannels, tempEncode);

            return stream;
        }

        return i->second;
    }
    //------------------------------------------------------------------------
    void Mpg123Codec::close(DataStream * input)
    {
        HandleList::const_iterator i = mHandles.find(input.get())
        if(i == mHandles.end())
        {
            N_EXCEPT(NoExist, _T("No DataStream entry found"));
        }
        StreamList::const_iterator j = mStreams.find((i->second).fd)
        if(j == mStreams.end())
        {
            N_EXCEPT(InternalError, _T("MPG123: INTERNAL MAP ERROR"));
        }
        mpg123_close(j->second.handle);
        mpg123_delete(j->second.handle);
        mHandles.erase(i);
        mStreams.erase(j);
    }
    //------------------------------------------------------------------------
    void Mpg123Codec::close()
    {
        StreamList::const_iterator j, jend = mStreams.end();
        for(j = mStreams.begin(); j != jend ; ++j)
        {
            mpg123_close((j->second).handle);
            mpg123_delete((j->second).handle);
        }

        mHandles.clear();
        mStreams.clear();
    }
    //------------------------------------------------------------------------
    static NIIi NII_MPG123_NEXT_FD()
    {
        return NII_MPG123_NEXTFD ++;
    }
    //------------------------------------------------------------------------
    static ssize_t nii_mpg123_read(NIIi fd, void * buffer, NCount count)
    {
        N_assert(fd < mStreams.size() && "out streams");

        StreamList::const_iterator j = mStreams.find(fd);
        FileDataStream * source = static_cast<FileDataStream *>((i->second).src);

        return source.read(buffer, count);
    }
    //------------------------------------------------------------------------
    static off_t nii_mpg123_seek(NIIi fd, off_t offset, NIIi whence)
    {
        N_assert(fd < mStreams.size() && "out stream array");

        StreamList::const_iterator j = mStreams.find(fd);
        FileDataStream * source = static_cast<FileDataStream *>((i->second).src);

        switch(whence)
        {
        case SEEK_SET:
            source->seek(offset);
            break;
        case SEEK_CUR:
            source->skip(offset);
            break;
        case SEEK_END:
            source->seek(source->size() + offset);
            break;
        default:
            return -1;
        }
        return source->tell();
    }
    //------------------------------------------------------------------------
    SampleFormat FromMpg123SampleFormat(mpg123_enc_enum in)
    {
        SampleFormat out;
        switch(in)
        {
        case MPG123_ENC_SIGNED_16:
            out = SF_S16;break;
        case MPG123_ENC_16:
        case MPG123_ENC_UNSIGNED_16:
            out = SF_U16;break;
        case MPG123_ENC_8:
        case MPG123_ENC_UNSIGNED_8:
            out = SF_U8;break;
        case MPG123_ENC_SIGNED_8:
            out = SF_S8;break;
        case MPG123_ENC_SIGNED_32:
            out = SF_S32;break;
        case MPG123_ENC_32:
        case MPG123_ENC_UNSIGNED_32:
            out = SF_U32;break;
        case MPG123_ENC_SIGNED_24:
            out = SF_S24;break;
        case MPG123_ENC_24:
        case MPG123_ENC_UNSIGNED_24:
            out = SF_U24;break;
        case MPG123_ENC_FLOAT:
        case MPG123_ENC_FLOAT_32:
            out = SF_F;break;
        case MPG123_ENC_FLOAT_64:
            out = SF_D;break;
        default:
            out = SF_UNKNOWN;
        }
        return out;
    }
    //------------------------------------------------------------------------
    mpg123_enc_enum ToMpg123SampleFormat(SampleFormat in)
    {
        mpg123_enc_enum out;
        switch(in)
        {
        case SF_S8:
            out = MPG123_ENC_SIGNED_8;break;
        case SF_U8:
            out = MPG123_ENC_UNSIGNED_8;break;
        case SF_S16:
            out = MPG123_ENC_SIGNED_16;break;
        case SF_U16:
            out = MPG123_ENC_UNSIGNED_16;break;
        case SF_S24:
            out = MPG123_ENC_SIGNED_24;break;
        case SF_U24:
            out = MPG123_ENC_UNSIGNED_24;break;
        case SF_S32:
            out = MPG123_ENC_SIGNED_32;break;
        case SF_U32:
            out = MPG123_ENC_UNSIGNED_32;break;
        case SF_F:
            out = MPG123_ENC_FLOAT_32;break;
        case SF_D:
            out = MPG123_ENC_FLOAT_64;break;
        default:
            out = MPG123_ENC_ANY;
        }
        return out;
    }
    //------------------------------------------------------------------------
    bool Mpg123Codec::seek(DataStream * input, NIIi pos)
    {
        HandleList::const_iterator j = mHandles.find(fd);
        mpg123_seek((j->second).handle, pos, SEEK_SET);
    }
    //------------------------------------------------------------------------
    bool Mpg123Codec::seek(DataStream * input, NIId time)
    {
        HandleList::const_iterator j = mHandles.find(fd);
        mpg123_timeframe((j->second).handle, time);
    }
    //------------------------------------------------------------------------
}