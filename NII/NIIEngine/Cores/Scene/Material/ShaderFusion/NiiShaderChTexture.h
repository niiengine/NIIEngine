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

#ifndef _NII_SHADERCH_TEXTURE_H_
#define _NII_SHADERCH_TEXTURE_H_

#include "NiiPreInclude.h"
#include "NiiGpuProgram.h"
#include "NiiMatrix4.h"
#include "NiiColour.h"

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
        inline void setBlend(TextureBlendOp op, TextureBlendSource op1, TextureBlendSource op2)
        {
            mOP = op;
            mColourSrc1 = op1;
            mColourSrc2 = op2;
        }

        /** 混合组
        @version NIIEngine 3.0.0
        */
        inline void setBlend(TextureBlendSource op1, TextureBlendSource op2, NIIf factor)
        {
            mOP = TBO_CONSTANT_FACTOR;
            mColourSrc1 = op1;
            mColourSrc2 = op2;
            mConstantFactor = factor;
        }

        /** 混合组
        @version NIIEngine 3.0.0
        */
        inline void setBlend(TextureBlendOp op, const Colour & c1, TextureBlendSource op2)
        {
            mOP = op;
            mColourSrc1 = TBS_CONSTANT;
            mColourSrc2 = op2;
            mConstant1 = c1;
        }

        /** 混合组
        @version NIIEngine 3.0.0
        */
        inline void setBlend(TextureBlendOp op, TextureBlendSource op1, const Colour & c2)
        {
            mOP = op;
            mColourSrc1 = op1;
            mColourSrc2 = TBS_CONSTANT;
            mConstant2 = c2;
        }

        /** 混合组
        @version NIIEngine 3.0.0
        */
        inline void setBlend(TextureBlendOp op, NIIf f1, TextureBlendSource op2)
        {
            mOP = op;
            mColourSrc1 = TBS_CONSTANT;
            mColourSrc2 = op2;
            mConstant1.a = f1;
        }

        /** 混合组
        @version NIIEngine 3.0.0
        */
        inline void setBlend(TextureBlendOp op, TextureBlendSource op1, NIIf f2)
        {
            mOP = op;
            mColourSrc1 = op1;
            mColourSrc2 = TBS_CONSTANT;
            mConstant2.a = f2;
        }

        /** 设置混合操作
        @version NIIEngine 3.0.0
        */
        inline void setBlendOp(TextureBlendOp op)              { mOP = op; }

        /** 设置颜色类型
        @version NIIEngine 3.0.0
        */
        inline void setBlendType(TextureColourType type)       { mColourType = type; }

        /** 设置第一操作数
        @param[in] tbs 操作数
        @version NIIEngine 3.0.0
        */
        inline void setBlendSource1(TextureBlendSource tbs)    { mColourSrc1 = tbs; }

        /** 设置第二操作数
        @param[in] tbs 操作数
        @version NIIEngine 3.0.0
        */
        inline void setBlendSource2(TextureBlendSource tbs)    { mColourSrc2 = tbs; }

        /** 设置常量第一操作数
        @note TextureBlendSource 为 TBS_CONSTANT 和 TextureColourType 为 TCT_RGB 时使用
        @version NIIEngine 3.0.0
        */
        inline void setConstant1(const Colour & c1)            { mConstant1 = c1; }

        /** 设置常量第二操作数
        @note TextureBlendSource 为 TBS_CONSTANT 和 TextureColourType 为 TCT_RGB 时使用
        @version NIIEngine 3.0.0
        */
        inline void setConstant2(const Colour & c2)            { mConstant2 = c2; }

        /** 设置常量第一操作数
        @note TextureBlendSource 为 TBS_CONSTANT 和 TextureColourType 为 TCT_AAA 时使用
        @version NIIEngine 3.0.0
        */
        inline void setConstant1(NIIf f1)                      { mConstant1.a = f1; }

        /** 设置常量第一操作数
        @note TextureBlendSource 为 TBS_CONSTANT 和 TextureColourType 为 TCT_AAA 时使用
        @version NIIEngine 3.0.0
        */
        inline void setConstant2(NIIf f2)                      { mConstant2.a = f2; }

        /** 设置常量混合因子
        @note TextureBlendOp 为 TBO_CONSTANT_FACTOR 时使用
        @version NIIEngine 3.0.0
        */
        inline void setConstantFactor(NIIf f)                  { mConstantFactor = f; }
    public:
        TextureBlendOp mOP;
        TextureColourType mColourType;
        TextureBlendSource mColourSrc1;
        TextureBlendSource mColourSrc2;
        NIIf mConstantFactor;
        Colour mConstant1;
        Colour mConstant2;
    };
    
    /** 纹理样本模式
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TextureSampleUnit : public ShaderAlloc
    {
        friend ShaderChTextureUnit;
    public:
        TextureSampleUnit();
        TextureSampleUnit(TextureFilterOP min, TextureFilterOP mag, TextureFilterOP mip,
            TextureAddressingMode u, TextureAddressingMode v, TextureAddressingMode w);
        virtual ~TextureSampleUnit();

        TextureSampleUnit & operator =(const TextureSampleUnit & o);
        bool operator !=(const TextureSampleUnit & o) const;
        bool operator ==(const TextureSampleUnit & o) const;
        bool operator <(const TextureSampleUnit & o) const;

        /** 设置寻址模式
        @note 默认是 TAM_REPEAT,在可编程管线或固定管线中有效
        @version NIIEngine 3.0.0
        */
        inline void setMode(const TextureAddressing & uvw)  { mAddressMode = uvw; }

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
        inline void setAnisotropy(Nui32 aniso)      { mMaxAniso = aniso; }

        /** 设置抗锯齿等级
        @param[in] max 默认为1
        @note 这个渲染可以应用在固定管线和可编程管线中
        @version NIIEngine 3.0.0
        */
        inline Nui32 getAnisotropy() const          { return mMaxAniso; }

        /** 设置应用到mipmap计算的偏量值
        @remark Mipmap 就是纹理的LOD概念
        @param[in] bias fn(x) = mip-; fn(-x) = mip+;
        @version NIIEngine 
        */
        inline void setMipmapBias(NIIf bias)        { mMipmapBias = bias; }

        /** 获取应用到mipmap计算的偏量值
        @remark Mipmap 就是纹理的LOD概念
        @param[in] bias fn(x) = mip-; fn(-x) = mip+;
        @version NIIEngien 3.0.0
        */
        inline NIIf getMipmapBias() const           { return mMipmapBias; }

        /** 设置边缘颜色
        @remark TAM_BORDER模式特有
        @version NIIEngien 3.0.0
        */
        inline void setBorder(const Colour& colour) { mBorder = colour; }

        /** 获取边缘颜色
        @remark TAM_BORDER模式特有
        @version NIIEngien 3.0.0
        */
        inline const Colour & getBorder() const     { return mBorder; }

        /** 设置比较函数
        @version NIIEngine 3.0.0
        */
        inline void setCompareFunc(CmpMode function) { mCompareFunc = function; }

        /** 获取比较函数
        @version NIIEngien 3.0.0
        */
        inline CmpMode getCompareFunc() const       { return mCompareFunc; }
    protected:
        TextureFilterOP mMinOP;
        TextureFilterOP mMagOP;
        TextureFilterOP mMipOP;
        TextureAddressing mAddressMode;
        CmpMode mCompareFunc;
        Colour mBorder;
        Nui32 mMaxAniso;
        NIIf mMipmapBias;
        NIIf mMinLod;
        NIIf mMaxLod;
    };
    
    inline void TextureSampleUnit::setMode(TextureAddressingMode u, TextureAddressingMode v, TextureAddressingMode w)
    {
        mAddressMode.mS_U = u;
        mAddressMode.mT_V = v;
        mAddressMode.mR_W = w;
    }

    inline void TextureSampleUnit::setFiltering(TextureFilterOP minop, TextureFilterOP magop, TextureFilterOP mipop)
    {
        mMinOP = minop;
        mMagOP = magop;
        mMipOP = mipop;
    }
	
	typedef vector<TextureSampleUnit *>::type TextureSampleUnitList;
    
    /** 纹理样本模式设置
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TextureSample : public ShaderAlloc
    {
        friend ShaderChTextureUnit;
    public:
        typedef map<VString, TextureSampleUnit>::type UnitList;
    public:
        TextureSample();
        virtual ~TextureSample();
        
        TextureSample & operator = (const TextureSample & o) const;
        
        bool operator == (const TextureSample & o) const;
        
        bool operator != (const TextureSample & o) const;
        
        bool operator < (const TextureSample & o) const;
        
        /** 添加
        @version NIIEngine 3.0.0
        */
        void add(const VString & name, const TextureSampleUnit & unit);
        
        /** 移去
        @version NIIEngine 3.0.0
        */
        void remove(const VString & name);
        
        /** 设置
        @version NIIEngine 3.0.0
        */
        void set(Nidx slot, const VString & name);
        
        /** 获取
        @version NIIEngine 3.0.0
        */
        TextureSampleUnit * get(const VString & name) const;
        
        /** 获取
        @version NIIEngine 3.0.0
        */
        TextureSampleUnit * get(Nidx slot) const;
        
        /** 获取列表
        @version NIIEngine 3.0.0
        */
        inline const TextureSampleUnitList & getList() const         		    { return mSetList; }

        /** 设置使用数量
        @version NIIEngine 3.0.0
        */
        inline void setUsedCount(GpuProgram::ShaderType type, NCountb cnt) const{ N_assert1(type < GpuProgram::ST_Cnt); return mUsed[type] = cnt; }
        
        /** 获取使用数量
        @version NIIEngine 3.0.0
        */
        inline NCountb getUsedCount(GpuProgram::ShaderType type) const          { N_assert1(type < GpuProgram::ST_Cnt); return mUsed[type]; }
    protected:
        UnitList mUnitList;
        TextureSampleUnitList mSetList;
        NCountb mUsed[GpuProgram::ST_Cnt];
    };
    typedef vector<TextureSample *>::type TextureSampleList;
    
    /** 纹理数据检索类型
    @remark 如果纹理保存的并非像素,而是其他着色程序使用的信息
    @version NIIEngine 3.0.0 高级api
    */
    enum TextureDataFetch
    {
        TDF_FS      = 0,            ///< 常规片段处理单元 - 默认.
        TDF_TS      = 1,            ///< 细分面
        TDF_DS      = 2,            ///< 细分面评估
        TDF_VS      = 3,            ///< 顶点处理单元
        TDF_GS      = 4,            ///< 几何处理
        TDF_CS      = 5,            ///< 计算处理
        TDF_Count   = 6             ///< 属于着色程序范畴,并没有很多选择
    };

    /** 纹理坐标处理
    @note 类似的其他纹理设置 如 gltexcoord 将会失效,一般高光反透物体才需要,如枪,车,水面
    @version NIIEngine 3.0.0 高级api
    */
    enum TextureCoordPrc
    {
        TCP_NONE                = 0,    ///< 不计算纹理坐标
        TCP_ENV_SPHERE          = 1,    ///< 基于场景球形
        TCP_ENV_NORMAL          = 2,    ///< 基于顶点法线
        TCP_ENV_REFLECTION      = 3,    ///< 基于顶点位置
        TCP_ENV_REFLECTION_INV  = 4,    ///< 基于反射平面
        TCP_ENV_PROJECTIVE      = 5,    ///< 纹理投影(视口)
        TCP_Count               = 6,    ///< 尽量使用现有的,因为被底层API约束,内部其实还有个距阵系数
    };

    class ShaderChTexture;
    typedef vector<ShaderChTexture *>::type ShaderChTextureList;
    
    /** 基本纹理单元
    @remark
        和Texture类不同,这个类有基础状态.涉及到纹理距阵/环境映射的在可编程管线中没有效果
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChTextureUnit : public ShaderAlloc
    {
        friend class RenderSys;
    public:
        /** 纹理单元应用类型
        @remark 内置着色程序自动处理
        @version NIIEngine 3.0.0
        */
        enum Type
        {
            T_NORMAL = 0,       ///< 普通定义纹理/缓存(默认)
            T_PHOTO = 1,        ///< 物体相片,类speedtree的远处树/广告牌(远处的东西本来就是模糊的)
            T_SHADOW = 2,       ///< 阴影纹理(纹理阴影技术用)
            T_INV_PHOTO = 3,    ///< 镜子/水倒影相片(renderFrameObject)
            T_FUSION = 4,       ///< 帧合成
            T_HALO = 5,         ///< 光晕
            T_NORMALSMAP = 6,   ///< 法线纹理
            T_HIGHLIGHT = 7,    ///< 高光相片(放射性光体除了用片段着色程序,也可以用图片模拟)
            T_Count = 8         ///< 类型总数
        };
    
        /** 环境映射模式
        @version NIIEngine 3.0.0
        */
        enum EnvMapMode
        {
            ENM_Planar,     ///< 关于摄像机到顶点位置的映射
            ENM_Sphere,     ///< 关于摄像机到顶点位置和法线的映射
            ENM_Reflection, ///< 关于摄像机到顶点位置的反转映射
            ENM_Normal      ///< 关于摄像机空间的法线
        };

        /** 纹理变换
        @version NIIEngine 3.0.0
        */
        enum TextureTransformType
        {
            TTT_U_OFT,      ///< U方向坐标偏移
            TTT_V_OFT,      ///< V方向坐标偏移
            TTT_UV_OFT,     ///< UV方向坐标偏移
            TTT_U_SCALE,    ///< U方向坐标缩放
            TTT_V_SCALE,    ///< V方向坐标缩放
            TTT_UV_SCALE,   ///< UV方向坐标缩放
            TTT_ROTATE,     ///< 坐标旋转
            TTT_FUNCTION    ///< 函子坐标
        };

        /** 纹理效果
        @version NIIEngine 3.0.0
        */
        struct TextureEffect
        {
            TextureTransformType mType;             ///< 基础类型
            TextureTransformType mEffect;           ///< 实际效果类型
            DataValue<NIIf> * mIn;                  ///< 纹理坐标输入
            NIIf mFactor;                           ///< 纹理坐标输入参数
            DataFunc<TimeDurMS, NIIf> * mOutFunc;   ///< 纹理坐标输出函子
            DataEquation<TimeDurMS, NIIf> * mTimer; ///< 帧控制器
        };
        typedef multimap<TextureTransformType, TextureEffect>::type EffectMap;
        
        struct TextureView
        {
            NCount mMipmap;
            NCount mMipmapCount;
            NCount mLayer;
            NCount mLayerCount;
            PixelFormat mPixelFormat;
            bool mAs2DArray;
        };
        
        struct BufferData
        {
            TextureBuffer * mBuffer;
            NCount mOffet;
            NCount mCount;
        };
    public:
        ShaderChTextureUnit();
        ShaderChTextureUnit(ResourceID rid);
        ShaderChTextureUnit(const ShaderChTextureUnit & o);
        virtual ~ShaderChTextureUnit();

        ShaderChTextureUnit & operator = (const ShaderChTextureUnit & o);
        
        /** 设置资源组
        @version NIIEngine 3.0.0
        */
        inline void setResourceGroup(GroupID gid)              { mResGroupID = gid; }
        
        /** 获取资源组
        @version NIIEngine 3.0.0
        */
        inline GroupID getResourceGroup() const                { return mResGroupID; }

        /** 设置纹理单元的名字
        @remark 辅助设置
        @version NIIEngine 3.0.0
        */
        inline void setName(const String & name)               {mName = name; }

        /** 获取单元名字
        @remark 辅助设置
        @version NIIEngine 3.0.0
        */
        inline const String & getName() const                  { return mName; }

        /** 设置来源
        @note T_CUBE 纹理需要设置 setEnvMap(true) 和 setEnvMapType(ENM_Sphere).
        @version NIIEngine 3.0.0
        */
        void setSrc(const ResourceIDList & rid, Texture::Type type, Type ctype = T_NORMAL);

        /** 设置来源
        @note T_CUBE 纹理需要设置 setEnvMap(true) 和 setEnvMapType(ENM_Sphere).
        @version NIIEngine 3.0.0
        */
        void setSrc(ResourceID rid, const StringList & src, Texture::Type type, Type ctype = T_NORMAL);

        /** 设置来源
        @note T_CUBE 纹理需要设置 setEnvMap(true) 和 setEnvMapType(ENM_Sphere).
        @version NIIEngine 3.0.0
        */
        void setSrc(const Texture * tex, Texture::Type type, Type ctype = T_NORMAL);

        /** 设置合成
        @param[in] framefusion 帧合成
        @param[in] tex 合成纹理
        @version NIIEngine 3.0.0
        */
        void setFusion(ResourceID fusion, const String & tex);

        /** 设置动画
        @param rid 资源ID集 [rid[0], rid[count - 1]] 集合
        @param count 面片数量
        @param ms 动画总时长
        @version NIIEngine 3.0.0
        */
        void setAni(const ResourceIDList & rid, TimeDurMS ms = 0);

        /** 获取当前纹理
        @version NIIEngine 3.0.0
        */
        inline const Texture * getTexture() const              { return getTexture(mCurrentFrame); }

        /** 获取帧纹理
        @version NIIEngine 3.0.0
        */
        const Texture * getTexture(NCount frame) const;

        /** 设置当前帧的纹理指针
        @version NIIEngine 3.0.0
        */
        inline void setTexture(const Texture * tex)            { setTexture(tex, mCurrentFrame); }

        /** 给定一个帧,设置纹理指针
        @version NIIEngine 3.0.0
        */
        void setTexture(const Texture * tex, NCount frame);

        /** 获取当前纹理图象的
        @version NIIEngine 3.0.0
        */
        ResourceID getResourceID() const;

        /** 获取合成器
        @version NIIEngine 3.0.0
        */
        inline ResourceID getFrameFusion() const               { return mFrameFusion; }

        /** 获取合成纹理
        @version NIIEngine 3.0.0
        */
        inline const String & getFusionTexture() const         { return mFusionTex; }

        /** 滚动动画.
        @remark
        @param[in] u fn(u) = right, fn(-u) = left
        @param[in] v fn(v) = up, fn(-v) = down
        @version NIIEngine 3.0.0
        */
        void setScrollAnimation(NIIf u, NIIf v);

        /** 缩放动画
        @param[in] u 系数
        @param[in] v 系数
        @version NIIEngine 3.0.0
        */
        void setScaleAnimation(NIIf u, NIIf v);

        /** 旋转动画
        @param[in] f 系数
        @version NIIEngine 3.0.0
        */
        void setRotateAnimation(NIIf f);

        /** 设置函子动画
        @note 使用 [0.0, 1.0] 坐标范围,所以均为 NIIf 类型
        @param[in] in 输入坐标控制
        @param[in] out 输出坐标控制
        @version NIIEngine 3.0.0
        */
        void addAnimation(TextureTransformType ttt, DataValue<NIIf> * in, DataFunc<TimeDurMS, NIIf> * out);

        /** 移去效果
        @param[in] type 效果类型
        @version NIIEngine 3.0.0
        */
        void remove(TextureTransformType type);

        /** 移去这个应用到这个纹理层的所有效果
        @version NIIEngine 3.0.0
        */
        void removeAllEffects();

        /** 获取在一个复合映射配对的纹理效果数组
        @version NIIEngine 3.0.0
        */
        inline const EffectMap & getEffects() const                { return mEffects; }

        /** 动画时长
        @remark 动画纹理专用
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getAniTime() const                        { return mAnimDuration; }
        
        /** 设置是否自动加载
        @version NIIEngine 3.0.0
        */
        void setAutoLoad(bool b);
        
        /** 设置是否自动加载
        @version NIIEngine 3.0.0
        */
        bool isAutoLoad() const;

        /** 设置环境映射
        @note 在可编程管线中没有效果
        @version NIIEngine 3.0.0
        */
        void setEnvMap(bool b);

        /** 是否环境映射
        @note 在可编程管线中没有效果
        @version NIIEngine 3.0.0
        */
        bool isEnvMap() const;

        /** 设置环境映射类型
        @note 在可编程管线中没有效果
        @version NIIEngine 3.0.0
        */
        inline void setEnvMapType(EnvMapMode type)                 { mEnvMapType = type; }

        /** 获取环境映射类型
        @note 在可编程管线中没有效果
        @version NIIEngine 3.0.0
        */
        inline EnvMapMode getEnvMapType() const                    { return mEnvMapType; }

        /** 设置是否投影纹理
        @note 在可编程管线中没有效果
        @param[in] b 如果使用,则需要设置 setProjFrustum
        @version NIIEngine 3.0.0
        */
        void setProjTexture(bool b);

        /** 获取是否投影纹理
        @note 在可编程管线中没有效果,如果使用,则需要设置setProjFrustum
        @version NIIEngine 3.0.0
        */
        bool isProjTexture() const;

        /** 设置投影纹理的投影空间
        @note 在可编程管线中没有效果
        @param[in] proj 投影空间
        @version NIIEngine 3.0.0
        */
        inline void setProjFrustum(Frustum * proj)                 { mProjFrustum = proj; }

        /** 获取投影纹理的投影空间
        @note 在可编程管线中没有效果
        @version NIIEngine 3.0.0
        */
        inline const Frustum * getProjFrustum() const              { return mProjFrustum; }

        /** 添加一个纹理名字到帧容器的末尾
        @param[in] name 纹理名字
        @note 固定管线和可编程管线中有效果
        @version NIIEngine 3.0.0
        */
        void addFrame(ResourceID rid);

        /** 删除一个指定纹理帧.这个纹理没有被删除,但这个纹理不会再在纹理单元里
            使用.引发一个异常,如果帧数量超出了实际的帧数
        @param[in] frame
        @note 固定管线和可编程管线中有效果
        @version NIIEngine 3.0.0
        */
        void removeFrame(NCount frame);

        /** 设置关联一个帧的纹理名字
        @param[in] name 纹理名字
        @note 固定管线和可编程管线中有效果
        @version NIIEngine 3.0.0
        */
        void setFrame(ResourceID rid, NCount frame);
        
        /** 获取关联一个帧数量的纹理名字. 抛出一个异常,如果frameNumber超过了存储帧的数量.
        @note 固定管线和可编程管线中有效果
        @version NIIEngine 3.0.0
        */
        inline ResourceID getFrame(NCount frame) const     { N_assert (frame >= mTexIDList.size(), ""); return mTexIDList[frame]; }

        /** 为一纹理获取帧数量.
        @note 固定管线和可编程管线中有效果
        */
        inline NCount getFrameCount() const                { return mTexIDList.size(); }

        /** 设置活动帧
        @remark 设置活动帧
        @note 固定管线和可编程管线中有效果
        */
        inline void setCurrentFrame(NCount frame)          { N_assert (frame < mTexIDList.size(), ""); mCurrentFrame = frame; }

        /** 获取活动帧
        @note 固定管线和可编程管线中有效果
        @version NIIEngine 3.0.0
        */
        inline NCount getCurrentFrame() const              { return mCurrentFrame; }

        /** 设置这个层使用纹理坐标集的索引
        @note 默认0,除非使用多纹理多纹理坐标
        @note 固定管线和可编程管线中有效果
        @version NIIEngine 3.0.0
        */
        inline void setCoordSet(Nidx16 index)              { mCoordSet = index; }

        /** 获取这个层使用纹理坐标集的索引
        @note 固定管线和可编程管线中有效果
        @version NIIEngine 3.0.0
        */
        inline Nidx16 getCoordSet() const                  { return mCoordSet; }

        /** 设置着色程序应用类型
        @note 着色程序专用设置
        @version NIIEngine 3.0.0 高级api
        */
        inline void setFetchType(TextureDataFetch bt)      { mFetchType = bt; }

        /** 获取着色程序应用类型
        @note 着色程序专用设置
        @verison NIIEngien 3.0.0 高级api
        */
        inline TextureDataFetch getFetchType() const       { return mFetchType; }

        /** 设置内容的类型
        @remark
        @version NIIEngine 3.0.0
        */
        void setContentType(Type type);

        /** 获取内容的类型
        @version NIIEngine 3.0.0
        */
        inline Type getContentType() const                 { return mContentType; }

        /** 获取是否由6个面元组成
        @note 固定管线和可编程管线中都有效
        */
        bool isCubic() const;

        /** 返回这个纹理的类型.
        @note 应用到固定管线和可编程管线
        @version NIIEngine 3.0.0
        */
        inline Texture::Type getType() const               { return mTextureType; }

        /** 设置像素格式
        @remark 期望
        @version NIIEngine 3.0.0
        */
        inline void setFormat(PixelFormat pf)              { mFormat = pf; }

        /** 获取像素格式
        @remark 期望
        @version NIIEngine 3.0.0
        */
        inline PixelFormat getFormat() const               { return mFormat; }

        /** 设置
        @remark 期望
        @version NIIEngine 3.0.0
        */
        inline void setMipmapCount(NIIi count)             { mMipmapCount = count; }

        /** 获取
        @remark 期望
        @version NIIEngine 3.0.0
        */
        inline NIIi getMipmapCount() const                 { return mMipmapCount; }

        /** 设置是否纯Alpha通道
        @version NIIEngine 3.0.0
        */
        void setIsAlpha(bool b);

        /** 获取是否纯Alpha通道
        @version NIIEngine 3.0.0
        */
        bool getIsAlpha() const;

        /// @copydetails Texture::setHardwareSRGB
        void setHardwareSRGB(bool b);

        /// @copydetails Texture::isHardwareSRGB
        bool isHardwareSRGB() const;

        /** 设置(纹理集)中的索引
        @version NIIEngine 3.0.0 高级api
        */
        void setLocalIndex(ShaderChTexture * sct, Nidx16 index);

        /** 获取(纹理集)中的索引
        @version NIIEngine 3.0.0 高级api
        */
        Nidx16 getLocalIndex(ShaderChTexture * sct) const;

        /** 设置复合中的索引
        @version NIIEngine 3.0.0 高级api
        */
        inline void setMultiIndex(Nidx16 index)            { mMultiIndex = index; }

        /** 获取复合中的索引
        @version NIIEngine 3.0.0 高级api
        */
        inline Nidx16 getMultiIndex() const                { return mMultiIndex; }

        /** 设置U方向滚动值
        @param[in] value
        @note 在可编程管线中没有效果
        @version NIIEngien 3.0.0
        */
        void setUOft(NIIf value);

        /** 获取U方向滚动值
        @note 在可编程管线中没有效果
        @version NIIEngien 3.0.0
        */
        inline NIIf getUOft() const                        { return mUOft; }

        /** 设置V方向滚动值
        @param[in] value
        @note 在可编程管线中没有效果
        @version NIIEngien 3.0.0
        */
        void setVOft(NIIf value);

        /** 获取V方向滚动值
        @note 在可编程管线中没有效果
        @version NIIEngien 3.0.0
        */
        inline NIIf getVOft() const                        { return mVOft; }

        /** 设置U方向缩放
        @note 在可编程管线中没有效果
        @version NIIEngine 3.0.0
        */
        void setUScale(NIIf value);

        /** 获取U方向缩放
        @version NIIEngine 3.0.0
        */
        inline NIIf getUScale() const                      { return mUScale; }

        /** 设置V方向缩放
        @note 在可编程管线中没有效果
        @version NIIEngine 3.0.0
        */
        void setVScale(NIIf value);

        /** 获取V方向缩放
        @version NIIEngine 3.0.0
        */
        inline NIIf getVScale() const                      {return mVScale;}

        /** 设置应用在纹理坐标的逆时针旋转因子
        @note 在可编程管线中没有效果
        @param[in] angle 旋转角度(逆时针)
        */
        void setRotate(const Radian & r);

        /** 获取纹理旋转效果angle值
        @version NIIEngine 3.0.0
        */
        inline const Radian & getRotate() const            {return mRotate;}

        /** 获取最终混算
        @note 在可编程管线中没有效果
        @version NIIEngien 3.0.0
        */
        void setMatrix(const Matrix4f & x);

        /** 获取最终混算
        @note 在可编程管线中没有效果
        @version NIIEngine 3.0.0
        */
        const Matrix4f & getMatrix() const;

        /** 获取多纹理颜色混合
        @version NIIEngine 3.0.0 高级api
        */
        inline TextureBlend * getColourBlend() const        { return const_cast<TextureBlend *>(&mColourBlend); }

        /** 获取多纹理透明度混合
        @version NIIEngine 3.0.0 高级api
        */
        inline TextureBlend * getAlphaBlend() const         { return const_cast<TextureBlend *>(&mAlphaBlend); }

        /** 获取纹理样本模式
        @version NIIEngien 3.0.0 高级api
        */
        inline TextureSampleUnit * getSample() const        { return const_cast<TextureSampleUnit *>(&mSample); }

        /** 状态/性质变化了
        @version NIIEngine 3.0.0
        */
        void notify();

        /** 添加引用
        @version NIIEngine 3.0.0
        */
        void addRef(const ShaderChTexture * sct);
        
        /** 移去引用
        @version NIIEngine 3.0.0
        */
        void removeRef(const ShaderChTexture * sct);

        /** 进入加载策略
        @version NIIEngine 3.0.0 高级api
        */
        virtual void plan();

        /** 移出加载策略
        @version NIIEngine 3.0.0 高级api
        */
        virtual void unplan();

        /** 加载
        @version NIIEngine 3.0.0 高级api
        */
        virtual void load();

        /** 卸载
        @version NIIEngine 3.0.0 高级api
        */
        virtual void unload();
        
        /** 正式引用
        @remark 引用数量为0,如果是设置为自动删除,这个缓存将被销毁
        @version NIIEngine 3.0.0
        */
        NCount ref();

        /** 解除引用
        @remark 引用数量为0,如果是设置为自动删除,这个缓存将被销毁
        @version NIIEngine 3.0.0
        */
        NCount unref();

        /** 获取引用数量
        @version NIIEngine 3.0.0
        */
        inline NCount getRefCount() const                   { return mRefCount;  }

        /** 是否已经加载
        @version NIIEngien 3.0.0
        */
        bool isLoad() const;

        /** 纹理是否与空间相关
        @remark 一般涉及到变换的,就和空间有关系
        @version NIIEngine 3.0.0
        */
        virtual bool isMatrixRelation() const;

        /** 纹理是否与摄象机相关
        @remark
            如果涉及环境映射的纹理都是和摄像机有关系的,如同纹理摄像机那样,需要
            一个摆位/借位摄像机
        @version NIIEngien 3.0.0
        */
        virtual bool isCameraRelation() const;
    protected:
        /// 回收加载的资源
        void recover();
    protected:
        typedef map<ShaderChTexture *, Nidx16>::type LocalIndex;
    protected:
        String mName;
        ShaderChTextureList mApplyList;
        LocalIndex mLocalIndexList;
        DataEquation<TimeDurMS, NIIf> * mTimeFunc;
        GroupID mResGroupID;
        Texture::Type mTextureType;         ///< 纹理类型
        Type mContentType;                  ///< 纹理内容类型
        ResourceID mFrameFusion;            ///< 帧合成
        String mFusionTex;                  ///< 合成纹理
        TimeDurMS mAnimDuration;            ///< 动画长度(单位:毫秒)
        NCount mCurrentFrame;               ///< 当前动画帧
        TextureDataFetch mFetchType;        ///< 数据检索模式
        TextureAddressing mAddressMode;     ///< 纹理地址模式
        EnvMapMode mEnvMapType;             ///< 环境映射类型
        Frustum * mProjFrustum;             ///< 纹理投影空间
        PixelFormat mFormat;                ///< 像素格式
        NIIi mMipmapCount;                  ///< 层次数量
        EffectMap mEffects;
        Nidx16 mCoordSet;
        Nidx16 mMultiIndex;
        TextureSampleUnit mSample;
        TextureBlend mColourBlend;
        TextureBlend mAlphaBlend;
        NCount mRefCount;

        NIIf mUOft, mVOft, mUScale, mVScale;
        Radian mRotate;
        mutable Matrix4f mMatrix;
        ResourceIDList mTexIDList;
        mutable TextureList mTextures;
        mutable Nmark mMark;
    };
        
    /** 应用纹理
    @remark
        具体的使用和对象本身有关系,纹理的通用性并不强,和网格顶点/纹理坐标有直接有关
    @note 16个绑定口
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChTexture : public ShaderAlloc
    {
        friend class ShaderCh;
    public:
        /** 纹理单元内容类型
        @version NIIEngine 5.0.0
        */
        enum SamplerType
        {
            ST_Storage,
            ST_Sampler
        };

        /** 缓存槽
        @version NIIEngine 5.0.0
        */
        struct StorageUnit
        {
            StorageUnit()            { memset(this, 0, sizeof(StorageUnit)); }

            StorageUnit(TextureBuffer * buf, NCount oft, NCount size) : mBuffer(buf), mOffset(oft), mSize(size)
            {
                if(mBuffer)
                    mBuffer->ref();
            }
            
            ~StorageUnit()
            {
                if(mBuffer)
                {
                    mBuffer->unref();
                    mBuffer = 0;
                }
            }
            
            void setBuffer(TextureBuffer * buf)         
            {
                if(mBuffer)
                    mBuffer->unref();
                mBuffer = buf;
                if(mBuffer)
                    mBuffer->ref();
            }
            
            inline TextureBuffer * getBuffer() const           { return mBuffer; }
            
            StorageUnit & operator =(const StorageUnit & o)
            {
                if(mBuffer)
                    mBuffer->unref();
                mBuffer = o.mBuffer;
                if(mBuffer)
                    mBuffer->ref();
                mOffset = o.mOffset;
                mSize = o.mSize;
                return *this;
            }
            
            inline bool operator == (const StorageUnit & o) const
            {
                return mBuffer == o.mBuffer && mOffset == o.mOffset && mSize == o.mSize;
            }
            
            inline bool operator != (const StorageUnit & o) const
            {
                return mBuffer != o.mBuffer || mOffset != o.mOffset || mSize != o.mSize;
            }

            inline bool operator < (const StorageUnit & o) const
            {
                if(mBuffer != o.mBuffer)
                    return mBuffer < o.mBuffer;
                if(mOffset != o.mOffset)
                    return mOffset < o.mOffset;
                if(mSize != o.mSize)
                    return mSize < o.mSize;

                return false;
            }
        protected:
            TextureBuffer * mBuffer;
            PixelFormat mPadding;       ///< no using
        public:
            Nui32 mOffset;
            Nui32 mSize;
        };

        /** 纹理槽
        @version NIIEngine 5.0.0
        */
        struct SamplerUnit
        {
            SamplerUnit()           { memset(this, 0, sizeof(SamplerUnit)); }
            
            SamplerUnit(ShaderChTextureUnit * buf, PixelFormat pf, Nui16 mipmaps, Nui16 arrayidx, Nui16 mipmap, bool force2d) :
                mTexture(buf), mFormat(pf), mMipmapCount(mipmaps), mArrayIndex(arrayidx), mMipmap(mipmap), mForce2DArray(force2d)
            {
                if(mTexture)
                {
                    mTexture->ref();
                    if(mFormat == PF_UNKNOWN)
                        mFormat = mTexture->getFormat();
                }
            }
            
            ~SamplerUnit()
            {
                if(mTexture)
                {
                    mTexture->unref();
                    mTexture = 0;
                }
            }
            
            void setBuffer(ShaderChTextureUnit * buf)         
            {
                if(mTexture)
                    mTexture->unref();
                mTexture = buf;
                if(mTexture)
                    mTexture->ref();
            }
            
            inline ShaderChTextureUnit * getBuffer() const      { return mTexture; }
            
            SamplerUnit & operator =(const SamplerUnit & o)
            {
                if(mTexture)
                    mTexture->unref();
                mTexture = o.mTexture;
                if(mTexture)
                    mTexture->ref();
                mFormat = o.mFormat;
                mMipmapCount = o.mMipmapCount;
                mArrayIndex = o.mArrayIndex;
                mForce2DArray = o.mForce2DArray;
                mMipmap = o.mMipmap;
                return *this;
            }

            inline bool operator == (const SamplerUnit & o) const
            {
                return mTexture == o.mTexture && mMipmap == o.mMipmap && mMipmapCount == o.mMipmapCount &&
                    mArrayIndex == o.mArrayIndex && mFormat == o.mFormat;
            }

            inline bool operator != (const SamplerUnit & o) const
            {
                return mTexture != o.mTexture || mMipmap != o.mMipmap || mMipmapCount != o.mMipmapCount || 
                    mArrayIndex != o.mArrayIndex || mFormat != o.mFormat;
            }

            bool operator < (const SamplerUnit & o) const
            {
                if(mTexture != o.mTexture)
                    return mTexture < o.mTexture;
                if(mMipmap != o.mMipmap)
                    return mMipmap < o.mMipmap;
                if(mMipmapCount != o.mMipmapCount)
                    return mMipmapCount < o.mMipmapCount;
                if(mArrayIndex != o.mArrayIndex)
                    return mArrayIndex < o.mArrayIndex;
                if(mFormat != o.mFormat)
                    return mFormat < o.mFormat;

                return false;
            }

            inline bool isValidView() const
            {
                return mFormat == mTexture->getFormat() && mMipmap == 0 && mArrayIndex == 0 && mMipmapCount == 0 &&
                    !(mForce2DArray && (mTexture->getType() == Texture::T_CUBE || mTexture->getType() == Texture::T_CUBEArray));
            }
            
            ShaderChTextureUnit * mTexture;
            PixelFormat mFormat;
            Nui16 mMipmapCount;
            Nui16 mArrayIndex;
            Nui16 mMipmap;
            Nui16 mForce2DArray;
        };

        struct Slot
        {
        public:
            Slot() : mPType(ST_Storage)
            {
                memset(this, 0, sizeof(Slot));
            }

            Slot(SamplerType type) : mPType(type)
            {
                memset(this, 0, sizeof(Slot));
            }
            
            void operator =(const Slot & o)
            {
                mPType = o.mPType;
                if(mPType == ST_Storage)
                {
                    return mBuffer = o.mBuffer;
                }
                else
                {
                    return mTexture = o.mTexture;
                }
            }

            inline bool empty() const
            {
                return mBuffer.mBuffer != 0 || mTexture.mTexture != 0;
            }

            inline bool isBuffer() const
            {
                return mPType == ST_Storage;
            }
            
            inline bool isTexture() const
            {
                return mPType == ST_Sampler;
            }

            inline StorageUnit & getBuffer()
            {
                N_assert1(mPType == ST_Storage);
                return mBuffer;
            }

            inline const StorageUnit & getBuffer() const
            {
                N_assert1(mPType == ST_Storage);
                return mBuffer;
            }

            inline SamplerUnit & getTexture()
            {
                N_assert1(mPType == ST_Sampler);
                return mTexture;
            }

            inline const SamplerUnit & getTexture() const
            {
                N_assert1(mPType == ST_Sampler);
                return mTexture;
            }

            bool operator == (const Slot & o) const
            {
                if(mPType == o.mPType)
                {
                    if(mPType == ST_Storage)
                    {
                        return mBuffer == o.mBuffer;
                    }
                    else
                    {
                        return mTexture == o.mTexture;
                    }
                }
                return false;
            }
            
            bool operator != (const Slot & o) const
            {
                if(mPType == o.mPType)
                {
                    if(mPType == ST_Storage)
                    {
                        return mBuffer != o.mBuffer;
                    }
                    else
                    {
                        return mTexture != o.mTexture;
                    }
                }
                return true;
            }

            bool operator < (const Slot & o) const
            {
                if(mPType != o.mPType)
                    return mPType < o.mPType;

                if(mPType == ST_Storage)
                {
                    return mBuffer < o.mBuffer;
                }
                else
                {
                    return mTexture < o.mTexture;
                }
            }
        protected:
            SamplerType mPType;
            union
            {
                StorageUnit mBuffer;
                SamplerUnit mTexture;
            };
        };
        
        typedef vector<Slot>::type SlotList;
    public:
        ShaderChTexture();
        ShaderChTexture(const ShaderChTexture & o);
        ~ShaderChTexture();

        ShaderChTexture & operator =(const ShaderChTexture & o);
        
        bool operator == (const ShaderChTexture & o) const;
        
        bool operator != (const ShaderChTexture & o) const;

        bool operator < (const ShaderChTexture & o) const;
        
        /** 状态/性质变化了
        @version NIIEngine 3.0.0
        */
        void notify();
        
        /** 添加引用
        @version NIIEngine 3.0.0
        */
        void addRef(const ShaderCh * sch);
        
        /** 移去引用
        @version NIIEngine 3.0.0
        */
        void removeRef(const ShaderCh * sch);

        /** 局部化多线程
        @see Resource::plan
        @version NIIEngine 3.0.0
        */
        void plan();

        /** 局部化多线程
        @see Resource::unplan
        @version NIIEngine 3.0.0
        */
        void unplan();

        /** 局部化多线程
        @see Resource::load
        @version NIIEngine 3.0.0
        */
        void load();

        /** 局部化多线程
        @see Resource::unload
        @version NIIEngine 3.0.0
        */
        void unload();
        
        /** 状态/性质变化了
        @version NIIEngine 3.0.0
        */
        void notify();

        /** 设置每个纹理单元的纹理过滤
        @param[in] mode 过滤组合模式
        @version NIIEngine 3.0.0
        */
        void setFilterMode(TextureFilterMode mode);

        /** 设置所有纹理使用的各向异性等级
        @param[in] value 值
        @version NIIEngine 3.0.0
        */
        void setAnisotropy(Nui32 value);

        /** 创建一个应用纹理
        @param[in] type 纹理类型
        @version NIIEngine 3.0.0
        */
        ShaderChTextureUnit * create(GpuProgram::ShaderType type, Type type = T_NORMAL);

        /** 创建一个应用纹理
        @param[in] rid 资源ID(应用程序中唯一)
        @param[in] slot
            纹理套口,基本渲染系统一般支持同一时刻渲染多个纹理,默认只有一个,所以默认为0,
            此处的纹理套口并不是直接能设置 Type 的所有类型, Type 的意义并不和纹理套口
            相同
        @param[in] type 纹理类型
        @version NIIEngine 3.0.0
        */
        ShaderChTextureUnit * create(ResourceID rid, GpuProgram::ShaderType type, Nidx slot = 0, GpuProgram::ShaderType type, Type type = T_NORMAL);

        /** 创建一个应用纹理
        @param[in] tex 纹理对象
        @param[in] slot
            纹理套口,基本渲染系统一般支持同一时刻渲染多个纹理,默认只有一个,所以默认为0,
            此处的纹理套口并不是直接能设置 Type 的所有类型, Type 的意义并不和纹理套口
            相同
        @param[in] type 纹理类型
        @version NIIEngine 3.0.0
        */
        ShaderChTextureUnit * create(const Texture * tex, GpuProgram::ShaderType type, Nidx slot = 0, Type type = T_NORMAL);

        /** 添加的应用纹理单元
        @param[in] unit
        @version NIIEngine 3.0.0
        */
        void add(GpuProgram::ShaderType type, ShaderChTextureUnit * unit, Nidx idx = -1);

        /** 获取应用纹理单元
        @param[in] index
        @version NIIEngine 3.0.0
        */
        Slot & get(Nidx index) const;

        /** 获取应用纹理
        @param[in] name 纹理单元名字
        @version NIIEngine 3.0.0
        */
        Slot & get(const String & name) const;

        /** 获取类型第N个纹理.
        @param[in] type 类型
        @param[in] index 索引
        @version NIIEngine 3.0.0
        */
        Slot & get(GpuProgram::ShaderType type, Nidx index) const;

        /** 获取内容类型纹理迭代
        @version NIIEngine 3.0.0 高级api
        */
        Slot & get(GpuProgram::ShaderType type, SlotList::const_iterator & begin, SlotList::const_iterator & end) const;

        /** 移动应用纹理到另一个应用纹理中
        @remark Type([begin, end))的纹理移动到参数o中
        @param[in] type 类型
        @param[in] begin 开始索引
        @param[in] end 结束索引
        @param[in] o 另一个应用纹理集
        @note 这里指的是剪切粘贴,并非复制
        @version NIIEngine 3.0.0 高级api
        */
        void move(GpuProgram::ShaderType type, Nidx begin, Nidx end, ShaderChTexture * o);

        /** 移去应用纹理
        @param[in] type
        @param[in] index
        @version NIIEngine 3.0.0
        */
        void remove(GpuProgram::ShaderType type, Nidx index);

        /** 移去所有应用纹理
        @version NIIEngine 3.0.0
        */
        void removeAll();
        
        /** 是否有效
        @version NIIEngine 3.0.0
        */
        bool isValid() const;

        /** 获取应用纹理列表
        @remark 这个函数会直接影响整个应用纹理,注意操作限制mUsed[x]
        @version NIIEngien 3.0.0 高级api
        */
        inline const SlotList & getSlots() const                            { return mSlotList; }
        
        /** 设置使用数量
        @version NIIEngine 3.0.0
        */
        void setUsedCount(GpuProgram::ShaderType type, NCountb cnt) const   { N_assert1(type < GpuProgram::ST_Cnt); return mUsed[type] = cnt; }
        
        /** 获取使用数量
        @version NIIEngine 3.0.0
        */
        NCountb getUsedCount(GpuProgram::ShaderType type) const             { N_assert1(type < GpuProgram::ST_Cnt); return mUsed[type]; }
    private:
        ShaderChList mApplyList;
        SlotList mSlotList;
        NCountb mUsed[GpuProgram::ST_Cnt];
        static Slot NullSlot;
    };
}
#endif