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

#ifndef _NII_RenderFeature_H_
#define _NII_RenderFeature_H_

#include "NiiPreInclude.h"
#include "NiiGPUInfo.h"

#define N_CategoryGroupValue(cnt, grp, val) ((grp << (32 - cnt)) | (1 << val))

namespace NII
{
    /** 类别组
    @version NIIEngine 3.0.0
    */
    enum GpuFeatureGroup
    {
        CC_Common1  = 0,
        CC_Common2  = 1,
        CC_Ext1     = 2,
        CC_D3D      = 3,
        CC_GL       = 4,
        CC_Spec3    = 5,
        CC_Spec4    = 6,
        CC_Count    = 7
    };

    /** Gpu特性
    @version NIIEngine 3.0.0
    */
    enum GpuFeature
    {
        GF_ClipRect                     = N_CategoryGroupValue(CC_Count, CC_Common1, 0),
        GF_ClipPlanes                   = N_CategoryGroupValue(CC_Count, CC_Common1, 1),
        GF_Infinite_FarPlane            = N_CategoryGroupValue(CC_Count, CC_Common1, 2),

        GF_Tex_AutoMipmapCompress       = N_CategoryGroupValue(CC_Count, CC_Common1, 3),
        GF_Tex_Anisotropy               = N_CategoryGroupValue(CC_Count, CC_Common1, 4),
        GF_Tex_Compress                 = N_CategoryGroupValue(CC_Count, CC_Common1, 5),
        GF_Tex_1D                       = N_CategoryGroupValue(CC_Count, CC_Common1, 6),
        GF_Tex_3D                       = N_CategoryGroupValue(CC_Count, CC_Common1, 7),
        GF_Tex_Dot3                     = N_CategoryGroupValue(CC_Count, CC_Common1, 8),
        GF_Tex_Float                    = N_CategoryGroupValue(CC_Count, CC_Common1, 9),
        GF_Tex_NonPow2                  = N_CategoryGroupValue(CC_Count, CC_Common1, 10),
        GF_Tex_MipMapBias               = N_CategoryGroupValue(CC_Count, CC_Common1, 11),

        GF_Program_Vertex               = N_CategoryGroupValue(CC_Count, CC_Common1, 12),
        GF_Program_Fragment             = N_CategoryGroupValue(CC_Count, CC_Common1, 13),
        GF_Program_Geometry             = N_CategoryGroupValue(CC_Count, CC_Common1, 14),
        GF_Program_VertexTexFetch       = N_CategoryGroupValue(CC_Count, CC_Common1, 15),
        GF_Fixed_Pipeline               = N_CategoryGroupValue(CC_Count, CC_Common1, 16),

        GF_Point_Param                  = N_CategoryGroupValue(CC_Count, CC_Common1, 17),
        GF_Point_Sprites                = N_CategoryGroupValue(CC_Count, CC_Common1, 18),
        GF_LineWide                     = N_CategoryGroupValue(CC_Count, CC_Common1, 19),

        GF_HW_Stencil                   = N_CategoryGroupValue(CC_Count, CC_Common1, 20),
        GF_HW_GAMMA                     = N_CategoryGroupValue(CC_Count, CC_Common1, 21),
        GF_HW_Buffer                    = N_CategoryGroupValue(CC_Count, CC_Common1, 22),
        GF_HW_PixelQuery                = N_CategoryGroupValue(CC_Count, CC_Common1, 23),

        GF_Vertex_UByte4                = N_CategoryGroupValue(CC_Count, CC_Common1, 24),
        GF_Vertex_Instance              = N_CategoryGroupValue(CC_Count, CC_Common1, 25),
        GF_Index_32                     = N_CategoryGroupValue(CC_Count, CC_Common1, 26),

        GF_Stencil_BothSide             = N_CategoryGroupValue(CC_Count, CC_Common1, 27),
        GF_Stencil_Wrap                 = N_CategoryGroupValue(CC_Count, CC_Common1, 28),
        GF_Colour_Blend                 = N_CategoryGroupValue(CC_Count, CC_Common1, 29),

