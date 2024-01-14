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
#include "NiiFFmpegACodecs.h"

#include "NiiException.h"
#include "NiiLogManager.h"

extern "C"
{
  #include "libavformat/avio.h"
  #include "libavformat/avformat.h"
  #include "libavcodec/avcodec.h"
}

//nii使用的网络协议头
#define Nii_URL "nii://"
#define Nii_Protocol "nii"
#define Nii_Protocol_Size 6
namespace NII
{
    //-----------------------------------------------------------------------
    FFmpegACodec::FFmpegACodec() : mType("ffmpegA")
    {
        static URLProtocol protocol;
        protocol.name = Nii_Protocol;
        protocol.url_open  = nii_ffmpega_open;
        protocol.url_read  = nii_ffmpega_read;
        protocol.url_write = nii_ffmpega_write;
        protocol.url_seek  = nii_ffmpega_seek;
        protocol.url_close = nii_ffmpega_close;
        // 注册这个协议
        av_register_protocol2(&protocol, sizeof(protocol));
        // 把log输出扔回给我们自己
        av_log_set_callback(&nii_ffmpega_log);
    }
    //-----------------------------------------------------------------------
    DataStream * FFmpegACodec::encode(MemStream * input, const String & outFileName,
        AudioFormat format, CodecDataPtr & data) const
    {

    }
    //-----------------------------------------------------------------------
    void FFmpegACodec::encode(MemStream * input, const String & outFileName,
        AudioFormat format, CodecDataPtr & data) const
    {

    }
    //-----------------------------------------------------------------------
    DecodeResult FFmpegACodec::decode(DataStream * input, NCount size);
    {
        //一些关于音频的基本描述
        AudioDate * audioData = N_new AudioDate();

        FFACodecFile codefile = ref(input);
        AVCodecContext * codecCtx = codefile.codecCtx;
        AVFormatContext * fmtCtx =  codefile.fmtCtx;
        NIIi stream = codefile.stream;

        audioData->format = FromFFmpegASampleFormat(codecCtx->sample_fmt);
        audioData->rate = codecCtx->sample_rate;
        audioData->channel = codecCtx->channels;

        static AVPacket packet;
        Nui8 * pktdata;
        NIIi pktsize;
        // 在一个读取行为中实际读取出来的字节数目
        NIIi out_size;
        //用于存储临时缓存的开始处
        Nui8 * temp = mBuffer;
        // 最终读取的字节数目
        Nui32 readsize = 0;
        //pInFmtCtx中调用对应格式的packet获取函数
        while(readsize<=size && av_read_frame(fmtCtx, &packet)>=0)
        {
            if(packet.stream_index == stream)//如果是音频
            {
                pktdata = packet.data;
                pktsize = packet.size;
                while(pktsize > 0)
                {
                    out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE * 3;
                    //解码
                    NIIi len = avcodec_decode_audio2(codecCtx,(NIIs*)temp, &out_size,
                        pktdata, pktsize);
                    if (len < 0)
                    {
                        avcodec_close(codecCtx);
                        av_close_input_file(fmtCtx);
                        N_EXCEPT(NoExist, _T("Error while decoding"));
                        break;
                    }
                    if(out_size > 0)
                    {
                        temp += out_size;
                        readsize += out_size;
                    }
                    pktsize -= len;
                    pktdata += len;
                }
            }
            av_free_packet(&packet);
        }
        //包装输出的缓存到内存中
        MemStream * output;
        output = N_new MemStream(readsize);
        audioDate->size = readsize;
        Nui8 * pDst = output->getData();
        //复制实际需要的缓存
        memcpy(pDst, mBuffer, readsize);

        DecodeResult ret;
        ret.first = output;
        ret.second = CodecDataPtr(audioData);
        return ret;
    }
    //-----------------------------------------------------------------------
    DecodeResult decode(DataStream * input, NIId time, NCount size)const
    {
        //一些关于音频的基本描述
        AudioDate * audioData = N_new AudioDate();

        FFACodecFile codefile = ref(input);
        AVCodecContext * codecCtx = codefile.codecCtx;
        AVFormatContext * fmtCtx =  codefile.fmtCtx;

        NIIi stream = codefile.stream;
        audioData->format = FromFFmpegASampleFormat(codecCtx->sample_fmt);
        audioData->rate = codecCtx->sample_rate;
        audioData->channel = codecCtx->channels;

        static AVPacket packet;
        Nui8 * pktdata;
        NIIi pktsize;
        // 在一个读取行为中实际读取出来的字节数目
        NIIi out_size;
        //用于存储临时缓存的开始处
        Nui8 * temp = mBuffer;
        // 最终读取的字节数目
        Nui32 readsize = 0;
        // 跳转
        seek(input, time);
        //pInFmtCtx中调用对应格式的packet获取函数
        while(readsize<=size && av_read_frame(fmtCtx, &packet)>=0)
        {
            if(packet.stream_index == stream)//如果是音频
            {
                pktdata = packet.data;
                pktsize = packet.size;
                while(pktsize > 0)
                {
                    out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE * 3;
                    //解码
                    NIIi len = avcodec_decode_audio2(codecCtx,(NIIs*)temp, &out_size,
                        pktdata, pktsize);
                    if (len < 0)
                    {
                        avcodec_close(codecCtx);
                        av_close_input_file(fmtCtx);
                        N_EXCEPT(NoExist, _T("Error while decoding"));
                        break;
                    }
                    if(out_size > 0)
                    {
                        temp += out_size;
                        readsize += out_size;
                    }
                    pktsize -= len;
                    pktdata += len;
                }
            }
            av_free_packet(&packet);
        }
        //包装输出的缓存到内存中
        MemStream * output;
        output = N_new MemStream(readsize);
        audioDate->size = readsize;
        Nui8 * pDst = output->getData();
        //复制实际需要的缓存
        memcpy(pDst, mBuffer, readsize);

        DecodeResult ret;
        ret.first = output;
        ret.second = CodecDataPtr(audioData);
        return ret;
    }
    //-----------------------------------------------------------------------
    FFACodecFile FFmpegACodec::ref(DataStream * input)const
    {
        ostringstream stream;

        stream << Nii_URL << (void *)(input.getPointer());
        String filename = stream.str();

        if (filename == NULL)
            N_EXCEPT(NoExist, _T("file name is null"));

        // 找到已经绑定的数据流
        BufferObjectList::const_iterator i = mObjectList.find(input.get());
        if(i == mObjectList.end())
        {
            FFACodecFile file;
            file.fmtCtx = 0;
            //获取文件格式
            if (av_open_input_file(&(file.fmtCtx), filename.c_str(), NULL, 0, NULL)!=0)
                N_EXCEPT(NoExist, _T("av_open_input_file error"));

            //获取文件内音视频流的信息
            if(av_find_stream_info(file.fmtCtx) < 0)
                N_EXCEPT(NoExist, _T("av_find_stream_info error"));
            AVCodecContext * codecCtx = file.codecCtx;
            // 找到第一个音频流
            NIIi audioStream = -1;
            NIIi j;
            for(j = 0; j < fmtCtx->nb_streams; j++)//找到音频对应的stream
            if(fmtCtx->streams[j]->codec->codec_type == CODEC_TYPE_AUDIO)
            {
                audioStream = j;
                break;
            }
            if(audioStream == -1)
            {
                N_EXCEPT(NoExist, _T("input file has no audio stream"));
                return 0; // Didn't find a audio stream
            }
            file.stream = audioStream;
            file.codecCtx = file.fmtCtx->streams[audioStream]->codec;
            //音频的编码上下文
            AVCodecContext * codecCtx = file.codecCtx;

            NIIi streams = codefile->stream;

            //根据编码ID找到用于解码的结构体
            AVCodec * pInCodec = avcodec_find_decoder(codecCtx->codec_id);

            if(pInCodec == NULL)
            {
                N_EXCEPT(NoExist, _T("error no Codec found"));
            }

            //测试一下是否存在具体的解码器
            if(avcodec_open(codecCtx, pInCodec) < 0)
            {
                N_EXCEPT(NoExist, _T("error avcodec_open failed"));
            }

            mObjectList.push(input.get(), file);

            return file;
        }
        return i->second;
    }
    //-----------------------------------------------------------------------
    bool FFmpegACodec::match(const String & hint) const
    {
        return false;
    }
    //-----------------------------------------------------------------------
    String FFmpegACodec::match(const Nui8 * magic, NCount valid) const
    {
        // FFMPEG uses magic internally to determine file types when
        // necessary but does not expose the code in its API.
        // This makes it difficult to implement this function, but also
        // reduces its importance. Just for now, here is a kludge to
        // get Nii to build and ensure that it always tries to load files
        // that FFMPEG might be able to load.
        return StrUtil::BLANK;
    }
    //-----------------------------------------------------------------------
    NIIi nii_ffmpega_open(URLContext * h, const NIIb * filename, NIIi flags)
    {
        filename += Nii_Protocol_Size;

        istringstream stream(filename);
        void * ptr = 0;
        stream >> ptr;

        h->priv_data = ptr;
        return 0;
    }
    //-----------------------------------------------------------------------
    NIIi nii_ffmpega_read(URLContext * h, Nui8 * buf, NIIi size)
    {
        FileDataStream * source = static_cast<FileDataStream *>(h->priv_data);

        return source->read(buf,size);
    }
    //-----------------------------------------------------------------------
    NIIi nii_ffmpega_write(URLContext * h, const Nui8 * buf, NIIi size)
    {
        FileDataStream * source = static_cast<FileDataStream *>(h->priv_data);

        return source->write(buf,size);
    }
    //-----------------------------------------------------------------------
    int64_t nii_ffmpega_seek(URLContext * h, int64_t pos, NIIi whence)
    {
        FileDataStream * source = static_cast<FileDataStream *>(h->priv_data);
        switch(whence)
        {
        case SEEK_SET:
            source->seek(pos);
            break;
        case SEEK_CUR:
            source->skip(pos);
            break;
        case SEEK_END:
            source->seek(source.size() + pos);
            break;
        case AVSEEK_SIZE:
            return source->size();
        default:
            source->tell();//clear error status
            return -1;
        }
        return source->tell();
    }
    //-----------------------------------------------------------------------
    NIIi nii_ffmpega_close(URLContext * h)
    {
        return 0;
    }
    //-----------------------------------------------------------------------
    void nii_ffmpega_log(void * ptr, NIIi level, const NIIb * fmt, va_list v1)
    {
        StringStream str;
        str << fmt;
        LogLevel i;
#ifdef AV_LOG_PANIC
        if (level <= AV_LOG_PANIC)
        {
            i = LML_CRITICAL;
        }
#endif
        else if (level <= AV_LOG_ERROR)
        {
            i = LML_CRITICAL;
#ifdef AV_LOG_WARNING
        }
        else if (level <= AV_LOG_WARNING)
        {
            i = LML_NORMAL;
#endif

        }
        else if (level <= AV_LOG_INFO)
        {
            i = LML_NORMAL;

#ifdef AV_LOG_VERBOSE
        }
        else if (level <= AV_LOG_VERBOSE)
        {
            i = LML_TRIVIAL;
#endif
        }
        else /* level <= AV_LOG_DEBUG */
        {
            i = LML_TRIVIAL;
        }
        static const NCount buffer_size = 4096;
        static NIIb buffer[buffer_size];
        vsnprintf(buffer, buffer_size, fmt, v1);

        N_Only(Log).log(i,String(buffer));
    }
    //-----------------------------------------------------------------------
    void FFmpegACodec::close(DataStream * input)
    {
        BufferObjectList::iterator i = mObjectList.find(input.get());
        if(i == mObjectList.end())
        {
            N_EXCEPT(NoExist, _T("No DataStream entry found"));
        }
        avcodec_close(i->second.codecCtx);
        av_close_input_file(i->second.fmtCtx);
        mObjectList.erase(i);
    }
    //-----------------------------------------------------------------------
    void FFmpegACodec::close()
    {
        BufferObjectList::iterator i, iend = mObjectList.end();
        for (i = mObjectList.begin(); i != iend; ++i)
        {
            avcodec_close(i->second.codecCtx);
            av_close_input_file(i->second.fmtCtx);
        }
        mObjectList.clear();
    }
    //-----------------------------------------------------------------------
    static SampleFormat FromFFmpegASampleFormat(AVSampleFormat in)
    {
        SampleFormat a;
        switch(in)
        {
        case AV_SAMPLE_FMT_NONE:
        case AV_SAMPLE_FMT_NB:
            a = SF_UNKNOWN;break;
        case AV_SAMPLE_FMT_U8:
        case AV_SAMPLE_FMT_U8P:
            a = SF_U8;break;
        case AV_SAMPLE_FMT_S16:
        case AV_SAMPLE_FMT_S16P:
            a = SF_S16;break;
        case AV_SAMPLE_FMT_S32:
        case AV_SAMPLE_FMT_S32P:
            a = SF_S32;break;
        case AV_SAMPLE_FMT_FLT:
        case AV_SAMPLE_FMT_FLTP:
            a = SF_F;break;
        case AV_SAMPLE_FMT_DBL:
        case AV_SAMPLE_FMT_DBLP:
            a = SF_D;break;
        }
        return a;
    }
    //-----------------------------------------------------------------------
    static AVSampleFormat ToFFmpegASampleFormat(SampleFormat in)
    {
        AVSampleFormat format;
        switch(in)
        {
        case SF_U8:
        case SF_S8:
            format = AV_SAMPLE_FMT_U8;break;
        case SF_S16:
        case SF_U16:
            format = AV_SAMPLE_FMT_S16;break;
        case SF_S32:
        case SF_U32:
            format = AV_SAMPLE_FMT_S32;break;
        case SF_F:
            format = AV_SAMPLE_FMT_FLT;break;
        case SF_F64:
            format = AV_SAMPLE_FMT_DBL;break;
        default:
            format = SF_UNKNOWN;
        }
        return format;
    }
    //-----------------------------------------------------------------------
    bool FFmpegACodec::seek(DataStream * input, NIIi pos)
    {
        FFACodecFile codefile = ref(input);

        AVCodecContext * codecCtx = codefile.codecCtx;
        AVFormatContext * fmtCtx =  codefile.fmtCtx;
        NIIi stream_index = codefile.stream;

        uint64_t st_time = fmtCtx->start_time;

        uint64_t seek_pos = ((uint64_t)pos)*((uint64_t)AV_TIME_BASE)
            /((uint64_t)codecCtx->sample_rate);

        if (seek_pos < 0)
        {
            seek_pos = 0;
        }
        if (st_time != AV_NOPTS_VALUE)
        {
            seek_pos += st_time;
        }
        if(av_seek_frame(m_formatCtx,stream_index,seek_pos,AVSEEK_FLAG_BACKWARD)<0)
        {
            N_EXCEPT(InternalError, _T("Cann't seek codec stream"));
        }
        else
        {
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool FFmpegACodec::seek(DataStream * input, NIId time)
    {
        FFACodecFile codefile = ref(input);

        AVFormatContext * fmtCtx =  codefile.fmtCtx;
        NIId pos = time * AV_TIME_BASE;
        NIIi stream_index = codefile.stream;

        int64_t seek_target= av_rescale_q((int64_t)pos,AV_TIME_BASE_Q,
            fmtCtx->streams[stream_index]->time_base);

        if(av_seek_frame(fmtCtx,stream_index,seek_target,AVSEEK_FLAG_BACKWARD)<0)
        {
            N_EXCEPT(InternalError, _T("Cann't seek codec stream"));
        }
        else
        {
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    void rewind(DataStream * input)
    {
        seek(input, 0.0);
    }
    //-----------------------------------------------------------------------
}