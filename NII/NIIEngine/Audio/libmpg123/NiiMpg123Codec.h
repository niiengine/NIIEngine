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

#ifndef _NII_MPG123_CODEC_H_
#define _NII_MPG123_CODEC_H_

#include "NiiPreInclude.h"

struct mpg123_handle;

namespace NII
{
    class Mpg123Codec : public AudioCodec
    {
    private:
        typedef struct _Mpg123CodecFile
        {
            mpg123_handle * handle;         ///< 主对象
            NIIi fd;                        ///< 当前使用的内部文件句柄
            DataStream * src;               ///< 源头
        }Mpg123CodecFile;
        /// 正推倒,加快搜索速度
        typedef map<DataStream *, Mpg123CodecFile>::type HandleList;
        /// 反推倒,加快搜索速度
        typedef map<NIIi, Mpg123CodecFile>::type StreamList;
    private:
        HandleList mHandles;                ///< 句柄池
        StreamList mStreams;                ///< 实例池,加快映射
    public :
        Mpg123Codec();

        virtual ~Mpg123Codec() {}

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

        /// @copydetails AudioCodec::rewind
        bool rewind(DataStream * input)

        /// @copydetails AudioCodec::close
        void close(DataStream * input);

        /// @copydetails AudioCodec::close
        void close();

        ///@copydetails AudioCodec::seek
        bool seek(DataStream * input, NIIi pos);

        ///@copydetails AudioCodec::seek
        bool seek(DataStream * input, NIIl time);
    private:
        /// 寻找解码管理器中已存在的句柄
        Mpg123CodecFile ref(DataStream * input);
    };
}
#endif