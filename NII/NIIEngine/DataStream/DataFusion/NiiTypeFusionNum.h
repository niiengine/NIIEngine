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

#ifndef _NII_TYPE_FUSION_NUM_H_
#define _NII_TYPE_FUSION_NUM_H_

#include "NiiPreInclude.h"
#include "NiiFusionNum.h"

namespace NII
{
    /** 隐式基础类型混算
    @version NIIEngine 3.0.0
    */
    template <typename C, typename T> class TypeFusionNum : public FusionNum
    {
    public:
        TypeFusionNum(){};
        TypeFusionNum(const T & value) : mObj(value){}
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
}
#endif