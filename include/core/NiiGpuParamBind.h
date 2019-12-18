/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-25

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

#ifndef _NII_GPU_PARAM_BIND_H_
#define _NII_GPU_PARAM_BIND_H_

#include "NiiPreInclude.h"
#include "NiiGpuParamDef.h"

namespace NII
{
    /** 着色参数内存绑定
    @version NIIEngine 3.0.0
    */
    struct _EngineAPI GpuParamMemBind
    {
    public:
    public:
        GpuParamMemBind();
        GpuParamMemBind(Nidx memidx, NCount size, Nmark mark);

        Nidx mMemIndex;
        NCount mSize;
        Nmark mTypeMark;
    };
    typedef map<Nui32, GpuParamMemBind>::type GpuParamBindList;

    /** 同步参数内存绑定
    @version NIIEngine 3.0.0
    */
    struct _EngineAPI GpuSyncParamBind
    {
    public:
        GpuSyncParamBind(GpuSyncParam type, Nidx memidx, NIIi input, Nmark mark, NCount count = 4);

        GpuSyncParamBind(GpuSyncParam type, Nidx memidx, NIIf input, Nmark mark, NCount count = 4);

        Nidx mMemIndex;
        GpuSyncParam mSyncParam;
        Nmark mTypeMark;
        NCount mUnitCount;
        union
        {
            NIIi mInputInt;
            NIIf mInputFloat;
        };
    };
    typedef vector<GpuSyncParamBind>::type SyncParamList;

    /** 着色参数内存绑定
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuParamBindMap : public GpuParamsAlloc
    {
    public:
        GpuParamBindMap();

        GpuParamBindList mIntList;
        GpuParamBindList mFloatList;
        GpuParamBindList mDoubleList;
        NCount mIntSize;
        NCount mFloatSize;
        NCount mDoubleSize;
    };
}
#endif