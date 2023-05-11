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

#ifndef _NII_SHADER_CH_H_
#define _NII_SHADER_CH_H_

#include "NiiPreInclude.h"
#include "NiiColour.h"
#include "NiiCommon.h"
#include "NiiShaderChProgram.h"
#include "NiiShaderChTexture.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiExtData.h"

//#define ShaderChAlpha_Value                     0xFF
#define ShaderChAlpha_AlphaCoverage             0x200
#define ShaderChAlpha_OneAlphaCoverage          0x400

#define ShaderChBlendSeparate                   0x0100
#define ShaderChBlendSeparateMode               0x0200
#define ShaderChBlendColour                     0x0400
#define ShaderChBlendAplha                      0x0800
#define ShaderChBlendSrcOne                     0x10000
#define ShaderChBlendDstZERO                    0x20000
#define ShaderChBlendSrcOneDstZERO              0x30000
#define ShaderChBlendSrcAlphaOne                0x40000
#define ShaderChBlendDstAlphaZERO               0x80000
#define ShaderChBlendSrcAlphaOneAlphaZERO       0xC0000
#define ShaderChBlendPartSrcOneZERO             0xF0000

#define ShaderCh_DepthWrite                     0x01
#define ShaderCh_DepthCheck                     0x02
#define ShaderCh_DepthClip                      0x04
#define ShaderCh_StencilCheck                   0x08
#define ShaderCh_StencilBothSide                0x10

#define ShaderChPoint_Sprites                   0x01
#define ShaderChPoint_Attenuat                  0x02

namespace NII
{
    /** 颜色掩码
    @version NIIEngine 3.0.0
    */
    enum ColourMark
    {
        CM_ZERO     = 0,
        CM_RED      = 0x01,
        CM_GREEN    = 0x02,
        CM_BLUE     = 0x04,
        CM_ALPHA    = 0x08,
        CM_RGB      = 0x07,
        CM_BGR      = CM_RGB,
        CM_RGBA     = 0x0F,
        CM_BGRA     = CM_RGBA
    };

    enum ColourTraceMark
    {
        CTM_NONE         = 0x00,
        CTM_T_AMBIENT    = 0x01,
        CTM_T_DIFFUSE    = 0x02,
        CTM_T_SPECULAR   = 0x04,
        CTM_T_EMISSIVE   = 0x08,
        CTM_T_NONE       = 0x10 | 0x20 | 0x40 | 0x80,

        CTM_M_AMBIENT    = 0x10,
        CTM_M_DIFFUSE    = 0x20,
        CTM_M_SPECULAR   = 0x40,
        CTM_M_EMISSIVE   = 0x80,
        
        CTM_Front        = 0x100,
        CTM_Back         = 0x200,
        CTM_FrontBack    = 0x400,
        CTM_NORMAL       = 0x10 | 0x20 | 0x40 | 0x80 | 0x400
    };
    
    /** 着色成分
    @version NIIEngine 3.0.0
    */
    enum ShaderBase
    {
        SB_Colour_R             = 0x01,
        SB_Colour_G             = 0x02,
        SB_Colour_B             = 0x04,
        SB_Colour_A             = 0x08,
        SB_Colour_RGB           = 0x0E,
        SB_Colour_RGBA          = 0x0F,
        SB_Fog                  = 0x10,
        SB_Point                = 0x20,
        SB_Colour               = 0x40,
        SB_Blend                = 0x80,
        SB_Depth                = 0x100,
        SB_Alpha                = 0x200,
        SB_Stencil              = 0x400,
        SB_DepthStencil         = SB_Depth | SB_Stencil,
        SB_ColourAlpha          = SB_Colour | SB_Alpha,
        SB_Texture              = 0x800,
        SB_Program              = 0x1000,
        SB_FogRef               = 0x2000,                       ///< enable() 无效
        SB_PointRef             = 0x4000,                       ///< enable() 无效
        SB_ColourRef            = 0x8000,                       ///< enable() 无效
        SB_BlendRef             = 0x10000,                      ///< enable() 无效
        SB_DepthRef             = 0x20000,                      ///< enable() 无效
        SB_AlphaRef             = 0x40000,                      ///< enable() 无效
        SB_StencilRef           = 0x80000,                      ///< enable() 无效
        SB_DepthStencilRef      = SB_DepthRef | SB_StencilRef,  ///< enable() 无效
        SB_ColourAlphaRef       = SB_ColourRef | SB_AlphaRef,   ///< enable() 无效
        SB_TextureRef           = 0x100000,                     ///< enable() 无效
        SB_ProgramRef           = 0x200000,                     ///< enable() 无效
        SB_FogSet               = SB_Fog | SB_FogRef,           ///< enable() 无效
        SB_PointSet             = SB_Point | SB_PointRef,       ///< enable() 无效
        SB_ColourSet            = SB_Colour | SB_ColourRef,     ///< enable() 无效
        SB_BlendSet             = SB_Blend | SB_BlendRef,       ///< enable() 无效
        SB_DepthSet             = SB_Depth | SB_DepthRef,       ///< enable() 无效
        SB_AlphaSet             = SB_Alpha | SB_AlphaRef,       ///< enable() 无效
        SB_StencilSet           = SB_Stencil | SB_StencilRef,   ///< enable() 无效
        SB_DepthStencilSet      = SB_DepthSet | SB_StencilSet,  ///< enable() 无效
        SB_ColourAlphaSet       = SB_ColourSet | SB_AlphaSet,   ///< enable() 无效
        SB_TextureSet           = SB_Texture | SB_TextureRef,   ///< enable() 无效
        SB_ProgramSet           = SB_Program | SB_ProgramRef,   ///< enable() 无效
        SB_Frame_Depth          = 0x400000,
        SB_Frame_Stencil        = 0x800000,
        SB_Clip                 = 0x1000000,
        SB_Light                = 0x2000000,
        SB_FogValid             = 0x4000000,
        SB_UnitNormals          = 0x8000000,
        SB_InvertVertexWinding  = 0x10000000,
        SB_LightClip            = 0x20000000,
        SB_LightClipPlane       = 0x40000000,
        SB_DepthWriteDisable    = 0x80000000,
        SB_DepthCheckDisable    = 0x100000000,
        // 扩展
        SB_Fusion               = 0x200000000,                  ///< enable() disbale 无效 64bit enum
    };
    
    /**
    @version NIIEngine 6.0.0
    */
    enum ShaderChType
    {
        SCT_Colour,
        SCT_Blend,
        SCT_Stencil,
        SCT_Fog,
        SCT_Point
    };
    
    /**
    @version NIIEngine 6.0.0
    */
    class _EngineAPI ShaderChBase : public ShaderAlloc
    {
    public:
        ShaderChBase(ShaderChType type, Nmark bindmark = 0xFFFFFFFF);
        virtual ~ShaderChBase();
        
        /**
        @version NIIEngine 6.0.0
        */
        virtual void read(GpuProgramParam * out, NCount memoft) const;

        /**
        @version NIIEngine 6.0.0
        */
        virtual void write(const GpuProgramParam * in, NCount memoft);
        
        /**
        @version NIIEngine 6.0.0
        */
        virtual void bind();
    protected:
        Nid mId;
        ShaderChType mType;
        NCount mRefCount;
        Nmark mBindMark;
    };

    /** 渲染通路中的基本颜色
    @remark 适合基本渲染系统
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChColour : public ShaderChBase
    {
    public:
        ShaderChColour();
        ShaderChColour(const ShaderChColour & o);
        ShaderChColour(CmpMode func, Nui8 value, bool alphaCoverage);
        ShaderChColour(const Colour & ambient, const Colour & diffuse, const Colour & specular, NIIf shinin);
        ShaderChColour(const Colour & ambient, const Colour & diffuse, const Colour & specular, const Colour & emissive, 
            NIIf shinin, Nmark mark);

        bool operator == (const ShaderChColour & o) const;
        bool operator < (const ShaderChColour & o) const;

        /** 获取是否存在反射颜色
        @version NIIEngine 3.0.0
        */
        bool isReflexColour() const;
        
        /** 设置渲染通道环境反射颜色属性
        @param[in] c 3基参
        @version NIIEngine 3.0.0
        */
        inline void setAmbient(const Colour & c)    { mAmbient = c; }

        /** 设置渲染通道漫反射颜色属性
        @param[in] c 4基参
        @version NIIEngine 3.0.0
        */
        inline void setDiffuse(const Colour & c)    { mDiffuse = c; }

