/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-7

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

#ifndef _NII_PIXEL_BLOCK_H_
#define _NII_PIXEL_BLOCK_H_

#include "NiiPreInclude.h"
#include "NiiPixelFormat.h"
#include "NiiBox.h"
namespace NII
{
    /** 像素块
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelBlock : public Box, public BufferAlloc
    {
    public:
        /** 像素过滤
        @version NIIEngine 3.0.0
        */
        enum Filter
        {
            F_Nearest,
            F_Linear,
            F_Bilinear
        };
    public:
        PixelBlock();
        PixelBlock(const Box & area, PixelFormat pf, Nui8 * data = 0);
        PixelBlock(NCount w, NCount h, NCount d, PixelFormat pf, Nui8 * data = 0);

        ~PixelBlock();

        /** 行偏量
        @version NIIEngine 3.0.0
        */
        NCount getRowOft() const;

        /** 片偏量
        @version NIIEngine 3.0.0
        */
        NCount getSliceOft() const;

        /** 是否是连续存储
        @version NIIEngine 3.0.0
        */
        bool isConsecutive() const;

        /** 获取连续存储时的大小
        @version NIIEngine 3.0.0
        */
        NCount getConsecutiveSize() const;

        /** 获取子区域
        @note 使用同一个数据区域,所以子区域使用完后不能删除数据指针
        @version NIIEngine 3.0.0
        */
        PixelBlock getSub(const Box & area) const;

        /** 设置颜色
        @version NIIEngine 3.0.0
        */
        void setColour(const Colour & out, NCount x, NCount y, NCount z);

        /** 获取颜色
        @version NIIEngine 3.0.0
        */
        void getColour(Colour & out, NCount x, NCount y, NCount z);

        /** 转换
        @note 像素格式依照src,尺寸依照dst,dst存放缩放后的缓存(覆盖原来的缓存)
        @param[in] src 源
        @param[in] dst 目标
        @version NIIEngine 3.0.0
        */
        static void scale(const PixelBlock & src, const PixelBlock & dst,
            Filter mode = F_Bilinear);

        /** 调整伽玛
        @param[in] size 数据大小
        @param[in] bpp 位深
        @version NIIEngine 3.0.0
        */
        static void applyGamma(Nui8 * data, NIIf gamma, NCount size, NCount bpp);
    public:
        Nui8 * mData;
        PixelFormat mFormat;
        NCount mRowPitch;
        NCount mSlicePitch;
    };
}
#endif