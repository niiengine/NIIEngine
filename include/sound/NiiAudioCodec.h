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

#ifndef _NII_AUDIO_CODEC_H_
#define _NII_AUDIO_CODEC_H_

#include "NiiPreInclude.h"
#include "NiiCodec.h"
#include "NiiAudio.h"

namespace NII
{
    /** 音频编码专用基类
    @note 和图片解码不同,音频编码是流媒体
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AudioCodec : public Codec
    {
    public:
        AudioCodec(CodecID cid, const String & hint);
        virtual ~AudioCodec();

        /** 是否可跳转
        @version NIIEngine 3.0.0
        */
        virtual bool isSeekable() const;

        /** 用于输入码流中的数据解码
        @param[in] in 图片对象
        @param[out] out 输出协议(可以直接是文件名/数据通道),查看niiengine://协议
        @param[in] hint 如果这个编码组件含概多个编码,指定其一
        @version NIIEngine 3.0.0
        */
        virtual CodecResult encode(const Audio * in, const String & out, const String & hint) const;

        /** 用于输入码流中的数据解码,
        @param[in] in 图片对象
        @param[out] out 类似niiengine://协议的其中一个实现,数据流实现
        @param[in] hint 如果这个编码组件含概多个编码,指定其一(否则不使用)
        @version NIIEngine 3.0.0
        */
        virtual CodecResult encode(const Audio * in, DataStream *& out, const String & hint) const;

        /** 打开文件解码
        @param[in] in 输入协议(可以直接是文件名/数据通道),查看niiengine://协议
        @version NIIEngine 3.0.0
        */
        virtual CodecResult open(const String & in);

        /** 打开流解码
        @param[in] in 解码流
        @version NIIEngine 3.0.0
        */
        virtual CodecResult open(DataStream * in);

        /** 解码
        @version NIIEngine 3.0.0
        */
        virtual CodecResult decode(Audio * out, TimeDurMS time) const;

        /** 跳转
        @param[in] time
        @version NIIEngine 3.0.0
        */
        virtual CodecResult seek(TimeTotalMS time);

        /** 关闭解码
        @param[in] in 解码任务
        @version NIIEngine 3.0.0
        */
        virtual CodecResult close();
    };
}
#endif