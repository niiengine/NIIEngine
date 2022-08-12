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

#ifndef _NII_RENDERSYS_H_
#define _NII_RENDERSYS_H_

#include "NiiPreInclude.h"
#include "NiiSysOption.h"
#include "NiiDriverVersion.h"
#include "NiiExtFrameBuffer.h"
#include "NiiGeometryRaw.h"
#include "NiiGPUInfo.h"
#include "NiiShaderCh.h"
#include "NiiCommon.h"
#include "NiiDataType.h"
#include "NiiPlane.h"

#define Independent N_StrBlank

#define N_CategoryGroupValue(cnt, grp, val) ((grp << (48 - cnt)) | (1 << val))

namespace NII
{
    /** 类别组
    @version NIIEngine 3.0.0
    */
    enum GpuFeatureGroup
    {
        CC_Common1 = 0,
        CC_Common2 = 1,
        CC_Ext1 = 2,
        CC_D3D = 3,
        CC_GL = 4,
        CC_Spec3 = 5,
        CC_Spec4 = 6,
        CC_Count = 7
    };

    /** Gpu特性
    @version NIIEngine 3.0.0
    */
    enum GpuFeature
    {
        GF_ClipRect = N_CategoryGroupValue(CC_Count, CC_Common1,            0),
        GF_ClipPlanes = N_CategoryGroupValue(CC_Count, CC_Common1,          1),
        GF_Infinite_FarPlane = N_CategoryGroupValue(CC_Count, CC_Common1,   2),

        GF_Tex_AutoMipmapCompress = N_CategoryGroupValue(CC_Count, CC_Common1, 3),
        GF_Tex_Anisotropy = N_CategoryGroupValue(CC_Count, CC_Common1,      4),
        GF_Tex_Compress = N_CategoryGroupValue(CC_Count, CC_Common1,        5),
        GF_Tex_1D = N_CategoryGroupValue(CC_Count, CC_Common1,              6),
        GF_Tex_3D = N_CategoryGroupValue(CC_Count, CC_Common1,              7),
        GF_Tex_Dot3 = N_CategoryGroupValue(CC_Count, CC_Common1,            8),
        GF_Tex_Float = N_CategoryGroupValue(CC_Count, CC_Common1,           9),
        GF_Tex_NonPow2 = N_CategoryGroupValue(CC_Count, CC_Common1,         10),
        GF_Tex_MipMapBias = N_CategoryGroupValue(CC_Count, CC_Common1,      11),

        GF_Program_Vertex = N_CategoryGroupValue(CC_Count, CC_Common1,      12),
        GF_Program_Fragment = N_CategoryGroupValue(CC_Count, CC_Common1,    13),
        GF_Program_Geometry = N_CategoryGroupValue(CC_Count, CC_Common1,    14),
        GF_Program_Tess = N_CategoryGroupValue(CC_Count, CC_Common1,        15),
        GF_Program_Domain = N_CategoryGroupValue(CC_Count, CC_Common1,      16),
        GF_Program_Compute = N_CategoryGroupValue(CC_Count, CC_Common1,     17),
        GF_Program_VertexTexFetch = N_CategoryGroupValue(CC_Count, CC_Common1, 18),
        GF_Fixed_Pipeline = N_CategoryGroupValue(CC_Count, CC_Common1,      19),

        GF_Point_Param = N_CategoryGroupValue(CC_Count, CC_Common1,         20),
        GF_Point_Sprites = N_CategoryGroupValue(CC_Count, CC_Common1,       21),
        GF_LineWide = N_CategoryGroupValue(CC_Count, CC_Common1,            22),

        GF_HW_Stencil = N_CategoryGroupValue(CC_Count, CC_Common1,          23),
        GF_HW_GAMMA = N_CategoryGroupValue(CC_Count, CC_Common1,            24),
        GF_HW_Buffer = N_CategoryGroupValue(CC_Count, CC_Common1,           25),
        GF_HW_PixelQuery = N_CategoryGroupValue(CC_Count, CC_Common1,       26),

        GF_Vertex_UByte4 = N_CategoryGroupValue(CC_Count, CC_Common1,       27),
        GF_Vertex_Instance = N_CategoryGroupValue(CC_Count, CC_Common1,     28),
        GF_Index_32 = N_CategoryGroupValue(CC_Count, CC_Common1,            29),

        GF_Stencil_BothSide = N_CategoryGroupValue(CC_Count, CC_Common1,    30),
        GF_Stencil_Wrap = N_CategoryGroupValue(CC_Count, CC_Common1,        31),
        GF_Colour_Blend = N_CategoryGroupValue(CC_Count, CC_Common1,        32),
        GF_WBuffer = N_CategoryGroupValue(CC_Count, CC_Common1,             33),
        GF_Debug = N_CategoryGroupValue(CC_Count, CC_Common1,               34),
        CF_PersistentMapping = N_CategoryGroupValue(CC_Count, CC_Common1,   35),
        CF_InstanceBase = N_CategoryGroupValue(CC_Count, CC_Common1,        36),
        CF_IndirectBuffer = N_CategoryGroupValue(CC_Count, CC_Common1,      37),

