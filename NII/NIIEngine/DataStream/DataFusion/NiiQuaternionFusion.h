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

#ifndef _NII_Quaternion_FUSION_H_
#define _NII_Quaternion_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiFusionData.h"
#include "NiiStrConv.h"
#include "NiiException.h"

namespace NII
{
    /** 隐式基础类型混算
    @version NIIEngine 3.0.0
    */
    class _EngineAPI QuaternionFusion : public FusionNum
    {
    public:
        QuaternionFusion(){};
        QuaternionFusion(const Quaternion & value) : mObj(value){}
        QuaternionFusion(const FusionNum & obj)
        {
            mObj = static_cast<const QuaternionFusion &>(obj).mObj;
        }

        /// @copydetails FusionNum::Add
        void Add(const FusionNum * in, FusionNum * out)
        {
            N_assert(in, "error");
            N_assert(out, "error");
            static_cast<QuaternionFusion *>(out)->mObj =
                mObj + static_cast<const QuaternionFusion *>(in)->mObj;
        }

        /// @copydetails FusionNum::Sub
        void Sub(const FusionNum * in, FusionNum * out)
        {
            N_assert(in, "error");
            N_assert(out, "error");
            static_cast<QuaternionFusion *>(out)->mObj =
                mObj - static_cast<const QuaternionFusion *>(in)->mObj;
        }

        /// @copydetails FusionNum::Mul
        void Mul(const FusionNum * in, FusionNum * out)
        {
            N_assert(in, "error");
            N_assert(out, "error");
            static_cast<QuaternionFusion *>(out)->mObj =
                mObj * static_cast<const QuaternionFusion *>(in)->mObj;
        }

        /// @copydetails FusionNum::Div
        void Div(const FusionNum * in, FusionNum * out)
        {
            N_assert(in, "error");
            N_assert(out, "error");
            static_cast<QuaternionFusion *>(out)->mObj =
                mObj * (static_cast<const QuaternionFusion *>(in)->mObj).inverse();
        }

        /// @copydetails FusionNum::Add
        void Add(NIIf in, FusionNum * out)
        {
            N_assert(out, "error");
            static_cast<QuaternionFusion *>(out)->mObj = mObj;
        }

        /// @copydetails FusionNum::Sub
        void Sub(NIIf in, FusionNum * out)
        {
            N_assert(out, "error");
            static_cast<QuaternionFusion *>(out)->mObj = mObj;
        }

        /// @copydetails FusionNum::Mul
        void Mul(NIIf in, FusionNum * out)
        {
            N_assert(out, "error");
            static_cast<QuaternionFusion *>(out)->mObj = mObj * in;
        }

        /// @copydetails FusionNum::Div
        void Div(NIIf in, FusionNum * out)
        {
            N_assert(out, "error");
            static_cast<QuaternionFusion *>(out)->mObj = mObj * (1 / in);
        }

        /// @copydetails FusionNum::Fusion
        virtual void Fusion(FusionNum * k1, FusionNum * k2, NIIf t)
        {
            const Quaternion & val1 = static_cast<QuaternionFusion *>(k1)->mObj;
            const Quaternion & val2 = static_cast<QuaternionFusion *>(k2)->mObj;

            mObj = Quaternion::slerp(t, val1, val2);
        }

        /// @copydetails FusionNum::RelFusion
        virtual void RelFusion(FusionNum * base, FusionNum * k1, FusionNum * k2, NIIf t)
        {
            const Quaternion & bas = static_cast<QuaternionFusion *>(base)->mObj;
            const Quaternion & val1 = static_cast<QuaternionFusion *>(k1)->mObj;
            const Quaternion & val2 = static_cast<QuaternionFusion *>(k2)->mObj;

            mObj = bas * Quaternion::slerp(t, val1, val2);
        }

        /// @copydetails FusionNum::RelMulFusion
        virtual void RelMulFusion(FusionNum * base, FusionNum * k1, FusionNum * k2, NIIf t)
        {
            const Quaternion & bas = static_cast<QuaternionFusion *>(base)->mObj;
            const Quaternion & val1 = static_cast<QuaternionFusion *>(k1)->mObj;
            const Quaternion & val2 = static_cast<QuaternionFusion *>(k2)->mObj;

            mObj = bas * Quaternion::slerp(t, val1, val2);
        }

        /// @copydetails FusionNum::to
        virtual void to(String & out) const
        {
            StrConv::conv(mObj, out);
        }

        /// @copydetails FusionNum::from
        virtual void from(const String & in)
        {
            StrConv::conv(in, mObj);
        }

        /** 设置值
        @version NIIEngine 3.0.0
        */
        void setData(const Quaternion & data)
        {
            mObj = data;
        }

        /** 获取值
        @version NIIEngine 3.0.0
        */
        const Quaternion & getData() const
        {
            return mObj;
        }

        void setData(const void * data)
        {
            mObj = *static_cast<const Quaternion *>(data);
        }

        /// @copydetails FusionNum::getDataPtr
        const void * getDataPtr() const
        {
            return &mObj;
        }

        /// @copydetails FusionNum::cloneType
        FusionNum * cloneType() const
        {
            return N_new QuaternionFusion();
        }

        /// @copydetails FusionNum::clone
        FusionNum * clone() const
        {
            return N_new QuaternionFusion(mObj);
        }
    protected:
        Quaternion mObj;
    };
}
#endif