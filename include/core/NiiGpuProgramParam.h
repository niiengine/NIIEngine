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
#include "NiiGpuParamMap.h"
#include "NiiVector4.h"
#include "NiiMatrix4.h"
#include "NiiColour.h"

namespace NII
{
    class ShareParamSync;
    /** 着色程序参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuProgramParam : public GpuParamsAlloc
    {
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
        void set(Nui32 index, NIIi in, NIIi in2 = 0, NIIi in3 = 0, NIIi in4 = 0);

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
        void set(Nui32 index, const NIIi * in, NCount cnt4x);

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
        void setExtData(Nui32 index, NIIi ext);

        /** 获取扩展数据
        @version NIIEngine 5.0.0
        */
        bool getExtData(Nui32 index, NIIi & ext) const;

        /** 设置参数值
        @param[in] name 命名参数
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NIIf in);

        /** 设置参数值
        @param[in] name 命名参数
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NIIi in);

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
        void set(const VString & name, const NIIi * in, NCount count, NCount size = 4);

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, NIIf in){ _write(memidx, &in, 1); }

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        inline void _write(Nidx memidx, NIIi in){ _write(memidx, &in, 1); }

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
        inline void _write(Nidx memidx, const NIIi * in, NCount count){
            N_assert(memidx + count <= mIntData.size(), "error"); memcpy(&mIntData[memidx], in, sizeof(NIIi) * count); }

        /** 读取缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        inline void _read(Nidx memidx, NCount count, NIIi * out){
            N_assert(memidx + count <= mIntData.size(), "error"); memcpy(out, &mIntData[memidx], sizeof(NIIi) * count); }

        /** 设置同步参数
        @param[in] index
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, GpuSyncParam type, NIIi ext = 0);

        /** 设置同步参数
        @version NIIEngine 3.0.0
        */
        void set(Nui32 index, GpuSyncParam type, NIIf ext);

        /** 设置同步参数
        @param[in] name 参数的名字
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, GpuSyncParam type, NIIi ext = 0);

        /** 设置同步参数
        @param[in] name 参数的名字
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, GpuSyncParam type, NIIf ext);

        /** 设置同步参数
        @version NIIEngine 3.0.0 高级api
        */
        void _set(Nidx memidx, GpuSyncParam type, NIIi input, Nmark mark, NCount size = 4);

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

        /** 获取NIIi常量列表的引用
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
        inline NIIi * getIntData(Nidx pos) const{ return &mIntData[pos]; }

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
        typedef map<Nui32, NIIi>::type ExtDataList;
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
}
#endif
