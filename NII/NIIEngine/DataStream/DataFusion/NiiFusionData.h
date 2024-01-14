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
    /** 隐式基础运算
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FusionNum : public DataAlloc
    {
    public:
        FusionNum();
        virtual ~FusionNum();

        /** 两对象相加
        @param[out] out 输出,对象本身已经实例化
        @version NIIEngine 3.0.0
        */
        virtual void Add(const FusionNum * in, FusionNum * out) = 0;

        /** 两对象相减
        @param[out] out 输出,对象本身已经实例化
        @version NIIEngine 3.0.0
        */
        virtual void Sub(const FusionNum * in, FusionNum * out) = 0;

        /** 两对象相乘
        @param[out] out 输出,对象本身已经实例化
        @version NIIEngine 3.0.0
        */
        virtual void Mul(const FusionNum * in, FusionNum * out) = 0;

        /** 两对象相除
        @param[out] out 输出,对象本身已经实例化
        @version NIIEngine 3.0.0
        */
        virtual void Div(const FusionNum * in, FusionNum * out) = 0;

        /** 加上浮点
        @param[out] out 输出,对象本身已经实例化
        @version NIIEngine 3.0.0
        */
        virtual void Add(NIIf in, FusionNum * out) = 0;

        /** 减去浮点
        @param[out] out 输出,对象本身已经实例化
        @version NIIEngine 3.0.0
        */
        virtual void Sub(NIIf in, FusionNum * out) = 0;

        /** 乘以浮点
        @param[out] out 输出,对象本身已经实例化
        @version NIIEngine 3.0.0
        */
        virtual void Mul(NIIf in, FusionNum * out) = 0;

        /** 除于浮点
        @param[out] out 输出,对象本身已经实例化
        @version NIIEngine 3.0.0
        */
        virtual void Div(NIIf in, FusionNum * out) = 0;

        /** 混合
        @version NIIEngine 3.0.0
        */
        virtual void Fusion(FusionNum * k1, FusionNum * k2, NIIf t) = 0;

        /** 混合
        @version NIIEngine 3.0.0
        */
        virtual void RelFusion(FusionNum * bas, FusionNum * k1, FusionNum * k2, NIIf t) = 0;

        /** 混合
        @version NIIEngine 3.0.0
        */
        virtual void RelMulFusion(FusionNum * bas, FusionNum * k1, FusionNum * k2, NIIf t) = 0;

        /** 导出数据到字符串
        @version NIIEngine 3.0.0
        */
        virtual void to(String & out) const = 0;

        /** 从字符串导入数据
        @version NIIEngine 3.0.0
        */
        virtual void from(const String & in) = 0;

        /** 数据指针
        @version NIIEngine 3.0.0
        */
        virtual void setData(const void * data) = 0;

        /** 数据指针
        @version NIIEngine 3.0.0
        */
        virtual const void * getDataPtr() const = 0;

        /** 同类型副本
        @note 不包含数据赋值
        @version NIIEngine 3.0.0
        */
        virtual FusionNum * cloneType() const = 0;

        /** 副本
        @note 包含数据赋值
        @version NIIEngine 3.0.0
        */
        virtual FusionNum * clone() const = 0;
    };

    /** 隐式基础类型混算
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

        /** 设置值
        @version NIIEngine 3.0.0
        */
        void setData(const T & data)
        {
            mObj = data;
        }

        /** 获取值
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

    /** 能进行计算的类型
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FusionData : public DataAlloc
    {
    public:
        FusionData();
        FusionData(const FusionData & o);
        FusionData(FusionNum * obj, bool copy = true, bool autodestroy = true);
        virtual ~FusionData();

        /** 设置数据
        @version NIIEngine 3.0.0
        */
        void setBaseValue(FusionNum * data, bool copy = true, bool autodestroy = true);

        /** 获取数据
        @note 属性级
        @version NIIEngine 3.0.0
        */
        FusionNum * getBaseValue() const    { return mObj; }

        /** 把基础值作为当前值
        @note 属性级
        @version NIIEngine 3.0.0
        */
        virtual void setBaseAsCurrent();

        /** 把当前值作为基础值
        @note 属性级
        @version NIIEngine 3.0.0
        */
        virtual void setCurrentAsBase();

        /** 设置基础值
        @note 属性级
        @version NIIEngine 3.0.0
        */
        virtual void setValue(const FusionNum & obj);

        /** 附加当前值
        @note 属性级
        @version NIIEngine 3.0.0
        */
        virtual void addValue(const FusionNum & obj);

        /** 当前值加上
        @note 属性级
        @version NIIEngine 3.0.0
        */
        virtual void subValue(const FusionNum & obj);

        /** 当前值乘以
        @note 属性级
        @version NIIEngine 3.0.0
        */
        virtual void mulValue(const FusionNum & obj);

        /** 当前值除于
        @note 属性级
        @version NIIEngine 3.0.0
        */
        virtual void divValue(const FusionNum & obj);

        /** 插值
        @version NIIEngine 3.0.0
        */
        void FusionValue(const FusionData & k1, const FusionData & k2, NIIf t);

        /** 插值
        @version NIIEngine 3.0.0
        */
        void RelFusionValue(const FusionData & bas, const FusionData & k1, const FusionData & k2, NIIf t);

        /** 插值
        @version NIIEngine 3.0.0
        */
        void RelMulFusionValue(const FusionData & bas, const FusionData & k1, const FusionData & k2, NIIf t);

        /** 设置是否自动删除混合数据
        @version NIIEngine 3.0.0
        */
        void setAutoDestroy(bool b)             { mAutoDestroy = b; }

        /** 获取是否自动删除混合数据
        @version NIIEngine 3.0.0
        */
        bool isAutoDestroy() const              { return mAutoDestroy; }

        /** 同类型加法
        @remark 必须是同类型的,否则会异常
        @version NIIEngine 3.0.0
        */
        FusionData & operator +=(const FusionData & o) { N_assert(mObj, "error"); mObj->Add(o.mObj, mObj); return *this; }

        /** 同类型减法
        @remark 必须是同类型的,否则会异常
        @version NIIEngine 3.0.0
        */
        FusionData & operator -=(const FusionData & o) { N_assert(mObj, "error"); mObj->Sub(o.mObj, mObj); return *this; }

        /** 同类型乘法
        @remark 必须是同类型的,否则会异常
        @version NIIEngine 3.0.0
        */
        FusionData & operator *=(const FusionData & o) { N_assert(mObj, "error"); mObj->Mul(o.mObj, mObj); return *this; }

        /** 同类型除法
        @remark 必须是同类型的,否则会异常
        @version NIIEngine 3.0.0
        */
        FusionData & operator /=(const FusionData & o) { N_assert(mObj, "error"); mObj->Div(o.mObj, mObj); return *this; }

        /** 值偏移(加法)
        @version NIIEngine 3.0.0
        */
        FusionData & operator +=(NIIf o) { N_assert(mObj, "error"); mObj->Add(o, mObj); return *this; }

        /** 值偏移(减法)
        @version NIIEngine 3.0.0
        */
        FusionData & operator -=(NIIf o) { N_assert(mObj, "error"); mObj->Sub(o, mObj); return *this; }

        /** 值偏移(乘法)
        @remark 必须是同类型的,否则会异常
        @version NIIEngine 3.0.0
        */
        FusionData & operator *=(NIIf o) { N_assert(mObj, "error"); mObj->Mul(o, mObj); return *this; }

        /** 值偏移(除法)
        @remark 必须是同类型的,否则会异常
        @version NIIEngine 3.0.0
        */
        FusionData & operator /=(NIIf o) { N_assert(mObj, "error"); mObj->Div(o, mObj); return *this; }

        /** 同类型加法
        @remark 必须是同类型的,否则会异常
        @version NIIEngine 3.0.0
        */
        FusionData operator +(const FusionData & o) const;

        /** 同类型减法
        @remark 必须是同类型的,否则会异常
        @version NIIEngine 3.0.0
        */
        FusionData operator -(const FusionData & o) const;

        /** 同类型乘法
        @remark 必须是同类型的,否则会异常
        @version NIIEngine 3.0.0
        */
        FusionData operator *(const FusionData & o) const;

        /** 同类型除法
        @remark 必须是同类型的,否则会异常
        @version NIIEngine 3.0.0
        */
        FusionData operator /(const FusionData & o) const;

        /** 值偏移(加法)
        @version NIIEngine 3.0.0
        */
        FusionData operator +(NIIf o) const;

        /** 值偏移(减法)
        @version NIIEngine 3.0.0
        */
        FusionData operator -(NIIf o) const;

        /** 值偏移(乘法)
        @version NIIEngine 3.0.0
        */
        FusionData operator *(NIIf o) const;

        /** 值偏移(除法)
        @version NIIEngine 3.0.0
        */
        FusionData operator /(NIIf o) const;

        /** 同类型赋值
        @remark 必须是同类型的,否则会异常.产生独立数据
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