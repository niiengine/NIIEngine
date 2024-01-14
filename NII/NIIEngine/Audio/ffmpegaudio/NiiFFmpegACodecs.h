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

#ifndef _NII_FFMPEG_CODECS_H_
#define _NII_FFMPEG_CODECS_H_

#include "NiiPreInclude.h"

struct AVFormatContext;
struct AVCodecContext;
namespace NII
{
    /** NII使用的ffmpeg专用类
    @remark 
        ffmpeg支持很多类型,而且这些类型可能最终都是以音频加视频混合出现的,所以在格
        式不能直接单凭文件后缀来识别
    @par 
        由于ffmpeg不都是以文件后缀形式体现格式的,所以如果你有一个音频格式,无论你知
        不知道这种格式一般是用什么文件后缀的,你可以把这个文件后缀改成".ffmpegA"文
        件后缀名字,让ffmpeg去处理它,如果ffmpeg不能处理的,则会抛出关于为什么不能处
        理的异常消息
    */
    class FFmpegACodec : public AudioCodec , public Lisence
    {
    private:
        typedef struct _FFACodecFile
        {
            AVFormatContext * fmtCtx;   ///< 格式上下文
            AVCodecContext * codecCtx;  ///< 编码器上下文
            NIIi stream;                ///< 音频对象(主对象索引)
        }FFACodecFile;
        typedef map<DataStream *, FFACodecFile>::type BufferObjectList;
    private:
        BufferObjectList mObjectList;   ///< 编码器中临时缓存对象
    public :
        FFmpegACodec();

        virtual ~FFmpegACodec() {}

        /// @copydetails AudioCodec::encode
        DataStream * encode(MemStream * input, const String & outFileName,
            AudioFormat format, CodecDataPtr & data) const;

        /// @copydetails AudioCodec::encode
        void encode(MemStream * input, const String & outFileName, 
            AudioFormat format, CodecDataPtr & data) const;

        /// @copydetails AudioCodec::decode
        DecodeResult decode(DataStream * input, NCount size);

        /// @copydetails AudioCodec::decode
        DecodeResult decode(DataStream * input, NIId time, NCount size);

        /// @copydetails Codec::match
        bool match(const String & hint) const;
        
        /// @copydetails Codec::match
        String match(const Nui8 * magic, NCount valid) const;

        /// @copydetails AudioCodec::rewind
        void rewind(DataStream * input);

        /// @copydetails AudioCodec::close
        static void close(DataStream * input);

        /// @copydetails AudioCodec::close
        static void close();
        
        ///@copydetails AudioCodec::seek
        static bool seek(DataStream * input, NIIi pos);

        ///@copydetails AudioCodec::seek
        static bool seek(DataStream * input, NIId time);        
    private:
        /// 寻找解码管理器中已存在的句柄
        FFACodecFile ref(DataStream * input)const;
    }
}
#endif