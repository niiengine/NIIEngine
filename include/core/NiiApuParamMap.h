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

#ifndef _NII_APU_PARAMMap_H_
#define _NII_APU_PARAMMap_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 加速参数内存绑定
    @version NIIEngine 4.0.0
    */
    class _EngineAPI ApuParamMap : public GpuParamsAlloc
    {
    public:
        /**
        @version NIIEngine 4.0.0
        */
        enum Type
        {
            T_Read,
            T_Write,
            T_Copy,
            T_Ref,
            T_Host
        };

        /**
        @version NIIEngine 4.0.0
        */
        enum OPType
        {
            OT_Read,
            OT_Write,
            OT_Whole
        };

        typedef struct Unit
        {
            void * mSrc;
            Type mType;
            OPType mMutex;
            NCount mSize;
        } Unit;
        typedef map<Nidx, Unit>::type UnitList;
    public:
        ApuParamMap();
        ~ApuParamMap();

        /** 绑定缓存
        @param[in] idx 参数栈索引
        @version NIIEngine 4.0.0
        */
        bool bind(Nidx idx, Nmark tmark, void * src, NCount size);

        /** 映射缓存
        @note 可能当bind函数含有T_Ref非T_Copy,并且属于同一操作体系平台,则可能与源指针相同
        @version NIIEngine 4.0.0
        */
        void * lock(Nidx idx, NCount oft, NCount size);

        /** 解除映射
        @version NIIEngine 4.0.0
        */
        void unlock(Nidx idx);

        /** 读取
        @version NIIEngine 4.0.0
        */
        void read(Nidx idx, NCount oft, NCount size, void * out) const;

        /** 写入
        @version NIIEngine 4.0.0
        */
        void write(Nidx idx, NCount oft, NCount size, const void * in);

        /** 获取列表
        @version NIIEngine 4.0.0
        */
        inline const UnitList & getUnitList() const { return mUnitList; }
    protected:
        UnitList mUnitList;
    };
}
#endif