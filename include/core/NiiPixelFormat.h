/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-1

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

#ifndef _NII_PIXEL_FORMAT_H_
#define _NII_PIXEL_FORMAT_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 像素格式
    @version NIIEngine 3.0.0
    */
    enum PixelFormat
    {
        PF_UNKNOWN = 0,                 ///< 不明格式
        PF_L8 = 1,                      ///< 8位亮度
        PF_L16 = 2,                     ///< 16位亮度
        PF_A8 = 3,                      ///< 8位透明度.
        PF_A4L4 = 4,                    ///< 4位透明度/4位亮度.
        PF_L8A8 = 5,                    ///< 8位亮度/8位透明度
        PF_R8 = 6,                      ///< 8位红
        PF_RG8 = 7,                     ///< 8位红/绿
        PF_SHORT_RGBA = 8,              ///< 16位红/绿/蓝/透明度
        PF_SHORT_GR = 9,                ///< 16位绿/红
        PF_SHORT_RGB = 10,              ///< 16位红/绿/蓝
        PF_R5G6B5 = 11,                 ///< 5位红/6位绿/5位蓝
        PF_B5G6R5 = 12,                 ///< 5位红/6位绿/5位蓝
        PF_R3G3B2 = 13,                 ///< 2位蓝/3位绿/3位红
        PF_A4R4G4B4 = 14,               ///< 4位透明度/红/绿/蓝
        PF_A1R5G5B5 = 15,               ///< 5位蓝/绿/红/1位透明度
        PF_R8G8B8 = 16,                 ///< 8位红/绿/蓝
        PF_B8G8R8 = 17,                 ///< 8位蓝/绿/红
        PF_A8R8G8B8 = 18,               ///< 8位透明度/红/绿/蓝
        PF_A8B8G8R8 = 19,               ///< 8位蓝/绿/红/透明度
        PF_B8G8R8A8 = 20,               ///< 8位蓝/绿/红/透明度
        PF_R8G8B8A8 = 21,               ///< 8位红/绿/蓝/透明度
        PF_X8R8G8B8 = 22,               ///< 8位红/绿/蓝,无透明度
        PF_X8B8G8R8 = 23,               ///< 8位绿/蓝/红,无透明度
#if N_ENDIAN == N_ENDIAN_B
        PF_BYTE_RGB = PF_R8G8B8,        ///< 1字节红/绿/蓝
        PF_BYTE_BGR = PF_B8G8R8,        ///< 1字节蓝/绿/红
        PF_BYTE_BGRA = PF_B8G8R8A8,     ///< 1字节蓝/红/绿/透明度
        PF_BYTE_RGBA = PF_R8G8B8A8,     ///< 1字节红/绿/蓝/透明度
#else
        PF_BYTE_RGB = PF_B8G8R8,        ///< 1字节红/绿/蓝
        PF_BYTE_BGR = PF_R8G8B8,        ///< 1字节红/绿/蓝
        PF_BYTE_BGRA = PF_A8R8G8B8,     ///< 1字节蓝/绿/红/透明度
        PF_BYTE_RGBA = PF_A8B8G8R8,     ///< 1字节红/绿/蓝/透明度
#endif
        PF_A2R10G10B10 = 24,            ///< 10位红/绿/蓝/2位透明度
        PF_A2B10G10R10 = 25,            ///< 10位蓝/绿/红/2位透明度
        PF_FLOAT16_R = 26,              ///< 16位红
        PF_FLOAT16_RGB = 27,            ///< 16位红/绿/蓝
        PF_FLOAT16_RGBA = 28,           ///< 16位红/绿/蓝/透明度
        PF_FLOAT32_R = 29,              ///< 32位红
        PF_FLOAT32_RGB = 30,            ///< 32位红/绿/蓝
        PF_FLOAT32_RGBA = 31,           ///< 32位红/绿/蓝/透明度
        PF_FLOAT16_GR = 32,             ///< 16位绿/红
        PF_FLOAT32_GR = 33,             ///< 32位绿/红
        PF_DEPTH = 34,                  ///< 深度形式
        PF_DXT1 = 35,                   ///< DirectDraw Surface格式
        PF_DXT2 = 36,                   ///< DirectDraw Surface格式
        PF_DXT3 = 37,                   ///< DirectDraw Surface格式
        PF_DXT4 = 38,                   ///< DirectDraw Surface格式
        PF_DXT5 = 39,                   ///< DirectDraw Surface格式
        PF_PVRTC_RGB2 = 40,             ///< PowerVR 格式
        PF_PVRTC_RGBA2 = 41,            ///< PowerVR 格式
        PF_PVRTC_RGB4 = 42,             ///< PowerVR 格式
        PF_PVRTC_RGBA4 = 43,            ///< PowerVR 格式
        PF_COUNT = 44
    };
    typedef vector<PixelFormat>::type PixelFormatList;

    /** 像素基元数据格式
    @version NIIEngine 3.0.0
    */
    enum PixelDataType
    {
        PDT_Unknow  = 0,
        PDT_Byte    = 0,
        PDT_Short   = 1,
        PDT_UShort  = 2,
        PDT_Int     = 3,
        PDT_UInt    = 4,
        PDT_Float16 = 5,
        PDT_Float   = 6,
        PDT_Count   = 7
    };

    /** 像素格式特点
    @version NIIEngine 3.0.0
    */
    enum PixelFormatFeature
    {
        PFF_None = 0,
        PFF_Float = 0x01,
        PFF_Alpha = 0x02,
        PFF_Depth = 0x04,
        PFF_Compress = 0x08,
        PFF_Grayscale = 0x20,     ///< 灰阶格式,没有颜色,只代表黑到白的过渡
        PFF_NonEndian = 0x10      ///< 一般8位基元的都是非字节序相关数据
    };

    /** 像素数据实用功能
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelUtil
    {
    public:
        /** 转换格式
        @param[in] src 指向资源区域
        @param[in] srcpf 资源区域的像素格式
        @param[in] dst 指向目的区域
        @param[in] destpf 目的区域的像素格式
        @version NIIEngine 3.0.0
        */
        static void conv(Nui8 * src, PixelFormat srcpf, Nui8 * dst, PixelFormat destpf, NCount count);

        /** 转换格式
        @remark 资源和目的盒必须相同的尺寸. 如果资源和目标格式匹配,将简单复制
        @param[in] src PixelBlock包含资源像素,节距和格式
        @param[in] dst PixelBlock包含目的像素,节距和格式
        @version NIIEngine 3.0.0
        */
        static void conv(const PixelBlock & src, const PixelBlock & dst);

        /** 获取内存大小
        @note 不是压缩
        @param[in] pf 该范围的格式
        @return 字节大小
        @version NIIEngine 3.0.0
        */
        static NCount getMemorySize(NCount w, NCount h, NCount d, PixelFormat pf);

        /** 获取像素格式特征标记
        @return 如PFF_Alpha, PFF_ISCOMPRESSED,PFF_Float, PFF_Depth, PFF_Grayscale
            组合特征
        */
        static Nui32 getExentMark(PixelFormat pf);

        /** 获取格式中是否有alpha成份
        @version NIIEngine 3.0.0
        */
        static bool isAlpha(PixelFormat pf);

        /** 获取格式是否浮点
        @version NIIEngine 3.0.0
        */
        static bool isFloat(PixelFormat pf);

        /** 获取格式是否压缩
        @version NIIEngine 3.0.0
        */
        static bool isCompress(PixelFormat pf);

        /** 获取格式是否是深度特征
        @version NIIEngine 3.0.0
        */
        static bool isDepth(PixelFormat pf);

        /** 获取格式使用本机字节顺序
        @version NIIEngine 3.0.0
        */
        static bool isNativeEndian(PixelFormat pf);

        /** 获取格式是否是亮度特征.
        @version NIIEngine 3.0.0
        */
        static bool isGrayscale(PixelFormat pf);

        /** 获取(RGBA)位深
        @remark 对于非彩色格式(dxt, depth)这个返回[0,0,0,0]
        @version NIIEngine 3.0.0
        */
        static void getBitDepth(PixelFormat pf, NIIi rgba[4]);

        /** 获取(RGBA)掩码
        @note 仅本地字节序有效
        @version NIIEngine 3.0.0
        */
        static void getBitMask(PixelFormat pf, Nui32 rgba[4]);

        /** 获取(RGBA)位移
        @note 仅本地字节序有效
        @version NIIEngine 3.0.0
        */
        static void getBitShift(PixelFormat pf, Nui8 rgba[4]);

        /** 获取元素大小(字节)
        @remark 输入PF_UNKNOWN结果将返回0字节大小
        @version NIIEngine 3.0.0
        */
        static NCount getUnitSize(PixelFormat pf);

        /** 获取图象内部标识
        @version NIIEngine 3.0.0
        */
        static String getMarkName(PixelFormat srcpf);

        /** 是否直接操作格式
        @return 是则返回真,否则返回假
        @version NIIEngine 3.0.0
        */
        static bool isDirect(PixelFormat srcpf);

        /** 获取单元中数据格式
        @version NIIEngine 3.0.0
        */
        static PixelDataType getDataType(PixelFormat pf);

        /** 获取像素数据成分数量
        @return返回 3(无透明度)或4(有透明度)
        @version NIIEngine 3.0.0
        */
        static NCount getDataCount(PixelFormat pf);

        /** 给定名字,获取格式
        @param[in] markname 格式名字
        @param[in] direct 直接可以使用的格式
        @param[in] caseSensitive 格式名字是否区分大小写
        @version NIIEngine 3.0.0
        */
        static PixelFormat getFormat(const String & markname, bool direct = false, bool caseSensitive = false);

        /** 返回相似图像格式
        @return位深依照首选位深
        @param[in] pf 原格式.
        @param[in] intBits 整型像素格首选位深(像素位).0,16,32.
        @param[in] floatBits NIIf 浮点像素首选位深(像素位).0,16,32.
        @version NIIEngine 3.0.0
        */
        static PixelFormat getFormat(PixelFormat pf, Nui16 intBits, Nui16 floatBits);

        /** 打包一个颜色值到内存
        @param[in] c 颜色
        @param[in] pf 目的像素格式
        @param[in] dest 目的内存位置
        @version NIIEngine 3.0.0
        */
        static void packColour(const Colour & c, PixelFormat dstpf, Nui8 * dst);

        /** 转换颜色值到内存
        @param[in] r,g,b,a 颜色成分
        @param[in] pf 目的像素格式
        @param[in] dest 目的内存位置
        @version NIIEngine 3.0.0
        */
        static void packColour(Nui8 r, Nui8 g, Nui8 b, Nui8 a, PixelFormat dstpf, Nui8 * dst);

        /** 转换颜色值到内存
       @param[in] r,g,b,a 颜色成分
       @param[in] pf 目的像素格式
       @param[in] dest 目的内存位置
       @version NIIEngine 3.0.0
       */
        static void packColour(NIIf r, NIIf g, NIIf b, NIIf a, PixelFormat dstpf, Nui8 * dst);

        /** 从内存中解包一个颜色值
        @param[in] colour 返回的颜色
        @param[in] pf 需读取的颜色的像素格式
        @param[in] src 资源内存位置
        @version NIIEngine 3.0.0
        */
        static void unpackColour(Colour * colour, PixelFormat srcpf, const Nui8 * src);

        /** 从内存中解包一个颜色值
        @param[in] r,g,b,a 返回的颜色 (为 byte)
        @param[in] pf 资源的像素格式
        @param[in] src 资源内存位置
        @remark 8位精度的颜色成分
        @version NIIEngine 3.0.0
        */
        static void unpackColour(Nui8 & r, Nui8 & g, Nui8 & b, Nui8 & a, PixelFormat srcpf, const Nui8 * src);

        /** 从内存中解包一个颜色值
        @param[in] r,g,b,a 返回的颜色 (为 NIIf)
        @param[in] pf 资源的像素格式
        @param[in] src 资源内存位置
        @version NIIEngine 3.0.0
        */
        static void unpackColour(NIIf & r, NIIf & g, NIIf & b, NIIf & a, PixelFormat pf, const Nui8 * src);
    };
}

#endif