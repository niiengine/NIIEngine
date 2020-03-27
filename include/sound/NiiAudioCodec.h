/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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