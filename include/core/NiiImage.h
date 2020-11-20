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

#ifndef _NII_IMAGE_H_
#define _NII_IMAGE_H_

#include "NiiPreInclude.h"
#include "NiiDataStream.h"
#include "NiiPixelFormat.h"
#include "NiiCodecList.h"

namespace NII
{
    /** 描述一个图片类
    @par
        通常,当你加载前的纹理做一些额外的操作或要分割现有纹理,则需要使用图像对象
        来加载这个纹理.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Image : public ImageAlloc
    {
        friend class ImageCodec;
    public:
        /** 图片特征
        @version NIIEngine 3.0.0
        */
        enum Mode
        {
            M_Compress  = 0x01,  ///< 压缩
            M_Animation = 0x10,  ///< 动画
            M_Cube      = 0x02,  ///< 立方
            M_HDR       = 0x04,  ///< 高光
            M_3D        = 0x08,  ///< 三维
        };
    public:
        Image();
        Image(const Image & o);
        ~Image();

        Image & operator = (const Image & o);

        /** 从内存中建立图像
        @param[in] data 数据
        @param[in] w 图像宽
        @param[in] h 图像高
        @param[in] pf 像素格式
        @param[in] autoDestroy 是否自动释放参数一中数据
        */
        Image & load(Nui8 * data, NCount w, NCount h, PixelFormat pf, bool autoDestroy = false);

        /** 从内存中建立图像
        @param[in] data 数据
        @param[in] w 图像宽
        @param[in] h 图像高
        @param[in] d 图像深度,一般为1
        @param[in] pf 像素格式
        @param[in] faces 面数量,一般为1(立方体映射为6)
        @param[in] mipmap LOD数量,一般为0(图片应该不包含lod结构的)
        @param[in] autoDestroy 是否自动释放参数一中数据
        @version NIIEngine 3.0.0
         */
        Image & load(Nui8 * data, NCount w, NCount h, NCount d, PixelFormat pf, NCount faces, NCount mipmap, 
            bool autoDestroy = false);

        /**  从数据流中建立图像
        @note 数据流已解码
        @param[in] data 数据
        @param[in] w 图像宽
        @param[in] h 图像高
        @param[in] pf 像素格
        @version NIIEngine 3.0.0
        */
        Image & load(DataStream * data, NCount w, NCount h, PixelFormat pf);

        /** 从数据流中建立图像
        @note 数据流已解码
        @param[in] w 图像宽
        @param[in] h 图像高
        @param[in] d 图像深度,一般为1
        @param[in] pf 像素格式
        @param[in] faces 面数量,一般为1(立方体映射为6)
        @param[in] mipmap LOD数量,一般为0(图片应该不包含lod结构的)
        @version NIIEngine 3.0.0
        */
        Image & load(DataStream * data, NCount w, NCount h, NCount d, PixelFormat pf, NCount faces, NCount mipmap);

        /** 加载图像文件.
        @note 文件未解码
        @param[in] file 文件名字/协议
        @param[in] gid 资源组的名字
        @version NIIEngine 3.0.0
        */
        Image & load(const String & file, GroupID gid);

        /** 加载图像数据流.
        @note 数据流未解码
        @param[in] data 数据流
        @param[in] hint 格式提示
        @version NIIEngine 3.0.0
        */
        Image & load(DataStream * data, const String & hint = StrUtil::WBLANK);

        /** 通过两文件合成图片
        @param[in] rgb RGB通道
        @param[in] a A通道
        @param[in] gid 加载纹理使用的资源组
        @param[in] pf 目的格式
        */
        Image & combine(const String & rgb, const String & a, PixelFormat pf = PF_BYTE_RGBA,
            GroupID gid = GroupUnknow);

        /** 通过两数据流合成图片
        @param[in] rgb RGB通道
        @param[in] a A通道
        @param[in] pf 目的格式
        @param[in] rgbhint 格式提示
        @param[in] ahint 格式提示
        */
        Image & combine(DataStream * rgb, DataStream * a, PixelFormat pf = PF_BYTE_RGBA,
            const String & rgbhint = StrUtil::WBLANK, const String & ahint = StrUtil::WBLANK);

