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

#ifndef _NII_TEXTURE_BLEND_H_
#define _NII_TEXTURE_BLEND_H_

#include "NiiPreInclude.h"
#include "NiiColour.h"

namespace NII
{
    /** 纹理混合操作数类型
    @version NIIEngine 3.0.0
    */
    enum TextureColourType
    {
        TCT_RGB,        ///< (Rs，Gs, Bs)
        TCT_AAA         ///< (As, As, As)
    };

    /** 纹理基本混合
    @remark
    @version NIIEngine 3.0.0
    */
    enum TextureBlendMode
    {
        TBM_Src_Replace_Dst,    ///< 后纹理代替
        TBM_Src_Add_Dst,        ///< 后纹理和前结果相加
        TBM_Src_Sub_Dst,        ///< 后纹理和前结果相减
        TBM_Src_Modulate_Dst,   ///< 后纹理和前结果相乘
        TBM_Src_Alpha_Dst,      ///< 后纹理和前结果的透明度加权
        TBM_Count               ///< 尽量使用现有的,因为被渲染系统约束,否则可以使用片段着色程序
    };

    /** 纹理第一/第二操作数类型
    @version NIIEngine 3.0.0
    */
    enum TextureBlendSource
    {
        TBS_PRE_TEXTURE,        ///< 前一个纹理层/阶段输出颜色.如当前是首层操作,则为顶点漫反射颜色.
        TBS_TEXTURE,            ///< 当前操作的纹理颜色
        TBS_DIFFUSE,            ///< 顶点漫反射插值(GOURAUD),渲染系统描述为对外反射的颜色
        TBS_SPECULAR,           ///< 顶点镜面反射插值(GOURAUD),渲染系统描述为对外反射的颜色
        TBS_CONSTANT,           ///< 自定义常量
        TBS_Count               ///< 尽量使用现有的,因为被渲染系统约束,否则可以使用片段着色程序
    };

    /** 纹理第一/第二操作数操作类型
    @note 第三操作属于内部类型
    @version NIIEngine 3.0.0
    */
    enum TextureBlendOp
    {
        TBO_OPERAND1,       ///< 返回一参,fn(src1, src2) = src1
        TBO_OPERAND2,       ///< 返回二参,fn(src1, src2) = src2
        TBO_OPERAND3,       ///< 返回三参,fn(src1, src2, src3) = src3
        TBO_MODULATE,       ///< 两参相乘后输出,fn(src1, src2) = src1 * src2
        TBO_MODULATE_2X,    ///< 两参相乘后再乘以2,fn(src1, src2) = src1 * src2 * 2
        TBO_MODULATE_4X,    ///< 两参相乘后再乘以4,fn(src1, src2) = src1 * src2 * 4
        TBO_SUB,            ///< 两参相减,fn(src1, src2) = src1 - src2
        TBO_ADD,            ///< 两参相加,fn(src1, src2) = src1 + src2
        TBO_ADD_SPECULAR,   ///< 像TBO_ADD, fn(src1, src2, TBS_PRE_TEXTURE) = (src1 * TBS_PRE_TEXTURE) + (src2 * (1-TBS_PRE_TEXTURE))
        TBO_ADD_DIFFUSE,    ///< 像TBO_ADD, fn(src1, src2, TBS_DIFFUSE) = (src1 * TBS_DIFFUSE) + (src2 * (1-TBS_DIFFUSE))
        TBO_ADD_TEXTURE,    ///< 像TBO_ADD, fn(src1, src2, GL_TEXTURE) = (src1 * GL_TEXTURE) + (src2 * (1-GL_TEXTURE))
        TBO_ADD_PRE_TEXTURE,///< 像TBO_ADD, fn(src1, src2, TBS_PRE_TEXTURE) = (src1 * TBS_PRE_TEXTURE) + (src2 * (1-TBS_PRE_TEXTURE))
        /** 两参相加后减去0.5输出.(其范围限制在[-0.5, 0.5]中.这样相加后,比较暗的部分,
            即负数部分,将对原来的纹理产生衰减,而较亮部分则对原来的纹理增强.可用作细节
            纹理.如果有的显卡不支持 D3DTOP_ADDSIGNED方式,可考虑采用 TBO_MODULATE_2X
            来模拟这种效果). fn(src1, src2) = src1 + src2 – 0.5
        */
        TBO_ADD_SIGNED,
        TBO_ADD_SIGNED_2X,  ///< 同上,结果再乘以2,fn(src1, src2) = (src1 + src2 – 0.5) * 2
        TBO_ADD_SMOOTH,     ///< 两参和减去两参的积,fn(src1, src2, src3) = (src1 * src3) + (src2 * (1-src3))
        TBO_DOT_PRODUCT,    ///< 像LBX_MODULATE, fn(src1, src2) = dot(src1, src2)
        TBO_CONSTANT_FACTOR,///< fn(src1) = (Rs * src1, Gs * src1, Bs * src1, As * src1)
        TBO_Count           ///< 尽量使用现有的,因为被渲染系统约束,否则可以使用片段着色程序
    };
    
