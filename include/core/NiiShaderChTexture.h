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
#include "NiiTexture.h"
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
        inline const Colour& getBorder() const      { return mBorder; }

        /** 设置比较模式
        @version NIIEngien 3.0.0
         */
        inline void setCompare(bool enabled)        { mCompare = enabled; }

        /** 是否比较模式
        @version NIIEngien 3.0.0
        */
        inline bool isCompare() const               { return mCompare; }

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
        bool mCompare;
    };
    
    /** 纹理数据检索类型
    @remark 如果纹理保存的并非像素,而是其他顶点使用的信息
    @version NIIEngine 3.0.0 高级api
    */
    enum TextureDataFetch
    {
        TDF_FS = 0,             ///< 常规片段处理单元 - 默认.
        TDF_VS = 1,             ///< 顶点处理单元
        TDF_Count = 2           ///< 属于着色程序范畴,并没有很多选择
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

    /** 应用纹理
    @remark
        具体的使用和对象本身有关系,纹理的通用性并不强,和网格顶点/纹理坐标有直接有关
    @note 每种类型支持255个纹理,不能超出. 性质特殊,不能在任意渲染通路中切换
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChTexture : public ShaderAlloc
    {
        friend class ShaderCh;
    public:
        /** 纹理单元内容类型
        @version NIIEngine 3.0.0
        */
        enum Type
        {
            T_NORMAL = 0,       ///< 普通定义纹理(自动)
            T_PHOTO = 1,        ///< 物体相片,类speedtree的远处树/广告牌(远处的东西本来就是模糊的)
            T_SHADOW = 2,       ///< 阴影纹理(纹理阴影技术用)
            T_INV_PHOTO = 3,    ///< 镜子/水倒影相片
            T_FUSION = 4,       ///< 帧合成
            T_HALO = 5,         ///< 光晕
            T_NORMALMAP = 6,    ///< 法线纹理
            T_HIGHLIGHT = 7,    ///< 高光相片(放射性光体除了用片段着色程序,也可以用图片模拟)
            T_Count = 8         ///< 类型总数
        };

        typedef vector<ShaderChTextureUnit *>::type Textures;
    public:
        ShaderChTexture(ShaderCh * ch);
        ShaderChTexture(ShaderCh * ch, const ShaderChTexture & o);
        ~ShaderChTexture();

        ShaderChTexture & operator =(const ShaderChTexture & o);

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
        ShaderChTextureUnit * create(Type type = T_NORMAL);

        /** 创建一个应用纹理
        @param[in] rid 资源ID(应用程序中唯一)
        @param[in] slot
            纹理套口,基本渲染系统一般支持同一时刻渲染多个纹理,默认只有一个,所以默认为0,
            此处的纹理套口并不是直接能设置 Type 的所有类型, Type 的意义并不和纹理套口
            相同
        @param[in] type 纹理类型
        @version NIIEngine 3.0.0
        */
        ShaderChTextureUnit * create(ResourceID rid, Nidx slot = 0, Type type = T_NORMAL);

        /** 创建一个应用纹理
        @param[in] tex 纹理对象
        @param[in] slot
            纹理套口,基本渲染系统一般支持同一时刻渲染多个纹理,默认只有一个,所以默认为0,
            此处的纹理套口并不是直接能设置 Type 的所有类型, Type 的意义并不和纹理套口
            相同
        @param[in] type 纹理类型
        @version NIIEngine 3.0.0
        */
        ShaderChTextureUnit * create(const Texture * tex, Nidx slot = 0, Type type = T_NORMAL);

        /** 添加的应用纹理单元
        @param[in] unit
        @version NIIEngine 3.0.0
        */
        void add(ShaderChTextureUnit * unit);

        /** 获取应用纹理单元
        @param[in] index
        @version NIIEngine 3.0.0
        */
        ShaderChTextureUnit * get(Nidx index) const;

        /** 获取应用纹理
        @param[in] name 纹理单元名字
        @version NIIEngine 3.0.0
        */
        ShaderChTextureUnit * get(const String & name) const;

        /** 获取类型第N个纹理.
        @param[in] type 类型
        @param[in] index 索引
        @version NIIEngine 3.0.0
        */
        ShaderChTextureUnit * get(Type type, Nidx index) const;

        /** 获取内容类型纹理迭代
        @version NIIEngine 3.0.0 高级api
        */
        ShaderChTextureUnit * get(Type type, Textures::const_iterator & begin, Textures::const_iterator & end) const;

        /** 移动应用纹理到另一个应用纹理中
        @remark Type([begin, end])的纹理移动到参数o中
        @param[in] type 类型
        @param[in] begin 开始索引
        @param[in] end 结束索引
        @param[in] o 另一个应用纹理集
        @note 这里指的是剪切粘贴,并非复制
        @version NIIEngine 3.0.0 高级api
        */
        void move(Type type, Nidx begin, Nidx end, ShaderChTexture * o);

        /** 移去应用纹理
        @param[in] index
        @version NIIEngine 3.0.0
        */
        void remove(Nidx index);

        /** 移去应用纹理
        @param[in] type
        @param[in] index
        @version NIIEngine 3.0.0
        */
        void remove(Type type, Nidx index);

        /** 移去所有应用纹理
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** 指定类型纹理数量
        @version NIIEngine 3.0.0
        */
        NCount getCount(Type type) const;

        /** 应用纹理数量
        @remark 含概所有类型
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** 获取应用纹理列表
        @remark 这个函数会直接影响整个应用纹理,注意操作限制
        @version NIIEngien 3.0.0 高级api
        */
        const Textures & getTextures() const;
    protected:
        ShaderChTexture();
    private:
        ShaderCh * mParent;
        Textures mTextures;
        Nui8 mMark[T_Count];
    };

    /** 基本纹理单元
    @remark
        和Texture类不同,这个类有基础状态.涉及到纹理距阵/环境映射的在可编程管线中没有效果
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChTextureUnit : public ShaderAlloc
    {
        friend class RenderSys;
    public:
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
    public:
        ShaderChTextureUnit(ShaderCh * ch);
        ShaderChTextureUnit(ShaderCh * ch, ResourceID rid);
        ShaderChTextureUnit(ShaderCh * ch, const ShaderChTextureUnit & o);
        virtual ~ShaderChTextureUnit();

        ShaderChTextureUnit & operator = (const ShaderChTextureUnit & o);

        /** 获取通道
        @version NIIEngine 3.0.0
        */
        ShaderCh * getParent() const;

        /** 设置纹理单元的名字
        @remark 辅助设置
        @version NIIEngine 3.0.0
        */
        void setName(const String & name);

        /** 获取单元名字
        @remark 辅助设置
        @version NIIEngine 3.0.0
        */
        const String & getName() const;

        /** 设置来源
        @note T_CUBE 纹理需要设置 setEnvMap(true) 和 setEnvMapType(ENM_Sphere).
        @version NIIEngine 3.0.0
        */
        void setSrc(const ResourceIDList & rid, Texture::Type type, ShaderChTexture::Type ctype = ShaderChTexture::T_NORMAL);

        /** 设置来源
        @note T_CUBE 纹理需要设置 setEnvMap(true) 和 setEnvMapType(ENM_Sphere).
        @version NIIEngine 3.0.0
        */
        void setSrc(ResourceID rid, const StringList & src, Texture::Type type, ShaderChTexture::Type ctype = ShaderChTexture::T_NORMAL);

        /** 设置来源
        @note T_CUBE 纹理需要设置 setEnvMap(true) 和 setEnvMapType(ENM_Sphere).
        @version NIIEngine 3.0.0
        */
        void setSrc(const Texture * tex, Texture::Type type, ShaderChTexture::Type ctype = ShaderChTexture::T_NORMAL);

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
        const Texture * getTexture() const;

        /** 获取帧纹理
        @version NIIEngine 3.0.0
        */
        const Texture * getTexture(NCount frame) const;

        /** 设置当前帧的纹理指针
        @version NIIEngine 3.0.0
        */
        void setTexture(const Texture * tex);

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
        ResourceID getFrameFusion() const;

        /** 获取合成纹理
        @version NIIEngine 3.0.0
        */
        const String & getFusionTexture() const;

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
        const EffectMap & getEffects() const;

        /** 动画时长
        @remark 动画纹理专用
        @version NIIEngine 3.0.0
        */
        TimeDurMS getAniTime() const;

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
        void setEnvMapType(EnvMapMode type);

        /** 获取环境映射类型
        @note 在可编程管线中没有效果
        @version NIIEngine 3.0.0
        */
        EnvMapMode getEnvMapType() const;

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
        void setProjFrustum(Frustum * proj);

        /** 获取投影纹理的投影空间
        @note 在可编程管线中没有效果
        @version NIIEngine 3.0.0
        */
        const Frustum * getProjFrustum() const;

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
        @param[in] frame The frame the texture name is to be placed in
        @note 固定管线和可编程管线中有效果
        @version NIIEngine 3.0.0
        */
        void setFrame(ResourceID rid, NCount frame);

        /** 获取关联一个帧数量的纹理名字. 抛出一个异常,如果frameNumber超过了存储帧的数量.
        @note 固定管线和可编程管线中有效果
        @version NIIEngine 3.0.0
        */
        ResourceID getFrame(NCount frame) const;

        /** 为一纹理获取帧数量.
        @note 固定管线和可编程管线中有效果
        */
        NCount getFrameCount() const;

        /** 设置活动帧
        @remark 设置活动帧
        @note 固定管线和可编程管线中有效果
        */
        void setCurrentFrame(NCount frame);

        /** 获取活动帧
        @note 固定管线和可编程管线中有效果
        @version NIIEngine 3.0.0
        */
        NCount getCurrentFrame() const;

        /** 设置这个层使用纹理坐标集的索引
        @note 默认0,除非使用多纹理多纹理坐标
        @note 固定管线和可编程管线中有效果
        @version NIIEngine 3.0.0
        */
        void setCoordSet(Nidx index);

        /** 获取这个层使用纹理坐标集的索引
        @note 固定管线和可编程管线中有效果
        @version NIIEngine 3.0.0
        */
        Nidx getCoordSet() const;

        /** 设置纹理数据检索类型
        @version NIIEngine 3.0.0 高级api
        */
        void setFetchType(TextureDataFetch bt);

        /** 获取纹理数据检索类型
        @verison NIIEngien 3.0.0 高级api
        */
        TextureDataFetch getFetchType() const;

        /** 设置内容的类型
        @remark
        @version NIIEngine 3.0.0
        */
        void setContentType(ShaderChTexture::Type type);

        /** 获取内容的类型
        @version NIIEngine 3.0.0
        */
        ShaderChTexture::Type getContentType() const;

        /** 获取是否由6个面元组成
        @note 固定管线和可编程管线中都有效
        */
        bool isCubic() const;

        /** 返回这个纹理的类型.
        @note 应用到固定管线和可编程管线
        @version NIIEngine 3.0.0
        */
        Texture::Type getType() const;

        /** 设置像素格式
        @remark 期望
        @version NIIEngine 3.0.0
        */
        void setFormat(PixelFormat pf);

        /** 获取像素格式
        @remark 期望
        @version NIIEngine 3.0.0
        */
        PixelFormat getFormat() const;

        /** 设置
        @remark 期望
        @version NIIEngine 3.0.0
        */
        void setMipmapCount(NIIi count);

        /** 获取
        @remark 期望
        @version NIIEngine 3.0.0
        */
        NIIi getMipmapCount() const;

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
        void setLocalIndex(Nidx index);

        /** 获取(纹理集)中的索引
        @version NIIEngine 3.0.0 高级api
        */
        Nidx getLocalIndex() const;

        /** 设置复合中的索引
        @version NIIEngine 3.0.0 高级api
        */
        void setMultiIndex(Nidx index);

        /** 获取复合中的索引
        @version NIIEngine 3.0.0 高级api
        */
        Nidx getMultiIndex() const;

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
        NIIf getUOft() const;

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
        NIIf getVOft() const;

        /** 设置U方向缩放
        @note 在可编程管线中没有效果
        @version NIIEngine 3.0.0
        */
        void setUScale(NIIf value);

        /** 获取U方向缩放
        @version NIIEngine 3.0.0
        */
        NIIf getUScale() const;

        /** 设置V方向缩放
        @note 在可编程管线中没有效果
        @version NIIEngine 3.0.0
        */
        void setVScale(NIIf value);

        /** 获取V方向缩放
        @version NIIEngine 3.0.0
        */
        NIIf getVScale() const;

        /** 设置应用在纹理坐标的逆时针旋转因子
        @note 在可编程管线中没有效果
        @param[in] angle 旋转角度(逆时针)
        */
        void setRotate(const Radian & r);

        /** 获取纹理旋转效果angle值
        @version NIIEngine 3.0.0
        */
        const Radian & getRotate() const;

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
        inline TextureBlend * getColourBlend() const { return const_cast<TextureBlend *>(&mColourBlend); }

        /** 获取多纹理透明度混合
        @version NIIEngine 3.0.0 高级api
        */
        inline TextureBlend * getAlphaBlend() const { return const_cast<TextureBlend *>(&mAlphaBlend); }

        /** 获取纹理样本模式
        @version NIIEngien 3.0.0 高级api
        */
        inline TextureSample * getSample() const { return const_cast<TextureSample *>(&mSample); }

        /** 状态/性质变化了
        @version NIIEngine 3.0.0
        */
        void notify();

        /** 主对象改变了
        @version NIIEngine 3.0.0
        */
        void notify(ShaderCh * ch);

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
        ShaderCh * mParent;
        DataEquation<TimeDurMS, NIIf> * mTimeFunc;
        String mName;
        ResourceID mFrameFusion;            ///< 帧合成
        String mFusionTex;                  ///< 合成纹理
        TimeDurMS mAnimDuration;            ///< 动画长度(单位:毫秒)
        Texture::Type mTextureType;         ///< 纹理类型
        NCount mCurrentFrame;               ///< 当前动画帧
        ShaderChTexture::Type mContentType; ///< 纹理内容类型
        TextureDataFetch mFetchType;        ///< 数据检索模式
        TextureAddressing mAddressMode;     ///< 纹理地址模式
        EnvMapMode mEnvMapType;             ///< 环境映射类型
        Frustum * mProjFrustum;             ///< 纹理投影空间
        PixelFormat mFormat;                ///< 像素格式
        NIIi mMipmapCount;                  ///< 层次数量
        EffectMap mEffects;
        Nui16 mCoordSet;
        Nui16 mLocalIndex;
        Nui16 mMultiIndex;
        TextureSample mSample;
        TextureBlend mColourBlend;
        TextureBlend mAlphaBlend;

        NIIf mUOft, mVOft, mUScale, mVScale;
        Radian mRotate;
        mutable Matrix4f mMatrix;
        ResourceIDList mTexIDList;
        mutable TextureList mTextures;
        mutable Nmark mMark;
    };
}
#endif