        GF_Alpha_Coverage = N_CategoryGroupValue(CC_Count, CC_Common2,      0),
        GF_Render_To_Texture = N_CategoryGroupValue(CC_Count, CC_Common2,   1),
        GF_Render_To_Vertex = N_CategoryGroupValue(CC_Count, CC_Common2,    2),
        GF_Tex_Fromat_DXT = N_CategoryGroupValue(CC_Count, CC_Common2,      3),
        GF_Tex_Fromat_VTC = N_CategoryGroupValue(CC_Count, CC_Common2,      4),
        GF_Tex_Fromat_PVRTC = N_CategoryGroupValue(CC_Count, CC_Common2,    5),
        GF_Tex_Fromat_ETC2 = N_CategoryGroupValue(CC_Count, CC_Common2,     6),
        GF_Tex_Fromat_BC4_5 = N_CategoryGroupValue(CC_Count, CC_Common2,    7),
        GF_Tex_Fromat_BC6H_7 = N_CategoryGroupValue(CC_Count, CC_Common2,   8),
        GF_Tex_Fromat_ASTC = N_CategoryGroupValue(CC_Count, CC_Common2,     9),
        GF_Shader_Binary_Get = N_CategoryGroupValue(CC_Count, CC_Common2,   10),

        GF_RenderTarget_LessDepth = N_CategoryGroupValue(CC_Count, CC_Ext1,         0),
        GF_RenderTarget_AttachMainDepth = N_CategoryGroupValue(CC_Count, CC_Ext1,   1),
        GF_MultiRenderTarget_EachDepthBit = N_CategoryGroupValue(CC_Count, CC_Ext1, 2),

        GF_D3D = N_CategoryGroupValue(CC_Count, CC_D3D,                     0),
        GF_D3D9 = N_CategoryGroupValue(CC_Count, CC_D3D,                    1),
        GF_D3D11 = N_CategoryGroupValue(CC_Count, CC_D3D,                   2),
        GF_D3D12 = N_CategoryGroupValue(CC_Count, CC_D3D,                   3),
        GF_D3D_PerStageConstant = N_CategoryGroupValue(CC_Count, CC_D3D,    4),
        GF_GL = N_CategoryGroupValue(CC_Count, CC_GL,                       0),
        GF_GL_3_1 = N_CategoryGroupValue(CC_Count, CC_GL,                   1),
        GF_GL_4_1 = N_CategoryGroupValue(CC_Count, CC_GL,                   2),
        GF_GL_ES = N_CategoryGroupValue(CC_Count, CC_GL,                    3),
        GF_GL_ES_1_2 = N_CategoryGroupValue(CC_Count, CC_GL,                4),
        GF_GL_ES_2_1 = N_CategoryGroupValue(CC_Count, CC_GL,                5),
        GF_GL_ES_3_1 = N_CategoryGroupValue(CC_Count, CC_GL,                6),
        GF_GL_PBuffer = N_CategoryGroupValue(CC_Count, CC_GL,               7),
        GF_GL_VArrayObj = N_CategoryGroupValue(CC_Count, CC_GL,             8),
        GF_GLSL_SeparateShaderObject = N_CategoryGroupValue(CC_Count, CC_GL, 9)
    };

    /** 渲染系统特性
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI RenderFeature : public SysAlloc
    {
    public:
        RenderFeature();
        virtual ~RenderFeature();

        /** 获取Gpu信息
        @version NIIEngine 3.0.0
        */
        inline const GPUInfo * getGpuInfo() const           { return &mGPUInfo; }

        /** 添加特性支持
        @version NIIEngine 3.0.0
        */
        void add(GpuFeature c);

        /** 移去特性支持
        @version NIIEngine 3.0.0
        */
        void remove(GpuFeature c);

        /** 是否特性支持
        @version NIIEngine 3.0.0
        */
        bool isSupport(GpuFeature c) const;

        /** 是否特定渲染系统特性
        @version NIIEngine 3.0.0
        */
        bool isSpecFeature(GpuFeature c) const;

        /** 添加着色程序支持
        @version NIIEngine 3.0.0
        */
        inline void add(ShaderLanguageMark m)               { mSLMark |= m; }

        /** 移去着色程序支持
        @version NIIEngine 3.0.0
        */
        inline void remove(ShaderLanguageMark m)            { mSLMark &= ~m; }

        /** 是否支持着色程序
        @version NIIEngine 3.0.0
        */
        inline bool isShaderSupport(ShaderLanguage m) const { return mSLMark & m; }

        /** 获取支持着色程序(集合)
        @version NIIEngine 3.0.0
        */
        inline ShaderLanguageMark getShaderMark() const     { return mSLMark; }

        /** 获取名称
        @version NIIEngine 3.0.0
        */
        inline const String & getName() const               { return mName; }

        /** 获取多纹理采样最大数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxTexUnit() const                  { return mMaxTextureUnit; }

        /** 获取点的最大大小
        @remark 像素
        @version NIIEngine 3.0.0
        */
        inline NIIf getMaxPointSize() const { return mMaxPointSize; }

        /** 获取支持的同步渲染目标数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxMultiFrameObj() const { return mMaxMultiFrameObj; }

        /** 获取最大空间数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxSpace() const { return mMaxSpace; }

        /** 获取硬件模板位深
        @version NIIEngine 3.0.0
        */
        inline Nui16 getStencilBit() const { return mStencilBit; }

        /** 获取顶点纹理最大数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxVertexTexUnit() const { return mMaxVertexTexUnit; }

        /** 获取顶点属性最大数量
        @version NIIEngine 3.0.0
        */
        inline void setMaxVertexAttrib(Nui16 num) { mMaxVertexAttrib = num; }

        /** 获取顶点属性最大数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxVertexAttrib() const { return mMaxVertexAttrib; }

        /** 设置最大的各向异性
        @version NIIEngine 4.0.0
        */
        inline void setMaxAnisotropy(NIIf s) { mMaxSupportedAnisotropy = s; }

