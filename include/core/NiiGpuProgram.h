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

#ifndef _NII_GPU_PROGRAM_H_
#define _NII_GPU_PROGRAM_H_

#include "NiiPreInclude.h"
#include "NiiResource.h"
#include "NiiGpuProgramParam.h"

namespace NII
{
    /** 着色程序语法标准
    @remark 对照表
    @note 新版语法标准一般是支持其标准的以上版本
    @version NIIEngine 3.0.0
    */
    enum ShaderLanguage
    {
        SL_Unknow   = 0,
        SL_OPENGL   = 0x80000000,           ///< (glsl)
        SL_DX       = 0x40000000,           ///< (hlsl)
        SL_GLSLES   = 0x10000000,           ///< (gles glsles) language
        SL_CG       = 0x8000000,            ///< (nv cg) language
        SL_ASM      = 0x4000000,            ///< (汇编) language
        SL_ALTER    = 0x1000000,            ///< 一般存在代替方案的着色程序

        SL_ARBVP1 = 0x01 | SL_OPENGL,       ///< gl1.1 arbvp1
        SL_VP_20 = 0x02 | SL_OPENGL,        ///< gl2.0 vp20
        SL_VP_30 = 0x04 | SL_OPENGL,        ///< gl3.0 vp30
        SL_VP_40 = 0x08 | SL_OPENGL,        ///< gl4.0 vp40
        SL_GP4_VP = 0x10 | SL_OPENGL,       ///< gl4.0 gp4vp
        SL_GPU_VP = 0x20 | SL_OPENGL,       ///< gl4.0 gpu_vp
        SL_ARBFP1 = 0x40 | SL_OPENGL,       ///< gl1.1 arbfp1
        SL_FP_20 = 0x80 | SL_OPENGL,        ///< gl2.0 fp20
        SL_FP_30 = 0x100 | SL_OPENGL,       ///< gl3.0 fp30
        SL_FP_40 = 0x200 | SL_OPENGL,       ///< gl4.0 fp40
        SL_GP4_FP = 0x400 | SL_OPENGL,      ///< gl4.0 gp4fp
        SL_GPU_FP = 0x800 | SL_OPENGL,      ///< gl4.0 gpu_fp
        SL_NV_GP4 = 0x1000 | SL_OPENGL,     ///< nvgp4
        SL_GP4_GP = 0x2000 | SL_OPENGL,     ///< gp4gp
        SL_GPU_GP = 0x8000 | SL_OPENGL,     ///< gpu_gp
        SL_GLSL = 0x10000 | SL_OPENGL,      ///< (gl glsl) language
        SL_GLSL_1_0 = 0x20000 | SL_OPENGL,  ///< (gl glsl1.0) language
        SL_GLSL_1_1 = 0x40000 | SL_OPENGL,  ///< (gl glsl1.1) language
        SL_GLSL_1_2 = 0x80000 | SL_OPENGL,  ///< (gl glsl1.2) language
        SL_GLSL_1_3 = 0x100000 | SL_OPENGL,  ///< (gl glsl1.3) language
        SL_GLSL_1_4 = 0x200000 | SL_OPENGL,  ///< (gl glsl1.4) language
        SL_GLSL_1_5 = 0x400000 | SL_OPENGL,  ///< (gl glsl1.5) language
        SL_GLSL_3_3 = 0x800000 | SL_OPENGL,  ///< (gl glsl3.3) language
        SL_GLSL_4_2 = 0x1000000 | SL_OPENGL,  ///< (gl glsl4.2) language
        SL_GLSL_4_3 = 0x2000000 | SL_OPENGL,  ///< (gl glsl4.3) language
        SL_GLSL_4_4 = 0x4000000 | SL_OPENGL,  ///< (gl glsl4.4) language

        SL_VS_1_1 = 0x01 | SL_DX,           ///< dx8 vs_1_1
        SL_VS_2_0 = 0x02 | SL_DX,           ///< dx9 vs_2_0
        SL_VS_2_A = 0x04 | SL_DX,           ///< dx9 vs_2_a
        SL_VS_2_X = 0x08 | SL_DX,           ///< dx9 vs_2_x
        SL_VS_3_0 = 0x10 | SL_DX,           ///< dx9 vs_3_0
        SL_VS_4_0 = 0x20 | SL_DX,           ///< dx10 vs_4_0
        SL_VS_4_1 = 0x40 | SL_DX,           ///< dx10.1 vs_4_1
        SL_VS_5_0 = 0x80 | SL_DX,           ///< dx11 vs_5_0
        SL_PS_1_1 = 0x100 | SL_DX,          ///< dx8 ps_1_1
        SL_PS_1_2 = 0x200 | SL_DX,          ///< dx8 ps_1_2
        SL_PS_1_3 = 0x400 | SL_DX,          ///< dx8 ps_1_3
        SL_PS_1_4 = 0x800 | SL_DX,          ///< dx9 ps_1_4
        SL_PS_2_0 = 0x1000 | SL_DX,         ///< dx9 ps_2_0
        SL_PS_2_A = 0x2000 | SL_DX,         ///< dx9 ps_2_a
        SL_PS_2_B = 0x4000 | SL_DX,         ///< dx9 ps_2_b
        SL_PS_2_X = 0x8000 | SL_DX,         ///< dx9 ps_2_x
        SL_PS_3_0 = 0x10000 | SL_DX,        ///< dx9 ps_3_0
        SL_PS_3_X = 0x20000 | SL_DX,        ///< dx9 ps_3_x
        SL_PS_4_0 = 0x40000 | SL_DX,        ///< dx10 ps_4_0
        SL_PS_4_1 = 0x80000 | SL_DX,        ///< dx10.1 ps_4_1
        SL_PS_5_0 = 0x100000 | SL_DX,       ///< dx11 ps_5_0
        SL_GS_4_0 = 0x200000 | SL_DX,       ///< dx10 gs_4_0
        SL_GS_4_1 = 0x400000 | SL_DX,       ///< dx10.1 gs_4_1
        SL_GS_5_0 = 0x800000 | SL_DX,       ///< dx11 gs_5_0
        SL_HLSL = 0x1000000 | SL_DX         ///< (dx hlsl)language
    };
    typedef NIIi ShaderLanguageMark;