        /** 设置渲染通道镜面反射颜色属性
        @param[in] c 4基参
        @version NIIEngine 3.0.0
        */
        inline void setSpecular(const Colour & c)   { mSpecular = c; }

        /** 设置对象具有自发光的颜色
        @param[in] c 3基参
        @version NIIEngine 3.0.0
        */
        inline void setEmissive(const Colour & c)   { mEmissive = c; }

        /** 设置通道的光亮度,影响镜面高光点的大小
        @param[in] val 亮度
        @version NIIEngine 3.0.0
        */
        inline void setShininess(NIIf val)          { mShininess = val; }

        /** 设置顶点颜色追踪
        @remark 追踪 glColour / D3DMCS_COLOR1 设置到指定属性中
        @version NIIEngine 3.0.0
        */
        void setColourTracking(Nmark mark);
        
        /** 附加顶点颜色追踪
        @remark 追踪 glColour / D3DMCS_COLOR1 设置到指定属性中
        @version NIIEngine 3.0.0
        */
        inline void addColourTracking(Nmark mark)   { mMark |= mark; }
        
        /** 获取渲染通道环境反射颜色
        @return 3基参
        @version NIIEngine 3.0.0
        */
        inline const Colour & getAmbient() const    { return mAmbient; }

        /** 获取渲染通道漫反射颜色
        @return 4基参
        @version NIIEngine 3.0.0
        */
        inline const Colour & getDiffuse() const    { return mDiffuse; }

        /** 获取渲染通道镜面反射颜色
        @return 4基参
        @version NIIEngine 3.0.0
        */
        inline const Colour & getSpecular() const   { return mSpecular; }

        /** 获取渲染通路自发光颜色
        @return 3基参
        @version NIIEngine 3.0.0
        */
        inline const Colour & getEmissive() const   { return mEmissive; }

        /** 获取渲染通道"光亮"属性(影响镜面高光点)
        @version NIIEngine 3.0.0
        */
        inline NIIf getShininess() const            { return mShininess; }

        /** 获取顶点颜色追踪
        @remark 追踪 glColour / D3DMCS_COLOR1 等设置到指定属性中
        @version NIIEngine 3.0.0
        */
        inline Nmark getColourTracking() const      { return mMark & CTM_T_NONE; }
        
        /** 设置是否使用将绘制的片元的alpha来计算最终的覆盖比率
        @remark
            多重采样缓冲区在默认情况下使用片段的RGB值,并不包括颜色的alpha成分.
            所以需要特殊设置
        @note 值会被最后一次设置代替
        @version NIIEngine 3.0.0
        */
        void setAlphaCoverage(bool b)               { b ?  mMark |= ShaderChAlpha_AlphaCoverage : mMark &= ~ShaderChAlpha_AlphaCoverage;}

        /** 返回是否使用将绘制的片元的alpha来计算最终的覆盖比率
        @remark
            多重采样缓冲区在默认情况下使用片段的RGB值,并不包括颜色的alpha成分.
            所以需要特殊设置
        @note 值会被最后一次设置代替
        @version NIIEngine 3.0.0
        */
        bool isAlphaCoverage() const                { return mMark & ShaderChAlpha_AlphaCoverage; }

        /** 设置是否使用将绘制的片元的alpha值设置为最大值(1)来计算覆盖比率
        @remark
            多重采样缓冲区在默认情况下使用片段的RGB值,并不包括颜色的alpha成分.
            所以需要特殊设置
        @note 值会被最后一次设置代替
        @version NIIEngine 3.0.0
        */
        void setPercentAlphaCoverage(bool b)        { b? mMark |= ShaderChAlpha_OneAlphaCoverage : mMark &= ~ShaderChAlpha_OneAlphaCoverage; }

        /** 返回是否使用将绘制的片元的alpha值设置为最大值(1)来计算覆盖比率
        @remark
            多重采样缓冲区在默认情况下使用片段的RGB值,并不包括颜色的alpha成分.
            所以需要特殊设置
        @note 值将会被最后一次设置代替
        @version NIIEngine 3.0.0
        */
        bool isPercentAlphaCoverage() const         { return mMark & ShaderChAlpha_OneAlphaCoverage; }

        /** 设置透明度拒绝模式.
        @version NIIEngine 3.0.0
        */
        inline void setAlphaReject(CmpMode mode)    { mRejectMode = mode; }

        /** 获取透明度拒绝模式.
        @version NIIEngine 3.0.0
        */
        inline CmpMode getAlphaReject() const       { return mRejectMode; }

        /** 获取透明度参考值
        @param[in] val [0.f,1.f]
        @version NIIEngine 3.0.0
        */
        inline void setValue(NIIf val)              { mRefValue = val; }

        /** 获取透明度参考值.
        @return [0.f,1.f]
        @version NIIEngine 3.0.0
        */
        inline NIIf getValue() const                { return mRefValue; }

        /** 上传到着色程序参数中
        @note 5个vec4,{vec4(mAmbient), vec4(mDiffuse), vec4(mSpecular), vec4(mEmissive), float(mEmissive), float(mShininess), 
            float(mRefValue), int(mRejectMode), int(mMark)}
        @version NIIEngine 5.0.0
        */
        void read(GpuProgramParam * out, NCount memoft) const;