        /** 获取最大的各向异性
        @version NIIEngine 4.0.0
        */
        inline NIIf getMaxAnisotropy() const { return mMaxSupportedAnisotropy; }

        /** 获取是否仅2次幂纹理
        @version NIIEngine 3.0.0
        */
        inline bool isOnlyPow2Texture() const { return mOnlyPow2Texture; }

        /** 设置顶点着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxVPFloatParamCount() const { return mMaxVPParam[0]; }

        /** 设置顶点着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxVPIntParamCount() const { return mMaxVPParam[2]; }

        /** 设置顶点着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxVPBoolParamCount() const { return mMaxVPParam[3]; }

        /** 获取片段着色程序支持floating常量数目
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxFPFloatCount() const { return mMaxFPParam[0]; }

        /** 获取片段着色程序支持integer常量数目
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxFPIntCount() const { return mMaxFPParam[2]; }

        /** 获取片段着色程序支持boolean常量数目
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxFPBoolCount() const { return mMaxFPParam[3]; }

        /** 获取几何着色程序支持float常量数目
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxGPFloatCount() const { return mMaxGPParam[0]; }

        /** 获取几何着色程序支持int常量数目
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxGPIntCount() const { return mMaxGPParam[2]; }

        /** 获取几何着色程序支持bool常量数目
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxGPBoolCount() const { return mMaxGPParam[3]; }

        /** 获取几何程序输出顶点最大数量
        @version NIIEngine 3.0.0
        */
        inline NIIi getMaxGeometryOutput() const { return mMaxGPOutput; }

        /** 输出细节
        @version NIIEngine 3.0.0
        */
        void log(Log * log);
    public:
        /** 设置名称
        @version NIIEngine 3.0.0
        */
        inline void setName(const String & rs) { mName = rs; }

        /** 设置多纹理采样最大数量
        @version NIIEngine 3.0.0 高级api
        */
        inline void setMaxTexUnit(Nui16 num) { mMaxTextureUnit = num; }

        /** 设置点的最大大小
        @remark 像素
        @version NIIEngine 3.0.0 高级api
        */
        inline void setMaxPointSize(NIIf s) { mMaxPointSize = s; }

        /** 设置支持同步渲染目标的数目
        @version NIIEngine 3.0.0
        */
        void setMaxMultiFrameObj(Nui16 num) { mMaxMultiFrameObj = num; }

        /** 设置最大空间数量
        @version NIIEngine 3.0.0
        */
        void setMaxSpace(Nui16 num) { mMaxSpace = num; }

        /** 设置硬件模板位深
        @version NIIEngine 3.0.0
        */
        void setStencilBit(Nui16 num) { mStencilBit = num; }

        /** 设置顶点纹理最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxVertexTexUnit(Nui16 n) { mMaxVertexTexUnit = n; }

        /** 设置几何程序输出顶点最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxGeometryOutput(NIIi count) { mMaxGPOutput = count; }

        /** 设置是否仅2次幂纹理
        @version NIIEngine 3.0.0
        */
        void setOnlyPow2Texture(bool l) { mOnlyPow2Texture = l; }

        /** 设置顶点着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxVertexProgramParam(Nui16 c);

        /** 设置顶点着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxVertexProgramParam(Nui16 f, Nui16 d, Nui16 i, Nui16 b);

        /** 设置片段着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxFragmentProgramParam(Nui16 c);

        /** 设置片段着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxFragmentProgramParam(Nui16 f, Nui16 d, Nui16 i, Nui16 b);

        /** 设置几何着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxGeometryProgramParam(Nui16 c);

        /** 设置几何着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxGeometryProgramParam(Nui16 f, Nui16 d, Nui16 i, Nui16 b);

        /** 设置细化着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxTessControlProgramParam(Nui16 f, Nui16 d, Nui16 i, Nui16 b);

        /** 设置细化着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxTessControlProgramParam(Nui16 c);
        
        /** 设置细化着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxDomainProgramParam(Nui16 f, Nui16 d, Nui16 i, Nui16 b);

        /** 设置细化着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxDomainProgramParam(Nui16 c);
        
        /** 设置计算着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxComputeProgramParam(Nui16 f, Nui16 d, Nui16 i, Nui16 b);

        /** 设置计算着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxComputeProgramParam(Nui16 c);
    private:
        String mName;
        GPUInfo mGPUInfo;
        ShaderLanguageMark mSLMark;
        Nmark64 mFeature[CC_Count];
        NIIf mMaxSupportedAnisotropy;
        NIIf mMaxPointSize;
        NIIi mMaxGPOutput;
        Nui16 mMaxMultiFrameObj;
        Nui16 mMaxVertexTexUnit;
        Nui16 mMaxTextureUnit;
        Nui16 mMaxVertexAttrib;
        Nui16 mMaxSpace;
        Nui16 mStencilBit;
        Nui16 mMaxVPParam[4];
        Nui16 mMaxFPParam[4];
        Nui16 mMaxGPParam[4];
        Nui16 mMaxTPParam[4];
        Nui16 mMaxDPParam[4];
        Nui16 mMaxCPParam[4];
        bool mOnlyPow2Texture;
    };
    
    /** 平台特定数据
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI RenderSysData
    {
    public:
        enum Type
        {
            T_Unknow        = 0,
            T_DX9_0         = 0x01,
            T_DX10_0        = 0x02,
            T_DX11_0        = 0x04,
            T_DX12_0        = 0x08,
            T_OPENGL1_1     = 0x10,
            T_OPENGL2_1     = 0x20,
            T_OPENGL3_3     = 0x30,
            T_OPENGL4_3     = 0x40,
            T_OPENGL4_6     = 0x80,
            T_OPENGL_ES1_1  = 0x100,
            T_OPENGL_ES2_1  = 0x200,
            T_OPENGL_ES3_1  = 0x400,
            T_DIRECT_FB     = 0x800,

            T_WIN32         = 0x100000,
            T_WINRT         = 0x200000,     ///< win8编程框架
            T_WIN8_PHONE    = 0x400000,
            T_LINUX         = 0x800000,     ///< linux内核
            T_LINUX_X       = 0x1000000,    ///< x窗体级别的linux系统
            T_OSX           = 0x2000000,    ///< 苹果mac os x(darwin)
            T_IOS           = 0x4000000,    ///< 苹果iphone(darwin)
            T_ANDROID       = 0x8000000,    ///< 安桌系统(linux内核级)
            T_EMSCRIPTEN    = 0x10000000,   ///< web编程框架
            T_FREEBSD       = 0x20000000,   ///< freeBSD
        };
    public:
        /** 获取平台额外信息
        @param[in] name 需要获取的属性名字
        @param[in] data 指向存储获取内容的内存指针
        @return 是否获取成功
        @note 一般在最底端接口重写这个函数
        @version NIIEngien 3.0.0 高级api
        */
        virtual bool getPlatformData(const String & name, void * data) const;

