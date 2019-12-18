/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-3-15

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_IMAGE_CODEC_H_
#define _NII_IMAGE_CODEC_H_

#include "NiiPreInclude.h"
#include "NiiCodec.h"

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