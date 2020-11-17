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

#ifndef _NII_GPU_PROGRAMPARAM_H_
#define _NII_GPU_PROGRAMPARAM_H_

#include "NiiPreInclude.h"
#include "NiiVector4.h"
#include "NiiMatrix4.h"
#include "NiiColour.h"

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
    
    /** 着色参数内存绑定
    @version NIIEngine 3.0.0
    */
    struct _EngineAPI GpuParamIndex
    {
    public:
        GpuParamIndex(): mSize(0), mMemIndex(0), mTypeMark(GPT_Render){}
        
        GpuParamIndex(Nidx memidx, NCount size, Nmark mark) : mMemIndex(memidx), mSize(size), mTypeMark(mark) {}
    public:
        Nidx mMemIndex;
        NCount mSize;
        Nmark mTypeMark;
    };
    typedef map<Nui32, GpuParamIndex>::type GpuParamIndexList;

    /** 同步参数内存绑定
    @version NIIEngine 3.0.0
    */
    struct _EngineAPI GpuSParamIndex
    {
    public:
        GpuSParamIndex(GpuSyncParam type, Nidx memidx, Ni32 input, Nmark mark, NCount count = 4):
            mSyncParam(type), mMemIndex(memidx), mTypeMark(mark), mInputInt(input), mUnitCount(count){}

        GpuSParamIndex(GpuSyncParam type, Nidx memidx, NIIf input, Nmark mark, NCount count = 4):
            mSyncParam(type), mMemIndex(memidx), mTypeMark(mark), mInputFloat(input), mUnitCount(count){}

        Nidx mMemIndex;
        GpuSyncParam mSyncParam;
        Nmark mTypeMark;
        NCount mUnitCount;
        union
        {
            Ni32 mInputInt;
            NIIf mInputFloat;
        };
    };
    typedef vector<GpuSParamIndex>::type GpuSParamIndexList;

    /** 着色参数内存绑定
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuParamMap : public GpuParamsAlloc
    {
    public:
        GpuParamMap(): mIntSize(0), mFloatSize(0), mDoubleSize(0){}

        GpuParamIndexList mIntList;
        GpuParamIndexList mFloatList;
        GpuParamIndexList mDoubleList;
        NCount mIntSize;
        NCount mFloatSize;
        NCount mDoubleSize;
    };
    
    /** 预定义着色程序值
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuParamValue : public GpuParamsAlloc
    {
    public:
        GpuParamValue(const String & name);
        virtual ~GpuParamValue();

        /** 集合名字
        @version NIIEngine 3.0.0
        */
        inline const String & getName()             { return mName; }

        /** 添加变量
        @version NIIEngine 3.0.0
        */
        void add(const VString & name, GpuDataType type, NCount count = 1);

        /** 获取变量
        @version NIIEngine 3.0.0
        */
        const GpuParamUnit & get(const VString & name) const;

        /** 移去变量
        @version NIIEngine 3.0.0
        */
        void remove(const VString & name);

        /** 移去所有变量
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, Ni32 in){ set(name, &in, 1); }

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, NIIf in){ set(name, &in, 1); }

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, const Colour & in){ set(name, &in.r, 4); }

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, const Vector4f & in){ set(name, &in.x, 4); }

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, const Vector3f & in){ set(name, &in.x, 3); }
        
        /** 设置数据
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, const Matrix4f & in){ set(name, in[0], 16); }

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const Ni32 * in, NCount cnt);

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const NIIf * in, NCount cnt);

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const NIId * in, NCount cnt);

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, const Matrix4f * in, NCount cnt){ set(name, in[0][0], 16 * cnt); }

        /** 获取定义列表
        @version NIIEngine 3.0.0 高级api
        */
        inline const GpuParamUnitList & getDefList() const          { return mDefines; }

        /** 获取状态掩码
        @version NIIEngine 3.0.0
        */
        inline Nul getStateMark() const                             { return mDirtyMark; }

        /** 获取浮点部分数据
        @version NIIEngine 3.0.0
        */
        inline NIIf * getFloatData(NCount pos) const                { return &mFloatData[pos];}

        /** 获取整形部分数据
        @version NIIEngine 3.0.0
        */
        inline NIId * getDoubleData(NCount pos) const               { return &mDoubleData[pos]; }

        /** 获取整形部分数据
        @version NIIEngine 3.0.0
        */
        inline Ni32 * getIntData(NCount pos) const                  { return &mIntData[pos]; }
    protected:
        String mName;
        GpuParamUnitList mDefines;
        mutable IntArray mIntData;
        mutable FloatArray mFloatData; 
        mutable DoubleArray mDoubleData;
        NCount mIntSize;
        NCount mFloatSize;
        NCount mDoubleSize;
        Nmark mDirtyMark;
    };

    class ShareParamSync;
    /** 着色程序参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuProgramParam : public GpuParamsAlloc
    {
    public:
        enum ParamType
        {
            PT_Const,           ///< 一次性
            PT_Texture          ///< 纹理缓存,uav数据模型,uniform缓存
        };
    public:
        GpuProgramParam();
        GpuProgramParam(const GpuProgramParam & o);
        ~GpuProgramParam();

        GpuProgramParam & operator=(const GpuProgramParam & o);

        /** 设置参数定义
        @version NIIEngine 3.0.0 高级api
        */
        void setDefine(GpuParamDefine * def);

        /** 获取参数定义
        @note 仅适用于命名参数对象 高级api
        */
        inline const GpuParamDefine * getDefine() const{ return mParamDefine; }

        /** 设置参数绑定
        @version NIIEngine 3.0.0 高级api
        */
        void setMap(GpuParamMap * map);

        /** 获取参数绑定
        @version NIIEngine 3.0.0 高级api
        */
        inline const GpuParamMap * getMap() const{ return mParamMap; }

        /** 获取参数定义
        @version NIIEngine 3.0.0
        */
        const GpuParamUnit & getUnit(const VString & name) const;

        /** 设置参数值
        @param[in] index 定义索引
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, NIIf in, NIIf in2 = 0.0f, NIIf in3 = 0.0f, NIIf in4 = 0.0f);

        /** 设置参数值
        @param[in] index 定义索引
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, NIId in, NIId in2 = 0.0f, NIId in3 = 0.0f, NIId in4 = 0.0f);

        /** 设置参数值
        @param[in] index 定义索引
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, Ni32 in, Ni32 in2 = 0, Ni32 in3 = 0, Ni32 in4 = 0);

        /** 设置参数值
        @param[in] index 定义索引
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, const Vector3f & in);

        /** 设置参数值.
        @param[in] index 定义索引
        @version NIIEngine 3.0.0
        */
        inline void set(Nui32 index, const Vector4f & in) { set(index, &in.x, 1); }

        /** 设置参数值.
        @param[in] index 定义索引
        @version NIIEngine 3.0.0
        */
        inline void set(Nui32 index, const Vector4i & in) { set(index, &in.x, 1); }

        /** 设置参数值
        @param[in] index 定义索引
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, const Matrix4f & in);

        /** 设置参数值
        @param[in] index 定义索引
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, const Matrix4f * in, NCount count);

        /** 设置参数值
        @param[in] index 定义索引
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, const Ni32 * in, NCount cnt4x);

        /** 设置参数值
        @param[in] index 定义索引
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, const NIIf * in, NCount cnt4x);

        /** 设置参数值
        @param[in] index 定义索引
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, const NIId * in, NCount cnt4x);

        /** 设置参数值
        @param[in] index 定义索引
        @version NIIEngine 3.0.0
        */
        inline void set(Nui32 index, const Colour & in) { set(index, &in.r, 1); }

        /** 设置扩展数据
        @version NIIEngien 5.0.0
        */
        void setExtData(Nui32 index, Ni32 ext);

        /** 获取扩展数据
        @version NIIEngine 5.0.0
        */
        bool getExtData(Nui32 index, Ni32 & ext) const;

        /** 设置参数值
        @param[in] name 命名参数
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NIIf in);

        /** 设置参数值
        @param[in] name 命名参数
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, Ni32 in);

        /** 设置参数值
        @param[in] name 命名参数
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const Vector4f & in);

        /** 设置参数值
        @param[in] name 命名参数
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const Vector3f & in);

        /** 设置参数值
        @param[in] name 命名参数
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const Matrix4f & in);

        /** 设置参数值
        @param[in] name 命名参数
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const Matrix4f * in, NCount count);

        /** 设置参数值
        @param[in] name 参数的名字
        @param[in] colour 设置为的值
        */
        void set(const VString & name, const Colour & in);

        /** 设置参数值
        @param[in] name 命名参数
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const NIIf * in, NCount count, NCount size = 4);

        /** 设置参数值
        @@param[in] name 命名参数
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const NIId * in, NCount count, NCount size = 4);

        /** 设置参数值
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const Ni32 * in, NCount count, NCount size = 4);

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, NIIf in){ _write(memidx, &in, 1); }

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, Ni32 in){ _write(memidx, &in, 1); }

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, const Vector3f & in){ _write(memidx, &in.x, 3); }

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @param[in] size 实际写入字节
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, const Vector4f & in, NCount size = 4){ _write(memidx, &in.x, size); }

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @param[in] size 实际写入字节
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, const Vector4i & in, NCount size = 4){ _write(memidx, &in.x, size); }

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @param[in] size 实际写入字节
        @version NIIEngine 3.0.0 高级api
        */
        void _write(Nidx memidx, const Matrix4f & in, NCount size);

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @param[in] numEntries Matrix4项的数量
        @version NIIEngine 3.0.0 高级api
        */
        void _write(Nidx memidx, const Matrix4f * in, NCount count);

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, const Colour & in, NCount size = 4){ _write(memidx, &in.r, size); }

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, const NIIf * in, NCount count){
            N_assert(memidx + count <= mFloatData.size(), "error"); memcpy(&mFloatData[memidx], in, sizeof(NIIf) * count); }

        /** 读取缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        inline void _read(Nidx memidx, NCount count, NIIf * out){
            N_assert(memidx + count <= mFloatData.size(), "error"); memcpy(out, &mFloatData[memidx], sizeof(NIIf) * count);}

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, const NIId * in, NCount count){
            N_assert(memidx + count <= mDoubleData.size(), "error"); memcpy(&mDoubleData[memidx], in, sizeof(NIId) * count); }
            
        /** 读取缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        inline void _read(Nidx memidx, NCount count, NIId * out){
            N_assert(memidx + count <= mDoubleData.size(), "error"); memcpy(out, &mDoubleData[memidx], sizeof(NIId) * count); }

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, const Ni32 * in, NCount count){
            N_assert(memidx + count <= mIntData.size(), "error"); memcpy(&mIntData[memidx], in, sizeof(Ni32) * count); }

        /** 读取缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        inline void _read(Nidx memidx, NCount count, Ni32 * out){
            N_assert(memidx + count <= mIntData.size(), "error"); memcpy(out, &mIntData[memidx], sizeof(Ni32) * count); }

        /** 设置同步参数
        @param[in] index
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, GpuSyncParam type, Ni32 ext = 0);

        /** 设置同步参数
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, GpuSyncParam type, NIIf ext);

        /** 设置同步参数
        @param[in] name 参数的名字
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, GpuSyncParam type, Ni32 ext = 0);

        /** 设置同步参数
        @param[in] name 参数的名字
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, GpuSyncParam type, NIIf ext);

        /** 设置同步参数
        @version NIIEngine 3.0.0 高级api
        */
        void _set(Nidx memidx, GpuSyncParam type, Ni32 input, Nmark mark, NCount size = 4);

        /** 设置同步参数
        @version NIIEngine 3.0.0 高级api
        */
        void _set(Nidx memidx, GpuSyncParam type, NIIf input, Nmark mark, NCount size = 4);

        /** 获取同步参数绑定
        @version NIIEngine 3.0.0
        */
        const GpuSParamIndex * getSParamIndex(const VString & name) const;

        /** 获取同步参数绑定
        @param[in] index 参数索引
        @version NIIEngine 3.0.0 高级api
        */
        const GpuSParamIndex * getFloatSParamIndex(Nidx index) const;

        /** 获取同步参数绑定
        @param[in] index 参数索引
        @version NIIEngine 3.0.0 高级api
        */
        const GpuSParamIndex * getDoubleSParamIndex(Nidx index) const;

        /** 获取同步参数绑定
        @param[in] index 参数索引
        @version NIIEngine 3.0.0 高级api
        */
        const GpuSParamIndex * getIntSParamIndex(Nidx index) const;

        /** 获取同步参数绑定
        @param[in] index 缓存索引
        @version NIIEngine 3.0.0
        */
        const GpuSParamIndex * _getFloatSParamIndex(Nidx memidx) const;

        /** 获取同步参数绑定
        @param[in] index 缓存索引
        @version NIIEngine 3.0.0
        */
        const GpuSParamIndex * _getDoubleSParamIndex(Nidx memidx) const;

        /** 获取同步参数绑定
        @param[in] index 缓存索引
        @version NIIEngine 3.0.0
        */
        const GpuSParamIndex * _getIntSyncParamBind(Nidx memidx) const;

        /** 移去同步参数
        @version NIIEngine 3.0.0
        */
        void removeSyncParam(Nidx index);

        /** 移去同步参数
        @version NIIEngine 3.0.0
        */
        void removeSyncParam(const VString & name);

        /** 移去所有同步参数
        @version NIIEngine 3.0.0
        */
        void removeAllSyncParam();

        /** 获取同步参数列表
        @version NIIEngine 3.0.0 高级api
        */
        inline const GpuSParamIndexList & getSyncParamList() const { return mSyncParamList; }

        /** 更新同步参数
        @param[in] src 参数资源
        @param[in] mark GpuParamType
        @version NIIEngine 3.0.0 高级api
        */
        void syncSys(const SysSyncParam * src, Nmark mark);

        /** 添加公共参数
        @version NIIEngine 3.0.0
        */
        void addValue(GpuParamValue * param);

        /** 添加公共参数
        @param[in] param
        @version NIIEngine 3.0.0
        */
        void addValue(const String & param);

        /** 公共参数是否存在
        @version NIIEngine 3.0.0
        */
        bool isValueExist(const String & param) const;

        /** 移去公共参数
        @version NIIEngine 3.0.0
        */
        void removeValue(const String & param);

        /** 移去所有公共参数
        @version NIIEngine 3.0.0
        */
        void removeAllValue();

        /** 同步公共参数
        @version NIIEngine 3.0.0 高级api
        */
        void syncValue();

        /** 是否允许参数丢失
        @version NIIEngine 3.0.0  高级api
        */
        inline void setAllowMissing(bool b){ mAllowParamLost = b;}
        
        /** 是否允许参数丢失
        @version NIIEngine 3.0.0  高级api
        */
        inline bool isAllowMissing() const { return mAllowParamLost;}

        /** 设置是否需要转换矩阵
        @note d3d gles 需要使用转置矩阵,或者设置前就已经转了
        @version NIIEngine 3.0.0 高级api
        */
        inline void setTransposeMatrix(bool b) { mTransposeMatrix = b;}

        /** 获取是否需要转换矩阵
        @note d3d gles 需要使用转置矩阵,或者设置前就已经转了
        @version NIIEngine 3.0.0 高级api
        */
        inline bool isTransposeMatrix() const{ return mTransposeMatrix; }

        /** 复制所有参数值
        @version NIIEngine 3.0.0
        */
        void copyParamValue(const GpuProgramParam & src);

        /** 复制命名参数值
        @version NIIEngine 3.0.0
        */
        void copyNamedParamValue(const GpuProgramParam & src);

        /** 设置多重绘制数量参数
        @version NIIEngine 3.0.0
        */
        void setRenderCount(NIIf c);

        /** 获取多重绘制数量参数
        @version NIIEngine 3.0.0
        */
        NIIf getRenderCount() const;

        /** 获取多重绘制数量参数缓存
        @version NIIEngine 3.0.0
        */
        inline NIIf * getRenderCountPtr() const { return mRenderCountPtr; }

        /** 获取多重绘制数量参数索引
        @version NIIEngine 3.0.0
        */
        inline Nui32 getRenderCountIndex() const{ return mRenderCountIndex; }

        /** 获取多重绘制数量参数内存索引
        @version NIIEngine 3.0.0
        */
        inline Nidx getRenderCountMemIndex() const{ return mRenderCountMemIndex; }

        /** 从缓存索引获取参数索引
        @version NIIEngine 3.0.0
        */
        Nui32 getFloatIndex(Nidx memidx);

        /** 从缓存索引获取参数索引
        @version NIIEngine 3.0.0
        */
        Nui32 getDoubleIndex(Nidx memidx);

        /** 从缓存索引获取参数索引
        @version NIIEngine 3.0.0
        */
        Nui32 getIntIndex(Nidx memidx);

        /** 获取命名参数定义
        @version NIIEngine 3.0.0
        */
        inline const GpuParamUnitList & getDefList() const{ N_assert(mParamDefine, "error"); return mParamDefine->mDefines; }

        /** 获取NIIf常量列表的引用
        @version NIIEngine 3.0.0 高级api
        */
        inline const FloatArray & getFloatData() const{ return mFloatData; }

        /** 获取NIId常量列表的引用
        @version NIIEngine 3.0.0 高级api
        */
        inline const DoubleArray & getDoubleData() const{ return mDoubleData; }

        /** 获取Ni32常量列表的引用
        @version NIIEngine 3.0.0 高级api
        */
        inline const IntArray & getIntData() const { return mIntData; }

        /** 获取浮点缓存
        @version NIIEngine 3.0.0 高级api
        */
        inline NIIf * getFloatData(Nidx pos) const { return &mFloatData[pos]; }

        /** 获取浮点缓存
        @version NIIEngine 3.0.0 高级api
        */
        inline NIId * getDoubleData(Nidx pos) const{ return &mDoubleData[pos]; }

        /** 获取整数缓存
        @version NIIEngine 3.0.0 高级api
        */
        inline Ni32 * getIntData(Nidx pos) const{ return &mIntData[pos]; }

        /** 获取Gpu数据类型大小
        @version NIIEngine 3.0.0
        */
        static NCount getTypeCount(GpuDataType type, bool pad4x);
    protected:
        /** 获取浮点参数绑定
        @version NIIEngine 3.0.0
        */
        GpuParamIndex * getFloatIndex(Nui32 index, NCount size, Nmark typemark);

        /** 获取浮点参数绑定
        @version NIIEngine 3.0.0
        */
        GpuParamIndex * getDoubleIndex(Nui32 index, NCount size, Nmark typemark);

        /** 获取整形参数绑定
        @version NIIEngine 3.0.0
        */
        GpuParamIndex * getIntIndex(Nui32 index, NCount size, Nmark typemark);

        /** 获取参数种类
        @version NIIEngine 3.0.0
        */
        Nmark getParamTypeMark(GpuSyncParam sparam);
    protected:
        typedef vector<ShareParamSync *>::type ShareSyncList;
        typedef map<Nui32, Ni32>::type ExtDataList;
    protected:
        GpuParamDefine * mParamDefine;
        GpuParamMap * mParamMap;
        GpuSParamIndexList mSyncParamList;
        ShareSyncList mShareSyncList;
        ExtDataList mExtDataList;
        mutable IntArray mIntData;
        mutable FloatArray mFloatData;
        mutable DoubleArray mDoubleData;
        NIIf * mRenderCountPtr;
        Nui32 mRenderCountIndex;
        Nidx mRenderCountMemIndex;
        Nmark mParamTypeMark;
        bool mTransposeMatrix;
        bool mAllowParamLost;
    };

    inline void GpuProgramParam::set(Nui32 index, NIIf in, NIIf in2, NIIf in3, NIIf in4)
    {
        NIIf temp[4] = { in, in2, in3, in4 }; set(index, temp, 1);
    }

    inline void GpuProgramParam::set(Nui32 index, NIId in, NIId in2, NIId in3, NIId in4)
    {
        NIId temp[4] = { in, in2, in3, in4 }; set(index, temp, 1);
    }

    inline void GpuProgramParam::set(Nui32 index, Ni32 in, Ni32 in2, Ni32 in3, Ni32 in4)
    {
        Ni32 temp[4] = { in, in2, in3, in4 }; set(index, temp, 1);
    }

    inline void GpuProgramParam::set(Nui32 index, const Vector3f & in)
    {
        NIIf temp[4] = { in.x, in.y, in.z, 1.0f }; set(index, temp, 1);
    }
}
#endif
