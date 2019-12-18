/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-27

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_GPU_PROGRAMPARAMS_H_
#define _NII_GPU_PROGRAMPARAMS_H_

#include "NiiPreInclude.h"
#include "NiiGpuParamBind.h"
#include "NiiVector4.h"
#include "NiiMatrix4.h"
#include "NiiColour.h"

namespace NII
{
    class ShareParamSync;
    /** 典型的着色程序参数
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
        void setParamDef(GpuParamDefSet * def);

        /** 获取参数定义
        @note 仅适用于命名参数对象 高级api
        */
        const GpuParamDefSet * getParamDef() const;

        /** 设置参数绑定
        @version NIIEngine 3.0.0 高级api
        */
        void setBindMap(GpuParamBindMap * bind);

        /** 获取参数绑定
        @version NIIEngine 3.0.0 高级api
        */
        const GpuParamBindMap * getBindMap() const;

        /** 获取参数定义
        @version NIIEngine 3.0.0
        */
        const GpuParamDef & getDef(const VString & name) const;

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
        void _write(Nidx memidx, NIIf in);

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        void _write(Nidx memidx, NIIi in);

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        void _write(Nidx memidx, const Vector3f & in);

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @param[in] size 实际写入字节
        @version NIIEngine 3.0.0 高级api
        */
        void _write(Nidx memidx, const Vector4f & in, NCount size = 4);

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @param[in] size 实际写入字节
        @version NIIEngine 3.0.0 高级api
        */
        void _write(Nidx memidx, const Vector4i & in, NCount size = 4);

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
        void _write(Nidx memidx, const Colour & in, NCount size = 4);

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        void _write(Nidx memidx, const NIIf * in, NCount count);

        /** 读取缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        void _read(Nidx memidx, NCount count, NIIf * out);

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        void _write(Nidx memidx, const NIId * in, NCount count);

        /** 写入缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        void _write(Nidx memidx, const NIIi * in, NCount count);

        /** 读取缓存数据
        @param[in] memidx 缓存存储索引
        @version NIIEngine 3.0.0 高级api
        */
        void _read(Nidx memidx, NCount count, NIIi * out);

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
        const GpuSyncParamBind * getSyncParamBind(const VString & name) const;

        /** 获取同步参数绑定
        @param[in] index 参数索引
        @version NIIEngine 3.0.0 高级api
        */
        const GpuSyncParamBind * getFloatSyncParamBind(Nidx index) const;

        /** 获取同步参数绑定
        @param[in] index 参数索引
        @version NIIEngine 3.0.0 高级api
        */
        const GpuSyncParamBind * getDoubleSyncParamBind(Nidx index) const;

        /** 获取同步参数绑定
        @param[in] index 参数索引
        @version NIIEngine 3.0.0 高级api
        */
        const GpuSyncParamBind * getIntSyncParamBind(Nidx index) const;

        /** 获取同步参数绑定
        @param[in] index 缓存索引
        @version NIIEngine 3.0.0
        */
        const GpuSyncParamBind * _getFloatSyncParamBind(Nidx memidx) const;

        /** 获取同步参数绑定
        @param[in] index 缓存索引
        @version NIIEngine 3.0.0
        */
        const GpuSyncParamBind * _getDoubleSyncParamBind(Nidx memidx) const;

        /** 获取同步参数绑定
        @param[in] index 缓存索引
        @version NIIEngine 3.0.0
        */
        const GpuSyncParamBind * _getIntSyncParamBind(Nidx memidx) const;

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
        inline const SyncParamList & getSyncParamList() const { return mSyncParamList; }

        /** 更新同步参数
        @param[in] src 参数资源
        @param[in] mark GpuParamType
        @version NIIEngine 3.0.0 高级api
        */
        void syncSys(const SysSyncParam * src, Nmark mark);

        /** 添加公共参数
        @version NIIEngine 3.0.0
        */
        void addShare(GpuPreDefineValue * param);

        /** 添加公共参数
        @param[in] param
        @version NIIEngine 3.0.0
        */
        void addShare(const String & param);

        /** 公共参数是否存在
        @version NIIEngine 3.0.0
        */
        bool isShareExist(const String & param) const;

        /** 移去公共参数
        @version NIIEngine 3.0.0
        */
        void removeShare(const String & param);