        /**
        @param[in] rgb RGB通道
        @param[in] a A通道
        @param[in] format 目标格式
        */
        Image & combine(const Image & rgb, const Image & a, PixelFormat pf = PF_BYTE_RGBA);

        /** 保存图像到一个文件
        @note 文件名可能包含了后缀
        @version NIIEngine 3.0.0
        */
        void save(const String & filename, CodecID cid = N_IMAGE_NONE);

        /** 加编码图像和返回数据的一个流
        @param[in] ext 一个扩展标识
        @version NIIEngine 3.0.0
        */
        DataStream * encode(const String & ext) const;

        /** 获取图片色值
        @param[out] c
        @param[in] x 坐标
        @param[in] y 坐标
        @param[in] z 容积图片才有效
        @version NIIEngine 3.0.0
        */
        void getColour(Colour & out, NCount x, NCount y, NCount z) const;

        /** 设置图片色值
        @param[in] c
        @param[in] x 坐标
        @param[in] y 坐标
        @param[in] z 容积图片才有效
        @version NIIEngine 3.0.0
        */
        void setColour(const Colour & c, NCount x, NCount y, NCount z);

        /** 返回内部图像缓存的一个指针
        @remark 注意状态
        @version NIIEngine 3.0.0 adv
        */
        inline Nui8 * getData() const { return mBuffer; };

        /** 返回数据缓存的大小
        @version NIIEngine 3.0.0
        */
        inline NCount getSize() const { return mSize; }

        /** 获取像素图像的宽度
        @version NIIEngine 3.0.0
        */
        inline NCount getWidth() const { return mWidth; }

        /** 获取像素图像的高度
        @version NIIEngine 3.0.0
        */
        inline NCount getHeight() const { return mHeight; }

        /** 获取图像的色深
        @remark 这个由屏幕和设备器决定,图像没有主动权,图象本身期望由最完整状态呈现
        @version NIIEngine 3.0.0
        */
        inline NCount getDepth() const { return mDepth; }

        /** 返回的图像格式
        @version NIIEngine 3.0.0
        */
        inline PixelFormat getFormat() const { return mFormat; }

        /** 获取图像面数量
        @remark 普通图片基本为1, cubemap为6, gif为n
        @version NIIEngine 3.0.0
        */
        inline NCount getFaceCount() const{ return isMode(M_Cube) ? 6 : 1; }

        /** 获取包含在图像的mipmaps数量
        @version NIIEngine 3.0.0
        */
        inline NCount getMipmapCount() const { return mMipmapCount; }

        /** 像素单元大小
        @remark 单位字节
        @version NIIEngine 3.0.0
        */
        inline NCount getUnitSize() const { return mPixelSize; }

        /** 重新调整2D图像大小,采用适当的过滤
        @version NIIEngine 3.0.0
        */
        void resize(NCount w, NCount h, PixelBlock::Filter filter = PixelBlock::F_Bilinear);

        /** 是否存在透明成分
        @version NIIEngine 3.0.0
        */
        bool isAlpha() const;

        /** 是否存在特性
        @param[in] ext 特性
        @version NIIEngine 3.0.0
        */
        inline bool isMode(Mode ext) const { return (mFlags & ext) != 0; }

        /** 获取像素合
        @version NIIEngine 3.0.0
        */
        PixelBlock getPixel(NCount face = 0, NCount mipmap = 0) const;

        /** 水平镜像
        @version NIIEngine 3.0.0
        */
        Image & horizontalFlip();

        /** 垂直镜像
        @version NIIEngine 3.0.0
        */
        Image & verticalFlip();
    protected:
        NCount mWidth;
        NCount mHeight;
        NCount mDepth;
        NCount mSize;
        NCount mMipmapCount;
        PixelFormat mFormat;
        Nmark mFlags;
        Nui8 * mBuffer;
        Nui8 mPixelSize;
        bool mAutoDelete;
    };
    typedef vector<Image *>::type ImageList;
}
#endif