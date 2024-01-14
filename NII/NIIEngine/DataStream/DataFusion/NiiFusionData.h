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
    /** ��ʽ��������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FusionNum : public DataAlloc
    {
    public:
        FusionNum();
        virtual ~FusionNum();

        /** ���������
        @param[out] out ���,�������Ѿ�ʵ����
        @version NIIEngine 3.0.0
        */
        virtual void Add(const FusionNum * in, FusionNum * out) = 0;

        /** ���������
        @param[out] out ���,�������Ѿ�ʵ����
        @version NIIEngine 3.0.0
        */
        virtual void Sub(const FusionNum * in, FusionNum * out) = 0;

        /** ���������
        @param[out] out ���,�������Ѿ�ʵ����
        @version NIIEngine 3.0.0
        */
        virtual void Mul(const FusionNum * in, FusionNum * out) = 0;

        /** ���������
        @param[out] out ���,�������Ѿ�ʵ����
        @version NIIEngine 3.0.0
        */
        virtual void Div(const FusionNum * in, FusionNum * out) = 0;

        /** ���ϸ���
        @param[out] out ���,�������Ѿ�ʵ����
        @version NIIEngine 3.0.0
        */
        virtual void Add(NIIf in, FusionNum * out) = 0;

        /** ��ȥ����
        @param[out] out ���,�������Ѿ�ʵ����
        @version NIIEngine 3.0.0
        */
        virtual void Sub(NIIf in, FusionNum * out) = 0;

        /** ���Ը���
        @param[out] out ���,�������Ѿ�ʵ����
        @version NIIEngine 3.0.0
        */
        virtual void Mul(NIIf in, FusionNum * out) = 0;

        /** ���ڸ���
        @param[out] out ���,�������Ѿ�ʵ����
        @version NIIEngine 3.0.0
        */
        virtual void Div(NIIf in, FusionNum * out) = 0;

        /** ���
        @version NIIEngine 3.0.0
        */
        virtual void Fusion(FusionNum * k1, FusionNum * k2, NIIf t) = 0;

        /** ���
        @version NIIEngine 3.0.0
        */
        virtual void RelFusion(FusionNum * bas, FusionNum * k1, FusionNum * k2, NIIf t) = 0;

        /** ���
        @version NIIEngine 3.0.0
        */
        virtual void RelMulFusion(FusionNum * bas, FusionNum * k1, FusionNum * k2, NIIf t) = 0;

        /** �������ݵ��ַ���
        @version NIIEngine 3.0.0
        */
        virtual void to(String & out) const = 0;

        /** ���ַ�����������
        @version NIIEngine 3.0.0
        */
        virtual void from(const String & in) = 0;

        /** ����ָ��
        @version NIIEngine 3.0.0
        */
        virtual void setData(const void * data) = 0;

        /** ����ָ��
        @version NIIEngine 3.0.0
        */
        virtual const void * getDataPtr() const = 0;

        /** ͬ���͸���
        @note ���������ݸ�ֵ
        @version NIIEngine 3.0.0
        */
        virtual FusionNum * cloneType() const = 0;

        /** ����
        @note �������ݸ�ֵ
        @version NIIEngine 3.0.0
        */
        virtual FusionNum * clone() const = 0;
    };

    /** ��ʽ�������ͻ���
    @version NIIEngine 3.0.0
    */
    template <typename C, typename T> class TypeFusionNum : public FusionNum
    {
    public:
        TypeFusionNum() {};
        TypeFusionNum(const T & value) : mObj(value) {}
        TypeFusionNum(const FusionNum & obj)
        {
            mObj = static_cast<const TypeFusionNum<C, T> &>(obj).mObj;
        }

        /// @copydetails FusionNum::Add
        void Add(const FusionNum * in, FusionNum * out)
        {
            N_assert(in, "error");
            N_assert(out, "error");
            static_cast<TypeFusionNum<C, T> *>(out)->mObj =
                mObj + static_cast<const TypeFusionNum<C, T> *>(in)->mObj;
        }

        /// @copydetails FusionNum::Sub
        void Sub(const FusionNum * in, FusionNum * out)
        {
            N_assert(in, "error");
            N_assert(out, "error");
            static_cast<TypeFusionNum<C, T> *>(out)->mObj =
                mObj - static_cast<const TypeFusionNum<C, T> *>(in)->mObj;
        }

        /// @copydetails FusionNum::Mul
        void Mul(const FusionNum * in, FusionNum * out)
        {
            N_assert(in, "error logic");
            N_assert(out, "error logic");
            static_cast<TypeFusionNum<C, T> *>(out)->mObj =
                mObj * static_cast<const TypeFusionNum<C, T> *>(in)->mObj;
        }

        /// @copydetails FusionNum::Div
        void Div(const FusionNum * in, FusionNum * out)
        {
            N_assert(in, "error");
            N_assert(out, "error");
            static_cast<TypeFusionNum<C, T> *>(out)->mObj =
                mObj / static_cast<const TypeFusionNum<C, T> *>(in)->mObj;
        }

        /// @copydetails FusionNum::Add
        void Add(NIIf in, FusionNum * out)
        {
            N_assert(out, "error");
            static_cast<TypeFusionNum<C, T> *>(out)->mObj = mObj + in;
        }

        /// @copydetails FusionNum::Sub
        void Sub(NIIf in, FusionNum * out)
        {
            N_assert(out, "error");
            static_cast<TypeFusionNum<C, T> *>(out)->mObj = mObj - in;
        }

        /// @copydetails FusionNum::Mul
        void Mul(NIIf in, FusionNum * out)
        {
            N_assert(out, "error");
            static_cast<TypeFusionNum<C, T> *>(out)->mObj = mObj * in;
        }

        /// @copydetails FusionNum::Div
        void Div(NIIf in, FusionNum * out)
        {
            N_assert(out, "error");
            static_cast<TypeFusionNum<C, T> *>(out)->mObj = mObj / in;
        }

        /// @copydetails FusionNum::Fusion
        virtual void Fusion(FusionNum * k1, FusionNum * k2, NIIf t)
        {
            mObj = static_cast<TypeFusionNum<C, T> *>(k1)->mObj +
                (static_cast<TypeFusionNum<C, T> *>(k2)->mObj -
                    static_cast<TypeFusionNum<C, T> *>(k1)->mObj) * t;
        }

        /// @copydetails FusionNum::RelFusion
        virtual void RelFusion(FusionNum * bas, FusionNum * k1, FusionNum * k2, NIIf t)
        {
            mObj = static_cast<TypeFusionNum<C, T> *>(bas)->mObj +
                static_cast<TypeFusionNum<C, T> *>(k1)->mObj +
                (static_cast<TypeFusionNum<C, T> *>(k2)->mObj -
                    static_cast<TypeFusionNum<C, T> *>(k1)->mObj) * t;
        }

        /// @copydetails FusionNum::RelMulFusion
        virtual void RelMulFusion(FusionNum * bas, FusionNum * k1, FusionNum * k2, NIIf t)
        {
            mObj = static_cast<TypeFusionNum<C, T> *>(bas)->mObj *
                (static_cast<TypeFusionNum<C, T> *>(k1)->mObj +
                (static_cast<TypeFusionNum<C, T> *>(k2)->mObj -
                    static_cast<TypeFusionNum<C, T> *>(k1)->mObj) * t);
        }

        /// @copydetails FusionNum::to
        virtual void to(String & out) const
        {
            C::conv(mObj, out);
        }

        /// @copydetails FusionNum::from
        virtual void from(const String & in)
        {
            C::conv(in, mObj);
        }

        /** ����ֵ
        @version NIIEngine 3.0.0
        */
        void setData(const T & data)
        {
            mObj = data;
        }

        /** ��ȡֵ
        @version NIIEngine 3.0.0
        */
        const T & getData() const
        {
            return mObj;
        }

        /// @copydetails FusionNum::setData
        void setData(const void * data)
        {
            mObj = *static_cast<const T *>(data);
        }

        /// @copydetails FusionNum::getDataPtr
        const void * getDataPtr() const
        {
            return &mObj;
        }

        /// @copydetails FusionNum::cloneType
        FusionNum * cloneType() const
        {
            return N_new TypeFusionNum<C, T>();
        }

        /// @copydetails FusionNum::clone
        FusionNum * clone() const
        {
            return N_new TypeFusionNum<C, T>(mObj);
        }
    protected:
        T mObj;
    };

    typedef TypeFusionNum<StrConv, NIIf>     NIIfFusion;
    typedef TypeFusionNum<StrConv, Nui32>    Nui32Fusion;
    typedef TypeFusionNum<StrConv, Ni32>     Ni32Fusion;
    typedef TypeFusionNum<StrConv, Nui16>    Nui16Fusion;
    typedef TypeFusionNum<StrConv, Ni16>     Ni16Fusion;
    typedef TypeFusionNum<StrConv, Vector2f> Vector2fFusion;
    typedef TypeFusionNum<StrConv, Vector3f> Vector3fFusion;
    typedef TypeFusionNum<StrConv, Vector4f> Vector4fFusion;
    typedef TypeFusionNum<StrConv, Rectf>    RectfFusion;
    typedef TypeFusionNum<StrConv, PlaneSizef> PlaneSizefFusion;
    typedef TypeFusionNum<StrConv, Colour>     ColourFusion;

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
        FusionNum * getBaseValue() const    { return mObj; }

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
        void setAutoDestroy(bool b)             { mAutoDestroy = b; }

        /** ��ȡ�Ƿ��Զ�ɾ���������
        @version NIIEngine 3.0.0
        */
        bool isAutoDestroy() const              { return mAutoDestroy; }

        /** ͬ���ͼӷ�
        @remark ������ͬ���͵�,������쳣
        @version NIIEngine 3.0.0
        */
        FusionData & operator +=(const FusionData & o) { N_assert(mObj, "error"); mObj->Add(o.mObj, mObj); return *this; }

        /** ͬ���ͼ���
        @remark ������ͬ���͵�,������쳣
        @version NIIEngine 3.0.0
        */
        FusionData & operator -=(const FusionData & o) { N_assert(mObj, "error"); mObj->Sub(o.mObj, mObj); return *this; }

        /** ͬ���ͳ˷�
        @remark ������ͬ���͵�,������쳣
        @version NIIEngine 3.0.0
        */
        FusionData & operator *=(const FusionData & o) { N_assert(mObj, "error"); mObj->Mul(o.mObj, mObj); return *this; }

        /** ͬ���ͳ���
        @remark ������ͬ���͵�,������쳣
        @version NIIEngine 3.0.0
        */
        FusionData & operator /=(const FusionData & o) { N_assert(mObj, "error"); mObj->Div(o.mObj, mObj); return *this; }

        /** ֵƫ��(�ӷ�)
        @version NIIEngine 3.0.0
        */
        FusionData & operator +=(NIIf o) { N_assert(mObj, "error"); mObj->Add(o, mObj); return *this; }

        /** ֵƫ��(����)
        @version NIIEngine 3.0.0
        */
        FusionData & operator -=(NIIf o) { N_assert(mObj, "error"); mObj->Sub(o, mObj); return *this; }

        /** ֵƫ��(�˷�)
        @remark ������ͬ���͵�,������쳣
        @version NIIEngine 3.0.0
        */
        FusionData & operator *=(NIIf o) { N_assert(mObj, "error"); mObj->Mul(o, mObj); return *this; }

        /** ֵƫ��(����)
        @remark ������ͬ���͵�,������쳣
        @version NIIEngine 3.0.0
        */
        FusionData & operator /=(NIIf o) { N_assert(mObj, "error"); mObj->Div(o, mObj); return *this; }

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