        /** 移去所有公共参数
        @version NIIEngine 3.0.0
        */
        void removeAllShare();

        /** 同步公共参数
        @version NIIEngine 3.0.0 高级api
        */
        void syncShare();

        /** 是否允许参数丢失
        @version NIIEngine 3.0.0  高级api
        */
        void setAllocParamLost(bool b);

        /** 设置是否需要转换矩阵
        @note d3d gles 需要使用转置矩阵
        @version NIIEngine 3.0.0 高级api
        */
        void setTransposeMatrix(bool b);

        /** 获取是否需要转换矩阵
        @note d3d gles 需要使用转置矩阵
        @version NIIEngine 3.0.0 高级api
        */
        bool isTransposeMatrix() const;

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
        const NIIf * getRenderCountPtr() const;

        /** 获取多重绘制数量参数索引
        @version NIIEngine 3.0.0
        */
        Nidx getRenderCountIndex() const;

        /** 从缓存索引获取参数索引
        @version NIIEngine 3.0.0
        */
        Nui32 getFloatParamIndex(Nidx memidx);

        /** 从缓存索引获取参数索引
        @version NIIEngine 3.0.0
        */
        Nui32 getDoubleParamIndex(Nidx memidx);

        /** 从缓存索引获取参数索引
        @version NIIEngine 3.0.0
        */
        Nui32 getIntParamIndex(Nidx memidx);

        /** 获取命名参数定义
        @version NIIEngine 3.0.0
        */
        const GpuParamDefines & getDefList() const;

        /** 获取NIIf常量列表的引用
        @version NIIEngine 3.0.0 高级api
        */
        const FloatArray & getFloatDataArray() const;

        /** 获取NIId常量列表的引用
        @version NIIEngine 3.0.0 高级api
        */
        const DoubleArray & getDoubleDataArray() const;

        /** 获取NIIi常量列表的引用
        @version NIIEngine 3.0.0 高级api
        */
        const IntArray & getIntDataArray() const;

        /** 获取浮点缓存
        @version NIIEngine 3.0.0
        */
        NIIf * getFloatData(Nidx pos);

        /** 获取浮点缓存
        @version NIIEngine 3.0.0
        */
        const NIIf * getFloatData(Nidx pos) const;

        /** 获取浮点缓存
        @version NIIEngine 3.0.0
        */
        NIId * getDoubleData(Nidx pos);

        /** 获取浮点缓存
        @version NIIEngine 3.0.0
        */
        const NIId * getDoubleData(Nidx pos) const;

        /** 获取整数缓存
        @version NIIEngine 3.0.0
        */
        NIIi * getIntData(Nidx pos);

        /** 获取整数缓存
        @version NIIEngine 3.0.0
        */
        const NIIi * getIntData(Nidx pos) const;

        /** 获取Gpu数据类型大小
        @version NIIEngine 3.0.0
        */
        static NCount getTypeCount(GpuDataType type, bool pad4x);
    protected:
        /** 获取浮点参数绑定
        @version NIIEngine 3.0.0
        */
        GpuParamMemBind * getFloatParamBind(Nui32 index, NCount size, Nmark typemark);

        /** 获取浮点参数绑定
        @version NIIEngine 3.0.0
        */
        GpuParamMemBind * getDoubleParamBind(Nui32 index, NCount size, Nmark typemark);

        /** 获取整形参数绑定
        @version NIIEngine 3.0.0
        */
        GpuParamMemBind * getIntParamBind(Nui32 index, NCount size, Nmark typemark);

        /** 获取参数种类
        @version NIIEngine 3.0.0
        */
        Nmark getParamTypeMark(GpuSyncParam sparam);
    protected:
        typedef vector<ShareParamSync *>::type ShareSyncList;
        typedef map<Nui32, NIIi>::type ExtDataList;
    protected:
        GpuParamDefSet * mParamDefs;
        GpuParamBindMap * mParamBindMap;
        SyncParamList mSyncParamList;
        ShareSyncList mShareSyncList;
        ExtDataList mExtDataList;
        IntArray mIntData;
        FloatArray mFloatData;
        DoubleArray mDoubleData;
        NIIf * mRenderCountPtr;
        Nidx mRenderCountIndex;
        Nmark mParamTypeMark;
        bool mTransposeMatrix;
        bool mAllowParamLost;
    };
}
#endif
