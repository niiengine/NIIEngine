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

#ifndef _NII_GPU_PARAMMAP_H_
#define _NII_GPU_PARAMMAP_H_

#include "NiiPreInclude.h"
#include "NiiGpuParamDefine.h"

namespace NII
{
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
}
#endif