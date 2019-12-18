/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-1

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

#ifndef _NII_GPU_PREDEFINE_VALUE_H_
#define _NII_GPU_PREDEFINE_VALUE_H_

#include "NiiPreInclude.h"
#include "NiiGpuParamDef.h"

namespace NII
{
    /** 预定义着色程序值
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuPreDefineValue : public GpuParamsAlloc
    {
    public:
        GpuPreDefineValue(const String & name);
        virtual ~GpuPreDefineValue();

        /** 集合名字
        @version NIIEngine 3.0.0
        */
        const String & getName();

        /** 添加变量
        @version NIIEngine 3.0.0
        */
        void add(const VString & name, GpuDataType type, NCount count = 1);

        /** 获取变量
        @version NIIEngine 3.0.0
        */
        const GpuParamDef & get(const VString & name) const;

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
        void set(const VString & name, NIIi in);

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NIIf in);

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const Colour & in);

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const Vector4f & in);

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const Vector3f & in);

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const NIIi * in, NCount count);

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const NIIf * in, NCount count);

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const NIId * in, NCount count);

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const Matrix4f & in);

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const Matrix4f * in, NCount count);

        /** 获取定义列表
        @version NIIEngine 3.0.0 高级api
        */
        const GpuParamDefines & getDefList() const;

        /** 获取状态掩码
        @version NIIEngine 3.0.0
        */
        Nul getStateMark() const;

        /** 获取浮点部分数据
        @version NIIEngine 3.0.0
        */
        NIIf * getFloatData(NCount pos);

        /** 获取浮点部分数据
        @version NIIEngine 3.0.0
        */
        const NIIf * getFloatData(NCount pos) const;

        /** 获取整形部分数据
        @version NIIEngine 3.0.0
        */
        NIId * getDoubleData(NCount pos);

        /** 获取整形部分数据
        @version NIIEngine 3.0.0
        */
        const NIId * getDoubleData(NCount pos) const;

        /** 获取整形部分数据
        @version NIIEngine 3.0.0
        */
        NIIi * getIntData(NCount pos);

        /** 获取整形部分数据
        @version NIIEngine 3.0.0
        */
        const NIIi * getIntData(NCount pos) const;
    protected:
        String mName;
        GpuParamDefines mDefines;
        IntArray mIntData;
        FloatArray mFloatData; 
        DoubleArray mDoubleData;
        NCount mIntSize;
        NCount mFloatSize;
        NCount mDoubleSize;
        Nmark mDirtyMark;
    };
}

#endif