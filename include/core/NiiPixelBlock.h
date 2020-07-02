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
        inline NCount getRowOft() const{ return mRowPitch - getWidth(); }

        /** 片偏量
        @version NIIEngine 3.0.0
        */
        inline NCount getSliceOft() const{ return mSlicePitch - (getHeight() * mRowPitch); }

        /** 是否是连续存储
        @version NIIEngine 3.0.0
        */
        bool isConsecutive() const;

        /** 获取连续存储时的大小
        @version NIIEngine 3.0.0
        */
        NCount getConsecutiveSize() const;

        /** 获取头部指针
        @version NIIEngine 3.0.0
        */
        Nui8 * getTopLeftFrontPixelPtr() const;

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

        /** 垂直翻转像素
        @version NIIEngine 4.0.0
        */
        void verticalFlip();

        /** 转换
        @note 像素格式依照src,尺寸依照dst,dst存放缩放后的缓存(覆盖原来的缓存)
        @param[in] src 源
        @param[in] dst 目标
        @version NIIEngine 3.0.0
        */
        static void scale(const PixelBlock & src, const PixelBlock & dst, Filter mode = F_Bilinear);

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