    /** 多纹理状态的纹理混合
    @note 可编程管线中无效
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TextureBlend : public ShaderAlloc
    {
    public:
        TextureBlend();
        TextureBlend(TextureColourType type);
        TextureBlend(TextureColourType type, TextureBlendOp op, TextureBlendSource op1, TextureBlendSource op2);
        ~TextureBlend();

        void operator =(const TextureBlend & o);
        
        bool operator==(const TextureBlend & o) const;

        bool operator!=(const TextureBlend & o) const;

        /** 纹理颜色混合模式
        @note 可编程管线中无效
        @param[in] op 模式
        @version NIIEngine 3.0.0
        */
        void setColourBlend(TextureBlendMode mode);

        /** 纹理透明度混合模式
        @note 可编程管线中无效
        @param[in] op 模式
        @version NIIEngine 3.0.0
        */
        void setAlphaBlend(TextureBlendMode mode);

        /** 混合组
        @version NIIEngine 3.0.0
        */
        void setBlend(TextureBlendOp op, TextureBlendSource op1, TextureBlendSource op2);

        /** 混合组
        @version NIIEngine 3.0.0
        */
        void setBlend(TextureBlendSource op1, TextureBlendSource op2, NIIf factor);

        /** 混合组
        @version NIIEngine 3.0.0
        */
        void setBlend(TextureBlendOp op, const Colour & c1, TextureBlendSource op2);

        /** 混合组
        @version NIIEngine 3.0.0
        */
        void setBlend(TextureBlendOp op, TextureBlendSource op1, const Colour & c2);

        /** 混合组
        @version NIIEngine 3.0.0
        */
        void setBlend(TextureBlendOp op, NIIf f1, TextureBlendSource op2);

        /** 混合组
        @version NIIEngine 3.0.0
        */
        void setBlend(TextureBlendOp op, TextureBlendSource op1, NIIf f2);

        /** 设置混合操作
        @version NIIEngine 3.0.0
        */
        void setBlendOp(TextureBlendOp op);

        /** 设置颜色类型
        @version NIIEngine 3.0.0
        */
        void setBlendType(TextureColourType type);

        /** 设置第一操作数
        @param[in] tbs 操作数
        @version NIIEngine 3.0.0
        */
        void setBlendSource1(TextureBlendSource tbs);

        /** 设置第二操作数
        @param[in] tbs 操作数
        @version NIIEngine 3.0.0
        */
        void setBlendSource2(TextureBlendSource tbs);

        /** 设置常量第一操作数
        @note TextureBlendSource 为 TBS_CONSTANT 和 TextureColourType 为 TCT_RGB 时使用
        @version NIIEngine 3.0.0
        */
        void setConstant1(const Colour & c1);

        /** 设置常量第二操作数
        @note TextureBlendSource 为 TBS_CONSTANT 和 TextureColourType 为 TCT_RGB 时使用
        @version NIIEngine 3.0.0
        */
        void setConstant2(const Colour & c2);

        /** 设置常量第一操作数
        @note TextureBlendSource 为 TBS_CONSTANT 和 TextureColourType 为 TCT_RGB 时使用
        @version NIIEngine 3.0.0
        */
        void setConstant1(NIIf f1);

        /** 设置常量第一操作数
        @note TextureBlendSource 为 TBS_CONSTANT 和 TextureColourType 为 TCT_RGB 时使用
        @version NIIEngine 3.0.0
        */
        void setConstant2(NIIf f2);

        /** 设置常量混合因子
        @note TextureBlendOp 为 TBO_CONSTANT_FACTOR 时使用
        @version NIIEngine 3.0.0
        */
        void setConstantFactor(NIIf f);
    public:
        TextureBlendOp mOP;
        TextureColourType mColourType;
        TextureBlendSource mColourSrc1;
        TextureBlendSource mColourSrc2;
        NIIf mConstantFactor;
        Colour mConstant1;
        Colour mConstant2;
    };
}
#endif