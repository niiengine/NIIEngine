/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-7

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

#ifndef _NII_SHADER_CH_H_
#define _NII_SHADER_CH_H_

#include "NiiPreInclude.h"
#include "NiiColour.h"
#include "NiiCommon.h"
#include "NiiShaderChProgram.h"
#include "NiiShaderChTexture.h"
#include "NiiExtData.h"

namespace NII
{
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

    /** 渲染通路中的基本颜色
    @remark 适合基本渲染系统
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChColour : public ShaderAlloc
    {
    public:
        ShaderChColour();
        ShaderChColour(const ShaderChColour & o);
        ShaderChColour(const Colour & ambient, const Colour & diffuse, const Colour & specular, NIIf shinin);
        ShaderChColour(const Colour & ambient, const Colour & diffuse, const Colour & specular, const Colour & emissive, 
            NIIf shinin, Nmark mark);

        bool operator ==(const ShaderChColour & o) const;

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

        /** 上传到着色程序参数中
        @version NIIEngine 5.0.0
        */
        void upload(GpuProgramParam * param) const;

        /** 从着色程序参数中设置
        @version NIIEngine 5.0.0
        */
        void download(const GpuProgramParam * param);
    public:
        Colour mAmbient;
        Colour mDiffuse;
        Colour mSpecular;
        Colour mEmissive;
        NIIf mShininess;
    private:
        Nmark mMark;
    };
    
    /** 渲染通路中的基本透明测试
    @remark 适合基本渲染系统
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChAlpha : public ShaderAlloc
    {
    public:
        ShaderChAlpha();
        ShaderChAlpha(const ShaderChAlpha & o);
        ShaderChAlpha(CmpMode func, Nui8 value, bool alphaCoverage);

        bool operator == (const ShaderChAlpha & o) const;

        /** 设置是否使用将绘制的片元的alpha来计算最终的覆盖比率
        @remark
            多重采样缓冲区在默认情况下使用片段的RGB值,并不包括颜色的alpha成分.
            所以需要特殊设置
        @note 值会被最后一次设置代替
        @version NIIEngine 3.0.0
        */
        void setAlphaCoverage(bool b);

        /** 返回是否使用将绘制的片元的alpha来计算最终的覆盖比率
        @remark
            多重采样缓冲区在默认情况下使用片段的RGB值,并不包括颜色的alpha成分.
            所以需要特殊设置
        @note 值会被最后一次设置代替
        @version NIIEngine 3.0.0
        */
        bool isAlphaCoverage() const;

        /** 设置是否使用将绘制的片元的alpha值设置为最大值(1)来计算覆盖比率
        @remark
            多重采样缓冲区在默认情况下使用片段的RGB值,并不包括颜色的alpha成分.
            所以需要特殊设置
        @note 值会被最后一次设置代替
        @version NIIEngine 3.0.0
        */
        void setOneAlphaCoverage(bool b);

        /** 返回是否使用将绘制的片元的alpha值设置为最大值(1)来计算覆盖比率
        @remark
            多重采样缓冲区在默认情况下使用片段的RGB值,并不包括颜色的alpha成分.
            所以需要特殊设置
        @note 值将会被最后一次设置代替
        @version NIIEngine 3.0.0
        */
        bool isOneAlphaCoverage() const;

        /** 设置透明度拒绝模式.
        @version NIIEngine 3.0.0
        */
        inline void setAlphaReject(CmpMode mode)    { mRejectMode = mode; }

        /** 获取透明度拒绝模式.
        @version NIIEngine 3.0.0
        */
        inline CmpMode getAlphaReject() const       { return mRejectMode; }

        /** 获取透明度拒绝值.
        @param[in] val 8位值
        @version NIIEngine 3.0.0
        */
        inline void setValue(Nui8 val)              { mMark |= (Nmark)val; }

        /** 获取透明度拒绝值.
        @return 8位值
        @version NIIEngine 3.0.0
        */
        inline Nui8 getValue() const                { return (Nui8)(mMark & 0xFF); }

        /** 上传到着色程序参数中
        @version NIIEngine 5.0.0
        */
        void upload(GpuProgramParam * param) const;

        /** 从着色程序参数中设置
        @version NIIEngine 5.0.0
        */
        void download(const GpuProgramParam * param);
    public:
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
    class _EngineAPI ShaderChBlend : public ShaderAlloc
    {
    public:
        ShaderChBlend();
        ShaderChBlend(const ShaderChBlend & o);
        ShaderChBlend(ColourFactor src, ColourFactor dst, ColourBlendMode mode);
        ShaderChBlend(ColourFactor src, ColourFactor dst, ColourFactor asrc,
            ColourFactor adst, ColourBlendMode mode);
        ShaderChBlend(ColourFactor src, ColourFactor dst, ColourFactor asrc,
            ColourFactor adst, ColourBlendMode mode, ColourBlendMode amode);

        bool operator ==(const ShaderChBlend & o) const;

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
        inline ColourBlendMode getBlendMode() const { return mMode; }

        /** 获取当前的透明混合操作
        @version NIIEngine 3.0.0
        */
        inline ColourBlendMode getAlphaBlendMode() const { return mAlphaMode;}

        /** 设置混合因子
        @param[in] src 将绘制的RGB
        @param[in] dst 帧缓存的RGB
        @param[in] asrc 将绘制的A
        @param[in] adst 帧缓存的A
        @version NIIEngine 3.0.0 高级api
        */
        void setBlend(ColourFactor src, ColourFactor dst, ColourFactor asrc, ColourFactor adst);

        /** 获取这个材质来源混合因子
        @version NIIEngine 3.0.0
        */
        inline ColourFactor getSrcFactor() const { return mSrcFactor; }

        /** 获取这个材质目标混合因子
        @version NIIEngine 3.0.0
        */
        inline ColourFactor getDstFactor() const { return mDstFactor; }

        /** 获取这个材质透明来源混合因子
        @version NIIEngine 3.0.0
        */
        inline ColourFactor getSrcAlphaFactor() const { return mSrcAlphaFactor; }

        /** 获取这个材质透明目标混合因子
        @version NIIEngine 3.0.0
        */
        inline ColourFactor getDstAlphaFactor() const { return mDstAlphaFactor; }

        /** 是否产生覆盖效应
        @version NIIEngine 3.0.0
        */
        bool isReplace() const;

        /** 是否产生覆盖效应(分离透明通道设置)
        @version NIIEngine 3.0.0
        */
        bool isPartReplace() const;

        /** 是否产生透明度效应
        @version NIIEngine3.0.0
        */
        bool isTransparent() const;

        /** 是否分离透明通道设置
        @version NIIEngine 3.0.0
        */
        bool isPartAlpha() const;

        /** 是否分离透明通道混合设置
        @version NIIEngine 3.0.0
        */
        bool isPartAlphaBlendMode() const;

        /** 上传到着色程序参数中
        @version NIIEngine 5.0.0
        */
        void upload(GpuProgramParam * param) const;

        /** 从着色程序参数中设置
        @version NIIEngine 5.0.0
        */
        void download(const GpuProgramParam * param);
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
    
    /** 渲染通路中的基本深度缓存
    @remark 适合基本渲染系统
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChDepth : public ShaderAlloc
    {
    public:
        ShaderChDepth();    
        ShaderChDepth(const ShaderChDepth & o); 
        ShaderChDepth(bool check, CmpMode mode, bool write, NIIf bias = 0.0f, NIIf scaleslopebias = 0.0f);

        bool operator == (const ShaderChDepth & o) const;

        /** 设置测试的比较深度.
        @version NIIEngine 3.0.0
        */
        void setCmp(CmpMode mode) { mCmpMode = mode; }

        /** 获取测试的比较深度
        @version NIIEngine 3.0.0
        */
        CmpMode getCmp() const { return mCmpMode; }

        /** 设置缓存测试
        @version NIIEngine 3.0.0
        */
        void setCheck(bool b);

        /** 是否缓存测试
        @version NIIEngine 3.0.0
        */
        bool isCheck() const;

        /** 设置缓存写入
        @version NIIEngine 3.0.0
        */
        void setWrite(bool b);

        /** 是否缓存写入
        @version NIIEngine 3.0.0
        */
        bool isWrite() const;

        /** 设置偏量常数
        @version NIIEngine 3.0.0
        */
        inline void setBiasConstant(NIIf f) { mBias = f; }

        /** 获取偏量常数
        @remark 基本渲染系统中存在
        @version NIIEngine 3.0.0
        */
        inline NIIf getBiasConstant() const { return mBias; }

        /** 设置偏量常数因子
        @version NIIEngine 3.0.0
        */
        inline void setBiasConstantFactor(NIIf f) { mBiasFactor = f; }

        /** 获取偏量常数因子
        @version NIIEngine 3.0.0
        */
        inline NIIf getBiasConstantFactor() const { return mBiasFactor; }

        /** 设置斜率补偿
        @param[in] f
        @note 旧硬件可能不支持
        @version NIIEngine 3.0.0
        */
        inline void setBiasSlopeScale(NIIf f) { mSlopeScaleBias = f; }

        /** 获取斜率补偿
        @remark 基本渲染系统中存在
        @version NIIEngine 3.0.0
        */
        inline NIIf getBiasSlopeScale() const { return mSlopeScaleBias; }

        /** 上传到着色程序参数中
        @version NIIEngine 5.0.0
        */
        void upload(GpuProgramParam * param) const;

        /** 从着色程序参数中设置
        @version NIIEngine 5.0.0
        */
        void download(const GpuProgramParam * param);
    public:
        CmpMode mCmpMode;           ///< 深度比较函数
        NIIf mBias;                 ///< 深度缓存设置(多通道的基础值)
        NIIf mBiasFactor;           ///< 深度缓存设置(多通道的基础值系数)
        NIIf mSlopeScaleBias;       ///< 深度斜率补偿
    private:
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
        SOT_INCR_WRAP,          ///< 用1增量增加模版值,当值超出了最大范围调转到0
        SOT_DECR_WRAP           ///< 用1减量减少模版值,超出范围跳转到0
    };

    class _EngineAPI ShaderChStencil : public ShaderAlloc
    {
    public:
        ShaderChStencil();
        ShaderChStencil(const ShaderChStencil & o);
        ShaderChStencil(bool twoside, Nui32 mask = 0xFFFFFFFF, Nui32 cmpmask = 0xFFFFFFFF,
            CmpMode func = CPM_ALWAYS_PASS, Nui32 value = 0, StencilOpType stencilfail = SOT_KEEP,
            StencilOpType depthfail = SOT_KEEP, StencilOpType pass = SOT_KEEP);

        bool operator == (const ShaderChStencil & o) const;

        /** 设置是否双面模板测试
        @version NIIEngine 3.0.0
        */
        inline void setBothSide(bool s)             { mBothSide = false; }

        /** 获取是否双面模板测试
        @version NIIEngine 3.0.0
        */
        inline bool isBothSide() const              { return mBothSide; }

        /** 设置模板测试掩码
        @version NIIEngine 3.0.0
        */
        inline void setTestMask(Nui32 mask)         { mTestMark = mask; }

        /** 获取模板测试掩码
        @version NIIEngine 3.0.0
        */
        inline Nui32 getTestMask() const            { return mTestMark; }

        /** 设置模板比较掩码
        @version NIIEngine 3.0.0
        */
        inline void setCompressMask(Nui32 mask)     { mCmpMark = mask; }

        /** 获取模板比较掩码
        @version NIIEngine 3.0.0
        */
        inline Nui32 getCompressMask() const         { return mCmpMark; }

        /** 设置模板比较值
        @version NIIEngine 3.0.0
        */
        inline void setCompressValue(Nui32 value)   { mCmpValue = value; }

        /** 设置模板比较值
        @version NIIEngine 3.0.0
        */
        inline Nui32 getCompressValue() const       { return mCmpValue; }

        /** 设置模板比较失败操作
        @version NIIEngien 3.0.0
        */
        inline void setStencilFailOp(StencilOpType op)  { mStencilFailOp = op; }

        /** 获取模板比较失败操作
        @version NIIEngien 3.0.0
        */
        inline StencilOpType getStencilFailOp() const   { return mStencilFailOp; }

        /** 设置深度比较失败操作
        @version NIIEngien 3.0.0
        */
        inline void setDepthFailOp(StencilOpType op)    { mDepthFailOp = op; }

        /** 获取深度比较失败操作
        @version NIIEngien 3.0.0
        */
        inline StencilOpType getDepthFailOp() const     { return mDepthFailOp; }

        /** 设置模板比较通过操作
        @version NIIEngien 3.0.0
        */
        inline void setTestPassOp(StencilOpType op)     { mTestPassOp = op; }

        /** 获取模板比较通过操作
        @version NIIEngien 3.0.0
        */
        inline StencilOpType getTestPassOp() const      { return mTestPassOp; }

        /** 上传到着色程序参数中
        @version NIIEngine 5.0.0
        */
        void upload(GpuProgramParam * param) const;

        /** 从着色程序参数中设置
        @version NIIEngine 5.0.0
        */
        void download(const GpuProgramParam * param);
    public:
        bool mBothSide;
        Nui32 mTestMark;
        Nui32 mCmpMark;
        CmpMode mFunc;
        Nui32 mCmpValue;
        StencilOpType mStencilFailOp;
        StencilOpType mDepthFailOp;
        StencilOpType mTestPassOp;
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
        FM_EXP,     ///< 普通渲染系统中存在
        FM_EXP2,    ///< 普通渲染系统中存在
        FM_LINEAR,  ///< 普通渲染系统中存在
        FM_Count    ///< 尽量使用现有的,因为被渲染系统约束,否则只能用粒子系统或片段着色程序
    };

    /** 渲染通路中的基本雾
    @remark 适合基本渲染系统
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChFog : public ShaderAlloc
    {
    public:
        ShaderChFog();
        ShaderChFog(const ShaderChFog & o);
        ShaderChFog(FogMode m, const Colour & c, NIIf density, NIIf start, NIIf end);

        bool operator ==(const ShaderChFog & o) const;
        
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
        @version NIIEngine 5.0.0
        */
        void upload(GpuProgramParam * param) const;

        /** 从着色程序参数中设置
        @version NIIEngine 5.0.0
        */
        void download(const GpuProgramParam * param);
    public:
        static const ShaderChFog White;
        static const ShaderChFog Red;
        static const ShaderChFog Green;
        static const ShaderChFog Yellow;
        static const ShaderChFog Blue;
        static const ShaderChFog None;
    public:
        FogMode mMode;      ///< 雾质量模式
        Colour mColour;     ///< 雾颜色
        NIIf mBegin;        ///< 雾开始处(与平截头相关)
        NIIf mEnd;          ///< 雾结束处(与平截头相关)
        NIIf mDensity;      ///< 雾质量
    };
    
    /** 渲染通路中的基本点属性
    @remark 适合基本渲染系统
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChPoint : public ShaderAlloc
    {
    public:
        ShaderChPoint();
        ShaderChPoint(const ShaderChPoint & o);
        ShaderChPoint(NIIf size, NIIf minsize, NIIf maxsize);

        bool operator ==(const ShaderChPoint & o) const;

        /** 设置点大小
        @param[in] ps
        @version NIIEngine 3.0.0
        */
        inline void setSize(NIIf ps) { mSize = ps; }

        /** 获取点大小
        @remark 这个属性确定在渲染点列表时使用的点大小
        @version NIIEngine 3.0.0
        */
        inline NIIf getSize() const { return mSize; }

        /** 设置点最小大小
        @version NIIEngine 3.0.0
        */
        inline void setMinSize(NIIf min) { mMinSize = min; }

        /** 获取点最小大小
        @version NIIEngine 3.0.0
        */
        inline NIIf getMinSize() const { return mMinSize; }

        /** 设置线宽
        @version NIIEngine 3.0.0
        */
        inline void setLineWidth(NIIf w) { mLineWidth = w; }

        /** 获取线宽
        @version NIIEngine 3.0.0
        */
        inline NIIf getLineWidth() const { return mLineWidth; }

        /** 设置点最大大小
        @param[in] max 渲染系统支持的最大设置为 0
        @version NIIEngine 3.0.0
        */
        inline void setMaxSize(NIIf max) { mMaxSize = max; }

        /** 获取点最大大小
        @remark 渲染系统支持的最大设置为 0
        @version NIIEngine 3.0.0
        */
        inline NIIf getMaxSize() const { return mMaxSize; }

        /** 设置点列表渲染模式
        @remark 基本渲染中可能包含的特性,使用 POINTLIST / POINTS 模式绘制点,粒子系统可能能用上
        @version NIIEngine 3.0.0
        */
        void setSpritesEnable(bool b);

        /** 是否是点列表渲染模式
        @return 基本渲染中可能包含的特性,使用 POINTLIST / POINTS 模式绘制点,粒子系统可能能用上
        @version NIIEngine 3.0.0
        */
        bool isSpritesEnable() const;

        /** 设置是否启用距离衰减
        @return attenuate = 1 / (constant + linear * dist + quadratic * d^2).
        @version NIIEngine 3.0.0
        */
        void setAttenuateEnable(bool b);

        /** 返回点是否距启用距离衰减
        @version NIIEngine 3.0.0
        */
        bool isAttenuateEnable() const;

        /** 设置点距离衰减常数系数
        @version NIIEngine 3.0.0
        */
        inline void setConstant(NIIf f) { mConstant = f; }

        /** 获取点距离衰减常数系数
        @version NIIEngine 3.0.0
        */
        inline NIIf getConstant() const { return mConstant; }

        /** 设置点距离衰减线性系数.
        @version NIIEngine 3.0.0
        */
        inline void setLinear(NIIf f) { mLinear = f; }

        /** 获取点距离衰减线性系数.
        @version NIIEngine 3.0.0
        */
        inline NIIf getLinear() const { return mLinear; }

        /** 设置点距离衰减二次系数
        @version NIIEngine 3.0.0
        */
        inline void setQuadratic(NIIf f) { mQuadratic = f; }

        /** 返回点距离衰减二次系数.
        @version NIIEngine 3.0.0
        */
        inline NIIf getQuadratic() const { return mQuadratic; }

        /** 上传到着色程序参数中
        @version NIIEngine 5.0.0
        */
        void upload(GpuProgramParam * param) const;

        /** 从着色程序参数中设置
        @version NIIEngine 5.0.0
        */
        void download(const GpuProgramParam * param);

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
    
    /** 着色成分
    @version NIIEngine 3.0.0
    */
    enum ShaderBase
    {
        SB_Colour_R             = 0x01,
        SB_Colour_G             = 0x02,
        SB_Colour_B             = 0x04,
        SB_Colour_A             = 0x08,
        SB_Colour_RGB           = 0x07,
        SB_Colour_RGBA          = 0x0F,
        SB_Fog                  = 0x10,
        SB_Point                = 0x20,
        SB_Colour               = 0x40,
        SB_Blend                = 0x80,
        SB_Depth                = 0x100,
        SB_Alpha                = 0x200,
        SB_Stencil              = 0x400,
        SB_Texture              = 0x800,
        SB_Program              = 0x1000,
        SB_FogRef               = 0x2000,                       ///< enable() 无效
        SB_PointRef             = 0x4000,                       ///< enable() 无效
        SB_ColourRef            = 0x8000,                       ///< enable() 无效
        SB_BlendRef             = 0x10000,                      ///< enable() 无效
        SB_DepthRef             = 0x20000,                      ///< enable() 无效
        SB_AlphaRef             = 0x40000,                      ///< enable() 无效
        SB_StencilRef           = 0x80000,                      ///< enable() 无效
        SB_TextureRef           = 0x100000,                     ///< enable() 无效
        SB_ProgramRef           = 0x200000,                     ///< enable() 无效
        SB_FogSet               = SB_Fog | SB_FogRef,           ///< enable() 无效
        SB_PointSet             = SB_Point | SB_PointRef,       ///< enable() 无效
        SB_ColourSet            = SB_Colour | SB_ColourRef,     ///< enable() 无效
        SB_BlendSet             = SB_Blend | SB_BlendRef,       ///< enable() 无效
        SB_DepthSet             = SB_Depth | SB_DepthRef,       ///< enable() 无效
        SB_AlphaSet             = SB_Alpha | SB_AlphaRef,       ///< enable() 无效
        SB_StencilSet           = SB_Stencil | SB_StencilRef,   ///< enable() 无效
        SB_TextureSet           = SB_Texture | SB_TextureRef,   ///< enable() 无效
        SB_ProgramSet           = SB_Program | SB_ProgramRef,   ///< enable() 无效
        SB_Light                = 0x400000,
        SB_FogValid             = 0x800000,
        SB_UnitNormals          = 0x1000000,
        SB_LightClip            = 0x2000000,
        SB_LightClipPlane       = 0x4000000,
        // 扩展
        SB_Fusion               = 0x10000000,                   ///< enable() disbale 无效
    };

    /** 颜色掩码
    @version NIIEngine 3.0.0
    */
    enum ColourMark
    {
        CM_ZERO             = 0,
        CM_RED              = 0x01,
        CM_GREEN            = 0x02,
        CM_BLUE             = 0x04,
        CM_ALPHA            = 0x08,
        CM_RGBA             = 0x0F,
        CM_BGRA             = CM_RGBA
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
        SS_Frame,               ///< 对象当前帧缓寸
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

        ShaderCh & operator=(const ShaderCh & o);

        /** 所属渲染集
        @version NIIEngine 3.0.0
        */
        ShaderFusion * getParent() const;

        /** 局部索引
        @version NIIEngine 3.0.0
        */
        Nidx getIndex() const;

        /** 设置的名字
        @verison NIIEngine 3.0.0
        */
        void setName(const String & name);

        /** 获取的名字
        @version NIIEngine 3.0.0
        */
        const String & getName() const;

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
        bool isFusion() const;

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
        inline const ShaderChPoint & getPoint() const { return *mPoint; }

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
        inline const ShaderChFog & getFog() const { return *mFog; }

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
        inline const ShaderChBlend & getBlend() const { return *mBlend; }

        /** 设置深度测试应用
        @remark 基础事物
        @param[in] refIn
        @version NIIEngine 3.0.0
        */
        ShaderChDepth * setDepth(SetOperatorType type, const ShaderChDepth * refIn = 0) ;

        /** 获取深度测试应用
        @note 不能设置返回值内容
        @version NIIEngine 3.0.0
        */
        inline const ShaderChDepth & getDepth() const { return *mDepth; }

        /** 获取Alpha测试应用
        @remark 基础事物
        @param[in] refIn
        @version NIIEngine 3.0.0
        */
        ShaderChAlpha * setAlpha(SetOperatorType type, const ShaderChAlpha * refIn = 0);

        /** 获取Alpha测试应用
        @note 不能设置返回值内容
        @version NIIEngine 3.0.0
        */
        inline const ShaderChAlpha & getAlpha() const { return *mAlpha; }

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
        @param[in] mark ColourMark 的一个或多个组合
        @version NIIEngine 3.0.0
        */
        void setColourWrite(Nmark mark);

        /** 获取颜色写入
        @remark 基础事物
        @version NIIEngine 3.0.0
        */
        Nmark getColourWrite() const;

        /** 设置是否启用深度排序.
        @remark
        @version NIIEngine 3.0.0
        */
        void setDepthSort(bool b);

        /** 返回是否启用深度排序
        @version NIIEngine 3.0.0
        */
        bool isDepthSort() const;

        /** 设置是否强制深度排序
        @param[in] enabled
        @version NIIEngine 3.0.0
        */
        void setForceDepthSort(bool b);

        /** 返回是否强制深度排序
        @version nIIEngine 3.0.0
        */
        bool isForceDepthSort() const;

        /** 设置是否自动退化
        @param[in] b
        @version NIIEngine 3.0.0
        */
        void setAutoReduce(bool b);

        /** 获取是否自动退化
        @version NIIEngine 3.0.0
        */
        bool getAutoReduce() const;

        /** 设置光体模式
        @version NIIEngine 3.0.0
        */
        void setShadeMode(ShadeMode m);

        /** 获取光体模式
        @version NIIEngine 3.0.0
        */
        ShadeMode getShaderMode() const;

        /** 设置多边形模式
        @version NIIEngine 3.0.0
        */
        void setPolygonMode(PolygonMode m);

        /** 获取多边形模式
        @version NIIEngine 3.0.0
        */
        PolygonMode getPolygonMode() const;

        /** 设置渲染通道基于三角形面序.
        @version NIIEngine 3.0.0
        */
        void setCullingMode(CullingMode mode);

        /** 获取渲染通道基于三角形面序.
        @version NIIEngine 3.0.0
        */
        CullingMode getCullingMode() const;

        /** 设置系统拣选模式
        @version NIIEngine 3.0.0
        */
        void setSysCullingMode(SysCullingMode mode);

        /** 获取系统拣选模式
        @version NIIEngine 3.0.0
        */
        SysCullingMode getSysCullingMode() const;

        /** 设置是否动态单位化法线
        @version NIIEngine 3.0.0
        */
        void setUnitNormals(bool b);

        /** 获取是否动态单位化法线
        @version NIIEngine 3.0.0
        */
        bool isUnitNormals() const;

        /** 设置是否启用动态照明
        @param[in] b
        @version NIIEngine 3.0.0
        */
        void setLight(bool b);

        /** 是否启用动态照明
        @version NIIEngine 3.0.0
        */
        bool isLight() const;

        /** 设置是否启用雾设置
        @version NIIEngine 3.0.0
        */
        void setFogValid(bool b);

        /** 是否启用雾设置
        @version NIIEngine 3.0.0
        */
        bool isFogValid() const;

        /** 设置渲染通道使用的最大灯光数量
        @version NIIEngine 3.0.0
        */
        inline void setMaxLightCount(Nui16 count) { mMaxLightCount = count; }

        /** 获取这条通道使用的最大灯光数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxLightCount() const { return mMaxLightCount; }

        /** 设置渲染通道开始的灯光列表的索引
        @version NIIEngine 3.0.0
        */
        inline void setStartLight(Nui16 i) { mStartLight = i; }

        /** 获取这条通道开始的灯光列表的索引.
        @version NIIEngine 3.0.0
        */
        inline Nui16 getStartLight() const { return mStartLight; }

        /** 设置灯光掩码
        @param[in] mark 匹配码
        @version NIIEngine 3.0.0
        */
        inline void setLightMark(Nmark mark) { mLightMask = mark; }

        /** 获取灯光掩码
        @version NIIEngine 3.0.0
        */
        inline Nmark getLightMark() const { return mLightMask; }

        /** 设置多通道渲染次数
        @remark 用于着色程序控制通路时用
        @version NIIEngine 3.0.0
        */
        inline void setRenderCount(Nui16 c) { mRenderCount = c; }

        /** 获取多通道渲染次数
        @remark 用于着色程序控制通路时用
        @version NIIEngine 3.0.0
        */
        inline Nui16 getRenderCount() const { return mRenderCount; }

        /** 设置是否每个灯光渲染执行一次渲染
        @remark 用于渲染阴影到模板缓存或纹理时使用
        @version NIIEngine 3.0.0
        */
        inline void setEachLight(bool b) { mEachLight = b; }

        /** 获取是否每个灯光渲染执行一次渲染
        @remark 用于渲染阴影到模板缓存或纹理时使用
        @version NIIEngine 3.0.0
        */
        inline bool isEachLight() const { return mEachLight; }

        /** 设置灯光类型限制
        @version NIIEngine 3.0.0
        */
        inline void setLightTypeLimit(LightType type) { mOnlyLightType = type; }

        /** 获取灯光类型限制
        @version NIIEngine 3.0.0
        */
        inline LightType getLightTypeLimit() const { return mOnlyLightType; }

        /** 灯光数量限制
        @version NIIEngine 3.0.0
        */
        inline void setLightCountLimit(Nui16 c) { mLightCountLimit = c; }

        /** 如果启用灯光迭代,每个迭代的灯光数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getLightCountLimit() const { return mLightCountLimit; }

        /** 设置灯光是否矩形剪切
        @version NIIEngine 3.0.0
        */
        void setLightClip(bool b);

        /** 获取灯光是否矩形剪切
        @version NIIEngine 3.0.0
        */
        bool isLightClip() const;

        /** 设置灯光是否应用面裁剪
        @version NIIEngine 3.0.0
        */
        void setLightClipPlane(bool b);

        /** 获取灯光是否应用面裁剪
        @version NIIEngine 3.0.0
        */
        bool isLightClipPlane() const;

        /** 设置渲染阶段
        @version NIIEngine 3.0.0
        */
        inline void setShaderStep(ShaderStep ss) { mShaderStep = ss; }

        /** 获取渲染阶段
        @version NIIEngine 3.0.0
        */
        inline ShaderStep getShaderStep() const { return mShaderStep; }

        /** 状态掩码
        @version NIIEngine 3.0.0 高级api
        */
        Nmark getMark() const;
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
        ShaderChDepth * mDepth;         ///< 基础深度测试
        ShaderChAlpha * mAlpha;         ///< 基础透明测试
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

        Nui16 mStartLight;              ///< 开始灯光
        Nui16 mMaxLightCount;           ///< 灯光最大数量
        Nui16 mLightCountLimit;
        Nui16 mRenderCount;
        bool mEachLight;
        bool mDepthSort;
        bool mForceDepthSort;
        bool mAutoReduce;
        bool mAuto;
    };
}
#endif