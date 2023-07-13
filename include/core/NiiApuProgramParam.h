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

#ifndef _NII_APU_PROGRAMPARAM_H_
#define _NII_APU_PROGRAMPARAM_H_

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
                mSrc(src), mDst(0), mTypeMark(typemark | M_OnlyREAD),
                mMutexMark(0), mUnitSize(unitSize), mUnitCount(unitCnt) 
            {
                memcpy(mRawData, data, unitSize * unitCnt);
            }

            inline const void * getConstData() const    { return mRawData; }

            inline bool isConst() const                 { return N_MarkOnlyTrue(mTypeMark, M_USAGEMARK, M_OnlyREAD); }

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
    
    /** 着色程序参数
    @remark 在ApuParamMap的基础上再涵盖共享/公共参数
    @version NIIEngine 4.0.0
    */
    class _EngineAPI ApuProgramParam : public GpuParamsAlloc
    {
    public:
        ApuProgramParam();
        ApuProgramParam(const ApuProgramParam & o);
        ~ApuProgramParam();

        ApuProgramParam & operator=(const ApuProgramParam & o);

        /** 设置参数总数
        @version NIIEngine 4.0.0 adv
        */
        inline void setCount(NCount cnt)            { mParamCount = cnt; }

        /** 获取参数总数
        @version NIIEngine 4.0.0 adv
        */
        inline NCount getCount() const              { return mParamCount; }

        /** 当前参数是否有效
        @version NIIEngine 4.0.0
        */
        bool isValid() const;

        /** 设置参数绑定(基础)
        @version NIIEngine 4.0.0 高级api
        */
        inline void setMap(ApuParamMap * map)       { mParamMap = map; }

        /** 获取参数绑定(基础)
        @version NIIEngine 4.0.0 高级api
        */
        inline const ApuParamMap * getMap() const   { return mParamMap; }

        /** 添加参数（公共）
        @version NIIEngine 4.0.0
        */
        bool addDefault(Nidx idx, Nmark tmark, void * src, NCount unitSize, NCount unitCnt);

        /** 参数是否存在
        @version NIIEngine 4.0.0
        */
        inline bool isDefaultExist(Nidx idx) const  { return mShareSyncList.find(idx) != mShareSyncList.end(); }

        /** 获取参数数量
        @version NIIEngine 4.0.0
        */
        inline NCount getDefaultCount() const       { return mShareSyncList.size(); }

        /** 映射缓存（公共）
        @note 可能当bind函数含有T_Ref非T_Copy,并且属于同一操作体系平台,则可能与源指针相同
        @version NIIEngine 4.0.0
        */
        void * lockDefault(Nidx idx, Nmark opmark, NCount oft, NCount size);

        /** 解除映射（公共）
        @version NIIEngine 4.0.0
        */
        void unlockDefault(Nidx idx);

        /** 读取（公共）
        @version NIIEngine 4.0.0
        */
        void readDefault(Nidx idx, void * out, NCount oft, NCount size) const;

        /** 写入（公共）
        @version NIIEngine 4.0.0
        */
        void writeDefault(Nidx idx, const void * in, NCount oft, NCount size);

        /** 移去参数（公共）
        @version NIIEngine 4.0.0
        */
        void removeDefault(Nidx idx);

        /** 移去所有参数（公共）
        @version NIIEngine 4.0.0
        */
        void removeAllDefault();

        /** 获取列表
        @version NIIEngine 4.0.0
        */
        inline const ApuParamMap::UnitList & getDefaultUnitList() const { return mShareSyncList; }

        /** 复制所有参数（公共）
        @version NIIEngine 4.0.0
        */
        void copyDefault(const ApuProgramParam & src);
    protected:
        NCount mParamCount;
        ApuParamMap * mParamMap;
        ApuParamMap::UnitList mShareSyncList;
    };
}
#endif
