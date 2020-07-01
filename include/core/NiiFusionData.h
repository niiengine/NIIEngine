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

#ifndef _NII_FUSION_DATA_H_
#define _NII_FUSION_DATA_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** �ܽ��м��������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FusionData : public DataAlloc
    {
    public:
        FusionData();
        FusionData(const FusionData & o);
        FusionData(FusionNum * obj, bool copy = true, bool autodestroy = true);
        virtual ~FusionData();

        /** ��������
        @version NIIEngine 3.0.0
        */
        void setBaseValue(FusionNum * data, bool copy = true, bool autodestroy = true);

        /** ��ȡ����
        @note ���Լ�
        @version NIIEngine 3.0.0
        */
        FusionNum * getBaseValue() const;

        /** �ѻ���ֵ��Ϊ��ǰֵ
        @note ���Լ�
        @version NIIEngine 3.0.0
        */
        virtual void setBaseAsCurrent();

        /** �ѵ�ǰֵ��Ϊ����ֵ
        @note ���Լ�
        @version NIIEngine 3.0.0
        */
        virtual void setCurrentAsBase();

        /** ���û���ֵ
        @note ���Լ�
        @version NIIEngine 3.0.0
        */
        virtual void setValue(const FusionNum & obj);

        /** ���ӵ�ǰֵ
        @note ���Լ�
        @version NIIEngine 3.0.0
        */
        virtual void addValue(const FusionNum & obj);

        /** ��ǰֵ����
        @note ���Լ�
        @version NIIEngine 3.0.0
        */
        virtual void subValue(const FusionNum & obj);

        /** ��ǰֵ����
        @note ���Լ�
        @version NIIEngine 3.0.0
        */
        virtual void mulValue(const FusionNum & obj);

        /** ��ǰֵ����
        @note ���Լ�
        @version NIIEngine 3.0.0
        */
        virtual void divValue(const FusionNum & obj);

        /** ��ֵ
        @version NIIEngine 3.0.0
        */
        void FusionValue(const FusionData & k1, const FusionData & k2, NIIf t);

        /** ��ֵ
        @version NIIEngine 3.0.0
        */
        void RelFusionValue(const FusionData & bas, const FusionData & k1, const FusionData & k2, NIIf t);

        /** ��ֵ
        @version NIIEngine 3.0.0
        */
        void RelMulFusionValue(const FusionData & bas, const FusionData & k1, const FusionData & k2, NIIf t);

        /** �����Ƿ��Զ�ɾ���������
        @version NIIEngine 3.0.0
        */
        void setAutoDestroy(bool b);

        /** ��ȡ�Ƿ��Զ�ɾ���������
        @version NIIEngine 3.0.0
        */
        bool isAutoDestroy() const;

        /** ͬ���ͼӷ�
        @remark ������ͬ���͵�,������쳣
        @version NIIEngine 3.0.0
        */
        FusionData & operator +=(const FusionData & o);

        /** ͬ���ͼ���
        @remark ������ͬ���͵�,������쳣
        @version NIIEngine 3.0.0
        */
        FusionData & operator -=(const FusionData & o);

        /** ͬ���ͳ˷�
        @remark ������ͬ���͵�,������쳣
        @version NIIEngine 3.0.0
        */
        FusionData & operator *=(const FusionData & o);

        /** ͬ���ͳ���
        @remark ������ͬ���͵�,������쳣
        @version NIIEngine 3.0.0
        */
        FusionData & operator /=(const FusionData & o);

        /** ֵƫ��(�ӷ�)
        @version NIIEngine 3.0.0
        */
        FusionData & operator +=(NIIf o);

        /** ֵƫ��(����)
        @version NIIEngine 3.0.0
        */
        FusionData & operator -=(NIIf o);

        /** ֵƫ��(�˷�)
        @remark ������ͬ���͵�,������쳣
        @version NIIEngine 3.0.0
        */
        FusionData & operator *=(NIIf o);

        /** ֵƫ��(����)
        @remark ������ͬ���͵�,������쳣
        @version NIIEngine 3.0.0
        */
        FusionData & operator /=(NIIf o);

        /** ͬ���ͼӷ�
        @remark ������ͬ���͵�,������쳣
        @version NIIEngine 3.0.0
        */
        FusionData operator +(const FusionData & o) const;

        /** ͬ���ͼ���
        @remark ������ͬ���͵�,������쳣
        @version NIIEngine 3.0.0
        */
        FusionData operator -(const FusionData & o) const;

        /** ͬ���ͳ˷�
        @remark ������ͬ���͵�,������쳣
        @version NIIEngine 3.0.0
        */
        FusionData operator *(const FusionData & o) const;

        /** ͬ���ͳ���
        @remark ������ͬ���͵�,������쳣
        @version NIIEngine 3.0.0
        */
        FusionData operator /(const FusionData & o) const;

        /** ֵƫ��(�ӷ�)
        @version NIIEngine 3.0.0
        */
        FusionData operator +(NIIf o) const;

        /** ֵƫ��(����)
        @version NIIEngine 3.0.0
        */
        FusionData operator -(NIIf o) const;

        /** ֵƫ��(�˷�)
        @version NIIEngine 3.0.0
        */
        FusionData operator *(NIIf o) const;

        /** ֵƫ��(����)
        @version NIIEngine 3.0.0
        */
        FusionData operator /(NIIf o) const;

        /** ͬ���͸�ֵ
        @remark ������ͬ���͵�,������쳣.������������
        @version NIIEngine 3.0.0
        */
        FusionData & operator =(const FusionData & o);

        friend FusionData operator +(NIIf left, const FusionData & right);
        friend FusionData operator -(NIIf left, const FusionData & right);
        friend FusionData operator *(NIIf left, const FusionData & right);
        friend FusionData operator /(NIIf left, const FusionData & right);
    protected:
        FusionNum * mObj;
        bool mAutoDestroy;
    };
}
#endif