    /** 着色程序
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuProgram : public Resource
    {
    public:
        /** 着色程序类型
        @version NIIEngine 3.0.0
        */
        enum ShaderType
        {
            ST_VS = 0,       ///< 顶点
            ST_TS = 1,       ///< 细分面/tess control shader/hull-shader
            ST_DS = 2,       ///< domain-shader/tess tvaluation shader
            ST_GS = 3,       ///< 几何
            ST_FS = 4,       ///< 片段/pixel-shader
            ST_CS = 5,       ///< compute-shader
            ST_Cnt = 8
        };
    public:
        /** 构造函数
        @version NIIEngine 3.0.0 高级api
        */
        GpuProgram(ResourceID rid, GroupID gid, ResLoadScheme * ls = 0, ResResultScheme * rs = 0, 
            ScriptTypeID stid = N_CmdObj_GpuProgram, LangID lid = N_PrimaryLang);

        virtual ~GpuProgram();

        /** 是否有效
        @remark 一般着色程序都涉及渲染系统和硬件支持
        @version NIIEngine 3.0.0
        */
        virtual bool isValid() const;

        /** 获取错误代码
        @version NIIEngine 3.0.0
        */
        inline Nui32 getErrorCode() const               { return mErrorCode; }

        /** 设置着色程序类型(只在加载前有效)
        @version NIIEngine 3.0.0
        */
        virtual void setType(ShaderType t);

        /** 获取着色程序类型
        @version NIIEngine 3.0.0
        */
        virtual ShaderType getType() const;

        /** 设置语法类型
        @version NIIEngine 3.0.0
        */
        inline void setSyntaxType(ShaderLanguage sl)    { mSyntax = sl; }

        /** 获取语法类型
        @version NIIEngine 3.0.0
        */
        inline ShaderLanguage getSyntaxType() const     { return mSyntax; }

        /** 获取语言种类
        @version NIIEngine 3.0.0
        */
        virtual ShaderLanguage getLanguage() const;

        /** 为这个着色程序,设置资源汇编的来源文件
        @remark 设置这个没有效果直到(重新)加载这个程序
        @version NIIEngine 3.0.0
        */
        void setProgramSrc(const String & file);
        
        /** 获取这个着色程序使用的资源文件名
        @version NIIEngine 3.0.0
        */
        inline const String & getProgramSrc() const     { return mFile; }
        
        /** 设置代码串
        @version NIIEngine 3.0.0
        */
        void setProgramStr(const VString & code);

        /** 获取代码串
        @version NIIEngine 3.0.0
        */
        inline const VString & getProgramStr() const    { return mSource; }

        /** 获取代码串hash
        @version NIIEngine 3.0.0
        */
        Nui32 getHash(Nui32 in = 0) const;
        
        /** 设置自定义程序参数
        @version NIIEngine 3.0.0
        */
        virtual void setParamDef(const GpuParamDefine & o);

        /** 获取自定义程序参数
        @version NIIEngine 3.0.0
        */
        virtual const GpuParamDefine & getParamDef() const;

        /** 设置自定义程序参数导入文件
        @version NIIEngine 3.0.0
        */
        virtual void setParamDefSrc(const String & file);

        /** 获取自定义程序参数导入文件
        @version NIIEngine 3.0.0
        */
        virtual const String & getParamDefSrc() const   { return mParamDefFile; }

        /** 设置程序入口
        @note 如果存在多个使用;分割.一般着色程序只有一个入口main.而这个函数用于高级合成着色程序MultiGpuProgram
        @version NIIEngine 6.0.0
        */
        inline void setKernel(const VString & kernel)   { mKernel = kernel; }

        /** 获取程序入口
        @note 如果存在多个使用;分割.一般着色程序只有一个入口main.而这个函数用于高级合成着色程序MultiGpuProgram
        @version NIIEngine 6.0.0
        */
        inline const VString & getKernel() const        { return mKernel; }