        GF_Alpha_Coverage               = N_CategoryGroupValue(CC_Count, CC_Common2, 0),
        GF_Render_To_Texture            = N_CategoryGroupValue(CC_Count, CC_Common2, 1),
        GF_Render_To_Vertex             = N_CategoryGroupValue(CC_Count, CC_Common2, 2),
        GF_Tex_Fromat_DXT               = N_CategoryGroupValue(CC_Count, CC_Common2, 3),
        GF_Tex_Fromat_VTC               = N_CategoryGroupValue(CC_Count, CC_Common2, 4),
        GF_Tex_Fromat_PVRTC             = N_CategoryGroupValue(CC_Count, CC_Common2, 5),
        GF_Shader_Binary_Get            = N_CategoryGroupValue(CC_Count, CC_Common2, 6),

        GF_RenderTarget_LessDepth           = N_CategoryGroupValue(CC_Count, CC_Ext1, 0),
        GF_RenderTarget_AttachMainDepth     = N_CategoryGroupValue(CC_Count, CC_Ext1, 1),
        GF_MultiRenderTarget_EachDepthBit   = N_CategoryGroupValue(CC_Count, CC_Ext1, 2),

        GF_D3D                              = N_CategoryGroupValue(CC_Count, CC_D3D, 0),
        GF_D3D9                             = N_CategoryGroupValue(CC_Count, CC_D3D, 1),
        GF_D3D11                            = N_CategoryGroupValue(CC_Count, CC_D3D, 2),
        GF_D3D12                            = N_CategoryGroupValue(CC_Count, CC_D3D, 3),
        GF_D3D_PerStageConstant             = N_CategoryGroupValue(CC_Count, CC_D3D, 4),
        GF_GL                               = N_CategoryGroupValue(CC_Count, CC_GL, 0),
        GF_GL_3_1                           = N_CategoryGroupValue(CC_Count, CC_GL, 1),
        GF_GL_4_1                           = N_CategoryGroupValue(CC_Count, CC_GL, 2),
        GF_GL_ES                            = N_CategoryGroupValue(CC_Count, CC_GL, 3),
        GF_GL_ES_1_2                        = N_CategoryGroupValue(CC_Count, CC_GL, 4),
        GF_GL_ES_2_1                        = N_CategoryGroupValue(CC_Count, CC_GL, 5),
        GF_GL_ES_3_1                        = N_CategoryGroupValue(CC_Count, CC_GL, 6),
        GF_GL_PBuffer                       = N_CategoryGroupValue(CC_Count, CC_GL, 7),
        GF_GL_VArrayObj                     = N_CategoryGroupValue(CC_Count, CC_GL, 8),
        GF_GLSL_SeparateShaderObject        = N_CategoryGroupValue(CC_Count, CC_GL, 9)
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
        const GPUInfo * getGpuInfo() const;

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

        /** 是否特定特性
        @version NIIEngine 3.0.0
        */
        bool isSpecFeature(GpuFeature c) const;

        /** 添加着色程序支持
        @version NIIEngine 3.0.0
        */
        inline void add(ShaderLanguageMark m)           { mSLMark |= m; }

        /** 移去着色程序支持
        @version NIIEngine 3.0.0
        */
        inline void remove(ShaderLanguageMark m)        { mSLMark &= ~m; }

        /** 是否支持着色程序
        @version NIIEngine 3.0.0
        */
        inline bool isSupport(ShaderLanguage m) const   { return mSLMark & m; }

        /** 获取支持着色程序(集合)
        @version NIIEngine 3.0.0
        */
        inline ShaderLanguageMark getShaderMark() const { return mSLMark; }

        /** 获取名称
        @version NIIEngine 3.0.0
        */
        inline const String & getName() const           { return mName; }

        /** 获取多纹理采样最大数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxTexUnit() const             { return mMaxTextureUnit; }

        /** 获取点的最大大小
        @remark 像素
        @version NIIEngine 3.0.0
        */
        inline NIIf getMaxPointSize() const             { return mMaxPointSize; }

        /** 获取支持的同步渲染目标数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxMultiFrameObj() const        { return mMaxMultiFrameObj; }

        /** 获取最大空间数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxSpace() const                { return mMaxSpace; }

        /** 获取硬件模板位深
        @version NIIEngine 3.0.0
        */
        inline Nui16 getStencilBit() const              { return mStencilBit; }

