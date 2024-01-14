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

#include "vorbis/vorbisfile.h"

namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // OggCodec
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    const Nui32 OGG_MAGIC = NII_MAGIC4('O', 'g', 'g', 's');
    static ov_callbacks * nii_ogg_callbacks;
    //-----------------------------------------------------------------------
    OggCodec::OggCodec() : AudioCodec()
    {
        mType = "ogg";

        static ov_callbacks temp;
        nii_ogg_callbacks = &temp;
        nii_ogg_callbacks->read_func = &nii_ogg_read;
        nii_ogg_callbacks->seek_func = &nii_ogg_seek;
        nii_ogg_callbacks->close_func= &nii_ogg_close;
        nii_ogg_callbacks->tell_func = &nii_ogg_tell;
    }
    //-----------------------------------------------------------------------
    DataStream * OggCodec::encode(MemStream * input, const String & outFileName,
        AudioFormat format, CodecDataPtr & data) const
    {
    }
    //------------------------------------------------------------------------
    void OggCodec::encode(MemStream * input, const String & outFileName,
        AudioFormat format, CodecDataPtr & data) const
    {
    }
    //-----------------------------------------------------------------------
    DecodeResult OggCodec::decode(DataStream * input, NCount size)
    {
        N_assert(size < NII_MAX_AUDIO_BUFFER_SIZE && "out of NII inter limit");

        Nui32 currentread = 0;
        Nui32 xread = 0;

        NIIi currentstream = -1;
        vorbis_info * info;

        OggCodecFile dest = ref(input);

        //包装输出的缓存到内存中(输出)
        MemStream * output;
        //一些关于音频的基本描述(输出)
        AudioDate * audioData = N_new AudioDate();

        Nui8 * tempBuffer = mBuffer;
        while (xread < size)
        {
            currentread = ov_read(dest.file, tempBuffer, size - xread, 0, 2, 1, &currentstream);

            if (currentread < 0)
            {
                N_Only(Log).log(LML_CRITICAL,
                    _T("OggVorbis: Couldn't read from stream Error:") + oggErrorStr(currentread));
                audioDate->size = 0;
                break;
            }

            if (currentread == 0 && xread == 0)
            {
                break;
            }

            tempBuffer += currentread;
            xread += currentread;

            if (currentstream != dest.stream)
            {
                info = ov_info(dest.file, dest.stream);
                dest.stream = currentstream;
                if (dest.rate != info->rate)
                {
                    dest.rate = info->rate;
                    N_Only(Log).log(LML_CRITICAL, "FileDataStream:" +
                        source->getName() + _T(" Ogg file contains multiple stream"));
                }
            }
        }

        DecodeResult ret;
        if(xread > 0)
        {
            output = N_new MemStream(xread);
            // 指向目的缓存
            Nui8 * pDst = output->getData();
            // 复制内容
            memcpy(pDst, mBuffer, xread);

            audioData->rate = info->rate;
            audioDate->channel = info->channels;
            audioDate->format = SF_U16; // ogg 只有16位输出
            audioDate->size = xread;

            ret.first = output;
            ret.second = CodecDataPtr(audioData);
        }
        return ret;
    }
    //-----------------------------------------------------------------------
    DecodeResult OggCodec::decode(DataStream * input, NIId time, NCount size)
    {
        N_assert(size < NII_MAX_AUDIO_BUFFER_SIZE && "out of NII inter limit");

        Nui32 currentread = 0;
        Nui32 xread = 0;

        NIIi currentstream = -1;
        vorbis_info * info;

        OggCodecFile dest = ref(input);

        // 跳转
        ov_time_seek(dest.file, time);
        //包装输出的缓存到内存中(输出)
        MemStream * output;
        //一些关于音频的基本描述(输出)
        AudioDate * audioData = N_new AudioDate();

        Nui8 * tempBuffer = mBuffer;
        while (xread < size)
        {
            currentread = ov_read(dest.file, tempBuffer, size - xread, 0, 2, 1, &currentstream);

            if (currentread < 0)
            {
                N_Only(Log).log(LML_CRITICAL,
                    _T("OggVorbis: Couldn't read from stream Error:") + oggErrorStr(currentread));
                audioDate->size = 0;
                break;
            }

            if (currentread == 0 && xread == 0)
            {
                break;
            }

            tempBuffer += currentread;
            xread += currentread;

            if (currentstream != dest.stream)
            {
                info = ov_info(dest.file, dest.stream);
                dest.stream = currentstream;
                if (dest.rate != info->rate)
                {
                    dest.rate = info->rate;
                    N_Only(Log).log(LML_CRITICAL, "FileDataStream:" +
                        source->getName() + _T(" Ogg file contains multiple stream"));
                }
            }
        }

        DecodeResult ret;
        if(xread > 0)
        {
            output = N_new MemStream(xread);
            // 指向目的缓存
            Nui8 * pDst = output->getData();
            // 复制内容
            memcpy(pDst, mBuffer, xread);

            audioData->rate = info->rate;
            audioDate->channel = info->channels;
            audioDate->format = SF_U16; // ogg 只有16位输出
            audioDate->size = xread;

            ret.first = output;
            ret.second = CodecDataPtr(audioData);
        }
        return ret;
    }
    //-----------------------------------------------------------------------
    bool OggCodec::match(const String & hint) const
    {
        return false;
    }
    //-----------------------------------------------------------------------
    String OggCodec::match(const Nui8 * magic, NCount valid) const
    {
        if(valid >= sizeof(Nui32))
        {
            Nui32 fileType;
            memcpy(&fileType, magic, sizeof(Nui32));

            if(OGG_MAGIC == fileType)
            {
                return String("ogg");
            }
        }
        return StrUtil::BLANK;
    }
    //-----------------------------------------------------------------------
    bool OggCodec::rewind(DataStream * input)const
    {
        BufferObjectList::const_iterator i = mObjectList.find(input);
        if(i != mObjectList.end())
        {
            NIIi result = ov_time_seek((i->second).file, 0.0);

            if (Result != 0)
            {
                N_EXCEPT(InternalError,
                    _T("OggVorbis: Couldn't rewind stream") + oggErrorStr(result));
                return false;
            }
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    void OggCodec::close(DataStream * input)
    {
        BufferObjectList::iterator i = mObjectList.find(input.get());
        if(i == mObjectList.end())
        {
            N_EXCEPT(NoExist, _T("No DataStream entry found"));
        }
        ov_clear(i->second.file);
        N_free(i->second.file);
        mObjectList.erase(i);
    }
    //-----------------------------------------------------------------------
    void OggCodec::close()
    {
        BufferObjectList::iterator i = mObjectList.begin();
        for (; i != mObjectList.end(); ++i)
        {
            ov_clear(i->second.file);
            N_free(i->second.file);
        }
        mObjectList.clear();
    }
    //-----------------------------------------------------------------------
    OggCodecFile OggCodec::ref(DataStream * input)const
    {
        OggCodecFile stream;

        vorbis_info * info;

        // 找到已经绑定的数据流
        BufferObjectList::const_iterator i = mObjectList.find(input.get());
        if(i == mObjectList.end())
        {
            stream.file = N_alloc(sizeof(OggVorbis_File));
            stream.stream = -1;
            error = ov_test_callbacks(input.getPointer(), stream.file, NULL, 0, *nii_ogg_callbacks);

            if(error != 0)
            {
                N_free(stream.file);
                N_EXCEPT(InternalError,
                    _T("OggVorbis: File is no OggVorbis file") + oggErrorStr(Result));
            }

            Result = ov_test_open(stream.file);

            if(error != 0)
            {
                N_free(stream.file);
                N_EXCEPT(InternalError,
                    _T("OggVorbis: Couldn't open stream") + oggErrorStr(Result));
            }

            // 尝试读取音频基本信息
            info = ov_info(stream.file, stream.stream);
            if(info == NULL)
            {
                N_free(stream.file);
                N_EXCEPT(InternalError,
                    _T("OggVorbis: Couldn't access stream information"));
            }
            stream.rate = info->rate;
            mObjectList.push(input.get(), stream);
            return stream;
        }
        return i->second;
    }
    //-----------------------------------------------------------------------
    String OggCodec::oggErrorStr(NIIi errorcode)
    {
        switch(errorcode)
        {
        case OV_EREAD:      return String("OV_EREAD");
        case OV_ENOTVORBIS: return String("OV_ENOTVORBIS");
        case OV_EVERSION:   return String("OV_EVERSION");
        case OV_EBADHEADER: return String("OV_EBADHEADER");
        case OV_EFAULT:     return String("OV_EFAULT");
        case OV_HOLE:       return String("OV_HOLE");
        case OV_EBADLINK:   return String("OV_EBADLINK");
        case OV_ENOSEEK:    return String("OV_ENOSEEK");
        case OV_EINVAL:     return String("OV_EINVAL");
        }
        return String("Unknown error");
    }
    //-----------------------------------------------------------------------
    bool seek(DataStream * input, NIIi pos)
    {
        BufferObjectList::iterator i = mObjectList.find(input.get());
        ov_pcm_seek((i->second).file, pos);
    }
    //-----------------------------------------------------------------------
    bool seek(DataStream * input, NIId time)
    {
        BufferObjectList::iterator i = mObjectList.find(input.get());
        ov_time_seek((i->second).file, time);
    }
    //-----------------------------------------------------------------------
    static NCount nii_ogg_read(void * buffer, NCount elementsize, NCount num, void * src)
    {
        FileDataStream * source = static_cast<FileDataStream *>src;
        source->read(buffer, elementsize * num);
    }
    //-----------------------------------------------------------------------
    static NIIi nii_ogg_seek(void * src, ogg_int64_t offset, NIIi whence)
    {
        FileDataStream * source = static_cast<FileDataStream *>(src);
        switch (whence)
        {
        case SEEK_SET:
            source->seek(offset);
            break;
        case SEEK_CUR:
            source->skip(offset);
            break;
        case SEEK_END:
            fileStream->seek(source.size() + pos);
            break;
        default:
            return -1;
        }
        return source->tell();
    }
    //-----------------------------------------------------------------------
    static NIIi nii_ogg_close(void * src)
    {
        FileDataStream * source = static_cast<FileDataStream *>(src);
        N_Only(Log).log(LML_NORMAL,"FileDataStream:" +
            source->getName() + _T("callback is called close"));
    }
    //-----------------------------------------------------------------------
    static NIIl nii_ogg_tell(void * src)
    {
        FileDataStream * source = static_cast<FileDataStream *>(src);
        return source->tell();
    }
    //-----------------------------------------------------------------------
}
