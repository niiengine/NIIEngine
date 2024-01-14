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

#ifndef _NII_FFMPEG_CODECS_H__
#define _NII_FFMPEG_CODECS_H__

#include "NiiPreInclude.h"

struct OggVorbis_File;
struct vorbis_info;

namespace NII
{
    class OggCodec : public AudioCodec
    {
    private:
        typedef struct _OggCodecFile
        {
            OggVorbis_File * file;          ///< 主对象
            NIIi rate;                      ///< 当前流的频率
            NIIi stream;                    ///< 当前流的标识
        }OggCodecFile;
        typedef map<DataStream *, OggCodecFile>::type BufferObjectList;
    private:
        OggCodec * mInstance;               ///< 解码实例
        BufferObjectList mObjectList;       ///< 编码器中临时缓存对象
    public :
        OggCodec();

        virtual ~OggCodec() {}

        /// @copydetails AudioCodec::
        void encode(MemStream * in, EncodeJob * out) const;

        /// @copydetails AudioCodec::
        void decode(DataStream * in, DecodeJob * out) const;

        /// @copydetails AudioCodec::
        void decode(const String & in, DecodeJob * out) const;

        /// @copydetails AudioCodec::
        bool match(const String & hint) const;

        /// @copydetails AudioCodec::
        String match(const Nui8 * magic, NCount valid) const;

        /// @copydetails Audiocodec::rewind
        bool rewind(DataStream * input)const;

        /// @copydetails Audiocodec::close
        static void close(DataStream * input);

        /// @copydetails Audiocodec::close
        static void close();

        ///@copydetails Audiocodec::seek
        static bool seek(DataStream * input, NIIi pos);

        ///@copydetails Audiocodec::seek
        static bool seek(DataStream * input, NIId time);
    private:
        /// 转换错误标号为字符串
        static String oggErrorStr(NIIi errorcode);

        /// 寻找解码管理器中已存在的句柄
        OggCodecFile ref(DataStream * input);
    };
}