        /** 获取顶点纹理最大数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxVertexTexUnit() const        { return mMaxVertexTexUnit; }
        
        /** 获取顶点属性最大数量
        @version NIIEngine 3.0.0
        */
        inline void setMaxVertexAttrib(Nui16 num)       { mMaxVertexAttrib = num; }

        /** 获取顶点属性最大数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxVertexAttrib() const         { return mMaxVertexAttrib; }

        /** 设置最大的各向异性
        @version NIIEngine 4.0.0
        */
        inline void setMaxAnisotropy(NIIf s)            {  mMaxSupportedAnisotropy = s; }
        
        /** 获取最大的各向异性
        @version NIIEngine 4.0.0
        */
        inline NIIf getMaxAnisotropy() const            { return mMaxSupportedAnisotropy; }

        /** 获取是否仅2次幂纹理
        @version NIIEngine 3.0.0
        */
        inline bool isOnlyPow2Texture() const           { return mOnlyPow2Texture; }

        /** 设置顶点着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxVPFloatParamCount() const    { return mMaxVPParam[0]; }

        /** 设置顶点着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxVPIntParamCount() const      { return mMaxVPParam[2]; }

        /** 设置顶点着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxVPBoolParamCount() const     {  return mMaxVPParam[3]; }

        /** 获取片段着色程序支持floating常量数目
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxFPFloatCount() const         { return mMaxFPParam[0]; }

        /** 获取片段着色程序支持integer常量数目
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxFPIntCount() const           { return mMaxFPParam[2]; }

        /** 获取片段着色程序支持boolean常量数目
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxFPBoolCount() const          {  return mMaxFPParam[3]; }

        /** 获取几何着色程序支持float常量数目
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxGPFloatCount() const         { return mMaxGPParam[0]; }

        /** 获取几何着色程序支持int常量数目
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxGPIntCount() const           { return mMaxGPParam[2]; }

        /** 获取几何着色程序支持bool常量数目
        @version NIIEngine 3.0.0
        */
        inline Nui16 getMaxGPBoolCount() const          { return mMaxGPParam[3]; }

        /** 获取几何程序输出顶点最大数量
        @version NIIEngine 3.0.0
        */
        inline NIIi getMaxGeometryOutput() const      { return mMaxGPOutput; }

        /** 输出细节
        @version NIIEngine 3.0.0
        */
        void log(Log * log);
    public:
        /** 设置名称
        @version NIIEngine 3.0.0
        */
        inline void setName(const String & rs)          { mName = rs; }

        /** 设置多纹理采样最大数量
        @version NIIEngine 3.0.0 高级api
        */
        inline void setMaxTexUnit(Nui16 num)            { mMaxTextureUnit = num; }

        /** 设置点的最大大小
        @remark 像素
        @version NIIEngine 3.0.0 高级api
        */
        inline void setMaxPointSize(NIIf s)             { mMaxPointSize = s; }

        /** 设置支持同步渲染目标的数目
        @version NIIEngine 3.0.0
        */
        void setMaxMultiFrameObj(Nui16 num)             { mMaxMultiFrameObj = num;}

        /** 设置最大空间数量
        @version NIIEngine 3.0.0
        */
        void setMaxSpace(Nui16 num)                     { mMaxSpace = num; }

        /** 设置硬件模板位深
        @version NIIEngine 3.0.0
        */
        void setStencilBit(Nui16 num)                   { mStencilBit = num; }

        /** 设置顶点纹理最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxVertexTexUnit(Nui16 n)               { mMaxVertexTexUnit = n; }

        /** 设置几何程序输出顶点最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxGeometryOutput(NIIi count)           { mMaxGPOutput = count; }

        /** 设置是否仅2次幂纹理
        @version NIIEngine 3.0.0
        */
        void setOnlyPow2Texture(bool l)                 { mOnlyPow2Texture = l; }

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
        void setMaxTessellationProgramParam(Nui16 f, Nui16 d, Nui16 i, Nui16 b);

        /** 设置细化着色程序局部参数最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxTessellationProgramParam(Nui16 c);
    private:
        String mName;
        GPUInfo mGPUInfo;
        ShaderLanguageMark mSLMark;
        Nmark mFeature[CC_Count];
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
        bool mOnlyPow2Texture;
    };
}
#endif