        /** 获取实际着色程序
        @remark 一般是存在自动代替方案的时候使用
        @version NIIEngine 3.0.0
        */
        virtual GpuProgram * getMain();

        /** 是否保留渲染通路的颜色状态
        @version NIIEngine 3.0.0
        */
        virtual bool isEnvColourEnable() const;

        /** 是否保留渲染通路的灯光状态
        @version NIIEngine 3.0.0
        */
        virtual bool isEnvLightEnable() const;

        /** 是否保留渲染通路的雾状态
        @version NIIEngine 3.0.0
        */
        virtual bool isEnvFogEnable() const;

        /** 是否保留渲染通路的空间状态
        @version NIIEngine 3.0.0
        */
        virtual bool isEnvSpaceEnable() const;

        /** 设置是否需要顶点纹理其他数据获取
        @note ST_VS 用
        @version NIIEngine 3.0.0
        */
        virtual void setVertexTextureFetch(bool b);

        /** 获取是否需要顶点纹理其他数据获取
        @note ST_VS 用
        @version NIIEngine 3.0.0
        */
        virtual bool isVertexTextureFetch() const;

        /** 设置是否需要执行纹理距阵混合
        @note ST_VS 用
        @version NIIEngine 3.0.0
        */
        virtual void setVertexMatrixFusion(bool b);

        /** 获取是否需要执行顶点距阵变换
        @note ST_VS 用
        @version NIIEngine 3.0.0
        */
        virtual bool isVertexMatrixFusion() const;

        /** 设置是否需要执行顶点距阵变换
        @note ST_VS 用
        @version NIIEngine 3.0.0
        */
        virtual void setVertexInterpolation(bool b);

        /** 获取是否需要执行顶点距阵变换
        @note ST_VS 用
        @version NIIEngine 3.0.0
        */
        virtual bool isVertexInterpolation() const;

        /** 设置是否需要顶点偏移混合
        @param[in] cnt 混合参子数量
        @note ST_VS 用
        @version NIIEngine 3.0.0
        */
        virtual void setVertexOffsetCount(NCount cnt);

        /** 设置是否需要顶点偏移混合
        @note ST_VS 用
        @version NIIEngine 3.0.0
        */
        virtual NCount getVertexOffsetCount() const;

        /** 创建兼容的程序参数
        @version NIIEngine 3.0.0
        */
        virtual GpuProgramParam * createParam();

        /** 创建内部兼容的程序参数
        @version NIIEngine 3.0.0
        */
        virtual GpuProgramParam * createDefaultParam();

        /** 获取内部兼容的程序参数
        @version NIIEngine 3.0.0
        */
        virtual GpuProgramParam * getDefaultParam() const;

        /** 通过帧时间控制着色程序参数
        @param[in] param 着色程序参数
        @param[in] idx 着色程序参数索引
        @param[in] factor 时间因子
        @version NIIEngine 3.0.0
        */
        DataEquation<TimeDurMS, NIIf> * createTimeParam(GpuProgramParam * param, NCount idx, NIIf factor = 1.0f);
        
        /** 设置计算着色程序分组
        @version NIIEngine 5.0.0
         */
        inline void setComputeDim(const Vector3i & dim) { mComputeDim = dim; }
        
        /** 获取计算着色程序分组
        @version NIIEngine 5.0.0
        */
        inline const Vector3i & getComputeDim() const   { return mComputeDim; }

        /** 设置需要顶点绘制的相临基元信息
        @remark 顶点着色程序/几何着色程序
        @version NIIEngine 3.0.0
        */
        inline void setAdjacencyPrimitive(bool b)       { mAdjacencyPrimitive = b;}

        /** 是否需要顶点绘制的相临基元信息
        @remark 顶点着色程序/几何着色程序
        @version NIIEngine 3.0.0
        */
        inline bool isAdjacencyPrimitive() const        { return mAdjacencyPrimitive; }
    protected:
        /// @copydoc Resource::loadImpl
        void loadImpl();

        /// @copydoc Resource::calcSize
        NCount calcSize() const;

        /// @copydetails PropertyCmdObj::init
        bool initCmd(PropertyCmdSet * dest);

        /** 加载代码
        @version NIIEngine 3.0.0
        */
        virtual void loadCodeImpl(const VString & code) = 0;

        /** 创建参数值
        @version NIIEngine 3.0.0
        */
        void createParamMap();

        /** 渲染系统是否支持
        @version NIIEngine 3.0.0
        */
        bool isSysSupport() const;
    protected:
        ShaderType mType;
        GpuProgramID mID;
        ShaderLanguage mSyntax;
        String mFile;
        String mParamDefFile;
        VString mSource;
        VString mKernel;
        Nui32 mSourceHash;
        Nui32 mErrorCode;
        NCount mFusionCount;
        GpuParamDefine * mDefines;
        GpuProgramParam * mParams;
        Vector3i mComputeDim;
        bool mVertexMatrixValid;
        bool mVertexTextureFetch;
        bool mParamValid;
        bool mProgramSrcValid;
        bool mAdjacencyPrimitive;
        bool mVertexInterpolation;
    };
}
#endif