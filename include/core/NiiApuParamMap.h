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
    /** ���ٲ����ڴ��
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

        /** �󶨻���
        @param[in] idx ����ջ����
        @version NIIEngine 4.0.0
        */
        bool bind(Nidx idx, Nmark tmark, void * src, NCount size);

        /** ӳ�仺��
        @note ���ܵ�bind��������T_Ref��T_Copy,��������ͬһ������ϵƽ̨,�������Դָ����ͬ
        @version NIIEngine 4.0.0
        */
        void * lock(Nidx idx, NCount oft, NCount size);

        /** ���ӳ��
        @version NIIEngine 4.0.0
        */
        void unlock(Nidx idx);

        /** ��ȡ
        @version NIIEngine 4.0.0
        */
        void read(Nidx idx, NCount oft, NCount size, void * out) const;

        /** д��
        @version NIIEngine 4.0.0
        */
        void write(Nidx idx, NCount oft, NCount size, const void * in);

        /** ��ȡ�б�
        @version NIIEngine 4.0.0
        */
        inline const UnitList & getUnitList() const { return mUnitList; }
    protected:
        UnitList mUnitList;
    };
}
#endif