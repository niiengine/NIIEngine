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

#ifndef _NII_VOICE_FORMAT_H_
#define _NII_VOICE_FORMAT_H_

#include "NiiPreInclude.h"

namespace NII
{
    // Mono8, Mono16ADPCM, Mono16, Stereo8, Stereo16
    /** 样本格式
    @version NIIEngine 3.0.0
    */
    enum SampleFormat
    {
        SF_Unknow       = 0,
        SF_U8           = 1,    ///< 无符号8位
        SF_S16          = 2,    ///< 有符号16位
        SF_S16L         = 15,   ///< 有符号16位小端
        SF_S16B         = 16,   ///< 有符号16位大端
        SF_S16P         = 8,    ///< planar有符号16位
        SF_S32          = 3,    ///< 有符号32位
        SF_S32L         = 17,   ///< 有符号32位小端
        SF_S32B         = 18,   ///< 有符号32位大端
        SF_S32P         = 9,    ///< planar有符号32位
        SF_S24W3        = 4,    ///< 有符号24位(占用3字节)
        SF_S24W3L       = 19,   ///< 有符号24位小端(占用3字节)
        SF_S24W3B       = 20,   ///< 有符号24位大端(占用3字节)
        SF_S24W3P       = 10,   ///< planar有符号24位
        SF_S24W4        = 5,    ///< 有符号24位(占用4字节)
        SF_S24W4L       = 21,   ///< 有符号24位小端(占用4字节)
        SF_S24W4B       = 22,   ///< 有符号24位大端(占用4字节)
        SF_S24W4P       = 11,   ///< planar有符号24位
        SF_S24W4MSB     = 23,   ///< 左对齐有符号24位
        SF_S24W4MSBP    = 12,   ///< planar左对齐有符号24位
        SF_Float        = 6,    ///< 单精浮点
        SF_FloatP       = 13,   ///< planar单精浮点
        SF_Double       = 7,    ///< 双精浮点
        SF_DoubleP      = 14,   ///< planar双精浮点

        SF_AAC          = 24,   ///< 流保真格式
        SF_AC3          = 25,   ///< 流保真格式
        SF_DTS          = 26,   ///< 流保真格式
        SF_EAC3         = 27,   ///< 流保真格式
        SF_TRUEHD       = 28,   ///< 流保真格式
        SF_DTSHD        = 29,   ///< 流保真格式
        SF_LPCM         = 30,   ///< 流保真格式
        SF_Count        = 31
    };
    typedef vector<SampleFormat>::type SampleFormatList;
    
    #define N_AUDIO_NEndian(x) ((x) >= SF_U8 && (x) <= SF_Double)
    #define N_AUDIO_RAW(x) ((x) >= SF_AAC && (x) <= SF_LPCM)
    #define N_AUDIO_RAW_HD(x) ((x) >= SF_EAC3 && (x) <= SF_LPCM)
    #define N_AUDIO_PLANAR(x) ((x) >= SF_S16P && (x) <= SF_DoubleP)

    /** 样本频率
    @version NIIEngine 3.0.0
    */
    enum SampleRate
    {
        SR_Unknow   = 0,
        SR_5512Hz   = 5512,     ///< 每秒5512个振动
        SR_8000Hz   = 8000,     ///< 每秒8000个振动
        SR_11025Hz  = 11025,    ///< 每秒11025个振动
        SR_12000Hz  = 12000,    ///< 每秒12000个振动
        SR_16000Hz  = 16000,    ///< 每秒16000个振动
        SR_22050Hz  = 22050,    ///< 每秒22050个振动
        SR_24000Hz  = 24000,    ///< 每秒24000个振动
        SR_32000Hz  = 32000,    ///< 每秒32000个振动
        SR_44100Hz  = 44100,    ///< 每秒44100个振动
        SR_48000Hz  = 48000,    ///< 每秒48000个振动
        SR_64000Hz  = 64000,    ///< 每秒64000个振动
        SR_88200Hz  = 88200,    ///< 每秒88200个振动
        SR_96000Hz  = 96000,    ///< 每秒96000个振动
        SR_176400Hz = 176400,   ///< 每秒176400个振动
        SR_192000Hz = 192000,   ///< 每秒192000个振动
        SR_384000Hz = 384000,   ///< 每秒384000个振动
    };
    typedef vector<NCount>::type AudioSampleRateList;
    
