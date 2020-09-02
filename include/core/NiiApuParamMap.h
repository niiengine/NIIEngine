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

    /** ���ٲ����ڴ��
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

        /** �󶨻���
        @param[in] idx ����ջ����
        @version NIIEngine 4.0.0
        */
        bool bind(Nidx idx, Nmark typemark, void * src, NCount unitSize, NCount unitCnt);

        /** ӳ�仺��
        @note ���ܵ�bind��������T_Ref��T_Copy,��������ͬһ������ϵƽ̨,�������Դָ����ͬ
        @version NIIEngine 4.0.0
        */
        void * lock(Nidx idx, Nmark opmark, NCount oft, NCount size);

        /** ���ӳ��
        @version NIIEngine 4.0.0
        */
        void unlock(Nidx idx);

        /** ��ȡ
        @version NIIEngine 4.0.0
        */
        void read(Nidx idx, void * out, NCount oft, NCount size) const;

        /** д��
        @version NIIEngine 4.0.0
        */
        void write(Nidx idx, const void * in, NCount oft, NCount size);

        /** �����Ƿ����
        @version NIIEngine 4.0.0
        */
        inline bool isExist(Nidx idx) const         { return mUnitList.find(idx) != mUnitList.end(); }

        /** ��ȡ��������
        @version NIIEngine 4.0.0
        */
        inline NCount getCount() const              { return mUnitList.size(); }

        /** ��ȡ�б�
        @version NIIEngine 4.0.0
        */
        inline const UnitList & getUnitList() const { return mUnitList; }
    protected:
        const ApuContext * mCtx;
        UnitList mUnitList;
    };
}
#endif