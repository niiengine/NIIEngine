/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-5-7

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

#ifndef _NII_AUDIO_H_
#define _NII_AUDIO_H_

#include "NiiPreInclude.h"
#include "NiiCodecList.h"
#include "NiiAudioFormat.h"

namespace NII
{
    /** 这是是音频的基本对象,描述的是一个音频文件的基本文件信息这个是一个音频素材类,所
        以不包含任何声音特效的描述.
    @remark
        由于音频流解压后会占用大量内存,所以这个里音频仅仅是描述一个读写器的功能.
    @par
        由于各种解码库基本都不能如你所愿的解出相应编码大小的缓存,也就是说只能选择解出
        "更多"缓存,而这部分缓存不是当前所需要的,但如果你还需要下一次解码中的缓存,这里
        的"更多"缓存是需要你把它存储下来的
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Audio : public AudioAlloc
    {
    public:
        Audio();
        Audio(const Audio & o);
        ~Audio();

        Audio & operator = (const Audio & audio);

        /** 加载
        @param[in] data 源数据
        @param[in] dur 音频时长
        @param[in] sf 音频格式
        @param[in] sr 音频样本频率
        @param[in] ch 音频通道数量
        @param[in] autoDelete 不再需要的时候,是否自动删除这个对象
        @version NIIEngine 3.0.0
        */
        Audio & load(Nui8 * data, TimeDurMS dur, SampleFormat sf, SampleRate sr, Nui8 ch,
            bool autoDelete = false);

        /** 加载
        @param[in] stream 源数据流
        @param[in] dur 音频时长
        @param[in] sf 音频格式
        @param[in] sr 音频频率
        @param[in] ch 音频通道数
        @version NIIEngine 3.0.0
        */
        Audio & load(DataStream * stream, TimeDurMS dur, SampleFormat sf, SampleRate sr, Nui8 ch);

        /** 加载
        @param[in] stream 数据流
        @param[in] dur 音频时长
        @param[in] hint 编码提示
        @version NIIEngine 3.0.0
        */
        Audio & load(DataStream * stream, TimeDurMS dur, const String & hint = StrUtil::BLANK);

        /** 加载
        @param[in] file 文件的名字
        @param[in] gid 资源组
        @param[in] dur 音频时长
        @version NIIEngine 3.0.0
        */
        Audio & load(const String & file, GroupID gid, TimeDurMS dur);

        /** 设置音频量化位宽
        @version NIIEngine 3.0.0
        */
        void setSampleFormat(SampleFormat br);

        /** 获取音频量化位宽
        @version NIIEngine 3.0.0
        */
        SampleFormat getSampleFormat();

        /** 设置样本频率
        @version NIIEngine 3.0.0
        */
        void setSampleRate(SampleRate sr);

        /** 获取样本频率
        @version NIIEngine 3.0.0
        */
        SampleRate getSampleRate();

        /** 保存到一个文件中
        @version NIIEngine 3.0.0
        */
        void save(const String & file, CodecID cid = N_AUDIO_NONE);

        /** 加编码音频和返回数据的一个流
        @param[in] ext 一个扩展,标识编码到的音频格式 ,如"ogg"或"mp3"
        @version NIIEngine 3.0.0
        */
        DataStream * encode(const String & ext);

        /** 获取数据流大小(单位:字节)
        @version NIIEngine 3.0.0
        */
        NCount getSize();

        /** 获取这段音频的长度(单位:秒)
        @version NIIEngine 3.0.0
        */
        TimeDurMS getLength();

        Nui8 * getData();

        /** 获取数据
        @version NIIEngine 3.0.0
        */
        const Nui8 * getData() const;
    protected:
        SampleFormat mFormat;   ///< 样本格式
        NCount mSize;           ///< 缓存的大小
        TimeDurMS mLength;      ///<
        Nui16 mSampleBits;      ///< 样本位宽(样本格式)
        Nui16 mSampleRate;      ///< 音频样本频率
        Nui8 mChannel;          ///< 通道数量
        Nui8 * mBuffer;         ///< 音频缓存
        bool mAutoDelete;
    };
    typedef vector<Audio *>::type AudioList;
}
#endif