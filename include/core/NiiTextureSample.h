/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-7-7

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

#ifndef _NII_TEXTURESample_H_
#define _NII_TEXTURESample_H_

#include "NiiPreInclude.h"
#include "NiiShaderCh.h"

namespace NII
{
    /** 纹理寻址模式
    @version NIIEngine 3.0.0
    */
    enum TextureAddressingMode
    {
        TAM_REPEAT = 0,                 ///< 超出1.0,则返回(1.0 - x)
        TAM_MIRROR = 1,                 ///< 超出1.0,则镜面重复
        TAM_CLAMP = 2,                  ///< 限制在[0.0-1.0]里
        TAM_BORDER = 3,                 ///< 超出0.0-1.0范围,设置为边缘颜色
        TAM_Count = 4                   ///< 尽量使用现有的,因为被底层API约束,否则只能从图片本身入手
    };

    /** 纹理地址模式,用于每个纹理坐标
    @version NIIEngine 3.0.0
    */
    struct TextureAddressing
    {
        TextureAddressingMode mS_U;     ///< x方向
        TextureAddressingMode mT_V;     ///< y方向
        TextureAddressingMode mR_W;     ///< w方向
    };

    /** 纹理过滤类型
    @version NIIEngine 3.0.0
    */
    enum TextureFilterType
    {
        TFT_MIN = 0,                    ///< 收缩时
        TFT_MAG = 1,                    ///< 放大时
        TFT_MIP = 2,                    ///< mipmap时
        TFT_Count = 3                   ///< 尽量使用现有的,因为被渲染系统约束,否则只能从图片本身入手
    };

    /** 纹理过滤操作
    @version NIIEngine 3.0.0
    */
    enum TextureFilterOP
    {
        TFOP_NONE = 0,                  ///< 不过滤
        TFOP_POINT = 1,                 ///< 位置最接近像素
        TFOP_LINEAR = 2,                ///< 线形取值
        TFOP_ANISOTROPIC = 3,           ///< 线形取值 + 补偿角度
        TFOP_Count = 4                  ///< 尽量使用现有的,因为被渲染系统约束,否则只能从图片本身入手
    };

    /** 纹理过滤模式
    @remark 纹理过滤预组合
    @version NIIEngine 3.0.0
    */
    enum TextureFilterMode
    {
        TFM_BILINEAR = 0,               ///< LINEAR, LINEAR, POINT
        TFM_TRILINEAR = 1,              ///< LINEAR, LINEAR, LINEAR
        TFM_ANISOTROPIC = 2,            ///< ANISOTROPIC, ANISOTROPIC, LINEAR
        TFM_NONE = 3,                   ///< POINT, POINT, NONE (小型游戏机用)
        TFM_Count = 4                   ///< 尽量使用现有的,因为被底层API约束,否则只能从图片本身入手
    };

    /** 纹理样本模式
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TextureSample : public ShaderAlloc
    {
        friend ShaderChTextureUnit;
    public:
        TextureSample();
        TextureSample(TextureFilterOP min, TextureFilterOP mag, TextureFilterOP mip,
            TextureAddressingMode u, TextureAddressingMode v, TextureAddressingMode w);
        virtual ~TextureSample();

        TextureSample & operator =(const TextureSample & o);

        /** 设置寻址模式
        @note 默认是 TAM_REPEAT,在可编程管线或固定管线中有效
        @version NIIEngine 3.0.0
        */
        inline void setMode(const TextureAddressing & uvw) { mAddressMode = uvw; }

        /** 设置寻址模式
        @note 默认是 TAM_REPEAT,在可编程管线或固定管线中有效
        @version NIIEngine 3.0.0
        */
        void setMode(TextureAddressingMode u, TextureAddressingMode v, TextureAddressingMode w);

        /** 获取寻址模式
        @note 默认是 TAM_REPEAT,在可编程管线或固定管线中有效
        @version NIIEngine 3.0.0
        */
        inline const TextureAddressing & getMode() const { return mAddressMode; }

        /** 设置过滤模式
        @version NIIEngine 3.0.0
        */
        void setFiltering(TextureFilterMode type);

        /** 设置过滤模式
        @version NIIEngine 3.0.0
        */
        void setFiltering(TextureFilterType type, TextureFilterOP op);

        /** 设置过滤模式
        @version NIIEngine 3.0.0
        */
        void setFiltering(TextureFilterOP minop, TextureFilterOP magop, TextureFilterOP mipop);

        /** 获取过滤模式
        @version NIIEngine 3.0.0
        */
        TextureFilterOP getFiltering(TextureFilterType type) const;

        /** 获取过滤模式
        @version NIIEngine 3.0.0
        */
        inline TextureFilterOP getMinFiltering() const { return mMinOP; }

        /** 获取过滤模式
        @version NIIEngine 3.0.0
        */
        inline TextureFilterOP getMagFiltering() const { return mMagOP; }

        /** 获取过滤模式
        @version NIIEngine 3.0.0
        */
        inline TextureFilterOP getMipFiltering() const { return mMipOP; }

        /** 设置抗锯齿等级
        @param[in] max 默认为1
        @note 这个渲染可以应用在固定管线和可编程管线中
        @version NIIEngine 3.0.0
        */
        inline void setAnisotropy(Nui32 aniso) { mMaxAniso = aniso; }

        /** 设置抗锯齿等级
        @param[in] max 默认为1
        @note 这个渲染可以应用在固定管线和可编程管线中
        @version NIIEngine 3.0.0
        */
        inline Nui32 getAnisotropy() const { return mMaxAniso; }

        /** 设置应用到mipmap计算的偏量值
        @remark Mipmap 就是纹理的LOD概念
        @param[in] bias fn(x) = mip-; fn(-x) = mip+;
        @version NIIEngine 
        */
        inline void setMipmapBias(NIIf bias) { mMipmapBias = bias; }

        /** 获取应用到mipmap计算的偏量值
        @remark Mipmap 就是纹理的LOD概念
        @param[in] bias fn(x) = mip-; fn(-x) = mip+;
        @version NIIEngien 3.0.0
        */
        inline NIIf getMipmapBias() const { return mMipmapBias; }

        /** 设置边缘颜色
        @remark TAM_BORDER模式特有
        @version NIIEngien 3.0.0
        */
        inline void setBorder(const Colour& colour) { mBorder = colour; }

        /** 获取边缘颜色
        @remark TAM_BORDER模式特有
        @version NIIEngien 3.0.0
        */
        inline const Colour& getBorder() const { return mBorder; }

        /** 设置比较模式
        @version NIIEngien 3.0.0
         */
        inline void setCompare(bool enabled) { mCompare = enabled; }

        /** 是否比较模式
        @version NIIEngien 3.0.0
        */
        inline bool isCompare() const { return mCompare; }

        /** 设置比较函数
        @version NIIEngine 3.0.0
        */
        inline void setCompareFunc(CmpMode function) { mCompareFunc = function; }

        /** 获取比较函数
        @version NIIEngien 3.0.0
        */
        inline CmpMode getCompareFunc() const { return mCompareFunc; }
    protected:
        TextureFilterOP mMinOP;
        TextureFilterOP mMagOP;
        TextureFilterOP mMipOP;
        TextureAddressing mAddressMode;
        CmpMode mCompareFunc;
        Colour mBorder;
        Nui32 mMaxAniso;
        NIIf mMipmapBias;
        bool mCompare;
    };
}
#endif