    /** 音频通道类型
    @version NIIEngine 3.0.0
    */
    enum AudioChannel
    {
        AE_CH_None              = 0,
        AE_CH_Unknow            = 1,
        AE_CH_RAW               = 2,

        AC_Front_Left           = 3,
        AC_Front_Right          = 4,
        AC_Front_Center         = 5,
        AC_Low_Frequency        = 6,
        AC_Back_Left            = 7,
        AC_Back_Right           = 8,
        AC_Front_Left_Of_Center = 9,
        AC_Front_Right_Of_Center= 10,
        AC_Back_Left_Of_Center  = 11,
        AC_Back_Right_Of_Center = 12,
        AC_Back_Center          = 13,
        AC_Side_Left            = 14,
        AC_Side_Right           = 15,
        AC_Top_Center           = 16,
        AC_Top_Front_Left       = 17,
        AC_Top_Front_Center     = 18,
        AC_Top_Front_Right      = 19,
        AC_Top_Back_Left        = 20,
        AC_Top_Back_Center      = 21,
        AC_Top_Back_Right       = 22,
        AC_Count                = 23
    };
    typedef vector<AudioChannel>::type AudioChannelList;

    /** 声音段,描述音乐的组成段
    @remark 描述声音流信息
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SoundFormat
    {
    public:
        SoundFormat();
        ~SoundFormat();
    public:
        //AudioChanneInfo mChannelLayout; ///< 通道排列
        SampleFormat mSampleFormat;     ///< 数据格式
        NCount mSampleRate;             ///< 样本频率
        NCount mEncodeRate;             ///< 流样本频率
    };

    /** 一些实用的方法
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AudioUtil
    {
    public:
        typedef void (*convert)(Nui8 * in, Nui8 * out, NCount count);
        typedef void (*convert2)(Nui8 * in, NIIf * out, NCount count);
        typedef void (*convert3)(NIIf * in, Nui8 * out, NCount count);
    public:
        /** 返回相应位宽枚举的值
        @param[in] sf 样本格式枚举
        @version NIIEngine 3.0.0
        */
        static NCount toByte(SampleFormat sf);

        /** 返回相应样本频率的值
        @param[in] sr 样本频率枚举
        @version NIIEngine 3.0.0
        */
        static NCount toNum(SampleRate sr);

        /** 返回相应值的样本频率枚举
        @version NIIEngine 3.0.0
        */
        static SampleRate toSampleRate(Nui32 num);
        
        /// 计算一个音频块1秒钟的大小(单位字节)
        static NCount calcSize(Nui32 bits, Nui32 sr, Nui8 ch);

        /// 计算一个音频单元1秒钟的大小(单位字节)[比特率]
        static NCount calcSize(SampleFormat sf, SampleRate sr, Nui8 ch);

        /// 计算一个音频单元给定时间的大小
        static NCount calcSize(TimeDurMS dur, SampleFormat sf, SampleRate sr, Nui8 ch);

        /// 计算一个音频单元单位时间的大小
        static NCount calcSize(SampleFormat sf, Nui8 ch);

        /** 音频数据格式转换函数
        @param[in] sf 目标格式
        @return 转换函数
        @version NIIEngine 3.0.0
        */
        convert Nui8toNui8(SampleFormat sf);

        /** 音频数据格式转换函数
        @param[in] sf 目标格式
        @return 转换函数
        @version NIIEngine 3.0.0
        */
        convert Nui8toNIIf(SampleFormat sf);

        /** 音频数据格式转换
        @param[in] sf
        @return 转换函数
        @version NIIEngine 3.0.0
        */
        convert NIIftoNui8(SampleFormat sf);

