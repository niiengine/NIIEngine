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
#include "NiiApuBuffer.h"

namespace NII
{
    #define N_ApuMaxConstSize 8

    /** 加速参数内存绑定
    @version NIIEngine 4.0.0
    */
    class _EngineAPI ApuParamMap : public GpuParamsAlloc
    {
    public:
        typedef struct Unit
        {
            Unit(void * src, ApuBuffer * buf, Nmark typemark, NCount unitSize, NCount unitCnt) :
                mSrc(src), mDst(buf), mTypeMark(typemark),
                mMutexMark(0), mUnitSize(unitSize), mUnitCount(unitCnt){}

            Unit(void * src, const void * data, Nmark typemark, NCount unitSize, NCount unitCnt) :
                mSrc(src), mDst(0), mTypeMark(typemark | Buffer::M_CONST),
                mMutexMark(0), mUnitSize(unitSize), mUnitCount(unitCnt) 
            {
                memcpy(mRawData, data, unitSize * unitCnt);
            }

            inline const void * getConstData() const    { return mRawData; }

            inline bool isConst() const                 { return mTypeMark & Buffer::M_CONST; }

            void * mSrc;
            ApuBuffer * mDst;
            Nui8 mRawData[N_ApuMaxConstSize];
            Nmark mTypeMark;
            Nmark mMutexMark;
            NCount mUnitSize;
            NCount mUnitCount;
        } Unit;
        typedef map<Nidx, Unit>::type UnitList;
    public:
        ApuParamMap(const ApuContext * ctx);
        ~ApuParamMap();

        inline const ApuContext * getContext() const{ return mCtx; }

        /** 绑定缓存
        @param[in] idx 参数栈索引
        @version NIIEngine 4.0.0
        */
        bool bind(Nidx idx, Nmark typemark, void * src, NCount unitSize, NCount unitCnt);

        /** 映射缓存
        @note 可能当bind函数含有T_Ref非T_Copy,并且属于同一操作体系平台,则可能与源指针相同
        @version NIIEngine 4.0.0
        */
        void * lock(Nidx idx, Nmark opmark, NCount oft, NCount size);

        /** 解除映射
        @version NIIEngine 4.0.0
        */
        void unlock(Nidx idx);

        /** 读取
        @version NIIEngine 4.0.0
        */
        void read(Nidx idx, void * out, NCount oft, NCount size) const;

        /** 写入
        @version NIIEngine 4.0.0
        */
        void write(Nidx idx, const void * in, NCount oft, NCount size);

        /** 参数是否存在
        @version NIIEngine 4.0.0
        */
        inline bool isExist(Nidx idx) const         { return mUnitList.find(idx) != mUnitList.end(); }

        /** 获取参数数量
        @version NIIEngine 4.0.0
        */
        inline NCount getCount() const              { return mUnitList.size(); }

        /** 获取列表
        @version NIIEngine 4.0.0
        */
        inline const UnitList & getUnitList() const { return mUnitList; }
    protected:
        const ApuContext * mCtx;
        UnitList mUnitList;
    };
}
#endif