        /** 获取平台额外信息
        @param[in] name 需要获取的属性名字
        @param[in] data 指向存储获取内容的内存指针
        @return 组合类型
        @note 一般在最底端接口重写这个函数
        @version NIIEngien 3.0.0 高级api
        */
        virtual Nmark getPlatformType() const;
    };

    typedef vector<ViewWindow *>::type WindowList;
    /** 渲染系统
    @note 部分函数很接近底层
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderSys : public SysAlloc
    {
        friend class Engine;
        friend class SpaceManager;
    public:
        typedef map<String, FrameObj *>::type FrameList;
        typedef multimap<GroupID, FrameObj *>::type FrameLevelList;
    public:
        RenderSys();
        virtual ~RenderSys();

        /** 初始化系统
        @version NIIEngine 3.0.0
        */
        virtual void init();

        /** 重新初始化系统
        @version NIIEngine 3.0.0
        */
        virtual void reinit() = 0;

        /** 关闭系统
        @version NIIEngine 3.0.0
        */
        virtual void shutdown();

        /** 返回渲染系统名字.
        @version NIIEngine 3.0.0
        */
        virtual const String & getName() const = 0;

        /** 构建配置
        @version NIIEngine 3.0.0
        */
        virtual void setupConfig();

        /** 应用/检测自定义属性
        @version NIIEngine 3.0.0
        */
        virtual String checkConfig() = 0;

        /** 设置自定义属性
        @param[in] name 名字
        @param[in] value 值
        @version NIIEngine 3.0.0
        */
        virtual bool setConfig(const String & name, const String & value);

        /** 设置自定义属性
        @param[in] name 名字
        @param[in] value 值
        @version NIIEngine 3.0.0
        */
        void addConfig(PropertyID pid, const String & name, const String & value);

        /** 获取自定义属性集
        @version NIIEngine 3.0.0
        */
        inline const SysOptionList & getConfig() const { return mOptions; }

        /** 获取自定义属性集数据
        @version NIIEngine 3.0.0
        */
        inline const PropertyData * getConfigData() const { return &mOptionData; }

        /** 构建渲染系统特性
        @version NIIEngine 3.0.0
        */
        virtual void setupFeature() = 0;

        /** 设置自定义渲染系统特性
        @remark 参数内存交由本类管理
        @version NIIEngine 3.0.0 高级api
        */
        virtual void setCustomFeature(RenderFeature * rsf);

        /** 获取渲染系统能力
        @version NIIEngine 3.0.0
        */
        inline RenderFeature * getFeature() const { N_assert(mFeature, "SYS Uninitialized");  return mFeature; }

        /** 构建固定编程管线着色程序参数
        @version NIIEngine 5.0.0
        */
        virtual void setupFFPParam();

        /** 获取固定编程管线着色程序参数
        @verison NIIEngine 5.0.0
        */
        inline GpuProgramParam * getFFPParam() const { N_assert(mFFGPP, "SYS Uninitialized"); return mFFGPP; }

        /** 设置动态灯光
        @version NIIEngine 3.0.0
        */
        virtual void setLight(bool b) = 0;

        /** 设置光体计算模式
        @version NIIEngine 3.0.0
        */
        virtual void setShadingType(ShadeMode sm) = 0;

        /** 设置环境(灯光)颜色
        @version NIIEngine 3.0.0 高级api
        */
        virtual void setAmbient(const Colour & c) = 0;

        /** 设置是否使用同步垂直
        @remark Has NO effec
        @version NIIEngine 3.0.0 高级api
        */
        void setVSync(bool b);

        /** 获取是否使用同步垂直
        @version NIIEngine 3.0.0 高级api
        */
        bool isVSync() const;

        /** 设置是否使用深度缓存
        @version NIIEngine 3.0.0 高级api
        */
        void setWBuffer(bool b);

        /** 获取是否使用深度缓存
        @version NIIEngine 3.0.0 高级api
        */
        bool getWBuffer() const;

        /** 设置投影矩阵
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _setProj(const Matrix4f & m) = 0;

        /** 设置视图矩阵
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _setView(const Matrix4f & m) = 0;

        /** 设置模型矩阵
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _setModel(const Matrix4f & m) = 0;

        /** 设置几何空间矩阵
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _setSpace(const Matrix4f * m, NCount cnt);

        /** 设置动态灯光
        @version NIIEngine 3.0.0
        */
        virtual void _setLight(NCount cnt) = 0;

        /** 创建一个新的渲染窗体
        @param[in] w 窗体的宽
        @param[in] h 窗体的高
        @version NIIEngine 3.0.0
        */
        virtual ViewWindow * createView(const String & name, NCount w, NCount h, bool full, const PropertyData * ext = 0) = 0;

        /** 加入渲染帧对象
        @version NIIEngine 3.0.0
        */
        virtual void add(FrameObj * obj);

        /** 移去渲染帧对象
        @version NIIEngine 3.0.0
        */
        virtual FrameObj * remove(const FrameObj * obj);

        /** 创建多个窗体
        @version NIIEngine 3.0.0
        */
        virtual bool createView(const WindowDefineList & dlist, WindowList & out);

        /** 创建复合渲染帧对象
        @version NIIEngine 3.0.0
        */
        virtual MultiTextureFrame * createMultiFrame(const String & name) = 0;

        /** 删除渲染帧对象
        @version NIIEngine 3.0.0
        */
        virtual void destroyFrame(const FrameObj * fo);

        /** 获取渲染帧对象
        @version NIIEngine 3.0.0
        */
        virtual FrameObj * getFrame(const String & name);

        /** 创建扩展缓存
        @version NIIEngine 3.0.0
        */
        virtual void createExtBuffer(FrameObj * obj);

        /** 删除扩展缓存
        @version NIIEngine 3.0.0
        */
        void destroyAllExtBuffer(bool all = true);

        /** 创建一个像素查询
        @version NIIEngine 3.0.0
        */
        virtual void create(PixelCountQuery *& obj) = 0;

        /** 删除一个像素查询
        @version NIIEngine 3.0.0
        */
        virtual void destroy(PixelCountQuery * obj);

        /** 设置顶点缓存
        @version NIIEngine 3.0.0
        */
        void setInstanceData(VertexBuffer * buf);

        /** 获取顶点缓存
        @version NIIEngine 3.0.0
        */
        VertexBuffer * getInstanceBuffer() const;

        /** 设置复用顶点
        @version NIIEngine 3.0.0
        */
        void setInstanceData(VertexData * vd);

        /** 获取复用顶点
        @version NIIEngine 3.0.0
        */
        VertexData * getInstanceData() const;

        /** 设置复用顶点次数
        @version NIIEngine 3.0.0
        */
        void setInstanceCount(NCount b);

        /** 获取复用顶点次数
        @version NIIEngine 3.0.0
        */
        NCount getInstanceCount() const;

        /** 设置视口类型
        @versin NIIEngine 4.0.0
        */
        virtual bool setViewportType(ViewportType type);

        /** 创建颜色属性
        @versin NIIEngine 4.0.0
        */
        virtual ShaderChColour * createColour(const String & name = Independent, bool Bindtosys = false) const;

        /** 获取颜色属性
        @versin NIIEngine 4.0.0
        */
        virtual ShaderChColour * getColour(const String & name) const;

        /** 创建渲染点的属性
        @version NIIEngine 3.0.0
        */
        virtual ShaderChPoint * createPoint(const String & name = Independent, bool Bindtosys = false) const;

        /** 创建渲染点的属性
        @version NIIEngine 3.0.0
        */
        virtual ShaderChPoint * getPoint(const String & name) const;

        /** 创建颜色缓存
        @version NIIEngine 3.0.0
        */
        virtual ShaderChBlend * createBlend(const String & name = Independent, bool Bindtosys = false) const;

        /** 创建颜色缓存
        @version NIIEngine 3.0.0
        */
        virtual ShaderChBlend * getBlend(const String & name) const;

        /** 创建透明度测试
        @version NIIEngine 3.0.0
        */
        virtual ShaderChAlpha * createAlpha(const String & name = Independent, bool Bindtosys = false) const;

        /** 创建透明度测试
        @version NIIEngine 3.0.0
        */
        virtual ShaderChAlpha * getAlpha(const String & name) const;

        /** 创建雾模式
        @version NIIEngine 3.0.0
        */
        virtual ShaderChFog * createFog(const String & name = Independent, bool Bindtosys = false) const;

        /** 创建雾模式
        @version NIIEngine 3.0.0
        */
        virtual ShaderChFog * getFog(const String & name) const;

        /** 创建深度测试
        @version NIIEngine 3.0.0
        */
        virtual ShaderChDepth * createDepth(const String & name = Independent, bool Bindtosys = false) const;

        /** 创建深度测试
        @version NIIEngine 3.0.0
        */
        virtual ShaderChDepth * getDepth(const String & name) const;

        /** 创建模板测试
        @version NIIEngine 3.0.0
        */
        virtual ShaderChStencil * createStencil(const String & name = Independent, bool Bindtosys = false) const;

        /** 创建模板测试
        @version NIIEngine 3.0.0
        */
        virtual ShaderChStencil * getStencil(const String & name) const;

        /** 创建纹理混合
        @version NIIEngine 3.0.0
        */
        virtual TextureBlend * createTexBlend(const String & name = Independent, bool Bindtosys = false) const;

        /** 获取纹理混合
        @version NIIEngine 3.0.0
        */
        virtual TextureBlend * getTexBlend(const String & name) const;

        /** 获取纹理样本
        @version NIIEngine 3.0.0
        */
        virtual TextureSampleUnit * createTexSample(const String & name = Independent, bool Bindtosys = false) const;

        /** 获取纹理样本
        @version NIIEngine 3.0.0
        */
        virtual TextureSampleUnit * getTexSample(const String & name) const;

        /** 设置当前颜色属性
        @version NIIEngine 3.0.0
        */
        virtual void setColour(const ShaderChColour & c, Nmark applymark = 0xFFFFFFFF) = 0;

        /** 设置当前颜色属性
        @version NIIEngine 3.0.0
        */
        void setColour(const String & name, Nmark applymark = 0xFFFFFFFF);

        /** 设置渲染点的属性
        @version NIIEngine 3.0.0
        */
        virtual void setPoint(const ShaderChPoint & p, Nmark applymark = 0xFFFFFFFF) = 0;

        /** 设置渲染点的属性
        @version NIIEngine 3.0.0
        */
        void setPoint(const String & name, Nmark applymark = 0xFFFFFFFF);

        /** 设置颜色缓存
        @version NIIEngine 3.0.0
        */
        virtual void setBlend(const ShaderChBlend & b, Nmark applymark = 0xFFFFFFFF) = 0;

        /** 设置颜色缓存
        @version NIIEngine 3.0.0
        */
        void setBlend(const String & name, Nmark applymark = 0xFFFFFFFF);

        /** 设置透明度测试
        @version NIIEngine 3.0.0
        */
        virtual void setAlpha(const ShaderChAlpha & a, Nmark applymark = 0xFFFFFFFF) = 0;

        /** 设置透明度测试
        @version NIIEngine 3.0.0
        */
        void setAlpha(const String & name, Nmark applymark = 0xFFFFFFFF);

        /** 设置雾模式
        @version NIIEngine 3.0.0
        */
        virtual void setFog(const ShaderChFog & f, Nmark applymark = 0xFFFFFFFF) = 0;

        /** 设置雾模式
        @version NIIEngine 3.0.0
        */
        void setFog(const String & name, Nmark applymark = 0xFFFFFFFF);

        /** 设置深度测试
        @version NIIEngine 3.0.0
        */
        virtual void setDepth(const ShaderChDepth & d, Nmark applymark = 0xFFFFFFFF) = 0;

        /** 设置深度测试
        @version NIIEngine 3.0.0
        */
        void setDepth(const String & name, Nmark applymark = 0xFFFFFFFF);

        /** 设置模版测试参数
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void setStencil(const ShaderChStencil & d, Nmark applymark = 0xFFFFFFFF) = 0;

        /** 设置模版测试
        @version NIIEngine 3.0.0
        */
        void setStencil(const String & name, Nmark applymark = 0xFFFFFFFF);

        /** 设置固定管线着色程序参数
        @version NIIEngine 4.0.0
        */
        virtual void setFFPParam(const GpuProgramParam & param, Nmark typemark) = 0;

        /** 设置深度测试(分离操作)
        @see ShaderChDepth
        @note 用于共面三角形的前后区分
        @version NIIEngine 3.0.0
        */
        virtual void _setDepthOpFunc(CmpMode func = CPM_LESS_EQUAL) = 0;

        /** 设置深度偏量(分离操作)
        @see ShaderChDepth
        @note 用于共面三角形的前后区分
        @version NIIEngine 3.0.0
        */
        virtual void _setDepthBias(NIIf bias, NIIf slopeScale = 0.0f) = 0;

        /** 设置深度偏量(分离操作)
        @param[in] derive 如果为真,深度偏差 = base + multi(current) * basefactor
        @see ShaderChDepth
        @note 用于共面三角形的前后区分
        @version NIIEngine 3.0.0
        */
        virtual void _setDepthBias(bool derive, NIIf base = 0.0f, NIIf basefactor = 0.0f, NIIf slopeScale = 0.0f);

        /** 获取深度缓存最小值
        @version NIIEngine 3.0.0
        */
        virtual NIIf getMinDepth() = 0;

        /** 获取深度缓存最大值
        @version NIIEngine 3.0.0
        */
        virtual NIIf getMaxDepth() = 0;

        /** 设置颜色掩码
        @param[in] mark ColourMark一个或多个组合
        @version NIIEngine 3.0.0
        */
        virtual void setColourWrite(Nmark mark) = 0;

        /** 设置模版测试
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void setStencilTest(bool set) = 0;

        /** 设置光栅模式
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setPolygonMode(PolygonMode m) = 0;

        /** 设置顶点饶向拣选模式
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setCullingMode(CullingMode m) = 0;

        /** 获取顶点饶向拣选模式
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual CullingMode _getCullingMode() const;

        /** 设置是否顶点反向绕
        @version NIIEngine 3.0.0
        */
        virtual void setInvertVertexWinding(bool b);

        /** 获取是否顶点反向绕
        @version NIIEngine 3.0.0
        */
        virtual bool isInvertVertexWinding();

        /** 绑定渲染帧
        @version NIIEngine 3.0.0
        */
        virtual void _setFrame(FrameObj * fo) = 0;

        /** 启用纹理
        @note 原始概念
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _enableTexture(Nidx index, const ShaderChTextureUnit * tl);

        /** 启用纹理单元数量
        @note 原始概念
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _enableTexture(NCount count);

        /** 关闭纹理单元
        @note 原始概念
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _disableTexture(Nidx index);

        /** 绑定纹理
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _bindTexture(Nidx index, ResourceID rid, bool enable);

        /** 绑定纹理
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _bindTexture(Nidx index, const Texture * tex, bool enable) = 0;

        /** 绑定纹理顶点
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _bindVertexTexture(Nidx index, const Texture * tex);

        /** 设置纹理单元使用的坐标
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setTextureCoordSet(Nidx index, Nidx setidx) = 0;

        /** 设置纹理的变形空间
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setTextureMatrix(Nidx index, const Matrix4f & space) = 0;

        /** 纹理混合模式
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setTextureBlend(Nidx index, const TextureBlend & tb) = 0;

        /** 纹理样本模式
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setTextureSample(Nidx indx, const TextureSampleUnit & ts) = 0;

        /** 设置纹理寻址计算
        @param[in] rel 投影(环境)纹理用
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setTextureCoordPrc(Nidx index, TextureCoordPrc m, const Frustum * rel = 0) = 0;

        /** 通知渲染系统,相对于一个不同原点,它应该调整纹理投影
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setTextureProjRel(bool enable, const Vector3f & pos);

        /** 获取实际纹理元素x偏移
        @version NIIEngine 3.0.0
        */
        virtual NIIf getTexelXOffset() = 0;

        /** 获取实际纹理元素y偏移
        @version NIIEngine 3.0.0
        */
        virtual NIIf getTexelYOffset() = 0;

        /** 设置法线是否自动单位化
        @version NIIEngine 3.0.0
        */
        virtual void setUnitNormals(bool set) = 0;

        /** 设置视口
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setViewport(Viewport * vp) = 0;

        /** 获取视口
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual Viewport * _getViewport();

        /** 清除缓存
        @version NIIEngine 3.0.0
        */
        virtual void clearBuffer(Nmark buffers, const Colour & colour = Colour::Black, NIIf depth = 1.0f, Nui16 stencil = 0) = 0;

        /** 设置裁剪区域
        @version NIIEngine 3.0.0
        */
        virtual void setScissorTest(bool enable, NCount left = 0, NCount top = 0, NCount right = 0, NCount bottom = 0) = 0;

        /** 设置裁剪区域
        @param[in] clip
        @version NIIEngine 3.0.0
        */
        virtual void setClipPlanes(const PlaneList & clip);

        /** 添加裁剪面
        @version NIIEngine 3.0.0
        */
        virtual void addClipPlane(const Plane & clip);

        /** 添加裁剪面
        @version NIIEngine 3.0.0
        */
        virtual void addClipPlane(NIIf a, NIIf b, NIIf c, NIIf d);

        /** 清除裁剪区域
        @version NIIEngine 3.0.0
        */
        virtual void resetClipPlanes();

        /** 设置是否固定管线模式
        @note 如果启用,将关闭可编程管线
        @version NIIEngine 3.0.0
        */
        void setFixedPipelineMode(bool b);

        /** 获取是否固定管线模式
        @note 如果启用,将关闭可编程管线
        @version NIIEngine 3.0.0
        */
        bool isFixedPipelineMode() const;

        /** 启用着色程序
        @version NIIEngine 3.0.0
        */
        virtual void on(GpuProgram * prg);

        /** 绑定着色程序参数
        @version NIIEngine 3.0.0
        */
        virtual void on(GpuProgram::ShaderType type, GpuProgramParam * params, Nmark range) = 0;

        /** 禁用指定类型着色程序
        @version NIIEngine 3.0.0
        */
        virtual void off(GpuProgram::ShaderType type);

        /*** 是否启用了指定类型着色程序
        @version NIIEngine 3.0.0
        */
        virtual bool isOn(GpuProgram::ShaderType type);

        /** 设置渲染次数(多渲染)
        @param[in] count 渲染当前状态的次数
        */
        virtual void setRenderCount(NCount count);

        /** 渲染几何元
        @note 把所有状态设置好了才渲染几何元
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _render(const GeometryRaw & op);

        /** 初始化帧对象内容
        @version NIIEngine 3.0.0
        */
        virtual void _initAllFrame();

        /** 更新帧对象内容
        @param[in] swap 向渲染系统交换显示内存
        @version NIIEngine 3.0.0
        */
        virtual void _updateAllFrame(bool swap = true);

        /** 向渲染系统交换显示内存
        @version NIIEngine 3.0.0
        */
        virtual void _swapAllFrame(bool vsync = true);

        /** 重至几何数量
        @version NIIEngine 3.0.0
        */
        virtual void _resetGeoCount();

        /** 开始帧
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _beginFrame() = 0;

        /** 暂停渲染
        @version NIIEngine 3.0.0 高级api
        */
        virtual FrameObj * _pauseFrame();

        /** 恢复渲染
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _resumeFrame(FrameObj * fo);

        /** 结束当前视口的每帧渲染
        @version NIIEngine 3.0.0
        */
        virtual void _endFrame() = 0;

        /** 转换成系统原始颜色的格式
        @note DT_Colour_ARGB 或 DT_Colour_ABGR 之前的转换
        @version NIIEngine 3.0.0
        */
        virtual void convertType(const Colour & src, Nui32 * dest);

        /** 获取渲染系统原始的颜色格式
        @note 返回 DT_Colour_ARGB 或 DT_Colour_ABGR
        @version NIIEngine 3.0.0
        */
        virtual DataType getColourType() const = 0;

        /** 获取渲染系统专用的投影矩阵
        @version NIIEngine 3.0.0
        */
        virtual void getSysProj(const Matrix4f & in, Matrix4f & out, bool gpuprog = false) = 0;

        /** 获取渲染系统专用的投影矩阵
        @version NIIEngine 3.0.0
        */
        virtual void getSysProj(const Radian & fovy, NIIf aspect, NIIf near, NIIf far, Matrix4f & out, bool gpuprog = false) = 0;

        /** 获取渲染系统专用的投影矩阵
        @version NIIEngine 3.0.0
        */
        virtual void getSysProj(NIIf l, NIIf r, NIIf b, NIIf t, NIIf near, NIIf far, Matrix4f & out,  bool gpuprog = false) = 0;

        /** 获取渲染系统专用的正交投影矩阵
        @version NIIEngine 3.0.0
        */
        virtual void getSysOrtho(const Radian & fovy, NIIf aspect, NIIf near, NIIf far, Matrix4f & out,  bool gpuprog = false) = 0;

        /** 推导渲染系统专用的倾斜深度投影
        @param[in] 输入和输出
        @version NIIEngine 3.0.0
        */
        virtual void getObliqueDepthProj(const Plane & plane, Matrix4f & io, bool gpuprog) = 0;

        /** 获取显示器的数量
        @version NIIEngine 3.0.0
        */
        virtual NCount getDisplayMonitorCount() const = 0;

        /** 获取系统材质方案
        @note opengl 和 d3d 至少在着色程序上有所区别
        @version NIIEngine 3.0.0
        */
        virtual SchemeID getMaterialScheme() const;

        /** 返回驱动版本
        @version NIIEngine 3.0.0
        */
        virtual const DriverVersion & getDriverVersion() const;

        /** 获取当前渲染顶点数量
        @version NIIEngine 3.0.0
        */
        virtual NCount getVertexCount() const;

        /** 获取当前渲染三角形数量
        @version NIIEngine 3.0.0
        */
        virtual NCount getTriangleCount() const;

        /** 获取当前渲染批次数量
        @version NIIEngine 3.0.0
        */
        virtual NCount getBatchCount() const;

        /** 错误码代表的字符串
        @version NIIEngine 3.0.0
        */
        virtual String getErrorString(NIIl num) const = 0;
    protected:
        /** 下一重复渲染
        @return 是否还需要重复渲染
        @version NIIEngine 3.0.0 高级api
        */
        virtual bool updateParam();

        /** 创建帧扩展缓存
        @version NIIEngine 3.0.0 高级api
        */
        virtual ExtFrameBuffer * createExtFrameImpl(FrameObj * dst) = 0;

        /** 设置裁减面
        @version NIIEngine 3.0.0 高级api
        */
        virtual void setClipPlanesImpl(const PlaneList & clip) = 0;

        /** 初始化渲染系统
        @version NIIEngine 3.0.0 高级api
        */
        virtual void initImpl(ViewWindow * primary) = 0;

        /** 通知摄象机被删除了
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _notifyDestroy(const Camera * cam);
    public:
        static const Matrix4f ProjClipSpace2DToImageSpace;
    protected:
        typedef list<PixelCountQuery *>::type PixelCountQueryList;
        typedef map<GroupID, ExtFrameBufferList>::type ExtFrameList;
        typedef map<String, ShaderChColour *>::type ColourList;
        typedef map<String, ShaderChPoint *>::type PointList;
        typedef map<String, ShaderChBlend *>::type BlendList;
        typedef map<String, ShaderChAlpha *>::type AlphaList;
        typedef map<String, ShaderChFog *>::type FogList;
        typedef map<String, ShaderChDepth *>::type DepthList;
        typedef map<String, ShaderChStencil *>::type StencilList;
        typedef map<String, TextureBlend *>::type TexBlendList;
        typedef map<String, TextureSampleUnit *>::type TexSampleList;
    protected:
        String mName;
        DriverVersion mVersion;
        RenderFeature * mVendorFeature;
        Viewport * mActiveViewport;
        FrameObj * mActiveFrame;
        GpuProgramParam * mActiveVGPP;
        GpuProgramParam * mActiveGGPP;
        GpuProgramParam * mActiveFGPP;
        GpuProgramParam * mActiveTGPP;
        GpuProgramParam * mActiveDGPP;
        GpuProgramParam * mActiveCGPP;
        GpuProgramParam * mFFGPP;
        RenderFeature * mFeature;
        TextureManager * mTextureManager;
        ExtFrameList mExtFrameList;
        FrameList mFrameObjList;
        FrameLevelList mFrameLevelList;
        PixelCountQueryList mPixelQueryList;
        CullingMode mCullingMode;
        PlaneList mClipPlanes;
        Colour mTextureBlend[NII_MAX_TextureLayer][2];
        NCount mEnableTexCount;
        NCount mVertexCount;
        NCount mTriangleCount;
        NCount mBatchCount;
        NCount mRenderCount;
        NCount mCurrentRender;
        Vector3f mTextureProjRel;
        Nui32 mVSyncInterval;
        NIIf mDepthBiasBase;
        NIIf mDepthBiasFactor;
        NIIf mDepthBiasSlopeScale;
        NCount mInstanceCount;
        VertexBuffer * mInstanceBuffer;
        VertexData * mInstanceData;
        SysOptionList mOptions;
        PropertyData mOptionData;
        ColourList mColourList;
        PointList mPointList;
        BlendList mBlendList;
        AlphaList mAlphaList;
        FogList mFogList;
        DepthList mDepthList;
        StencilList mStencilList;
        TexBlendList mTexBlendList;
        TexSampleList mTexSampleList;
        bool mFixedPipelineMode;
        bool mVertexTextureShared;
        bool mVPMode;
        bool mGPMode;
        bool mFPMode;
        bool mTPMode;
        bool mDPMode;
        bool mCPMode;
        bool mClipValid;
        bool mAutoDepthBias;
        bool mInvertVertexWinding;
        bool mSysFeatureValid;
        bool mTexProjRelative;
        bool mWBuffer;
        bool mVSync;
    };
    typedef vector<RenderSys *>::type RenderSysList;
}
#endif