        /** Nui8 到 Ni16 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Nui8toNi16(Nui8 * in, Nui8 * out, NCount count);

        /** Nui8 到 Ni24 大端部分的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Nui8toNi24be(Nui8 * in, Nui8 * out, NCount count);

        /** Nui8 到 Ni24 小端部分的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Nui8toNi24le(Nui8 * in, Nui8 * out, NCount count);

        /** Nui8 到 Ni32 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Nui8toNi32(Nui8 * in, Nui8 * out, NCount count);

        /** Nui8 到 NIIf 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Nui8toNIIf(Nui8 * in, Nui8 * out, NCount count);

        /** Nui8 到 NIId 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Nui8toNIId(Nui8 * in, Nui8 * out, NCount count);

        /** Ni16 到 Nui8 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni16toNui8(Nui8 * in, Nui8 * out, NCount count);

        /** Ni16 到 Ni24 大端部分的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni16toNi24be(Nui8 * in, Nui8 * out, NCount count);

        /** Ni16 到 Ni24 小端部分的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni16toNi24le(Nui8 * in, Nui8 * out, NCount count);

        /** Ni16 到 Ni32 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni16toNi32(Nui8 * in, Nui8 * out, NCount count);

        /** Ni16 到 NIIf 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni16toNIIf(Nui8 * in, Nui8 * out, NCount count);

        /** Ni16 到 NIId 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni16toNIId(Nui8 * in, Nui8 * out, NCount count);

        /** Ni24 到 Nui8be 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni24toNui8be(Nui8 * in, Nui8 * out, NCount count);

        /** Ni24 到 Nui8le 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni24toNui8le(Nui8 * in, Nui8 * out, NCount count);

        /** Ni24 到 Ni16be 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni24toNi16be(Nui8 * in, Nui8 * out, NCount count);

        /** Ni24 到 Ni16le 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni24toNi16le(Nui8 * in, Nui8 * out, NCount count);

        /** Ni24 到 Ni24 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni24toNi24(Nui8 * in, Nui8 * out, NCount count);

        /** Ni24 到 Ni32be 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni24toNi32be(Nui8 * in, Nui8 * out, NCount count);

        /** Ni24 到 Ni32le 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni24toNi32le(Nui8 * in, Nui8 * out, NCount count);

        /** Ni24 到 NIIfbe 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni24toNIIfbe(Nui8 * in, Nui8 * out, NCount count);

        /** Ni24 到 NIIfle 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni24toNIIfle(Nui8 * out, Nui8 * in, NCount count);

        /** Ni24 到 NIIdbe 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni24toNIIdbe(Nui8 * in, Nui8 * out, NCount count);

        /** Ni24 到 NIIdle 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni24toNIIdle(Nui8 * in, Nui8 * out, NCount count);

        /** Ni32 到 Nui8 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni32toNui8(Nui8 * in, Nui8 * out, NCount count);

        /** Ni32 到 Ni16 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni32toNi16(Nui8 * in, Nui8 * out, NCount count);

        /** Ni32 到 Ni24be 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni32toNi24be(Nui8 * in, Nui8 * out, NCount count);

        /** Ni32 到 Ni24le 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni32toNi24le(Nui8 * in, Nui8 * out, NCount count);

        /** Ni32 到 NIIf 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni32toNIIf(Nui8 * in, Nui8 * out, NCount count);

        /** Ni32 到 NIId 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void Ni32toNIId(Nui8 * in, Nui8 * out, NCount count);

        /** NIIf 到 Nui8 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void NIIftoNui8(Nui8 * in, Nui8 * out, NCount count);

        /** NIIf 到 Ni16 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void NIIftoNi16(Nui8 * in, Nui8 * out, NCount count);

        /** NIIf 到 Ni24be 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void NIIftoNi24be(Nui8 * in, Nui8 * out, NCount count);

        /** NIIf 到 Ni24le 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void NIIftoNi24le(Nui8 * in, Nui8 * out, NCount count);

        /** NIIf 到 Ni32 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void NIIftoNi32(Nui8 * in, Nui8 * out, NCount count);

        /** NIIf 到 NIId 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void NIIftoNIId(Nui8 * in, Nui8 * out, NCount count);

        /** NIId 到 Nui8 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void NIIdtoNui8(Nui8 * in, Nui8 * out, NCount count);

        /** NIId 到 Ni16 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void NIIdtoNi16(Nui8 * in, Nui8 * out, NCount count);

        /** NIId 到 Ni24be 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void NIIdtoNi24be(Nui8 * in, Nui8 * out, NCount count);

        /** NIId 到 Ni24le 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void NIIdtoNi24le(Nui8 * in, Nui8 * out, NCount count);

        /** NIId 到 Ni32 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void NIIdtoNi32(Nui8 * in, Nui8 * out, NCount count);

        /** NIId 到 NIIf 的转换
        @param[in] in 输入
        @param[out] out 输出
        @param[in] count 数据单位数量
        @version NIIEngine 3.0.0
        */
        static void NIIdtoNIIf(Nui8 * in, Nui8 * out, NCount count);
    };
}
#endif