        /** 从着色程序参数中设置
        @note 5个vec4,{vec4(mAmbient), vec4(mDiffuse), vec4(mSpecular), vec4(mEmissive), float(mEmissive), float(mShininess),
            float(mRefValue), int(mRejectMode), int(mMark)}
        @version NIIEngine 5.0.0
        */
        void write(const GpuProgramParam * in, NCount memoft);
    public:
        Colour mAmbient;
        Colour mDiffuse;
        Colour mSpecular;
        Colour mEmissive;
        NIIf mShininess;
        NIIf mRefValue;
        CmpMode mRejectMode;
    private:
        Nmark mMark;
    };
    
    /** 应用混合
    @remark ColourBlendMode 和 ColourFactor 预组合
    @version NIIEngine 3.0.0
    */
    enum FrameBlendMode
    {
        FBM_ADD         = 0,    ///< CF_ONE, CF_ONE, CBM_ADD
        FBM_MIN         = 1,    ///< /, /, CBM_MIN
        FBM_MAX         = 2,    ///< /, /, CBM_MAX
        FBM_SUBTRACT    = 3,    ///< CF_ONE, CF_ONE, CBM_SUBTRACT
        FBM_MODULATE    = 4,    ///< CF_DST, CF_ZERO, CBM_ADD
        FBM_REPLACE     = 5,    ///< CF_ONE, CF_ZERO, CBM_ADD / CBM_SUBTRACT
        FBM_INV_ALPHA   = 6,
        FBM_INV_COLOUR  = 7,
        FBM_Count       = 8
    };

    /** 颜色混合模式
    @remark
        Cf/混合后显示的颜色 Fdst/混合前颜色缓冲已存在的颜色值 Tsrc/将要绘制的颜色
        src/ColourFactor 函数设置时的第一个参数 dst/ColourFactor 函数设置时的第二个参数
    @version NIIEngine 3.0.0
    */
    enum ColourBlendMode
    {
        CBM_ADD         = 0,    ///< Cf = (Tsrc * src)+(Fdst * dst)
        CBM_SUBTRACT    = 1,    ///< Cf = (Tsrc * src)-(Fdst * dst)
        CBM_REV_SUBTRACT= 2,    ///< Cf = (Fdst * dst)-(Tsrc * src)
        CBM_MIN         = 3,    ///< Cf = min(Tsrc, Fdst)
        CBM_MAX         = 4,    ///< Cf = max(Tsrc, Fdst)
        CBM_COUNT       = 5     ///< 尽量使用现有的,因为被渲染系统约束,否则只能多次渲染通道混合
    };

    /** src/dst 颜色混合因子
    @remark 在各种渲染器中以基础存在的
    @version NIIEngine 3.0.0
    */
    enum ColourFactor
    {
        CF_ONE          = 0,    ///< 使用1.0作为因子,实际上相当于完全的使用了这种颜色参与混合运算。
        CF_SRC          = 1,    ///< 源颜色的四个分量分别作为因子的四个分量
        CF_DST          = 2,    ///< 目标颜色的四个分量分别作为因子的四个分量
        CF_INV_DST      = 3,    ///< 用1.0减去目标颜色的四个分量分别作为因子的四个分量
        CF_INV_SRC      = 4,    ///< 用1.0减去原颜色的四个分量分别作为因子的四个分量
        CF_SRC_ALPHA    = 5,    ///< 使用源颜色的alpha值来作为因子。
        CF_DST_ALPHA    = 6,    ///< 使用目标颜色的alpha值来作为因子。
        CF_INV_DST_ALPHA= 7,    ///< 用1.0减去目标颜色的alpha值来作为因子。
        CF_INV_SRC_ALPHA= 8,    ///< 用1.0减去源颜色的alpha值来作为因子。
        CF_ZERO         = 9,    ///< 使用0.0作为因子，实际上相当于不使用这种颜色参与混合运算。
        CF_COUNT        = 10    ///< 尽量使用现有的,因为被渲染系统约束,否则只能多次渲染通道混合
    };

    /** 渲染通路中的基本颜色混合
    @remark 适合基本渲染系统
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChBlend : public ShaderChBase
    {
    public:
        ShaderChBlend();
        ShaderChBlend(const ShaderChBlend & o);
        ShaderChBlend(ColourFactor src, ColourFactor dst, ColourBlendMode mode);
        ShaderChBlend(ColourFactor src, ColourFactor dst, ColourFactor asrc, ColourFactor adst, ColourBlendMode mode);
        ShaderChBlend(ColourFactor src, ColourFactor dst, ColourFactor asrc, ColourFactor adst, ColourBlendMode mode, ColourBlendMode amode);

        bool operator == (const ShaderChBlend & o) const;
        bool operator < (const ShaderChBlend & o) const;
        
        /** 设置颜色写入
        @remark 基础事物
        @param[in] mark ColourMark 的一个或多个组合
        @version NIIEngine 3.0.0
        */
        void setColourWrite(Nmark mark)                 { mMark |= (mark & CM_RGBA);}

        /** 获取颜色写入
        @remark 基础事物
        @version NIIEngine 3.0.0
        */
        Nmark getColourWrite() const                    { return mMark & CM_RGBA; }
        
        /** 设置渲染通道混合场景存在内容的混合类型
        @param[in] fbm 模式
        @version NIIEngine 3.0.0
        */
        void setBlend(FrameBlendMode fbm);

        /** 设置渲染通道混合场景存在内容的混合类型,分别为彩色和透明通道
        @param[in] fbm RGB的模式
        @param[in] afbm A的模式
        @version NIIEngine 3.0.0
        */
        void setBlend(FrameBlendMode fbm, FrameBlendMode afbm);

        /** 设置混合因子
        @param[in] src RGB的因子
        @param[in] dst A的因子
        @version NIIEngine 3.0.0 高级api
        */
        void setBlend(ColourFactor src, ColourFactor dst);

        /** 颜色混合模式
        @version NIIEngine 3.0.0
        */
        void setBlendMode(ColourBlendMode mode);

        /** 颜色混合模式
        @param[in] mode RGB的混合模式
        @param[in] amode A的混合模式
        @version NIIEngine 3.0.0
        */
        void setBlendMode(ColourBlendMode mode, ColourBlendMode amode);

        /** 获取当前的混合操作
        @version NIIEngine 3.0.0
        */
        inline ColourBlendMode getBlendMode() const     { return mMode; }

        /** 获取当前的透明混合操作
        @version NIIEngine 3.0.0
        */
        inline ColourBlendMode getAlphaBlendMode() const{ return mAlphaMode;}

        /** 设置混合因子
        @param[in] src 将要绘制的RGB
        @param[in] dst 帧缓存的RGB
        @param[in] asrc 将要绘制的A
        @param[in] adst 帧缓存的A
        @version NIIEngine 3.0.0 高级api
        */
        void setBlend(ColourFactor src, ColourFactor dst, ColourFactor asrc, ColourFactor adst);

        /** 获取这个材质来源混合因子
        @version NIIEngine 3.0.0
        */
        inline ColourFactor getSrcFactor() const        { return mSrcFactor; }

        /** 获取这个材质目标混合因子
        @version NIIEngine 3.0.0
        */
        inline ColourFactor getDstFactor() const        { return mDstFactor; }

        /** 获取这个材质透明来源混合因子
        @version NIIEngine 3.0.0
        */
        inline ColourFactor getSrcAlphaFactor() const   { return mSrcAlphaFactor; }

        /** 获取这个材质透明目标混合因子
        @version NIIEngine 3.0.0
        */
        inline ColourFactor getDstAlphaFactor() const   { return mDstAlphaFactor; }

        /** 是否产生覆盖效应
        @version NIIEngine 3.0.0
        */
        bool isReplace() const                          { return (mMark & ShaderChBlendSrcOneDstZERO) == ShaderChBlendSrcOneDstZERO; }

        /** 是否产生覆盖效应(分离透明通道设置)
        @version NIIEngine 3.0.0
        */
        bool isPartReplace() const                      { return (mMark & ShaderChBlendPartSrcOneZERO) == ShaderChBlendPartSrcOneZERO; }

        /** 是否产生透明度效应
        @version NIIEngine3.0.0
        */
        bool isTransparent() const;

        /** 是否分离透明通道设置
        @version NIIEngine 3.0.0
        */
        bool isPartAlpha() const                        { return mMark & ShaderChBlendSeparate; }

        /** 是否分离透明通道混合设置
        @version NIIEngine 3.0.0
        */
        bool isPartAlphaBlendMode() const               { return mMark & ShaderChBlendSeparateMode;}

        /** 上传到着色程序参数中
        @note {int(mSrcFactor), int(mDstFactor), int(mSrcAlphaFactor), int(mDstAlphaFactor),
        int(mMode), int(mAlphaMode), int(mMark)}
        @version NIIEngine 5.0.0
        */
        void read(GpuProgramParam * out, NCount memoft) const;

        /** 从着色程序参数中设置
        @note {int(mSrcFactor), int(mDstFactor), int(mSrcAlphaFactor), int(mDstAlphaFactor),
        int(mMode), int(mAlphaMode), int(mMark)}
        @version NIIEngine 5.0.0
        */
        void write(const GpuProgramParam * in, NCount memoft);
    public:
        ColourFactor mSrcFactor;        ///< 混合函子
        ColourFactor mDstFactor;        ///< 混合函子
        ColourFactor mSrcAlphaFactor;   ///< 混合函子
        ColourFactor mDstAlphaFactor;   ///< 混合函子
    private:
        ColourBlendMode mMode;          ///< 混合操作(颜色)
        ColourBlendMode mAlphaMode;     ///< 混合操作(透明度)
        Nmark mMark;
    };
    
    /** 模板缓冲操作类型
    @version NIIEngine 3.0.0
    */
    enum StencilOpType
    {
        SOT_KEEP,               ///< 保持模版缓存不改变
        SOT_ZERO,               ///< 设置模版值为0
        SOT_REPLACE,            ///< 替换模版值为参考值
        SOT_INCR,               ///< 用1增量增加模版值,范围在最大值下(包含最大值)
        SOT_DECR,               ///< 用1减量减少模版值,范围在0上(包含0)
        SOT_INVERT,             ///< 反转模板缓冲区的位
        SOT_INCR_WRAP,          ///< 用1增量增加模版值,超出最大范围后跳转到0
        SOT_DECR_WRAP           ///< 用1减量减少模版值,超出范围保持0
    };
    
    /** 渲染通路中的基本深度缓存
    @remark 适合基本渲染系统
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChStencil : public ShaderChBase
    {
    public:
        ShaderChStencil();    
        ShaderChStencil(const ShaderChStencil & o); 
        ShaderChStencil(bool depthCheck, CmpMode mode, bool write, NIIf bias = 0.0f, NIIf scaleslopebias = 0.0f);
        ShaderChStencil(bool stencilTwoside, Nui32 mask = 0xFFFFFFFF, Nui32 cmpmask = 0xFFFFFFFF,
            CmpMode func = CPM_ALWAYS_PASS, Nui32 value = 0, StencilOpType stencilfail = SOT_KEEP,
            StencilOpType depthfail = SOT_KEEP, StencilOpType pass = SOT_KEEP);
        bool operator == (const ShaderChStencil & o) const;
        bool operator < (const ShaderChStencil & o) const;
        
        /** 设置是否启动模板测试
        @version NIIEngine 3.0.0
        */
        inline void setStencilEnable(bool s)                { b ? mMark |= ShaderCh_StencilCheck : mMark &= ~ShaderCh_StencilCheck; }

        /** 获取是否启动模板测试
        @version NIIEngine 3.0.0
        */
        inline bool isStencilEnable() const                 { return mMark & ShaderCh_StencilCheck; }

        /** 设置是否双面模板测试
        @version NIIEngine 3.0.0
        */
        inline void setBothSideStencil(bool s)              { b ? mMark |= ShaderCh_StencilBothSide : mMark &= ~ShaderCh_StencilBothSide; }

        /** 获取是否双面模板测试
        @version NIIEngine 3.0.0
        */
        inline bool isBothSideStencil() const               { return mMark & ShaderCh_StencilBothSide; }
        
        /** 设置测试的比较深度.
        @version NIIEngine 3.0.0
        */
        void setDepthCompareFunc(CmpMode mode)              { mDepthFunc = mode; }

        /** 获取测试的比较深度
        @version NIIEngine 3.0.0
        */
        CmpMode getDepthCompareFunc() const                 { return mDepthFunc; }

        /** 设置缓存测试
        @version NIIEngine 3.0.0
        */
        void setDepthEnable(bool b)                         { b ? mMark |= ShaderCh_DepthCheck : mMark &= ~ShaderCh_DepthCheck; }

        /** 是否缓存测试
        @version NIIEngine 3.0.0
        */
        bool isDepthEnable() const                          { return mMark & ShaderCh_DepthCheck; }

        /** 设置缓存写入
        @version NIIEngine 3.0.0
        */
        void setDepthWrite(bool b)                          { b ? mMark |= ShaderCh_DepthWrite : mMark &= ~ShaderCh_DepthWrite; }

        /** 是否缓存写入
        @version NIIEngine 3.0.0
        */
        bool isDepthWrite() const                           { return mMark & ShaderCh_DepthWrite; }

        /** 设置偏量常数
        @version NIIEngine 3.0.0
        */
        inline void setBiasConstant(NIIf f)                 { mBias = f; }

        /** 获取偏量常数
        @remark 基本渲染系统中存在
        @version NIIEngine 3.0.0
        */
        inline NIIf getBiasConstant() const                 { return mBias; }

        /** 设置偏量常数因子
        @version NIIEngine 3.0.0
        */
        inline void setBiasConstantFactor(NIIf f)           { mBiasFactor = f; }

        /** 获取偏量常数因子
        @version NIIEngine 3.0.0
        */
        inline NIIf getBiasConstantFactor() const           { return mBiasFactor; }

        /** 设置斜率补偿
        @param[in] f
        @note 旧硬件可能不支持
        @version NIIEngine 3.0.0
        */
        inline void setBiasSlopeScale(NIIf f)               { mSlopeScaleBias = f; }

        /** 获取斜率补偿
        @remark 基本渲染系统中存在
        @version NIIEngine 3.0.0
        */
        inline NIIf getBiasSlopeScale() const               { return mSlopeScaleBias; }

        /** 设置模板测试掩码
        @version NIIEngine 3.0.0
        */
        inline void setTestMask(Nui32 mask)                 { mFrontTestMark = mBackTestMark = mask; }
        
        /** 设置模板测试掩码
        @version NIIEngine 3.0.0
        */
        inline void setFrontTestMask(Nui32 mask)            { mFrontTestMark = mask; }
        
        /** 设置模板测试掩码
        @version NIIEngine 3.0.0
        */
        inline void setBackTestMask(Nui32 mask)             { mBackTestMark = mask; }

        /** 获取模板测试掩码
        @version NIIEngine 3.0.0
        */
        inline Nui32 getTestMask() const                    { return mFrontTestMark; }

        /** 设置模板比较掩码
        @version NIIEngine 3.0.0
        */
        inline void setCompressMask(Nui32 mask)             { mFrontCmpMark = mBackCmpMark = mask; }
        
        /** 设置模板比较掩码
        @version NIIEngine 3.0.0
        */
        inline void setFrontCompressMask(Nui32 mask)        { mFrontCmpMark =  mask; }
        
        /** 设置模板比较掩码
        @version NIIEngine 3.0.0
        */
        inline void setBackCompressMask(Nui32 mask)         { mBackCmpMark = mask; }

        /** 获取模板比较掩码
        @version NIIEngine 3.0.0
        */
        inline Nui32 getCompressMask() const                { return mFrontCmpMark; }

        /** 设置模板比较值
        @version NIIEngine 3.0.0
        */
        inline void setCompressValue(Nui32 value)           { mFrontCmpValue = mBackCmpValue = value; }
        
        /** 设置模板比较值
        @version NIIEngine 3.0.0
        */
        inline void setFrontCompressValue(Nui32 value)      { mFrontCmpValue = value; }
        
        /** 设置模板比较值
        @version NIIEngine 3.0.0
        */
        inline void setBackCompressValue(Nui32 value)       { mBackCmpValue = value; }

        /** 设置模板比较值
        @version NIIEngine 3.0.0
        */
        inline Nui32 getCompressValue() const               { return mFrontCmpValue; }

        /** 设置模板比较失败操作
        @version NIIEngien 3.0.0
        */
        inline void setStencilFailOp(StencilOpType op)      { mFrontStencilFailOp = mBackStencilFailOp = op; }

        /** 设置模板比较失败操作
        @version NIIEngien 3.0.0
        */
        inline void setFrontStencilFailOp(StencilOpType op) { mFrontStencilFailOp =  op; }
        
        /** 设置模板比较失败操作
        @version NIIEngien 3.0.0
        */
        inline void setBackStencilFailOp(StencilOpType op)  { mBackStencilFailOp = op; }

        /** 获取模板比较失败操作
        @version NIIEngien 3.0.0
        */
        inline StencilOpType getStencilFailOp() const       { return mFrontStencilFailOp; }

        /** 设置深度比较失败操作
        @version NIIEngien 3.0.0
        */
        inline void setDepthFailOp(StencilOpType op)        { mFrontDepthFailOp = mBackDepthFailOp = op; }
        
        /** 设置深度比较失败操作
        @version NIIEngien 3.0.0
        */
        inline void setFrontDepthFailOp(StencilOpType op)   { mFrontDepthFailOp =  op; }
        
        /** 设置深度比较失败操作
        @version NIIEngien 3.0.0
        */
        inline void setBackDepthFailOp(StencilOpType op)    { mBackDepthFailOp = op; }

        /** 获取深度比较失败操作
        @version NIIEngien 3.0.0
        */
        inline StencilOpType getDepthFailOp() const         { return mFrontDepthFailOp; }

        /** 设置模板比较通过操作
        @version NIIEngien 3.0.0
        */
        inline void setTestPassOp(StencilOpType op)         { mFrontTestPassOp = mBackTestPassOp = op; }
        
        /** 设置模板比较通过操作
        @version NIIEngien 3.0.0
        */
        inline void setFrontTestPassOp(StencilOpType op)    { mFrontTestPassOp =  op; }
        
        /** 设置模板比较通过操作
        @version NIIEngien 3.0.0
        */
        inline void setBackTestPassOp(StencilOpType op)     { mBackTestPassOp = op; }

        /** 获取模板比较通过操作
        @version NIIEngien 3.0.0
        */
        inline StencilOpType getTestPassOp() const          { return mFrontTestPassOp; }
        
        /** 翻转操作
        @version NIIEngine 5.0.0
        */
        void flipStencil();
        
        /** 上传到着色程序参数中
        @note {int(mFrontTestMark), int(mBackTestMark), int(mFrontCmpMark), int(mBackCmpMark), 
            int(mFrontCmpValue), int(mBackCmpValue), int(mFrontFunc), int(mBackFunc),
            int(mFrontStencilFailOp), int(mFrontDepthFailOp), int(mFrontTestPassOp), int(mBackStencilFailOp),
            int(mBackDepthFailOp), int(mBackTestPassOp) float(mBias), float(mBiasFactor), float(mSlopeScaleBias), int(mDepthFunc), int(mMark)}
        @version NIIEngine 5.0.0
        */
        void read(GpuProgramParam * out, NCount memoft) const;

        /** 从着色程序参数中设置
        @note {int(mFrontTestMark), int(mBackTestMark), int(mFrontCmpMark), int(mBackCmpMark), 
            int(mFrontCmpValue), int(mBackCmpValue), int(mFrontFunc), int(mBackFunc),
            int(mFrontStencilFailOp), int(mFrontDepthFailOp), int(mFrontTestPassOp), int(mBackStencilFailOp),
            int(mBackDepthFailOp), int(mBackTestPassOp) float(mBias), float(mBiasFactor), float(mSlopeScaleBias), int(mDepthFunc), int(mMark)}
        @version NIIEngine 5.0.0
        */
        void write(const GpuProgramParam * in, NCount memoft);
    private:
        Nmark mMark;
        NIIf mBias;                 ///< 深度缓存设置(多通道的基础值)
        NIIf mBiasFactor;           ///< 深度缓存设置(多通道的基础值系数)
        NIIf mSlopeScaleBias;       ///< 深度斜率补偿
        CmpMode mDepthFunc;         ///< 深度比较函数
        Nui32 mFrontTestMark;       ///< write mark
        Nui32 mBackTestMark;        ///< write mark
        Nui32 mFrontCmpMark;        ///< read mark
        Nui32 mBackCmpMark;         ///< read mark
        Nui32 mFrontCmpValue;       ///< ref value
        Nui32 mBackCmpValue;        ///< ref value
        CmpMode mFrontFunc;
        CmpMode mBackFunc;
        StencilOpType mFrontStencilFailOp;
        StencilOpType mFrontDepthFailOp;
        StencilOpType mFrontTestPassOp;
        StencilOpType mBackStencilFailOp;
        StencilOpType mBackDepthFailOp;
        StencilOpType mBackTestPassOp;
    };
    
    /** 雾状着色模式.
    @remark
        基础概念,普通的opengl/dx已经有api能实现,最好在最后一次渲染时使用,因为效果并不好,
        使用粒子程序配合,做出朦胧效果.
    @version NIIEngine 3.0.0
    */
    enum FogMode
    {
        FM_NONE = 0,///< 没有雾
        FM_EXP,     ///< 通用普通渲染
        FM_EXP2,    ///< 通用普通渲染
        FM_LINEAR,  ///< 通用普通渲染
        FM_Count    ///< 尽量使用现有的,因为被渲染系统约束,否则只能用粒子系统或片段着色程序
    };

    /** 渲染通路中的基本雾
    @remark 适合基本渲染系统
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChFog : public ShaderChBase
    {
    public:
        ShaderChFog();
        ShaderChFog(const ShaderChFog & o);
        ShaderChFog(FogMode m, const Colour & c, NIIf density, NIIf start, NIIf end);

        bool operator == (const ShaderChFog & o) const;
        bool operator < (const ShaderChFog & o) const;
        
        /** 设置这条通道的雾模式.
        @version NIIEngine 3.0.0
        */
        inline void setMode(FogMode m)          { mMode = m; }

        /** 获取这条通道的雾模式.
        @version NIIEngine 3.0.0
        */
        inline FogMode getMode() const          { return mMode; }

        /** 设置雾颜色
        @version NIIEngine 3.0.0
        */
        inline void setColour(const Colour & c) { mColour = c; }

        /** 返回雾颜色
        @version NIIEngine 3.0.0
        */
        inline const Colour & getColour() const { return mColour; }

        /** 设置雾开始的距离.
        @version NIIEngine 3.0.0
        */
        inline void setBegin(NIIf f)            { mBegin = f; }

        /** 获取雾开始的距离.
        @remark 仅 FM_LINEAR 模式
        @version NIIEngine 3.0.0
        */
        inline NIIf getBegin() const            { return mBegin; }

        /** 设置雾结束的距离
        @version NIIEngine 3.0.0
        */
        inline void setEnd(NIIf f)              { mEnd = f; }

        /** 获取雾结束的距离
        @remark 仅 FM_LINEAR 模式
        @version NIIEngine 3.0.0
        */
        inline NIIf getEnd() const              { return mEnd; }

        /** 设置雾的密度
        @version NIIEngine 3.0.0
        */
        inline void setDensity(NIIf f)          { mDensity = f; }

        /** 返回雾的密度
        @remark FM_EXP/FM_EXP2 模式使用,[0,1] 默认 0.001.
        @version NIIEngine 3.0.0
        */
        inline NIIf getDensity() const          { return mDensity; }

        /** 上传到着色程序参数中
        @note {vec4(mColour), int(mMode), float(mBegin), float(mEnd), float(mDensity)}
        @version NIIEngine 5.0.0
        */
        void read(GpuProgramParam * out, NCount memoft) const;

        /** 从着色程序参数中设置
        @note {vec4(mColour), int(mMode), float(mBegin), float(mEnd), float(mDensity)}
        @version NIIEngine 5.0.0
        */
        void write(const GpuProgramParam * in, NCount memoft);
    public:
        static const ShaderChFog White;
        static const ShaderChFog Red;
        static const ShaderChFog Green;
        static const ShaderChFog Yellow;
        static const ShaderChFog Blue;
        static const ShaderChFog None;
    public:
        Colour mColour;     ///< 雾颜色
        FogMode mMode;      ///< 雾质量模式
        NIIf mBegin;        ///< 雾开始处(与平截头相关)
        NIIf mEnd;          ///< 雾结束处(与平截头相关)
        NIIf mDensity;      ///< 雾质量
    };
    
    /** 渲染通路中的基本点属性
    @remark 适合基本渲染系统
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChPoint : public ShaderChBase
    {
    public:
        ShaderChPoint();
        ShaderChPoint(const ShaderChPoint & o);
        ShaderChPoint(NIIf size, NIIf minsize, NIIf maxsize);

        bool operator == (const ShaderChPoint & o) const;
        bool operator < (const ShaderChPoint & o) const;

        /** 设置点大小
        @param[in] ps
        @version NIIEngine 3.0.0
        */
        inline void setSize(NIIf ps)            { mSize = ps; }

        /** 获取点大小
        @remark 这个属性确定在渲染点列表时使用的点大小
        @version NIIEngine 3.0.0
        */
        inline NIIf getSize() const             { return mSize; }

        /** 设置点最小大小
        @version NIIEngine 3.0.0
        */
        inline void setMinSize(NIIf min)        { mMinSize = min; }

        /** 获取点最小大小
        @version NIIEngine 3.0.0
        */
        inline NIIf getMinSize() const          { return mMinSize; }

        /** 设置线宽
        @version NIIEngine 3.0.0
        */
        inline void setLineWidth(NIIf w)        { mLineWidth = w; }

        /** 获取线宽
        @version NIIEngine 3.0.0
        */
        inline NIIf getLineWidth() const        { return mLineWidth; }

        /** 设置点最大大小
        @param[in] max 渲染系统支持的最大设置为 0
        @version NIIEngine 3.0.0
        */
        inline void setMaxSize(NIIf max)        { mMaxSize = max; }

        /** 获取点最大大小
        @remark 渲染系统支持的最大设置为 0
        @version NIIEngine 3.0.0
        */
        inline NIIf getMaxSize() const          { return mMaxSize; }

        /** 设置点列表渲染模式
        @remark 基本渲染中可能包含的特性,使用 POINTLIST / POINTS 模式绘制点,粒子系统可能能用上
        @version NIIEngine 3.0.0
        */
        void setSpritesEnable(bool b)           { b ? mMark |= ShaderChPoint_Sprites : mMark &= ~ShaderChPoint_Sprites;}

        /** 是否是点列表渲染模式
        @return 基本渲染中可能包含的特性,使用 POINTLIST / POINTS 模式绘制点,粒子系统可能能用上
        @version NIIEngine 3.0.0
        */
        bool isSpritesEnable() const            { return mMark & ShaderChPoint_Sprites;}

        /** 设置是否启用距离衰减
        @return attenuate = 1 / (constant + linear * dist + quadratic * d^2).
        @version NIIEngine 3.0.0
        */
        void setAttenuateEnable(bool b)         { b ? mMark |= ShaderChPoint_Attenuat: mMark &= ~ShaderChPoint_Attenuat; }

        /** 返回点是否距启用距离衰减
        @version NIIEngine 3.0.0
        */
        bool isAttenuateEnable() const          { return mMark & ShaderChPoint_Attenuat; }

        /** 设置点距离衰减常数系数
        @version NIIEngine 3.0.0
        */
        inline void setConstant(NIIf f)         { mConstant = f; }

        /** 获取点距离衰减常数系数
        @version NIIEngine 3.0.0
        */
        inline NIIf getConstant() const         { return mConstant; }

        /** 设置点距离衰减线性系数.
        @version NIIEngine 3.0.0
        */
        inline void setLinear(NIIf f)           { mLinear = f; }

        /** 获取点距离衰减线性系数.
        @version NIIEngine 3.0.0
        */
        inline NIIf getLinear() const           { return mLinear; }

        /** 设置点距离衰减二次系数
        @version NIIEngine 3.0.0
        */
        inline void setQuadratic(NIIf f)        { mQuadratic = f; }

        /** 返回点距离衰减二次系数.
        @version NIIEngine 3.0.0
        */
        inline NIIf getQuadratic() const        { return mQuadratic; }

        /** 上传到着色程序参数中
        @note {float(mSize), float(mMinSize), float(mMaxSize), float(mLineWidth), float(mConstant), float(mLinear),
            float(mQuadratic), int(mMark)}
        @version NIIEngine 5.0.0
        */
        void read(GpuProgramParam * out, NCount memoft) const;

        /** 从着色程序参数中设置
        @note {float(mSize), float(mMinSize), float(mMaxSize), float(mLineWidth), float(mConstant), float(mLinear),
            float(mQuadratic), int(mMark)}
        @version NIIEngine 5.0.0
        */
        void write(const GpuProgramParam * in, NCount memoft);

        /// 渲染系统默认设置
        static const ShaderChPoint Default;
    public:
        NIIf mSize;
        NIIf mMinSize;
        NIIf mMaxSize;
        NIIf mLineWidth;
        NIIf mConstant;
        NIIf mLinear;
        NIIf mQuadratic;
    private:
        Nmark mMark;
    };

    /** 定义灯光类型
    @version NIIEngine 3.0.0
    */
    enum LightType
    {
        LT_POINT            = 0,    ///< 点光
        LT_SPOT             = 2,    ///< 聚光
        LT_DIRECTIONAL      = 1,    ///< 方向光
        LT_FRAME            = 3,    ///< 像素光/假光(光晕)
        LT_REVISE           = 4,    ///< 数值修正光/假光(法线纹理)
        LT_NONE             = 5     ///< 无
    };

    /** 三角形面序
    @remark 基础概念,普通的opengl/dx含概,用着色顶点的方向确定一个面的正面(着光面)/反面(背光面)
    @version NIIEngine 3.0.0
    */
    enum CullingMode
    {
        CM_None = 1,            ///< 渲染任何接收的三角形
        CM_Clockwise = 2,       ///< 在视口中以顺时针方向列出的顶点(默认).
        CM_Anticlockwise = 3    ///< 在视口中以逆时针方向列出的顶点
    };

    /** 基于顶点法线手动拣选模式.
    @remarks
        这个设置应用在发送到硬件API如何软件拣选三角形这个拣选模式由场景管理器
        使用-通常处理大数量的固定世界几何,平面化的(软件拣选可移动几何变量消耗大).
    */
    enum SysCullingMode
    {
        SCM_NONE = 1,           ///< 不拣选,直接通过
        SCM_BACK = 2,           ///< 拣选三角形,法线指向远离相机的(默认).
        SCM_FRONT = 3           ///< 拣选三角形,法线指向接近摄象机.
    };

    /** 光体计算模式.
    @remark
        基础概念,普通的opengl/dx已经有api能实现,一般是用SO_PHONG,但很明显需要更多计算.如果
        在需要灯光而又并不是物体本身的颜色,如水中倒影用SO_GOURAUD,
    @version NIIEngine 3.0.0
    */
    enum ShadeMode
    {
        SM_NONE,                ///<
        SM_FLAT,                ///<
        SM_GOURAUD,             ///<
        SM_PHONG                ///<
    };

    /** 像素着色模式.
    @remark
        基础概念,普通的opengl/dx已经有api能实现,一般使用PM_SOLID ,或许在物体被选中时再用
        PM_LINE 渲染一遍混合
    @version NIIEngine 3.0.0
    */
    enum PolygonMode
    {
        PM_POINTS = 1,          ///< 渲染点
        PM_LINE = 2,            ///< 渲染线框
        PM_SOLID = 3            ///< 渲染实体
    };

    /** 着色融混阶段
    @remark
        高级概念,需要不断重绘同一个事物,通过融混合成由不同着色产生的结果,很明显需要更多计算.
        使用 Instancing 顶点能减轻效率问题(和显卡有关),或使用多通道和着色程序配合.
    @version NIIEngine 3.0.0
    */
    enum ShaderStep
    {
        SS_Global,              ///< 渲染主环境阶段
        SS_Global_Light,        ///< 渲染灯光阶段(主)
        SS_Detail_Light,        ///< 渲染灯光阶段(细)
        SS_Revise_Light,        ///< 修正灯光阶段(如法线纹理)
        SS_Frame,               ///< 对象当前帧缓存
        SS_Shadow,              ///< 渲染阴影阶段
        SS_Revise_Shadow,       ///<
        SS_Decal,               ///< 渲染贴花阶段
        SS_Revise_Decal,        ///<
        SS_Unknow               ///< 不确定阶段
    };

    /** 单次着色渲染
    @remark
        图形驱动都是面向过程的,这个类的目的则是期望它们能变的更对象化一点
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderCh : public ExtData, public ShaderAlloc
    {
        friend class ShaderFusion;
        friend class MaterialManager;
    public:
        /** 设置操作类型
        @version NIIEngine 3.0.0
        */
        enum SetOperatorType
        {
            SOT_Set = 0x01,
            SOT_Ref = 0x02
        };

        struct IndexLess
        {
            bool _EngineAPI operator()(const ShaderCh * a, const ShaderCh * b) const;
        };
    public:
        /** 普通构造函数
        @param[in] sf 父对象
        @param[in] index 使用的内部句柄
        @param[in] fusion 自动通道(内部创建的通道)
        @version NIIEngine 3.0.0
        */
        ShaderCh(ShaderFusion * sf, Nidx index, bool fusion = false);

        /** 普通构造函数
        @param[in] sf 父对象
        @param[in] index 使用的内部句柄
        @param[in] o 被复制的对象
        @param[in] fusion 自动通道(内部创建的通道)
        @version NIIEngine 3.0.0
        */
        ShaderCh(ShaderFusion * sf, Nidx index, const ShaderCh & o, bool fusion = false);

        virtual ~ShaderCh();

        ShaderCh & operator= (const ShaderCh & o);
        ShaderCh & operator< (const ShaderCh & o);
        
        /** 添加引用
        @version NIIEngine 3.0.0
        */
        void addRef(ShaderFusion * fusion);
        
        /** 移去引用
        @version NIIEngine 3.0.0
        */
        void removeRef(ShaderFusion * fusion);

        /** 所属渲染集
        @version NIIEngine 3.0.0
        */
        ShaderFusion * getParent() const                { return mParent; }

        /** 局部索引
        @version NIIEngine 3.0.0
        */
        Nidx getIndex() const                           { return mIndex; }

        /** 设置的名字
        @verison NIIEngine 3.0.0
        */
        void setName(const String & name)               { mName = name; }

        /** 获取的名字
        @version NIIEngine 3.0.0
        */
        const String & getName() const                  { return mName; }

        /** 获取资源群组
        @version NIIEngine 3.0.0
        */
        GroupID getResourceGroup() const;

        /** 是否已经加载
        @version NIIEngine 3.0.0
        */
        bool isLoad() const;

        /** 是否混合出来的通道
        @version NIIEngine 3.0.0
        */
        bool isFusion() const                           { return mMark & SB_Fusion; }

        /** 启用基础设置
        @version NIIEngine 3.0.0
        */
        void enable(ShaderBase mark);

        /** 禁用基础设置
        @version NIIEngine 3.0.0
        */
        void disable(ShaderBase mark);

        /** 设置颜色应用
        @param[in] refIn
        @version NIIEngine 3.0.0
        */
        ShaderChColour * setColour(SetOperatorType type, const ShaderChColour * refIn = 0);

        /** 获取颜色应用
        @note 不能设置返回值内容
        @version NIIEngine 3.0.0
        */
        inline const ShaderChColour & getColour() const { return *mColour; }

        /** 设置点应用
        @remark 扩展事物
        @param[in] refIn
        @version NIIEngine 3.0.0
        */
        ShaderChPoint * setPoint(SetOperatorType type, const ShaderChPoint * refIn = 0);

        /** 获取点应用
        @note 不能设置返回值内容
        @version NIIEngine 3.0.0
        */
        inline const ShaderChPoint & getPoint() const   { return *mPoint; }

        /** 设置雾应用
        @remark 扩展事物
        @param[in] refIn
        @version NIIEngine 3.0.0
        */
        ShaderChFog * setFog(SetOperatorType type, const ShaderChFog * refIn = 0);

        /** 获取雾应用
        @note 不能设置返回值内容
        @version NIIEngine 3.0.0
        */
        inline const ShaderChFog & getFog() const       { return *mFog; }

        /** 设置像素混合应用
        @remark 基础事物
        @param[in] refIn
        @version NIIEngine 3.0.0
        */
        ShaderChBlend * setBlend(SetOperatorType type, const ShaderChBlend * refIn = 0);

        /** 获取像素混合应用
        @note 不能设置返回值内容
        @version NIIEngine 3.0.0
        */
        inline const ShaderChBlend & getBlend() const   { return *mBlend; }

        /** 设置深度测试应用
        @remark 基础事物
        @param[in] refIn
        @version NIIEngine 3.0.0
        */
        ShaderChStencil * setStencil(SetOperatorType type, const ShaderChStencil * refIn = 0) ;

        /** 获取深度测试应用
        @note 不能设置返回值内容
        @version NIIEngine 3.0.0
        */
        inline const ShaderChStencil & getStencil() const   { return *mStencil; }

        /** 获取纹理应用
        @remark 基础事物
        @param[in] refIn
        @version NIIEngine 3.0.0
        */
        ShaderChTexture * setTexture(SetOperatorType type, const ShaderChTexture * refIn = 0);

        /** 获取纹理应用
        @note 不能设置返回值内容
        @version NIIEngine 3.0.0
        */
        inline const ShaderChTexture & getTexture() const { return *mTexture; }

        /** 获取着色程序应用
        @remark 扩展事物
        @param[in] refIn
        @version NIIEngine 3.0.0
        */
        ShaderChProgram * setProgram(SetOperatorType type, const ShaderChProgram * refIn = 0);

        /** 获取着色程序应用
        @note 不能设置返回值内容
        @version NIIEngine 3.0.0
        */
        inline const ShaderChProgram & getProgram() const { return *mProgram; }

        /** 设置颜色写入
        @remark 基础事物
        @param[in] mark SB_Colour_X 的一个或多个组合
        @version NIIEngine 3.0.0
        */
        void setColourWrite(Nmark mark)                 { mMark |= (mark & SB_Colour_RGBA);}

        /** 获取颜色写入
        @remark 基础事物
        @version NIIEngine 3.0.0
        */
        Nmark getColourWrite() const                    { return mMark & SB_Colour_RGBA; }

        /** 设置是否启用深度排序.
        @remark
        @version NIIEngine 3.0.0
        */
        inline void setDepthSort(bool b)                { mDepthSort = b;}

        /** 返回是否启用深度排序
        @version NIIEngine 3.0.0
        */
        inline bool isDepthSort() const                 { return mDepthSort;}

        /** 设置是否强制深度排序
        @param[in] enabled
        @version NIIEngine 3.0.0
        */
        inline void setForceDepthSort(bool b)           { mForceDepthSort = b;}

        /** 返回是否强制深度排序
        @version nIIEngine 3.0.0
        */
        inline bool isForceDepthSort() const            { return mForceDepthSort;}

        /** 设置是否自动退化
        @param[in] b
        @version NIIEngine 3.0.0
        */
        inline void setAutoReduce(bool b)               { mAutoReduce = b;}

        /** 获取是否自动退化
        @version NIIEngine 3.0.0
        */
        inline bool getAutoReduce() const               { return mAutoReduce;}

        /** 设置光体模式
        @version NIIEngine 3.0.0
        */
        inline void setShadeMode(ShadeMode m)           { mShaderMode = m; }

        /** 获取光体模式
        @version NIIEngine 3.0.0
        */
        inline ShadeMode getShaderMode() const          { return mShaderMode;}

        /** 设置多边形模式
        @version NIIEngine 3.0.0
        */
        inline void setPolygonMode(PolygonMode m)       { mPolygonMode = m; }

        /** 获取多边形模式
        @version NIIEngine 3.0.0
        */
        inline PolygonMode getPolygonMode() const       { return mPolygonMode;}

        /** 设置渲染通道基于三角形面序.
        @version NIIEngine 3.0.0
        */
        inline void setCullingMode(CullingMode mode)    { mCullMode = mode; }

        /** 获取渲染通道基于三角形面序.
        @version NIIEngine 3.0.0
        */
        inline CullingMode getCullingMode() const       { return mCullMode; }

        /** 设置系统拣选模式
        @version NIIEngine 3.0.0
        */
        inline void setSysCullingMode(SysCullingMode mode){ mSysCullMode = mode;}

        /** 获取系统拣选模式
        @version NIIEngine 3.0.0
        */
        inline SysCullingMode getSysCullingMode() const { return mSysCullMode;}

        /** 设置是否动态单位化法线
        @version NIIEngine 3.0.0
        */
        void setUnitNormals(bool b)                     { b ? mMark = mMark | SB_UnitNormals : mMark = mMark & ~SB_UnitNormals; }

        /** 获取是否动态单位化法线
        @version NIIEngine 3.0.0
        */
        bool isUnitNormals() const                      { return mMark & SB_UnitNormals; }

        /** 设置是否启用动态照明
        @param[in] b
        @version NIIEngine 3.0.0
        */
        void setLight(bool b)                           { b? mMark |= SB_Light : mMark &= ~SB_Light; }

        /** 是否启用动态照明
        @version NIIEngine 3.0.0
        */
        bool isLight() const                            { return mMark & SB_Light; }

        /** 设置是否启用雾设置
        @version NIIEngine 3.0.0
        */
        void setFogValid(bool b)                        { b ? mMark |= SB_FogValid : mMark &= ~SB_FogValid; }

        /** 是否启用雾设置
        @version NIIEngine 3.0.0
        */
        bool isFogValid() const                         { return mMark & SB_FogValid; }

        /** 设置渲染通道使用的最大灯光数量
        @version NIIEngine 3.0.0
        */
        inline void setLightMax(Nui16 count)            { mMaxLightCount = count; }

        /** 获取这条通道使用的最大灯光数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getLightMax() const                { return mMaxLightCount; }

        /** 设置渲染通道开始的灯光列表的索引
        @version NIIEngine 3.0.0
        */
        inline void setLightBegin(Nui16 i)              { mLightBegin = i; }

        /** 获取这条通道开始的灯光列表的索引.
        @version NIIEngine 3.0.0
        */
        inline Nui16 getLightBegin() const              { return mLightBegin; }

        /** 设置灯光掩码
        @param[in] mark 匹配码
        @version NIIEngine 3.0.0
        */
        inline void setLightMark(Nmark mark)            { mLightMask = mark; }

        /** 获取灯光掩码
        @version NIIEngine 3.0.0
        */
        inline Nmark getLightMark() const               { return mLightMask; }

        /** 设置多通道渲染次数
        @remark 用于着色程序控制通路时用
        @version NIIEngine 3.0.0
        */
        inline void setRenderCount(Nui16 c)             { mRenderCount = c; }

        /** 获取多通道渲染次数
        @remark 用于着色程序控制通路时用
        @version NIIEngine 3.0.0
        */
        inline Nui16 getRenderCount() const             { return mRenderCount; }

        /** 设置是否每个灯光渲染执行一次渲染
        @remark 用于渲染阴影到模板缓存或纹理时使用
        @version NIIEngine 3.0.0
        */
        inline void setEachLight(bool b)                { mEachLight = b; }

        /** 获取是否每个灯光渲染执行一次渲染
        @remark 用于渲染阴影到模板缓存或纹理时使用
        @version NIIEngine 3.0.0
        */
        inline bool isEachLight() const                 { return mEachLight; }

        /** 设置灯光类型限制
        @version NIIEngine 3.0.0
        */
        inline void setLightTypeLimit(LightType type)   { mOnlyLightType = type; }

        /** 获取灯光类型限制
        @version NIIEngine 3.0.0
        */
        inline LightType getLightTypeLimit() const      { return mOnlyLightType; }

        /** 灯光数量限制
        @version NIIEngine 3.0.0
        */
        inline void setLightRenderLimit(Nui16 c)        { mLightCountLimit = c; }

        /** 如果启用灯光迭代,每个迭代的灯光数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getLightRenderLimit() const        { return mLightCountLimit; }

        /** 设置灯光是否矩形剪切
        @version NIIEngine 3.0.0
        */
        void setLightClip(bool b)                       { b ? mMark = mMark | SB_LightClip : mMark = mMark & ~SB_LightClip; }

        /** 获取灯光是否矩形剪切
        @version NIIEngine 3.0.0
        */
        bool isLightClip() const                        { return mMark & SB_LightClip; }

        /** 设置灯光是否应用面裁剪
        @version NIIEngine 3.0.0
        */
        void setLightClipPlane(bool b)                  { b ? mMark = mMark | SB_LightClipPlane : mMark = mMark & ~SB_LightClipPlane; }

        /** 获取灯光是否应用面裁剪
        @version NIIEngine 3.0.0
        */
        bool isLightClipPlane() const                   { return mMark & SB_LightClipPlane;}

        /** 设置渲染阶段
        @version NIIEngine 3.0.0
        */
        inline void setShaderStep(ShaderStep ss)        { mShaderStep = ss; }

        /** 获取渲染阶段
        @version NIIEngine 3.0.0
        */
        inline ShaderStep getShaderStep() const         { return mShaderStep; }

        /** 状态掩码
        @version NIIEngine 3.0.0 高级api
        */
        Nmark getMark() const                           { return mMark; }
    public:
        /** 状态/性质变化了
        @version NIIEngine 3.0.0 高级api
        */
        void notify();

        /** 进入加载策略
        @version NIIEngine 3.0.0 高级api
        */
        void plan();

        /** 移出加载策略
        @version NIIEngine 3.0.0 高级api
        */
        void unplan();

        /** 加载
        @version NIIEngine 3.0.0 高级api
        */
        void load();

        /** 卸载
        @version NIIEngine 3.0.0 高级api
        */
        void unload();
    protected:
        ShaderFusion * mParent;
        Nidx mIndex;
        String mName;

        ShaderChColour * mColour;       ///< 基础颜色
        ShaderChPoint * mPoint;         ///< 扩展点设置
        ShaderChFog * mFog;             ///< 扩展雾设置
        ShaderChBlend * mBlend;         ///< 基础混合
        ShaderChStencil * mStencil;     ///< 基础深度测试
        ShaderChTexture * mTexture;     ///< 基础纹理
        ShaderChProgram * mProgram;     ///< 基础着色程序

        CullingMode mCullMode;          ///< 面拣选模式(gpu)
        SysCullingMode mSysCullMode;    ///< 面拣选模式(cpu)
        ShadeMode mShaderMode;          ///< 像素着色模式
        PolygonMode mPolygonMode;       ///< 几何渲染模式
        ShaderStep mShaderStep;         ///< 渲染阶段
        LightType mOnlyLightType;       ///< 灯光类型,多通道模式只能用着色程序改变状态
        Nmark mLightMask;               ///< 灯光掩码
        Nmark mMark;                    ///< 状态掩码

        Nui16 mLightBegin;
        Nui16 mMaxLightCount;
        Nui16 mLightCountLimit;
        Nui16 mRenderCount;
        bool mEachLight;
        bool mDepthSort;
        bool mForceDepthSort;
        bool mAutoReduce;
        bool mAuto;
    };
    
    /**
    @version NIIEngine 6.0.0
    */
    enum ShadowType
    {
        ST_Normal   = 0,
        ST_Cast     = 1,
        ST_Receive  = 2,
        ST_Count    = 3
    };
    
    /**
    @version NIIEngine 6.0.0
    */
    class _EngineAPI ShaderChMaterial : public ShaderAlloc
    {
        friend class RenderQueue;
    public:
        ShaderChMaterial(Nid name, RenderPattern * rp, ShaderChStencil * stencil, ShaderChBlend * blend,
            const PropertyData & params);
        virtual ~ShaderChMaterial();

        /**
        @version NIIEngine 6.0.0
        */
        Nid getID() const                   { return mID; }
        
        /**
        @version NIIEngine 6.0.0
        */
        void setName(const String & name )  { mName = name; }

        /**
        @version NIIEngine 6.0.0
        */
        const String & getName() const      { return mName; }

        /**
        @version NIIEngine 6.0.0
        */
        RenderPattern * getParent() const   { return mParent; }

        /**
        @version NIIEngine 6.0.0
        */
        void setStencil(ShaderChStencil * stencil, ShadowType type = ShadowType::ST_Normal);

        /**
        @version NIIEngine 6.0.0
        */
        const ShaderChStencil * getStencil(ShadowType type = ShadowType::ST_Normal) const{ return mDepth[type]; }

        /**
        @version NIIEngine 6.0.0
        */
        void setBlend(ShaderChBlend * blend, ShadowType type = ShadowType::ST_Normal);

        /**
        @version NIIEngine 6.0.0
        */
        const ShaderChBlend * getBlend(ShadowType type = ShadowType::ST_Normal) const    { return mBlend[type]; }

        /**
        @version NIIEngine 6.0.0
        */
        virtual void setAlphaTest(CmpMode mode, bool onlyInCast = false);

        /**
        @version NIIEngine 6.0.0
        */
        inline CmpMode getAlphaTest() const             { return mAlphaTestCmp; }

        /**
        @version NIIEngine 6.0.0
        */
        bool isAlphaTestOnlyInCast() const              { return mAlphaTestOnlyInCast; }

        /**
        @version NIIEngine 6.0.0
        */
        virtual void setAlphaTestValue(float value);

        /**
        @version NIIEngine 6.0.0
        */
        float getAlphaTestValue() const                 { return mAlphaTestValue; }

        /**
        @version NIIEngine 6.0.0
        */
        void addRef(GeometryObj * obj);

        /**
        @version NIIEngine 6.0.0
        */
        void removeRef(GeometryObj * obj);

        /**
        @version NIIEngine 6.0.0
        */
        const GeometryObjList & getRefList() const      { return mRefList; }

        /**
        @version NIIEngine 6.0.0
        */
        virtual bool isBackFaceInCast() const;

        /**
        @version NIIEngine 6.0.0
        */
        virtual const Colour & getDiffuseColour() const;

        /**
        @version NIIEngine 6.0.0
        */
        virtual const Colour & getEmissiveColour() const;
        
        /**
        @version NIIEngine 6.0.0
        */
        ShaderChMaterial * clone(const String & name) const;
    protected:
        /**
        @version NIIEngine 6.0.0
        */
        void queueRef(bool onlyUnRef = false);

        /**
        @version NIIEngine 6.0.0
        */
        virtual void init();
        
        /**
        @version NIIEngine 6.0.0
        */
        virtual void cloneImpl(ShaderChMaterial * o) const;
    protected:
        Nid mID;
        String mName;
        RenderPatternType mType;
        RenderPattern * mParent;
        GeometryObjList mRefList;
        ShaderChStencil * mDepth[ST_Count];
        ShaderChBlend * mBlend[ST_Count];
        CmpMode mAlphaTestCmp;
        float mShadowConstantBias;
        float mAlphaTestValue;
        Nui16 mHash[4];
        bool mAutoLoadTexture;
        bool mAlphaTestOnlyInCast;
    };
    
    /**
    @version NIIEngine 5.0.0
    */
    class _EngineAPI SampleObject
    {
    public:
        SampleObject & operator = (const SampleObject & o)
        {
            memcpy(this, &o, sizeof(SampleObject));
            return *this;
        }

        bool operator == (const SampleObject & o) const
        {
            return !memcmp(this, &o, sizeof(SampleObject));
        }

        bool operator != (const SampleObject & o) const
        {
            return !(*this == o);
        }

        bool operator < (const SampleObject & o) const
        {
            return memcmp(this, &o, sizeof(SampleObject)) < 0;
        }
    public:
        PixelFormat mColourPF[NII_MAX_RenderTarget];
        PixelFormat mDepthPF;
        ShaderChStencil * mStencil;
        ShaderChColour * mColour;
        SampleType mSampleType;
        Nui32 mDevID;
    };

    /**
    @version NIIEngine 5.0.0
    */
    class _EngineAPI RenderStateObject
    {
    public:
        virtual ~RenderStateObject(){}
        void init();

        RenderStateObject & operator = (const RenderStateObject & o);

        /**
        @version NIIEngine 5.0.0
        */
        bool equalModel(const RenderStateObject & o) const;
        
        /**
        @version NIIEngine 5.0.0
        */
        int lesslModel(const RenderStateObject & o) const;

        /**
        @version NIIEngine 5.0.0
        */
        bool equalRender(const RenderStateObject & o) const;
        
        /**
        @version NIIEngine 5.0.0
        */
        bool lessRender(const RenderStateObject & o) const;
        
        /** 
        @version NIIEngine 5.0.0
        */
        void initStencil();
    public:
        SampleObject mSampleObject;
        VertexUnitListList mUnitsList;
        GpuProgram * mVS;
        GpuProgram * mGS;
        GpuProgram * mTS;
        GpuProgram * mDS;
        GpuProgram * mFS;
        OperationType mType;
        ShaderChStencil * mStencil;
        ShaderChBlend * mBlend;
        Nmark mRenderMark;
    };

    /**
    @version NIIEngine 5.0.0
    */
    class _EngineAPI ComputeStateObject
    {
    public:
        virtual ~ComputeStateObject(){}
        void init();
    public:
        GpuProgram * mCS;
        GpuProgramParam * mParam;
        NCount mCeilCount[3];
        NCount mGroupCount[3];
    };
}
#endif