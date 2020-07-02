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

#ifndef _NII_GPU_PARAM_DEF_H_
#define _NII_GPU_PARAM_DEF_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** Gpu参数种类
    @version NIIEngine 3.0.0
    */
    enum GpuParamType
    {
        GPT_View            = 0x01,   ///< 视图空间
        GPT_Space           = 0x02,   ///< 模型空间
        GPT_Colour          = 0x04,
        GPT_Light           = 0x08,
        GPT_Texture         = 0x10,
        GPT_Fog             = 0x20,
        GPT_Fps             = 0x40,
        GPT_ChMulti         = 0x80,
        GPT_Other           = 0x100,
        GPT_Render          = GPT_View | GPT_Colour | GPT_Texture | GPT_Fog | GPT_Fps | GPT_Other,
        GPT_Unknow          = 0xFFFFFFFF
    };

    /** Gpu数据类型
    @version NIIEngine 3.0.0
    */
    enum GpuDataType
    {
        GDT_Unknow          = 0,
        GDT_Float           = 1,
        GDT_Float2X         = 2,
        GDT_Float3X         = 3,
        GDT_Float4X         = 4,
        GDT_Matrix2X2       = 5,
        GDT_Matrix2X3       = 6,
        GDT_Matrix2X4       = 7,
        GDT_Matrix3X2       = 8,
        GDT_Matrix3X3       = 9,
        GDT_Matrix3X4       = 10,
        GDT_Matrix4X2       = 11,
        GDT_Matrix4X3       = 12,
        GDT_Matrix4X4       = 13,
        GDT_Int             = 14,
        GDT_Int2X           = 15,
        GDT_Int3X           = 16,
        GDT_Int4X           = 17,
        GDT_UInt            = 18,
        GDT_UInt2X          = 19,
        GDT_UInt3X          = 20,
        GDT_UInt4X          = 21,
        GDT_Bool            = 22,
        GDT_Bool2X          = 23,
        GDT_Bool3X          = 24,
        GDT_Bool4X          = 25,
        GDT_Sampler1D       = 26,
        GDT_Sampler2D       = 27,
        GDT_Sampler3D       = 28,
        GDT_SamplerCube     = 29,
        GDT_SamplerRect     = 30,
        GDT_SamplerShadow1D = 31,
        GDT_SamplerShadow2D = 32,
        GDT_SamplerArray2D  = 33,
        GDT_SamplerOES      = 34,
        GDT_Double          = 35,
        GDT_Double2         = 36,
        GDT_Double3         = 37,
        GDT_Double4         = 38,
        GDT_Matrixd2X2      = 39,
        GDT_Matrixd2X3      = 40,
        GDT_Matrixd2X4      = 41,
        GDT_Matrixd3X2      = 42,
        GDT_Matrixd3X3      = 43,
        GDT_Matrixd3X4      = 44,
        GDT_Matrixd4X2      = 45,
        GDT_Matrixd4X3      = 46,
        GDT_Matrixd4X4      = 47,
        GDT_Interface       = 48
    };
    /** Gpu内部同步参数
    @version NIIEngine 3.0.0
    */
    enum GpuSyncParam
    {
        GSP_Matrix_Proj = 0,            ///< 投影矩阵
        GSP_Matrix_View,                ///< 视图矩阵
        GSP_Matrix_Space,               ///< 模型矩阵
        GSP_Matrix_Proj_X_View,         ///< 几何原点矩阵
        GSP_Matrix_View_X_Space,        ///< VR空间矩阵
        GSP_Matrix_Proj_X_View_X_Space, ///< 世界矩阵(world matrix)

        GSP_Matrix_Proj_Inverse,
        GSP_Matrix_View_Inverse,
        GSP_Matrix_Space_Inverse,
        GSP_Matrix_Proj_X_View_Inverse,
        GSP_Matrix_View_X_Space_Inverse,
        GSP_Matrix_Proj_X_View_X_Space_Inverse,

        GSP_Matrix_Proj_Transpose,
        GSP_Matrix_View_Transpose,
        GSP_Matrix_Space_Transpose,
        GSP_Matrix_Proj_X_View_Transpose,
        GSP_Matrix_View_X_Space_Transpose,
        GSP_Matrix_Proj_X_View_X_Space_Transpose,

        GSP_Matrix_Proj_InverseTranspose,
        GSP_Matrix_View_InverseTranspose,
        GSP_Matrix_Space_InverseTranspose,
        GSP_Matrix_Proj_X_View_InverseTranspose,
        GSP_Matrix_View_X_Space_InverseTranspose,
        GSP_Matrix_Proj_X_View_X_Space_InverseTranspose,

        GSP_Matrix3x4_Space_Array,
        GSP_Matrix4x4_Space_Array,
        GSP_Matrix3x4_Space_Translate,
        GSP_Matrix3x4_Space_Scale,
        GSP_Matrix3x4_Space_Rotate,
        GSP_Matrix3x4_Space_Scale_Shear,
        GSP_DualQuaternion_Space,

        GSP_View_PixelWidth,
        GSP_View_PixelHeight,
        GSP_View_PixelSize,

        GSP_Camera_Pos,
        GSP_Camera_Pos_ChSpace,
        GSP_Camera_Dir,
        GSP_Camera_Right,
        GSP_Camera_Up,
        GSP_Camera_FOV,
        GSP_Camera_NearClip,
        GSP_Camera_FarClip,
        GSP_Camera_LOD_Pos,
        GSP_Camera_LOD_Pos_ChSpace,

        GSP_Ambient,
        GSP_FPS,
        GSP_Vertex_Winding,
        GSP_Shadow_VolExtent,
        GSP_Shadow_ChDepthRange_ParamIndex, ///< float4(MinDepth, MaxDepth, DepthRange, 1 / DepthRange)
        GSP_Shadow_Colour,
        GSP_Fog_Params,                     ///< Vector4(density, start, end, 1/(end - start)) Vector4(FogColour)
        GSP_Ponit_Params,

        GSP_Ch_Ambient,
        GSP_Ch_Diffuse,
        GSP_Ch_Specular,
        GSP_Ch_Emissive,
        GSP_Ch_Shininess,
        GSP_Ch_Emissive_Evn,
        GSP_Ch_Ambient_X_Env,
        GSP_Ch_Depth_Range,                 ///< float4(MinDepth, MaxDepth, DepthRange, 1 / DepthRange)
        GSP_Ch_Index,
        GSP_Ch_RenderCount,
        GSP_Ch_Custom,
        GSP_Ch_AniParam,

        GSP_Light_Index_ParamIndex,
        GSP_Light_Diffuse_ParamIndex,
        GSP_Light_Specular_ParamIndex,
        GSP_Light_Attenuation_ParamIndex,   ///< Vector4(range, constant, linear, quadric)
        GSP_Light_Pos_ParamIndex,
        GSP_Light_Dir_ParamIndex,
        GSP_Light_Dis_ChSpace_ParamIndex,
        GSP_Light_Pos_ChSpace_ParamIndex,
        GSP_Light_Pos_ChView_ParamIndex,
        GSP_Light_Dir_ChSpace_ParamIndex,
        GSP_Light_Dir_ChView_ParamIndex,
        GSP_Light_Spot_Params_ParamIndex,   ///< Vector4(inner, outer, falloff, spot)
        GSP_Light_CastShadow_ParamIndex,
        GSP_Light_BrightFactor_ParamIndex,
        GSP_Light_Diffuse_Ch_ParamIndex,
        GSP_Light_Specular_Ch_ParamIndex,
        GSP_Light_Attenuation_ParamCount,
        GSP_Light_Diffuse_ParamCount,
        GSP_Light_Specular_ParamCount,
        GSP_Light_Pos_ParamCount,
        GSP_Light_Pos_ChSpace_ParamCount,
        GSP_Light_Pos_ChView_ParamCount,
        GSP_Light_Dir_ParamCount,
        GSP_Light_Dir_ChSpace_ParamCount,
        GSP_Light_Dir_ChView_ParamCount,
        GSP_Light_Dis_ChSpace_ParamCount,
        GSP_Light_BrightFactor_ParamCount,
        GSP_Light_Spot_Params_ParamCount,
        GSP_Light_Diffuse_Ch_ParamCount,
        GSP_Light_Specular_Ch_ParamCount,
        GSP_Light_Spot_Matrix_Proj_X_View_ParamIndex,
        GSP_Light_Spot_Matrix_Proj_X_View_ParamCount,
        GSP_Light_Spot_Matrix_Proj_X_View_X_Space,
        GSP_Light_ActiveCount,
        GSP_Light_Custom,

        GSP_Texture_Flipping,               ///< 纹理采样D3D左上角作为原点,OpenGL左下角,主要是翻转了y轴,导致z轴方向相反
        GSP_Texture_Dim_ParamIndex,         ///< Vector4(width, height, depth, 1)
        GSP_Texture_DimUnit_ParamIndex,     ///< Vector4(width, height, 1 / width, 1 / height)
        GSP_Texture_InverseDim_ParamIndex,  ///< Vector4(1 / width, 1 / height, 1 / depth, 1)
        GSP_Texture_Matrix_Transform_ParamIndex,
        GSP_Texture_Matrix_Proj_X_View_ParamIndex,
        GSP_Texture_Matrix_Proj_X_View_X_Space_ParamIndex,
        GSP_Texture_Matrix_Proj_X_View_ParamCount,
        GSP_Texture_Matrix_Proj_X_View_X_Space_ParamCount,
        GSP_Texel_Oft,                      ///< float4(absHOffset, absVOffset, hOffset / viewportWidth, vOffset / viewportHeight)
                                            ///< dxd9 different between opengl about mapping texels to pixels(像素中心,位置1:1纹理坐标时的问题)
        GSP_Sys_CostTime,
        GSP_Sys_CostTime_Fmod_ParamData,
        GSP_Frame_CostTime
    };

    /** 同步常量的额外数据类型.
    @version NIIEngine 3.0.0
    */
    enum SyncParamInput
    {
        SPI_None    = 0,
        SPI_Int     = 1,
        SPI_Float   = 2,
        SPI_Double  = 3
    };

    inline bool isFloat(GpuDataType c) { return c > GDT_Unknow && c < GDT_Int; }
    inline bool isDouble(GpuDataType c) { return c > GDT_SamplerOES && c < GDT_Interface; }
    inline bool isSampler(GpuDataType c) { return c > GDT_Bool4X && c < GDT_Double; }
    inline bool isInt(GpuDataType c) { return c > GDT_Matrix4X4 && c < GDT_UInt; }
    inline bool isUInt(GpuDataType c) { return c > GDT_Int4X && c < GDT_Bool; }
    inline bool isBool(GpuDataType c) { return c > GDT_UInt4X && c < GDT_Sampler1D; }
    inline bool isInterface(GpuDataType c) { return c == GDT_Interface; }

    /** 定义同步常量,适用于在参数对象中使用
    @version NIIEngine 3.0.0
    */
    struct _EngineAPI GpuSParamDefine
    {
        GpuSParamDefine(const String & name, GpuSyncParam sync, NCount cnt, Nui16 exttype, Nui16 datatype);

        String mName;
        GpuSyncParam mSyncParam;
        Nui16 mDataType;
        Nui16 mInputType;
        NCount mCount;
    };
    
    /** 着程序自定义参数
    @version NIIEngine 3.0.0
    */
    struct _EngineAPI GpuParamUnit
    {
        GpuParamUnit();

        inline bool isFloat() const { return mDataType > GDT_Unknow && mDataType < GDT_Int; }

        inline bool isDouble() const { return mDataType > GDT_SamplerOES && mDataType < GDT_Interface; }

        inline bool isSampler() const { return mDataType > GDT_Bool4X && mDataType < GDT_Double; }

        inline bool isInt() const { return mDataType > GDT_Matrix4X4 && mDataType < GDT_UInt; }

        inline bool isUInt() const { return mDataType > GDT_Int4X && mDataType < GDT_Bool; }

        inline bool isBool() const { return mDataType > GDT_UInt4X && mDataType < GDT_Sampler1D; }

        inline bool isInterface() const { return mDataType == GDT_Interface; }

        GpuDataType mDataType;
        Nidx mIndex;
        Nmark mTypeMark;
        Nidx mMemIndex;
        NCount mUnitSize;
        NCount mUnitCount;
    };
    typedef map<VString, GpuParamUnit>::type GpuParamUnitList;

    /** 着色程序自定义参数集
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuParamDefine : public GpuParamsAlloc
    {
    public:
        GpuParamDefine();
        GpuParamDefine(const GpuParamUnitList & defs);

        GpuParamDefine & operator = (const GpuParamDefine & o);

        /** 设置是否启用数组模式
        @remark 结构数据会变大
        @version NIIEngine 3.0.0
        */
        inline void setArrayMode(bool b){ mArrayMode = b; }

        /** 获取是否启用数组模式
        @remark 结构数据会变大
        @version NIIEngine 3.0.0
        */
        inline bool isArrayMode() const{return mArrayMode;}

        /** 添加定义
        @version NIIEngine 3.0.0
        */
        void add(const VString & name, const GpuParamUnit & def);

        /** 获取定义
        @version NIIEngine 3.0.0
        */
        GpuParamUnit * get(const VString & name) const;

        /** 移去定义
        @version NIIEngine 3.0.0
        */
        void remove(const VString & name);

        /** 移去所有定义
        @version NIIEnginee 3.0.0
        */
        void removeAll(const String & name);

        /** 保存到文件
        @version NIIEngine 3.0.0
        */
        void save(const String & file) const;

        /** 从文件中加载
        @version NIIEngine 3.0.0
        */
        void load(const String & file);

        /** 从文件中加载
        @version NIIEngine 3.0.0
        */
        void load(DataStream * stream);
    public:
        GpuParamUnitList mDefines;
        NCount mFloatSize;
        NCount mDoubleSize;
        NCount mIntSize;
        bool mArrayMode;
    };
}
#endif