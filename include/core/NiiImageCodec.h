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

#ifndef _NII_IMAGE_CODEC_H_
#define _NII_IMAGE_CODEC_H_

#include "NiiPreInclude.h"
#include "NiiCodec.h"
#include "NiiImage.h"

namespace NII
{
    /** 图片专用编码基类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ImageCodec : public Codec
    {
    public:
        ImageCodec(CodecID cid, const String & hint);
        virtual ~ImageCodec();

        /** 用于输入码流中的数据解码
        @param[in] in 图片对象
        @param[out] out 输出协议(可以直接是文件名/数据通道),查看niiengine://协议
        @param[in] hint 如果这个编码组件含概多个编码,指定其一
        @version NIIEngine 3.0.0
        */
        virtual CodecResult encode(const Image * in, const String & out, const String & hint) const;

        /** 用于输入码流中的数据解码,
        @param[in] in 图片对象
        @param[out] out 数据流,一般是内存流
        @param[in] hint 如果这个编码组件含概多个编码,指定其一(否则不使用)
        @version NIIEngine 3.0.0
        */
        virtual CodecResult encode(const Image * in, DataStream *& out, const String & hint) const;

        /** 从编码文件解码数据到图片对象.
        @param[in] in 输入协议(可以直接是文件名/数据通道),查看niiengine://协议
        @param[out] out 输出图片对象
        @version NIIEngine 3.0.0
        */
        virtual CodecResult decode(const String & in, Image * out) const;

        /** 从编码文件解码数据到图片对象.
        @param[in] in 类似niiengine://协议的其中一个实现,数据流实现
        @param[out] out 输出图片对象
        @version NIIEngine 3.0.0
        */
        virtual CodecResult decode(DataStream * in, Image * out) const;
    };
}

#endif