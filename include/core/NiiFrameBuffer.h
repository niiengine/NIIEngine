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

#ifndef _NII_FRAME_BUFFER_H_
#define _NII_FRAME_BUFFER_H_

#include "NiiPreInclude.h"
#include "NiiBuffer.h"
#include "NiiPixelUtil.h"
#include "NiiPixelBlock.h"

namespace NII
{
    /** 帧缓存
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameBuffer : public Buffer
    {
		friend class TextureFrame;
    public:
        FrameBuffer(BufferManager * mag, NCount w, NCount h, NCount d, PixelFormat pf, Nmark mode);
        ~FrameBuffer();

        using Buffer::lock;

        /** 锁定的缓寸(可)读/写.
        @param[in] area 锁定缓存的区域
        @param[in] m 锁定选项
        @return PixelBlock 包含锁定区域,节距和像素格式
        @version NIIEngine 3.0.0
        */
        virtual const PixelBlock & lock(const Box & area, Nmark mm);

        /** 获取当前锁定的像素块
        @version NIIEngine 3.0.0
        */
        const PixelBlock & getMutexData();

        /** 从另外帧缓存复制
        @version NIIEngine 3.0.0
        */
        void write(const FrameBuffer * src);

        /** 从另外帧缓存中复制某个区域到这个帧缓存的某个区域
        @note 自动完成区域缩放
        @version NIIEngine 3.0.0
         */
        virtual void write(const FrameBuffer * target, const Box & src, const Box & dst);

        /** 写入像素缓存块
        @param[in] target 目标区域
        @version NIIEngine 3.0.0
        */
        virtual void write(const PixelBlock & src, const Box & target) = 0;

        /** 读取像素缓存块
        @param[in] target 包含在内存的目的像素和格式
        @version NIIEngine 3.0.0
        */
        virtual void read(const Box & src, PixelBlock & target) = 0;

        /** 获取可供使用的纹理帧
        @version NIIEngine 3.0.0
        */
        virtual TextureFrame * getBuffer(Nidx index = 0);

        /** 获取这个缓存的宽度
        @version NIIEngine 3.0.0
        */
        NCount getWidth() const;

        /** 获取这个缓存的高度
        @version NIIEngine 3.0.0
        */
        NCount getHeight() const;

        /** 获取这个缓存的深度
        @version NIIEngine 3.0.0
        */
        NCount getDepth() const;

        /** 获取这个缓存的原像素格式
        @version NIIEngine 3.0.0
        */
        PixelFormat getFormat() const;

        /// @copydetails Buffer::lock
        virtual void * lock(Nmark m, NCount oft, NCount size);

        /// @ copydetails Buffer::read
        virtual void read(void * dest, NCount oft, NCount size);

        /// @ copydetails Buffer::write
        virtual void write(const void * src, NCount oft, NCount size);
    protected:
        /** 锁定缓存
        @version NIIEngine 3.0.0
        */
        virtual PixelBlock lockImpl(Nmark m, const Box & src) = 0;

        /** 清理缓存
        @version NIIEngine 3.0.0
        */
        virtual void destroyImpl(TextureFrame * obj, Nidx index);

        /// @copydetails Buffer::lockImpl
        virtual void * lockImpl(Nmark m, NCount oft, NCount size);
    protected:
        NCount mWidth;
        NCount mHeight;
        NCount mDepth;
        NCount mRowPitch;
        NCount mSlicePitch;
        PixelFormat mFormat;
        Box mMutexArea;
        PixelBlock mMutexData;
    };
}
#endif