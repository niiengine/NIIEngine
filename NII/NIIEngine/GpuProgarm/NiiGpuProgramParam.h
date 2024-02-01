﻿/*
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
#include "NiiMatrix4.h"
#include "NiiColour.h"
#include "NiiTexture.h"
#include "NiiBitSet.h"

namespace NII
{
    /** Gpu程序参数绑定类型
    @version NIIEngine 3.0.0
    */
    enum GpuBindType
    {
        GPT_VS              = 0x01,
        GPT_TS              = 0x02,
        GPT_DS              = 0x04,
        GPT_GS              = 0x08,
        GPT_FS              = 0x10,
        GPT_CS              = 0x20,
        GPT_ProgramMark     = 0x3F,
        GPT_VS_Param        = 0x40,
        GPT_TS_Param        = 0x80,
        GPT_DS_Param        = 0x100,
        GPT_GS_Param        = 0x200,
        GPT_FS_Param        = 0x400,
        GPT_CS_Param        = 0x800,
        GPT_ParamMark       = 0xFC0,
        GPT_GPGPMark        = 0xFFF,
        GPT_View_Param      = 0x1000,   ///< 视图空间
        GPT_Space_Param     = 0x2000,   ///< 模型空间
        GPT_Colour_Param    = 0x4000,
        GPT_Light_Param     = 0x8000,
        GPT_Texture_Param   = 0x10000,
        GPT_Fog_Param       = 0x20000,
        GPT_Fps_Param       = 0x40000,
        GPT_ShaderCh_Param  = 0x80000,
        GPT_Other_Param     = 0x100000,
        GPT_Queue_Param     = 0x200000,
        GPT_Frame_Param     = 0x300000,
        GPT_Render_Param    = GPT_View_Param | GPT_Colour_Param | GPT_Texture_Param | GPT_Fog_Param | GPT_Fps_Param | GPT_Other_Param,
        GPT_TypeParamMark   = 0xFFF000,
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
        GDT_64Int           = 34,
        GDT_64Int2X         = 35,
        GDT_64Int3X         = 36,
        GDT_64Int4X         = 37,
        GDT_64UInt          = 38,
        GDT_64UInt2X        = 39,
        GDT_64UInt3X        = 40,
        GDT_64UInt4X        = 41,
        GDT_Double          = 42,
        GDT_Double2X        = 43,
        GDT_Double3X        = 44,
        GDT_Double4X        = 45,
        GDT_Matrixd2X2      = 46,
        GDT_Matrixd2X3      = 47,
        GDT_Matrixd2X4      = 48,
        GDT_Matrixd3X2      = 49,
        GDT_Matrixd3X3      = 50,
        GDT_Matrixd3X4      = 51,
        GDT_Matrixd4X2      = 52,
        GDT_Matrixd4X3      = 53,
        GDT_Matrixd4X4      = 54,
        GDT_Block           = 55
    };
    /** Gpu内部同步参数
    @version NIIEngine 3.0.0
    */
    enum GpuSyncParam
    {
        GSP_Null = 0,
        GSP_Matrix_Proj,                ///< 投影矩阵
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
    
    union SyncParamInputData
    {
        Ni32 mInputInt;
        NIIf mInputFloat;
        NIId mInputDouble;
    };

    inline bool isFloat(GpuDataType c)      { return c > GDT_Unknow && c < GDT_Int; }
    inline bool isDouble(GpuDataType c)     { return c > GDT_SamplerArray2D && c < GDT_Block; }
    inline bool isSampler(GpuDataType c)    { return c > GDT_Bool4X && c < GDT_64Int; }
    inline bool isInt(GpuDataType c)        { return c > GDT_Matrix4X4 && c < GDT_UInt; }
    inline bool isUInt(GpuDataType c)       { return c > GDT_Int4X && c < GDT_Bool; }
    inline bool isBool(GpuDataType c)       { return c > GDT_UInt4X && c < GDT_Sampler1D; }
    inline bool isBlock(GpuDataType c)      { return c == GDT_Block; }
        
    /** 着程序自定义参数
    @version NIIEngine 3.0.0
    */
    struct _EngineAPI GpuParamUnit
    {
        GpuParamUnit(const GpuParamUnit & o);
        GpuParamUnit(Nui16 syncParam, Nui32 unitCnt, Nui16 inputType, Nui16 dataType);
        GpuParamUnit(Nui16 syncParam, Nui32 unitCnt, Nui16 inputType, Nui16 dataType);

        inline bool isFloat() const         { return isFloat(mDataType); }

        inline bool isDouble() const        { return isDouble(mDataType); }

        inline bool isSampler() const       { return isSampler(mDataType); }

        inline bool isInt() const           { return isInt(mDataType); }

        inline bool isUInt() const          { return isUInt(mDataType); }

        inline bool isBool() const          { return isBool(mDataType); }

        inline bool isBlock() const         { return isBlock(mDataType); }
        
        static const GpuParamUnit UnitNone;

        Nidx mMemIndex;
        Nui32 mGBTMark;             ///< GpuBindType
        Nui32 mUnitCount;           ///< Unit数量
        Nui16 mUnit32bSize;         ///< Unit(32位,4字节)大小
        Nui16 mSyncParam;           ///< GpuSyncParam
        Nui16 mIndex;               ///< hwbuf idx
        Nui16 mArrayIndex;          ///< 数组索引
        Nui16 mDataType;
        Nui16 mInputType;           ///< SyncParamInput
        bool mArrayMode;
        bool mValidValue;           ///< 辅助
    };
    typedef map<VString, GpuParamUnit>::type GpuParamUnitList;

    /** 着色参数内存绑定
    @version NIIEngine 3.0.0
    */
    struct _EngineAPI GpuParamBlock
    {
    public:
        GpuParamBlock() : mMemIndex(0), mDataType(GDT_Unknow), m32bSize(0), mGBTMark(GPT_Render_Param) {}

        GpuParamBlock(Nui16 idx, Nidx memidx, Nui16 dataType, Nui32 _32bSize, Nui32 gbtMark) :
            mIndex(idx), mMemIndex(memidx), mDataType(dataType), mSyncParam(GSP_Null), m32bSize(_32bSize), mGBTMark(gbtMark) {}

        GpuParamBlock(Nui16 idx, Nidx memidx, Nui16 syncParam, Ni32 inputInt, Nui32 _32bSize, Nui32 gbtMark) :
            mIndex(idx), mMemIndex(memidx), mDataType(GDT_Float), mSyncParam(syncParam), m32bSize(_32bSize), mGBTMark(gbtMark), mInputData.mInputInt(inputInt){}

        GpuParamBlock(Nui16 idx, Nidx memidx, Nui16 syncParam, NIIf inputFloat, Nui32 _32bSize, Nui32 gbtMark) :
            mIndex(idx),  mMemIndex(memidx), mDataType(GDT_Float), mSyncParam(syncParam), m32bSize(_32bSize), mGBTMark(gbtMark), mInputData.mInputFloat(inputFloat){}

        GpuParamBlock(Nui16 idx, Nidx memidx, Nui16 syncParam, NIId inputDouble, Nui32 _32bSize, Nui32 gbtMark) :
            mIndex(idx),  mMemIndex(memidx), mDataType(GDT_Float), mSyncParam(syncParam), m32bSize(_32bSize), mGBTMark(gbtMark), mInputData.mInputDouble(inputDouble){}
            
        inline bool isFloat() const         { return isFloat(mDataType); }

        inline bool isDouble() const        { return isDouble(mDataType); }

        inline bool isSampler() const       { return isSampler(mDataType); }

        inline bool isInt() const           { return isInt(mDataType); }

        inline bool isUInt() const          { return isUInt(mDataType); }

        inline bool isBool() const          { return isBool(mDataType); }

        inline bool isBlock() const         { return isBlock(mDataType); }
    public:
        Nidx mMemIndex;
        Nui32 m32bSize; ///< mUnitCount * mUnit32bSize
        Nui32 mGBTMark;
        Nui16 mIndex;
        Nui16 mDataType;
        Nui16 mSyncParam;
        Nui16 mPadding;
        SyncParamInputData mInputData;
    };
    typedef map<Nui32, GpuParamBlock>::type GpuParamBlockMap;
    typedef vector<GpuParamBlock *>::type GpuParamBlockList;

    /** 着色程序自定义参数集
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuParamDefine : public GpuParamsAlloc
    {
        friend class GpuProgramParam;
    public:
        GpuParamDefine();
        GpuParamDefine(const GpuParamUnitList & defs);

        GpuParamDefine & operator = (const GpuParamDefine & o);

        /** 添加定义
        @version NIIEngine 3.0.0
        */
        void add(const VString & name, const GpuParamUnit & def, bool arrayMode = false);

        /** 获取定义
        @version NIIEngine 3.0.0
        */
        GpuParamUnit * get(const VString & name) const;

        /** 移去定义
        @version NIIEngine 3.0.0
        */
        void remove(const VString & name);

        /** 移去所有定义
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** 获取列表
        @version NIIEngine 3.0.0
        */
        inline const GpuParamUnitList & getList() const     { return mDefineList; }

        /** 获取大小
        @version NIIEngine 3.0.0
        */
        inline NCount getSize() const                       { return m32bSize; }

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
        GpuParamUnitList mDefineList;
        NCount m32bSize;
    };
    
    /** 着色参数缓存单元
    @note glsl和hlsl的常量结构缓存都是有字节对齐要求的,一般是128位操作.具体的着色程序有具体的打包对齐规定.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuParamBufferUnit : public GpuParamsAlloc
    {
    public:
        enum ParamType
        {
            PT_Storage,         ///< (ShaderStorage, uav)
            PT_Sampler          ///< 纹理缓存
        };

        enum OpType
        {
            OT_Unknow   = 0,
            OT_READ     = 0x01,
            OT_WRITE    = 0x02,
            OT_RW       = OT_READ|OT_WRITE
        };

        struct StorageUnit
        {
            StorageUnit()             { memset(this, 0, sizeof(StorageUnit));}

            inline StorageUnit & operator= (const StorageUnit & o) const
            {
                mBuffer = o.mBuffer;
                mOffset = o.mOffset;
                mSize = o.mSize;
            }

            inline bool operator != (const StorageUnit & o) const
            {
                if (mBuffer != o.mBuffer || mOffset != o.mOffset || mSize != o.mSize)
                    return true;
                return false;
            }

            inline bool operator == (const StorageUnit & o) const
            {
                if (mBuffer == o.mBuffer && mOffset == o.mOffset && mSize == o.mSize)
                    return true;
                return false;
            }
            inline bool operator < (const StorageUnit & o) const
            {
                if (mBuffer < o.mBuffer || mOffset < o.mOffset || mSize < o.mSize)
                    return true;
                return false;
            }
            
            StorageBuffer * mBuffer;
            NCount mOffset;
            NCount mSize;
            PixelFormat mPadding;   ///< no using
        };

        struct SamplerUnit
        {
            SamplerUnit()             { memset(this, 0, sizeof(SamplerUnit));}
            SamplerUnit(Texture * tex, PixelFormat pf): mTexture(tex), mMipmap(0), mArrayIndex(0)
            {
                if(pf == PF_UNKNOWN)
                    mFormat = PixelUtil::getLinear(tex->getFormat());
                else
                    mFormat = pf;
            }

            inline SamplerUnit & operator= (const SamplerUnit & o) const
            {
                mTexture = o.mTexture;
                mMipmap = o.mMipmap;
                mArrayIndex = o.mArrayIndex;
                mFormat = o.mFormat;
            }
            
            inline bool operator != (const SamplerUnit & o) const
            {
                if (mTexture != o.mTexture || mMipmap != o.mMipmap || mArrayIndex != o.mArrayIndex || mFormat != o.mFormat)
                    return true;
                return false;
            }

            inline bool operator == (const SamplerUnit & o) const
            {
                if (mTexture == o.mTexture && mMipmap == o.mMipmap && mArrayIndex == o.mArrayIndex && mFormat == o.mFormat)
                    return true;
                return false;
            }

            inline bool operator < (const SamplerUnit & o) const
            {
                if (mTexture < o.mTexture || mMipmap < o.mMipmap || mArrayIndex < o.mArrayIndex || mFormat < o.mFormat)
                    return true;
                return false;
            }
            
            inline bool isValidView() const
            {
                return mFormat == mTexture->getFormat() && mMipmap == 0 && mArrayIndex == 0;
            }
            
            Texture * mTexture;
            NCount mMipmap;
            NCount mArrayIndex;
            PixelFormat mFormat;   ///<default : mTexture->getFormat()
        };
    public:
        GpuParamBufferUnit();
        ~GpuParamBufferUnit();
        
        inline bool empty() const
        {
            return mStorage.mBuffer == 0 && mSampler.mTexture == 0;
        }

        inline bool isStorage() const
        {
            return mParamType == PT_Storage;
        }
        
        inline bool isTexture() const
        {
            return mParamType == PT_Sampler;
        }

        inline StorageUnit & getBuffer()
        {
            N_assert(mParamType == PT_Storage);
            return mStorage;
        }

        inline const StorageUnit & getBuffer() const
        {
            N_assert(mParamType == PT_Storage);
            return mStorage;
        }

        inline SamplerUnit & getTexture()
        {
            N_assert(mParamType == PT_Sampler);
            return mSampler;
        }

        inline const SamplerUnit & getTexture() const
        {
            N_assert(mParamType == PT_Sampler);
            return mSampler;
        }

        bool operator != (const GpuParamBufferUnit & o) const
        {
            if (mParamType == o.mParamType && mOpType == o.mOpType)
            {
                return mParamType == PT_Storage ? mStorage != o.mStorage : mSampler != o.mSampler;
            }
            return true;
        }

        bool operator == (const GpuParamBufferUnit & o) const
        {
            if (mParamType == o.mParamType && mOpType == o.mOpType)
            {
                return mParamType == PT_Storage ? mStorage == o.mStorage : mSampler == o.mSampler;
            }
            return false;
        }

        bool operator < (const GpuParamBufferUnit & o) const
        {
            if (mParamType < o.mParamType || mOpType < o.mOpType)
            {
                return mParamType == PT_Storage ? mStorage < o.mStorage : mSampler < o.mSampler;
            }
            return false;
        }
        
        /** 设置参数值
        @param[in] oft 偏移(单位:字节)
        @version NIIEngine 3.0.0
        */
        void set(NCount oft, NIIf in, NIIf in2 = 0.0f, NIIf in3 = 0.0f, NIIf in4 = 0.0f);

        /** 设置参数值
        @param[in] oft 偏移(单位:字节)
        @version NIIEngine 3.0.0
        */
        void set(NCount oft, NIId in, NIId in2 = 0.0f, NIId in3 = 0.0f, NIId in4 = 0.0f);

        /** 设置参数值
        @param[in] oft 偏移(单位:字节)
        @version NIIEngine 3.0.0
        */
        void set(NCount oft, Ni32 in, Ni32 in2 = 0, Ni32 in3 = 0, Ni32 in4 = 0);
        
        /** 设置参数值
        @param[in] oft 偏移(单位:字节)
        @version NIIEngine 3.0.0
        */
        void set(NCount oft, const bool * in, NCount cnt = 1);

        /** 设置参数值
        @param[in] oft 偏移(单位:字节)
        @version NIIEngine 3.0.0
        */
        void set(NCount oft, const Ni32 * in, NCount cnt = 1);

        /** 设置参数值
        @param[in] oft 偏移(单位:字节)
        @version NIIEngine 3.0.0
        */
        void set(NCount oft, const NIIf * in, NCount cnt = 1);

        /** 设置参数值
        @param[in] oft 偏移(单位:字节)
        @version NIIEngine 3.0.0
        */
        void set(NCount oft, const NIId * in, NCount cnt = 1);

        /** 设置参数值
        @param[in] oft 偏移(单位:字节)
        @version NIIEngine 3.0.0
        */
        inline void set(NCount oft, const Vector3f * in, NCount cnt = 1)        { set(oft, in[0].data(), 3 * cnt); }

        /** 设置参数值.
        @param[in] oft 偏移(单位:字节)
        @version NIIEngine 3.0.0
        */
        inline void set(NCount oft, const Vector4f * in, NCount cnt = 1)        { set(oft, in[0].data(), 4 * cnt); }

        /** 设置参数值.
        @param[in] oft 偏移(单位:字节)
        @version NIIEngine 3.0.0
        */
        inline void set(NCount oft, const Vector4i * in, NCount cnt = 1)        { set(oft, in[0].data(), 4 * cnt); }
        
        /** 设置参数值
        @param[in] oft 偏移(单位:字节)
        @version NIIEngine 3.0.0
        */
        inline void set(NCount oft, const Colour * in, NCount cnt = 1)          { set(oft, &in.r, 4 * cnt); }

        /** 设置参数值
        @param[in] oft 偏移(单位:字节)
        @version NIIEngine 3.0.0
        */
        inline void set(NCount oft, const Matrix4f * in, NCount cnt = 1)        { set(oft, &in.data(), 16 * cnt); }
    public:
        OpType mOpType;
        ParamType mParamType;
        union
        {
            StorageUnit mStorage;
            SamplerUnit mSampler;
        };
        NCount mRefCount;
        bool mAutoDestroy;
    };
    typedef vector<GpuParamBufferUnit>::type GpuParamBufferUnitList;
    
    /** 着色参数缓存
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuParamBuffer : public GpuParamsAlloc
    {
    public:
        GpuParamBuffer() {}
        virtual ~GpuParamBuffer() {}

        bool operator == (const GpuParamBuffer & o) const;

        bool operator < (const GpuParamBuffer & o) const;

        /** 添加绑定点缓存
        @version NIIEngine 3.0.0
        */
        void addBinding(Nidx slot, const GpuParamBufferUnit & unit);

        /** 移去绑定点缓存
        @version NIIEngine 3.0.0
        */
        void removeBinding(Nidx slot);

        /** 获取绑定点缓存
        @version NIIEngine 3.0.0
        */
        const GpuParamBufferUnit * getBinding(Nidx slot) const;

        /** 获取绑定点缓存数量
        @version NIIEngine 3.0.0
        */
        inline NCount getBindingCount() const           { return mSlotList.size(); }

        /** 添加绑定点
        @version NIIEngine 3.0.0
        */
        inline void addBindingPoint(const VString & name, Nidx slot){ mNamedSlotList.insert_or_assign(Npair(name, slot));}

        /** 移去绑定点
        @version NIIEngine 3.0.0
        */
        void removeBindingPoint(const VString & name);

        /** 获取绑定点
        @version NIIEngine 3.0.0
        */
        Nidx getBindingPoint(const VString & name) const;

        /** 获取绑定点数量
        @version NIIEngine 3.0.0
        */
        inline NCount getBindingPointsCount() const     { return mNamedSlotList.size();}

        /** 所有成分是否有效
        @version NIIEngine 5.0.0
        */
        bool isValid() const;
    protected:
        typedef map<VString, Nidx>::type NamedBindList;//<GPUProgram name, GPUbinding[Nidx]
        typedef map<Nidx, Nidx>::type SlotList; //<mUnitList[Nidx], GPUbinding[Nidx]
    protected:
        GpuParamBufferUnitList mUnitList;
        NamedBindList mNamedSlotList;
        SlotList mSlotList;
    };
    typedef vector<GpuParamBuffer>::type GpuParamBufferList;

    /** 预定义着色程序值
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuParamValue : public GpuParamsAlloc
    {
    public:
        GpuParamValue(const String & name);
        virtual ~GpuParamValue();
        
        /** 更新值到程序参数中
        @param reset 即使值有效也重复设置
        @version NIIEngine 5.0.0
        */
        void update(GpuProgramParam * params, bool reset = true);

        /** 集合名字
        @version NIIEngine 3.0.0
        */
        inline const String & getName() const           { return mName; }

        /** 添加变量
        @version NIIEngine 3.0.0
        */
        void add(const VString & name, GpuDataType type, NCount count = 1);
        
        /** 添加变量
        @param[in] extData 需要查看
        @version NIIEngine 3.0.0
        */
        void add(const VString & name, GpuSyncParam param, SyncParamInputData extData);

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
        
        /** 优化存储
        @version NIIEngine 6.0.0
        */
        inline void reserve(NCount size32b)                                     { mBufferData.reserve(size32b); }

        /** 设置数据
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, NCount oft32b, Ni32 in)           { set(name, oft32b, &in, 1); }

        /** 设置数据
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, NCount oft32b, NIIf in)           { set(name, oft32b, &in, 1); }

        /** 设置数据
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, NCount oft32b, const Colour & in) { set(name, oft32b, &in.r, 4); }

        /** 设置数据
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, NCount oft32b, const Vector3f & in){ set(name, oft32b, &in.x, 3); }
        
        /** 设置数据
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, NCount oft32b, const Vector4f & in){ set(name, oft32b, &in.x, 4); }
        
        /** 设置数据
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, NCount oft32b, const Matrix4f & in){ set(name, oft32b, in[0], 16); }
        
        /** 设置数据
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, NCount oft32b, const Matrix4d & in){ set(name, oft32b, in[0], 16); }

        /** 设置数据
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NCount oft32b, const Ni32 * in, NCount cnt);

        /** 设置数据
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NCount oft32b, const NIIf * in, NCount cnt);

        /** 设置数据
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NCount oft32b, const NIId * in, NCount cnt);

        /** 设置数据
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, NCount oft32b, const Vector3f * in, NCount cnt){ set(name, oft32b, &in.x, 3 * cnt, 1); }
        
        /** 设置数据
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, NCount oft32b, const Vector4f * in, NCount cnt){ set(name, oft32b, &in.x, 4 * cnt, 1); }
        
        /** 设置数据
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, NCount oft32b, const Matrix4f * in, NCount cnt){ set(name, oft32b, in[0][0], 16 * cnt, 1); }

        /** 设置数据
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, NCount oft32b, const Matrix4d * in, NCount cnt) { set(name, oft32b, in[0][0], 16 * cnt, 1); }

        /** 获取定义列表
        @version NIIEngine 3.0.0 高级api
        */
        inline GpuParamUnitList & getDefList()                      { return mDefines; }
        
        /** 获取定义列表
        @version NIIEngine 3.0.0 高级api
        */
        inline const GpuParamUnitList & getDefList() const          { return mDefines; }
        
        /** 手动设置状态
        @version NIIEngine 3.0.0
        */
        inline void setStateMark() const                            { ++mDirtyMark; }

        /** 获取状态掩码
        @version NIIEngine 3.0.0
        */
        inline Nul getStateMark() const                             { return mDirtyMark; }

        /** 获取数据
        @param[in] oft 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        inline void * getBufferData(NCount pos = 0) const           { return &mBufferData[pos];}

        /** 读出数据到着色程序结构体缓存中
        @note 注意结构体的成员对齐方式
        @param[in] dstoft 偏移(单位:字节)
        @param[in] oft32b 偏移(单位:4字节)
        @param[in] size32b 读取大小(单位:4字节)
        @version NIIEngine 3.0.0
        */
        inline void read(StructBuffer * dst, NCount dstoft, NCount oft32b, NCount size32b)
        {
            dst->write(&mBufferData[oft32b], dstoft, size32b * 4);
        }

        /** 从着色程序结构体缓存中写入数据
        @note 注意结构体的成员对齐方式
        @param[in] srcoft 偏移(单位:字节)
        @param[in] oft32b 偏移(单位:4字节)
        @param[in] size32b 写入大小(单位:4字节)
        @version NIIEngine 3.0.0
        */
        inline void write(StructBuffer * src, NCount srcoft, NCount oft32b, NCount size32b)
        {
            if (mBufferData.size() < oft32b + size32b)
                mBufferData.resize(oft32b + size32b);
            src->read(&mBufferData[oft32b], srcoft, size32b * 4);
        }
    protected:
        typedef map<VString, SyncParamInputData>::type InputDataList;
    protected:
        String mName;
        GpuParamUnitList mDefines;
        InputDataList mInputDataList;
        mutable GBufferArray mBufferData; 
        NCount mDataSize;
        NCount mDirtyMark;
    };

    class ParamValueSync;
    /** 着色程序参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuProgramParam : public GpuParamsAlloc
    {
    public:
        typedef vector<ParamValueSync *>::type ShareSyncList;
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
        inline const GpuParamDefine * getDefine() const     { return mParamDefine; }

        /** 获取参数定义
        @version NIIEngine 3.0.0
        */
        const GpuParamUnit & getUnit(const VString & name) const;

        /** 定义参数
        @version NIIEngine 3.0.0
        */
        void define(Nui32 index, Nui32 gbtMark, GpuDataType dataType, NCount cnt);

        /** 取消定义
        @version NIIEngine 3.0.0
        */
        void undefine(Nui32 index);

        /** 设置参数值
        @param[in] index 定义索引
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, NCount oft32b, NIIf in, NIIf in2 = 0.0f, NIIf in3 = 0.0f, NIIf in4 = 0.0f);

        /** 设置参数值
        @param[in] index 定义索引
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, NCount oft32b, NIId in, NIId in2 = 0.0f, NIId in3 = 0.0f, NIId in4 = 0.0f);

        /** 设置参数值
        @param[in] index 定义索引
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, NCount oft32b, Ni32 in, Ni32 in2 = 0, Ni32 in3 = 0, Ni32 in4 = 0);

        /** 设置参数值
        @param[in] index 定义索引
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, NCount oft32b, const Vector3f & in);

        /** 设置参数值.
        @param[in] index 定义索引
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        inline void set(Nui32 index, NCount oft32b, const Vector4f & in) { set(index, oft32b, &in.x, 1); }

        /** 设置参数值.
        @param[in] index 定义索引
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        inline void set(Nui32 index, NCount oft32b, const Vector4i & in) { set(index, oft32b, &in.x, 1); }

        /** 设置参数值
        @param[in] index 定义索引
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, NCount oft32b, const Matrix4f & in);

        /** 设置参数值
        @param[in] index 定义索引
        @param[in] oft32b 偏移(单位:4字节)
        @param[in] cnt 个数(单位:Matrix4f)
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, NCount oft32b, const Matrix4f * in, NCount cnt);

        /** 设置参数值
        @param[in] index 定义索引
        @param[in] oft32b 偏移(单位:4字节)
        @param[in] cnt4x 参数为1则写入4个Ni32，参数为2则写入8个Ni32. ...(单位: 4*Nui32)
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, NCount oft32b, const Ni32 * in, NCount cnt4x);

        /** 设置参数值
        @param[in] index 定义索引
        @param[in] oft32b 偏移(单位:4字节)
        @param[in] cnt4x 参数为1则写入4个NIIf，参数为2则写入8个NIIf. ...(单位: 4*Nui32)
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, NCount oft32b, const NIIf * in, NCount cnt4x);

        /** 设置参数值
        @param[in] index 定义索引
        @param[in] oft32b 偏移(单位:4字节)
        @param[in] cnt4x 参数为1则写入4个NIId，参数为2则写入8个NIId. ...(单位: 4*Nui32)
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, NCount oft32b, const NIId * in, NCount cnt4x);

        /** 设置参数值
        @param[in] index 定义索引
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        inline void set(Nui32 index, NCount oft32b, const Colour & in) { set(index, oft32b, &in.r, 1); }

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
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NCount oft32b, NIIf in);

        /** 设置参数值
        @param[in] name 命名参数
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NCount oft32b, Ni32 in);

        /** 设置参数值
        @param[in] name 命名参数
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NCount oft32b, const Vector4f & in);

        /** 设置参数值
        @param[in] name 命名参数
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NCount oft32b, const Vector3f & in);

        /** 设置参数值
        @param[in] name 命名参数
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NCount oft32b, const Matrix4f & in);

        /** 设置参数值
        @param[in] name 命名参数
        @param[in] oft32b 偏移(单位:4字节)
        @param[in] cnt 写入Matrix4f的数量
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NCount oft32b, const Matrix4f * in, NCount cnt);

        /** 设置参数值
        @param[in] name 参数的名字
        @param[in] oft32b 偏移(单位:4字节)
        @param[in] colour 设置为的值
        */
        void set(const VString & name, NCount oft32b, const Colour & in);

        /** 设置参数值
        @param[in] name 命名参数
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NCount oft32b, const NIIf * in, NCount count, NCount size32b = 4);

        /** 设置参数值
        @@param[in] name 命名参数
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NCount oft32b, const NIId * in, NCount count, NCount size64b = 4);

        /** 设置参数值
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NCount oft32b, const Ni32 * in, NCount count, NCount size32b = 4);

        /** 写入缓存数据(直接操作缓存,需要明确的oft32b)
        @param[in] memidx 缓存存储索引
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, NCount oft32b, NIIf in)                { _write(memidx + oft32b, &in, 1); }

        /** 写入缓存数据(直接操作缓存,需要明确的oft32b)
        @param[in] memidx 缓存存储索引
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, NCount oft32b, Ni32 in)                { _write(memidx + oft32b, &in, 1); }

        /** 写入缓存数据(直接操作缓存,需要明确的oft32b)
        @param[in] memidx 缓存存储索引
        @param[in] oft32b 偏移(单位:4字节)
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, NCount oft32b, const Vector3f & in)    { _write(memidx + oft32b, &in.x, 3); }

        /** 写入缓存数据(直接操作缓存,需要明确的oft32b)
        @param[in] memidx 缓存存储索引
        @param[in] oft32b 偏移(单位:4字节)
        @param[in] size32b 实际写入(单位:4字节)
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, NCount oft32b, const Vector4f & in, NCount size32b = 4){ _write(memidx + oft32b, &in.x, size); }

        /** 写入缓存数据(直接操作缓存,需要明确的oft32b)
        @param[in] memidx 缓存存储索引
        @param[in] oft32b 偏移(单位:4字节)
        @param[in] size32b 实际写入(单位:4字节)
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, NCount oft32b, const Vector4i & in, NCount size32b = 4){ _write(memidx + oft32b, &in.x, size); }

        /** 写入缓存数据(直接操作缓存,需要明确的oft32b)
        @param[in] memidx 缓存存储索引
        @param[in] oft32b 偏移(单位:4字节)
        @param[in] size32b 实际写入(单位:4字节)
        @version NIIEngine 3.0.0 高级api
        */
        void _write(Nidx memidx, NCount oft32b, const Matrix4f & in, NCount size32b = 16);

        /** 写入缓存数据(直接操作缓存,需要明确的oft32b)
        @param[in] memidx 缓存存储索引
        @param[in] oft32b 偏移(单位:4字节)
        @param[in] cnt 写入Matrix4f的数量
        @version NIIEngine 3.0.0 高级api
        */
        void _write(Nidx memidx, NCount oft32b, const Matrix4f * in, NCount cnt);

        /** 写入缓存数据(直接操作缓存,需要明确的oft32b)
        @param[in] memidx 缓存存储索引
        @param[in] oft32b 偏移(单位:4字节)
        @param[in] size32b 实际写入(单位:4字节)
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, NCount oft32b, const Colour & in, NCount size32b = 4){ _write(memidx + oft32b, &in.r, size32b); }

        /** 写入缓存数据(直接操作缓存,需要明确的memidx)
        @param[in] memidx 缓存存储索引(单位:4字节)
        @param[in] size32b 实际写入(单位:4字节)
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, const void * in, NCount size32b = 1){
            N_assert(memidx + size32b <= mBufferData.size(), "error"); memcpy(&mBufferData[memidx], in, sizeof(NIIf) * size32b); }

        /** 读取缓存数据(直接操作缓存,需要明确的memidx)
        @param[in] memidx 缓存存储索引(单位:4字节)
        @version NIIEngine 3.0.0 高级api
        */
        inline void _read(Nidx memidx, void * out, NCount size32b = 1) const{
            N_assert(memidx + size32b <= mBufferData.size(), "error"); memcpy(out, &mBufferData[memidx], sizeof(NIIf) * size32b);}

        /** 设置缓存数据大小
        @param[in] size32b 实际写入(单位:4字节)
        @version NIIEngine 3.0.0 高级api
        */
        inline void _resize(NCount size32b) { 
            if (mBufferData.size() < size32b) mBufferData.resize(size32b, 0.0f); }

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

        /** 设置同步参数(直接操作缓存,需要明确的memidx)
        @version NIIEngine 3.0.0 高级api
        */
        void _set(Nidx memidx, GpuSyncParam type, Ni32 input, Nui32 gbtMark, NCount size = 4);

        /** 设置同步参数(直接操作缓存,需要明确的memidx)
        @version NIIEngine 3.0.0 高级api
        */
        void _set(Nidx memidx, GpuSyncParam type, NIIf input, Nui32 gbtMark, NCount size = 4);

        /** 获取同步参数绑定
        @version NIIEngine 3.0.0
        */
        const GpuParamBlock * getSyncParamBlock(const VString & name) const;

        /** 获取同步参数绑定
        @param[in] index 参数索引
        @version NIIEngine 3.0.0 高级api
        */
        const GpuParamBlock * getSyncParamBlock(Nidx index) const;

        /** 获取同步参数绑定
        @param[in] memidx 缓存索引
        @version NIIEngine 3.0.0
        */
        const GpuParamBlock * _getSyncParamBlock(Nidx memidx) const;

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
        inline const GpuParamBlockList & getSyncParamList() const { return mSyncBlockList; }

        /** 更新同步参数
        @param[in] src 参数资源
        @param[in] mark GpuBindType
        @version NIIEngine 3.0.0 高级api
        */
        void sync(const SysSyncParam * src, Nmark mark);

        /** 添加公共参数
        @version NIIEngine 3.0.0
        */
        void addValue(GpuParamValue * param);

        /** 添加公共参数
        @param[in] param
        @version NIIEngine 3.0.0
        */
        void addValue(const String & param);

        /** 获取公共参数是否存在
        @version NIIEngine 3.0.0
        */
        bool isValueExist(const String & param) const;

        /** 移去公共参数
        @version NIIEngine 3.0.0
        */
        void removeValue(const String & param);
        
        /** 获取公共参数
        @version NIIEngine 4.0.0
        */
        GpuParamValue * getValue(Nidx idx) const;
        
        /** 获取公共参数总数
        @version NIIEngine 4.0.0
        */
        inline NCount getValueCount() const                     { return mShareSyncList.size(); }

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
        inline void setAllowMissing(bool b)                     { mAllowParamLost = b;}
        
        /** 是否允许参数丢失
        @version NIIEngine 3.0.0  高级api
        */
        inline bool isAllowMissing() const                      { return mAllowParamLost;}

        /** 设置是否需要转置矩阵
        @note d3d gles 需要使用转置矩阵,或者设置前就已经转置了
        @version NIIEngine 3.0.0 高级api
        */
        inline void setTransposeMatrix(bool b)                  { mTransposeMatrix = b;}

        /** 获取是否需要转置矩阵
        @note d3d gles 需要使用转置矩阵,或者设置前就已经转置了
        @version NIIEngine 3.0.0 高级api
        */
        inline bool isTransposeMatrix() const                   { return mTransposeMatrix; }

        /** 复制所有参数值
        @version NIIEngine 3.0.0
        */
        void copyParamValue(const GpuProgramParam & src);

        /** 复制定义参数值
        @version NIIEngine 3.0.0
        */
        void copyDefineValue(const GpuProgramParam & src);

        /** 设置多重绘制数量参数
        @version NIIEngine 3.0.0
        */
        inline void setRenderCount(NIIf c)                      { if(mRenderCountPtr) *mRenderCountPtr = c; }

        /** 获取多重绘制数量参数
        @version NIIEngine 3.0.0
        */
        inline NIIf * getRenderCount() const                    { return mRenderCountPtr; }

        /** 从缓存索引获取参数索引
        @version NIIEngine 3.0.0
        */
        Nui32 getLocation(Nidx memidx) const;

        /** 获取命名参数定义
        @version NIIEngine 3.0.0
        */
        inline const GpuParamUnitList & getDefineList() const   { N_assert(mParamDefine, "error"); return mParamDefine->mDefineList; }

        /** 获取块参数定义
        @version NIIEngine 3.0.0
        */
        inline const GpuParamBlockMap & getBlockList() const   { return mBlockList; }

        /** 获取NIIf常量列表的引用
        @version NIIEngine 3.0.0 高级api
        */
        inline const GBufferArray & getBufferData() const       { return mBufferData; }

        /** 获取浮点缓存
        @version NIIEngine 3.0.0 高级api
        */
        inline void * getBufferData(Nidx oft32b) const          { return &mBufferData[oft32b]; }
        
        /** 读出数据到着色程序结构体缓存中
        @note 注意结构体的成员对齐方式
        @version NIIEngine 3.0.0
        */
        inline void read(StructBuffer * dst, NCount dstoft, NCount oft32b, NCount size32b)
        {
            dst->write(&mBufferData[oft32b], dstoft, size32b * 4);
        }

        /** 从着色程序结构体缓存中写入数据
        @note 注意结构体的成员对齐方式
        @version NIIEngine 3.0.0
        */
        void write(StructBuffer * src, NCount srcoft, NCount oft32b, NCount size32b)
        {
            if (mBufferData.size() < oft32b + size32b)
                mBufferData.resize(oft32b + size32b);
            src->read(&mBufferData[oft32b], srcoft, size32b * 4);
        }

        /** 获取Gpu数据类型大小
        @version NIIEngine 3.0.0
        */
        static NCount getTypeCount(GpuDataType type, bool pad128b);
        
        /** 获取Gpu数据类型大小
        @version NIIEngine 3.0.0
        */
        static NCount get32bCount(GpuDataType type, bool pad128b);
    protected:
        /** 获取参数绑定
        @param[in] size32b 需要多少个32bits(32bits)
        @param[in] reduce 如果已经存在的块比实际需要的大,是否缩减
        @version NIIEngine 3.0.0
        */
        GpuParamBlock * getBlock(Nui32 index, NCount size32b, Nui32 gbtMark, GpuDataType dtype, bool reduce = false);
        
        /** 转换为自动参数
        @version NIIEngine 6.0.0
        */
        void set(GpuParamBlock * block, GpuSyncParam type, Ni32 extraInfo);
        
        /** 转换为自动参数
        @version NIIEngine 6.0.0
        */
        void set(GpuParamBlock * block, GpuSyncParam type, NIIf extraInfo);

        /** 获取参数种类
        @version NIIEngine 3.0.0
        */
        Nui32 getGBTMark(GpuSyncParam sparam);
    protected:
        typedef map<Nui32, Ni32>::type ExtDataList;
    protected:
        GpuParamDefine * mParamDefine;
        GpuParamBuffer * mParamBuffer;
        GpuParamBlockMap mBlockList;
        GpuParamBlockList mSyncBlockList;
        ShareSyncList mShareSyncList;
        ExtDataList mExtDataList;
        mutable GBufferArray mBufferData;      // shared layout 预分配缓存为128byte(32*(float 4byte)) -> alter uav buffer uniform buffer
        NIIf * mRenderCountPtr;
        Nmark mParamGBTMark;
        BitSet mBitSet;
        bool mTransposeMatrix;
        bool mAllowParamLost;
    };

    inline void GpuProgramParam::set(Nui32 index, NCount oft32b, NIIf in, NIIf in2, NIIf in3, NIIf in4)
    {
        NIIf temp[4] = { in, in2, in3, in4 }; set(index, oft32b, temp, 1);
    }

    inline void GpuProgramParam::set(Nui32 index, NCount oft32b, NIId in, NIId in2, NIId in3, NIId in4)
    {
        NIId temp[4] = { in, in2, in3, in4 }; set(index, oft32b, temp, 1);
    }

    inline void GpuProgramParam::set(Nui32 index, NCount oft32b, Ni32 in, Ni32 in2, Ni32 in3, Ni32 in4)
    {
        Ni32 temp[4] = { in, in2, in3, in4 }; set(index, oft32b, temp, 1);
    }

    inline void GpuProgramParam::set(Nui32 index, NCount oft32b, const Vector3f & in)
    {
        NIIf temp[4] = { in.x, in.y, in.z, 1.0f }; set(index, oft32b, temp, 1);
    }
    
    /** 着色程序自定义参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CustomGpuParam
    {
    public:
        CustomGpuParam();

        /** 为这个可渲染物设置一个自定义参数,用于推倒计算这个指定的可渲染物,
            像GPU着色程序参数那样
        @param[in] index
        @param[in] value
        @version NIIEngine 3.0.0
        */
        inline void setCustonParam(Nidx index, const Vector4f & value)  { mCGPUParamList.emplace(index, value); }

        /** 给定索引获取关联这个可渲染物的自定义值
        @param[in] index 参数使用的索引
        @version NIIEngine 3.0.0
        */
        const Vector4f & getCustomParam(Nidx index) const;

        /** 移去自定义参数
        @param[in] index 参数使用的索引
        @version NIIEngine 3.0.0
        */
        void removeCustomParam(Nidx index);

        /** 是否存在自定义参数
        @param[in] index 参数使用的索引
        @version NIIEngine 3.0.0
        */
        inline bool isCustomParamExist(Nidx index) const                { return mCGPUParamList.find(index) != mCGPUParamList.end(); }

        /** 更新自定义参数到实际的着色程序参数中
        @param[in] dst 需要更新的着色程序参数集
        @param[in] src 源着色程序数据块
        @version NIIEngine 3.0.0
        */
        virtual void updateCustom(GpuProgramParam * dst, const GpuParamBlock & src) const;
        
        /** 更新自定义参数到实际的着色程序参数中
        @param[in] dst 需要更新的着色程序参数集
        @param[in] index 参数使用的索引, mCGPUParamList[index]
        @param[in] src 源着色程序数据块
        @version NIIEngine 3.0.0
        */
        virtual void updateCustom(GpuProgramParam * dst, Nidx index, const GpuParamBlock & src) const;
    protected:
        typedef map<Nidx, Vector4f>::type GParamValueList;
    protected:
        GParamValueList mCGPUParamList;
    };
}
#endif